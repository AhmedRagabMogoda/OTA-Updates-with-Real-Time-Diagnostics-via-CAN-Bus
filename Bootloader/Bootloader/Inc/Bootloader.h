/*
 *  Bootloader.h
 *
 *  Created on: Apr 23, 2025
 *  Author: Ahmed Ragab
 */

#ifndef INC_BOOTLOADER_H_
#define INC_BOOTLOADER_H_


/* Flash memory layout parameters */
#define FLASH_BASE_ADDR        0x08000000U                       /* Start of flash */
//#define FLASH_PAGE_SIZE        0x00000400U                     /* 1 KB per flash page */
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

/* CAN message IDs */
#define UPDATE_FRAME_ID    0x200U   /* Pi -> STM32: size or data frames */
#define UPDATE_ACK_ID      0x201U   /* STM32 -> Pi: ACK/NACK frames */

/* ACK/NACK payload codes */
#define START_UPDATE       0xAB
#define CODE_NEXT          0xA5U     /* Request next chunk */
#define CODE_DONE          0x5AU     /* Update complete */
#define CODE_ERROR         0xFFU     /* Error, retry */


/** Metadata structure in flash ("Option Bytes") **/
typedef struct {
    uint32_t magic;       /** == METADATA_MAGIC if valid */
    uint32_t active_app;  /** 1 or 2 */
    uint32_t version;     /** firmware version counter */
    uint32_t pending;     /** 1 if update pending */
} metadata_t;


/** Prototypes **/
void ReadMetadata(void);
void WriteMetadata(void);
void JumpToApp(uint32_t address);
void StartFirmwareReception(void);
void HandleFrame(const CAN_RxHeaderTypeDef *hdr, const uint8_t *data);
uint8_t EraseAppRegion(uint32_t startAddr);
uint8_t WriteChunkToFlash(uint32_t pageAddr, const uint8_t *buf);
void SendAck(uint16_t Code);







#endif /* INC_BOOTLOADER_H_ */
