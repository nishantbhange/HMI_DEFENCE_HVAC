
#include"LCD.h"



//#include <pins_driver.h>
//#include <pin_mux.h>
void LCD_Init(void ){
	 // 15ms wait time after power on
	    DelayMs(15);
	 // uses 2 line and initialize 5*7 matrix of LCD
	    LCD_Command((char)TWOLINES_5X8MATRIX);
	    DelayUs(50);
	 // clear display screen
	    LCD_Command((char)CLEAR_DISPLAY);
	    DelayMs(2);
	 // display on cursor off
	    LCD_Command((char)DISPLAYON_CURSOROFF);
	    DelayUs(50);
	 // increment cursor (shift cursor to right)
	    LCD_Command((char)INCREMENT_CURSOR);
	    DelayUs(50);

}
void LCD_String_XY(char row ,char position ,const char* msg){
	 char location=0;
	    if(row==1)
	    {
	    	//Print message on 1st row and desired location
	        location=(0x80) | ((position) & 0x0f);
	        LCD_Command(location);
	    }
	    else
	    {
	    	//Print message on 2nd row and desired location
	        location=(0xC0) | ((position) & 0x0f);
	        LCD_Command(location);
	    }

	    LCD_String(msg);
}


void LCD_Clear()
{      //clear display screen
    	LCD_Command((char)CLEAR_DISPLAY);
    	DelayMs(2);
}

void LCD_String(const char *msg)
{
	while((*msg)!=0)
	{
	  LCD_Char(*msg);
	  msg++;

    	}
}

void LCD_Command(char cmd ){

PINS_DRV_ClearPins(LCD_PORT , 0xFF);
PINS_DRV_SetPins(LCD_PORT,(uint8_t)cmd);
// RS=0 for sending command
PINS_DRV_ClearPins(LCD_PORT, 1 << LCD_PIN_RS);
// RW=0
PINS_DRV_ClearPins(LCD_PORT, 1 << LCD_PIN_RW);
// High to low pulse
PINS_DRV_SetPins(LCD_PORT, 1 << LCD_PIN_EN);
DelayMs(1);
PINS_DRV_ClearPins(LCD_PORT, 1 << LCD_PIN_EN);



}

void LCD_Char(char dat)
{
	//Send data to LCD
	PINS_DRV_ClearPins(LCD_PORT , 0xFF);
	PINS_DRV_SetPins(LCD_PORT,(uint8_t)dat);

	//RS = 1 Data Register is selected

	PINS_DRV_SetPins(LCD_PORT, 1 << LCD_PIN_RS);
	// High-to-Low pulse on Enable pin to latch data
	//EN=1;
	PINS_DRV_SetPins(LCD_PORT, 1 << LCD_PIN_EN);
	//MSdelay(1);
	DelayMs(1);
	//EN=0;
	PINS_DRV_ClearPins(LCD_PORT, 1 << LCD_PIN_EN);

}

