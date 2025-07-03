/*
 *  CANReceiveTask.h
 *
 *  Created on: May 6, 2025
 *  Author: Ahmed Ragab
 */

#ifndef INC_CANRECEIVETASK_H_
#define INC_CANRECEIVETASK_H_

#include "TaskHandle.h"

/**
 * @brief  Initializes the CAN receive queue and task.
 *         Must be called before vTaskStartScheduler().
 */
void InitCANReceiveTask(void);


#endif /* INC_CANRECEIVETASK_H_ */
