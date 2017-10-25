/*==================================================================================================
    HEADER NAME : sec_tl.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
    
    GENERAL DESCRIPTION
        In this file,define the initial function prototype ,and will be update later.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-07-07   steven ding        0.0.1         Draft
    
==================================================================================================*/

/***************************************************************************************************
*   multi-Include-Prevent Section
***************************************************************************************************/
#ifndef SEC_TL_H
#define SEC_TL_H

/***************************************************************************************************
*   Type Define Section
***************************************************************************************************/
typedef struct tagSt_SecTlFncId 
{
    E_SecDpFncId                eType;
}St_SecTlFncId;
typedef St_SecTlFncId       St_SecTlFunctionId ;

typedef struct tagSt_SecTlUeFncId
{
  E_SecDpFncId                  eType;
}St_SecTlUeFncId;
typedef St_SecTlUeFncId     St_SecTlUeFunctionId;

typedef struct tagSt_SecTlWimFncId 
{
  E_SecDpFncId                  eType;
}St_SecTlWimFncId;
typedef St_SecTlWimFncId    St_SecTlWimFunctionId ;

typedef struct tagSt_SecTlConfirmDlgResp
{
    E_SecDpFncId                eType;
    WE_UINT8                    ucAnswer;
}St_SecTlConfirmDlgResp;

typedef struct tagSt_SecTlStoreCertDlgResp
{
    E_SecDpFncId                eType;
    WE_UCHAR*                   pucCertFriendlyName;
    WE_UINT16                   usFriendlyNameLen;
    WE_INT16                    sCharSet;
    WE_UINT8                    ucAnswer;
}St_SecTlStoreCertDlgResp;

typedef struct tagSt_SecTlChooseCertResp
{
    E_SecDpFncId                eType;
    WE_INT32                    iCertId;
    E_SecUeReturnVal            eResult;
}St_SecTlChooseCertResp;

typedef struct tagSt_SecTlPinDlgResult
{
    E_SecDpFncId                eType;
    E_SecUeReturnVal            eResult;
}St_SecTlPinDlgResult;

typedef struct tagSt_SecTlVerifyPin
{
    E_SecDpFncId                eType;
    WE_CHAR*                    pcPin;
    WE_UINT8                    ucKeyType;
}St_SecTlVerifyPin;

typedef struct tagSt_SecTlChangePin
{
    E_SecDpFncId                eType;
    WE_CHAR*                    pcOldPin;
    WE_CHAR*                    pcNewPin;
    WE_UINT8                    ucKeyType;
}St_SecTlChangePin;

typedef struct tagSt_SecTlGenPinResp
{
      E_SecDpFncId              eType;
      E_SecUeReturnVal          eResult;
      WE_CHAR*                  pcPin;
}St_SecTlGenPinResp;

typedef struct St_SecTlVerifyHash 
{
    E_SecDpFncId                eType;
    WE_INT32                    iDialogId;
    WE_UINT8*                   pucHash;
}St_SecTlVerifyHash;

typedef struct tagSt_SecTlInitWimResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID;
    WE_INT32                    iResult;
}St_SecTlInitWimResp;

typedef struct tagSt_SecTlGetWtlsMethodsResp
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID;
    WE_INT32                    iResult;
    WE_UCHAR*                   pucCipherMethods;
    WE_INT32                    iCipherMethodsLen;
    WE_UCHAR*                   pucKeyExchangeIds;
    WE_INT32                    iKeyExchangeIdsLen;
    WE_UCHAR*                   pucTrustedKeyIds;
    WE_INT32                    iTrustedKeyIdsLen;
    WE_INT32                    iTooManyCerts;
}St_SecTlGetWtlsMethodsResp;

typedef struct tagSt_SecTlKeyXchResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID;
    WE_INT32                    iResult;
    WE_INT32                    iMasterSecretId;
    WE_UCHAR*                   pucPublicValue;
    WE_INT32                    iPublicValueLen;
}St_SecTlKeyXchResp;

typedef struct tagSt_SecTlPrfResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID;
    WE_INT32                    iResult;
    WE_UCHAR*                   pucBuf;
    WE_INT32                    iBufLen;
}St_SecTlPrfResp;

typedef struct tagSt_SecTlGenRsaKeypairResp
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID;
    WE_INT32                    iResult;
    WE_UCHAR*                   pucPublicKey;
    WE_INT32                    iPublicKeyLen;
    WE_UCHAR*                   pucSig;
    WE_INT32                    iSigLen;
    WE_UINT8                    ucKeyType;
}St_SecTlGenRsaKeypairResp;

typedef struct tagSt_SecTlRmKeysAndUcertsResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID;
    WE_INT32                    iResult;
}St_SecTlRmKeysAndUcertsResp;

typedef struct tagSt_SecTlVerifyCertChainResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID;
    WE_INT32                    iResult;
    WE_INT32                    iCertTooOld;
    WE_INT32                    iCertTooOldId;
    WE_UINT16                   usKeyExchKeyLen;
}St_SecTlVerifyCertChainResp;

typedef struct tagSt_SecTlVerifyUserCertResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID;
    WE_INT32                    iResult;
    WE_UCHAR*                   pucCaCert;
    WE_INT32                    iRootCertTooOld;
    WE_INT32                    iRootCertTooOldId;
    WE_UINT16                   usCaCertLen;
    WE_UINT8                    ucAlg;
}St_SecTlVerifyUserCertResp;

typedef struct tagSt_SecTlVerifyRootCertresp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID;
    WE_INT32                    iResult;
}St_SecTlVerifyRootCertresp;

typedef struct tagSt_SecTlChangeWtlsCertAbleresp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID;
    WE_INT32                    iResult;
    WE_UINT8                    ucState;
}St_SecTlChangeWtlsCertAbleresp;

typedef struct tagSt_SecTlComputeSignResp
{
  E_SecDpFncId                  eType;
  WE_INT32                      iTargetID;
  WE_INT32                      iResult;
  WE_UCHAR*                     pucSig;
  WE_INT32                      iSigLen;
}St_SecTlComputeSignResp;

typedef struct tagSt_SecTlFindPrivkeyResp
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID;
    WE_INT32                    iResult;
    WE_UINT32                   uiKeyId; 
    WE_INT32                    iKeyCount;
    WE_UINT8                    ucKeyUsage;
}St_SecTlFindPrivkeyResp;

typedef struct tagSt_SecTlRmPeerLinksResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iResult;
}St_SecTlRmPeerLinksResp;

typedef struct tagSt_SecTlLinkPeerSessionResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iResult;
}St_SecTlLinkPeerSessionResp;

typedef struct tagSt_SecTlActiveSessionResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iResult;
}St_SecTlActiveSessionResp;

typedef struct tagSt_SecTlInvalidSessionResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iResult;
}St_SecTlInvalidSessionResp;

typedef struct tagSt_SecTlCleanUpSessionResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iResult;
}St_SecTlCleanUpSessionResp;

typedef struct tagSt_SecTlUpdateSessionResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iResult;
}St_SecTlUpdateSessionResp;

typedef struct tagSt_SecTlLookupPeerResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID;
    WE_INT32                    iResult;
    WE_INT32                    iMasterSecretId;
    WE_INT32                    iDeletedIndex;
    WE_UINT8                    ucConnectionType;
}St_SecTlLookupPeerResp;

typedef struct tagSt_SecTlFetchSessionResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID;
    WE_INT32                    iResult;
    WE_UCHAR*                   pucSessionId;
    WE_UCHAR*                   pucPrivateKeyId;
    WE_UINT32                   uiCreationTime;
    WE_UINT8                    ucSessionOptions;
    WE_UINT8                    ucSessionIdLen;
    WE_UINT8                    aucCipherSuite[2];
    WE_UINT8                    ucCompressionAlg;
}St_SecTlFetchSessionResp;

typedef struct tagSt_SecTlCertIsExistResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID;
    WE_INT32                    iResult;
    WE_UINT32                   uiNewValidNotBefore; 
    WE_UINT32                   uiNewValidNotAfter;
    WE_UINT32                   uiOldValidNotBefore; 
    WE_UINT32                   uiOldValidNotAfter;
    WE_INT32                    iFileId;
    WE_UINT8                    ucExists;
}St_SecTlCertIsExistResp;

typedef struct tagSt_SecTlSelfSignedCertResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID;
    WE_INT32                    iResult;
    WE_UINT8                    ucSelfsigned;
}St_SecTlSelfSignedCertResp;

typedef struct tagSt_SecTlStoreCertResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID;
    WE_INT32                    iResult;
    WE_INT32                    iCertId;
    WE_UINT32                   uiCertCount;
    WE_INT32                    iURLCount;
    WE_INT32                    iNameCount;
}St_SecTlStoreCertResp;

typedef struct tagSt_SecTlStoreUcertResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID;
    WE_INT32                    iResult;
}St_SecTlStoreUcertResp;

typedef struct tagSt_SecTlRmCertResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iCertId; 
    WE_INT32                    iResult;
}St_SecTlRmCertResp;

typedef struct tagSt_SecTlGetCertResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iResult;
    WE_UCHAR*                   pucCert;
    WE_UCHAR*                   pucTrustedUsage;
    WE_UINT16                   usTrustedUsageLen;
    WE_UINT16                   usCertLen;
}St_SecTlGetCertResp;

typedef struct tagSt_SecTlCertNamesResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iResult;
    WE_INT32                    iNbrOfCerts;
    St_SecCertName*             pstCertNames;
}St_SecTlCertNamesResp;

typedef struct tagSt_SecTlConnCertNamesResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iResult;
    WE_INT32                    iNbrOfCerts;
    St_SecCertName*             pstCertNames;
    WE_INT8*                    pcChainDepth;
    WE_UINT32**                 ppuiCertChainIds;
}St_SecTlConnCertNamesResp;

typedef struct tagSt_SecTlGetChainOfCertsResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iResult;
    WE_INT32                    iNbrOfCerts;
    WE_UCHAR**                  ppucCerts;
    WE_UINT16*                  pusCertLen;
}St_SecTlGetChainOfCertsResp;

typedef struct tagSt_SecTlGetCertIdsResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iResult;
    WE_INT32                    iNbrOfCerts;
    WE_UINT8*                   pucCertificateIds;
}St_SecTlGetCertIdsResp ;

typedef struct tagSt_SecTlGetNbrCertsResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID;
    WE_INT32                    iResult;
    WE_INT32                    iNbrCerts;
}St_SecTlGetNbrCertsResp ;

typedef struct tagSt_SecTlGetUcertSignedkeyResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iResult;
    WE_UCHAR*                   pucCert;
    WE_UCHAR*                   pucSig;
    WE_INT32                    iSigLen;
    WE_UINT16                   usCertLen;
    WE_UINT8                    ucAlg;
}St_SecTlGetUcertSignedkeyResp ;


typedef struct tagSt_SecTlGetUcertKeyIdResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iResult;
    WE_UCHAR*                   pucCert;
    WE_INT32                    iCertLen;
    WE_UCHAR*                   pucKeyId;
    WE_INT32                    iKeyIdLen;
}St_SecTlGetUcertKeyIdResp;

typedef struct tagSt_SecTlReadViewsCertsResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iResult;
    WE_UCHAR*                   pucData;
    WE_INT32                    iLen;
}St_SecTlReadViewsCertsResp;
                                                                        
typedef struct tagSt_SecTlSignText
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID; 
    WE_INT32                    iSignId; 
    WE_INT32                    iAlgorithm; 
    WE_CHAR*                    pcSignature;
    WE_INT32                    iSigLen; 
    WE_CHAR*                    pcHashedKey;
    WE_INT32                    iHashedKeyLen;
    WE_CHAR*                    pcCertificate;
    WE_INT32                    iCertificateLen;
    WE_INT32                    iCertificateType; 
    WE_INT32                    iErr;
}St_SecTlSignText;

typedef struct tagSt_SecTlGetSignMatchesResp
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID; 
    WE_INT32                    iSignId; 
    WE_INT32                    iResult; 
    WE_INT32                    iNbrMatches;
    St_SecCertName              astCertNames[M_SEC_USER_CERT_MAX_SIZE];
}St_SecTlGetSignMatchesResp;

typedef struct tagSt_SecTlGetContractsResp
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID; 
    WE_INT32                    iResult; 
    WE_INT32                    iNbrContracts;
    St_SecContractInfo*         pstContracts;
    WE_INT32*                   piContractsLen;
}St_SecTlGetContractsResp;

typedef struct tagSt_SecTlStoreContractsResp
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID; 
    WE_INT32                    iResult; 
}St_SecTlStoreContractsResp;

typedef struct tagSt_SecTlRmContractsResp
{
    E_SecDpFncId                eType;
    WE_INT32                    iContractId; 
    WE_INT32                    iResult; 
}St_SecTlRmContractsResp;
        
typedef struct tagSt_SecTlPinResp 
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID; 
    WE_INT32                    iResult;
}St_SecTlPinResp;

//St_SecTlVerifySignResp
typedef struct tagSt_SecTlVerifySignResp
{
    E_SecDpFncId                eType;
    WE_INT32                    iTargetID; 
    WE_INT32                    iResult; 
}St_SecTlVerifySignResp;

typedef struct tagSt_SecTlUserCertReqResp
{
    E_SecDpFncId        eType;
    WE_INT32            iTargetID;
    WE_INT32            iResult;
    WE_UCHAR*           pucCertReqMsg;
    WE_UINT32           uiCertReqMsgLen; 
}St_SecTlUserCertReqResp;

/***************************************************************************************************
*   Prototype Declare Section
***************************************************************************************************/
WE_VOID Sec_TlHandleUeStructFree(WE_VOID* pvFncId);

WE_VOID Sec_TlHandleWimStructFree(WE_VOID* pvFncId);

WE_VOID Sec_TlHandleDpStructFree(WE_VOID* pvPara);

void Sec_TlHandleConvertStr4ToUint16( const WE_UINT8* pucStr, WE_UINT16* pusUint );

void Sec_TlHandleConvertStr4ToUint32( const WE_UINT8* pucStr, WE_UINT32* puiUint );

void Sec_TlHandleConvertUint16ToStr4( WE_UINT16* pusUint, WE_UINT8* pucStr );

void Sec_TlHandleConvertUint32ToStr4( WE_UINT32* puiUint, WE_UINT8* pucStr );


void Sec_StoreStrUint8to16( const WE_UINT8 *pucStr8, WE_UINT16 *pusAdd16 );

void Sec_StoreStrUint8to32( const WE_UINT8 *pucStr8, WE_UINT32 *puiAdd32 );

void Sec_ExportStrUint16to8( WE_UINT16 *pusAdd16, WE_UINT8 *pucStr8 );

void Sec_ExportStrUint32to8( WE_UINT32 *puiAdd32, WE_UINT8 *pucStr8 );

WE_INT32 Base64_Encode (WE_UINT8 **ppucTargetOut,
                                WE_UINT8 *pucDataIn, WE_UINT32 *puiDataLength);

WE_INT32 Base64_Decode (WE_UINT8 **ppucTargetOut, 
                                WE_UINT8 *pucDataIn, WE_UINT32 *puiDataLength);

#endif
