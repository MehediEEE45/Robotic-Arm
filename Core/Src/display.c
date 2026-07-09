#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
#include <ctype.h> // for tolower()
#include <stdio.h>
#include <string.h>

void LCD_Config(void) {
  /* LCD Initialization */
  BSP_LCD_Init();

  /* Use a single RGB565 layer so the LTDC output matches the generated setup. */
  BSP_LCD_LayerRgb565Init(0, LCD_FB_START_ADDRESS);

  /* Enable the LCD */
  BSP_LCD_DisplayOn();

  /* Select the LCD Background Layer  */
  BSP_LCD_SelectLayer(0);

  /* Clear the Background Layer */
  BSP_LCD_Clear(LCD_COLOR_BLACK);
  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
}

void setFont(char size) {
  size = tolower(size); // supports 'S' or 's', etc.

  switch (size) {
  case 's': // small
    BSP_LCD_SetFont(&Font12);
    break;
  case 'm': // medium
    BSP_LCD_SetFont(&Font16);
    break;
  case 'l': // large
    BSP_LCD_SetFont(&Font24);
    break;
  default: // default font
    BSP_LCD_SetFont(&Font20);
    break;
  }
}

uint32_t GetColorFromChar(char c) {
  switch (tolower(c)) {
  case 'b':
    return LCD_COLOR_BLUE;
  case 'g':
    return LCD_COLOR_GREEN;
  case 'r':
    return LCD_COLOR_RED;
  case 'c':
    return LCD_COLOR_CYAN;
  case 'm':
    return LCD_COLOR_MAGENTA;
  case 'y':
    return LCD_COLOR_YELLOW;
  case 'w':
    return LCD_COLOR_WHITE;
  case 'k':
    return LCD_COLOR_BLACK; // 'k' for blacK
  case 'o':
    return LCD_COLOR_ORANGE;
  case 'p':
    return LCD_COLOR_BROWN;
  case 'a':
    return LCD_COLOR_GRAY;
  case 'd':
    return LCD_COLOR_DARKGRAY;
  case 'l':
    return LCD_COLOR_LIGHTGRAY;
  case '1':
    return LCD_COLOR_LIGHTBLUE;
  case '2':
    return LCD_COLOR_LIGHTGREEN;
  case '3':
    return LCD_COLOR_LIGHTRED;
  case '4':
    return LCD_COLOR_LIGHTCYAN;
  case '5':
    return LCD_COLOR_LIGHTMAGENTA;
  case '6':
    return LCD_COLOR_LIGHTYELLOW;
  case '7':
    return LCD_COLOR_DARKBLUE;
  case '8':
    return LCD_COLOR_DARKGREEN;
  case '9':
    return LCD_COLOR_DARKRED;
  case '0':
    return LCD_COLOR_DARKCYAN;
  case '-':
    return LCD_COLOR_DARKMAGENTA;
  case '=':
    return LCD_COLOR_DARKYELLOW;
  case 't':
    return LCD_COLOR_TRANSPARENT;
  default:
    return LCD_COLOR_BLACK; // Default color
  }
}

void text(int x, int y, char ps, char text_str[], char text_color,
          char scr_color) {

  //	 text(0, 100, 'c', "Hello STM32!", 'w', 'b');
  char buf[64]; // temporary text buffer

  // Convert inputs to lowercase so it works with small or capital letters
  ps = tolower(ps);
  text_color = tolower(text_color);
  scr_color = tolower(scr_color);

  // ----- Clear Screen -----
  BSP_LCD_Clear(GetColorFromChar(scr_color));

  // ----- Set Text Color -----
  BSP_LCD_SetTextColor(GetColorFromChar(text_color));

  // ----- Copy and Display Text -----
  snprintf(buf, sizeof(buf), "%s", text_str);

  if (ps == 'l')
    BSP_LCD_DisplayStringAt(x, y, (uint8_t *)buf, LEFT_MODE);
  else if (ps == 'r')
    BSP_LCD_DisplayStringAt(x, y, (uint8_t *)buf, RIGHT_MODE);
  else
    BSP_LCD_DisplayStringAt(x, y, (uint8_t *)buf, CENTER_MODE);
}

void multitext(int x, int y, char ps, char text_str[], char text_color) {
  char buf[64];
  ps = tolower(ps);
  text_color = tolower(text_color);

  // ----- Set Text Color -----
  BSP_LCD_SetTextColor(GetColorFromChar(text_color));

  // ----- Display each line -----
  int line_y = y;                      // starting y position
  char *line = strtok(text_str, "\n"); // split by \n

  while (line != NULL) {
    snprintf(buf, sizeof(buf), "%s", line);

    if (ps == 'l')
      BSP_LCD_DisplayStringAt(x, line_y, (uint8_t *)buf, LEFT_MODE);
    else if (ps == 'r')
      BSP_LCD_DisplayStringAt(x, line_y, (uint8_t *)buf, RIGHT_MODE);
    else
      BSP_LCD_DisplayStringAt(x, line_y, (uint8_t *)buf, CENTER_MODE);

    line_y += BSP_LCD_GetFont()->Height; // move down for next line
    line = strtok(NULL, "\n");
  }
}
