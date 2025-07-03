/*
 * TemperatureTask.h
 *
 *  Created on: Jun 26, 2025
 *      Author: Ahmed Ragab
 */

#ifndef INC_TEMPERATURETASK_H_
#define INC_TEMPERATURETASK_H_

#include "TaskHandle.h"
#include "LM35_interface.h"


/** Task period in milliseconds **/
#define TEMPERATURE_SENSOR_PERIOD_MS 500U

/**
 * @brief  Create the temperature sensor task.
 *         Call this before vTaskStartScheduler().
 */
void InitTemperatureSensorTask(void);

#endif /* INC_TEMPERATURETASK_H_ */
