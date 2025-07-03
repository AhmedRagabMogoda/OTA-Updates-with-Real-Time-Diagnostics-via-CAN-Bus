/*
 *  Bootloader.c
 *
 *  Created on: Apr 23, 2025
 *  Author: Ahmed Ragab
 *
 * Bootloader for STM32F103:
 * - Split flash: Bootloader, App1, App2
 * - Metadata stored in reserved flash page
 *   contains magic, active app index (1 or 2), version, pending flag
 * - On reset: Bootloader reads metadata; if update pending, fetch FW via CAN ;
 *   else jumps to active app
 */

#include "stm32f1xx_hal.h"
#include "Bootloader.h"
#include <string.h>

extern CAN_HandleTypeDef hcan;

extern metadata_t MetaData;

/* Globals for update */
static uint8_t  ChunkBuffer[FLASH_PAGE_SIZE];
static uint32_t ChunkOffset;
static uint32_t ChunkIndex;
static uint32_t TotalChunks;
static uint32_t DestAddGlobal;
static uint8_t  SizeReceived;   /* 0: waiting size, 1: receiving data */


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

/**
 * @brief  Jumps to the user application located at a specified address.
 * @param  appAddress: Start address of the user application in flash.
 * @note   This function does not return.
 */
void JumpToApp(uint32_t Address)
{
	/* Pointer to function to hold the address of the reset handler of the user App */
	void (*App_ResetHandler)(void);

	uint32_t ResetHandlerAddress;

	/* Configuration MSP of the user App by Reading value from base address */
	uint32_t Local_u32MSPVal = *((volatile uint32_t*)Address);
	__asm volatile("MSR MSP,%0"::"r"(Local_u32MSPVal));

	ResetHandlerAddress = *((volatile uint32_t*)(Address + 4));

	App_ResetHandler = (void*)ResetHandlerAddress;

	/* Vector Table Relocation */
	//SCB->VTOR = Address;

	/* Jump to the user App Reset Handler */
	App_ResetHandler();
}

//void JumpToApp(uint32_t appAddress)
//{
//	typedef void (*pFunc)(void);
//	pFunc appResetHandler;
//
//    // 1. Deinitialize all peripherals initialized by the bootloader
//    //HAL_DeInit();
//
//    // 2. Disable all interrupts
//    __disable_irq();
//    SysTick->CTRL = 0;
//    SysTick->LOAD = 0;
//    SysTick->VAL  = 0;
//
//    // 3. Set Main Stack Pointer (MSP) from the value at the application base
//    uint32_t mspValue = *(volatile uint32_t*)appAddress;
//    __set_MSP(mspValue);
//
//    // 4. Relocate vector table to the application's vector table base
//    __DSB();  // ensure completion of memory operations
//    __ISB();  // flush pipeline
//    //SCB->VTOR = (appAddress & SCB_VTOR_TBLOFF_Msk);
//
//    // 5. Retrieve the application's Reset_Handler address
//    uint32_t resetHandlerAddr = *(volatile uint32_t*)(appAddress + 4U);
//    appResetHandler = (pFunc)resetHandlerAddr;
//
//    // 6. Jump to the application's Reset_Handler (this function does not return)
//    appResetHandler();
//}


void StartFirmwareReception(void)
{
    /* Dest region and reset state */
    DestAddGlobal = (MetaData.active_app==1U)?FLASH_APP2_START:FLASH_APP1_START;
    SizeReceived   = 0;
    ChunkIndex     = 0;
    ChunkOffset    = 0;

	/* ACK bootloader ready for update */
    SendAck(START_UPDATE);
}

void HandleFrame(const CAN_RxHeaderTypeDef*RxHeader,const uint8_t*Data)
{
    if (SizeReceived == 0)
    {
        /* first frame: data[0] = number of 1KB pages */
        TotalChunks = Data[0];
        if (TotalChunks==0 || TotalChunks > APP_NUM_PAGE)
        {
            SendAck(CODE_ERROR);
        }
        else
        {
        	if (EraseAppRegion(DestAddGlobal)!=0)
        	{
        		SendAck(CODE_ERROR);
        	}
        	else
        	{
        		SizeReceived = 1;
        		SendAck(CODE_NEXT);
        	}
        }
    }
    else
    {
        /* receiving chunk data */
    	uint32_t Count;
        for (Count = 0 ; Count < RxHeader->DLC && ChunkOffset < FLASH_PAGE_SIZE ; Count++)
            ChunkBuffer[ChunkOffset++] = Data[Count];

        if (ChunkOffset >= FLASH_PAGE_SIZE)
        {
            uint32_t Address = DestAddGlobal + (ChunkIndex * FLASH_PAGE_SIZE);
            if (WriteChunkToFlash(Address,ChunkBuffer)==0)
            {
                ChunkIndex++;
                ChunkOffset=0;
                /* if more chunks needed? */
                if (ChunkIndex < TotalChunks)
                {
                	SendAck(CODE_NEXT);
                }
                else
                {
                    /* done */
                    MetaData.active_app = (MetaData.active_app==1U)?2U:1U;
                    MetaData.version++;
                    MetaData.pending=0U;
                    WriteMetadata();
                    SendAck(CODE_DONE);
                    JumpToApp((MetaData.active_app==1U)?FLASH_APP1_START:FLASH_APP2_START);
                }
            }
            else
            {
                SendAck(CODE_ERROR);
                /* keep chunkOffset to retry */
            }
        }
    }
}

uint8_t EraseAppRegion(uint32_t PageAddr)
{
	uint8_t LocalStatus = 0;
    FLASH_EraseInitTypeDef EraseInit;
    uint32_t PageError;
    EraseInit.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInit.PageAddress = PageAddr;
    EraseInit.NbPages     = APP_NUM_PAGE;

    /* Unlock flash */
    HAL_FLASH_Unlock();

    if(HAL_FLASHEx_Erase(&EraseInit,&PageError)!=HAL_OK)
    {
    	LocalStatus = -1;
    }

    /* Lock flash */
    HAL_FLASH_Lock();

    return LocalStatus;
}

uint8_t  WriteChunkToFlash(uint32_t Address,const uint8_t*Buffer)
{
	uint8_t LocalStatus = 0;
	uint32_t Count;

    /* Unlock flash */
    HAL_FLASH_Unlock();

    for (Count =0; Count < FLASH_PAGE_SIZE; Count += 2)
    {
        uint16_t Data = Buffer[Count] | (Buffer[Count+1] << 8);
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,Address,Data)!=HAL_OK)
        {
        	LocalStatus = -1;
        	break;
        }
        Address +=2;
    }

    /* Lock flash */
    HAL_FLASH_Lock();

    return LocalStatus;
}

void SendAck(uint16_t Code)
{
    CAN_TxHeaderTypeDef TxHeader = {0};
    uint8_t Data[8] = {0};
    uint32_t TxMailbox;

    /** Set CAN header for ACK message **/
    TxHeader.StdId = UPDATE_ACK_ID;
    TxHeader.IDE   = CAN_ID_STD;
    TxHeader.RTR   = CAN_RTR_DATA;
    TxHeader.DLC   = 2;  /** Sending 2 bytes: LSB + MSB of Code **/

    /** Split the 16-bit code into 2 bytes (Little Endian) **/
    Data[0] = (uint8_t)(Code & 0xFF);        /** LSB **/
    Data[1] = (uint8_t)((Code >> 8) & 0xFF); /** MSB **/

    /** Send CAN message **/
    HAL_CAN_AddTxMessage(&hcan, &TxHeader, Data, &TxMailbox);
}

/* CAN RX interrupt: dispatch based on message ID */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t Data[8];
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, Data);

    if (RxHeader.StdId == UPDATE_FRAME_ID)
    {
    	HandleFrame(&RxHeader,Data);
    }
}

