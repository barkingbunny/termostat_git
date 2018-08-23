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

void RTC_TimeMark_Log_Struct(log_item_t* log_handle, char* showtime)
{

  /* Display time Format : yy:mm:dd:hh:mm */
  sprintf((char*)showtime,"20%2d-%02d-%02d-%02d:%02d",log_handle->year, log_handle->month, log_handle->day, log_handle->hour , log_handle->minute);
}


uint8_t Log_Temperature(RTC_HandleTypeDef* RtcHandle, int32_t temperature, int32_t humidity)
{
	humidity = humidity/1024;
	int16_t humidity_16 = (int16_t)humidity;
	int16_t temperature_16 = temperature;

	log_data[index_log_wr].temp_1= temperature_16;
	log_data[index_log_wr].hum_1 = humidity_16;


	RTC_TimeTypeDef rtc_time_structure;
	/* Get the RTC current Time */
	HAL_RTC_GetTime(RtcHandle, &rtc_time_structure, RTC_FORMAT_BIN);
	RTC_DateTypeDef rtc_date_structure;
	/* Get the RTC current Date */
	HAL_RTC_GetDate(RtcHandle, &rtc_date_structure, RTC_FORMAT_BIN);

	log_data[index_log_wr].year = rtc_date_structure.Year;
	log_data[index_log_wr].month = rtc_date_structure.Month;
	log_data[index_log_wr].day = rtc_date_structure.Date;
	log_data[index_log_wr].hour = rtc_time_structure.Hours;
	log_data[index_log_wr].minute = rtc_time_structure.Minutes;

	index_log_wr++;
	if (index_log_wr >= LOG_DATA_LENGTH)
		index_log_wr = 0;

//DEBUG
char buffer_s [32];
lcd_setCharPos(7,0);
snprintf(buffer_s, 7, "Log=%i;", index_log_wr);
lcd_printString(buffer_s);
lcd_setCharPos(4,0);
snprintf(buffer_s, 7, "%d;",log_data[index_log_wr-1].hum_1);
lcd_printString(buffer_s);
lcd_setCharPos(5,0);
snprintf(buffer_s, 7, "%d;",log_data[index_log_wr-1].temp_1);
lcd_printString(buffer_s);

//end DEBUG


return index_log_wr;
}

/*This function is for reading the logged data from memory. Separately - each call is one record
 * return statements:
 * 1 - finished, all (non zero) data were read
 * 2 - there are still data, that were not read.
 *
 */
uint8_t Log_Read(log_item_t* log_Handle){
	if (0xfffe == index_log_read) index_log_read=index_log_wr;  // first time, when this function is used.

char buffer_s [32];
lcd_setCharPos(0,0);
snprintf(buffer_s, 12, "rd=%i;wr%i  ", index_log_read, index_log_wr);
lcd_printString(buffer_s);

	while (0 == log_data[index_log_read].day){
lcd_printString("R");
		index_log_read++;
		if (index_log_read >= LOG_DATA_LENGTH)
			index_log_read = 0;
	}

	//memcpy(log_Handle, log_data[index_log_read], arraysize * sizeof (struct log_item_t));
	log_Handle->temp_1 = log_data[index_log_read-1].temp_1;
	log_Handle->hum_1 = log_data[index_log_read-1].hum_1;
	log_Handle->year = log_data[index_log_read-1].year;
	log_Handle->month = log_data[index_log_read-1].month;
	log_Handle->day = log_data[index_log_read-1].day;
	log_Handle->hour = log_data[index_log_read-1].hour;
	log_Handle->minute = log_data[index_log_read-1].minute;

	if (index_log_read != index_log_wr)
		return 2;

	index_log_read = 0xfffe;
	return 1;
}

uint8_t Log_To_String(char* field_of_char, uint8_t field_lenght){
	log_item_t log_Handle;
	uint8_t log_read_stat= 0;
	char* TimeMark[25];

	log_read_stat=Log_Read(&log_Handle);
	RTC_TimeMark_Log_Struct(&log_Handle, TimeMark);

	snprintf((char*)field_of_char, 32, "%s;%d;%d;",TimeMark,log_Handle.temp_1, log_Handle.hum_1);

	if (2 == log_read_stat) // if there are more data to read, return 2;
		return 2;
	// if all memory was read, return 1;
	return 1;
}
