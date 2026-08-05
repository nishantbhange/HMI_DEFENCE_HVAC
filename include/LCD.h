/*
 * LCD.h
 *
 *  Created on: Jun 27, 2026
 *      Author: Nishant Bhange
 */

#ifndef LCD_H_
#define LCD_H_

#include "S32K144.h"
#include <stdint.h>
#include "Delay.h"
#include "GPIO.h"
#define LCD_PORT IP_PTC

#define LCDON_CURSOROFF        0x0F
#define CLEAR_DISPLAY          0x01
#define RETURN_HOME            0x02
#define DECREMENT_CURSOR       0x04
#define INCREMENT_CURSOR       0x06
#define SHIFT_DISPLAY_RIGHT    0x05
#define SHIFT_DISPLAY_LEFT    0x07
#define DISPLAYON_CURSORBLINK  0x0E
#define CURSOR_ON_FIRSTLINE    0x80
#define CURSOR_ON_SECONDLINE   0xC0
#define TWOLINES_5X8MATRIX     0x38
#define CURSOR_LINE1_POSITION3 0x83
#define ACTIVATE_SECOND_LINE   0x3C
#define DISPLAYOFF_CURSOROFF   0x08
#define JMP_SECOND_LINE_POS1   0xC1
#define DISPLAYON_CURSOROFF    0x0C
#define JMP_SECOND_LINE_POS2   0xC2



void LCD_Init(void );
void LCD_Command(char cmd );
void LCD_Char(char data);
void LCD_String_XY(char ,char ,const char*);
void LCD_Clear(void);
void LCD_String(const char *msg);




#endif /* LCD_H_ */
