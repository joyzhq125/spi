/*==================================================================================================
    HEADER NAME : iSecB.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
    
    
    GENERAL DESCRIPTION
        In this file,define the interface function prototype ,and the related structure 
        used in these interfaces.This is a public head file of isecb module.
    
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

#ifndef _ISECB_H_
#define _ISECB_H_

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/

/************************************************************************/
/* Response Event                                                                                                   */
/************************************************************************/
#define    G_GENKEYPAIR_RESP               0x20
#define    G_WTLSCURCLASS_RESP             0x21
#define    G_VIEWGETCERT_RESP              0x22
#define    G_DELCERT_RESP                  0x23
#define    G_GETCERTNAME_RESP              0x25
#define    G_GETSESSIONINFO_RESP           0x26
#define    G_GETCURSVRCERT_RESP            0x27
#define    G_GETCONTRACT_RESP              0x29
#define    G_DELCONTRACT_RESP              0x2A
#define    G_USERCERTREQ_RESP              0x2F
#define    G_CHANGECERTSTATE_RESP     0x2E   /*added by Bird 061128*/

/***************************************************************************
* The result value from response
***************************************************************************/
#define G_SEC_BUSY                          0x11

#define G_SEC_PRIVKEY_NOTFOUND              0x12

#define G_SEC_HASHNOTVERIFIED                 0x13
#define G_SEC_REACHED_USER_CERT_LIMIT       0x14


/************************************************************************/
/* Dialog Event                                                                                                         */
/************************************************************************/
#define    G_Evt_SELECTCERT     0x30
#define    G_Evt_CONFIRM        0x31
#define    G_Evt_NAMECONFIRM    0x32
#define    G_Evt_HASH                      0x38
#define    G_Evt_SHOWCERTCONTENT           0x39
#define    G_Evt_SHOWCERTLIST              0x3A
#define    G_Evt_SHOWSESSIONCONTENT        0x3B
#define    G_Evt_SHOWCONTRACTLIST          0x3C
#define    G_Evt_STORECERT                 0x3D
#define    G_Evt_SHOWCONTRACTCONTENT       0x3E
/*added by Bird 070208*/
#define    G_Evt_SHOWCURSVRCERT                   0x3F
/***************************************************
* Certificate types used in sec module
****************************************************/
#define G_SEC_USER_CERT                    0x01
#define G_SEC_ROOT_CERT                    0x02  

/***************************************************
* Key types used in sec module
****************************************************/
#define G_SEC_NONREPUDIATION_KEY           0x01
#define G_SEC_AUTHENTICATION_KEY           0x02

/************************************************************************
  hash With RSA Encryption                                               
************************************************************************/
#define G_SEC_SIG_RSA_SHA                    0x01
#define G_SEC_SIG_RSA_MD5                    0x02
#define G_SEC_SIG_RSA_MD2                    0x03

/************************************************************************
  usage of key                                               
************************************************************************/
#define    G_SEC_CERT_USAGE_SERVER_AUTH        0x01
#define    G_SEC_CERT_USAGE_CLIENT_AUTH        0x02
#define    G_SEC_CERT_USAGE_CODE_SIGN          0x04
#define    G_SEC_CERT_USAGE_EMAIL_PROTECT      0x08

/************************************************************************/
/* string constants                                                                                                    */
/************************************************************************/

/* confirmation constants */

/*"Can't verify the certificate. Continue to save it?"*/
#define G_SEC_ID_CONFIRM_CANNOT_VERIFY_CERT   ( 0x11) 
/*"A certificate having the same valid date already exists. Overwrite it?"*/
#define G_SEC_ID_CONFIRM_SAME_DATE_CERT_EXIST ( 0x12) 
/*"A certificate having an older valid date already exists. Overwrite it?"*/
#define G_SEC_ID_CONFIRM_OLD_DATE_CERT_EXIST  ( 0x13) 
/*"A certificate having a newer valid date already exists. Overwrite it?"*/
#define G_SEC_ID_CONFIRM_NEW_DATE_CERT_EXIST  ( 0x14) 
/*"Save successfully."*/
#define G_SEC_ID_CONFIRM_SAVE_SUCCESS         ( 0x15) 
/*"Can't save the certificate."*/
#define G_SEC_ID_CONFIRM_CANNOT_STORE_CERT    ( 0x16) 
/*"Save the certificate?"*/
#define G_SEC_ID_CONFIRM_SAVE_CERT            ( 0x17) 

/* Session  */
/*"WTLS connection type"*/
#define G_SEC_ID_SESSION_WTLS                 ( 0x18) 

/*"HMAC_MD5 algorithm"*/
#define G_SEC_ID_SESSION_MD5                  ( 0x19) 
/*"HMAC_SHA-1 algorithm"*/
#define G_SEC_ID_SESSION_SHA1                 ( 0x1a) 

/*"Cipher Method: NULL"*/
#define G_SEC_ID_SESSION_BULK_NULL            ( 0x1b) 
/*"Cipher Method: RC5"*/
#define G_SEC_ID_SESSION_BULK_RC5             ( 0x1c) 
/*"Cipher Method: RC5 40"*/
#define G_SEC_ID_SESSION_BULK_RC5_40          ( 0x1d) 
/*"Cipher Method: RC5 56"*/
#define G_SEC_ID_SESSION_BULK_RC5_56          ( 0x1e) 
/*"Cipher Method: RC5 64"*/
#define G_SEC_ID_SESSION_BULK_RC5_64          ( 0x1f) 
/*"Cipher Method: Triple DES"*/
#define G_SEC_ID_SESSION_BULK_3DES            ( 0x20) 

/*"KeyExchange : NULL"*/
#define G_SEC_ID_SESSION_KEYEXCH_NULL         ( 0x21) 
/*"KeyExchange : RSA ANON"*/
#define G_SEC_ID_SESSION_KEYEXCH_RSA_ANON     ( 0x22) 
/*"KeyExchange : RSA ANON 512"*/
#define G_SEC_ID_SESSION_KEYEXCH_RSA_ANON_512 ( 0x23) 
/*"KeyExchange : RSA ANON 768"*/
#define G_SEC_ID_SESSION_KEYEXCH_RSA_ANON_768 ( 0x24) 
/*"KeyExchange : RSA"*/
#define G_SEC_ID_SESSION_KEYEXCH_RSA          ( 0x25) 
/*"KeyExchange : RSA 512"*/
#define G_SEC_ID_SESSION_KEYEXCH_RSA_512      ( 0x26) 
/*"KeyExchange : RSA 768"*/
#define G_SEC_ID_SESSION_KEYEXCH_RSA_768      ( 0x27) 

/* Hash  */
/*"Input hash for name:"*/
#define G_SEC_ID_INPUT_HASH                   ( 0x34) 
/*"The inputed hash is wrong. Input again:"*/
#define G_SEC_ID_ERROR_HASH                   ( 0x35) 

/* Month  */
/*"January"*/
#define G_SEC_ID_JANUARY                      ( 0x36) 
/*"February"*/
#define G_SEC_ID_FEBRUARY                     ( 0x37) 
/*"March"*/
#define G_SEC_ID_MARCH                        ( 0x38) 
/*"April"*/
#define G_SEC_ID_APRIL                        ( 0x39) 
/*"May"*/
#define G_SEC_ID_MAY                          ( 0x3a) 
/*"June"*/
#define G_SEC_ID_JUNE                         ( 0x3b) 
/*"July"*/
#define G_SEC_ID_JULY                         ( 0x3c)
/*"August"*/ 
#define G_SEC_ID_AUGUST                       ( 0x3d) 
/*"September"*/
#define G_SEC_ID_SEPTEMBER                    ( 0x3e) 
/*"October"*/
#define G_SEC_ID_OCTOBER                      ( 0x3f) 
/*"November"*/
#define G_SEC_ID_NOVEMBER                     ( 0x40) 
/*"December"*/
#define G_SEC_ID_DECEMBER                     ( 0x41) 


/*******************************************************************************
*   Type Define Section
*******************************************************************************/

/************************************************************************/
/* structure                                                                                                            */
/************************************************************************/
typedef struct  tagSt_SecCertContent
{  
   WE_UINT32   uiCertLen;
   WE_CHAR  *  pcMime; 
   WE_UCHAR *  pucCertData;
}St_SecCertContent;

typedef struct  tagSt_SecUserCertReqInfo
{
   WE_INT32     iTargetID;
   WE_UCHAR  *  pucCountry; 
   WE_UCHAR  *  pucProvince;
   WE_UCHAR  *  pucCity;
   WE_UCHAR  *  pucCompany;
   WE_UCHAR  *  pucDepartment;
   WE_UCHAR  *  pucName;
   WE_UCHAR  *  pucEMail;
   WE_UCHAR     ucCertUsage;
   WE_UINT16    usCountryLen;
   WE_UINT16    usProvinceLen;
   WE_UINT16    usCityLen;
   WE_UINT16    usCompanyLen;
   WE_UINT16    usDepartmentLen;
   WE_UINT16    usNameLen;
   WE_UINT16    usEMailLen;
   WE_UCHAR     ucKeyUsage;
   WE_UCHAR     ucSignType;
}St_SecUserCertReqInfo;

/************************************************************************/
/* Response Structure                                                                                               */
/************************************************************************/
typedef struct tagSt_SecGenKeyPairResp
{
   WE_INT32   iTargetID;
   WE_UCHAR   *pucPublicKey;
   WE_UCHAR   *pucPKHashSig;
   WE_UINT16  usResult;
   WE_UINT16  usPublicKeyLen;
   WE_UINT16  usPKHashSigLen;
   WE_UINT8   ucKeyType; 
}St_SecGenKeyPairResp;

typedef struct tagSt_SecGetWtlsCurClassResp
{
   WE_INT32 iTargetID; 
   WE_UINT8 ucSecClass;
   WE_UINT8 ucSessionInfoReady;
   WE_UINT8 ucCertReady;
}St_SecGetWtlsCurClassResp;


typedef struct tagSt_SecGetSessionInfoResp
{
   WE_INT32  iTargetID;
   WE_UINT16 usResult;
}St_SecGetSessionInfoResp;


typedef struct tagSt_SecGetCertNameListResp
{
   WE_INT32  iTargetID; 
   WE_UINT16 usResult;
}St_SecGetCertNameListResp;

typedef struct tagSt_SecGetContractsListResp
{
   WE_INT32  iTargetID;
   WE_UINT16 usResult;
}St_SecGetContractsListResp;

typedef struct tagSt_SecGetCertContentResp
{
   WE_INT32   iTargetID;
   WE_UCHAR * pucCert;
   WE_UINT16  usResult;
   WE_UINT16  usCertLen;
   WE_UINT8   ucCertId;
}St_SecGetCertContentResp;

typedef struct tagSt_SecRemCertResp
{
   WE_INT32  iTargetID;
   WE_UINT16 usResult;
   WE_UINT8  ucCertId;
}St_SecRemCertResp;


typedef struct tagSt_SecGetCurSvrCertResp
{
   WE_INT32  iTargetID;
   WE_UINT16 usResult;
}St_SecGetCurSvrCertResp;

typedef struct tagSt_SecDelContractResp
{
   WE_INT32  iTargetID;
   WE_UINT16 usResult;
}St_SecDelContractResp;

typedef struct tagSt_SecUserCertRequestResp
{
   WE_INT32  iTargetID;    
   WE_INT32  iResult;
   WE_UCHAR* pucCertReqMsg;
   WE_UINT32 uiCertReqMsgLen;   
}St_SecUserCertRequestResp;

/*added by Bird 061128*/
typedef struct tagSt_ChangeCertStateResp
{
    WE_INT32 iTargetID;
    WE_INT32 iResult;
    WE_UINT8 ucState;
}St_ChangeCertStateResp;

/**************************************************
* structure used in i interface for user event
**************************************************/
/*delete by Bird 061208*/
/*St_Pin,St_ChangePin,St_CreatePin*/ 

typedef struct tagSt_Confirm
{
    WE_INT32  iConfirmStrId;
}St_Confirm;

typedef struct tagSt_ConfirmAction
{
    WE_BOOL bIsOk;
}St_ConfirmAction;

typedef struct tagSt_NameConfirm
{
    WE_CHAR* pcCertName;
}St_NameConfirm;

typedef struct tagSt_NameConfirmAction
{
    WE_BOOL bIsOk;
} St_NameConfirmAction;


typedef struct tagSt_SelectCert
{
    WE_INT32 nbrOfCerts; 
    WE_CHAR** ppcCertName;  
}St_SelectCert;

typedef struct tagSt_SelectCertAction
{
    WE_INT32  iSelId;
    WE_BOOL   bIsOk;
}St_SelectCertAction;

typedef struct tagSt_Hash
{
   WE_INT32 iInfoId;
   WE_CHAR* pcDisplayName;
}St_Hash;

typedef struct tagSt_HashAction
{
   WE_CHAR* pcHashValue;
   WE_BOOL bIsOk;
}St_HashAction;

typedef struct tagSt_ShowCertContent
{
   WE_CHAR* pcIssuerStr;
   WE_CHAR* pcSubjectStr;
   WE_INT32 iNotBefMonth;
   WE_CHAR* pcNotBefStr;
   WE_INT32 iNotAftMonth;
   WE_CHAR* pcNotAftStr;   
   /*add by bird for UI spec new feature 061107*/
   WE_UCHAR* pucSerialNumber;/*serial number*/
   WE_UCHAR* pucAlg;/*signature algorthm*/
   WE_UCHAR* pucPubKey;/*public key*/
       
}St_ShowCertContent;

typedef struct tagSt_ShowCertContentAction
{
   WE_BOOL bIsOk;
} St_ShowCertContentAction;

/*added by bird 061120 for St_ShowCertList certificate type  */
typedef enum tagE_CertType
{
    E_X509_CA_CERT   = 0x03,  /*x509 ca cert ,ssl*/
    E_WTLS_CA_CERT  = 0x04    /*wtls ca cert*/
}E_CertType;
typedef struct tagSt_ShowCertList
{
   WE_INT32 nbrOfCerts;
   WE_CHAR** ppcCertName;  
   WE_UINT8*  pucAble; /*0: disable, 1: enable*/
   E_CertType*  pueCertType;
}St_ShowCertList;

/*added by bird 061120 for St_ShowCertListAction right key type when select rignt key  */
typedef enum tagE_RKeyType
{
    E_KEY_DISABLE = 0x00,     /*disable*/
    E_KEY_ENABLE , /*enable*/
    E_KEY_CANCEL       /*Cancel*/
}E_RKeyType;

typedef struct tagSt_ShowCertListAction
{
   WE_INT32 iSelId;
   WE_BOOL  bIsOk;
   E_RKeyType eRKeyType; /*when bIsOk is Flase,then it is used*/
} St_ShowCertListAction;

typedef struct tagSt_ShowSessionContent
{
   WE_INT32 iConnTypeId;
   WE_INT32 iHmacId;
   WE_CHAR* pcHmacKeyLenStr;
   WE_INT32 iEncralgId;
   WE_CHAR* pcEncrKeyLenStr;
   WE_INT32 iKeyExchId;
   WE_CHAR* pcKeyExchKeyLenStr;
}St_ShowSessionContent;

typedef struct tagSt_ShowContractsList
{
   WE_INT32  nbrOfContract; 
   WE_INT32* piMonthID;
   WE_CHAR** ppcTime ;
}St_ShowContractsList;

typedef struct tagSt_ShowContractsListAction
{
   WE_INT32 iSelId;
   WE_BOOL  bIsOk;
}St_ShowContractsListAction;

typedef struct tagSt_StoreCert
{
   WE_CHAR* pcIssuerStr;
   WE_CHAR* pcSubjectStr;
   WE_INT32 iNotBefMonth;
   WE_CHAR* pcNotBefStr;
   WE_INT32 iNotAftMonth;
   WE_CHAR* pcNotAftStr;
   WE_CHAR  cCertType;
}St_StoreCert;

typedef struct tagSt_StoreCertAction
{
   WE_CHAR* pcText;
   WE_BOOL  bIsOk;
} St_StoreCertAction;

typedef struct tagSt_ShowContractContent
{
   WE_CHAR* pcContractStr;
   WE_INT32  iMonthID;
   WE_CHAR* pcTimeStr;
   WE_CHAR* pcSignatureStr;
}St_ShowContractContent;

typedef struct tagSt_ShowContractContentAction
{
   WE_BOOL bIsOk;
} St_ShowContractContentAction;

/*The following is ISecB interface and struct definition. */
typedef struct ISecB ISecB;

AEEINTERFACE(ISecB) 
{
   INHERIT_IBase(ISecB);    
   /*register callback function*/
   WE_UINT32 (*RegSecClientEvtCB)(ISecB * pMe,WE_HANDLE hPrivData,Fn_ISecEventHandle pcbSecEvtFunc);
   /* for Browser application*/
   WE_UINT32 (*GetCertNameList)(ISecB * pMe, WE_INT32 iTargetID, WE_INT32 iCertType);
   WE_UINT32 (*GenKeyPair)(ISecB * pMe, WE_INT32 iTargetID, WE_UINT8 ucKeyType);
   WE_UINT32 (*GetCurSvrCert)(ISecB * pMe, WE_INT32 iTargetID, WE_INT32 iSecId);
   WE_UINT32 (*GetSessionInfo)(ISecB * pMe, WE_INT32 iTargetID, WE_INT32 iSecId);
   WE_UINT32 (*GetWtlsCurClasss)(ISecB * pMe, WE_INT32 iTargetID, WE_INT32 iSecId);
   WE_UINT32 (*GetContractList)(ISecB * pMe, WE_INT32 iTargetID);
   /*for transferring certificate*/
   WE_UINT32 (*TransferCert)(ISecB * pMe, St_SecCertContent stCertContent);

   /* generate request of user certificate in Base64 DER code. */
   WE_UINT32 (*GetRequestUserCert)(ISecB * pMe, St_SecUserCertReqInfo stShowContractContent);
  
   /*user event response interface*/
   WE_UINT32 (*EvtConfirmAction)(ISecB * pMe,WE_INT32 iTargetID,St_ConfirmAction stConfirm);
   WE_UINT32 (*EvtNameConfirmAction)(ISecB * pMe,WE_INT32 iTargetID,St_NameConfirmAction stNameConfirm);
   WE_UINT32 (*EvtHashAction)(ISecB * pMe,WE_INT32 iTargetID,St_HashAction stHash);
   WE_UINT32 (*EvtShowCertContentAction)(ISecB * pMe,WE_INT32 iTargetID,St_ShowCertContentAction stShowCertContent);
   WE_UINT32 (*EvtShowCertListAction)(ISecB * pMe,WE_INT32 iTargetID,St_ShowCertListAction stShowCertList);
   WE_UINT32 (*EvtShowContractsListAction)(ISecB * pMe,WE_INT32 iTargetID,St_ShowContractsListAction stShowContractsList);
   WE_UINT32 (*EvtStoreCertAction)(ISecB * pMe,WE_INT32 iTargetID,St_StoreCertAction stStoreCert);
   WE_UINT32 (*EvtShowContractContentAction)(ISecB * pMe,WE_INT32 iTargetID,St_ShowContractContentAction stShowContractContent);
    
};
/*-------------------------------------------------------------------
Extension structure. All variables in here are reference via "pMe->"
-------------------------------------------------------------------*/
struct ISecB
{
   AEEVTBL(ISecB) *pvt;
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

/*Public ISecB interface opened to external*/
#define ISecB_AddRef(p)                             AEEGETPVTBL((p),ISecB)->AddRef((p))
#define ISecB_Release(p)                            AEEGETPVTBL((p),ISecB)->Release((p))

#define ISecB_RegSecClientEvtCB(p,v,c)              AEEGETPVTBL((p),ISecB)->RegSecClientEvtCB((p),(v),(c))

#define ISecB_GetCertNameList(p,i,c)                AEEGETPVTBL((p),ISecB)->GetCertNameList((p),(i),(c))
#define ISecB_GenKeyPair(p,i,k)                     AEEGETPVTBL((p),ISecB)->GenKeyPair((p),(i),(k))
#define ISecB_GetCurSvrCert(p,i,s)                  AEEGETPVTBL((p),ISecB)->GetCurSvrCert((p),(i),(s))
#define ISecB_GetSessionInfo(p,i,s)                 AEEGETPVTBL((p),ISecB)->GetSessionInfo((p),(i),(s))
#define ISecB_GetWtlsCurClass(p,i,s)                AEEGETPVTBL((p),ISecB)->GetWtlsCurClasss((p),(i),(s))
#define ISecB_GetContractList(p,i)                  AEEGETPVTBL((p),ISecB)->GetContractList((p),(i))

#define ISecB_TransferCert(p,t)                     AEEGETPVTBL((p),ISecB)->TransferCert((p),(t))

#define ISecB_GetRequestUserCert(p,s)               AEEGETPVTBL((p),ISecB)->GetRequestUserCert((p),(s))

#define ISecB_EvtConfirmAction(p,i,s)                 AEEGETPVTBL((p),ISecB)->EvtConfirmAction((p),(i),(s))
#define ISecB_EvtNameConfirmAction(p,i,s)             AEEGETPVTBL((p),ISecB)->EvtNameConfirmAction((p),(i),(s))
#define ISecB_EvtHashAction(p,i,s)                    AEEGETPVTBL((p),ISecB)->EvtHashAction((p),(i),(s))
#define ISecB_EvtShowCertContentAction(p,i,s)         AEEGETPVTBL((p),ISecB)->EvtShowCertContentAction((p),(i),(s))
#define ISecB_EvtShowCertListAction(p,i,s)            AEEGETPVTBL((p),ISecB)->EvtShowCertListAction((p),(i),(s))
#define ISecB_EvtShowContractsListAction(p,i,s)       AEEGETPVTBL((p),ISecB)->EvtShowContractsListAction((p),(i),(s))
#define ISecB_EvtStoreCertAction(p,i,s)               AEEGETPVTBL((p),ISecB)->EvtStoreCertAction((p),(i),(s))
#define ISecB_EvtShowContractAction(p,i,s)            AEEGETPVTBL((p),ISecB)->EvtShowContractContentAction((p),(i),(s))

#endif/*_ISECB_H_*/


