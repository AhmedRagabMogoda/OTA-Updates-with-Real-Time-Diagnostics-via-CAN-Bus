/*
 *  UDS.h
 *
 *  Created on: May 8, 2025
 *  Author: Ahmed Ragab
 */

#ifndef DIAGNOSTICS_H_
#define DIAGNOSTICS_H_

#include "TaskHandle.h"
#include "DTC.h"

/** Service IDs **/
#define SID_DIAG_SESSION_CTRL  0x10U
#define SID_SECURITY_ACCESS    0x27U
#define SID_READ_DATA_BY_ID    0x22U
#define SID_READ_DTC           0x19U
#define SID_CLEAR_DTC          0x14U
#define SID_REQUEST_DOWNLOAD   0x34U

/** Negative response codes **/
#define NRC_SECURITY_REQUIRED  0x31U
#define NRC_SECURITY_DENIED    0x35U
#define NRC_SERVICE_NOT_SUPP   0x11U

/** Sessions **/
#define SESSION_DEFAULT        0x00U
#define SESSION_SENSOR         0x01U
#define SESSION_CONTROL        0x02U
#define SESSION_PROGRAMMING    0x03U

/** Fixed password **/
#define DIAG_PASSWORD          0x1234U

/*====================================================================*/

/**
 * SendResponse
 *
 * Splits a data buffer into UDS diagnostic frames and
 * enqueues them onto the CAN transmit queue.
 *
 * @param Data Pointer to data bytes to send
 * @param DLC  Number of valid data bytes in Data
 */
void SendResponse(uint8_t *Data, uint8_t DLC);

/**
 * SendNegative
 *
 * Send a UDS negative response with standard 0x7F prefix.
 *
 * @param SID Service Identifier that failed
 * @param NRC Negative Response Code
 */
void SendNegative(uint8_t SID, uint8_t NRC);

/**
 * SendPositive
 *
 * Send a UDS positive response (SID + 0x40) with one sub-function byte.
 *
 * @param SID Service Identifier
 * @param Sub Sub-function or parameter byte
 */
void SendPositive(uint8_t SID, uint8_t Sub);

/**
 * ProcessSessionControl
 *
 * Handle UDS Session Control request.
 *
 * @param Sub Requested session type
 */
void ProcessSessionControl(uint8_t Sub);

/**
 * ProcessSecurityAccess
 *
 * Handle UDS Security Access (seed/key) challenge.
 *
 * @param Sub   Security sub-function (0 = send password)
 * @param Data  Pointer to incoming CAN data buffer
 * @param DLC   Number of valid bytes in Data
 */
void ProcessSecurityAccess(uint8_t Sub, uint8_t *Data, uint8_t DLC);

/**
 * ProcessReadDataById
 *
 * Handle UDS Read Data By Identifier request.
 *
 * @param Sub  Sub-function (offset for response)
 * @param Data Pointer to incoming CAN data buffer
 */
void ProcessReadDataById(uint8_t Sub, uint8_t *Data);

/**
 * ProcessReadDTC
 *
 * Handle UDS Read DTC request. Returns count + individual entries.
 *
 * @param Sub Sub-function (offset for response)
 */
void ProcessReadDTC(uint8_t Sub);

/**
 * ProcessClearDTC
 *
 * Handle UDS Clear DTC request: erase stored DTCs.
 *
 * @param Sub Sub-function (offset for response)
 */
void ProcessClearDTC(uint8_t Sub);

/**
 * ProcessRequestDownload
 *
 * Handle UDS Request Download (enter bootloader) request.
 * On success, queue OTA message and perform system reset.
 *
 * @param Sub Sub-function (offset for response)
 * @param Msg Pointer to the full CAN message buffer
 */
void ProcessRequestDownload(uint8_t Sub, uint8_t* Msg);


#endif /* DIAGNOSTICS_H_ */
