/*
 * OTAManagerTask.h
 *
 *  Created on: May 6, 2025
 *      Author: Ahmed Ragab
 */

#ifndef INC_OTAMANAGERTASK_H_
#define INC_OTAMANAGERTASK_H_

#include "TaskHandle.h"
#include <string.h>

/* Flash memory layout parameters */
#define FLASH_BASE_ADDR        0x08000000U                       /* Start of flash */
//#define FLASH_PAGE_SIZE      0x00000400U                       /* 1 KB per flash page */
#define APP_NUM_PAGE           26U                               /* 26 KB per app */
#define FLASH_BOOT_SIZE        (8U * FLASH_PAGE_SIZE)            /* 8 KB bootloader */
#define FLASH_APP_SIZE         (APP_NUM_PAGE * FLASH_PAGE_SIZE)  /* 26 KB per app */
#define FLASH_METADATA_SIZE    (1U * FLASH_PAGE_SIZE)            /* 1 KB metadata */

/* Derived section start addresses (page-aligned) */
#define FLASH_BOOT_START       FLASH_BASE_ADDR
#define FLASH_APP1_START       (FLASH_BOOT_START + FLASH_BOOT_SIZE)
#define FLASH_APP2_START       (FLASH_APP1_START + FLASH_APP_SIZE)
#define FLASH_METADATA_ADDR    (FLASH_APP2_START + FLASH_APP_SIZE)

#define METADATA_MAGIC     0xA5A5A5A5U

/** Metadata structure in flash ("Option Bytes") **/
typedef struct {
    uint32_t magic;       /** == METADATA_MAGIC if valid */
    uint32_t active_app;  /** 1 or 2 */
    uint32_t version;     /** firmware version counter */
    uint32_t pending;     /** 1 if update pending */
} metadata_t;



/**
 * @brief  Initializes the OTA manager task and its queue.
 *         Call before vTaskStartScheduler().
 */
void InitOTAManagerTask(void);

/** Read metadata from flash; if invalid, initialize defaults **/
void ReadMetadata(void);

/** Write current metadata to reserved flash page **/
void WriteMetadata(void);

#endif /* INC_OTAMANAGERTASK_H_ */
