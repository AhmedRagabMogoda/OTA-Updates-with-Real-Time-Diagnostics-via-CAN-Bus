/*
 *  MotorTask.c
 *
 *  Created on: May 2, 2025
 *  Author: Ahmed Ragab
 */

#include "MotorTask.h"

void SendSensorFrame(uint16_t id, uint16_t value);

/** Extern TIM handle for PWM (configured in CubeMX) */
extern TIM_HandleTypeDef htim2;

/** Queue handle for incoming motor commands */
QueueHandle_t xMotorCommandQueue = NULL;

/** Left‐side motors (front+rear) on TIM2_CH1 */
static Motor_t motor_left = {
    .timer       = &htim2,
    .pwm_channel = TIM_CHANNEL_1,
    .in1_port    = MOTOR1_IN1_GPIO,
    .in1_pin     = MOTOR1_IN1_PIN,
    .in2_port    = MOTOR1_IN2_GPIO,
    .in2_pin     = MOTOR1_IN2_PIN,
};

/** Right‐side motors (front+rear) on TIM2_CH2 */
static Motor_t motor_right = {
    .timer       = &htim2,
    .pwm_channel = TIM_CHANNEL_2,
    .in1_port    = MOTOR2_IN3_GPIO,
    .in1_pin     = MOTOR2_IN3_PIN,
    .in2_port    = MOTOR2_IN4_GPIO,
    .in2_pin     = MOTOR2_IN4_PIN,

};

/**
 * @brief  Motor control task entry.
 *         Blocks on xMotorCommandQueue, then executes direction+speed.
 */
static void StartMotorTask(void *pvParameters)
{
    MotorCommand_t cmd;

    /** Initialize both motors before entering loop **/
    Motor_Init(&motor_left);
    Motor_Init(&motor_right);

    for (;;)
    {
        /** Wait indefinitely for next command **/
        if (xQueueReceive(xMotorCommandQueue, &cmd, portMAX_DELAY) == pdTRUE)
        {
            /** Execute direction **/
            switch (cmd.direction)
            {
                case MOTOR_DIR_FORWARD:
                    Motor_Forward(&motor_left);
                    Motor_Forward(&motor_right);
                    break;

                case MOTOR_DIR_BACKWARD:
                    Motor_Backward(&motor_left);
                    Motor_Backward(&motor_right);
                    break;

                case MOTOR_DIR_LEFT:
                    Motor_Backward(&motor_left);
                    Motor_Forward(&motor_right);
                    break;

                case MOTOR_DIR_RIGHT:
                    Motor_Forward(&motor_left);
                    Motor_Backward(&motor_right);
                    break;

                case MOTOR_DIR_STOP:
                    Motor_Stop(&motor_left);
                    Motor_Stop(&motor_right);
                    break;
                default:

            }

            /** Apply the same speed to both motors **/
            Motor_SetSpeed(&motor_left,  cmd.speed);
            Motor_SetSpeed(&motor_right, cmd.speed);


            SendSensorFrame(SPEED_ID,(uint16_t)cmd.speed);

        }
    }
}

/**
 * @brief  Creates motor command queue and starts the motor task.
 *         Call this before vTaskStartScheduler().
 */
void InitMotorTask(void)
{
    /** Create a queue to hold up to 10 MotorCommand_t items **/
    xMotorCommandQueue = xQueueCreate(10, sizeof(MotorCommand_t));

    /** Create the motor control task at high priority **/
    xTaskCreate(
        StartMotorTask,            /** Task function **/
        "MotorCtrl",              /** Task name **/
        256,                       /** Stack size in words **/
        NULL,                      /** No pvParameters **/
        tskIDLE_PRIORITY + 4,      /** High priority **/
        NULL                       /** No task handle needed **/
    );
}

