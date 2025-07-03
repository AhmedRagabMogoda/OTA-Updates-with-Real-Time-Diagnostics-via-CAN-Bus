/*
 * Bluetooth_interface.h
 *
 *  Created on: Apr 17, 2025
 *      Author: Ahmed Ragab
 */

#ifndef BLUETOOTH_INTERFACE_H_
#define BLUETOOTH_INTERFACE_H_

/**
 * @brief  Initializes the Bluetooth UART interface
 * @param  huart: Pointer to UART handle (e.g., &huart1)
 */
void Bluetooth_Init(UART_HandleTypeDef *Uart);

/**
 * @brief  Registers a user-defined callback function to be called when a byte is received
 * @param  func: Pointer to callback function (takes received byte as parameter)
 */
void Bluetooth_RegisterCallback(void (*Func)(uint8_t cmd));

/**
 * @brief  Starts receiving a single byte from Bluetooth (non-blocking)
 */
void Bluetooth_StartReceive(void);

/**
 * @brief  Must be called inside HAL_UART_RxCpltCallback to process received byte
 */
void Bluetooth_IRQHandler(void);


#endif /* BLUETOOTH_INTERFACE_H_ */
