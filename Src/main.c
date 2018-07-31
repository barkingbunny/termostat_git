/**
 ******************************************************************************
 * File Name          : main.c
 * Description        : Main program body
 ******************************************************************************
 * This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * Copyright (c) 2018 STMicroelectronics International N.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l0xx_hal.h"
#include "adc.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */

/**popis
 *
 *Pracuji tedkon na logovani
 *
 * Ukoly:
 * pridej MENU, s nastavenim casu a teplot. funkcni
 * pridej logovaci funkci.
 *
 * USB*
 * nastveni - 115200-8-N-1, /dev/ttyACM0
 */

#include "pinmap.h"
#include "peripherals.h"
#include "lcd_12864.h"
#include "BME280.h"
#include "ds18b20.h"
#include <stdio.h>
#include "usbd_cdc_if.h"
#include "rtc_api.h"
#include "Time.h"
#include "menu.h"
#include "log.h"
#include "sleep.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
static States_loop current_state;
static Screen show;
Flags_main flags;
uint32_t citac=0;
int8_t en_count=0;
int32_t temperature=-20000;
int32_t humid = -5;
uint32_t presure = 0;

// Temp_seting and driving
int32_t temperature_set=2000;
// END Temp_seting and driving

Buttons pushed_button; //cleared each main cycle

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	char buffer_s [32];
	char buffer_usb [32];

	uint16_t T_led=20;


	uint32_t actual_HALtick=0;
	uint32_t InputVoltage=0;
	uint8_t en_count_last=0;
	int16_t count1;

	char aShowTime[50] = {0};

	//debug
	Bool beta_part=0, show_time=TRUE;
	//debug

	//timeouts
	uint32_t backlite_compare, measure_compare, led_compare,time_compare, button_compare, heating_compare,logging_compare, show_timeout, heating_instant_timeout;

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_SPI1_Init();
	MX_TIM22_Init();
	MX_TIM21_Init();
	MX_I2C1_Init();
	MX_USB_DEVICE_Init();
	MX_RTC_Init();
	MX_ADC_Init();

	/* USER CODE BEGIN 2 */
	lcd12864_init(&hspi1);
	line(0,60,110,60,1);
	lcd_setCharPos(0,0);
	lcd_printString("Initialization unit\r");
	lcd_printString("termostat_git\r");
	lcd_printString( "SW v 0.220");
	HAL_TIM_Encoder_Start(&htim22,TIM_CHANNEL_1);

	htim22.Instance->EGR = 1;           // Generate an update event
	htim22.Instance->CR1 = 1;           // Enable the counter

	BME280_init(&hi2c1,DEFAULT_SLAVE_ADDRESS); // initialization of temp/humid sensor BOSH

	sprintf(buffer_usb, "Hello World, ahha123456");
	CDC_Transmit_FS(buffer_usb,25);

	HAL_Delay(1700);
	lcd_clear();

	//settings variables to default values
	flags.heating_up = FALSE;

	current_state = MEASURING;
	show=desktop;
	flags.log_enabled = TRUE; // logging of the actions
	//init timers
	led_compare = fill_comparer(LED_PERIODE);
	measure_compare = fill_comparer(MEASURE_PERIODE);
	time_compare = fill_comparer(TIME_PERIODE);
	button_compare = fill_comparer(BUT_DELAY);
	heating_compare = fill_comparer(10); // check it immediately
	logging_compare = fill_comparer_seconds(LOG_PERIODE); // check it immediately
	show_timeout = 0xfffffffe;
	heating_instant_timeout = 0;

	HAL_GPIO_WritePin(D_LCD_LIGHT_GPIO_Port,D_LCD_LIGHT_Pin,GPIO_PIN_SET);
	backlite_compare = fill_comparer(BACKLITE_TIMEOUT);
	// NEW ADDED
	/*##- 4- Start the conversion process #######################################*/
	if (HAL_ADC_Start(&hadc) != HAL_OK)
	{
		/* Start Conversation Error */
		Error_Handler();
	}

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	for (;;)
	{
		switch (current_state){
		case SLEEP:
		{//			neni potreba spanku
			Go_To_Sleep(&hrtc);
			current_state = MEASURING;
			show_time=TRUE;
			break;
		}
		case IDLE:
		{
			break;
		}
		case MEASURING:
		{

			temperature=BME280_getTemperature();
			humid=BME280_getHumidity();
		//	presure=BME280_getPressure();

			current_state = VOLTAGE;

			flags.new_data_to_show=TRUE;

			measure_compare = fill_comparer(MEASURE_PERIODE);
			break;
		}
		case VOLTAGE:

		{
			/*##- 5- Wait for the end of conversion #####################################*/
			/*  Before starting a new conversion, you need to check the current state of
  			         the peripheral; if it�s busy you need to wait for the end of current
  			         conversion before starting a new one.
  			         For simplicity reasons, this example is just waiting till the end of the
  			         conversion, but application may perform other tasks while conversion
  			         operation is ongoing. */
			HAL_ADC_PollForConversion(&hadc, 10);

			/* Check if the continous conversion of regular channel is finished */
			if ((HAL_ADC_GetState(&hadc) & HAL_ADC_STATE_REG_EOC) == HAL_ADC_STATE_REG_EOC)
			{
				/*##-6- Get the converted value of regular channel  ########################*/
				InputVoltage = HAL_ADC_GetValue(&hadc);
			}

			flags.new_data_to_show=TRUE;
			current_state = IDLE;

			break;
		}
		/* there is a process, of heating with the heater.
		 *The heater is turned on by OUT1, when the temperature is below temperature_set-Hysteresis
		 */
		case HEATING:
		{
			if (flags.heating_instant){
				if(heating_instant_timeout > actual_HALtick)
					turnOnHeater(temperature);
				else
					flags.heating_instant=FALSE;
			}
			else{
				if (flags.regulation_temp & (temperature < temperature_set))
				{
					if (flags.heating_up) // if the heater is turned on, keep continue with heating.
					{
						turnOnHeater(temperature);
					}
					else
					{	// when the heater is not active, wait until the temperature fall below threshold-Hysteresis
						if (temperature < (temperature_set-HEATING_HYSTERESIS))
						{
							flags.heating_up = TRUE;
							turnOnHeater(temperature);
						}
					}
				}// end-if (flags.regulation_temp&(temperature < temperature_set))

				else // vypni topeni
				{


					turnOffHeater();
					flags.heating_up = FALSE;

				}
			}
			heating_compare = fill_comparer(HEATING_PERIODE);
			current_state = IDLE;
			break;
		}

		case LOG:
		{
			if (flags.log_enabled)
			{
				Log_Temperature(&hrtc, temperature, humid);
				logging_compare = fill_comparer_seconds(LOG_PERIODE);
			}

			break;
		}
		default:
		{
			break;
		}

		}// switch CURRENT STATE


		/* **** SCREEN **** */
		switch (show){

		case blind:
		{
			lcd_clear();
			show = idle;
			break;
		}
		case desktop:
		{// showing main screen - temperatures and Hum
			if (flags.new_data_to_show==TRUE){
				// BME280 sensor
				lcd_setCharPos(1,3);
				char_magnitude(2);
				snprintf(buffer_s, 12, "%3ld.%02d C",temperature/100,abs(temperature%100));
				lcd_printString(buffer_s);

				lcd_setCharPos(4,3);
				snprintf(buffer_s, 14, "%3ld.%02ld %%",(humid / 1024), humid%1024*100/1024);
				lcd_printString(buffer_s);

				// Marking - heating is active/not active
				lcd_setCharPos(1,19);
				if(!flags.regulation_temp)
					lcd_printString("-");
				else _putc(0x07f);

				lcd_setCharPos(4,19);
				if(!flags.heating_instant)
					lcd_printString(" ");
				else _putc(0x07f);

				// END Marking - heating is active/not active

				char_magnitude(1);

				lcd_setCharPos(3,2);
				snprintf(buffer_s, 12, "set %3ld.%02d C",temperature_set/100,abs(temperature_set%100));
				lcd_printString(buffer_s);

				/*	lcd_setCharPos(6,4);
  					snprintf(buffer_s, 18, "Pres %d.%02d hp",presure/100,presure%100);
  					lcd_printString(buffer_s);
				 */
				lcd_setCharPos(6,0);
				snprintf(buffer_s, 13, "%lu-> %lu.%02luV",InputVoltage,InputVoltage*66/2550,(InputVoltage*66%2550*100/255)/10 );// get two numbers for voltage
				lcd_printString(buffer_s);

				flags.new_data_to_show=FALSE; // the data was showed.


			}// end if - new data to show
			if (show_time){

				RTC_DateShow(&hrtc,aShowTime);
				lcd_setCharPos(0,11);
				lcd_printString(aShowTime);
				RTC_TimeShow(&hrtc,aShowTime);
				//lcd_setCharPos(0,12);
				lcd_setCharPos(0,1);
				lcd_printString(aShowTime);

#ifdef DEBUG	//debug
				lcd_setCharPos(3,20);
				if (beta_part){
					lcd_printString("T");
					beta_part = FALSE;
				}
				else {
					lcd_printString("-");
					beta_part = TRUE;
				}
/* Time of the working the unit.
				lcd_setCharPos(7,0);
				snprintf(buffer_s, 9, "%ld;",hrtc.Instance->TR);
				lcd_printString(buffer_s);
*/
				lcd_setCharPos(7,9);
				snprintf(buffer_s, 13, "sys%8ld;",actual_HALtick);
				lcd_printString(buffer_s);

				lcd_setCharPos(6,12);
				snprintf(buffer_s, 10, "%8ld;",backlite_compare);
				lcd_printString(buffer_s);

				//debug
#endif
				// end of the time part - new timer set.
				time_compare = fill_comparer(TIME_PERIODE);
				show_time = FALSE;
			}
			break;
		}
		case idle:
		{
			break;
		}

		case menu:
		{
			display_menu(ActualMenu);
			break;
		}

		case debug:
		{
			lcd_clear();
			RTC_DateShow(&hrtc,aShowTime);
			lcd_setCharPos(1,10);
			lcd_printString(aShowTime);

			RTC_TimeShow(&hrtc,aShowTime);
			lcd_setCharPos(1,0);
			lcd_printString(aShowTime);

			//CITAC
			uint32_t sekundy = (SlaveCounter << 16)/1000000;

			lcd_setCharPos(4,1);
			snprintf(buffer_s, 20, "%03lu:%02lu", sekundy/60,sekundy%60 );
			lcd_printString(buffer_s);

			// reading the actual number from encoder

			count1=htim22.Instance->CNT;
			lcd_setCharPos(5,0);
			snprintf(buffer_s, 25, "s %i \r t %i ms", (count1+T_led)*MAIN_LOOP*2, MAIN_LOOP*2*T_led);
			lcd_printString(buffer_s);

			lcd_setCharPos(7,0);
			snprintf(buffer_s, 9, "EN%03i", (count1));
			lcd_printString(buffer_s);

			snprintf(buffer_s, 12, "; enc%03i", (en_count));
			lcd_printString(buffer_s);

			HAL_Delay(500);
			break;
		}
		default:{
			lcd_setCharPos(1,1);
			lcd_printString("DEFAULT   DEFFFF" );
		}
		}// switch show
		/**
		 * 2.1.2018 -
  	1) objevil jsem chybu pri cteni time elapsing. Mam to spatne. Muze se stat, ze mi timer bude blbe fungovat, kdyz budu pricitat testne pred pretecenim promene a vykonavani se bude konat mozna az po preteceni timeru...

  	2) Cteni tlacitek by se melo provadet pokazde a po stisknuti by se melo dalsi cteni oddalit na delsi cas - 200-300 ms?
		 *
		 */

		/* *------ TIME ELAPSING CHECK -------* */
		actual_HALtick = HAL_GetTick();
		if(logging_compare <= actual_HALtick) //log data after defined periode.
		{
			current_state = LOG;
		}
		if(heating_compare <= actual_HALtick) //measure after defined periode.
		{
			current_state = HEATING;
		}

		if(measure_compare <= actual_HALtick) //measure after defined periode.
		{
			current_state = MEASURING;
		}

		if(time_compare <= actual_HALtick) //change time after defined periode.
		{
			show_time = TRUE;
		}
		if(show_timeout <= actual_HALtick) //change time after defined periode.
		{
			show = desktop;
		}
// MENU TIMEOUT
		if ((flags.menu_running==1)) // je to takhle slozite , protoze jsem neprisel na jiny efektivni zpusob, jak smazat displej, po zkonceni menu
			if(menu_timout()) {
				if (!menu_action()){ // exit from menu condition
					flags.menu_running=0;
					lcd_clear();
					show = desktop;
				}
				else
					show = menu;

			} // if menu - TIMEOUT

		if (flags.temp_new_set){
			flags.temp_new_set = FALSE;
			show = desktop;
		}
		/*
  			if (led_compare<=actual_HALtick){
  				togleLED(LED1);
  				led_compare=fill_comparer(LED_PERIODE);
  			}//if BLIK LEDka
		 */
		if (backlite_compare <= actual_HALtick) // shut down the backligh
		{
			HAL_GPIO_WritePin(D_LCD_LIGHT_GPIO_Port,D_LCD_LIGHT_Pin,GPIO_PIN_RESET);
		}

		/* *---- READ KEYBOARD -----* */

		pushed_button = BUT_NONE;
		if(button_compare<=actual_HALtick) //every delay time
		{
			pushed_button = checkButtons();
			button_compare = fill_comparer(BUT_DELAY);
			//flags.enc_changed = FALSE;
			// reading the actual number from encoder
			uint32_t actual_counter_CNT = htim22.Instance->CNT;
			if (en_count_last != actual_counter_CNT){
				en_count+= actual_counter_CNT-en_count_last;
				en_count_last=actual_counter_CNT;
				flags.enc_changed = TRUE;
				if (pushed_button == BUT_NONE) // enabling light/ increase time constants.
					pushed_button = ENCODER;
			}
		}
		if(pushed_button != BUT_NONE) // any button pushed?
		{
			HAL_GPIO_WritePin(D_LCD_LIGHT_GPIO_Port,D_LCD_LIGHT_Pin,GPIO_PIN_SET);
			button_compare = fill_comparer(BUT_DELAY*200); // 200x - zpozdeni cteni pri stisknuti
			backlite_compare = fill_comparer(BACKLITE_TIMEOUT);
			show_timeout = fill_comparer(SHOW_TIMEOUT);
		}

		// -- BUTTON PROCCESS
		switch (pushed_button){
		case BUT_1:
		{// activate heater
			if (!flags.regulation_temp){
				flags.regulation_temp=TRUE;
				flags.heating_up = TRUE;

			}
			else {
				flags.regulation_temp=FALSE;
			}
			// new data to show - heating icon.
			flags.new_data_to_show=TRUE;
			//	show = debug;

			break;
		}
		case BUT_2:
		{// Immediattely heating for 15 minutes

			flags.heating_instant = TRUE;
			heating_instant_timeout = fill_comparer_seconds(HEATING_INSTANT);
			break;
		}
		case BUT_ENC:
		{
			if (0 == flags.menu_running){
				flags.menu_activate=1;
				en_count = 0;
				en_count_last=0;
				htim22.Instance->CNT= 0;
				activation_memu();
				lcd_clear();
				// if this command means go to menu, That there shouldn't be no more pressed.
				pushed_button = BUT_NONE;
			}
			break;
		}
		case ENCODER:
		{
			break;
		}
		default:
		{

			break;
		}

		} // switch pushed button

		HAL_Delay(MAIN_LOOP);


		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

	}
	/* USER CODE END 3 */

}

/** System Clock Configuration
 */
void SystemClock_Config(void)
{

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	/**Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/**Configure LSE Drive Capability
	 */
	__HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_12;
	RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_3;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_RTC
			|RCC_PERIPHCLK_USB;
	PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
	PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure the Systick interrupt time
	 */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/**Configure the Systick
	 */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void _Error_Handler(char * file, int line)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while(1)
	{
		togleLED(LED2);
		HAL_Delay(500);
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT

/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */

}

#endif

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
