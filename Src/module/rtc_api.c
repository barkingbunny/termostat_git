/*
 * rtc_api.c
 *
 *  Created on: Jan 5, 2017
 *      Author: jakub
 */

#include "rtc_api.h"

void RTC_TimeShow(RTC_HandleTypeDef* RtcHandle, char* showtime)
{
  RTC_TimeTypeDef stimestructureget;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
  /* Display time Format : hh:mm:ss */
  sprintf((char*)showtime,"%02d:%02d:%02d",stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
}

void RTC_DateShow(RTC_HandleTypeDef* RtcHandle, char* showtime)
{
	RTC_DateTypeDef sdatestructureget;
	/* Get the RTC current Date */
	HAL_RTC_GetDate(RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
	/* Display time Format : hh:mm:ss */
	  sprintf((char*)showtime,"%02d/%02d/20%2d",sdatestructureget.Date, sdatestructureget.Month, sdatestructureget.Year);

}

void RTC_TimeDateShow(RTC_HandleTypeDef* RtcHandle, char* showtime)
{
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
  /* Display time Format : hh:mm:ss */
  sprintf((char*)showtime,"%02d/%02d/20%2d %02d:%02d:%02d",sdatestructureget.Date, sdatestructureget.Month, sdatestructureget.Year, stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
}

uint8_t RTC_setTime(RTC_HandleTypeDef* RtcHandle, uint16_t newTime){
	// time is declarated in minutes (with hours)


	return 0;
}
