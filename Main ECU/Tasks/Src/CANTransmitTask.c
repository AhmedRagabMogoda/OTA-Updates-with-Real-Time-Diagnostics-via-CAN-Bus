/*
 *  CANTransmitTask.c
 *
 *  Created on: May 8, 2025
 *  Author: Ahmed Ragab
 */

#include "CANTransmitTask.h"


/** External CAN peripheral handle (configured in CubeMX) **/
extern CAN_HandleTypeDef hcan;

/** Handle for the CAN Tx queue **/
QueueHandle_t xCanTxQueue = NULL;

/**
 * @brief  Task: dequeues CAN messages and transmits them on the bus.
 * @param  pvParameters  Not used
 */
static void StartCANTransmitTask(void *pvParameters)
{
    CanTxMessage_t msg;
    CAN_TxHeaderTypeDef txHeader;
    uint32_t txMailbox;

    for (;;)
    {
        /** Block until a CAN message is enqueued **/
        if (xQueueReceive(xCanTxQueue, &msg, portMAX_DELAY) == pdTRUE)
        {
            /** Populate CAN Tx header **/
            txHeader.StdId = msg.StdId;      /** Standard ID **/
            txHeader.IDE   = CAN_ID_STD;     /** Standard frame **/
            txHeader.RTR   = CAN_RTR_DATA;   /** Data frame **/
            txHeader.DLC   = msg.DLC;        /** Payload length **/

            /** Send the CAN frame **/
            HAL_CAN_AddTxMessage(&hcan, &txHeader, msg.Data, &txMailbox);
        }
    }
}

/**
 * @brief  Initializes the CAN transmit queue and task.
 *         Call this before vTaskStartScheduler().
 */
void InitCANTransmitTask(void)
{
    /** Create queue for up to 20 CAN messages **/
    xCanTxQueue = xQueueCreate(20, sizeof(CanTxMessage_t));

    /** Create the CAN Transmit task at high priority **/
    xTaskCreate(
        StartCANTransmitTask,     /** Task function **/
        "CanTx",                  /** Task name **/
        128,                      /** Stack depth (words) **/
        NULL,                     /** No parameters **/
        tskIDLE_PRIORITY + 3,     /** High priority **/
        NULL                      /** No task handle required **/
    );
}

