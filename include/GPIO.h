/*
 * GPIO.h
 *
 *  Created on: Jun 27, 2026
 *      Author: Nishant Bhange
 */

#ifndef GPIO_H_
#define GPIO_H_

#include<stdint.h>
#include<stdio.h>
#include<stdbool.h>
#include<S32K144.h>



#define LCD_PIN_RS   0U
#define LCD_PIN_RW   1U
#define LCD_PIN_EN   2U
#define DEBOUNCE_MS 20U
#define LONG_PRESS_MS 3000U

#define LCD_PIN_DB0   3U
#define LCD_PIN_DB1   4U
#define LCD_PIN_DB2   5U
#define LCD_PIN_DB3   6U
#define LCD_PIN_DB4   7U
#define LCD_PIN_DB5   8U
#define LCD_PIN_DB6   9U
#define LCD_PIN_DB7   10U

#define SW_PIN_PWR    4U
#define SW_PIN_LPSW   3U
#define SW_PIN_HPSW   2U

//PTB5 temp++
//PTB4 PWR
//PTA3 LPSW
//PTA2 HPSW
//PTC0 -ADC channel 8
//PTC1 -ADC channel 9
//PTC2 -compressor switch
//PTC3 -Heater Switch
//PTC17 -ADC channel 15
//PTC15 -ADC channel 12

#define TEMP_INC_FLAG 5U
#define PWR_FLAG      4U





#define COMPRESSOR_SW_FLAG      2U
#define HEATER_SW_FLAG          3U


//PTE8 -Temp--
#define TEMP_DEC_FLAG           8U
//PTD7 -Blower Switch
#define BLOWER_SW_FLAG          7U

//PTA2 -HPSW
#define HPSW_FLAG               2U
//PTA3 -LPSW
#define LPSW_FLAG               3U


#define Interrupt_DI            0x0
#define Logic_0                 0x8
#define Rising_Edge             0x9
#define Falling_Edge            0xA
#define Either_Edge             0xB
#define Logic_1                 0xC


typedef enum {
    BTN_TEMP_INC = 0,
    BTN_TEMP_DEC,
    BTN_PWR,
    BTN_LPSW,
    BTN_HPSW,
    BTN_COMPRESSOR_SW,
    BTN_HEATER_SW,
    BTN_BLOWER_SW,
    BTN_COUNT
} Button_Id_t;


 void PINS_DRV_ClearPins( GPIO_Type * const base , uint32_t pins);
 void PINS_DRV_SetPins( GPIO_Type * const base , uint32_t pins);
 void PINS_DRV_TogglePins(GPIO_Type * const base , uint32_t pins);
 uint32_t PINS_DRV_ReadPins(GPIO_Type * const base);
 void Interrupt_Init(void );
 extern volatile uint32_t Press_Start_Tick;
 extern volatile bool Long_Press_Flag ;


#endif /* GPIO_H_ */
