/*
 * HMI.c
 *
 *  Created on: Jul 1, 2026
 *      Author: Nishant Bhange
 */
#include"HMI.h"
#include"EEPROM.h"
 volatile HMI_t HMI;
 volatile uint32_t Global_Tick_Count =0 ;
 static uint32_t Systick_Tick_Count =0;
 static uint8_t Prev_Mode=(Mode_t)0xFF;
 static uint8_t Prev_Set_Temp=0xFF;
 static uint8_t Prev_Curr_Temp=0xFF;
 EEPROM_Data_t First_EEPROM_Data;
 volatile HMI_Event_t Event;
 volatile ErrorCode_t Current_Error;



 void HMI_Process_Event(HMI_Event_t Event ){
	switch(Event){

	    case Event_Machine_status :

	    	                    if(HMI.status==AC_on){
	    							HMI.status=AC_off;
	    							//turn off the ac .
	    							}
	    						else if(HMI.status==AC_off){
	    							HMI.status=AC_on;
	    							//turn on the ac .

	    						}

	                            EEPROM.AC_State=HMI.status;
	                        break;

	     case Event_Mode :
	    	                        if(HMI.mode==Auto_Mode){
	    		                           HMI.mode=Manual_mode;

	    	                                               }
	    	                        else{
	    		                           HMI.mode=Auto_Mode;

	    	                                               }
	    	                            EEPROM.Curr_Mode=HMI.mode;
	    	                        break;


	   case Event_Increase_Temp :
		                    if( HMI.set_temp>=MAX_TEMP){
		                    	HMI.set_temp=MAX_TEMP;
		                    }
		                    else{
	 	                        HMI.set_temp++;
	 	                        }
		                    EEPROM.Set_Temp=HMI.set_temp;
	 	                    break;

	   case Event_Decrease_Temp :
		                    if( HMI.set_temp<=MIN_TEMP){
		  		                 HMI.set_temp=MIN_TEMP;
		  		            }
		  		            else{
		  	 	                HMI.set_temp--;
		  	 	                }
		                    EEPROM.Set_Temp=HMI.set_temp;
	 	 	                break;

	   case Event_Blower :
	 	 	                if(HMI.Blower_state==Blower_on){
	 	 		            HMI.Blower_state=Blower_off;

	 	 	                                       }
	 	 	                else{
	 	 		            HMI.Blower_state=Blower_on;
	 	 	                                       }
	 	 	 	            break;

	   case Event_Heater :
		 	 	            if(HMI.heater_state==Heater_on){

		 	 		        HMI.heater_state=Heater_off;

		 	 	                                    }
		 	 	            else{
		 	 		        HMI.heater_state=Heater_on;
		 	 	                                     }
		 	 	            EEPROM.heater_state=HMI.heater_state;
		 	 	 	        break;

	 case Event_User_Compressor :
			 	 	        if(HMI.user_compressor_state==Compressor_on){
			 	 		    HMI.user_compressor_state=Compressor_off;
			 	 	                                  }
			 	 	        else if(HMI.user_compressor_state==Compressor_off){
			 	 		    HMI.user_compressor_state=Compressor_on;
			 	 	                                  }
			 	 	        EEPROM.User_Compressor_state=HMI.user_compressor_state;

			 	 	        break;
	 case Event_Error :

		 //turn off compressor call error handler and do troubleshooting steps push error logs into eeprom !!
		 //push error flags into EEprom too
		   HMI.error_flag=error_flag_set;
		   EEPROM.Error_Present=HMI.error_flag;
		   EEPROM.ErrorCode=Current_Error;
			 	 	 	    break;
	 default:
		                    break ;

	}

}

 void Update_Output(HMI_t *HMI){
	if(HMI->mode==Auto_Mode && HMI->error_flag==error_flag_reset && HMI->status==AC_on){
		//auto mode
		//in auto mode condenser on and blower on and vent off

		//condenser on
		Relay_Cntrl(Condenser,Condenser_on);
		Led_Cntrl(Condenser,Condenser_on);


		//blower on
		Relay_Cntrl(Blower,Blower_on);
		Led_Cntrl(Blower,Blower_on);


		//Vent off
		Relay_Cntrl(Vent,Vent_off);
		Led_Cntrl(Vent,Vent_off);


		//heater on if temperature falls below 16 degree
		if(HMI->heater_state==Heater_off){
		Relay_Cntrl(Heater,Heater_off);
		Led_Cntrl(Heater,Heater_off);

		}
		else if(HMI->heater_state==Heater_on) {
		Relay_Cntrl(Heater,Heater_on);
		Led_Cntrl(Heater,Heater_on);

		}

		//compressor turns on after 3 mins
		if(HMI->compressor_state==Compressor_off ){

		Relay_Cntrl(Compressor,Compressor_off);
		Led_Cntrl(Compressor,Compressor_off);



		}
		else if(HMI->compressor_state==Compressor_on)
		{
		Relay_Cntrl(Compressor,Compressor_on);
		Led_Cntrl(Compressor,Compressor_on);

		Relay_Cntrl(Heater,Heater_off);
		Led_Cntrl(Heater,Heater_off);

		}


	}



	else if (HMI->mode==Manual_mode&&HMI->error_flag==error_flag_reset && HMI->status==AC_on){
		         //manual mode

		if(HMI->condenser_state==Condenser_on){
				 //condenser on
				Relay_Cntrl(Condenser,Condenser_on);
				Led_Cntrl(Condenser,Condenser_on);

				}
		else if(HMI->condenser_state==Condenser_off) {
			    //condenser off
				Relay_Cntrl(Condenser,Condenser_off);
				Led_Cntrl(Condenser,Condenser_off);
			    }

		if(HMI->Blower_state==Blower_on){
				//Blower on
			Relay_Cntrl(Blower,Blower_on);
			Led_Cntrl(Blower,Blower_on);

					}
		else if(HMI->Blower_state==Blower_off) {
				//Blower off
			Relay_Cntrl(Blower,Blower_off);
			Led_Cntrl(Blower,Blower_off);
				    }

         if(HMI->vent_state==Vent_on){
				//Vent on
				Relay_Cntrl(Vent,Vent_on);
				Led_Cntrl(Vent,Vent_on);

				}
         else if(HMI->vent_state==Vent_off){
        	   //Vent off
        	    Relay_Cntrl(Vent,Vent_off);
       			Led_Cntrl(Vent,Vent_off);
         }

				//heater on if temperature falls below 16 degree
				if(HMI->heater_state==Heater_on){
					//heater on
					Relay_Cntrl(Heater,Heater_on);
					Led_Cntrl(Heater,Heater_on);


				}
				else {
					//heater off
					Relay_Cntrl(Heater,Heater_off);
					Led_Cntrl(Heater,Heater_off);


				}

				//compressor turns on after 3 mins

			   if(HMI->compressor_state==Compressor_on )
				{
				Relay_Cntrl(Compressor,Compressor_on);
				Led_Cntrl(Compressor,Compressor_on);

				Relay_Cntrl(Heater,Heater_off);
				Led_Cntrl(Heater,Heater_off);

				}
			   else if(HMI->compressor_state==Compressor_off ){

			   	Relay_Cntrl(Compressor,Compressor_off);
			 	Led_Cntrl(Compressor,Compressor_off);
			   		 }
	}
	else{
		//either error has occured or machine state if off
		  //condenser off
		  Relay_Cntrl(Condenser,Condenser_off);
		  Led_Cntrl(Condenser,Condenser_off);
		//Blower off
      	  Relay_Cntrl(Blower,Blower_off);
		  Led_Cntrl(Blower,Blower_off);
	    //Vent off
		  Relay_Cntrl(Vent,Vent_off);
		  Led_Cntrl(Vent,Vent_off);
		//heater off
		  Relay_Cntrl(Heater,Heater_off);
		  Led_Cntrl(Heater,Heater_off);
		 //compressor off
		  Relay_Cntrl(Compressor,Compressor_off);
		  Led_Cntrl(Compressor,Compressor_off);
		  //solenoid valve off
		  Relay_Cntrl(Solenoid_Valve,Solenoid_Valve_off);


	}
}

 void Update_Compressor_State(HMI_t *HMI){

	 if (HMI->curr_temp >= (HMI->set_temp + 2))
	    {
	        if (HMI->compressor_state == Compressor_off ||HMI->compressor_state==Compressor_wait_to_off)
	        {
	        	Systick_Tick_Count = 0U;
	            HMI->compressor_state = Compressor_wait_to_on;
	        }
	    }
	    else if (HMI->curr_temp <= (HMI->set_temp - 2))
	    {
	    	if(HMI->compressor_state==Compressor_on||HMI->compressor_state==Compressor_wait_to_on){
	    		Systick_Tick_Count = 0U;
	    		HMI->compressor_state = Compressor_wait_to_off;

	    	}
	    }
	    else {
	    	//Do nothing previous state is hold in hysterisis region
	    }


}

 void Update_Display(HMI_t *HMI){
	const char* Display_Mode;


   if(HMI->mode!=Prev_Mode){
	if(HMI->mode==Auto_Mode){

	Display_Mode="AUTO VENT MODE";

	}
	else {

	Display_Mode="MANUAL VENT MODE";

	}

	Prev_Mode=HMI->mode;
	LCD_String_XY(0, 0, Display_Mode);

   }

   char Display_Temp[8];
   if(HMI->set_temp!=Prev_Set_Temp){

	snprintf(Display_Temp,sizeof(Display_Temp),"SET %d",(uint8_t)HMI->set_temp);
	LCD_String_XY(1, 0, Display_Temp);
	Prev_Set_Temp=HMI->set_temp;

   }
   if(HMI->curr_temp != Prev_Curr_Temp){
	snprintf(Display_Temp,sizeof(Display_Temp),"AIR %d",(uint8_t)HMI->curr_temp);
	LCD_String_XY(1, 8, Display_Temp);
	Prev_Curr_Temp=HMI->curr_temp;

   }


}

 void HMI_Init(HMI_t *HMI ){
	if(!EEPROM_Is_Valid(&EEPROM)){

    HMI->set_temp=25.00f;
    HMI->user_compressor_state=Compressor_on;
	HMI->status=AC_on;
	HMI->mode=Auto_Mode;
	HMI->compressor_state=Compressor_off;
	HMI->condenser_state=Condenser_off;
	HMI->curr_temp=25.00f;
    HMI->heater_state=Heater_off;
    HMI->vent_state=Vent_off;
    HMI->Blower_state=Blower_on;
    HMI->error_flag=error_flag_reset;
    }
	else{
	//these variables must be restored from eeprom !!
	EEPROM_Read(&First_EEPROM_Data);
    HMI->set_temp=First_EEPROM_Data.Set_Temp;
	HMI->status=First_EEPROM_Data.AC_State;
	HMI->mode=First_EEPROM_Data.Curr_Mode;
    HMI->condenser_state=First_EEPROM_Data.Condenser_state;
    HMI->user_compressor_state=First_EEPROM_Data.User_Compressor_state;
	HMI->curr_temp=First_EEPROM_Data.Curr_Temp;
    HMI->heater_state=First_EEPROM_Data.heater_state;
    HMI->vent_state=First_EEPROM_Data.vent_state;
    HMI->error_flag=First_EEPROM_Data.Error_Present;

	}


}

void Relay_Cntrl( Part_t part,bool Enable){
	switch(part){
	//R1-PTB1-solenoid valve
	//R2-PTB0-Reserve
	//R3-PTC9-Heater
	//R4-PTC8-Reserve
	//R5-PTA7-Blower
	//R6-PTA6-Condenser
	//R7-PTE7-Compressor

	case Condenser :
		if(Enable){
			PINS_DRV_SetPins(IP_PTA , 1U<<6);
		}
		else{
			PINS_DRV_ClearPins(IP_PTA , 1U<<6);
		}
	break ;

	case Compressor :
		if(Enable){
			PINS_DRV_SetPins(IP_PTE , 1U<<7);
		}
		else{
			PINS_DRV_ClearPins(IP_PTE , 1U<<7);
		}
	break ;

	case Blower :
		if(Enable){
			PINS_DRV_SetPins(IP_PTA , 1U<<6);
		}
		else{
			PINS_DRV_ClearPins(IP_PTA , 1U<<6);
		}
	break ;

	case Heater :
		if(Enable){
			PINS_DRV_SetPins(IP_PTC , 1U<<9);
		}
		else{
			PINS_DRV_ClearPins(IP_PTC , 1U<<9);
		}
	break ;

	case Solenoid_Valve :
		if(Enable){
			PINS_DRV_SetPins(IP_PTB , 1U<<9);
		}
		else{
			PINS_DRV_ClearPins(IP_PTB , 1U<<9);
		}
	break ;

	default :
	break;

	}
}


void Led_Cntrl( Part_t part,bool Enable){

	switch(part){

		case Condenser :
			if(Enable){
				PINS_DRV_ClearPins(IP_PTE , 1U<<3);
			}
			else{
				PINS_DRV_SetPins(IP_PTE , 1U<<3);
			}

		break ;

		case Compressor :
			if(Enable){
				PINS_DRV_ClearPins(IP_PTB , 1U<<6);
			}
			else{
				PINS_DRV_SetPins(IP_PTB , 1U<<6);
			}

	    break ;
		case Blower :
			if(Enable){
				PINS_DRV_ClearPins(IP_PTB , 1U<<7);
			}
			else{
				PINS_DRV_SetPins(IP_PTB , 1U<<7);
			}

		break ;

		case Heater :
			if(Enable){
				PINS_DRV_ClearPins(IP_PTD , 1U<<15);
			}
			else{
				PINS_DRV_SetPins(IP_PTD , 1U<<15);
			}
		break ;

		case Vent :
			if(Enable){
				PINS_DRV_ClearPins(IP_PTD , 1U<<16);
			}
			else{
				PINS_DRV_SetPins(IP_PTD , 1U<<16);
			}
		break ;


		default :
		break;


		}



}

void SysTick_Handler(void){
	Global_Tick_Count++;
	if(HMI.user_compressor_state==Compressor_on){
	if(HMI.compressor_state==Compressor_wait_to_on || HMI.compressor_state==Compressor_wait_to_off)
	{

		Systick_Tick_Count++;
	if(Systick_Tick_Count>=COMPRESSOR_DELAY_MS){

		if(HMI.compressor_state==Compressor_wait_to_on){
		HMI.compressor_state=Compressor_on;
	     }
		else if(HMI.compressor_state==Compressor_wait_to_off){
			HMI.compressor_state=Compressor_off;

	     }

		Systick_Tick_Count=0U;

	}
	}

	}

	else if(HMI.user_compressor_state==Compressor_off){
		Systick_Tick_Count++;
		if(Systick_Tick_Count>=COMPRESSOR_DELAY_MS){

				HMI.compressor_state=Compressor_off;
			Systick_Tick_Count=0U;

		}
		}



}
void Error_Handler( void  ){

}
