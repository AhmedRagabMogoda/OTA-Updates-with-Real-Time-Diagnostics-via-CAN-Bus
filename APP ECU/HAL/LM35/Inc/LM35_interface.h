/*
 *  LM35_interface.h
 *
 *  Created on: Apr 18, 2025
 *  Author: Ahmed Ragab
 */

#ifndef LM35_INTERFACE_H_
#define LM35_INTERFACE_H_



/**
 * @brief Initializes the LM35 temperature sensor (via ADC)
 * @param hadc Pointer to the ADC handle
 */
void LM35_Init(ADC_HandleTypeDef *Copy_ADC);

/**
 * @brief Starts ADC conversion via interrupt
 */
void LM35_StartConversion(void);

/**
 * @brief Callback function to be called in HAL_ADC_ConvCpltCallback
 */
void LM35_ProcessADCValue(uint32_t ADC_value);

/**
 * @brief Gets the latest temperature value
 * @return Temperature in Celsius
 */
float LM35_GetTemperature(void);


#endif /* LM35_INTERFACE_H_ */
