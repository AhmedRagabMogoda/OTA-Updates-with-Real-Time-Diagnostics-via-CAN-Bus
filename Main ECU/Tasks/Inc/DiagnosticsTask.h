/*
 * DiagnosticsTask.h
 *
 *  Created on: May 8, 2025
 *      Author: Ahmed Ragab
 */

#ifndef INC_DIAGNOSTICSTASK_H_
#define INC_DIAGNOSTICSTASK_H_

#include "TaskHandle.h"

/**
 * @brief  Initialize the diagnostics task and its request queue.
 *         Must be called before starting the FreeRTOS scheduler.
 */
void InitDiagnosticsTask(void);

#endif /* INC_DIAGNOSTICSTASK_H_ */
