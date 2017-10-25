/*==================================================================================================
HEADER NAME : sec_wimresp.c
MODULE NAME : SEC

  PRE-INCLUDE FILES DESCRIPTION
  
    
      GENERAL DESCRIPTION
      In this file, define the response function for WIM module, and will be updated later.
      
        TECHFAITH Software Confidential Proprietary
        (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
        ====================================================================================================
        Revision History
        
          Modification                   Tracking
          Date          Author            Number      Description of changes
          ----------   ------------    ---------   -------------------------------------------------------
          2006-07-19    Stone An          0.0.1           create sec_wimresp.c
==================================================================================================*/

/*******************************************************************************
*   Include File Section
*******************************************************************************/
#include "sec_comm.h"


/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
/*Define Macro Function start*/
#define   M_SEC_WANTS_TO_RUN                     (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->iWantsToRun)
#define   M_SEC_WAITING_FOR_WIM_RESULT           (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->iWaitingForWim)
#define   M_SEC_CURRENT_WIM_RESPONSE_VALUES      (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pvCurWimResp)
/*Define Macro Function end*/

/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*=====================================================================================
FUNCTION: 
        Sec_WimCertNamesResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        Store certificate name in 'CurrentWimResponsevalues'.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iResult[IN]: result of the calling function.    
        iNumOfCerts[IN]: number of certificate.
        ppvCertFriendlyNames[IN]: name of certificate.
        pusFriendlyNameLen[IN]: length of name.
        psCharSet[IN]: character set of certificate.
        pcNameType[IN]: type of name.
        piCertIds[IN]: Id of certificate.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
static WE_VOID Sec_WimCertNamesResponse(WE_HANDLE hSecHandle,
                                  WE_INT32 iResult, WE_INT32 iNumOfCerts, WE_UINT8 *pucCertType, 
                                  const WE_VOID *const *ppvCertFriendlyNames, 
                                  WE_UINT16 *pusFriendlyNameLen, WE_INT16 *psCharSet, 
                                  WE_INT8 *pcNameType, const WE_INT32 *piCertIds, WE_UINT8 *pucAble)
{
    St_SecTlCertNamesResp *pstValue = NULL;
    WE_INT32        iIndex = 0;
    WE_INT32        iIndexj = 0;
    WE_CHAR         **ppcQ = (WE_CHAR **)ppvCertFriendlyNames;
    WE_CHAR         *pcName = NULL;
    
    if (NULL == hSecHandle) 
    {
        return;
    }
    if ((iNumOfCerts>0) && (ppvCertFriendlyNames!=NULL) && (pusFriendlyNameLen!=NULL))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"friendlyNameLen[0] = %d", pusFriendlyNameLen[0]));
        if (pusFriendlyNameLen[0] > 0)
        {    
            WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0, "certFriendlyNames[0] =\n"));
            WE_LOG_DATA(WE_LOG_DETAIL_MEDIUM, 0, ppvCertFriendlyNames[0], pusFriendlyNameLen[0]);
        }
    }
    else
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0, "certFriendlyNames[0] = NULL"));
    }
    if ((iNumOfCerts > 0) && (NULL!=psCharSet) && (NULL!=pcNameType) && (NULL!=piCertIds) && (NULL!=pucCertType))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
                    "charSet[0] = %d, nameType[0] = %d, certIds[0] = %d\n",\
                    psCharSet[0], pcNameType[0], piCertIds[0]));
    }
    else
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
                    "charSet[0] = 0, nameType[0] = 0, certIds[0] = 0\n"));
    }

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    pstValue = (St_SecTlCertNamesResp *)WE_MALLOC(sizeof(St_SecTlCertNamesResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    (WE_VOID)WE_MEMSET(pstValue, 0, sizeof(St_SecTlCertNamesResp));

    pstValue->eType = E_SEC_DP_GET_CERT_NAMES; 
    if ((pstValue->iResult = iResult) == M_SEC_ERR_WIM_NOT_INITIALISED)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
        return;
    }
    if (0 == iNumOfCerts)
    { 
        pstValue->iNbrOfCerts = iNumOfCerts;
        pstValue->pstCertNames = NULL;
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
        return;
    }
    if((pstValue->iResult != M_SEC_ERR_OK) && (NULL == piCertIds))
    {
        WE_FREE(pstValue);
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->pstCertNames = (St_SecCertName *)WE_MALLOC((WE_UINT32)iNumOfCerts * sizeof(St_SecCertName));
    if (NULL == pstValue->pstCertNames)
    {
        WE_FREE(pstValue);
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }

    if ((NULL!=pusFriendlyNameLen) && (NULL!=piCertIds) &&
        (NULL!=psCharSet) && (NULL!=pcNameType) && (NULL!=pucCertType) && ( NULL != pucAble)&&(NULL!=ppcQ))
    {
        for (iIndex=0; iIndex<iNumOfCerts; iIndex++)
        {
            pstValue->pstCertNames[iIndex].pvFriendlyName = (WE_CHAR *)\
                                WE_MALLOC((pusFriendlyNameLen[iIndex] + 1));
            if (NULL == (pstValue->pstCertNames[iIndex].pvFriendlyName))
            {
                for (iIndexj=0; iIndexj<iIndex; iIndexj++)
                {
                    WE_FREE(pstValue->pstCertNames[iIndexj].pvFriendlyName);
                }           
                WE_FREE(pstValue->pstCertNames);           
                WE_FREE(pstValue);
                M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
                return;     
            }

            pcName = pstValue->pstCertNames[iIndex].pvFriendlyName;
            for (iIndexj=0; iIndexj<pusFriendlyNameLen[iIndex]; iIndexj++)
            {
                pcName[iIndexj] = ppcQ[iIndex][iIndexj];
            }
            pcName[iIndexj] = '\0';
            pstValue->pstCertNames[iIndex].usFriendlyNameLen = pusFriendlyNameLen[iIndex];
            pstValue->pstCertNames[iIndex].iCertId = piCertIds[iIndex];
            pstValue->pstCertNames[iIndex].sCharSet = psCharSet[iIndex];
            pstValue->pstCertNames[iIndex].cNameType = pcNameType[iIndex];
            pstValue->pstCertNames[iIndex].ucAble= pucAble[iIndex];
            pstValue->pstCertNames[iIndex].ucCertType= pucCertType[iIndex];
        }
    }
    pstValue->iNbrOfCerts = iNumOfCerts;
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimInitializeResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        iResult[IN]: result of the calling function.    
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimInitializeResponse (WE_HANDLE   hSecHandle, 
                                WE_INT32 iTargetID, WE_INT32 iResult)
{
    St_SecTlInitWimResp   *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }    
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, 0,
                "Sec_WimInitializeResponse(iTargetID = %d, iResult = %d)\n", iTargetID, iResult));   
    if (!(M_SEC_WAITING_FOR_WIM_RESULT))                
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlInitWimResp *)WE_MALLOC(sizeof(St_SecTlInitWimResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->eType = E_SEC_DP_INIT;
    pstValue->iTargetID = iTargetID;
    pstValue->iResult = iResult;
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimWtlsGetCipherSuiteResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        omit.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimWtlsGetCipherSuiteResponse (WE_HANDLE hSecHandle, 
                                WE_INT32 iTargetID, WE_INT32 iResult,
                                const WE_UCHAR * pucCipherMethods, WE_INT32 iCipherMethodsLen,
                                const WE_UCHAR *pucKeyExchangeIds, WE_INT32 iKeyExchangeIdsLen,
                                const WE_UCHAR *pucTrustedKeyIds, WE_INT32 iTrustedKeyIdsLen, 
                                WE_INT32  iTooManyCerts)
{
    WE_INT32  iIndex = 0;
    St_SecTlGetWtlsMethodsResp *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
        "Sec_WimWtlsGetCipherSuiteResponse(iTargetID = %d, iResult = %d, cipherMethodsLen = %d, keyExchangeIdsLen = %d, trustedKeyIdsLen = %d, too_many_certs = %d)\n",\
        iTargetID, iResult,  iCipherMethodsLen, iKeyExchangeIdsLen, iTrustedKeyIdsLen, iTooManyCerts));
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, 0,"cipherMethods =\n"));
    WE_LOG_DATA (WE_LOG_DETAIL_MEDIUM, 0, pucCipherMethods, iCipherMethodsLen);
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, 0,"keyExchangeIds =\n"));
    WE_LOG_DATA (WE_LOG_DETAIL_MEDIUM, 0, pucKeyExchangeIds, iKeyExchangeIdsLen);
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, 0,"trustedKeyIds =\n"));
    WE_LOG_DATA (WE_LOG_DETAIL_MEDIUM, 0, pucTrustedKeyIds, iTrustedKeyIdsLen);

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))                
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlGetWtlsMethodsResp *)WE_MALLOC(sizeof(St_SecTlGetWtlsMethodsResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    (WE_VOID)WE_MEMSET(pstValue, 0, sizeof(St_SecTlGetWtlsMethodsResp));
    
    pstValue->eType = E_SEC_DP_GET_CIPHERSUITE;
    if ((pstValue->iResult = iResult) == M_SEC_ERR_WIM_NOT_INITIALISED)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
        return;
    }
    if((pucCipherMethods == NULL) || (pucKeyExchangeIds == NULL))
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        WE_FREE(pstValue);
        return;
    }
    pstValue->pucCipherMethods = (WE_UCHAR *)WE_MALLOC((WE_UINT32)iCipherMethodsLen * sizeof(WE_UCHAR));
    if ((NULL == (pstValue->pucCipherMethods)) && (iCipherMethodsLen != 0))
    {
        WE_FREE(pstValue);
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->pucKeyExchangeIds = (WE_UCHAR *)WE_MALLOC((WE_UINT32)iKeyExchangeIdsLen * sizeof(WE_UCHAR));
    if ((NULL == (pstValue->pucKeyExchangeIds)) && (iKeyExchangeIdsLen != 0))
    {
        WE_FREE(pstValue->pucCipherMethods);
        WE_FREE(pstValue);
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    
    pstValue->iTargetID = iTargetID;
    pstValue->iResult = iResult;
    pstValue->iCipherMethodsLen = iCipherMethodsLen;
    for (iIndex = 0; iIndex < iCipherMethodsLen; iIndex++)
    {
        pstValue->pucCipherMethods[iIndex] = pucCipherMethods[iIndex];
    }
    pstValue->iKeyExchangeIdsLen = iKeyExchangeIdsLen;
    for (iIndex = 0; iIndex < iKeyExchangeIdsLen; iIndex++)
    {
        pstValue->pucKeyExchangeIds[iIndex] = pucKeyExchangeIds[iIndex];
    }
    pstValue->iTrustedKeyIdsLen = iTrustedKeyIdsLen;
    pstValue->iTooManyCerts = iTooManyCerts;
    if ((0 >= pstValue->iTrustedKeyIdsLen)||!pucTrustedKeyIds)
    {
        pstValue->pucTrustedKeyIds = NULL;
    }
    else
    {
        pstValue->pucTrustedKeyIds = (WE_UCHAR *)WE_MALLOC((WE_UINT32)iTrustedKeyIdsLen * sizeof(WE_UCHAR));
        if ((NULL == (pstValue->pucTrustedKeyIds)) && (iTrustedKeyIdsLen != 0))
        {
            WE_FREE(pstValue->pucKeyExchangeIds);
            WE_FREE(pstValue->pucCipherMethods);
            WE_FREE(pstValue);
            M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
            return;
        }
        for (iIndex = 0; iIndex < iTrustedKeyIdsLen; iIndex++)
        {
            pstValue->pucTrustedKeyIds[iIndex] = pucTrustedKeyIds[iIndex];
        }
    }

    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimWtlsKeyExchangeResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        iResult[IN]: result of the calling function.        
        iMasterSecretId[IN]: ID of the master secret.
        pucPublicValue[IN]: public value.
        iPublicValueLen[IN]: length of public value.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimWtlsKeyExchangeResponse (WE_HANDLE   hSecHandle, WE_INT32 iTargetID, 
                                 WE_INT32 iResult, WE_INT32 iMasterSecretId,
                                 const WE_UCHAR* pucPublicValue, WE_INT32 iPublicValueLen)
{
    WE_INT32              iIndex = 0;
    St_SecTlKeyXchResp      *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
      "Sec_WimWtlsKeyExchangeResponse(iTargetID = %d, iResult = %d, masterSecretId = %d, publicValueLen = %d)\n",\
      iTargetID, iResult, iMasterSecretId, iPublicValueLen));
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, 0,"publicValue =\n"));
    WE_LOG_DATA (WE_LOG_DETAIL_MEDIUM, 0, pucPublicValue, iPublicValueLen);

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))                
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlKeyXchResp *)WE_MALLOC(sizeof(St_SecTlKeyXchResp));  
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    (WE_VOID)WE_MEMSET(pstValue, 0, sizeof(St_SecTlKeyXchResp));
    
    pstValue->eType = E_SEC_DP_XCH_KEY;
    pstValue->pucPublicValue = NULL;
    if( (pstValue->iResult = iResult) == M_SEC_ERR_WIM_NOT_INITIALISED)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
        return;
    }
    if((!pucPublicValue) && (0 != iPublicValueLen))/*add by bird for keyexchange NULL 061107*/
    {
        WE_FREE(pstValue);
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    else
    {
        pstValue->pucPublicValue = (WE_UCHAR *)WE_MALLOC((WE_UINT32)iPublicValueLen * sizeof(WE_UCHAR));
        if ((NULL == (pstValue->pucPublicValue)) && (0 != iPublicValueLen))/*add by bird for keyexchange NULL 061107*/
        {
            WE_FREE(pstValue);
            M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
            return;
        }
        pstValue->iTargetID = iTargetID;
        pstValue->iResult = iResult;
        pstValue->iMasterSecretId = iMasterSecretId;
        pstValue->iPublicValueLen = iPublicValueLen;
        if((pucPublicValue != NULL) && (0 != iPublicValueLen))
        {
            for (iIndex = 0; iIndex < iPublicValueLen; iIndex++)
            {
                pstValue->pucPublicValue[iIndex] = pucPublicValue[iIndex];
            }
        }
    }
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimWtlsGetPrfResultResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        iResult[IN]: result of the calling function.    
        pucBuf[IN]: pointer to the data buffer.
        iBufLen[IN]: The length of the buffer.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimWtlsGetPrfResultResponse (WE_HANDLE   hSecHandle, 
                         WE_INT32 iTargetID, WE_INT32 iResult,
                         const WE_UCHAR *pucBuf, WE_INT32 iBufLen)
{
    WE_INT32            iIndex = 0;
    St_SecTlPrfResp     *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
      "Sec_WimWtlsGetPrfResultResponse (iTargetID =%d, iResult = %d, bufLen = %d)\n",\
      iTargetID, iResult, iBufLen));
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, 0,"buf =\n"));
    WE_LOG_DATA (WE_LOG_DETAIL_MEDIUM, 0, pucBuf, iBufLen);

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))                
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlPrfResp *)WE_MALLOC(sizeof(St_SecTlPrfResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    (WE_VOID)WE_MEMSET(pstValue, 0, sizeof(St_SecTlPrfResp));
    
    pstValue->eType = E_SEC_DP_PRF;
    if( (pstValue->iResult = iResult) == M_SEC_ERR_WIM_NOT_INITIALISED)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
        return;
    }
    pstValue->iBufLen = iBufLen;
    if((0 == iBufLen) || (NULL == pucBuf))
    {
        pstValue->pucBuf = NULL;
        pstValue->iBufLen = 0;
    }
    else
    {
        pstValue->pucBuf = (WE_UCHAR *)WE_MALLOC((WE_UINT32)iBufLen * sizeof(WE_UCHAR)+1);
        if (NULL == (pstValue->pucBuf))
        {
            WE_FREE(pstValue);
            M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
            return;
        }
        for (iIndex=0; iIndex<iBufLen; iIndex++)
        {
            pstValue->pucBuf[iIndex] = pucBuf[iIndex];
        }
    }
    pstValue->iTargetID = iTargetID;
    pstValue->iBufLen = iBufLen;
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimViewPrivKeyPinResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        iResult[IN]: result of the calling function.    
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimViewPrivKeyPinResponse(WE_HANDLE hSecHandle, 
                                        WE_INT32 iTargetID, WE_INT32 iResult)
{
    St_SecTlPinResp       *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
           "Sec_WimViewPrivKeyPinResponse(iTargetID = %d, iResult = %d)\n", iTargetID, iResult));

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))                
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlPinResp *)WE_MALLOC(sizeof(St_SecTlPinResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->eType = E_SEC_DP_PIN_INFO;
    pstValue->iTargetID = iTargetID;
    pstValue->iResult = iResult;
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimVerifyPrivKeyPinResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        iResult[IN]: result of the calling function.    
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimVerifyPrivKeyPinResponse(WE_HANDLE hSecHandle, 
                                        WE_INT32 iTargetID, WE_INT32 iResult)
{
    St_SecTlPinResp    *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
           "Sec_WimVerifyPrivKeyPinResponse(iTargetID = %d, iResult = %d)\n", iTargetID, iResult));

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))                
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlPinResp *)WE_MALLOC(sizeof(St_SecTlPinResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->eType = E_SEC_DP_VERIFY_PIN;
    pstValue->iTargetID = iTargetID;
    pstValue->iResult = iResult;
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimModifyPrivKeyPinResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        iResult[IN]: result of the calling function.    
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimModifyPrivKeyPinResponse(WE_HANDLE hSecHandle, 
                                            WE_INT32 iTargetID, WE_INT32 iResult)
{
    St_SecTlPinResp        *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
      "Sec_WimModifyPrivKeyPinResponse(iTargetID = %d, iResult = %d)\n", iResult, iResult));

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))               
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlPinResp *)WE_MALLOC(sizeof(St_SecTlPinResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->eType = E_SEC_DP_CHANGE_PIN;
    pstValue->iTargetID = iTargetID;
    pstValue->iResult = iResult;
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimGetUserCertAndPrivKeyResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iResult[IN]: result of the calling function.
        pucCert[IN]: certificate.
        iCertLen[IN]: length of certificate.        
        pucSig[IN]: signature.
        iSigLen[IN]: length of signature.
        ucAlg[IN]: algorithm.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimGetUserCertAndPrivKeyResponse(WE_HANDLE hSecHandle,
                                            WE_INT32 iResult, const WE_UCHAR *pucCert,
                                            WE_UINT16 usCertLen, const WE_UCHAR *pucSig,
                                            WE_INT32 iSigLen, WE_UINT8 ucAlg)
{
    WE_INT32    iIndex = 0;
    St_SecTlGetUcertSignedkeyResp *pstValue = NULL;    
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
       "Sec_WimGetUserCertAndPrivKeyResponse(iResult = %d, certLen = %d, sigLen = %d, alg = %u)\n",\
       iResult, usCertLen, iSigLen, ucAlg));
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"cert =\n"));
    WE_LOG_DATA(WE_LOG_DETAIL_MEDIUM, 0, pucCert, usCertLen);
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"sig =\n"));
    WE_LOG_DATA(WE_LOG_DETAIL_MEDIUM, 0, pucSig, iSigLen);

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))                
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlGetUcertSignedkeyResp *)WE_MALLOC(sizeof(St_SecTlGetUcertSignedkeyResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    (WE_VOID)WE_MEMSET(pstValue, 0, sizeof(St_SecTlGetUcertSignedkeyResp));
    
    pstValue->eType = E_SEC_DP_GET_UCERT_AND_SKEY;
    if( (pstValue->iResult = iResult) == M_SEC_ERR_WIM_NOT_INITIALISED)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
        return;
    }
    if ((pstValue->iResult!=M_SEC_ERR_OK) || (NULL==pucCert) || 
        (0==usCertLen) || (NULL==pucSig)|| (0==iSigLen))
    {
        WE_FREE(pstValue);
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->pucCert = (WE_UCHAR *)WE_MALLOC((usCertLen)*sizeof(WE_UCHAR));
    if (NULL == (pstValue->pucCert))
    {
        WE_FREE(pstValue);
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->usCertLen = usCertLen;
    for (iIndex=0; iIndex<usCertLen; iIndex++)
    {
        pstValue->pucCert[iIndex] = pucCert[iIndex];
    }
    pstValue->pucSig = (WE_UCHAR *)WE_MALLOC(((WE_UINT32)iSigLen + 1) * sizeof(WE_UCHAR));
    if (NULL == (pstValue->pucSig))
    {
        WE_FREE(pstValue->pucCert);
        WE_FREE(pstValue);
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->iSigLen = iSigLen;
    for (iIndex=0; iIndex<iSigLen; iIndex++)
    {
        pstValue->pucSig[iIndex] = pucSig[iIndex];
    }
    pstValue->pucSig[iIndex] = '\0';
    pstValue->ucAlg = ucAlg;
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimViewNameOfUserCertByTypeResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        omit.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimViewNameOfUserCertByTypeResponse(WE_HANDLE hSecHandle,
                                        WE_INT32 iResult, WE_INT32 iNumOfCerts, WE_UINT8 *pucCertType, 
                                        const WE_VOID* const* ppvCertFriendlyNames,
                                        WE_UINT16 *pusFriendlyNameLen, WE_INT16 *psCharSet,
                                        WE_INT8 *pcNameType, const WE_INT32 *piCertIds,
                                        WE_UCHAR *pucAble)
{    
    if (NULL == hSecHandle)
    {
        return;
    }
    
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimViewNameOfUserCertByTypeResponse(iResult = %d, iNumOfCerts = %d)\n",
              iResult, iNumOfCerts));

    Sec_WimCertNamesResponse(hSecHandle, iResult, iNumOfCerts, pucCertType,
                            ppvCertFriendlyNames, pusFriendlyNameLen, 
                            psCharSet, pcNameType, piCertIds, pucAble);
}

/*=====================================================================================
FUNCTION: 
        Sec_WimViewNameOfUserCertResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        omit.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimViewNameOfUserCertResponse(WE_HANDLE hSecHandle, WE_INT32 iResult, 
                                     WE_INT32 iNumOfCerts,
                                     const WE_VOID *const *ppvCertFriendlyNames, 
                                     WE_UINT16 *pusFriendlyNameLen, WE_INT16 *psCharSet, 
                                     WE_INT8 *pcNameType, const WE_INT32 *piCertIds,
                                     WE_INT8 *pcChainDepth, WE_UINT32 **ppuiCertChainIds)
{
    St_SecTlConnCertNamesResp *pstValue = NULL;
    WE_INT32         iIndex = 0;
    WE_INT32         iIndexj = 0;
    WE_CHAR          **ppcQ = (WE_CHAR **)ppvCertFriendlyNames;
    WE_CHAR          *pcName = NULL;
    
    if (NULL == hSecHandle) 
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
      "Sec_WimViewNameOfUserCertResponse(iResult = %d, iNumOfCerts = %d)\n",\
      iResult, iNumOfCerts));

    if ((iNumOfCerts>0) && (ppvCertFriendlyNames!=NULL) && (pusFriendlyNameLen!=NULL))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"friendlyNameLen[0] = %d", pusFriendlyNameLen[0]));
        if (pusFriendlyNameLen[0] > 0)
        {     
            WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
                        "ppvCertFriendlyNames[0] =\n"));
            WE_LOG_DATA(WE_LOG_DETAIL_MEDIUM, 0,
                        ppvCertFriendlyNames[0], pusFriendlyNameLen[0]);
        }
    }
    else
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
                    "certFriendlyNames[0] = NULL"));
    }
    if ((iNumOfCerts>0) && (NULL!=psCharSet) && (NULL!=pcNameType) && (NULL!=piCertIds))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
                    "charSet[0] = %d, nameType[0] = %d, certIds[0] = %d\n",
                    psCharSet[0], pcNameType[0], piCertIds[0]));
    }
    else
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
                    "charSet[0] = 0, nameType[0] = 0, certIds[0] = 0\n"));
    }

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlConnCertNamesResp *)WE_MALLOC(sizeof(St_SecTlConnCertNamesResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    (WE_VOID)WE_MEMSET(pstValue, 0, sizeof(St_SecTlConnCertNamesResp));

    pstValue->eType = E_SEC_DP_GET_CONN_CERT_NAMES; 
    if ((pstValue->iResult = iResult) == M_SEC_ERR_WIM_NOT_INITIALISED)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
        return;
    }
    if (0 == iNumOfCerts)
    { 
        pstValue->iNbrOfCerts = iNumOfCerts;
        pstValue->pstCertNames = NULL;
        pstValue->pcChainDepth = NULL;
        pstValue->ppuiCertChainIds = NULL;
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
        return;
    }
    if((pstValue->iResult != M_SEC_ERR_OK) && (NULL == piCertIds))
    {
        WE_FREE(pstValue);
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->pstCertNames = (St_SecCertName *)WE_MALLOC((WE_UINT32)iNumOfCerts * sizeof(St_SecCertName));
    if (NULL == pstValue->pstCertNames)
    {
        WE_FREE(pstValue);
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    if ((pcChainDepth!=NULL) && (ppuiCertChainIds!=NULL))
    {
        pstValue->ppuiCertChainIds = (WE_UINT32 **)WE_MALLOC((WE_UINT32)iNumOfCerts * sizeof(WE_UINT32 *));
        pstValue->pcChainDepth = (WE_INT8 *)WE_MALLOC((WE_UINT32)iNumOfCerts * sizeof(WE_INT8));
        if ((NULL == pstValue->ppuiCertChainIds ) || (NULL == pstValue->pcChainDepth))
        {
            if (NULL != pstValue->ppuiCertChainIds)
            {
                WE_FREE(pstValue->ppuiCertChainIds);
            }        
            if (NULL != pstValue->pcChainDepth )
            {
                WE_FREE(pstValue->pcChainDepth);
            }
            WE_FREE(pstValue->pstCertNames);
            WE_FREE(pstValue);
            M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
            return;       
        }
    }
    else
    {
        pstValue->pcChainDepth = NULL;
        pstValue->ppuiCertChainIds = NULL;
    }

    if ((pusFriendlyNameLen!=NULL) && (piCertIds!=NULL) && 
        (psCharSet!=NULL) && (pcNameType!=NULL) && (ppcQ!=NULL))
    {
        for (iIndex=0; iIndex<iNumOfCerts; iIndex++)
        {
            pstValue->pstCertNames[iIndex].pvFriendlyName =
                        (WE_CHAR *)WE_MALLOC((pusFriendlyNameLen[iIndex] + 1));
            if (NULL == pstValue->pstCertNames[iIndex].pvFriendlyName)
            {
                for (iIndexj=0; iIndexj<iIndex; iIndexj++)
                {
                    WE_FREE(pstValue->pstCertNames[iIndexj].pvFriendlyName);  
                }                       
                if (NULL != pstValue->ppuiCertChainIds)
                {
                    WE_FREE(pstValue->ppuiCertChainIds);  
                }
                if (NULL != pstValue->pcChainDepth)
                {
                    WE_FREE(pstValue->pcChainDepth);
                }
                WE_FREE(pstValue->pstCertNames);
                WE_FREE(pstValue);
                M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
                return;      
            }
            pcName = pstValue->pstCertNames[iIndex].pvFriendlyName;
            for (iIndexj=0; iIndexj<pusFriendlyNameLen[iIndex]; iIndexj++)
            {
                pcName[iIndexj] = ppcQ[iIndex][iIndexj];
            }
            pcName[iIndexj] = '\0';                
            pstValue->pstCertNames[iIndex].usFriendlyNameLen = pusFriendlyNameLen[iIndex];
            pstValue->pstCertNames[iIndex].iCertId = piCertIds[iIndex];
            pstValue->pstCertNames[iIndex].sCharSet = psCharSet[iIndex];
            pstValue->pstCertNames[iIndex].cNameType = pcNameType[iIndex];
            if ((pcChainDepth != NULL) && (ppuiCertChainIds != NULL)
                && (pstValue->pcChainDepth != NULL))
            {
                pstValue->pcChainDepth[iIndex] = pcChainDepth[iIndex];
                if ((pcChainDepth[iIndex] != 0) && (pstValue->ppuiCertChainIds != NULL))
                {
                    pstValue->ppuiCertChainIds[iIndex] = (WE_UINT32 *)\
                         WE_MALLOC((WE_UINT8)(pcChainDepth[iIndex]) * sizeof(WE_UINT32));
                    if ((NULL == pstValue->ppuiCertChainIds[iIndex]) && (pcChainDepth[iIndex] != 0))
                    {
                        for (iIndexj=0; iIndexj<=iIndex; iIndexj++)
                        {
                            WE_FREE(pstValue->pstCertNames[iIndexj].pvFriendlyName);  
                        }        
                        if (NULL != pstValue->ppuiCertChainIds)
                        {
                            WE_FREE(pstValue->ppuiCertChainIds);  
                        }
                        if (NULL != pstValue->pcChainDepth)
                        {
                            WE_FREE(pstValue->pcChainDepth);
                        }
                        WE_FREE(pstValue->pstCertNames);
                        WE_FREE(pstValue);
                        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
                        return;      
                    } 
                    for (iIndexj=0; iIndexj<pcChainDepth[iIndex]; iIndexj++)
                    {
                        pstValue->ppuiCertChainIds[iIndex][iIndexj] = ppuiCertChainIds[iIndex][iIndexj];
                    }
                }
            }
        }
    }
    pstValue->iNbrOfCerts = iNumOfCerts;
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimGetUserCertAndKeyIdResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN]: Global data handle.
        iResult[IN]: result of the calling function.
        pucCert[IN]: certificate.
        iCertLen[IN]: length of certificate.
        pucKeyId[IN]: Id of public key.
        iKeyIdLen[IN]: length of public key.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimGetUserCertAndKeyIdResponse(WE_HANDLE hSecHandle, WE_INT32 iResult,
                                        WE_UCHAR *pucCert, WE_INT32 iCertLen, 
                                        WE_UCHAR *pucKeyId, WE_INT32 iKeyIdLen)
{
    WE_INT32  iIndex = 0;
    St_SecTlGetUcertKeyIdResp *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimGetUserCertAndKeyIdResponse(iResult = %d, certLen = %d, keyIdLen = %d)\n",\
              iResult, iCertLen, iKeyIdLen));
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"cert =\n"));
    WE_LOG_DATA(WE_LOG_DETAIL_MEDIUM, 0, pucCert, iCertLen);
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"keyId =\n"));
    WE_LOG_DATA(WE_LOG_DETAIL_MEDIUM, 0, pucKeyId, iKeyIdLen);

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))                
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlGetUcertKeyIdResp *)WE_MALLOC(sizeof(St_SecTlGetUcertKeyIdResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    (WE_VOID)WE_MEMSET(pstValue, 0, sizeof(St_SecTlGetUcertKeyIdResp));
        
    pstValue->eType = E_SEC_DP_GET_USER_CERT;
    if ( (pstValue->iResult = iResult) == M_SEC_ERR_WIM_NOT_INITIALISED)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
        return;
    }
    if((NULL == pucCert) || (NULL == pucKeyId)) 
    {
        WE_FREE(pstValue);
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->pucCert = (WE_UCHAR *)WE_MALLOC((WE_UINT32)iCertLen * sizeof(WE_UCHAR));
    if ((NULL == (pstValue->pucCert)) && (iCertLen != 0))
    {
        WE_FREE(pstValue);
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->iCertLen = iCertLen;
    for (iIndex=0; iIndex<iCertLen; iIndex++)
    {
        pstValue->pucCert[iIndex] = pucCert[iIndex];
    }
    pstValue->pucKeyId = (WE_UCHAR *)WE_MALLOC((WE_UINT32)(iKeyIdLen) * sizeof(WE_UCHAR)); 
    if ((NULL == (pstValue->pucKeyId)) && (iKeyIdLen != 0))
    {
        WE_FREE(pstValue->pucCert); 
        WE_FREE(pstValue);
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    
    for (iIndex=0; iIndex<iKeyIdLen; iIndex++)
    {        
        pstValue->pucKeyId[iIndex] = pucKeyId[iIndex];
    }
    pstValue->iKeyIdLen= iKeyIdLen;    
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimKeyPairGenResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        iResult[IN]: result of the calling function.    
        ucKeyType[IN]: type of key.
        pucPublicKey[IN]: public key.
        iPublicKeyLen[IN]: length of public key.
        pucSig[IN]: signature.
        iSigLen[IN]: length of signature.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimKeyPairGenResponse(WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                                    WE_INT32 iResult, WE_UINT8 ucKeyType, 
                                    const WE_UCHAR* pucPublicKey, WE_INT32 iPublicKeyLen, 
                                    const WE_UCHAR *pucSig, WE_INT32 iSigLen)
{
    WE_INT32   iIndex = 0;
    St_SecTlGenRsaKeypairResp *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimKeyPairGenResponse(iTargetID = %d, iResult = %d, keyType = %u, publicKeyLen = %d, sigLen %d)\n",\
              iTargetID, iResult, ucKeyType, iPublicKeyLen, iSigLen));
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, 0,"publicKey =\n"));
    WE_LOG_DATA (WE_LOG_DETAIL_MEDIUM, 0, pucPublicKey, iPublicKeyLen);
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, 0,"sig =\n"));
    WE_LOG_DATA (WE_LOG_DETAIL_MEDIUM, 0, pucSig, iSigLen);

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))    
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlGenRsaKeypairResp *)WE_MALLOC(sizeof(St_SecTlGenRsaKeypairResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    (WE_VOID)WE_MEMSET(pstValue, 0, sizeof(St_SecTlGenRsaKeypairResp));
    
    pstValue->eType = E_SEC_DP_GEN_KEY_PAIR;
    pstValue->iTargetID= iTargetID;
    pstValue->iResult=iResult;
    pstValue->ucKeyType =ucKeyType;
    if(((NULL == pucPublicKey) || (NULL == pucSig)) && (iResult == M_SEC_ERR_OK))
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
        return;
    }
    
    if (NULL == pucPublicKey)
    {
        pstValue->pucPublicKey = NULL;
        pstValue->iPublicKeyLen = 0;
    }
    else
    {
        pstValue->pucPublicKey = (WE_UCHAR *)WE_MALLOC((WE_UINT32)iPublicKeyLen * sizeof(WE_UCHAR));
        if ((NULL == (pstValue->pucPublicKey)) && (iPublicKeyLen != 0))
        {
            WE_FREE(pstValue);
            M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
            return;
        }
        for (iIndex=0; iIndex<iPublicKeyLen; iIndex++)
        {
            pstValue->pucPublicKey[iIndex] = pucPublicKey[iIndex];
        }
        pstValue->iPublicKeyLen= iPublicKeyLen;
    }
    
    if (NULL == pucSig)
    {
        pstValue->pucSig = NULL;
        pstValue->iSigLen = 0;
    }
    else
    {
        pstValue->pucSig= (WE_UCHAR *)WE_MALLOC((WE_UINT32)iSigLen * sizeof(WE_UCHAR));
        if ((NULL == (pstValue->pucSig)) && (iSigLen != 0))
        {
          if ((pstValue->pucPublicKey) != NULL)          
          {
              WE_FREE(pstValue->pucPublicKey);           
          }
          WE_FREE(pstValue);
          M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
          return;
        }
        for (iIndex=0; iIndex<iSigLen; iIndex++)
        {
            pstValue->pucSig[iIndex] = pucSig[iIndex];
        }
        pstValue->iSigLen= iSigLen;
    }
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimDelPrivKeysAndUserCertsByTypeResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        iResult[IN]: result of the calling function.    
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimDelPrivKeysAndUserCertsByTypeResponse(WE_HANDLE   hSecHandle,
                                                WE_INT32 iTargetID, WE_INT32 iResult)
{
    St_SecTlRmKeysAndUcertsResp *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimDelPrivKeysAndUserCertsByTypeResponse(iTargetID = %d, iResult = %d)\n",\
              iTargetID, iResult));
    if (!(M_SEC_WAITING_FOR_WIM_RESULT))                
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlRmKeysAndUcertsResp *)WE_MALLOC(sizeof(St_SecTlRmKeysAndUcertsResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->eType = E_SEC_DP_RM_KEYS_AND_CERTS;
    pstValue->iTargetID = iTargetID;
    pstValue->iResult = iResult;
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimVerifyCertChainResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        iResult[IN]: result of the calling function.    
        usKeyExchKeyLen[IN]: length of exchanged key.
        iRootCertIsOld[IN]: sign of root certificate is old or not.
        iRootCertIsOldId[IN]: Id of root certificate which is old.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimVerifyCertChainResponse(WE_HANDLE   hSecHandle, WE_INT32 iTargetID, 
                                           WE_INT32 iResult, WE_UINT16 usKeyExchKeyLen,
                                           WE_INT32 iCertTooOld, WE_INT32 iCertTooOldId)
{
    St_SecTlVerifyCertChainResp *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimVerifyCertChainResponse(iTargetID = %d, iResult = %d, keyExchKeyLen = %d, certTooOld = %d, certTooOldId = %d)\n",\
              iTargetID, iResult, usKeyExchKeyLen, iCertTooOld, iCertTooOldId));

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))          
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlVerifyCertChainResp *)WE_MALLOC(sizeof(St_SecTlVerifyCertChainResp));   
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->eType = E_SEC_DP_VERIFY_CERT_CHAIN;
    pstValue->iTargetID = iTargetID;
    pstValue->iResult = iResult;
    pstValue->usKeyExchKeyLen = usKeyExchKeyLen;
    pstValue->iCertTooOld = iCertTooOld;
    pstValue->iCertTooOldId = iCertTooOldId;
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimVerifySignatureResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        iResult[IN]: result of the calling function.    
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimVerifySignatureResponse(WE_HANDLE hSecHandle, 
                                                WE_INT32 iTargetID, WE_INT32 iResult)
{
    St_SecTlVerifySignResp *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimVerifySignatureResponse(iTargetID = %d, iResult = %d)\n",\
              iTargetID, iResult));

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))         
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlVerifySignResp *)WE_MALLOC(sizeof(St_SecTlVerifySignResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->eType = E_SEC_DP_VRRIFY_SIGN;
    pstValue->iTargetID = iTargetID;
    pstValue->iResult = iResult;    
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimWtlsVerifyUserCertChainResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        iResult[IN]: result of the calling function.
        ucAlg[IN]: algorithm of user certificate.
        pucCaCert[IN]: CA certificate.
        usCaCertLen[IN]: length of CA certificate.
        iRootCertIsOld[IN]: sign of root certificate is old or not.
        iRootCertIsOldId[IN]: Id of root certificate which is old.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimWtlsVerifyUserCertChainResponse(WE_HANDLE hSecHandle, 
                                            WE_INT32 iTargetID, WE_INT32 iResult, WE_UINT8 ucAlg,
                                            WE_UCHAR *pucCaCert, WE_UINT16 usCaCertLen, 
                                            WE_INT32 iRootCertTooOld, WE_INT32 iRootCertTooOldId)
{
    St_SecTlVerifyUserCertResp    *pstValue = NULL;
    WE_INT32  iIndex = 0;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimWtlsVerifyUserCertChainResponse(iTargetID = %d, iResult = %d, alg = %u, rootCertTooOld = %d, rootCertTooOldId = %d, caCertLen = %u)\n",\
              iTargetID, iResult, ucAlg, iRootCertTooOld, iRootCertTooOldId, usCaCertLen));
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, 0,"caCert =\n"));
    if (pucCaCert != NULL)
    {
        WE_LOG_DATA (WE_LOG_DETAIL_MEDIUM, 0, pucCaCert, usCaCertLen);
    }

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))  
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    pstValue = (St_SecTlVerifyUserCertResp *)WE_MALLOC(sizeof(St_SecTlVerifyUserCertResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    (WE_VOID)WE_MEMSET(pstValue, 0, sizeof(St_SecTlVerifyUserCertResp));
    
    if ((iResult != M_SEC_ERR_OK) || (0 == usCaCertLen) || (NULL == pucCaCert))
    {
        pstValue->pucCaCert = NULL;
    }
    else 
    {
        pstValue->pucCaCert = (WE_UCHAR *)WE_MALLOC(usCaCertLen * sizeof(WE_UCHAR));
        if (NULL == (pstValue->pucCaCert))
        {
            WE_FREE(pstValue);
            M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
            return;
        }
        for (iIndex=0; iIndex<usCaCertLen; iIndex++)
        {
            pstValue->pucCaCert[iIndex] = pucCaCert[iIndex];
        }
    }
    pstValue->eType = E_SEC_DP_VERIRY_USER_CERT_CHAIN;
    pstValue->iTargetID = iTargetID;
    pstValue->iResult = iResult;
    pstValue->ucAlg = ucAlg;
    pstValue->usCaCertLen = usCaCertLen;
    pstValue->iRootCertTooOld = iRootCertTooOld;
    pstValue->iRootCertTooOldId = iRootCertTooOldId;
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimVerifyRootCertResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        iResult[IN]: result of the calling function.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimVerifyRootCertResponse(WE_HANDLE hSecHandle,
                                                    WE_INT32 iTargetID, WE_INT32 iResult)
{
    St_SecTlVerifyRootCertresp  *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimVerifyRootCertResponse(iTargetID = %d, iResult = %d)\n", iTargetID, iResult));
    
    if (!(M_SEC_WAITING_FOR_WIM_RESULT))  
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlVerifyRootCertresp *)WE_MALLOC(sizeof(St_SecTlVerifyRootCertresp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->eType = E_SEC_DP_VERIRY_ROOT_CERT;
    pstValue->iTargetID = iTargetID;
    pstValue->iResult = iResult;  
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimSelfSignedCertResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        iResult[IN]: result of the calling function.
        ucSelfsigned[IN]: 1 represent the certificate is signed; 0 represent not.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimSelfSignedCertResponse(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                     WE_INT32 iResult, WE_UINT8 ucSelfsigned)
{
    St_SecTlSelfSignedCertResp *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }    
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
               "Sec_WimSelfSignedCertResponse(iTargetID = %d, iResult = %d, selfsigned = %u)\n",\
               iTargetID, iResult, ucSelfsigned));
    
    if (!(M_SEC_WAITING_FOR_WIM_RESULT))            
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;     
    
    pstValue = (St_SecTlSelfSignedCertResp *)WE_MALLOC(sizeof(St_SecTlSelfSignedCertResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->eType = E_SEC_DP_SELF_SIGNED_CERT; 
    pstValue->iTargetID = iTargetID;
    pstValue->iResult = iResult;
    pstValue->ucSelfsigned = ucSelfsigned;
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue; 
}

/*=====================================================================================
FUNCTION: 
        Sec_WimCertExistsResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        iResult[IN]: result of the calling function.
        ucExists[IN]: 1 represent the certificate exists; 0 represent not.
        uiNewValidStart[IN]: new valid start time.
        uiNewValidEnd[IN]:  new valid end time.
        uiOldValidStart[IN]: old valid start time.
        uiOldValidEnd[IN]: old valid end time.
        iCertId[IN]: Id of certificate.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimCertExistsResponse(WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                                      WE_INT32 iResult, WE_UINT8 ucExists,
                                      WE_UINT32 uiNewValidStart,
                                      WE_UINT32 uiNewValidEnd,
                                      WE_UINT32 uiOldValidStart,
                                      WE_UINT32 uiOldValidEnd,
                                      WE_INT32 iCertId)
{
    St_SecTlCertIsExistResp  *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
        "Sec_WimCertExistsResponse(iTargetID = %d, iResult = %d, exists = %u, new_valid_not_before = %u,\
         new_valid_not_after = %u, old_valid_not_before = %u, old_valid_not_after = %u, iCertId = %d)\n",\
        iTargetID, iResult, ucExists, uiNewValidStart, uiNewValidEnd, \
        uiOldValidStart, uiOldValidEnd, iCertId));

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))  
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlCertIsExistResp *)WE_MALLOC(sizeof(St_SecTlCertIsExistResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->eType = E_SEC_DP_CERT_IS_EXIST; 
    pstValue->iTargetID = iTargetID;
    pstValue->iResult = iResult;
    pstValue->ucExists = ucExists;
    pstValue->iFileId = iCertId;
    pstValue->uiNewValidNotBefore = uiNewValidStart; 
    pstValue->uiNewValidNotAfter = uiNewValidEnd;
    pstValue->uiOldValidNotBefore = uiOldValidStart; 
    pstValue->uiOldValidNotAfter = uiOldValidEnd;    
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimStoreCertResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        iResult[IN]: result of the calling function.
        iCertId[IN]: ID of certificate.
        uiCertCount[IN]: length of certificate.
        iURLCount[IN]: length of certificate's URL.
        iNameCount[IN]: length of certificate's name.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimStoreCertResponse(WE_HANDLE   hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult, 
                              WE_INT32 iCertId, WE_UINT32 uiCertCount, 
                              WE_INT32 iURLCount,WE_INT32 iNameCount)
{
    St_SecTlStoreCertResp *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimStoreCertResponse(iTargetID = %d, iResult = %d, certId = %d, certCount = %u, URLCount = %u, nameCount = %d)\n",\
              iTargetID, iResult, iCertId, uiCertCount, iURLCount, iNameCount));

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))               
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlStoreCertResp *)WE_MALLOC(sizeof(St_SecTlStoreCertResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->eType = E_SEC_DP_STROE_CERT; 
    pstValue->iTargetID = iTargetID;
    pstValue->iResult = iResult;
    pstValue->iCertId = iCertId;
    pstValue->uiCertCount = uiCertCount;
    pstValue->iURLCount = iURLCount;
    pstValue->iNameCount = iNameCount;    
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimStoreUserCertDataResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        iResult[IN]: result of the calling function.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimStoreUserCertDataResponse(WE_HANDLE hSecHandle, 
                                                    WE_INT32 iTargetID, WE_INT32 iResult)
{
    St_SecTlStoreUcertResp  *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimStoreUserCertDataResponse(iTargetID = %d, iResult = %d)\n", iTargetID, iResult));

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))  
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlStoreUcertResp *)WE_MALLOC(sizeof(St_SecTlStoreUcertResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->eType = E_SEC_DP_STORE_USER_CERT_DATA; 
    pstValue->iTargetID = iTargetID;
    pstValue->iResult = iResult;    
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimDelCertResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iCertId[IN]: ID of certificate.
        iResult[IN]: result of the calling function.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimDelCertResponse(WE_HANDLE hSecHandle, WE_INT32 iCertId, 
                                      WE_INT32 iResult)
{
    St_SecTlRmCertResp *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimDelCertResponse(iCertId = %d, iResult = %d)\n",\
              iCertId, iResult));

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))               
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;

    pstValue = (St_SecTlRmCertResp *)WE_MALLOC(sizeof(St_SecTlRmCertResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->eType = E_SEC_DP_RM_CERT; 
    pstValue->iCertId = iCertId;
    pstValue->iResult = iResult;    
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimGetCertResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iResult[IN]: result of the calling function.
        pucCert[IN]: pointer to the certificate.
        usCertLen[IN]: length of certificate.
        pucTrustedUsage[IN]: trust usage of certificate.
        usTrustedUsageLen[IN]: length of trust usage.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimGetCertResponse(WE_HANDLE hSecHandle, WE_INT32 iResult, 
                                   WE_UCHAR *pucCert, WE_UINT16 usCertLen,
                                   WE_UCHAR *pucTrustedUsage, WE_UINT16 usTrustedUsageLen)
{
    WE_INT32            iIndex = 0;
    St_SecTlGetCertResp *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
                            "Sec_WimGetCertResponse(iResult = %d, certLen = %u)\n", iResult, usCertLen));
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"cert =\n"));
    WE_LOG_DATA(WE_LOG_DETAIL_MEDIUM, 0, pucCert, usCertLen);
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"trustedUsage =\n"));
    WE_LOG_DATA(WE_LOG_DETAIL_MEDIUM, 0, pucTrustedUsage, usTrustedUsageLen);

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlGetCertResp *)WE_MALLOC(sizeof(St_SecTlGetCertResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    (WE_VOID)WE_MEMSET(pstValue, 0, sizeof(St_SecTlGetCertResp));
    
    pstValue->eType = E_SEC_DP_GET_CERT;
    if( (pstValue->iResult = iResult) == M_SEC_ERR_WIM_NOT_INITIALISED)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
        return;
    }
    pstValue->usCertLen = usCertLen;
    if((NULL == pucCert)) 
    {
        pstValue->pucCert = NULL;
    }
    else
    {
        pstValue->pucCert = (WE_UCHAR *)WE_MALLOC(usCertLen * sizeof(WE_UCHAR));
        if ((NULL == (pstValue->pucCert)) && (usCertLen != 0))
        {
            WE_FREE(pstValue);
            M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
            return;
        }
        for (iIndex=0; iIndex<usCertLen; iIndex++)
        {
            pstValue->pucCert[iIndex] = pucCert[iIndex];
        }
    }  
    pstValue->usTrustedUsageLen = usTrustedUsageLen;
    if((NULL == pucTrustedUsage)) 
    {
        pstValue->pucTrustedUsage = NULL;
    }
    else
    {
        pstValue->pucTrustedUsage = (WE_UCHAR *)WE_MALLOC(usTrustedUsageLen * sizeof(WE_UCHAR));
        if ((NULL == pstValue->pucTrustedUsage) && (usTrustedUsageLen != 0))
        {
            if (NULL != (pstValue->pucCert))
            {
                WE_FREE(pstValue->pucCert);
            }
            WE_FREE(pstValue);
            M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
            return;           
        }
        (WE_VOID)WE_MEMCPY (pstValue->pucTrustedUsage, pucTrustedUsage, usTrustedUsageLen);
    }  
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimViewNameOfCertResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        omit.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimViewNameOfCertResponse(WE_HANDLE hSecHandle, WE_INT32 iResult, 
                                     WE_INT32 iNumOfCerts, WE_UINT8 *pucCertType, 
                                     const WE_VOID *const *ppvCertFriendlyNames,
                                     WE_UINT16 *pusFriendlyNameLen, WE_INT16* psCharSet, 
                                     WE_INT8 *pcNameType, const WE_INT32 *piCertIds, WE_UCHAR *pucAble)
{    
    if (NULL == hSecHandle)
    {
        return;
    }
    
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
                "Sec_WimViewNameOfCertResponse(iResult = %d, iNumOfCerts = %d\n",
                iResult, iNumOfCerts));

    Sec_WimCertNamesResponse(hSecHandle, iResult, iNumOfCerts, pucCertType,
                          ppvCertFriendlyNames, pusFriendlyNameLen, 
                          psCharSet, pcNameType, piCertIds, pucAble);
}

/*=====================================================================================
FUNCTION: 
        Sec_WimViewIdOfCertResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iResult[IN]: result of the calling function.
        iNumOfCerts[IN]: number of certificates.
        piCertIds[IN]: Id of certificates.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimViewIdOfCertResponse(WE_HANDLE hSecHandle, WE_INT32 iResult,
                                      WE_INT32 iNumOfCerts, const WE_INT32 *piCertIds)
{
    St_SecTlGetCertIdsResp   *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
                "Sec_WimViewIdOfCertResponse(iResult = %d, nbrOfCerts = %d\n",
                iResult, iNumOfCerts));  
    if (!(M_SEC_WAITING_FOR_WIM_RESULT))  
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }    
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlGetCertIdsResp *)WE_MALLOC(sizeof(St_SecTlGetCertIdsResp));    
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }    
    (WE_VOID)WE_MEMSET(pstValue, 0, sizeof(St_SecTlGetCertIdsResp));
    
    pstValue->eType = E_SEC_DP_GET_CERT_IDS;
    pstValue->iNbrOfCerts = iNumOfCerts;
    
    if ((pstValue->iResult = iResult) == M_SEC_ERR_WIM_NOT_INITIALISED)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
        return;
    }    
    if ((0 == iNumOfCerts) || (NULL == piCertIds))
    { 
        pstValue->pucCertificateIds = NULL;
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
        return;
    }
    else
    {
        WE_INT32 iIndex = 0;          
        pstValue->pucCertificateIds = (WE_UINT8 *)WE_MALLOC((WE_UINT32)iNumOfCerts * sizeof(WE_UINT8));
        if (NULL == pstValue->pucCertificateIds)
        {
            WE_FREE(pstValue);
            M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
            return;
        }
        for (iIndex=0; iIndex<iNumOfCerts; iIndex++)
        {
            pstValue->pucCertificateIds[iIndex] = (WE_UINT8)piCertIds[iIndex];
        }       
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
    }
}

/*=====================================================================================
FUNCTION: 
        Sec_WimGetNumOfCertResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        iResult[IN]: result of the calling function.
        iNumOfCerts[IN]: number of certificates.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimGetNumOfCertResponse(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                WE_INT32 iResult,  WE_INT32 iNumOfCerts)
{
    St_SecTlGetNbrCertsResp    *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimGetNumOfCertResponse(iTargetID = %d, iResult = %d, nbrCerts = %d)\n",\
              iTargetID, iResult, iNumOfCerts));
    if (!(M_SEC_WAITING_FOR_WIM_RESULT))                
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlGetNbrCertsResp *)WE_MALLOC(sizeof(St_SecTlGetNbrCertsResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }

    pstValue->eType = E_SEC_DP_GET_NBR_CERTS;
    pstValue->iTargetID = iTargetID;
    pstValue->iNbrCerts = iNumOfCerts;
    if ((pstValue->iResult = iResult) == M_SEC_ERR_WIM_NOT_INITIALISED)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
        return;
    }    
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimReadViewResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iResult[IN]: result of the calling function.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimReadViewResponse(WE_HANDLE hSecHandle, WE_INT32 iResult,
                                      const WE_UCHAR *pucData, WE_INT32 iLen)
{
    WE_INT32         iIndex = 0;
    St_SecTlReadViewsCertsResp *pstValue = NULL;   
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
        "Sec_WimReadViewResponse(iResult = %d, data = %d, len = %u)\n", iResult, pucData,  iLen));
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"data =\n"));
    WE_LOG_DATA(WE_LOG_DETAIL_MEDIUM, 0, pucData, iLen);

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))  
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlReadViewsCertsResp *)WE_MALLOC(sizeof(St_SecTlReadViewsCertsResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    (WE_VOID)WE_MEMSET(pstValue, 0, sizeof(St_SecTlReadViewsCertsResp));
    
    pstValue->eType = E_SEC_DP_READ_VIEWS_CERTS;
    pstValue->iResult = iResult;
    pstValue->iLen = iLen;
    if((NULL == pucData) || (0 == iLen))
    {
        pstValue->pucData = NULL;
    }
    else
    {
        pstValue->pucData = (WE_UCHAR *)WE_MALLOC((WE_UINT32)iLen * sizeof(WE_UCHAR));
        if (NULL == (pstValue->pucData))
        {
            WE_FREE(pstValue);
            M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
            return;
        }
        for (iIndex=0; iIndex<iLen; iIndex++)
        {
            pstValue->pucData[iIndex] = pucData[iIndex];
        }
    }
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimDeletePeerLinkSessionResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iResult[IN]: result of the calling function.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
          omit.
=====================================================================================*/
WE_VOID Sec_WimDeletePeerLinkSessionResponse (WE_HANDLE hSecHandle, WE_INT32 iResult)
{
    St_SecTlRmPeerLinksResp *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"Sec_WimDeletePeerLinkSessionResponse(iResult = %d)\n", iResult));
    if (!(M_SEC_WAITING_FOR_WIM_RESULT)) 
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlRmPeerLinksResp *)WE_MALLOC(sizeof(St_SecTlRmPeerLinksResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }   
    pstValue->eType = E_SEC_DP_RM_PEER_LINKS;
    pstValue->iResult = iResult;    
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimPeerLinkSessionResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iResult[IN]: result of the calling function.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimPeerLinkSessionResponse (WE_HANDLE hSecHandle, WE_INT32 iResult)
{
    St_SecTlLinkPeerSessionResp *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
                "Sec_WimPeerLinkSessionResponse(iResult = %d)\n", iResult));

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))    
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlLinkPeerSessionResp *)WE_MALLOC(sizeof(St_SecTlLinkPeerSessionResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }       
    pstValue->eType = E_SEC_DP_LINK_PEER_SESSION;
    pstValue->iResult = iResult; 
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimPeerSearchResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        iResult[IN]: result of the calling function.
        ucConnectionType[IN]: type of connection.
        iMasterSecretId[IN]: Id of master secret.
        iDeletedIndex[IN]: index.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimPeerSearchResponse (WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                                WE_INT32 iResult, WE_UINT8 ucConnectionType, 
                                WE_INT32 iMasterSecretId, WE_INT32 iDeletedIndex)
{
    St_SecTlLookupPeerResp  *pstValue = NULL;   
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimPeerSearchResponse(iTargetID = %d, iResult = %d, connectionType = %u, masterSecretId = %d, deletedIndex = %d)\n",\
              iTargetID, iResult, ucConnectionType, iMasterSecretId, iDeletedIndex));

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))  
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlLookupPeerResp *)WE_MALLOC(sizeof(St_SecTlLookupPeerResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    
    pstValue->eType = E_SEC_DP_LOOKUP_PEER;
    pstValue->iTargetID = iTargetID;
    pstValue->iResult = iResult;
    pstValue->ucConnectionType = ucConnectionType;
    pstValue->iMasterSecretId = iMasterSecretId;
    pstValue->iDeletedIndex = iDeletedIndex;    
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimSessionActiveResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iResult[IN]: result of the calling function.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimSessionActiveResponse (WE_HANDLE hSecHandle, WE_INT32 iResult)
{
    St_SecTlActiveSessionResp *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimSessionActiveResponse(iResult = %d)\n", iResult));

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))     
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlActiveSessionResp *)WE_MALLOC(sizeof(St_SecTlActiveSessionResp));    
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->eType = E_SEC_DP_ACTIVE_SESSION;
    pstValue->iResult = iResult;
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimSessionInvalidateResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iResult[IN]: result of the calling function.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimSessionInvalidateResponse (WE_HANDLE hSecHandle, WE_INT32 iResult)
{
    St_SecTlInvalidSessionResp     *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimSessionInvalidateResponse(iResult = %d)\n", iResult));

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))         
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlInvalidSessionResp *)WE_MALLOC(sizeof(St_SecTlInvalidSessionResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }  
    pstValue->eType = E_SEC_DP_INVALID_SESSION;
    pstValue->iResult = iResult;
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimSessionCleanResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iResult[IN]: result of the calling function.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimSessionCleanResponse (WE_HANDLE hSecHandle, WE_INT32 iResult)
{
    St_SecTlCleanUpSessionResp *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimSessionCleanResponse(iResult = %d)\n", iResult));

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))  
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlCleanUpSessionResp *)WE_MALLOC(sizeof(St_SecTlCleanUpSessionResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }    
    pstValue->eType = E_SEC_DP_CLEANUP_SESSION;
    pstValue->iResult = iResult;
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimGetSessionMastResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        omit.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimGetSessionMastResponse (WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                WE_INT32 iResult, WE_UINT8 ucSessionOptions, 
                                const WE_UCHAR *pucSessionId, WE_UINT8 ucSessionIdLen,
                                WE_UINT8 aucCipherSuite[2], WE_UINT8 ucCompressionAlg,
                                const WE_UCHAR *pucPrivateKeyId, WE_UINT32 uiCreationTime)
{
    WE_INT32        iIndex = 0;
    St_SecTlFetchSessionResp     *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    if (aucCipherSuite!=NULL) 
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimGetSessionMastResponse(iTargetID = %d, iResult = %d, sessionOptions = %u, sessionIdLen = %u, cipherSuite[2] = %u%u, compressionAlg = %u, creationTime = %u)\n",\
              iTargetID, iResult, ucSessionOptions, ucSessionIdLen, \
              aucCipherSuite[0], aucCipherSuite[1], ucCompressionAlg, uiCreationTime));
    }
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, 0,"sessionId =\n"));
    WE_LOG_DATA (WE_LOG_DETAIL_MEDIUM, 0, pucSessionId, ucSessionIdLen);
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, 0,"privateKeyId =\n"));
    WE_LOG_DATA (WE_LOG_DETAIL_MEDIUM, 0, pucPrivateKeyId, 4);

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))                
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    /* modify by Sam [070122] */
    pstValue = (St_SecTlFetchSessionResp *)WE_MALLOC(sizeof(St_SecTlFetchSessionResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    (WE_VOID)WE_MEMSET(pstValue, 0, sizeof(St_SecTlFetchSessionResp));
    pstValue->eType = E_SEC_DP_FETCH_SESSION;
    if (M_SEC_ERR_OK != iResult)
    {
        pstValue->iResult = iResult;
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
        return;
    }
    
    if((NULL == pucSessionId)) 
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    
    pstValue->pucSessionId = (WE_UCHAR *)WE_MALLOC(ucSessionIdLen * sizeof(WE_UCHAR));
    if ((NULL == (pstValue->pucSessionId)) && (ucSessionIdLen != 0))
    {
        WE_FREE(pstValue);
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }    
    if (!pucPrivateKeyId)   
    {
        pstValue->pucPrivateKeyId = NULL;
    }
    else
    {
        pstValue->pucPrivateKeyId = (WE_UCHAR *)WE_MALLOC(4 * sizeof(WE_UCHAR));
        if (NULL == (pstValue->pucPrivateKeyId))
        {
            WE_FREE(pstValue->pucSessionId);
            WE_FREE(pstValue);
            M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
            return;
        }
    }
    pstValue->iTargetID = iTargetID;
    pstValue->iResult = iResult;
    pstValue->ucSessionOptions = ucSessionOptions;
    pstValue->ucSessionIdLen = ucSessionIdLen;
    for (iIndex=0; iIndex<ucSessionIdLen; iIndex++)
    {
        pstValue->pucSessionId[iIndex] = pucSessionId[iIndex];
    }
    if ((aucCipherSuite!=NULL) && (pucPrivateKeyId!=NULL))
    {
        pstValue->aucCipherSuite[0] = aucCipherSuite[0];
        pstValue->aucCipherSuite[1] = aucCipherSuite[1];
        pstValue->ucCompressionAlg = ucCompressionAlg;
        for (iIndex=0; iIndex<4; iIndex++)
        {
            pstValue->pucPrivateKeyId[iIndex] = pucPrivateKeyId[iIndex];
        }
    }
    pstValue->uiCreationTime = uiCreationTime;
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimSessionUpdateResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iResult[IN]: result of the calling function.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimSessionUpdateResponse (WE_HANDLE hSecHandle, WE_INT32 iResult)
{
    St_SecTlUpdateSessionResp *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimSessionUpdateResponse(iResult = %d)\n", iResult));

    if (!(M_SEC_WAITING_FOR_WIM_RESULT)) 
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlUpdateSessionResp *)WE_MALLOC(sizeof(St_SecTlUpdateSessionResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->eType = E_SEC_DP_UPDATE_SESSION;
    pstValue->iResult = iResult;
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimWtlsComputeSignatureResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        iResult[IN]: result of the calling function.
        pucSig[IN]: signature.
        iSigLen[IN]: length of signature.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimWtlsComputeSignatureResponse(WE_HANDLE hSecHandle,
                                                    WE_INT32 iTargetID, WE_INT32 iResult,
                                                    const WE_UCHAR *pucSig, WE_INT32 iSigLen)
{
    WE_INT32         iIndex = 0;
    St_SecTlComputeSignResp *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimWtlsComputeSignatureResponse(iTargetID = %d, iResult = %d, sigLen = %d)\n",\
              iTargetID, iResult, iSigLen));
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, 0,"sig =\n"));
    WE_LOG_DATA (WE_LOG_DETAIL_MEDIUM, 0, pucSig, iSigLen);

    if (!(M_SEC_WAITING_FOR_WIM_RESULT)) 
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlComputeSignResp *)WE_MALLOC(sizeof(St_SecTlComputeSignResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }   
    (WE_VOID)WE_MEMSET(pstValue, 0, sizeof(St_SecTlComputeSignResp));
    
    pstValue->pucSig = (WE_UCHAR *)WE_MALLOC((WE_UINT32)iSigLen * sizeof(WE_UCHAR));
    if ((NULL == (pstValue->pucSig)) && (iSigLen != 0))
    {
        WE_FREE(pstValue);
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }   
    pstValue->eType = E_SEC_DP_COMPUTE_SIGN;
    pstValue->iTargetID = iTargetID;
    pstValue->iResult = iResult;
    pstValue->iSigLen =iSigLen;
    if (pucSig != NULL)
    {
        for (iIndex=0; iIndex<iSigLen; iIndex++)
        {
            pstValue->pucSig[iIndex] = pucSig[iIndex];
        }    
    }
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimGetMatchedPrivKeyResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        iResult[IN]: result of the calling function.
        uiKeyId[IN]: Id of private key.
        iKeyCount[IN]: length of private key.
        ucKeyUsage[IN]: usage of private key.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimGetMatchedPrivKeyResponse(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                           WE_INT32 iResult, WE_UINT32 uiKeyId, 
                                           WE_INT32 iKeyCount, WE_UINT8 ucKeyUsage)
{
    St_SecTlFindPrivkeyResp *pstValue = NULL;  
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimGetMatchedPrivKeyResponse(iTargetID = %d, iResult = %d, keyId = %d, keyCount = %d, keyUsage = %u)\n",\
              iTargetID, iResult, uiKeyId, iKeyCount, ucKeyUsage));   

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))         
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlFindPrivkeyResp *)WE_MALLOC(sizeof(St_SecTlFindPrivkeyResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }    
    pstValue->eType = E_SEC_DP_LOOKUP_PRIV_KEY;
    pstValue->iTargetID = iTargetID;
    pstValue->iResult = iResult;
    pstValue->uiKeyId = 1;
    uiKeyId = uiKeyId;
    pstValue->iKeyCount = iKeyCount;
    pstValue->ucKeyUsage = ucKeyUsage;
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimSignTextResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimSignTextResponse(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iSignId,
                       WE_INT32 iAlg, const WE_CHAR *pcSign, WE_INT32 iSigLen,
                       const WE_CHAR *pcHashedKey, WE_INT32 iHashedKeyLen, 
                       const WE_CHAR *pcCert, WE_INT32 iCertLen, 
                       WE_INT32 iCertType, WE_INT32 iErr)
{
    WE_INT32         iIndex = 0;
    St_SecTlSignText *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimSignTextResponse(iTargetID = %d, signId = %d, algorithm = %d, sigLen = %d, hashedKeyLen = %d,  certificateLen = %d, certificateType = %d, err = %d)\n",\
              iTargetID, iSignId, iAlg, iSigLen,  \
              iHashedKeyLen,  iCertLen, iCertType, iErr));
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"signature =\n"));
    WE_LOG_DATA(WE_LOG_DETAIL_MEDIUM, 0, (const WE_UCHAR*)pcSign, iSigLen);
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"hashedKey =\n"));
    WE_LOG_DATA(WE_LOG_DETAIL_MEDIUM, 0, (const WE_UCHAR*)pcHashedKey, iHashedKeyLen);
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"certificate =\n"));
    WE_LOG_DATA(WE_LOG_DETAIL_MEDIUM, 0, (const WE_UCHAR*)pcCert, iCertLen);

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))      
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    pstValue = (St_SecTlSignText *)WE_MALLOC(sizeof(St_SecTlSignText));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    (WE_VOID)WE_MEMSET(pstValue, 0, sizeof(St_SecTlSignText));
    
    pstValue->eType = E_SEC_DP_UI_SIGN_TEXT;
    if(iErr == M_SEC_ERR_WIM_NOT_INITIALISED)
    {
        pstValue->iErr = M_SEC_ERR_WIM_NOT_INITIALISED;
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
        return;
    }
    pstValue->iTargetID = iTargetID;
    pstValue->iSignId = iSignId;
    pstValue->iAlgorithm = iAlg;
    pstValue->iSigLen = iSigLen;
    if ((0 == iSigLen) || (NULL == pcSign))
    {
        pstValue->pcSignature = NULL;
    }
    else
    {
        pstValue->pcSignature = (WE_CHAR *)WE_MALLOC((WE_UINT32)iSigLen);
        if (NULL == (pstValue->pcSignature))
        {
            WE_FREE(pstValue);
            M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
            return;
        }
        for (iIndex=0; iIndex<iSigLen; iIndex++)
        {
            pstValue->pcSignature[iIndex] = pcSign[iIndex];
        }
    }
    
    pstValue->iHashedKeyLen= iHashedKeyLen;
    if ((0 == iHashedKeyLen) || (NULL == pcHashedKey))
    {
        pstValue->pcHashedKey = NULL;
    }
    else
    {
        pstValue->pcHashedKey = (WE_CHAR *)WE_MALLOC((WE_UINT32)iHashedKeyLen * sizeof(WE_CHAR));
        if (NULL == (pstValue->pcHashedKey))
        {
            if ((pstValue->pcSignature) != NULL)
            {
                WE_FREE(pstValue->pcSignature);    
            }
            WE_FREE(pstValue);
            M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
            return;
        }
        for (iIndex=0; iIndex<iHashedKeyLen; iIndex++)
        {
            pstValue->pcHashedKey[iIndex] = pcHashedKey[iIndex];
        }
    }
    
    pstValue->iCertificateLen = iCertLen;
    if ((0 == iCertLen) || (NULL == pcCert))
    {
        pstValue->pcCertificate = NULL;
    }
    else
    {
        pstValue->pcCertificate = (WE_CHAR *)WE_MALLOC((WE_UINT32)iCertLen * sizeof(WE_CHAR));
        if (NULL == (pstValue->pcCertificate))
        {
            if ((pstValue->pcHashedKey) != NULL)
            {
                WE_FREE(pstValue->pcHashedKey);     
            }
            if ((pstValue->pcSignature) != NULL)
            {
                WE_FREE(pstValue->pcSignature); 
            }
            WE_FREE(pstValue);
            M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
            return;
        }
        for (iIndex=0; iIndex<iCertLen; iIndex++)
        {
            pstValue->pcCertificate[iIndex] = pcCert[iIndex];
        }
    }
    pstValue->iCertificateType = iCertType;
    pstValue->iErr = iErr; 
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimFindMatchedSignTextResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the signature.
        iSignId[IN]: ID of the object.
        iResult[IN]: result of the calling function.
        iNumOfMatches[IN]: number of matched signtext.
        ppvFriendlyNames[IN]: friendly name of user certificate.
        pusFriendlyNamesLen[IN]: length of friendly name.
        psCharSet[IN]: character set of user certificate.
        pcNameType[IN]: type of name of user certificate.
        piCertIds[IN]: ID of user certificate.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimFindMatchedSignTextResponse(WE_HANDLE hSecHandle,
                                        WE_INT32 iTargetID, WE_INT32 iSignId, 
                                        WE_INT32 iResult, WE_INT32 iNumOfMatches,
                                        const WE_VOID *const *ppvFriendlyNames,
                                        WE_UINT16 *pusFriendlyNamesLen,
                                        WE_INT16 *psCharSet, WE_INT8 *pcNameType, 
                                        WE_INT32 *piCertIds)
{
    WE_INT32            iIndex = 0; 
    WE_INT32            iIndexj = 0;
    St_SecTlGetSignMatchesResp *pstValue = NULL;
    WE_UCHAR            **ppucQ = (WE_UCHAR **)ppvFriendlyNames;
    WE_CHAR             *pcTmpName = NULL;
    
    if ((NULL==hSecHandle)|| (NULL==ppvFriendlyNames) || (NULL==pusFriendlyNamesLen) 
        || (NULL==psCharSet) || (NULL==pcNameType) || (NULL==piCertIds))
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimFindMatchedSignTextResponse(iTargetID = %d, signId = %d, iResult = %d, iNumOfMatches = %d\n",\
              iTargetID, iSignId, iResult, iNumOfMatches));
    for (iIndex=0; iIndex<iNumOfMatches; iIndex++)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"friendlyNamesLen[%d] = %d, friendlyNames =\n",iIndex, pusFriendlyNamesLen[iIndex]));
        WE_LOG_DATA(WE_LOG_DETAIL_MEDIUM, 0,(WE_UCHAR *)ppvFriendlyNames[iIndex], pusFriendlyNamesLen[iIndex]);
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"charSet[%d] = %d",iIndex, psCharSet[iIndex]));
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"nameType[%d] = %d",iIndex, pcNameType[iIndex]));
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"certIds[%d] = %d",iIndex, piCertIds[iIndex]));
    }

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))  
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlGetSignMatchesResp *)WE_MALLOC(sizeof(St_SecTlGetSignMatchesResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    (WE_VOID)WE_MEMSET(pstValue, 0, sizeof(St_SecTlGetSignMatchesResp));
    pstValue->eType = E_SEC_DP_GET_SIGN_MATCHES;
    if(iResult == M_SEC_ERR_WIM_NOT_INITIALISED)
    {
        pstValue->iResult = M_SEC_ERR_WIM_NOT_INITIALISED;
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
        return;
    }
    pstValue->iResult = iResult;
    pstValue->iTargetID = iTargetID;
    pstValue->iSignId = iSignId;
    pstValue->iNbrMatches = iNumOfMatches;

    if ((iNumOfMatches > 0) && (ppucQ != NULL))
    {
        for (iIndex=0; iIndex<iNumOfMatches; iIndex++)
        {
            if ((NULL == ppvFriendlyNames[iIndex]) || (0 == pusFriendlyNamesLen[iIndex]))
            {
                pstValue->astCertNames[iIndex].pvFriendlyName = NULL;
                pstValue->astCertNames[iIndex].usFriendlyNameLen = 0;
                pstValue->astCertNames[iIndex].sCharSet = 0;
                pstValue->astCertNames[iIndex].iCertId = 0;
                pstValue->astCertNames[iIndex].cNameType = 0;
                break;
            }
            pstValue->astCertNames[iIndex].pvFriendlyName = 
                                        (WE_CHAR *)WE_MALLOC(pusFriendlyNamesLen[iIndex] + 1);
            if (NULL == (pstValue->astCertNames[iIndex].pvFriendlyName))
            {
                WE_FREE(pstValue);
                M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
                return;
            }
            pcTmpName = (WE_CHAR *)pstValue->astCertNames[iIndex].pvFriendlyName;
            for (iIndexj=0; iIndexj<pusFriendlyNamesLen[iIndex]; iIndexj++)
            {
                pcTmpName[iIndexj] = (WE_CHAR)ppucQ[iIndex][iIndexj];
            }
            pcTmpName[iIndexj] = '\0';
            pstValue->astCertNames[iIndex].usFriendlyNameLen = pusFriendlyNamesLen[iIndex];
            pstValue->astCertNames[iIndex].sCharSet = psCharSet[iIndex];
            pstValue->astCertNames[iIndex].cNameType = pcNameType[iIndex];
            pstValue->astCertNames[iIndex].iCertId = piCertIds[iIndex];
        } 
    }
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}
    
/*=====================================================================================
FUNCTION: 
        Sec_WimViewContractsResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        iResult[IN]: result of the calling function.
        iNumOfContracts[IN]: number of contracts.
        ppcContracts[IN]: content to be stored.
        ppcSign[IN]: pointer to signature.
        pusSignLen[IN]: length of signature.
        puiTime[IN]: time of contract.
        piContractIds[IN]: Id of contract.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimViewContractsResponse(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                WE_INT32 iResult, WE_INT32 iNumOfContracts,
                                const WE_CHAR *const *ppcContracts,WE_UINT16 *pusContractLen,
                                const WE_CHAR *const *ppcSign, 
                                WE_UINT16 *pusSignLen, WE_UINT32 *puiTime, 
                                WE_INT32 *piContractIds)
{
    St_SecTlGetContractsResp     *pstValue = NULL;
    WE_INT32   iIndex = 0;
    WE_INT32   iIndexK = 0;
    
    if (NULL == hSecHandle)
    {
        return;
    }

    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimViewContractsResponse(iTargetID = %d, iResult = %d, nbrContracts = %d\n",\
              iTargetID, iResult, iNumOfContracts));

    if (NULL != ppcContracts)
    {
        if (NULL != ppcContracts[0])
        {
            WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"contracts[0] = %s\n",ppcContracts[0]));
        }
        else
        {
            WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"contracts[0] = NULL\n"));
        }
    }
    else
    {
    }
    
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"contracts = NULL\n"));    
    if ((NULL != ppcSign) && (pusSignLen != NULL))
    {
      WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"signatures[0] =\n"));
      WE_LOG_DATA(WE_LOG_DETAIL_MEDIUM, 0, (const WE_UCHAR*)ppcSign[0], pusSignLen[0]);
    }
    else
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"signatures = NULL\n"));
    }

    if (NULL != puiTime)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"time[0] = %d\n", puiTime[0]));
    }
    else
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"time = NULL\n"));
    }

    if (NULL != piContractIds)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"contractIds[0] = %d\n",piContractIds[0]));
    }
    else
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"contractIds = NULL\n"));
    }

    if (!(M_SEC_WAITING_FOR_WIM_RESULT)) 
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;

    pstValue = (St_SecTlGetContractsResp *)WE_MALLOC(sizeof(St_SecTlGetContractsResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    (WE_VOID)WE_MEMSET(pstValue, 0, sizeof(St_SecTlGetContractsResp));
  
    pstValue->eType = E_SEC_DP_GET_CONTRACTS_RESP;
    if(iResult == M_SEC_ERR_WIM_NOT_INITIALISED)
    {
        pstValue->iResult = M_SEC_ERR_WIM_NOT_INITIALISED;
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
        return;
    }
    pstValue->iTargetID = iTargetID;
    pstValue->iResult = iResult;
    pstValue->iNbrContracts = iNumOfContracts;

    if ((0==iNumOfContracts) || (NULL==ppcContracts) || (NULL == pusContractLen) ||
        (NULL==ppcSign) || (0==pusSignLen) || (NULL==puiTime) || (NULL==piContractIds))
    {
        pstValue->pstContracts = NULL;        
        pstValue->piContractsLen = NULL;
    }
    else
    {
        pstValue->pstContracts = (St_SecContractInfo *)WE_MALLOC((WE_UINT32)iNumOfContracts * sizeof(St_SecContractInfo)); 
        pstValue->piContractsLen = (WE_INT32 *)WE_MALLOC((WE_UINT32)iNumOfContracts*sizeof(WE_INT32));
        if ((NULL == (pstValue->pstContracts)) || (NULL == (pstValue->piContractsLen)))
        {
            if (NULL != pstValue->pstContracts)
            {
                WE_FREE(pstValue->pstContracts);
            }
            if (NULL != pstValue->piContractsLen)
            {
                WE_FREE(pstValue->piContractsLen);
            }
            WE_FREE(pstValue);
            M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
            return;
        }
        else
        {
            for (iIndex=0; iIndex<iNumOfContracts; iIndex++)
            {
                pstValue->piContractsLen[iIndex] = (WE_INT32)(pusContractLen[iIndex]);
                if (0 == pstValue->piContractsLen[iIndex])
                {
                    pstValue->pstContracts[iIndex].pcContract = NULL;
                }
                else
                {
                    pstValue->pstContracts[iIndex].pcContract = 
                              (WE_CHAR *)WE_MALLOC((WE_UINT32)(pstValue->piContractsLen[iIndex] + 1));
                    if (NULL == pstValue->pstContracts[iIndex].pcContract)
                    {
                        for (iIndexK=0; iIndexK<iIndex; iIndexK++)
                        {
                            WE_FREE(pstValue->pstContracts[iIndexK].pcContract);
                        }
                        
                        WE_FREE(pstValue->pstContracts);                        
                        WE_FREE(pstValue->piContractsLen);
                        WE_FREE(pstValue); 
                        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
                        return;
                    }
                    for (iIndexK=0; iIndexK<(pstValue->piContractsLen[iIndex]); iIndexK++)
                    {
                        pstValue->pstContracts[iIndex].pcContract[iIndexK] = ppcContracts[iIndex][iIndexK];
                    }
                    pstValue->pstContracts[iIndex].pcContract[iIndexK] = '\0';
                }
                pstValue->pstContracts[iIndex].usSignatureLen = pusSignLen[iIndex];
                pstValue->pstContracts[iIndex].pcSignature = (WE_CHAR *)WE_MALLOC(pstValue->pstContracts[iIndex].usSignatureLen);
                if ((NULL == pstValue->pstContracts[iIndex].pcSignature) 
                    && ((pstValue->pstContracts[iIndex].usSignatureLen) != 0))
                {
                    for (iIndexK=0; iIndexK<=iIndex; iIndexK++)
                    {
                        WE_FREE(pstValue->pstContracts[iIndexK].pcContract);
                    }
                   
                    WE_FREE(pstValue->pstContracts);                    
                    WE_FREE(pstValue->piContractsLen);
                    WE_FREE(pstValue); 
                    M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
                    return;
                }
                for (iIndexK=0; iIndexK<pstValue->pstContracts[iIndex].usSignatureLen; iIndexK++)
                {
                    pstValue->pstContracts[iIndex].pcSignature[iIndexK] = ppcSign[iIndex][iIndexK];
                }
                pstValue->pstContracts[iIndex].uiTime = puiTime[iIndex];
                pstValue->pstContracts[iIndex].usContractId = (WE_UINT16)piContractIds[iIndex];
            }
        }
    }    
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimStoreContractResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        iResult[IN]: result of the calling function.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimStoreContractResponse(WE_HANDLE hSecHandle, 
                                                WE_INT32 iTargetID, WE_INT32 iResult)
{
    St_SecTlStoreContractsResp   *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }

    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimStoreContractResponse(iTargetID = %d, iResult = %d)\n",iTargetID, iResult));

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))               
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlStoreContractsResp *)WE_MALLOC(sizeof(St_SecTlStoreContractsResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    
    pstValue->eType = E_SEC_DP_STORE_CONTRACT_RESP;
    pstValue->iTargetID = iTargetID;
    pstValue->iResult = iResult;
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimDelContractResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iContractId[IN]: ID of contract.
        iResult[IN]: result of the calling function.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimDelContractResponse(WE_HANDLE hSecHandle, 
                                            WE_INT32 iContractId, WE_INT32 iResult)
{
    St_SecTlRmContractsResp *pstValue = NULL;

    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimDelContractResponse(contractId = %d, iResult = %d)\n", iContractId, iResult));
    if (!(M_SEC_WAITING_FOR_WIM_RESULT))                
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlRmContractsResp *)WE_MALLOC(sizeof(St_SecTlRmContractsResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }    
    pstValue->eType = E_SEC_DP_RM_CNTRACT_RESP;
    pstValue->iContractId = iContractId;
    pstValue->iResult = iResult;
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*=====================================================================================
FUNCTION: 
        Sec_WimUserCertReqResponse
CREATE DATE: 
        2006-7-19
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of target.
        iResult[IN]: result of the calling function.
        pucCertReqMsg[IN]: Pointer to the request message.
        uiCertReqMsgLen[]: Length of the request message.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimUserCertReqResponse(WE_HANDLE hSecHandle,
                                 WE_INT32 iTargetID, WE_INT32 iResult, 
                                 WE_UCHAR *pucCertReqMsg, WE_UINT32 uiCertReqMsgLen)
{
    St_SecTlUserCertReqResp *pstValue = NULL;
        
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
           "Sec_UserCertReqResponse(iTargetID = %d, iResult = %d)\n", iTargetID, iResult));

    if (!(M_SEC_WAITING_FOR_WIM_RESULT))                
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlUserCertReqResp *)WE_MALLOC(sizeof(St_SecTlUserCertReqResp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    (WE_VOID)WE_MEMSET(pstValue, 0, sizeof(St_SecTlUserCertReqResp));

    pstValue->pucCertReqMsg = (WE_UCHAR *)WE_MALLOC(uiCertReqMsgLen * sizeof(WE_UCHAR));
    if ((NULL == (pstValue->pucCertReqMsg)) && (uiCertReqMsgLen != 0))
    {
        WE_FREE(pstValue);
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    (WE_VOID)WE_MEMCPY(pstValue->pucCertReqMsg, pucCertReqMsg, uiCertReqMsgLen);
   
    pstValue->eType = E_SEC_DP_GET_REQ_USER_CERT_RESP;
    pstValue->iTargetID = iTargetID;
    pstValue->iResult = iResult;
    pstValue->uiCertReqMsgLen = uiCertReqMsgLen;
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}


/*=====================================================================================
FUNCTION: 
        Sec_WimChangeWtlsCertAbleResponse
CREATE DATE: 
        2006-11-13
AUTHOR: 
        Stone An
DESCRIPTION:
        omit.
ARGUMENTS PASSED:
        hSecHandle[IN/OUT]: Global data handle.
        iTargetID[IN]: ID of the object.
        iResult[IN]: result of the calling function.
        ucState[IN]: state of WTLS cert.
RETURN VALUE:
        none.
USED GLOBAL VARIABLES:
        none.    
USED STATIC VARIABLES:
        none.
CALL BY:
        omit.   
IMPORTANT NOTES:
        omit.
=====================================================================================*/
WE_VOID Sec_WimChangeWtlsCertAbleResponse(WE_HANDLE hSecHandle,
                                WE_INT32 iTargetID, WE_INT32 iResult, WE_UINT8 ucState)
{
    St_SecTlChangeWtlsCertAbleresp  *pstValue = NULL;
    
    if (NULL == hSecHandle)
    {
        return;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WimChangeWtlsCertAbleResponse(iTargetID = %d, iResult = %d)\n", iTargetID, iResult));
    
    if (!(M_SEC_WAITING_FOR_WIM_RESULT))  
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_WAITING_FOR_WIM_RESULT = 0;
    M_SEC_WANTS_TO_RUN = 1;
    
    pstValue = (St_SecTlChangeWtlsCertAbleresp *)WE_MALLOC(sizeof(St_SecTlChangeWtlsCertAbleresp));
    if (NULL == pstValue)
    {
        M_SEC_CURRENT_WIM_RESPONSE_VALUES = NULL;
        return;
    }
    pstValue->eType = E_SEC_DP_CHANGE_WTLS_CERT_ABLE_RESP;
    pstValue->ucState = ucState;
    pstValue->iTargetID = iTargetID;
    pstValue->iResult = iResult;  
    M_SEC_CURRENT_WIM_RESPONSE_VALUES = (St_SecTlWimFunctionId *)pstValue;
}

/*************************************************************************************************
end
*************************************************************************************************/
