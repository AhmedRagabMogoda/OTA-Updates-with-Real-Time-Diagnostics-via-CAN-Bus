/*
 *  UARTReceiveTask.c
 *
 *  Created on: Jun 29, 2025
 *  Author: Ahmed Ragab
 */

#include "UARTReceiveTask.h"

extern UART_HandleTypeDef huart2;
extern QueueHandle_t xCanTxQueue;

/** Latest sensor values **/
extern float LatestTemp;
extern float LatestDist;

QueueHandle_t xUartQueue = NULL;
uint8_t RXMsg[4] ={0};

/**
 * @brief  Control task: processes sensor data from UART and sends motor commands via UART.
 * @param  pvParameters  Not used
 */
static void StartUARTReceiveTask(void *pvParameters)
{
	uint8_t Msg[4] ={0};
    uint16_t sensorId;
    uint16_t sensorValue;
    uint16_t SpeedValue;
    CanTxMessage_t  TXMsg;
    uint8_t motorCmdFrame[4];  /* [0..1] = MOTOR_CMD_ID, [2] = direction, [3] = speed */

    for (;;)
    {
        /** Wait for a UART sensor message (4 bytes) **/
        if (xQueueReceive(xUartQueue, Msg, portMAX_DELAY) == pdTRUE)
        {
            /** Parse little-endian sensor ID and value **/
            sensorId    = (uint16_t)(Msg[0] | (Msg[1] << 8));
            sensorValue = (uint16_t)(Msg[2] | (Msg[3] << 8));

            switch (sensorId)
            {
                case SENSOR_TEMP_ID:
                    LatestTemp = (float)sensorValue;
                    if (LatestTemp > TEMP_THRESHOLD)
                    {
                        WriteDTCListToFlash(DTC_OVER_TEMPERATURE);
                    }
                    else if (LatestTemp == 0)
                    {
                        WriteDTCListToFlash(DTC_TEMP_SENSOR_FAILURE);
                    }
                    break;

                case SENSOR_DIST_ID:
                    LatestDist = (float)sensorValue;

                    if (LatestDist < MOTOR_BLOCK_DISTANCE_CM)
                    {
                        /** Prepare motor STOP command **/
                        motorCmdFrame[0] = (uint8_t)(MOTOR_CMD_ID & 0xFF);
                        motorCmdFrame[1] = (uint8_t)((MOTOR_CMD_ID >> 8) & 0xFF);
                        motorCmdFrame[2] = MOTOR_DIR_STOP;
                        motorCmdFrame[3] = 0U;

                        /** Send motor command over UART **/
                        HAL_UART_Transmit_IT(&huart2, motorCmdFrame, 4);

                        /** Log DTC **/
                        WriteDTCListToFlash(DTC_ULTRASONIC_BLOCKED);
                    }
                    else if (LatestDist == 0)
                    {
                        WriteDTCListToFlash(DTC_ULTRASONIC_FAILURE);
                    }
                    break;

                case SPEED_ID:
                	SpeedValue = sensorValue;
                    TXMsg.StdId = SPEED_ID;
                    TXMsg.DLC   = 2;
                    TXMsg.Data[0] = (uint8_t)(SpeedValue & 0xFF);
                    TXMsg.Data[1] = (uint8_t)((SpeedValue >> 8) & 0xFF);
                    xQueueSend(xCanTxQueue, &TXMsg, portMAX_DELAY);

                    break;

                default:
                    break;
            }
        }
    }
}

/**
 * @brief  Initializes the UART receive queue and task.
 *         Must be called before vTaskStartScheduler().
 */
void InitUARTReceiveTask(void)
{
    /** Create queue to hold up to 20 UART Rx messages **/
	xUartQueue = xQueueCreate(20, 4);

    /** Activate UART RX interrupt notifications **/
    HAL_UART_Receive_IT(&huart2, RXMsg, 4);

    /** Create the CAN receive task at high priority **/
    xTaskCreate(
        StartUARTReceiveTask,       /** Task function **/
        "UARTReceive",              /** Task name **/
        128,                        /** Stack size in words **/
        NULL,                       /** No parameters **/
        tskIDLE_PRIORITY + 3,       /** High priority **/
        NULL                        /** No task handle required **/
    );
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        BaseType_t xWoken = pdFALSE;

        /** Enqueue for deferred processing in task context **/
        xQueueSendFromISR(xUartQueue, RXMsg, &xWoken);

        /* Restart UART receive interrupt */
        HAL_UART_Receive_IT(&huart2, RXMsg, 4);
        //HAL_Delay(500);
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5);

        portYIELD_FROM_ISR(xWoken);
    }
}
