/*
 * RTC.h
 *
 *  Created on: May 8, 2025
 *      Author: Ahmed Ragab
 */

#ifndef RTC_H_
#define RTC_H_

/**
 * @brief  Get current time from RTC, pack into 32-bit word:
 *         [7:0]   Seconds (0-59)
 *         [15:8]  Minutes (0-59)
 *         [23:16] Hours   (0-23)
 *         [24]    AM/PM flag (0=AM,1=PM)
 *         [31:25] Reserved
 * @return packed 32-bit time word
 */
uint32_t RTC_GetTime(void);


/**
 * @brief  Get current date from RTC, pack into 32-bit word:
 *         [7:0]   Day   (1-31)
 *         [15:8]  Month (1-12)
 *         [31:16] Year  (full year, e.g., 2025)
 * @return packed 32-bit date word
 */
uint32_t RTC_GetDate(void);

void RTC_ConfigTime(uint8_t Copy_u8Hour,uint8_t Copy_u8Min,uint8_t Copy_u8Sec);

void RTC_ConfigData(uint8_t Copy_u8Day,uint8_t Copy_u8Month,uint8_t Copy_u8Your);

#endif /* RTC_H_ */
