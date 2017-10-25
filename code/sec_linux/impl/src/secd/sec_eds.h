/*=====================================================================================
    FILE NAME :
        Sec_eds.h
    MODULE NAME :
        secsic
    GENERAL DESCRIPTION

    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2002-2006 by TECHFAITH Software. All Rights Reserved.
    =======================================================================================
    Revision History
    Modification              Tracking
    Date       Author         Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2007-03-20 Bird           none      Init
  
=====================================================================================*/


/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/

#ifndef Sec_EDS_H
#define Sec_EDS_H

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/

#define SEC_ED_MSG_HEADER_LENGTH  5
#define SEC_ED_MSG_TYPE_LENGTH    1    /* Msg type length 1 byte */

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
WE_UINT8 Sec_GetMsgType(WE_VOID *pvBuf);
WE_INT32 Sec_GetMsgLen(WE_VOID *pvBuf);

WE_INT32 Sec_EncodeSignText(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID,
                        WE_INT32 iSignId, WE_INT32 iAlgorithm,
                      const WE_CHAR * pcSignature, WE_INT32 iSigLen,
                      const WE_CHAR * pcHashedKey, WE_INT32 iHashedKeyLen,
                      const WE_CHAR * pcCertificate, WE_INT32 iCertificateLen,
                      WE_INT32 iCertificateType, WE_INT32 iErr);
WE_INT32 Sec_EncodeWtlsGetPrfResult(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult,
                              const WE_UCHAR * pucBuf, WE_INT32 iBufLen);
WE_INT32 Sec_EncodeWtlsVerifySvrCert(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult);
WE_INT32 Sec_EncodeWtlsGetUsrCert(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult,
                            const WE_UCHAR * pucKeyId, WE_INT32 iKeyIdLen,
                            const WE_UCHAR * pucCert, WE_INT32 iCertLen);
WE_INT32 Sec_EncodeTlsGetUsrCert(WE_UINT8 **ppucBuffer,  WE_INT32 iTargetID, WE_INT32 iResult,
                        const WE_UCHAR * pucPubkeyHash, WE_INT32 usPubKeyHashLen,
                        St_SecTlsAsn1Certificate * pstCert, WE_INT32 iNbrCerts);
WE_INT32 Sec_EncodeTlsGetCipherSuite(WE_UINT8 **ppucBuffer , WE_INT32 iTargetID, WE_UINT16 usResult,
                               const WE_UCHAR * pucCipherSuites, WE_UINT16 usCipherSuitesLen);
WE_INT32 Sec_EncodeSSLGetMasterSecret(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult,
                         const WE_UCHAR * pucMasterSecret, WE_INT32 iMasterSecretLen);
WE_INT32 Sec_EncodeWtlsGetCipherSuite(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_UINT16 usResult,
                                const WE_UCHAR * pucCipherMethods, WE_UINT16 usCipherMethodsLen,
                                const WE_UCHAR * pucKeyExchangeIds, WE_UINT16 usKeyExchangeIdsLen,
                                const WE_UCHAR * pucTrustedKeyIds, WE_UINT16 usTrustedKeyIdsLen);
WE_INT32 Sec_EncodeWtlsKeyExchange(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult,
                             WE_INT32 iMasterSecretId, const WE_UCHAR * pucPublicValue,
                             WE_INT32 iPublicValueLen);
WE_INT32 Sec_EncodeWtlsCompSig(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult,
                         const WE_UCHAR * pucSignature, WE_INT32 iSignatureLen);
WE_INT32 Sec_EncodeSearchPeer(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult,
                        WE_UINT8 ucConnectionType,WE_INT32 iMasterSecretID,
                        WE_INT32 iSecurityID);
WE_INT32 Sec_EncodeSessionGet(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_UINT16 usResult,
                        WE_UINT8 ucSessionOptions, const WE_UCHAR * pucSessionId,
                        WE_UINT16 usSessionIdLen, WE_UINT8 *pucCipherSuite,
                        WE_UINT8 ucCompressionAlg, const WE_UCHAR * pucPrivateKeyId,
                        WE_UINT32 uiCreationTime);
WE_INT32 Sec_EncodeGetCertNameList(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult);
WE_INT32 Sec_EncodeDeleteCert(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, 
                            WE_INT32 iCertId, WE_INT32 iResult);
WE_INT32 Sec_EncodeKeyPairGen(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID,
                        WE_INT32 iResult, WE_UINT8 ucKeyType, 
                        const WE_UCHAR * pucPublicKey, WE_INT32 iPublicKeyLen, 
                        const WE_UCHAR * pucSig, WE_INT32 iSigLen);
WE_INT32 Sec_EncodeGetPubKey(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult, 
                      const WE_UCHAR * pucPublicKey, WE_INT32 iPublicKeyLen,
                      const WE_UCHAR * pucSig, WE_INT32 iSigLen);
WE_INT32 Sec_EncodeModifyPin(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult);
WE_INT32 Sec_EncodeViewAndGetCert(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult, 
                            WE_INT32 iCertId, WE_UCHAR * pucCert, WE_INT32 iCertLen);
WE_INT32 Sec_EncodeGetCurSvrCert(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult);
WE_INT32 Sec_EncodeGetSessionInfo(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult);
WE_INT32 Sec_EncodeWtlsCurClass(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iSecClass,
                          WE_INT32 iInfoAvailable, WE_INT32 iCertAvailable);
WE_INT32 Sec_EncodeClrSession(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult);
WE_INT32 Sec_EncodeGetContractsList(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult);
WE_INT32 Sec_EncodeRemoveContract(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult);
WE_INT32 Sec_EncodeUserCertReq(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, WE_INT32 iResult, 
                                   WE_UCHAR* pucCertReqMsg, WE_UINT32 uiCertReqMsgLen);
WE_INT32 Sec_EncodeChangeWTLSCertAble(WE_UINT8 **ppucBuffer, WE_INT32 iTargetID, 
                                        WE_INT32 iResult, WE_UINT8 ucState);

WE_INT32 Sec_EncodeConfirm(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData);
WE_INT32 Sec_EncodeChangePin(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData);
WE_INT32 Sec_EncodeSelCert(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData);
WE_INT32 Sec_EncodeNameConfirm(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData);
WE_INT32 Sec_EncodeSigntextConfirm(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData);
WE_INT32 Sec_EncodeCreatePin(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData);
WE_INT32 Sec_EncodeHash(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData);
WE_INT32 Sec_EncodePin(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData);
WE_INT32 Sec_EncodeShowCertContent(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData);
WE_INT32 Sec_EncodeShowCertList(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData);
WE_INT32 Sec_EncodeShowSessionContent(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData);
WE_INT32 Sec_EncodeShowContractList(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData);
WE_INT32 Sec_EncodeStoreCert(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData);
WE_INT32 Sec_EncodeWarning(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData);
WE_INT32 Sec_EncodeStoreContract(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData);
WE_INT32 Sec_EncodeCurSvrCert(WE_UINT8 **ppucBuffer, WE_INT32 iEvent, WE_VOID* pvData);

WE_INT32 Sec_DecodeGetCipherSuite
(
    WE_VOID *pvMsg,
    WE_INT32* piTargetID,
    WE_UINT8* pucConnType
);
WE_INT32 Sec_DecodeConnType
(
    WE_VOID *pvMsg,
    WE_INT32 *piType
);

WE_INT32 Sec_DecodeSetUpConn
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    WE_INT32 *piMasterSecretId, 
    WE_INT32 *piSecId, 
    WE_INT32 *piFullHandshake,
    St_SecSessionInformation *pstSessionInfo,
    WE_UINT8   *pucConnType
);
WE_INT32 Sec_DecodeStopConn
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    WE_INT32 *piSecId, 
    WE_UINT8   *pucConnType
);
WE_INT32 Sec_DecodeRemovePeer
(
    WE_VOID *pvMsg,
    WE_INT32 *piMasterSecId
);

WE_INT32 Sec_DecodeAttachPeerToSession
(
    WE_VOID *pvMsg,
    WE_UCHAR **ppucAddress, 
    WE_INT32 *piAddressLen,
    WE_UINT16 *pusPortnum, 
    WE_INT32 *piMasterSecretId,
    WE_UINT8   *pucConnType
);
WE_INT32 Sec_DecodeSearchPeer
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    WE_UCHAR **ppucAddress, 
    WE_INT32  *piAddressLen,
    WE_UINT16 *pusPortNum, 
    WE_UINT8  *pucConnType
);
WE_INT32 Sec_DecodeEnableSession
(
    WE_VOID *pvMsg,
    WE_INT32 *piMasterSecId,
    WE_UINT8 *pucIsActive
);

WE_INT32 Sec_DecodeDisableSession
(
    WE_VOID *pvMsg,
    WE_INT32 *piMasterSecId
);
WE_INT32 Sec_DecodeSessionGet
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    WE_INT32 *piMasterSecID
);
WE_INT32 Sec_DecodeSessionRenew
(
    WE_VOID *pvMsg,
    WE_INT32 *piMasterSecID,
    WE_UINT8 *pucSessionOpts,
    WE_UCHAR **ppucSessionId,
    WE_UINT8 *pucSessionIdLen,
    WE_UINT8 **ppucCipherSuite,
    WE_UINT8 *pucCompressionAlg,
    WE_UCHAR **ppucPrivateKeyId,
    WE_UINT32 *puiCreationTime
);
WE_INT32 Sec_DecodeGetPrfResult
(
    WE_VOID  *pvMsg,
    WE_INT32 *piTargetID, 
    WE_UINT8 *pucAlg,
    WE_INT32 *piMasterSecretId, 
    WE_UCHAR **ppucSecret, 
    WE_INT32 *piSecretLen, 
    WE_CHAR  **ppcLabel, 
    WE_UCHAR **ppucSeed,
    WE_INT32 *piSeedLen,
    WE_INT32 *piOutputLen,
    WE_UINT8 *pucConnType
);
WE_INT32 Sec_DecodeVerifySvrCertChain
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetId,
    WE_VOID **ppvBuf,
    WE_INT32 *piBufLen,
    WE_UINT8 *pucConnType
);
WE_INT32 Sec_DecodeKeyExchange
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetId,
    WE_VOID **ppvParam,
    WE_UINT8 *pucAlg,
    WE_UCHAR **ppucRandVal,
    WE_UINT8 *pucConnType
);
WE_INT32 Sec_DecodeGetUserCert
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetId,
    WE_VOID **ppvBuf,
    WE_INT32 *piBufLen,
    WE_UINT8 *pucConnType
);

WE_INT32 Sec_DecodeCompSign
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetId,
    WE_UCHAR **ppucKeyId,
    WE_INT32 *piKeyIdLen,
    WE_UCHAR **ppucBuf,
    WE_INT32 *piBufLen,
    WE_UINT8 *pucConnType
);
WE_INT32 Sec_DecodeSSLGetWMasterSec
(
    WE_VOID *pvMsg,
    WE_INT32 *piMasterSecId
);
WE_INT32 Sec_DecodeEvtShowDlgAction
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetId,
    WE_VOID **ppvData,
    WE_UINT32 *puiLen
);
WE_INT32 Sec_DecodeGetCertNameList
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    WE_INT32 *piCertType
);
WE_INT32 Sec_DecodeGenKeyPair
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    WE_UINT8 *pucKeyType    
);
#ifdef G_SEC_CFG_SHOW_PIN
WE_INT32 Sec_DecodePinModify
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    WE_UINT8 *pucKeyType    
);

WE_INT32 Sec_DecodeEvtChangePinAction
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID, 
    St_ChangePinAction *pstChangePin
);

WE_INT32 Sec_DecodeEvtCreatePinAction
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID, 
    St_CreatePinAction *pstGenPin
);
WE_INT32 Sec_DecodeEvtPinAction
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID, 
    St_PinAction *pstPin
);
#endif
WE_INT32 Sec_DecodeGetCurSvrCert
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID, 
    WE_INT32 *piSecId
);

WE_INT32 Sec_DecodeGetSessionInfo
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID, 
    WE_INT32 *piSecId
);
WE_INT32 Sec_DecodeGetWtlsCurClasss
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    WE_INT32 *piSecId
);
WE_INT32 Sec_DecodeGetContractsList
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID
);
WE_INT32 Sec_DecodeTransferCert
(    
    WE_VOID *pvMsg, 
    St_SecCertContent *pstCertContent
);
WE_INT32 Sec_DecodeGetRequestUserCert
(   
    WE_VOID *pvMsg, 
    St_SecUserCertReqInfo *pstShowContractContent
);

WE_INT32 Sec_DecodeEvtConfirmAction
(
   WE_VOID *pvMsg,
   WE_INT32 *piTargetID,
   St_ConfirmAction *pstConfirm
);

WE_INT32 Sec_DecodeEvtNameConfirmAction
(   
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    St_NameConfirmAction *pstNameConfirm
);
WE_INT32 Sec_DecodeEvtHashAction
(   
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    St_HashAction *pstHash
);
WE_INT32 Sec_DecodeEvtShowCertContentAction
(    
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    St_ShowCertContentAction *pstShowCertContent
);
WE_INT32 Sec_DecodeEvtShowCertListAction
(    
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    St_ShowCertListAction *pstShowCertList
);
WE_INT32 Sec_DecodeEvtShowContractsListAction
(    
    WE_VOID *pvMsg,
    WE_INT32 piTargetID,
    St_ShowContractsListAction *pstShowContractsList
);
WE_INT32 Sec_DecodeEvtStoreCertAction
(    
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    St_StoreCertAction *pstStoreCert
);

WE_INT32 Sec_DeocdeEvtShowContractContentAction
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    St_ShowContractContentAction *pstShowContractContent
);
WE_INT32 Sec_DecodeHandle
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetID,
    WE_INT32 *piSignId, 
    WE_CHAR  **ppcText,
    WE_INT32 *piKeyIdType,
    WE_CHAR **ppcKeyId,
    WE_INT32 *piKeyIdLen,
    WE_CHAR **ppcStringToSign, 
    WE_INT32 *piStringToSignLen, 
    WE_INT32 *piOptions
 );
WE_INT32 Sec_DecodeEvtSelectCertAction
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetId,
    St_SelectCertAction *pstSelectCert
);
WE_INT32 SecS_DecodeEvtPinAction
(
    WE_VOID *pvMsg,
    WE_INT32 *piTargetId,
    St_PinAction *pstPin
);

#endif
