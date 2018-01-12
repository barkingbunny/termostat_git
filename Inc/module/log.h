/*
 * log.h
 *
 *  Created on: Dec 7, 2017
 *      Author: jakub
 */

#ifndef MODULE_LOG_H_
#define MODULE_LOG_H_

#include "pinmap.h"

uint8_t Log_Data(RTC_HandleTypeDef* RtcHandle, int16_t temperature, int16_t humidity, int16_t pressure, uint16_t diagnostics);
void RTC_TimeMark(RTC_HandleTypeDef* RtcHandle, char* showtime);


#endif /* MODULE_LOG_H_ */
