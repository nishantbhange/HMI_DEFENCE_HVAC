/*
 * EEPROM.c
 * space
 *  Created on: Jul 3, 2026
 *      Author: Nishant Bhange
 */
#include"EEPROM.h"
#include "Wdog_Ip.h"
#include<math.h>
EEPROM_Data_t EEPROM;
static void Wait_CCIF(void);
static void Wait_EEERDY(void);
static bool Config_Ram_Operation(void );

volatile bool CCIF_TIMEOUT_FLAG;
volatile bool EEERDY_TIMEOUT_FLAG;
volatile uint32_t CCIF_TIMEOUT_COUNT;
volatile uint32_t EEERDY_TIMEOUT_COUNT;


static const char* Error_Msg1="WARNING EEPROM";
static const char* Error_Msg3="INIT FAILED";

// The on-flash wrapper. EEPROM.c-internal only - nothing outside this file
// ever needs to know a record has a magic number, a version, or a CRC;
 // everywhere else in the codebase only ever sees EEPROM_Data_t, exactly as
 // before.
typedef struct {
    uint32_t       magic;    /* 0x00 */
    uint16_t       version;  /* 0x04 */
    uint16_t       length;   /* 0x06 */
    EEPROM_Data_t  data;     /* 0x08, 24 bytes */
    uint32_t       crc32;    /* 0x20 */
} EEPROM_Record_t;

_Static_assert(sizeof(EEPROM_Record_t) == 36U, "EEPROM_Record_t_size_must_be_36_bytes");
_Static_assert((sizeof(EEPROM_Record_t) % 4U) == 0U, "EEPROM_Record_t_must_be_word_aligned");

#define FLEXRAM_RECORD  ((volatile EEPROM_Record_t*)FlexRAM_BASE_ADDR)


/* EEPROM_Data_t must remain naturally aligned.
 * This write routine assumes sizeof(EEPROM_Data_t)
 * is a multiple of sizeof(uint32_t).
 */

 int16_t Temp_To_Fixed(float t)
{
    if (!(t == t))   { return 0; }           /* NaN */
    if (t < -99.0f)  { t = -99.0f; }
    if (t > 199.0f)  { t = 199.0f; }
    return (int16_t)lrintf(t * 10.0f);
}
 float Fixed_To_Temp(int16_t f) { return ((float)f) / 10.0f; }

 uint16_t Amps_To_Fixed(float a)
{
    if (!(a == a))     { return 0U; }
    if (a < 0.0f)      { a = 0.0f; }
    if (a > 655.35f)   { a = 655.35f; }
    return (uint16_t)lrintf(a * 100.0f);
}
 float Fixed_To_Amps(uint16_t f) { return ((float)f) / 100.0f; }

 uint32_t Millis_To_Fixed(float ms)
{
    if (!(ms == ms))         { return 0U; }
    if (ms < 0.0f)           { ms = 0.0f; }
    if (ms > 4294967040.0f)  { ms = 4294967040.0f; }
    return (uint32_t)ms;
}
 float Fixed_To_Millis(uint32_t ms) { return (float)ms; }

/* Small bitwise CRC-32 (CRC-32/ISO-HDLC). No lookup table: this runs once
 * per commit, well under the flash write time itself. */
static uint32_t Crc32(const uint8_t *data, uint32_t len)
{
    uint32_t crc = 0xFFFFFFFFU;
    for (uint32_t i = 0U; i < len; i++)
    {
        crc ^= data[i];
        for (uint32_t bit = 0U; bit < 8U; bit++)
        {
            crc = (crc & 1U) ? ((crc >> 1) ^ 0xEDB88320U) : (crc >> 1);
        }
    }
    return ~crc;
}

static bool Record_Is_Valid(const EEPROM_Record_t *rec)
{
    if (rec == NULL)                             { return false; }
    if (rec->magic   != MAGIC_NO)                { return false; }
    if (rec->version != EEPROM_FORMAT_VERSION)   { return false; }
    if (rec->length  != sizeof(EEPROM_Record_t)) { return false; }
    uint32_t computed = Crc32((const uint8_t *)rec, offsetof(EEPROM_Record_t, crc32));
    return (computed == rec->crc32);
}


bool EEPROM_Read(EEPROM_Data_t *Data)
{
    if (Data == NULL) { return false; }

    EEPROM_Record_t rec = *FLEXRAM_RECORD;   /* one volatile struct read */
    if (!Record_Is_Valid(&rec)) { return false; }

    *Data = rec.data;
    return true;
}

/* EEPROM_Data_t must remain naturally aligned - guaranteed now by the
 * explicit field ordering in EEPROM.h, not by luck. */
bool EEPROM_Write(const EEPROM_Data_t *Data)
{
    if (Data == NULL) { return false; }

    EEPROM_Record_t rec;
    rec.magic   = MAGIC_NO;
    rec.version = EEPROM_FORMAT_VERSION;
    rec.length  = sizeof(EEPROM_Record_t);
    rec.data    = *Data;
    rec.crc32   = Crc32((const uint8_t *)&rec, offsetof(EEPROM_Record_t, crc32));

    volatile uint32_t *Destination = (volatile uint32_t *)FlexRAM_BASE_ADDR;
    const uint32_t    *source      = (const uint32_t *)&rec;

    for (uint32_t i = 0U; i < (sizeof(EEPROM_Record_t)/sizeof(uint32_t)); i++)
    {
        /* The CCIF/EEERDY handshake here is unchanged from before, and is
         * a SEPARATE, already-identified issue (the earlier review's S-7:
         * no watchdog service inside Wait_CCIF, and CCIF used where
         * EEERDY is the documented interlock for FlexRAM EEE writes).
         * Out of scope for this fix - apply that patch on top of this
         * one, not instead of it. */
        Wait_CCIF();
        Destination[i] = source[i];
        Wait_CCIF();
        if ((IP_FTFC->FSTAT) & ((1<<FTFC_FSTAT_BIT_FPVIOL) |
                                 (1<<FTFC_FSTAT_BIT_MGSTAT0) |
                                 (1<<FTFC_FSTAT_BIT_ACCERR)  |
                                 (1<<FTFC_FSTAT_BIT_RDCOLERR)))
        {
            return false;
        }
    }
    return true;
}

bool EEPROM_Init(void)
{
    if (!EEPROM_Partition())        { return false; }
    if (!Config_Ram_Operation())    { return false; }
    Wait_EEERDY();
    return true;
}

bool EEPROM_Partition(void)
{
    /* unchanged from the existing implementation */
    uint8_t Depart     = (IP_SIM->FCFG1 >> SIM_FCFG1_BIT_DEPART) & 0x0F;
    uint8_t EEERAMSIZE = (IP_SIM->FCFG1 >> SIM_FCFG1_BIT_EEERAMSIZE) & 0x0F;

    if (Depart == 0x0F)
    {
        Wait_CCIF();
        IP_FTFC->FSTAT = ((1<<FTFC_FSTAT_BIT_FPVIOL)|(1<<FTFC_FSTAT_BIT_ACCERR)|(1<<FTFC_FSTAT_BIT_RDCOLERR));
        IP_FTFC->FCCOB[3] = 0x80;
        IP_FTFC->FCCOB[2] = 0x00;
        IP_FTFC->FCCOB[1] = 0x00;
        IP_FTFC->FCCOB[0] = 0x00;
        IP_FTFC->FCCOB[7] = 0x02;
        IP_FTFC->FCCOB[6] = 0x04;
        IP_FTFC->FSTAT = (1<<FTFC_FSTAT_BIT_CCIF);
        Wait_CCIF();
        if ((IP_FTFC->FSTAT) & ((1<<FTFC_FSTAT_BIT_FPVIOL) | (1<<FTFC_FSTAT_BIT_MGSTAT0) |
                                 (1<<FTFC_FSTAT_BIT_ACCERR) | (1<<FTFC_FSTAT_BIT_RDCOLERR)))
        {
            return false;
        }
        return true;
    }
    else
    {
        return (Depart == 0x04 && EEERAMSIZE == 0x02);
    }
}

static void Wait_CCIF(void ){
	CCIF_TIMEOUT_FLAG=SET;
	  while(!((IP_FTFC->FSTAT>>FTFC_FSTAT_BIT_CCIF)&(0x01))){
		   Wdog_Ip_Service(WDOG_INST);
             if(CCIF_TIMEOUT_COUNT>TICK_COUNT_5SEC){
    	    	 LCD_Clear();
    	    	 LCD_String_XY(0,0,Error_Msg1);
    	    	 LCD_String_XY(1,0,Error_Msg3);
    	    	 DelayMs(Delay_1_SEC);
    	    	 Wdog_Ip_Service(WDOG_INST);
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
		  Wdog_Ip_Service(WDOG_INST);
		EEERDY_TIMEOUT_FLAG=SET;
		if(EEERDY_TIMEOUT_COUNT>TICK_COUNT_5SEC){
	    	 LCD_Clear();
	    	 LCD_String_XY(0,0,Error_Msg1);
	    	 LCD_String_XY(1,0,Error_Msg3);
	    	 DelayMs(Delay_1_SEC);
	    	 Wdog_Ip_Service(WDOG_INST);
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

