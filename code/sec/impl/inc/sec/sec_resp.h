/*==================================================================================================
    HEADER NAME : sec_resp.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
    wap_cm.h
    wap_tmdf.h
    
    GENERAL DESCRIPTION
        In this file,define the response function prototype.
    
    TECHFAITH Wireless Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-06-29   Kevin Yang        None         Init
==================================================================================================*/
/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/

#ifndef _SEC_RESP_H
#define _SEC_RESP_H

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
#define    G_SESSIONCLEAR_RESP              0x28
#define    G_GETPUBKEY_RESP                 0x2B

/*******************************************************************************
*   Type Define Section
*******************************************************************************/

typedef struct tagSt_SecSessionClearResp
{
    WE_INT32  iTargetID; 
    WE_UINT16 usResult;
}St_SecSessionClearResp;

typedef struct tagSt_GetPubkeyResp
{
    WE_INT32   iTargetID;
    WE_UINT8 * pucPubKey;
    WE_UINT8 * pucPubKeySig;
    WE_UINT16  usResult; 
    WE_UINT16  usPubKeyLen;
    WE_UINT16  usPubKeySigLen;
}St_GetPubkeyResp;

typedef struct tagSt_CallBackData
{
    WE_HANDLE hSecHandle;
    WE_INT32    iEvent;
    WE_VOID    *pvData;
}St_CallBackData;


/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
#ifdef M_SEC_CFG_CAN_SIGN_TEXT 

void Sec_SignTextResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iSignId, WE_INT32 iAlgorithm,
                      const WE_CHAR * pucSignature, WE_INT32 iSigLen,
                      const WE_CHAR * pucHashedKey, WE_INT32 iHashedKeyLen,
                      const WE_CHAR * pucCertificate, WE_INT32 iCertificateLen,
                      WE_INT32 iCertificateType, WE_INT32 iErr);
#endif 

void Sec_SearchPeerResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult,
                        WE_UINT8 ucConnectionType,WE_INT32 iMasterSecretID,
                        WE_INT32 iSecurityID);

void Sec_SessionGetResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult,
                        WE_UINT8 ucSessionOptions, const WE_UCHAR * pucSessionId,
                        WE_UINT8 ucSessionIdLen, WE_UINT8 *ucCipherSuite,
                        WE_UINT8 ucCompressionAlg, const WE_UCHAR * pucPrivateKeyId,
                        WE_UINT32 uiCreationTime);


void Sec_SaveCertResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult);

void Sec_TrustedCAInfoResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult);

void Sec_CertDeliveryResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult);

void Sec_GetCertNameListResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult);

/* modify by Sam[070119] */
/*#if( defined(M_SEC_CFG_WTLS_CLASS_3) ||defined(M_SEC_CFG_CAN_SIGN_TEXT) )*/
void Sec_KeyPairGenResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult, WE_UINT8 ucKeyType, 
                        const WE_UCHAR * pucPublicKey, WE_INT32 iPublicKeyLen, 
                        const WE_UCHAR * pucSig, WE_INT32 iSigLen);
void Sec_DelKeysUserCertsByTypeResponse(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult);
void Sec_GetPubKeyResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult, 
                      const WE_UCHAR * pucPublicKey, WE_INT32 iPublicKeyLen,
                      const WE_UCHAR * pucSig, WE_INT32 iSigLen);
void Sec_ModifyPinResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult);
/*#endif */

void Sec_GetCurSvrCertResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult);

void Sec_GetSessionInfoResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult);


void Sec_WtlsCurClassResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iSecClass,
                         WE_INT32 iInfoAvailable, WE_INT32 iCertAvailable);

void Sec_ClrSessionResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult);

#if( (defined(M_SEC_CFG_CAN_SIGN_TEXT) && defined(M_SEC_CFG_STORE_CONTRACTS)))

void Sec_GetContractsListResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult);
void Sec_RemoveContractResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult);
#endif 



void Sec_ViewAndGetCertResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult, 
                            WE_INT32 certId, WE_UCHAR * pucCert, WE_INT32 iCertLen);

void Sec_DeleteCertResponse(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iCertId, WE_INT32 iResult);


void Sec_WtlsGetCipherSuiteResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult,
                                const WE_UCHAR * pucCipherMethods, WE_INT32 iCipherMethodsLen,
                                const WE_UCHAR * pucKeyExchangeIds, WE_INT32 iKeyExchangeIdsLen,
                                const WE_UCHAR * pucTrustedKeyIds, WE_INT32 iTrustedKeyIdsLen);

void Sec_WtlsKeyExchangeResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult,
                             WE_INT32 iMasterSecretId, const WE_UCHAR * pucPublicValue,
                             WE_INT32 iPublicValueLen);

void Sec_WtlsGetPrfResultResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult,
                              const WE_UCHAR * pucBuf, WE_INT32 iBufLen);

void Sec_WtlsVerifySvrCertResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult);

void Sec_WtlsGetUsrCertResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult,
                            const WE_UCHAR * pucKeyId, WE_INT32 iKeyIdLen,
                            const WE_UCHAR * pucCert, WE_INT32 iCertLen);

void Sec_WtlsCompSigResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult,
                         const WE_UCHAR * pucSignature, WE_INT32 iSignatureLen);
void Sec_UserCertReqResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult, 
                         WE_UCHAR* pucCertReqMsg, WE_UINT32 uiCertReqMsgLen);
void Sec_ChangeWTLSCertAbleResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                        WE_INT32 iResult, WE_UINT8 ucState);

WE_VOID Sec_ReleaseRespCB(WE_VOID* pvData);

#endif
