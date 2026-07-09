#ifndef TOUCH_BUTTON_H
#define TOUCH_BUTTON_H

#include "stm32746g_discovery_ts.h"
#include "stm32746g_discovery_lcd.h"
#include <stdint.h>
#include <stdbool.h>

/* Structure for a touchscreen button */
typedef struct {
    uint16_t x;          // X position
    uint16_t y;          // Y position
    uint16_t width;      // Button width
    uint16_t height;     // Button height
    uint32_t color;      // Button fill color
    uint32_t textColor;  // Text color
    char label[30];      // Button label
} TS_Button;

/* Function prototypes */
void touchInit(void);                     // Initialize touchscreen
void drawButton(TS_Button* btn);          // Draw a button
bool isButtonPressed(TS_Button* btn);     // Detect press inside a button
void drawRecordButton(TS_Button* btn);    // Predefined RECORD button

/* Reports the current raw touch point (first touch only). Returns false
 * (and leaves *x/*y untouched) when nothing is touching the screen. */
bool getTouchPosition(uint16_t* x, uint16_t* y);

#endif /* TOUCH_BUTTON_H */
