/*
 *  CANTransmitTask.h
 *
 *  Created on: May 8, 2025
 *  Author: Ahmed Ragab
 */

#ifndef INC_CANTRANSMITTASK_H_
#define INC_CANTRANSMITTASK_H_

#include "TaskHandle.h"

/**
 * @brief  Initializes the CAN transmit queue and task.
 *         Call this before vTaskStartScheduler().
 */
void InitCANTransmitTask(void);

#endif /* INC_CANTRANSMITTASK_H_ */
