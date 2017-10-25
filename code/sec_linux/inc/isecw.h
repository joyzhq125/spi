/*==================================================================================================
    HEADER NAME : iSecW.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
    
    GENERAL DESCRIPTION
        In this file,define the interface function prototype ,and the related structure 
        used in these interfaces.This is a public head file of sec module.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-07-07   Kevin Yang        None         Draft
==================================================================================================*/

/***************************************************************************************************
*   Multi-Include-Prevent Section
***************************************************************************************************/

#ifndef _ISECW_H_
#define _ISECW_H_

/*******************************************************************************
*   macro Define Section
*******************************************************************************/
typedef struct _ISecW ISecW;
#define G_SEC_WTLS_CONNECTION_TYPE        0x00
#define G_SEC_TLS_CONNECTION_TYPE         0x01
#define G_SEC_SSL_CONNECTION_TYPE         0x02
/* Options for key exchange */
#define G_SEC_TLS_OPTION_IS_SSL                         1
#define G_SEC_TLS_OPTION_SSL_CLIENT_HELLO               2
/***********************************************************************
*   Response Event
************************************************************************/
#define   G_SEARCHPEER_RESP             0x00
#define   G_SESSIONGET_RESP             0x01 
#define   G_VERIFYSVRCERT_RESP          0x02 
#define   G_GETPRF_RESP                 0x03 
#define   G_GETUSERCERT_RESP            0x04 
#define   G_COMPUTESIGN_RESP            0x05 
#define   G_GETCIPHERSUITE_RESP         0x06 
#define   G_KEYEXCH_RESP                0x07 
/*add by tang070320*/
#define   G_TLS_GETUSERCERT_RESP        0x08  
#define   G_TLS_GETCIPHERSUITE_RESP     0x09 
#define   G_TLS_GETMASTERSECRET_RESP    0x0a 



/***************************************************************************
*   The result value from response
***************************************************************************/
#define G_SEC_NOTFOUND                      0x11
#define G_SEC_INVALID_COMMON_NAME           0x12
#define G_SEC_MISSING_AUTKEYHANDSHAKE       0x13
#define G_SEC_DEGRADED_TO_ANON              0x14
/*---------------------------------------------------
 * All the cipher suites for tls
 *---------------------------------------------------*/
#define G_SEC_TLS_NULL_WITH_NULL_NULL                   0x0000
#define G_SEC_TLS_RSA_WITH_NULL_MD5                     0x0001
#define G_SEC_TLS_RSA_WITH_NULL_SHA                     0x0002
#define G_SEC_TLS_RSA_EXPORT_WITH_RC4_40_MD5            0x0003
#define G_SEC_TLS_RSA_WITH_RC4_128_MD5                  0x0004
#define G_SEC_TLS_RSA_WITH_RC4_128_SHA                  0x0005
#define G_SEC_TLS_RSA_EXPORT_WITH_RC2_CBC_40_MD5        0x0006
#define G_SEC_TLS_RSA_WITH_IDEA_CBC_SHA                 0x0007
#define G_SEC_TLS_RSA_EXPORT_WITH_DES40_CBC_SHA         0x0008
#define G_SEC_TLS_RSA_WITH_DES_CBC_SHA                  0x0009
#define G_SEC_TLS_RSA_WITH_3DES_EDE_CBC_SHA             0x000A
#define G_SEC_TLS_DH_DSS_EXPORT_WITH_DES40_CBC_SHA      0x000B
#define G_SEC_TLS_DH_DSS_WITH_DES_CBC_SHA               0x000C
#define G_SEC_TLS_DH_DSS_WITH_3DES_EDE_CBC_SHA          0x000D
#define G_SEC_TLS_DH_RSA_EXPORT_WITH_DES40_CBC_SHA      0x000E
#define G_SEC_TLS_DH_RSA_WITH_DES_CBC_SHA               0x000F
#define G_SEC_TLS_DH_RSA_WITH_3DES_EDE_CBC_SHA          0x0010
#define G_SEC_TLS_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA     0x0011
#define G_SEC_TLS_DHE_DSS_WITH_DES_CBC_SHA              0x0012
#define G_SEC_TLS_DHE_DSS_WITH_3DES_EDE_CBC_SHA         0x0013
#define G_SEC_TLS_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA     0x0014
#define G_SEC_TLS_DHE_RSA_WITH_DES_CBC_SHA              0x0015
#define G_SEC_TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA         0x0016
#define G_SEC_TLS_DH_ANON_EXPORT_WITH_RC4_40_MD5        0x0017
#define G_SEC_TLS_DH_ANON_WITH_RC4_128_MD5              0x0018
#define G_SEC_TLS_DH_ANON_EXPORT_WITH_DES40_CBC_SHA     0x0019
#define G_SEC_TLS_DH_ANON_WITH_DES_CBC_SHA              0x001A
#define G_SEC_TLS_DH_ANON_WITH_3DES_EDE_CBC_SHA         0x001B

/************************************************************************
*   The key exchange algorithms defined by WTLS                                                     *
************************************************************************/
#define G_SEC_KEYEXCH_NULL                  0x00
#define G_SEC_KEYEXCH_SHARED_SECRET         0x01
#define G_SEC_KEYEXCH_DH_ANON               0x02
#define G_SEC_KEYEXCH_DH_ANON_512           0x03
#define G_SEC_KEYEXCH_DH_ANON_768           0x04
#define G_SEC_KEYEXCH_RSA_ANON              0x05
#define G_SEC_KEYEXCH_RSA_ANON_512          0x06
#define G_SEC_KEYEXCH_RSA_ANON_768          0x07
#define G_SEC_KEYEXCH_RSA                   0x08
#define G_SEC_KEYEXCH_RSA_512               0x09
#define G_SEC_KEYEXCH_RSA_768               0x0a
#define G_SEC_KEYEXCH_ECDH_ANON             0x0b
#define G_SEC_KEYEXCH_ECDH_ANON_113         0x0c
#define G_SEC_KEYEXCH_ECDH_ANON_131         0x0d
#define G_SEC_KEYEXCH_ECDH_ECDSA            0x0e

/************************************************************************
*   The key exchange algorithms defined by TLS                                                     *
************************************************************************/
#define    G_SEC_TLS_KEYEXCH_NULL                0x00                     
#define    G_SEC_TLS_KEYEXCH_RSA                 0x01  
#define    G_SEC_TLS_KEYEXCH_RSA_EXPORT          0x02
#define    G_SEC_TLS_KEYEXCH_DH_DSS_EXPORT       0x03
#define    G_SEC_TLS_KEYEXCH_DH_DSS              0x04   
#define    G_SEC_TLS_KEYEXCH_DH_RSA_EXPORT       0x05
#define    G_SEC_TLS_KEYEXCH_DH_RSA              0x06
#define    G_SEC_TLS_KEYEXCH_DHE_DSS_EXPORT      0x07
#define    G_SEC_TLS_KEYEXCH_DHE_DSS             0x08  
#define    G_SEC_TLS_KEYEXCH_DHE_RSA_EXPORT      0x09
#define    G_SEC_TLS_KEYEXCH_DHE_RSA             0x0a 
#define    G_SEC_TLS_KEYEXCH_DH_ANON_EXPORT      0x0b
#define    G_SEC_TLS_KEYEXCH_DH_ANON             0x0c 
/************************************************************************
* The cipher algorithms macro defined for WTLS and tls                                                          *
************************************************************************/
#define G_SEC_ALG_CIPHER_NULL               0x00        
#define G_SEC_ALG_CIPHER_RC5_CBC_40         0x01  
#define G_SEC_ALG_CIPHER_RC5_CBC_56         0x02  
#define G_SEC_ALG_CIPHER_RC5_CBC            0x03     
#define G_SEC_ALG_CIPHER_DES_CBC_40         0x04  
#define G_SEC_ALG_CIPHER_DES_CBC            0x05     
#define G_SEC_ALG_CIPHER_3DES_CBC_EDE       0x06
#define G_SEC_ALG_CIPHER_IDEA_CBC_40        0x07 
#define G_SEC_ALG_CIPHER_IDEA_CBC_56        0x08 
#define G_SEC_ALG_CIPHER_IDEA_CBC           0x09    
#define G_SEC_ALG_CIPHER_RC5_CBC_64         0x0A  
#define G_SEC_ALG_CIPHER_IDEA_CBC_64        0x0B 
#define G_SEC_ALG_CIPHER_RC2_CBC_40         0x14  
#define G_SEC_ALG_CIPHER_RC4_40             0x15      
#define G_SEC_ALG_CIPHER_RC4_128            0x16  

/************************************************************************
*   Hash algorithm macro                                                                                        *
************************************************************************/
#define G_SEC_HASH_NULL                     0x00
#define G_SEC_HASH_MD5                      0x01 
#define G_SEC_HASH_SHA                      0x02 
#define G_SEC_HASH_MD2                      0x03

/************************************************************************
*   Session options                                                                                                 *
************************************************************************/
#define G_SEC_SESSION_OPTIONS_RESUMABLE     0x80
#define G_SEC_SESSION_OPTIONS_SERVER_AUTH   0x20
#define G_SEC_SESSION_OPTIONS_CLIENT_AUTH   0x10


/*******************************************************************************
*   Type Define Section
*******************************************************************************/
/************************************************
*   structure used in i interface for wtls                          *
************************************************/
typedef struct tagSt_SecSessionInformation
{
    WE_UINT32     uiCertLen;
    WE_UCHAR  *   pucCert;
    WE_UINT16     usKeyExchangeKeyLen;
    WE_UINT16     usHmacLen;
    WE_UINT16     usEncryptionKeyLen;
    WE_UINT8      ucSecurityClass;
    WE_UINT8      ucKeyExchangeAlg;
    WE_UINT8      ucHmacAlg;
    WE_UINT8      ucEncryptionAlg;
    WE_UINT8      ucNumCerts;
    WE_UINT8      ucCertFormat;
}St_SecSessionInformation;

typedef struct tagSt_SecCrptPubKeyRsa
{
    WE_UCHAR * pucExponent;
    WE_UCHAR * pucModulus;  
    WE_UINT16  usExpLen;
    WE_UINT16  usModLen;
}St_SecCrptPubKeyRsa;
typedef St_SecCrptPubKeyRsa St_SecPubKeyRsa;

typedef union tagSt_SecWtlsPublicKeyRsa
{
    St_SecPubKeyRsa     stRsaKey;
}St_SecWtlsPublicKeyRsa;

typedef struct tagSt_SecWtlsPublicKey
{
    St_SecWtlsPublicKeyRsa stPubKey;    
}St_SecWtlsPublicKey;

typedef struct tagSt_SecWtlsParamSpecifier
{
    WE_UCHAR * pucParams;
    WE_UINT16  usParamLen;
    WE_UINT8   ucParameterIndex;
}St_SecWtlsParamSpecifier;

typedef struct tagSt_SecSecretKey
{
    WE_UCHAR * pucIdentifier;
    WE_UINT16  usIdLen;
}St_SecSecretKey;

typedef struct tagSt_SecKeyParam
{
    St_SecWtlsPublicKey      stPubKey;
    St_SecWtlsParamSpecifier stParamSpecifier;
}St_SecKeyParam;

typedef struct tagSt_SecCertificates
{
    WE_UCHAR * pucBuf;
    WE_UINT16  usBufLen;
}St_SecCertificates;

typedef union tagSt_SecKeyExchParams
{
    St_SecKeyParam     stKeyParam;
    St_SecCertificates stCertificates;
    St_SecSecretKey    stSecretKey;
}St_SecKeyExchParams;

typedef struct tagSt_SecWtlsKeyExchParams
{
    St_SecKeyExchParams stParams;
    WE_UINT8            ucKeyExchangeSuite;
}St_SecWtlsKeyExchParams;

typedef struct tagSt_SecWtlsVerifyCertChainParam
{
  WE_UCHAR     *pucBuf;    
  WE_INT32     iBufLen;
  WE_UCHAR     *pucAddr;
  WE_UCHAR     *pucDomain;
  WE_INT32     iAddrLen;  
  WE_UINT16    usDomainLen;
} St_SecWtlsVerifyCertChainParam;

/****************************************
*        structure used in i interface for tls                 *
********************************************/
typedef struct tagSt_SecTlsDistingubshedName
{
   WE_UCHAR       *pucName;  
   WE_UINT16      usNameLen;
   WE_UINT16      usReserved;
} St_SecTlsDistingubshedName;

typedef struct tagSt_SecTlsGetUsrCertParam
{
    St_SecTlsDistingubshedName *pstCertAuthorities;
    WE_UCHAR    *pucCertTypes;
    WE_UINT8    ucNumCertAuthorities;
    WE_UINT8    ucNumCertTypes;
    WE_UINT16   usReserved;
}St_SecTlsGetUsrCertParam;

typedef struct tagSt_SecTlsAsn1Certificate
{
  WE_UINT32       uiCertLen;
  WE_UCHAR        *pucCert;
  WE_UINT8        ucFormat;
  WE_UINT8        ucReserved1;
  WE_UINT16       usReserved2;
} St_SecTlsAsn1Certificate;

typedef struct tagSt_SecTlsVerifyCertChainParam
{
  WE_UCHAR      *pucAddr;
  WE_UCHAR      *pucDomain;
  St_SecTlsAsn1Certificate *pstCerts;
  WE_UINT16     usAddrLen;
  WE_UINT16     usDomainLen;
  WE_UINT8      ucNumCerts;
  WE_UINT8      ucReserved1;
  WE_UINT16     usReserved2;
} St_SecTlsVerifyCertChainParam;

/*for St_SecTlsKeyExchangeParams*/
typedef struct tagSt_SecTlsSignature
{
   WE_UINT8      *pucSignature;
   WE_UINT16     usSignLen;
   WE_UINT16     usReserved;
}St_SecTlsSignature;

typedef struct tagSt_SecTlsSeverRSAParams
{
   WE_UINT8     *pucModulus;
   WE_UINT8     *pucExponent;
   WE_UINT16    usModLen;
   WE_UINT16    usExpLen;
}St_SecTlsSeverRSAParams;

typedef struct tagSt_SecTlsRSAParam
{
   St_SecTlsSeverRSAParams stTlsRSAParam;
   St_SecTlsSignature      stTlsSignature;
}St_SecTlsRSAParam;

typedef struct tagSt_SecTlsSeverDHParams
{
   WE_UINT8    *pucDHP;
   WE_UINT8    *pucDHG;
   WE_UINT8    *pucDHYs;
   WE_UINT16   usDHPLen;
   WE_UINT16   usDHGLen;
   WE_UINT16   usDHYsLen;
   WE_UINT16   usReserved1;
}St_SecTlsSeverDHParams;

typedef struct tagSt_SecTlsDHParam
{
    St_SecTlsSeverDHParams stTlsDHParam;
    St_SecTlsSignature     stTlsSignature;
}St_SecTlsDHParam;

typedef struct tagSt_SecTlsKeyExchangeParams
{
    St_SecTlsRSAParam       stRSAParam;
    St_SecTlsDHParam        stDHParam;
    St_SecCertificates      stSecCert;
    WE_INT32                iOptions;    
}St_SecTlsKeyExchangeParams;
/************************************************
*   structure used in response                                          *
************************************************/
typedef struct tagSt_SecGetCipherSuiteResp
{
    WE_INT32   iTargetID;
    WE_UCHAR * pucCipherMethods;
    WE_UCHAR * pucKeyExchangeIds;
    WE_UCHAR * pucTrustedKeyIds;
    WE_UINT16  usResult;
    WE_UINT16  usCipherMethodsLen;
    WE_UINT16  usKeyExchangeIdsLen;
    WE_UINT16  usTrustedKeyIdsLen;
}St_SecGetCipherSuiteResp;

typedef struct tagSt_SecKeyExchResp
{
    WE_INT32   iTargetID;    
    WE_UCHAR * pucPreMsKey;
    WE_UINT16  usResult;
    WE_UINT16  usPreMsKeyLen;
    WE_UINT8   ucMasterSecretId;
}St_SecKeyExchResp;

typedef struct tagSt_SecGetPrfResp
{
    WE_INT32   iTargetID;
    WE_UCHAR * pucBuf;
    WE_UINT16  usResult;
    WE_UINT16  usBufLen;
}St_SecGetPrfResp;

typedef struct tagSt_SecVerifySvrCertResp
{
    WE_INT32 iTargetID;    
    WE_UINT16 usResult;
}St_SecVerifySvrCertResp;

typedef struct tagSt_SecGetUserCertResp
{
    WE_INT32   iTargetID;    
    WE_UCHAR * pucKeyId;
    WE_UCHAR * pucCert;
    WE_UINT16  usResult;
    WE_UINT16  usKeyIdLen;
    WE_UINT16  usCertLen;
}St_SecGetUserCertResp;

typedef struct tagSt_SecCompSignResp
{
    WE_INT32   iTargetID;
    WE_UCHAR * pucSignature;
    WE_UINT16  usResult;
    WE_UINT16  usSignatureLen;
}St_SecCompSignResp;

typedef struct tagSt_SecSearchPeerResp
{
    WE_INT32  iTargetID;    
    WE_INT32  iSecId;
    WE_UINT16 usResult;
    WE_UINT8  ucConnectionType;
    WE_UINT8  ucMasterSecretId;
}St_SecSearchPeerResp;

typedef struct tagSt_SecSessionGetResp
{
    WE_INT32   iTargetID;    
    WE_UCHAR * pucSessionId;
    WE_UCHAR * pucPrivateKeyId;
    WE_UINT32  uiCreationTime;
    WE_UINT8   aucCipherSuite[2];

    WE_UINT16  usResult;
    WE_UINT16  usSessionIdLen;
    WE_UINT16  usPrivateKeyIdLen; 
    WE_UINT8   ucSessionOptions;
    WE_UINT8   ucCompressionAlg;
}St_SecSessionGetResp;

typedef struct tagSt_SecTlsGetUserCertResp
{
   WE_INT32    iTargetID;  
   WE_UCHAR    *pucPubKeyHash;
   St_SecTlsAsn1Certificate * pstCert;
   WE_UINT16   usPubKeyHashLen;
   WE_UINT16   usResult;
   WE_UINT8    ucNbrCerts;
}St_SecTlsGetUserCertResp;

typedef struct  tagSt_SecTlsGetCipherSuitesResp
{
   WE_INT32     iTargetID; 
   WE_UCHAR     *pucCipherSuites;
   WE_UINT16    usCipherSuitesLen;
   WE_UINT16    usResult;
}St_SecTlsGetCipherSuitesResp;
typedef struct tagSt_SecSSLGetMasterSecretResp
{
    WE_INT32    iTargetID;  
    WE_UCHAR    *pucMasterSecret ;
    WE_INT32    iMasterSecretLen ;
    WE_UINT16   usResult;
}St_SecSSLGetMasterSecretResp;

/************************************************
*   ISecW interface definition                  *
************************************************/
WE_UINT32 ISecW_SetUpConnection
(
    ISecW * pISec, 
    WE_INT32 iTargetID,
    WE_INT32 iMasterSecretId, 
    WE_INT32 iSecId, 
    WE_INT32 iFullHandshake,
    St_SecSessionInformation stSessionInfo,
    WE_UINT8   ucConnType
);
WE_UINT32 ISecW_StopConnection
(
    ISecW * pISec, 
    WE_INT32 iTargetID,
    WE_INT32 iSecId, 
    WE_UINT8   ucConnType
);
WE_UINT32 ISecW_RemovePeer
(
    ISecW * pISec, 
    WE_INT32 iMasterSecretId
);
WE_UINT32 ISecW_AttachPeerToSession
(
    ISecW * pISec, 
    WE_UCHAR * pucAddress, 
    WE_INT32 iAddressLen,
    WE_UINT16 usPortNum, 
    WE_INT32 iMasterSecretId,
    WE_UINT8   ucConnType
);
WE_UINT32 ISecW_SearchPeer
(
    ISecW * pISec, 
    WE_INT32 iTargetID, 
    WE_UCHAR * pucAddress,
    WE_INT32 iAddressLen,
    WE_UINT16 usPortNum,
    WE_UINT8   ucConnType
);
WE_UINT32 ISecW_EnableSession
(
    ISecW * pISec, 
    WE_INT32 iMasterSecId, 
    WE_UINT8   ucIsActive
);
WE_UINT32 ISecW_DisableSession
(
    ISecW * pISec, 
    WE_INT32 iMasterSecId
);
WE_UINT32 ISecW_SessionGet
(
    ISecW * pISec, 
    WE_INT32 iTargetID,
    WE_INT32 iMasterSecId
);
WE_UINT32 ISecW_SessionRenew
(
    ISecW * pISec, 
    WE_INT32 iMasterSecID,
    WE_UINT8 ucSessionOpts,
    WE_UCHAR *pucSessionId,
    WE_UINT8 ucSessionIdLen,
    WE_UINT8 *aucCipherSuite,
    WE_UINT8 ucCompressionAlg,
    WE_UCHAR *pucPrivateKeyId,
    WE_UINT32 uiCreationTime
);
WE_UINT32 ISecW_GetPrfResult
(
    ISecW *pISec,
    WE_INT32 iTargetID, 
    WE_UINT8 ucAlg,
    WE_INT32 iMasterSecretId, 
    WE_UCHAR * pucSecret, 
    WE_INT32 iSecretLen, 
    WE_CHAR * pcLabel, 
    WE_UCHAR * pucSeed,
    WE_INT32 iSeedLen,
    WE_INT32 iOutputLen,
    WE_UINT8 ucConnType
);
WE_UINT32 ISecW_VerifySvrCertChain
(
    ISecW *pISec,
    WE_INT32 iTargetID, 
    WE_VOID *pvBuf,
    WE_INT32 iBufLen,
    WE_UINT8 ucConnType
);
WE_UINT32 ISecW_GetCipherSuite
(
    ISecW *pISec,
    WE_INT32 iTargetID, 
    WE_UINT8 ucConnType
);
WE_UINT32 ISecW_KeyExchange
(
    ISecW *pISec,
    WE_INT32 iTargetID, 
    WE_VOID *pvParam,
    WE_UINT8 ucAlg,
    WE_UCHAR *pucRandval,
    WE_UINT8 ucConnType
);
WE_UINT32 ISecW_GetUserCert
(
    ISecW *pISec,
    WE_INT32 iTargetID, 
    WE_VOID *pvBuf,
    WE_INT32 iBufLen,
    WE_UINT8 ucConnType
);
WE_UINT32 ISecW_CompSign
(
    ISecW *pISec,
    WE_INT32 iTargetID, 
    WE_UCHAR *pucKeyId,
    WE_INT32 iKeyIdLen,
    WE_UCHAR *pucBuf,
    WE_INT32 iBufLen,
    WE_UINT8 ucConnType
);
WE_UINT32 ISecW_SslHashUpdateWMasterSec
(
    ISecW *pISec,
    WE_INT32 iMasterSecId
);
WE_UINT32 ISecW_EvtShowDlgAction
(
    ISecW *pISec,
    WE_INT32 iTargetID, 
    WE_VOID *pvData,
    WE_UINT32 uiLength
);


#endif

