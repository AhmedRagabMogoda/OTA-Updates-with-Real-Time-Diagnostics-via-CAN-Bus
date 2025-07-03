/*
 *  Motor_interface.h
 *
 *  Created on: Apr 17, 2025
 *  Author: Ahmed Ragab
 */

#ifndef MOTOR_INTERFACE_H_
#define MOTOR_INTERFACE_H_


/**
 * @brief Motor driver interface for L298N with dual-motor-per-driver support
 */
typedef struct {
    TIM_HandleTypeDef *timer;      /* PWM timer */
    uint32_t pwm_channel;          /* PWM channel (e.g., TIM_CHANNEL_1 or TIM_CHANNEL_2) */
    GPIO_TypeDef *in1_port;
    uint16_t in1_pin;
    GPIO_TypeDef *in2_port;
    uint16_t in2_pin;

} Motor_t;

/**
 * @brief Initializes the motor by starting the PWM and stopping any motion.
 * @param Motor Pointer to the motor structure containing timer and GPIO config.
 */
void Motor_Init(Motor_t *Motor);

/**
 * @brief Sets the motor speed using PWM duty cycle.
 * @param Motor Pointer to the motor structure.
 * @param speed_percent Speed as a percentage (0% to 100%).
 */
void Motor_SetSpeed(Motor_t *Motor, uint8_t speed_percent);

/**
 * @brief Moves the motor forward by setting appropriate GPIO states.
 * @param Motor Pointer to the motor structure.
 */
void Motor_Forward(Motor_t *Motor);

/**
 * @brief Moves the motor backward by setting appropriate GPIO states.
 * @param Motor Pointer to the motor structure.
 */
void Motor_Backward(Motor_t *Motor);

/**
 * @brief Stops the motor by setting all direction pins low and stopping PWM.
 * @param Motor Pointer to the motor structure.
 */
void Motor_Stop(Motor_t *Motor);

#endif /* MOTOR_INTERFACE_H_ */
