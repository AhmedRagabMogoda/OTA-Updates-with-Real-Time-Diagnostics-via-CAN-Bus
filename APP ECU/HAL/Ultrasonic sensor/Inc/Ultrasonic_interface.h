/*
 * Ultrasonic_interface.h
 *
 *  Created on: Apr 18, 2025
 *      Author: Ahmed Ragab
 */

#ifndef ULTRASONIC_INTERFACE_H_
#define ULTRASONIC_INTERFACE_H_

#define TRIG_PORT GPIOB
#define TRIG_PIN  GPIO_PIN_5

/**
 * @brief  Initialize ultrasonic sensor driver
 * @param  Copy_UltrasonicTimer Pointer to timer handle used for input capture
 * @param  Copy_TrigPort        GPIO port for TRIG pin
 * @param  Copy_TrigPin         GPIO pin for TRIG
 * @param  Copy_EchoChannel     TIM channel for ECHO input capture (e.g., TIM_CHANNEL_1)
 */
void Ultrasonic_Init(TIM_HandleTypeDef *Copy_UltrasonicTimer,GPIO_TypeDef *Copy_TrigPort, uint16_t Copy_TrigPin, uint32_t Copy_EchoChannel);

/**
 * @brief  Generate a 10us pulse on TRIG to start measurement
 */
void Ultrasonic_Trigger(void);

/**
 * @brief  To be called in HAL_TIM_IC_CaptureCallback
 * @param  Copy_UltrasonicTimer Pointer to timer
 */
void Ultrasonic_CaptureCallback(TIM_HandleTypeDef *Copy_UltrasonicTimer);

/**
 * @brief  Get last measured distance in centimeters
 * @return Distance in cm
 */
float Ultrasonic_GetDistance(void);


#endif /* ULTRASONIC_INTERFACE_H_ */
