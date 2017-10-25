/*=====================================================================================
    FILE NAME : sec_wimsp.h
    MODULE NAME : SEC
    
    GENERAL DESCRIPTION
        In this file, define the structure and functions used in the SEC module.

    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2002-2006 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
    
    Modification Tracking
    Date       Author         Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-07-22 Bird     none      Init
    
=====================================================================================*/

/***************************************************************************************************
*   multi-Include-Prevent Section
***************************************************************************************************/
#ifndef SEC_WIMSP_H
#define SEC_WIMSP_H

/***************************************************************************************************
*   macro Define Section
***************************************************************************************************/
#define M_FILE_LENGTH_BITS 4

/***************************************************************************************************
*   Type Define Section
***************************************************************************************************/
typedef struct tagSt_SecSession 
{
    WE_UINT32 uiCreationTime;
    WE_UINT8  ucSessionOptions;
    WE_UCHAR  aucSessionId[M_SEC_SESSION_ID_LEN]; 
    WE_UINT8  ucSessionIdLen;
    WE_UINT8  aucCipherSuite[2];
    WE_UINT8  ucCompressionAlg;
    WE_UCHAR  aucPrivateKeyId[M_SEC_PRIVATE_KEY_ID_LEN];
}St_SecSession;

typedef struct tagSt_SecSessionRec 
{
    St_SecSession stInfo;
    WE_UCHAR      aucMasterSecret[M_SEC_TLS_MASTER_SECRET_LEN];
    WE_UINT8      ucIsActive;
    WE_UINT8      ucSlotInUse;
}St_SecSessionRec;

typedef struct tagSt_SecPeerRec 
{
    WE_INT32  iConnectionType;
    WE_INT32  iAddrLen;
    WE_INT32  iMasterSecretId;
    WE_UINT16 usPortNum;
    WE_UINT8  ucSlotInUse;
    WE_UCHAR  aucAddress[M_SEC_ADDRESS_LEN];
}St_SecPeerRec ;


/***************************************************************************************************
*   Prototype Declare Section
***************************************************************************************************/
void Sec_WimGetMastSecret(WE_HANDLE hSecHandle, WE_INT32 iMasterSecretId, WE_UCHAR *pucMasterSecret, 
                          WE_INT32 iMasterSecretLen, WE_INT32 *piRes);

void Sec_WimSaveMastSecret(WE_HANDLE hSecHandle, WE_UCHAR *pucMasterSecret, WE_INT32 iMasterSecretLen, 
                           WE_INT32 *piMasterSecretId, WE_INT32 *piRes);

void Sec_WimClrSessionInfoRec(St_SecSession *pstInfo);

void Sec_WimClrSessionRec(St_SecSessionRec *pstRec);

void Sec_WimClrPeerRec(St_SecPeerRec *pstrec);

void Sec_WimClrSessionInfo(WE_HANDLE hSecHandle,WE_INT32 iIndex);

void Sec_WimClrPeerInfo(WE_HANDLE hSecHandle, WE_INT32 iIndex);

void Sec_WimSessionStart(WE_HANDLE hSecHandle);

void Sec_WimSessionStop(WE_HANDLE hSecHandle);

void Sec_WimPeerSearch(WE_HANDLE hSecHandle, WE_INT32 iWid, WE_UINT8 ucConnectionType,
                       const WE_UCHAR *pucAddress, WE_INT32 iAddrLen, WE_UINT16 usPortNum);

void Sec_WimPeerAttachSession(WE_HANDLE hSecHandle,WE_UINT8 uiConnectionType,
                              const WE_UCHAR *pucAddress, WE_INT32 iAddressLen,
                              WE_UINT16 usPortNum, WE_INT32 iMasterSecretId);

void Sec_WimPeerRemove (WE_HANDLE hSecHandle,WE_INT32 iMasterSecretId);

void Sec_WimSessionEnable(WE_HANDLE hSecHandle,WE_INT32 iMasterSecretId, WE_UINT8 ucIsActive);

void Sec_WimSessionDisable(WE_HANDLE hSecHandle,WE_INT32 iMasterSecretId);

void Sec_WimClrSession(WE_HANDLE hSecHandle);

void Sec_WimSessionSlotClr(WE_HANDLE hSecHandle, WE_INT32 iMasterSecretId);

void Sec_WimFetchSession(WE_HANDLE hSecHandle, WE_INT32 iWid, WE_INT32 iMasterSecretId);

void Sec_WimUpdateSession(WE_HANDLE hSecHandle, WE_INT32 iMasterSecretId, WE_UINT8 ucSessionOptions,
                          const WE_UCHAR *pucSessionId, WE_UINT8 ucSessionIdLen,
                          WE_UINT8 ucCipherSuite[2], WE_UINT8 ucCompressionAlg,
                          const WE_UCHAR *pucPrivateKeyId,WE_UINT32 uiCreationTime);

WE_INT32 Sec_WimGetSessionLength(WE_HANDLE hSecHandle,WE_HANDLE hFileHandle,
                                 WE_UINT32* puiSessionLength, WE_UINT32* uiPeerLength,
                                 WE_UINT32* uiViewLength,WE_LONG *plReadNum);
#endif 

