#include "servo.h"
#include "touch_button.h"
#include "stm32746g_discovery_lcd.h"
#include "main.h"
#include <string.h>
#include <stdio.h>

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim12;

/* Horizontal slider geometry (one row per servo, fader-style: left = 0 deg,
 * right = that servo's max angle). Track is shortened on the right to make
 * room for a row of quick-jump preset buttons. */
#define LABEL_X        6
#define TRACK_LEFT_X   100
#define TRACK_RIGHT_X  360
#define RAIL_H         8
#define KNOB_R         20
#define ROW_HIT_HALF_H 26

/* One-tap preset buttons per row: 0 / 45 / 90 degrees. */
#define PRESET_COUNT      3
#define PRESET_BTN_X      390
#define PRESET_BTN_W      26
#define PRESET_BTN_H      32
#define PRESET_BTN_GAP    4
static const int16_t PRESET_ANGLES[PRESET_COUNT] = {0, 45, 90};

typedef struct {
  TIM_HandleTypeDef *htim;
  uint32_t channel;
  int16_t angle_max;
  int16_t row_y;
  const char *label;
  int16_t angle;
  int8_t auto_dir;
  int16_t last_drawn_angle;
} Servo_t;

/* D9 keeps its original 0-180 deg range; D3/D6/D11 are 90 deg servos. */
static Servo_t g_servos[SERVO_COUNT] = {
    {&htim2, TIM_CHANNEL_1, 180, 74, "D9", 90, 1, -1},
    {&htim3, TIM_CHANNEL_1, 90, 130, "D3", 45, 1, -1},
    {&htim12, TIM_CHANNEL_1, 90, 186, "D6", 45, 1, -1},
    {&htim12, TIM_CHANNEL_2, 90, 242, "D11", 45, 1, -1},
};

static TS_Button g_btn_home;
static TS_Button g_btn_mode;
static TS_Button g_btn_preset[SERVO_COUNT][PRESET_COUNT];
static uint8_t g_preset_touch_prev[SERVO_COUNT][PRESET_COUNT];

static uint8_t g_initialized = 0;
static uint8_t g_view_dirty = 1; /* forces a full (re)draw of the page */

static uint8_t g_auto_mode = 0; /* 0 = MANUAL, 1 = AUTO */
static uint32_t g_last_step_time = 0;

static uint8_t g_home_touch_prev = 0;
static uint8_t g_mode_touch_prev = 0;
static uint8_t g_home_pressed = 0;

static uint8_t g_last_drawn_mode = 0xFF;

static void Servo_SetPulse(Servo_t *sv) {
  uint32_t pulse =
      SERVO_PULSE_MIN_US + ((uint32_t)(sv->angle - SERVO_ANGLE_MIN) *
                             (SERVO_PULSE_MAX_US - SERVO_PULSE_MIN_US)) /
                                (sv->angle_max - SERVO_ANGLE_MIN);
  __HAL_TIM_SET_COMPARE(sv->htim, sv->channel, pulse);
}

static int16_t Servo_AngleToKnobX(const Servo_t *sv) {
  return (int16_t)(TRACK_LEFT_X + ((int32_t)(sv->angle - SERVO_ANGLE_MIN) *
                                    (TRACK_RIGHT_X - TRACK_LEFT_X)) /
                                       (sv->angle_max - SERVO_ANGLE_MIN));
}

static int16_t Servo_KnobXToAngle(const Servo_t *sv, int16_t knob_x) {
  int16_t angle = (int16_t)(SERVO_ANGLE_MIN +
                             ((int32_t)(knob_x - TRACK_LEFT_X) *
                              (sv->angle_max - SERVO_ANGLE_MIN)) /
                                 (TRACK_RIGHT_X - TRACK_LEFT_X));
  if (angle < SERVO_ANGLE_MIN)
    angle = SERVO_ANGLE_MIN;
  if (angle > sv->angle_max)
    angle = sv->angle_max;
  return angle;
}

/* Redraws one slider row in full: erase the row's track column, then rail,
 * level fill, knob and angle text on top. Simpler and glitch-free than
 * tracking partial dirty rectangles for a knob that can move every frame. */
static void Servo_DrawRow(const Servo_t *sv) {
  int16_t knob_x = Servo_AngleToKnobX(sv);
  uint32_t accent = g_auto_mode ? LCD_COLOR_DARKGRAY : LCD_COLOR_BLUE;
  char buf[8];

  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_FillRect(TRACK_LEFT_X - KNOB_R - 4, sv->row_y - KNOB_R - 4,
                    (TRACK_RIGHT_X - TRACK_LEFT_X) + 2 * (KNOB_R + 4),
                    2 * (KNOB_R + 4));

  /* Plain gray rail, full length */
  BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
  BSP_LCD_FillRect(TRACK_LEFT_X, sv->row_y - RAIL_H / 2,
                    TRACK_RIGHT_X - TRACK_LEFT_X, RAIL_H);

  /* Colored "level" fill from the left up to the knob */
  BSP_LCD_SetTextColor(accent);
  BSP_LCD_FillRect(TRACK_LEFT_X, sv->row_y - RAIL_H / 2, knob_x - TRACK_LEFT_X,
                    RAIL_H);

  /* Knob: light disc with a colored ring, angle printed inside */
  BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
  BSP_LCD_FillCircle(knob_x, sv->row_y, KNOB_R);
  BSP_LCD_SetTextColor(accent);
  BSP_LCD_DrawCircle(knob_x, sv->row_y, KNOB_R);
  BSP_LCD_DrawCircle(knob_x, sv->row_y, KNOB_R - 1);

  snprintf(buf, sizeof(buf), "%d", (int)sv->angle);
  BSP_LCD_SetFont(&Font12);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  /* DrawChar always paints the glyph's "off" pixels with BackColor, so this
   * must match the knob's own fill color or the text shows up on a
   * mismatched box. */
  BSP_LCD_SetBackColor(LCD_COLOR_LIGHTGRAY);
  BSP_LCD_DisplayStringAt(knob_x - KNOB_R, sv->row_y - (Font12.Height / 2),
                           (uint8_t *)buf, CENTER_MODE);
  BSP_LCD_SetFont(&Font24); /* restore the font drawButton()/labels expect */
}

static void Servo_DrawLabels(void) {
  BSP_LCD_SetFont(&Font16);
  BSP_LCD_SetBackColor(LCD_COLOR_BLACK); /* labels sit on the black page bg */
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  for (int i = 0; i < SERVO_COUNT; i++) {
    BSP_LCD_DisplayStringAt(LABEL_X, g_servos[i].row_y - (Font16.Height / 2),
                             (uint8_t *)g_servos[i].label, LEFT_MODE);
  }
  BSP_LCD_SetFont(&Font24);
}

/* Static per-row preset buttons (0/45/90 deg): position/label/color never
 * change, so they're only ever drawn once, in the g_view_dirty block. */
static void Servo_DrawPresets(void) {
  BSP_LCD_SetFont(&Font12);
  for (int i = 0; i < SERVO_COUNT; i++) {
    for (int j = 0; j < PRESET_COUNT; j++) {
      drawButton(&g_btn_preset[i][j]);
    }
  }
  BSP_LCD_SetFont(&Font24);
}

void Servo_Init(void) {
  for (int i = 0; i < SERVO_COUNT; i++) {
    HAL_TIM_PWM_Start(g_servos[i].htim, g_servos[i].channel);
    Servo_SetPulse(&g_servos[i]);
    g_servos[i].last_drawn_angle = -1;

    for (int j = 0; j < PRESET_COUNT; j++) {
      TS_Button *pb = &g_btn_preset[i][j];
      pb->x = PRESET_BTN_X + j * (PRESET_BTN_W + PRESET_BTN_GAP);
      pb->y = g_servos[i].row_y - PRESET_BTN_H / 2;
      pb->width = PRESET_BTN_W;
      pb->height = PRESET_BTN_H;
      pb->color = LCD_COLOR_DARKGRAY;
      pb->textColor = LCD_COLOR_WHITE;
      snprintf(pb->label, sizeof(pb->label), "%d", (int)PRESET_ANGLES[j]);
      g_preset_touch_prev[i][j] = 0;
    }
  }

  g_btn_home.x = 8;
  g_btn_home.y = 6;
  g_btn_home.width = 80;
  g_btn_home.height = 30;
  g_btn_home.color = LCD_COLOR_DARKGRAY;
  g_btn_home.textColor = LCD_COLOR_WHITE;
  strcpy(g_btn_home.label, "HOME");

  g_btn_mode.x = 300;
  g_btn_mode.y = 6;
  g_btn_mode.width = 172;
  g_btn_mode.height = 34;
  g_btn_mode.textColor = LCD_COLOR_WHITE;

  g_auto_mode = 0;
  g_last_step_time = HAL_GetTick();

  g_initialized = 1;
  g_view_dirty = 1;
}

void Servo_Process(void) {
  if (!g_initialized)
    return;

  uint8_t home_now = isButtonPressed(&g_btn_home) ? 1 : 0;
  if (home_now && !g_home_touch_prev) {
    g_home_pressed = 1;
  }
  g_home_touch_prev = home_now;

  uint8_t mode_now = isButtonPressed(&g_btn_mode) ? 1 : 0;
  if (mode_now && !g_mode_touch_prev) {
    g_auto_mode = !g_auto_mode;
    g_view_dirty = 1; /* re-color the mode button */
  }
  g_mode_touch_prev = mode_now;

  /* One-tap presets: jump straight to 0/45/90 deg. Ignored in AUTO mode
   * since the sweep would just overwrite them on the next tick. */
  if (!g_auto_mode) {
    for (int i = 0; i < SERVO_COUNT; i++) {
      for (int j = 0; j < PRESET_COUNT; j++) {
        uint8_t now = isButtonPressed(&g_btn_preset[i][j]) ? 1 : 0;
        if (now && !g_preset_touch_prev[i][j]) {
          int16_t angle = PRESET_ANGLES[j];
          if (angle > g_servos[i].angle_max)
            angle = g_servos[i].angle_max;
          g_servos[i].angle = angle;
        }
        g_preset_touch_prev[i][j] = now;
      }
    }
  }

  /* Drag-follow: while a finger is down in MANUAL mode near a row's rail,
   * that servo's knob tracks the touch X position continuously. */
  if (!g_auto_mode) {
    uint16_t tx, ty;
    if (getTouchPosition(&tx, &ty) && tx >= (TRACK_LEFT_X - KNOB_R) &&
        tx <= (TRACK_RIGHT_X + KNOB_R)) {
      for (int i = 0; i < SERVO_COUNT; i++) {
        Servo_t *sv = &g_servos[i];
        if (ty >= (sv->row_y - ROW_HIT_HALF_H) &&
            ty <= (sv->row_y + ROW_HIT_HALF_H)) {
          sv->angle = Servo_KnobXToAngle(sv, (int16_t)tx);
          break;
        }
      }
    }
  }
}

void Servo_UpdateState(void) {
  if (!g_initialized)
    return;

  if (g_auto_mode) {
    uint32_t now = HAL_GetTick();
    if (now - g_last_step_time >= SERVO_AUTO_STEP_MS) {
      g_last_step_time = now;
      for (int i = 0; i < SERVO_COUNT; i++) {
        Servo_t *sv = &g_servos[i];
        sv->angle += (int16_t)sv->auto_dir * SERVO_AUTO_STEP_DEG;
        if (sv->angle >= sv->angle_max) {
          sv->angle = sv->angle_max;
          sv->auto_dir = -1;
        } else if (sv->angle <= SERVO_ANGLE_MIN) {
          sv->angle = SERVO_ANGLE_MIN;
          sv->auto_dir = 1;
        }
      }
    }
  }

  for (int i = 0; i < SERVO_COUNT; i++) {
    Servo_SetPulse(&g_servos[i]);
  }

  if (g_view_dirty) {
    drawButton(&g_btn_home);
    Servo_DrawLabels();
    Servo_DrawPresets();
    g_view_dirty = 0;
  }

  if (g_auto_mode != g_last_drawn_mode) {
    g_btn_mode.color = g_auto_mode ? LCD_COLOR_DARKBLUE : LCD_COLOR_DARKGREEN;
    strcpy(g_btn_mode.label, g_auto_mode ? "MODE: AUTO" : "MODE: MANUAL");
    drawButton(&g_btn_mode);
    g_last_drawn_mode = g_auto_mode;
    for (int i = 0; i < SERVO_COUNT; i++) {
      g_servos[i].last_drawn_angle = -1; /* force sliders to redraw (color) */
    }
  }

  for (int i = 0; i < SERVO_COUNT; i++) {
    Servo_t *sv = &g_servos[i];
    if (sv->angle != sv->last_drawn_angle) {
      Servo_DrawRow(sv);
      sv->last_drawn_angle = sv->angle;
    }
  }
}

uint8_t Servo_IsHomePressed(void) {
  uint8_t pressed = g_home_pressed;
  g_home_pressed = 0;
  return pressed;
}

void Servo_ResetView(void) {
  g_view_dirty = 1;
  g_last_drawn_mode = 0xFF;
  for (int i = 0; i < SERVO_COUNT; i++) {
    g_servos[i].last_drawn_angle = -1;
  }
}
