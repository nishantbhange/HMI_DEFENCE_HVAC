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

typedef struct{
	uint32_t                Magic_No;
	float                   Set_Temp;
	Mode_t                  Curr_Mode;
	Condenser_t             Condenser_state;
	Heater_t                heater_state;
	Vent_t                  vent_state;
	Machine_Status_t        AC_State;
	Compressor_t            User_Compressor_state;
	float                   Curr_Temp;
	ErrorCode_t             ErrorCode ;
	bool                    Error_Present ;

	//ERROR logs like error counts


}EEPROM_Data_t;

// _Static_assert((sizeof(EEPROM_Data_t) % 4U) == 0U,"EEPROM_Data_t must be word aligned");


#define FLEXRAM_EEE ((volatile EEPROM_Data_t*)FlexRAM_BASE_ADDR)

extern EEPROM_Data_t EEPROM;

bool EEPROM_Init(void);
bool EEPROM_Write(const EEPROM_Data_t *EEPROM);
bool EEPROM_Is_Valid(const EEPROM_Data_t *EEPROM);
bool EEPROM_Partition(void );
bool EEPROM_Read( EEPROM_Data_t *Data);





#endif /* EEPROM_H_ */
