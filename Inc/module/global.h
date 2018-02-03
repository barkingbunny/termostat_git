/*
 * global.h
 *
 *  Created on: Dec 29, 2016
 *      Author: jakub
 */

#ifndef MODULE_GLOBAL_H_
#define MODULE_GLOBAL_H_

#include "stm32l0xx_hal.h"

typedef enum {FALSE = 0u, TRUE = 1u} Bool;
// the priority is selected by place in the list.
typedef enum {	MEASURING,
				HEATING,
				TEST,
				MENUn,
				TE_ERROR,
				SLEEP,
				TIME,
				IDLE,
				VOLTAGE,
				LOG
} States_loop;

typedef enum  	{
	menu,
	blind,
	desktop,
	debug,
	idle,
	temp_show
} Screen;

typedef struct {
	uint8_t new_data_to_show:1;
	uint8_t new_data:1;
	uint8_t measure_activate:1;
	uint8_t measure_running:1;
	uint8_t menu_activate:1;
	uint8_t menu_running:1;
	uint8_t temp_new_set:1;
	uint8_t temp_running:1;
	uint8_t enc_changed:1;
// regulation of the temperature
	uint8_t regulation_temp:1; // Signal when regulation of temp is enabled.
	uint8_t heating_up:1; // record that heating is UP
	uint8_t regulation_disabled:1;
	uint8_t log_enabled:1;
	uint8_t log_requsition:1;


}Flags_main;



#define BUT_DELAY 1		// in milisecond - I want to read it quckly
#define MAIN_LOOP 25		// in milisecond
#define MEASURE_PERIODE 5000 // every 5 secondn
#define LED_PERIODE 500 //
#define TIME_PERIODE 400 // ms/ definition of periode for checking time change (RTC change )
#define HEATING_PERIODE 1000 // every 5 minute check for change - turn on / off heater
#define LOG_PERIODE 3000 // every 5 minute check for change - turn on / off heater

#define HEATING_HYSTERESIS 100 // hysteresis is 1 deg C ( X/100)

extern Flags_main flags;


#endif /* MODULE_GLOBAL_H_ */