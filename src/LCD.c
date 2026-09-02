
#include"LCD.h"
#include "Delay.h"
#include "GPIO.h"

static void LCD_Write_Pin(LCD_Pin_t p, bool level);
static void LCD_Write_Data(char data);

//#include <pins_driver.h>
//#include <pin_mux.h>
void LCD_Init(void ){
	 // 15ms wait time after power on
	    DelayMs(15);
	 // uses 2 line and initialize 5*7 matrix of LCD
	    LCD_Command((char)TWOLINES_5X8MATRIX);
	    DelayUs(50);
	 // uses 2 line and initialize 5*7 matrix of LCD
	    LCD_Command((char)TWOLINES_5X8MATRIX);
	    DelayUs(50);
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
	    if(row==0)
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
    	DelayMs(3);
}

void LCD_String(const char *msg)
{
	while((*msg)!=0)
	{
	  LCD_Char(*msg);
	  msg++;

    	}
}
static void LCD_Write_Pin(LCD_Pin_t p, bool level){
    if(level) PINS_DRV_SetPins(p.base, 1U << p.pin);
    else      PINS_DRV_ClearPins(p.base, 1U << p.pin);
}
static void LCD_Write_Data(char data){
    const LCD_Pin_t d[8] = {LCD_D0,LCD_D1,LCD_D2,LCD_D3,LCD_D4,LCD_D5,LCD_D6,LCD_D7};
    for(int i = 0; i < 8; i++){
        LCD_Write_Pin(d[i], (data >> i) & 0x01);
    }
}

void LCD_Command(char cmd){
    LCD_Write_Data(cmd);
    LCD_Write_Pin(LCD_RS, 0);
    LCD_Write_Pin(LCD_EN, 1);
    DelayUs(10);
    LCD_Write_Pin(LCD_EN, 0);
    DelayUs(50);
}

void LCD_Char(char dat)
{
	//Send data to LCD
	 LCD_Write_Data(dat);
	//RS = 1 Data Register is selected
	 LCD_Write_Pin(LCD_RS, 1);
	// High-to-Low pulse on Enable pin to latch data
	//EN=1;
	 LCD_Write_Pin(LCD_EN, 1);
	//USdelay(10);
	 DelayUs(10);
	//EN=0;
	LCD_Write_Pin(LCD_EN, 0);
	DelayUs(50);

}

