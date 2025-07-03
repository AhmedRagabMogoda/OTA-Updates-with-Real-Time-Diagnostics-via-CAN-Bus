/*
 *  OTAManagerTask.c
 *
 *  Created on: May 6, 2025
 *  Author: Ahmed Ragab
 */

#include "OTAManagerTask.h"

static metadata_t MetaData;

/** Queue handle for OTA trigger messages **/
QueueHandle_t xOtaQueue = NULL;

/**
 * @brief  OTAManager task: waits for update trigger frame,
 *         sets metadata pending flag, then resets MCU to
 *         enter bootloader for firmware reception.
 * @param  pvParameters  Not used
 */
static void StartOTAManagerTask(void *pvParameters)
{
    CanRxMessage_t msg;

    for (;;)
    {
        /** Wait indefinitely for OTA trigger message **/
        if (xQueueReceive(xOtaQueue, &msg, portMAX_DELAY) == pdTRUE)
        {

			/** Set pending flag to 1 in metadata **/
			ReadMetadata(); /** read current metadata **/
			MetaData.pending = 1U; /** mark update pending **/
			WriteMetadata(); /** write back to flash **/

			/** Short delay to ensure flash write completes **/
			vTaskDelay(50);

			/** Perform system reset to jump into bootloader **/
			NVIC_SystemReset();

        }
    }
}

/**
 * @brief  Initializes the OTA manager task and its queue.
 *         Call before vTaskStartScheduler().
 */
void InitOTAManagerTask(void)
{
    /** Create queue for OTA messages (holding one CanRxMessage_t) **/
    xOtaQueue = xQueueCreate(1, sizeof(CanRxMessage_t));

    /** Create the OTA manager task at medium priority **/
    xTaskCreate(
        StartOTAManagerTask,       /** Task function **/
        "OTAMgr",                 /** Task name **/
        128,                       /** Stack depth in words **/
        NULL,                      /** No parameters **/
        tskIDLE_PRIORITY + 0,      /** height priority **/
        NULL                       /** No task handle required **/
    );
}


/** Read metadata from flash; if invalid, initialize defaults **/
void ReadMetadata(void)
{
    metadata_t *pMetaDataAdd = (metadata_t *)FLASH_METADATA_ADDR;
    if (pMetaDataAdd->magic != METADATA_MAGIC)
    {
        /** First boot: set defaults **/
    	MetaData.magic = METADATA_MAGIC;
    	MetaData.active_app = 1U;
    	MetaData.version = 0U;
    	MetaData.pending = 0U;
        WriteMetadata();
    }
    else
    {
        memcpy(&MetaData, pMetaDataAdd, sizeof(metadata_t));
    }
}

/** Write current metadata to reserved flash page **/
void WriteMetadata(void)
{
    FLASH_EraseInitTypeDef EraseInit;
    uint32_t PageError;

    /* Unlock flash */
    HAL_FLASH_Unlock();

    /** Erase metadata page **/
    EraseInit.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInit.PageAddress = FLASH_METADATA_ADDR;
    EraseInit.NbPages     = 1;
    HAL_FLASHEx_Erase(&EraseInit, &PageError);

    /** Program metadata words **/
    uint32_t Address = FLASH_METADATA_ADDR;
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address,       MetaData.magic);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address + 4U,  MetaData.active_app);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address + 8U,  MetaData.version);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address + 12U, MetaData.pending);

    /* Lock flash */
    HAL_FLASH_Lock();
}
