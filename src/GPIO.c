#include"GPIO.h"
#include"HMI.h"
#include"ADC.h"
#include"NVIC.h"
#include <math.h>
static volatile uint32_t Press_Start_Tick=0;
volatile uint32_t Last_Tick[BTN_COUNT]={0};

static float V_Temp=0;
static float R_Temp=0;
static float Temp_k=0;
static bool Is_Pressed=0;


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
	base->PTOR = pins;
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
	  NVIC_SetPriority(PORTE_IRQn, 2);
	  NVIC_EnableIRQ(PORTE_IRQn);

      //PTE8-temp--
	  IP_PORTE->PCR[8] &=~(0x0f<<PORT_PCR_IRQC_SHIFT);
	  IP_PORTE->PCR[8] |= PORT_PCR_IRQC(Rising_Edge);
	  NVIC_SetPriority(PORTE_IRQn, 2);
	  NVIC_EnableIRQ(PORTE_IRQn);

  }


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
		            ADC_Data.ADC_Compressor_Val = raw_voltage;
		            break;
		        case ADC_BLOWER_CT:
		            ADC_Data.ADC_Blower_Val = raw_voltage;
		            break;
		        case ADC_TEMP_SENSOR:
		            V_Temp = raw_voltage;
		            if(V_Temp >= VREF-0.001f || V_Temp <= 0.001f){
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

		            Temp_k = (T0 * Beta) / (Beta + T0 * logf(R_Temp / R0));
		            if(Temp_k < 233.15f || Temp_k > 423.15f){
		                Event = Event_Error;
		                Current_Error = Error_Event_ADC;
		                break;
		            }
		            ADC_Data.Temp_Sensor_Val = Temp_k - 273.15f;

		            if(HMI.error_flag == error_flag_set && HMI.Display_Error_Code == Error_Event_ADC){
		                Event = Event_Error_Clear;
		                Current_Error = Error_ADC_Clear;
		            }
		            break;

		        default:
		            break;
		    }

		    ADC_Ctrl.Status = ADC_FREE;

}


void PORTA_IRQHandler(void){
	uint32_t flags =IP_PORTA->ISFR;
	//PTA2 -HPSW
	if((flags>>HPSW_FLAG)&0x01U){

		//HPSW interrupt handler set the error flag
			if(Debounce_Check(BTN_HPSW)){
			 Is_Pressed=(PINS_DRV_ReadPins(IP_PTA)>>SW_PIN_HPSW)&0x01;
				//rising edge ->hpsw error has happened !!
				              if(Is_Pressed){
								 Event=Event_Error;
								 Current_Error=Error_Event_HPSW;
								                }
			  //falling edge ->hpsw error has cleared !!
				              else{
					            Event=Event_Error_Clear;
					            Current_Error= Error_HPSW_Clear;
					                            }

			         }
			 IP_PORTA->ISFR|=(1<<HPSW_FLAG);

			}
	//PTA3 -LPSW
	if((flags>>LPSW_FLAG)&0x01U){
		//LPSW interrupt handler set the error flag
			if(Debounce_Check(BTN_LPSW)){
				 Is_Pressed=(PINS_DRV_ReadPins(IP_PTA)>>SW_PIN_LPSW)&0x01;
				 //rising edge ->lpsw error has happened !!
				 if(Is_Pressed){
						Event=Event_Error;
					    Current_Error=Error_Event_LPSW;
				                }
	           //falling edge ->lpsw error has cleared !!
				 else {
	               	Event=Event_Error_Clear;
	               	Current_Error= Error_LPSW_Clear;
	               }

	        }
			IP_PORTA->ISFR|=(1<<LPSW_FLAG);

		}
}

void PORTB_IRQHandler(void){
	//PTB5 temp++
	//PTB4 PWR

	uint32_t flags =IP_PORTB->ISFR;
if((flags>>TEMP_INC_FLAG)&0x01U){

    // temperature increase interrupt handler
         if(Debounce_Check(BTN_TEMP_INC)){
            Event=Event_Increase_Temp;
         }

         IP_PORTB->ISFR|=(1<<TEMP_INC_FLAG);
	}

if((flags>>PWR_FLAG)&0x01U){
		 //PWR interrupt handler
		 Is_Pressed=(PINS_DRV_ReadPins(IP_PTB)>>SW_PIN_PWR)&0x01;
				if(Is_Pressed){

				 if(Debounce_Check(BTN_PWR)){
					 Press_Start_Tick=Global_Tick_Count;
				         }
				       }

				else{
					if(Debounce_Check(BTN_PWR)){
						if(Press_Start_Tick==0){
							IP_PORTB->ISFR|=(1<<PWR_FLAG);
							return;
						}
		                  if((Global_Tick_Count-Press_Start_Tick)>=LONG_PRESS_MS){
		                	  //long press detected !! change current mode .
		                	  Event=Event_Mode;
		                  }
		                  else{
		                	  //on or off the machine
		                	  Event=Event_Machine_status;
		                  }
		                  Press_Start_Tick=0;
					}
				}

	IP_PORTB->ISFR|=(1<<PWR_FLAG);

	}



}
void PORTC_IRQHandler(void){

	//PTC2 -compressor switch
	//PTC3 -Heater Switch

	uint32_t flags =IP_PORTC->ISFR;

	 if((flags>>COMPRESSOR_SW_FLAG)&0x01U){
		 if(Debounce_Check(BTN_COMPRESSOR_SW)){
					Event=Event_User_Compressor;

		         }
   IP_PORTC->ISFR|=(1<<COMPRESSOR_SW_FLAG);
			}
	 if((flags>>HEATER_SW_FLAG)&0x01U){
		 if(Debounce_Check(BTN_HEATER_SW)){
					Event=Event_Heater;
				         }
   IP_PORTC->ISFR|=(1<<HEATER_SW_FLAG);

			}

}
void PORTD_IRQHandler(void){
	uint32_t flags =IP_PORTD->ISFR;
	//PTD7 -Blower Switch
	if((flags>>BLOWER_SW_FLAG)&0x01U){
		 if(Debounce_Check(BTN_BLOWER_SW)){
			 Event=Event_Blower;
						         }
		 IP_PORTD->ISFR|=(1<<BLOWER_SW_FLAG);
    }


}

void PORTE_IRQHandler(void){
	uint32_t flags =IP_PORTE->ISFR;
	//PTE8-Temp--
	if((flags>>TEMP_DEC_FLAG)&0x01U){
		 if(Debounce_Check(BTN_TEMP_DEC)){
			Event=Event_Decrease_Temp;
	 }
		 IP_PORTE->ISFR|=(1<<TEMP_DEC_FLAG);
	}
}






