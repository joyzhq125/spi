/*=====================================================================================
    FILE NAME :
        Sec_iue.c
    MODULE NAME :
        sec
    GENERAL DESCRIPTION
        
    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2002-2006 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
    Modification Tracking
    Date       Author         Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-07-20 Bird     none      Init
    
=====================================================================================*/

/***************************************************************************************
*   Include File Section
****************************************************************************************/

#include"sec_comm.h"

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
#define SEC_NBRPINTRIES  (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))-> iNbrPinTries)
#define SEC_NBROFCERTS   (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))-> iNbrofCerts)
#define SEC_MAXNBRCERTS (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))-> iMaxNbrCerts)
#define SEC_CERTNAMES     (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pstCertNames)
#define SEC_CERTIDS          (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->piCertIds)
#define SEC_CERTTYPE        (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->cCertType)
#define SEC_CERTID            (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->iCertId)
#define SEC_KEYTYPE         (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->ucKeyType)
#define SEC_PINCAUSE      (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->ePinCause)
#define SEC_DISPLAYNAME (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))-> pcDisplayName)
#define SEC_DISPLAYNAME_LEN (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))-> iDisplayNameLen)
#define SEC_CHARSET             (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))-> usCharSet)
#define SEC_NBRCONTRACTS    (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))-> iNbrContracts)
#define SEC_MAX_NBRCONTRACTS (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->iMaxNbrContracts)
#define SEC_CONTRACTVEC     (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pstContractVec)
#define SEC_INDEX               (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->iIndex)
#define SEC_CONTRACT_INDEX       (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->iContractIndex)
#define SEC_IFTYPE              (((ISec*)hSecHandle)->iIFType)

#define SEC_IUE_FREEIF(p)   {if ((p)){WE_FREE((p));(p) = NULL;}}
#define SEC_IUE_FREE(p)     {WE_FREE((p));(p) = NULL;}
/*******************************************************************************
*   Function Define Section
*******************************************************************************/
static void 
Sec_UeCreatePinAgain (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_UINT8 ucKeyType, WE_INT32 iReason);

static void
Sec_UePinAgain (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,E_SecPinReason ePinCause, WE_INT32 iReason);

static void
Sec_UeChangePinDialogAgain (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_UINT8 ucKeyType, WE_INT32 iReason);


#ifdef M_SEC_CFG_USE_CERT
static void
Sec_UeHashAgain (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_INT16 sCharSet, 
                 void *pvDisplayName, WE_INT32 iDisplayNameLen, WE_INT32 iReason);

static void
Sec_UeShowCertListAgain (WE_HANDLE   hSecHandle,WE_INT32 iTargetId);
#endif

#if ((defined(M_SEC_CFG_CAN_SIGN_TEXT) && defined(M_SEC_CFG_STORE_CONTRACTS)))
static void
Sec_UeShowContractListAgain (WE_HANDLE   hSecHandle,WE_INT32 iTargetId);
#endif



/*==================================================================================================
FUNCTION: 
    Sec_iUeStart
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    initial dialog global value    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void
Sec_iUeStart (WE_HANDLE   hSecHandle)
{
    if(NULL == hSecHandle)
    {
        return;
    }
#ifdef M_SEC_CFG_USE_KEYS
    SEC_NBRPINTRIES = 0;
#endif
}

#ifdef M_SEC_CFG_USE_CERT
/*==================================================================================================
FUNCTION: 
    Sec_iUeShowCertList
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    call Sec_UeShowCertList fuction to create dialog,see description of Sec_UeShowCertList    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    WE_INT32 iNbrOfCerts[IN]:The number of elements in certNames.
    St_SecCertName *pstCertNames[IN]:array of certname
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_iUeShowCertList (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_INT32 iNbrOfCerts, 
                     St_SecCertName *pstCertNames)
{
    WE_INT32       iIndex = 0;
    WE_BOOL bRet = FALSE;
    
    if((NULL == hSecHandle) ||((NULL == pstCertNames)&&(iNbrOfCerts != 0)))
    {
        return;
    }
    WE_LOG_MSG((0,0, "SEC: Sec_iUeShowCertList\n"));
 
    SEC_NBROFCERTS = iNbrOfCerts;
    SEC_MAXNBRCERTS = iNbrOfCerts;
    
    if (iNbrOfCerts > 0) 
    {
        SEC_CERTNAMES = WE_MALLOC (sizeof (St_SecCertName) *(WE_UINT32)iNbrOfCerts);
        if (NULL == SEC_CERTNAMES)
        {
            return;
        }
    }
    else 
    {
        SEC_CERTNAMES = NULL;
    }
    for (iIndex = 0; iIndex < SEC_MAXNBRCERTS; iIndex++) 
    {
        if ((SEC_CERTNAMES != NULL) && (pstCertNames != NULL))
        {
            SEC_CERTNAMES[iIndex].iCertId = pstCertNames[iIndex].iCertId;
            SEC_CERTNAMES[iIndex].sCharSet = pstCertNames[iIndex].sCharSet;
            SEC_CERTNAMES[iIndex].usFriendlyNameLen = pstCertNames[iIndex].usFriendlyNameLen;
            SEC_CERTNAMES[iIndex].pvFriendlyName = SEC_STRDUP(pstCertNames[iIndex].pvFriendlyName);
            if(NULL == SEC_CERTNAMES[iIndex].pvFriendlyName)
            {
                WE_INT32 iIndexJ = 0;
                for (; iIndexJ < iIndex; iIndexJ++)
                {
                    SEC_IUE_FREE(SEC_CERTNAMES[iIndexJ].pvFriendlyName);
                }
                SEC_IUE_FREE(SEC_CERTNAMES);
                return;
            }
            SEC_CERTNAMES[iIndex].cNameType = pstCertNames[iIndex].cNameType;
        }
    }
    bRet = Sec_UeShowCertList(hSecHandle,iTargetId, iNbrOfCerts, pstCertNames);
    if (!bRet ) 
    {    
        for (iIndex = 0; iIndex < SEC_MAXNBRCERTS; iIndex++) 
        {
            WE_FREE (SEC_CERTNAMES[iIndex].pvFriendlyName);
        }
        WE_FREE (SEC_CERTNAMES);
        SEC_CERTNAMES = NULL;
    }
}
#endif 

#ifdef M_SEC_CFG_USE_CERT
/*==================================================================================================
FUNCTION: 
    Sec_UeShowCertListAgain
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
     call Sec_UeShowCertList fuction to create dialog,show certificate Name dialog again    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

static void 
Sec_UeShowCertListAgain (WE_HANDLE   hSecHandle,WE_INT32 iTargetId)
{
    /* St_SecTi *dlg;*/
    WE_BOOL bRet = FALSE;
    WE_INT32       iIndex = 0;
    if(NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((0,0, "SEC:Sec_UeShowCertListAgain\n"));
    bRet = Sec_UeShowCertList (hSecHandle, iTargetId,SEC_NBROFCERTS, SEC_CERTNAMES);
    if (!bRet ) 
    {    
        for (iIndex = 0; iIndex < SEC_MAXNBRCERTS; iIndex++) 
        {
            WE_FREE (SEC_CERTNAMES[iIndex].pvFriendlyName);
        }
        WE_FREE (SEC_CERTNAMES);
        SEC_CERTNAMES = NULL;
    }
}
#endif 


#ifdef M_SEC_CFG_USE_KEYS
/*==================================================================================================
FUNCTION: 
    Sec_iUeSelectCert
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    call Sec_UeSelectCert fuction to create dialog    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    WE_INT32 iTitle[IN]:An index to the message displayed to the end user.
    WE_INT32 iOkActionId[IN]:An index to the message displayed on the "yes" button
    WE_INT32 iNbrOfCerts[IN]:The number of elements in certNames
    St_SecCertName *pstCertNames[IN]:An array of elements 

RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void 
Sec_iUeSelectCert (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_INT32 iTitle, 
                   WE_INT32 iOkAction, 
                   WE_INT32 iNbrOfCerts, St_SecCertName *pstCertNames)
{
    WE_INT32       iIndex = 0;
    WE_BOOL bRet = FALSE;
    
    if(NULL == hSecHandle || ((NULL == pstCertNames)&&(iNbrOfCerts != 0)))
    {
        Sec_UeChooseCertByNameResp(hSecHandle,0, E_SEC_UE_OTHER_ERROR);
        return;
    }
    WE_LOG_MSG(( 0,0,"SEC:Sec_iUeSelectCert\n"));
    SEC_CERTIDS = (WE_INT32 *)WE_MALLOC ((WE_UINT32)iNbrOfCerts * sizeof (int) + 1);
    if((NULL == SEC_CERTIDS)&&(iNbrOfCerts != 0))
    {
        Sec_UeChooseCertByNameResp(hSecHandle,0, E_SEC_UE_OTHER_ERROR);
        return;
    }
    for (iIndex = 0; iIndex < iNbrOfCerts; iIndex++) 
    {
        if ((SEC_CERTIDS != NULL) && (pstCertNames != NULL))
        {
            SEC_CERTIDS[iIndex] = pstCertNames[iIndex].iCertId;
        }
    }
    bRet = Sec_UeSelectCert (hSecHandle,iTargetId, iTitle, iOkAction, 
        iNbrOfCerts, pstCertNames);
    if (!bRet ) 
    {   
        WE_FREE (SEC_CERTIDS);
        SEC_CERTIDS = NULL;
        Sec_UeChooseCertByNameResp(hSecHandle,0, E_SEC_UE_OTHER_ERROR);
    }
}
#endif 

#ifdef M_SEC_CFG_USE_CERT
/*==================================================================================================
FUNCTION: 
    Sec_iUeStoreCert
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    call Sec_UeStoreCert fuction to create dialog    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    WE_INT16 sIssuerCharSet[IN]:defined character set for WTLS certificates. For X.509 certificates this value is -1
    void *pvIssuer[IN]:The issuer of the certificate.  
                                X.509: DER encoded issuer beginning with a "SEQUENCE OF" 
                                according to the X.509 specification. [X.509] 
                                WTLS: Name according to the WTLS specification.[WAP-WTLS] 
    WE_INT32 iIssuerLen[IN]:The length of issuer
    WE_UINT32 uiValidNotBefore[IN]:The beginning of the validity period of the certificate
    WE_UINT32 uiValidNotAfter[IN]:The end of the validity period of the certificate
    WE_INT16 sSubjectCharSet[IN]:defined character set for WTLS certificates. For X.509 certificates this value is -1
    void *pvSubject[IN]:The subject of the certificate
    WE_INT32 iSubjectLen[IN]:The length of subject
    WE_CHAR cCertType[IN]:The certificate type. M_SEC_USER_CERT for a user certificate 
                                        and M_SEC_CA_CERT for a CA/root certificate

RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void 
Sec_iUeStoreCert (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,
                  WE_INT16 sIssuerCharSet, void *pvIssuer, 
                  WE_INT32 iIssuerLen, WE_UINT32 uiValidNotBefore, WE_UINT32 uiValidNotAfter, 
                  WE_INT16 sSubjectCharSet,  void *pvSubject, WE_INT32 iSubjectLen, 
                  WE_CHAR cCertType)
{
    WE_BOOL bRet = FALSE;
    if(NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((0,0, "SEC:Sec_iUeStoreCert\n"));
    SEC_CERTTYPE = cCertType;
    
    bRet = Sec_UeStoreCert (hSecHandle,iTargetId, sIssuerCharSet, pvIssuer, iIssuerLen, 
                                                        uiValidNotBefore, uiValidNotAfter, 
                                                        sSubjectCharSet, 
                                                        pvSubject, iSubjectLen, cCertType);
    if (!bRet) 
    {
        #ifdef M_SEC_CFG_SHOW_WARNINGS
        Sec_iUeWarning (hSecHandle,iTargetId,M_SEC_ERR_GENERAL_ERROR);
        #endif
    }
    else 
    {
        /*  sec_tiPushWindow (dlg);*/
    }
}
#endif 

#ifdef M_SEC_CFG_USE_CERT
/*==================================================================================================
FUNCTION: 
    Sec_iUeShowCertContent
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    call Sec_UeShowCertContent fuction to create dialog
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    WE_INT16 sIssuerCharSet[IN]:defined character set for WTLS certificates. For X.509 certificates this value is -1
    void *pvIssuer[IN]:The issuer of the certificate.  
                                X.509: DER encoded issuer beginning with a "SEQUENCE OF" 
                                according to the X.509 specification. [X.509] 
                                WTLS: Name according to the WTLS specification.[WAP-WTLS] 
    WE_INT32 iIssuerLen[IN]:The length of issuer
    WE_UINT32 uiValidNotBefore[IN]:The beginning of the validity period of the certificate
    WE_UINT32 uiValidNotAfter[IN]:The end of the validity period of the certificate
    WE_INT16 sSubjectCharSet[IN]:defined character set for WTLS certificates. For X.509 certificates this value is -1
    void *pvSubject[IN]:The subject of the certificate
    WE_INT32 iSubjectLen[IN]:The length of subject
    WE_UCHAR *pucFingerPrint[IN]:The fingerprint of the certificate
    WE_INT32 iFingerPrintLen[IN]:The length of the fingerprint
    WE_INT32 iCancelStr[IN]:The message to display on the cancel choice
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void 
Sec_iUeShowCertContent (WE_HANDLE  hSecHandle,WE_INT32 iTargetId,WE_INT32 iCertId, 
                        WE_INT16 sIssuerCharSet, 
                        void *pvIssuer, WE_INT32 iIssuerLen,
                        WE_UINT32 uiValidNotBefore, WE_UINT32 uiValidNotAfter, 
                        WE_INT16 sSubjectCharSet, void *pvSubject, WE_INT32 iSubjectLen,
                        WE_UCHAR *pucSerialNumber , WE_UINT16 usSerialNumberLen,
                        WE_UCHAR* pucAlg,WE_UCHAR* pucPubKey,WE_UINT16 usPubKeyLen,
                        WE_INT32 iCancelStr)
{
    WE_BOOL bRet = FALSE;
    if(NULL == hSecHandle)
    {
        #ifdef M_SEC_CFG_SHOW_WARNINGS
        Sec_iUeWarning (hSecHandle,iTargetId,M_SEC_ERR_GENERAL_ERROR); 
        #endif
        return;
    }
    WE_LOG_MSG(( 0,0,"SEC:Sec_iUeShowCertContent\n"));
    SEC_CERTID = iCertId;
    bRet = Sec_UeShowCertContent (hSecHandle, iTargetId,sIssuerCharSet, 
        pvIssuer, iIssuerLen,
        uiValidNotBefore, uiValidNotAfter, 
        sSubjectCharSet, pvSubject, iSubjectLen,
        pucSerialNumber, usSerialNumberLen,
        pucAlg,pucPubKey,usPubKeyLen,
        iCancelStr);
    if (!bRet) 
    {
        #ifdef M_SEC_CFG_SHOW_WARNINGS
        Sec_iUeWarning (hSecHandle,iTargetId,M_SEC_ERR_GENERAL_ERROR); 
        #endif
    }
    else 
    {
        /* sec_tiPushWindow (dlg);*/
    }
}
#endif 

/*==================================================================================================
FUNCTION: 
    Sec_iUeConfirm
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    call Sec_UeConfirm fuction to create dialog, see description of   Sec_UeConfirm    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    WE_INT32 iMsg[IN]:An index to the message displayed to the end user.
    WE_INT32 iYesMsg[IN]:An index to the message displayed on the "yes" button
    WE_INT32 iNoMsg[IN]:An index to the message displayed on the "no" button
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void 
Sec_iUeConfirm (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_INT32 iMsg, WE_INT32 iYesMsg, WE_INT32 iNoMsg)
{
    WE_BOOL bRet = FALSE;
    WE_LOG_MSG(( 0,0,"SEC:Sec_iUeConfirm\n"));
    bRet = Sec_UeConfirm (hSecHandle,iTargetId, iMsg, iYesMsg, iNoMsg);
    
    if (!bRet) 
    {
        Sec_UeConfirmDialogResp(hSecHandle,FALSE);
    }
}

#ifdef M_SEC_CFG_CAN_SIGN_TEXT
/*==================================================================================================
FUNCTION: 
    Sec_iUeSignTextConfirm
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    call Sec_UeSignTextConfirm fuction to create dialog,see description of   Sec_UeSignTextConfirm    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    WE_INT32 iMsg[IN]:An index to the message displayed to the end user.
    WE_CHAR *pcText[IN]:The message to be signed
    WE_INT32 iYesMsg[IN]:An index to the message displayed on the "yes" button
    WE_INT32 iNoMsg[IN]:An index to the message displayed on the "no" button
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void 
Sec_iUeSignTextConfirm (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,
                        WE_INT32 iMsg, const WE_CHAR *pcText, 
                        WE_INT32 iYesMsg, WE_INT32 iNoMsg)
{
    WE_BOOL bRet = FALSE;
    WE_LOG_MSG(( 0,0,"SEC:Sec_iUeSignTextConfirm\n"));
    bRet = Sec_UeSignTextConfirm (hSecHandle, iTargetId,iMsg, pcText, iYesMsg, iNoMsg);
    
    if (!bRet) 
    {
        Sec_UeExtConfirmDialogResp (hSecHandle,FALSE);
    }
}
#endif 

/*==================================================================================================
FUNCTION: 
    Sec_iUeShowSessionContent
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    call Sec_UeSessionInfo fuction to create dialog,see description of   Sec_iUeShowSessionContent    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    St_SecSessionInfo stSessInf[IN]:The session information.
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void 
Sec_iUeShowSessionContent (WE_HANDLE hSecHandle,WE_INT32 iTargetId,St_SecSessionInfo stSessInf)
{
    WE_BOOL bRet = FALSE;
    
    WE_LOG_MSG(( 0,0,"SEC:Sec_iUeShowSessionContent\n"));
    bRet = Sec_UeSessionInfo (hSecHandle, iTargetId,stSessInf);
    
    if (!bRet) 
    {
        #ifdef M_SEC_CFG_SHOW_WARNINGS
        Sec_iUeWarning (hSecHandle,iTargetId,M_SEC_ERR_GENERAL_ERROR);
        #endif
    }
    else 
    {
        /* sec_tiPushWindow (dlg);*/
    }
}



#ifdef M_SEC_CFG_SHOW_WARNINGS
/*==================================================================================================
FUNCTION: 
    Sec_iUeWarning
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    call Sec_UeShowCertList fuction to create dialog,see description of   Sec_iUeWarning    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    WE_INT32 iMsg[IN]:An index to the message displayed to the end user.
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void 
Sec_iUeWarning (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_INT32 iMsg)
{  
    WE_BOOL bResult = TRUE;
    WE_LOG_MSG((0,0, "SEC:Sec_iUeWarning\n"));
    bResult = Sec_UeWarning (hSecHandle,iTargetId, iMsg);
    if (bResult != TRUE)
    {
        return;
    }
    /* sec_tiPushWindow (dlg);*/
}           
#endif 


#ifdef M_SEC_CFG_USE_KEYS

/*==================================================================================================
FUNCTION: 
    Sec_iUeCreatePin
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    call Sec_UeShowCertList fuction to create dialog,see description of   Sec_iUeCreatePin    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    WE_UINT8 ucKeyType[IN]:The type of the key (M_SEC_PRIVATE_NONREPKEY or 
                                            M_SEC_PRIVATE_AUTHKEY ) 
                                            with which this PIN code will be associated
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void
Sec_iUeCreatePin (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_UINT8 ucKeyType)
{
    WE_BOOL bRet = FALSE;
    if(NULL == hSecHandle)
    {
        Sec_UeGenPinResp (hSecHandle,E_SEC_UE_OTHER_ERROR, NULL);
        return;
    }
    WE_LOG_MSG((0,0, "SEC:Sec_iUeCreatePin\n"));
    SEC_KEYTYPE = ucKeyType;
    if (SEC_NBRPINTRIES >= M_SEC_MAX_NBR_PIN_TRIES) 
    {
        Sec_UeGenPinResp (hSecHandle,E_SEC_UE_PIN_LOCKED, NULL); 
        return;
    }
    bRet = Sec_UeGenPin (hSecHandle, iTargetId,ucKeyType, 0);
    if (!bRet) 
    {
        Sec_UeGenPinResp (hSecHandle,E_SEC_UE_OTHER_ERROR, NULL);
    }
}
#endif

#ifdef M_SEC_CFG_USE_KEYS

/*==================================================================================================
FUNCTION: 
    Sec_UeCreatePinAgain
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    due to input pin error,request to input again
    call Sec_UeShowCertList fuction to create dialog    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    WE_UINT8 ucKeyType[IN]:The type of the key (M_SEC_PRIVATE_NONREPKEY or 
                                            M_SEC_PRIVATE_AUTHKEY ) 
                                            with which this PIN code will be associated
    WE_INT32 iReason:The reason to call this dialog again

RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static void 
Sec_UeCreatePinAgain (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_UINT8 ucKeyType, WE_INT32 iReason)
{
    WE_BOOL bRet = FALSE;
    
    WE_LOG_MSG((0,0, "SEC:Sec_UeCreatePinAgain\n"));
    bRet = Sec_UeGenPin (hSecHandle,iTargetId, ucKeyType, iReason);
    if (!bRet) 
    {
        Sec_UeGenPinResp (hSecHandle,E_SEC_UE_OTHER_ERROR, NULL);
    }
}
#endif

#ifdef M_SEC_CFG_USE_KEYS
/*==================================================================================================
FUNCTION: 
    Sec_iUeChangePin
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    call Sec_UeChangePin fuction to create dialog,see description of   Sec_iUeChangePin    
ARGUMENTS PASSED:
    WE_UINT8 ucKeyType[IN]:The type of the key (M_SEC_PRIVATE_NONREPKEY or 
                                            M_SEC_PRIVATE_AUTHKEY ) 
                                            with which this PIN code will be associated.

RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void 
Sec_iUeChangePin (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_UINT8 ucKeyType)
{
    WE_BOOL bRet = FALSE;
    if(NULL == hSecHandle)
    {
        Sec_UeChangePinDialogResp (hSecHandle,E_SEC_UE_OTHER_ERROR);
        return;
    }
    SEC_KEYTYPE = ucKeyType;
    
    WE_LOG_MSG((0,0, "SEC:Sec_iUeChangePin\n"));
    if (SEC_NBRPINTRIES >= M_SEC_MAX_NBR_PIN_TRIES) 
    {
        Sec_UeChangePinDialogResp (hSecHandle,E_SEC_UE_PIN_LOCKED);
        return;
    }
    bRet = Sec_UeChgPin (hSecHandle, iTargetId,ucKeyType, 0);
    if (!bRet) 
    {
        Sec_UeChangePinDialogResp (hSecHandle,E_SEC_UE_OTHER_ERROR);
    }
}
#endif

#ifdef M_SEC_CFG_USE_KEYS
/*==================================================================================================
FUNCTION: 
    Sec_UeChangePinDialogAgain
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    due to input pin error,request to input pin code again,
    call Sec_UeChangePin fuction to create dialog
    
ARGUMENTS PASSED:
    WE_UINT8 ucKeyType[IN]:The type of the key (M_SEC_PRIVATE_NONREPKEY or 
                                            M_SEC_PRIVATE_AUTHKEY ) 
                                            with which this PIN code will be associated.
    WE_INT32 iReason[IN]:The reason to call this dialog again.

RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static void
Sec_UeChangePinDialogAgain (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_UINT8 ucKeyType, WE_INT32 iReason)
{
    WE_BOOL bRet = FALSE;
    if(NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((0,0, "SEC:Sec_UeChangePinDialogAgain\n"));
    if (SEC_NBRPINTRIES == M_SEC_MAX_NBR_PIN_TRIES) 
    {
        Sec_UeChangePinDialogResp (hSecHandle,E_SEC_UE_PIN_LOCKED);
        return;
    }
    bRet = Sec_UeChgPin (hSecHandle,iTargetId, ucKeyType, iReason);
    if (!bRet)  
    {
        Sec_UeChangePinDialogResp (hSecHandle,E_SEC_UE_OTHER_ERROR);
    }
}
#endif


#ifdef M_SEC_CFG_USE_KEYS
/*==================================================================================================
FUNCTION: 
    Sec_iUeChangePinResp
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    reponse of changpin dialog ,according the return result    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    E_SecUeReturnVal eResult[IN]:error code    
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void 
Sec_iUeChangePinResp (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,E_SecUeReturnVal eResult)
{
    WE_LOG_MSG((0,0, "SEC:Sec_iUeChangePinResp\n"));
    
    if(NULL == hSecHandle)
    {
        return;
    }
    if ((SEC_NBRPINTRIES == M_SEC_MAX_NBR_PIN_TRIES) && (eResult != E_SEC_UE_OK))
    {
        Sec_UeChangePinDialogResp (hSecHandle,E_SEC_UE_PIN_LOCKED);
    }
    else if (eResult == E_SEC_UE_OK) 
    {
        Sec_UeChangePinDialogResp (hSecHandle,eResult);
        SEC_NBRPINTRIES = 0;
    }
    else if (eResult == E_SEC_UE_OTHER_ERROR) 
    {
        Sec_UeChangePinDialogResp(hSecHandle,eResult);
    }
    else if ((eResult == E_SEC_UE_PIN_FALSE) || (eResult == E_SEC_UE_PIN_TOO_SHORT) ||
        (eResult == E_SEC_UE_PIN_TOO_LONG)) 
    {
        SEC_NBRPINTRIES++;
        Sec_UeChangePinDialogAgain (hSecHandle,iTargetId,SEC_KEYTYPE, M_SEC_UE_INPUT_FALSE);
    }
    else 
    {
        Sec_UeChangePinDialogResp (hSecHandle,E_SEC_UE_OTHER_ERROR);
    }
}
#endif

#ifdef M_SEC_CFG_USE_KEYS


/*==================================================================================================
FUNCTION: 
    Sec_iUePin
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    call Sec_UePin fuction to create dialog,see description of Sec_UePin    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    E_SecPinReason ePinCause[IN]::Parameter indicating the reason for asking for the PIN code.
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void 
Sec_iUePin (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,E_SecPinReason ePinCause)
{
    WE_BOOL bRet = FALSE;
    
    if(NULL == hSecHandle)
    {
        Sec_UePinDialogResp (hSecHandle,E_SEC_UE_OTHER_ERROR);
        return;
    }
    WE_LOG_MSG(( 0,0,"SEC:Sec_iUePin\n"));
    SEC_PINCAUSE = ePinCause;
    if (SEC_NBRPINTRIES >= M_SEC_MAX_NBR_PIN_TRIES) 
    {
        Sec_UePinDialogResp (hSecHandle,E_SEC_UE_PIN_LOCKED);
        return;
    }
    bRet = Sec_UePin (hSecHandle, iTargetId,ePinCause, 0);
    if (!bRet) 
    {
        Sec_UePinDialogResp (hSecHandle,E_SEC_UE_OTHER_ERROR);
    }
}
#endif
#ifdef M_SEC_CFG_USE_KEYS
/*==================================================================================================
FUNCTION: 
    Sec_UePinAgain
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    due to some errors,so request to show dialog again,
    call Sec_UeShowCertList fuction to create dialog
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    E_SecPinReason ePinCause[IN]::Parameter indicating the reason for asking for the PIN code.
    WE_INT32 iReason[IN]:The reason to call this dialog again.
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static void
Sec_UePinAgain (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,E_SecPinReason ePinCause, WE_INT32 iReason)
{
    WE_BOOL bRet = FALSE;
    if(NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((0,0, "SEC:Sec_UePinAgain\n"));
    
    SEC_PINCAUSE = ePinCause;
    
    if (SEC_NBRPINTRIES == M_SEC_MAX_NBR_PIN_TRIES) 
    {
        Sec_UePinDialogResp (hSecHandle,E_SEC_UE_PIN_LOCKED);
        return;
    }
    bRet = Sec_UePin (hSecHandle,iTargetId, ePinCause, iReason);
    if (!bRet) 
    {
        Sec_UePinDialogResp (hSecHandle,E_SEC_UE_OTHER_ERROR);
    }
}
#endif
#ifdef M_SEC_CFG_USE_KEYS

/*==================================================================================================
FUNCTION: 
    Sec_iUeVerifyPinResp
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    response of verify pin,according return error code    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    E_SecUeReturnVal eResult[IN]:error code
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void 
Sec_iUeVerifyPinResp (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,E_SecUeReturnVal eResult)
{
    if(NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((0,0, "SEC:Sec_iUeVerifyPinResp\n"));
    if ((SEC_NBRPINTRIES == M_SEC_MAX_NBR_PIN_TRIES) && (eResult != E_SEC_UE_OK))
    {
        Sec_UePinDialogResp(hSecHandle,E_SEC_UE_PIN_LOCKED);
    }
    else if (eResult == E_SEC_UE_OK)
    {
        Sec_UePinDialogResp(hSecHandle,eResult);
        SEC_NBRPINTRIES = 0;
    }
    else if (eResult == E_SEC_UE_OTHER_ERROR)
    {
        Sec_UePinDialogResp(hSecHandle,eResult);
    }
    else if (eResult == E_SEC_UE_PIN_FALSE)
    {
        SEC_NBRPINTRIES++;
        Sec_UePinAgain(hSecHandle,iTargetId,SEC_PINCAUSE, M_SEC_UE_INPUT_FALSE);
    }
    else /*add by birdzhang 061024*/
    {
        Sec_UePinDialogResp (hSecHandle,E_SEC_UE_OTHER_ERROR);
    }

}
#endif
#ifdef M_SEC_CFG_USE_KEYS



/*==================================================================================================
FUNCTION: 
    Sec_iUeDelUserKeys
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    delete user certificate
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    WE_UINT8 ucKeyType[IN]:An index to the message displayed to the end user.

RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void 
Sec_iUeDelUserKeys (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_UINT8 ucKeyType)
{
    iTargetId=iTargetId;
    ucKeyType = ucKeyType;
    Sec_UeAllowDelKeysResp (hSecHandle,E_SEC_UE_OK);
}
#endif 


#ifdef M_SEC_CFG_USE_CERT
/*==================================================================================================
FUNCTION: 
    Sec_iUeHash
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    call Sec_UeHash fuction to create dialog,see description of Sec_UeHash    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    WE_INT16 sCharSet[IN]:character set
    void *pvDisplayName[IN]:The name that must be displayed to the end user.
    WE_INT32 iDisplayNameLen[IN]:The length of displayName.
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void 
Sec_iUeHash (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_INT16 sCharSet, 
             void *pvDisplayName, WE_INT32 iDisplayNameLen)
{
    WE_BOOL bRet = FALSE;
    
    if(NULL == hSecHandle)
    {
        Sec_UeHashDialogResp (hSecHandle,E_SEC_UE_OTHER_ERROR);
        return;
    }
    WE_LOG_MSG((0,0, "SEC:Sec_iUeHash\n"));
    SEC_DISPLAYNAME = (WE_CHAR *)WE_MALLOC((WE_UINT32)iDisplayNameLen * sizeof (WE_CHAR));
    if((NULL == SEC_DISPLAYNAME)&&(iDisplayNameLen != 0))
    {
        Sec_UeHashDialogResp (hSecHandle,E_SEC_UE_OTHER_ERROR);
        return;
    }
    (void)WE_MEMCPY ((void*)(SEC_DISPLAYNAME), pvDisplayName, (WE_UINT32)iDisplayNameLen);
    
    SEC_DISPLAYNAME_LEN = iDisplayNameLen;
    SEC_CHARSET = (WE_UINT16)sCharSet;
    
    bRet = Sec_UeHash (hSecHandle,iTargetId, sCharSet, pvDisplayName, iDisplayNameLen, 0);
    if (!bRet) 
    {
        Sec_UeHashDialogResp (hSecHandle,E_SEC_UE_OTHER_ERROR);
    }
}
#endif 
#ifdef M_SEC_CFG_USE_CERT
/*==================================================================================================
FUNCTION: 
    Sec_UeHashAgain
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    some errors happened,call Sec_UeHashAgain fuction to create dialog    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    WE_INT16 sCharSet[IN]:character set
    void *pvDisplayName[IN]:The name that must be displayed to the end user.
    WE_INT32 iDisplayNameLen[IN]:The length of displayName.
    WE_INT32 iReason[IN]:The reason to call this dialog again.

RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static void
Sec_UeHashAgain (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_INT16 sCharSet, 
                 void *pvDisplayName, WE_INT32 iDisplayNameLen, WE_INT32 iReason)
{
    WE_BOOL bRet = FALSE;
    
    WE_LOG_MSG(( 0,0,"SEC:Sec_UeHashAgain\n"));
    bRet = Sec_UeHash (hSecHandle, iTargetId,sCharSet, pvDisplayName, iDisplayNameLen, iReason); 
    if (!bRet) 
    {
        Sec_UeHashDialogResp (hSecHandle,E_SEC_UE_OTHER_ERROR);
    }
}
#endif
#ifdef M_SEC_CFG_USE_CERT
/*==================================================================================================
FUNCTION: 
    Sec_iUeVerifyHashResp
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    response of verify hash,according to error code    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    E_SecUeReturnVal eResult[IN]:error code.
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void 
Sec_iUeVerifyHashResp (WE_HANDLE   hSecHandle,WE_INT32 iTargetId, E_SecUeReturnVal eResult)
{
    if(NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG(( 0,0,"SEC:Sec_iUeVerifyHashResp\n"));
    if (eResult == E_SEC_UE_OK) 
    {
        Sec_UeHashDialogResp (hSecHandle,eResult);
        WE_FREE (SEC_DISPLAYNAME);
        SEC_DISPLAYNAME = NULL;
        SEC_DISPLAYNAME_LEN = 0;
        SEC_CHARSET = 0;
    }
    else if (eResult == E_SEC_UE_HASH_FALSE) 
    {
        Sec_UeHashAgain (hSecHandle,iTargetId,(WE_INT16)(SEC_CHARSET), SEC_DISPLAYNAME, 
            SEC_DISPLAYNAME_LEN, M_SEC_UE_INPUT_FALSE);
    }
    else
    {
        Sec_UeHashDialogResp (hSecHandle,E_SEC_UE_OTHER_ERROR);
        WE_FREE (SEC_DISPLAYNAME);
        SEC_DISPLAYNAME = NULL;
        SEC_DISPLAYNAME_LEN = 0;
        SEC_CHARSET = 0;

        
    }
}

/*==================================================================================================
FUNCTION: 
    Sec_iUeNameConfirm
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    call Sec_iUeNameConfirm fuction to create dialog,see description of Sec_iUeNameConfirm
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    WE_INT16 sCharSet[IN]: character set for subject of WTLS certificates 
                                        and CertDisplayName according to the WPKI-specification [WAP-WPKI]. 
                                        For subject of X.509 certificates this value is -1.
    void *pvDisplayName[IN]:The name displayed to the user
    WE_INT32 iDisplayNameLen[IN]:The length of displayName
    WE_INT32 iMsg[IN]:An index to the message displayed to the end user.
    WE_INT32 iYesMsg[IN]:An index to the message displayed on the "yes" button
    WE_INT32 iNoMsg[IN]:An index to the message displayed on the "no" button

RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void 
Sec_iUeNameConfirm (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_INT16 sCharSet, 
                    void *pvDisplayName, WE_INT32 iDisplayNameLen, 
                    WE_INT32 iMsg, WE_INT32 iYesMsg, WE_INT32 iNoMsg)
{
    WE_BOOL bRet = FALSE;
    if(NULL == hSecHandle)
    {
        Sec_UeConfirmDispNameResp (hSecHandle,0);
    }
    WE_LOG_MSG(( 0,0,"SEC:Sec_iUeNameConfirm\n"));
    bRet = Sec_UeNameConfirm (hSecHandle,iTargetId, sCharSet, 
                              pvDisplayName, iDisplayNameLen, 
                              iMsg, iYesMsg, iNoMsg);    
    if (!bRet) 
    {
        Sec_UeConfirmDispNameResp (hSecHandle,0);
    }
}

#endif 


/*==================================================================================================
FUNCTION: 
    Sec_iUeShowContractsList
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    call Sec_iUeShowContractsList fuction to create dialog
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    WE_INT32 iNbrContracts[IN]:The number of supplied contracts.
    St_SecContractInfo *pstContractVec[IN]:An array of the stored contracts

RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void 
Sec_iUeShowContractsList (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,
                          WE_INT32 iNbrContracts, St_SecContractInfo *pstContracts)
{
    WE_INT32       iIndex = 0;
    WE_BOOL        bRet = FALSE;
    WE_INT32       iIndexJ = 0;
    if(NULL == hSecHandle || ((NULL == pstContracts)&&(iNbrContracts != 0)))
    {
        return;
    }
    WE_LOG_MSG(( 0,0,"SEC:Sec_iUeShowContractsList\n"));
    SEC_NBRCONTRACTS = iNbrContracts;
    SEC_MAX_NBRCONTRACTS = iNbrContracts;
    if (iNbrContracts > 0) 
    {
        SEC_CONTRACTVEC =
            (St_SecContractInfo *)WE_MALLOC ((WE_UINT32)iNbrContracts * sizeof (St_SecContractInfo));
        if(NULL == SEC_CONTRACTVEC)
        {
            return;
        }
    }
    else 
    {
        SEC_CONTRACTVEC = NULL;
    }
    for (iIndex = 0; iIndex < iNbrContracts; iIndex++) 
    {    
        if ((SEC_CONTRACTVEC != NULL) && (pstContracts != NULL))
        {
            SEC_CONTRACTVEC[iIndex].pcContract = SEC_STRDUP (pstContracts[iIndex].pcContract);
            if(NULL == SEC_CONTRACTVEC[iIndex].pcContract)
            {
                for (iIndexJ = 0; iIndexJ < iIndex; iIndexJ++)
                {
                    SEC_IUE_FREE(SEC_CONTRACTVEC[iIndexJ].pcContract);
                    SEC_IUE_FREE(SEC_CONTRACTVEC[iIndexJ].pcSignature);
                }
                SEC_IUE_FREE(SEC_CONTRACTVEC);
                return;
            }
            SEC_CONTRACTVEC[iIndex].pcSignature = (WE_CHAR *)WE_MALLOC (pstContracts[iIndex].usSignatureLen + 1);
            if(NULL == SEC_CONTRACTVEC[iIndex].pcSignature)
            {
                for (iIndexJ = 0; iIndexJ < iIndex; iIndexJ++)
                {
                    SEC_IUE_FREE(SEC_CONTRACTVEC[iIndexJ].pcContract);
                    SEC_IUE_FREE(SEC_CONTRACTVEC[iIndexJ].pcSignature);
                }
                SEC_IUE_FREE(SEC_CONTRACTVEC[iIndex].pcContract);
                SEC_IUE_FREE(SEC_CONTRACTVEC);
                return;
            }
            (void)WE_MEMCPY (SEC_CONTRACTVEC[iIndex].pcSignature,
                pstContracts[iIndex].pcSignature, 
                pstContracts[iIndex].usSignatureLen);
            
            SEC_CONTRACTVEC[iIndex].pcSignature[pstContracts[iIndex].usSignatureLen] = '\0';
            SEC_CONTRACTVEC[iIndex].usSignatureLen = pstContracts[iIndex].usSignatureLen;
            SEC_CONTRACTVEC[iIndex].uiTime = pstContracts[iIndex].uiTime;
            SEC_CONTRACTVEC[iIndex].usContractId = pstContracts[iIndex].usContractId;
        }
    }
    
    bRet = Sec_UeShowContractsList (hSecHandle, iTargetId,iNbrContracts, pstContracts);
    if (!bRet) 
    {
        
        for (iIndex = 0; iIndex < SEC_NBRCONTRACTS; iIndex++) 
        {
            WE_FREE (SEC_CONTRACTVEC[iIndex].pcContract);
            WE_FREE (SEC_CONTRACTVEC[iIndex].pcSignature);
        }
        WE_FREE (SEC_CONTRACTVEC);
        SEC_CONTRACTVEC = NULL;
    }
}

/*==================================================================================================
FUNCTION: 
    Sec_UeShowContractListAgain
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    call Sec_UeShowContractsList fuction to create dialog    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static void
Sec_UeShowContractListAgain (WE_HANDLE   hSecHandle,WE_INT32 iTargetId)
{
    WE_BOOL bRet = FALSE;    
    WE_INT32       iIndex = 0;
    /*sec_tiPopWindow ();*/
    if(NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG(( 0,0,"SEC:Sec_UeShowContractListAgain\n"));
    bRet = Sec_UeShowContractsList (hSecHandle, iTargetId,SEC_NBRCONTRACTS, SEC_CONTRACTVEC);
    if (!bRet) 
    {    
        for (iIndex = 0; iIndex < SEC_MAX_NBRCONTRACTS; iIndex++) 
        {
            WE_FREE (SEC_CONTRACTVEC[iIndex].pcContract);
            WE_FREE (SEC_CONTRACTVEC[iIndex].pcSignature);
        }
        WE_FREE (SEC_CONTRACTVEC);
        SEC_CONTRACTVEC = NULL;
    }
}

/*==================================================================================================
FUNCTION: 
    Sec_UeShowContract
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    call Sec_UeShowContractContent fuction to create dialog    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    WE_INT32 iIndexPos:the choise index
RETURN VALUE:
    M_SEC_ERR_OK:success
    M_SEC_ERR_GENERAL_ERROR:fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32
Sec_UeShowContract (WE_HANDLE   hSecHandle,WE_INT32 iTargetId,WE_INT32 iIndexPos)
{
    WE_BOOL bRet = FALSE;
    if(NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    WE_LOG_MSG((0,0, "SEC:Sec_UeShowContract\n"));
    SEC_CONTRACT_INDEX = iIndexPos;
    bRet = Sec_UeShowContractContent (hSecHandle,iTargetId, SEC_CONTRACTVEC [iIndexPos]);
    if (!bRet) 
    {
        return M_SEC_ERR_GENERAL_ERROR;
    }
    
    return M_SEC_ERR_OK;
}


/*****************************************************************************
*Interface to STK or Borwser, Called after the customer finish operation on dialog and push
*OK or Cancel button
******************************************************************************/

/*1.confirm*/
/*==================================================================================================
FUNCTION: 
    Sec_UeConfirmAction
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    confirm dialog response
    
ARGUMENTS PASSED:
WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
St_ConfirmAction stConfirm[IN]:user data

RETURN VALUE:
    M_SEC_ERR_OK is success ,otherwise is fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_UeConfirmAction(WE_HANDLE hSecHandle ,WE_INT32 iTargetId,St_ConfirmAction stConfirm)
{
    iTargetId=iTargetId;
    WE_LOG_MSG( (0,0,"SEC: Sec_UeConfirmAction(bIsOk = %d)", stConfirm.bIsOk));

    if(NULL == hSecHandle)
    {
         Sec_UeConfirmDialogResp (hSecHandle,FALSE);
         return M_SEC_ERR_INVALID_PARAMETER;
    }
    if(!stConfirm.bIsOk)
    {
        Sec_UeConfirmDialogResp (hSecHandle,FALSE);
    }
    else
    {
        Sec_UeConfirmDialogResp (hSecHandle,TRUE);
    }
    return M_SEC_ERR_OK;
    
}
#ifdef M_SEC_CFG_USE_KEYS
/*2.change pin*/
/*==================================================================================================
FUNCTION: 
    Sec_UeChangePinAction
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    change pin dialog response
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    St_ChangePinAction stChangePin[IN]:user data
RETURN VALUE:
    M_SEC_ERR_OK is success ,otherwise is fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_UeChangePinAction(WE_HANDLE hSecHandle ,WE_INT32 iTargetId,St_ChangePinAction stChangePin)
{
    WE_CHAR *pcPin = stChangePin.pcNewPinValue;
    WE_CHAR *pcNewPin2 = stChangePin.pcNewPinAgainValue; 
    WE_CHAR *pcOldPin = stChangePin.pcOldPinValue; 
    if(NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    WE_LOG_MSG(( 0,0,"SEC: Sec_UeChangePinAction(bIsOk = %d)", stChangePin.bIsOk));
    if(!stChangePin.bIsOk)
    {
        Sec_UeChangePinDialogResp (hSecHandle,(E_SecUeReturnVal)E_SEC_UE_CANCEL);
        SEC_KEYTYPE = 0;
    }
    else
    {        
        /*ASCII*/
        WE_INT32   iPinLen = M_SEC_PIN_MAX_SIZE + 1;
        WE_INT32   iNewPin2Len = M_SEC_PIN_MAX_SIZE + 1;
        WE_INT32   iOldPinLen = M_SEC_PIN_MAX_SIZE + 1;
        if(NULL == pcPin)
        {
            iPinLen = 0;
        }
        else            
        {
            iPinLen  = (WE_INT32)SEC_STRLEN(pcPin);
        }

        if(NULL == pcNewPin2)
        {
            iNewPin2Len = 0;
        }
        else
        {
            iNewPin2Len = (WE_INT32)SEC_STRLEN(pcNewPin2);
        }

        if(NULL == pcOldPin)
        {
            iOldPinLen = 0;
        }
        else
        {
            iOldPinLen = (WE_INT32)SEC_STRLEN(pcOldPin);
        }
        
        if ((iPinLen > (M_SEC_PIN_MAX_SIZE)) || 
            (iNewPin2Len > (M_SEC_PIN_MAX_SIZE)) || 
            (iOldPinLen > (M_SEC_PIN_MAX_SIZE)))
        {
            Sec_UeChangePinDialogAgain (hSecHandle,iTargetId,SEC_KEYTYPE, M_SEC_UE_INPUT_TOO_LONG);
        }
        else if ((iPinLen < (M_SEC_PIN_MIN_SIZE)) || 
            (iNewPin2Len < (M_SEC_PIN_MIN_SIZE)) || 
            (iOldPinLen < (M_SEC_PIN_MIN_SIZE)))
        {
            Sec_UeChangePinDialogAgain (hSecHandle,iTargetId,SEC_KEYTYPE, M_SEC_UE_INPUT_TOO_SHORT);
        }
        else if (iPinLen != iNewPin2Len)
        {
            Sec_UeChangePinDialogAgain (hSecHandle,iTargetId,SEC_KEYTYPE, M_SEC_UE_INPUT_MISIATCH);
        }
        else if((NULL != pcPin)&&(NULL != pcNewPin2)&&(NULL != pcOldPin))
        {
            WE_INT32 iIndex = 0;
            
            for (iIndex = 0; iIndex < iPinLen; iIndex++) 
            {
                if (pcPin[iIndex] != pcNewPin2[iIndex]) 
                {
                    Sec_UeChangePinDialogAgain (hSecHandle,iTargetId,SEC_KEYTYPE, M_SEC_UE_INPUT_MISIATCH);
                    return 0;
                }
            }
            Sec_UeChangePin (hSecHandle,SEC_KEYTYPE, pcOldPin, pcNewPin2);
        }
    }
    WE_FREE (pcPin);
    WE_FREE (pcNewPin2);
    WE_FREE (pcOldPin);     

    return M_SEC_ERR_OK;
}
#endif


#ifdef M_SEC_CFG_USE_KEYS  
/*3.choose certificate by name*/
/*==================================================================================================
FUNCTION: 
    Sec_UeSelectAction
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    choose certificate by name dialog response    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    St_SelectCertAction stChooseCerByName[IN]:user data
RETURN VALUE:
    M_SEC_ERR_OK is success ,otherwise is fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_UeSelectAction(WE_HANDLE hSecHandle ,WE_INT32 iTargetId,
                            St_SelectCertAction stChooseCerByName)
{
    iTargetId=iTargetId;
    if(NULL == hSecHandle)
    {
        Sec_UeChooseCertByNameResp (hSecHandle,0, (E_SecUeReturnVal)E_SEC_UE_CANCEL);
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    WE_LOG_MSG(( 0,0,"SEC: Sec_UeSelectAction(bIsOk =%d)", stChooseCerByName.bIsOk));
    if(!stChooseCerByName.bIsOk)
    {
        Sec_UeChooseCertByNameResp (hSecHandle,0, (E_SecUeReturnVal)E_SEC_UE_CANCEL);
        
    }
    else
    {
        WE_INT32 iIndex = stChooseCerByName.iSelId;
        WE_INT32 iCertId = 0;
        
        if (iIndex < 0) 
        {
            iCertId = 0;
        }
        else 
        {
            iCertId = SEC_CERTIDS[iIndex];
        }
        Sec_UeChooseCertByNameResp (hSecHandle,iCertId, (E_SecUeReturnVal)E_SEC_UE_OK);
    }
    
    WE_FREE (SEC_CERTIDS);
    SEC_CERTIDS = NULL;
    return M_SEC_ERR_OK;    
}
#endif
/*4.confirm display name*/
/*==================================================================================================
FUNCTION: 
    Sec_DlgCreateConfDispNameAction
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    confirm dialog response    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    St_NameConfirmAction stConfName[IN]:user data
RETURN VALUE:
    M_SEC_ERR_OK is success ,otherwise is fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_UeNameConfirmAction(WE_HANDLE hSecHandle ,WE_INT32 iTargetId,St_NameConfirmAction stConfName)
{
    iTargetId=iTargetId;
    if(NULL == hSecHandle)
    {
        Sec_UeConfirmDispNameResp (hSecHandle,FALSE);        
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    WE_LOG_MSG((0,0, "SEC: Sec_DlgCreateConfDispNameAction(bIsOk = %d)", stConfName.bIsOk));
    if(!stConfName.bIsOk)
    {
        Sec_UeConfirmDispNameResp (hSecHandle,FALSE);        
    }
    else
    {
        Sec_UeConfirmDispNameResp(hSecHandle,TRUE);
    }
    return M_SEC_ERR_OK;    
}

/*5.sign text confirm*/
/*==================================================================================================
FUNCTION: 
    Sec_UeSignTextConfirmAction
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    extension confirm dialog response    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    St_SignTextConfirmAction stExtConfirm[IN]:user data
RETURN VALUE:
    M_SEC_ERR_OK is success ,otherwise is fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_UeSignTextConfirmAction(WE_HANDLE hSecHandle ,WE_INT32 iTargetId,St_SignTextConfirmAction stExtConfirm)
{
    iTargetId=iTargetId;
    if(NULL == hSecHandle)
    {
        Sec_UeConfirmDialogResp (hSecHandle,FALSE);        
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    WE_LOG_MSG( (0,0,"SEC: Sec_UeSignTextConfirmAction(bIsOk =%d )", stExtConfirm.bIsOk));

    if(!stExtConfirm.bIsOk)
    {
        Sec_UeConfirmDialogResp (hSecHandle,FALSE);        
    }
    else
    {
        Sec_UeConfirmDialogResp (hSecHandle,TRUE);
    }
    return M_SEC_ERR_OK;
}
#ifdef M_SEC_CFG_USE_KEYS
/*6.generate pin*/
/*==================================================================================================
FUNCTION: 
    Sec_UeCreatePinAction
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    generate pin  dialog response
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    St_CreatePinAction stGenPin[IN]:user data
RETURN VALUE:
    M_SEC_ERR_OK is success ,otherwise is fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_UeCreatePinAction(WE_HANDLE hSecHandle ,WE_INT32 iTargetId,St_CreatePinAction stGenPin)
{
    /*UTF8*/
    WE_CHAR *pcPin = stGenPin.pcPinValue;
    WE_INT32   iPinLen = M_SEC_PIN_MAX_SIZE + 1; 

    if(NULL == hSecHandle)
    {
        Sec_UeGenPinResp (hSecHandle,(E_SecUeReturnVal)E_SEC_UE_CANCEL, NULL);        
        WE_FREE (pcPin);
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    WE_LOG_MSG((0,0, "SEC: Sec_UeCreatePinAction(bIsOk = %d)", stGenPin.bIsOk));
    if(!stGenPin.bIsOk)
    {
        Sec_UeGenPinResp (hSecHandle,(E_SecUeReturnVal)E_SEC_UE_CANCEL, NULL);        
    }
    else
    {
        if(NULL == pcPin)
        {
            iPinLen = 0;
        }
        else
        {
            iPinLen = (WE_INT32)SEC_STRLEN(pcPin);
        }
        
        if (iPinLen > (M_SEC_PIN_MAX_SIZE))
        {
            Sec_UeCreatePinAgain (hSecHandle,iTargetId,SEC_KEYTYPE, M_SEC_UE_INPUT_TOO_LONG);
        }
        else if (iPinLen < (M_SEC_PIN_MIN_SIZE))
        {
            Sec_UeCreatePinAgain (hSecHandle,iTargetId,SEC_KEYTYPE, M_SEC_UE_INPUT_TOO_SHORT);
        }
        else
        {
            Sec_UeGenPinResp (hSecHandle,(E_SecUeReturnVal)E_SEC_UE_OK, pcPin);
        }
        
    }
    WE_FREE (pcPin);
    return M_SEC_ERR_OK;
    
}

#endif
#ifdef M_SEC_CFG_USE_CERT

/*7.hash*/
/*==================================================================================================
FUNCTION: 
    Sec_UeHashAction
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    hash dialog response    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    St_HashAction stHash[IN]:user data
RETURN VALUE:
    M_SEC_ERR_OK is success ,otherwise is fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_UeHashAction(WE_HANDLE hSecHandle ,WE_INT32 iTargetId,St_HashAction stHash)
{
    WE_CHAR      *pcHash = stHash.pcHashValue; 
    if(NULL == hSecHandle)
    {
        Sec_UeHashDialogResp (hSecHandle,(E_SecUeReturnVal)E_SEC_UE_CANCEL);
        WE_FREE (pcHash);
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    WE_LOG_MSG(( 0,0,"SEC: Sec_UeHashAction(bIsOk = %d)", stHash.bIsOk));
    if(!stHash.bIsOk)
    {
        Sec_UeHashDialogResp (hSecHandle,(E_SecUeReturnVal)E_SEC_UE_CANCEL);
        WE_FREE (SEC_DISPLAYNAME);
        SEC_DISPLAYNAME = NULL;
        SEC_DISPLAYNAME_LEN = 0;
        SEC_CHARSET = 0;        
    }
    else
    {
        WE_INT32        iHashLen = 31;
        WE_INT32        iIndex = 0;
        WE_UINT8  uiZeroInAscii = 48; /*'0'*/
        WE_UINT8  uiHashAsNumber[30];

        if(NULL == pcHash)
        {
            iHashLen = 0;
        }
        else
        {
            iHashLen = (WE_INT32)SEC_STRLEN(pcHash);
        }
        
        if (iHashLen < 30)
        {
            Sec_UeHashAgain (hSecHandle,iTargetId,(WE_INT16)SEC_CHARSET, SEC_DISPLAYNAME, 
                                SEC_DISPLAYNAME_LEN, M_SEC_UE_INPUT_TOO_SHORT);
        }
        else if(NULL != pcHash)
        {
            for (iIndex = 0; iIndex < 30; iIndex++) 
            {
                uiHashAsNumber[iIndex] = (WE_UINT8)((pcHash[iIndex]) - uiZeroInAscii);
            }
            Sec_UeVerifyHash (hSecHandle,(WE_UINT8 *)uiHashAsNumber);
        }        
        
    }
    WE_FREE (pcHash);
    return M_SEC_ERR_OK;
}
#endif
#ifdef M_SEC_CFG_USE_KEYS

/*8.pin*/
/*==================================================================================================
FUNCTION: 
    Sec_UePinAction
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
   pin dialog response    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    St_PinAction stPin[IN]:user data
RETURN VALUE:
    M_SEC_ERR_OK is success ,otherwise is fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_UePinAction(WE_HANDLE hSecHandle ,WE_INT32 iTargetId,St_PinAction stPin)
{
    WE_CHAR *pcPin = stPin.pcPinValue;
    if(NULL == hSecHandle)
    {
        Sec_UePinDialogResp (hSecHandle,(E_SecUeReturnVal)E_SEC_UE_CANCEL);
        WE_FREE (pcPin);
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    WE_LOG_MSG( (0,0,"SEC: Sec_UePinAction(bIsOk = %d)", stPin.bIsOk));
    if(!stPin.bIsOk)
    {
        Sec_UePinDialogResp (hSecHandle,(E_SecUeReturnVal)E_SEC_UE_CANCEL);
        
    }
    else
    {
        WE_INT32   iPinLen = M_SEC_PIN_MAX_SIZE+1; 

        if(NULL == pcPin)
        {
            iPinLen = 0;
        }
        else
        {
            iPinLen = (WE_INT32)SEC_STRLEN(pcPin);
        }
        
        if (iPinLen > (M_SEC_PIN_MAX_SIZE))
        {
            Sec_UePinAgain (hSecHandle,iTargetId,SEC_PINCAUSE, M_SEC_UE_INPUT_TOO_LONG);
        }
        else if (iPinLen < (M_SEC_PIN_MIN_SIZE))
        {
            Sec_UePinAgain (hSecHandle,iTargetId,SEC_PINCAUSE,M_SEC_UE_INPUT_TOO_SHORT);
        }
        else 
        {
            switch (SEC_PINCAUSE) 
            {
            case E_SEC_OPEN_WIM:
            case E_SEC_STORE_CERT:
            case E_SEC_DELETE_CERT:
            case E_SEC_CREATE_AUTH_KEY:
                Sec_UeVerifyPin (hSecHandle,M_SEC_PRIVATE_AUTHKEY, pcPin);
                break;
                
            case E_SEC_SIGN_TEXT:
            case E_SEC_CREATE_NONREP_KEY:
                Sec_UeVerifyPin (hSecHandle,M_SEC_PRIVATE_NONREPKEY, pcPin);
                break;
                                
            default:
                break;
            }
        }
        
    }
    WE_FREE (pcPin);
    return M_SEC_ERR_OK;
}
#endif

/*9.show certificate*/

#ifdef M_SEC_CFG_USE_CERT
/*==================================================================================================
FUNCTION: 
    Sec_UeShowCertContentAction
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
   show certificate dialog response
    
ARGUMENTS PASSED:
Sec_UeShowCertContentAction[IN]:user data

RETURN VALUE:
    M_SEC_ERR_OK is success ,otherwise is fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_UeShowCertContentAction(WE_HANDLE hSecHandle ,WE_INT32 iTargeId,St_ShowCertContentAction stShowCer )
{
    WE_INT32 iIndex = 0;
    WE_INT32 iRes = 0;
    St_SecCertName* pstCertNames = NULL;
    if((NULL == hSecHandle)||(NULL == SEC_CERTNAMES))
    {         
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    pstCertNames = SEC_CERTNAMES;
    WE_LOG_MSG(( 0,0,"SEC: Sec_UeShowCertContentAction(bIsOk = %d)", stShowCer.bIsOk));

    /*SHOW Cert*/
    /*modify by bird 061208*/
    if(!stShowCer.bIsOk)
    {
        iRes = Sec_MsgDeleteCert (hSecHandle, iTargeId, SEC_CERTID);
        if (iRes != M_SEC_ERR_OK)
        {                
        }
        SEC_NBROFCERTS--;
        
        WE_FREE (pstCertNames[SEC_INDEX].pvFriendlyName);
        for (iIndex = SEC_INDEX; iIndex < SEC_NBROFCERTS; iIndex++) 
        {
            pstCertNames[iIndex] = pstCertNames[iIndex + 1];
        }
        pstCertNames[SEC_NBROFCERTS].pvFriendlyName = NULL;
        
        Sec_UeShowCertListAgain (hSecHandle,iTargeId);
        SEC_CERTID = 0;
        if(0 == SEC_NBROFCERTS)
        {
            WE_FREE (pstCertNames);
            SEC_CERTNAMES = NULL;
        }            
    }
    
    else
    {
        for (iIndex = 0; iIndex <SEC_NBROFCERTS; iIndex++) 
        {
            WE_FREE (pstCertNames[iIndex].pvFriendlyName);
        }
        WE_FREE (pstCertNames);
        SEC_CERTNAMES = NULL;
        SEC_NBROFCERTS = 0;

        SEC_CERTID = 0;            
    }        
    return M_SEC_ERR_OK;    
}
/*10.show certificate name*/
/*==================================================================================================
FUNCTION: 
    Sec_UeShowCerListAction
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    show certificate names dialog response    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    St_ShowCertListAction stShowCertName[IN]:user data
RETURN VALUE:
    M_SEC_ERR_OK is success ,otherwise is fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_UeShowCerListAction(WE_HANDLE hSecHandle ,WE_INT32 iTargetId,St_ShowCertListAction stShowCertName)
{
    St_SecCertName* pstCertNames = NULL;
    WE_INT32 iIndex = 0;    
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    if (NULL == SEC_CERTNAMES)
    {
        return M_SEC_ERR_OK;
    }
    pstCertNames = SEC_CERTNAMES;
    WE_LOG_MSG((0,0, "SEC: Sec_UeShowCerListAction(bIsOk = %d)", stShowCertName.bIsOk));    
    if(!stShowCertName.bIsOk)
    {
        if(stShowCertName.eRKeyType != E_KEY_CANCEL )
        {
            WE_INT32 iCertId = 0;        
            iIndex = stShowCertName.iSelId;        
            SEC_INDEX = iIndex;
            if (iIndex < 0) 
            {
                iCertId = 0;
            }
            else 
            {
                iCertId = pstCertNames[iIndex].iCertId;
            }
            iRes = Sec_MsgChangeWTLSCertAble(hSecHandle,iTargetId,iCertId);
        }
        else /*free memory*/
        {
            for (iIndex = 0; iIndex <SEC_NBROFCERTS; iIndex++) 
            {
                WE_FREE (pstCertNames[iIndex].pvFriendlyName);
            }
            SEC_NBROFCERTS = 0;
            WE_FREE (pstCertNames);
            SEC_CERTNAMES = NULL;        
        }
    }
    else
    {
        WE_INT32 iCertId = 0;        
        iIndex = stShowCertName.iSelId;        
        SEC_INDEX = iIndex;
        if (iIndex < 0) 
        {
            iCertId = 0;
        }
        else 
        {
            iCertId = pstCertNames[iIndex].iCertId;
        }
        iRes = Sec_MsgViewAndGetCert(hSecHandle, iTargetId, iCertId);      
        if (iRes != M_SEC_ERR_OK)
        {
            return iRes;
        }
    }
    
    return M_SEC_ERR_OK;    
}
#endif

/*11.show session info*/
/*None*/

/*12.show stored contract*/
/*==================================================================================================
FUNCTION: 
    Sec_UeShowContractsListAction
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    show stored contracts  dialog response
    
ARGUMENTS PASSED:
St_StoredContractsAction stStoredContracts[IN]:user data

RETURN VALUE:
    M_SEC_ERR_OK is success ,otherwise is fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_UeShowContractsListAction(WE_HANDLE hSecHandle, WE_INT32 iTargetId,St_ShowContractsListAction stStoredContracts)
{
    St_SecContractInfo* pstContractVec = NULL;
    WE_INT32 iIndex = 0;
    if(NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    if (NULL == SEC_CONTRACTVEC)
    {
        return M_SEC_ERR_OK;
    }
    pstContractVec = SEC_CONTRACTVEC;
    WE_LOG_MSG((0,0, "SEC: Sec_UeShowContractsListAction(bIsOk = %d)", stStoredContracts.bIsOk));
    if(!stStoredContracts.bIsOk)
    {        
        SEC_NBRCONTRACTS = 0;
        
        for (iIndex = 0;iIndex < SEC_MAX_NBRCONTRACTS; iIndex++) 
        {
            WE_FREE (pstContractVec[iIndex].pcContract);
            WE_FREE (pstContractVec[iIndex].pcSignature);
        }
        WE_FREE (pstContractVec); 
        SEC_CONTRACTVEC = NULL;        
    }
    else
    {
        iIndex = stStoredContracts.iSelId;        
        SEC_CONTRACT_INDEX = iIndex;
        if ((iIndex >= 0) && (SEC_NBRCONTRACTS > 0)) 
        {
            if (Sec_UeShowContract (hSecHandle,iTargetId,iIndex) != M_SEC_ERR_OK) 
            {
#ifdef M_SEC_CFG_SHOW_WARNINGS
                Sec_iUeWarning(hSecHandle,iTargetId,M_SEC_ERR_GENERAL_ERROR);
#endif
                SEC_CONTRACT_INDEX = 0;
            }
        }
    }
    return M_SEC_ERR_OK;    
}
#ifdef M_SEC_CFG_USE_CERT

/*13.store certificate*/
/*==================================================================================================
FUNCTION: 
    Sec_UeStoreCertAction
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    store certificate dialog response
    
ARGUMENTS PASSED:
St_StoreCertAction stStoreCert[IN]:user data

RETURN VALUE:
    M_SEC_ERR_OK is success ,otherwise is fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_UeStoreCertAction(WE_HANDLE hSecHandle ,WE_INT32 iTargetId,St_StoreCertAction stStoreCert)
{
    WE_CHAR       *pcFriendlyName = NULL;
    iTargetId=iTargetId;
    if(NULL == hSecHandle)
    {
        Sec_UeStoreCertDialogResp (hSecHandle,FALSE, NULL, 0, 0);        
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    WE_LOG_MSG((0,0, "SEC: Sec_UeStoreCertAction(bIsOk = %d)", stStoreCert.bIsOk));
    if(!stStoreCert.bIsOk)
    {
        Sec_UeStoreCertDialogResp (hSecHandle,FALSE, NULL, 0, 0);        
    }
    else
    {
        /*user certificate*/
        if (SEC_CERTTYPE == M_SEC_UE_USER_CERT) 
        {
            WE_UINT16      usFriendlyNameLen = M_SEC_MAX_INPUT_LEN + 1;
            pcFriendlyName = stStoreCert.pcText;
            
            if(NULL == pcFriendlyName)
            {
                usFriendlyNameLen = 0;
            }
            else
            {
                usFriendlyNameLen = (WE_UINT16)SEC_STRLEN(pcFriendlyName);
            }
            
            if (usFriendlyNameLen == 0) 
            {
                Sec_UeStoreCertDialogResp (hSecHandle,TRUE, NULL, 0, 0);
            }
            else 
            {
                Sec_UeStoreCertDialogResp (hSecHandle,TRUE, pcFriendlyName, usFriendlyNameLen,
                                                                        WE_ASN1_UTF8STRING);
            }
            WE_FREE (pcFriendlyName);
        }
        /*CA certificate*/
        else 
        {
            Sec_UeStoreCertDialogResp (hSecHandle,(WE_UINT8)TRUE, NULL, 0, 0);
        }
    }
    return M_SEC_ERR_OK;
    
    
}

#endif 
/*14.warning*/
/*None*/

/*15.show stored contracts*/
/*==================================================================================================
FUNCTION: 
    Sec_UeShowContractContentAction
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    show contract dialog response    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN/OUT]: Global data handle.
    St_ShowContractContentAction stShowContract[IN]:user data
RETURN VALUE:
    M_SEC_ERR_OK is success ,otherwise is fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_UeShowContractContentAction(WE_HANDLE hSecHandle ,WE_INT32 iTargetID,St_ShowContractContentAction stShowContract)
{
    St_SecContractInfo* pstContractVec = NULL;
    WE_INT32 iIndex = 0;
    WE_INT32 iRes = 0;

    if((NULL == hSecHandle)||(NULL == SEC_CONTRACTVEC))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    pstContractVec = SEC_CONTRACTVEC;
    
    WE_LOG_MSG((0,(WE_UINT8)0, "SEC: Sec_UeShowContractContentAction(bIsOk = %d)", stShowContract.bIsOk));
    if(!stShowContract.bIsOk)
    {
        iRes = Sec_MsgDeleteContract (hSecHandle, 15, (WE_INT32)(pstContractVec[SEC_CONTRACT_INDEX].usContractId));
        if (iRes != M_SEC_ERR_OK)
        {
        }

        SEC_NBRCONTRACTS--;        
        WE_FREE (pstContractVec[SEC_CONTRACT_INDEX].pcContract);
        WE_FREE (pstContractVec[SEC_CONTRACT_INDEX].pcSignature);
        for (iIndex = SEC_CONTRACT_INDEX; iIndex < SEC_NBRCONTRACTS; iIndex++) 
        {
            pstContractVec[iIndex] = pstContractVec[iIndex + 1];
        }
        pstContractVec[SEC_NBRCONTRACTS].pcContract = NULL;
        pstContractVec[SEC_NBRCONTRACTS].pcSignature = NULL;
        
        Sec_UeShowContractListAgain (hSecHandle,iTargetID);
        if(0 == SEC_NBRCONTRACTS)
        {
            WE_FREE(pstContractVec);
            SEC_CONTRACTVEC = NULL;
        }
        SEC_CONTRACT_INDEX = 0;        
    }
    else
    {
        for (iIndex = 0;iIndex < SEC_MAX_NBRCONTRACTS; iIndex++) 
        {
            WE_FREE (pstContractVec[iIndex].pcContract);
            WE_FREE (pstContractVec[iIndex].pcSignature);
        }
        WE_FREE (pstContractVec); 
        SEC_NBRCONTRACTS = 0;        
        SEC_CONTRACTVEC = NULL;
    }
    return M_SEC_ERR_OK;    
}
/*16.allow delete keys*/
/*None*/
