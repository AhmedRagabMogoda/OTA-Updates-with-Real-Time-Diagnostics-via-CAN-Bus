/*
 *  TaskHandle.c
 *
 *  Created on: Jun 26, 2025
 *  Author: Ahmed Ragab
 */


#include "TaskHandle.h"

/* Initialize FreeRTOS queues and tasks */
void FreeRTOS_Init(void)
{
	  InitMotorTask();
	  InitBluetoothTask();
	  InitUltrasonicTask();
	  InitTemperatureSensorTask();
	  InitCANTransmitTask();
	  InitCANReceiveTask();
	  InitUARTReceiveTask();
	  InitUARTTransmitTask();
}

void Start_Scheduler(void)
{
    /* Start FreeRTOS scheduler */
    vTaskStartScheduler();
}
