/*
 *  CANReceiveTask.c
 *
 *  Created on: May 6, 2025
 *  Author: Ahmed Ragab
 */

#include "CANReceiveTask.h"

/** Handle for internal CAN Rx queue **/
QueueHandle_t xCanRxQueue = NULL;

/** External queues to dispatch messages **/
extern QueueHandle_t xOtaQueue;        /** Queue for OTA manager **/
extern QueueHandle_t xDiagQueue;       /** Queue for diagnostics **/
extern QueueHandle_t xSensorQueue;     /** Queue for sensor data **/

/** CAN peripheral handle (configured via CubeMX) **/
extern CAN_HandleTypeDef hcan;


/**
 * @brief  Task to process received CAN messages.
 *         Dispatches frames to the appropriate subsystem queues.
 */
static void StartCANReceiveTask(void *pvParameters)
{
    CanRxMessage_t Msg;

    for (;;)
    {
        /** Wait indefinitely for next CAN Rx message **/
        if (xQueueReceive(xCanRxQueue, &Msg, portMAX_DELAY) == pdTRUE)
        {
            switch (Msg.header.StdId)
            {
                case UDS_REQUEST_ID:
                    /** Forward diagnostic requests to diagnostics task **/
                    xQueueSend(xDiagQueue, &Msg.data, 0);
                    break;

                case SENSOR_TEMP_ID:
                case SENSOR_DIST_ID:
                    /** Forward sensor data to control task **/
                    xQueueSend(xSensorQueue, &Msg, 0);
                    break;

                default:
                    /** Unknown CAN ID: ignore **/
                    break;
            }
        }
    }
}

/**
 * @brief  Initializes the CAN receive queue and task.
 *         Must be called before vTaskStartScheduler().
 */
void InitCANReceiveTask(void)
{
    /** Create queue to hold up to 20 CAN Rx messages **/
    xCanRxQueue = xQueueCreate(20, sizeof(CanRxMessage_t));

    /** Activate CAN RX FIFO0 interrupt notifications **/
    HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

    /** Create the CAN receive task at high priority **/
    xTaskCreate(
        StartCANReceiveTask,        /** Task function **/
        "CanReceive",              /** Task name **/
        256,                        /** Stack size in words **/
        NULL,                       /** No parameters **/
        tskIDLE_PRIORITY + 3,       /** High priority **/
        NULL                        /** No task handle required **/
    );
}



/**
 * @brief  CAN Rx ISR: reads pending message and enqueues it
 *         to xCanRxQueue for processing by the receive task.
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CanRxMessage_t msg;
    BaseType_t xWoken = pdFALSE;

    /** Retrieve CAN message from FIFO0 **/
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &msg.header, msg.data);

    /** Enqueue for deferred processing in task context **/
    xQueueSendFromISR(xCanRxQueue, &msg, &xWoken);
    portYIELD_FROM_ISR(xWoken);
}
