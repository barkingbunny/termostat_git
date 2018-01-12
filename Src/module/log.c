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
