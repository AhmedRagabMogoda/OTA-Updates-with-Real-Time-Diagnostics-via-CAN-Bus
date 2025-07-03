/*
 *  CANReceiveTask.c
 *
 *  Created on: May 3, 2025
 *  Author: Ahmed Ragab
 */


#include "CANReceiveTask.h"


/** Queue handle for CAN Rx messages **/
QueueHandle_t xCanRxQueue = NULL;

/** Extern CAN peripheral handle (configured in CubeMX) **/
extern CAN_HandleTypeDef hcan;
/** Extern queue for motor commands created in MotorTask **/
extern QueueHandle_t xMotorCommandQueue;

/**
 * @brief  FreeRTOS task to process incoming CAN Rx messages.
 *         Dispatches motor commands to the motor queue.
 */
static void StartCANReceiveTask(void *pvParameters)
{
    CanRxMessage_t msg;
    MotorCommand_t cmd;

    for (;;)
    {
        /* Wait indefinitely for next CAN Rx message */
        if (xQueueReceive(xCanRxQueue, &msg, portMAX_DELAY) == pdTRUE)
        {
            /* Dispatch based on StdId */
            switch (msg.header.StdId)
            {
                case MOTOR_CMD_ID:
                    /** Extract direction and speed **/
                    cmd.direction = msg.data[0];
                    cmd.speed     = msg.data[1];
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
 * @brief  Initializes the CAN Rx queue and task.
 *         Call this before vTaskStartScheduler().
 */
void InitCANReceiveTask(void)
{
    /* Create queue to hold up to 10 CAN Rx messages */
    xCanRxQueue = xQueueCreate(10, sizeof(CanRxMessage_t));

    /* Enable CAN RX FIFO 0 message pending interrupt */
    HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

    /* Create CAN receive task at high priority */
    xTaskCreate(
        StartCANReceiveTask,        /* Task entry function */
        "CanRx",                    /* Task name */
        128,                         /* Stack depth in words */
        NULL,                        /* No parameters */
        tskIDLE_PRIORITY + 3,        /* High priority */
        NULL                         /* No task handle required */
    );
}

/**
 * @brief  HAL callback for CAN RX FIFO 0 message pending.
 *         Reads Rx message and enqueues to xCanRxQueue.
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CanRxMessage_t msg;
    BaseType_t xWoken = pdFALSE;

    /* Retrieve CAN message from FIFO0 */
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &msg.header, msg.data);

    /* Enqueue received message to CAN Rx queue */
    xQueueSendFromISR(xCanRxQueue, &msg, &xWoken);
    portYIELD_FROM_ISR(xWoken);
}
