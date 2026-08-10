/*
 * HMI.h
 *
 *  Created on: Jul 1, 2026
 *      Author: Nishant Bhange
 */

#ifndef HMI_H_
#define HMI_H_

#include "S32K144.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "GPIO.h"

#include "LCD.h"
#include "Delay.h"

#define ON 1
#define OFF 0




#define MAX_TEMP             30
#define MIN_TEMP             20

#define Error_Set             1
#define Error_Reset           0

#define COMPRESSOR_CT_VAl_120x     2
#define COMPRESSOR_CT_VAl_150x     3
#define COMPRESSOR_CT_VAl_200x     4

typedef enum{

  Auto_Mode ,
  Manual_mode
}Mode_t;

typedef enum{
	AC_off,
	AC_on

}Machine_Status_t;

typedef enum{
  Blower_off,
  Blower_on

}Blower_state_t;

typedef enum{
	Compressor_off,
	Compressor_on,
	Compressor_wait_to_on,
	Compressor_wait_to_off
}Compressor_t;

typedef enum{

	Condenser_off,
	Condenser_on
}Condenser_t;

typedef enum{

    Heater_off ,
	Heater_on
}Heater_t;

typedef enum{

    Vent_off ,
	Vent_on

}Vent_t;

typedef enum{
    Solenoid_Valve_off ,
	Solenoid_Valve_on
}Solenoid_Valve_t;

typedef enum{

	Condenser ,
	Vent      ,
	Blower    ,
	Heater    ,
	Compressor ,
	Solenoid_Valve

}Part_t;

typedef enum {

	error_flag_reset ,
	error_flag_set

 }error_flag_t;

typedef struct{

	Mode_t                  mode ;
	float                   set_temp ;
	float                   curr_temp ;
	Machine_Status_t        status ;
	Blower_state_t          Blower_state;
	Compressor_t            compressor_state;
	Compressor_t            user_compressor_state;
	Condenser_t             condenser_state;
	Heater_t                heater_state;
	Vent_t                  vent_state;
	error_flag_t            error_flag;

}HMI_t;

typedef enum {
	Event_NONE,
	Event_Mode ,
	Event_Increase_Temp,
	Event_Decrease_Temp,
	Event_Blower,
	Event_Heater ,
	Event_User_Compressor,
	Event_Error ,
	Event_Error_Clear,
	Event_Machine_status


}HMI_Event_t;

typedef enum {
	Error_Event_LPSW,
	Error_Event_HPSW,
	Error_Event_ADC ,
	Error_LPSW_Clear,
	Error_HPSW_Clear,
    Error_ADC_Clear ,

}ErrorCode_t;

extern volatile ErrorCode_t Current_Error;

extern volatile HMI_Event_t Event;

extern volatile  HMI_t HMI;
extern volatile  uint32_t Global_Tick_Count;

void Update_Output(HMI_t *HMI);
void Update_Display(HMI_t HMI);
void HMI_Process_Event(HMI_Event_t Event );
void HMI_Init(HMI_t *HMI );
void Relay_Cntrl( Part_t part,bool Enable);
void Led_Cntrl( Part_t part,bool Enable);
void Error_Handler( void  );
void Update_Compressor_State(HMI_t *HMI);

void LPSW_Error_Handler(bool Error_Set_Reset );
void HPSW_Error_Handler(bool Error_Set_Reset );
void ADC_Error_Handler(bool Error_Set_Reset );
void Check_States(void);

#endif /* HMI_H_ */
