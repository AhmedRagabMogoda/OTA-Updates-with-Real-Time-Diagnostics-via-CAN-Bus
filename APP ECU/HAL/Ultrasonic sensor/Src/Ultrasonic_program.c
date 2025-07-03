/*
 *  Ultrasonic_program.c
 *
 *  Created on: Apr 18, 2025
 *  Author: Ahmed Ragab
 */


#include "stm32f1xx_hal.h"
#include "Ultrasonic_interface.h"

/** Speed of sound in cm/us (approx. 343 m/s) **/
#define ULTRASONIC_SPEED_CM_US   0.0343f

static TIM_HandleTypeDef *Ultrasonic_Timer = NULL;
static GPIO_TypeDef *Trig_Port = NULL;
static uint16_t Trig_Pin = 0;
static uint32_t Echo_Channel = 0;

static uint32_t ICU_Start = 0;
static uint32_t ICU_End = 0;
static uint8_t Capture_State = 0;
static float Last_Distance_Cm = 0.0f;

void Ultrasonic_Init(TIM_HandleTypeDef *Copy_UltrasonicTimer,GPIO_TypeDef *Copy_TrigPort, uint16_t Copy_TrigPin, uint32_t Copy_EchoChannel)
{
	Ultrasonic_Timer = Copy_UltrasonicTimer;               /** Store handles **/
	Trig_Port = Copy_TrigPort;
	Trig_Pin = Copy_TrigPin;
	Echo_Channel = Copy_EchoChannel;

    /** Start input capture interrupt on rising edge **/
    HAL_TIM_IC_Start_IT(Ultrasonic_Timer, Echo_Channel);
}

void Ultrasonic_Trigger(void)
{
    /** Send 10us pulse on TRIG **/
    HAL_GPIO_WritePin(Trig_Port, Trig_Pin, GPIO_PIN_SET);
    for (volatile uint32_t i = 0; i < 1600; i++);   /* ~10us delay at 16MHz CPU */
    HAL_GPIO_WritePin(Trig_Port, Trig_Pin, GPIO_PIN_RESET);
}

void Ultrasonic_CaptureCallback(TIM_HandleTypeDef *Copy_UltrasonicTimer)
{
	if (Copy_UltrasonicTimer == Ultrasonic_Timer)
	{
		/** Read captured value **/
		uint32_t val = HAL_TIM_ReadCapturedValue(Copy_UltrasonicTimer, Echo_Channel);
		if (Capture_State == 0)
		{
			ICU_Start = val; /** Rising edge: record start **/
			/** Switch to capture falling edge **/
			__HAL_TIM_SET_CAPTUREPOLARITY(Copy_UltrasonicTimer, Echo_Channel, TIM_INPUTCHANNELPOLARITY_FALLING);
			Capture_State = 1;
		}
		else
		{
			ICU_End = val; /** Falling edge: record end **/
			uint32_t diff = (ICU_End >= ICU_Start) ? (ICU_End - ICU_Start) :
											         ((Copy_UltrasonicTimer->Init.Period + 1) - ICU_Start + ICU_End);
			/** Calculate time in us: diff * (prescaler+1) / timer_clock_MHz **/
			float time_us = ((float) diff * (Copy_UltrasonicTimer->Init.Prescaler + 1)) / (HAL_RCC_GetPCLK1Freq() / 1000000.0f);
			/** Distance in cm = time_us * speed / 2 **/
			Last_Distance_Cm = (time_us * ULTRASONIC_SPEED_CM_US) / 2.0f;
			/** Reset for next measurement **/
			__HAL_TIM_SET_CAPTUREPOLARITY(Copy_UltrasonicTimer, Echo_Channel, TIM_INPUTCHANNELPOLARITY_RISING);
			Capture_State = 0;
		}
	}
}

float Ultrasonic_GetDistance(void)
{
    return Last_Distance_Cm;
}
