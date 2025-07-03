/*
 *  UltrasonicTask.c
 *
 *  Created on: May 3, 2025
 *  Author: Ahmed Ragab
 */


#include "UltrasonicTask.h"

void SendSensorFrame(uint16_t id, uint16_t value);

/** Queue for CAN transmit (to be created in CAN task) **/
extern QueueHandle_t xCanTxQueue;

/** Extern TIM handle (configured in CubeMX) */
extern TIM_HandleTypeDef htim3;

/** Semaphore to signal echo capture completion **/
SemaphoreHandle_t xUltrasonicSem = NULL;

float Distance = 0;

/**
 * @brief  Ultrasonic measurement task.
 *         Triggers the sensor, waits for capture, reads distance,
 *         then enqueues a CAN message for transmission.
 */
static void StartUltrasonicTask(void *pvParameters)
{
    CanMessage_t msg;
    uint16_t distInt;

    /** Prepare CAN header once **/
    msg.StdId = SENSOR_DIST_ID;
    msg.DLC   = 2;

    /** Initialize ultrasonic driver hardware **/
    Ultrasonic_Init(&htim3, TRIG_PORT, TRIG_PIN, TIM_CHANNEL_1);

    for (;;)
    {
        /** Wait next cycle **/
        vTaskDelay(pdMS_TO_TICKS(ULTRASONIC_PERIOD_MS));

        /** Send 10µs pulse to trigger measurement **/
        Ultrasonic_Trigger();

        /** Wait for capture complete or timeout **/
        if (xSemaphoreTake(xUltrasonicSem, pdMS_TO_TICKS(ULTRASONIC_TIMEOUT_MS)) == pdTRUE)
        {
            Distance = Ultrasonic_GetDistance();
            distInt = (uint16_t)Distance;
        }
        else
        {
            /** No echo: indicate error with max value **/
            distInt = 0xFFFFU;
        }

        /** Pack little-endian **/
        msg.Data[0] = (uint8_t)(distInt & 0xFFU);
        msg.Data[1] = (uint8_t)(distInt >> 8U);

        /** Zero unused bytes **/
        for (int i = 2; i < 8; i++)
        {
            msg.Data[i] = 0U;
        }

        /** Enqueue CAN message for transmission **/
        //xQueueSend(xCanTxQueue, &msg, portMAX_DELAY);

        SendSensorFrame(SENSOR_DIST_ID,distInt);
    }
}

/**
 * @brief  Creates the ultrasonic task and semaphore.
 *         Call this before vTaskStartScheduler().
 */
void InitUltrasonicTask(void)
{
    /** Create binary semaphore for echo capture **/
    xUltrasonicSem = xSemaphoreCreateBinary();

    /** Start input capture interrupt (rising edge) **/
    HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);

    /** Create the ultrasonic task at medium priority **/
    xTaskCreate(
        StartUltrasonicTask,       /** Task function **/
        "Ultrasonic",              /** Name **/
        256,                       /** Stack depth (words) **/
        NULL,                      /** No parameters **/
        tskIDLE_PRIORITY + 2,      /** Medium priority **/
        NULL                       /** No task handle needed **/
    );
}

/**
 * @brief  HAL TIM Input Capture callback.
 *         Calls driver to process edges, then gives semaphore to task.
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    /** Process rising/falling edge and compute distance **/
    Ultrasonic_CaptureCallback(htim);

    /** Notify task that measurement is ready **/
    BaseType_t xWoken = pdFALSE;
    if (xUltrasonicSem != NULL)
    {
        xSemaphoreGiveFromISR(xUltrasonicSem, &xWoken);
        portYIELD_FROM_ISR(xWoken);
    }
}
