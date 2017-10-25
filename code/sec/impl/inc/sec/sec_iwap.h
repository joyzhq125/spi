/*==================================================================================================
    HEADER NAME : sec_iwap.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
    
    GENERAL DESCRIPTION
        In this file,define the function prototype ,to support the wtls stack.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-06-29   Kevin Yang        None         Init
    
==================================================================================================*/

/***************************************************************************************************
*   multi-Include-Prevent Section
***************************************************************************************************/
#ifndef _SEC_IWAP_H
#define _SEC_IWAP_H


/***************************************************************************************************
*   macro Define Section
***************************************************************************************************/
/*move for i interface*/

/*1.sec.h*/
#define G_SEC_PINLOCKED                     0x0f

/*2.isecb.h*/
/*response value*/
#define G_SEC_PIN_TOO_SHORT                 0x15  
#define G_SEC_PIN_TOO_LONG                  0x16  
#define G_SEC_PIN_FALSE                     0x17

/*response*/
#define    G_MODIFYPIN_RESP                0x24


typedef struct tagSt_SecModifyPinResp
{
   WE_INT32  iTargetID;
   WE_UINT16 usResult;
}St_SecModifyPinResp;


/*3.isecw.h*/
/*4.isigntext.h*/


/* Bulk encryption alg */
typedef E_WeCipherAlgType      E_SecCipherAlgType;
typedef E_WeHashAlgType        E_SecHashAlgType;
typedef E_WeHmacAlgType        E_SecHmacAlgType;
#define E_SEC_ALG_HASH_NULL    E_WE_ALG_HASH_NULL
#define E_SEC_ALG_HASH_MD5     E_WE_ALG_HASH_MD5 
#define E_SEC_ALG_HASH_SHA1    E_WE_ALG_HASH_SHA1
#define E_SEC_ALG_HASH_MD2     E_WE_ALG_HASH_MD2

#define M_SEC_WTLS_RSA_SIGN_CERT          0x01
#define M_SEC_WTLS_CONNECTION_MODE        0x02

/* Function return values , internal used. */
#define M_SEC_ERR_OK                               G_SEC_OK            
#define M_SEC_ERR_GENERAL_ERROR                    G_SEC_GENERAL_ERROR         
#define M_SEC_ERR_UNSUPPORTED_METHOD               G_SEC_UNSUPPORTED_METHOD       
#define M_SEC_ERR_INSUFFICIENT_MEMORY              G_SEC_NOTENOUGH_MEMORY      
#define M_SEC_ERR_NOT_IMPLEMENTED                  G_SEC_NOT_SUPPORT          
#define M_SEC_ERR_INVALID_PARAMETER                G_SEC_INVALID_PARAMETER        
#define M_SEC_ERR_DATA_LENGTH                      G_SEC_DATA_LENGTH              
#define M_SEC_ERR_INVALID_KEY                      G_SEC_INVALID_KEY              
#define M_SEC_ERR_MISSING_KEY                      G_SEC_MISSING_KEY                
#define M_SEC_ERR_BUSY                             G_SEC_BUSY   /* another app is using. */       

#define M_SEC_ERR_UNKNOWN_CERTIFICATE_TYPE         G_SEC_UNKNOWN_CERTTYPE
#define M_SEC_ERR_NO_MATCHING_ROOT_CERTIFICATE     G_SEC_NOMATCH_ROOTCERT
#define M_SEC_ERR_BAD_CERTIFICATE                  G_SEC_INVALIDATE_CERT
#define M_SEC_ERR_CERTIFICATE_EXPIRED              G_SEC_CERTEXPIRED
#define M_SEC_ERR_MISSING_CERTIFICATE              G_SEC_NOTFOUND_CERT
#define M_SEC_ERR_NOT_FOUND                        G_SEC_NOTFOUND
#define M_SEC_ERR_INVALID_COMMON_NAME              G_SEC_INVALID_COMMON_NAME

#define M_SEC_ERR_USER_NOT_VERIFIED                G_SEC_USER_NOTVERIFIED
#define M_SEC_ERR_MISSING_AUT_KEY_HANDSHAKE        G_SEC_MISSING_AUTKEYHANDSHAKE
#define M_SEC_ERR_REACHED_USER_CERT_LIMIT          G_SEC_REACHED_USER_CERT_LIMIT

#define M_SEC_ERR_PIN_TOO_SHORT                    G_SEC_PIN_TOO_SHORT  
#define M_SEC_ERR_PIN_TOO_LONG                     G_SEC_PIN_TOO_LONG  
#define M_SEC_ERR_PIN_FALSE                        G_SEC_PIN_FALSE
#define M_SEC_ERR_PIN_LOCKED                       G_SEC_PINLOCKED
                                                       
#define M_SEC_ERR_HASH_NOT_VERIFIED                G_SEC_HASHNOTVERIFIED

#define M_SEC_ERR_PRIV_KEY_NOT_FOUND               G_SEC_PRIVKEY_NOTFOUND
#define M_SEC_ERR_COULD_NOT_STORE_CONTRACT         G_SEC_NOTSTORE_CONTRACT
#define M_SEC_ERR_CANCEL                           G_SEC_CANCEL
#define M_SEC_ERR_DOWNGRADED_TO_ANON               G_SEC_DEGRADED_TO_ANON


#define M_SEC_ERR_BUFFER_TOO_SMALL                 (0x20)
#define M_SEC_ERR_MISSING_AUT_KEY_OPEN_WIM         (0x21)
#define M_SEC_ERR_MISSING_NR_KEY_SIGN_TEXT         (0x22)

/***************************************************************************************************
*   Type Define Section
***************************************************************************************************/
typedef enum tagE_BulkEncryAlg
{
    E_SEC_BULK_NULL = 0x00,
    E_SEC_BULK_RC5_CBC_40,
    E_SEC_BULK_RC5_CBC_56,
    E_SEC_BULK_RC5_CBC,
    E_SEC_BULK_DES_CBC_40,
    E_SEC_BULK_DES_CBC,
    E_SEC_BULK_3DES_CBC_EDE,
    E_SEC_BULK_IDEA_CBC_40,
    E_SEC_BULK_IDEA_CBC_56,
    E_SEC_BULK_IDEA_CBC,
    E_SEC_BULK_RC5_CBC_64,
    E_SEC_BULK_IDEA_CBC_64,
    E_SEC_BULK_RC4_40 = 0x15,
    E_SEC_BULK_RC4_128 = 0x17,
    E_SEC_BULK_AES_CBC_128 = 0x1e
}E_BulkEncryAlg;

/* WTLS Key exchange alg */
typedef enum tagE_WTLSKeyExchMethods
{
    E_SEC_WTLS_KEYEXCHANGE_NULL = 0x00,
    E_SEC_WTLS_KEYEXCHANGE_SHARED_SECRET,
    E_SEC_WTLS_KEYEXCHANGE_DH_ANON,
    E_SEC_WTLS_KEYEXCHANGE_DH_ANON_512,
    E_SEC_WTLS_KEYEXCHANGE_DH_ANON_768,
    E_SEC_WTLS_KEYEXCHANGE_RSA_ANON,
    E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_512,
    E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_768,
    E_SEC_WTLS_KEYEXCHANGE_RSA,
    E_SEC_WTLS_KEYEXCHANGE_RSA_512,
    E_SEC_WTLS_KEYEXCHANGE_RSA_768,
    E_SEC_WTLS_KEYEXCHANGE_ECDH_ANON,
    E_SEC_WTLS_KEYEXCHANGE_ECDH_ANON_113,
    E_SEC_WTLS_KEYEXCHANGE_ECDH_ANON_131,
    E_SEC_WTLS_KEYEXCHANGE_ECDH_ECDSA
}E_WTLSKeyExchMethods;

/* SEC Message Used in The Module */
typedef enum tagE_SecModuleMsg
{
    E_SEC_MSG_WTLSGETCIPHERSUITE = 0x01,
    E_SEC_MSG_WTLSCOMPSIG,
    E_SEC_MSG_WTLSVERIFYSVRCERT,
    E_SEC_MSG_WTLSGETPRFRESULT,
    E_SEC_MSG_WTLSKEYEXCHANGE,
    E_SEC_MSG_WTLSGETUSRCERT,
    E_SEC_MSG_SETUPCON,
    E_SEC_MSG_STOPCON,
    E_SEC_MSG_UPSESSION,
    E_SEC_MSG_GETSESINFO,
    E_SEC_MSG_DISSESSION,
    E_SEC_MSG_DELPEER,
    E_SEC_MSG_ATTACHPEER,
    E_SEC_MSG_SEARCHPEER,
    E_SEC_MSG_ENVSESSION,

    E_SEC_MSG_CERTSAVE,
    E_SEC_MSG_CERTSAVE_RESP,
    E_SEC_MSG_TRUSTEDCA,
    E_SEC_MSG_TRUSTEDCA_RESP,
    E_SEC_MSG_DLVCERT,
    E_SEC_MSG_DLVCERT_RESP,
    E_SEC_MSG_GETCERTNAMELIST,
    E_SEC_MSG_GENKEYPAIR,
    E_SEC_MSG_GETPUBKEY,
    E_SEC_MSG_MODIFYPRIKEYPIN,
    E_SEC_MSG_VIEW_CURCERT,
    E_SEC_MSG_VIEW_INFOSESSION,
    E_SEC_MSG_GET_CURCLASS,
    E_SEC_MSG_CLRSESSION,
    E_SEC_MSG_GET_CERTCONTENT,
    E_SEC_MSG_CHANGE_WTLS_CERT_ABLE,
    E_SEC_MSG_REMOVECERT,
    E_SEC_MSG_CERTTRANSFER,    
    E_SEC_MSG_USERCERTREQ,
    E_SEC_MSG_APP_SIGNTEXT,
    E_SEC_MSG_REMOVECONTRACT,
    E_SEC_MSG_GETCONTRACTINFOS,
    E_SEC_MSG_ENCRYPT_PKC_RESP,
    E_SEC_MSG_DECRYPT_PKC_RESP,
    E_SEC_MSG_VERIFY_SIGN_RESP,
    E_SEC_MSG_COMPUTE_SIGN_RESP,
    E_SEC_MSG_GEN_KEYPAIR_RESP,
    /*added by bird 061221,action function asynchronism*/
    E_SEC_MSG_SHOWCONTRACTLIST,
    E_SEC_MSG_SHOWCONTRACTCONTENT,
    E_SEC_MSG_SHOWCERTLIST,
    E_SEC_MSG_SHOWCERTCONTENT,
    /*added by Bird 070130*/
    E_SEC_MSG_HASH,
    E_SEC_MSG_STORECERT,
    E_SEC_MSG_CONFIRM,
    E_SEC_MSG_NAMECONFIRM,
    E_SEC_MSG_SELECTCERT
    

}E_SecModuleMsg;

typedef St_WeCipherCrptKey     St_SecCipherCrptKey;

typedef struct tagSt_SecAsn1Certificate
{    
    WE_UINT32 uiCertLen;
    WE_UCHAR * pucCert;
    WE_UINT8 ucFormat;
}St_SecAsn1Certificate;

typedef struct tagSt_SecSessionInfo
{
    St_SecAsn1Certificate * pstCerts;
    WE_UINT16     usHmacLen;
    WE_UINT16     usEncryptionKeyLen;
    WE_UINT16     usKeyExchangeKeyLen;
    WE_UINT8      ucHmacAlg;
    WE_UINT8      ucEncryptionAlg;
    WE_UINT8      ucConnectionType;
    WE_UINT8      ucSecurityClass;
    WE_UINT8      ucKeyExchangeAlg;
    WE_UINT8      ucNumCerts;
}St_SecSessionInfo;

typedef struct tagSt_SecSetupConnection
{
    WE_INT32 iTargetID;
    WE_INT32 iSecurity_id;
    WE_UINT8 ucFull_handshake;
    WE_UINT8 ucMasterSecretId;
    St_SecSessionInfo stSessionInfo;
}St_SecSetupConnection;

typedef struct tagSt_SecStopConnection
{
    WE_INT32 iTargetID;
    WE_INT32 iSecurityId;
    WE_UINT8 ucConnectionType;
}St_SecStopConnection;

typedef struct tagSt_SecWtlsVerifySvrCert
{
    WE_INT32   iTargetID;
    WE_UCHAR * pucBuf;
    WE_UCHAR * pucAddr;
    WE_UCHAR * pucDomain;
    WE_UINT16  usBufLen;
    WE_UINT16  usAddrLen;
    WE_UINT16  usDomainLen;
}St_SecWtlsVerifySvrCert;

typedef struct tagSt_SecWtlsGetUsrCert
{
    WE_INT32   iTargetID;
    WE_UCHAR * pucBuf;
    WE_UINT16  usBufLen;
}St_SecWtlsGetUsrCert;

typedef struct tagSt_SecWtlsCompSig
{
    WE_INT32   iTargetID;
    WE_UCHAR * pucKeyId;
    WE_UCHAR * pucBuf;
    WE_UINT16  usKeyIdLen;
    WE_UINT16  usBufLen;
}St_SecWtlsCompSign;

typedef struct tagSt_SecWtlsGetCipherSuite
{
    WE_INT32 iTargetID;
}St_SecWtlsGetCipherSuite;

typedef struct tagSt_SecWtlsGetPrfResult
{
    WE_INT32   iTargetID;
    WE_UCHAR * pucSecret;
    WE_CHAR  * pcLabel;
    WE_UCHAR * pucSeed;
    WE_UINT16  usSeedLen;
    WE_UINT16  usOutputLen;
    WE_UINT16  usSecretLen;
    WE_UINT8   ucAlg;
    WE_UINT8   ucMasterSecretId;
}St_SecWtlsGetPrfResult;

typedef struct tagSt_SecWtlsKeyExchange
{
    WE_INT32   iTargetID;
    WE_UCHAR * pucRandval;
    WE_UINT8   ucHashAlg;
    St_SecWtlsKeyExchParams stParam;
}St_SecWtlsKeyExchange;

typedef struct tagSt_SecRemovePeer
{
    WE_INT32   iTargetID;
    WE_UINT8 ucMasterSecretId;
}St_SecRemovePeer;

typedef struct tagSt_SecAttachPeer
{
    WE_UCHAR * pucAddress;
    WE_UCHAR * pucDomain;
    WE_UINT16  usDomainLen;
    WE_UINT16  usPortnum;
    WE_UINT16  usAddressLen;
    WE_UINT8   ucConnectionType;
    WE_UINT8   ucMasterSecretId;
}St_SecAttachPeer;

typedef struct tagSt_SecSearchPeer
{
    WE_INT32   iTargetID;
    WE_UCHAR * pucAddress;
    WE_UCHAR * pucDomain;
    WE_UINT16  usDomainLen;
    WE_UINT16  usPortnum;
    WE_UINT16  usAddressLen;
    WE_UINT8   ucConnectionType;
}St_SecSearchPeer;

typedef struct tagSt_SecEnableSession
{
    WE_UINT8 ucMasterSecretId;
    WE_UINT8 ucIsActive;
}St_SecEnableSession;

typedef struct tagSt_SecDisableSession
{
    WE_UINT8 ucMasterSecretId;
}St_SecDisableSession;

typedef struct tagSt_SecFetchSessionInfo
{
    WE_INT32 iTargetID;
    WE_UINT8 ucMasterSecretId;
}St_SecFetchSessionInfo;

typedef struct tagSt_SecUpdateSession
{
    WE_UCHAR * pucPrivateKeyId;
    WE_UCHAR * pucSessionId;
    WE_UINT32  uiCreationTime;
    WE_UINT8   ucMasterSecretId;
    WE_UINT8   ucSessionOptions;
    WE_UINT8   ucSessionIdLen;
    WE_UINT8   ucCipherSuite[2];
    WE_UINT8   ucCompressionAlg;
}St_SecUpdateSession;

/***************************************************************************************************
*   Prototype Declare Section
***************************************************************************************************/
WE_INT32 Sec_MsgSetupCon (WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iMasterSecretId, 
                          WE_INT32 iSecurityId, WE_INT32 iFullHandshake,
                          St_SecSessionInfo stSessionInfo);

WE_INT32 Sec_MsgStopCon (WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iSecurityId,
                         WE_UINT8 ucConnectionType);
WE_INT32 Sec_MsgEnableSession (WE_HANDLE hSecHandle, WE_INT32 iMasterSecretId, WE_UINT8 ucIsActive);

WE_INT32 Sec_MsgDisableSession (WE_HANDLE hSecHandle, WE_INT32 iMasterSecretId);
WE_INT32 Sec_MsgRemovePeer(WE_HANDLE hSecHandle, WE_INT32 iTargetID,WE_INT32 iMasterSecretId);

WE_INT32 Sec_MsgAttachPeerToSession (WE_HANDLE hSecHandle, WE_UINT8 ucConnectionType,
                                     const WE_UCHAR * pucAddress, WE_INT32 iAddressLen,
                                     const WE_CHAR * pcDomain, WE_UINT16 usPortnum,
                                     WE_INT32 iMasterSecretId);

WE_INT32 Sec_MsgSearchPeer (WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_UINT8 ucConnectionType,
                            const WE_UCHAR * pucAddress, WE_INT32 iAddressLen,
                            const WE_CHAR * pcDomain, WE_UINT16 usPortnum);
WE_INT32 Sec_MsgRenewSession (WE_HANDLE hSecHandle, WE_INT32 iMasterSecretId, 
                               WE_UINT8 ucSessionOptions, const WE_UCHAR * pucSessionId, 
                               WE_UINT8 ucSessionIdLen,  WE_UINT8 aucCipherSuite[2],
                               WE_UINT8 ucCompressionAlg, const WE_UCHAR * pucPrivateKeyId,
                               WE_UINT32 uiCreationTime);
WE_INT32 Sec_MsgGetSessionInfo (WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iMasterSecretId);


WE_INT32 Sec_MsgWtlsGetPrfResult(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_UINT8 ucAlg, 
                                WE_INT32 iMasterSecretId, const WE_UCHAR * pucSecret, WE_INT32 iSecretLen,
                                const WE_CHAR * pcLabel, const WE_UCHAR * pucSeed, WE_INT32 iSeedLen,
                                WE_INT32 iOutputLen);

WE_INT32 Sec_MsgWtlsVerifySvrCert(WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                                     const WE_UCHAR *pucBuf, WE_INT32 iBufLen,
                                     const WE_UCHAR *pucAddr, WE_INT32 iAddrLen,
                                     const WE_CHAR * pcDomain);
WE_INT32 Sec_MsgWtlsGetCipherSuite (WE_HANDLE hSecHandle, WE_INT32 iTargetID);

WE_INT32 Sec_MsgWtlsKeyExchange (WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                 St_SecWtlsKeyExchParams stParam, WE_UINT8 ucHashAlg, 
                                 const WE_UCHAR * pucRandval);
WE_INT32 Sec_MsgWtlsGetUsrCert (WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                   const WE_UCHAR * pucBuf, WE_INT32 iBufLen);

WE_INT32 Sec_MsgWtlsCompSign(WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                            const WE_UCHAR * pucKeyId, WE_INT32 iKeyIdLen,
                            const WE_UCHAR * pucBuf, WE_INT32 iBufLen);




#endif
