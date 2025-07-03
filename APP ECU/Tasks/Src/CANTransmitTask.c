/*
 *  CANTransmitTask.c
 *
 *  Created on: May 3, 2025
 *  Author: Ahmed Ragab
 */

#include "CANTransmitTask.h"


/** Handle for the CAN transmit queue **/
QueueHandle_t xCanTxQueue = NULL;

/** External CAN peripheral handle (configured in CubeMX) **/
extern CAN_HandleTypeDef hcan;

/**
 * @brief  Task that dequeues CAN messages and transmits them on the bus.
 * @param  pvParameters  Not used
 */
static void StartCANTransmitTask(void *pvParameters)
{
    CanMessage_t msg;
    CAN_TxHeaderTypeDef txHeader;
    uint32_t txMailbox;

    for (;;)
    {
        /** Wait indefinitely for next CAN message **/
        if (xQueueReceive(xCanTxQueue, &msg, portMAX_DELAY) == pdTRUE)
        {
            /** Populate CAN Tx header **/
            txHeader.StdId = msg.StdId;         /** Standard ID **/
            txHeader.IDE   = CAN_ID_STD;        /** Standard frame **/
            txHeader.RTR   = CAN_RTR_DATA;      /** Data frame **/
            txHeader.DLC   = msg.DLC;           /** Payload length **/

            /** Attempt to add message to Tx mailbox **/
            HAL_CAN_AddTxMessage(&hcan, &txHeader, msg.Data, &txMailbox);
        }
    }
}

/**
 * @brief  Initializes the CAN transmit queue and task.
 *         Call before vTaskStartScheduler().
 */
void InitCANTransmitTask(void)
{
    /** Create queue for CAN messages (up to 20 messages) **/
    xCanTxQueue = xQueueCreate(20, sizeof(CanMessage_t));

    /** Create the CAN transmit task at high priority **/
    xTaskCreate(
        StartCANTransmitTask,        /** Task entry function **/
        "CanTx",                    /** Task name **/
        128,                         /** Stack depth in words **/
        NULL,                        /** No parameters **/
        tskIDLE_PRIORITY + 3,        /** High priority **/
        NULL                         /** No task handle needed **/
    );
}


