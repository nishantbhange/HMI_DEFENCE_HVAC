#include"GPIO.h"
#include"HMI.h"
#include"ADC.h"
#include"NVIC.h"
#include <math.h>
 volatile uint32_t Press_Start_Tick=0;
 volatile bool Long_Press_Flag=RESET ;
 volatile bool OK_Key_Locked=RESET;
volatile uint32_t Last_Tick[BTN_COUNT]={0};

/* ---------------------------------------------------------------------
 * Noise-immune "confirm after quiet" debounce.
 *
 * Every button pin ISR does is: note that an edge happened, and restart a
 * per-button timer. Nothing is acted on there anymore. Once per ms (from
 * SysTick_Handler) Buttons_Poll_1ms() checks each button that had an edge:
 * if DEBOUNCE_MS has passed with NO further edges on that pin, the line is
 * trusted and the pin is re-sampled live.
 *
 * Any burst of edges - real contact bounce or EMI ringing, doesn't matter
 * which - just keeps re-arming the window, so nothing fires mid-burst.
 * If the line has simply settled back to the state it already had (a pure
 * glitch that comes and goes), the "level changed?" check below is false
 * and NOTHING is generated - the application never even hears about it.
 * That's the property you don't get from a bare "ignore edges closer than
 * 40ms apart" gate: that only rejects the *first* retrigger, not a second,
 * cleanly-spaced glitch, and it also has no idea whether the edge it did
 * accept was ever properly paired with its matching return edge - which is
 * exactly how Long_Press_Flag was getting stuck SET forever on PWR noise.
 * --------------------------------------------------------------------- */
static volatile uint32_t Btn_Last_Edge_Tick[BTN_COUNT] = {0};
static volatile bool     Btn_Pending[BTN_COUNT]        = {RESET};
static volatile uint8_t  Btn_Confirmed_Level[BTN_COUNT]= {0}; // meaningful only for level-type buttons: HPSW, LPSW, PWR

static inline void Btn_Note_Edge(Button_Id_t id){
    Btn_Last_Edge_Tick[id] = Global_Tick_Count;
    Btn_Pending[id]        = SET;
}

/* Call once at startup (after Interrupt_Init has cleared ISFR) so the
 * level-type buttons don't report a false "change" the first time the
 * line ever moves - we need to know what they were already sitting at. */
void Buttons_Debounce_Init(void){
    Btn_Confirmed_Level[BTN_HPSW] = (uint8_t)((PINS_DRV_ReadPins(IP_PTA)>>SW_PIN_HPSW)&0x01U);
    Btn_Confirmed_Level[BTN_LPSW] = (uint8_t)((PINS_DRV_ReadPins(IP_PTA)>>SW_PIN_LPSW)&0x01U);
    Btn_Confirmed_Level[BTN_PWR]  = (uint8_t)((PINS_DRV_ReadPins(IP_PTB)>>SW_PIN_PWR)&0x01U);
}

/* Call once per ms from SysTick_Handler. */
void Buttons_Poll_1ms(void){
	 if(!((PINS_DRV_ReadPins(IP_PTC)>>COMPRESSOR_SW_FLAG)&0x01U)){
	        OK_Key_Locked = RESET;   // pin is physically low -> unlock
	    }

    for(Button_Id_t id=(Button_Id_t)0; id<BTN_COUNT; id++){
        if(!Btn_Pending[id]) continue;
        if((Global_Tick_Count - Btn_Last_Edge_Tick[id]) < DEBOUNCE_MS) continue; // burst still settling

        Btn_Pending[id] = RESET;   // line has been quiet for a full DEBOUNCE_MS - trust it now

        switch(id){

        case BTN_HPSW: {
            uint8_t level = (uint8_t)((PINS_DRV_ReadPins(IP_PTA)>>SW_PIN_HPSW)&0x01U);
            if(level != Btn_Confirmed_Level[id]){
                Btn_Confirmed_Level[id] = level;
                if(!level){ Event=Event_Error;       Current_Error=Error_Event_HPSW; }
                else     { Event=Event_Error_Clear;  Current_Error=Error_HPSW_Clear; }
            }
            break;
        }

        case BTN_LPSW: {
            uint8_t level = (uint8_t)((PINS_DRV_ReadPins(IP_PTA)>>SW_PIN_LPSW)&0x01U);
            if(level != Btn_Confirmed_Level[id]){
                Btn_Confirmed_Level[id] = level;
                if(!level){ Event=Event_Error;       Current_Error=Error_Event_LPSW; }
                else     { Event=Event_Error_Clear;  Current_Error=Error_LPSW_Clear; }
            }
            break;
        }

        case BTN_PWR: {
            uint8_t level = (uint8_t)((PINS_DRV_ReadPins(IP_PTB)>>SW_PIN_PWR)&0x01U);
            if(level != Btn_Confirmed_Level[id]){
                Btn_Confirmed_Level[id] = level;
                if(level){
                    // confirmed, sustained press (held down through 40ms of quiet)
                    Press_Start_Tick = Global_Tick_Count;
                    Long_Press_Flag  = SET;
                } else {
                    // confirmed, sustained release - this can never be lost now,
                    // so Long_Press_Flag can never get stuck SET again.
                    Long_Press_Flag = RESET;
                    if(Press_Start_Tick!=0 && (Global_Tick_Count-Press_Start_Tick) < LONG_PRESS_MS){
                        Event = Event_Machine_status;   // genuine short press-release
                    }
                    // else: it was a long hold - SysTick_Handler's timeout path
                    // already fired Event_Mode while it was still held down.
                    Press_Start_Tick = 0;
                }
            }
            break;
        }

        case BTN_TEMP_INC:
            // Rising-edge-only pin: no interrupt on release, so re-sample live.
            // If it's still high 40ms after the edge that woke us, the pulse
            // outlasted the whole debounce window - a bare EMI spike won't.
            if((PINS_DRV_ReadPins(IP_PTB)>>TEMP_INC_FLAG)&0x01U){
                bool CompSw_Also_Held = (PINS_DRV_ReadPins(IP_PTC)>>COMPRESSOR_SW_FLAG)&0x01U;
                if(!(CompSw_Also_Held && UI_State==UI_Normal)){
                    Event = Event_Decrease_Temp;
                }
            }
            break;

        case BTN_TEMP_DEC:
            if((PINS_DRV_ReadPins(IP_PTE)>>TEMP_DEC_FLAG)&0x01U){
                Event = Event_Increase_Temp;
            }
            break;

        case BTN_COMPRESSOR_SW:
            if((PINS_DRV_ReadPins(IP_PTC)>>COMPRESSOR_SW_FLAG)&0x01U){
                if(!OK_Key_Locked){
                    Event = Event_User_Compressor;
                }
            }
            break;

        case BTN_HEATER_SW:
            if((PINS_DRV_ReadPins(IP_PTC)>>HEATER_SW_FLAG)&0x01U){
                Event = Event_User_Heater;
            }
            break;

        case BTN_BLOWER_SW:
            if((PINS_DRV_ReadPins(IP_PTD)>>BLOWER_SW_FLAG)&0x01U){
                Event = Event_Blower;
            }
            break;

        default: break;
        }
    }
}


static float V_Temp=0;
static float R_Temp=0;
static float Temp_k=0;
static volatile float V_at_Rated_curr ;

#define ADC_MAX     4095.0f
#define VREF        5.0f
#define R0          5000.0f
#define Rfixed      5000.0f
#define T0          298.15f
#define Beta        3470.0f



 void PINS_DRV_ClearPins(GPIO_Type * const base, uint32_t pins)
{
	base->PCOR=pins;
}

  void PINS_DRV_SetPins(GPIO_Type * const base , uint32_t pins)
{
     base->PSOR=pins;
}
  void PINS_DRV_TogglePins(GPIO_Type * const base , uint32_t pins)
{
	base->PTOR= pins;
}

  uint32_t PINS_DRV_ReadPins(GPIO_Type * const base)
{
    return base->PDIR;
}

  void Interrupt_Init(void ){

	  IP_PORTB->ISFR = 0xFFFFFFFF;
	  IP_PORTC->ISFR = 0xFFFFFFFF;
	  IP_PORTD->ISFR = 0xFFFFFFFF;
	  IP_PORTE->ISFR = 0xFFFFFFFF;
	  IP_PORTA->ISFR = 0xFFFFFFFF;
	  //PTB5 temp++
	  //PTB4 PWR
	  IP_PORTB->PCR[5] &=~(0x0f<<PORT_PCR_IRQC_SHIFT);
	  IP_PORTB->PCR[5] |= PORT_PCR_IRQC(Rising_Edge);


	  IP_PORTB->PCR[4] &=~(0x0f<<PORT_PCR_IRQC_SHIFT);
	  IP_PORTB->PCR[4] |= PORT_PCR_IRQC(Either_Edge);
	  NVIC_SetPriority(PORTB_IRQn, 2);
	  NVIC_EnableIRQ(PORTB_IRQn);


	 //PTC0 -ADC channel 8
	 //PTC1 -ADC channel 9
	 //PTC2 -compressor switch
	 //PTC3 -Heater Switch
	 //PTC17 -ADC channel 15
	 //PTC15 -ADC channel 12

	 IP_PORTC->PCR[2] &=~(0x0f<<PORT_PCR_IRQC_SHIFT);
	 IP_PORTC->PCR[2] |= PORT_PCR_IRQC(Rising_Edge);


	 IP_PORTC->PCR[3] &=~(0x0f<<PORT_PCR_IRQC_SHIFT);
	 IP_PORTC->PCR[3] |=PORT_PCR_IRQC(Rising_Edge);
	 NVIC_SetPriority(PORTC_IRQn, 2);
	 NVIC_EnableIRQ(PORTC_IRQn);


	  //PTA2 -HPSW
	  //PTA3 -LPSW
	  IP_PORTA->PCR[2] &=~(0x0f<<PORT_PCR_IRQC_SHIFT);
	  IP_PORTA->PCR[2] |= PORT_PCR_IRQC(Either_Edge);


	  IP_PORTA->PCR[3] &=~(0x0f<<PORT_PCR_IRQC_SHIFT);
	  IP_PORTA->PCR[3] |= PORT_PCR_IRQC(Either_Edge);
	  NVIC_SetPriority(PORTA_IRQn, 1);
	  NVIC_EnableIRQ(PORTA_IRQn);


	  //PTD7-blower
	  IP_PORTD->PCR[7] &=~(0x0f<<PORT_PCR_IRQC_SHIFT);
	  IP_PORTD->PCR[7] |= PORT_PCR_IRQC(Rising_Edge);
	  NVIC_SetPriority(PORTD_IRQn, 2);
	  NVIC_EnableIRQ(PORTD_IRQn);

      //PTE8-temp--
	  IP_PORTE->PCR[8] &=~(0x0f<<PORT_PCR_IRQC_SHIFT);
	  IP_PORTE->PCR[8] |= PORT_PCR_IRQC(Rising_Edge);
	  NVIC_SetPriority(PORTE_IRQn, 2);
	  NVIC_EnableIRQ(PORTE_IRQn);

	  // ADC interrupt enable
	  NVIC_SetPriority(ADC0_IRQn,2);
	  NVIC_EnableIRQ(ADC0_IRQn);

	  Buttons_Debounce_Init();

  }


/* Superseded by Buttons_Poll_1ms()'s confirm-after-quiet debounce above.
 * Left unused rather than deleted in case anything else in the project
 * still links against it - safe to remove once you've confirmed nothing
 * else calls it. */
static bool Debounce_Check(Button_Id_t id) {
    if ((Global_Tick_Count - Last_Tick[id]) > DEBOUNCE_MS) {
        Last_Tick[id] = Global_Tick_Count;
        return true;
    }
    return false;
}
//in s32k144 using RTD,  interrupts are delivered through INTctrl , startup file belongs to INTctrl
//these handlers function pointer is modified in startup file so make sure to change vector table

void ADC0_IRQHandler(void){
	    //PTC0 -ADC channel 8
		//PTC1 -ADC channel 9
		//PTC17 -ADC channel 15
		//PTC15 -ADC channel 12

		 if((IP_ADC0->SC1[0] & ADC_SC1_COCO_MASK) == 0){
		        return;   //conversion not actually done
		    }

		    float raw_voltage = ((float)IP_ADC0->R[0] * VREF) / ADC_MAX;

		    switch(Current_ADC_Channel){
		        case ADC_CONDENSER_CT:
		            ADC_Data.ADC_Condenser_Val = raw_voltage;
		            break;
		        case ADC_COMPRESSOR_CT:
              //oc current value is a input from user in runtime !!
		        	 V_at_Rated_curr=((HMI.OC_Current_Val)*0.311f)/3.0f;
		        	 ADC_Data.ADC_Compressor_Val = raw_voltage;
		            if( raw_voltage>= V_at_Rated_curr){

		            	 if(Check_Status_Flag==RESET){
		            	        Compressor_Overcurrent_Time_ms=0;   // only zero it the moment overcurrent begins
		            	    }

		            	Check_Status_Flag=SET;


		            	if( raw_voltage<(1.5f*V_at_Rated_curr) && Compressor_Overcurrent_Time_ms>=HMI.OC_Time_Val)
		            	{
		            		Event=Event_Error;
		            		Current_Error=Error_Event_OC;
		            		Compressor_Overcurrent_Time_ms=0;
		            		Check_Status_Flag=RESET;

		            	}
		            	else if( raw_voltage>=(1.5f*V_at_Rated_curr) && raw_voltage<(2.0f*V_at_Rated_curr) && Compressor_Overcurrent_Time_ms>=TICK_COUNT_30SEC){

		            		Event=Event_Error;
		            		Current_Error=Error_Event_OC;
		            		Compressor_Overcurrent_Time_ms=0;
		            		Check_Status_Flag=RESET;

		            	}
		            	else if( raw_voltage>=(2.0f*V_at_Rated_curr) && Compressor_Overcurrent_Time_ms>=TICK_COUNT_5SEC){

		            		Event=Event_Error;
		            		Current_Error=Error_Event_OC;
		            		Compressor_Overcurrent_Time_ms=0;
		            		Check_Status_Flag=RESET;

		            		}



		            }else{

		            	Check_Status_Flag=RESET;
		            	Compressor_Overcurrent_Time_ms=0;
	                  if(HMI.error_flag==error_flag_set && HMI.Display_Error_Code[OC_ERROR_INDEX]==Error_Event_OC){
		            	Event = Event_Error_Clear;
		            	Current_Error = Error_OC_Clear;
		            	}

		            }

		            break;
		        case ADC_BLOWER_CT:
		            ADC_Data.ADC_Blower_Val = raw_voltage;
		            break;
		        case ADC_TEMP_SENSOR:
		            V_Temp = raw_voltage;
		            if(V_Temp >= VREF-0.001f || V_Temp <= 0.001f ){
		                Event = Event_Error;
		                Current_Error = Error_Event_ADC;
		                break;
		            }
		            R_Temp = (V_Temp * Rfixed) / (VREF - V_Temp);
		            if(R_Temp <= 0){
		                Event = Event_Error;
		                Current_Error = Error_Event_ADC;
		                break;
		            }

		            Temp_k = (T0 * Beta) / (Beta + T0 * logf(R_Temp / R0) );
		            if(Temp_k < 233.15f || Temp_k > 423.15f){
		                Event = Event_Error;
		                Current_Error = Error_Event_ADC;
		                break;
		            }
		            ADC_Data.Temp_Sensor_Val = Temp_k - 273.15f;

		            if(HMI.error_flag==error_flag_set && HMI.Display_Error_Code[ADC_ERROR_INDEX]==Error_Event_ADC){
		                Event = Event_Error_Clear;
		                Current_Error = Error_ADC_Clear;
		            }
		            break;

		        default:
		            break;
		    }
		    ADC_Bsy_Timeout_Flag = RESET;
		    ADC_Bsy_Timeout_Count = 0;
		    ADC_Ctrl.Status = ADC_FREE;

}


void PORTA_IRQHandler(void){
	uint32_t flags =IP_PORTA->ISFR;
	//PTA2 -HPSW
	if((flags>>HPSW_FLAG)&0x01U){
		// Don't decide anything here - just mark that the line moved and
		// let Buttons_Poll_1ms act once it's been quiet for DEBOUNCE_MS.
		Btn_Note_Edge(BTN_HPSW);
		IP_PORTA->ISFR|=(1<<HPSW_FLAG);
	}
	//PTA3 -LPSW
	if((flags>>LPSW_FLAG)&0x01U){
		Btn_Note_Edge(BTN_LPSW);
		IP_PORTA->ISFR|=(1<<LPSW_FLAG);
	}
}

void PORTB_IRQHandler(void){
	//PTB5 temp++
	//PTB4 PWR
	uint32_t flags =IP_PORTB->ISFR;

	if((flags>>TEMP_INC_FLAG)&0x01U){
		Btn_Note_Edge(BTN_TEMP_INC);
		IP_PORTB->ISFR|=(1<<TEMP_INC_FLAG);
	}

	if((flags>>PWR_FLAG)&0x01U){
		// Every press AND every release edge now reliably restarts the
		// confirm window - the release can no longer be swallowed by a
		// time gate the way it was with Debounce_Check, so Long_Press_Flag
		// can't get stranded SET by a glitch anymore.
		Btn_Note_Edge(BTN_PWR);
		IP_PORTB->ISFR|=(1<<PWR_FLAG);
	}
}
void PORTC_IRQHandler(void){

	//PTC2 -compressor switch
	//PTC3 -Heater Switch

	uint32_t flags =IP_PORTC->ISFR;

	if((flags>>COMPRESSOR_SW_FLAG)&0x01U){
		Btn_Note_Edge(BTN_COMPRESSOR_SW);
		IP_PORTC->ISFR|=(1<<COMPRESSOR_SW_FLAG);
	}
	if((flags>>HEATER_SW_FLAG)&0x01U){
		Btn_Note_Edge(BTN_HEATER_SW);
		IP_PORTC->ISFR|=(1<<HEATER_SW_FLAG);
	}
}
void PORTD_IRQHandler(void){
	uint32_t flags =IP_PORTD->ISFR;
	//PTD7 -Blower Switch
	if((flags>>BLOWER_SW_FLAG)&0x01U){
		Btn_Note_Edge(BTN_BLOWER_SW);
		IP_PORTD->ISFR|=(1<<BLOWER_SW_FLAG);
	}
}

void PORTE_IRQHandler(void){
	uint32_t flags =IP_PORTE->ISFR;
	//PTE8-Temp--
	if((flags>>TEMP_DEC_FLAG)&0x01U){
		Btn_Note_Edge(BTN_TEMP_DEC);
		IP_PORTE->ISFR|=(1<<TEMP_DEC_FLAG);
	}
}
