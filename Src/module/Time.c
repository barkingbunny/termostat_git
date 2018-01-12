/*
 * Time.c
 *
 *  Created on: Feb 5, 2017
 *      Author: jakub
 */

#include "tim.h"
#include "global.h"
#include "rtc.h"

/* return number after elapsing periode */
uint32_t fill_comparer (uint16_t periode){
	uint32_t x_compare;
	x_compare = HAL_GetTick()+periode;
	return (x_compare);
}
/*
uint32_t fill_comparer (uint16_t periode){
	uint32_t x_compare;
	x_compare = periode + hrtc->Instance->TR;

	return (x_compare);
}

*/
