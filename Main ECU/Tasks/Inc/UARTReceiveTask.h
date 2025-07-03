/*
 *  UARTReceiveTask.h
 *
 *  Created on: Jun 29, 2025
 *  Author: Ahmed Ragab
 */

#ifndef INC_UARTRECEIVETASK_H_
#define INC_UARTRECEIVETASK_H_

#include "ControlTask.h"

/**
 * @brief  Initializes the UART receive queue and task.
 *         Must be called before vTaskStartScheduler().
 */
void InitUARTReceiveTask(void);


#endif /* INC_UARTRECEIVETASK_H_ */
