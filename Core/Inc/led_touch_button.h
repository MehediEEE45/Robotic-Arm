#ifndef LED_TOUCH_BUTTON_H
#define LED_TOUCH_BUTTON_H

#include "main.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void LEDTouchButton_Init(void);
void LEDTouchButton_Process(void);

uint8_t LEDTouchButton_IsManualPressed(void);
uint8_t LEDTouchButton_IsBlinkPressed(void);
uint8_t LEDTouchButton_IsHomePressed(void);
void LEDTouchButton_UpdateState(uint8_t led_on, uint8_t blink_mode);

/* Forces the page to redraw from scratch next time it's shown (call when
 * navigating back to this page from the home menu). */
void LEDTouchButton_ResetView(void);

#ifdef __cplusplus
}
#endif

#endif /* LED_TOUCH_BUTTON_H */
