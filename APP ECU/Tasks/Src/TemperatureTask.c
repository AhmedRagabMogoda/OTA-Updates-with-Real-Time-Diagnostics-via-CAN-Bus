/*
 *  TemperatureTask.c
 *
 *  Created on: May 3, 2025
 *  Author: Ahmed Ragab
 */


#include "TemperatureTask.h"

void SendSensorFrame(uint16_t id, uint16_t value);

/** Extern ADC handle for temperature sensor (configured in CubeMX) **/
extern ADC_HandleTypeDef hadc1;

/** Extern queue for CAN transmissions **/
extern QueueHandle_t xCanTxQueue;

float Temperature = 0;

/**
 * @brief  Task to read temperature from LM35 and enqueue CAN message.
 * @param  pvParameters  Not used
 */
static void StartTemperatureSensorTask(void *pvParameters)
{
    CanMessage_t msg;
    uint16_t tempInt;

    /** Initialize the temperature sensor driver (starts ADC) **/
    LM35_Init(&hadc1);

    /** Prepare CAN header **/
    msg.StdId = SENSOR_TEMP_ID;
    msg.DLC   = 2;  /** Two-byte payload **/

    for (;;)
    {
        /** Wait for next measurement cycle **/
        vTaskDelay(pdMS_TO_TICKS(TEMPERATURE_SENSOR_PERIOD_MS));

        /** Read latest temperature value **/
        Temperature = LM35_GetTemperature();
        tempInt     = (uint16_t)Temperature;

        /** Pack into CAN data (little-endian) **/
        msg.Data[0] = (uint8_t)(tempInt & 0xFFU);
        msg.Data[1] = (uint8_t)(tempInt >> 8U);
        for (int i = 2; i < 8; i++) { msg.Data[i] = 0U; }

        /** Enqueue for CAN transmission **/
       // xQueueSend(xCanTxQueue, &msg, portMAX_DELAY);

        SendSensorFrame(SENSOR_TEMP_ID,tempInt);
    }
}

/**
 * @brief  Create the temperature sensor task.
 *         Call this before vTaskStartScheduler().
 */
void InitTemperatureSensorTask(void)
{
    xTaskCreate(
        StartTemperatureSensorTask,          /** Task function **/
        "TempSensor",                       /** Name **/
        128,                                 /** Stack depth in words **/
        NULL,                                /** No parameters **/
        tskIDLE_PRIORITY + 2,                /** Medium priority **/
        NULL                                 /** No handle required **/
    );
}
