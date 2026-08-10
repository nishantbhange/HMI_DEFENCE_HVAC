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

#include "Mcal.h"
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


static uint8_t ADC_Channel_Index = 0U;
static const ADC_Channel_t ADC_Channel_Sequence[] = {
    ADC_TEMP_SENSOR,
    ADC_COMPRESSOR_CT,
    ADC_CONDENSER_CT,
    ADC_BLOWER_CT
};

static EEPROM_Data_t EEPROM_Last_Written;
static bool     EEPROM_Dirty        = false;
static uint32_t EEPROM_Dirty_Since  = 0U;

static void System_Init(void);
static void Process_Pending_Event(void);
static void Service_EEPROM(void);
static void Service_ADC(void);

int main(void)
{
    System_Init();

    for(;;)
    {

        Process_Pending_Event();


        Update_Compressor_State((HMI_t *)&HMI);


        Update_Output((HMI_t *)&HMI);

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

    Systick_Init(SYSTICK_1MS_LOAD_VALUE, SysTick_CTRL_CLKSOURCE_PROCESSOR_CLK, SysTick_EXCEPTION_EN);
    LPIT_Init();

    LCD_Init();
    LCD_Clear();

    EEPROM_Init();
    ADC_Init();

    Interrupt_Init();

    Event = Event_NONE;

    // Loads defaults or restores from EEPROM, and copies the EEPROM into EEPROM_Last_Written
    HMI_Init((HMI_t *)&HMI);

    // Snapshot what HMI_Init just settled on/restored, so the very
     //first loop pass doesn't immediately treat it as a pending change.
    EEPROM_Last_Written=EEPROM;
    EEPROM_Dirty=false;

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
        EEPROM_Write((const EEPROM_Data_t *)&EEPROM);
        EEPROM_Last_Written = EEPROM;
        EEPROM_Dirty        = false;
    }
}

static void Service_ADC(void)
{
    if(ADC_Ctrl.Status == ADC_FREE)
    {
        ADC_Task(ADC_Channel_Sequence[ADC_Channel_Index]);
        ADC_Channel_Index = (uint8_t)((ADC_Channel_Index + 1U) % ADC_CHANNEL_COUNT);
    }
}
