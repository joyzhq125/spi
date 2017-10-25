/*=====================================================================================
    FILE NAME :
        Sec_iwapim.c
    MODULE NAME :
        sec
    GENERAL DESCRIPTION
        this file is for running wtls, implementation of main support for wtls.
    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2002-2006 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
    Modification              Tracking
    Date       Author         Number        Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-07-04 Kevin          none          Init
    
=====================================================================================*/

/**************************************************************************************
*   Include File Section
**************************************************************************************/
#include "sec_comm.h"


/*******************************************************************************
*   Macro Define Section
*******************************************************************************/

#define M_SEC_CFG_CONN_WITHOUT_CERT_UE

#define M_SEC_CFG_TITLE_MUST_EXIST

#define SEC_STATES                      (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->iStates)
#define SEC_WAITING_FOR_WIM_RESULT      (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->iWaitingForWim)
#define SEC_WANTS_TO_RUN                (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->iWantsToRun)
#define SEC_CUR_WIM_RESP                (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pvCurWimResp)
#define SEC_USER_ASKED                  (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->iUserReq)
#define SEC_NO_CERT_DIALOG_SHOWN        (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->iNocertUeShown)
#define SEC_WAITING_FOR_UE_RESULT       (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->iWaitingForUe)
#define SEC_CUR_UE_RESP                 (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pvCurUeResp)
#define SEC_DIALOG_SHOWN                (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->iUeShown)
#define SEC_TOO_OLD_CERT_ID             (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->iTooOldCertId)
/*for tls, add by tang 070108*/
#define SEC_KEY_ID                      (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pucKeyId)
#define SEC_CHAIN_DEPTH                 (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->ucChainDepth)
#define SEC_CERT_CHAIN_ID               (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->auiCertChainIds)
#define SEC_ASN1_CERT                   (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pstAsn1Cert)
#define SEC_TLS_SIGN_ALG                (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->iSignAlg) 
#define SEC_NO_TITLE_IN_CERT 200
/*******************************************************************************
*   Type Define Section
*******************************************************************************/

enum tagE_KeyExchangeIm_State
{
    E_STATE_KEY_EXCHANGE_DIRECT_RETURN = 0x00, 
    E_STATE_GET_WTLS_KEY_EXCHANGE, 
    E_STATE_GET_WTLS_KEY_EXCHANGE_RESP, 
    E_STATE_GET_WTLS_KEY_EXCHANGE_AGAIN
};

enum tagE_VerifySvrCertIm_State
{
    E_STATE_VERIFY_SVR_CERT_DIRECT_RETURN = 0x00, 
    E_STATE_WTLS_VERIFY_SVR_CERT_CHAIN, 
    E_STATE_WTLS_VERIFY_SVR_CERT_RESP, 
    E_STATE_WTLS_VERIFY_SVR_CERT_CHAIN_AGAIN, 
    E_STATE_HANDLE_OLD_ROOT_CERT, 
    E_STATE_HANDLE_UE_CONFIRM 
};

enum tagE_GetPrfResultIm_State
{
    E_STATE_PRF_RESULT_DIRECT_RETURN = 0x00, 
    E_STATE_WTLS_GET_PRF_RESULT, 
    E_STATE_WTLS_GET_PRF_RESULT_RESP, 
    E_STATE_WTLS_GET_PRF_RESULT_AGAIN
};

enum tagE_GetCipherSuiteIm_State
{
    E_STATE_CIPHER_SUITE_DIRECT_RETURN = 0x00, 
    E_STATE_WTLS_GET_CIPHER_SUITE,  
    E_STATE_WTLS_GET_METHODS_RESP, 
    E_STATE_WTLS_GET_CIPHER_SUITE_AGAIN, 
    E_STATE_UE_CONFIRM, 
    E_STATE_UE_CONFIRM_HANDLE
};

#ifdef M_SEC_CFG_TLS

enum tagE_TlsGetUsrCertIm_State
{
    E_STATE_TLS_GET_USER_CERT_DIRECT_RETURN = 0x00, 
    E_STATE_TLS_VIEW_NAME_OF_USER_CERT, 
    E_STATE_TLS_TL_CONN_CERT_NAME_RESP, 
    E_STATE_TLS_VIEW_NAME_OF_USER_CERT_AGAIN, 
    E_STATE_TLS_GET_USER_CERT_AND_KEY_ID, 
    E_STATE_TLS_GET_USER_CERT_CHAIN_OR_RESP,
    E_STATE_TLS_GET_USER_CERT_RESP, 
    E_STATE_TLS_RESP_HANDLE_UE_CONFIRM
};

enum tagE_TlsGetCipherSuiteIm_State
{
    E_STATE_TLS_CIPHER_SUITE_DIRECT_RETURN = 0x00, 
    E_STATE_TLS_GET_CIPHER_SUITE,  
    E_STATE_TLS_GET_METHODS_RESP, 
    E_STATE_TLS_GET_CIPHER_SUITE_AGAIN, 
};

enum tagE_TlsVerifySvrCertIm_State
{
    E_STATE_TLS_VERIFY_SVR_CERT_DIRECT_RETURN = 0x00, 
    E_STATE_TLS_VERIFY_SVR_CERT_CHAIN_COMNAME, 
    E_STATE_TLS_VERIFY_SVR_CERT_RESP, 
    E_STATE_TLS_VERIFY_SVR_CERT_CHAIN,
    E_STATE_TLS_VERIFY_SVR_CERT_CHAIN_RESP,
    E_STATE_TLS_VERIFY_SVR_CERT_CHAIN_AGAIN, 
    E_STATE_TLS_HANDLE_OLD_ROOT_CERT, 
    E_STATE_TLS_HANDLE_UE_CONFIRM 
};
enum tagE_TlsCompSignatureIm_State
{
    E_STATE_TLS_COMPUTE_SIGN_DIRECT_RETURN = 0x00, 
    E_STATE_TLS_COMPUTE_SIGN, 
    E_STATE_TLS_COMPUTE_SIGN_RESP, 
    E_STATE_TLS_COMPUTE_SIGN_AGAIN 
};

enum tagE_TlsKeyExchangeIm_State
{
    E_STATE_TLS_KEY_EXCHANGE_DIRECT_RETURN = 0x00, 
    E_STATE_GET_TLS_KEY_EXCHANGE, 
    E_STATE_GET_TLS_KEY_EXCHANGE_RESP, 
    E_STATE_GET_TLS_KEY_EXCHANGE_AGAIN
};

enum tagE_TlsGetPrfResultIm_State
{
    E_STATE_TLS_PRF_RESULT_DIRECT_RETURN = 0x00, 
    E_STATE_TLS_GET_PRF_RESULT, 
    E_STATE_TLS_GET_PRF_RESULT_RESP, 
    E_STATE_TLS_GET_PRF_RESULT_AGAIN
};




#endif


enum tagE_CompSignatureIm_State
{
    E_STATE_COMPUTE_SIGN_DIRECT_RETURN = 0x00, 
    E_STATE_WTLS_COMPUTE_SIGN, 
    E_STATE_WTLS_COMPUTE_SIGN_RESP, 
    E_STATE_WTLS_COMPUTE_SIGN_AGAIN 
};

enum tagE_GetUsrCertIm_State
{
    E_STATE_GET_USER_CERT_DIRECT_RETURN = 0x00, 
    E_STATE_VIEW_NAME_OF_USER_CERT, 
    E_STATE_TL_CONN_CERT_NAME_RESP, 
    E_STATE_VIEW_NAME_OF_USER_CERT_AGAIN, 
    E_STATE_GET_USER_CERT_AND_KEY_ID, 
    E_STATE_GET_USER_CERT_RESP, 
    E_STATE_RESP_HANDLE_UE_CONFIRM
};

enum tagE_TlConnCertNameResp_State
{
    E_STATE_RESP_UE_CONFIRM = 0x00, 
    E_STATE_RESP_GET_USER_CERT_AND_KEY_ID_AGAIN 
};


enum tagE_RemovePeer_State
{
    E_STATE_REMOVE_PEER_DIRECT_RETURN = 0x00, 
    E_STATE_PEER_REMOVE, 
    E_STATE_HANDLE_PEER_REMOVE_RESULT, 
    E_STATE_PEER_REMOVE_AGAIN 
};

enum tagE_AttachPeerToSessionIm_State
{
    E_STATE_ATTACH_PEER_DIRECT_RETURN = 0x00, 
    E_STATE_PEER_ATTACH_SESSION, 
    E_STATE_HANDLE_PEER_LINK_RESULT, 
    E_STATE_PEER_ATTACH_SESSION_AGAIN 
};

enum tagE_EnableSessionIm_State
{
    E_STATE_ENABLE_SESSION_DIRECT_RETURN = 0x00, 
    E_STATE_SESSION_ENABLE, 
    E_STATE_HANDLE_SESSION_ENABLE_RESULT, 
    E_STATE_SESSION_ENABLE_AGAIN 
};

enum tagE_DisableSessionIm_State
{
    E_STATE_DISABLE_SESSION_DIRECT_RETURN = 0x00, 
    E_STATE_SESSION_DISABLE, 
    E_STATE_HANDLE_SESSION_DISABLE_RESULT, 
    E_STATE_SESSION_DISABLE_AGAIN 
};

enum tagE_UpdateSessionIm_State
{
    E_STATE_UPDATE_SESSION_DIRECT_RETURN = 0x00, 
    E_STATE_UPDATE_SESSION, 
    E_STATE_HANDLE_UPDATE_SESSION_RESULT, 
    E_STATE_UPDATE_SESSION_AGAIN 
};

enum tagE_SearchPeerIm_State
{
    E_STATE_SEARCH_PEER_DIRECT_RETURN = 0x00, 
    E_STATE_PEER_SEARCH, 
    E_STATE_SEARCH_PEER_RESP, 
    E_STATE_PEER_SEARCH_AGAIN 
};

enum tagE_FetchSessionInfoIm_State
{
    E_STATE_FETCH_SESSOIN_INFO_DIRECT_RETURN = 0x00, 
    E_STATE_FETCH_SESSION, 
    E_STATE_GET_SESSION_INFO_RESP, 
    E_STATE_FETCH_SESSION_AGAIN 
};

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
WE_INT32 Sec_ExamineTitleGetWtlsCN( WE_HANDLE hSecHandle, const WE_UCHAR *pucBuf, WE_INT32 iBufLen,
                                    WE_UCHAR **ppucCommonName, WE_INT32 *piCommonNameLen);
WE_INT32 Sec_ExamineTitleOfCertOrRoot(WE_HANDLE hSecHandle, St_SecCertificate stSpCert);
WE_INT32 Sec_GetX509ComName( WE_HANDLE hSecHandle, const WE_UCHAR *pucBuf, WE_UCHAR **ppucCommonName, 
                             WE_INT32 *piCommonNameLen,
                             WE_UCHAR **ppucDNSName,WE_UINT16 *pusDNSNameLen,
                             WE_UCHAR **ppucIPAddress,WE_UINT16 *pusIPAddressLen);
WE_INT32 Sec_ExamineComName(const WE_UCHAR *pucAddr, WE_INT32 iAddrLen, 
                            const WE_UCHAR *pucDomain,WE_INT32 iDomainLen, 
                            WE_UCHAR *pucCommonName, WE_INT32 iCommonNameLen);
WE_INT32 Sec_GetWtlsComName( WE_HANDLE hSecHandle, const WE_UCHAR *pucBuf, WE_UCHAR **ppucCommonName, 
                             WE_INT32 *piCommonNameLen);


/*==================================================================================================
FUNCTION: 
    Sec_ExamineTitleOfCertOrRoot
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    examine the title in the root certificate or common certificate.
ARGUMENTS PASSED:
    WE_UCHAR *pucAddr[IN]:Pointer to the address.  
    WE_INT32 iAddrLen[IN]:The length of the address.
    WE_UCHAR *pucDomain[IN]:Pointer to the domain.
    WE_INT32 iDomainLen[IN]:The length of the domain.
    WE_UCHAR *pucCommonName[IN]:Pointer to the common name.
    WE_INT32 iCommonNameLen[IN]:The length of the common name.
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
WE_INT32 Sec_ExamineTitleOfCertOrRoot(WE_HANDLE hSecHandle,St_SecCertificate stSpCert)
{
    WE_UINT8              ucNbrIssuerAtts = 0;
    WE_INT32              iSubcript = 0;
    WE_INT32              iSelfSigned = 1;    
    WE_INT32              iTCaExists = 0;   
    WE_INT32              iRes = 0;
    St_SecNameAttribute * pstIssuerA = NULL;
  
    if (stSpCert.stCertUnion.stWtls.ucIssuerLen == stSpCert.stCertUnion.stWtls.ucSubjectLen)
    {
        for (iSubcript=0; iSubcript<stSpCert.stCertUnion.stWtls.ucIssuerLen; iSubcript++)
        {
            if (stSpCert.pucIssuer[iSubcript] != stSpCert.pucSubject[iSubcript])
            {
                iSelfSigned = 0;
            }
        }
    }
    else
    {
        iSelfSigned = 0;
    }
    
    if (iSelfSigned)
    {
        return M_SEC_ERR_OK;
    }

    iRes = Sec_WtlsGetIssuerInfo(hSecHandle,(const WE_UCHAR *)stSpCert.pucIssuer, 
                                    (WE_UINT16)stSpCert.stCertUnion.stWtls.ucIssuerLen,
                                    &ucNbrIssuerAtts, NULL);
    if (iRes != M_SEC_ERR_OK)
    {
        return iRes;
    }
    pstIssuerA = (St_SecNameAttribute *)WE_MALLOC((WE_ULONG)(ucNbrIssuerAtts*(WE_INT32)sizeof(St_SecNameAttribute)));
    if(NULL == pstIssuerA)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    if(Sec_WtlsGetIssuerInfo(hSecHandle,(const WE_UCHAR *)stSpCert.pucIssuer, 
                                    (WE_UINT16)stSpCert.stCertUnion.stWtls.ucIssuerLen,
                                    &ucNbrIssuerAtts, pstIssuerA) != M_SEC_ERR_OK)
    {
        for (iSubcript=0; iSubcript<ucNbrIssuerAtts; iSubcript++)
        {
            WE_FREE(pstIssuerA[iSubcript].pucAttributeValue);
        }
    
        WE_FREE(pstIssuerA);

        return M_SEC_ERR_INVALID_PARAMETER;
    }
            
    for (iSubcript=0; iSubcript<ucNbrIssuerAtts; iSubcript++)
    {
        if (pstIssuerA[iSubcript].uiAttributeType == (WE_UINT32)M_SEC_TITLE)
        {
            if (!SEC_STRCMP((WE_CHAR *)pstIssuerA[iSubcript].pucAttributeValue, (WE_CHAR *)"T=ca"))
            {
                iTCaExists = 1;
            }
        }
    }
              
    for (iSubcript=0; iSubcript<ucNbrIssuerAtts; iSubcript++)
    {
        WE_FREE(pstIssuerA[iSubcript].pucAttributeValue);
    }
                
    WE_FREE(pstIssuerA);
                  
    if (!iTCaExists)
    {
        return SEC_NO_TITLE_IN_CERT;
    }
    
    return M_SEC_ERR_OK;
}


/*==================================================================================================
FUNCTION: 
    Sec_ExamineTitleGetWtlsCN
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    examint title and common name.
ARGUMENTS PASSED:
    WE_UCHAR *pucAddr[IN]:Pointer to the address.  
    WE_INT32 iAddrLen[IN]:The length of the address.
    WE_UCHAR *pucDomain[IN]:Pointer to the domain.
    WE_INT32 iDomainLen[IN]:The length of the domain.
    WE_UCHAR *pucCommonName[IN]:Pointer to the common name.
    WE_INT32 iCommonNameLen[IN]:The length of the common name.
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
#define SEC_IWAPIM_TMPAUC_LEN 70
WE_INT32 Sec_ExamineTitleGetWtlsCN(WE_HANDLE hSecHandle,const WE_UCHAR *pucBuf, WE_INT32 iBufLen,
                                    WE_UCHAR **ppucCommonName, WE_INT32 *piCommonNameLen)
{    
    WE_UINT16           usCertLen = 0;    
    WE_UINT8            ucIssuerLen = 0;
    WE_UINT8            ucNbrS = 0;
    WE_UCHAR *          pucCert = NULL;
    WE_INT32            iLenLeft = 0;
    WE_INT32            iResult = 0;
    WE_INT32            iSubcript = 0;
    WE_INT32            iStrSize = 0;
    WE_UCHAR            aucTmpStr[SEC_IWAPIM_TMPAUC_LEN] = {0};
    St_SecCertificate   stSpCert = {0};
    St_SecCertificate   stSpCaCert = {0};

    if ((NULL == pucBuf) || (NULL == ppucCommonName) || (NULL == piCommonNameLen))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
  
    iResult = Sec_WtlsCertParse(hSecHandle,pucBuf+1, &usCertLen, &stSpCert); 
    if (iResult != M_SEC_ERR_OK)
    {
        return iResult;
    }
    ucIssuerLen = stSpCert.stCertUnion.stWtls.ucIssuerLen;
          
    for (iSubcript=0; iSubcript<ucIssuerLen; iSubcript++)
    {
        aucTmpStr[iStrSize] = stSpCert.pucIssuer[iSubcript];

        if(((stSpCert.pucIssuer[iSubcript] == ';') && (stSpCert.pucIssuer[iSubcript+1] == ' ')) || 
        ((iSubcript == (ucIssuerLen-1)) && (ucNbrS == 3)))
        { 
            ucNbrS++;
            if (ucNbrS == 4)
            {  
                if (iStrSize>0) 
                {
                    *ppucCommonName = WE_MALLOC((WE_ULONG)iStrSize);
                    if (NULL == *ppucCommonName)
                    {
                        return M_SEC_ERR_INSUFFICIENT_MEMORY;
                    }
                    (void)WE_MEMCPY(*ppucCommonName,aucTmpStr,(WE_UINT32)iStrSize);
                    *piCommonNameLen = iStrSize;
                }
                else
                {
                    ppucCommonName[0] = NULL;
                }
    
                iSubcript = ucIssuerLen;
            } 
            iStrSize = 0;
            iSubcript += 1;
        }
        else
        {
            iStrSize++;
        }
    }
              
    if(ucNbrS <= 3)
    {
        ppucCommonName[0] = NULL;
    }
                
    iResult = Sec_ExamineTitleOfCertOrRoot(hSecHandle,stSpCert);
    if (iResult != M_SEC_ERR_OK)
    {
        return iResult;
    }
                  
    pucCert = (WE_UCHAR *)pucBuf;
    iLenLeft = iBufLen - usCertLen - 1;
    while (iLenLeft > 0)
    {
        if ((*pucCert) == 1)
        {
            pucCert += 1 + usCertLen;
        }
        else if ((*pucCert) == 2)
        {
            pucCert += 5 + usCertLen;
        }
        else
        {
            return M_SEC_ERR_INVALID_PARAMETER;
        }

        iResult = Sec_WtlsCertParse(hSecHandle,pucBuf+1, &usCertLen,    &stSpCaCert); 
        if (iResult != M_SEC_ERR_OK)
        {
            return iResult;
        }

        iResult = Sec_ExamineTitleOfCertOrRoot(hSecHandle,stSpCaCert);

        if (iResult != M_SEC_ERR_OK)
        {
            return iResult;
        }
        if ((*pucCert) == 1)
        {
            iLenLeft -= (usCertLen + 1);
        }
        else if ((*pucCert) == 2)
        {
            iLenLeft -= (usCertLen + 5);
        }
    }
    return iResult;
}

/*==================================================================================================
FUNCTION: 
    Sec_ExamineComName
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    examine the common name
ARGUMENTS PASSED:
    WE_UCHAR *pucAddr[IN]:Pointer to the address.  
    WE_INT32 iAddrLen[IN]:The length of the address.
    WE_UCHAR *pucDomain[IN]:Pointer to the domain.
    WE_INT32 iDomainLen[IN]:The length of the domain.
    WE_UCHAR *pucCommonName[IN]:Pointer to the common name.
    WE_INT32 iCommonNameLen[IN]:The length of the common name.
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
#define  SEC_IWAPIM_HOST_LEN 16
WE_INT32 Sec_ExamineComName(const WE_UCHAR *pucAddr, WE_INT32 iAddrLen, 
                            const WE_UCHAR *pucDomain, WE_INT32 iDomainLen, 
                            WE_UCHAR *pucCommonName, WE_INT32 iCommonNameLen)
{
    WE_INT32  iSubcript = 0;
    WE_CHAR * pcTemp = NULL;
    WE_CHAR   acHost[SEC_IWAPIM_HOST_LEN] = {0};
    
    if (pucCommonName == NULL)
    {
        return M_SEC_ERR_OK; 
    }
    
    if ((ct_isdigit (pucCommonName[0])) && (pucAddr != NULL) && (iAddrLen != 0) && (iCommonNameLen < 16)) 
    {
        (void)WE_MEMCPY(acHost, (WE_CHAR*) pucCommonName, (WE_UINT32)iCommonNameLen);  
        for (iSubcript = 0, pcTemp = acHost; iSubcript < 4; iSubcript++) 
        {
            if (SEC_ATOI(pcTemp) != pucAddr[iSubcript+2]) 
            {
                return M_SEC_ERR_INVALID_COMMON_NAME;
            }
            
            pcTemp = SEC_STRCHR(pcTemp, '.');
            
            if (pcTemp == NULL) 
            {
                if (iSubcript == 3)
                {
                    return M_SEC_ERR_OK;
                }
                else
                {
                    return M_SEC_ERR_INVALID_COMMON_NAME;
                }
            }
            pcTemp++;
        }
    }
    if (pucDomain == NULL) 
    {        
        return M_SEC_ERR_OK;
    }
    if ((iDomainLen !=0) && (iDomainLen == iCommonNameLen) && 
        !WE_MEMCMP (pucDomain, pucCommonName, (WE_UINT32)iCommonNameLen))
    {
        return M_SEC_ERR_OK;
    }
    return M_SEC_ERR_INVALID_COMMON_NAME;
}

/*==================================================================================================
FUNCTION: 
    Sec_GetWtlsComName
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    get wtls common name
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle. 
    WE_UCHAR *pucBuf[IN]:Pointer to the data buffer.
    WE_UCHAR **ppucCommonName[OT]:Pointer to the buffer of the common name.
    WE_INT32 *piCommonNameLen[OT]:Pointer to the common name length.
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
WE_INT32 Sec_GetWtlsComName(WE_HANDLE hSecHandle,const WE_UCHAR *pucBuf, WE_UCHAR **ppucCommonName, 
                           WE_INT32 *piCommonNameLen)
{    
    WE_UINT16 usCertLen = 0;
    WE_INT32 iResult = 0;
    WE_INT32 iSubcript = 0;
    WE_INT32 iStrSize = 0;
    WE_UINT8 ucIssuerLen = 0;
    WE_UINT8 ucNbrS = 0;
    WE_UCHAR aucTmpStr[SEC_IWAPIM_TMPAUC_LEN] = {0};
    St_SecCertificate stSpCert = {0};

    if ((NULL == pucBuf) || (NULL == ppucCommonName) || (NULL == piCommonNameLen))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    iResult = Sec_WtlsCertParse(hSecHandle,pucBuf+1, &usCertLen, &stSpCert); 
    if (iResult != M_SEC_ERR_OK)
    {
        return iResult;
    }
    ucIssuerLen = stSpCert.stCertUnion.stWtls.ucIssuerLen;
          
    for (iSubcript=0; iSubcript<ucIssuerLen; iSubcript++)
    {
        aucTmpStr[iStrSize] = stSpCert.pucIssuer[iSubcript];
        
        if(((stSpCert.pucIssuer[iSubcript] == ';') && (stSpCert.pucIssuer[iSubcript+1] == ' ')) || 
            ((iSubcript == (ucIssuerLen-1)) && (ucNbrS == 3)))
        { 
            ucNbrS++;
            if (ucNbrS == 4)
            {  
                if (iStrSize>0)
                {
                    *ppucCommonName = WE_MALLOC((WE_ULONG)iStrSize);
                    if (NULL == *ppucCommonName)
                    {                        
                        return M_SEC_ERR_INSUFFICIENT_MEMORY;
                    }
                    (void)WE_MEMCPY(*ppucCommonName,aucTmpStr,(WE_UINT32)iStrSize);
                    *piCommonNameLen = iStrSize;
                }
                else
                {
                    ppucCommonName[0] = NULL;
                }
                iSubcript = ucIssuerLen;
            } 
            iStrSize = 0;
            iSubcript += 1;
        }
        else
        {
            iStrSize++;
        }
    }
    if(ucNbrS <= 3)
    {
        ppucCommonName[0] = NULL;
    }
    return iResult;
}

/*==================================================================================================
FUNCTION: 
    Sec_GetX509ComName
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    get x509 common name
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.  
    WE_UCHAR *pucBuf[IN]:Pointer to the data.
    WE_UCHAR **ppucCommonName[OT]:Pointer to the buffer of the common name.
    WE_INT32 *piCommonNameLen[OT]:Pointer to the common name length.
    WE_UCHAR **ppucDNSName[OT]:Pointet to the buffer of the DNS name.
    WE_UINT16 *pusDNSNameLen[OT]:Pointer to the DNS name length.
    WE_UCHAR **ppucIPAddress[IO]:Pointer to the buffer of the ip address.
    WE_UINT16 *pusIPAddressLen[OT]:Pointer to the length of the ip address.
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
WE_INT32 Sec_GetX509ComName( WE_HANDLE hSecHandle, const WE_UCHAR *pucBuf, WE_UCHAR **ppucCommonName, 
                           WE_INT32 *piCommonNameLen,
                           WE_UCHAR **ppucDNSName,WE_UINT16 *pusDNSNameLen,
                           WE_UCHAR **ppucIPAddress,WE_UINT16 *pusIPAddressLen)
{
    WE_INT32            iResult = 0;
    WE_INT32            iSubcript = 0;
    WE_UINT16           usRealCertLen = 0;
    WE_UCHAR *          pucInSubject = NULL;
    WE_UINT16           usSubjectLen = 0;
    WE_UINT8            ucNbrSubjectStrings = 0;
    St_SecCertificate   stSpCert = {0};
    St_SecNameAttribute *pstIssuerAtt= NULL;

    if ((NULL == ppucCommonName) || (NULL == piCommonNameLen))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    ppucDNSName = ppucDNSName;
    pusDNSNameLen = pusDNSNameLen;
    ppucIPAddress = ppucIPAddress;
    pusIPAddressLen = pusIPAddressLen;
    
    iResult = Sec_X509CertParse(hSecHandle,pucBuf, &usRealCertLen, &stSpCert);    
    if (iResult != M_SEC_ERR_OK)
    {
        return iResult;
    }
    pucInSubject = stSpCert.pucSubject;
    usSubjectLen = stSpCert.stCertUnion.stX509.usSubjectLen;
    
    iResult = Sec_X509GetNbrIssuerStr(hSecHandle,pucInSubject, usSubjectLen, &ucNbrSubjectStrings);
    if(iResult != M_SEC_ERR_OK)
    {
        return iResult;
    }
    
    
    pstIssuerAtt = (St_SecNameAttribute *)WE_MALLOC((WE_ULONG)(ucNbrSubjectStrings*(WE_INT32)sizeof(St_SecNameAttribute)));
    if(NULL == pstIssuerAtt)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    
    iResult = Sec_X509GetIssuerInfo(hSecHandle, (const WE_UCHAR *)pucInSubject, 
                                (WE_UINT16)usSubjectLen, &ucNbrSubjectStrings, 
                                pstIssuerAtt);
    for (iSubcript=0; iSubcript<ucNbrSubjectStrings; iSubcript++)
    {
        if ((pstIssuerAtt[iSubcript].uiAttributeType) == (WE_UINT32)M_SEC_COMMON_NAME)
        {
            if (*ppucCommonName != NULL)
            {
                WE_FREE (*ppucCommonName); 
            }
            *ppucCommonName = pstIssuerAtt[iSubcript].pucAttributeValue;
            *piCommonNameLen = pstIssuerAtt[iSubcript].usAttributeValueLen; 
        } 
        else
        {
            WE_FREE(pstIssuerAtt[iSubcript].pucAttributeValue);
        }
    }
    
    WE_FREE(pstIssuerAtt);
    
    return M_SEC_ERR_OK;    
}

/*==================================================================================================
FUNCTION: 
    Sec_KeyExchangeIm
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    exchange key in wtls
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IO]:Global data handle.  
    WE_INT32 iTargetID[IN]:ID of the object.
    St_SecWtlsKeyExchParams stParam[IN]:The structure of the param.
    WE_UINT8 ucHashAlg[IN]:The value of the hash alg.
    WE_UCHAR * pucRandval[IN]]:Pointer to the randval.
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
void Sec_KeyExchangeIm (WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                        St_SecWtlsKeyExchParams *pstParam,  WE_UINT8 ucHashAlg, 
                        const WE_UCHAR * pucRandval)
{
    St_SecTlKeyXchResp *pstWimStKeyexchResp = NULL;
    St_SecWtlsKeyExchParams *pstWtlsKeyExchParams = NULL;

    if (NULL == hSecHandle)
    {
        return ;
    }
    
    switch (SEC_STATES)
    {
        case E_STATE_KEY_EXCHANGE_DIRECT_RETURN: 
            return;    
        case E_STATE_GET_WTLS_KEY_EXCHANGE: 
            SEC_WAITING_FOR_WIM_RESULT = 1;
            SEC_WANTS_TO_RUN = 0;
            pstWtlsKeyExchParams = pstParam; 
            Sec_iWimWtlsKeyExchange(hSecHandle,iTargetID, *pstWtlsKeyExchParams,  pucRandval,ucHashAlg);
            SEC_STATES++;
            break;
        case E_STATE_GET_WTLS_KEY_EXCHANGE_RESP:
            if (SEC_WAITING_FOR_WIM_RESULT)
            {
                return;
            }
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_KeyExchangeResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, 
                                        0, NULL, 0);
                Sec_DpHandleCleanUp( hSecHandle );
                return;
            }
            pstWimStKeyexchResp = ( St_SecTlKeyXchResp *)SEC_CUR_WIM_RESP;
            if(pstWimStKeyexchResp->eType != E_SEC_DP_XCH_KEY)
            {
                Sec_KeyExchangeResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, 
                                        0, NULL, 0);
                Sec_DpHandleCleanUp( hSecHandle );
                Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                SEC_CUR_WIM_RESP = NULL;
                return;
            }
        
            if(pstWimStKeyexchResp->iResult == M_SEC_ERR_WIM_NOT_INITIALISED)
            {
                SEC_STATES++; 
            }
            else 
            {
                Sec_KeyExchangeResp(hSecHandle, pstWimStKeyexchResp->iTargetID, pstWimStKeyexchResp->iResult, 
                                        pstWimStKeyexchResp->iMasterSecretId, 
                                        pstWimStKeyexchResp->pucPublicValue, pstWimStKeyexchResp->iPublicValueLen);
                Sec_DpHandleCleanUp( hSecHandle );
            }
            Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
            SEC_CUR_WIM_RESP = NULL;
            break;
        case E_STATE_GET_WTLS_KEY_EXCHANGE_AGAIN:
            {
                WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle, iTargetID, FALSE);
                switch(iStatus)
                {
                    case M_SEC_ERR_IN_PROGRESS:
                        break;
                    case M_SEC_ERR_MISSING_KEY:
                        #ifdef M_SEC_CFG_SHOW_WARNINGS
                        Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_MISSING_AUT_KEY_HANDSHAKE);
                        #endif
                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        pstWtlsKeyExchParams = pstParam; 
                        Sec_iWimWtlsKeyExchange(hSecHandle,iTargetID, 
                                                *pstWtlsKeyExchParams,  pucRandval,ucHashAlg);
                        SEC_STATES--;
                        break;
                    case M_SEC_ERR_OK:
                
                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        pstWtlsKeyExchParams = pstParam; 
                        Sec_iWimWtlsKeyExchange(hSecHandle,iTargetID, 
                                                *pstWtlsKeyExchParams,  pucRandval,ucHashAlg);
                        SEC_STATES--;
                        break;
                    case M_SEC_ERR_USER_NOT_VERIFIED:
                        iStatus = M_SEC_ERR_OK;   /* fall through */
                    default:                    
                        Sec_KeyExchangeResp(hSecHandle, iTargetID, iStatus, 0, NULL, 0);
                        Sec_DpHandleCleanUp( hSecHandle );
                        break;
                }
            }
            break;
        default : 
            return;
    }
}
/*==================================================================================================
FUNCTION: 
    Sec_GetPrfResultIm
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    get prf function result.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IO]:Global data handle.  
    WE_INT32 iTargetID[IN]:ID of the object.
    WE_UINT8 ucAlg[IN]:Value of the alg.
    WE_INT32 iMasterSecretId[IN]:ID of the master secret.
    WE_UCHAR * pucSecret[IN]:Pointer to the secret.
    WE_INT32 iSecretLen[IN]:The length of the secret.
    WE_CHAR * pcLabel[IN]:The pointer to the lable.
    WE_UCHAR * pucSeed[IN]:Pointer to the seed.
    WE_INT32 iSeedLen[IN]:The length of the seed.
    WE_INT32 iOutputLen[IN]:The length of the output data.
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
#define SEC_DEL_ID_FLAG(a) (((a)&0x80)?((a)^0x80):(a))
void Sec_GetPrfResultIm (WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_UINT8 ucAlg, WE_INT32 iMasterSecretId,
                        const WE_UCHAR * pucSecret, WE_INT32 iSecretLen, const WE_CHAR * pcLabel,
                        const WE_UCHAR * pucSeed, WE_INT32 iSeedLen, WE_INT32 iOutputLen)
{
    St_SecTlPrfResp * pstSecWimStPrfResp = NULL;

    if (NULL == hSecHandle)
    {
        return ;
    }
    
    switch (SEC_STATES)
    {
        case E_STATE_PRF_RESULT_DIRECT_RETURN:
            return;
        case E_STATE_WTLS_GET_PRF_RESULT:    
            SEC_WAITING_FOR_WIM_RESULT = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimGetPrfResult(hSecHandle, SEC_DEL_ID_FLAG(iTargetID),  iMasterSecretId, 
                                    pucSecret, iSecretLen,  pcLabel, pucSeed, iSeedLen, 
                                    iOutputLen, (WE_UINT8)M_SEC_WTLS_CONNECTION_TYPE,ucAlg);
            SEC_STATES++;
            break;
        case E_STATE_WTLS_GET_PRF_RESULT_RESP:
            if (SEC_WAITING_FOR_WIM_RESULT)
            {
                return;
            }
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_GetPrfResultResp(hSecHandle, iTargetID, 
                                        M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0);
                Sec_DpHandleCleanUp( hSecHandle );
                return;
            }
            pstSecWimStPrfResp = (St_SecTlPrfResp *)SEC_CUR_WIM_RESP;
            if(pstSecWimStPrfResp->eType != E_SEC_DP_PRF)
            {
                Sec_GetPrfResultResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, NULL, 0);
                Sec_DpHandleCleanUp( hSecHandle );
                Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                SEC_CUR_WIM_RESP = NULL;
                return;
            }
            if(pstSecWimStPrfResp->iResult == M_SEC_ERR_WIM_NOT_INITIALISED)
            {
                SEC_STATES++; 
            }
            else
            {
                Sec_GetPrfResultResp(hSecHandle, iTargetID, pstSecWimStPrfResp->iResult, 
                                        pstSecWimStPrfResp->pucBuf, pstSecWimStPrfResp->iBufLen);
                Sec_DpHandleCleanUp( hSecHandle );
            }
        
            Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
            SEC_CUR_WIM_RESP = NULL;
            break;
        case E_STATE_WTLS_GET_PRF_RESULT_AGAIN:
            {
                WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle,iTargetID&0x7F, FALSE);
                switch(iStatus)
                {
                    case M_SEC_ERR_IN_PROGRESS:
                        break;
                    case M_SEC_ERR_MISSING_KEY:
                        #ifdef M_SEC_CFG_SHOW_WARNINGS
                        Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_MISSING_AUT_KEY_HANDSHAKE);
                        #endif
                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_iWimGetPrfResult(hSecHandle, SEC_DEL_ID_FLAG(iTargetID), 
                                                iMasterSecretId,  pucSecret, iSecretLen, pcLabel, pucSeed, iSeedLen, 
                                                iOutputLen, (WE_UINT8)M_SEC_WTLS_CONNECTION_TYPE,ucAlg);
                        SEC_STATES--;
                        break;
                    case M_SEC_ERR_OK:
                
                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_iWimGetPrfResult(hSecHandle, SEC_DEL_ID_FLAG(iTargetID), 
                                                iMasterSecretId,pucSecret, iSecretLen,  pcLabel, pucSeed, iSeedLen, 
                                                iOutputLen, (WE_UINT8)M_SEC_WTLS_CONNECTION_TYPE,ucAlg);
                        SEC_STATES--;
                        break;
                    case M_SEC_ERR_USER_NOT_VERIFIED:
                        iStatus = M_SEC_ERR_OK;   /* fall through */
                        //lint -fallthrough
                    default:                              
                        Sec_GetPrfResultResp(hSecHandle, iTargetID, iStatus, NULL, 0);
                        Sec_DpHandleCleanUp( hSecHandle );
                        break;
                }
            }
            break;
        default :
            return;
    }
}
/*==================================================================================================
FUNCTION: 
    Sec_GetCipherSuiteIm
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    get cipher suite supporting wtls
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IO]:Global data handle.  
    WE_INT32 iTargetID[IN]:ID of the object.
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
void Sec_GetCipherSuiteIm (WE_HANDLE hSecHandle, WE_INT32 iTargetID)
{
    St_SecTlGetWtlsMethodsResp *pstWtlsGetMethodsResp = NULL;
    St_SecTlConfirmDlgResp *pstUeStConfirmEvtResp = NULL;

    if (NULL == hSecHandle)
    {
        return ;
    }
    
    switch (SEC_STATES)
    {
        case E_STATE_CIPHER_SUITE_DIRECT_RETURN: 
            return;
        case E_STATE_WTLS_GET_CIPHER_SUITE:
            SEC_WAITING_FOR_WIM_RESULT = 1;
            SEC_WANTS_TO_RUN = 0;
            SEC_USER_ASKED = 0;
            Sec_iWimWtlsGetCipherSuite(hSecHandle,iTargetID, (WE_UINT8)0);
            SEC_STATES++;
            break;
        case E_STATE_WTLS_GET_METHODS_RESP:
            if(SEC_WAITING_FOR_WIM_RESULT)
            {
                return;
            }
            if (!(SEC_CUR_WIM_RESP))
            { 
                Sec_WtlsGetCipherSuiteResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, 
                                           NULL, 0, NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp( hSecHandle );
                return;
            }
            pstWtlsGetMethodsResp = (St_SecTlGetWtlsMethodsResp *)SEC_CUR_WIM_RESP;
            if(pstWtlsGetMethodsResp->eType != E_SEC_DP_GET_CIPHERSUITE)
            {
                Sec_WtlsGetCipherSuiteResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, 
                                        NULL, 0, NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp( hSecHandle );
                Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                SEC_CUR_WIM_RESP = NULL;
                return;
            }
            if (pstWtlsGetMethodsResp->iResult == M_SEC_ERR_OK)
            {            
                if ((pstWtlsGetMethodsResp->iTrustedKeyIdsLen ==0) && (SEC_USER_ASKED == 0)
                    && (pstWtlsGetMethodsResp->iTooManyCerts == 0))
                {
                    SEC_NO_CERT_DIALOG_SHOWN =1;
                    SEC_STATES += 2;
                }
                else
                {
                    if (SEC_USER_ASKED == 0)
                    {                  
                        SEC_NO_CERT_DIALOG_SHOWN = 0;
                    }
                    Sec_WtlsGetCipherSuiteResp(hSecHandle, pstWtlsGetMethodsResp->iTargetID, pstWtlsGetMethodsResp->iResult, 
                                              pstWtlsGetMethodsResp->pucCipherMethods, pstWtlsGetMethodsResp->iCipherMethodsLen,
                                              pstWtlsGetMethodsResp->pucKeyExchangeIds, pstWtlsGetMethodsResp->iKeyExchangeIdsLen,
                                              pstWtlsGetMethodsResp->pucTrustedKeyIds, pstWtlsGetMethodsResp->iTrustedKeyIdsLen);
                    Sec_DpHandleCleanUp( hSecHandle );
                }
            }
            else if(pstWtlsGetMethodsResp->iResult == M_SEC_ERR_WIM_NOT_INITIALISED)
            {
                SEC_STATES++; 
            }
            else
            {
                Sec_WtlsGetCipherSuiteResp(hSecHandle, pstWtlsGetMethodsResp->iTargetID, pstWtlsGetMethodsResp->iResult, 
                                          pstWtlsGetMethodsResp->pucCipherMethods, pstWtlsGetMethodsResp->iCipherMethodsLen,
                                          pstWtlsGetMethodsResp->pucKeyExchangeIds, pstWtlsGetMethodsResp->iKeyExchangeIdsLen,
                                          pstWtlsGetMethodsResp->pucTrustedKeyIds, pstWtlsGetMethodsResp->iTrustedKeyIdsLen);
                Sec_DpHandleCleanUp( hSecHandle );
            }
            Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
            SEC_CUR_WIM_RESP = NULL;
            break;
        case E_STATE_WTLS_GET_CIPHER_SUITE_AGAIN:
            {
                WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle,iTargetID, FALSE);
                switch(iStatus)
                {
                    case M_SEC_ERR_IN_PROGRESS:
                        break;
                    case M_SEC_ERR_MISSING_KEY:
                        #ifdef M_SEC_CFG_SHOW_WARNINGS
                        Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_MISSING_AUT_KEY_HANDSHAKE);
                        #endif

                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_iWimWtlsGetCipherSuite(hSecHandle,iTargetID,(WE_UINT8)0);
                        SEC_STATES--;
                        break;
                    case M_SEC_ERR_OK:
                
                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_iWimWtlsGetCipherSuite(hSecHandle,iTargetID,(WE_UINT8)0);
                        SEC_STATES--;
                        break;
                    case M_SEC_ERR_USER_NOT_VERIFIED:
                        iStatus = M_SEC_ERR_OK;   /* fall through */
                        //lint -fallthrough
                    default:                
                        Sec_WtlsGetCipherSuiteResp(hSecHandle, iTargetID, iStatus, NULL, 0, 
                                                  NULL, 0, NULL, 0);
                        Sec_DpHandleCleanUp( hSecHandle );
                        break;
                }
            }
            break;
        case E_STATE_UE_CONFIRM:    
            SEC_WAITING_FOR_UE_RESULT = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iUeConfirm(hSecHandle,iTargetID, M_SEC_ID_CONFIRM_1, M_SEC_ID_OK, M_SEC_ID_CANCEL);
            SEC_STATES++;                                                    
            break;
        case E_STATE_UE_CONFIRM_HANDLE:
            if (SEC_WAITING_FOR_UE_RESULT)    
            {
                return;
            }
            if (!SEC_CUR_UE_RESP)
            {
                Sec_WtlsGetCipherSuiteResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, 
                                           NULL, 0, NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp( hSecHandle );
                return;                         
            }
            pstUeStConfirmEvtResp = (St_SecTlConfirmDlgResp *)SEC_CUR_UE_RESP;
            if ((pstUeStConfirmEvtResp->eType != E_SEC_DP_CONFIRM_DLG) || (!(pstUeStConfirmEvtResp->ucAnswer)))
            {
                if (!(pstUeStConfirmEvtResp->ucAnswer))
                {
                    Sec_WtlsGetCipherSuiteResp(hSecHandle, iTargetID, M_SEC_ERR_NO_MATCHING_ROOT_CERTIFICATE, 
                                               NULL, 0, NULL, 0, NULL, 0);
                }
                else
                {                
                    Sec_WtlsGetCipherSuiteResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, 
                                               NULL, 0, NULL, 0, NULL, 0);
                }
                Sec_DpHandleCleanUp( hSecHandle );
                Sec_TlHandleUeStructFree(SEC_CUR_UE_RESP);
                SEC_CUR_UE_RESP = NULL;
                   return;
            }
        
            SEC_WAITING_FOR_WIM_RESULT = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimWtlsGetCipherSuite(hSecHandle, iTargetID, (WE_UINT8)1);
            Sec_TlHandleUeStructFree(SEC_CUR_UE_RESP);
            SEC_CUR_UE_RESP = NULL;
            SEC_USER_ASKED = 1;
            SEC_STATES = 2;                                                
            break;
        default : 
            return;
    }
}
/*==================================================================================================
FUNCTION: 
    Sec_VerifySvrCertIm
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    verify the server certificate chain.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IO]:Global data handle.  
    WE_INT32 iTargetID[IN]:ID of the object.
    WE_UCHAR * pucBuf[IN]:Pointer to the data.
    WE_INT32 iBufLen[IN]:The length of the buffer.
    WE_UCHAR * pucAddr[IN]:Pointer to the address.
    WE_INT32 iAddrLen[IN]:The length of the addr.
    WE_CHAR * pcLabel[IN]:Pointer to the label.
    WE_CHAR * pucDomain[IN]:Pointer to the domain.
    WE_INT32 iDomainLen[IN]:The length of the domain.
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
void Sec_VerifySvrCertIm (WE_HANDLE hSecHandle, WE_INT32 iTargetID, const WE_UCHAR * pucBuf, 
                          WE_INT32 iBufLen,  const WE_UCHAR * pucAddr, WE_INT32 iAddrLen,
                          const WE_CHAR * pucDomain, WE_INT32 iDomainLen)
{
    
    WE_UCHAR *pucCommonName = NULL;
    WE_INT32 iCommonNameLen = 0;
    WE_INT32 iResult = 0;
    WE_UCHAR *pucDNSName = NULL; 
    WE_UINT16 usDNSNameLen = 0;
    WE_UCHAR *pucIPAddress = NULL; 
    WE_UINT16 usIPAddressLen = 0;
    St_SecTlVerifyCertChainResp *pstWimStVerifyCertChainResp = NULL;
    #ifdef M_SEC_CFG_CONN_WITHOUT_CERT_UE
        St_SecTlConfirmDlgResp *pstUeStConfirmEvtResp = NULL;
    #endif
        
    if ((NULL == hSecHandle) || (NULL == pucBuf))
    {
        return ;
    }
    
    switch (SEC_STATES)
    {    
        case E_STATE_VERIFY_SVR_CERT_DIRECT_RETURN: 
            return;    
        case E_STATE_WTLS_VERIFY_SVR_CERT_CHAIN: 
            SEC_USER_ASKED =0;
            SEC_DIALOG_SHOWN = 0;
        
            if ((iBufLen==0)&&(SEC_NO_CERT_DIALOG_SHOWN ==1))
            {
                Sec_WtlsVerifySvrCertResp(hSecHandle, iTargetID,M_SEC_ERR_DOWNGRADED_TO_ANON);
                Sec_DpHandleCleanUp( hSecHandle );
                return;
            }        
            if (iBufLen==0)
            { 
                SEC_WAITING_FOR_UE_RESULT = 1;
                SEC_WANTS_TO_RUN = 0;            
                Sec_iUeConfirm(hSecHandle, iTargetID,M_SEC_ID_CONFIRM_12,M_SEC_ID_OK, M_SEC_ID_CANCEL);
                SEC_STATES = 5;
                break;
            }
        
            if ((*pucBuf)==M_SEC_CERTIFICATE_WTLS_TYPE)
            {            
            #ifdef M_SEC_CFG_TITLE_MUST_EXIST
                iResult = Sec_ExamineTitleGetWtlsCN(hSecHandle,pucBuf, iBufLen, &pucCommonName,&iCommonNameLen);
            #else
                iResult = Sec_GetWtlsComName(hSecHandle,pucBuf, &pucCommonName,&iCommonNameLen);
            #endif
            }
            else if ((*pucBuf)==M_SEC_CERTIFICATE_X509_TYPE)
            {
                iResult = Sec_GetX509ComName(hSecHandle,pucBuf+1, &pucCommonName,&iCommonNameLen,
                                            &pucDNSName,&usDNSNameLen,&pucIPAddress,&usIPAddressLen);
            }
            else 
            {
                Sec_WtlsVerifySvrCertResp(hSecHandle, iTargetID, 
                    M_SEC_ERR_INVALID_PARAMETER);
                Sec_DpHandleCleanUp( hSecHandle );
                return;
            }
        
            if (iResult == SEC_NO_TITLE_IN_CERT)
            {
                Sec_WtlsVerifySvrCertResp(hSecHandle, iTargetID, M_SEC_ERR_BAD_CERTIFICATE);
                if (pucCommonName)
                {
                    WE_FREE(pucCommonName);
                }
                Sec_DpHandleCleanUp( hSecHandle );
                return;
            }
            else if (iResult != M_SEC_ERR_OK)
            {
                Sec_WtlsVerifySvrCertResp(hSecHandle, iTargetID, iResult);
                if (pucCommonName)
                {
                    WE_FREE(pucCommonName);
                }
                Sec_DpHandleCleanUp( hSecHandle );
                return;
            }
        
            if (pucCommonName)
            {
                iResult= Sec_ExamineComName(pucAddr, iAddrLen,(const WE_UCHAR *)pucDomain, 
                                            iDomainLen, pucCommonName,iCommonNameLen);         
                if (iResult != 0)
                {
                    Sec_WtlsVerifySvrCertResp(hSecHandle, iTargetID,M_SEC_ERR_INVALID_COMMON_NAME);
                    WE_FREE(pucCommonName);
                    Sec_DpHandleCleanUp( hSecHandle );
                    return;
                }
            }
        
            if (pucCommonName)
            {
                WE_FREE(pucCommonName);
            }
            SEC_WAITING_FOR_WIM_RESULT = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimWtlsVerifySvrCertChain(hSecHandle, iTargetID, pucBuf, iBufLen);
            SEC_STATES++;
            break;
        case E_STATE_WTLS_VERIFY_SVR_CERT_RESP:
            if (SEC_WAITING_FOR_WIM_RESULT)
            {
                return;
            }
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_WtlsVerifySvrCertResp(hSecHandle, iTargetID,M_SEC_ERR_INSUFFICIENT_MEMORY);    
                Sec_DpHandleCleanUp( hSecHandle );
                return;
            }
        
            pstWimStVerifyCertChainResp = (St_SecTlVerifyCertChainResp *)SEC_CUR_WIM_RESP;
            if(pstWimStVerifyCertChainResp->eType != E_SEC_DP_VERIFY_CERT_CHAIN)
            {
                Sec_WtlsVerifySvrCertResp(hSecHandle, iTargetID,M_SEC_ERR_GENERAL_ERROR);
                Sec_DpHandleCleanUp( hSecHandle );
                Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                SEC_CUR_WIM_RESP = NULL;
                return;
            }
        
            if(pstWimStVerifyCertChainResp->iResult == M_SEC_ERR_WIM_NOT_INITIALISED)
            {
                SEC_STATES++; 
            }
            else if((pstWimStVerifyCertChainResp->iResult == M_SEC_ERR_OLD_ROOTCERT_FOUND) && (SEC_USER_ASKED == 0)) 
            {    
                SEC_TOO_OLD_CERT_ID = pstWimStVerifyCertChainResp->iCertTooOldId;
                SEC_STATES+=2;
                SEC_USER_ASKED = 1;
            }
            #ifdef M_SEC_CFG_CONN_WITHOUT_CERT_UE          
            else if (pstWimStVerifyCertChainResp->iResult != M_SEC_ERR_OK)
            {
                SEC_WAITING_FOR_UE_RESULT = 1;
                SEC_WANTS_TO_RUN = 0;
                if (pstWimStVerifyCertChainResp->iResult == M_SEC_ERR_NO_MATCHING_ROOT_CERTIFICATE)
                {            
                    Sec_iUeConfirm(hSecHandle,iTargetID, M_SEC_ID_CONFIRM_8, M_SEC_ID_OK, M_SEC_ID_CANCEL);
                }
                else if (pstWimStVerifyCertChainResp->iResult == M_SEC_ERR_CERTIFICATE_EXPIRED)
                {
                    Sec_iUeConfirm(hSecHandle,iTargetID, M_SEC_ID_CONFIRM_11, M_SEC_ID_OK, M_SEC_ID_CANCEL);
                }
                else
                {
                    Sec_iUeConfirm(hSecHandle,iTargetID, M_SEC_ID_CONFIRM_10, M_SEC_ID_OK, M_SEC_ID_CANCEL);
                }
                SEC_STATES += 3;                                                    
            }
            #endif 
            else
            {
                Sec_WtlsVerifySvrCertResp(hSecHandle, pstWimStVerifyCertChainResp->iTargetID, 
                                        pstWimStVerifyCertChainResp->iResult);
                Sec_DpHandleCleanUp( hSecHandle );
            }
            Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
            SEC_CUR_WIM_RESP = NULL;
            break;
        case E_STATE_WTLS_VERIFY_SVR_CERT_CHAIN_AGAIN:
            {
                WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle, iTargetID, FALSE);
                switch(iStatus)
                {
                    case M_SEC_ERR_IN_PROGRESS:
                        break;
                    case M_SEC_ERR_MISSING_KEY:
                    #ifdef M_SEC_CFG_SHOW_WARNINGS
                        Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_MISSING_AUT_KEY_HANDSHAKE);
                    #endif
                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_iWimWtlsVerifySvrCertChain(hSecHandle, iTargetID, pucBuf, iBufLen);
                        SEC_STATES--;
                        break;
                    case M_SEC_ERR_OK:                
                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_iWimWtlsVerifySvrCertChain(hSecHandle, iTargetID, pucBuf, iBufLen);
                        SEC_STATES--;
                        break;
                    case M_SEC_ERR_USER_NOT_VERIFIED:
                        iStatus = M_SEC_ERR_OK;   /* fall through */
                        //lint -fallthrough
                    default:                
                        Sec_WtlsVerifySvrCertResp(hSecHandle, iTargetID, iStatus);
                        Sec_DpHandleCleanUp( hSecHandle );
                        break;
                }
            }
            break;
        case E_STATE_HANDLE_OLD_ROOT_CERT : 
            {
                WE_INT32 iStatus = Sec_HandleOldRootCert(hSecHandle, iTargetID,SEC_TOO_OLD_CERT_ID);
                switch(iStatus)
                {
                    case M_SEC_ERR_IN_PROGRESS:
                        break;
                    default:
                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_iWimWtlsVerifySvrCertChain(hSecHandle, iTargetID, pucBuf, iBufLen);
                        SEC_STATES-=2;
                        break;
                }
            }
            break;                    
        #ifdef M_SEC_CFG_CONN_WITHOUT_CERT_UE
        case E_STATE_HANDLE_UE_CONFIRM :
            if (SEC_WAITING_FOR_UE_RESULT)
            {
                return;
            }
            if(!SEC_CUR_UE_RESP)
            {
                Sec_WtlsVerifySvrCertResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
                Sec_DpHandleCleanUp( hSecHandle );
                return;
            }
            pstUeStConfirmEvtResp = (St_SecTlConfirmDlgResp *)SEC_CUR_UE_RESP;
        
            if(pstUeStConfirmEvtResp->eType != E_SEC_DP_CONFIRM_DLG)
            {
                Sec_WtlsVerifySvrCertResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR);
                Sec_DpHandleCleanUp( hSecHandle );
                Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                SEC_CUR_WIM_RESP = NULL;
                return;
            }
        
            if (!(pstUeStConfirmEvtResp->ucAnswer))                                    
            {
                Sec_WtlsVerifySvrCertResp(hSecHandle, iTargetID, M_SEC_ERR_BAD_CERTIFICATE);            
            }
            else
            {
                Sec_WtlsVerifySvrCertResp(hSecHandle, iTargetID, M_SEC_ERR_DOWNGRADED_TO_ANON);
            }
            SEC_DIALOG_SHOWN = 1;
            Sec_DpHandleCleanUp( hSecHandle );
            Sec_TlHandleUeStructFree(SEC_CUR_UE_RESP);
            SEC_CUR_UE_RESP = NULL;
            break;
        #endif 
        default :
            return;
    }
}
/*==================================================================================================
FUNCTION: 
    Sec_CompSignatureIm
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    compute signature implementation.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IO]:Global data handle.  
    WE_INT32 iTargetID[IN]:ID of the object.
    WE_UCHAR * pucKeyId[IN]:Pointer to the key id.
    WE_INT32 iKeyIdLen[IN]:The length of the key id.
    WE_UCHAR * pucBuf[IN]:Pointer to the data buffer.
    WE_INT32 iBufLen[IN]:the length of the data buffer. 
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
#ifdef M_SEC_CFG_WTLS_CLASS_3
void Sec_CompSignatureIm (WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                          const WE_UCHAR * pucKeyId, WE_INT32 iKeyIdLen,
                          const WE_UCHAR * pucBuf, WE_INT32 iBufLen)
{
    St_SecTlComputeSignResp * pstWimStComputeSigResp = NULL;
    if (NULL == hSecHandle)
    {
        return ;
    }
    
    switch (SEC_STATES)
    {
        case E_STATE_COMPUTE_SIGN_DIRECT_RETURN : 
            return;    
        case E_STATE_WTLS_COMPUTE_SIGN: 
            SEC_WAITING_FOR_WIM_RESULT = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimCalSign(hSecHandle, iTargetID, pucKeyId, iKeyIdLen, 
                                        pucBuf, iBufLen, (WE_UINT8)M_SEC_RSA_SIGN_CERT);
            SEC_STATES++;
            break;
        case E_STATE_WTLS_COMPUTE_SIGN_RESP:
            if (SEC_WAITING_FOR_WIM_RESULT)
            {
                return;
            }
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_WtlsCompSigResp(hSecHandle, iTargetID, 
                                    M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0);
                Sec_DpHandleCleanUp( hSecHandle );
                return;
            }
            pstWimStComputeSigResp = (St_SecTlComputeSignResp *)SEC_CUR_WIM_RESP;
            if(pstWimStComputeSigResp->eType != E_SEC_DP_COMPUTE_SIGN)
            {
                Sec_WtlsCompSigResp(hSecHandle, iTargetID, 
                                    M_SEC_ERR_GENERAL_ERROR, NULL, 0);
                Sec_DpHandleCleanUp( hSecHandle );
                Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                SEC_CUR_WIM_RESP = NULL;
                return;
            }
            if(pstWimStComputeSigResp->iResult == M_SEC_ERR_WIM_NOT_INITIALISED)
            {
                SEC_STATES++; 
            }
            else
            {
                Sec_WtlsCompSigResp(hSecHandle, pstWimStComputeSigResp->iTargetID, pstWimStComputeSigResp->iResult, 
                                    pstWimStComputeSigResp->pucSig, pstWimStComputeSigResp->iSigLen);
                Sec_DpHandleCleanUp( hSecHandle );
            }
            Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
            SEC_CUR_WIM_RESP = NULL;
            break;
        case E_STATE_WTLS_COMPUTE_SIGN_AGAIN:
            {
                WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle,iTargetID, FALSE);
                switch(iStatus)
                {
                    case M_SEC_ERR_IN_PROGRESS:
                        break;
                    case M_SEC_ERR_MISSING_KEY:
                    #ifdef M_SEC_CFG_SHOW_WARNINGS
                        Sec_iUeWarning(hSecHandle, iTargetID,M_SEC_ERR_MISSING_AUT_KEY_HANDSHAKE);
                    #endif
                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_iWimCalSign(hSecHandle,iTargetID, pucKeyId, iKeyIdLen, 
                                                    pucBuf, iBufLen, (WE_UINT8)M_SEC_RSA_SIGN_CERT);
                        SEC_STATES--;
                        break;
                    case M_SEC_ERR_OK:
                
                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_iWimCalSign(hSecHandle,iTargetID, pucKeyId, iKeyIdLen, 
                                                    pucBuf, iBufLen, (WE_UINT8)M_SEC_RSA_SIGN_CERT);
                        SEC_STATES--;
                        break;
                    case M_SEC_ERR_USER_NOT_VERIFIED:
                        iStatus = M_SEC_ERR_OK;    /* fall through */
                        //lint -fallthrough
                    default:                
                        Sec_WtlsCompSigResp(hSecHandle, iTargetID, iStatus, NULL, 0);
                        Sec_DpHandleCleanUp( hSecHandle );
                        break;
                }
            }
            break;
        default : 
            return;
    }
}
#endif

/*==================================================================================================
FUNCTION: 
    Sec_GetUsrCertIm
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    get user certificate used in wtls class 3.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IO]:Global data handle.  
    WE_INT32 iTargetID[IN]:ID of the object.
    WE_UCHAR * pucBuf[IN]:Pointer to the data buffer.
    WE_INT32 iBufLen[IN]:The length of the data buffer.
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
#ifdef M_SEC_CFG_WTLS_CLASS_3
void Sec_GetUsrCertIm (WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                       const WE_UCHAR * pucBuf, WE_INT32 iBufLen)
{
    St_SecTlConnCertNamesResp *pstWimStCertNamesConnResp = NULL;
    St_SecTlGetUcertKeyIdResp *pstWimStGetUcertKeyidResp = NULL;
    St_SecTlChooseCertResp *pstTiStChooseCertByNameResp = NULL;
    St_SecTlConfirmDlgResp *pstTiStConfirmResp = NULL;
    WE_INT32 iCertId = 0;

    if (NULL == hSecHandle)
    {
        return ;
    }
    
    switch (SEC_STATES)
    {
        case E_STATE_GET_USER_CERT_DIRECT_RETURN : 
            return;    
        case E_STATE_VIEW_NAME_OF_USER_CERT : 
        #ifdef M_SEC_CFG_CONN_WITHOUT_CERT_UE
            if (SEC_DIALOG_SHOWN == 1)  
            {
                Sec_WtlsGetUsrCertResp(hSecHandle, iTargetID, M_SEC_ERR_OK, 
                                       NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp( hSecHandle );
                return;
            }
        #endif 
            SEC_WAITING_FOR_WIM_RESULT = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimViewNameOfUserCert(hSecHandle, pucBuf, iBufLen);
            SEC_STATES++;
            break;
        case E_STATE_TL_CONN_CERT_NAME_RESP :
            if (SEC_WAITING_FOR_WIM_RESULT)
            {
                return;
            }
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_WtlsGetUsrCertResp(hSecHandle, iTargetID, 
                                       M_SEC_ERR_INSUFFICIENT_MEMORY,NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp( hSecHandle );
                return;
            }
            pstWimStCertNamesConnResp = (St_SecTlConnCertNamesResp *)SEC_CUR_WIM_RESP;
            if(pstWimStCertNamesConnResp->eType != E_SEC_DP_GET_CONN_CERT_NAMES)
            {
                Sec_WtlsGetUsrCertResp(hSecHandle, iTargetID, 
                                       M_SEC_ERR_GENERAL_ERROR, NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp( hSecHandle );
                Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                SEC_CUR_WIM_RESP = NULL;
                return;
            }
            if(pstWimStCertNamesConnResp->iResult == M_SEC_ERR_WIM_NOT_INITIALISED)
            {
                SEC_STATES++; 
                Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                SEC_CUR_WIM_RESP = NULL;
            }
            else
            {
                switch(pstWimStCertNamesConnResp->iNbrOfCerts)
                {
                    case E_STATE_RESP_UE_CONFIRM:
                
                        SEC_WAITING_FOR_UE_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_iUeConfirm(hSecHandle, iTargetID,M_SEC_ID_CONFIRM_2, M_SEC_ID_OK, M_SEC_ID_CANCEL); 
                        SEC_STATES += 4;
                        Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                        SEC_CUR_WIM_RESP = NULL;
                        break;
                    case E_STATE_RESP_GET_USER_CERT_AND_KEY_ID_AGAIN:                        
                        iCertId = pstWimStCertNamesConnResp->pstCertNames[0].iCertId;                
                        Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                        SEC_CUR_WIM_RESP = NULL;
                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_iWimGetKeyIdAndUCert(hSecHandle, iCertId, 
                                                    (WE_UINT8)M_SEC_WTLS_CONNECTION_TYPE);
                        SEC_STATES+=3;    
                        break;
                    default:                        
                        SEC_WAITING_FOR_UE_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_iUeSelectCert (hSecHandle,iTargetID, M_SEC_ID_TITLE_CHOOSE_CERT, 
                                           M_SEC_ID_OK, pstWimStCertNamesConnResp->iNbrOfCerts, 
                                           pstWimStCertNamesConnResp->pstCertNames);
                        SEC_STATES+=2;    
                        Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                        SEC_CUR_WIM_RESP = NULL;
                        break;
                }
            }
            break;
        case E_STATE_VIEW_NAME_OF_USER_CERT_AGAIN: 
            {
                WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle, iTargetID, FALSE);
                switch(iStatus)
                {
                    case M_SEC_ERR_IN_PROGRESS:
                        break;
                    case M_SEC_ERR_MISSING_KEY:
                    #ifdef M_SEC_CFG_SHOW_WARNINGS
                        Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_MISSING_AUT_KEY_HANDSHAKE);
                    #endif
                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_iWimViewNameOfUserCert(hSecHandle, pucBuf, iBufLen);
                        SEC_STATES--;
                        break;
                    case M_SEC_ERR_OK:                
                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_iWimViewNameOfUserCert(hSecHandle, pucBuf, iBufLen);
                        SEC_STATES--;
                        break;
                    case M_SEC_ERR_USER_NOT_VERIFIED:
                        iStatus = M_SEC_ERR_OK;    /* fall through */
                        //lint -fallthrough
                    default:                
                        Sec_WtlsGetUsrCertResp(hSecHandle, iTargetID, iStatus, 
                                               NULL, 0, NULL, 0);
                        Sec_DpHandleCleanUp( hSecHandle );
                        break;
                }
            }
            break;
        case E_STATE_GET_USER_CERT_AND_KEY_ID:    
            if (SEC_WAITING_FOR_UE_RESULT)
            {
                return;
            }
            if (!SEC_CUR_UE_RESP)
            {
                Sec_DpHandleCleanUp( hSecHandle );
                Sec_WtlsGetUsrCertResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, 
                                       NULL, 0, NULL, 0);
                return;
            }
            pstTiStChooseCertByNameResp = ( St_SecTlChooseCertResp *)SEC_CUR_UE_RESP;
            if(pstTiStChooseCertByNameResp->eType != E_SEC_DP_CHOOSE_CERT)
            {
                Sec_WtlsGetUsrCertResp(hSecHandle, iTargetID, 
                                       M_SEC_ERR_GENERAL_ERROR, NULL, 0,NULL, 0);
                Sec_DpHandleCleanUp(hSecHandle );
                Sec_TlHandleUeStructFree(SEC_CUR_UE_RESP);
                SEC_CUR_UE_RESP = NULL;
                return;
            }
            if (pstTiStChooseCertByNameResp->eResult == E_SEC_UE_CANCEL)
            {
                Sec_WtlsGetUsrCertResp(hSecHandle, iTargetID, 
                                       M_SEC_ERR_CANCEL, NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp(hSecHandle );
                Sec_TlHandleUeStructFree(SEC_CUR_UE_RESP);
                SEC_CUR_UE_RESP = NULL;
                return;
            }
            else if ((pstTiStChooseCertByNameResp->eResult != E_SEC_UE_OK) || (pstTiStChooseCertByNameResp->iCertId < 1))
            {
                Sec_WtlsGetUsrCertResp(hSecHandle, iTargetID, 
                                       M_SEC_ERR_GENERAL_ERROR, NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp(hSecHandle );
                Sec_TlHandleUeStructFree(SEC_CUR_UE_RESP);
                SEC_CUR_UE_RESP = NULL;
                return;
            }
            SEC_WAITING_FOR_WIM_RESULT = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimGetKeyIdAndUCert(hSecHandle, pstTiStChooseCertByNameResp->iCertId, 
                                        M_SEC_WTLS_CONNECTION_TYPE);
            Sec_TlHandleUeStructFree(SEC_CUR_UE_RESP);
            SEC_CUR_UE_RESP = NULL;
            SEC_STATES++;
            break;
        case E_STATE_GET_USER_CERT_RESP:    
            if(SEC_WAITING_FOR_WIM_RESULT)
            {
                return;
            }
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_WtlsGetUsrCertResp(hSecHandle, iTargetID, 
                                      M_SEC_ERR_INSUFFICIENT_MEMORY, 
                                      NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp( hSecHandle );
                return;
            }
            pstWimStGetUcertKeyidResp = (St_SecTlGetUcertKeyIdResp *)SEC_CUR_WIM_RESP;
            if(pstWimStGetUcertKeyidResp->eType != E_SEC_DP_GET_USER_CERT)
            {
                Sec_WtlsGetUsrCertResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, 
                                       NULL, 0, NULL, 0);
            }                                
            else if ((pstWimStGetUcertKeyidResp->iCertLen)==0)
            {
                Sec_WtlsGetUsrCertResp(hSecHandle, iTargetID, M_SEC_ERR_MISSING_CERTIFICATE, 
                                       NULL, 0, NULL, 0);
            }                                
            else
            {
                Sec_WtlsGetUsrCertResp(hSecHandle, iTargetID, M_SEC_ERR_OK, 
                                       pstWimStGetUcertKeyidResp->pucKeyId, pstWimStGetUcertKeyidResp->iKeyIdLen, 
                                       pstWimStGetUcertKeyidResp->pucCert, pstWimStGetUcertKeyidResp->iCertLen);
            }                                
            Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
            SEC_CUR_WIM_RESP = NULL;
            Sec_DpHandleCleanUp( hSecHandle );
            break;    
        case E_STATE_RESP_HANDLE_UE_CONFIRM:    
            if(SEC_WAITING_FOR_UE_RESULT)
            {
                return;
            }
            if (!SEC_CUR_UE_RESP)
            {
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_WtlsGetUsrCertResp(hSecHandle, iTargetID, 
                                       M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0, NULL, 0);
                return;
            }
            pstTiStConfirmResp = (St_SecTlConfirmDlgResp *)SEC_CUR_UE_RESP;
            if(pstTiStConfirmResp->eType != E_SEC_DP_CONFIRM_DLG)
            {
                Sec_WtlsGetUsrCertResp(hSecHandle, iTargetID, 
                                       M_SEC_ERR_GENERAL_ERROR, NULL, 0, NULL, 0);
                Sec_TlHandleUeStructFree(SEC_CUR_UE_RESP);
                SEC_CUR_UE_RESP = NULL;
                Sec_DpHandleCleanUp( hSecHandle );
                return;
            }
            if (pstTiStConfirmResp->ucAnswer)
            {
                Sec_WtlsGetUsrCertResp(hSecHandle, iTargetID, 
                                       M_SEC_ERR_OK, NULL, 0, NULL, 0);
            }
            else    
            {
                Sec_WtlsGetUsrCertResp(hSecHandle, iTargetID, 
                                       M_SEC_ERR_BAD_CERTIFICATE, NULL, 0, NULL, 0);
            }
            Sec_TlHandleUeStructFree(SEC_CUR_UE_RESP);
            SEC_CUR_UE_RESP = NULL;
            Sec_DpHandleCleanUp( hSecHandle );
            break;
        default :
            return;
    }
}
#endif 

#ifdef M_SEC_CFG_TLS
/*==================================================================================================
FUNCTION: 
    Sec_GetTlsCipherSuiteIm
CREATE DATE:
    2007-01-19
AUTHOR:
    tang
DESCRIPTION:
    get cipher suite supporting tls
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IO]:Global data handle.  
    WE_INT32 iTargetID[IN]:ID of the object.
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
void Sec_GetTlsCipherSuiteIm (WE_HANDLE hSecHandle, WE_INT32 iTargetID)
{
    St_SecTlGetTlsCipherSuitesResp *pstTlsGetCipherSuitesResp = NULL;
    
    if (NULL == hSecHandle)
    {
        return ;
    }
    
    switch (SEC_STATES)
    {
    case E_STATE_TLS_CIPHER_SUITE_DIRECT_RETURN: /*0*/
        return;
        
    case E_STATE_TLS_GET_CIPHER_SUITE:/*1*/
        SEC_WAITING_FOR_WIM_RESULT = 1;
        SEC_WANTS_TO_RUN = 0;
        Sec_iWimTlsGetCipherSuites(hSecHandle,iTargetID);
        SEC_STATES++;
        break;        
        
    case E_STATE_TLS_GET_METHODS_RESP:/*2*/
        if(SEC_WAITING_FOR_WIM_RESULT)
        {
            return;
        }
        if (!(SEC_CUR_WIM_RESP))
        { 
            Sec_TlsGetCipherSuiteResp(hSecHandle, iTargetID, 
                M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0);
            
            Sec_DpHandleCleanUp( hSecHandle );
            return;
        }
        pstTlsGetCipherSuitesResp = (St_SecTlGetTlsCipherSuitesResp *)SEC_CUR_WIM_RESP;
        if(pstTlsGetCipherSuitesResp->eType != E_SEC_DP_GET_TLS_CIPHERSUITE)
        {
            Sec_TlsGetCipherSuiteResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, NULL, 0);
            Sec_DpHandleCleanUp( hSecHandle );
            Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
            SEC_CUR_WIM_RESP = NULL;
            return;
        }
        if(pstTlsGetCipherSuitesResp->iResult == M_SEC_ERR_WIM_NOT_INITIALISED)
        {
            SEC_STATES++; 
        }
        else
        {
            Sec_TlsGetCipherSuiteResp(hSecHandle, pstTlsGetCipherSuitesResp->iTargetID, \
                pstTlsGetCipherSuitesResp->iResult, \
                pstTlsGetCipherSuitesResp->pucCipherSuites, \
                pstTlsGetCipherSuitesResp->iCipherSuitesLen);
            Sec_DpHandleCleanUp( hSecHandle );
        }
        Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
        SEC_CUR_WIM_RESP = NULL;
        break;
        
    case E_STATE_TLS_GET_CIPHER_SUITE_AGAIN:/*3*/
        {
            WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle,iTargetID, FALSE);
            switch(iStatus)
            {
            case M_SEC_ERR_IN_PROGRESS:
                break;
            case M_SEC_ERR_MISSING_KEY:
#ifdef M_SEC_CFG_SHOW_WARNINGS
                Sec_iUeWarning(hSecHandle,iTargetID,M_SEC_ERR_MISSING_AUT_KEY_HANDSHAKE);
#endif
                SEC_WAITING_FOR_WIM_RESULT = 1;
                SEC_WANTS_TO_RUN = 0;
                Sec_iWimTlsGetCipherSuites(hSecHandle,iTargetID);
                SEC_STATES--;
                break;
            case M_SEC_ERR_OK:
                
                SEC_WAITING_FOR_WIM_RESULT = 1;
                SEC_WANTS_TO_RUN = 0;
                Sec_iWimTlsGetCipherSuites(hSecHandle,iTargetID);
                SEC_STATES--;
                break;
            case M_SEC_ERR_USER_NOT_VERIFIED:
                iStatus = M_SEC_ERR_OK;
            default:
                Sec_TlsGetCipherSuiteResp(hSecHandle, iTargetID, iStatus, NULL, 0);
                Sec_DpHandleCleanUp( hSecHandle );
                break;
            }
        }
        break;
    default:
        return;
    }
}

/*==================================================================================================
FUNCTION: 
    Sec_GetTlsUsrCertIm
CREATE DATE:
    2007-01-19
AUTHOR:
    tang
DESCRIPTION:
    get client user certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle. 
    WE_INT32 iTargetID[IN]:ID of the object.
    const WE_UCHAR *pucCertificateTypes[IN]: point  to the type of certificate.
    WE_INT32 iNbrCertificateTypes[IN]:the number of type.
    St_SecTlsDistingubshedName *pstCertAuthorities[IN]point to the authorities name.
    WE_INT32 iNumCertAuthorities[IN]:the number of uthorities name.
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
WE_VOID Sec_GetTlsUsrCertIm (WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                             const WE_UCHAR *pucCertificateTypes, 
                             WE_INT32 iNbrCertificateTypes,
                             St_SecTlsDistingubshedName *pstCertAuthorities,
                             WE_INT32 iNumCertAuthorities)
{
    WE_INT32                        iCertId = 0;
    WE_INT32                        iILoop=0;
    WE_INT32                        iJLoop=0;
    WE_UCHAR                        *pucTmpName=NULL;
    St_SecTlConnCertNamesResp       *pstWimStCertNamesConnResp = NULL;
    St_SecTlGetUcertKeyIdResp       *pstWimStGetUcertKeyidResp = NULL;
    St_SecTlChooseCertResp          *pstTiStChooseCertByNameResp = NULL;
    St_SecTlConfirmDlgResp          *pstTiStConfirmResp = NULL;
    St_SecTlsGetChainOfCertsResp    *pstWimStTlsGetChainOfCertsResp=NULL;
    St_SecTlsDistingubshedName      **ppstWimCertAuthorities=NULL;

    if (NULL == hSecHandle||NULL==pstCertAuthorities)
    {
       return ;
    }
        
    switch (SEC_STATES)
    {
       case E_STATE_TLS_GET_USER_CERT_DIRECT_RETURN : /*0*/
            return;    
       case E_STATE_TLS_VIEW_NAME_OF_USER_CERT : /*1 ,get user cert name */

//#ifdef M_SEC_CFG_CONN_WITHOUT_CERT_UE
//            if (SEC_DIALOG_SHOWN == 1)  
//            {
//               Sec_TlsGetUsrCertResp(hSecHandle, iTargetID, M_SEC_ERR_OK,NULL,0, NULL, 0);
//               Sec_DpHandleCleanUp( hSecHandle );
//               return;
//            }
//#endif tang for tlsue
            SEC_WAITING_FOR_WIM_RESULT = 1;
            SEC_WANTS_TO_RUN = 0;
            ppstWimCertAuthorities=(St_SecTlsDistingubshedName **)&pstCertAuthorities;
            Sec_iWimGetTLSUserCertNames(hSecHandle,pucCertificateTypes, \
                 iNbrCertificateTypes, *ppstWimCertAuthorities, iNumCertAuthorities);
            SEC_STATES++;
            break;
        case E_STATE_TLS_TL_CONN_CERT_NAME_RESP :/*2*/
            if (SEC_WAITING_FOR_WIM_RESULT)
            {
                return;
            }
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_TlsGetUsrCertResp(hSecHandle, iTargetID, 
                                       M_SEC_ERR_INSUFFICIENT_MEMORY,NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp( hSecHandle );
                return;
            }
            
            pstWimStCertNamesConnResp = (St_SecTlConnCertNamesResp *)SEC_CUR_WIM_RESP;
            if(pstWimStCertNamesConnResp->eType != E_SEC_DP_GET_CONN_CERT_NAMES)
            {
                Sec_TlsGetUsrCertResp(hSecHandle, iTargetID, 
                                       M_SEC_ERR_GENERAL_ERROR, NULL, 0, NULL, 0);
                Sec_DpHandleCleanUp( hSecHandle );
                Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                SEC_CUR_WIM_RESP = NULL;
                return;
            }
            if(pstWimStCertNamesConnResp->iResult == M_SEC_ERR_WIM_NOT_INITIALISED)
            {
                SEC_STATES++; 
                Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                SEC_CUR_WIM_RESP = NULL;
            }
            else
            {
                switch(pstWimStCertNamesConnResp->iNbrOfCerts)
                {
                    case E_STATE_RESP_UE_CONFIRM:/*0*/
                
                        SEC_WAITING_FOR_UE_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_iUeConfirm(hSecHandle, iTargetID,M_SEC_ID_CONFIRM_2, M_SEC_ID_OK, M_SEC_ID_CANCEL); 
                        SEC_STATES += 5;
                        Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                        SEC_CUR_WIM_RESP = NULL;
                        break;
                    case E_STATE_RESP_GET_USER_CERT_AND_KEY_ID_AGAIN:  /*1 :only one user certificate*/                      
                        iCertId = pstWimStCertNamesConnResp->pstCertNames[0].iCertId;   /*user cert id*/             
                         SEC_CHAIN_DEPTH = (WE_UINT8)pstWimStCertNamesConnResp->pcChainDepth[0];/*cert chain depth*/
                         for (iILoop=0; iILoop<SEC_CHAIN_DEPTH; iILoop++)
                         {
                             SEC_CERT_CHAIN_ID[iILoop] = pstWimStCertNamesConnResp->ppuiCertChainIds[0][iILoop];
                         }
                         Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                         SEC_CUR_WIM_RESP = NULL;
                         SEC_WAITING_FOR_WIM_RESULT = 1;
                         SEC_WANTS_TO_RUN = 0;
                         Sec_iWimGetKeyIdAndUCert(hSecHandle,iCertId, M_SEC_TLS_CONNECTION_TYPE);
                         SEC_STATES+=3;    
                         break;              
                    default:                        
                        SEC_WAITING_FOR_UE_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_iUeSelectCert (hSecHandle,iTargetID, M_SEC_ID_TITLE_CHOOSE_CERT, 
                                           M_SEC_ID_OK, pstWimStCertNamesConnResp->iNbrOfCerts, 
                                           pstWimStCertNamesConnResp->pstCertNames);
                        SEC_STATES+=2;   
                        break;
                }
            }
            break;
            case E_STATE_TLS_VIEW_NAME_OF_USER_CERT_AGAIN: /*3*/
                {
                    WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle, iTargetID, FALSE);
                    switch(iStatus)
                    {
                        case M_SEC_ERR_IN_PROGRESS:
                            break;
                        case M_SEC_ERR_MISSING_KEY:
                    #ifdef M_SEC_CFG_SHOW_WARNINGS
                            Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_MISSING_AUT_KEY_HANDSHAKE);
                    #endif
                            SEC_WAITING_FOR_WIM_RESULT = 1;
                            SEC_WANTS_TO_RUN = 0;
                            ppstWimCertAuthorities = (St_SecTlsDistingubshedName **)&pstCertAuthorities; 
                            Sec_iWimGetTLSUserCertNames(hSecHandle,pucCertificateTypes, iNbrCertificateTypes,
                                                        *ppstWimCertAuthorities, iNumCertAuthorities);
                            SEC_STATES--;
                            break;

                        case M_SEC_ERR_OK:                
                            SEC_WAITING_FOR_WIM_RESULT = 1;
                            SEC_WANTS_TO_RUN = 0;
                            ppstWimCertAuthorities = (St_SecTlsDistingubshedName **)&pstCertAuthorities; 
                            Sec_iWimGetTLSUserCertNames(hSecHandle,pucCertificateTypes, iNbrCertificateTypes,
                                                        *ppstWimCertAuthorities, iNumCertAuthorities);

                            SEC_STATES--;
                            break;
                        case M_SEC_ERR_USER_NOT_VERIFIED:
                            iStatus = M_SEC_ERR_OK;    /* fall through */
                            //lint -fallthrough
                        default:                
                            Sec_TlsGetUsrCertResp(hSecHandle, iTargetID, iStatus, 
                                                   NULL, 0, NULL, 0);
                            Sec_DpHandleCleanUp( hSecHandle );
                            break;
                    }
                }
                break;
            case E_STATE_TLS_GET_USER_CERT_AND_KEY_ID:    /*4*/
                if (SEC_WAITING_FOR_UE_RESULT)
                {
                    return;
                }
                if (!SEC_CUR_UE_RESP)
                {
                    Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);;
                    Sec_DpHandleCleanUp( hSecHandle );
                    Sec_TlsGetUsrCertResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, 
                                           NULL, 0, NULL, 0);
                    return;
                }           
                pstTiStChooseCertByNameResp = ( St_SecTlChooseCertResp *)SEC_CUR_UE_RESP;
                if(pstTiStChooseCertByNameResp->eType != E_SEC_DP_CHOOSE_CERT)
                {
                    Sec_TlsGetUsrCertResp(hSecHandle, iTargetID, \
                                           M_SEC_ERR_GENERAL_ERROR, NULL, 0,NULL, 0);
                    Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                    Sec_TlHandleUeStructFree(SEC_CUR_UE_RESP);
                    Sec_DpHandleCleanUp(hSecHandle );
                    SEC_CUR_UE_RESP = NULL;
                    return;
                }
                if (pstTiStChooseCertByNameResp->eResult == E_SEC_UE_CANCEL)
                {
                    Sec_TlsGetUsrCertResp(hSecHandle, iTargetID, M_SEC_ERR_CANCEL, NULL, 0, NULL, 0);
                    Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                    Sec_TlHandleUeStructFree(SEC_CUR_UE_RESP);
                    Sec_DpHandleCleanUp(hSecHandle );
                    SEC_CUR_UE_RESP = NULL;
                    return;
                }
                else if ((pstTiStChooseCertByNameResp->eResult != E_SEC_UE_OK) || (pstTiStChooseCertByNameResp->iCertId < 1))
                {
                    Sec_TlsGetUsrCertResp(hSecHandle, iTargetID,  M_SEC_ERR_GENERAL_ERROR, NULL, 0, NULL, 0);
                    Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                    Sec_TlHandleUeStructFree(SEC_CUR_UE_RESP);
                    Sec_DpHandleCleanUp(hSecHandle );
                    SEC_CUR_UE_RESP = NULL;
                    return;
                }
                pstWimStCertNamesConnResp = (St_SecTlConnCertNamesResp *)SEC_CUR_WIM_RESP;
                for (iILoop=0; iILoop<pstWimStCertNamesConnResp->iNbrOfCerts; iILoop++)
                {
                  if (pstTiStChooseCertByNameResp->iCertId == pstWimStCertNamesConnResp->pstCertNames[iILoop].iCertId)
                  {
                    SEC_CHAIN_DEPTH = (WE_UINT8)pstWimStCertNamesConnResp->pcChainDepth[iILoop];
                    for (iJLoop=0; iJLoop<SEC_CHAIN_DEPTH; iJLoop++)
                    {
                       SEC_CERT_CHAIN_ID[iJLoop] = pstWimStCertNamesConnResp->ppuiCertChainIds[iILoop][iJLoop];
                    }
                  }
                }
                Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                SEC_CUR_WIM_RESP = NULL;
                SEC_WAITING_FOR_WIM_RESULT = 1;
                SEC_WANTS_TO_RUN = 0;
                Sec_iWimGetKeyIdAndUCert(hSecHandle, pstTiStChooseCertByNameResp->iCertId, 
                                            M_SEC_TLS_CONNECTION_TYPE);
                Sec_TlHandleUeStructFree(SEC_CUR_UE_RESP);
                SEC_CUR_UE_RESP = NULL;
                SEC_STATES++;
                break;
            case E_STATE_TLS_GET_USER_CERT_CHAIN_OR_RESP:    /*5*/
                if(SEC_WAITING_FOR_WIM_RESULT)
                {
                    return;
                }
                if (!SEC_CUR_WIM_RESP)
                {
                    Sec_TlsGetUsrCertResp(hSecHandle, iTargetID,M_SEC_ERR_INSUFFICIENT_MEMORY, 
                                          NULL, 0, NULL, 0);
                    Sec_DpHandleCleanUp( hSecHandle );
                    return;
                }
                pstWimStGetUcertKeyidResp = (St_SecTlGetUcertKeyIdResp *)SEC_CUR_WIM_RESP;
                if(pstWimStGetUcertKeyidResp->eType != E_SEC_DP_GET_USER_CERT)
                {
                    Sec_TlsGetUsrCertResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, 
                                           NULL, 0, NULL, 0);
                }                                
                else if ((pstWimStGetUcertKeyidResp->iCertLen)==0)
                {
                    Sec_TlsGetUsrCertResp(hSecHandle, iTargetID, M_SEC_ERR_MISSING_CERTIFICATE, 
                                           NULL, 0, NULL, 0);
                }                                
                else
                {
                     SEC_ASN1_CERT = (St_SecTlsAsn1Certificate *)WE_MALLOC( 4*sizeof(St_SecTlsAsn1Certificate));
                     if(!SEC_ASN1_CERT)
                     {
                        Sec_TlsGetUsrCertResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, 
                                           NULL, 0, NULL, 0);
                     }
                     SEC_ASN1_CERT[0].uiCertLen = (WE_UINT32)pstWimStGetUcertKeyidResp->iCertLen ;
                     SEC_ASN1_CERT[0].pucCert = (WE_UCHAR *)WE_MALLOC((WE_UINT32)pstWimStGetUcertKeyidResp->iCertLen*sizeof(WE_UCHAR));
                     if(NULL==SEC_ASN1_CERT[0].pucCert )
                     {
                        WE_FREE(SEC_ASN1_CERT);
                        Sec_TlsGetUsrCertResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, 
                                           NULL, 0, NULL, 0);
                     }
                     for (iILoop=0; iILoop<pstWimStGetUcertKeyidResp->iCertLen;iILoop++)
                     {
                         SEC_ASN1_CERT[0].pucCert[iILoop] = pstWimStGetUcertKeyidResp->pucCert[iILoop] ;
                     }
                }  
                if (SEC_CHAIN_DEPTH != 0)
                {
                   SEC_KEY_ID = (WE_UCHAR *)WE_MALLOC((WE_UINT32)pstWimStGetUcertKeyidResp->iKeyIdLen*sizeof(WE_UCHAR));
                   if(!SEC_KEY_ID)
                   {
                      if(SEC_ASN1_CERT[0].pucCert)
                      {
                          WE_FREE(SEC_ASN1_CERT[0].pucCert);
                      }
                      if(SEC_ASN1_CERT)
                      {
                          WE_FREE(SEC_ASN1_CERT);
                          SEC_ASN1_CERT=NULL;
                      }
                       Sec_TlsGetUsrCertResp(hSecHandle, iTargetID,M_SEC_ERR_INSUFFICIENT_MEMORY, 
                                            NULL, 0, NULL, 0);
                      Sec_DpHandleCleanUp( hSecHandle );
                      return;
                   }
                   (WE_VOID)WE_MEMCPY((WE_CHAR *)SEC_KEY_ID, 
                              (const WE_CHAR *) pstWimStGetUcertKeyidResp->pucKeyId, 
                              (WE_UINT32)(pstWimStGetUcertKeyidResp->iKeyIdLen));
                   Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                   SEC_WAITING_FOR_WIM_RESULT = 1;
                   SEC_WANTS_TO_RUN = 0;
                   Sec_iWimGetChainOfCertificates(hSecHandle,SEC_CHAIN_DEPTH, (WE_UINT32 *)SEC_CERT_CHAIN_ID);
                   SEC_STATES++;
                } 
                else
                {
                    Sec_TlsGetUsrCertResp(hSecHandle, iTargetID,M_SEC_ERR_OK,
                                             (pstWimStGetUcertKeyidResp->pucKeyId)+1,20, SEC_ASN1_CERT, 1);
                    Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                    WE_FREE(SEC_ASN1_CERT[0].pucCert);
                    WE_FREE(SEC_ASN1_CERT);
                    Sec_DpHandleCleanUp( hSecHandle );
                }
                break;    
            case E_STATE_TLS_GET_USER_CERT_RESP:  /*6*/
                if (SEC_WAITING_FOR_WIM_RESULT)
                {
                    return;
                }
                if(!SEC_CUR_WIM_RESP)
                {
                    Sec_TlsGetUsrCertResp(hSecHandle, iTargetID,\
                                     M_SEC_ERR_INSUFFICIENT_MEMORY,0, NULL, NULL, 0);                               
                    Sec_DpHandleCleanUp( hSecHandle );
                    return;
                }
                pstWimStTlsGetChainOfCertsResp = (St_SecTlsGetChainOfCertsResp *)SEC_CUR_WIM_RESP;
                if ((pstWimStTlsGetChainOfCertsResp->eType != E_SEC_DP_GET_CERT_CHAIN) || \
                       (pstWimStTlsGetChainOfCertsResp->ppucCerts == NULL))
                {
                    Sec_TlsGetUsrCertResp(hSecHandle, iTargetID,M_SEC_ERR_MISSING_CERTIFICATE, NULL,0, NULL, 0);
                }
                for (iJLoop=1; iJLoop<(pstWimStTlsGetChainOfCertsResp->iNbrOfCerts+1); iJLoop++)
                {
                    pucTmpName = pstWimStTlsGetChainOfCertsResp->ppucCerts[iJLoop-1];
                    SEC_ASN1_CERT[iJLoop].uiCertLen = pstWimStTlsGetChainOfCertsResp->pusCertLen[iJLoop-1]; 
                    SEC_ASN1_CERT[iJLoop].pucCert =
                        (WE_UCHAR *)WE_MALLOC(pstWimStTlsGetChainOfCertsResp->pusCertLen[iJLoop-1]*sizeof(WE_UCHAR));
                    for (iILoop=0; iILoop<pstWimStTlsGetChainOfCertsResp->pusCertLen[iJLoop-1] ;iILoop++)
                    {
                       SEC_ASN1_CERT[iJLoop].pucCert[iILoop] = *pucTmpName;
                       pucTmpName++;
                    }
                }
                Sec_TlsGetUsrCertResp(hSecHandle, iTargetID, M_SEC_ERR_OK,(SEC_KEY_ID)+1, 20,\
                                         SEC_ASN1_CERT, (pstWimStTlsGetChainOfCertsResp->iNbrOfCerts +1));
                for (iJLoop=0; iJLoop<(pstWimStTlsGetChainOfCertsResp->iNbrOfCerts+1); iJLoop++)
                {
                      WE_FREE(SEC_ASN1_CERT[iJLoop].pucCert);
                }
                WE_FREE(SEC_ASN1_CERT);
                WE_FREE(SEC_KEY_ID);
                Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                Sec_DpHandleCleanUp( hSecHandle );
                break;
            case E_STATE_TLS_RESP_HANDLE_UE_CONFIRM:/*7*/
               if (SEC_WAITING_FOR_UE_RESULT)
               {
                  return;
               }
               if (!SEC_CUR_UE_RESP)
               {
                    Sec_TlsGetUsrCertResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0,NULL, 0);
                    Sec_DpHandleCleanUp( hSecHandle );
                    return;
               }
               pstTiStConfirmResp = (St_SecTlConfirmDlgResp *)SEC_CUR_UE_RESP;
               if(pstTiStConfirmResp->eType != E_SEC_DP_CONFIRM_DLG)
               {
                   Sec_TlsGetUsrCertResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, NULL, 0,NULL, 0);
               }
               else if (pstTiStConfirmResp->ucAnswer)    
               {
                   Sec_TlsGetUsrCertResp(hSecHandle, iTargetID, M_SEC_ERR_OK, NULL, 0,NULL, 0);
               }
               else
               {
                   Sec_TlsGetUsrCertResp(hSecHandle, iTargetID, M_SEC_ERR_BAD_CERTIFICATE, NULL, 0,NULL, 0);
               }
               Sec_TlHandleUeStructFree(SEC_CUR_UE_RESP);
               SEC_CUR_UE_RESP=NULL;
               Sec_DpHandleCleanUp( hSecHandle );
               break;
             default :
                 return;
    }
}
/*==================================================================================================
FUNCTION: 
    Sec_VerifyTlsCertChainIm
CREATE DATE:
    2007-01-19
AUTHOR:
    tang
DESCRIPTION:
    verify the server certificate chain.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IO]:Global data handle.  
    WE_INT32 iTargetID[IN]:ID of the object.
    WE_INT32 iBufLen[IN]:The length of the buffer.
    WE_UCHAR * pucAddr[IN]:Pointer to the address.
    WE_INT32 iAddrLen[IN]:The length of the addr.
    WE_CHAR * pcLabel[IN]:Pointer to the label.
    WE_CHAR * pucDomain[IN]:Pointer to the domain.
    WE_INT32 iDomainLen[IN]:The length of the domain.
    const St_SecTlsAsn1Certificate *pstCerts[IN]:pointer to the certificate.
    WE_UINT8 ucNumCerts[IN}:number of cert.
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
void Sec_VerifyTlsCertChainIm (WE_HANDLE hSecHandle, WE_INT32 iTargetID, const WE_UCHAR * pucAddr, 
                               WE_UINT16 usAddrLen,  const WE_UCHAR * pucDomain, WE_UINT16 usDomainLen,
                               const St_SecTlsAsn1Certificate *pstCerts,WE_UINT8 ucNumCerts)
{
    WE_UCHAR              *pucCommonName = NULL;
    WE_INT32              iCommonNameLen = 0;
    WE_INT32              iRes = 0;
    WE_UCHAR              *pucDnsName = NULL; 
    WE_UINT16             usDnsNameLen = 0;
    WE_UCHAR              *pucIpAddress = NULL; 
    WE_UINT16             usIpAddressLen = 0;
    St_SecTlVerifyCertChainResp      *pstWimStVerifyCertChainResp=NULL;
    St_SecTlConfirmDlgResp           *pstUeStConfirmEvtResp = NULL; 
    St_SecTlsAsn1Certificate         **ppstWimCertificates = NULL;

    if (NULL == hSecHandle||NULL==pstCerts)
    {
        return ;
    }
    
    switch (SEC_STATES)
    {    
    case   E_STATE_TLS_VERIFY_SVR_CERT_DIRECT_RETURN: /*0*/
        return;
    case   E_STATE_TLS_VERIFY_SVR_CERT_CHAIN_COMNAME: /*1*/
        if ((pucAddr != NULL) || (pucDomain != NULL))     
        { 
            if ((((*pstCerts).pucCert) == NULL) || (((*pstCerts).uiCertLen) == 0))
            {
                Sec_TlsVerifySvrCertResp(hSecHandle, iTargetID,M_SEC_ERR_INVALID_PARAMETER);
                Sec_DpHandleCleanUp( hSecHandle );
                return; 
            }
            /*tls only support x509 cert in this Pro*/
            iRes = Sec_GetX509ComName(hSecHandle,(const WE_UCHAR *)(pstCerts[0].pucCert), \
                                     &pucCommonName,&iCommonNameLen,\
                                     &pucDnsName,&usDnsNameLen,\
                                     &pucIpAddress,&usIpAddressLen);
            if (iRes != M_SEC_ERR_OK)
            {
                Sec_TlsVerifySvrCertResp(hSecHandle, iTargetID, iRes);
                if (pucCommonName)
                {
                    WE_FREE(pucCommonName);
                }
                Sec_DpHandleCleanUp( hSecHandle );
                return;
            }
            if ((pucCommonName != NULL))
            {
                iRes= Sec_ExamineComName(pucAddr, usAddrLen,  pucDomain, usDomainLen, \
                    pucCommonName,iCommonNameLen);
                if (iRes != 0)
                {                  
                    SEC_WAITING_FOR_UE_RESULT = 1;
                    SEC_WANTS_TO_RUN = 0;                    
                    Sec_iUeConfirm(hSecHandle,iTargetID,M_SEC_ID_CONFIRM_1, M_SEC_ID_OK, M_SEC_ID_CANCEL);
                    WE_FREE(pucCommonName);
                    SEC_STATES++;                                                    
                    return;
                }
                else 
                {
                    SEC_STATES+=2; 
                }
            }
            else
            {
                SEC_STATES+=2; 
            }
            if (pucCommonName)
            {
                WE_FREE(pucCommonName);
            }
        } 
        else
        {
            SEC_STATES+=2; 
        }
        break;
    case   E_STATE_TLS_VERIFY_SVR_CERT_RESP: /*2*/
        if (SEC_WAITING_FOR_UE_RESULT)
        {
            return;
        }
        if (!SEC_CUR_UE_RESP)
        {
            Sec_TlsVerifySvrCertResp(hSecHandle, iTargetID,  M_SEC_ERR_INSUFFICIENT_MEMORY);
            Sec_DpHandleCleanUp( hSecHandle );
            return;
        }
        pstUeStConfirmEvtResp = (St_SecTlConfirmDlgResp *)SEC_CUR_UE_RESP;
        if(pstUeStConfirmEvtResp->eType != E_SEC_DP_CONFIRM_DLG)
        {
            Sec_TlsVerifySvrCertResp(hSecHandle, iTargetID,  M_SEC_ERR_GENERAL_ERROR);
            Sec_TlHandleUeStructFree(SEC_CUR_UE_RESP);
            SEC_CUR_UE_RESP = NULL;
            Sec_DpHandleCleanUp( hSecHandle );
            return;
        }
        if (!(pstUeStConfirmEvtResp->ucAnswer))                                    
        {
            Sec_TlsVerifySvrCertResp(hSecHandle, iTargetID,  M_SEC_ERR_NO_MATCHING_ROOT_CERTIFICATE);
            Sec_DpHandleCleanUp( hSecHandle );
        }
        else                            
        {
            SEC_STATES++;
        }
        Sec_TlHandleUeStructFree(SEC_CUR_UE_RESP);
        SEC_CUR_UE_RESP = NULL;
        break;            
    case   E_STATE_TLS_VERIFY_SVR_CERT_CHAIN: /*3*/
        SEC_WAITING_FOR_WIM_RESULT = 1;
        SEC_WANTS_TO_RUN = 0;
        ppstWimCertificates = ( St_SecTlsAsn1Certificate **)&pstCerts; 
        Sec_iWimVerifyTLSCertificateChain(hSecHandle,iTargetID,  *ppstWimCertificates, ucNumCerts);
        SEC_STATES++;
        break;
    case   E_STATE_TLS_VERIFY_SVR_CERT_CHAIN_RESP:/*4*/
        if (SEC_WAITING_FOR_WIM_RESULT)
        {
            return;
        }
        if (!SEC_CUR_WIM_RESP)
        {
            Sec_TlsVerifySvrCertResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
            Sec_DpHandleCleanUp( hSecHandle );
            return;
        }
        pstWimStVerifyCertChainResp = (St_SecTlVerifyCertChainResp *)SEC_CUR_WIM_RESP;
        if(pstWimStVerifyCertChainResp->eType != E_SEC_DP_VERIFY_CERT_CHAIN)
        {
            Sec_TlsVerifySvrCertResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR);
            Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
            SEC_CUR_WIM_RESP = NULL;            
            Sec_DpHandleCleanUp( hSecHandle );
            return;
        }
        if(pstWimStVerifyCertChainResp->iResult == M_SEC_ERR_WIM_NOT_INITIALISED)
        {
            SEC_STATES++; 
        }
        else if ((pstWimStVerifyCertChainResp->iResult == M_SEC_ERR_OLD_ROOTCERT_FOUND) && (SEC_USER_ASKED == 0 ))
        {    
            SEC_TOO_OLD_CERT_ID = pstWimStVerifyCertChainResp->iCertTooOld;
            SEC_STATES+=2;
            SEC_USER_ASKED =1;
        }
#ifdef M_SEC_CFG_CONN_WITHOUT_CERT_UE        
        else if (pstWimStVerifyCertChainResp->iResult != M_SEC_ERR_OK)
        {
            SEC_WAITING_FOR_UE_RESULT = 1;
            SEC_WANTS_TO_RUN = 0;
            if (pstWimStVerifyCertChainResp->iResult == M_SEC_ERR_NO_MATCHING_ROOT_CERTIFICATE)
            {
                Sec_iUeConfirm(hSecHandle, iTargetID,M_SEC_ID_CONFIRM_1, M_SEC_ID_OK, M_SEC_ID_CANCEL);
            }
            else if (pstWimStVerifyCertChainResp->iResult == M_SEC_ERR_CERTIFICATE_EXPIRED)
            {
                Sec_iUeConfirm(hSecHandle, iTargetID,M_SEC_ID_CONFIRM_1, M_SEC_ID_OK, M_SEC_ID_CANCEL);
            }
            else
            {
                Sec_iUeConfirm(hSecHandle, iTargetID,M_SEC_ID_CONFIRM_1,  M_SEC_ID_OK, M_SEC_ID_CANCEL);    
            }
            SEC_STATES += 3;                                                    
        }
#endif 
        else
        {
            Sec_TlsVerifySvrCertResp(hSecHandle, pstWimStVerifyCertChainResp->iTargetID, pstWimStVerifyCertChainResp->iResult);
            Sec_DpHandleCleanUp( hSecHandle );
        }
        Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
        SEC_CUR_WIM_RESP = NULL;
        break;
    case   E_STATE_TLS_VERIFY_SVR_CERT_CHAIN_AGAIN:/*5*/
        {
            WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle,iTargetID, FALSE);
            switch(iStatus)
            {
            case M_SEC_ERR_IN_PROGRESS:
                break;
            case M_SEC_ERR_MISSING_KEY:
#ifdef M_SEC_CFG_SHOW_WARNINGS
                SECa_tiWarningDialog(SEC_ERR_MISSING_AUT_KEY_HANDSHAKE);
#endif
                SEC_WAITING_FOR_WIM_RESULT = 1;
                SEC_WANTS_TO_RUN = 0;
                ppstWimCertificates = ( St_SecTlsAsn1Certificate **)&pstCerts; 
                Sec_iWimVerifyTLSCertificateChain(hSecHandle,iTargetID, *ppstWimCertificates, ucNumCerts);
                SEC_STATES--;
                break;
            case M_SEC_ERR_OK:                
                SEC_WAITING_FOR_WIM_RESULT = 1;
                SEC_WANTS_TO_RUN = 0;
                ppstWimCertificates = ( St_SecTlsAsn1Certificate **)& pstCerts; 
                Sec_iWimVerifyTLSCertificateChain(hSecHandle,iTargetID, *ppstWimCertificates, ucNumCerts);
                SEC_STATES--;
                break;
            case M_SEC_ERR_USER_NOT_VERIFIED:
                iStatus = M_SEC_ERR_OK;
            default:                
                Sec_TlsVerifySvrCertResp(hSecHandle, iTargetID, iStatus);
                Sec_DpHandleCleanUp( hSecHandle );
                break;
            }
        }
        break;
    case    E_STATE_TLS_HANDLE_OLD_ROOT_CERT : /*6*/
        {
            WE_INT32 iStatus = Sec_HandleOldRootCert(hSecHandle,iTargetID,SEC_TOO_OLD_CERT_ID);
            switch(iStatus)
            {
            case M_SEC_ERR_IN_PROGRESS:
                break;
            case M_SEC_ERR_OK:
                SEC_WAITING_FOR_WIM_RESULT = 1;
                SEC_WANTS_TO_RUN = 0;
                ppstWimCertificates = ( St_SecTlsAsn1Certificate **)& pstCerts; 
                Sec_iWimVerifyTLSCertificateChain(hSecHandle,iTargetID, *ppstWimCertificates, ucNumCerts);
                SEC_STATES-=2;
                break;
            default:
                SEC_WAITING_FOR_WIM_RESULT = 1;
                SEC_WANTS_TO_RUN = 0;
                ppstWimCertificates = ( St_SecTlsAsn1Certificate **)& pstCerts; 
                Sec_iWimVerifyTLSCertificateChain(hSecHandle,iTargetID, *ppstWimCertificates, ucNumCerts);
                SEC_STATES-=2;
                break;
            }
        }
        break;
#ifdef M_SEC_CFG_CONN_WITHOUT_CERT_UE
    case   E_STATE_TLS_HANDLE_UE_CONFIRM:/*7*/
        if (SEC_WAITING_FOR_UE_RESULT)
        {
            return;
        }
        if (!SEC_CUR_UE_RESP)
        {
            Sec_TlsVerifySvrCertResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
            Sec_DpHandleCleanUp( hSecHandle );
            return;
        }
        pstUeStConfirmEvtResp = ( St_SecTlConfirmDlgResp *)SEC_CUR_UE_RESP;
        if(pstUeStConfirmEvtResp->eType != E_SEC_DP_CONFIRM_DLG)
        {
            Sec_TlsVerifySvrCertResp(hSecHandle, iTargetID,  M_SEC_ERR_GENERAL_ERROR);
            Sec_DpHandleCleanUp( hSecHandle );
            Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
            SEC_CUR_WIM_RESP = NULL;
            return;
        }
        if (!(pstUeStConfirmEvtResp->ucAnswer))                                    
        {
            Sec_TlsVerifySvrCertResp(hSecHandle, iTargetID, M_SEC_ERR_BAD_CERTIFICATE);
        }
        else
        {
            Sec_TlsVerifySvrCertResp(hSecHandle, iTargetID, M_SEC_ERR_OK);
        }
        SEC_DIALOG_SHOWN = 1;
        Sec_DpHandleCleanUp( hSecHandle );
        Sec_TlHandleUeStructFree(SEC_CUR_UE_RESP);
        SEC_CUR_UE_RESP = NULL;
        break;
#endif 
    default :  
        return;
        }
    }    

/*==================================================================================================
FUNCTION: 
    Sec_CompTlsSignatureIm
CREATE DATE:
    2007-01-19
AUTHOR:
    tang
DESCRIPTION:
    compute signature implementation.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IO]:Global data handle.  
    WE_INT32 iTargetID[IN]:ID of the object.
    const WE_UCHAR * pucPubKeyHash[IN]:public key hash.
    WE_UINT16  usPubKeyHashLen[IN]:length of public key hash.
    const WE_UCHAR *pucData[IN]:pointer to the data to be signature.
    WE_UINT16 usDataLen[IN]:length of data.
    WE_UCHAR  ucAlg[IN]:rsa.
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

void Sec_CompTlsSignatureIm (WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                            const WE_UCHAR * pucPubKeyHash,WE_UINT16  usPubKeyHashLen,
                            const WE_UCHAR *pucData,WE_UINT16 usDataLen,WE_UCHAR  ucAlg)
{
    St_SecTlComputeSignResp *pstWimStComputeSigResp=NULL;
    WE_INT32                iIIndex=0;
    WE_INT32                iKeyIdLen = 1+20;
    WE_UCHAR                *pucKeyId = NULL;

    if (NULL == hSecHandle)
    {
        return ;
    }
    usPubKeyHashLen=usPubKeyHashLen;
    switch (SEC_STATES)
    {
    case   E_STATE_TLS_COMPUTE_SIGN_DIRECT_RETURN:/*0*/
        return; 
    case   E_STATE_TLS_COMPUTE_SIGN: /*1*/
        if (ucAlg == E_SEC_TLS_KEYEXCH_RSA)
        {
            SEC_TLS_SIGN_ALG = M_SEC_RSA_SIGN_CERT;
        }
#ifdef M_SEC_CFG_DHE_DSS
        else if (ucAlg == E_SEC_TLS_KEYEXCH_DHE_DSS)
        {
            SEC_TLS_SIGN_ALG = M_SEC_DSS_SIGN_CERT;
        }
#endif
        else
        {
            Sec_TlsCompSigResp(hSecHandle, iTargetID, M_SEC_ERR_INVALID_PARAMETER, NULL, 0);
            Sec_DpHandleCleanUp(hSecHandle);
            return;
        }
        if(!pucPubKeyHash)
        {
            Sec_TlsCompSigResp(hSecHandle, iTargetID, M_SEC_ERR_INVALID_PARAMETER, NULL, 0);
            Sec_DpHandleCleanUp(hSecHandle);
            return ;
        }
        SEC_WAITING_FOR_WIM_RESULT = 1;
        SEC_WANTS_TO_RUN = 0;
        pucKeyId= (WE_UCHAR *)WE_MALLOC((WE_UINT32)iKeyIdLen*sizeof(WE_UCHAR));
        if(!pucKeyId)
        {
            Sec_TlsCompSigResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0);
            Sec_DpHandleCleanUp(hSecHandle);
            return ;
        }
        pucKeyId[0] = 254;
        for (iIIndex=0; iIIndex<20; iIIndex++)/*usPubKeyHashLen=20*/
        {
            pucKeyId[iIIndex+1] = pucPubKeyHash[iIIndex];
        }
        Sec_iWimCalSign(hSecHandle,iTargetID, pucKeyId, iKeyIdLen, pucData, usDataLen, (WE_UINT8)SEC_TLS_SIGN_ALG);
        WE_FREE(pucKeyId);
        SEC_STATES++;
        break;
    case  E_STATE_TLS_COMPUTE_SIGN_RESP:/*2*/
        if (SEC_WAITING_FOR_WIM_RESULT)
        {
            return;
        }
        if (!SEC_CUR_WIM_RESP)
        {
            Sec_TlsCompSigResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0);
            Sec_DpHandleCleanUp(hSecHandle);
            return;
        }
        pstWimStComputeSigResp = ( St_SecTlComputeSignResp *)SEC_CUR_WIM_RESP;
        if(pstWimStComputeSigResp->eType != E_SEC_DP_COMPUTE_SIGN)    
        {
            Sec_TlsCompSigResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, NULL, 0);
            Sec_DpHandleCleanUp(hSecHandle);
            Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
            SEC_CUR_WIM_RESP = NULL;
            return;
        }
        if(pstWimStComputeSigResp->iResult == M_SEC_ERR_WIM_NOT_INITIALISED)
        {
            SEC_STATES++; 
        }
        else
        {
            Sec_TlsCompSigResp(hSecHandle, pstWimStComputeSigResp->iTargetID,\
                               pstWimStComputeSigResp->iResult, pstWimStComputeSigResp->pucSig, pstWimStComputeSigResp->iSigLen);
            Sec_DpHandleCleanUp(hSecHandle);
        }
        Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
        SEC_CUR_WIM_RESP = NULL;
        break;
    case   E_STATE_TLS_COMPUTE_SIGN_AGAIN:/*3*/
        {
            WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle,iTargetID, FALSE);
            switch(iStatus)
            {
            case M_SEC_ERR_IN_PROGRESS:
                break;
            case M_SEC_ERR_MISSING_KEY:
#ifdef M_SEC_CFG_SHOW_WARNINGS
                SECa_tiWarningDialog(SEC_ERR_MISSING_AUT_KEY_HANDSHAKE);
#endif
                if(!pucPubKeyHash)
                {
                    Sec_TlsCompSigResp(hSecHandle, iTargetID, M_SEC_ERR_INVALID_PARAMETER, NULL, 0);
                    Sec_DpHandleCleanUp(hSecHandle);
                    return ;
                }
                SEC_WAITING_FOR_WIM_RESULT = 1;
                SEC_WANTS_TO_RUN = 0;
                pucKeyId = (WE_UCHAR *)WE_MALLOC((WE_UINT32)iKeyIdLen*sizeof( WE_UCHAR));
                if(!pucKeyId)
                {
                    Sec_TlsCompSigResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0);
                    Sec_DpHandleCleanUp(hSecHandle);
                    return ;
                }
                pucKeyId[0] = 254;
                for (iIIndex=0; iIIndex<20; iIIndex++)
                {
                    pucKeyId[iIIndex+1] = pucPubKeyHash[iIIndex];
                }
                Sec_iWimCalSign(hSecHandle,iTargetID, pucKeyId, iKeyIdLen, pucData, usDataLen, (WE_UINT8)SEC_TLS_SIGN_ALG);
                WE_FREE(pucKeyId);
                SEC_STATES--;
                break;
            case M_SEC_ERR_OK: 
                if(!pucPubKeyHash)
                {
                    Sec_TlsCompSigResp(hSecHandle, iTargetID, M_SEC_ERR_INVALID_PARAMETER, NULL, 0);
                    Sec_DpHandleCleanUp(hSecHandle);
                    return ;
                }
                SEC_WAITING_FOR_WIM_RESULT = 1;
                SEC_WANTS_TO_RUN = 0;
                pucKeyId = (WE_UCHAR *)WE_MALLOC((WE_UINT32)iKeyIdLen*sizeof( WE_UCHAR));
                if(!pucKeyId)
                {
                    Sec_TlsCompSigResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0);
                    Sec_DpHandleCleanUp(hSecHandle);
                    return ;
                }
                pucKeyId[0] = 254;
                for (iIIndex=0; iIIndex<20; iIIndex++)
                {
                    pucKeyId[iIIndex+1] = pucPubKeyHash[iIIndex];
                }
                Sec_iWimCalSign(hSecHandle,iTargetID, pucKeyId, iKeyIdLen,  pucData, usDataLen, (WE_UINT8)SEC_TLS_SIGN_ALG);
                WE_FREE(pucKeyId);
                SEC_STATES--;
                break;
            case M_SEC_ERR_USER_NOT_VERIFIED:
                iStatus = M_SEC_ERR_OK;
                break;
            default:                
                Sec_TlsCompSigResp(hSecHandle, iTargetID, iStatus, NULL, 0);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
        }
        break;
    default : 
        return;
    }
}
/*==================================================================================================
FUNCTION: 
    Sec_TlsKeyExchangeIm
CREATE DATE:
    2007-01-19
AUTHOR:
    tang
DESCRIPTION:
    exchange key in tls
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IO]:Global data handle.  
    WE_INT32 iTargetID[IN]:ID of the object.
    WE_UINT8 ucKeyExchangeAlgorithm[IN]:the algorithm of key exchange.
    WE_INT32  iOptions[IN]:the option of connection to decide use tls prf or ssl prf.
    St_SecTlsKeyExchangeParams *pstParam[IN]:pointer to the St_SecTlsKeyExchangeParams.
    const WE_UCHAR *pucRandval[IN]:random number.
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
WE_VOID Sec_TlsKeyExchangeIm (WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                              WE_UINT8 ucKeyExchangeAlgorithm, WE_INT32  iOptions,
                              St_SecTlsKeyExchangeParams *pstParam,  
                              const WE_UCHAR *pucRandval)
{
    St_SecTlsTlKeyXchResp              *pstWimStKeyexchResp=NULL;
    St_SecTlsKeyExchangeParams         **ppstTlsKeyExchParams=NULL;

    if (NULL == hSecHandle)
    {
        return ;
    }    
    switch (SEC_STATES)
    {
    case   E_STATE_TLS_KEY_EXCHANGE_DIRECT_RETURN:/*0*/
        return;    
    case   E_STATE_GET_TLS_KEY_EXCHANGE:/*1*/
        SEC_WAITING_FOR_WIM_RESULT = 1;
        SEC_WANTS_TO_RUN = 0;
        ppstTlsKeyExchParams = (St_SecTlsKeyExchangeParams **)&pstParam; 
        Sec_iWimTLSkeyExchange(hSecHandle,iTargetID,ucKeyExchangeAlgorithm,iOptions,*ppstTlsKeyExchParams, pucRandval);
        SEC_STATES++;
        break;
    case   E_STATE_GET_TLS_KEY_EXCHANGE_RESP:/*2*/
        if (SEC_WAITING_FOR_WIM_RESULT)
        {
            return;
        }
        if (!SEC_CUR_WIM_RESP)
        {
            Sec_KeyExchangeResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, 0  , NULL, 0);
            Sec_DpHandleCleanUp(hSecHandle);
            return;
        }
        pstWimStKeyexchResp = (St_SecTlsTlKeyXchResp *)SEC_CUR_WIM_RESP;
        if(pstWimStKeyexchResp->eType != E_SEC_DP_XCH_KEY)
        {
            Sec_KeyExchangeResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, 0, NULL, 0);
            Sec_DpHandleCleanUp(hSecHandle);
            Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
            SEC_CUR_WIM_RESP = NULL;
            return;
        }
        if(pstWimStKeyexchResp->iResult == M_SEC_ERR_WIM_NOT_INITIALISED)
        {
            SEC_STATES++; 
        }
        else if (pstWimStKeyexchResp->iResult == M_SEC_ERR_OK)
        {
            Sec_KeyExchangeResp(hSecHandle, pstWimStKeyexchResp->iTargetID, pstWimStKeyexchResp->iResult, 
                pstWimStKeyexchResp->iMasterSecretId, 
                pstWimStKeyexchResp->pucPublicValue, pstWimStKeyexchResp->iPublicValueLen);
            Sec_DpHandleCleanUp(hSecHandle);
        }
        else
        {
            Sec_KeyExchangeResp(hSecHandle, pstWimStKeyexchResp->iTargetID, pstWimStKeyexchResp->iResult, 
                pstWimStKeyexchResp->iMasterSecretId, 
                pstWimStKeyexchResp->pucPublicValue, pstWimStKeyexchResp->iPublicValueLen);
            Sec_DpHandleCleanUp(hSecHandle);
        }
        Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
        SEC_CUR_WIM_RESP = NULL;
        break;
    case   E_STATE_GET_TLS_KEY_EXCHANGE_AGAIN:/*3*/
        {
            WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle,iTargetID, FALSE);
            switch(iStatus)
            {
            case M_SEC_ERR_IN_PROGRESS:
                break;
            case M_SEC_ERR_MISSING_KEY:
#ifdef M_SEC_CFG_SHOW_WARNINGS
                SECa_tiWarningDialog(SEC_ERR_MISSING_AUT_KEY_HANDSHAKE);
#endif
                SEC_WAITING_FOR_WIM_RESULT = 1;
                SEC_WANTS_TO_RUN = 0;
                ppstTlsKeyExchParams = (St_SecTlsKeyExchangeParams **)&pstParam; 
                Sec_iWimTLSkeyExchange(hSecHandle,iTargetID, ucKeyExchangeAlgorithm,  
                     iOptions, *ppstTlsKeyExchParams, pucRandval);
                SEC_STATES--;
                break;
            case M_SEC_ERR_OK:                    
                SEC_WAITING_FOR_WIM_RESULT = 1;
                SEC_WANTS_TO_RUN = 0;
                ppstTlsKeyExchParams = (St_SecTlsKeyExchangeParams **)&pstParam; 
                Sec_iWimTLSkeyExchange(hSecHandle,iTargetID, ucKeyExchangeAlgorithm,  
                    iOptions,  *ppstTlsKeyExchParams, pucRandval) ;
                SEC_STATES--;
                break;
            case M_SEC_ERR_USER_NOT_VERIFIED:
                iStatus = M_SEC_ERR_OK;
            default:                
                Sec_KeyExchangeResp(hSecHandle, iTargetID, iStatus, 0, NULL, 0);
                Sec_DpHandleCleanUp(hSecHandle);
                break;
            }
        }
        break;
    default :
        return;
    }
}
/*==================================================================================================
FUNCTION: 
    Sec_GetTlsPrfResultIm
CREATE DATE:
    2007-01-19
AUTHOR:
    tang
DESCRIPTION:
    get prf function result.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IO]:Global data handle.  
    WE_INT32 iTargetID[IN]:ID of the object.
    WE_UINT8 ucAlg[IN]:Value of the alg.
    WE_INT32 iMasterSecretId[IN]:ID of the master secret.
    WE_UCHAR * pucSecret[IN]:Pointer to the secret.
    WE_INT32 iSecretLen[IN]:The length of the secret.
    WE_CHAR * pcLabel[IN]:The pointer to the lable.
    WE_UCHAR * pucSeed[IN]:Pointer to the seed.
    WE_INT32 iSeedLen[IN]:The length of the seed.
    WE_INT32 iOutputLen[IN]:The length of the output data.
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
void Sec_GetTlsPrfResultIm (WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_UINT8 ucAlg, WE_INT32 iMasterSecretId,
                        const WE_UCHAR * pucSecret, WE_INT32 iSecretLen, const WE_CHAR * pcLabel,
                        const WE_UCHAR * pucSeed, WE_INT32 iSeedLen, WE_INT32 iOutputLen)
{
        St_SecTlPrfResp *pstSecWimStPrfResp=NULL;
        if (NULL == hSecHandle)
        {
            return ;
        }
        
        switch (SEC_STATES)
        {
        case   E_STATE_TLS_PRF_RESULT_DIRECT_RETURN:/*0*/
            return;
        case   E_STATE_TLS_GET_PRF_RESULT:    /*1*/
            SEC_WAITING_FOR_WIM_RESULT = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_iWimGetPrfResult(hSecHandle,iTargetID, iMasterSecretId, 
                                    pucSecret, iSecretLen, pcLabel,
                                    pucSeed, iSeedLen, iOutputLen,
                                    M_SEC_TLS_CONNECTION_TYPE, ucAlg);
            SEC_STATES++;
            break;
        case   E_STATE_TLS_GET_PRF_RESULT_RESP:/*2*/
            if (SEC_WAITING_FOR_WIM_RESULT)
                return;
            if (!SEC_CUR_WIM_RESP)
            {
                Sec_GetPrfResultResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, NULL, 0);
                Sec_DpHandleCleanUp(hSecHandle);
                return;
            }
            pstSecWimStPrfResp = (St_SecTlPrfResp *)SEC_CUR_WIM_RESP;
            if(pstSecWimStPrfResp->eType != E_SEC_DP_PRF)
            {
                Sec_GetPrfResultResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, NULL, 0);
                Sec_DpHandleCleanUp(hSecHandle);
                Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
                SEC_CUR_WIM_RESP = NULL;
                return;
            }
            if(pstSecWimStPrfResp->iResult == M_SEC_ERR_WIM_NOT_INITIALISED)
                SEC_STATES++; 
            else
            {
                Sec_GetPrfResultResp(hSecHandle, pstSecWimStPrfResp->iTargetID, 
                    pstSecWimStPrfResp->iResult, pstSecWimStPrfResp->pucBuf, pstSecWimStPrfResp->iBufLen);
                Sec_DpHandleCleanUp(hSecHandle);
            }
            Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
            SEC_CUR_WIM_RESP = NULL;
            break;
        case   E_STATE_TLS_GET_PRF_RESULT_AGAIN:/*3*/
            {
                WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle,iTargetID, FALSE);
                switch(iStatus)
                {
                case M_SEC_ERR_IN_PROGRESS:
                    break;
                case M_SEC_ERR_MISSING_KEY:
#ifdef M_SEC_CFG_SHOW_WARNINGS
                    Sec_iUeWarning(hSecHandle,iTargetID,M_SEC_ERR_MISSING_AUT_KEY_HANDSHAKE);
#endif
                    SEC_WAITING_FOR_WIM_RESULT = 1;
                    SEC_WANTS_TO_RUN = 0;
                    Sec_iWimGetPrfResult(hSecHandle,iTargetID, iMasterSecretId, 
                                pucSecret, iSecretLen, pcLabel, pucSeed, 
                                iSeedLen, iOutputLen,M_SEC_TLS_CONNECTION_TYPE, ucAlg);
                    SEC_STATES--;
                    break;
                case M_SEC_ERR_OK:
                    
                    SEC_WAITING_FOR_WIM_RESULT = 1;
                    SEC_WANTS_TO_RUN = 0;
                    Sec_iWimGetPrfResult(hSecHandle,iTargetID, iMasterSecretId, 
                        pucSecret, iSecretLen, pcLabel, pucSeed, 
                        iSeedLen, iOutputLen,M_SEC_TLS_CONNECTION_TYPE, ucAlg);
                    SEC_STATES--;
                    break;
                case M_SEC_ERR_USER_NOT_VERIFIED:
                    iStatus = M_SEC_ERR_OK;
                default:                    
                    Sec_GetPrfResultResp(hSecHandle, iTargetID, iStatus, NULL, 0);
                    Sec_DpHandleCleanUp(hSecHandle);
                    break;
                }
            }
            break;
        default : 
            return;
        }
 }

#endif

/*==================================================================================================
FUNCTION: 
    Sec_SetupConIm
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    set up session connection implementation.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IO]:Global data handle.  
    St_SecSetupConnection * pstPara[IN]:Pointer to the pata.
    
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
void Sec_SetupConIm (WE_HANDLE hSecHandle, St_SecSetupConnection * pstPara)
{
    WE_INT32 iRes = 0;
    
    if ((NULL == hSecHandle) || (NULL == pstPara))
    {
        return ;
    }
    
    if (1 == SEC_STATES) 
    {
        if (Sec_WimAddInfo( hSecHandle, 
                            (WE_INT32)pstPara->ucMasterSecretId, 
                            pstPara->iSecurity_id, 
                            (WE_INT32)pstPara->ucFull_handshake, 
                            &pstPara->stSessionInfo) != M_SEC_ERR_OK)
        {
            iRes = Sec_WimStopInfo(hSecHandle);
            if (iRes != M_SEC_ERR_OK)
            {
                return;
            }
        }
        Sec_DpHandleCleanUp( hSecHandle );
    }
    else
    {
        return;
    }
}

/*==================================================================================================
FUNCTION: 
    Sec_StopCon
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    stop the session connection implementation.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.  
    WE_INT32 iTargetID[IN]:ID of the oject.
    WE_INT32 iSecurityId[IN]:ID of the security.
    WE_UINT8 ucConnectionType[IN]: The value of the connection type.
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
void Sec_StopCon (WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iSecurityId, WE_UINT8 ucConnectionType)
{
    if (NULL == hSecHandle)
    {
        return ;
    }
    
    if (1 == SEC_STATES)
    {
        WE_UNUSED_PARAMETER(iTargetID);
        WE_UNUSED_PARAMETER(iSecurityId);
        WE_UNUSED_PARAMETER(ucConnectionType);
        Sec_DpHandleCleanUp( hSecHandle );
    }
    else
    {
        return;
    }
}

/*==================================================================================================
FUNCTION: 
    Sec_RemovePeer
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    remove peer from one session based on mastersecret id.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.  
    WE_INT32 iMasterSecretId[IN]:ID of the master secret
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
void Sec_RemovePeer (WE_HANDLE hSecHandle, WE_INT32 iTargetID,WE_INT32 iMasterSecretId)
{
    St_SecTlRmPeerLinksResp *pstWimStPeerDeleteLinksResp = NULL;

    if (NULL == hSecHandle)
    {
        return ;
    }
    
    switch(SEC_STATES)
    {
        case E_STATE_REMOVE_PEER_DIRECT_RETURN:    
            return;
        case E_STATE_PEER_REMOVE:    
            SEC_WAITING_FOR_WIM_RESULT = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_WimPeerRemove(hSecHandle, iMasterSecretId);
            SEC_STATES++;
            break;
        case E_STATE_HANDLE_PEER_REMOVE_RESULT:
            if (SEC_WAITING_FOR_WIM_RESULT)
            {
                return;
            }
            if (!SEC_CUR_WIM_RESP)
            {
            #ifdef M_SEC_CFG_SHOW_WARNINGS
                Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
            #endif
                Sec_DpHandleCleanUp( hSecHandle );
                return;
            }
            pstWimStPeerDeleteLinksResp = ( St_SecTlRmPeerLinksResp *)SEC_CUR_WIM_RESP;
            if(pstWimStPeerDeleteLinksResp->eType != E_SEC_DP_RM_PEER_LINKS)
            {
            #ifdef M_SEC_CFG_SHOW_WARNINGS
                Sec_iUeWarning(hSecHandle, iTargetID,M_SEC_ERR_GENERAL_ERROR);
            #endif
                Sec_DpHandleCleanUp( hSecHandle );
            }
            else if(pstWimStPeerDeleteLinksResp->iResult == M_SEC_ERR_WIM_NOT_INITIALISED)
            {
                SEC_STATES++; 
            }
            else
            {
            #ifdef M_SEC_CFG_SHOW_WARNINGS
                if (pstWimStPeerDeleteLinksResp->iResult != M_SEC_ERR_OK)
                {
                    Sec_iUeWarning(hSecHandle,iTargetID, pstWimStPeerDeleteLinksResp->iResult);
                }
            #endif
                Sec_DpHandleCleanUp( hSecHandle );
            }
            Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
            SEC_CUR_WIM_RESP = NULL;
            break;
        case E_STATE_PEER_REMOVE_AGAIN: 
            {
                WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle, 1, FALSE);
                switch(iStatus)
                {
                    case M_SEC_ERR_IN_PROGRESS:
                        break;
                    case M_SEC_ERR_MISSING_KEY:
                    #ifdef M_SEC_CFG_SHOW_WARNINGS
                        Sec_iUeWarning(hSecHandle, iTargetID,M_SEC_ERR_MISSING_AUT_KEY_HANDSHAKE);
                    #endif
                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_WimPeerRemove(hSecHandle, iMasterSecretId);
                        SEC_STATES--;
                        break;
                    case M_SEC_ERR_OK:                
                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_WimPeerRemove(hSecHandle, iMasterSecretId);
                        SEC_STATES--;
                        break;
                    case M_SEC_ERR_USER_NOT_VERIFIED:
                    default:
                        
                    #ifdef M_SEC_CFG_SHOW_WARNINGS
                        Sec_iUeWarning(hSecHandle,iTargetID, iStatus);
                    #endif
                        Sec_DpHandleCleanUp( hSecHandle );
                        break;
                }
            }
            break;
        default:
            return;
    } 
    iTargetID = iTargetID;
}
/*==================================================================================================
FUNCTION: 
    Sec_AttachPeerToSessionIm
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    attachment one peer to session implementation.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.  
    WE_UINT8 ucConnectionType[IN]:The value of the connection type.
    WE_UCHAR * pucAddress[IN]:Pointer to the address.
    WE_INT32 iAddressLen[IN]:The length of the address.
    WE_UINT16 usPortnum[IN]:The value of the port number.
    WE_INT32 iMasterSecretId[IN]:ID of the master secret.
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
void Sec_AttachPeerToSessionIm (WE_HANDLE hSecHandle, WE_UINT8 ucConnectionType,
                               const WE_UCHAR * pucAddress, WE_INT32 iAddressLen,
                               WE_UINT16 usPortnum,  WE_INT32 iMasterSecretId)
{
    St_SecTlLinkPeerSessionResp *pWimStPeerLinkToSessionResp = NULL;    
    #ifdef M_SEC_CFG_SHOW_WARNINGS
    WE_INT32 iTargetID = 0;
    #endif

    if (NULL == hSecHandle)
    {
        return ;
    }
    
    switch(SEC_STATES)
    {
        case E_STATE_ATTACH_PEER_DIRECT_RETURN:
            return;
        case E_STATE_PEER_ATTACH_SESSION: 
            SEC_WAITING_FOR_WIM_RESULT = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_WimPeerAttachSession(hSecHandle, ucConnectionType, pucAddress, 
                                    iAddressLen, usPortnum, iMasterSecretId);
            SEC_STATES++;
            break;
        case E_STATE_HANDLE_PEER_LINK_RESULT:
            if (SEC_WAITING_FOR_WIM_RESULT)
            {
                return;
            }
            if (!SEC_CUR_WIM_RESP)
            {
            #ifdef M_SEC_CFG_SHOW_WARNINGS
                Sec_iUeWarning(hSecHandle, iTargetID,M_SEC_ERR_INSUFFICIENT_MEMORY);
            #endif
                Sec_DpHandleCleanUp( hSecHandle );
                return;
            }
            pWimStPeerLinkToSessionResp = ( St_SecTlLinkPeerSessionResp *)SEC_CUR_WIM_RESP;
            if(pWimStPeerLinkToSessionResp->eType != E_SEC_DP_LINK_PEER_SESSION)
            {
            #ifdef M_SEC_CFG_SHOW_WARNINGS
                Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_GENERAL_ERROR);
            #endif
                Sec_DpHandleCleanUp( hSecHandle );
            }
            else if(pWimStPeerLinkToSessionResp->iResult == M_SEC_ERR_WIM_NOT_INITIALISED)
            {
                SEC_STATES++; 
            }            
            else
            {
            #ifdef M_SEC_CFG_SHOW_WARNINGS
                if (pWimStPeerLinkToSessionResp->iResult != M_SEC_ERR_OK)
                {
                    Sec_iUeWarning(hSecHandle,iTargetID, pWimStPeerLinkToSessionResp->iResult);
                }                   
            #endif
                Sec_DpHandleCleanUp( hSecHandle );
            }
            Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
            SEC_CUR_WIM_RESP = NULL;
            break;
        case E_STATE_PEER_ATTACH_SESSION_AGAIN: 
            {
                WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle, 1, FALSE);
                switch(iStatus)
                {
                    case M_SEC_ERR_IN_PROGRESS:
                        break;
                    case M_SEC_ERR_MISSING_KEY:
                    #ifdef M_SEC_CFG_SHOW_WARNINGS
                        Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_MISSING_AUT_KEY_HANDSHAKE);
                    #endif
                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_WimPeerAttachSession(hSecHandle, ucConnectionType, 
                                                pucAddress, iAddressLen, usPortnum, iMasterSecretId);
                        SEC_STATES--;
                        break;
                    case M_SEC_ERR_OK:                
                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_WimPeerAttachSession(hSecHandle, ucConnectionType, 
                                                pucAddress, iAddressLen, usPortnum, iMasterSecretId);
                        SEC_STATES--;
                        break;
                    case M_SEC_ERR_USER_NOT_VERIFIED:
                    default:                
                    #ifdef M_SEC_CFG_SHOW_WARNINGS
                        Sec_iUeWarning(hSecHandle,iTargetID, iStatus);
                    #endif
                        Sec_DpHandleCleanUp( hSecHandle );
                        break;
                }
            }
            break;
        default:
            return;
    }
}
/*==================================================================================================
FUNCTION: 
    Sec_EnableSessionIm
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    enable one session implementation.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IO]:Global data handle.  
    WE_INT32 iMasterSecretId[IN]:ID of the master secret.
    WE_UINT8 ucIsActive[IN]:The flg of the status.
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
void Sec_EnableSessionIm (WE_HANDLE hSecHandle, WE_INT32 iMasterSecretId, WE_UINT8 ucIsActive)
{
    St_SecTlActiveSessionResp * pstWimStSessionActiveResp = NULL;
    #ifdef M_SEC_CFG_SHOW_WARNINGS    
    WE_INT32 iTargetID = 0;
    #endif

    if (NULL == hSecHandle)
    {
        return ;
    }
    
    switch(SEC_STATES)
    {
        case E_STATE_ENABLE_SESSION_DIRECT_RETURN: 
            return;
        case E_STATE_SESSION_ENABLE:
            SEC_WAITING_FOR_WIM_RESULT = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_WimSessionEnable(hSecHandle, iMasterSecretId, ucIsActive);
            SEC_STATES++;
            break;
        case E_STATE_HANDLE_SESSION_ENABLE_RESULT:
            if (SEC_WAITING_FOR_WIM_RESULT)
            {
                return;
            }
            if (!SEC_CUR_WIM_RESP)
            {
            #ifdef M_SEC_CFG_SHOW_WARNINGS
                Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
            #endif
                Sec_DpHandleCleanUp( hSecHandle );
                return;
            }
            pstWimStSessionActiveResp = ( St_SecTlActiveSessionResp *)SEC_CUR_WIM_RESP;
            if(pstWimStSessionActiveResp->eType != E_SEC_DP_ACTIVE_SESSION)
            {
            #ifdef M_SEC_CFG_SHOW_WARNINGS
                Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_GENERAL_ERROR);
            #endif
                Sec_DpHandleCleanUp( hSecHandle );
            }
            else if(pstWimStSessionActiveResp->iResult == M_SEC_ERR_WIM_NOT_INITIALISED)
            {
                SEC_STATES++; 
            }
            else
            {
            #ifdef M_SEC_CFG_SHOW_WARNINGS
                if (pstWimStSessionActiveResp->iResult != M_SEC_ERR_OK)
                {
                    Sec_iUeWarning(hSecHandle,iTargetID, pstWimStSessionActiveResp->iResult);
                }                
            #endif
                Sec_DpHandleCleanUp( hSecHandle );
            }
            Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
            SEC_CUR_WIM_RESP = NULL;
            break;
        case E_STATE_SESSION_ENABLE_AGAIN: 
            {
                WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle, 1, FALSE);
                switch(iStatus)
                {
                    case M_SEC_ERR_IN_PROGRESS:
                        break;
                    case M_SEC_ERR_MISSING_KEY:
                    #ifdef M_SEC_CFG_SHOW_WARNINGS
                        Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_MISSING_AUT_KEY_HANDSHAKE);
                    #endif
                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_WimSessionEnable(hSecHandle, iMasterSecretId, ucIsActive);
                        SEC_STATES--;
                        break;
                    case M_SEC_ERR_OK:                
                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_WimSessionEnable(hSecHandle, iMasterSecretId, ucIsActive);
                        SEC_STATES--;
                        break;
                    case M_SEC_ERR_USER_NOT_VERIFIED:
                    default:                
                    #ifdef M_SEC_CFG_SHOW_WARNINGS
                        Sec_iUeWarning(hSecHandle,iTargetID, iStatus);
                    #endif
                        Sec_DpHandleCleanUp( hSecHandle );
                        break;
                }
            }
            break;
        default:
            return;
    }
}
/*==================================================================================================
FUNCTION: 
    Sec_DisableSessionIm
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    disable one session implementation.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IO]:Global data handle.  
    WE_INT32 iMasterSecretId[IN]:ID of the master sceret.
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
void Sec_DisableSessionIm (WE_HANDLE hSecHandle, WE_INT32 iMasterSecretId)
{
    St_SecTlInvalidSessionResp * pstWimStSessionInvalidateResp = NULL; 
    #ifdef M_SEC_CFG_SHOW_WARNINGS
    WE_INT32 iTargetID = 0;
    #endif

    if (NULL == hSecHandle)
    {
        return ;
    }
    
    switch(SEC_STATES)
    {
        case E_STATE_DISABLE_SESSION_DIRECT_RETURN: 
            return;
        case E_STATE_SESSION_DISABLE: 
            SEC_WAITING_FOR_WIM_RESULT = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_WimSessionDisable(hSecHandle, iMasterSecretId);
            SEC_STATES++;
            break;
        case E_STATE_HANDLE_SESSION_DISABLE_RESULT:
            if (SEC_WAITING_FOR_WIM_RESULT)
            {
                return;
            }
            if (!SEC_CUR_WIM_RESP)
            {
            #ifdef M_SEC_CFG_SHOW_WARNINGS
                Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY);
            #endif
                Sec_DpHandleCleanUp( hSecHandle );
                return;
            }
            pstWimStSessionInvalidateResp = ( St_SecTlInvalidSessionResp *)SEC_CUR_WIM_RESP;
            if(pstWimStSessionInvalidateResp->eType != E_SEC_DP_INVALID_SESSION)
            {
            #ifdef M_SEC_CFG_SHOW_WARNINGS
                Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_GENERAL_ERROR);
            #endif
                Sec_DpHandleCleanUp( hSecHandle );
            }
            else if(pstWimStSessionInvalidateResp->iResult == M_SEC_ERR_WIM_NOT_INITIALISED)
            {
                SEC_STATES++;
            }                
            else
            {
            #ifdef M_SEC_CFG_SHOW_WARNINGS
                if (pstWimStSessionInvalidateResp->iResult != M_SEC_ERR_OK)
                {
                    Sec_iUeWarning(hSecHandle,iTargetID, pstWimStSessionInvalidateResp->iResult);
                }
            
            #endif
                Sec_DpHandleCleanUp( hSecHandle );
            }
            Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
            SEC_CUR_WIM_RESP = NULL;
            break;
        case E_STATE_SESSION_DISABLE_AGAIN: 
            {
                WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle, 1, FALSE);
                switch(iStatus)
                {
                    case M_SEC_ERR_IN_PROGRESS:
                        break;
                    case M_SEC_ERR_MISSING_KEY:
                    #ifdef M_SEC_CFG_SHOW_WARNINGS
                        Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_MISSING_AUT_KEY_HANDSHAKE);
                    #endif
                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_WimSessionDisable(hSecHandle, iMasterSecretId);
                        SEC_STATES--;
                        break;
                    case M_SEC_ERR_OK:                
                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_WimSessionDisable(hSecHandle, iMasterSecretId);
                        SEC_STATES--;
                        break;
                    case M_SEC_ERR_USER_NOT_VERIFIED:
                    default:                
                    #ifdef M_SEC_CFG_SHOW_WARNINGS
                        Sec_iUeWarning(hSecHandle,iTargetID, iStatus);
                    #endif
                        Sec_DpHandleCleanUp( hSecHandle );
                        break;
                }
            }
            break;
        default:
            return;
    }
}
/*==================================================================================================
FUNCTION: 
    Sec_UpdateSessionIm
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    update session implementation
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.  
    WE_INT32 iMasterSecretId[IN]:ID of the master secret.
    WE_UINT8 ucSessionOptions[IN]:The value of the session option.
    WE_UCHAR * pucSessionId[IN]:ID of the session.
    WE_UINT8 ucSessionIdLen[IN]:The length of the session id.
    WE_UINT8 aucCipherSuite[IN]:Array to cipher suite.
    WE_UINT8 ucCompressionAlg[IN]:The value of compression alg.
    WE_UCHAR * pucPrivateKeyId[IN]:ID of the private key.
    WE_UINT32 uiCreationTime[IN]:the value of the creation time.
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
void Sec_UpdateSessionIm (WE_HANDLE hSecHandle, WE_INT32 iMasterSecretId, WE_UINT8 ucSessionOptions,
                          WE_UCHAR * pucSessionId, WE_UINT8 ucSessionIdLen,
                          WE_UINT8 aucCipherSuite[2], WE_UINT8 ucCompressionAlg,
                          WE_UCHAR * pucPrivateKeyId, WE_UINT32 uiCreationTime)
{
    St_SecTlUpdateSessionResp *pstWimStSessionUpdateResp = NULL;
    #ifdef M_SEC_CFG_SHOW_WARNINGS
    WE_INT32 iTargetID = 0;
    #endif
    
    if (NULL == hSecHandle)
    {
        return ;
    }
    
    switch(SEC_STATES)
    {
        case E_STATE_UPDATE_SESSION_DIRECT_RETURN: 
            return;
        case E_STATE_UPDATE_SESSION: 
            SEC_WAITING_FOR_WIM_RESULT = 1;
            SEC_WANTS_TO_RUN = 0;
            Sec_WimUpdateSession(hSecHandle, iMasterSecretId, ucSessionOptions,
                                pucSessionId, ucSessionIdLen,
                                aucCipherSuite, ucCompressionAlg,
                                pucPrivateKeyId, uiCreationTime);
            SEC_STATES++;
            break;
        case E_STATE_HANDLE_UPDATE_SESSION_RESULT:
            if (SEC_WAITING_FOR_WIM_RESULT)
            {
                return;
            }
            if (!SEC_CUR_WIM_RESP)
            {
            #ifdef M_SEC_CFG_SHOW_WARNINGS
                Sec_iUeWarning(hSecHandle, iTargetID,M_SEC_ERR_INSUFFICIENT_MEMORY);
            #endif
                Sec_DpHandleCleanUp( hSecHandle );
                return;
            }
            pstWimStSessionUpdateResp = ( St_SecTlUpdateSessionResp *)SEC_CUR_WIM_RESP;
            if(pstWimStSessionUpdateResp->eType != E_SEC_DP_UPDATE_SESSION)
            {
            #ifdef M_SEC_CFG_SHOW_WARNINGS
                Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_GENERAL_ERROR);
            #endif
                Sec_DpHandleCleanUp( hSecHandle );
            }
            else if(pstWimStSessionUpdateResp->iResult == M_SEC_ERR_WIM_NOT_INITIALISED)
            {
                SEC_STATES++; 
            }
            else
            {
            #ifdef M_SEC_CFG_SHOW_WARNINGS
                if (pstWimStSessionUpdateResp->iResult != M_SEC_ERR_OK)
                {
                    Sec_iUeWarning(hSecHandle,iTargetID, pstWimStSessionUpdateResp->iResult);
                }            
            #endif
                Sec_DpHandleCleanUp( hSecHandle );
            }
            Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
            SEC_CUR_WIM_RESP = NULL;
            break;
        case E_STATE_UPDATE_SESSION_AGAIN: 
            {
                WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle, 1, FALSE);
                switch(iStatus)
                {
                    case M_SEC_ERR_IN_PROGRESS:
                        break;
                    case M_SEC_ERR_MISSING_KEY:
                    #ifdef M_SEC_CFG_SHOW_WARNINGS
                        Sec_iUeWarning(hSecHandle,iTargetID, M_SEC_ERR_MISSING_AUT_KEY_HANDSHAKE);
                    #endif
                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_WimUpdateSession(hSecHandle, iMasterSecretId, ucSessionOptions, 
                                            pucSessionId, ucSessionIdLen,
                                            aucCipherSuite, ucCompressionAlg, 
                                            pucPrivateKeyId, uiCreationTime);
                        SEC_STATES--;
                        break;
                    case M_SEC_ERR_OK:
                
                        SEC_WAITING_FOR_WIM_RESULT = 1;
                        SEC_WANTS_TO_RUN = 0;
                        Sec_WimUpdateSession(hSecHandle, iMasterSecretId, ucSessionOptions, 
                                            pucSessionId, ucSessionIdLen,
                                            aucCipherSuite, ucCompressionAlg, 
                                            pucPrivateKeyId, uiCreationTime);
                        SEC_STATES--;
                        break;
                    case M_SEC_ERR_USER_NOT_VERIFIED:
                    default:
                
                    #ifdef M_SEC_CFG_SHOW_WARNINGS
                        Sec_iUeWarning(hSecHandle,iTargetID, iStatus);
                    #endif
                        Sec_DpHandleCleanUp( hSecHandle );
                        break;
                }
            }
            break;
        default:
            return;
    }
}
/*==================================================================================================
FUNCTION: 
    Sec_SearchPeerIm
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    search peer implementation.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IO]:Global data handle.  
    WE_INT32 iId[IN]: The value of the ID.
    WE_UINT8 ucConnectionType[IN]:The value of the connection type.
    WE_UCHAR * pucAddress[IN]:Pointer to the address.
    WE_INT32 iAddressLen[IN]:The length of the address.
    WE_UINT16 usPortnum[IN]: The value of the port number.
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
void Sec_SearchPeerIm (WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_UINT8 ucConnectionType,
                        const WE_UCHAR * pucAddress, WE_INT32 iAddressLen, WE_UINT16 usPortnum)
{
   St_SecTlLookupPeerResp *pstWimStPeerLookupResp = NULL;

    if (NULL == hSecHandle)
    {
        return ;
    }
   
   switch(SEC_STATES)
   {
       case E_STATE_SEARCH_PEER_DIRECT_RETURN: 
           return;
       case E_STATE_PEER_SEARCH: 
           SEC_WAITING_FOR_WIM_RESULT = 1;
           SEC_WANTS_TO_RUN = 0;
           Sec_WimPeerSearch(hSecHandle, iTargetID, ucConnectionType, pucAddress, 
                            iAddressLen, usPortnum);
           SEC_STATES++;
           break;
       case E_STATE_SEARCH_PEER_RESP:
           if (SEC_WAITING_FOR_WIM_RESULT)
           {
               return;
           }
           if (!SEC_CUR_WIM_RESP)
           {
               Sec_SearchPeerResp(hSecHandle, iTargetID, M_SEC_ERR_INSUFFICIENT_MEMORY, ucConnectionType, 0,-1);
               Sec_DpHandleCleanUp( hSecHandle );
               return;
           }
           pstWimStPeerLookupResp = ( St_SecTlLookupPeerResp *)SEC_CUR_WIM_RESP;
           if (pstWimStPeerLookupResp->eType != E_SEC_DP_LOOKUP_PEER)
           {
               Sec_SearchPeerResp(hSecHandle, iTargetID, M_SEC_ERR_GENERAL_ERROR, ucConnectionType, 0,-1);
               Sec_DpHandleCleanUp(hSecHandle);
           }
           else if (pstWimStPeerLookupResp->iResult == M_SEC_ERR_WIM_NOT_INITIALISED)
           {
               SEC_STATES++;            
           }
           else if (pstWimStPeerLookupResp->iResult == M_SEC_ERR_OK)
           {                
               Sec_SearchPeerResp(hSecHandle, pstWimStPeerLookupResp->iTargetID, pstWimStPeerLookupResp->iResult,
                                 pstWimStPeerLookupResp->ucConnectionType, pstWimStPeerLookupResp->iMasterSecretId, Sec_WimGetNewSecIdInfo( hSecHandle));
               Sec_DpHandleCleanUp( hSecHandle );
           }
           else
           {
               Sec_SearchPeerResp(hSecHandle, pstWimStPeerLookupResp->iTargetID, pstWimStPeerLookupResp->iResult, pstWimStPeerLookupResp->ucConnectionType,
                                 pstWimStPeerLookupResp->iMasterSecretId, Sec_WimGetNewSecIdInfo( hSecHandle));
               Sec_DpHandleCleanUp( hSecHandle );
           }
       
           Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
           SEC_CUR_WIM_RESP = NULL;
           break;
       case  E_STATE_PEER_SEARCH_AGAIN:
           {
               WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle, iTargetID, FALSE);
               switch(iStatus)
               {
                   case M_SEC_ERR_IN_PROGRESS:
                       break;
                   case M_SEC_ERR_MISSING_KEY:
               
                    #ifdef M_SEC_CFG_WTLS_CLASS_3
                    #ifdef M_SEC_CFG_SHOW_WARNINGS
                       Sec_iUeWarning(hSecHandle, iTargetID,M_SEC_ERR_MISSING_AUT_KEY_HANDSHAKE);
                    #endif
                    #endif
                       SEC_WAITING_FOR_WIM_RESULT = 1;
                       SEC_WANTS_TO_RUN = 0;
                       Sec_WimPeerSearch(hSecHandle, iTargetID, ucConnectionType, 
                                         pucAddress, iAddressLen, usPortnum);
                       SEC_STATES--;
                       break;
                   case M_SEC_ERR_OK:
               
                       SEC_WAITING_FOR_WIM_RESULT = 1;
                       SEC_WANTS_TO_RUN = 0;
                       Sec_WimPeerSearch(hSecHandle, iTargetID, ucConnectionType, 
                                        pucAddress, iAddressLen, usPortnum);
                       SEC_STATES--;
                       break;
                   case M_SEC_ERR_USER_NOT_VERIFIED:
                       iStatus = M_SEC_ERR_NOT_FOUND;    /* fall through */
                       //lint -fallthrough
                   default:               
                       Sec_SearchPeerResp(hSecHandle, iTargetID, iStatus, ucConnectionType, 0,-1);
                       Sec_DpHandleCleanUp( hSecHandle );
                       break;
               }
           }
           break;
       default:
           return;
   }
}
/*==================================================================================================
FUNCTION: 
    Sec_FetchSessionInfoIm
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    fetch sessin information.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IO]:Global data handle.
    WE_INT32 iId[IN]:The value of the ID.
    WE_INT32 iMasterSecretId[IN]:ID of the master secret.
RETURN VALUE:
    None.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_FetchSessionInfoIm (WE_HANDLE hSecHandle, WE_INT32 iId, WE_INT32 iMasterSecretId)
{
    St_SecTlFetchSessionResp *pstWimStSessionFetchResp = NULL;

    if (NULL == hSecHandle)
    {
        return ;
    }
   
   switch(SEC_STATES)
   {
       case E_STATE_FETCH_SESSOIN_INFO_DIRECT_RETURN: 
           return;
       case E_STATE_FETCH_SESSION: 
       
           SEC_WAITING_FOR_WIM_RESULT = 1;
           SEC_WANTS_TO_RUN = 0;
           Sec_WimFetchSession(hSecHandle, iId, iMasterSecretId);
           SEC_STATES++;
           break;
       case E_STATE_GET_SESSION_INFO_RESP:
           if(SEC_WAITING_FOR_WIM_RESULT)
           {
               return;
           }
           if (!SEC_CUR_WIM_RESP)
           {
               Sec_SessionGetResp(hSecHandle, iId, M_SEC_ERR_INSUFFICIENT_MEMORY, (WE_UINT8)0, NULL, (WE_UINT8)0, 0, (WE_UINT8)0, NULL, (WE_UINT32)0);
               Sec_DpHandleCleanUp( hSecHandle );
               return;
           }
           pstWimStSessionFetchResp = ( St_SecTlFetchSessionResp *)SEC_CUR_WIM_RESP;
           if(pstWimStSessionFetchResp->eType != E_SEC_DP_FETCH_SESSION)
           {
               Sec_SessionGetResp(hSecHandle, iId, M_SEC_ERR_GENERAL_ERROR, (WE_UINT8)0, NULL, (WE_UINT8)0, 0, (WE_UINT8)0, NULL, (WE_UINT32)0);
               Sec_DpHandleCleanUp( hSecHandle );
           }
           else if(pstWimStSessionFetchResp->iResult == M_SEC_ERR_WIM_NOT_INITIALISED)
           {
               SEC_STATES++; 
           }
           else
           {
               Sec_SessionGetResp(hSecHandle, 
                                      pstWimStSessionFetchResp->iTargetID, 
                                      pstWimStSessionFetchResp->iResult, 
                                      pstWimStSessionFetchResp->ucSessionOptions, 
                                      pstWimStSessionFetchResp->pucSessionId, 
                                      pstWimStSessionFetchResp->ucSessionIdLen,
                                      pstWimStSessionFetchResp->aucCipherSuite, 
                                      pstWimStSessionFetchResp->ucCompressionAlg, 
                                      pstWimStSessionFetchResp->pucPrivateKeyId, 
                                      pstWimStSessionFetchResp->uiCreationTime);
               Sec_DpHandleCleanUp( hSecHandle );
           }
           Sec_TlHandleWimStructFree(SEC_CUR_WIM_RESP);
           SEC_CUR_WIM_RESP = NULL;
           break;
       case E_STATE_FETCH_SESSION_AGAIN:
           {
               WE_INT32 iStatus = Sec_DpHandleWimInit(hSecHandle, iId, FALSE);
               switch(iStatus)
               {
                   case M_SEC_ERR_IN_PROGRESS:
                       break;
                   case M_SEC_ERR_MISSING_KEY:
                    #ifdef M_SEC_CFG_SHOW_WARNINGS
                       Sec_iUeWarning(hSecHandle,iId, M_SEC_ERR_MISSING_AUT_KEY_HANDSHAKE);
                    #endif
                       SEC_WAITING_FOR_WIM_RESULT = 1;
                       SEC_WANTS_TO_RUN = 0;
                       Sec_WimFetchSession(hSecHandle, iId, iMasterSecretId);
                       SEC_STATES--;
                       break;
                   case M_SEC_ERR_OK:
               
                       SEC_WAITING_FOR_WIM_RESULT = 1;
                       SEC_WANTS_TO_RUN = 0;
                       Sec_WimFetchSession(hSecHandle, iId, iMasterSecretId);
                       SEC_STATES--;
                       break;
                   case M_SEC_ERR_USER_NOT_VERIFIED:  iStatus = M_SEC_ERR_OK;   
                       /* fall through */
                       //lint -fallthrough
                   default:               
                       Sec_SessionGetResp(hSecHandle, iId, iStatus, (WE_UINT8)0, NULL, (WE_UINT8)0, 0, (WE_UINT8)0, NULL, (WE_UINT32)0);
                       Sec_DpHandleCleanUp( hSecHandle );
                       break;
               }
           }
           break;
       default:
           return;
   }
}
