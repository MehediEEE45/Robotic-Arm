#include "led_touch_button.h"
#include "stm32746g_discovery_lcd.h"
#include "touch_button.h"
#include <stdio.h>
#include <string.h>

static TS_Button g_btn_manual;
static TS_Button g_btn_blink;
static TS_Button g_btn_home;

static uint8_t g_initialized = 0;
static uint8_t g_last_led_on = 0xFF;
static uint8_t g_last_blink_mode = 0xFF;
static uint8_t g_view_dirty = 1; /* forces the HOME button to (re)draw */

// Variables to export touch press state
static uint8_t g_manual_pressed = 0;
static uint8_t g_blink_pressed = 0;
static uint8_t g_home_touch_prev = 0;
static uint8_t g_home_pressed = 0;

void LEDTouchButton_Init(void) {
  touchInit();

  // "Manual Control" button (simulates holding PI11)
  g_btn_manual.x = 20;
  g_btn_manual.y = 190;
  g_btn_manual.width = 200;
  g_btn_manual.height = 60;
  g_btn_manual.textColor = LCD_COLOR_WHITE;

  // "Blink Mode" button
  g_btn_blink.x = 260;
  g_btn_blink.y = 190;
  g_btn_blink.width = 200;
  g_btn_blink.height = 60;
  g_btn_blink.textColor = LCD_COLOR_WHITE;

  // "Back to menu" button
  g_btn_home.x = 10;
  g_btn_home.y = 10;
  g_btn_home.width = 90;
  g_btn_home.height = 30;
  g_btn_home.color = LCD_COLOR_DARKGRAY;
  g_btn_home.textColor = LCD_COLOR_WHITE;
  strcpy(g_btn_home.label, "HOME");

  g_initialized = 1;
  g_view_dirty = 1;
}

void LEDTouchButton_Process(void) {
  if (!g_initialized)
    return;

  // Poll the touchscreen natively inside the loop
  g_manual_pressed = isButtonPressed(&g_btn_manual) ? 1 : 0;
  g_blink_pressed = isButtonPressed(&g_btn_blink) ? 1 : 0;

  uint8_t home_now = isButtonPressed(&g_btn_home) ? 1 : 0;
  if (home_now && !g_home_touch_prev) {
    g_home_pressed = 1;
  }
  g_home_touch_prev = home_now;
}

uint8_t LEDTouchButton_IsManualPressed(void) { return g_manual_pressed; }

uint8_t LEDTouchButton_IsBlinkPressed(void) { return g_blink_pressed; }

uint8_t LEDTouchButton_IsHomePressed(void) {
  uint8_t pressed = g_home_pressed;
  g_home_pressed = 0;
  return pressed;
}

void LEDTouchButton_ResetView(void) {
  g_last_led_on = 0xFF;
  g_last_blink_mode = 0xFF;
  g_view_dirty = 1;
}

void LEDTouchButton_UpdateState(uint8_t led_on, uint8_t blink_mode) {
  if (!g_initialized)
    return;

  if (g_view_dirty) {
    drawButton(&g_btn_home);
    g_view_dirty = 0;
  }

  // Only redraw when state visually changes to prevent tearing!
  if (g_last_led_on != led_on) {
    g_btn_manual.color = led_on ? LCD_COLOR_GREEN : LCD_COLOR_RED;
    strcpy(g_btn_manual.label, led_on ? "LED: ON" : "LED: OFF");
    drawButton(&g_btn_manual);
    g_last_led_on = led_on;
  }

  if (g_last_blink_mode != blink_mode) {
    g_btn_blink.color = blink_mode ? LCD_COLOR_DARKBLUE : LCD_COLOR_DARKGRAY;
    strcpy(g_btn_blink.label, blink_mode ? "AUTO: BLINK" : "AUTO: OFF");
    drawButton(&g_btn_blink);
    g_last_blink_mode = blink_mode;
  }
}
