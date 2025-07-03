/*
 *  UARTTransmitTask.c
 *
 *  Created on: Jul 1, 2025
 *  Author: Ahmed Ragab
 */

#include "UARTTransmitTask.h"


/** UART handle **/
extern UART_HandleTypeDef huart2;

/** Queue handle for outgoing frames **/
static QueueHandle_t xUARTTxQueue = NULL;

/**
 * @brief  TX Task: waits for a Frame_t from the queue and sends it via UART.
 * @param  pvParameters  Not used
 */
static void StartUARTTransmitTask(void *pvParameters)
{
	uint8_t frame[4] = {0};

    for (;;)
    {
        /** Wait indefinitely for next UARTTXFrame **/
        if (xQueueReceive(xUARTTxQueue, frame, portMAX_DELAY) == pdTRUE)
        {
            /** Transmit the 4-byte UARTTXFrame over UART **/
            HAL_UART_Transmit_IT(&huart2, frame, 4);
        }
    }
}

/**
 * @brief  Initialize TX interface: create queue and start vTxTask.
 *         Call this after UART init and before scheduler start.
 */
void InitUARTTransmitTask(void)
{
    /** Create a queue to hold up to 10 frames **/
    xUARTTxQueue = xQueueCreate(10, 4);
    configASSERT(xUARTTxQueue != NULL);

    /** Create the TX task **/
    BaseType_t res = xTaskCreate(
    		StartUARTTransmitTask, /* Task function */
        "TxTask",              /* Task name for debugging */
        128,                   /* Stack size in words */
        NULL,                  /* pvParameters not used */
        tskIDLE_PRIORITY + 1,  /* Priority */
        NULL                   /* Task handle not needed */
    );
    configASSERT(res == pdPASS);
}

/**
 * @brief  Enqueue a sensor UARTTXFrame for transmission.
 * @param  id      Sensor ID (16-bit)
 * @param  value   Sensor value (16-bit)
 * @return pdTRUE on success, pdFALSE if queue is full
 */
void SendSensorFrame(uint16_t id, uint16_t value)
{
	uint8_t UARTTXFrame[4] = {0};

    UARTTXFrame[0] = (uint8_t)(id & 0xFF);
    UARTTXFrame[1] = (uint8_t)((id >> 8) & 0xFF);
    UARTTXFrame[2] = (uint8_t)(value & 0xFF);
    UARTTXFrame[3] = (uint8_t)((value >> 8) & 0xFF);
    xQueueSend(xUARTTxQueue, UARTTXFrame, 0);
}
