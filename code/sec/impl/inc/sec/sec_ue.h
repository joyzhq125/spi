/*=====================================================================================
    FILE NAME :
        Sec_ue.h
    MODULE NAME :
        sec
    GENERAL DESCRIPTION

    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2002-2006 by TECHFAITH Software. All Rights Reserved.
    =======================================================================================
    Revision History
    Modification              Tracking
    Date       Author         Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-07-20 Bird           none      Init
  
=====================================================================================*/

/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/
#ifndef SEC_UE_H
#define SEC_UE_H
/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
#ifndef M_SEC_CFG_USE_CERT
#define M_SEC_CFG_USE_CERT
#endif

#ifndef M_SEC_CFG_USE_KEYS
#define M_SEC_CFG_USE_KEYS
#endif

/***************************************************/
/* Constants                                       */
/***************************************************/

/* Reasons to call dialog again */
#define M_SEC_UE_INPUT_TOO_SHORT 100
#define M_SEC_UE_INPUT_TOO_LONG  101

#define M_SEC_UE_INPUT_FALSE     102

#define M_SEC_UE_INPUT_MISIATCH  103

/* Define the different types of certificates used */
#define M_SEC_UE_USER_CERT 'U'
#define M_SEC_UE_CA_CERT   'C'

/* Friendly name is taken from the certificate,and is the name in the Identifier (see WTLS spec.)        */
#define M_SEC_CERT_NAME_WTLS 1
/* Friendly name is taken from the certificate ,  and is a X.509 distingubshed name       */
#define M_SEC_CERT_NAME_X509 2    
/* Friendly name is typed in by the user */                                
#define M_SEC_WRITTEN_NAME   3    


/*******************************************************************************
*   Type Define Section
*******************************************************************************/

/* Define the reasons for asking for the PIN code */
typedef enum tagE_SecPinReason
{
    E_SEC_OPEN_WIM,
    E_SEC_SIGN_TEXT,
    E_SEC_STORE_CERT,
    E_SEC_DELETE_CERT,
    E_SEC_CREATE_AUTH_KEY,
    E_SEC_CREATE_NONREP_KEY
} E_SecPinReason;


typedef struct tagSt_SecCertName
{
    void        *pvFriendlyName;
    WE_INT32    iCertId;
    WE_UINT16   usFriendlyNameLen;
    WE_INT16    sCharSet;
    WE_INT8     cNameType;
    WE_UINT8    ucAble;
    WE_UINT8    ucCertType;
} St_SecCertName;


/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/

WE_BOOL 
Sec_UeConfirm(WE_HANDLE   hSecHandle, WE_INT32 iTargetId,WE_INT32 iMsg, WE_INT32 iYesMsg, WE_INT32 iNoMsg);

/*
* Create an extended confirmation dialog with the supplied text, 
* message, yes message, and no message.
*/
WE_BOOL
Sec_UeSignTextConfirm(WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_INT32 iMsg, const WE_CHAR *pcText, 
                      WE_INT32 iYesMsg, WE_INT32 iNoMsg);

/*
* Create a warning dialog with the supplied message.
*/
WE_BOOL
Sec_UeWarning(WE_HANDLE   hSecHandle,WE_INT32 iTargetId, WE_INT32 iMsg);

/*
* Create a form with the supplied session info.
*/
WE_BOOL
Sec_UeSessionInfo(WE_HANDLE   hSecHandle,WE_INT32 iTargetId,St_SecSessionInfo stSessInf);

#ifdef M_SEC_CFG_USE_CERT
/*
* Create a menu with the supplied certificate names.
*/
WE_BOOL
Sec_UeShowCertList(WE_HANDLE   hSecHandle,WE_INT32 iTargetId, WE_INT32 iNbrOfCerts, 
                   St_SecCertName *pstCertNames);

/*
* Create a store certificate form with the supplied certificate fields.
*/
WE_BOOL
Sec_UeStoreCert(WE_HANDLE   hSecHandle, WE_INT32 iTargetId,WE_INT16 sIssuerCharSet, 
                void *pvIssuer, WE_INT32 iIssuerLen, 
                WE_UINT32 uiValidNotBefore, 
                WE_UINT32 uiValidNotAfter, 
                WE_INT16 sSubjectCharSet, 
                void *pvSubject, WE_INT32 iSubjectLen, WE_CHAR cCertType);


#endif /*M_SEC_CFG_USE_CERT*/

#ifdef M_SEC_CFG_USE_KEYS
WE_BOOL
Sec_UeSelectCert(WE_HANDLE   hSecHandle,WE_INT32 iTargetId, WE_INT32 iTitle, 
                 WE_INT32 iOkActionId, WE_INT32 iNbrOfCerts,
                 St_SecCertName *pstCertNames);
#endif /*M_SEC_CFG_USE_KEYS*/

WE_BOOL
Sec_UeNameConfirm(WE_HANDLE   hSecHandle, WE_INT32 iTargetId,WE_INT16 sCharSet, 
                  void *pvDisplayName, WE_INT32 iDisplayNameLen, 
                  WE_INT32 iMsg, WE_INT32 iYesMsg, WE_INT32 iNoMsg);

/*
* Create a form with the supplied certificate parts and cancel string.
*/
#ifdef M_SEC_CFG_USE_CERT
WE_BOOL
Sec_UeShowCertContent(WE_HANDLE hSecHandle,WE_INT32 iTargetId, WE_INT16 sIssuerCharSet, 
                              void *pvIssuer, WE_INT32 iIssuerLen,
                              WE_UINT32 iValidNotBefore, WE_UINT32 iValidNotAfter, 
                              WE_INT16 sSubjectCharSet, void *pvSubject, 
                              WE_INT32 iSubjectLen, WE_UCHAR *pucSerialNumber , 
                              WE_UINT16 usSerialNumberLen,WE_UCHAR* pucAlg,
                              WE_UCHAR* pucPubKey,WE_UINT16 usPubKeyLen,
                              WE_INT32 iCancelStr);
#endif /*M_SEC_CFG_USE_CERT*/

#ifdef M_SEC_CFG_USE_KEYS
/* 
* Create a PIN form with one string gadget, one input gadget
* and two actions in it. 
*/
WE_BOOL
Sec_UeGenPin(WE_HANDLE   hSecHandle, WE_INT32 iTargetId,WE_UINT8 ucKeyType, WE_INT32 iReason);

/* 
* Create a PIN form with three string gadgets, three input gadgets,
* and two actions in it. 
*/
WE_BOOL
Sec_UeChgPin(WE_HANDLE   hSecHandle, WE_INT32 iTargetId,WE_UINT8 ucKeyType, WE_INT32 iReason);

/* 
* Create a PIN form with one string gadget, one input gadget,
* and two actions in it. 
*/
WE_BOOL
Sec_UePin(WE_HANDLE   hSecHandle, WE_INT32 iTargetId,E_SecPinReason ePinCause, WE_INT32 iReason);

#endif

#ifdef M_SEC_CFG_USE_CERT
/* 
* Create a hash form with the supplied display name. 
*/
WE_BOOL
Sec_UeHash(WE_HANDLE   hSecHandle, WE_INT32 iTargetId,WE_INT16 sCharSet, 
           void *pvDisplayName, WE_INT32 iDisplayNameLen, WE_INT32 iReason);
#endif

/* 
* Create a menu with the supplied contracts and signing times. 
*/
WE_BOOL
Sec_UeShowContractsList(WE_HANDLE   hSecHandle, WE_INT32 iTargetId,WE_INT32 iNbrContracts, 
                        St_SecContractInfo *pstContractVec);
/*
* This function displays the supplied contract, 
* the signature of the contract, and the time it was signed.
* Returns a pointer to a struct sec_ti_t, or NULL on error.
*/
WE_BOOL
Sec_UeShowContractContent(WE_HANDLE   hSecHandle, WE_INT32 iTargetId,St_SecContractInfo stChosenContract/*int indexPos*/);


WE_INT32 
Sec_DecodeEvtAction(WE_VOID* pvSrcData,WE_UINT32 uiLen,WE_INT32 *piEvent,WE_VOID** pvDesData);

WE_VOID
Sec_ReleaseUeCB(WE_VOID* pvData);



#endif
