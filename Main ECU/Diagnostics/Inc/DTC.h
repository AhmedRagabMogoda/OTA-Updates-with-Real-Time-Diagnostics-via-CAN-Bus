/*
 *  DTC.h
 *
 *  Created on: May 8, 2025
 *  Author: Ahmed Ragab
 */

#ifndef DTC_H_
#define DTC_H_

#include "OTAManagerTask.h"

/**
 * Define the flash page address for storing DTC list.
 */
#define FLASH_DTC_PAGE_ADDR   (FLASH_METADATA_ADDR + FLASH_METADATA_SIZE)

/** DTC storage **/
#define MAX_DTC_CODES  10

/** Structure for a stored DTC entry: code + timestamp (tick) + datastamp (tick) */
typedef struct {
    uint32_t code;
    uint32_t timestamp;
    uint32_t datastamp;
} DTC_Entry_t;

/**
 * Read stored DTC entries into RAM buffer. Returns count.
 */
uint8_t ReadDTCListFromFlash(DTC_Entry_t *DTC_List, uint8_t maxCount);

/**
 * Clear all stored DTC entries: erase the flash page and write zero count
 */
void ClearDTCListFlash(void);

/**
 * Write the current DTC list: reads existing entries, appends new, rewrites flash.
 */
void WriteDTCListToFlash(uint32_t NewDTC);

#endif /* DTC_H_ */
