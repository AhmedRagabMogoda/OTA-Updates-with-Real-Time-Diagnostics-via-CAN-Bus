/*
 *  BluetoothTask.c
 *
 *  Created on: May 2, 2025
 *  Author: Ahmed Ragab
 */

#include "BluetoothTask.h"

void Motor_IRQHandler(void);

/** Queue for raw Bluetooth command bytes **/
QueueHandle_t xBluetoothQueue = NULL;

/** Handle for the Bluetooth FreeRTOS task **/
static TaskHandle_t xBluetoothTaskHandle = NULL;

/** External handle **/
extern QueueHandle_t xMotorCommandQueue;
extern UART_HandleTypeDef huart1;
/**
 * @brief  ISR callback invoked by Bluetooth driver when a byte arrives.
 *         Enqueues the byte into xBluetoothQueue from ISR context.
 * @param  cmd  Received command byte
 */
void BluetoothISR_Callback(uint8_t cmd)
{
    BaseType_t xWoken = pdFALSE;
    xQueueSendFromISR(xBluetoothQueue, &cmd, &xWoken);
    portYIELD_FROM_ISR(xWoken);
}

/**
 * @brief  FreeRTOS task that processes Bluetooth commands.
 *         Blocks on xBluetoothQueue, translates each byte into a MotorCommand,
 *         then sends it to the motor control task via xMotorCommandQueue.
 * @param  pvParameters  Not used
 */
static void StartBluetoothTask(void *pvParameters)
{
    uint8_t cmd;
    MotorCommand_t motorCmd;

    /* Initialize the Bluetooth driver with UART handle */
    Bluetooth_Init(&huart1);
    /* Register ISR callback to enqueue incoming bytes */
    Bluetooth_RegisterCallback(BluetoothISR_Callback);
    /* Start interrupt-driven reception */
    Bluetooth_StartReceive();

    for (;;)
    {
        /* Wait indefinitely for next byte from ISR */
        if (xQueueReceive(xBluetoothQueue, &cmd, portMAX_DELAY) == pdTRUE)
        {
			switch (cmd)
			{
			/* Numeric '1'..'9': adjust speed */
			case '1' ... '9':
				motorCmd.speed = (cmd - '0') * 10; /* Translate ASCII command into MotorCommand_t */
				break;
			case 'F':
				motorCmd.direction = MOTOR_DIR_FORWARD;
				break;
			case 'B':
				motorCmd.direction = MOTOR_DIR_BACKWARD;
				break;
			case 'L':
				motorCmd.direction = MOTOR_DIR_LEFT;
				break;
			case 'R':
				motorCmd.direction = MOTOR_DIR_RIGHT;
				break;
			case 'Y':
				motorCmd.direction = MOTOR_DIR_STOP;
				break;
			default:
				/* Unknown command: ignore */
				continue;
			}

            /* Send the assembled motor command to the motor control task */
            xQueueSend(xMotorCommandQueue, &motorCmd, portMAX_DELAY);
        }
    }
}

/**
 * @brief  Creates the Bluetooth processing task and its queue.
 *         Must be called before starting the scheduler.
 */
void InitBluetoothTask(void)
{
    /* Create queue for raw Bluetooth bytes (length = 10) */
    xBluetoothQueue = xQueueCreate(10, sizeof(uint8_t));

    /* Create the Bluetooth task at medium priority */
    xTaskCreate(
        StartBluetoothTask,      /* Task entry function */
        "Bluetooth",             /* Name for debugging */
        256,                     /* Stack size in words */
        NULL,                    /* No parameters */
        tskIDLE_PRIORITY + 2,    /* Medium priority */
        &xBluetoothTaskHandle    /* Task handle */
    );
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        Bluetooth_IRQHandler();
    }
    else if (huart->Instance == USART2)
    {
    	Motor_IRQHandler();
    }
}
