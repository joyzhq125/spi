/*==================================================================================================
    HEADER NAME : sec_wimresp.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION    
    
    GENERAL DESCRIPTION
        In this file, define the response function prototype for WIM module, and will be updated later.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-07-19    Stone An         0.0.1         create sec_wimresp.h
    
==================================================================================================*/

/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/
#ifndef SEC_WIMRESP_H
#define SEC_WIMRESP_H

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
void Sec_WimInitializeResponse (WE_HANDLE hSecHandle, 
                                WE_INT32 iTargetID, 
                                WE_INT32 iResult);
void Sec_WimWtlsGetCipherSuiteResponse (WE_HANDLE hSecHandle,
                                WE_INT32 iTargetID, 
                                WE_INT32 iResult,
                                const WE_UCHAR * pucCipherMethods,
                                WE_INT32 iCipherMethodsLen,
                                const WE_UCHAR *pucKeyExchangeIds,
                                WE_INT32 iKeyExchangeIdsLen,
                                const WE_UCHAR *pucTrustedKeyIds,
                                WE_INT32 iTrustedKeyIdsLen, 
                                WE_INT32  iTooManyCerts);
void Sec_WimKeyExchangeResponse (WE_HANDLE hSecHandle,
                                 WE_INT32 iTargetID, 
                                 WE_INT32 iResult,
                                 WE_INT32 iMasterSecretId,
                                 const WE_UCHAR* pucPublicValue,
                                 WE_INT32 iPublicValueLen);
void Sec_WimGetPrfResultResponse (WE_HANDLE hSecHandle,
                         WE_INT32 iTargetID, WE_INT32 iResult,
                         const WE_UCHAR *pucBuf, 
                         WE_INT32 iBufLen);
void Sec_WimKeyPairGenResponse(WE_HANDLE hSecHandle,
                                    WE_INT32 iTargetID, 
                                    WE_INT32 iResult, 
                                    WE_UINT8 ucKeyType, 
                                    const WE_UCHAR* pucPublicKey, 
                                    WE_INT32 iPublicKeyLen,
                                    const WE_UCHAR *pucSig, 
                                    WE_INT32 iSigLen);
void Sec_WimDelPrivKeysAndUserCertsByTypeResponse(WE_HANDLE hSecHandle,
                                                 WE_INT32 iTargetID, 
                                                 WE_INT32 iResult);
void Sec_WimGetUserCertAndPrivKeyResponse(WE_HANDLE hSecHandle,
                                            WE_INT32 iResult,
                                            const WE_UCHAR *pucCert, 
                                            WE_UINT16 usCertLen,
                                            const WE_UCHAR *pucSig, 
                                            WE_INT32 iSigLen, 
                                            WE_UINT8 ucAlg);
void Sec_WimViewNameOfUserCertByTypeResponse(WE_HANDLE hSecHandle,                       
                                           WE_INT32 iResult, 
                                           WE_INT32 iNumberOfCertificates,
                                           WE_UINT8 *pucCertType, 
                                           const void* const* ppvCertificateFriendlyNames,
                                           WE_UINT16*pusFriendlyNameLen, 
                                           WE_INT16 *psCharSet, 
                                           WE_INT8 *pcNameType,
                                           const WE_INT32*piCertificateIds,
                                           WE_UCHAR *pucAble);
void Sec_WimViewNameOfUserCertResponse(WE_HANDLE hSecHandle,
                                     WE_INT32 iResult, 
                                     WE_INT32 iNumberOfCertificates,
                                     const void *const *ppvCertificateFriendlyNames, 
                                     WE_UINT16 *pusFriendlyNameLen, 
                                     WE_INT16 *psCharSet,
                                     WE_INT8 *pcNameType, 
                                     const WE_INT32 *piCertificateIds,
                                     WE_INT8 *pcChainDepth, 
                                     WE_UINT32 **ppuiCertChainIds);  
void Sec_WimGetUserCertAndKeyIdResponse(WE_HANDLE hSecHandle,
                                        WE_INT32 iResult,
                                        WE_UCHAR *pucCert, 
                                        WE_INT32 iCertLen, 
                                        WE_UCHAR *pucKeyId, 
                                        WE_INT32 iKeyIdLen);
void Sec_WimViewPrivKeyPinResponse(WE_HANDLE hSecHandle,
                               WE_INT32 iTargetID,
                               WE_INT32 iResult);
void Sec_WimVerifyPrivKeyPinResponse(WE_HANDLE hSecHandle,
                              WE_INT32 iTargetID, 
                              WE_INT32 iResult);
void Sec_WimModifyPrivKeyPinResponse(WE_HANDLE hSecHandle,
                              WE_INT32 iTargetID,
                              WE_INT32 iResult);
void Sec_WimVerifyCertChainResponse(WE_HANDLE hSecHandle,
                                           WE_INT32 iTargetID, 
                                           WE_INT32 iResult,
                                           WE_UINT16 usKeyExchKeyLen,
                                           WE_INT32 iCertTooOld, 
                                           WE_INT32 iCertTooOldId);
void Sec_WimVerifySignatureResponse(WE_HANDLE hSecHandle,
                                          WE_INT32 iTargetID,
                                          WE_INT32 iResult);
void Sec_WimWtlsVerifyUserCertChainResponse(WE_HANDLE hSecHandle,
                                               WE_INT32 iTargetID, 
                                               WE_INT32 iResult, 
                                               WE_UINT8 ucAlg,
                                               WE_UCHAR *pucCaCert, 
                                               WE_UINT16 usCaCertLen, 
                                               WE_INT32 iRootCertTooOld, 
                                               WE_INT32 iRootCertTooOldId);
void Sec_WimVerifyRootCertResponse(WE_HANDLE hSecHandle,
                                   WE_INT32 iTargetID, 
                                   WE_INT32 iResult);
void Sec_WimSelfSignedCertResponse(WE_HANDLE hSecHandle,
                                   WE_INT32 iTargetID, 
                                   WE_INT32 iResult, 
                                   WE_UINT8 ucSelfsigned);
void Sec_WimCertExistsResponse(WE_HANDLE hSecHandle,
                                      WE_INT32 iTargetID, 
                                      WE_INT32 iResult, 
                                      WE_UINT8 ucExists,
                                      WE_UINT32 uiNewValidNotBefore,
                                      WE_UINT32 uiNewValidNotAfter,
                                      WE_UINT32 uiOldValidNotBefore,
                                      WE_UINT32 uiOldValidNotAfter,
                                      WE_INT32 iCertificateId);
void Sec_WimStoreCertResponse(WE_HANDLE hSecHandle,
                              WE_INT32 iTargetID, 
                              WE_INT32 iResult, 
                              WE_INT32 iCertId,
                              WE_UINT32 uiCertCount, 
                              WE_INT32 iURLCount,
                              WE_INT32 iNameCount);
void Sec_WimStoreUserCertDataResponse(WE_HANDLE hSecHandle,
                                      WE_INT32 iTargetID,
                                      WE_INT32 iResult);
void Sec_WimDelCertResponse(WE_HANDLE hSecHandle,
                                      WE_INT32 iCertificateId,
                                      WE_INT32 iResult);
void Sec_WimGetCertResponse(WE_HANDLE hSecHandle,
                                   WE_INT32 iResult,
                                   WE_UCHAR *pucCert,
                                   WE_UINT16 usCertLen,
                                   WE_UCHAR *pucTrustedUsage, 
                                   WE_UINT16 usTrustedUsageLen);
void Sec_WimViewNameOfCertResponse(WE_HANDLE hSecHandle,
                                     WE_INT32 iResult, 
                                     WE_INT32 iNumberOfCertificates,
                                     WE_UINT8 *pucCertType,                                      
                                     const void *const *ppvCertificateFriendlyNames,
                                     WE_UINT16 *pusFriendlyNameLen, 
                                     WE_INT16* psCharSet,
                                     WE_INT8 *pcNameType, 
                                     const WE_INT32 *piCertificateIds,
                                     WE_UCHAR *pucAble);
void Sec_WimViewIdOfCertResponse(WE_HANDLE hSecHandle,
                                      WE_INT32 iResult, 
                                      WE_INT32 iNbrOfCerts,
                                      const WE_INT32 *piCertificateIds);
void Sec_WimGetNumOfCertResponse(WE_HANDLE hSecHandle,
                                WE_INT32 iTargetID, 
                                WE_INT32 iResult,  
                                WE_INT32 iNbrCerts); 
void Sec_WimReadViewResponse(WE_HANDLE hSecHandle,
                                      WE_INT32 iResult,
                                      const WE_UCHAR *pucData,
                                      WE_INT32 iLen);
void Sec_WimDeletePeerLinkSessionResponse (WE_HANDLE hSecHandle,
                                     WE_INT32 iResult);
void Sec_WimPeerLinkSessionResponse (WE_HANDLE hSecHandle,
                                       WE_INT32 iResult);
void Sec_WimPeerSearchResponse (WE_HANDLE hSecHandle,
                                WE_INT32 iTargetID,
                                WE_INT32 iResult, 
                                WE_UINT8 ucConnectionType, 
                                WE_INT32 iMasterSecretId, 
                                WE_INT32 iDeletedIndex);
void Sec_WimSessionActiveResponse (WE_HANDLE hSecHandle,
                                   WE_INT32 iResult);
void Sec_WimSessionInvalidateResponse (WE_HANDLE hSecHandle,
                                       WE_INT32 iResult);
void Sec_WimSessionCleanResponse (WE_HANDLE hSecHandle,
                                  WE_INT32 iResult);
void Sec_WimGetSessionMastResponse (WE_HANDLE hSecHandle,
                                  WE_INT32 iTargetID,
                                  WE_INT32 iResult, 
                                  WE_UINT8 ucSessionOptions,
                                  const WE_UCHAR *pucSessionId,
                                  WE_UINT8 ucSessionIdLen, 
                                  WE_UINT8 aucCipherSuite[2],
                                  WE_UINT8 ucCompressionAlg,
                                  const WE_UCHAR *pucPrivateKeyId,
                                  WE_UINT32 uiCreationTime);
void Sec_WimSessionUpdateResponse (WE_HANDLE hSecHandle,
                                   WE_INT32 iResult);
void Sec_WimWtlsComputeSignatureResponse(WE_HANDLE hSecHandle, 
                                     WE_INT32 iTargetID, 
                                     WE_INT32 iResult,
                                     const WE_UCHAR *pucSig, 
                                     WE_INT32 iSigLen);
void Sec_WimGetMatchedPrivKeyResponse(WE_HANDLE hSecHandle,
                                           WE_INT32 iTargetID,
                                           WE_INT32 iResult, 
                                           WE_UINT32 uiKeyId, 
                                           WE_INT32 iKeyCount, 
                                           WE_UINT8 ucKeyUsage);

void Sec_WimSignTextResponse(WE_HANDLE hSecHandle,
                       WE_INT32 iTargetID, WE_INT32 iSignId, 
                       WE_INT32 iAlgorithm, 
                       const WE_CHAR *pcSignature, 
                       WE_INT32 iSigLen, 
                       const WE_CHAR *pcHashedKey, 
                       WE_INT32 iHashedKeyLen, 
                       const WE_CHAR *pcCertificate, 
                       WE_INT32 iCertificateLen, 
                       WE_INT32 iCertificateType, 
                       WE_INT32 iErr);
void Sec_WimFindMatchedSignTextResponse(WE_HANDLE hSecHandle,
                                       WE_INT32 iTargetID, 
                                       WE_INT32 iSignId,
                                       WE_INT32 iResult, 
                                       WE_INT32 iNbrMatches,
                                       const void *const *ppvFriendlyNames,
                                       WE_UINT16 *pusFriendlyNamesLen,
                                       WE_INT16 *psCharSet,
                                       WE_INT8 *pcNameType, 
                                       WE_INT32 *piCertIds);
void Sec_WimViewContractsResponse(WE_HANDLE hSecHandle,
                                 WE_INT32 iTargetID,
                                 WE_INT32 iResult, 
                                 WE_INT32 iNbrContracts,
                                 const WE_CHAR *const *ppcContracts,
                                 WE_UINT16 *pusContractLen,
                                 const WE_CHAR *const *ppcSignatures, 
                                 WE_UINT16 *pusSignaturesLen,
                                 WE_UINT32 *puiTime, 
                                 WE_INT32 *piContractIds);
void Sec_WimStoreContractResponse(WE_HANDLE hSecHandle,
                                  WE_INT32 iTargetID, 
                                  WE_INT32 iResult);
void Sec_WimDelContractResponse(WE_HANDLE hSecHandle,
                                   WE_INT32 iContractId,
                                   WE_INT32 iResult);
void Sec_WimUserCertReqResponse(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult, 
                                 WE_UCHAR *pucCertReqMsg, WE_UINT32 uiCertReqMsgLen);
WE_VOID Sec_WimChangeWtlsCertAbleResponse(WE_HANDLE hSecHandle,
                                 WE_INT32 iTargetID, WE_INT32 iResult, WE_UINT8 ucState);
WE_VOID Sec_WimGetCipherSuitesResponse(WE_HANDLE hSecHandle,WE_INT32 iTargetID, WE_INT32 iResult, 
                                     WE_UCHAR *pucCipherSuites, 
                                     WE_INT32 iCipherSuitesLen);
/*defied for tls*/
WE_VOID Sec_WimTlsGetUserCertNamesResponse(WE_HANDLE hSecHandle,WE_INT32 iResult, WE_INT32 iNumberOfCertificates, 
                                      const WE_VOID *const *pvCertificateFriendlyNames, 
                                      WE_UINT16 *pusFriendlyNameLen, WE_INT16 *pusCharSet, 
                                      WE_INT8 *pcNameType, const WE_INT32 *puiCertificateIds,
                                      WE_INT8 *pcChainDepth, WE_UINT32 **ppuiCertChainIds);
void Sec_WimGetChainOfCertificatesResponse(WE_HANDLE hSecHandle,WE_INT32 iResult,
                      WE_INT32 iNbrOfCerts, WE_UCHAR **ppucCerts, WE_UINT16 *pusCertLens);

#endif /* end of SEC_WIMRESP_H */
