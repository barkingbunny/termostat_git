/*
 * log.c
 *
 *  Created on: Dec 7, 2017
 *      Author: jakub
 */
/*
 * This file shall contains functions for logging the data from inputs
 */

#include "log.h"
#include "stdio.h"
#include "string.h"


uint16_t index_log_wr = 0;
uint16_t index_log_read = 0xfffe;
RTC_HandleTypeDef received_time;

uint8_t Log_Data(RTC_HandleTypeDef* RtcHandle, int16_t temperature, int16_t humidity, int16_t pressure, uint16_t diagnostics)
{
	char TimeMark[25] = {0};
	char buffer_data[50] = {0};
	RTC_TimeMark(RtcHandle, TimeMark);
	snprintf(buffer_data, 45, "%s;%d;%d;%d;%d;",TimeMark,temperature,humidity, pressure, diagnostics);

	return 1;
}

/* for logging of extra events */
uint8_t Log_Extra(RTC_HandleTypeDef* RtcHandle)
{
	char TimeMark[25] = {0};
	RTC_TimeMark(RtcHandle, TimeMark);


	return 1;
}

void RTC_TimeMark(RTC_HandleTypeDef* RtcHandle, char* showtime)
{
  RTC_DateTypeDef rtc_date_structure;
  RTC_TimeTypeDef rtc_time_structure;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(RtcHandle, &rtc_time_structure, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(RtcHandle, &rtc_date_structure, RTC_FORMAT_BIN);
  /* Display time Format : hh:mm:ss */
  sprintf((char*)showtime,"20%2d-%02d-%02d-%02d:%02d:%02d",rtc_date_structure.Year, rtc_date_structure.Month, rtc_date_structure.Date, rtc_time_structure.Hours, rtc_time_structure.Minutes, rtc_time_structure.Seconds);
}

uint8_t Log_Temperature(RTC_HandleTypeDef* RtcHandle, int32_t temperature, int32_t humidity)
{
	humidity = humidity/1024;
	int16_t humidity_16 = (int16_t)humidity;
	int16_t temperature_16 = temperature;

	log_data[index_log_wr].temp_1= temperature_16;
	log_data[index_log_wr].hum_1 = humidity_16;
	log_data[index_log_wr].time_tr= RtcHandle->Instance->TR;
	log_data[index_log_wr].date_cr= RtcHandle->Instance->CR;

	index_log_wr++;
	if (index_log_wr >= LOG_DATA_LENGTH)
		index_log_wr = 0;

return 1;
}

/*This function is for reading the logged data from memory. Separately - each call is one record
 * return statements:
 * 1 - finished, all (non zero) data were read
 * 2 - there are still data, that were not read.
 *
 */
uint8_t Log_Read(log_item_t* log_Handle){
	if (0xfffe == index_log_read) index_log_read=index_log_wr;  // first time, when this function is used.
uint32_t valid_data_cr = (log_data[index_log_read].date_cr);
	while (0 == valid_data_cr){
		log_Handle = &log_data[index_log_read];
		index_log_read++;
		if (index_log_read >= LOG_DATA_LENGTH)
			index_log_read = 0;
		valid_data_cr = (log_data[index_log_read].date_cr);
	}

	if (index_log_read != index_log_wr)
		return 2;

	index_log_read = 0xfffe;
	return 1;
}
uint8_t Log_To_String(char* field_of_char, uint8_t field_lenght){
	log_item_t* log_Handle;
	uint8_t log_read_stat= 0;
	log_read_stat=Log_Read(log_Handle);

		char TimeMark[25] = {0};
		RTC_HandleTypeDef* RtcHandle;
		RtcHandle->Instance->TR = log_Handle->time_tr;
		RtcHandle->Instance->CR = log_Handle->date_cr;
		RTC_TimeMark(RtcHandle, TimeMark);

		snprintf(field_of_char, 32, "%s;%d;%d;",TimeMark,log_Handle->temp_1, log_Handle->hum_1);

	if (2 == log_read_stat) // if there are more data to read, return 2;
		return 2;
// if all memory was read, return 1;
		return 1;
	}
