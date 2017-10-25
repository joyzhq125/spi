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

/***************************************************************************
*   The result value from response
***************************************************************************/
#define G_SEC_NOTFOUND                      0x11
#define G_SEC_INVALID_COMMON_NAME           0x12
#define G_SEC_MISSING_AUTKEYHANDSHAKE       0x13
#define G_SEC_DEGRADED_TO_ANON              0x14

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
* The cipher algorithms macro defined by WTLS                                                           *
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
    WE_UINT8   ucSessionOptions;
    WE_UINT8   ucCompressionAlg;
}St_SecSessionGetResp;


/************************************************
*   ISecW interface definition                  *
************************************************/
/*The following is ISecW interface and struct definition. */
typedef struct ISecW ISecW;

AEEINTERFACE(ISecW) 
{
     INHERIT_IBase(ISecW);

    /*register callback function*/
    WE_UINT32 (*RegSecClientEvtCB)(ISecW * pMe,WE_HANDLE hPrivData,Fn_ISecEventHandle pcbSecEvtFunc);
    
    WE_UINT32 (*SetupConnection)(ISecW * pMe, WE_INT32 iTargetID, WE_INT32 iMasterSecretId, 
                                   WE_INT32 iSecId, WE_INT32 iFullHandshake,
                                   St_SecSessionInformation stSessionInfo);
    WE_UINT32 (*StopConnection)(ISecW * pMe, WE_INT32 iTargetID, WE_INT32 iSecId);
    WE_UINT32 (*RemovePeer)(ISecW * pMe, WE_INT32 iMasterSecretId);
    WE_UINT32 (*AttachPeerToSession)(ISecW * pMe,  WE_UCHAR * pucAddress, WE_INT32 iAddressLen,
                                       WE_UINT16 usPortnum, WE_INT32 iMasterSecretId);
    WE_UINT32 (*SearchPeer)(ISecW * pMe, WE_INT32 iTargetID,  WE_UCHAR * pucAddress, WE_INT32 iAddressLen,
                              WE_UINT16 usPortNum);
    WE_UINT32 (*EnableSession)(ISecW * pMe, WE_INT32 iMasterSecretId, WE_UINT8 ucIsActive);
    WE_UINT32 (*DisableSession)(ISecW * pMe, WE_INT32 iMasterSecretId);
    WE_UINT32 (*SessionGet)(ISecW * pMe, WE_INT32 iTargetID, WE_INT32 iMasterSecretId);
    WE_UINT32 (*SessionRenew)(ISecW * pMe, WE_INT32 iMasterSecretId, 
                                WE_UINT8 ucSessionOptions,  WE_UCHAR * pucSessionId, 
                                WE_UINT8 ucSessionIdLen,  WE_UINT8 *aucCipherSuite,
                                WE_UINT8 ucCompressionAlg,  WE_UCHAR * pucPrivateKeyId,
                                WE_UINT32 uiCreationTime);    
    WE_UINT32 (*WtlsGetPrfResult)(ISecW * pMe, WE_INT32 iTargetID, WE_UINT8 ucAlg, 
                                WE_INT32 iMasterSecretId,  WE_UCHAR * pucSecret, 
                                WE_INT32 iSecretLen,  WE_CHAR * pcLabel, 
                                 WE_UCHAR * pucSeed, WE_INT32 iSeedLen,
                                WE_INT32 iOutputLen);
    WE_UINT32 (*WtlsVerifySvrCertChain)(ISecW * pMe, WE_INT32 iTargetID,
                                  WE_UCHAR *pucBuf, WE_INT32 iBufLen,
                                  WE_UCHAR *pucAddr, WE_INT32 iAddrLen);
    WE_UINT32 (*WtlsGetCipherSuite)(ISecW * pMe, WE_INT32 iTargetID);
    WE_UINT32 (*WtlsKeyExchange)(ISecW * pMe, WE_INT32 iTargetID, 
                           St_SecWtlsKeyExchParams stParam, 
                           WE_UINT8 ucHashAlg,  WE_UCHAR * pucRandval);
    WE_UINT32 (*WtlsGetUserCert)(ISecW * pMe, WE_INT32 iTargetID, 
                                WE_UCHAR * pucBuf, WE_INT32 iBufLen);
    WE_UINT32 (*WtlsCompSign)(ISecW * pMe, WE_INT32 iTargetID,
                             WE_UCHAR * pucKeyId, WE_INT32 iKeyIdLen,
                             WE_UCHAR * pucBuf, WE_INT32 iBufLen);   
    /*added by Bird 061122*/
    WE_UINT32 (*EvtShowDlgAction)(ISecW * pMe,WE_INT32 iTargetId,
        WE_VOID* pvData,WE_UINT32 uiLength);
};

/* Extension structure. All variables in here are reference via "pMe->" */
struct ISecW
{
    AEEVTBL(ISecW) *pvt;

    WE_UINT32   m_nRefs;           
    IShell      *m_pIShell;        
    IModule     *m_pIModule;
    /*Add variable here*/
    WE_HANDLE   hPrivateData;
    WE_HANDLE   hSecPrivDataCB;
    WE_HANDLE   hcbSecEvtFunc; 
    /*added by Bird 061122*/
    WE_INT32    iIFType;     
    WE_HANDLE   hUserCertKeyPair;     
};

/* ISecW interface opened to external for wap wtls */
#define ISecW_AddRef(p)                             AEEGETPVTBL((p),ISecW)->AddRef((p))                            
#define ISecW_Release(p)                            AEEGETPVTBL((p),ISecW)->Release((p))                            

#define ISecW_RegWapEventCB(p,v,c)                  AEEGETPVTBL((p),ISecW)->RegSecClientEvtCB((p),(v),(c))

#define ISecW_SetUpConnection(p,i,m,s,h,e)          AEEGETPVTBL((p),ISecW)->SetupConnection((p),(i),(m),(s),(h),(e))
#define ISecW_StopConnection(p,i,s)                 AEEGETPVTBL((p),ISecW)->StopConnection((p),(i),(s))
#define ISecW_RemovePeer(p,i)                       AEEGETPVTBL((p),ISecW)->RemovePeer((p),(i))
#define ISecW_AttachPeerToSession(p,a,l,i,s)        AEEGETPVTBL((p),ISecW)->AttachPeerToSession((p),(a),(l),(i),(s))
#define ISecW_SearchPeer(p,w,a,l,i)                 AEEGETPVTBL((p),ISecW)->SearchPeer((p),(w),(a),(l),(i))
#define ISecW_EnableSession(p,m,a)                  AEEGETPVTBL((p),ISecW)->EnableSession((p),(m),(a))
#define ISecW_DisableSession(p,m)                   AEEGETPVTBL((p),ISecW)->DisableSession((p),(m))
#define ISecW_SessionGet(p,w,m)                     AEEGETPVTBL((p),ISecW)->SessionGet((p),(w),(m))
#define ISecW_SessionRenew(p,m,s,i,l,c,a,v,t)       AEEGETPVTBL((p),ISecW)->SessionRenew((p),(m),(s),(i),(l),(c),(a),(v),(t))
#define ISecW_GetPrfResult(p,i,a,m,s,l,e,b,d,o)     AEEGETPVTBL((p),ISecW)->WtlsGetPrfResult((p),(i),(a),(m),(s),(l),(e),(b),(d),(o))
#define ISecW_VerifySvrCertChain(p,i,b,l,a,r)       AEEGETPVTBL((p),ISecW)->WtlsVerifySvrCertChain((p),(i),(b),(l),(a),(r))
#define ISecW_GetCipherSuite(p,i)                   AEEGETPVTBL((p),ISecW)->WtlsGetCipherSuite((p),(i))
#define ISecW_KeyExchange(p,i,a,h,r)                AEEGETPVTBL((p),ISecW)->WtlsKeyExchange((p),(i),(a),(h),(r))
#define ISecW_GetUserCert(p,i,b,l)                  AEEGETPVTBL((p),ISecW)->WtlsGetUserCert((p),(i),(b),(l))
#define ISecW_CompSign(p,i,k,l,b,f)                 AEEGETPVTBL((p),ISecW)->WtlsCompSign((p),(i),(k),(l),(b),(f))
/*added by Bird 061122*/
#define ISecW_EvtShowDlgAction(p,t,v,l)              AEEGETPVTBL((p),ISecW)->EvtShowDlgAction((p),(t),(v),(l))

#endif

