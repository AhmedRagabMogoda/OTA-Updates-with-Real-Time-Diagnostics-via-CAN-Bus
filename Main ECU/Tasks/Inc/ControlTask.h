/*
 *  ControlTask.h
 *
 *  Created on: May 6, 2025
 *  Author: Ahmed Ragab
 */

#ifndef INC_CONTROLTASK_H_
#define INC_CONTROLTASK_H_

#include "TaskHandle.h"
#include "DTC.h"

/** Safety threshold: stop if distance < this (cm) **/
#define MOTOR_BLOCK_DISTANCE_CM  5U
#define MOTOR_DIR_STOP           0U        /** Stop all motors **/
#define TEMP_THRESHOLD           35U       /**< °C threshold for temperature DTC */
#define DTC_ULTRASONIC_FAILURE   0xF250U   /**< DTC code for high temperature */
#define DTC_ULTRASONIC_BLOCKED   0xF251U   /**< DTC code for obstacle distance */
#define DTC_TEMP_SENSOR_FAILURE  0xF260U   /** Temperature sensor hardware failure */
#define DTC_OVER_TEMPERATURE     0xF261U   /** Over-temperature detected */
/**
 * @brief  Initializes the control task and its sensor queue.
 *         Call before vTaskStartScheduler().
 */
void InitControlTask(void);

#endif /* INC_CONTROLTASK_H_ */
