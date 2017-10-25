/*=====================================================================================
    FILE NAME :
        Sec_iue.h
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

#ifndef SEC_IUE_H
#define SEC_IUE_H

/*******************************************************************************
*   Type Define Section
*******************************************************************************/

typedef enum tagE_SecUeReturnVal
{
    E_SEC_UE_OK, /*E_SEC_UE_OK*/
    E_SEC_UE_CANCEL, /*E_SEC_UE_CANCEL*/
    E_SEC_UE_PIN_LOCKED, /*E_SEC_UE_PIN_LOCKED*/
    E_SEC_UE_PIN_FALSE, /*E_SEC_UE_PIN_FALSE*/
    E_SEC_UE_HASH_FALSE, /*E_SEC_UE_HASH_FALSE*/
    E_SEC_UE_OTHER_ERROR, /*E_SEC_UE_OTHER_ERROR*/
    E_SEC_UE_PIN_TOO_LONG, /*E_SEC_UE_PIN_TOO_LONG*/
    E_SEC_UE_PIN_TOO_SHORT /*E_SEC_UE_PIN_TOO_SHORT*/
} E_SecUeReturnVal;

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
void
Sec_iUeStart (WE_HANDLE   hSecHandle);


#ifdef M_SEC_CFG_USE_CERT
void 
Sec_iUeShowCertList (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_INT32 iNbrOfCerts, St_SecCertName *pstCertNames);
void 
Sec_iUeStoreCert (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,
		    WE_INT16 sIssuerCharSet, void *pvIssuer, WE_INT32 iIssuerLen, 
                  WE_UINT32 uiValidNotBefore, WE_UINT32 uiValidNotAfter, 
                  WE_INT16 sSubjectCharSet, 
                  void *pvSubject, WE_INT32 iSubjectLen, WE_CHAR cCertType);
#endif

#ifdef M_SEC_CFG_USE_KEYS
void 
Sec_iUeSelectCert (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,
		     WE_INT32 iTitle, WE_INT32 iOkAction, WE_INT32 iNbrOfCerts, 
                   St_SecCertName *pstCertNames);
#endif


void 
Sec_iUeConfirm (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_INT32 iMsg, WE_INT32 iYesMsg, WE_INT32 iNoMsg);

void 
Sec_iUeSignTextConfirm (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_INT32 iMsg, const WE_CHAR *pcText, 
                        WE_INT32 iYesMsg, WE_INT32 iNoMsg);

#ifdef M_SEC_CFG_USE_CERT
void 
Sec_iUeShowCertContent (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,
			   WE_INT32 iCertId, WE_INT16 sIssuerCharSet, 
                        void *pvIssuer, WE_INT32 iIssuerLen,
                        WE_UINT32 uiValidNotBefore, WE_UINT32 uiValidNotAfter, 
                        WE_INT16 sSubjectCharSet, void *pvSubject, WE_INT32 iSubjectLen,
                        WE_UCHAR *pucSerialNumber , WE_UINT16 usSerialNumberLen,
                        WE_UCHAR* pucAlg,WE_UCHAR* pucPubKey,WE_UINT16 usPubKeyLen,
                        WE_INT32 iCancelStr);
#endif


void 
Sec_iUeShowSessionContent (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,St_SecSessionInfo stSessInf);

void 
Sec_iUeWarning (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_INT32 iMsg);

/* modify by Sam[070119] */
/*#if( defined(M_SEC_CFG_WTLS_CLASS_3) ||defined(M_SEC_CFG_CAN_SIGN_TEXT) )*/
void 
Sec_iUeCreatePin (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_UINT8 ucKeyType);
void 
Sec_iUeChangePin (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_UINT8 ucKeyType);
void 
Sec_iUeChangePinResp (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,E_SecUeReturnVal eResult);
void 
Sec_iUePin (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,E_SecPinReason ePinCause);
void 
Sec_iUeVerifyPinResp (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,E_SecUeReturnVal eResult);
void 
Sec_iUeDelUserKeys (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_UINT8 ucKeyType);
/* #endif */

#ifdef M_SEC_CFG_USE_CERT
void 
Sec_iUeVerifyHashResp (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,E_SecUeReturnVal eResult);
void 
Sec_iUeHash (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_INT16 sCharSet, 
             void *pvDisplayName, WE_INT32 iDisplayNameLen);

void 
Sec_iUeNameConfirm (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_INT16 sCharSet, 
                    void *pvDisplayName, WE_INT32 iDisplayNameLen, 
                    WE_INT32 iMsg, WE_INT32 iYesMsg, WE_INT32 iNoMsg);
#endif

void 
Sec_iUeShowContractsList (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,
			WE_INT32 iNbrContracts, St_SecContractInfo *pstContracts);


#endif
