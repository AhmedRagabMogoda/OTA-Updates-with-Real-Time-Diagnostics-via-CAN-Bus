/*
 * DiagnosticsTask.c
 *
 *  Created on: May 8, 2025
 *      Author: Ahmed Ragab
 */

#include "UDS.h"
#include "DiagnosticsTask.h"


/**< Queue for incoming diagnostic requests **/
QueueHandle_t xDiagQueue;


/**
 * @brief  Task that processes diagnostic requests directly.
 *         Receives frames from xDiagQueue and handles each service ID.
 */
static void StartDiagnosticsTask(void *pvParameters)
{
    uint8_t Data[8]={0};     /**< Buffer for received CAN frame data **/
    uint8_t DLC = 8;     /**< Default Data Length Code (all 8 bytes) **/

    for (;;)
    {
        /** Wait indefinitely for a diagnostic request frame **/
        if (xQueueReceive(xDiagQueue, Data, portMAX_DELAY) == pdTRUE)
        {
            uint8_t SID = Data[0];  /**< Service ID byte **/
            uint8_t Sub = Data[1];  /**< Sub-function byte **/

            switch (SID)
            {
                case SID_DIAG_SESSION_CTRL:
                    /** Process session control request **/
                    ProcessSessionControl(Sub);
                    break;

                case SID_SECURITY_ACCESS:
                    /** Process security access (password) **/
                    ProcessSecurityAccess(Sub, Data, DLC);
                    break;

                case SID_READ_DATA_BY_ID:
                    /** Process read sensor data by identifier **/
                    ProcessReadDataById(Sub, Data);
                    break;

                case SID_READ_DTC:
                    /** Process read DTC list request **/
                    ProcessReadDTC(Sub);
                    break;

                case SID_CLEAR_DTC:
                    /** Process clear DTC list request **/
                    ProcessClearDTC(Sub);
                    break;

                case SID_REQUEST_DOWNLOAD:
                    /** Process OTA firmware download request **/
                    ProcessRequestDownload(Sub,Data);
                    break;

                default:
                    /** Unsupported service, send negative response **/
                    SendNegative(SID, NRC_SERVICE_NOT_SUPP);
                    break;
            }
        }
    }
}

/**
 * @brief  Initialize the diagnostics task and its request queue.
 *         Must be called before starting the FreeRTOS scheduler.
 */
void InitDiagnosticsTask(void)
{
    /** Create queue for up to 10 diagnostic frames (8 bytes each) **/
    xDiagQueue = xQueueCreate(10, sizeof(uint8_t[8]));

    /** Create the Diagnostics task at medium priority **/
    xTaskCreate(
        StartDiagnosticsTask,      /**< Task entry function   **/
        "Diagnostics",            /**< Task name            **/
        256,                        /**< Stack depth in words **/
        NULL,                       /**< Task parameter       **/
        tskIDLE_PRIORITY + 2,      /**< Task priority        **/
        NULL                        /**< Task handle (unused) **/
    );
}


