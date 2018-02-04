/*
 * menu.c
 *
 *  Created on: Jan 23, 2017
 *      Author: jakub
 */

#include "menu.h"
#include "MenuDef.h"
#include "rtc_api.h" // for working with RTC

menu_item_t* ActualMenu;
uint32_t menu_compare;
int8_t position=0, position_x=0;
int8_t EN_count=-1;
uint32_t set_temperature=2000;
RTC_TimeTypeDef set_stimestructureget;
RTC_HandleTypeDef set_RtcHandle;


uint8_t activation_memu(){

	menu_compare = HAL_GetTick()+MENU_TIMOUT;
	position=0;
	position_x=0;
	EN_count=-1;
	ActualMenu=&MainMenu;
	flags.menu_activate=0;
	flags.menu_running=1;
	return 0;
}

uint8_t menu_action(){
	lcd_setCharPos(7,0);
	char buffer_s [32];
		snprintf(buffer_s, 12, "; enc%03i", (en_count));
		lcd_printString(buffer_s);

	if (flags.enc_changed) // move by encoder was detected - action on displai
				menu_compare = HAL_GetTick()+MENU_TIMOUT;	//enlarge time in menu

			switch (ActualMenu->action)
			{
			case (next):
						{
				if (flags.enc_changed){ // move by encoder was detected - action on dipley
					position = en_count;
					if ((position) < 0)
						en_count = 0;
					if ((position) > ActualMenu->numberOfChoices)
						en_count = ActualMenu->numberOfChoices;
					position = en_count;
					lcd_setCharPos((position+1),0);
					_putc('>');
					lcd_setCharPos((position+2),0);
					_putc(' ');
					lcd_setCharPos((position),0);
					_putc(' ');
					copy_to_lcd();
					flags.enc_changed = FALSE;
				}//if COUNTER!=0

				if (pushed_button == BUT_ENC){ // co se bude dit po stisknuti tlactika???
					if (position > (ActualMenu->numberOfChoices-1)){ // probably the end of the choices - back / exit
						if (ActualMenu->upmenu == NULL) // in main manu - exit
						{
							return 0; // exit the menu
						}
						else
							ActualMenu=ActualMenu->upmenu;
					}
					else{	// if is not chosen the exit or back.
						ActualMenu=ActualMenu->submenu[position];
						if ((ActualMenu->action == clock)){

							HAL_RTC_GetTime(&hrtc, &set_stimestructureget, RTC_FORMAT_BIN);
							HAL_RTC_SetTime(&set_RtcHandle, &set_stimestructureget, RTC_FORMAT_BIN);
							position_x=1;

#ifdef DEBUG
							lcd_setCharPos(5,8);
							char buffer_s [32];
							snprintf(buffer_s, 12, " COMPARISM");
							lcd_printString(buffer_s);
							break;
#endif //debug
						}
					}
					lcd_clear();
					en_count=0;
				}

				break;
				}

			case (clock):
					{
				if (pushed_button == BUT_ENC){
					position_x++;
				}
				if (position_x==1) {
					set_stimestructureget.Hours=set_stimestructureget.Hours+en_count;
					HAL_RTC_SetTime(&set_RtcHandle, &set_stimestructureget, RTC_FORMAT_BCD);
				}
				if (position_x==2){
					set_stimestructureget.Minutes=set_stimestructureget.Minutes+en_count;
					HAL_RTC_SetTime(&set_RtcHandle, &set_stimestructureget, RTC_FORMAT_BCD);
				}
				if (position_x==3){
					set_stimestructureget.Seconds=set_stimestructureget.Seconds+en_count;
					HAL_RTC_SetTime(&set_RtcHandle, &set_stimestructureget, RTC_FORMAT_BCD);
				}
				if (position_x==4){ // last click with encoder
					HAL_RTC_SetTime(&hrtc, &set_stimestructureget, RTC_FORMAT_BCD);
					position_x=0;
					flags.menu_running=0;
					lcd_clear();
					return 0; //exit menu
				}
				if (position_x>4)
					position_x=1;

#ifdef DEBUG
				en_count=0;
				return 1;
#endif
				break;
					}

			}


return 1; // correct function.
}

Bool menu_timout(void){
	if(HAL_GetTick() < menu_compare){
		return TRUE;
	}
	flags.menu_running=0;
	lcd_clear();
	return FALSE;
}

void display_menu(menu_item_t* display_menu) {
	char buffer_menu [32];
	uint8_t i=0;

	lcd_setCharPos(0,1);
	lcd_printString(display_menu->menuHeader);

	if (display_menu->numberOfChoices==0){ //set value
		lcd_setCharPos(2,2);
		switch (display_menu->action)
		{
		case (clock):
				{
			RTC_TimeShow_time(&set_stimestructureget,buffer_menu);
			//RTC_TimeShow(&set_RtcHandle,buffer_menu);
			lcd_setCharPos(1,10);
			lcd_printString(buffer_menu);
#ifdef DEBUG
		lcd_setCharPos(7,8);
		snprintf(buffer_menu, 12, "pozice %d", position_x);
		lcd_printString(buffer_menu);
#endif

			break;
				}
		case (date):
				{
			RTC_DateShow(&set_RtcHandle,buffer_menu);
			lcd_setCharPos(1,10);
			lcd_printString(buffer_menu);
			break;
				}
		case (setTemperature):
				{

			if (!flags.temp_new_set){
				{
					// writen of actual temp
					//				lcd_setCharPos(1,4);
					//				char_magnitude(2);
					//				snprintf(buffer_menu, 12, "%d.%02d C",temperature/100,temperature%100);
					//				lcd_printString(buffer_menu);

					lcd_setCharPos(3,0);
					char_magnitude(1);
					snprintf(buffer_menu, 21, "%Set temperature");
					lcd_printString(buffer_menu);

					set_temperature = temperature_set+en_count*50;

					lcd_setCharPos(5,4);
					char_magnitude(2);
					snprintf(buffer_menu, 12, "%2d.%02d C",set_temperature/100,set_temperature%100);
					lcd_printString(buffer_menu);
					char_magnitude(1);
#ifdef DEBUG
	// for debuf purpouses, delete after debuging ended

					lcd_setCharPos(7,6);
					snprintf(buffer_menu, 12, "  enc%03i", (en_count));
					lcd_printString(buffer_menu);
					// just for debug reason
#endif
				}
			}
			if (pushed_button == BUT_ENC)
			{ // BUTTONE PUSHED
				temperature_set = set_temperature;
				//			show = desktop;
				flags.temp_new_set = TRUE;
				flags.heating_up = TRUE; // if the buttone was pushed, turn-on the heater, even if the temperature is reached.

				// end of the Menu
				flags.menu_running=0;
				lcd_clear();
			} // end of BUTTONE PUSHED
			break;
				}
		default:
		{

			break;
		}
		}

	}
	else{ // write a list of menu choices

		for (i=0;i<display_menu->numberOfChoices;i++)
		{
			lcd_setCharPos((i+1),2);
			lcd_printString(display_menu->submenu[i]->menuHeader);
		}
		lcd_setCharPos(display_menu->numberOfChoices+1,2);
		if (display_menu->menuHeader==MainMenu.menuHeader)
			lcd_printString("EXIT");
		else
			lcd_printString("Back");
	}
}
