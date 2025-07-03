/*
 *  UARTReceiveTask.c
 *
 *  Created on: Jun 29, 2025
 *  Author: Ahmed Ragab
 */


#include "UARTReceiveTask.h"

/** Extern queue **/
extern QueueHandle_t xMotorCommandQueue;
extern UART_HandleTypeDef huart2;

QueueHandle_t xUartQueue = NULL;
uint8_t RXMsg[4] ={0};

/**
 * @brief  Control task: processes sensor data from UART and sends motor commands via UART.
 * @param  pvParameters  Not used
 */
static void StartUARTReceiveTask(void *pvParameters)
{
	MotorCommand_t cmd;
	uint8_t Msg[4] ={0};
    uint16_t sensorId;



    for (;;)
    {
        /** Wait for a UART sensor message (4 bytes) **/
        if (xQueueReceive(xUartQueue, Msg, portMAX_DELAY) == pdTRUE)
        {
            /** Parse little-endian sensor ID and value **/
            sensorId    = (uint16_t)(Msg[0] | (Msg[1] << 8));


            /* Dispatch based on StdId */
            switch (sensorId)
            {
            case MOTOR_CMD_ID:
            	/** Extract direction and speed **/
            	cmd.direction = Msg[2];
                cmd.speed     = Msg[3];
                /* Forward to motor control queue */
                xQueueSend(xMotorCommandQueue, &cmd, 0);
                break;
            default:
            	/* Unknown CAN ID: ignore */
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

void Motor_IRQHandler(void)
{
    BaseType_t xWoken = pdFALSE;

    /** Enqueue for deferred processing in task context **/
    xQueueSendFromISR(xUartQueue, RXMsg, &xWoken);

    /* Restart UART receive interrupt */
    HAL_UART_Receive_IT(&huart2, RXMsg, 4);

    portYIELD_FROM_ISR(xWoken);
}
