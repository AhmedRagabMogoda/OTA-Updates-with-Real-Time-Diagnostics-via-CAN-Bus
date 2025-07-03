/*
 *  TaskHandle.c
 *
 *  Created on: Jun 25, 2025
 *  Author: Ahmed Ragab
 */

#include "TaskHandle.h"

/* Initialize FreeRTOS queues and tasks */
void FreeRTOS_Init(void)
{
    InitCANReceiveTask();    /** Receive CAN frames **/
    InitCANTransmitTask();   /** Transmit CAN frames **/
    InitControlTask();       /** Sensor processing and motor safety **/
    InitDiagnosticsTask();   /** UDS diagnostic services **/
    InitOTAManagerTask();    /** Manage OTA trigger **/
    InitUARTReceiveTask();
}

void Start_Scheduler(void)
{
    /* Start FreeRTOS scheduler */
    vTaskStartScheduler();
}
