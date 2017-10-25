/*==================================================================================================
    HEADER NAME : sec_iwapim.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
    wap_cm.h
    wap_tmdf.h
    
    GENERAL DESCRIPTION
        In this file,define the function prototype 
    
    TECHFAITH Software Confidential Proprietary
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

#ifndef SEC_MSGIM_H
#define SEC_MSGIM_H

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/

void Sec_GetCipherSuiteIm (WE_HANDLE hSecHandle, WE_INT32 iTargetId);

void Sec_KeyExchangeIm (WE_HANDLE hSecHandle, WE_INT32 iTargetId, 
                        St_SecWtlsKeyExchParams *pstParam,  WE_UINT8 ucHashAlg, 
                        const WE_UCHAR * pucRandval);

void Sec_GetPrfResultIm (WE_HANDLE hSecHandle, WE_INT32 iTargetId, WE_UINT8 ucAlg, WE_INT32 iMasterSecretId,
                         const WE_UCHAR * pucSecret, WE_INT32 iSecretLen, const WE_CHAR * pcLabel,
                         const WE_UCHAR * pucSeed, WE_INT32 iSeedLen, WE_INT32 iOutputLen);

void Sec_VerifySvrCertIm (WE_HANDLE hSecHandle, WE_INT32 iTargetId, const WE_UCHAR * pucBuf, 
                          WE_INT32 iBufLen,  const WE_UCHAR * pucAddr, WE_INT32 iAddrLen,
                          const WE_CHAR * pucDomain, WE_INT32 iDomainLen);
#ifdef M_SEC_CFG_WTLS_CLASS_3

void Sec_GetUsrCertIm (WE_HANDLE hSecHandle, WE_INT32 iTargetId, 
                       const WE_UCHAR * pucBuf, WE_INT32 iBufLen);

void Sec_CompSignatureIm (WE_HANDLE hSecHandle, WE_INT32 iTargetId,
                          const WE_UCHAR * pucKeyId, WE_INT32 iKeyIdLen,
                          const WE_UCHAR * pucBuf, WE_INT32 iBufLen);
#endif 

#ifdef M_SEC_CFG_TLS
WE_VOID Sec_GetTlsUsrCertIm (WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                             const WE_UCHAR *pucCertificateTypes, 
                             WE_INT32 iNbrCertificateTypes,
                             St_SecTlsDistingubshedName *pstCertAuthorities,
                             WE_INT32 iNumCertAuthorities);

WE_VOID Sec_GetTlsCipherSuiteIm (WE_HANDLE hSecHandle, WE_INT32 iTargetID);
void Sec_VerifyTlsCertChainIm (WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                              const WE_UCHAR * pucAddr, WE_UINT16 usAddrLen,  
                              const WE_UCHAR * pucDomain, WE_UINT16 usDomainLen,
                              const St_SecTlsAsn1Certificate *pstCerts,
                              WE_UINT8 ucNumCerts);

WE_VOID Sec_CompTlsSignatureIm (WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                            const WE_UCHAR * pucPubKeyHash,WE_UINT16  usPubKeyHashLen,
                            const WE_UCHAR *pucData,WE_UINT16 usDataLen,WE_UCHAR  ucAlg);
void Sec_GetTlsPrfResultIm (WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_UINT8 ucAlg, WE_INT32 iMasterSecretId,
                            const WE_UCHAR * pucSecret, WE_INT32 iSecretLen, const WE_CHAR * pcLabel,
                            const WE_UCHAR * pucSeed, WE_INT32 iSeedLen, WE_INT32 iOutputLen);
WE_VOID Sec_TlsKeyExchangeIm (WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                              WE_UINT8 ucKeyExchangeAlgorithm, 
                              WE_INT32  iOptions, 
                              St_SecTlsKeyExchangeParams *pstParam,  
                              const WE_UCHAR *pucRandval);



#endif

void Sec_SetupConIm (WE_HANDLE hSecHandle, St_SecSetupConnection * pstPara);

void Sec_StopCon (WE_HANDLE hSecHandle, WE_INT32 iTargetId, WE_INT32 iSecurityId, 
                  WE_UINT8 ucConnectionType);


void Sec_RemovePeer (WE_HANDLE hSecHandle, WE_INT32 iTargetId,WE_INT32 iMasterSecretId);

void Sec_AttachPeerToSessionIm (WE_HANDLE hSecHandle, WE_UINT8 ucConnectionType,
                                const WE_UCHAR * pucAddress, WE_INT32 iAddressLen,
                                WE_UINT16 usPortnum,  WE_INT32 iMasterSecretId);
void Sec_SearchPeerIm (WE_HANDLE hSecHandle, WE_INT32 iTargetId, WE_UINT8 ucConnectionType,
                       const WE_UCHAR * pucAddress, WE_INT32 iAddressLen, WE_UINT16 usPortnum);
void Sec_EnableSessionIm (WE_HANDLE hSecHandle, WE_INT32 iMasterSecretId, WE_UINT8 ucIsActive);
void Sec_DisableSessionIm (WE_HANDLE hSecHandle, WE_INT32 iMasterSecretId);
void Sec_FetchSessionInfoIm (WE_HANDLE hSecHandle, WE_INT32 iTargetId, WE_INT32 iMasterSecretId);
void Sec_UpdateSessionIm (WE_HANDLE hSecHandle, WE_INT32 iMasterSecretId, WE_UINT8 ucSessionOptions,
                          WE_UCHAR * pucSessionId, WE_UINT8 ucSessionIdLen,
                          WE_UINT8 aucCipherSuite[2], WE_UINT8 ucCompressionAlg,
                          WE_UCHAR * pucPrivateKeyId, WE_UINT32 uiCreationTime);
#endif 
