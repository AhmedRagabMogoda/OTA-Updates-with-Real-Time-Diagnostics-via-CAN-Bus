/*
 *  LM35_program.c
 *
 *  Created on: Apr 18, 2025
 *  Author: Ahmed Ragab
 */


#include "stm32f1xx_hal.h"
#include "LM35_interface.h"

/** ADC and sensor calibration constants **/
#define ADC_MAX_COUNTS         4095.0f        /** Maximum ADC count for 12-bit resolution */
#define ADC_REFERENCE_VOLTAGE  3.3f           /** ADC reference voltage in volts */
#define LM35_V_PER_DEGREE      10.0f          /** Sensor output voltage per °C (10 mV/°C) */

static ADC_HandleTypeDef *LM35_ADC = NULL;
static float Latest_Temperature = 0.0f;

void LM35_Init(ADC_HandleTypeDef *Copy_ADC)
{
	LM35_ADC = Copy_ADC;
    HAL_ADC_Start_IT(LM35_ADC);  /* Start the first conversion */
}

void LM35_StartConversion(void)
{
    if (LM35_ADC != NULL)
    {
        HAL_ADC_Start_IT(LM35_ADC);  /* Start ADC conversion in interrupt mode */
    }
}

void LM35_ProcessADCValue(uint32_t ADC_Value)
{
    /** Convert ADC count to voltage **/
    float voltage = ((float)ADC_Value / ADC_MAX_COUNTS) * ADC_REFERENCE_VOLTAGE;
    /** Convert voltage to temperature: V / (V per °C) **/
    Latest_Temperature = (voltage * 1000.0f) / LM35_V_PER_DEGREE;
}

float LM35_GetTemperature(void)
{
    return Latest_Temperature;  /* Return the last stored temperature */
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1)
    {
        uint32_t ADC_val = HAL_ADC_GetValue(hadc);
        LM35_ProcessADCValue(ADC_val);            /** Process and update temperature **/
        LM35_StartConversion();                   /** Start new conversion **/
    }
}
