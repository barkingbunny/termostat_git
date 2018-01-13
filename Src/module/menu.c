/*
 * menu.c
 *
 *  Created on: Jan 23, 2017
 *      Author: jakub
 */

#include "menu.h"
#include "MenuDef.h"

menu_item_t* ActualMenu;
uint32_t menu_compare;
int8_t position=0;
int8_t EN_count=-1;
uint32_t set_temperature=2000;

uint8_t activation_memu(){

	menu_compare = HAL_GetTick()+MENU_TIMOUT;
	position=0;
	EN_count=-1;
	ActualMenu=&MainMenu;
	flags.menu_activate=0;
	flags.menu_running=1;
	return 0;
}

uint8_t menu_action(){


		if (EN_count!=0){
			menu_compare = HAL_GetTick()+MENU_TIMOUT;	//enlarge time in menu
			position = EN_count+position;
			if ((position) < 0)
				position = 0;
			if ((position) > ActualMenu->numberOfChoices)
				position = ActualMenu->numberOfChoices;
			htim22.Instance->CNT= 0;
char buffer_menu[20];
			lcd_setCharPos(7,0);
			snprintf(buffer_menu, 15, "EN%03i, pos %i", (EN_count),position);
			lcd_printString(buffer_menu);

			lcd_setCharPos((position+1),0);
			_putc('>');
			lcd_setCharPos((position+2),0);
			_putc(' ');
			lcd_setCharPos((position),0);
			_putc(' ');
			copy_to_lcd();
		}//if COUNTER!=0

	EN_count = htim22.Instance->CNT;

	if (pushed_button == BUT_ENC){ // coc se bude dit po stisknuti tlactika???
		if (ActualMenu->numberOfChoices>0)
			ActualMenu=ActualMenu->submenu[position];
		lcd_clear();
		en_count=0;
	}


	return 0;
}

Bool menu_timout(void){
	if(HAL_GetTick() < menu_compare){

		return TRUE;
	}
	flags.menu_running=0;
	lcd_clear();
	return FALSE;
}
void display_menu(menu_item_t *display_menu){
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
			RTC_TimeShow(&hrtc,buffer_menu);
			lcd_setCharPos(1,10);
			lcd_printString(buffer_menu);

			break;
			}
		case (date):
			{
			RTC_DateShow(&hrtc,buffer_menu);
			lcd_setCharPos(1,10);
			lcd_printString(buffer_menu);
			break;
			}
		case (setTemperature):
			{

			if (!flags.temp_new_set){

			{
					// writen of actual temp
					lcd_setCharPos(1,4);
					char_magnitude(2);
					snprintf(buffer_menu, 12, "%d.%02d C",temperature/100,temperature%100);
					lcd_printString(buffer_menu);

					lcd_setCharPos(3,0);
					char_magnitude(1);
					snprintf(buffer_menu, 21, "%Set temperature");
					lcd_printString(buffer_menu);

					set_temperature = temperature_set+en_count*50;

					lcd_setCharPos(5,4);
					char_magnitude(2);
					snprintf(buffer_menu, 12, "%d.%02d C",set_temperature/100,set_temperature%100);
					lcd_printString(buffer_menu);
					char_magnitude(1);

					// for debuf purpouses, delete after debuging ended

					lcd_setCharPos(7,6);
					snprintf(buffer_menu, 12, "  enc%03i", (en_count));
					lcd_printString(buffer_menu);

					// just for debug reason
				}
			}
			if (pushed_button == BUT_ENC)
			{
				temperature_set = set_temperature;
	//			show = desktop;
				flags.temp_new_set = TRUE;
				flags.heating_up = TRUE; // if the buttone was pushed, turn-on the heater, even if the temperature is reached.

				// end of the Menu
				flags.menu_running=0;
				lcd_clear();
			}
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
