/*
 *  ControlTask.c
 *
 *  Created on: May 6, 2025
 *  Author: Ahmed Ragab
 */


#include "ControlTask.h"

/** Queue for sensor data from CANReceiveTask **/
QueueHandle_t xSensorQueue = NULL;

/** Extern queue for CAN transmit **/
extern QueueHandle_t xCanTxQueue;
extern QueueHandle_t xCanRxQueue;

/** Latest sensor values **/
float LatestTemp = 0.0f;
float LatestDist = 0.0f;

/**
 * @brief  Control task: processes sensor data and issues motor safety commands.
 * @param  pvParameters  Not used
 */
static void StartControlTask(void *pvParameters)
{
    CanRxMessage_t RXMsg;
    CanTxMessage_t  TXMsg;
    uint16_t DistInt;

    /** Prepare motor command CAN header **/
    TXMsg.StdId = MOTOR_CMD_ID;
    TXMsg.DLC   = 2;  /** direction + speed **/

    for (;;)
    {
        /** Wait indefinitely for sensor data message **/
        if (xQueueReceive(xSensorQueue, &RXMsg, portMAX_DELAY) == pdTRUE)
        {
            switch (RXMsg.header.StdId)
            {
                case SENSOR_TEMP_ID:
                    /** Update temperature (little-endian) **/
                    LatestTemp = (float)(RXMsg.data[0] | (RXMsg.data[1] << 8));
                    if (LatestTemp > TEMP_THRESHOLD)
                    {
                    	/** Trigger DTC if over threshold **/
                    	WriteDTCListToFlash(DTC_OVER_TEMPERATURE);
                    }
                    else if (LatestTemp == 0)
					{
                    	WriteDTCListToFlash(DTC_TEMP_SENSOR_FAILURE);
                    }
                    break;

                case SENSOR_DIST_ID:
                    /** Update distance (little-endian) **/
                    DistInt     = (uint16_t)(RXMsg.data[0] | (RXMsg.data[1] << 8));
                    LatestDist = (float)DistInt;

                    /** Safety: if too close, send STOP command **/
                    if (LatestDist < MOTOR_BLOCK_DISTANCE_CM)
                    {
                        TXMsg.Data[0] = MOTOR_DIR_STOP;
                        TXMsg.Data[1] = 0U;
                        for (int i = 2; i < 8; i++) { TXMsg.Data[i] = 0U; }
                        xQueueSend(xCanTxQueue, &TXMsg, portMAX_DELAY);
                        /** Trigger DTC for obstacle **/
                        WriteDTCListToFlash(DTC_ULTRASONIC_BLOCKED);
                    }
                    else if (LatestDist == 0)
                    {
                        WriteDTCListToFlash(DTC_ULTRASONIC_FAILURE);
                    }
                    break;

                default:
                    /** Unhandled ID: ignore **/
                    break;
            }
        }
    }
}

/**
 * @brief  Initializes the control task and its sensor queue.
 *         Call before vTaskStartScheduler().
 */
void InitControlTask(void)
{
    /** Create queue for sensor CAN messages (up to 10) **/
    xSensorQueue = xQueueCreate(10, sizeof(CanRxMessage_t));

    /** Create control task at high priority **/
    xTaskCreate(
        StartControlTask,              /** Task function **/
        "Control",                    /** Task name **/
        256,                           /** Stack depth in words **/
        NULL,                          /** No parameters **/
        tskIDLE_PRIORITY + 4,          /** Very high priority **/
        NULL                           /** No handle required **/
    );
}


