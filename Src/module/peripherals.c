/*
 * peripherals.c
 *
 *  Created on: Dec 29, 2016
 *      Author: jakub
 *  This file is contains the work with Input and Outputs of the board/micro.
 *  this should take care about switches, LEDs and rest.
 *
 */

#include "pinmap.h"
#include "peripherals.h"

Buttons checkButtons(void){

	if (!SW1)
	{
		return BUT_1;
	}
	if (!SW2)
	{
		return BUT_2;
	}
	if (!SW_ENC)
	{
		return BUT_ENC;
	}
	return BUT_NONE;
}

void togleLED(uint8_t led)
{
	switch (led)
	{
	case LED1:
	{
		Led1Tgl;
		break;
	}
	case LED2:
	{
		Led2Tgl;
		break;
	}
	}
}

/*
void toggleLED(const uint8_t led){

	switch (led)
	{
		case LED_RED:
			{
				HAL_GPIO_TogglePin();
				break;
			}
		case LED_YELLOW:
			{
				HAL_GPIO_TogglePin();
				break;
			}
		case LED_GREEN:
			{
				HAL_GPIO_TogglePin();
				break;
			}
	}
}
*/
