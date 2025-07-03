/*
 *  TaskHandle.h
 *
 *  Created on: May 7, 2025
 *  Author: Ahmed Ragab
 */

#ifndef INC_TASKHANDLE_H_
#define INC_TASKHANDLE_H_

#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/** CAN IDs for different message types **/
#define UPDATE_FRAME_ID       0x200U  /** OTA update frames from Pi **/
#define UDS_REQUEST_ID        0x7E0U  /** Incoming UDS Diagnostic requests **/
#define UDS_RESPONSE_ID       0x7E8U  /** Outgoing diagnostic responses **/
#define SENSOR_TEMP_ID        0x300U  /** Temperature sensor frames from App ECU **/
#define SENSOR_DIST_ID        0x301U  /** Distance sensor frames from App ECU **/
#define MOTOR_CMD_ID          0x400U  /** Motor command ID **/
#define SPEED_ID              0x401U

/** Structure for CAN Rx messages **/
typedef struct {
    CAN_RxHeaderTypeDef header;  /** CAN Rx header **/
    uint8_t             data[8]; /** Data bytes **/
} CanRxMessage_t;

/** Structure for CAN messages to transmit **/
typedef struct {
    uint32_t StdId;    /** Standard ID **/
    uint8_t  DLC;      /** Data Length Code **/
    uint8_t  Data[8];  /** Data bytes **/
} CanTxMessage_t;


/*=================================================================*/

/* Initialize FreeRTOS queues and tasks */
void FreeRTOS_Init(void);

/* Start FreeRTOS scheduler */
void Start_Scheduler(void);

/* Task initialization prototypes */
void InitCANReceiveTask(void);
void InitCANTransmitTask(void);
void InitControlTask(void);
void InitDiagnosticsTask(void);
void InitOTAManagerTask(void);
void InitUARTReceiveTask(void);

#endif /* INC_TASKHANDLE_H_ */
