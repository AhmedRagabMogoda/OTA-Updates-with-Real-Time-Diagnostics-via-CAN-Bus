/*
 *  UDS.c
 *
 *  Created on: May 8, 2025
 *  Author: Ahmed Ragab
 */

#include "UDS.h"

/* Current UDS session (default, sensor, control, programming) */
static uint8_t CurrentSession = SESSION_DEFAULT;

/* Flag indicating whether security access has been unlocked */
static uint8_t SecurityUnlocked = 0;

/* External FreeRTOS queues and latest sensor values */
extern QueueHandle_t xCanTxQueue;   /* CAN transmit queue */
extern QueueHandle_t xOtaQueue;     /* OTA command queue */
extern float LatestTemp;            /* Most recent temperature reading */
extern float LatestDist;            /* Most recent distance reading */

/**
 * SendResponse
 */
void SendResponse(uint8_t *Data, uint8_t DLC)
{
    CanTxMessage_t Msg;
    uint8_t Count;
    Msg.StdId = UDS_RESPONSE_ID;
    Msg.DLC   = DLC;

    for (Count = 0; Count < DLC; Count++) Msg.Data[Count] = Data[Count];
    for (Count = DLC; Count < 8; Count++) Msg.Data[Count] = 0;
    xQueueSend(xCanTxQueue, &Msg, portMAX_DELAY);
}

/**
 * SendNegative
 */
void SendNegative(uint8_t SID, uint8_t NRC)
{
    uint8_t Resp[3] = {0x7F, SID, NRC};
    SendResponse(Resp, 3);
}

/**
 * SendPositive
 */
void SendPositive(uint8_t SID, uint8_t Sub)
{
    uint8_t Resp[2] = {(uint8_t)(SID + 0x40), Sub};
    SendResponse(Resp, 2);
}

/**
 * ProcessSessionControl
 */
void ProcessSessionControl(uint8_t Sub)
{
    if (Sub <= SESSION_SENSOR)
    {
        /* Allowed sessions in default or sensor mode */
        CurrentSession = Sub;
        SendPositive(SID_DIAG_SESSION_CTRL, Sub);
    }
    else if (Sub == SESSION_CONTROL || Sub == SESSION_PROGRAMMING)
    {
    	if(SecurityUnlocked == 1)
    	{
            CurrentSession = Sub;
            SendPositive(SID_DIAG_SESSION_CTRL, Sub);
    	}
    	else
    	{
            /* Control/programming require security */
            SendNegative(SID_DIAG_SESSION_CTRL, NRC_SECURITY_REQUIRED);
    	}
    }
    else
    {
        /* Unsupported session ID */
        SendNegative(SID_DIAG_SESSION_CTRL, NRC_SERVICE_NOT_SUPP);
    }
}

/**
 * ProcessSecurityAccess
 */
void ProcessSecurityAccess(uint8_t Sub, uint8_t *Data, uint8_t DLC)
{
    if (Sub == 0x00 && DLC >= 4)
    {
        /* Extract 16-bit password from bytes 2 and 3 */
        uint16_t Password = (Data[2] << 8) | Data[3];
        if (Password == DIAG_PASSWORD)
        {
            SecurityUnlocked = 1;
            SendPositive(SID_SECURITY_ACCESS, Sub);
        }
        else
        {
            SendNegative(SID_SECURITY_ACCESS, NRC_SECURITY_DENIED);
        }
    }
    else
    {
        /* Service not supported for other sub-functions */
        SendNegative(SID_SECURITY_ACCESS, NRC_SERVICE_NOT_SUPP);
    }
}

/**
 * ProcessReadDataByIdr
 */
void ProcessReadDataById(uint8_t Sub, uint8_t *Data)
{
    if (CurrentSession == SESSION_DEFAULT || CurrentSession == SESSION_SENSOR)
    {

        uint8_t Response[6];

        /* Build positive response header */
        Response[0] = SID_READ_DATA_BY_ID + 0x40;
        Response[1] = Sub;

        if (Sub == 0x01)
        {
            /* Report latest temperature as two bytes */
            uint16_t Temp = (uint16_t)LatestTemp;
            Response[2] = (uint8_t)(Temp & 0xFF);
            Response[3] = (uint8_t)(Temp >> 8);
            SendResponse(Response, 4);
        }
        else if (Sub == 0x02)
        {
            /* Report latest distance as two bytes */
            uint16_t Dist = (uint16_t)LatestDist;
            Response[2] = (uint8_t)(Dist & 0xFF);
            Response[3] = (uint8_t)(Dist >> 8);
            SendResponse(Response, 4);
        }
        else
        {
            /* Unsupported DID */
            SendNegative(SID_READ_DATA_BY_ID, NRC_SERVICE_NOT_SUPP);
        }
    }
    else
    {
        /* Not allowed in current session */
        SendNegative(SID_READ_DATA_BY_ID, NRC_SERVICE_NOT_SUPP);
    }
}

/**
 * ProcessReadDTC
 */
void ProcessReadDTC(uint8_t Sub)
{
    if (CurrentSession == SESSION_CONTROL && SecurityUnlocked)
    {
        DTC_Entry_t Entries[MAX_DTC_CODES];
        uint8_t Count = ReadDTCListFromFlash(Entries, MAX_DTC_CODES);
        uint8_t DTCMsg[8]={0};

        /* First send number-of-DTCs response */
        DTCMsg[0] = SID_READ_DTC + 0x40;

        /* Now send each DTC entry in two sequenced frames */
        for (uint8_t Local_Count = 0; Local_Count < Count; Local_Count++)
        {

            /* build the 12-byte payload */
            DTCMsg[1] = (uint8_t)(Entries[Local_Count].code );
            DTCMsg[2] = (uint8_t)(Entries[Local_Count].code >> 8);
            DTCMsg[3] = (uint8_t)(Entries[Local_Count].code >> 16);
            DTCMsg[4] = (uint8_t)(Entries[Local_Count].code >> 24);
            uint32_t Date = Entries[Local_Count].datastamp;
            DTCMsg[5]  = (uint8_t)(Date);
            DTCMsg[6]  = (uint8_t)(Date >> 8);
            DTCMsg[7] = (uint8_t)(Date >> 16);
            SendResponse(DTCMsg, 8);
        }
    }
    else
    {
        SendNegative(SID_READ_DTC, NRC_SERVICE_NOT_SUPP);
    }
}

/**
 * ProcessClearDTC
 */
void ProcessClearDTC(uint8_t Sub)
{
    if (CurrentSession == SESSION_CONTROL && SecurityUnlocked)
    {
        /* Erase flash and send positive ack */
        ClearDTCListFlash();
        SendPositive(SID_CLEAR_DTC, Sub);
    }
    else
    {
        /* Not allowed */
        SendNegative(SID_CLEAR_DTC, NRC_SERVICE_NOT_SUPP);
    }
}

/**
 * ProcessRequestDownload
 */
void ProcessRequestDownload(uint8_t Sub, uint8_t* Msg)
{
    if (CurrentSession == SESSION_PROGRAMMING && SecurityUnlocked)
    {
        /* Enqueue OTA start command and reset MCU */
        xQueueSend(xOtaQueue, &Msg, 0);

    }
    else
    {
        /* Bootloader entry not allowed */
        SendNegative(SID_REQUEST_DOWNLOAD, NRC_SERVICE_NOT_SUPP);
    }
}
