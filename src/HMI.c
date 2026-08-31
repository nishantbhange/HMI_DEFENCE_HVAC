/*
 * HMI.c
 *
 *  Created on: Jul 1, 2026
 *      Author: Nishant Bhange
 */
#include"HMI.h"
#include"EEPROM.h"
#include<stdbool.h>
#include<ADC.h>
#include<string.h>
#include"GPIO.h"
 volatile HMI_t HMI;
 volatile uint32_t Global_Tick_Count =0 ;
 static uint32_t Systick_Tick_Count=0;
//It gates both the 5-second condenser-before-compressor stagger (COMPRESSOR_STAGGER_DELAY_MS)
 //and the 10-second solenoid post-start hold (TICK_COUNT_10SEC) off the same counter


 static uint32_t Systick_Tick_Count_Stagger=0;
 static uint32_t Systick_Tick_Count_User_State=0;
 static uint8_t Prev_Mode=(Mode_t)0xFF;
 static uint8_t Prev_Set_Temp=0xFF;
 static ErrorCode_t Prev_Display_Error_Code = 0xFF;
 static uint8_t Prev_Curr_Temp=0xFF;
 EEPROM_Data_t First_EEPROM_Data;
 volatile HMI_Event_t Event;
 volatile ErrorCode_t Current_Error;
 static Compressor_t Prev_Compressor_State;
 int comp_ct_val=0;
 volatile bool Check_Status_Flag =RESET;
 static bool Heater_First_Boot =RESET;
 static bool OC_Compressor_Tripped =RESET;
 volatile bool Manual_Mode_Flag=RESET;
 volatile uint32_t Manual_Mode_Count=0;
 static volatile uint32_t Compressor_Overcurrent_Time_ms=0;
 static void Error_Display_Handler(void);
 static volatile  uint32_t Systick_Tick_Count_ADC_Error =0;

 static bool Forced_Flag=SET;

 static void update_state_ADC_Error(void );
 static void update_state_HPSW_Error(void );
 static void update_state_LPSW_Error(void );
 static void update_state_OC_Error(void );

 static void LPSW_Error_Handler(bool Error_Set_Reset );
 static void HPSW_Error_Handler(bool Error_Set_Reset );
 static void ADC_Error_Handler(bool Error_Set_Reset );
 static void OC_Error_Handler(bool Error_Set_Reset );


 typedef enum
 {
     DISPLAY_NORMAL,
     DISPLAY_ERROR,
     DISPLAY_OFF
 } DisplayState_t;

 static DisplayState_t Prev_Display_State;



 void HMI_Process_Event(HMI_Event_t CurrEvent ){
	 if(CurrEvent==Event_NONE){
		 return ;
	 }
	switch(CurrEvent){

	    case Event_Machine_status :

	    	                    if(HMI.status==AC_on){
	    							HMI.status=AC_off;
	    							Backlight_Cntrl(OFF);

	    							//turn off the ac .
	    							}
	    						else if(HMI.status==AC_off){
	    							HMI.status=AC_on;
	    						    HMI.mode=Auto_Mode;
	    						    HMI.compressor_state=Compressor_off;
	    							Backlight_Cntrl(ON);

	    							//turn on the ac .

	    						}

	                            EEPROM.AC_State=HMI.status;
	                            EEPROM.Curr_Mode=HMI.mode;
	                        break;

	     case Event_Mode :
	    	                        if(HMI.mode==Auto_Mode){
	    		                           HMI.mode=Manual_mode;
	    		                           HMI.Blower_state=Blower_on;
	    		                           HMI.user_compressor_state=Compressor_off ;
	    		                           HMI.user_Heater_state=Heater_off;

	    	                                               }
	    	                        else{
	    		                           HMI.mode=Auto_Mode;
	    		                           HMI.compressor_state=Compressor_off;
	    		                           Manual_Mode_Flag=RESET;
	    		                           Manual_Mode_Count=0;
	    	                                               }
	    	                         Systick_Tick_Count_Stagger=0;
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
		                    Forced_Flag=RESET;
	 	 	                if(HMI.Blower_state==Blower_on){
	 	 		            HMI.Blower_state=Blower_off;

	 	 	                                       }
	 	 	                else{
	 	 		            HMI.Blower_state=Blower_on;
	 	 	                                       }
	 	 	 	            break;

	   case Event_User_Heater :
		                    Forced_Flag=RESET;
		 	 	            if(HMI.user_Heater_state==Heater_on){

		 	 		        HMI.user_Heater_state=Heater_off;

		 	 	                                    }
		 	 	            else{
		 	 		        HMI.user_Heater_state=Heater_on;
		 	 		        HMI.user_compressor_state=Compressor_off;
		 	 	                                     }
		 	 	            EEPROM.heater_state=HMI.heater_state;
		 	 	 	        break;

	 case Event_User_Compressor :
		                    Systick_Tick_Count_User_State=0U;
		                    Forced_Flag=RESET;
			 	 	        if(HMI.user_compressor_state==Compressor_on){
			 	 		    HMI.user_compressor_state=Compressor_off;
			 	 	                                  }
			 	 	        else{
			 	 		    HMI.user_compressor_state=Compressor_on;
			 	 		    HMI.user_Heater_state=Heater_off;
			 	 		    Systick_Tick_Count_Stagger=0U;
			 	 	                                  }
			 	 	        EEPROM.User_Compressor_state=HMI.user_compressor_state;

			 	 	        break;
	 case Event_Error :

		                   //turn off compressor call error handler and do troubleshooting steps push error logs into eeprom !!
		                   //push error flags into EEprom too
		                   HMI.error_flag=error_flag_set;

		                   EEPROM.Error_Present=HMI.error_flag;
		                   EEPROM.ErrorCode=Current_Error;
		                   EEPROM.Active_Errors=HMI.Active_Errors;
		                   for(uint8_t i=0 ; i< ERORR_COUNT ;i++){
		                	   EEPROM.Display_Error_Code[i]=HMI.Display_Error_Code[i];
		                   }


			 	 	 	    break;
	 case Event_Error_Clear :
		                  if(HMI.Active_Errors==0){
		                  HMI.error_flag=error_flag_reset;
		                  Prev_Display_Error_Code = 0xFF;
		                  EEPROM.ErrorCode=-1;
		                  }
	                      EEPROM.Error_Present=HMI.error_flag;

	                      EEPROM.Active_Errors=HMI.Active_Errors;
	                      for(uint8_t i=0 ; i< ERORR_COUNT ;i++){
	                     		EEPROM.Display_Error_Code[i]=HMI.Display_Error_Code[i];
	                     		                   }


		                    break;
	 default:
		                    break ;

	}

}

 void Update_Output(HMI_t *HMI){
	if(HMI->mode==Auto_Mode && HMI->error_flag==error_flag_reset && HMI->status==AC_on){
		//auto mode
		//in auto mode condenser on and blower on and vent off
		Relay_Cntrl(Blower,Blower_on);
		Led_Cntrl(Blower,Blower_on);

		//Relay_Cntrl(Vent,Vent_on);
		Led_Cntrl(Vent,Vent_on);

		//Heater and cooling(compressor/solenoid/condenser) are mutually exclusive
		if(HMI->heater_state==Heater_on){

			Relay_Cntrl(Heater,Heater_on);
			Led_Cntrl(Heater,Heater_on);

			Relay_Cntrl(Compressor,Compressor_off);
			Led_Cntrl(Compressor,Compressor_off);
			Relay_Cntrl(Condenser,Condenser_off);
			Led_Cntrl(Condenser,Condenser_off);
			Relay_Cntrl(Solenoid_Valve,Solenoid_Valve_off);

		}
		else{
					Relay_Cntrl(Heater,Heater_off);
					Led_Cntrl(Heater,Heater_off);
		}
					//Condenser fan follows the compressor
					//stays energised through on , only drops with the compressor itself
			        //Compressor relay is only energised once fully ON - the wait_to_on/wait_to_off
					//states are the 3-min short-cycle-protection buffers timed in SysTick_Handler

					if(HMI->compressor_state==Compressor_on){
						Relay_Cntrl(Condenser,Condenser_on);
						Led_Cntrl(Condenser,Condenser_on);

						if(Systick_Tick_Count_Stagger>=COMPRESSOR_STAGGER_DELAY_MS){
							Relay_Cntrl(Compressor,Compressor_on);
							Led_Cntrl(Compressor,Compressor_on);
						}
						else{
							Relay_Cntrl(Compressor,Compressor_off);
							Led_Cntrl(Compressor,Compressor_off);
						}
					}
					else{
						Relay_Cntrl(Condenser,Condenser_off);
						Led_Cntrl(Condenser,Condenser_off);
						Relay_Cntrl(Compressor,Compressor_off);
						Led_Cntrl(Compressor,Compressor_off);
					}




					//Solenoid valve
					// - ON through the 3-min pre-start buffer, to build pressure ahead of compressor start
					// - stays ON for 10s after the compressor actually switches ON
					// - ON again once temp is within 0.5C of T_SET while still cooling (about to shut off)
					// - ON through the 3-min pre-stop buffer
					// - OFF the instant the compressor is fully OFF
					switch(HMI->compressor_state){

						case Compressor_wait_to_on:
							Relay_Cntrl(Solenoid_Valve,Solenoid_Valve_on);
							break;

						case Compressor_on:
							if(Systick_Tick_Count_Stagger<TICK_COUNT_10SEC){
								Relay_Cntrl(Solenoid_Valve,Solenoid_Valve_on);
							}
							else{
								Relay_Cntrl(Solenoid_Valve,Solenoid_Valve_off);
							}
							break;

						case Compressor_wait_to_off:
							Relay_Cntrl(Solenoid_Valve,Solenoid_Valve_off);
							break;

						case Compressor_off:
						default:
							Relay_Cntrl(Solenoid_Valve,Solenoid_Valve_off);
							break;
					}





	}



	else if (HMI->mode==Manual_mode&&HMI->error_flag==error_flag_reset && HMI->status==AC_on){
		         //manual mode- every part is user-controlled, no hysteresis
	             //manual mode is just for testing/maintaince purpose automatic temp control algo is disabled here !!
                 //manual mode has a timer of 3 mins user can do whaterver he/she wants to do in 3 mins after
		         // 3 mins has completed control will exit through manual mode
                 // no seperate control for solenoid valve
		if(Manual_Mode_Count<TICK_COUNT_3MINS){

			if(Manual_Mode_Flag==RESET){
					       Manual_Mode_Flag=SET;
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
			//Vent on
			           // Relay_Cntrl(Vent,Vent_on);
						Led_Cntrl(Vent,Vent_on);

						//heater : user-controlled
			if(HMI->user_Heater_state==Heater_on){
				//heater on

				        Relay_Cntrl(Compressor,Compressor_off);
				        Led_Cntrl(Compressor,Compressor_off);
				        Relay_Cntrl(Solenoid_Valve,Solenoid_Valve_off);
						Relay_Cntrl(Heater,Heater_on);
						Led_Cntrl(Heater,Heater_on);
						Relay_Cntrl(Condenser,Condenser_off);
						Led_Cntrl(Condenser,Condenser_off);
						Relay_Cntrl(Blower,Blower_on);
						Led_Cntrl(Blower,Blower_on);
						HMI->user_Heater_state=Heater_on;
						HMI->user_compressor_state=Compressor_off;
						HMI->Blower_state=Blower_on;

				}
			else {
				//heater off

						Relay_Cntrl(Heater,Heater_off);
						Led_Cntrl(Heater,Heater_off);
						HMI->user_Heater_state=Heater_off;


				}

			//Compressor : user-controlled - must read user_compressor_state (the manual toggle
			//driven by Event_User_Compressor), NOT the auto-hysteresis compressor_state
			if(HMI->user_compressor_state==Compressor_on)
							{
				        Relay_Cntrl(Heater,Heater_off);
						Led_Cntrl(Heater,Heater_off);

						 //condenser on immediately
						Relay_Cntrl(Condenser,Condenser_on);
						Led_Cntrl(Condenser,Condenser_on);

						//compressor staggered behind it
						if(Systick_Tick_Count_Stagger>=COMPRESSOR_STAGGER_DELAY_MS){
							Relay_Cntrl(Compressor,Compressor_on);
							Led_Cntrl(Compressor,Compressor_on);
						}
						else{
							Relay_Cntrl(Compressor,Compressor_off);
							Led_Cntrl(Compressor,Compressor_off);
						}

						Relay_Cntrl(Solenoid_Valve,Solenoid_Valve_off);
						Relay_Cntrl(Blower,Blower_on);
						Led_Cntrl(Blower,Blower_on);
						HMI->user_Heater_state=Heater_off;
						HMI->user_compressor_state=Compressor_on;
						HMI->Blower_state=Blower_on;


				}
			else{

						Relay_Cntrl(Compressor,Compressor_off);
						Led_Cntrl(Compressor,Compressor_off);
						Relay_Cntrl(Solenoid_Valve,Solenoid_Valve_off);
						//condenser off
						Relay_Cntrl(Condenser,Condenser_off);
						Led_Cntrl(Condenser,Condenser_off);
						HMI->user_compressor_state=Compressor_off;


			}

	       }
		else{
			 Manual_Mode_Count=0;
			 Manual_Mode_Flag=RESET;
			 HMI->mode=Auto_Mode;
			 //LCD_Clear();

			}
		// Update_Display(*HMI);
		}

	else{
		//either error has occured or machine state if off
		if(HMI->status==AC_off){
			//condenser off
					  Relay_Cntrl(Condenser,Condenser_off);
					  Led_Cntrl(Condenser,Condenser_off);
					//Blower on
			      	  Relay_Cntrl(Blower,Blower_off);
					  Led_Cntrl(Blower,Blower_off);
				    //Vent off
					  //Relay_Cntrl(Vent,Vent_on);
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

else{
             //error condition
             //ADC error -compressor on for 7 mins compressor off 3 mins do it in cycles
			if((HMI->Active_Errors>>ADC_Active_Error_Bit)&0x01){
				//everything off except blower
				update_state_ADC_Error();

			}
			//LPSW error -only blower on
			else if((HMI->Active_Errors>>LPSW_Active_Error_Bit)&0x01){
				//everything off except blower
				update_state_LPSW_Error();

			}
			//HPSW error -only blower on
			else if((HMI->Active_Errors>>HPSW_Active_Error_Bit)&0x01){
				//everything off except blower
				update_state_HPSW_Error();

			}
			//compressor overcurrent error-only blower on
			else if((HMI->Active_Errors>>OC_Active_Error_Bit)&0x01){
				//everything off except blower
				update_state_OC_Error();

						}

		}


	}

}

 void Update_Compressor_State(HMI_t *HMI){
	 if(HMI->error_flag == error_flag_set)
	 {
	     return;
	 }
	 if(HMI->mode==Auto_Mode){
	 if (HMI->curr_temp >= (HMI->set_temp + 2))
	    {
	        if (HMI->compressor_state == Compressor_off ||HMI->compressor_state==Compressor_wait_to_off)
	        {
	        	Systick_Tick_Count = 0U;
	            HMI->compressor_state = Compressor_wait_to_on;
	        }
	    }
	    else if (HMI->curr_temp <= HMI->set_temp)
	    {
	    	if(HMI->compressor_state==Compressor_on||HMI->compressor_state==Compressor_wait_to_on){
	    		Systick_Tick_Count = 0U;
	    		HMI->compressor_state = Compressor_off;

	    	}
	    }
	    else {
	    	    	//Do nothing previous state is hold in hysterisis region
	    	 }

	 }

}
 void Update_Heater_State(HMI_t *HMI){

	 if (HMI->mode != Auto_Mode || HMI->error_flag == error_flag_set  )
	        return;

	   /* First automatic heater activation after power-up */
	    if (Heater_First_Boot == RESET)
	    {
	        if (HMI->curr_temp <= Heater_Cut_On_Temp)
	        {
	            Heater_First_Boot = SET;

	        }
	        else
	        return;

	    }
	    else
     {
		        //heater ON once temp drops to/under the fixed low-temp floor (16C), with a 0.5C
		        //hysteresis buffer so it doesn't chatter right at the threshold
		 		if(HMI->curr_temp<=(HMI->set_temp-HEATER_HYSTERESIS)){
		 			HMI->heater_state=Heater_on;
		 			}
		 		//heater OFF once temp climbs back within 0.5C of T_SET , or hits the
		 		//absolute safety ceiling (24C) - whichever is reached first
		 		else if(HMI->curr_temp>=HMI->set_temp){
		 					HMI->heater_state=Heater_off;
		 					//Heater_First_Boot=RESET;
		 					}

		 		else {
		 		//Do nothing previous state is hold in hysterisis region
		 			    	 }
     }

 }

 void Update_Display(HMI_t HMI){
 	 DisplayState_t Current_State;
 	 static uint32_t Prev_Manual_Remaining_Sec = 0xFFFFFFFFU;

 	 // determine the state
 	     if(HMI.error_flag!=error_flag_set&&HMI.status == AC_on)
 	     {
 	         Current_State = DISPLAY_NORMAL;
 	     }
 	     else if(HMI.status == AC_off)
 	     {
 	         Current_State = DISPLAY_OFF;
 	     }
 	     else
 	     {
 	         Current_State = DISPLAY_ERROR;
 	     }

 	     if(Current_State != Prev_Display_State)
 	     {
 	         LCD_Clear();
 	         Prev_Display_State = Current_State;
 	         Prev_Mode      = -1;
 	         Prev_Set_Temp  = -1;
 	         Prev_Curr_Temp = -1;
 	         Prev_Manual_Remaining_Sec = 0xFFFFFFFFU;
 	         Prev_Display_Error_Code = 0xFF;
 	     }

    if(HMI.error_flag!=error_flag_set&&HMI.status==AC_on){
 		 Current_State=DISPLAY_NORMAL;
 	const char* Display_Mode;

    if(HMI.mode!=Prev_Mode ||(HMI.mode==Manual_mode&&Forced_Flag==RESET) ){
    if(HMI.mode!=Prev_Mode )LCD_Clear();
 	if(HMI.mode==Auto_Mode){
 	Display_Mode="   AUTO MODE    ";
 	}
 	else {
 		if(HMI.user_compressor_state==Compressor_on)
 	        Display_Mode=" FORCED COOLING ";
 		else if(HMI.user_Heater_state==Heater_on)
 			Display_Mode=" FORCED HEATING ";
 		else
 			Display_Mode=" FORCED BLOWER  ";

 		Forced_Flag=SET;
 	}

 	Prev_Mode=HMI.mode;
 	LCD_String_XY(0, 0, Display_Mode);

 	//line 1 layout differs between the two modes - force it to redraw on
 	//the mode-change edge so stale characters from the other layout don't linger
 	Prev_Set_Temp = -1;
 	Prev_Curr_Temp = -1;
 	Prev_Manual_Remaining_Sec = 0xFFFFFFFFU;
    }

    char Display_Buf[17];

    if(HMI.mode==Auto_Mode){

 	   if((uint8_t)HMI.set_temp!=Prev_Set_Temp){
 		snprintf(Display_Buf,sizeof(Display_Buf),"SET %d",(uint8_t)HMI.set_temp);
 		LCD_String_XY(1, 0, Display_Buf);
 		Prev_Set_Temp=HMI.set_temp;
 	   }
 	   if((uint8_t)HMI.curr_temp != Prev_Curr_Temp){
 		snprintf(Display_Buf,sizeof(Display_Buf),"AIR %-3d",(int8_t)HMI.curr_temp);
 		LCD_String_XY(1, 8, Display_Buf);
 		Prev_Curr_Temp=HMI.curr_temp;
 	   }
    }
    else { // Manual_mode
 	   uint32_t remaining_ms = (Manual_Mode_Count < TICK_COUNT_3MINS)
 	                             ? (TICK_COUNT_3MINS - Manual_Mode_Count) : 0U;
 	   uint16_t remaining_sec = remaining_ms / 1000U;

 	   if(remaining_sec != Prev_Manual_Remaining_Sec){
 		snprintf(Display_Buf,sizeof(Display_Buf),"%03d",
 		         (uint8_t)(remaining_sec));
 		LCD_String_XY(1, 2, Display_Buf);
 		Prev_Manual_Remaining_Sec=remaining_sec;
 	   }
 	   if(HMI.curr_temp != Prev_Curr_Temp){
 		snprintf(Display_Buf,sizeof(Display_Buf),"AIR %-3d",(int8_t)HMI.curr_temp);
 		LCD_String_XY(1, 8, Display_Buf);
 		Prev_Curr_Temp=HMI.curr_temp;
 	   }
    }
 	 }
 	 else if(HMI.status==AC_off){
 		 Current_State = DISPLAY_OFF;
 	 }
 	 else{
 		 Current_State = DISPLAY_ERROR;
 		 Error_Display_Handler();
          }
 }
 void HMI_Init(HMI_t *HMI ){
	Backlight_Cntrl(ON);
	 // pull whatever is currently in FlexRAM
	EEPROM_Read(&First_EEPROM_Data);
	if(!EEPROM_Is_Valid(&First_EEPROM_Data)){
     // first-ever boot  - set defaults

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
    HMI->Compressor_Error_State=Compressor_on;
    HMI->user_Heater_state=Heater_off;

    // push defaults into the RAM shadow

    EEPROM.Set_Temp = HMI->set_temp;
    EEPROM.AC_State = HMI->status;
    EEPROM.Curr_Mode = HMI->mode;
    EEPROM.Condenser_state = HMI->condenser_state;
    EEPROM.User_Compressor_state = HMI->user_compressor_state;
    EEPROM.Curr_Temp = HMI->curr_temp;
    EEPROM.heater_state = HMI->heater_state;
    EEPROM.vent_state = HMI->vent_state;
    EEPROM.Error_Present = HMI->error_flag;
    EEPROM.Active_Errors = HMI->Active_Errors;
    EEPROM.Magic_No = MAGIC_NO;
    EEPROM_Write(&EEPROM);
    }
	else{
	//these variables must be restored from eeprom !!

    HMI->set_temp=First_EEPROM_Data.Set_Temp;
	HMI->status=First_EEPROM_Data.AC_State;
	HMI->mode=First_EEPROM_Data.Curr_Mode;
    HMI->condenser_state=First_EEPROM_Data.Condenser_state;
    HMI->user_compressor_state=Compressor_off ;
	HMI->curr_temp=First_EEPROM_Data.Curr_Temp;
    HMI->heater_state=Heater_off;
    HMI->vent_state=First_EEPROM_Data.vent_state;
    HMI->error_flag=First_EEPROM_Data.Error_Present;
    HMI->Blower_state=Blower_on;
    HMI->Compressor_Error_State=Compressor_on;
    HMI->user_Heater_state=Heater_off;
    HMI->Active_Errors=First_EEPROM_Data.Active_Errors;
    for(uint8_t i=0;i<ERORR_COUNT;i++){
    	HMI->Display_Error_Code[i]=First_EEPROM_Data.Display_Error_Code[i];
    }
    HMI->compressor_state=Compressor_off;


    EEPROM = First_EEPROM_Data;

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
			PINS_DRV_SetPins(IP_PTA , 1U<<7);
		}
		else{
			PINS_DRV_ClearPins(IP_PTA , 1U<<7);
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
			PINS_DRV_SetPins(IP_PTB , 1U<<1);
		}
		else{
			PINS_DRV_ClearPins(IP_PTB , 1U<<1);
		}
	break ;

	default :
	break;

	}
}


void Led_Cntrl( Part_t part,bool Enable){
//PTB7 -BLOWER
//PTB6 -compressor
//PTE3-condenser
//PTD16-vent
//PTD15-heater
	switch(part){

		case Condenser :
			if(Enable){
				PINS_DRV_SetPins(IP_PTE , 1U<<3);

			}
			else{
				PINS_DRV_ClearPins(IP_PTE , 1U<<3);
			}

		break ;

		case Compressor :
			if(Enable){
				PINS_DRV_SetPins(IP_PTB , 1U<<6);

			}
			else{
				PINS_DRV_ClearPins(IP_PTB , 1U<<6);
			}

	    break ;
		case Blower :
			if(Enable){
				PINS_DRV_SetPins(IP_PTB , 1U<<7);

			}
			else{
				PINS_DRV_ClearPins(IP_PTB , 1U<<7);
			}

		break ;

		case Heater :
			if(Enable){
				PINS_DRV_SetPins(IP_PTD , 1U<<15);

			}
			else{
				PINS_DRV_ClearPins(IP_PTD , 1U<<15);
			}
		break ;

		case Vent :
			if(Enable){
				PINS_DRV_SetPins(IP_PTD , 1U<<16);

			}
			else{
				PINS_DRV_ClearPins(IP_PTD , 1U<<16);
			}
		break ;


		default :
		break;


		}



}

void SysTick_Handler(void){
	Global_Tick_Count++;
	if(HMI.error_flag==error_flag_reset){
	if(HMI.mode==Auto_Mode){
	if(HMI.compressor_state==Compressor_wait_to_on || HMI.compressor_state==Compressor_wait_to_off)
	{

		Systick_Tick_Count++;
	if(Systick_Tick_Count>=COMPRESSOR_DELAY_MS){

		if(HMI.compressor_state==Compressor_wait_to_on){
		HMI.compressor_state=Compressor_on;
		/* compressor has just switched ON - start the solenoid's 10s post-start hold (red note, row 13) */
		Systick_Tick_Count_Stagger=0U;

	     }
		else if(HMI.compressor_state==Compressor_wait_to_off){
			HMI.compressor_state=Compressor_off;

	     }

		Systick_Tick_Count=0U;

	}
	}
	else if(HMI.compressor_state==Compressor_on){
			/* keep counting while running so Update_Output can tell whether we're still
			 * inside the 10s post-start solenoid hold window  */
			if(Systick_Tick_Count_Stagger<TICK_COUNT_10SEC){
				Systick_Tick_Count_Stagger++;
			}
		}

	}
	else{
		//manual mode
		//manual mode
				if(HMI.user_compressor_state==Compressor_on){
					if(Systick_Tick_Count_Stagger<TICK_COUNT_10SEC){
						Systick_Tick_Count_Stagger++;
					}
				}


	}

	}

	//either error condition or just increasing tick count based on flags set .
	if((HMI.Active_Errors>>ADC_Active_Error_Bit)&0x01){

		Systick_Tick_Count_ADC_Error++;

	}
	if(Check_Status_Flag){
			Compressor_Overcurrent_Time_ms++;
		}

	if(CCIF_TIMEOUT_FLAG){
		CCIF_TIMEOUT_COUNT++;
	}
	if(EEERDY_TIMEOUT_FLAG){
		EEERDY_TIMEOUT_COUNT++;
		}
	if(LPIT_Timeout_Flag){
		LPIT_Timeout_Count++;
		}
	if(ADC_Init_Timeout_Flag){
		ADC_Init_Timeout_Count++;
		}
	if(ADC_Bsy_Timeout_Flag){
		ADC_Bsy_Timeout_Count++;
		}
	if(ADC_Timeout_Flag){
		ADC_Timeout_Count++;
			}
	if(EEPROM_Timeout_Flag){
		EEPROM_Timeout_Count++;
				}
	if(EEPROM_Write_Timeout_Flag){
		EEPROM_Write_Timeout_Count++;
				}
	if(Manual_Mode_Flag){
		Manual_Mode_Count++;
					}

	if((Global_Tick_Count-Press_Start_Tick)>=LONG_PRESS_MS && Long_Press_Flag==SET ){
			           //long press detected !! change current mode .
			           Event=Event_Mode;
			           Long_Press_Flag=RESET;

			                  }

}

void Error_Handler( void  ){
if(Event==Event_Error){
	switch(Current_Error){

	case Error_Event_LPSW:
		LPSW_Error_Handler(Error_Set);
		break ;
	case Error_Event_HPSW:
		HPSW_Error_Handler(Error_Set);
		break ;
	case Error_Event_ADC :
		ADC_Error_Handler(Error_Set);
       break ;
	case Error_Event_OC :
		OC_Error_Handler(Error_Set);
	       break ;

      default:

    	  break ;
	}

}
else if(Event==Event_Error_Clear){
	switch(Current_Error){

	case Error_LPSW_Clear:
		LPSW_Error_Handler(Error_Reset);
		break ;
	case Error_HPSW_Clear:
		HPSW_Error_Handler(Error_Reset);
		break ;
	case Error_ADC_Clear :
		ADC_Error_Handler(Error_Reset);
		break ;
	case Error_OC_Clear :
		OC_Error_Handler(Error_Reset);
		break ;

	 default:
	    break ;

	}

}

}
void LPSW_Error_Handler(bool Error_Set_Reset ){
if(Error_Set_Reset==Error_Set){
	Prev_Compressor_State=HMI.compressor_state;
	HMI.error_flag = error_flag_set;
    HMI.Display_Error_Code[LPSW_ERROR_INDEX]=Error_Event_LPSW;
    HMI.Active_Errors&=~(1<<LPSW_Active_Error_Bit);
    HMI.Active_Errors|=(1<<LPSW_Active_Error_Bit);
    HMI.Active_Errors&=(Active_Error_Mask);
}
else if(Error_Set_Reset==Error_Reset){
	HMI.Active_Errors&=~(1<<LPSW_Active_Error_Bit);
	HMI.Active_Errors&=(Active_Error_Mask);
	if(HMI.Active_Errors==0){
	HMI.error_flag = error_flag_reset;

     if(Prev_Compressor_State==Compressor_on){
		HMI.compressor_state=Compressor_wait_to_on;
	}
	else{
		HMI.compressor_state=Prev_Compressor_State;
	}
	}
}
//Update_Display(HMI);

}
void HPSW_Error_Handler(bool Error_Set_Reset){
if(Error_Set_Reset==Error_Set){
	Prev_Compressor_State=HMI.compressor_state;
	HMI.error_flag = error_flag_set;
	HMI.Display_Error_Code[HPSW_ERROR_INDEX]=Error_Event_HPSW;
	HMI.Active_Errors&=~(1<<HPSW_Active_Error_Bit);
	HMI.Active_Errors|=(1<<HPSW_Active_Error_Bit);
	HMI.Active_Errors&=(Active_Error_Mask);
}
else if(Error_Set_Reset==Error_Reset){
	HMI.Active_Errors&=~(1<<HPSW_Active_Error_Bit);
	HMI.Active_Errors&=(Active_Error_Mask);
	if(HMI.Active_Errors==0){
	HMI.error_flag = error_flag_reset;
	if(Prev_Compressor_State==Compressor_on){
	HMI.compressor_state=Compressor_wait_to_on;
	}
	else{
	HMI.compressor_state=Prev_Compressor_State;
	}
	}
}
//Update_Display(HMI);
}


void ADC_Error_Handler(bool Error_Set_Reset ){


	if(Error_Set_Reset==Error_Set){
		Prev_Compressor_State=HMI.compressor_state;
		HMI.error_flag = error_flag_set;
		HMI.Display_Error_Code[ADC_ERROR_INDEX]=Error_Event_ADC;
		HMI.Active_Errors&=~(1<<ADC_Active_Error_Bit);
		HMI.Active_Errors|=(1<<ADC_Active_Error_Bit);
		HMI.Active_Errors&=(Active_Error_Mask);
		if(HMI.Compressor_Error_State != Compressor_off  ){
		    HMI.Compressor_Error_State = Compressor_off;
		}

	}
	else if(Error_Set_Reset==Error_Reset){
		HMI.Active_Errors&=~(1<<ADC_Active_Error_Bit);
		HMI.Active_Errors&=(Active_Error_Mask);
		Systick_Tick_Count_ADC_Error=0;
	if(HMI.Active_Errors==0){
		HMI.error_flag = error_flag_reset;

			if(Prev_Compressor_State==Compressor_on){
			HMI.compressor_state=Compressor_wait_to_on;
			}
			else{
			HMI.compressor_state=Prev_Compressor_State;
			}
	}
	}
	//Update_Display(HMI);

}

void OC_Error_Handler(bool Error_Set_Reset ){
	//Compressor_Overcurrent_Time_ms=0;
	if(Error_Set_Reset==Error_Set){
		bool already_active = (HMI.Active_Errors & (1<<OC_Active_Error_Bit)) != 0;
		if(!already_active){
		Prev_Compressor_State=HMI.compressor_state;
		if(HMI.mode==Auto_Mode){
		HMI.Compressor_Error_State=HMI.compressor_state;
		}
		else{
		HMI.Compressor_Error_State=HMI.user_compressor_state;
		}
		Compressor_Overcurrent_Time_ms=0;
		Check_Status_Flag=SET;

		}
		HMI.error_flag = error_flag_set;
		HMI.Display_Error_Code[OC_ERROR_INDEX]=Error_Event_OC;
		HMI.Active_Errors&=~(1<<OC_Active_Error_Bit);
		HMI.Active_Errors|=(1<<OC_Active_Error_Bit);
		HMI.Active_Errors&=(Active_Error_Mask);
	}
	else if(Error_Set_Reset==Error_Reset){
		HMI.Active_Errors&=~(1<<OC_Active_Error_Bit);
		HMI.Active_Errors&=(Active_Error_Mask);
		Compressor_Overcurrent_Time_ms=0;
		Check_Status_Flag=RESET;
	if(HMI.Active_Errors==0){
		HMI.error_flag = error_flag_reset;
		 if(OC_Compressor_Tripped){
		// it really did run the full 1 min and got shut off - force a FULL fresh delay
		   HMI.compressor_state=Compressor_wait_to_on;  // Systick_Tick_Count already zeroed at the trip itself
		   OC_Compressor_Tripped=false;
		            }
		 else{
		 // cleared before 1 min - relay was never touched, so just resume exactly as it was
		   HMI.compressor_state=Prev_Compressor_State;
		            }

	}
	//HMI.Compressor_Error_State=Compressor_on;
	}
	//Update_Display(HMI);

}

void Error_Display_Handler(void)
{
    static uint8_t error_index = 0;
    static uint32_t start_tick = 0;



    static const ErrorCode_t error_code[] =
    {
        Error_Event_LPSW,
        Error_Event_HPSW,
        Error_Event_ADC,
		Error_Event_OC
    };

    uint8_t checked = 0;

    // Find an active error
    while (checked < 4U)
    {
        if (HMI.Active_Errors & (1U << error_index))
        {
            break;
        }

        error_index++;

        if (error_index >= 4U)
        {
            error_index = 0;
        }

        checked++;
    }

    // No active errors
    if (checked == 4U)
    {
        return;
    }

    // Change displayed error every 2 seconds
    if ((Global_Tick_Count - start_tick) >= 2000U)
    {
        start_tick = Global_Tick_Count;

        error_index++;

        if (error_index >= 4U)
        {
            error_index = 0;
        }

    // Find next active error
        checked = 0;

        while (checked < 4U)
        {
            if (HMI.Active_Errors & (1U << error_index))
            {

                break;
            }

            error_index++;

            if (error_index >= 4U)
            {
                error_index = 0;
            }

            checked++;
        }
    }

    if (error_code[error_index] != Prev_Display_Error_Code)
    {
        switch (error_code[error_index])
        {
            case Error_Event_LPSW:
                LCD_Clear();
                LCD_String_XY(0, 5, "ERROR");
                LCD_String_XY(1, 3, "LPSW_ERROR");
                break;

            case Error_Event_HPSW:
                LCD_Clear();
                LCD_String_XY(0, 5, "ERROR");
                LCD_String_XY(1, 3, "HPSW_ERROR");
                break;

            case Error_Event_ADC:
                LCD_Clear();
                LCD_String_XY(0, 5, "ERROR");
                LCD_String_XY(1, 3, "ADC_ERROR");
                break;

            case Error_Event_OC:
                LCD_Clear();
                LCD_String_XY(0, 5, "ERROR");
                LCD_String_XY(1, 4, "OC_ERROR");
                break;

            default:
                break;
        }

        Prev_Display_Error_Code = error_code[error_index];
    }
}

static void update_state_ADC_Error(void ){
	if(HMI.Blower_state!=Blower_on){
					     HMI.Blower_state=Blower_on;
						 Relay_Cntrl(Blower,Blower_on);
						 Led_Cntrl(Blower,Blower_on);
					}


				     //Vent off
					if(HMI.vent_state!=Vent_on){
						HMI.vent_state=Vent_on;
						//Relay_Cntrl(Vent,Vent_off);
						Led_Cntrl(Vent,Vent_on);
									}
	                 //heater off
					if(HMI.heater_state!=Heater_off || HMI.user_Heater_state!=Heater_off){
						HMI.heater_state=Heater_off;
						HMI.user_Heater_state=Heater_off;
						Relay_Cntrl(Heater,Heater_off);
						Led_Cntrl(Heater,Heater_off);
									}
					if((HMI.Active_Errors & ~(1U << ADC_ERROR_INDEX)) == 0U){

	               if(Systick_Tick_Count_ADC_Error>=TICK_COUNT_7MINS&&HMI.Compressor_Error_State==Compressor_on ){
	            	   HMI.Compressor_Error_State=Compressor_off;
	            	   Relay_Cntrl(Compressor,Compressor_off);
	            	   Relay_Cntrl(Solenoid_Valve,Solenoid_Valve_off);
	            	   Led_Cntrl(Compressor,Compressor_off);

	            	   HMI.condenser_state=Condenser_off;
	            	   Relay_Cntrl(Condenser,Condenser_off);
	            	   Led_Cntrl(Condenser,Condenser_off);
	            	 	Systick_Tick_Count_ADC_Error=0;
	             }
	               else if(Systick_Tick_Count_ADC_Error>=TICK_COUNT_3MINS&&HMI.Compressor_Error_State==Compressor_off){


	            	HMI.Compressor_Error_State=Compressor_on;
	            	Relay_Cntrl(Compressor,Compressor_on);
	            	Relay_Cntrl(Solenoid_Valve,Solenoid_Valve_off);
	            	Led_Cntrl(Compressor,Compressor_on);
	            	HMI.condenser_state=Condenser_on;
	            	Relay_Cntrl(Condenser,Condenser_on);
	            	Led_Cntrl(Condenser,Condenser_on);
	            	Systick_Tick_Count_ADC_Error=0;
	             }
	               else{
	            	   if(HMI.Compressor_Error_State==Compressor_off){
	            	   Relay_Cntrl(Compressor,Compressor_off);
	            	   HMI.Compressor_Error_State=Compressor_off;
	            	   HMI.condenser_state=Condenser_off;
	            	  Relay_Cntrl(Condenser,Condenser_off);
	            	  Led_Cntrl(Condenser,Condenser_off);
	            	   }

	             }
					}
					else{
						 HMI.Compressor_Error_State=Compressor_off;
						 Relay_Cntrl(Compressor,Compressor_off);
						 HMI.condenser_state=Condenser_off;
						 Relay_Cntrl(Condenser,Condenser_off);
						 Led_Cntrl(Condenser,Condenser_off);
						 Relay_Cntrl(Solenoid_Valve,Solenoid_Valve_off);
						 Led_Cntrl(Compressor,Compressor_off);

					}

}

static void update_state_HPSW_Error(void ){
	if(HMI.Blower_state!=Blower_on){
					HMI.Blower_state=Blower_on;
					Relay_Cntrl(Blower,Blower_on);
					Led_Cntrl(Blower,Blower_on);
   											}

					 //Vent off
	if(HMI.vent_state!=Vent_off){
					HMI.vent_state=Vent_off;
					//Relay_Cntrl(Vent,Vent_off);
					Led_Cntrl(Vent,Vent_off);
													}
					//heater off
	if(HMI.heater_state!=Heater_off || HMI.user_Heater_state!=Heater_off){
					HMI.heater_state=Heater_off;
					HMI.user_Heater_state=Heater_off;
					Relay_Cntrl(Heater,Heater_off);
					Led_Cntrl(Heater,Heater_off);
													}
					//compressor off
	if(HMI.Compressor_Error_State!=Compressor_off || HMI.user_compressor_state!=Compressor_off){
					HMI.Compressor_Error_State=Compressor_off;
					HMI.user_compressor_state=Compressor_off;
					Relay_Cntrl(Compressor,Compressor_off);
					Relay_Cntrl(Solenoid_Valve,Solenoid_Valve_off);
					Led_Cntrl(Compressor,Compressor_off);
					HMI.condenser_state=Condenser_off;
					Relay_Cntrl(Condenser,Condenser_off);
					Led_Cntrl(Condenser,Condenser_off);

	}
	 //solenoid valve off
						Relay_Cntrl(Solenoid_Valve,Solenoid_Valve_off);
						Led_Cntrl(Solenoid_Valve,Solenoid_Valve_off);


}

static void update_state_LPSW_Error(void ){
	if(HMI.Blower_state!=Blower_on){
				    HMI.Blower_state=Blower_on;
				    Relay_Cntrl(Blower,Blower_on);
				    Led_Cntrl(Blower,Blower_on);
									}


					//Vent off
	if(HMI.vent_state!=Vent_off){
					HMI.vent_state=Vent_off;
					//Relay_Cntrl(Vent,Vent_off);
					Led_Cntrl(Vent,Vent_off);
													}
					 //heater off
	if(HMI.heater_state!=Heater_off || HMI.user_Heater_state!=Heater_off){
					HMI.heater_state=Heater_off;
					HMI.user_Heater_state=Heater_off;
					Relay_Cntrl(Heater,Heater_off);
					Led_Cntrl(Heater,Heater_off);
													}
					//compressor off
	if(HMI.Compressor_Error_State!=Compressor_off || HMI.user_compressor_state!=Compressor_off){
					HMI.Compressor_Error_State=Compressor_off;
					HMI.user_compressor_state=Compressor_off;
					Relay_Cntrl(Compressor,Compressor_off);
					Relay_Cntrl(Solenoid_Valve,Solenoid_Valve_off);
					Led_Cntrl(Compressor,Compressor_off);
					HMI.condenser_state=Condenser_off;
					Relay_Cntrl(Condenser,Condenser_off);
					Led_Cntrl(Condenser,Condenser_off);
						                          }
	                //solenoid valve off
					Relay_Cntrl(Solenoid_Valve,Solenoid_Valve_off);
					Led_Cntrl(Solenoid_Valve,Solenoid_Valve_off);

}

static void update_state_OC_Error(void ){
	                    if(HMI.Blower_state!=Blower_on){
						     HMI.Blower_state=Blower_on;
							 Relay_Cntrl(Blower,Blower_on);
							 Led_Cntrl(Blower,Blower_on);
						}

					     //Vent off
						if(HMI.vent_state!=Vent_on){
							HMI.vent_state=Vent_on;
							//Relay_Cntrl(Vent,Vent_off);
							Led_Cntrl(Vent,Vent_on);
										}
		                 //heater off
						if(HMI.heater_state!=Heater_off || HMI.user_Heater_state!=Heater_off){
							HMI.heater_state=Heater_off;
							HMI.user_Heater_state=Heater_off;
							Relay_Cntrl(Heater,Heater_off);
							Led_Cntrl(Heater,Heater_off);
										}

	    if(Compressor_Overcurrent_Time_ms>=TICK_COUNT_1MIN &&( HMI.Compressor_Error_State==Compressor_on )){
	    	 Compressor_Overcurrent_Time_ms=0;
	         HMI.Compressor_Error_State=Compressor_off;

	         OC_Compressor_Tripped=true;
	         Systick_Tick_Count=0U;
	         Relay_Cntrl(Compressor,Compressor_off);
	         Relay_Cntrl(Solenoid_Valve,Solenoid_Valve_off);
	         Led_Cntrl(Compressor,Compressor_off);
	         //condenser off
	         Relay_Cntrl(Condenser,Condenser_off);
	         Led_Cntrl(Condenser,Condenser_off);
			 Check_Status_Flag=RESET;
		}
	    //solenoid valve off
	   		Relay_Cntrl(Solenoid_Valve,Solenoid_Valve_off);
	   		Led_Cntrl(Solenoid_Valve,Solenoid_Valve_off);

}
void Backlight_Cntrl(bool On_Off){
if(On_Off){
	PINS_DRV_SetPins(IP_PTE,1U<<PIN_BLC);
}
else{
	PINS_DRV_ClearPins(IP_PTE,1U<<PIN_BLC);
}
}

