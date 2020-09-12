/**********************************************************************************************
 * Filename:       doorLockService.c
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

#include "doorLockService.h"

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

// doorLockService Service UUID
CONST uint8_t doorLockServiceUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(DOORLOCKSERVICE_SERV_UUID)
};

// doorLockState UUID
CONST uint8_t doorLockService_DoorLockStateUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(DOORLOCKSERVICE_DOORLOCKSTATE_UUID)
};

/*********************************************************************
 * LOCAL VARIABLES
 */

static doorLockServiceCBs_t *pAppCBs = NULL;

/*********************************************************************
* Profile Attributes - variables
*/

// Service declaration
static CONST gattAttrType_t doorLockServiceDecl = { ATT_UUID_SIZE, doorLockServiceUUID };

// Characteristic "DoorLockState" Properties (for declaration)
static uint8_t doorLockService_DoorLockStateProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic "DoorLockState" Value variable
static uint8_t doorLockService_DoorLockStateVal[DOORLOCKSERVICE_DOORLOCKSTATE_LEN] = {0};

/*********************************************************************
* Profile Attributes - Table
*/

static gattAttribute_t doorLockServiceAttrTbl[] =
{
  // doorLockService Service Declaration
  {
    { ATT_BT_UUID_SIZE, primaryServiceUUID },
    GATT_PERMIT_READ,
    0,
    (uint8_t *)&doorLockServiceDecl
  },
    // DoorLockState Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &doorLockService_DoorLockStateProps
    },
      // DoorLockState Characteristic Value
      {
        { ATT_UUID_SIZE, doorLockService_DoorLockStateUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        doorLockService_DoorLockStateVal
      },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t doorLockService_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                           uint16_t maxLen, uint8_t method );
static bStatus_t doorLockService_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                            uint8_t *pValue, uint16_t len, uint16_t offset,
                                            uint8_t method );

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Simple Profile Service Callbacks
CONST gattServiceCBs_t doorLockServiceCBs =
{
  doorLockService_ReadAttrCB,  // Read callback function pointer
  doorLockService_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
* PUBLIC FUNCTIONS
*/

/*
 * DoorLockService_AddService- Initializes the DoorLockService service by registering
 *          GATT attributes with the GATT server.
 *
 */
extern bStatus_t DoorLockService_AddService( uint8_t rspTaskId )
{
  uint8_t status;

  // Register GATT attribute list and CBs with GATT Server App
  status = GATTServApp_RegisterService( doorLockServiceAttrTbl,
                                        GATT_NUM_ATTRS( doorLockServiceAttrTbl ),
                                        GATT_MAX_ENCRYPT_KEY_SIZE,
                                        &doorLockServiceCBs );

  return ( status );
}

/*
 * DoorLockService_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
bStatus_t DoorLockService_RegisterAppCBs( doorLockServiceCBs_t *appCallbacks )
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
 * DoorLockService_SetParameter - Set a DoorLockService parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
bStatus_t DoorLockService_SetParameter( uint8_t param, uint16_t len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case DOORLOCKSERVICE_DOORLOCKSTATE_ID:
      if ( len == DOORLOCKSERVICE_DOORLOCKSTATE_LEN )
      {
        memcpy(doorLockService_DoorLockStateVal, value, len);
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
 * DoorLockService_GetParameter - Get a DoorLockService parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
bStatus_t DoorLockService_GetParameter( uint8_t param, uint16_t *len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case DOORLOCKSERVICE_DOORLOCKSTATE_ID:
      memcpy(value, doorLockService_DoorLockStateVal, DOORLOCKSERVICE_DOORLOCKSTATE_LEN);
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }
  return ret;
}


/*********************************************************************
 * @fn          doorLockService_ReadAttrCB
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
static bStatus_t doorLockService_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                       uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                       uint16_t maxLen, uint8_t method )
{
  bStatus_t status = SUCCESS;

  // See if request is regarding the DoorLockState Characteristic Value
if ( ! memcmp(pAttr->type.uuid, doorLockService_DoorLockStateUUID, pAttr->type.len) )
  {
    if ( offset > DOORLOCKSERVICE_DOORLOCKSTATE_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, DOORLOCKSERVICE_DOORLOCKSTATE_LEN - offset);  // Transmit as much as possible
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
 * @fn      doorLockService_WriteAttrCB
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
static bStatus_t doorLockService_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
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
                                             offset, GATT_CLIENT_CFG_NOTIFY);
  }
  // See if request is regarding the DoorLockState Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, doorLockService_DoorLockStateUUID, pAttr->type.len) )
  {
    if ( offset + len > DOORLOCKSERVICE_DOORLOCKSTATE_LEN )
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      // Copy pValue into the variable we point to from the attribute table.
      memcpy(pAttr->pValue + offset, pValue, len);

      // Only notify application if entire expected value is written
      if ( offset + len == DOORLOCKSERVICE_DOORLOCKSTATE_LEN)
        paramID = DOORLOCKSERVICE_DOORLOCKSTATE_ID;
    }
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
      uint16_t svcUuid = DOORLOCKSERVICE_SERV_UUID;
      pAppCBs->pfnChangeCb(connHandle, svcUuid, paramID, len, pValue); // Call app function from stack task context.
    }
  return status;
}
