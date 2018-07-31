/*
 * log.h
 *
 *  Created on: Dec 7, 2017
 *      Author: jakub
 */

#ifndef MODULE_LOG_H_
#define MODULE_LOG_H_

#include "pinmap.h"
#include "global.h"


typedef struct {
	uint32_t time_tr;
	uint32_t date_cr;
	int16_t temp_1;
	int16_t hum_1;
}log_item_t;

uint8_t Log_Data(RTC_HandleTypeDef* RtcHandle, int16_t temperature, int16_t humidity, int16_t pressure, uint16_t diagnostics);
void RTC_TimeMark(RTC_HandleTypeDef* RtcHandle, char* showtime);
uint8_t Log_Temperature(RTC_HandleTypeDef* RtcHandle, int32_t temperature, int32_t humidity);
uint8_t Log_Read(log_item_t* log_Handle);
uint8_t Log_To_String(char* field_of_char, uint8_t field_lenght);


log_item_t log_data[LOG_DATA_LENGTH];

#endif /* MODULE_LOG_H_ */
