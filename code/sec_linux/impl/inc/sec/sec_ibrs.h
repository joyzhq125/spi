/*==================================================================================================
    HEADER NAME : sec_ibrs.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION    
    
    GENERAL DESCRIPTION
        In this file,define the function prototype , to support the browser engine and
        browser application.
    
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

#ifndef _SEC_IBRS_H
#define _SEC_IBRS_H

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
#define M_SEC_PRIVATE_NONREPKEY           0x01
#define M_SEC_PRIVATE_AUTHKEY             0x02
#define M_SEC_CERTIFICATE_WTLS_TYPE       0x01
#define M_SEC_CERTIFICATE_X509_TYPE       0x02
#define M_SEC_CRPT_OK                     0x00
/**************************************************
 * SignText
 **************************************************/
/* Values for certificateType parameter*/
#define M_SEC_SIGNTEXT_WTLSCERT           0x02
#define M_SEC_SIGNTEXT_X509CERT           0x03
#define M_SEC_SIGNTEXT_URLCERT            0x05

/* Values for err parameter */
#define M_SEC_SIGNTEXT_OK                 0x00
#define M_SEC_SIGNTEXT_MISSCERT           0x01
#define M_SEC_SIGNTEXT_ABORT              0x02
#define M_SEC_SIGNTEXT_GENERALERROR       0x03
/* Values for keyIdType parameter */
#define M_SEC_SIGN_NO_KEY                 0x00
#define M_SEC_SIGN_SHA_KEY                0x01
#define M_SEC_SIGN_SHA_CA_KEY             0x02

/* Values for options parameter */
#define M_SEC_SIGNTEXT_RETURNHASHEDKEY    0x02
#define M_SEC_SIGNTEXT_RETURNCERT         0x04

/* Values for algorithm parameter  */
#define M_SEC_SIGNTEXT_RSAALG             0x01

/*******************************************************************************
*   Type Define Section
*******************************************************************************/
typedef struct tagSt_SecSignText
{
    WE_INT32  iTargetID;
    WE_INT32  iSignId;
    WE_CHAR * pcText;
    WE_CHAR * pcData;
    WE_UINT16 usDataLen;
    WE_UINT8  ucKeyIdType;
    WE_CHAR * pcKeyId;
    WE_UINT16 usKeyIdLen;
    WE_UINT8  ucOptions;
}St_SecSignText;

typedef struct tagSt_SecContractInfo
{
    WE_CHAR * pcContract;
    WE_CHAR * pcSignature;
    WE_UINT16 usSignatureLen;
    WE_UINT32 uiTime;
    WE_UINT16 usContractId;
}St_SecContractInfo;

typedef St_SecCrptPubKeyRsa St_SecCrptPubKey;

typedef struct tagSt_SecCrptPrivKey
{
    WE_UINT16    usBufLen;
    WE_UCHAR    * pucBuf;
}St_SecCrptPrivKey;

typedef struct tagSt_SecCrptEncryptPkcResp
{
    WE_INT32    iTargetID;
    WE_INT16    sResult;  
    WE_UCHAR    * pucBuf;
    WE_INT16    sBufLen;
}St_SecCrptEncryptPkcResp;

typedef struct tagSt_SecCrptDecryptPkcResp
{
    WE_INT32    iTargetID;
    WE_INT16    sResult;
    WE_UCHAR    *pucBuf;
    WE_INT16    sBufLen;
}St_SecCrptDecryptPkcResp;

typedef struct tagSt_SecCrptVerifySignatureResp
{
    WE_INT32    iTargetID;
    WE_INT16    sResult;
}St_SecCrptVerifySignatureResp;

typedef struct tagSt_SecCrptComputeSignatureResp
{
    WE_INT32    iTargetID;
    WE_INT16    sResult;
    WE_UCHAR    * pucSig;
    WE_INT16    sSigLen;
}St_SecCrptComputeSignatureResp;

typedef struct tagSt_SecCrptGenKeypairResp
{
    WE_INT32            iTargetID;
    WE_INT16            sResult;
    WE_UINT8            ucPkcAlg;
    St_SecCrptPubKeyRsa stPubKey;
    St_SecCrptPrivKey   stPrivKey;
}St_SecCrptGenKeypairResp;

typedef struct tagSt_SecSaveCert
{
    WE_INT32   iTargetID;
    WE_CHAR *  pcContentType; 
    WE_UCHAR * pucCert;
    WE_UINT16  usCertLen; 
    WE_UCHAR * pucCertURL;
}St_SecSaveCert;

typedef struct tagSt_SecSaveCertResp
{
    WE_INT32 iTargetID;
    WE_UINT16 usResult;
}St_SecSaveCertResp;


typedef struct tagSt_SecTrustedCaInfo
{
    WE_INT32   iTargetID; 
    WE_CHAR *  pcContentType;
    WE_UCHAR * pucTrustedCAInfoStruct;
    WE_UINT16  usTrustedCAInfoStructLen;
}St_SecTrustedCaInfo;

typedef struct tagSt_SecCertDelivery
{
    WE_INT32   iTargetID;
    WE_UCHAR * pucCertResponse;
    WE_UINT16  usCertResponseLen;
}St_SecCertDelivery;

typedef struct tagSt_SecGetCertNameList
{
    WE_INT32 iTargetID; 
    WE_UINT8 ucCertOptions;
}St_SecGetCertNameList;

typedef struct tagSt_SecVerifyCertChain
{  
    WE_INT32 iTargetID;
    St_SecAsn1Certificate * pstCerts;
    WE_UINT8 ucNumCerts;
}St_SecVerifyCertChain;

typedef struct tagSt_SecGenKeyPair
{
    WE_INT32 iTargetID;
    WE_UINT8 ucKeyType;
    WE_UINT8 ucAlg;
}St_SecGenKeyPair;

typedef struct tagSt_SecDelKeysUserCerts
{
    WE_INT32 iTargetID;
    WE_UINT8 ucKeyType;
}St_SecDelKeysUserCerts;

typedef struct tagSt_SecGetPublicKey
{
    WE_INT32 iTargetID;
    WE_UINT8 ucKeyType;
}St_SecGetPublicKey;
 
typedef struct tagSt_SecModifyPriKeyPin
{
    WE_INT32 iTargetID;
    WE_UINT8 ucKeyType;
}St_SecModifyPriKeyPin;

typedef struct tagSt_SecViewCurCert
{
    WE_INT32 iTargetID;
    WE_INT32 iSecurityId;
}St_SecViewCurCert;

typedef struct tagSt_SecViewSessionInfo
{
    WE_INT32 iTargetID;
    WE_INT32 iSecurityId;
}St_SecViewSessionInfo;

typedef struct tagSt_SecGetSecCurClass
{
    WE_INT32 iTargetID; 
    WE_INT32 iSecurityId;
}St_SecGetSecCurClass;

typedef struct tagSt_SecClrSession
{
    WE_INT32 iTargetID;
}St_SecClrSession;

typedef struct tagSt_SecGetContractInfo
{
    WE_INT32 iTargetID;
}St_SecGetContractInfo;

/*no use*/
typedef struct tagSt_SecGetContracts
{
    WE_INT32 iWid;
}St_SecGetContracts;

typedef struct tagSt_SecGetCertContent
{
    WE_INT32 iTargetID;
    WE_UINT8 ucCertId;
}St_SecGetCertContent;

typedef struct tagSt_SecChangeWtlsCertAble
{
    WE_INT32 iTargetID;
    WE_UINT32 uiCertId;
}St_SecChangeWtlsCertAble;

typedef struct tagSt_SecRemoveCert
{
    WE_INT32 iTargetID;
    WE_UINT8 ucCertId;
}St_SecRemoveCert;

typedef struct tagSt_SecRemoveContract
{
    WE_INT32 iTargetID;
    WE_UINT8 ucContractId;
}St_SecRemoveContract;


typedef struct tagSt_SecShowCertListAction
{
    WE_INT32                    iTargetID;
    St_ShowCertListAction  stShowCertList;
}St_SecShowCertListAction;

typedef struct tagSt_SecShowCertContAction
{
    WE_INT32                            iTargetID;
    St_ShowCertContentAction    stShowCertCont;
}St_SecShowCertContAction;

typedef struct tagSt_SecShowContractListAction
{
    WE_INT32                            iTargetID;
    St_ShowContractsListAction  stShowContractList;
}St_SecShowContractListAction;

typedef struct tagSt_SecShowContractContAction
{
    WE_INT32                               iTargetID;
    St_ShowContractContentAction stShowContractCont;
}St_SecShowContractContAction;

/*added by Bird 070130*/
typedef struct tagSt_SecHashAction
{
    WE_INT32                            iTargetID;
    St_HashAction                      stHash;
}St_SecHashAction;

typedef struct tagSt_SecStoreCertAction
{
    WE_INT32                                   iTargetID;
    St_StoreCertAction                      stStoreCert;
}St_SecStoreAction;

typedef struct tagSt_SecConfirmAction
{
    WE_INT32                                   iTargetID;
    St_ConfirmAction                        stConfirm;
}St_SecConfirmAction;

typedef struct tagSt_SecNameConfirmAction
{
    WE_INT32                                   iTargetID;
    St_NameConfirmAction                stNameConfirm;
}St_SecNameConfirmAction;

typedef struct tagSt_SecSelCertAction
{
    WE_INT32                                   iTargetID;
    St_SelectCertAction                     stSelCert;
}St_SecSelCertAction;

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
WE_INT32 Sec_MsgSignText (WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iSignId, const WE_CHAR * pcText,
                          const WE_CHAR * pcData, WE_INT32 iDataLen, WE_INT32 iKeyIdType, 
                          const WE_CHAR * pcKeyId, WE_INT32 iKeyIdLen, WE_INT32 iOptions);

WE_INT32 Sec_MsgCertSave(WE_HANDLE hSecHandle, WE_INT32 iTargetID, const WE_CHAR * pcContentType, 
                         const WE_UCHAR * pucCert, WE_UINT16 usCertLen, const WE_UCHAR * pucCertURL);

WE_INT32 Sec_MsgTrustedCAInfoSave(WE_HANDLE hSecHandle, WE_INT32 iTargetID, const WE_CHAR * pcContentType,
                                  const WE_UCHAR * pucTrustedCAInfoStruct, WE_UINT16 usTrustedCAInfoStructLen);

WE_INT32 Sec_MsgDeliveryCert(WE_HANDLE hSecHandle, WE_INT32 iTargetID, const WE_UCHAR * pucCertResponse, 
                             WE_UINT16 usCertResponseLen);

WE_INT32 Sec_MsgGetCertNameList(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iCertOptions);


WE_INT32 Sec_MsgKeyPairGen(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_UINT8 ucKeyType, 
                           WE_UINT8 ucAlg);

WE_INT32 Sec_MsgGetPublicKey(WE_HANDLE hSecHandle, WE_INT32 iTargetID,WE_UINT8 ucKeyType);

WE_INT32 Sec_MsgModifyPin(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_UINT8 ucKeyType);

WE_INT32 Sec_MsgViewCurCert(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iSecurityId);

WE_INT32 Sec_MsgViewInfoSession(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iSecurityId);

WE_INT32 Sec_MsgGetSecCurClass(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iSecurityId); 

WE_INT32 Sec_MsgClrSession(WE_HANDLE hSecHandle, WE_INT32 iTargetID);

WE_INT32 Sec_MsgGetContractsInfos(WE_HANDLE hSecHandle, WE_INT32 iTargetID);

WE_INT32 Sec_MsgDeleteContract(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iContractId);

WE_INT32 Sec_MsgViewAndGetCert(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iCertId);

WE_INT32 Sec_MsgDeleteCert(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iCertId);

WE_INT32 Sec_MsgChangeWTLSCertAble(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iCertId);

WE_INT32 
Sec_MsgEvtShowContractsListAction(WE_HANDLE hSecHandle, WE_INT32 iTargetId,St_ShowContractsListAction stStoredContracts);

WE_INT32 
Sec_MsgEvtShowContractsContentAction(WE_HANDLE hSecHandle, WE_INT32 iTargetId,St_ShowContractContentAction stStoredContracts);

WE_INT32 
Sec_MsgEvtShowCertListAction(WE_HANDLE hSecHandle, WE_INT32 iTargetId,St_ShowCertListAction stShowCertList);

WE_INT32 
Sec_MsgEvtShowCertsContentAction(WE_HANDLE hSecHandle, WE_INT32 iTargetId,St_ShowCertContentAction stShowCertCont);

WE_INT32 
Sec_MsgEvtHashAction(WE_HANDLE hSecHandle, WE_INT32 iTargetId,St_HashAction stHash);

WE_INT32 
Sec_MsgEvtStoreCertAction(WE_HANDLE hSecHandle ,WE_INT32 iTargetId,St_StoreCertAction stStoreCert);

WE_INT32 
Sec_MsgEvtConfirmAction(WE_HANDLE hSecHandle ,WE_INT32 iTargetId,St_ConfirmAction stConfirm);

WE_INT32 
Sec_MsgEvtNameConfirmAction(WE_HANDLE hSecHandle ,WE_INT32 iTargetId,
                                                    St_NameConfirmAction stNameConfirm);

WE_INT32 
Sec_MsgEvtSelCertAction(WE_HANDLE hSecHandle ,WE_INT32 iTargetId,
                                                    St_SelectCertAction stSelCert);


#endif
