/*
 * EEPROM.c
 * space
 *  Created on: Jul 3, 2026
 *      Author: Nishant Bhange
 */
#include"EEPROM.h"
EEPROM_Data_t EEPROM;
static void Wait_CCIF(void);
static void Wait_EEERDY(void);
static bool Config_Ram_Operation(void );

volatile bool CCIF_TIMEOUT_FLAG;
volatile bool EEERDY_TIMEOUT_FLAG;
volatile uint32_t CCIF_TIMEOUT_COUNT;
volatile uint32_t EEERDY_TIMEOUT_COUNT;

bool EEPROM_Read( EEPROM_Data_t *Data){
	if(Data==NULL){
		return false;
	}

	*Data=*FLEXRAM_EEE;
    return true;
}
/* EEPROM_Data_t must remain naturally aligned.
 * This write routine assumes sizeof(EEPROM_Data_t)
 * is a multiple of sizeof(uint32_t).
 */
bool EEPROM_Write(const EEPROM_Data_t *Data){

	if(Data==NULL){
		return false;
	}

	volatile uint32_t * Destination =(volatile uint32_t *)FLEXRAM_EEE;
	const uint32_t * source =(const uint32_t *)Data;
	//reading a complete word instead of byte
	for (uint32_t i=0 ; i< (sizeof(EEPROM_Data_t)/sizeof(uint32_t )) ; i++){

		Wait_CCIF();
		Destination[i]=source[i];
		Wait_CCIF();
		//check FPVIOL/ACCER/RDCOLERR/MGSTAT0 flag
		if(	(IP_FTFC->FSTAT)&((1<<FTFC_FSTAT_BIT_FPVIOL) |
			    		(1 << FTFC_FSTAT_BIT_MGSTAT0)    |
			    		(1<<FTFC_FSTAT_BIT_ACCERR)       |
						 (1<<FTFC_FSTAT_BIT_RDCOLERR)))
		                  {
			    	         return false;
			              }
	}

    return true;

}

 bool EEPROM_Init(void){

if(! EEPROM_Partition()){
	return false ;
}
 if(! Config_Ram_Operation()){
	 return false ;
 }
 Wait_EEERDY();
return true ;

}

bool EEPROM_Is_Valid(const EEPROM_Data_t *EEPROM){
	if(EEPROM==NULL){
		return false;
	}

	return (EEPROM->Magic_No==MAGIC_NO);

}

bool EEPROM_Partition(void){
	uint8_t Depart=(IP_SIM->FCFG1>>SIM_FCFG1_BIT_DEPART)&0x0F;
	uint8_t EEERAMSIZE=(IP_SIM->FCFG1>>SIM_FCFG1_BIT_EEERAMSIZE)&0x0F;

    if(Depart==0x0F){

	//wait untill ftfc is ready (check ccif flag)
	Wait_CCIF();

	//clear previous error flags
	IP_FTFC->FSTAT=((1<<FTFC_FSTAT_BIT_FPVIOL)|(1<<FTFC_FSTAT_BIT_ACCERR)|(1<<FTFC_FSTAT_BIT_RDCOLERR));


	// FCCOB0: Selects the PGMPART command
	IP_FTFC->FCCOB[3]=0x80;
	// FCCOB1: No CSEC operation
	IP_FTFC->FCCOB[2]=0x00;
	// FCCOB2: No CSEC operation
	IP_FTFC->FCCOB[1]=0x00;
	// FCCOB3: FlexRAM loaded with valid EEPROM during reset sequence
	IP_FTFC->FCCOB[0]=0x00;
	//FCCOB4: EEPROM data set size code: EEESIZE =4 kB
	IP_FTFC->FCCOB[7]=0x02;
	IP_FTFC->FCCOB[6]=0x04;

    //clear CCIF flag by setting it to 1 to launch a command

    IP_FTFC->FSTAT =(1<<FTFC_FSTAT_BIT_CCIF);

    //wait till launching command is done
    Wait_CCIF();
    //check if there is any error
    if(	(IP_FTFC->FSTAT)&((1<<FTFC_FSTAT_BIT_FPVIOL) |
    		(1 << FTFC_FSTAT_BIT_MGSTAT0)           |
    		(1<<FTFC_FSTAT_BIT_ACCERR)               |
			 (1<<FTFC_FSTAT_BIT_RDCOLERR))){
    	return false;
    }

    return true;

   }


   else{

	   if(Depart==0x04 && EEERAMSIZE==0x02){

		 return true;

	   }
	   else{

	      return false;

	   }
   }
}

static void Wait_CCIF(void ){
	CCIF_TIMEOUT_FLAG=SET;
	  while(!((IP_FTFC->FSTAT>>FTFC_FSTAT_BIT_CCIF)&(0x01))){
             if(CCIF_TIMEOUT_COUNT>TICK_COUNT_5SEC){
            	 CCIF_TIMEOUT_COUNT=0;
            	 CCIF_TIMEOUT_FLAG=RESET;
            	break ;
             }
	    	}
	CCIF_TIMEOUT_FLAG=RESET;
	CCIF_TIMEOUT_COUNT=0;
}

static void Wait_EEERDY(void){

	while(!((IP_FTFC->FCNFG>>FTFC_FCNFG_BIT_EEERDY )&(0x01))){
		EEERDY_TIMEOUT_FLAG=SET;
		if(EEERDY_TIMEOUT_COUNT>TICK_COUNT_5SEC){
		    EEERDY_TIMEOUT_COUNT=0;
			EEERDY_TIMEOUT_FLAG=RESET;
		        break ;
		             }
	}
	 EEERDY_TIMEOUT_FLAG=RESET;
	 EEERDY_TIMEOUT_COUNT=0;
}
static bool Config_Ram_Operation(void ){

	//wait untill ftfc is ready (check ccif flag)
		Wait_CCIF();

	//clear previous error flags
		IP_FTFC->FSTAT=((1<<FTFC_FSTAT_BIT_FPVIOL)|
				(1<<FTFC_FSTAT_BIT_ACCERR)        |
				(1<<FTFC_FSTAT_BIT_RDCOLERR));

	// FCCOB0: Selects the PGMPART command 0x81 for configuring ram
			IP_FTFC->FCCOB[3]=0x81;
	// FCCOB1: Make FlexRAM available for emulated EEPROM
			IP_FTFC->FCCOB[2]=0x00;
	 //clear CCIF flag by setting it to 1 to launch a command
   		    IP_FTFC->FSTAT =(1<<FTFC_FSTAT_BIT_CCIF);
	// check if command execution is completed
			Wait_CCIF();
	//check error flags
			if ((IP_FTFC->FSTAT &
			        ((1 << FTFC_FSTAT_BIT_FPVIOL)   |
			         (1 << FTFC_FSTAT_BIT_ACCERR)   |
			         (1 << FTFC_FSTAT_BIT_RDCOLERR) |
			         (1 << FTFC_FSTAT_BIT_MGSTAT0))) )
			    {
			        return false;
			    }
			return true ;


}

