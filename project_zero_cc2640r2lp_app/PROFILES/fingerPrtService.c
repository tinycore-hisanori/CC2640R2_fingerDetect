/**********************************************************************************************
 * Filename:       fingerPrtService.c
 *
 * Description:    This file contains the implementation of the service.
 *
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *************************************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include <string.h>

#include <icall.h>

/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"

#include "fingerPrtService.h"

#include <uartlog/UartLog.h>

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
* GLOBAL VARIABLES
*/

// fingerPrtService Service UUID
CONST uint8_t fingerPrtServiceUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(FINGERPRTSERVICE_SERV_UUID)
};

// charaData1 UUID
CONST uint8_t fingerPrtService_CharaData1UUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(FINGERPRTSERVICE_CHARADATA1_UUID)
};
// charaData2 UUID
CONST uint8_t fingerPrtService_CharaData2UUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(FINGERPRTSERVICE_CHARADATA2_UUID)
};
// charaData3 UUID
CONST uint8_t fingerPrtService_CharaData3UUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(FINGERPRTSERVICE_CHARADATA3_UUID)
};
// evtDetect UUID
CONST uint8_t fingerPrtService_EvtDetectUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(FINGERPRTSERVICE_EVTDETECT_UUID)
};

/*********************************************************************
 * LOCAL VARIABLES
 */

static fingerPrtServiceCBs_t *pAppCBs = NULL;

/*********************************************************************
* Profile Attributes - variables
*/

// Service declaration
static CONST gattAttrType_t fingerPrtServiceDecl = { ATT_UUID_SIZE, fingerPrtServiceUUID };

// Characteristic "CharaData1" Properties (for declaration)
static uint8_t fingerPrtService_CharaData1Props = GATT_PROP_READ;

// Characteristic "CharaData1" Value variable
static uint8_t fingerPrtService_CharaData1Val[FINGERPRTSERVICE_CHARADATA1_LEN] = {0};
// Characteristic "CharaData2" Properties (for declaration)
static uint8_t fingerPrtService_CharaData2Props = GATT_PROP_READ;

// Characteristic "CharaData2" Value variable
static uint8_t fingerPrtService_CharaData2Val[FINGERPRTSERVICE_CHARADATA2_LEN] = {0};
// Characteristic "CharaData3" Properties (for declaration)
static uint8_t fingerPrtService_CharaData3Props = GATT_PROP_READ;

// Characteristic "CharaData3" Value variable
static uint8_t fingerPrtService_CharaData3Val[FINGERPRTSERVICE_CHARADATA3_LEN] = {0};
// Characteristic "EvtDetect" Properties (for declaration)
static uint8_t fingerPrtService_EvtDetectProps = GATT_PROP_READ | GATT_PROP_NOTIFY;

// Characteristic "EvtDetect" Value variable
static uint8_t fingerPrtService_EvtDetectVal[FINGERPRTSERVICE_EVTDETECT_LEN] = {0};

// Characteristic "EvtDetect" CCCD
static gattCharCfg_t *fingerPrtService_EvtDetectConfig;

/*********************************************************************
* Profile Attributes - Table
*/

static gattAttribute_t fingerPrtServiceAttrTbl[] =
{
  // fingerPrtService Service Declaration
  {
    { ATT_BT_UUID_SIZE, primaryServiceUUID },
    GATT_PERMIT_READ,
    0,
    (uint8_t *)&fingerPrtServiceDecl
  },
    // CharaData1 Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &fingerPrtService_CharaData1Props
    },
      // CharaData1 Characteristic Value
      {
        { ATT_UUID_SIZE, fingerPrtService_CharaData1UUID },
        GATT_PERMIT_READ,
        0,
        fingerPrtService_CharaData1Val
      },
    // CharaData2 Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &fingerPrtService_CharaData2Props
    },
      // CharaData2 Characteristic Value
      {
        { ATT_UUID_SIZE, fingerPrtService_CharaData2UUID },
        GATT_PERMIT_READ,
        0,
        fingerPrtService_CharaData2Val
      },
    // CharaData3 Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &fingerPrtService_CharaData3Props
    },
      // CharaData3 Characteristic Value
      {
        { ATT_UUID_SIZE, fingerPrtService_CharaData3UUID },
        GATT_PERMIT_READ,
        0,
        fingerPrtService_CharaData3Val
      },
    // EvtDetect Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &fingerPrtService_EvtDetectProps
    },
      // EvtDetect Characteristic Value
      {
        { ATT_UUID_SIZE, fingerPrtService_EvtDetectUUID },
        GATT_PERMIT_READ,
        0,
        fingerPrtService_EvtDetectVal
      },
      // EvtDetect CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8 *)&fingerPrtService_EvtDetectConfig
      },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t fingerPrtService_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                           uint16_t maxLen, uint8_t method );
static bStatus_t fingerPrtService_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                            uint8_t *pValue, uint16_t len, uint16_t offset,
                                            uint8_t method );

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Simple Profile Service Callbacks
CONST gattServiceCBs_t fingerPrtServiceCBs =
{
  fingerPrtService_ReadAttrCB,  // Read callback function pointer
  fingerPrtService_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
* PUBLIC FUNCTIONS
*/

/*
 * FingerPrtService_AddService- Initializes the FingerPrtService service by registering
 *          GATT attributes with the GATT server.
 *
 */
extern bStatus_t FingerPrtService_AddService( uint8_t rspTaskId )
{
  uint8_t status;

  // Allocate Client Characteristic Configuration table
  fingerPrtService_EvtDetectConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( fingerPrtService_EvtDetectConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( LINKDB_CONNHANDLE_INVALID, fingerPrtService_EvtDetectConfig );
  // Register GATT attribute list and CBs with GATT Server App
  status = GATTServApp_RegisterService( fingerPrtServiceAttrTbl,
                                        GATT_NUM_ATTRS( fingerPrtServiceAttrTbl ),
                                        GATT_MAX_ENCRYPT_KEY_SIZE,
                                        &fingerPrtServiceCBs );

  return ( status );
}

/*
 * FingerPrtService_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
bStatus_t FingerPrtService_RegisterAppCBs( fingerPrtServiceCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    pAppCBs = appCallbacks;

    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}


/*
 * FingerPrtService_SetParameter - Set a FingerPrtService parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
bStatus_t FingerPrtService_SetParameter( uint8_t param, uint16_t len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case FINGERPRTSERVICE_CHARADATA1_ID:
      if ( len <= FINGERPRTSERVICE_CHARADATA1_LEN )
      {
        memcpy(fingerPrtService_CharaData1Val, value, len);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case FINGERPRTSERVICE_CHARADATA2_ID:
      if ( len <= FINGERPRTSERVICE_CHARADATA2_LEN )
      {
        memcpy(fingerPrtService_CharaData2Val, value, len);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case FINGERPRTSERVICE_CHARADATA3_ID:
      if ( len <= FINGERPRTSERVICE_CHARADATA3_LEN )
      {
        memcpy(fingerPrtService_CharaData3Val, value, len);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case FINGERPRTSERVICE_EVTDETECT_ID:
      if ( len <= FINGERPRTSERVICE_EVTDETECT_LEN )
      {
        memcpy(fingerPrtService_EvtDetectVal, value, len);

        //Log_info0("FINGER\r\n");
        // Try to send notification.
        GATTServApp_ProcessCharCfg( fingerPrtService_EvtDetectConfig, (uint8_t *)&fingerPrtService_EvtDetectVal, FALSE,
                                    fingerPrtServiceAttrTbl, GATT_NUM_ATTRS( fingerPrtServiceAttrTbl ),
                                    INVALID_TASK_ID,  fingerPrtService_ReadAttrCB);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }
  return ret;
}


/*
 * FingerPrtService_GetParameter - Get a FingerPrtService parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
bStatus_t FingerPrtService_GetParameter( uint8_t param, uint16_t *len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  return ret;
}


/*********************************************************************
 * @fn          fingerPrtService_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      SUCCESS, blePending or Failure
 */
static bStatus_t fingerPrtService_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                       uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                       uint16_t maxLen, uint8_t method )
{
  bStatus_t status = SUCCESS;

  // See if request is regarding the CharaData1 Characteristic Value
if ( ! memcmp(pAttr->type.uuid, fingerPrtService_CharaData1UUID, pAttr->type.len) )
  {
    if ( offset > FINGERPRTSERVICE_CHARADATA1_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, FINGERPRTSERVICE_CHARADATA1_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  // See if request is regarding the CharaData2 Characteristic Value
else if ( ! memcmp(pAttr->type.uuid, fingerPrtService_CharaData2UUID, pAttr->type.len) )
  {
    if ( offset > FINGERPRTSERVICE_CHARADATA2_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, FINGERPRTSERVICE_CHARADATA2_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  // See if request is regarding the CharaData3 Characteristic Value
else if ( ! memcmp(pAttr->type.uuid, fingerPrtService_CharaData3UUID, pAttr->type.len) )
  {
    if ( offset > FINGERPRTSERVICE_CHARADATA3_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, FINGERPRTSERVICE_CHARADATA3_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  // See if request is regarding the EvtDetect Characteristic Value
else if ( ! memcmp(pAttr->type.uuid, fingerPrtService_EvtDetectUUID, pAttr->type.len) )
  {
    if ( offset > FINGERPRTSERVICE_EVTDETECT_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, FINGERPRTSERVICE_EVTDETECT_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  else
  {
    // If we get here, that means you've forgotten to add an if clause for a
    // characteristic value attribute in the attribute table that has READ permissions.
    *pLen = 0;
    status = ATT_ERR_ATTR_NOT_FOUND;
  }

  return status;
}


/*********************************************************************
 * @fn      fingerPrtService_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 */
static bStatus_t fingerPrtService_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                        uint8_t *pValue, uint16_t len, uint16_t offset,
                                        uint8_t method )
{
  bStatus_t status  = SUCCESS;
  uint8_t   paramID = 0xFF;



  // See if request is regarding a Client Characterisic Configuration
  if ( ! memcmp(pAttr->type.uuid, clientCharCfgUUID, pAttr->type.len) )
  {
  // Allow only notifications.
    status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                             offset, GATT_CLIENT_CFG_NOTIFY|GATT_CLIENT_CFG_INDICATE );
  }
  else
  {
    // If we get here, that means you've forgotten to add an if clause for a
    // characteristic value attribute in the attribute table that has WRITE permissions.
    status = ATT_ERR_ATTR_NOT_FOUND;
  }

  // Let the application know something changed (if it did) by using the
  // callback it registered earlier (if it did).
  if (paramID != 0xFF)
    if ( pAppCBs && pAppCBs->pfnChangeCb )
    {
      uint16_t svcUuid = FINGERPRTSERVICE_SERV_UUID;
      pAppCBs->pfnChangeCb(connHandle, svcUuid, paramID, len, pValue); // Call app function from stack task context.
    }
  return status;
}
