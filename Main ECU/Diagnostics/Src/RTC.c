/*
 *  RTC.c
 *
 *  Created on: May 8, 2025
 *  Author: Ahmed Ragab
 */
#include "stm32f1xx_hal.h"

extern RTC_HandleTypeDef hrtc;

/**
 * @brief  Get current time from RTC, pack into 32-bit word:
 *         [7:0]   Seconds (0-59)
 *         [15:8]  Minutes (0-59)
 *         [23:16] Hours   (0-23)
 *         [24]    AM/PM flag (0=AM,1=PM)
 *         [31:25] Reserved
 * @return packed 32-bit time word
 */
uint32_t RTC_GetTime(void)
{
	RTC_TimeTypeDef Local_RTCTime;
	uint32_t Time = 0;

    HAL_RTC_GetTime(&hrtc, &Local_RTCTime, RTC_FORMAT_BIN);

    Time |= (uint32_t)(Local_RTCTime.Seconds & 0xFF);
    Time |= (uint32_t)(Local_RTCTime.Minutes & 0xFF) << 8;
    Time |= (uint32_t)(Local_RTCTime.Hours   & 0xFF) << 16;

    return Time;
}

/**
 * @brief  Get current date from RTC, pack into 32-bit word:
 *         [7:0]   Day   (1-31)
 *         [15:8]  Month (1-12)
 *         [31:16] Year  (full year, e.g., 2025)
 * @return packed 32-bit date word
 */
uint32_t RTC_GetDate(void)
{
    RTC_DateTypeDef Local_RTCDate;
    uint32_t Date = 0;
    uint32_t year = 0;

    HAL_RTC_GetDate(&hrtc, &Local_RTCDate, RTC_FORMAT_BIN);

    year = (uint32_t)(Local_RTCDate.Year + 2000);
    Date |= (uint32_t)(Local_RTCDate.Date & 0xFF);
    Date |= (uint32_t)(Local_RTCDate.Month & 0xFF) << 8;
    Date |= year << 16;
    return Date;
}

void RTC_ConfigTime(uint8_t Copy_u8Hour,uint8_t Copy_u8Min,uint8_t Copy_u8Sec)
{
	RTC_TimeTypeDef Local_RTCTime;
	Local_RTCTime.Hours=Copy_u8Hour;
	Local_RTCTime.Minutes=Copy_u8Min;
	Local_RTCTime.Seconds=Copy_u8Sec;
	HAL_RTC_SetTime(&hrtc, &Local_RTCTime,RTC_FORMAT_BIN);
}

void RTC_ConfigData(uint8_t Copy_u8Day,uint8_t Copy_u8Month,uint8_t Copy_u8Your)
{
	RTC_DateTypeDef Local_RTCDate;
	Local_RTCDate.Date=Copy_u8Day;
	Local_RTCDate.Month=Copy_u8Month;
	Local_RTCDate.Year=Copy_u8Your;
	HAL_RTC_SetDate(&hrtc, &Local_RTCDate,RTC_FORMAT_BIN);
}

