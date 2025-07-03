/*
 *  Bluetooth_program.c
 *
 *  Created on: Apr 17, 2025
 *  Author: Ahmed Ragab
 */

#include "stm32f1xx_hal.h"
#include "Motor_interface.h"
#include "Bluetooth_interface.h"

UART_HandleTypeDef *BlueTooth_UART;
static void (*CommandCallback)(uint8_t) = NULL;
static uint8_t Received_Cmd;

void Bluetooth_Init(UART_HandleTypeDef *UART)
{
	BlueTooth_UART = UART;
}

void Bluetooth_RegisterCallback(void (*Func)(uint8_t cmd))
{
	CommandCallback = Func;
}

void Bluetooth_StartReceive(void)
{
	HAL_UART_Receive_IT(BlueTooth_UART, &Received_Cmd, 1);
}

void Bluetooth_IRQHandler(void)
{
    if(CommandCallback != NULL)
    {
    	CommandCallback(Received_Cmd);
    }
    /* Restart the reception for the next byte */
    HAL_UART_Receive_IT(BlueTooth_UART, &Received_Cmd, 1);
}
