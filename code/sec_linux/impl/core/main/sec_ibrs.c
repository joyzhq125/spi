/*=====================================================================================
    FILE NAME :
        Sec_ibrs.c
    MODULE NAME :
        sec
    GENERAL DESCRIPTION
        this file provides a set of functions, which is for browser engine and app.
    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2002-2006 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
    Modification              Tracking
    Date       Author         Number           Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-07-08 Kevin           none            Init
    
=====================================================================================*/

/**************************************************************************************
*   Include File Section
**************************************************************************************/

#include "sec_comm.h"

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/

#define M_SEC_AEECALLBACKK_IBRS      (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->m_cb)
#define M_SEC_ISHELL_IBRS                   ((ISec*)hSecHandle)->m_pIShell

/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*=====================================================================================
FUNCTION: 
      Sec_MsgSignText
CREATE DATE: 
      2006-07-04
AUTHOR: 
      Kevin
DESCRIPTION:
        sign text for wml script from browser engine. the parameter should include the text , 
        hashed data of text,hashed public key or CA public key, and the returned data can be 
        appointed by options parameter, including certificate or the hashed public key.
ARGUMENTS PASSED:
      hSecHandle[IN]:Global data handle.
      iTargetID[IN]: The indentify of the invoker.
      iSignId[IN]: ID of the sign input from the invoker.
      pcText[IN]: the text contents for signature.
      pcData[IN]: Pointer to the data.
      iDataLen[IN]:The length of the data.
      iKeyIdType[IN]: the type of the key id.
      pcKeyId[IN]: ID of the key.
      iKeyIdLen[IN]: the length of the key id.
      iOptions[IN]: The value of the options.
RETURN VALUE:
      M_SEC_ERR_INSUFFICIENT_MEMORY:
      M_SEC_ERR_INVALID_PARAMETER:
         The result is error.
      M_SEC_ERR_OK:
         The status of this app is ok.
      
USED GLOBAL VARIABLES:
    None.
USED STATIC VARIABLES:
    None.
CALL BY:
    Omit.
IMPORTANT NOTES:
      Describe anything that help understanding the function and important aspect of
      using the function i.e side effect..etc
=====================================================================================*/
WE_INT32 Sec_MsgSignText (WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iSignId, const WE_CHAR * pcText,
                          const WE_CHAR * pcData, WE_INT32 iDataLen, WE_INT32 iKeyIdType, 
                          const WE_CHAR * pcKeyId, WE_INT32 iKeyIdLen, WE_INT32 iOptions)
{
    St_SecSignText* pstSignText = NULL;
    WE_INT32 iRes = 0;
    
    if(NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    pstSignText = (St_SecSignText*)WE_MALLOC((WE_ULONG)sizeof(St_SecSignText));
    if (NULL == pstSignText)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstSignText->iTargetID = iTargetID;
    pstSignText->iSignId = iSignId;
    pstSignText->pcText = (WE_CHAR*)pcText;
    pstSignText->pcData = (WE_CHAR*)pcData;
    pstSignText->usDataLen =(WE_UINT16)iDataLen;
    pstSignText->ucKeyIdType = (WE_UINT8)iKeyIdType;
    pstSignText->pcKeyId = (WE_CHAR*)pcKeyId;
    pstSignText->usKeyIdLen = (WE_UINT16)iKeyIdLen;
    pstSignText->ucOptions = (WE_UINT8)iOptions;
    
    
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_APP_SIGNTEXT, (void*)pstSignText);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstSignText);
        return iRes;
    }
    
    return M_SEC_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    Sec_MsgCertSave
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION:    
    when content with MIME type application/vnd.wap.wtls-user-certificate, application/x-x509-user-cert, 
    application/vnd.wap.wtls-ca-certificate or application/x-x509-ca-cert has been routed to SEC. 
    It stores the incoming certificate after verification.    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.
    WE_INT32 iTargetID[IN]:The indentify of the invoker.
    WE_CHAR * pcContentType[IN]:NULL terminated MIME content type
    WE_UCHAR * pucCert [IN]:The certificate to store. The certificate is a byte-encoded sequence of elements of type Certificate.
    WE_UINT16 usCertLen[IN]:The length of the certificate in bytes
    WE_UCHAR * pucCertURL[IN]:The URL to where the certificate is stored
RETURN VALUE:
    M_SEC_ERR_INVALID_PARAMETER:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_MsgCertSave(WE_HANDLE hSecHandle, WE_INT32 iTargetID, const WE_CHAR * pcContentType, 
                         const WE_UCHAR * pucCert, WE_UINT16 usCertLen, const WE_UCHAR * pucCertURL)
{
    St_SecSaveCert * pstStoreCert = NULL;
    WE_INT32 iRes = 0;
    
    if(NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    pstStoreCert = (St_SecSaveCert*)WE_MALLOC((WE_ULONG)sizeof(St_SecSaveCert));
    if(NULL == pstStoreCert)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    
    if(NULL == pcContentType)
    {
        pstStoreCert->pcContentType = NULL;
    }
    else
    {
         pstStoreCert->pcContentType = SEC_STRDUP(pcContentType);
         if(NULL == pstStoreCert->pcContentType)
         {
            WE_FREE(pstStoreCert);
            return M_SEC_ERR_INSUFFICIENT_MEMORY;
         }
    }

    if(NULL == pucCert)
    {
        pstStoreCert->pucCert = NULL;
    }
    else
    {        
        pstStoreCert->pucCert = (WE_UCHAR *)WE_MALLOC((WE_ULONG)(usCertLen + 1)*((WE_ULONG)sizeof(WE_CHAR)));    
        if (NULL == pstStoreCert->pucCert) 
        {
            WE_FREE(pstStoreCert->pcContentType);
            WE_FREE(pstStoreCert);
            return M_SEC_ERR_INSUFFICIENT_MEMORY;
        }
        (void)WE_MEMCPY(pstStoreCert->pucCert + 1,pucCert,(WE_UINT32)usCertLen);
    }

    if(NULL == pucCertURL)
    {
        pstStoreCert->pucCertURL = NULL;
    }
    else
    {
        pstStoreCert->pucCertURL = (WE_UCHAR *)SEC_STRDUP((WE_CHAR*)pucCertURL);
        if(NULL == pstStoreCert->pucCertURL)
        {
            WE_FREE(pstStoreCert->pcContentType);
            WE_FREE(pstStoreCert->pucCert);
            WE_FREE(pstStoreCert);
            return M_SEC_ERR_INSUFFICIENT_MEMORY;
        }
    }

    pstStoreCert->iTargetID = iTargetID;
    pstStoreCert->usCertLen = usCertLen;
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_CERTSAVE, (void*)pstStoreCert);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_SIGNAL_DESTRUCT(0, (WE_INT32)M_SEC_DP_MSG_STORE_CERT, (void*)pstStoreCert);
        return iRes;
    }
    
    SEC_REGCALLBACK(M_SEC_ISHELL_IBRS,M_SEC_AEECALLBACKK_IBRS,Sec_RunMainFlow,hSecHandle);

    return M_SEC_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    Sec_MsgTrustedCAInfoSave
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION:
    when content with MIME type application/vnd.wap.hashed-certificate 
    or application/vnd.wap.signed-certificate has been routed to SEC. 
    Stores the incoming certificate after verification.
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.
    WE_INT32 iTargetID[IN]:The identifier of this call
    WE_CHAR * pcContentType [IN]:NULL terminated MIME content type. 
    WE_UCHAR * pucTrustedCAInfoStruct[IN]:The structure containing the trusted CA Information
    WE_UINT16 usTrustedCAInfoStructLen[IN]:The length of the structure
RETURN VALUE:
    M_SEC_ERR_INVALID_PARAMETER:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_MsgTrustedCAInfoSave(WE_HANDLE hSecHandle, WE_INT32 iTargetID, const WE_CHAR * pcContentType,
                                  const WE_UCHAR * pucTrustedCAInfoStruct, WE_UINT16 usTrustedCAInfoStructLen)
{
    St_SecTrustedCaInfo* pstTrustedCaInfo = NULL;
    WE_INT32 iRes = 0;
    
    if(NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    pstTrustedCaInfo = (St_SecTrustedCaInfo*)WE_MALLOC((WE_ULONG)sizeof(St_SecTrustedCaInfo)); 
    if(NULL == pstTrustedCaInfo)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    (void)WE_MEMSET(pstTrustedCaInfo,0,sizeof(St_SecTrustedCaInfo));
    /*type*/
    if (NULL != pcContentType)
    {
        pstTrustedCaInfo->pcContentType = SEC_STRDUP(pcContentType);
        if(NULL == pstTrustedCaInfo->pcContentType)
        {
            WE_FREE(pstTrustedCaInfo);
            return M_SEC_ERR_INSUFFICIENT_MEMORY;
        }
    }
    /*data*/
    if (NULL!= pucTrustedCAInfoStruct)
    {
        pstTrustedCaInfo->pucTrustedCAInfoStruct  = (WE_UCHAR *)WE_MALLOC((WE_ULONG)(usTrustedCAInfoStructLen + 1)*(WE_ULONG)sizeof(WE_CHAR));    
        if (NULL == pstTrustedCaInfo->pucTrustedCAInfoStruct) 
        {
            WE_FREE(pstTrustedCaInfo->pcContentType);
            WE_FREE(pstTrustedCaInfo);
            return M_SEC_ERR_INSUFFICIENT_MEMORY;
        }
        (void)WE_MEMCPY(pstTrustedCaInfo->pucTrustedCAInfoStruct ,pucTrustedCAInfoStruct,(WE_UINT32)usTrustedCAInfoStructLen);
        pstTrustedCaInfo->pucTrustedCAInfoStruct[usTrustedCAInfoStructLen] = '\0';
    }
 
    pstTrustedCaInfo->iTargetID = iTargetID;
    pstTrustedCaInfo->usTrustedCAInfoStructLen = usTrustedCAInfoStructLen;
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_TRUSTEDCA, (void*)pstTrustedCaInfo);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_SIGNAL_DESTRUCT(0, (WE_INT32)M_SEC_DP_MSG_TRUSTED_CA_INFO, (void*)pstTrustedCaInfo);
        return iRes;
    }

    SEC_REGCALLBACK(M_SEC_ISHELL_IBRS,M_SEC_AEECALLBACKK_IBRS,Sec_RunMainFlow,hSecHandle);

    return M_SEC_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    Sec_MsgDeliveryCert
CREATE DATE:
    2006-07-20
AUTHOR:
    Bird Zhang
DESCRIPTION:
    when content with MIME type "application/vnd.wap.cert-response" has been routed to SEC. 
    It stores the incoming certificate after verification    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.
    WE_INT32 iTargetID[IN]:The identifier of this call
    WE_UCHAR * pucCertResponse[IN]:The structure containing information about the certificate to store
    WE_UINT16 usCertResponseLen[IN]:The length of the certResponse structure.
RETURN VALUE:
    M_SEC_ERR_INVALID_PARAMETER:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_MsgDeliveryCert(WE_HANDLE hSecHandle, WE_INT32 iTargetID, const WE_UCHAR * pucCertResponse, 
                             WE_UINT16 usCertResponseLen)
{
    St_SecCertDelivery* pstCertDelivery = NULL;
    WE_INT32 iRes = 0;
    
    if(NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    pstCertDelivery = (St_SecCertDelivery*)WE_MALLOC((WE_ULONG)sizeof(St_SecCertDelivery));
    if(NULL == pstCertDelivery)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    (void)WE_MEMSET(pstCertDelivery,0,sizeof(St_SecCertDelivery));
    
    if(NULL !=  pucCertResponse)
    {
        pstCertDelivery->pucCertResponse = (WE_UCHAR *)WE_MALLOC((WE_ULONG)(usCertResponseLen + 1)*(WE_ULONG)sizeof(WE_CHAR));    
        if (NULL == pstCertDelivery->pucCertResponse) 
        {
            WE_FREE(pstCertDelivery);
            return M_SEC_ERR_INSUFFICIENT_MEMORY;
        }
        (void)WE_MEMCPY(pstCertDelivery->pucCertResponse,pucCertResponse,(WE_UINT32)usCertResponseLen);
    }

    pstCertDelivery->iTargetID = iTargetID;
    pstCertDelivery->usCertResponseLen = usCertResponseLen;
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_DLVCERT, (void*)pstCertDelivery);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_SIGNAL_DESTRUCT(0, (WE_INT32)M_SEC_DP_MSG_DELIVERY_CERT, (void*)pstCertDelivery);
        return iRes;
    }
    
    SEC_REGCALLBACK(M_SEC_ISHELL_IBRS,M_SEC_AEECALLBACKK_IBRS,Sec_RunMainFlow,hSecHandle);
    return M_SEC_ERR_OK;
}

/*=====================================================================================
FUNCTION: 
      Sec_MsgGetCertNameList
CREATE DATE: 
      2006-07-04
AUTHOR: 
      Kevin
DESCRIPTION:
      the function provide the function to list all the certificate name, and these name list will
      be return to the caller. and the certificate type can be selected.
ARGUMENTS PASSED:
      WE_HANDLE     hSecHandle[IN]:Global data handle.
      WE_INT32      iTargetID[IN]: The indentify of the invoker.
      WE_INT32      iCertOptions[IN]:The value of the cert option.
RETURN VALUE:
      M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
      M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
      None.
USED STATIC VARIABLES:
      None.
CALL BY:
    Omit.
IMPORTANT NOTES:
      Describe anything that help understanding the function and important aspect of
      using the function i.e side effect..etc
=====================================================================================*/
WE_INT32 Sec_MsgGetCertNameList(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iCertOptions)
{
    WE_INT32 iRes = 0;
    St_SecGetCertNameList* pstViewCertNames = (St_SecGetCertNameList*)WE_MALLOC((WE_ULONG)sizeof(St_SecGetCertNameList));
    
    if(NULL == pstViewCertNames)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    
    pstViewCertNames->iTargetID = iTargetID; 
    pstViewCertNames->ucCertOptions = (WE_UINT8)iCertOptions;
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_GETCERTNAMELIST, (void*)pstViewCertNames);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstViewCertNames);
        return iRes;
    }
   
    return M_SEC_ERR_OK;
}

/*=====================================================================================
FUNCTION: 
      Sec_MsgKeyPairGen
CREATE DATE: 
      2006-07-04
AUTHOR: 
      Kevin
DESCRIPTION:
    generate the public and private key pairs and private key will be stored after encrypted
     and public key will be saved as a selfmade certificate.
ARGUMENTS PASSED:
    WE_HANDLE   hSecHandle[IN]:Global data handle.
    WE_INT32    iTargetID[IN]:The indentify of the invoker.
    WE_UINT8    ucKeyType[IN]:The type of the key.
    WE_UINT8    ucAlg[IN]:The value of the alg.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
    None.
USED STATIC VARIABLES:
    None.
CALL BY:
    Omit.
IMPORTANT NOTES:
      Describe anything that help understanding the function and important aspect of
      using the function i.e side effect..etc
=====================================================================================*/
WE_INT32 Sec_MsgKeyPairGen(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_UINT8 ucKeyType, WE_UINT8 ucAlg)
{
    WE_INT32 iRes = 0;
    St_SecGenKeyPair* pstGenerateKeyPair = (St_SecGenKeyPair*)WE_MALLOC((WE_ULONG)sizeof(St_SecGenKeyPair));

    if(NULL == pstGenerateKeyPair)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstGenerateKeyPair->iTargetID = iTargetID;
    pstGenerateKeyPair->ucAlg = ucAlg;
    pstGenerateKeyPair->ucKeyType = ucKeyType;
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_GENKEYPAIR, (void*)pstGenerateKeyPair);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstGenerateKeyPair);
        return iRes;
    }

    return M_SEC_ERR_OK;
}


/*=====================================================================================
FUNCTION: 
      Sec_MsgModifyPin
CREATE DATE: 
      2006-07-04
AUTHOR: 
      Kevin
DESCRIPTION:
      modify the pin code for private key.
ARGUMENTS PASSED:
      WE_HANDLE hSecHandle[IN]:Global data handle.
      WE_INT32  iTargetID[IN]:The indentify of the invoker.
      WE_UINT8  ucKeyType[IN]:The type of the key.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
        None.
USED STATIC VARIABLES:
        None.
CALL BY:
    Omit.
IMPORTANT NOTES:
      Describe anything that help understanding the function and important aspect of
      using the function i.e side effect..etc
=====================================================================================*/
WE_INT32 Sec_MsgModifyPin(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_UINT8 ucKeyType)
{
    WE_INT32 iRes = 0;
    St_SecModifyPriKeyPin* pstChangePin = (St_SecModifyPriKeyPin*)WE_MALLOC((WE_ULONG)sizeof(St_SecModifyPriKeyPin));

    if(NULL == pstChangePin)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstChangePin->iTargetID = iTargetID;
    pstChangePin->ucKeyType = ucKeyType;

    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_MODIFYPRIKEYPIN, (void*)pstChangePin);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstChangePin);
        return iRes;
    }

    return M_SEC_ERR_OK;
}


/*=====================================================================================
FUNCTION: 
      Sec_MsgViewCurCert
CREATE DATE: 
      2006-07-04
AUTHOR: 
      Kevin
DESCRIPTION:
        view the current certificate information and return the content of the certificate.
ARGUMENTS PASSED:
       WE_HANDLE    hSecHandle[IN]:Global data handle.
       WE_INT32     iTargetID[IN]:The indentify of the invoker.
       WE_INT32     iSecurityId[IN]:The ID of the security.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
    None.
USED STATIC VARIABLES:
    None.
CALL BY:
    Omit.
IMPORTANT NOTES:
      Describe anything that help understanding the function and important aspect of
      using the function i.e side effect..etc
=====================================================================================*/
WE_INT32 Sec_MsgViewCurCert(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iSecurityId)
{
    WE_INT32 iRes = 0;
    St_SecViewCurCert *pstViewCurrentCert = (St_SecViewCurCert*)WE_MALLOC((WE_ULONG)sizeof(St_SecViewCurCert));

    if(NULL == pstViewCurrentCert)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstViewCurrentCert->iTargetID = iTargetID;
    pstViewCurrentCert->iSecurityId = iSecurityId;
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_VIEW_CURCERT, (void*)pstViewCurrentCert);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstViewCurrentCert);
        return iRes;
    }

    return M_SEC_ERR_OK;
}
/*=====================================================================================
FUNCTION: 
      Sec_MsgViewInfoSession
CREATE DATE: 
      2006-07-04
AUTHOR: 
      Kevin
DESCRIPTION:
      view session information when the security channel is working,get the session
      information.
ARGUMENTS PASSED:
      WE_HANDLE hSecHandle[IN]:Global data handle.
      WE_INT32  iTargetID[IN]:The indentify of the invoker.
      WE_INT32  iSecurityId[IN]:ID of the security.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
    None.
USED STATIC VARIABLES:
    None.
CALL BY:
    Omit.
IMPORTANT NOTES:
      Describe anything that help understanding the function and important aspect of
      using the function i.e side effect..etc
=====================================================================================*/
WE_INT32 Sec_MsgViewInfoSession(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iSecurityId)
{
    WE_INT32 iRes = 0;
    St_SecViewSessionInfo *pstViewSessionInfo = (St_SecViewSessionInfo*)WE_MALLOC((WE_ULONG)sizeof(St_SecViewSessionInfo));

    if(NULL == pstViewSessionInfo)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstViewSessionInfo->iTargetID = iTargetID;
    pstViewSessionInfo->iSecurityId = iSecurityId;
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_VIEW_INFOSESSION, (void*)pstViewSessionInfo);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstViewSessionInfo);
        return iRes;
    }

    return M_SEC_ERR_OK;
}

/*=====================================================================================
FUNCTION: 
      Sec_MsgGetSecCurClass
CREATE DATE: 
      2006-07-04
AUTHOR: 
      Kevin
DESCRIPTION:
      get current sec class level.
ARGUMENTS PASSED:
      WE_HANDLE     hSecHandle[IN]:Global data handle.
      WE_INT32      iTargetID[IN]:The indentify of the invoker.
      WE_INT32      iSecurityId[IN]:ID of the security.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit.
IMPORTANT NOTES:
      Describe anything that help understanding the function and important aspect of
      using the function i.e side effect..etc
=====================================================================================*/
WE_INT32 Sec_MsgGetSecCurClass(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iSecurityId)
{
    WE_INT32 iRes = 0;
    St_SecGetSecCurClass *pstGetCurrentSecClass = 
                    (St_SecGetSecCurClass*)WE_MALLOC((WE_ULONG)sizeof(St_SecGetSecCurClass));

    if(NULL == pstGetCurrentSecClass)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstGetCurrentSecClass->iTargetID = iTargetID;
    pstGetCurrentSecClass->iSecurityId = iSecurityId;
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_GET_CURCLASS, (void*)pstGetCurrentSecClass);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstGetCurrentSecClass);
        return iRes;
    }

    return M_SEC_ERR_OK;
}
/*=====================================================================================
FUNCTION: 
      Sec_MsgGetPublicKey
CREATE DATE: 
      2006-07-04
AUTHOR: 
      Kevin
DESCRIPTION:
      get public key based on the keytype, and there are two key types, one is for non-repudation
      and the other is for authentication.
ARGUMENTS PASSED:
      WE_HANDLE hSecHandle[IN]:Global data handle.
      WE_INT32  iTargetID[IN]:The indentify of the invoker.
      WE_UINT8  ucKeyType[IN]:key type.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit.
IMPORTANT NOTES:
      Describe anything that help understanding the function and important aspect of
      using the function i.e side effect..etc
=====================================================================================*/
WE_INT32 Sec_MsgGetPublicKey(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_UINT8 ucKeyType)
{  
    WE_INT32 iRes = 0;  
    St_SecGetPublicKey * pstGetPubKey = 
        (St_SecGetPublicKey*)WE_MALLOC((WE_ULONG)sizeof(St_SecGetPublicKey));
    
    if(NULL == pstGetPubKey)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstGetPubKey->iTargetID = iTargetID;
    pstGetPubKey->ucKeyType = ucKeyType;
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_GETPUBKEY, (void*)pstGetPubKey);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstGetPubKey);
        return iRes;
    }
    
    return M_SEC_ERR_OK;        
}

/*=====================================================================================
FUNCTION: 
      Sec_MsgClrSession
CREATE DATE: 
      2006-07-04
AUTHOR: 
      Kevin
DESCRIPTION:
      clear the current session information.
ARGUMENTS PASSED:
      WE_HANDLE hSecHandle[IN]:Global data handle.
      WE_INT32  iTargetID[IN]:The indentify of the invoker.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
    None.
USED STATIC VARIABLES:
    None.
CALL BY:
    Omit.
IMPORTANT NOTES:
      Describe anything that help understanding the function and important aspect of
      using the function i.e side effect..etc
=====================================================================================*/
WE_INT32 Sec_MsgClrSession(WE_HANDLE hSecHandle, WE_INT32 iTargetID)
{
    WE_INT32 iRes = 0;
    St_SecClrSession *pstSessionClear = (St_SecClrSession*)WE_MALLOC((WE_ULONG)sizeof(St_SecClrSession));
    
    if(NULL == pstSessionClear)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstSessionClear->iTargetID = iTargetID;
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_CLRSESSION, (void*)pstSessionClear);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstSessionClear);
        return iRes;
    }
    return M_SEC_ERR_OK;
}

/*=====================================================================================
FUNCTION: 
      Sec_MsgGetContractsInfos
CREATE DATE: 
      2006-07-04
AUTHOR: 
      Kevin
DESCRIPTION:
      get all the contract information, and will give the information list.
ARGUMENTS PASSED:
      WE_HANDLE     hSecHandle[IN]:Gloabl data handle.
      WE_INT32      iTargetID[IN]:The indentify of the invoker.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
    None.
USED STATIC VARIABLES:
    None.
CALL BY:
    Omit.
IMPORTANT NOTES:
      Describe anything that help understanding the function and important aspect of
      using the function i.e side effect..etc
=====================================================================================*/
WE_INT32 Sec_MsgGetContractsInfos(WE_HANDLE hSecHandle, WE_INT32 iTargetID)
{
    WE_INT32 iRes = 0;
    St_SecGetContractInfo* pstViewContracts = (St_SecGetContractInfo*)WE_MALLOC((WE_ULONG)sizeof(St_SecGetContractInfo));
    
    if(NULL == pstViewContracts)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstViewContracts->iTargetID = iTargetID;
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_GETCONTRACTINFOS, (void*)pstViewContracts);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstViewContracts);
        return iRes;
    }

    return M_SEC_ERR_OK;
}

/*=====================================================================================
FUNCTION: 
      Sec_MsgGetCertContent
CREATE DATE: 
      2006-07-04
AUTHOR: 
      Kevin
DESCRIPTION:
      get certificate content information.
ARGUMENTS PASSED:
      WE_HANDLE     hSecHandle[IN]:Gloabl data handle.
      WE_INT32      iTargetID[IN]:The indentify of the invoker.
      WE_INT32      iCertId[IN]:ID of cert.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
    None.
USED STATIC VARIABLES:
    None.
CALL BY:
    Omit.
IMPORTANT NOTES:
      Describe anything that help understanding the function and important aspect of
      using the function i.e side effect..etc
=====================================================================================*/
WE_INT32 Sec_MsgViewAndGetCert(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iCertId)
{
    WE_INT32 iRes = 0;
    St_SecGetCertContent* pstViewAndGetCert = NULL;
    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    pstViewAndGetCert = (St_SecGetCertContent*)WE_MALLOC((WE_ULONG)sizeof(St_SecGetCertContent));
    if(NULL == pstViewAndGetCert)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstViewAndGetCert->iTargetID = iTargetID;
    pstViewAndGetCert->ucCertId = (WE_UINT8)iCertId;
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_GET_CERTCONTENT, (void*)pstViewAndGetCert);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstViewAndGetCert);
        return iRes;
    }
    
    SEC_REGCALLBACK(M_SEC_ISHELL_IBRS,M_SEC_AEECALLBACKK_IBRS,Sec_RunMainFlow,hSecHandle);
    return M_SEC_ERR_OK;
}

/*=====================================================================================
FUNCTION: 
      Sec_MsgRemoveCert
CREATE DATE: 
      2006-07-04
AUTHOR: 
      Kevin
DESCRIPTION:
      remove certificate
ARGUMENTS PASSED:
       WE_HANDLE        hSecHandle[IN]:Gloabl data handle.
       WE_INT32         iTargetID[IN]:The indentify of the invoker.
       WE_INT32         iCertId[IN]:The ID of the cert.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
    None.
USED STATIC VARIABLES:
    None.
CALL BY:
    Omit.
IMPORTANT NOTES:
      Describe anything that help understanding the function and important aspect of
      using the function i.e side effect..etc
=====================================================================================*/
WE_INT32 Sec_MsgDeleteCert(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iCertId)
{
    WE_INT32 iRes = 0;
    St_SecRemoveCert *pstDeleteCert = NULL;
    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    pstDeleteCert = (St_SecRemoveCert*)WE_MALLOC((WE_ULONG)sizeof(St_SecRemoveCert));
    if(NULL == pstDeleteCert)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstDeleteCert->iTargetID = iTargetID;
    pstDeleteCert->ucCertId = (WE_UCHAR)iCertId;
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_REMOVECERT, (void*)pstDeleteCert);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstDeleteCert);
        return iRes;
    }

    SEC_REGCALLBACK(M_SEC_ISHELL_IBRS,M_SEC_AEECALLBACKK_IBRS,Sec_RunMainFlow,hSecHandle);

    return M_SEC_ERR_OK;
}
 
/*=====================================================================================
FUNCTION: 
      Sec_MsgDeleteContract
CREATE DATE: 
      2006-07-04
AUTHOR: 
      Kevin
DESCRIPTION:
       remove contract
ARGUMENTS PASSED:
       WE_HANDLE    hSecHandle[IN]:Global data handle.
       WE_INT32     iTargetID[IN]:The indentify of the invoker.
       WE_INT32     iContractId[IN]:ID of the contract.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
    None.
USED STATIC VARIABLES:
    None.
CALL BY:
    Omit.
IMPORTANT NOTES:
      Describe anything that help understanding the function and important aspect of
      using the function i.e side effect..etc
=====================================================================================*/
WE_INT32 Sec_MsgDeleteContract(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iContractId)
{
    WE_INT32 iRes = 0;
    St_SecRemoveContract *pstDeleteContract = NULL;
    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }

    pstDeleteContract = (St_SecRemoveContract*)WE_MALLOC((WE_ULONG)sizeof(St_SecRemoveContract));
    if(NULL == pstDeleteContract)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstDeleteContract->iTargetID = iTargetID;
    pstDeleteContract->ucContractId = (WE_UINT8)iContractId;
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_REMOVECONTRACT, (void*)pstDeleteContract);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstDeleteContract);
        return iRes;
    }
    
    SEC_REGCALLBACK(M_SEC_ISHELL_IBRS,M_SEC_AEECALLBACKK_IBRS,Sec_RunMainFlow,hSecHandle);

    return M_SEC_ERR_OK;
}

/*=====================================================================================
FUNCTION: 
      Sec_MsgChangeWTLSCertAble
CREATE DATE: 
      2006-07-04
AUTHOR: 
      Kevin
DESCRIPTION:
      change wtls certificate enable/disable
ARGUMENTS PASSED:
      WE_HANDLE     hSecHandle[IN]:Gloabl data handle.
      WE_INT32      iTargetID[IN]:The indentify of the invoker.
      WE_INT32      iCertId[IN]:ID of cert.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
    None.
USED STATIC VARIABLES:
    None.
CALL BY:
    Omit.
IMPORTANT NOTES:
      Describe anything that help understanding the function and important aspect of
      using the function i.e side effect..etc
=====================================================================================*/
WE_INT32 Sec_MsgChangeWTLSCertAble(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iCertId)
{
    WE_INT32 iRes = 0;
    St_SecChangeWtlsCertAble* pstViewAndGetCert = (St_SecChangeWtlsCertAble*)WE_MALLOC((WE_ULONG)sizeof(St_SecChangeWtlsCertAble));
    
    if ((NULL == hSecHandle) || (NULL == pstViewAndGetCert))
    {
        if(NULL != pstViewAndGetCert)
        {
            WE_FREE(pstViewAndGetCert);
            pstViewAndGetCert = NULL;
        }
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    
    pstViewAndGetCert->iTargetID = iTargetID;
    pstViewAndGetCert->uiCertId = (WE_UINT32)iCertId;
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_CHANGE_WTLS_CERT_ABLE, (void*)pstViewAndGetCert);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstViewAndGetCert);
        return iRes;
    }
    
    SEC_REGCALLBACK(M_SEC_ISHELL_IBRS, M_SEC_AEECALLBACKK_IBRS, Sec_RunMainFlow, hSecHandle);
    return M_SEC_ERR_OK;
}
/*=====================================================================================
FUNCTION: 
      Sec_MsgEvtShowContractsListAction
CREATE DATE: 
      2007-01-30
AUTHOR: 
      Bird Zhang
DESCRIPTION:
      show contracts list action
ARGUMENTS PASSED:
      WE_HANDLE     hSecHandle[IN]:Gloabl data handle.
      WE_INT32      iTargetID[IN]:The indentify of the invoker.
      St_ShowContractsListAction stShowContractList[IN]:input data
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
    None.
USED STATIC VARIABLES:
    None.
CALL BY:
    Omit.
IMPORTANT NOTES:
      Describe anything that help understanding the function and important aspect of
      using the function i.e side effect..etc
=====================================================================================*/

WE_INT32 
Sec_MsgEvtShowContractsListAction(WE_HANDLE hSecHandle, WE_INT32 iTargetId,St_ShowContractsListAction stShowContractList)
{
    WE_INT32 iRes = 0;
    St_SecShowContractListAction* pstData= (St_SecShowContractListAction*)WE_MALLOC\
                                                    ((WE_ULONG)sizeof(St_SecShowContractListAction));
    
    if ((NULL == hSecHandle) || (NULL == pstData))
    {
        if(NULL != pstData)
        {
            WE_FREE(pstData);
            pstData = NULL;
        }
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstData->iTargetID = iTargetId;
    
    (void)WE_MEMCPY(&(pstData->stShowContractList),&stShowContractList,sizeof(St_ShowContractsListAction));
    
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_SHOWCONTRACTLIST, (void*)pstData);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstData);
        return iRes;
    }    
    
    return M_SEC_ERR_OK;
}
/*=====================================================================================
FUNCTION: 
      Sec_MsgEvtShowContractsContentAction
CREATE DATE: 
      2007-01-30
AUTHOR: 
      Bird Zhang
DESCRIPTION:
      show contracts content
ARGUMENTS PASSED:
      WE_HANDLE     hSecHandle[IN]:Gloabl data handle.
      WE_INT32      iTargetID[IN]:The indentify of the invoker.
      St_ShowContractContentAction stShowContractCont[IN]:input data
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
    None.
USED STATIC VARIABLES:
    None.
CALL BY:
    Omit.
IMPORTANT NOTES:
      Describe anything that help understanding the function and important aspect of
      using the function i.e side effect..etc
=====================================================================================*/
WE_INT32 
Sec_MsgEvtShowContractsContentAction(WE_HANDLE hSecHandle, WE_INT32 iTargetId,St_ShowContractContentAction stShowContractCont)
{
    WE_INT32 iRes = 0;
    St_SecShowContractContAction* pstData= (St_SecShowContractContAction*)WE_MALLOC\
                            ((WE_ULONG)sizeof(St_SecShowContractContAction));
    
    if ((NULL == hSecHandle) || (NULL == pstData))
    {
        if(NULL != pstData)
        {
            WE_FREE(pstData);
            pstData = NULL;
        }
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstData->iTargetID = iTargetId;
    (void)WE_MEMCPY(&(pstData->stShowContractCont),&stShowContractCont,sizeof(St_ShowContractContentAction));
    
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_SHOWCONTRACTCONTENT, (void*)pstData);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstData);
        return iRes;
    }
    
    return M_SEC_ERR_OK;
}
/*=====================================================================================
FUNCTION: 
      Sec_MsgEvtShowCertListAction
CREATE DATE: 
      2007-01-30
AUTHOR: 
      Bird Zhang
DESCRIPTION:
      show certificate list action
ARGUMENTS PASSED:
      WE_HANDLE     hSecHandle[IN]:Gloabl data handle.
      WE_INT32      iTargetID[IN]:The indentify of the invoker.
      St_ShowCertListAction stShowCertList[IN]:input data
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
    None.
USED STATIC VARIABLES:
    None.
CALL BY:
    Omit.
IMPORTANT NOTES:
      Describe anything that help understanding the function and important aspect of
      using the function i.e side effect..etc
=====================================================================================*/
WE_INT32 
Sec_MsgEvtShowCertListAction(WE_HANDLE hSecHandle, WE_INT32 iTargetId,St_ShowCertListAction stShowCertList)
{
    WE_INT32 iRes = 0;
    St_SecShowCertListAction* pstData= (St_SecShowCertListAction*)WE_MALLOC\
                                                    ((WE_ULONG)sizeof(St_SecShowCertListAction));
    
    if ((NULL == hSecHandle) || (NULL == pstData))
    {
        if(NULL != pstData)
        {
            WE_FREE(pstData);
            pstData = NULL;
        }
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstData->iTargetID = iTargetId;
    
    (void)WE_MEMCPY(&(pstData->stShowCertList),&stShowCertList,sizeof(St_ShowCertListAction));
    
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_SHOWCERTLIST, (void*)pstData);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstData);
        return iRes;
    }
    
    return M_SEC_ERR_OK;
}
/*=====================================================================================
FUNCTION: 
      Sec_MsgEvtShowCertListAction
CREATE DATE: 
      2007-01-30
AUTHOR: 
      Bird Zhang
DESCRIPTION:
      show certificate list action
ARGUMENTS PASSED:
      WE_HANDLE     hSecHandle[IN]:Gloabl data handle.
      WE_INT32      iTargetID[IN]:The indentify of the invoker.
      St_ShowCertContentAction stShowCertCont[IN]:input data
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
    None.
USED STATIC VARIABLES:
    None.
CALL BY:
    Omit.
IMPORTANT NOTES:
      Describe anything that help understanding the function and important aspect of
      using the function i.e side effect..etc
=====================================================================================*/
WE_INT32 
Sec_MsgEvtShowCertsContentAction(WE_HANDLE hSecHandle, WE_INT32 iTargetId,St_ShowCertContentAction stShowCertCont)
{
    WE_INT32 iRes = 0;
    St_SecShowCertContAction* pstData= (St_SecShowCertContAction*)WE_MALLOC\
                            ((WE_ULONG)sizeof(St_SecShowCertContAction));    
    if ((NULL == hSecHandle) || (NULL == pstData))
    {
        if(NULL != pstData)
        {
            WE_FREE(pstData);
            pstData = NULL;
        }
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstData->iTargetID = iTargetId;
    (void)WE_MEMCPY(&(pstData->stShowCertCont),&stShowCertCont,sizeof(St_ShowCertContentAction));
    
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_SHOWCERTCONTENT, (void*)pstData);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstData);
        return iRes;
    }
    
    return M_SEC_ERR_OK;
}
/*=====================================================================================
FUNCTION: 
      Sec_MsgEvtHashAction
CREATE DATE: 
      2007-01-30
AUTHOR: 
      Bird Zhang
DESCRIPTION:
      hash action
ARGUMENTS PASSED:
      WE_HANDLE     hSecHandle[IN]:Gloabl data handle.
      WE_INT32      iTargetID[IN]:The indentify of the invoker.
      St_HashAction stHash[IN]:input data
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
    None.
USED STATIC VARIABLES:
    None.
CALL BY:
    Omit.
IMPORTANT NOTES:
      Describe anything that help understanding the function and important aspect of
      using the function i.e side effect..etc
=====================================================================================*/

WE_INT32 
Sec_MsgEvtHashAction(WE_HANDLE hSecHandle, WE_INT32 iTargetId,St_HashAction stHash)
{
    WE_INT32 iRes = 0;
    St_SecHashAction* pstData= (St_SecHashAction*)WE_MALLOC\
                            ((WE_ULONG)sizeof(St_SecHashAction));    
    if ((NULL == hSecHandle) || (NULL == pstData))
    {
        if(NULL != pstData)
        {
            WE_FREE(pstData);
            pstData = NULL;
        }
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstData->iTargetID = iTargetId;
    (void)WE_MEMCPY(&(pstData->stHash),&stHash,sizeof(St_HashAction));
    
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_HASH, (void*)pstData);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstData);
        return iRes;
    }
    
    return M_SEC_ERR_OK;
}
/*=====================================================================================
FUNCTION: 
      Sec_MsgEvtStoreCertAction
CREATE DATE: 
      2007-01-30
AUTHOR: 
      Bird Zhang
DESCRIPTION:
      store certificate action
ARGUMENTS PASSED:
      WE_HANDLE     hSecHandle[IN]:Gloabl data handle.
      WE_INT32      iTargetID[IN]:The indentify of the invoker.
      St_StoreCertAction stStoreCert[IN]:input data
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
    None.
USED STATIC VARIABLES:
    None.
CALL BY:
    Omit.
IMPORTANT NOTES:
      Describe anything that help understanding the function and important aspect of
      using the function i.e side effect..etc
=====================================================================================*/

WE_INT32 
Sec_MsgEvtStoreCertAction(WE_HANDLE hSecHandle ,WE_INT32 iTargetId,St_StoreCertAction stStoreCert)
{
    WE_INT32 iRes = 0;
    St_SecStoreAction* pstData= (St_SecStoreAction*)WE_MALLOC\
                            ((WE_ULONG)sizeof(St_SecStoreAction));    
    if ((NULL == hSecHandle) || (NULL == pstData))
    {
        if(NULL != pstData)
        {
            WE_FREE(pstData);
            pstData = NULL;
        }
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstData->iTargetID = iTargetId;
    (void)WE_MEMCPY(&(pstData->stStoreCert),&stStoreCert,sizeof(St_StoreCertAction));
    
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_STORECERT, (void*)pstData);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstData);
        return iRes;
    }
    
    return M_SEC_ERR_OK;
}
/*=====================================================================================
FUNCTION: 
      Sec_MsgEvtConfirmAction
CREATE DATE: 
      2007-01-30
AUTHOR: 
      Bird Zhang
DESCRIPTION:
      confirm action
ARGUMENTS PASSED:
      WE_HANDLE     hSecHandle[IN]:Gloabl data handle.
      WE_INT32      iTargetID[IN]:The indentify of the invoker.
      St_ConfirmAction stConfirm[IN]:input data
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
    None.
USED STATIC VARIABLES:
    None.
CALL BY:
    Omit.
IMPORTANT NOTES:
      Describe anything that help understanding the function and important aspect of
      using the function i.e side effect..etc
=====================================================================================*/

WE_INT32 
Sec_MsgEvtConfirmAction(WE_HANDLE hSecHandle ,WE_INT32 iTargetId,St_ConfirmAction stConfirm)
{
    WE_INT32 iRes = 0;
    St_SecConfirmAction* pstData= (St_SecConfirmAction*)WE_MALLOC\
                            ((WE_ULONG)sizeof(St_SecConfirmAction));    
    if ((NULL == hSecHandle) || (NULL == pstData))
    {
        if(NULL != pstData)
        {
            WE_FREE(pstData);
            pstData = NULL;
        }
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstData->iTargetID = iTargetId;
    (void)WE_MEMCPY(&(pstData->stConfirm),&stConfirm,sizeof(St_ConfirmAction));
    
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_CONFIRM, (void*)pstData);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstData);
        return iRes;
    }
    
    return M_SEC_ERR_OK;
}
/*=====================================================================================
FUNCTION: 
      Sec_MsgEvtNameConfirmAction
CREATE DATE: 
      2007-01-30
AUTHOR: 
      Bird Zhang
DESCRIPTION:
      name confirm action
ARGUMENTS PASSED:
      WE_HANDLE     hSecHandle[IN]:Gloabl data handle.
      WE_INT32      iTargetID[IN]:The indentify of the invoker.
      St_NameConfirmAction stNameConfirm[IN]:input data
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
    None.
USED STATIC VARIABLES:
    None.
CALL BY:
    Omit.
IMPORTANT NOTES:
      Describe anything that help understanding the function and important aspect of
      using the function i.e side effect..etc
=====================================================================================*/

WE_INT32 
Sec_MsgEvtNameConfirmAction(WE_HANDLE hSecHandle ,WE_INT32 iTargetId,
                                                    St_NameConfirmAction stNameConfirm)
{
    WE_INT32 iRes = 0;
    St_SecNameConfirmAction* pstData= (St_SecNameConfirmAction*)WE_MALLOC\
                            ((WE_ULONG)sizeof(St_SecNameConfirmAction));    
    if ((NULL == hSecHandle) || (NULL == pstData))
    {
        if(NULL != pstData)
        {
            WE_FREE(pstData);
            pstData = NULL;
        }
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstData->iTargetID = iTargetId;
    (void)WE_MEMCPY(&(pstData->stNameConfirm),&stNameConfirm,sizeof(St_NameConfirmAction));
    
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_NAMECONFIRM, (void*)pstData);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstData);
        return iRes;
    }
    
    return M_SEC_ERR_OK;
}
/*=====================================================================================
FUNCTION: 
      Sec_MsgEvtSelCertAction
CREATE DATE: 
      2007-01-30
AUTHOR: 
      Bird Zhang
DESCRIPTION:
      select certificate action
ARGUMENTS PASSED:
      WE_HANDLE     hSecHandle[IN]:Gloabl data handle.
      WE_INT32      iTargetID[IN]:The indentify of the invoker.
      St_SelectCertAction stSelCert[IN]:input data
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The result is OK.
USED GLOBAL VARIABLES:
    None.
USED STATIC VARIABLES:
    None.
CALL BY:
    Omit.
IMPORTANT NOTES:
      Describe anything that help understanding the function and important aspect of
      using the function i.e side effect..etc
=====================================================================================*/
WE_INT32 
Sec_MsgEvtSelCertAction(WE_HANDLE hSecHandle ,WE_INT32 iTargetId,
                                                    St_SelectCertAction stSelCert)
{
    WE_INT32 iRes = 0;
    St_SecSelCertAction* pstData= (St_SecSelCertAction*)WE_MALLOC\
                            ((WE_ULONG)sizeof(St_SecSelCertAction));    
    if ((NULL == hSecHandle) || (NULL == pstData))
    {
        if(NULL != pstData)
        {
            WE_FREE(pstData);
            pstData = NULL;
        }
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstData->iTargetID = iTargetId;
    (void)WE_MEMCPY(&(pstData->stSelCert),&stSelCert,sizeof(St_SelectCertAction));
    
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_SELECTCERT, (void*)pstData);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstData);
        return iRes;
    }
    
    return M_SEC_ERR_OK;
}


