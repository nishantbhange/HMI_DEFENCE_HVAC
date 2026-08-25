/*==================================================================================================
* Project : RTD AUTOSAR 4.7
* Platform : CORTEXM
* Peripheral : S32K14X
* Dependencies : none
*
* Autosar Version : 4.7.0
* Autosar Revision : ASR_REL_4_7_REV_0000
* Autosar Conf.Variant :
* SW Version : 3.0.0
* Build Version : S32K1_RTD_3_0_0_QLP04_D2509_ASR_REL_4_7_REV_0000_20250930
*
* Copyright 2020-2025 NXP
*
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be 
*   used strictly in accordance with the applicable license terms.  By expressly 
*   accepting such terms or by downloading, installing, activating and/or otherwise 
*   using the software, you are agreeing that you have read, and that you agree to 
*   comply with and are bound by, such license terms.  If you do not agree to be 
*   bound by the applicable license terms, then you may not retain, install,
*   activate or otherwise use the software.
==================================================================================================*/
#include "Clock_Ip.h"
#include "Port_Ci_Port_Ip.h"
#include <stdint.h>
#include <string.h>
#include "HMI.h"
#include "GPIO.h"
#include "ADC.h"
#include "LCD.h"
#include "EEPROM.h"
#include "Delay.h"




#define SYSTICK_1MS_LOAD_VALUE   (CoreClockHz / 1000U - 1U)

#define EEPROM_WRITE_QUIET_PERIOD_MS   5000U
#define Delay_5_SEC                    5000U

 volatile bool ADC_Timeout_Flag;
 volatile uint32_t ADC_Timeout_Count;

 volatile bool EEPROM_Timeout_Flag;
 volatile uint32_t EEPROM_Timeout_Count;

 volatile bool EEPROM_Write_Timeout_Flag;
 volatile uint32_t EEPROM_Write_Timeout_Count;

 volatile bool ADC_Bsy_Timeout_Flag;
  volatile uint32_t ADC_Bsy_Timeout_Count;

static uint8_t ADC_Channel_Index = 0U;
static const ADC_Channel_t ADC_Channel_Sequence[] = {
    ADC_TEMP_SENSOR,
    ADC_COMPRESSOR_CT,
    ADC_CONDENSER_CT,
    ADC_BLOWER_CT
};

static EEPROM_Data_t EEPROM_Last_Written;
static bool EEPROM_Dirty        = false;
static uint32_t EEPROM_Dirty_Since  = 0U;

static void System_Init(void);
static void Process_Pending_Event(void);
static void Service_EEPROM(void);
static void Service_ADC(void);
const char* Start_Msg1="WELCOME :)";
const char* Start_Msg2="TRANS ACNR";

const char* Error_Msg1="WARNING EEPROM";

const char* Error_Msg2="WARNING ADC";
const char* Error_Msg3="INIT FAILED";
const char* Error_Msg4="WRITE FAILED";

int main(void)
{
    System_Init();
    //check if any of lpsw or hpsw is high at start .

    uint32_t values=PINS_DRV_ReadPins(IP_PTA);
    if((values>>SW_PIN_LPSW)&0x01){
    	Event=Event_Error;
        Current_Error=Error_Event_LPSW;
    }
    if((values>>SW_PIN_HPSW)&0x01){
    	Event=Event_Error;
    	Current_Error=Error_Event_HPSW;
        }
    //so that the temp sensor reads the data before Update_Heater_State and heater dosent turn on flasly at very first boot
    Service_ADC();
    LCD_Clear();
    LCD_String_XY(0,3,Start_Msg1);
    LCD_String_XY(1,2,Start_Msg2);
    DelayMs(Delay_5_SEC);
    LCD_Clear();
    for(;;)
    {

    	//Check_OverCurrent();
        Process_Pending_Event();
        HMI.curr_temp = ADC_Data.Temp_Sensor_Val;
        if(HMI.error_flag!=error_flag_set){
        Update_Compressor_State((volatile HMI_t *)&HMI);
        Update_Heater_State((volatile HMI_t *)&HMI);
        }


        Update_Output((volatile HMI_t *)&HMI);

        Update_Display(HMI);
        //write to EEPROM only after its been dirty and quiet for a while
        Service_EEPROM();
        // service next ADC channel in sequence if the ADC is free
        Service_ADC();


    }

    return 0;
}

static void System_Init(void)
{
	 /* --- 1. Clock, before anything else touches a peripheral --- */
	    Clock_Ip_Init(Clock_Ip_aClockConfig);      /* from your generated Clock_Ip_Cfg.c - see note below */

	#if defined (FEATURE_CLOCK_IP_HAS_SPLL_CLK)
	    uint32_t Pll_Lock_Timeout = PLL_LOCK_TIMEOUT_ITER;   /* pick a generous bounded value */
	    while (CLOCK_IP_PLL_LOCKED != Clock_Ip_GetPllStatus())
	    {
	        if (--Pll_Lock_Timeout == 0U)
	        {
	            /* PLL never locked - same class of problem as your other
	             * *_Timeout_Flag guards. Decide a fallback here (e.g. stay on
	             * FIRC / flag a fault) rather than hanging forever. */
	            break;
	        }
	    }
	    Clock_Ip_DistributePll();
	#endif

	    /* --- 2. Pin muxing, before any driver touches a pin --- */
	    Port_Ci_Port_Ip_Init(NUM_OF_CONFIGURED_PINS_PortContainer_0_BOARD_InitPeripherals,
	    		g_pin_mux_InitConfigArr_PortContainer_0_BOARD_InitPeripherals);

	    Systick_Init(SYSTICK_1MS_LOAD_VALUE, SysTick_CTRL_CLKSOURCE_PROCESSOR_CLK, SysTick_EXCEPTION_EN);

	    LPIT_Init();

	    EEPROM_Timeout_Flag=SET;
	    while(EEPROM_Timeout_Count<=TICK_COUNT_500MS){
	    if(EEPROM_Init()){
	    	EEPROM_Timeout_Flag=RESET;
	    	EEPROM_Timeout_Count=0;
	    break;
	    }
	    if(EEPROM_Timeout_Count>=TICK_COUNT_500MS){
	    	//printf("Warning EEPROM init failed")
	    	 LCD_Clear();
	    	 LCD_String_XY(0,0,Error_Msg1);
	    	 LCD_String_XY(1,0,Error_Msg3);
	    	 EEPROM_Timeout_Flag=RESET;
	    	 EEPROM_Timeout_Count=0;
	    	 break;
	    }
	    }


	// Loads defaults or restores from EEPROM, and copies the EEPROM into EEPROM_Last_Written
    HMI_Init((HMI_t *)&HMI);
    //Snapshot what HMI_Init just settled on/restored, so the very first loop pass doesn't immediately treat it as a pending change.
    EEPROM_Last_Written=EEPROM;
    EEPROM_Dirty=false;



    Interrupt_Init();
    LCD_Init();
    LCD_Clear();


    ADC_Timeout_Flag=SET;
      while(ADC_Timeout_Count<=TICK_COUNT_500MS){
      if(ADC_Init()){
      ADC_Timeout_Flag=RESET;
      ADC_Timeout_Count=0;
      break;
      }
      if(ADC_Timeout_Count>=TICK_COUNT_500MS){
      	//printf("Warning ADC init failed")
      	 LCD_Clear();
      	 LCD_String_XY(0,0,Error_Msg2);
      	 LCD_String_XY(1,0,Error_Msg3);
      	 ADC_Timeout_Flag=RESET;
      	 ADC_Timeout_Count=0;
      	 break;
      }
      }






    Event = Event_NONE;




}

static void Process_Pending_Event(void)
{
    if(Event == Event_NONE)
    {
        return;
    }

    if(Event==Event_Error || Event==Event_Error_Clear)
    {
        Error_Handler();
        HMI_Process_Event(Event);
    }
    else
    {
        HMI_Process_Event(Event);
    }

    Event = Event_NONE;
}

static void Service_EEPROM(void)
{
    // Did anything change since the last confirmed write?
    if(memcmp((const void *)&EEPROM, (const void *)&EEPROM_Last_Written, sizeof(EEPROM_Data_t)) != 0)
    {
        if(!EEPROM_Dirty)
        {
            // Just became dirty - start the quiet-period timer
            EEPROM_Dirty       = true;
            EEPROM_Dirty_Since = Global_Tick_Count;
          }
           else
             {
            // Still changing - keep pushing the timer out so a burst of
            // rapid changes doesn't write mid-burst
            EEPROM_Dirty_Since = Global_Tick_Count;
       }
           return;
    }

    // Data is currently stable (matches last snapshot)commit it to flash
      if(EEPROM_Dirty && ((Global_Tick_Count - EEPROM_Dirty_Since) >= EEPROM_WRITE_QUIET_PERIOD_MS))
      {

    	EEPROM_Write_Timeout_Flag=SET;
          while(EEPROM_Write_Timeout_Count<=TICK_COUNT_100MS){
          if(EEPROM_Write((const EEPROM_Data_t *)&EEPROM)){
        	  EEPROM_Write_Timeout_Flag=RESET;
        	  EEPROM_Write_Timeout_Count=0;
          break;
          }
          if(EEPROM_Write_Timeout_Count>=TICK_COUNT_100MS){
          	//printf("Warning EEPROM WRITE failed")
          	 LCD_Clear();
          	 LCD_String_XY(0,0,Error_Msg1);
          	 LCD_String_XY(1,0,Error_Msg4);
          	EEPROM_Write_Timeout_Flag=RESET;
          	EEPROM_Write_Timeout_Count=0;
          	 break;
          }
          }
        EEPROM_Last_Written = EEPROM;
        EEPROM_Dirty        = false;
    }
}
static void Service_ADC(void)
{
    if(ADC_Ctrl.Status == ADC_FREE)
    {   ADC_Bsy_Timeout_Flag=SET;
        ADC_Task(ADC_Channel_Sequence[ADC_Channel_Index]);
        ADC_Channel_Index = (uint8_t)((ADC_Channel_Index + 1U) % ADC_CHANNEL_COUNT);
    }
   if(ADC_Bsy_Timeout_Count>=TICK_COUNT_5SEC && ADC_Ctrl.Status == ADC_BUSY){

	ADC_Ctrl.Status = ADC_FREE;
	ADC_Bsy_Timeout_Flag=RESET;
	ADC_Bsy_Timeout_Count=0;
	Event = Event_Error;
    Current_Error=Error_Event_ADC;
}

}
