/*
 *  TaskHandle.h
 *
 *  Created on: Jun 26, 2025
 *  Author: Ahmed Ragab
 */

#ifndef INC_TASKHANDLE_H_
#define INC_TASKHANDLE_H_


#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define SPEED_ID                     0x401U
#define MOTOR_CMD_ID                 0x400U  /** Identifier for motor command frames **/
#define SENSOR_TEMP_ID               0x300U  /** CAN ID for temperature frames **/
#define SENSOR_DIST_ID               0x301U  /** CAN ID for ultrasonic distance frames **/

/** Structure for CAN message **/
typedef struct {
    uint32_t StdId;    /** Standard Identifier **/
    uint8_t  DLC;      /** Data Length Code **/
    uint8_t  Data[8];  /** Data bytes **/
} CanMessage_t;

/** Structure for incoming CAN Rx messages **/
typedef struct {
    CAN_RxHeaderTypeDef header;  /** CAN Rx header **/
    uint8_t             data[8]; /** CAN Rx data bytes **/
} CanRxMessage_t;


void FreeRTOS_Init(void);
void Start_Scheduler(void);

void InitBluetoothTask(void);
void InitCANReceiveTask(void);
void InitCANTransmitTask(void);
void InitMotorTask(void);
void InitTemperatureSensorTask(void);
void InitUltrasonicTask(void);
void InitUARTReceiveTask(void);
void InitUARTTransmitTask(void);

#endif /* INC_TASKHANDLE_H_ */
