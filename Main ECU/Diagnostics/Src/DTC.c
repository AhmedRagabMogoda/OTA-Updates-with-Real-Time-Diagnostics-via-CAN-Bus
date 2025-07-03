/*
 *  DTC.c
 *
 *  Created on: May 8, 2025
 *  Author: Ahmed Ragab
 */

#include "stm32f1xx_hal.h"
#include "DTC.h"
#include "RTC.h"

/**
 * Read stored DTC entries into RAM buffer. Returns count.
 */
uint8_t ReadDTCListFromFlash(DTC_Entry_t *DTC_List, uint8_t MaxCount)
{
    uint32_t Address = FLASH_DTC_PAGE_ADDR;
    uint8_t Count = (uint8_t)(*(uint32_t*)Address & 0xFFU);
    Address += 4;
    if (Count > MaxCount) Count = MaxCount;

    uint8_t Counter;
    for (Counter = 0; Counter < Count; Counter++)
    {
        DTC_List[Counter].code = *(uint32_t*)Address;
        Address += 4;
        DTC_List[Counter].timestamp = *(uint32_t*)Address;
        Address += 4;
        DTC_List[Counter].datastamp = *(uint32_t*)Address;
        Address += 4;
    }

    return Count;
}

/**
 * Clear all stored DTC entries: erase the flash page and write zero count
 */
void ClearDTCListFlash(void)
{
    FLASH_EraseInitTypeDef EraseInit = {0};
    uint32_t PageError;

    HAL_FLASH_Unlock();

    /* Erase the DTC page */
    EraseInit.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInit.PageAddress = FLASH_DTC_PAGE_ADDR;
    EraseInit.NbPages     = 1;
    HAL_FLASHEx_Erase(&EraseInit, &PageError);

    /* Write zero count */
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_DTC_PAGE_ADDR, 0U);

    HAL_FLASH_Lock();
}

/**
 * Write the current DTC list: reads existing entries, appends new, rewrites flash.
 */
void WriteDTCListToFlash(uint32_t NewDTC)
{
    DTC_Entry_t Entries[MAX_DTC_CODES];
    uint8_t Count = ReadDTCListFromFlash(Entries, MAX_DTC_CODES);

    if (Count < MAX_DTC_CODES)
    {
        /* Append new entry */
        Entries[Count].code      = NewDTC;
        Entries[Count].timestamp = RTC_GetTime();
        Entries[Count].datastamp = RTC_GetDate();
        Count++;

        /* Erase and rewrite */

        FLASH_EraseInitTypeDef EraseInit = {0};
        uint32_t PageError;

        HAL_FLASH_Unlock();
        EraseInit.TypeErase   = FLASH_TYPEERASE_PAGES;
        EraseInit.PageAddress = FLASH_DTC_PAGE_ADDR;
        EraseInit.NbPages     = 1;
        HAL_FLASHEx_Erase(&EraseInit, &PageError);

        uint32_t Address = FLASH_DTC_PAGE_ADDR;
        /* Write count */
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, Count);
        Address += 4;

        /* Write each entry */
        uint8_t Counter;
        for (Counter = 0; Counter < Count; Counter++)
        {
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, Entries[Counter].code);
            Address += 4;
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, Entries[Counter].timestamp);
            Address += 4;
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, Entries[Counter].datastamp);
            Address += 4;
        }

        HAL_FLASH_Lock();
    }
}
