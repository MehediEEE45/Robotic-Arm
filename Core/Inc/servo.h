#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Pulse width bounds (microseconds), shared by every servo channel. Each
 * channel's timer runs a 1us tick / 20ms (50Hz) frame (TIM2/TIM3/TIM12
 * Prescaler=107, Period=19999 @ 108MHz APB1 timer clock). */
#define SERVO_PULSE_MIN_US   500U
#define SERVO_PULSE_MAX_US   2500U

#define SERVO_ANGLE_MIN        0

#define SERVO_AUTO_STEP_DEG    2   /* degrees per tick in AUTO sweep */
#define SERVO_AUTO_STEP_MS    20   /* how often the AUTO sweep advances */

#define SERVO_COUNT 4 /* D9 (0-180 deg), D3, D6, D11 (0-90 deg each) */

void Servo_Init(void);
void Servo_Process(void);
void Servo_UpdateState(void);

uint8_t Servo_IsHomePressed(void);

/* Forces the page to redraw from scratch next time it's shown (call when
 * navigating back to this page from the home menu). */
void Servo_ResetView(void);

#ifdef __cplusplus
}
#endif

#endif /* SERVO_H */
