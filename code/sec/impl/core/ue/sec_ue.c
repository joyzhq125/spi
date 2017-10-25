/*=====================================================================================
    FILE NAME : sec_ue.c
    MODULE NAME : SEC
    
    GENERAL DESCRIPTION
        
    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2002-2006 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
    
    Modification Tracking
    Date       Author         Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-07-04 Kevin     none      Init
    
=====================================================================================*/

/*==================================================================================================
*   Include File 
*=================================================================================================*/
#include"sec_comm.h"
#include "oem_seccsc.h"

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/

#define M_SEC_HASH_MD5               0x01 
#define M_SEC_HASH_SHA               0x02 

#define M_SEC_CFG_SHOW_COUNTRY_NAME             0x00000001
#define M_SEC_CFG_SHOW_ORGANISATION_NAME        0x00000002
#define M_SEC_CFG_SHOW_ORGANISATIONAL_UNIT_NAME 0x00000004
#define M_SEC_CFG_SHOW_STATE_OR_PROVINCE_NAME   0x00000008
#define M_SEC_CFG_SHOW_COMMON_NAME              0x00000010
#define M_SEC_CFG_SHOW_DOMAIN_COMPONENT         0x00000020
#define M_SEC_CFG_SHOW_SERIAL_NUMBER            0x00000040
#define M_SEC_CFG_SHOW_DN_QUALIFIER             0x00000080
#define M_SEC_CFG_SHOW_LOCALITY_NAME            0x00000100
#define M_SEC_CFG_SHOW_TITLE                    0x00000200
#define M_SEC_CFG_SHOW_SURNAME                  0x00000400
#define M_SEC_CFG_SHOW_GIVENNAME                0x00000800
#define M_SEC_CFG_SHOW_INITIALS                 0x00001000
#define M_SEC_CFG_SHOW_GENERATION_QUALIFIER     0x00002000
#define M_SEC_CFG_SHOW_NAME                     0x00004000
#define M_SEC_CFG_SHOW_EMAIL_ADDRESS            0x00008000
#define M_SEC_CFG_SHOW_UNKNOWN_ATTRIBUTE_TYPE   0x00010000

/* Global variables*/
#define SEC_CBBRS_EVTFUNC     (((ISec*)hSecHandle)-> hcbSecEvtFunc)
#define SEC_CBWAP_EVTFUNC   (((ISec*)hSecHandle)-> hcbSecEvtFunc)
#define SEC_BRS_PRIVDATA       (((ISec*)hSecHandle)-> hSecPrivDataCB)
#define SEC_WAP_PRIVDATA      (((ISec*)hSecHandle)-> hSecPrivDataCB)
#define SEC_CSC_SHELL              (((ISec*)hSecHandle)-> m_pIShell)
#define SEC_IFTYPE                     (((ISec*)hSecHandle)->iIFType)
#define M_SEC_UECALLBACK       (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->m_SecUeCB)
#define M_SEC_UECALLBACKDATA  (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pvUeCBData)
#define M_SEC_UECALLBACKEVENT  (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->iUeEvent)

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
static WE_BOOL Sec_UeSendEvent(WE_HANDLE hSecHandle,WE_INT32 iTargetId, WE_INT32 eSecEvt,void *pvData);

static WE_INT32 Sec_UeTimeToStr(WE_UINT32 uiTime, WE_CHAR **ppcTimeStr);

static WE_INT32 Sec_UeCsToUtf8(WE_HANDLE hSecHandle,WE_UCHAR *pucStrToCvt, WE_LONG lStrToCvtLen, 
                               WE_CHAR *pcDst, WE_LONG *plDstLen, WE_INT16 sCharSet);

static WE_CHAR* Sec_UeGetIssuerStr(WE_HANDLE hSecHandle,WE_INT16 sCharSet, 
                                   void *pvIssuer, WE_INT32 iIssuerLen);

static WE_CHAR* Sec_UeCreateFingerpr(WE_UCHAR *pucFingerPrint,WE_INT32 iFingerPrintLen);
static WE_VOID Sec_RunUeCallBackFun(WE_VOID* pvData);
static WE_VOID Sec_UeFreeMem(WE_INT32 eSecEvt, void* pvData);



/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*call by stk or browser*/
/*==================================================================================================
FUNCTION: 
    Sec_UeConfirm
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION:    
    displays a dialog to the end user with a given message    
ARGUMENTS PASSED:
    WE_INT32 iMsg[IN]:An index to the message displayed to the end user.
    WE_INT32 iYesMsg[IN]:An index to the message displayed on the "yes" button
    WE_INT32 iNoMsg[IN]:An index to the message displayed on the "no" button
RETURN VALUE:
    TRUE: success
    FALSE: fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_BOOL Sec_UeConfirm(WE_HANDLE hSecHandle,WE_INT32 iTargetId,
						WE_INT32 iMsg, WE_INT32 iYesMsg, WE_INT32 iNoMsg)
{
    St_Confirm *pstConfirm = NULL;
    iYesMsg = iYesMsg;
    iNoMsg = iNoMsg;
    if(NULL == (pstConfirm = (St_Confirm*)WE_MALLOC(sizeof(St_Confirm))))
    {
        WE_LOG_MSG((0,0, "SEC:malloc1 error in Sec_UeConfirm!\n"));
        return FALSE;
    }
    (void)WE_MEMSET((void*)pstConfirm,0,sizeof(St_Confirm));
    pstConfirm->iConfirmStrId = iMsg;
    /*send event to stk*/
    return Sec_UeSendEvent(hSecHandle,iTargetId, M_UE_CONFIRM_EVENT, (void *)pstConfirm);
}

/*==================================================================================================
FUNCTION: 
    Sec_UeSignTextConfirm
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
   Create an extended confirmation dialog with the supplied text, 
   message, yes message, and no message.
ARGUMENTS PASSED:
    WE_INT32 iMsg[IN]:An index to the message displayed to the end user.
    WE_CHAR *pcText[IN]:The message to be signed
    WE_INT32 iYesMsg[IN]:An index to the message displayed on the "yes" button
    WE_INT32 iNoMsg[IN]:An index to the message displayed on the "no" button

RETURN VALUE:
    TRUE: success
    FALSE: fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_BOOL Sec_UeSignTextConfirm(WE_HANDLE hSecHandle,WE_INT32 iTargetId,
					WE_INT32 iMsg, const WE_CHAR *pcText, 
                              	WE_INT32 iYesMsg, WE_INT32 iNoMsg)
{
    St_SignTextConfirm *pstExtConfirm = NULL;
    WE_CHAR* pcTextBuf = NULL;
    if((pstExtConfirm = (St_SignTextConfirm*)WE_MALLOC(sizeof(St_SignTextConfirm))) == NULL)
    {
        WE_LOG_MSG((0,0, "SEC:malloc1 error in Sec_UeSignTextConfirm!\n"));
        return FALSE;
    }
    (void)WE_MEMSET(pstExtConfirm,0,sizeof(St_SignTextConfirm));
    pstExtConfirm->iInfoId = iMsg;
    
    /*UTF8*/
    if(NULL != pcText )
    {
        if((pcTextBuf = (WE_CHAR*)WE_MALLOC(SEC_STRLEN(pcText)+1)) == NULL)
        {
            WE_FREE(pstExtConfirm);
            return FALSE;
        }
        (void)WE_MEMSET(pcTextBuf ,0,SEC_STRLEN(pcText)+1);
        (void)WE_MEMCPY(pcTextBuf,pcText,SEC_STRLEN(pcText));
    }
    pstExtConfirm->pcText = pcTextBuf;
    /*send event to browser*/
    iYesMsg = iYesMsg;
    iNoMsg = iNoMsg;
    return Sec_UeSendEvent(hSecHandle,iTargetId,M_UE_SIGNTEXT_CONFIRM_EVENT, (void *)pstExtConfirm);
}

#ifdef M_SEC_CFG_SHOW_WARNINGS
/*==================================================================================================
FUNCTION: 
    Sec_UeWarning
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    Create a warning dialog with the supplied message

ARGUMENTS PASSED:
    WE_INT32 iMsg[IN]:An index to the message displayed to the end user.
RETURN VALUE:
    TRUE: success
    FALSE: fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_BOOL Sec_UeWarning(WE_HANDLE hSecHandle, WE_INT32 iTargetId,
					WE_INT32 iMsg)
{
    St_Warning *pstWaring = NULL;

    if (NULL == hSecHandle)
    {
        return FALSE;
    }
    if(NULL == (pstWaring = (St_Warning*)WE_MALLOC(sizeof(St_Warning))))
    {
        WE_LOG_MSG((0,0, "SEC:malloc1 error in Sec_UeWarning!\n"));
        return FALSE;
    }
    (void)WE_MEMSET(pstWaring,0,sizeof(St_Warning));
    
    /* Assign the warning message */
    switch (iMsg)
    {
        case M_SEC_ERR_MISSING_NR_KEY_SIGN_TEXT:
            pstWaring->iWarningContent = M_SEC_ID_WARNING_MISSING_NR_KEY_SIGN_TEXT;
            break;
        case M_SEC_ERR_MISSING_AUT_KEY_OPEN_WIM:
            pstWaring->iWarningContent = M_SEC_ID_WARNING_MISSING_AUT_KEY_OPEN_WIM;
            break;
        case M_SEC_ERR_COULD_NOT_STORE_CONTRACT:
            pstWaring->iWarningContent = M_SEC_ID_WARNING_COULD_NOT_STORE_CONTRACT;
            break;
        case M_SEC_ERR_INSUFFICIENT_MEMORY:
            pstWaring->iWarningContent = M_SEC_ID_WARNING_INSUFFICIENT_MEMORY;
            break;
        case M_SEC_ERR_GENERAL_ERROR:
            pstWaring->iWarningContent = M_SEC_ID_WARNING_GENERAL_ERROR;
            break;
        case M_SEC_ERR_INVALID_PARAMETER:
            pstWaring->iWarningContent = M_SEC_ID_WARNING_INVALID_PARAMETER;
            break;

        case M_SEC_ERR_USER_NOT_VERIFIED:
            pstWaring->iWarningContent  = M_SEC_ID_WARNING_USER_NOT_VERIFIED;
            break;
        case M_SEC_ERR_MISSING_AUT_KEY_HANDSHAKE:
            pstWaring->iWarningContent  = M_SEC_ID_WARNING_MISSING_AUT_KEY_HANDSHAKE;
            break;
        default:
            WE_FREE(pstWaring);
            return FALSE;
    }    
    /*send event to STK*/
    return Sec_UeSendEvent(hSecHandle,iTargetId, M_UE_WARNING_EVENT, (void *)pstWaring);
}
#endif
/*==================================================================================================
FUNCTION: 
    Sec_UeSessionInfo
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    shows the session info of the current session
 ARGUMENTS PASSED:
    St_SecSessionInfo stSessInf[IN]:The session information.
RETURN VALUE:
    TRUE: success
    FALSE: fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_BOOL Sec_UeSessionInfo(WE_HANDLE hSecHandle,WE_INT32 iTargetId,
						St_SecSessionInfo stSessInf)
{
    St_ShowSessionContent *pstSessionInfo = NULL;
    WE_CHAR* pcText = NULL;
    if((pstSessionInfo = (St_ShowSessionContent*)WE_MALLOC(sizeof(St_ShowSessionContent))) == NULL)
    {
        WE_LOG_MSG((0,0, "SEC:malloc error1 in Sec_UeSessionInfo!\n"));
        return FALSE;
    }
    (void)WE_MEMSET(pstSessionInfo,0,sizeof(St_ShowSessionContent));
    
    switch (stSessInf.ucConnectionType)
    {        
        case M_SEC_WTLS_CONNECTION_MODE:
            pstSessionInfo->iConnTypeId = (M_SEC_ID_SESSION_WTLS);
            break;
            
        default:
            WE_LOG_MSG((0,0, "SEC:connectionType is not know in Sec_UeSessionInfo!\n"));
            WE_FREE(pstSessionInfo);
            return FALSE;
    }
    
    switch (stSessInf.ucHmacAlg)
    {
        case M_SEC_HASH_MD5:
            pstSessionInfo->iHmacId = (M_SEC_ID_SESSION_MD5);
            break;
        case M_SEC_HASH_SHA:
            pstSessionInfo->iHmacId = (M_SEC_ID_SESSION_SHA1);
            break;
        default:
            WE_LOG_MSG((0,0, "SEC:HMAC alg is not know in Sec_UeSessionInfo!\n"));
            WE_FREE(pstSessionInfo);
            return FALSE;
    }
    /*HMAC KEY LEN*/
    if(NULL == (pcText = (WE_CHAR *)WE_MALLOC(4)))
    {
        WE_LOG_MSG((0,0, "SEC:malloc error2 in Sec_UeSessionInfo!\n"));
        WE_FREE(pstSessionInfo);
        return FALSE;
    }
    (void)WE_MEMSET(pcText,0,4);
    (void)SEC_SPRINTF(pcText, "%u", stSessInf.usHmacLen);
    pstSessionInfo->pcHmacKeyLenStr = pcText;
    
    /*ENCRYPTION ALG*/
    switch (stSessInf.ucEncryptionAlg)
    {
        case E_SEC_BULK_NULL:
            pstSessionInfo->iEncralgId = (M_SEC_ID_SESSION_BULK_NULL);
            break;
        case E_SEC_BULK_RC5_CBC_40:
            pstSessionInfo->iEncralgId = (M_SEC_ID_SESSION_BULK_RC5_40);
            break;
        case E_SEC_BULK_RC5_CBC_56:
            pstSessionInfo->iEncralgId = (M_SEC_ID_SESSION_BULK_RC5_56);
            break;
        case E_SEC_BULK_RC5_CBC:
            pstSessionInfo->iEncralgId = (M_SEC_ID_SESSION_BULK_RC5);
            break;
        case E_SEC_BULK_3DES_CBC_EDE:
            pstSessionInfo->iEncralgId = (M_SEC_ID_SESSION_BULK_3DES);
            break;
        case E_SEC_BULK_RC5_CBC_64:
            pstSessionInfo->iEncralgId = (M_SEC_ID_SESSION_BULK_RC5_64);
            break;
        default:
            WE_LOG_MSG((0,0, "SEC:Encry alg is not know in Sec_UeSessionInfo!\n"));
            WE_FREE(pstSessionInfo);
            return FALSE;
    }
    /*ENCRYPTION KEY LEN*/
    if(NULL == (pcText = (WE_CHAR *)WE_MALLOC(4)))
    {
        WE_LOG_MSG(( 0,0,"SEC:malloc error3 in Sec_UeSessionInfo!\n"));
        WE_FREE(pstSessionInfo->pcHmacKeyLenStr);
        WE_FREE(pstSessionInfo);
        return FALSE;
    }
    (void)WE_MEMSET(pcText,0,4);
    (void)SEC_SPRINTF(pcText, "%d", stSessInf.usEncryptionKeyLen);
    pstSessionInfo->pcEncrKeyLenStr = pcText;
    
    /* Create key exchange alg  */
    switch (stSessInf.ucKeyExchangeAlg)
    {        
        case E_SEC_WTLS_KEYEXCHANGE_NULL:
            pstSessionInfo->iKeyExchId = (M_SEC_ID_SESSION_KEYEXCH_NULL);
            break;
        case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON:
            pstSessionInfo->iKeyExchId = (M_SEC_ID_SESSION_KEYEXCH_RSA_ANON);
            break;
        case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_512:
            pstSessionInfo->iKeyExchId = (M_SEC_ID_SESSION_KEYEXCH_RSA_ANON_512);
            break;
        case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_768:
            pstSessionInfo->iKeyExchId = (M_SEC_ID_SESSION_KEYEXCH_RSA_ANON_768);
            break;
        case E_SEC_WTLS_KEYEXCHANGE_RSA:
            pstSessionInfo->iKeyExchId = (M_SEC_ID_SESSION_KEYEXCH_RSA);
            break;
        case E_SEC_WTLS_KEYEXCHANGE_RSA_512:
            pstSessionInfo->iKeyExchId = (M_SEC_ID_SESSION_KEYEXCH_RSA_512);
            break;
        case E_SEC_WTLS_KEYEXCHANGE_RSA_768:
            pstSessionInfo->iKeyExchId = (M_SEC_ID_SESSION_KEYEXCH_RSA_768);
            break;
        default:
            WE_LOG_MSG((0,0, "SEC:WTLS key exchange alg is not know in Sec_UeSessionInfo!\n"));
            WE_FREE(pstSessionInfo->pcHmacKeyLenStr);
            WE_FREE(pstSessionInfo->pcEncrKeyLenStr);
            WE_FREE(pstSessionInfo);
            return FALSE;
    }
    
    /*Key exchange len*/
    pcText = (WE_CHAR *)WE_MALLOC(6);
    if(NULL == pcText)
    {
        WE_LOG_MSG((0,0, "SEC:malloc error4 in Sec_UeSessionInfo!\n"));
        WE_FREE(pstSessionInfo->pcHmacKeyLenStr);
        WE_FREE(pstSessionInfo->pcEncrKeyLenStr);
        WE_FREE(pstSessionInfo);
        return FALSE;
    }
    (void)WE_MEMSET(pcText,0,6);
    (void)SEC_SPRINTF(pcText, "%d", stSessInf.usKeyExchangeKeyLen);
    pstSessionInfo->pcKeyExchKeyLenStr = pcText;    
    /*send event to browser*/
    return Sec_UeSendEvent(hSecHandle,iTargetId, M_UE_SHOW_SESSION_CONTENT_EVENT, (void *)pstSessionInfo);
}

#ifdef M_SEC_CFG_USE_CERT
/*==================================================================================================
FUNCTION: 
    Sec_UeShowCertList
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    supplies a list of the ids and the friendly names of the stored certificates
ARGUMENTS PASSED:
    WE_INT32 iNbrOfCerts[IN]:The number of elements in certNames.
    St_SecCertName *pstCertNames[IN]:array of certname
RETURN VALUE:
    TRUE: success
    FALSE: fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_BOOL Sec_UeShowCertList(WE_HANDLE hSecHandle, WE_INT32 iTargetId,
                            WE_INT32 iNbrOfCerts, 
                            St_SecCertName *pstCertNames)
{
    St_ShowCertList *pstShowCertName = NULL;
    WE_CHAR* pcNameStr = NULL;
    WE_INT32 iIndex = 0;
    
    if((NULL == pstCertNames) && (iNbrOfCerts != 0))
    {
        return FALSE;
    }
    if(NULL == (pstShowCertName = (St_ShowCertList*)WE_MALLOC(sizeof(St_ShowCertList))))
    {
        WE_LOG_MSG(( 0,0,"SEC:malloc error1 in Sec_UeShowCertList!\n"));
        return FALSE;
    }
    (void)WE_MEMSET(pstShowCertName,0,sizeof(St_ShowCertList));
    
    pstShowCertName->nbrOfCerts = iNbrOfCerts;
    
    pstShowCertName->ppcCertName = (WE_CHAR**)WE_MALLOC((WE_UINT32)iNbrOfCerts * sizeof(WE_CHAR *));
    pstShowCertName->pucAble = (WE_UINT8*)WE_MALLOC((WE_UINT32)iNbrOfCerts * sizeof(WE_UINT8));
    pstShowCertName->pueCertType = (E_CertType*)WE_MALLOC((WE_UINT32)iNbrOfCerts * sizeof(E_CertType));
    if(((NULL == pstShowCertName->ppcCertName) ||
        (NULL == pstShowCertName->pucAble) ||
        (NULL == pstShowCertName->pueCertType))&&(0 != iNbrOfCerts))
    {
        WE_LOG_MSG((0,0, "SEC:malloc error2 in Sec_UeShowCertList!\n"));
        if(NULL != pstShowCertName->ppcCertName)
        {
            WE_FREE(pstShowCertName->ppcCertName);
            pstShowCertName->ppcCertName = NULL;
        }
        if(NULL != pstShowCertName->pucAble)
        {
            WE_FREE(pstShowCertName->pucAble);
            pstShowCertName->pucAble = NULL;
        }
        if(NULL != pstShowCertName->pueCertType)
        {
            WE_FREE(pstShowCertName->pueCertType);
            pstShowCertName->pueCertType = NULL;           
        }
        WE_FREE(pstShowCertName);
        pstShowCertName = NULL;
        return FALSE;
    }
    
    /* Create name strings */
    for (iIndex = 0; iIndex < iNbrOfCerts; iIndex++)
    {
        if (pstCertNames != NULL)
        {
            if ((pstCertNames[iIndex].cNameType == M_SEC_CERT_NAME_WTLS) || 
                (pstCertNames[iIndex].cNameType == M_SEC_CERT_NAME_X509))
                /* The friendly name is a WTLS or X.509 issuer/subject */
            {
                pcNameStr = Sec_UeGetIssuerStr(hSecHandle,pstCertNames[iIndex].sCharSet, 
                    (void *)pstCertNames[iIndex].pvFriendlyName, 
                    pstCertNames[iIndex].usFriendlyNameLen);
                if(NULL == pcNameStr)
                {
                    WE_INT32 iLoop=0;
                    WE_LOG_MSG((0,0, "SEC:WTLS x509 malloc error3 in Sec_UeShowCertList!\n"));
                    for(iLoop=0;iLoop<iIndex;iLoop++)
                    {
                        if(pstShowCertName->ppcCertName[iIndex])
                        {
                            WE_FREE(pstShowCertName->ppcCertName[iIndex]);
                        }
                    } 
                    WE_FREE(pstShowCertName->ppcCertName);
                    WE_FREE(pstShowCertName->pucAble);                   
                    WE_FREE(pstShowCertName->pueCertType);
                    WE_FREE(pstShowCertName);
                    return FALSE;
                }
            }            
            else if (pstCertNames[iIndex].cNameType == M_SEC_WRITTEN_NAME)
            {
                /* The friendly name is written by the user */
                WE_LONG  lTmpNameLen = pstCertNames[iIndex].usFriendlyNameLen ;
                pcNameStr = (WE_CHAR *)WE_MALLOC(2 * pstCertNames[iIndex].usFriendlyNameLen + 5);
                if(NULL == pcNameStr)
                {
                    WE_INT32 iLoop=0;
                    WE_LOG_MSG((0,0, "SEC:WRITTEN NAME malloc error3 in Sec_UeShowCertList!\n"));
                    for(iLoop=0;iLoop<iIndex;iLoop++)
                    {
                        if(pstShowCertName->ppcCertName[iIndex])
                        {
                            WE_FREE(pstShowCertName->ppcCertName[iIndex]);
                        }
                    } 
                    WE_FREE(pstShowCertName->ppcCertName);
                    WE_FREE(pstShowCertName->pucAble);
                    WE_FREE(pstShowCertName->pueCertType);
                    WE_FREE(pstShowCertName);
                    return FALSE;
                }
                (void)WE_MEMSET(pcNameStr,0,2 * pstCertNames[iIndex].usFriendlyNameLen + 5);
                /* Convert string to UTF8 */
                if (Sec_UeCsToUtf8(hSecHandle,(WE_UCHAR*)pstCertNames[iIndex].pvFriendlyName, 
                    pstCertNames[iIndex].usFriendlyNameLen, pcNameStr, &lTmpNameLen, 
                    pstCertNames[iIndex].sCharSet) != M_SEC_ERR_OK)
                {
                    WE_INT32 iLoop=0;
                    WE_LOG_MSG((0,0, "SEC:cvttoutf8 error in Sec_UeShowCertList!\n"));
                    for(iLoop=0;iLoop<iIndex;iLoop++)
                    {
                        if(pstShowCertName->ppcCertName[iIndex])
                        {
                            WE_FREE(pstShowCertName->ppcCertName[iIndex]);
                        }
                    } 
                    WE_FREE(pstShowCertName->ppcCertName);
                    WE_FREE(pstShowCertName->pucAble);
                    WE_FREE(pstShowCertName->pueCertType);
                    WE_FREE(pstShowCertName);
                    WE_FREE(pcNameStr);
                    return FALSE;
                }            
            }
            
            /* Insert elements */
            
            pstShowCertName->ppcCertName[iIndex] = pcNameStr;
            /*added by bird 061120*/
            pstShowCertName->pucAble[iIndex] = pstCertNames[iIndex].ucAble;
            pstShowCertName->pueCertType[iIndex] = (E_CertType)pstCertNames[iIndex].ucCertType;
        }
    }
    
    /*send event to browser*/
    return Sec_UeSendEvent(hSecHandle,iTargetId, M_UE_SHOW_CERT_LIST_EVENT, (void *)pstShowCertName);
}
#endif

#ifdef M_SEC_CFG_USE_KEYS
/* Create a certificate names menu with the supplied certificate names.
 * Returns a pointer to a struct sec_ti_t, or NULL on error. */
/*==================================================================================================
FUNCTION: 
    Sec_UeSelectCert
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    used when more than one user certificate is available in the functions
ARGUMENTS PASSED:
    WE_INT32 iTitle[IN]:An index to the message displayed to the end user.
    WE_INT32 iOkActionId[IN]:An index to the message displayed on the "yes" button
    WE_INT32 iNbrOfCerts[IN]:The number of elements in certNames
    St_SecCertName *pstCertNames[IN]:An array of elements 
RETURN VALUE:
    TRUE: success
    FALSE: fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_BOOL Sec_UeSelectCert(WE_HANDLE hSecHandle,WE_INT32 iTargetId,
				            WE_INT32 iTitle, 
                            WE_INT32 iOkActionId, WE_INT32 iNbrOfCerts,
                            St_SecCertName *pstCertNames)
{
    WE_INT32 iIndex = 0;    
    WE_CHAR* pcNameStr = NULL;    
    St_SelectCert *pstChooseCertByName = NULL;
    iTitle = iTitle;
    iOkActionId = iOkActionId;
    if(((NULL == pstCertNames)&&(iNbrOfCerts != 0)) || (NULL == hSecHandle))
    {
        return FALSE;
    }
    if(NULL == (pstChooseCertByName = (St_SelectCert*)WE_MALLOC(sizeof(St_SelectCert))))
    {
        WE_LOG_MSG((0,0, "SEC:malloc error1 in Sec_UeSelectCert!\n"));
        return FALSE;
    }
    (void)WE_MEMSET(pstChooseCertByName,0,sizeof(St_SelectCert));
    
    pstChooseCertByName->nbrOfCerts = iNbrOfCerts;    
    
    pstChooseCertByName->ppcCertName = (WE_CHAR**)WE_MALLOC((WE_UINT32)iNbrOfCerts * sizeof(WE_CHAR *));
    if((NULL == pstChooseCertByName->ppcCertName) && (iNbrOfCerts != 0))
    {
        WE_LOG_MSG((0,0, "SEC:malloc error2 in Sec_UeSelectCert!\n"));
        WE_FREE(pstChooseCertByName);
        return FALSE;
     }
    /* Create name strings */
    for (iIndex = 0; iIndex < iNbrOfCerts; iIndex++)
    {
        if (pstCertNames != NULL)
        {
            if ((pstCertNames[iIndex].cNameType == M_SEC_CERT_NAME_WTLS) 
                || (pstCertNames[iIndex].cNameType == M_SEC_CERT_NAME_X509))
                /* The friendly name is a WTLS or X.509 issuer/subject */
            {
                pcNameStr = Sec_UeGetIssuerStr(hSecHandle,pstCertNames[iIndex].sCharSet, 
                    (void *)pstCertNames[iIndex].pvFriendlyName, 
                    pstCertNames[iIndex].usFriendlyNameLen);
                if(NULL == pcNameStr)
                {
                    WE_INT32 iLoop=0;
                    WE_LOG_MSG(( 0,0,"SEC:WTLS X509 malloc error3 in Sec_UeSelectCert!\n"));
                    for(iLoop=0;iLoop<iIndex;iLoop++)
                    {
                        if(pstChooseCertByName->ppcCertName[iIndex])
                        {
                            WE_FREE(pstChooseCertByName->ppcCertName[iIndex]);
                        }
                    }                
                    WE_FREE(pstChooseCertByName->ppcCertName);
                    WE_FREE(pstChooseCertByName);
                    return FALSE;
                }
            }        
            else if (pstCertNames[iIndex].cNameType == M_SEC_WRITTEN_NAME)
            {
                /* The friendly name is written by the user */
                WE_LONG lTmpNameLen = pstCertNames[iIndex].usFriendlyNameLen* 2 +1 ;
                pcNameStr = (WE_CHAR *)WE_MALLOC((WE_UINT32)lTmpNameLen);
                if(NULL == pcNameStr)
                {
                    WE_INT32 iLoop=0;
                    WE_LOG_MSG((0,0, "SEC:WRITTEN NAME malloc error3 in Sec_UeSelectCert!\n"));
                    for(iLoop=0;iLoop<iIndex;iLoop++)
                    {
                        if(pstChooseCertByName->ppcCertName[iIndex])
                        {
                            WE_FREE(pstChooseCertByName->ppcCertName[iIndex]);
                        }
                    } 
                    WE_FREE(pstChooseCertByName->ppcCertName);
                    WE_FREE(pstChooseCertByName);
                    return FALSE;
                }
                (void)WE_MEMSET(pcNameStr,0,(WE_UINT32)lTmpNameLen);
                
                /* Convert string to UTF8 */
                if (Sec_UeCsToUtf8(hSecHandle,(WE_UCHAR*)pstCertNames[iIndex].pvFriendlyName, 
                    pstCertNames[iIndex].usFriendlyNameLen, pcNameStr, &lTmpNameLen, 
                    pstCertNames[iIndex].sCharSet) != M_SEC_ERR_OK)
                {
                    WE_INT32 iLoop=0;
                    WE_LOG_MSG((0,0, "SEC:cvttoutf8 error in Sec_UeSelectCert!\n"));
                    for(iLoop=0;iLoop<iIndex;iLoop++)
                    {
                        if(pstChooseCertByName->ppcCertName[iIndex])
                        {
                            WE_FREE(pstChooseCertByName->ppcCertName[iIndex]);
                        }
                    } 
                    WE_FREE(pcNameStr);
                    WE_FREE(pstChooseCertByName->ppcCertName);
                    WE_FREE(pstChooseCertByName);
                    return FALSE;
                }            
            }        
            /* Insert elements */        
            pstChooseCertByName->ppcCertName[iIndex] = pcNameStr;
        }
    }    
    /*send event to STK*/
    return Sec_UeSendEvent(hSecHandle, iTargetId,M_UE_SELECT_CET_EVENT, (void *)pstChooseCertByName);    
}
#endif
#ifdef M_SEC_CFG_USE_CERT
/* Create a store certificate form with the supplied certificate fields.
 * Returns a pointer to a struct sec_ti_t, or NULL on error. */
/*==================================================================================================
FUNCTION: 
    Sec_UeStoreCert
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    displays a dialog asking if the end user wants to store a certificate
ARGUMENTS PASSED:
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
    TRUE: success
    FALSE: fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_BOOL Sec_UeStoreCert(WE_HANDLE hSecHandle, WE_INT32 iTargetId,
				WE_INT16 sIssuerCharSet, 
                        	void *pvIssuer, WE_INT32 iIssuerLen, 
                        	WE_UINT32 uiValidNotBefore, WE_UINT32 uiValidNotAfter, 
                        	WE_INT16 sSubjectCharSet, void *pvSubject, 
                        	WE_INT32 iSubjectLen, WE_CHAR cCertType)
{
    St_StoreCert *pstStoreCert = NULL;
    WE_CHAR* pcText = NULL;
    WE_CHAR* pcTimeAft = NULL;
    WE_CHAR* pcTimeBef = NULL;
    
    if(NULL == (pstStoreCert = (St_StoreCert*)WE_MALLOC(sizeof(St_StoreCert))))
    {
        WE_LOG_MSG((0,0, "SEC:malloc error1 in Sec_UeStoreCert!\n"));
        return FALSE;
    }
    (void)WE_MEMSET(pstStoreCert,0,sizeof(St_StoreCert));
    
    pstStoreCert->cCertType = cCertType;
    
    if ( NULL == (pcText = Sec_UeGetIssuerStr(hSecHandle,sIssuerCharSet, 
                                                                    pvIssuer, iIssuerLen )))
    {
        WE_LOG_MSG((0,0, "SEC:malloc error2 in Sec_UeStoreCert!\n"));
        WE_FREE(pstStoreCert);
        return FALSE;
     }
    pstStoreCert->pcIssuerStr = pcText;
    
    if (NULL == (pcText = Sec_UeGetIssuerStr(hSecHandle,sSubjectCharSet, 
                                                                    pvSubject, iSubjectLen )))
    {
        WE_LOG_MSG((0,0, "SEC:malloc error3 in Sec_UeStoreCert!\n"));
        WE_FREE(pstStoreCert->pcIssuerStr);
        WE_FREE(pstStoreCert);

        return FALSE;
    }
    pstStoreCert->pcSubjectStr = pcText;
    
    /*NotBefore*/
    pcTimeBef = (WE_CHAR*)WE_MALLOC(30);
    if(NULL == pcTimeBef)
    {
        WE_LOG_MSG((0,0, "SEC:malloc error4 in Sec_UeStoreCert!\n"));
        WE_FREE(pstStoreCert->pcSubjectStr);
        WE_FREE(pstStoreCert->pcIssuerStr);
        WE_FREE(pstStoreCert);
        return FALSE;
     }
        /*
        WE_SCL_TIME2STR(iValidNotBefore,pcText);
    */
    pstStoreCert->iNotBefMonth = Sec_UeTimeToStr(uiValidNotBefore,&pcTimeBef);
    pstStoreCert->pcNotBefStr = pcTimeBef;
    
    /*Not After*/
    pcTimeAft = (WE_CHAR*)WE_MALLOC(30);
    if(NULL == pcTimeAft)
    {
        WE_LOG_MSG(( 0,0,"SEC:malloc error5 in Sec_UeStoreCert!\n"));
        WE_FREE(pstStoreCert->pcNotBefStr);
        WE_FREE(pstStoreCert->pcSubjectStr);
        WE_FREE(pstStoreCert->pcIssuerStr);
        WE_FREE(pstStoreCert);
        return FALSE;
    }
    /*
    WE_SCL_TIME2STR(iValidNotAfter,pcText);
    */
    pstStoreCert->iNotAftMonth = Sec_UeTimeToStr(uiValidNotAfter,&pcTimeAft);
    pstStoreCert->pcNotAftStr = pcTimeAft;
    
    /*send event to browser*/
    return Sec_UeSendEvent(hSecHandle,iTargetId,M_UE_STORE_CERT_EVENT, (void *)pstStoreCert);
}
#endif /*M_SEC_CFG_USE_CERT*/

/*==================================================================================================
FUNCTION: 
    Sec_UeNameConfirm
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    displays a dialog to the end user with a given message
ARGUMENTS PASSED:
    WE_INT16 sCharSet[IN]: character set for subject of WTLS certificates 
                                        and CertDisplayName according to the WPKI-specification [WAP-WPKI]. 
                                        For subject of X.509 certificates this value is -1.
    void *pvDisplayName[IN]:The name displayed to the user
    WE_INT32 iDisplayNameLen[IN]:The length of displayName
    WE_INT32 iMsg[IN]:An index to the message displayed to the end user.
    WE_INT32 iYesMsg[IN]:An index to the message displayed on the "yes" button
    WE_INT32 iNoMsg[IN]:An index to the message displayed on the "no" button
RETURN VALUE:
    TRUE: success
    FALSE: fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_BOOL Sec_UeNameConfirm(WE_HANDLE hSecHandle,WE_INT32 iTargetId,
				            WE_INT16 sCharSet, 
                            void *pvDisplayName, WE_INT32 iDisplayNameLen, 
                            WE_INT32 iMsg, WE_INT32 iYesMsg, WE_INT32 iNoMsg)
{
    St_NameConfirm* pstConfDispName = NULL;
    WE_CHAR* pcNameStr = NULL;
    WE_CHAR* pcTmpName = NULL;
    WE_LONG   lTmpNameLen = 2 * iDisplayNameLen + 3;

    if((NULL == pvDisplayName)&&(iDisplayNameLen != 0))
    {
        return FALSE;
    }
    if(NULL == (pstConfDispName = (St_NameConfirm*)WE_MALLOC(sizeof(St_NameConfirm))))
    {
        WE_LOG_MSG(( 0,0,"SEC:malloc error1 in Sec_UeNameConfirm!\n"));
        return FALSE;
    }
    (void)WE_MEMSET(pstConfDispName,0,sizeof(St_NameConfirm));
    if (-1 == sCharSet) /*X509 certificate*/
    {
        if (NULL == (pcNameStr = Sec_UeGetIssuerStr(hSecHandle,sCharSet, 
                                                    pvDisplayName, iDisplayNameLen)))
        {
            WE_LOG_MSG((0,0, "SEC:X509 malloc error2 in Sec_UeNameConfirm!\n"));
            WE_FREE(pstConfDispName);
            return FALSE;
        }
    }
    else
    {
        pcNameStr  = (WE_CHAR *)WE_MALLOC((WE_UINT32)iDisplayNameLen*2 + 3);
        if(NULL == pcNameStr)
        {
            WE_LOG_MSG((0,0, "SEC:WTLS malloc error2 in Sec_UeNameConfirm!\n"));
            WE_FREE(pstConfDispName);
            return FALSE;
        }
        (void)WE_MEMSET(pcNameStr,0,(WE_UINT32)iDisplayNameLen*2 + 3);
        pcTmpName = pcNameStr;
        
        /* Convert string to UTF8 */
        if (Sec_UeCsToUtf8(hSecHandle,(WE_UCHAR *)pvDisplayName, iDisplayNameLen, 
            pcTmpName, &lTmpNameLen, sCharSet) != M_SEC_ERR_OK)
        {
            WE_LOG_MSG((0,0, "SEC:cvttoutf8 error in Sec_UeNameConfirm!\n"));
            WE_FREE(pstConfDispName);
            WE_FREE(pcNameStr);
            return FALSE;
        }        
    }    
    pstConfDispName->pcCertName = pcNameStr;    
    /*send event to STK*/ 
    iMsg = iMsg;
    iYesMsg = iYesMsg;
    iNoMsg = iNoMsg;
    return Sec_UeSendEvent(hSecHandle, iTargetId,M_UE_NAME_CONFIRM_EVENT, (void *)pstConfDispName);
}

#ifdef M_SEC_CFG_USE_CERT
/*==================================================================================================
FUNCTION: 
    Sec_UeShowCertContent
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    displays a dialog to the end user with the given certificate information
ARGUMENTS PASSED:
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
    TRUE: success
    FALSE: fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_BOOL Sec_UeShowCertContent(WE_HANDLE hSecHandle,WE_INT32 iTargetId,
                                WE_INT16 sIssuerCharSet, 
                                void *pvIssuer, WE_INT32 iIssuerLen,
                                  WE_UINT32 iValidNotBefore, WE_UINT32 iValidNotAfter, 
                                  WE_INT16 sSubjectCharSet, void *pvSubject, 
                                  WE_INT32 iSubjectLen, WE_UCHAR *pucSerialNumber , 
                                  WE_UINT16 usSerialNumberLen,WE_UCHAR* pucAlg,
                                  WE_UCHAR* pucPubKey,WE_UINT16 usPubKeyLen,
                                  WE_INT32 iCancelStr)
{
    St_ShowCertContent *pstShowCert = NULL;
    WE_CHAR* pcTextIssuer = NULL;
    WE_CHAR* pcTextSub = NULL;
    WE_CHAR* pcTimeAft = NULL;
    WE_CHAR* pcTimeBef = NULL;
    WE_BOOL   bFlag = FALSE;
    if(NULL == (pstShowCert = (St_ShowCertContent*)WE_MALLOC(sizeof(St_ShowCertContent))))
    {
        WE_LOG_MSG((0,0, "SEC:malloc error1 in Sec_UeShowCertContent!\n"));
        return FALSE;
    }
    (void)WE_MEMSET(pstShowCert,0,sizeof(St_ShowCertContent));
    
    if ( NULL == (pcTextIssuer = Sec_UeGetIssuerStr(hSecHandle,sIssuerCharSet, 
                                                    pvIssuer, iIssuerLen)))
    {
        WE_LOG_MSG((0,0, "SEC:malloc error2 in Sec_UeShowCertContent!\n"));
        WE_FREE(pstShowCert);
        return FALSE;
    }
    pstShowCert->pcIssuerStr = pcTextIssuer;
    
    if (NULL == (pcTextSub = Sec_UeGetIssuerStr(hSecHandle,sSubjectCharSet, 
                                                pvSubject, iSubjectLen )))
    {
        WE_LOG_MSG((0,0, "SEC:malloc error3 in Sec_UeShowCertContent!\n"));
        WE_FREE(pstShowCert);
        WE_FREE(pcTextIssuer);
        return FALSE;
     }
    pstShowCert->pcSubjectStr = pcTextSub;
    
    /*NotBefore*/
    pcTimeBef = (WE_CHAR*)WE_MALLOC(30);
    if(NULL == pcTimeBef)
    {
        WE_LOG_MSG((0,0, "SEC:malloc error4 in Sec_UeShowCertContent!\n"));
        WE_FREE(pstShowCert);
        WE_FREE(pcTextSub);
        return FALSE;
    }
        /*
        WE_SCL_TIME2STR(iValidNotBefore,pcText);
    */
    pstShowCert->iNotBefMonth = Sec_UeTimeToStr(iValidNotBefore,&pcTimeBef);
    pstShowCert->pcNotBefStr = pcTimeBef;
    
    /*Not After*/
    pcTimeAft = (WE_CHAR*)WE_MALLOC(30);
    if(NULL == pcTimeAft)
    {
        WE_LOG_MSG(( 0,0,"SEC:malloc error5 in Sec_UeShowCertContent!\n"));
        WE_FREE(pstShowCert);
        WE_FREE(pcTextSub);
        WE_FREE(pcTimeBef);
        return FALSE;
    }
    /*
    WE_SCL_TIME2STR(iValidNotAfter,pcText);
    */
    pstShowCert->iNotAftMonth = Sec_UeTimeToStr(iValidNotAfter,&pcTimeAft);
    pstShowCert->pcNotAftStr = pcTimeAft;
    
    /*serial number*/
    pstShowCert->pucSerialNumber = (WE_UCHAR*)Sec_UeCreateFingerpr(pucSerialNumber,usSerialNumberLen);

    /*alg*/
    pstShowCert->pucAlg = (WE_UCHAR *)SEC_STRDUP((WE_CHAR *)pucAlg);

    /*pub key*/
    pstShowCert->pucPubKey = (WE_UCHAR*)Sec_UeCreateFingerpr(pucPubKey,usPubKeyLen);
        
    /*send event to browser*/
    if(M_SEC_ID_DELETE == iCancelStr )
    {
        bFlag =  Sec_UeSendEvent(hSecHandle,iTargetId, M_UE_SHOW_CERT_CONTENT_EVENT, (void *)pstShowCert);
    }
    else
    {
        bFlag =  Sec_UeSendEvent(hSecHandle,iTargetId, M_UE_CURSVRCERT, (void *)pstShowCert);
            
    }
    return bFlag;

}
#endif /*M_SEC_CFG_USE_CERT*/

#ifdef M_SEC_CFG_USE_KEYS
/*==================================================================================================
FUNCTION: 
    Sec_UeGenPin
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    let the end user enter a PIN code when the first keypair of each type is created. 
    When the end user has finished, SEC stores the new PIN. This PIN code will be associated with all keys 
    of the supported key type. 
ARGUMENTS PASSED:
    WE_UINT8 ucKeyType[IN]:The type of the key (M_SEC_PRIVATE_NONREPKEY or 
                                            M_SEC_PRIVATE_AUTHKEY ) 
                                            with which this PIN code will be associated
    WE_INT32 iReason:The reason to call this dialog again
RETURN VALUE:
    TRUE: success
    FALSE: fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_BOOL Sec_UeGenPin(WE_HANDLE hSecHandle, WE_INT32 iTargetId,WE_UINT8 ucKeyType, WE_INT32 iReason)
{
    St_CreatePin *pstGenPin = NULL;
    if((pstGenPin = (St_CreatePin*)WE_MALLOC(sizeof(St_CreatePin))) == NULL)
    {
        WE_LOG_MSG((0,0, "SEC:malloc error1 in Sec_UePin!\n"));
        return FALSE;
    }
    (void)WE_MEMSET(pstGenPin,0,sizeof(St_CreatePin));
    
    switch (iReason)
    {
        case 0:
            /* First time dialog is shown */
            pstGenPin->iInfoId = (M_SEC_ID_GEN_PIN);
            break;
        case M_SEC_UE_INPUT_TOO_LONG:
            /* Shown again because too long a PIN was entered */
            pstGenPin->iInfoId = (M_SEC_ID_PIN_TOO_LONG); 
            break;
        case M_SEC_UE_INPUT_TOO_SHORT:
            /* Shown again because too short a PIN was entered */
            pstGenPin->iInfoId = (M_SEC_ID_PIN_TOO_SHORT);
            break;
        default:
            pstGenPin->iInfoId = (M_SEC_ID_GEN_PIN);
            break;
    }
    
    switch (ucKeyType)
    {
        case M_SEC_PRIVATE_NONREPKEY:
            pstGenPin->iInfoIdAfter  = (M_SEC_ID_NONREP_KEY);
            break;
        case M_SEC_PRIVATE_AUTHKEY:
            pstGenPin->iInfoIdAfter  = (M_SEC_ID_AUTH_KEY);
            break;
        default:
            pstGenPin->iInfoIdAfter  = 0;
            break;
    }
    /*send event to browser*/
    return Sec_UeSendEvent(hSecHandle,iTargetId, M_UE_CREATE_PIN_EVENT, (void *)pstGenPin);
}

/*==================================================================================================
FUNCTION: 
    Sec_UeChgPin
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    change the stored PIN code. If the correct old PIN code is not entered, the stored PIN code will not be changed
ARGUMENTS PASSED:
    WE_UINT8 ucKeyType[IN]:The type of the key (M_SEC_PRIVATE_NONREPKEY or 
                                            M_SEC_PRIVATE_AUTHKEY ) 
                                            with which this PIN code will be associated.
    WE_INT32 iReason[IN]:The reason to call this dialog again.
RETURN VALUE:
    TRUE: success
    FALSE: fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_BOOL Sec_UeChgPin(WE_HANDLE hSecHandle,WE_INT32 iTargetId, WE_UINT8 ucKeyType, WE_INT32 iReason)
{
    St_ChangePin *pstChangePin = NULL;
    
    if(NULL == (pstChangePin = (St_ChangePin*)WE_MALLOC(sizeof(St_ChangePin))))
    {
        WE_LOG_MSG((0,0, "SEC:malloc error1 in Sec_UeChangePin!\n"));
        return FALSE;
    }
    (void)WE_MEMSET(pstChangePin,0,sizeof(St_ChangePin));
    
    switch (iReason)
    {
        case 0:
            /* First time dialog is shown */
            pstChangePin->iNewPinId = M_SEC_ID_CHANGE_PIN_NEW;
            break;
        case M_SEC_UE_INPUT_MISIATCH:
            /* Shown again because first and second new PIN not the same */
            pstChangePin->iNewPinId = M_SEC_ID_PIN_MISIATCH;
            break;
        case M_SEC_UE_INPUT_TOO_LONG:
            /* Shown again because too long a PIN was entered */
            pstChangePin->iNewPinId = M_SEC_ID_PIN_TOO_LONG; 
            break;
        case M_SEC_UE_INPUT_TOO_SHORT:
            /* Shown again because too short a PIN was entered */
            pstChangePin->iNewPinId = M_SEC_ID_PIN_TOO_SHORT;
            break;
        case M_SEC_UE_INPUT_FALSE:
            /* Shown again because the wrong PIN was entered */
            pstChangePin->iNewPinId = M_SEC_ID_WRONG_OLD_PIN;
            break;
        default :
            pstChangePin->iNewPinId = M_SEC_ID_CHANGE_PIN_NEW;
            break;
    }
    
    switch (ucKeyType)
    {
        case M_SEC_PRIVATE_NONREPKEY:
            pstChangePin->iNewPinIdAfter= M_SEC_ID_NONREP_KEY;
            break;
        case M_SEC_PRIVATE_AUTHKEY:
            pstChangePin->iNewPinIdAfter = M_SEC_ID_AUTH_KEY;
            break;
        default:
            pstChangePin->iNewPinIdAfter = 0;
            break;
    }
    /*send event to browser*/
    return Sec_UeSendEvent(hSecHandle,iTargetId, M_UE_CHANGE_PIN_EVENT, (void *)pstChangePin);
}

/*==================================================================================================
FUNCTION: 
    Sec_UePin
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    called whenever the end user identity needs to be verified by entering a PIN code.
 ARGUMENTS PASSED:
    E_SecPinReason ePinCause[IN]::Parameter indicating the reason for asking for the PIN code.
    WE_INT32 iReason[IN]:The reason to call this dialog again.
RETURN VALUE:
    TRUE: success
    FALSE: fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_BOOL Sec_UePin(WE_HANDLE hSecHandle, WE_INT32 iTargetId,E_SecPinReason ePinCause, WE_INT32 iReason)
{    
    St_Pin *pstPin = NULL;
    if((pstPin = (St_Pin*)WE_MALLOC(sizeof(St_Pin))) == NULL)
    {
        WE_LOG_MSG((0,0, "SEC:malloc error1 in Sec_UePin!\n"));
        return FALSE;
    }
    (void)WE_MEMSET(pstPin,0,sizeof(St_Pin));
    
    switch (iReason)
    {
        case 0:
            /* First time dialog is shown */
            pstPin->iInfoId = (M_SEC_ID_PIN);
            break;
        case M_SEC_UE_INPUT_TOO_LONG:
        case M_SEC_UE_INPUT_TOO_SHORT:
        case M_SEC_UE_INPUT_FALSE:
            /* Shown again because the wrong PIN was entered */
            pstPin->iInfoId = (M_SEC_ID_WRONG_PIN);
            break;
        default:
            pstPin->iInfoId = (M_SEC_ID_PIN);
            break;
    }
    
    switch (ePinCause)
    {
        case E_SEC_OPEN_WIM:
            pstPin->iInfoIdAfer = M_SEC_ID_PIN_OPEN_WIM;
            break;
        case E_SEC_SIGN_TEXT:
            pstPin->iInfoIdAfer = M_SEC_ID_PIN_SIGN_TEXT;
            break;
        case E_SEC_STORE_CERT:
            pstPin->iInfoIdAfer = M_SEC_ID_PIN_STORE_CERT;
            break;
        case E_SEC_DELETE_CERT:
            pstPin->iInfoIdAfer = M_SEC_ID_PIN_DEL_CERT;
            break;
        case E_SEC_CREATE_AUTH_KEY:
            pstPin->iInfoIdAfer = M_SEC_ID_PIN_GEN_AUT_KEY;
            break;
        case E_SEC_CREATE_NONREP_KEY:
            pstPin->iInfoIdAfer = M_SEC_ID_PIN_GEN_NR_KEY;
            break;
        default:
            WE_FREE(pstPin);
            return FALSE;
    }        
    /*send event to STK*/  
    return Sec_UeSendEvent(hSecHandle, iTargetId,M_UE_PIN_EVENT, (void *)pstPin);
}
#endif


#ifdef M_SEC_CFG_USE_CERT
/*==================================================================================================
FUNCTION: 
    Sec_UeHash
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    called when the end user needs to enter a hash received out of band to download trusted CA information. 
ARGUMENTS PASSED:
    WE_INT16 sCharSet[IN]:character set
    void *pvDisplayName[IN]:The name that must be displayed to the end user.
    WE_INT32 iDisplayNameLen[IN]:The length of displayName.
    WE_INT32 iReason[IN]:The reason to call this dialog again.
RETURN VALUE:
    TRUE: success
    FALSE: fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_BOOL Sec_UeHash(WE_HANDLE hSecHandle,WE_INT32 iTargetId, WE_INT16 sCharSet, 
                   void *pvDisplayName, WE_INT32 iDisplayNameLen, WE_INT32 iReason)
{
    St_Hash  *pstHash = NULL;
    WE_CHAR  *pcTextBuf = NULL;
    WE_LONG  lTmpNameLen = 2* iDisplayNameLen + 3;
    WE_INT32 iRes = 0;
    
    if((pstHash = (St_Hash*)WE_MALLOC(sizeof(St_Hash))) == NULL)
    {
        WE_LOG_MSG(( 0,0,"SEC:malloc error1 in Sec_UeHash!\n"));
        return FALSE;
    }
    (void)WE_MEMSET(pstHash,0,sizeof(St_Hash));
    
    switch (iReason)
    {
        case 0:
            /* First time dialog is shown */
            pstHash->iInfoId = (M_SEC_ID_ENTER_HASH);
            break;
        case M_SEC_UE_INPUT_TOO_SHORT:
        case M_SEC_UE_INPUT_FALSE:
            /* Show again because the wrong hash was entered */
            pstHash->iInfoId = (M_SEC_ID_WRONG_HASH);
            break;
        default :
            pstHash->iInfoId = (M_SEC_ID_ENTER_HASH);
            break;
    }
    pcTextBuf = (WE_CHAR *)WE_MALLOC(2*(WE_UINT32)iDisplayNameLen+3);
    if(NULL == pcTextBuf)
    {
        WE_LOG_MSG((0,0, "SEC:malloc error2 in Sec_UeHash!\n"));
        WE_FREE(pstHash);
        return FALSE;
    }
    (void)WE_MEMSET(pcTextBuf,0,(WE_UINT32)(iDisplayNameLen+3));
    iRes = Sec_UeCsToUtf8(hSecHandle,(WE_UCHAR *)pvDisplayName, iDisplayNameLen, 
                   pcTextBuf, &lTmpNameLen, sCharSet);
    if(iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pcTextBuf);
        WE_FREE(pstHash);
        return FALSE;
    }
    
    pstHash->pcDisplayName = pcTextBuf;    
    /*send event to browser*/
    return Sec_UeSendEvent(hSecHandle,iTargetId, M_UE_HASH_EVENT, (void *)pstHash);
}
#endif

/*==================================================================================================
FUNCTION: 
    Sec_UeShowContractsList
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    shows the supplied contracts.
ARGUMENTS PASSED:
    WE_INT32 iNbrContracts[IN]:The number of supplied contracts.
    St_SecContractInfo *pstContractVec[IN]:An array of the stored contracts
RETURN VALUE:
    TRUE: success
    FALSE: fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_BOOL Sec_UeShowContractsList(WE_HANDLE hSecHandle, WE_INT32 iTargetId,WE_INT32 iNbrContracts, 
                                St_SecContractInfo *pstContractVec)
{
    St_ShowContractsList *pstStoredContracts = NULL;
    WE_INT32 iIndex = 0;
    WE_CHAR* pText = NULL;

    if((NULL == hSecHandle)||((NULL == pstContractVec)&&(iNbrContracts != 0)))
    {
        return FALSE;
    }
    
    if(NULL == (pstStoredContracts = (St_ShowContractsList*)WE_MALLOC(sizeof(St_ShowContractsList))))
    {
        WE_LOG_MSG((0,0, "SEC:malloc error1 in Sec_UeShowContractsList!\n"));
        return FALSE;
    }
    (void)WE_MEMSET(pstStoredContracts,0,sizeof(St_ShowContractsList));
    
    pstStoredContracts->nbrOfContract = iNbrContracts;
    pstStoredContracts->ppcTime = (WE_CHAR**)WE_MALLOC((WE_UINT32)iNbrContracts * sizeof(WE_CHAR *));
    if((NULL == pstStoredContracts->ppcTime)&&(iNbrContracts != 0))
    {
        WE_LOG_MSG((0,0, "SEC:malloc error2 in Sec_UeShowContractsList!\n"));
        WE_FREE(pstStoredContracts);
        return FALSE;
    }
    pstStoredContracts->piMonthID = (WE_INT32*)WE_MALLOC((WE_UINT32)iNbrContracts * sizeof(WE_INT32));
    if((NULL == pstStoredContracts->piMonthID)&&(iNbrContracts != 0))
    {
        WE_LOG_MSG((0,0, "SEC:malloc error3 in Sec_UeShowContractsList!\n"));
        WE_FREE(pstStoredContracts->ppcTime);
        WE_FREE(pstStoredContracts);
        return FALSE;
    }
    
    for (iIndex=0; iIndex<iNbrContracts; iIndex++)
    {
        if (pstContractVec != NULL)
        {
            pText  = (WE_CHAR*)WE_MALLOC(30);
            if(NULL == pText)
            {
                WE_INT32 iLoop=0;
                WE_LOG_MSG((0,0, "SEC:malloc error4 in Sec_UeShowContractsList!\n"));
                for(iLoop=0;iLoop<iIndex;iLoop++)
                {
                    if(pstStoredContracts->ppcTime[iLoop])
                    {
                        WE_FREE(pstStoredContracts->ppcTime[iLoop]);
                    }                
                }
                WE_FREE(pstStoredContracts->ppcTime);
                WE_FREE(pstStoredContracts->piMonthID);
                WE_FREE(pstStoredContracts);
                return FALSE;
            }
            pstStoredContracts->piMonthID[iIndex] =  Sec_UeTimeToStr(pstContractVec[iIndex].uiTime,&pText);
            pstStoredContracts->ppcTime[iIndex] = pText;
        }
    }
    
    /*send event to browser*/
    return Sec_UeSendEvent(hSecHandle,iTargetId, M_UE_SHOW_CONTRACT_LIST_EVENT, (void *)pstStoredContracts);
}

/*==================================================================================================
FUNCTION: 
    Sec_UeShowContractContent
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
   show contract content
ARGUMENTS PASSED:
    St_SecContractInfo stChosenContract[IN]: contract info 

RETURN VALUE:
    TRUE: success
    FALSE: fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_BOOL Sec_UeShowContractContent(WE_HANDLE hSecHandle, WE_INT32 iTargetId,
									St_SecContractInfo stChosenContract)
{
    St_ShowContractContent *pstShowContract = NULL;
    WE_CHAR* pText = NULL;
    WE_CHAR* pTime = NULL;
    if((pstShowContract = (St_ShowContractContent*)WE_MALLOC(sizeof(St_ShowContractContent))) == NULL)
    {
        WE_LOG_MSG((0,0, "SEC:malloc error1 in Sec_UeShowContractContent!\n"));
        return FALSE;
    }
    (void)WE_MEMSET(pstShowContract,0,sizeof(St_ShowContractContent));
    
    /*ASCII*/
    /*contract*/
    pText = (WE_CHAR*)WE_MALLOC(SEC_STRLEN( stChosenContract.pcContract) +1);
    if(NULL == pText)
    {
        WE_LOG_MSG((0,0, "SEC:malloc error2 in Sec_UeShowContractContent!\n"));
        WE_FREE(pstShowContract);
        return FALSE;
     }
    (void)WE_MEMSET(pText,0,SEC_STRLEN( stChosenContract.pcContract) +1);
    (void)WE_MEMCPY(pText,stChosenContract.pcContract,SEC_STRLEN( stChosenContract.pcContract));
    pstShowContract->pcContractStr = pText;
    
    /*Time*/
    pTime = (WE_CHAR*)WE_MALLOC(20);
    if(NULL == pTime)
    {
        WE_LOG_MSG((0,0, "SEC:malloc error3 in Sec_UeShowContractContent!\n"));
        WE_FREE(pstShowContract->pcContractStr);
        WE_FREE(pstShowContract);
        return FALSE;
    }
    (void)WE_MEMSET(pTime,0,20);
    
    pstShowContract->iMonthID = Sec_UeTimeToStr(stChosenContract.uiTime,&pTime); 
    pstShowContract->pcTimeStr = pTime;
    
    /*signature*/
    
    pText = (WE_CHAR*)WE_MALLOC(stChosenContract.usSignatureLen +1);
    if(NULL == pText)
    {
        WE_LOG_MSG((0,0, "SEC:malloc error4 in Sec_UeShowContractContent!\n"));
        WE_FREE(pstShowContract->pcContractStr);
        WE_FREE(pstShowContract->pcTimeStr);
        WE_FREE(pstShowContract);

        return FALSE;
    }
    (void)WE_MEMSET(pText,0,stChosenContract.usSignatureLen +1);
    (void)WE_MEMCPY(pText,stChosenContract.pcSignature,stChosenContract.usSignatureLen);
    pstShowContract->pcSignatureStr = pText;
    
    /*send event to browser*/
    return Sec_UeSendEvent(hSecHandle,iTargetId, M_UE_STORE_CONTRACT_EVENT, (void *)pstShowContract);
    
}


/*==================================================================================================
FUNCTION: 
    Sec_UeTimeToStr
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    Converts the supported time to a string  format . 
ARGUMENTS PASSED:
    WE_UINT32 uiTime[IN]:the time wanted to convert.
    WE_CHAR **ppcTimeStr[OUT]:string format as "day year".
RETURN VALUE:
    month index 
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 Sec_UeTimeToStr(WE_UINT32 uiTime, WE_CHAR **ppcTimeStr)
{
    WE_UINT32 uiTmpTime = uiTime;
    WE_UINT32 uiDays = 0;
    WE_UINT32 uiDaysIn31month = 31;
    WE_UINT32 uiDaysIn30month = 30;
    WE_UINT32 uiDaysIn29month = 29;
    WE_UINT32 uiDaysIn28month = 28;
    WE_INT32  iNbrYears = 0; 
    WE_INT32  iLeapYear = 0;
    WE_UINT32 uiYear = 0;
    WE_INT32  iTmpStrID = 0;

    if(NULL == ppcTimeStr)
    {
        return 0;
    }
    /* Get minutes since 1/1 1970 */
    uiTmpTime /= 60;  
    /* Get hours since 1/1 1970 */
    uiTmpTime /= 60;
    /* Get days since 1/1 1970 */
    uiDays = uiTmpTime / 24;
    /* Get years since 1/1 1970 */
    uiTmpTime = uiDays / 365;
    uiYear = 1970 + uiTmpTime;
    
    /* 1972 is the first leap year after 1970 */
    iLeapYear = 1;
    /* For each year since 1970 subtract the days in each month */
    while ((uiDays > 0) && (iNbrYears >= 0))
    {
        iLeapYear++;
        if (uiDays <= 30)   /* January */
        {
            iTmpStrID = (M_SEC_ID_JAN);
            break;
        }
        uiDays -= uiDaysIn31month;    
        if (iLeapYear == 4)
        {
            if (uiDays <= 28) /* February */
            {
                iTmpStrID = (M_SEC_ID_FEB);
                break;
            }
            uiDays -= uiDaysIn29month;  
        }
        else
        {
            if (uiDays <= 27) /* February */
            {
                iTmpStrID = (M_SEC_ID_FEB);
                break;
            }
            uiDays -= uiDaysIn28month;  
        }
        if (uiDays <= 30)   /* March */
        {
            iTmpStrID = (M_SEC_ID_MARCH);
            break;
        }
        uiDays -= uiDaysIn31month;    
        if (uiDays <= 29)   /* April */
        {
            iTmpStrID = (M_SEC_ID_APR);
            break;
        }
        uiDays -= uiDaysIn30month;    
        if (uiDays <= 30)   /* May */
        {
            iTmpStrID = (M_SEC_ID_MAY);
            break;
        }
        uiDays -= uiDaysIn31month;    
        if (uiDays <= 29)   /* June */
        {
            iTmpStrID= (M_SEC_ID_JUNE);
            break;
        }
        uiDays -= uiDaysIn30month;    
        if (uiDays <= 30)   /* July */
        {
            iTmpStrID = (M_SEC_ID_JULY);
            break;
        }
        uiDays -= uiDaysIn31month;    
        if (uiDays <= 30)   /* August */
        {
            iTmpStrID = (M_SEC_ID_AUG);
            break;
        }
        uiDays -= uiDaysIn31month;    
        if (uiDays <= 29)   /* September */
        {
            iTmpStrID = (M_SEC_ID_SEP);
            break;
        }
        uiDays -= uiDaysIn30month;    
        if (uiDays <= 30)   /* October */
        {
            iTmpStrID = (M_SEC_ID_OCT);
            break;
        }
        uiDays -= uiDaysIn31month;   
        if (uiDays <= 29)   /* November */
        {
            iTmpStrID = (M_SEC_ID_NOV);
            break;
        }
        uiDays -= uiDaysIn30month;    
        if (uiDays <= 30)   /* December */
        {
            iTmpStrID = (M_SEC_ID_DEC);
            break;
        }
        uiDays -= uiDaysIn31month;    
        iNbrYears++;
        if (iLeapYear == 4)
        {
            iLeapYear = 0;
        }
    } /* while */
    if ((uiDays == 0) && (iTmpStrID == 0))
    {
        iTmpStrID = (M_SEC_ID_JAN);    
    }
    uiDays++;   /* To get the correct date, since 0 is no a date */
    (void)SEC_SPRINTF(*ppcTimeStr, "%u %u", uiDays, uiYear);
    
    return iTmpStrID;
}

/* Convert the supplied strToCvt to UTF-8 and store in the
 * supplied dst. Returns 0 on success and -1 otherwise. */
/*==================================================================================================
FUNCTION: 
    Sec_UeCsToUtf8
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    convert string to UTF8 fromat
ARGUMENTS PASSED:
    WE_UCHAR *pucStrToCvt[IN]:source string.
    WE_LONG lStrToCvtLen[IN]:source string length
    WE_CHAR *pcDst[OUT]:dst buffer
    WE_LONG *plDstLen[OUT]:dst length
    WE_INT16 sCharSet[IN]:charset
RETURN VALUE:
    0: sucess
    -1:fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 Sec_UeCsToUtf8(WE_HANDLE hSecHandle,WE_UCHAR *pucStrToCvt, WE_LONG lStrToCvtLen, 
                               WE_CHAR *pcDst, WE_LONG *plDstLen, WE_INT16 sCharSet)
{
    WE_INT32 iResult = M_SEC_ERR_OK;
    if((NULL == hSecHandle) || (NULL == pcDst) || (NULL == plDstLen))
    {
        return -1;
    }
    
    /* I WTLS: IANA defined character sets */
    /*WE_CSC_UTF_8*/
    switch (sCharSet)
    {
        case WE_CSC_UTF_8:           /* WTLS charSet */ 
        case WE_ASN1_UTF8STRING:        /* X.509 charSet */ 
            (void)WE_MEMCPY(pcDst, pucStrToCvt, (WE_UINT32)lStrToCvtLen);
            *plDstLen = lStrToCvtLen;
            break;        
        case WE_CSC_LATIN_1:         /* WTLS charSet */
        case WE_ASN1_T61STRING:         /* X.509 charSet, TeletexString */
        case WE_ASN1_PRINTABLESTRING:  /* X.509 charSet */
        case WE_ASN1_IA5STRING:   /* X.509 charSet */
            {
                if(!Sec_Csc(SEC_CSC_SHELL,WE_CSC_LATIN_1,WE_CSC_UTF_8,(WE_CHAR*)pucStrToCvt,\
                            lStrToCvtLen,pcDst,plDstLen))
                {
                    WE_LOG_MSG((0,0, "SEC:Sec_UeCsToUtf8 error1 !\n"));
                    return -1;
                }
            }        
            break;
            
        case WE_ASN1_UNIVERSALSTRING:   /* X.509 charSets */
        case WE_ASN1_BMPSTRING:         /* X.509 charSets */
            {
                
                if(!Sec_Csc(SEC_CSC_SHELL,WE_CSC_UCS_2,WE_CSC_UTF_8,(WE_CHAR*)pucStrToCvt,\
                            lStrToCvtLen,pcDst,plDstLen))
                {
                    WE_LOG_MSG((0,0, "SEC:Sec_UeCsToUtf8 error2 !\n"));
                    return -1;
                }            
            }        
            break;        
        default:
            return -1;
    }
    pcDst[*plDstLen] = '\0';
    return iResult;
}

/*==================================================================================================
FUNCTION: 
    Sec_UeSendEvent
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    registry callback , notify other module to create dialog
ARGUMENTS PASSED:
    WE_INT32 eSecEvt[IN]:dialog event.
    void *pvData[IN]:data of dialog
RETURN VALUE:
    TRUE:sucess
    FALSE:fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_BOOL Sec_UeSendEvent(WE_HANDLE hSecHandle, WE_INT32 iTargetId,WE_INT32 eSecEvt,void *pvData)
{ 
    WE_INT32     iRes = 0;
    St_ShowDlg* pstShowDlg = NULL;

    if((NULL == hSecHandle) || (NULL == pvData))
    {
        Sec_UeFreeMem(eSecEvt, pvData);
        return FALSE;
    }

    /*modified by Bird 070123*/
    if((E_SEC_BRS == (E_IFType)SEC_IFTYPE ) || (E_SEC_SIGNTEXT == (E_IFType)SEC_IFTYPE)) /*brs & signtext*/
    {
        /*fill the struct*/
        M_SEC_UECALLBACKEVENT = eSecEvt;
        M_SEC_UECALLBACKDATA = (WE_VOID*)pvData;
        SEC_REGCALLBACK(SEC_CSC_SHELL,M_SEC_UECALLBACK,Sec_RunUeCallBackFun,hSecHandle);
    }
    else /*wtls*/
    {
#ifdef M_SEC_WTLS_NO_CONFIRM_UE  /*wtls& signtext no confirm*/
        if(M_UE_CONFIRM_EVENT ==eSecEvt )
        {
            St_ConfirmAction stConfirm = {0};
            stConfirm.bIsOk = TRUE;
            iRes = Sec_UeConfirmAction(hSecHandle,iTargetId,stConfirm);
            Sec_UeFreeMem(eSecEvt, pvData);
            if( 0 != iRes)
            {
                return FALSE;
            }
            else
            {
                return TRUE;
            }

        }
        else
        {
            /*1.encode*/
            pstShowDlg = (St_ShowDlg*)WE_MALLOC(sizeof(St_ShowDlg));
            if(NULL == pstShowDlg)
            {
                Sec_UeFreeMem(eSecEvt, pvData);
                return FALSE;
            }
            pstShowDlg->iTargetId = iTargetId;
            iRes = Sec_EncodeEvt(eSecEvt,pvData,&(pstShowDlg->pvBuf),&(pstShowDlg->uiLength));
            if(FALSE == iRes)
            {
                M_SEC_SELF_FREE(pstShowDlg);
                Sec_UeFreeMem(eSecEvt, pvData);
                return FALSE;
            }
            /*2.send event*/
            /*fill the struct*/
            M_SEC_UECALLBACKEVENT = G_Evt_SHOW_DLG;
            M_SEC_UECALLBACKDATA = (WE_VOID*)pstShowDlg;
            SEC_REGCALLBACK(SEC_CSC_SHELL,M_SEC_UECALLBACK,Sec_RunUeCallBackFun,hSecHandle);
            Sec_UeFreeMem(eSecEvt, pvData);
            
        }
#else
            /*1.encode*/
            pstShowDlg = (St_ShowDlg*)WE_MALLOC(sizeof(St_ShowDlg));
            if(NULL == pstShowDlg)
            {
                M_SEC_SELF_FREE(pstCBData);
                Sec_UeFreeMem(eSecEvt, pvData);
                return FALSE;
            }
            pstShowDlg->iTargetId = iTargetId;
            iRes = Sec_EncodeEvt(eSecEvt,pvData,&(pstShowDlg->pvBuf),&(pstShowDlg->uiLength));
            if(FALSE == iRes)
            {
                M_SEC_SELF_FREE(pstShowDlg);
                M_SEC_SELF_FREE(pstCBData);
                Sec_UeFreeMem(eSecEvt, pvData);
                return FALSE;
            }
            /*2.send event*/
            /*fill the struct*/
            M_SEC_UECALLBACKEVENT = G_Evt_SHOW_DLG;
            M_SEC_UECALLBACKDATA = (WE_VOID*)pstShowDlg;
            SEC_REGCALLBACK(SEC_CSC_SHELL,M_SEC_UECALLBACK,Sec_RunUeCallBackFun,hSecHandle);
            Sec_UeFreeMem(eSecEvt, pvData);

#endif
        
    }   
    return TRUE;
}

/*==================================================================================================
FUNCTION: 
    Sec_UeGetIssuerStr
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    according to charset get issuer string.    
ARGUMENTS PASSED:
    WE_INT16 sCharSet[IN]:certificate charset.
    void *pvIssuer[IN]:issuer of certificate
    WE_INT32 iIssuerLen[IN]:issuer length
RETURN VALUE:
    issuer string
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_CHAR* Sec_UeGetIssuerStr(WE_HANDLE hSecHandle,WE_INT16 sCharSet, 
                                   void *pvIssuer, WE_INT32 iIssuerLen)
{
    WE_UINT8 ucNbrIssuerAtts = 0;
    St_SecNameAttribute *pstIssuerAtt = NULL;
    WE_INT32 iIndex = 0; 
    WE_INT32 iAttLen = 0; 
    WE_INT32 iAttStrPos = 0;
    WE_CHAR *pcAttStr = NULL; 
    WE_CHAR *pcTmpStr = NULL;
    WE_LONG lTmpStrLen = 0;
    WE_INT32 iResult = 0;
    WE_UINT32 uiTmpType = 0;
    
    /* Use parser to get issuer attributes. */
    if (-1 == sCharSet)  /* X.509 certificate */
    {
        iResult = Sec_X509GetNbrIssuerStr(hSecHandle,pvIssuer, (WE_UINT16)iIssuerLen, &ucNbrIssuerAtts);
        pstIssuerAtt = (St_SecNameAttribute *)WE_MALLOC(ucNbrIssuerAtts*sizeof(St_SecNameAttribute));
        if(!pstIssuerAtt)
        {
            return NULL;
        }
        (void)WE_MEMSET(pstIssuerAtt,0,ucNbrIssuerAtts*sizeof(St_SecNameAttribute));
        iResult = Sec_X509GetIssuerInfo(hSecHandle, (const WE_UCHAR *)pvIssuer, (WE_UINT16)iIssuerLen, \
                             &ucNbrIssuerAtts, pstIssuerAtt);
    }
    else /* WTLS certificate */
    {
        iResult = Sec_WtlsGetIssuerInfo(hSecHandle, pvIssuer, (WE_UINT16)iIssuerLen, &ucNbrIssuerAtts, NULL);
        pstIssuerAtt = (St_SecNameAttribute *)WE_MALLOC(ucNbrIssuerAtts*sizeof(St_SecNameAttribute));
        if(!pstIssuerAtt)
        {
            return NULL;
        }
        (void)WE_MEMSET(pstIssuerAtt,0,ucNbrIssuerAtts*sizeof(St_SecNameAttribute));
        iResult = Sec_WtlsGetIssuerInfo(hSecHandle, (const WE_UCHAR *)pvIssuer, (WE_UINT16)iIssuerLen,\
                             &ucNbrIssuerAtts, pstIssuerAtt);
        if (iResult != M_SEC_ERR_OK)
        {
            WE_FREE(pstIssuerAtt);
            return NULL;
        }
        for (iIndex=0; iIndex<ucNbrIssuerAtts; iIndex++)
        {
            pstIssuerAtt[iIndex].sAttributeCharSet = sCharSet;
        }
    }
    /* Get length of string to show */
    iAttLen = 0;
    for (iIndex=0; iIndex<ucNbrIssuerAtts; iIndex++)
    {
        iAttLen += pstIssuerAtt[iIndex].usAttributeValueLen * 2;
    }
    
    /*malloc memory*/  
    pcAttStr  = (WE_CHAR *)WE_MALLOC((WE_UINT32)iAttLen*2 +  (WE_UINT32)ucNbrIssuerAtts + 5);
    if (!pcAttStr )
    {
        for (iIndex = 0; iIndex < ucNbrIssuerAtts; iIndex++)
        {
            WE_FREE(pstIssuerAtt[iIndex].pucAttributeValue);
        }
        WE_FREE(pstIssuerAtt);
        return NULL;
    }
    
    /*GetData*/ 
#ifdef M_SEC_CFG_SHOW_COUNTRY_NAME
    uiTmpType |= M_SEC_CFG_SHOW_COUNTRY_NAME;
#endif
#ifdef M_SEC_CFG_SHOW_ORGANISATION_NAME
    uiTmpType |= M_SEC_CFG_SHOW_ORGANISATION_NAME;
#endif
#ifdef M_SEC_CFG_SHOW_ORGANISATIONAL_UNIT_NAME
    uiTmpType |= M_SEC_CFG_SHOW_ORGANISATIONAL_UNIT_NAME;
#endif
#ifdef M_SEC_CFG_SHOW_STATE_OR_PROVINCE_NAME
    uiTmpType |= M_SEC_CFG_SHOW_STATE_OR_PROVINCE_NAME;
#endif
#ifdef M_SEC_CFG_SHOW_COMMON_NAME
    uiTmpType |= M_SEC_CFG_SHOW_COMMON_NAME;
#endif
#ifdef M_SEC_CFG_SHOW_DOMAIN_COMPONENT
    uiTmpType |= M_SEC_CFG_SHOW_DOMAIN_COMPONENT;
#endif
#ifdef M_SEC_CFG_SHOW_SERIAL_NUMBER
    uiTmpType |= M_SEC_CFG_SHOW_SERIAL_NUMBER;
#endif
#ifdef M_SEC_CFG_SHOW_DN_QUALIFIER
    uiTmpType |= M_SEC_CFG_SHOW_DN_QUALIFIER;
#endif
#ifdef M_SEC_CFG_SHOW_LOCALITY_NAME
    uiTmpType |= M_SEC_CFG_SHOW_LOCALITY_NAME;
#endif
#ifdef M_SEC_CFG_SHOW_TITLE
    uiTmpType |= M_SEC_CFG_SHOW_TITLE;
#endif
#ifdef M_SEC_CFG_SHOW_SURNAME
    uiTmpType |= M_SEC_CFG_SHOW_SURNAME;
#endif
#ifdef M_SEC_CFG_SHOW_GIVENNAME
    uiTmpType |= M_SEC_CFG_SHOW_GIVENNAME;
#endif
#ifdef M_SEC_CFG_SHOW_INITIALS
    uiTmpType |= M_SEC_CFG_SHOW_INITIALS;
#endif
#ifdef M_SEC_CFG_SHOW_GENERATION_QUALIFIER
    uiTmpType |= M_SEC_CFG_SHOW_GENERATION_QUALIFIER;
#endif
#ifdef M_SEC_CFG_SHOW_NAME
    uiTmpType |= M_SEC_CFG_SHOW_NAME;
#endif
#ifdef M_SEC_CFG_SHOW_EMAIL_ADDRESS
    uiTmpType |= M_SEC_CFG_SHOW_EMAIL_ADDRESS;
#endif
#ifdef M_SEC_CFG_SHOW_UNKNOWN_ATTRIBUTE_TYPE
    uiTmpType |= M_SEC_CFG_SHOW_UNKNOWN_ATTRIBUTE_TYPE;
#endif
    pcTmpStr = pcAttStr ;
    iAttStrPos = 0;
    for (iIndex = 0; iIndex < ucNbrIssuerAtts ; iIndex++)
    {
        lTmpStrLen = (pstIssuerAtt[iIndex].usAttributeValueLen * 2) + 2;
        
        if (((pstIssuerAtt[iIndex].uiAttributeType) & uiTmpType) != 0)
        {
            /*TO UTF8*/
            iResult = Sec_UeCsToUtf8(hSecHandle,pstIssuerAtt[iIndex].pucAttributeValue, 
                                    pstIssuerAtt[iIndex].usAttributeValueLen, 
                                    pcTmpStr, 
                                    &lTmpStrLen, 
                                    pstIssuerAtt[iIndex].sAttributeCharSet);
            if (iResult != M_SEC_ERR_OK)
            {
                for (iIndex = 0; iIndex < ucNbrIssuerAtts; iIndex++)
                    WE_FREE(pstIssuerAtt[iIndex].pucAttributeValue);
                WE_FREE(pstIssuerAtt);
//                WE_FREE(pcTmpStr);
                return NULL;
            }            
            iAttStrPos += lTmpStrLen;
            pcTmpStr += (lTmpStrLen + 1);
            pcAttStr [iAttStrPos++] = ' ';
        }
    } /* for */
    iAttStrPos--;
    pcAttStr [iAttStrPos] = '\0';
    /*free Memory*/
    for (iIndex = 0; iIndex < ucNbrIssuerAtts; iIndex++)
    {
        WE_FREE(pstIssuerAtt[iIndex].pucAttributeValue);
    }
    WE_FREE(pstIssuerAtt);
    
    return pcAttStr ;
}

/*==================================================================================================
FUNCTION: 
    Sec_UeCreateFingerpr
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
   convert fingerprint to hexadecimal
ARGUMENTS PASSED:
    WE_UCHAR *pucFingerPrint[IN]:fingerprint.
    WE_INT32 iFingerPrintLen[IN]:length of fingerprint
RETURN VALUE:
    hexadecimal fingerprint
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_CHAR* Sec_UeCreateFingerpr(WE_UCHAR *pucFingerPrint,WE_INT32 iFingerPrintLen)
{
    WE_CHAR *pcTmpStr = NULL;
    WE_INT32 iIndex = 0;
    WE_UCHAR *pucFP = pucFingerPrint;
    WE_CHAR *pcBuf = NULL;
    
    if((NULL == pucFingerPrint) || (NULL == pucFP))
    {
        return NULL;
    }
    /* Convert fingerprint */
    pcTmpStr = (WE_CHAR *)WE_MALLOC((WE_UINT32)iFingerPrintLen*3+2);
    if(NULL == pcTmpStr)
    {
        return NULL;
    }
    pcBuf = pcTmpStr;
    for (iIndex = 0; iIndex < iFingerPrintLen; iIndex++)
    {
        WE_SCL_BYTE2HEX (*pucFP, pcBuf);
        pucFP++;
        pcBuf += 2;
        *pcBuf = ' ';
        pcBuf++;
    }
    pcBuf--;
    *pcBuf = '\0';
    return pcTmpStr;
}
/*==================================================================================================
FUNCTION: 
    Sec_UeFreeMem
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION: 
   free structure according to event type
ARGUMENTS PASSED:
    WE_INT32 eSecEvt,[IN]:event type.
    void* pvData[IN]:buffer for free
RETURN VALUE:
    TRUE:success
    FALSE:fail
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_VOID Sec_UeFreeMem(WE_INT32 eSecEvt, void* pvData)
{
    if(NULL == pvData)
    {
        return ;
    }
    switch (eSecEvt)
    {
        case M_UE_CONFIRM_EVENT:/*St_Confirm*/
            break;
        case M_UE_CHANGE_PIN_EVENT:/*St_Confirm*/
            break;
        case M_UE_SELECT_CET_EVENT:
            {
                St_SelectCert *pstChooseCertByName = (St_SelectCert *)pvData;
                WE_INT32 iNumOfCert = pstChooseCertByName->nbrOfCerts;
                WE_INT32 iIndex = 0;
                for(iIndex = 0;iIndex < iNumOfCert; iIndex ++)
                {
                    WE_FREE(pstChooseCertByName->ppcCertName[iIndex]);
                }
                WE_FREE(pstChooseCertByName->ppcCertName);                    
            }
            break;
            
        case M_UE_NAME_CONFIRM_EVENT:
            {
                St_NameConfirm *pstConfDispName = (St_NameConfirm*) pvData;
                WE_FREE(pstConfDispName->pcCertName);
            }
            break;
            
        case M_UE_SIGNTEXT_CONFIRM_EVENT:
            {
                St_SignTextConfirm* pstExtConfirm = (St_SignTextConfirm*)pvData;
                WE_FREE(pstExtConfirm->pcText);
            }
            break;
        case M_UE_CREATE_PIN_EVENT :/*St_CreatePin*/
            break;
        case M_UE_HASH_EVENT:
            {
                St_Hash* pstHash = (St_Hash*)pvData;
                WE_FREE(pstHash->pcDisplayName);
            }
            break;
        case M_UE_PIN_EVENT:/*St_Pin*/
            break;
        case M_UE_SHOW_CERT_CONTENT_EVENT:
            {
                St_ShowCertContent *pstShowCert = (St_ShowCertContent*)pvData;
                WE_FREE(pstShowCert->pucSerialNumber);
                WE_FREE(pstShowCert->pucAlg);
                WE_FREE(pstShowCert->pucPubKey);
                WE_FREE(pstShowCert->pcIssuerStr);
                WE_FREE(pstShowCert->pcNotAftStr);
                WE_FREE(pstShowCert->pcNotBefStr);
                WE_FREE(pstShowCert->pcSubjectStr);
            }
            break;
        case M_UE_SHOW_CERT_LIST_EVENT :
            {
                St_ShowCertList *pstShowCertName = (St_ShowCertList*)pvData;
                WE_INT32 iNumOfCert = pstShowCertName->nbrOfCerts;
                WE_INT32 iIndex = 0;
                for(iIndex = 0;iIndex < iNumOfCert; iIndex ++)
                {
                    WE_FREE(pstShowCertName->ppcCertName[iIndex]);
                }
                WE_FREE(pstShowCertName->ppcCertName);
                WE_FREE(pstShowCertName->pucAble);
                WE_FREE(pstShowCertName->pueCertType);
            }
            break;
        case M_UE_SHOW_SESSION_CONTENT_EVENT:
            {
                St_ShowSessionContent *pstShowSessionInfo = (St_ShowSessionContent*)pvData;
                WE_FREE(pstShowSessionInfo->pcEncrKeyLenStr);
                WE_FREE(pstShowSessionInfo->pcHmacKeyLenStr);
                WE_FREE(pstShowSessionInfo->pcKeyExchKeyLenStr);
            }
            break;
        case M_UE_SHOW_CONTRACT_LIST_EVENT :
            {
                St_ShowContractsList* pstStoredContracts = (St_ShowContractsList*)pvData;
                WE_INT32 iNumOfContract = pstStoredContracts->nbrOfContract;
                WE_INT32 iIndex = 0;
                for(iIndex = 0;iIndex < iNumOfContract; iIndex ++)
                {
                    WE_FREE(pstStoredContracts->ppcTime[iIndex]);
                }
                WE_FREE(pstStoredContracts->ppcTime);
                WE_FREE(pstStoredContracts->piMonthID);
            }
            break;
        case M_UE_STORE_CERT_EVENT:
            {
                St_StoreCert* pstStoreCert = (St_StoreCert*)pvData;
                WE_FREE(pstStoreCert->pcIssuerStr);
                WE_FREE(pstStoreCert->pcNotAftStr);
                WE_FREE(pstStoreCert->pcNotBefStr);
                WE_FREE(pstStoreCert->pcSubjectStr);
            }
            break;
        case M_UE_WARNING_EVENT:/*St_Warning*/
            break;
        case M_UE_STORE_CONTRACT_EVENT:
            {
                St_ShowContractContent* pstShowContract = (St_ShowContractContent*)pvData;
                WE_FREE(pstShowContract->pcContractStr);
                WE_FREE(pstShowContract->pcSignatureStr);
                WE_FREE(pstShowContract->pcTimeStr);
            }
            break;
        case G_Evt_SHOW_DLG:
            {
                St_ShowDlg* pstData = (St_ShowDlg*)pvData;
                M_SEC_SELF_FREE(pstData->pvBuf);
            }
            break;

        default:
            WE_LOG_MSG((0,0,"SEC:Sec_UeFreeMem, Unknown Event\n"));
            break;                
    }    
    WE_FREE(pvData);
}
/*==================================================================================================
FUNCTION: 
    Sec_RunUeCallBackFun
CREATE DATE:
    2006-12-09
AUTHOR:
    Bird 
DESCRIPTION:
    callback function of ue dialog
ARGUMENTS PASSED:
    WE_VOID* pvData:data to resolve
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_VOID Sec_RunUeCallBackFun(WE_VOID* hSecHandle)
{
    WE_INT32 iEvent = 0;
    WE_VOID* pvCBData = NULL;
    if( NULL == hSecHandle)
    {
        return;
    }
    iEvent = M_SEC_UECALLBACKEVENT;
    pvCBData = M_SEC_UECALLBACKDATA;
    /*set to null*/
    M_SEC_UECALLBACKDATA = NULL;
    if(NULL != SEC_CBBRS_EVTFUNC)
    {
        ((Fn_ISecEventHandle)(SEC_CBBRS_EVTFUNC))(iEvent,(void *)(pvCBData),SEC_BRS_PRIVDATA);
    }
    Sec_UeFreeMem(iEvent,pvCBData);

    
}
/*==================================================================================================
FUNCTION: 
    Sec_ReleaseUeCB
CREATE DATE:
    2006-12-19
AUTHOR:
    Bird
DESCRIPTION:
    free response callback data when release function is called, if the callback does not excute
ARGUMENTS PASSED:
    WE_VOID* pvData:data to resolve
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_VOID Sec_ReleaseUeCB(WE_VOID* hSecHandle)
{
    WE_INT32 iEvent = 0;
    WE_VOID* pvCBData = NULL;
    if( NULL == hSecHandle)
    {
        return;
    }
    iEvent = M_SEC_UECALLBACKEVENT;
    pvCBData = M_SEC_UECALLBACKDATA;
    Sec_UeFreeMem(iEvent,pvCBData);
}

