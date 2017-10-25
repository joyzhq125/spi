/*==================================================================================================
    HEADER NAME : sec_ueresp.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
 
    GENERAL DESCRIPTION
        In this file,define the function prototype 
    
    Techfaith Software Confidential Proprietary
    (c) Copyright 2006 by Techfaith Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-07-21   Tang             None         Init
==================================================================================================*/



/************************************************************************/
/*       Multi-Include-Prevent Section                                                                     */
/************************************************************************/
#ifndef SEC_UERESP_H
#define SEC_UERESP_H 

/************************************************************************/
/*      Prototype Declare Section                                                                     */
/************************************************************************/

void Sec_UeConfirmDialogResp(WE_HANDLE hSecHandle, WE_UINT8 ucAnswer);
void Sec_UeExtConfirmDialogResp(WE_HANDLE hSecHandle, WE_UINT8 ucAnswer);
void Sec_UeChooseCertByNameResp(WE_HANDLE hSecHandle, 
                                WE_INT32 iCertId, 
                                E_SecUeReturnVal eResult);
void Sec_UeGenPinResp(WE_HANDLE hSecHandle, 
                      E_SecUeReturnVal eResult,
                      const WE_CHAR *pcPin);
void Sec_UeChangePinDialogResp(WE_HANDLE hSecHandle, E_SecUeReturnVal eResult);
void Sec_UeChangePin(WE_HANDLE hSecHandle, WE_UINT8 ucKeyType, 
                     const WE_CHAR *pcOldPin, const WE_CHAR *pcNewPin);
void Sec_UePinDialogResp(WE_HANDLE hSecHandle, E_SecUeReturnVal eResult);
void Sec_UeVerifyPin(WE_HANDLE hSecHandle, WE_UINT8 ucKeyType, const WE_CHAR *pcPin);
void Sec_UeAllowDelKeysResp(WE_HANDLE hSecHandle, E_SecUeReturnVal eResult);
void Sec_UeStoreCertDialogResp(WE_HANDLE hSecHandle,
                               WE_UINT8 ucAnswer, 
                               void *pvFriendlyName, 
                               WE_UINT16 usFriendlyNameLen, 
                               WE_INT16 sCharSet);
void Sec_UeVerifyHash(WE_HANDLE hSecHandle, WE_UINT8 *pucHash);
void Sec_UeHashDialogResp(WE_HANDLE hSecHandle, E_SecUeReturnVal eResult);
void Sec_UeConfirmDispNameResp(WE_HANDLE hSecHandle, WE_UINT8 ucAnswer);

#endif


/*------------------------------------------END--------------------------------------------------*/
