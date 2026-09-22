/*
*      EEPROM.h
*      Created on: Jul 3, 2026
*      Author: Nishant Bhange
*
*/


#ifndef EEPROM_H_
#define EEPROM_H_
#include<stdio.h>
#include<stdint.h>
#include<stdbool.h>
#include "HMI.h"

#define FTFC_FSTAT_BIT_CCIF           7
#define FTFC_FCNFG_BIT_EEERDY         0
#define FTFC_FSTAT_BIT_FPVIOL         4
#define FTFC_FSTAT_BIT_ACCERR         5
#define FTFC_FSTAT_BIT_RDCOLERR       6
#define FTFC_FSTAT_BIT_MGSTAT0        0
#define SIM_FCFG1_BIT_DEPART          12
#define SIM_FCFG1_BIT_EEERAMSIZE      16
#define MAGIC_NO                      (0x0000F0F0U)
#define FlexRAM_BASE_ADDR             (0x14000000U)
#define TICK_COUNT_5SEC               5000U
#define Delay_1_SEC                   1000U
#define WDOG_INST                     (0U)
#define EEPROM_FORMAT_VERSION         1U


extern volatile bool CCIF_TIMEOUT_FLAG;
extern volatile bool EEERDY_TIMEOUT_FLAG;
extern volatile uint32_t CCIF_TIMEOUT_COUNT;
extern volatile uint32_t EEERDY_TIMEOUT_COUNT;

/*typedef struct{

	float                   Set_Temp;
	Mode_t                  Curr_Mode;
	Condenser_t             Condenser_state;
	Heater_t                heater_state;
	Vent_t                  vent_state;
	Machine_Status_t        AC_State;
	Compressor_t            User_Compressor_state;
	float                   Curr_Temp;
	ErrorCode_t             ErrorCode ;
	uint8_t                 Active_Errors;
	ErrorCode_t             Display_Error_Code[ERORR_COUNT];
	float                   OC_Current_Val;
	float                   OC_Time_Val;
	uint32_t                Magic_No;
	bool                    Error_Present ;

	//ERROR logs like error counts


}EEPROM_Data_t;*/


typedef struct {

    uint32_t oc_time_ms;            /* 0x00 - 4-byte fields first     */
    int16_t  set_temp_x10;          /* 0x04                           */
    int16_t  curr_temp_x10;         /* 0x06                           */
    uint16_t oc_current_x100;       /* 0x08                           */
    uint8_t  curr_mode;             /* 0x0A                           */
    uint8_t  condenser_state;       /* 0x0B                           */
    uint8_t  heater_state;          /* 0x0C                           */
    uint8_t  vent_state;            /* 0x0D                           */
    uint8_t  ac_state;              /* 0x0E                           */
    uint8_t  user_compressor_state; /* 0x0F                           */
    uint8_t  error_code;            /* 0x10                           */
    uint8_t  active_errors;         /* 0x11                           */
    uint8_t  display_error[4];      /* 0x12                           */
    uint8_t  error_present;         /* 0x16                           */
    uint8_t  reserved[1];           /* 0x17 - pads to 24 exactly      */
} EEPROM_Data_t;

_Static_assert((sizeof(EEPROM_Data_t) % 4U) == 0U,"EEPROM_Data_t must be word aligned");
_Static_assert((sizeof(EEPROM_Data_t)  ) == 24U,"EEPROM_Data_t size must be 24 bytes");

typedef enum { FTFC_OK = 0, FTFC_TIMEOUT, FTFC_ERROR } Ftfc_Status_t;

#define FLEXRAM_EEE ((volatile EEPROM_Data_t*)FlexRAM_BASE_ADDR)

extern EEPROM_Data_t EEPROM;

bool EEPROM_Init(void);
bool EEPROM_Write(const EEPROM_Data_t *EEPROM);
bool EEPROM_Is_Valid(const EEPROM_Data_t *EEPROM);
bool EEPROM_Partition(void );
bool EEPROM_Read( EEPROM_Data_t *Data);

int16_t  Temp_To_Fixed(float t);
float    Fixed_To_Temp(int16_t f);
uint16_t Amps_To_Fixed(float a);
float    Fixed_To_Amps(uint16_t f);
uint32_t Millis_To_Fixed(float ms);
float    Fixed_To_Millis(uint32_t ms);



#endif /* EEPROM_H_ */
