/*
 *  Motor_pragram.c
 *
 *  Created on: Apr 17, 2025
 *  Author: Ahmed Ragab
 */

#include "stm32f1xx_hal.h"
#include "Motor_interface.h"

static uint32_t motor_period;
static uint8_t motor_speed;

void Motor_Init(Motor_t *Motor)
{
    motor_period = Motor->timer->Init.Period + 1;
    motor_speed = 0;
    HAL_TIM_PWM_Start(Motor->timer, Motor->pwm_channel);
    Motor_Stop(Motor);
}

void Motor_SetSpeed(Motor_t *Motor, uint8_t speed_percent)
{
    if (speed_percent > 100)
    {
    	speed_percent = 100;
    }
    motor_speed = speed_percent;
    __HAL_TIM_SET_COMPARE(Motor->timer,Motor->pwm_channel,((motor_period * motor_speed) / 100));
}

void Motor_Forward(Motor_t *Motor)
{
    HAL_GPIO_WritePin(Motor->in1_port, Motor->in1_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Motor->in2_port, Motor->in2_pin, GPIO_PIN_RESET);
    Motor_SetSpeed(Motor, motor_speed);
}

void Motor_Backward(Motor_t *Motor)
{
    HAL_GPIO_WritePin(Motor->in1_port, Motor->in1_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Motor->in2_port, Motor->in2_pin, GPIO_PIN_SET);
    Motor_SetSpeed(Motor, motor_speed);
}

void Motor_Stop(Motor_t *Motor)
{
    HAL_GPIO_WritePin(Motor->in1_port, Motor->in1_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Motor->in2_port, Motor->in2_pin, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(Motor->timer, Motor->pwm_channel, 0);
}
