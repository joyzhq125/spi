/*=====================================================================================
    FILE NAME : sec_iwap.c
    MODULE NAME : SEC
    
    GENERAL DESCRIPTION
        these functions in this file support the wtls to normally run.
        
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
#include "sec_comm.h"

/*=====================================================================================
FUNCTION: 
    Sec_MsgWtlsGetPrfResult
CREATE DATE: 
    2006-07-04
AUTHOR: 
    Kevin
DESCRIPTION:
    get the Pseudo Random Function.If the master secret id is 
    to be used, "pucSecret" is NULL, and "iMasterSecretId" 
    denotes which master secret is to use. Or,"pucSecret" must be provided.
ARGUMENTS PASSED:
    WE_HANDLE     hSecHandle[IN]: Global data handle.
    WE_INT32      iTargetID[IN]:  the identity of the invoker input from the external interface.
    WE_UINT8      ucAlg[IN]:  The hash algorithm to use.
    WE_INT32      iMasterSecretId[IN]: ID of the master secret.
    const WE_UCHAR *  pucSecret[IN]:  pointer to the secret.
    WE_INT32      iSecretLen[IN]: the length of the secret.
    const WE_CHAR *   pcLabel[IN]:  pointer to the label.
    const WE_UCHAR *  pucSeed[IN]:  pointer to the seed.
    WE_INT32      iSeedLen[IN]: the length of the seed.
    WE_INT32      iOutputLen[IN]: the length of the output data.
    
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The status is ok.
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
WE_INT32 Sec_MsgWtlsGetPrfResult(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_UINT8 ucAlg, 
                                 WE_INT32 iMasterSecretId, const WE_UCHAR * pucSecret, WE_INT32 iSecretLen,
                                 const WE_CHAR * pcLabel, const WE_UCHAR * pucSeed, WE_INT32 iSeedLen,
                                 WE_INT32 iOutputLen)
{
    WE_INT32 iRes = 0;
    St_SecGetPrfResult * pstWtlsPrf = (St_SecGetPrfResult *)WE_MALLOC((WE_ULONG)sizeof(St_SecGetPrfResult));

    if(NULL == pstWtlsPrf)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }

    pstWtlsPrf->iTargetID = (WE_INT32)iTargetID;
    pstWtlsPrf->ucAlg = ucAlg;
    pstWtlsPrf->ucMasterSecretId = (WE_UINT8)iMasterSecretId;
    pstWtlsPrf->pucSecret = (WE_UCHAR *)pucSecret;
    pstWtlsPrf->usSecretLen = (WE_UINT16)iSecretLen;
    pstWtlsPrf->pcLabel = (WE_CHAR *)pcLabel;
    pstWtlsPrf->pucSeed = (WE_UCHAR *)pucSeed;
    pstWtlsPrf->usSeedLen = (WE_UINT16)iSeedLen;
    pstWtlsPrf->usOutputLen = (WE_UINT16)iOutputLen;
  
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_WTLSGETPRFRESULT, (void*)pstWtlsPrf);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstWtlsPrf);
        return iRes;
    }
    
    return M_SEC_ERR_OK;
  
}

/*=====================================================================================
FUNCTION: 
    Sec_MsgWtlsVerifySvrCert
CREATE DATE: 
    2006-07-04
AUTHOR: 
    Kevin
DESCRIPTION:
    Verify a certificates from gateway. This function may verify
    that the address name matches the ¡°common name¡± field of 
    the certificate. 
ARGUMENTS PASSED:
    WE_HANDLE     hSecHandle[IN]: Global data handle.
    WE_INT32      iTargetID[IN]:  the identity of the invoker input from the external interface.
    const WE_UCHAR *  pucBuf[IN]:  pointer to the data buf for certificate content.
    WE_INT32      iBufLen[IN]: the length of the data buf.
    const WE_UCHAR *  pucAddr[IN]: the pointer to the address of the issuer.
    WE_INT32      iAddrLen[IN]:The length of the address.
    const WE_UCHAR *  pcDomain[IN]:The pointer to the domain,now it is no use.

RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The status is ok.
    
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
WE_INT32 Sec_MsgWtlsVerifySvrCert (WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                                   const WE_VOID *pvBuf, WE_INT32 iBufLen)
{
    St_SecWtlsVerifySvrCert * pstVerCertChain = NULL;
    WE_INT32 iRes =M_SEC_ERR_OK;
    if(NULL==pvBuf)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    iBufLen=iBufLen;
    pstVerCertChain = (St_SecWtlsVerifySvrCert*)WE_MALLOC((WE_ULONG)sizeof(St_SecWtlsVerifySvrCert));
    if(NULL == pstVerCertChain)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstVerCertChain->usBufLen = (WE_UINT16)(((St_SecWtlsVerifyCertChainParam *)pvBuf)->iBufLen);
    pstVerCertChain->usAddrLen = (WE_UINT16)(((St_SecWtlsVerifyCertChainParam *)pvBuf)->iAddrLen);
    pstVerCertChain->usDomainLen = ((St_SecWtlsVerifyCertChainParam *)pvBuf)->usDomainLen;
    pstVerCertChain->iTargetID = iTargetID;
    if(NULL!=((St_SecWtlsVerifyCertChainParam *)pvBuf)->pucBuf 
        &&pstVerCertChain->usBufLen>0)
    {
       pstVerCertChain->pucBuf=(WE_UCHAR *)WE_MALLOC(pstVerCertChain->usBufLen);
       if(NULL==pstVerCertChain->pucBuf)
       {
          WE_FREE(pstVerCertChain);
          return M_SEC_ERR_INSUFFICIENT_MEMORY;
       }
       (WE_VOID)WE_MEMCPY(pstVerCertChain->pucBuf,((St_SecWtlsVerifyCertChainParam *)pvBuf)->pucBuf,
             pstVerCertChain->usBufLen);
    }
    if(NULL!=((St_SecWtlsVerifyCertChainParam *)pvBuf)->pucAddr &&pstVerCertChain->usAddrLen>0)
    {
       pstVerCertChain->pucAddr=(WE_UCHAR *)WE_MALLOC(pstVerCertChain->usAddrLen);
       if(NULL==pstVerCertChain->pucAddr)
       {
          WE_FREE(pstVerCertChain->pucBuf);
          WE_FREE(pstVerCertChain);
          return M_SEC_ERR_INSUFFICIENT_MEMORY;
       }
       (WE_VOID)WE_MEMCPY(pstVerCertChain->pucAddr,((St_SecWtlsVerifyCertChainParam *)pvBuf)->pucAddr,
             pstVerCertChain->usAddrLen);
    }
    
    if(NULL!=((St_SecWtlsVerifyCertChainParam *)pvBuf)->pucDomain &&pstVerCertChain->usAddrLen>0)
    {
       pstVerCertChain->pucDomain=(WE_UCHAR *)WE_MALLOC(pstVerCertChain->usDomainLen);
       if(NULL==pstVerCertChain->pucDomain)
       {
          WE_FREE(pstVerCertChain->pucAddr);
          WE_FREE(pstVerCertChain->pucBuf);
          WE_FREE(pstVerCertChain);
          return M_SEC_ERR_INSUFFICIENT_MEMORY;
       }
       (WE_VOID)WE_MEMCPY(pstVerCertChain->pucDomain,((St_SecWtlsVerifyCertChainParam *)pvBuf)->pucDomain,
             pstVerCertChain->usDomainLen);
    }
    iRes =Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_WTLSVERIFYSVRCERT, (void*)pstVerCertChain);
    if(iRes !=M_SEC_ERR_OK)
    {
        WE_FREE(pstVerCertChain->pucDomain);
        WE_FREE(pstVerCertChain->pucAddr);
        WE_FREE(pstVerCertChain->pucBuf);
        WE_FREE(pstVerCertChain);
        return iRes;
    }
    return M_SEC_ERR_OK;
  
}
/*=====================================================================================
FUNCTION: 
    Sec_MsgWtlsGetCipherSuite
CREATE DATE: 
    2006-07-04
AUTHOR: 
    Kevin
DESCRIPTION:
    Get lists of cipher methods, key exchange methods, and trusted CA cert.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]: Global data handle.
    WE_INT32  iTargetID[IN]:  the identity of the invoker input from the external interface.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The status is ok.
     
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
WE_INT32 Sec_MsgWtlsGetCipherSuite (WE_HANDLE hSecHandle, WE_INT32 iTargetID)
{
    WE_INT32 iRes = 0;
    
    St_SecWtlsGetCipherSuite * pstGetMethods = (St_SecWtlsGetCipherSuite *)WE_MALLOC((WE_ULONG)sizeof(St_SecWtlsGetCipherSuite));
    
    if(NULL == pstGetMethods)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }

    pstGetMethods->iTargetID = iTargetID;
    
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_WTLSGETCIPHERSUITE, (void*)pstGetMethods);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstGetMethods);
        return iRes;
    }
  
    return M_SEC_ERR_OK;
}

/*=====================================================================================
FUNCTION: 
    Sec_MsgWtlsKeyExchange
CREATE DATE: 
    2006-07-04
AUTHOR: 
    Kevin
DESCRIPTION:
    run a key exchange operation, using the selected key exchange method
    and get the master secret. The public key is either given in the parameters,
    or must be retrieved from a certificate passed in parameter.
ARGUMENTS PASSED:
    WE_HANDLE         hSecHandle[IN]: Global data handle.
    WE_INT32          iTargetID[IN] : the identity of the invoker input from the external interface.    
    St_SecWtlsKeyExchParams   stParam[IN]   : including the public key and key exchange method.
    WE_UINT8          ucHashAlg[IN] : The hash tring value.
    const WE_UCHAR *  pucRandval[IN]: 32 bytes of random data to create the master secret.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The status is ok.     
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
WE_INT32 Sec_MsgWtlsKeyExchange (WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                 const WE_VOID *pvParam, WE_UINT8 ucAlg, 
                                 const WE_UCHAR * pucRandval)
{
    St_SecWtlsKeyExchParams *pstParamMem =NULL;
    WE_UCHAR * pucRandValMem = NULL;
    St_SecWtlsKeyExchange * pstKeyExch=NULL;
    WE_INT32 iRes =M_SEC_ERR_OK;
    
    if(NULL == hSecHandle||NULL==pvParam)
    {
       return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if(NULL != pucRandval)
    {
       pucRandValMem = WE_MALLOC((WE_ULONG)(32));
       if(NULL == pucRandValMem)
       {
          return M_SEC_ERR_INSUFFICIENT_MEMORY;
       }
       (void)WE_MEMCPY(pucRandValMem,(WE_UCHAR *)pucRandval,(WE_UINT32)32);
    }
    pstParamMem=(St_SecWtlsKeyExchParams *)WE_MALLOC(sizeof(St_SecWtlsKeyExchParams));
    if(NULL==pstParamMem)
    {
       if(pucRandValMem)
       {
          WE_FREE(pucRandValMem);
       }
       return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    
    pstParamMem->ucKeyExchangeSuite =((St_SecWtlsKeyExchParams *)pvParam)->ucKeyExchangeSuite;
    
    switch(((St_SecWtlsKeyExchParams *)pvParam)->ucKeyExchangeSuite)
    {
       case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON:
       case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_512:
       case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_768:
          pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usExpLen = 
                                  ((St_SecWtlsKeyExchParams *)pvParam)->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usExpLen;
          pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usModLen = 
                                  ((St_SecWtlsKeyExchParams *)pvParam)->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usModLen;
          if(NULL != ((St_SecWtlsKeyExchParams *)pvParam)->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent
             && pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usExpLen > 0)
          {
             pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent = 
             WE_MALLOC((WE_ULONG)pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usExpLen);
    
             if(NULL == pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent)
             {
                if(NULL != pucRandValMem)
                {
                   WE_FREE(pucRandValMem);
                }
                WE_FREE(pstParamMem);
                return M_SEC_ERR_INSUFFICIENT_MEMORY;
             }
    
             (void)WE_MEMCPY(pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent,
                             ((St_SecWtlsKeyExchParams *)pvParam)->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent,
                             (WE_UINT32)pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usExpLen);
          }
    
          if(NULL != ((St_SecWtlsKeyExchParams *)pvParam)->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus
             && pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usModLen > 0)
          {
             pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus = 
             WE_MALLOC((WE_ULONG)pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usModLen);
    
             if(NULL == pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus)
             {
                if(NULL != pucRandValMem)
                {
                   WE_FREE(pucRandValMem);
                }
                if(NULL != pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent)
                {
                   WE_FREE(pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent);
                }
                WE_FREE(pstParamMem);
                return M_SEC_ERR_INSUFFICIENT_MEMORY;
             }
    
             (void)WE_MEMCPY(pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus,
                             ((St_SecWtlsKeyExchParams *)pvParam)->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus,
                             (WE_UINT32)pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usModLen);
          }
    
          if(255 == ((St_SecWtlsKeyExchParams *)pvParam)->stParams.stKeyParam.stParamSpecifier.ucParameterIndex)
          {
             pstParamMem->stParams.stKeyParam.stParamSpecifier.ucParameterIndex = 
             ((St_SecWtlsKeyExchParams *)pvParam)->stParams.stKeyParam.stParamSpecifier.ucParameterIndex;
             if(((St_SecWtlsKeyExchParams *)pvParam)->stParams.stKeyParam.stParamSpecifier.usParamLen > 0)
             {
                pstParamMem->stParams.stKeyParam.stParamSpecifier.usParamLen = 
                ((St_SecWtlsKeyExchParams *)pvParam)->stParams.stKeyParam.stParamSpecifier.usParamLen;
    
                pstParamMem->stParams.stKeyParam.stParamSpecifier.pucParams = 
                      WE_MALLOC((WE_ULONG)pstParamMem->stParams.stKeyParam.stParamSpecifier.usParamLen);
    
                if(NULL == pstParamMem->stParams.stKeyParam.stParamSpecifier.pucParams)
                {
                   if(NULL != pucRandValMem)
                   {
                      WE_FREE(pucRandValMem);
                   }
                   if(NULL != pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent)
                   {
                      WE_FREE(pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent);
                   }
                   if(NULL != pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus)
                   {
                      WE_FREE(pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus);
                   }
                   WE_FREE(pstParamMem);
                   return M_SEC_ERR_INSUFFICIENT_MEMORY;
                }
    
                (void)WE_MEMCPY(pstParamMem->stParams.stKeyParam.stParamSpecifier.pucParams,
                                ((St_SecWtlsKeyExchParams *)pvParam)->stParams.stKeyParam.stParamSpecifier.pucParams,
                                (WE_UINT32)pstParamMem->stParams.stKeyParam.stParamSpecifier.usParamLen);
             }
          }
          break;
       case E_SEC_WTLS_KEYEXCHANGE_RSA:
       case E_SEC_WTLS_KEYEXCHANGE_RSA_512:
       case E_SEC_WTLS_KEYEXCHANGE_RSA_768:
       case E_SEC_WTLS_KEYEXCHANGE_ECDH_ECDSA:
          if(((St_SecWtlsKeyExchParams *)pvParam)->stParams.stCertificates.usBufLen > 0)
          {        
             pstParamMem->stParams.stCertificates.usBufLen = 
             ((St_SecWtlsKeyExchParams *)pvParam)->stParams.stCertificates.usBufLen;
             pstParamMem->stParams.stCertificates.pucBuf = 
                            WE_MALLOC((WE_ULONG)pstParamMem->stParams.stCertificates.usBufLen);
    
             if(NULL == pstParamMem->stParams.stCertificates.pucBuf)
             {
                if(NULL != pucRandValMem)
                {
                   WE_FREE(pucRandValMem);
                }
                WE_FREE(pstParamMem);
                return M_SEC_ERR_INSUFFICIENT_MEMORY;
             }
    
             (void)WE_MEMCPY(pstParamMem->stParams.stCertificates.pucBuf,
                             ((St_SecWtlsKeyExchParams *)pvParam)->stParams.stCertificates.pucBuf,
                             (WE_UINT32)pstParamMem->stParams.stCertificates.usBufLen);
          }
          break;
       case E_SEC_WTLS_KEYEXCHANGE_SHARED_SECRET:
          if(((St_SecWtlsKeyExchParams *)pvParam)->stParams.stSecretKey.usIdLen > 0)
          {        
             pstParamMem->stParams.stSecretKey.usIdLen = 
             ((St_SecWtlsKeyExchParams *)pvParam)->stParams.stSecretKey.usIdLen;
             pstParamMem->stParams.stSecretKey.pucIdentifier = 
                            WE_MALLOC((WE_ULONG)pstParamMem->stParams.stSecretKey.usIdLen);
    
             if(NULL == pstParamMem->stParams.stSecretKey.pucIdentifier)
             {
                if(NULL != pucRandValMem)
                {
                   WE_FREE(pucRandValMem);
                }
                WE_FREE(pstParamMem);
                return M_SEC_ERR_INSUFFICIENT_MEMORY;
             }
    
             (void)WE_MEMCPY(pstParamMem->stParams.stSecretKey.pucIdentifier,
                             ((St_SecWtlsKeyExchParams *)pvParam)->stParams.stSecretKey.pucIdentifier,
                             (WE_UINT32)pstParamMem->stParams.stSecretKey.usIdLen);
          }
          break;
       case E_SEC_WTLS_KEYEXCHANGE_NULL:
          (void)WE_MEMSET(&pstParamMem,0,sizeof(St_SecWtlsKeyExchParams));
          break;
       default:
          break;
    }
/*post the message*/
    pstKeyExch = (St_SecWtlsKeyExchange*)WE_MALLOC((WE_ULONG)sizeof(St_SecWtlsKeyExchange));
    if(NULL == pstKeyExch)
    {
       if(NULL != pucRandValMem)
       {
          WE_FREE(pucRandValMem);
       }
       switch(((St_SecWtlsKeyExchParams *)pvParam)->ucKeyExchangeSuite)
       {
          case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON:
          case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_512:
          case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_768:
             if(NULL != pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent)
             {
                 WE_FREE(pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent);
             }
             if(NULL != pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus)
             {
                WE_FREE(pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus);
             }
             if(255 == ((St_SecWtlsKeyExchParams *)pvParam)->stParams.stKeyParam.stParamSpecifier.ucParameterIndex
                && NULL != pstParamMem->stParams.stKeyParam.stParamSpecifier.pucParams)
             {
                WE_FREE(pstParamMem->stParams.stKeyParam.stParamSpecifier.pucParams);
             }
             break;
          case E_SEC_WTLS_KEYEXCHANGE_RSA:
          case E_SEC_WTLS_KEYEXCHANGE_RSA_512:
          case E_SEC_WTLS_KEYEXCHANGE_RSA_768:
          case E_SEC_WTLS_KEYEXCHANGE_ECDH_ECDSA:
             if(NULL != pstParamMem->stParams.stCertificates.pucBuf)
             {
                WE_FREE(pstParamMem->stParams.stCertificates.pucBuf);
             }
             break;
          case E_SEC_WTLS_KEYEXCHANGE_SHARED_SECRET:
             if(NULL != pstParamMem->stParams.stSecretKey.pucIdentifier)
             {
                WE_FREE(pstParamMem->stParams.stSecretKey.pucIdentifier);
             }
             break;
          default :
             break; 
       }
       if(NULL!=pstParamMem)
       {
          WE_FREE(pstParamMem);
       }
       return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    
    /*post the message data*/
    pstKeyExch->iTargetID = iTargetID;
    pstKeyExch->pstParam = pstParamMem;
    pstKeyExch->ucHashAlg = ucAlg;
    pstKeyExch->pucRandval = pucRandValMem;

    iRes =Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_WTLSKEYEXCHANGE, (void*)pstKeyExch);
    if(iRes !=M_SEC_ERR_OK)
    {
       if(NULL != pucRandValMem)
       {
          WE_FREE(pucRandValMem);
       }
       switch(((St_SecWtlsKeyExchParams *)pvParam)->ucKeyExchangeSuite)
       {
          case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON:
          case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_512:
          case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_768:
             if(NULL != pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent)
             {
                 WE_FREE(pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent);
             }
             if(NULL != pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus)
             {
                WE_FREE(pstParamMem->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus);
             }
             if(255 == ((St_SecWtlsKeyExchParams *)pvParam)->stParams.stKeyParam.stParamSpecifier.ucParameterIndex
                && NULL != pstParamMem->stParams.stKeyParam.stParamSpecifier.pucParams)
             {
                WE_FREE(pstParamMem->stParams.stKeyParam.stParamSpecifier.pucParams);
             }
             break;
          case E_SEC_WTLS_KEYEXCHANGE_RSA:
          case E_SEC_WTLS_KEYEXCHANGE_RSA_512:
          case E_SEC_WTLS_KEYEXCHANGE_RSA_768:
          case E_SEC_WTLS_KEYEXCHANGE_ECDH_ECDSA:
             if(NULL != pstParamMem->stParams.stCertificates.pucBuf)
             {
                WE_FREE(pstParamMem->stParams.stCertificates.pucBuf);
             }
             break;
          case E_SEC_WTLS_KEYEXCHANGE_SHARED_SECRET:
             if(NULL != pstParamMem->stParams.stSecretKey.pucIdentifier)
             {
                WE_FREE(pstParamMem->stParams.stSecretKey.pucIdentifier);
             }
             break;
          default :
             break; 
       }
       if(NULL!=pstParamMem)
       {
          WE_FREE(pstParamMem);
       }
       WE_FREE(pstKeyExch);
       pstKeyExch =NULL;
       return iRes;
    }
    return M_SEC_ERR_OK;

}

/*=====================================================================================
FUNCTION: 
    Sec_MsgWtlsGetUsrCert
CREATE DATE: 
    2006-07-04
AUTHOR: 
    Kevin
DESCRIPTION:
    get a user certificate signed by one of the CA supplied. If the  
    CA is empty, any certificate can be returned.
ARGUMENTS PASSED:
    WE_HANDLE     hSecHandle[IN]: Global data handle.
    WE_INT32      iTargetID[IN]: the identity of the invoker input from the external interface.
    const WE_UCHAR *  pucBuf[IN]:   Pointer to the data buffer,contains a list of acceptable 
                                    CA, as byte encoded KeyExchangeIds.
    WE_INT32      iBufLen[IN]: The length of the buffer.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The status is ok.
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
WE_INT32 Sec_MsgWtlsGetUsrCert (WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                const WE_UCHAR * pucBuf, WE_INT32 iBufLen)
{
    WE_INT32 iRes = 0;
    St_SecWtlsGetUsrCert * pstGetClientCert = (St_SecWtlsGetUsrCert *)WE_MALLOC((WE_ULONG)sizeof(St_SecWtlsGetUsrCert));
    if(NULL == pstGetClientCert)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }

    pstGetClientCert->iTargetID = iTargetID;
    pstGetClientCert->pucBuf = (WE_UCHAR *)pucBuf;
    pstGetClientCert->usBufLen = (WE_UINT16)iBufLen;

    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_WTLSGETUSRCERT, (void*)pstGetClientCert);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstGetClientCert);
        return iRes;
    }
    
    return M_SEC_ERR_OK;    
}
#ifdef M_SEC_CFG_TLS
/*=====================================================================================
FUNCTION: 
    Sec_MsgTlsGetUsrCert
CREATE DATE: 
    2007-01-19
AUTHOR: 
    tang
DESCRIPTION:
    get a user certificate signed by one of the CA supplied. If the  
    CA is empty, any certificate can be returned.
ARGUMENTS PASSED:
    WE_HANDLE     hSecHandle[IN]: Global data handle.
    WE_INT32      iTargetID[IN]: the identity of the invoker input from the external interface.
    const WE_UCHAR *  pvBuf[IN]:    pointer to the structure of St_SecTlsGetUsrCertParam.
    WE_INT32      iBufLen[IN]: The length of the buffer.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The status is ok.
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
WE_INT32 Sec_MsgTlsGetUsrCert (WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                const WE_VOID * pvBuf, WE_INT32 iBufLen)
{
    WE_INT32 iLoop=0;
    WE_INT32 iJLoop=0;
    St_SecTlsGetUsrCertParam *pstParamIn=(St_SecTlsGetUsrCertParam *)pvBuf;
    St_SecTlsGetUsrCert * pstGetClientCert = NULL;
    St_SecTlsGetUsrCertParam *pstUsrCertParam=NULL;
    WE_INT32 iRes =M_SEC_ERR_OK;
    
    if(!pstParamIn)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    iBufLen=iBufLen;
    
    pstGetClientCert = (St_SecTlsGetUsrCert *)WE_MALLOC((WE_ULONG)sizeof(St_SecTlsGetUsrCert));
    if(NULL == pstGetClientCert)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstUsrCertParam=(St_SecTlsGetUsrCertParam *)WE_MALLOC(sizeof(St_SecTlsGetUsrCertParam));
    if(NULL==pstUsrCertParam)
    {
        WE_FREE(pstGetClientCert);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstUsrCertParam->ucNumCertAuthorities=pstParamIn->ucNumCertAuthorities;
    pstUsrCertParam->ucNumCertTypes=pstParamIn->ucNumCertTypes;
    pstUsrCertParam->pucCertTypes=(WE_UCHAR *)WE_MALLOC(pstUsrCertParam->ucNumCertTypes);
    if(NULL==pstUsrCertParam->pucCertTypes)
    {
        WE_FREE(pstGetClientCert);
        WE_FREE(pstUsrCertParam);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;  
    }
    (WE_VOID)WE_MEMCPY(pstUsrCertParam->pucCertTypes,pstParamIn->pucCertTypes,
             pstParamIn->ucNumCertTypes);
    
    pstUsrCertParam->pstCertAuthorities=(St_SecTlsDistingubshedName *)WE_MALLOC((WE_UINT32)(pstUsrCertParam->ucNumCertAuthorities)*sizeof(St_SecTlsDistingubshedName));
    if(NULL==pstUsrCertParam->pstCertAuthorities)
    {
        WE_FREE(pstGetClientCert);
        WE_FREE(pstUsrCertParam->pucCertTypes);
        WE_FREE(pstUsrCertParam);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }

    for(iLoop=0;iLoop<pstUsrCertParam->ucNumCertAuthorities;iLoop++)
    {
        pstUsrCertParam->pstCertAuthorities[iLoop].pucName=
            WE_MALLOC(pstParamIn->pstCertAuthorities[iLoop].usNameLen);
        if(NULL==pstUsrCertParam->pstCertAuthorities[iLoop].pucName)    
        {
            for(iJLoop=0;iJLoop<iLoop;iJLoop++)
            {
                WE_FREE(pstUsrCertParam->pstCertAuthorities[iJLoop].pucName);
            }
            WE_FREE(pstGetClientCert);
            WE_FREE(pstUsrCertParam->pucCertTypes);
            WE_FREE(pstUsrCertParam->pstCertAuthorities);
            WE_FREE(pstUsrCertParam);
            return M_SEC_ERR_INSUFFICIENT_MEMORY ;
        }
        (WE_VOID)WE_MEMCPY(pstUsrCertParam->pstCertAuthorities[iLoop].pucName,
            pstParamIn->pstCertAuthorities[iLoop].pucName,
            pstParamIn->pstCertAuthorities[iLoop].usNameLen);
        pstUsrCertParam->pstCertAuthorities[iLoop].usNameLen=pstParamIn->pstCertAuthorities[iLoop].usNameLen;
    }

    pstGetClientCert->iTargetID = iTargetID;
    pstGetClientCert->pstParam= pstUsrCertParam;

    iRes =Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_TLSGETUSRCERT, (void*)pstGetClientCert);
    if(iRes !=M_SEC_ERR_OK)
    {
        for(iJLoop=0;iJLoop<pstUsrCertParam->ucNumCertAuthorities;iJLoop++)
        {
            WE_FREE(pstUsrCertParam->pstCertAuthorities[iJLoop].pucName);
        }
        WE_FREE(pstGetClientCert);
        WE_FREE(pstUsrCertParam->pucCertTypes);
        WE_FREE(pstUsrCertParam->pstCertAuthorities);
        WE_FREE(pstUsrCertParam);
        return iRes ;
    }
    return M_SEC_ERR_OK;    
}
/*=====================================================================================
FUNCTION: 
    Sec_MsgTlsGetCipherSuite
CREATE DATE: 
    2007-01-19
AUTHOR: 
    tang
DESCRIPTION:
    Get lists of cipher methods, key exchange methods.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]: Global data handle.
    WE_INT32  iTargetID[IN]:  the identity of the invoker input from the external interface.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The status is ok.
     
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
WE_INT32 Sec_MsgTlsGetCipherSuite (WE_HANDLE hSecHandle, WE_INT32 iTargetID)
{
    St_SecTlsGetCipherSuite * pstGetMethods = (St_SecTlsGetCipherSuite *)WE_MALLOC((WE_ULONG)sizeof(St_SecTlsGetCipherSuite));
    WE_INT32 iRes =M_SEC_ERR_OK;
    
    if(NULL == pstGetMethods)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }

    pstGetMethods->iTargetID = iTargetID;
    
    iRes =Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_TLSGETCIPHERSUITE, (void*)pstGetMethods);
    if(iRes!=M_SEC_ERR_OK)
    {
        WE_FREE(pstGetMethods);
        pstGetMethods =NULL;
    }
    return M_SEC_ERR_OK;
}
/*=====================================================================================
FUNCTION: 
    Sec_MsgTlsVerifyCertChain
CREATE DATE: 
    2007-01-19
AUTHOR: 
    tang
DESCRIPTION:
    Verify a certificates from gateway. This function may verify
    that the address name matches the ¡°common name¡± field of 
    the certificate. 
ARGUMENTS PASSED:
    WE_HANDLE     hSecHandle[IN]: Global data handle.
    WE_INT32      iTargetID[IN]:  the identity of the invoker input from the external interface.
    const WE_VOID *pvBuf[IN]:pointer to the structure of St_SecTlsVerifyCertChainParam.
    WE_INT32 iBufLen[IN]:length of buffer.

RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The status is ok.
    
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
WE_INT32 Sec_MsgTlsVerifyCertChain (WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                                   const WE_VOID *pvBuf, WE_INT32 iBufLen)
{
    WE_INT32 iIndex=0;
    WE_INT32 iLoop=0;
    WE_INT32 iRes =M_SEC_ERR_OK;
    
    St_SecTlsVerifyCertChainParam *pstParamIn=(St_SecTlsVerifyCertChainParam *)pvBuf;
    St_SecTlsVerifySvrCert * pstVerCertChain =NULL;
    if(!pstParamIn)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    iBufLen=iBufLen;
    pstVerCertChain = (St_SecTlsVerifySvrCert*)WE_MALLOC((WE_ULONG)sizeof(St_SecTlsVerifySvrCert));
    if(NULL == pstVerCertChain)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstVerCertChain->iTargetID = iTargetID;
    pstVerCertChain->usAddrLen = pstParamIn->usAddrLen;
    pstVerCertChain->usDomainLen=pstParamIn->usDomainLen;
    pstVerCertChain->ucNumCerts = pstParamIn->ucNumCerts;
    if(pstVerCertChain->usAddrLen>0&&NULL!=(pstParamIn->pucAddr))
    {
        pstVerCertChain->pucAddr=(WE_UCHAR *)WE_MALLOC(pstVerCertChain->usAddrLen );
        if(NULL==pstVerCertChain->pucAddr)
        {
            WE_FREE(pstVerCertChain);
            return M_SEC_ERR_INSUFFICIENT_MEMORY;
        }
        (WE_VOID)WE_MEMCPY(pstVerCertChain->pucAddr,pstParamIn->pucAddr,
            pstVerCertChain->usAddrLen);
    }
    if((NULL!=pstParamIn->pucDomain)&&pstVerCertChain->usDomainLen)
    {
        pstVerCertChain->pucDomain=(WE_UCHAR *)WE_MALLOC(pstVerCertChain->usDomainLen );
        if(NULL==pstVerCertChain->pucDomain)
        {
            if(pstVerCertChain->pucAddr)
            {
                WE_FREE(pstVerCertChain->pucAddr);
                pstVerCertChain->pucAddr=NULL;
            }
            WE_FREE(pstVerCertChain);
            pstVerCertChain=NULL;
            return M_SEC_ERR_INSUFFICIENT_MEMORY;
        }
        (WE_VOID)WE_MEMCPY(pstVerCertChain->pucDomain,pstParamIn->pucDomain,
            pstVerCertChain->usDomainLen); 
    }
    
    pstVerCertChain->pstCerts=(St_SecTlsAsn1Certificate *)WE_MALLOC((WE_UINT32)(pstVerCertChain->ucNumCerts)*sizeof(St_SecTlsAsn1Certificate));
    if(NULL==pstVerCertChain->pstCerts)
    {
        if(pstVerCertChain->pucAddr)
        {
            WE_FREE(pstVerCertChain->pucAddr);
        }
        if(pstVerCertChain->pucDomain)
        {
            WE_FREE(pstVerCertChain->pucDomain);
        }
        WE_FREE(pstVerCertChain);
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    for(iIndex=0;iIndex<pstVerCertChain->ucNumCerts;iIndex++)
    {
        pstVerCertChain->pstCerts[iIndex].ucFormat= pstParamIn->pstCerts[iIndex].ucFormat;
        pstVerCertChain->pstCerts[iIndex].uiCertLen= pstParamIn->pstCerts[iIndex].uiCertLen;
        pstVerCertChain->pstCerts[iIndex].pucCert=
            (WE_UINT8 *)WE_MALLOC(pstVerCertChain->pstCerts[iIndex].uiCertLen);
        if(NULL==pstVerCertChain->pstCerts[iIndex].pucCert)
        {
            for(iLoop=0;iLoop<iIndex;iLoop++)
            {
                if(pstVerCertChain->pstCerts[iLoop].pucCert)
                {
                    WE_FREE(pstVerCertChain->pstCerts[iLoop].pucCert);
                }
            }
            WE_FREE(pstVerCertChain->pstCerts);
            pstVerCertChain->pstCerts =NULL;
            if(pstVerCertChain->pucAddr)
            {
                WE_FREE(pstVerCertChain->pucAddr);
            }
            if(pstVerCertChain->pucDomain)
            {
                WE_FREE(pstVerCertChain->pucDomain);
            }
            WE_FREE(pstVerCertChain);
            return M_SEC_ERR_INSUFFICIENT_MEMORY;
        }
        (WE_VOID)WE_MEMCPY(pstVerCertChain->pstCerts[iIndex].pucCert,
            pstParamIn->pstCerts[iIndex].pucCert,
            pstVerCertChain->pstCerts[iIndex].uiCertLen);
        }

    
    iRes =Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_TLSVERIFYCERTCHAIN, (void*)pstVerCertChain);
    if(iRes !=M_SEC_ERR_OK)
    {        
        for(iLoop=0;iLoop<pstVerCertChain->ucNumCerts;iLoop++)
        {
            if(pstVerCertChain->pstCerts[iLoop].pucCert)
            {
                WE_FREE(pstVerCertChain->pstCerts[iLoop].pucCert);
            }
        }
        if(pstVerCertChain->pstCerts)
        {
            WE_FREE(pstVerCertChain->pstCerts);
        }
        if(pstVerCertChain->pucAddr)
        {
            WE_FREE(pstVerCertChain->pucAddr);
        }
        if(pstVerCertChain->pucDomain)
        {
            WE_FREE(pstVerCertChain->pucDomain);
        }
        WE_FREE(pstVerCertChain);
        return iRes;
    }
    return M_SEC_ERR_OK;
  
}
/*=====================================================================================
FUNCTION: 
    Sec_MsgTlsCompSign
CREATE DATE: 
    2007-01-19
AUTHOR: 
    tang
DESCRIPTION:
    compute signature by using private key.
ARGUMENTS PASSED:
    WE_HANDLE             hSecHandle[IN]: Global data handle.
    WE_INT32              iTargetID[IN]:  the identity of the invoker input from the external interface.
    const WE_UCHAR * pucPubKeyHash[IN]:public key hash.
    WE_INT32 iPubKeyHashLen[IN]:length of public key hash.
    const WE_UCHAR *pucData[IN]:Pointer to the buffer will be signed
    WE_INT32 iDataLen[IN]:Length of data.
    WE_INT32 iAlg[IN]:M_SEC_SP_RSA.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The status is ok.    
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
WE_INT32 Sec_MsgTlsCompSign (WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                             const WE_UCHAR * pucPubKeyHash,WE_INT32 iPubKeyHashLen,
                             const WE_UCHAR *pucData,WE_INT32 iDataLen, 
                             WE_INT32 iAlg)
{
    St_SecTlsComputeSign * pstCompSig = (St_SecTlsComputeSign *)WE_MALLOC((WE_ULONG)sizeof(St_SecTlsComputeSign));
    WE_INT32 iRes = M_SEC_ERR_OK;
    
    if(NULL == pstCompSig)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstCompSig->iTargetID = iTargetID;
    pstCompSig->pucPubKeyHash= (WE_UCHAR *)pucPubKeyHash;
    pstCompSig->usPubKeyHashLen= (WE_UINT16)iPubKeyHashLen;
    pstCompSig->pucData = (WE_UINT8 *)pucData;
    pstCompSig->usDataLen = (WE_UINT16)iDataLen;
    pstCompSig->ucAlg = (WE_UINT8)iAlg;

    iRes =Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_TLSCOMPSIG, (void*)pstCompSig);
    if(iRes !=M_SEC_ERR_OK)
    {
        WE_FREE(pstCompSig);
        pstCompSig =NULL;
        return iRes;
    }
    
    return M_SEC_ERR_OK;    
}
/*=====================================================================================
FUNCTION: 
    Sec_MsgTlsKeyExchange
CREATE DATE: 
    2007-01-19
AUTHOR: 
    tang
DESCRIPTION:
    run a key exchange operation, using the selected key exchange method
    and get the master secret. The public key is either given in the parameters,
    or must be retrieved from a certificate passed in parameter.
ARGUMENTS PASSED:
    WE_HANDLE         hSecHandle[IN]: Global data handle.
    WE_INT32          iTargetID[IN] : the identity of the invoker input from the external interface.    
    const WE_VOID *pvParam[IN]:Pointer to the param. 
    WE_UINT8 ucAlg[IN]:ucAlg is the Algorithm of key Exchange.
    const WE_UCHAR * pucRandval[IN]:64 bytes of random data, used to generate the master secret.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The status is ok.     
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
WE_INT32 Sec_MsgTlsKeyExchange (WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                const WE_VOID *pvParam, WE_UINT8 ucAlg, 
                                const WE_UCHAR * pucRandval)
{
    St_SecTlsKeyExchangeParams *pstParamMem = NULL;
    WE_UCHAR                   *pucRandValMem = NULL;
    St_SecTlsKeyExchange       *pstKeyExch =NULL;
    WE_INT32 iRes = M_SEC_ERR_OK;
    
    if(NULL == hSecHandle||NULL==pvParam)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    if(NULL != pucRandval)
    {
        /*the length of pucRandval is 64 defined in rfc2246*/
        pucRandValMem = WE_MALLOC((WE_ULONG)(64));
        if(NULL == pucRandValMem)
        {
            return M_SEC_ERR_INSUFFICIENT_MEMORY;
        }
        (void)WE_MEMCPY(pucRandValMem,pucRandval,64);
    }
    pstParamMem=(St_SecTlsKeyExchangeParams *)WE_MALLOC(sizeof(St_SecTlsKeyExchangeParams));
    if(NULL==pstParamMem)
    {
        if(pucRandValMem)
        {
            WE_FREE(pucRandValMem);
        }
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    
    pstParamMem->stSecCert.usBufLen=((St_SecTlsKeyExchangeParams *)pvParam)->stSecCert.usBufLen;   
    switch(ucAlg)
    {
    case E_SEC_TLS_KEYEXCH_DHE_DSS: /* sec_tls_server_dh_params */
    case E_SEC_TLS_KEYEXCH_DHE_DSS_EXPORT:
    case E_SEC_TLS_KEYEXCH_DHE_RSA:
    case E_SEC_TLS_KEYEXCH_DHE_RSA_EXPORT:
        /*copy stTlsDHParam */
        pstParamMem->stDHParam.stTlsDHParam.usDHPLen = ((St_SecTlsKeyExchangeParams *)pvParam)->stDHParam.stTlsDHParam.usDHPLen;
        pstParamMem->stDHParam.stTlsDHParam.usDHGLen = ((St_SecTlsKeyExchangeParams *)pvParam)->stDHParam.stTlsDHParam.usDHGLen;
        pstParamMem->stDHParam.stTlsDHParam.usDHYsLen = ((St_SecTlsKeyExchangeParams *)pvParam)->stDHParam.stTlsDHParam.usDHYsLen;
        pstParamMem->stDHParam.stTlsSignature.usSignLen  =((St_SecTlsKeyExchangeParams *)pvParam)->stDHParam.stTlsSignature.usSignLen;
        
        if(NULL != ((St_SecTlsKeyExchangeParams *)pvParam)->stDHParam.stTlsDHParam.pucDHG \
            && pstParamMem->stDHParam.stTlsDHParam.usDHGLen > 0)
        {
            pstParamMem->stDHParam.stTlsDHParam.pucDHG = 
                WE_MALLOC((WE_ULONG)pstParamMem->stDHParam.stTlsDHParam.usDHGLen);
            
            if(NULL == pstParamMem->stDHParam.stTlsDHParam.pucDHG)
            {
                if(NULL != pucRandValMem)
                {
                    WE_FREE(pucRandValMem);
                }
                WE_FREE(pstParamMem);
                return M_SEC_ERR_INSUFFICIENT_MEMORY;
            }
            
            (void)WE_MEMCPY(pstParamMem->stDHParam.stTlsDHParam.pucDHG,
                ((St_SecTlsKeyExchangeParams *)pvParam)->stDHParam.stTlsDHParam.pucDHG,
                (WE_UINT32)pstParamMem->stDHParam.stTlsDHParam.usDHGLen);
        }
        
        if(NULL != ((St_SecTlsKeyExchangeParams *)pvParam)->stDHParam.stTlsDHParam.pucDHYs
            && pstParamMem->stDHParam.stTlsDHParam.usDHYsLen > 0)
        {
            pstParamMem->stDHParam.stTlsDHParam.pucDHYs = 
                WE_MALLOC((WE_ULONG)pstParamMem->stDHParam.stTlsDHParam.usDHYsLen);
            
            if(NULL == pstParamMem->stDHParam.stTlsDHParam.pucDHYs)
            {
                if(NULL != pucRandValMem)
                {
                    WE_FREE(pucRandValMem);
                }
                if(NULL != pstParamMem->stDHParam.stTlsDHParam.pucDHG)
                {
                    WE_FREE(pstParamMem->stDHParam.stTlsDHParam.pucDHG);
                }                
                WE_FREE(pstParamMem);
                return M_SEC_ERR_INSUFFICIENT_MEMORY;
            }
            
            (void)WE_MEMCPY(pstParamMem->stDHParam.stTlsDHParam.pucDHYs,
                ((St_SecTlsKeyExchangeParams *)pvParam)->stDHParam.stTlsDHParam.pucDHYs,
                (WE_UINT32)pstParamMem->stDHParam.stTlsDHParam.usDHYsLen);                
        }
        if(NULL != ((St_SecTlsKeyExchangeParams *)pvParam)->stDHParam.stTlsDHParam.pucDHP
            && pstParamMem->stDHParam.stTlsDHParam.usDHPLen > 0)
        {
            pstParamMem->stDHParam.stTlsDHParam.pucDHP = 
                WE_MALLOC((WE_ULONG)pstParamMem->stDHParam.stTlsDHParam.usDHPLen);
            
            if(NULL == pstParamMem->stDHParam.stTlsDHParam.pucDHP)
            {
                if(NULL != pucRandValMem)
                {
                    WE_FREE(pucRandValMem);
                }
                if(NULL != pstParamMem->stDHParam.stTlsDHParam.pucDHG)
                {
                    WE_FREE(pstParamMem->stDHParam.stTlsDHParam.pucDHG);
                }
                if(NULL != pstParamMem->stDHParam.stTlsDHParam.pucDHYs)
                {
                    WE_FREE(pstParamMem->stDHParam.stTlsDHParam.pucDHYs);
                }                
                WE_FREE(pstParamMem);
                return M_SEC_ERR_INSUFFICIENT_MEMORY;
            }
            
            (void)WE_MEMCPY(pstParamMem->stDHParam.stTlsDHParam.pucDHP,
                ((St_SecTlsKeyExchangeParams *)pvParam)->stDHParam.stTlsDHParam.pucDHP,
                (WE_UINT32)pstParamMem->stDHParam.stTlsDHParam.usDHPLen);                
        }
        if(NULL != ((St_SecTlsKeyExchangeParams *)pvParam)->stDHParam.stTlsSignature.pucSignature
            && pstParamMem->stDHParam.stTlsSignature.usSignLen > 0)
        {
            pstParamMem->stDHParam.stTlsSignature.pucSignature = 
                WE_MALLOC((WE_ULONG)pstParamMem->stDHParam.stTlsSignature.usSignLen);
            
            if(NULL == pstParamMem->stDHParam.stTlsSignature.pucSignature)
            {
                if(NULL != pucRandValMem)
                {
                    WE_FREE(pucRandValMem);
                }
                if(NULL != pstParamMem->stDHParam.stTlsDHParam.pucDHG)
                {
                    WE_FREE(pstParamMem->stDHParam.stTlsDHParam.pucDHG);
                }
                if(NULL != pstParamMem->stDHParam.stTlsDHParam.pucDHYs)
                {
                    WE_FREE(pstParamMem->stDHParam.stTlsDHParam.pucDHYs);
                }
                if(NULL != pstParamMem->stDHParam.stTlsDHParam.pucDHP)
                {
                    WE_FREE(pstParamMem->stDHParam.stTlsDHParam.pucDHP);
                }                
                WE_FREE(pstParamMem);
                return M_SEC_ERR_INSUFFICIENT_MEMORY;
            }
            
            (void)WE_MEMCPY(pstParamMem->stDHParam.stTlsSignature.pucSignature,
                ((St_SecTlsKeyExchangeParams *)pvParam)->stDHParam.stTlsSignature.pucSignature,
                (WE_UINT32)pstParamMem->stDHParam.stTlsSignature.usSignLen);                
        }
        
        break;
          case E_SEC_TLS_KEYEXCH_RSA:/* When RSA is used */
          case E_SEC_TLS_KEYEXCH_DH_DSS_EXPORT:
          case E_SEC_TLS_KEYEXCH_DH_DSS:
          case E_SEC_TLS_KEYEXCH_DH_RSA_EXPORT:
          case E_SEC_TLS_KEYEXCH_DH_RSA:
              break;
          case  E_SEC_TLS_KEYEXCH_RSA_EXPORT:
              /*copy stRSAParam */
              pstParamMem->stRSAParam.stTlsRSAParam.usModLen = ((St_SecTlsKeyExchangeParams *)pvParam)->stRSAParam.stTlsRSAParam.usModLen;
              pstParamMem->stRSAParam.stTlsRSAParam.usExpLen = ((St_SecTlsKeyExchangeParams *)pvParam)->stRSAParam.stTlsRSAParam.usExpLen;
              pstParamMem->stRSAParam.stTlsSignature.usSignLen = ((St_SecTlsKeyExchangeParams *)pvParam)->stRSAParam.stTlsSignature.usSignLen;
              
              if(NULL != ((St_SecTlsKeyExchangeParams *)pvParam)->stRSAParam.stTlsRSAParam.pucExponent
                  && pstParamMem->stRSAParam.stTlsRSAParam.usExpLen > 0)
              {
                  pstParamMem->stRSAParam.stTlsRSAParam.pucExponent = \
                      WE_MALLOC((WE_ULONG)pstParamMem->stRSAParam.stTlsRSAParam.usExpLen);
                  
                  if(NULL == pstParamMem->stRSAParam.stTlsRSAParam.pucExponent)
                  {
                      if(NULL != pucRandValMem)
                      {
                          WE_FREE(pucRandValMem);
                      }                      
                      WE_FREE(pstParamMem);
                      return M_SEC_ERR_INSUFFICIENT_MEMORY;
                  }
                  
                  (void)WE_MEMCPY(pstParamMem->stRSAParam.stTlsRSAParam.pucExponent,
                      ((St_SecTlsKeyExchangeParams *)pvParam)->stRSAParam.stTlsRSAParam.pucExponent,
                      (WE_UINT32)pstParamMem->stRSAParam.stTlsRSAParam.usExpLen);
              }
              
              if(NULL != ((St_SecTlsKeyExchangeParams *)pvParam)->stRSAParam.stTlsRSAParam.pucModulus \
                  && pstParamMem->stRSAParam.stTlsRSAParam.usModLen > 0)
              {
                  pstParamMem->stRSAParam.stTlsRSAParam.pucModulus = 
                      WE_MALLOC((WE_ULONG)pstParamMem->stRSAParam.stTlsRSAParam.usModLen);
                  
                  if(NULL == pstParamMem->stRSAParam.stTlsRSAParam.pucModulus)
                  {
                      if(NULL != pucRandValMem)
                      {
                          WE_FREE(pucRandValMem);
                      }
                      if(NULL != pstParamMem->stRSAParam.stTlsRSAParam.pucExponent)
                      {
                          WE_FREE(pstParamMem->stRSAParam.stTlsRSAParam.pucExponent);
                      }                      
                      WE_FREE(pstParamMem);
                      return M_SEC_ERR_INSUFFICIENT_MEMORY;
                  }
                  
                  (void)WE_MEMCPY(pstParamMem->stRSAParam.stTlsRSAParam.pucModulus,
                      ((St_SecTlsKeyExchangeParams *)pvParam)->stRSAParam.stTlsRSAParam.pucModulus,
                      (WE_UINT32)pstParamMem->stRSAParam.stTlsRSAParam.usModLen);
              }
              if(NULL != ((St_SecTlsKeyExchangeParams *)pvParam)->stRSAParam.stTlsSignature.pucSignature \
                  && pstParamMem->stRSAParam.stTlsSignature.usSignLen > 0)
              {
                  pstParamMem->stRSAParam.stTlsSignature.pucSignature = 
                      WE_MALLOC((WE_ULONG)pstParamMem->stRSAParam.stTlsSignature.usSignLen);
                  
                  if(NULL == pstParamMem->stRSAParam.stTlsSignature.pucSignature )
                  {
                      if(NULL != pucRandValMem)
                      {
                          WE_FREE(pucRandValMem);
                      }
                      if(NULL != pstParamMem->stRSAParam.stTlsRSAParam.pucExponent)
                      {
                          WE_FREE(pstParamMem->stRSAParam.stTlsRSAParam.pucExponent);
                      }
                      if(NULL != pstParamMem->stRSAParam.stTlsRSAParam.pucModulus)
                      {
                          WE_FREE(pstParamMem->stRSAParam.stTlsRSAParam.pucModulus);
                      }
                      WE_FREE(pstParamMem);
                      return M_SEC_ERR_INSUFFICIENT_MEMORY;
                  }
                  
                  (void)WE_MEMCPY(pstParamMem->stRSAParam.stTlsSignature.pucSignature ,
                      ((St_SecTlsKeyExchangeParams *)pvParam)->stRSAParam.stTlsSignature.pucSignature,
                      (WE_UINT32)pstParamMem->stRSAParam.stTlsSignature.usSignLen);
              }
              
              break;
          case E_SEC_TLS_KEYEXCH_DH_ANON:
          case E_SEC_TLS_KEYEXCH_DH_ANON_EXPORT:  
              pstParamMem->stDHParam.stTlsDHParam.usDHPLen = ((St_SecTlsKeyExchangeParams *)pvParam)->stDHParam.stTlsDHParam.usDHPLen;
              pstParamMem->stDHParam.stTlsDHParam.usDHGLen = ((St_SecTlsKeyExchangeParams *)pvParam)->stDHParam.stTlsDHParam.usDHGLen;
              pstParamMem->stDHParam.stTlsDHParam.usDHYsLen = ((St_SecTlsKeyExchangeParams *)pvParam)->stDHParam.stTlsDHParam.usDHYsLen;
              pstParamMem->stDHParam.stTlsSignature.usSignLen  =((St_SecTlsKeyExchangeParams *)pvParam)->stDHParam.stTlsSignature.usSignLen;
              
              if(NULL != ((St_SecTlsKeyExchangeParams *)pvParam)->stDHParam.stTlsDHParam.pucDHG \
                  &&pstParamMem->stDHParam.stTlsDHParam.usDHGLen > 0)
              {
                  pstParamMem->stDHParam.stTlsDHParam.pucDHG = 
                      WE_MALLOC((WE_ULONG)pstParamMem->stDHParam.stTlsDHParam.usDHGLen);
                  
                  if(NULL == pstParamMem->stDHParam.stTlsDHParam.pucDHG)
                  {
                      if(NULL != pucRandValMem)
                      {
                          WE_FREE(pucRandValMem);
                      }                      
                      WE_FREE(pstParamMem);
                      return M_SEC_ERR_INSUFFICIENT_MEMORY;
                  }
                  
                  (void)WE_MEMCPY(pstParamMem->stDHParam.stTlsDHParam.pucDHG,
                      ((St_SecTlsKeyExchangeParams *)pvParam)->stDHParam.stTlsDHParam.pucDHG,
                      (WE_UINT32)pstParamMem->stDHParam.stTlsDHParam.usDHGLen);
              }
              
              if(NULL != ((St_SecTlsKeyExchangeParams *)pvParam)->stDHParam.stTlsDHParam.pucDHYs
                  &&pstParamMem->stDHParam.stTlsDHParam.usDHYsLen > 0)
              {
                  pstParamMem->stDHParam.stTlsDHParam.pucDHYs = 
                      WE_MALLOC((WE_ULONG)pstParamMem->stDHParam.stTlsDHParam.usDHYsLen);
                  
                  if(NULL ==pstParamMem->stDHParam.stTlsDHParam.pucDHYs)
                  {
                      if(NULL != pucRandValMem)
                      {
                          WE_FREE(pucRandValMem);
                      }
                      if(NULL != pstParamMem->stDHParam.stTlsDHParam.pucDHG)
                      {
                          WE_FREE(pstParamMem->stDHParam.stTlsDHParam.pucDHG);
                      }                      
                      WE_FREE(pstParamMem);
                      return M_SEC_ERR_INSUFFICIENT_MEMORY;
                  }
                  
                  (void)WE_MEMCPY(pstParamMem->stDHParam.stTlsDHParam.pucDHYs,
                      ((St_SecTlsKeyExchangeParams *)pvParam)->stDHParam.stTlsDHParam.pucDHYs,
                      (WE_UINT32)pstParamMem->stDHParam.stTlsDHParam.usDHYsLen);                
              }
              if(NULL != ((St_SecTlsKeyExchangeParams *)pvParam)->stDHParam.stTlsDHParam.pucDHP
                  && pstParamMem->stDHParam.stTlsDHParam.usDHPLen > 0)
              {
                  pstParamMem->stDHParam.stTlsDHParam.pucDHP = 
                      WE_MALLOC((WE_ULONG)pstParamMem->stDHParam.stTlsDHParam.usDHPLen);
                  
                  if(NULL == pstParamMem->stDHParam.stTlsDHParam.pucDHP)
                  {
                      if(NULL != pucRandValMem)
                      {
                          WE_FREE(pucRandValMem);
                      }
                      if(NULL != pstParamMem->stDHParam.stTlsDHParam.pucDHG)
                      {
                          WE_FREE(pstParamMem->stDHParam.stTlsDHParam.pucDHG);
                      }
                      if(NULL != pstParamMem->stDHParam.stTlsDHParam.pucDHYs)
                      {
                          WE_FREE(pstParamMem->stDHParam.stTlsDHParam.pucDHYs);
                      }
                      WE_FREE(pstParamMem);
                      return M_SEC_ERR_INSUFFICIENT_MEMORY;
                  }
                  
                  (void)WE_MEMCPY(pstParamMem->stDHParam.stTlsDHParam.pucDHP,
                      ((St_SecTlsKeyExchangeParams *)pvParam)->stDHParam.stTlsDHParam.pucDHP,
                      (WE_UINT32)pstParamMem->stDHParam.stTlsDHParam.usDHPLen);                
              }
              break;
          default:
              break;
              
       }
       /*copy certificate*/
       if(NULL != ((St_SecTlsKeyExchangeParams *)pvParam)->stSecCert.pucBuf && pstParamMem->stSecCert.usBufLen > 0)
       {
          pstParamMem->stSecCert.pucBuf =  WE_MALLOC((WE_ULONG)pstParamMem->stSecCert.usBufLen);
           
           if(NULL == pstParamMem->stSecCert.pucBuf)
           {
               if(NULL != pucRandValMem)
               {
                   WE_FREE(pucRandValMem);
               }
               if(NULL != pstParamMem->stDHParam.stTlsDHParam.pucDHG)
               {
                   WE_FREE(pstParamMem->stDHParam.stTlsDHParam.pucDHG);
               }
               if(NULL !=pstParamMem->stDHParam.stTlsDHParam.pucDHYs)
               {
                   WE_FREE(pstParamMem->stDHParam.stTlsDHParam.pucDHYs);
               }
               
               if(NULL != pstParamMem->stRSAParam.stTlsRSAParam.pucExponent)
               {
                   WE_FREE(pstParamMem->stRSAParam.stTlsRSAParam.pucExponent);
               }
               if(NULL != pstParamMem->stRSAParam.stTlsRSAParam.pucModulus)
               {
                   WE_FREE(pstParamMem->stRSAParam.stTlsRSAParam.pucModulus);
               }
               if(NULL!=pstParamMem->stRSAParam.stTlsSignature.pucSignature)
               {
                   WE_FREE(pstParamMem->stRSAParam.stTlsSignature.pucSignature);
               }                 
               WE_FREE(pstParamMem);
               return M_SEC_ERR_INSUFFICIENT_MEMORY;
           }
           
           (void)WE_MEMCPY(pstParamMem->stSecCert.pucBuf, ((St_SecTlsKeyExchangeParams *)pvParam)->stSecCert.pucBuf,\
               (WE_UINT32)pstParamMem->stSecCert.usBufLen);                
       }              
       
       
       pstKeyExch = (St_SecTlsKeyExchange*)WE_MALLOC((WE_ULONG)sizeof(St_SecTlsKeyExchange));
       
       if(NULL == pstKeyExch)
       {
           if(NULL != pucRandValMem)
           {
               WE_FREE(pucRandValMem);
           }
           switch(ucAlg)
           {
           case E_SEC_TLS_KEYEXCH_DHE_DSS: /* sec_tls_server_dh_params */
           case E_SEC_TLS_KEYEXCH_DHE_DSS_EXPORT:
           case E_SEC_TLS_KEYEXCH_DHE_RSA:
           case E_SEC_TLS_KEYEXCH_DHE_RSA_EXPORT:
               
               if(NULL != pstParamMem->stDHParam.stTlsDHParam.pucDHG )
               {
                   WE_FREE(pstParamMem->stDHParam.stTlsDHParam.pucDHG);
               }
               
               if(NULL != pstParamMem->stDHParam.stTlsDHParam.pucDHYs)                   
               {
                   WE_FREE(pstParamMem->stDHParam.stTlsDHParam.pucDHYs);             
               }
               if(NULL != pstParamMem->stDHParam.stTlsDHParam.pucDHP)
               {
                   WE_FREE(pstParamMem->stDHParam.stTlsDHParam.pucDHP);              
               }
               if(NULL != pstParamMem->stDHParam.stTlsSignature.pucSignature)
               {
                   WE_FREE(pstParamMem->stDHParam.stTlsSignature.pucSignature);
               }
               
               break;
           case E_SEC_TLS_KEYEXCH_RSA:/* When RSA is used */
           case E_SEC_TLS_KEYEXCH_DH_DSS_EXPORT:
           case E_SEC_TLS_KEYEXCH_DH_DSS:
           case E_SEC_TLS_KEYEXCH_DH_RSA_EXPORT:
           case E_SEC_TLS_KEYEXCH_DH_RSA:
               break;
           case E_SEC_TLS_KEYEXCH_RSA_EXPORT:
               if(NULL != pstParamMem->stRSAParam.stTlsRSAParam.pucExponent)
               {             
                   WE_FREE(pstParamMem->stRSAParam.stTlsRSAParam.pucExponent);
               }
               
               if(NULL != pstParamMem->stRSAParam.stTlsRSAParam.pucModulus )
               {
                   WE_FREE(pstParamMem->stRSAParam.stTlsRSAParam.pucModulus);
               }
               if(NULL != pstParamMem->stRSAParam.stTlsSignature.pucSignature )
               {             
                   WE_FREE(pstParamMem->stRSAParam.stTlsSignature.pucSignature );
               }
               break;
           case E_SEC_TLS_KEYEXCH_DH_ANON:
           case E_SEC_TLS_KEYEXCH_DH_ANON_EXPORT:             
               if(NULL != pstParamMem->stDHParam.stTlsDHParam.pucDHG )
               {             
                   WE_FREE(pstParamMem->stDHParam.stTlsDHParam.pucDHG);
               }
               
               if(NULL != pstParamMem->stDHParam.stTlsDHParam.pucDHYs)
               {
                   WE_FREE(pstParamMem->stDHParam.stTlsDHParam.pucDHYs);                
               }
               if(NULL != pstParamMem->stDHParam.stTlsDHParam.pucDHP
                   && pstParamMem->stDHParam.stTlsDHParam.usDHPLen > 0)
               {              
                   WE_FREE(pstParamMem->stDHParam.stTlsDHParam.pucDHP);                
               }
               break;
           default:
               break;
               
           }
           
           WE_FREE(pstParamMem);
           return M_SEC_ERR_INSUFFICIENT_MEMORY;
       }
       
       /*post message data*/
       pstKeyExch->iTargetID = iTargetID;
       pstKeyExch->pstParam = pstParamMem;
       pstKeyExch->pucRandval = pucRandValMem;
       pstKeyExch->ucHashAlg= ucAlg;
       pstKeyExch->iOptions= ((St_SecTlsKeyExchangeParams *)pvParam)->iOptions;
       
       iRes =Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_TLSKEYEXCHANGE, (void*)pstKeyExch);
       iRes = iRes;
       return M_SEC_ERR_OK;
       
}
/*=====================================================================================
FUNCTION: 
    Sec_MsgTlsGetPrfResult
CREATE DATE: 
    2007-01-19
AUTHOR: 
    tang
DESCRIPTION:
    get the Pseudo Random Function.If the master secret id is 
    to be used, "pucSecret" is NULL, and "iMasterSecretId" 
    denotes which master secret is to use. Or,"pucSecret" must be provided.
ARGUMENTS PASSED:
    WE_HANDLE     hSecHandle[IN]: Global data handle.
    WE_INT32      iTargetID[IN]:  the identity of the invoker input from the external interface.
    WE_UINT8      ucAlg[IN]:  The hash algorithm to use.
    WE_INT32      iMasterSecretId[IN]: ID of the master secret.
    const WE_UCHAR *  pucSecret[IN]:  pointer to the secret.
    WE_INT32      iSecretLen[IN]: the length of the secret.
    const WE_CHAR *   pcLabel[IN]:  pointer to the label.
    const WE_UCHAR *  pucSeed[IN]:  pointer to the seed.
    WE_INT32      iSeedLen[IN]: the length of the seed.
    WE_INT32      iOutputLen[IN]: the length of the output data.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The status is ok.
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
WE_INT32 Sec_MsgTlsGetPrfResult(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_UINT8 ucAlg, 
                                 WE_INT32 iMasterSecretId, const WE_UCHAR * pucSecret, WE_INT32 iSecretLen,
                                 const WE_CHAR * pcLabel, const WE_UCHAR * pucSeed, WE_INT32 iSeedLen,
                                 WE_INT32 iOutputLen)
{
    St_SecGetPrfResult * pstWtlsPrf = (St_SecGetPrfResult *)WE_MALLOC((WE_ULONG)sizeof(St_SecGetPrfResult));
    WE_INT32 iRes = M_SEC_ERR_OK;
    
    if(NULL == pstWtlsPrf)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }

    pstWtlsPrf->iTargetID = (WE_INT32)iTargetID;
    pstWtlsPrf->ucAlg = ucAlg;
    pstWtlsPrf->ucMasterSecretId = (WE_UINT8)iMasterSecretId;
    pstWtlsPrf->pucSecret = (WE_UCHAR *)pucSecret;
    pstWtlsPrf->usSecretLen = (WE_UINT16)iSecretLen;
    pstWtlsPrf->pcLabel = (WE_CHAR *)pcLabel;
    pstWtlsPrf->pucSeed = (WE_UCHAR*)pucSeed;
    pstWtlsPrf->usSeedLen = (WE_UINT16)iSeedLen;
    pstWtlsPrf->usOutputLen = (WE_UINT16)iOutputLen;
  
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_TLSGETPRFRESULT, (void*)pstWtlsPrf);
    if(iRes !=M_SEC_ERR_OK)
    {
        WE_FREE(pstWtlsPrf);
        pstWtlsPrf =NULL;
        return iRes;
    }
    return M_SEC_ERR_OK;
  
}
#endif

/*=====================================================================================
FUNCTION: 
    Sec_MsgWtlsCompSign
CREATE DATE: 
    2006-07-04
AUTHOR: 
    Kevin
DESCRIPTION:
    compute signature by using private key.
ARGUMENTS PASSED:
    WE_HANDLE             hSecHandle[IN]: Global data handle.
    WE_INT32              iTargetID[IN]:  the identity of the invoker input from the external interface.
    const WE_UCHAR *      pucKeyId[IN]:   A byte-encoded Identifier defined in wtls spec.
    WE_INT32              iKeyIdLen[IN]:  the length of the key id.
    const WE_UCHAR *      pucBuf[IN]:     pointer to the data buffer to be signed.
    WE_INT32              iBufLen[IN]:    The length of the buffer.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The status is ok.
    
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
WE_INT32 Sec_MsgWtlsCompSign (WE_HANDLE hSecHandle, WE_INT32 iTargetID,
                              const WE_UCHAR * pucKeyId, WE_INT32 iKeyIdLen,
                              const WE_UCHAR * pucBuf, WE_INT32 iBufLen)
{
    WE_INT32 iRes = 0;
    St_SecWtlsCompSign * pstCompSig = (St_SecWtlsCompSign *)WE_MALLOC((WE_ULONG)sizeof(St_SecWtlsCompSign));
    if(NULL == pstCompSig)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }

    pstCompSig->iTargetID = iTargetID;
    pstCompSig->pucKeyId = (WE_UCHAR *)pucKeyId;
    pstCompSig->usKeyIdLen = (WE_UINT16)iKeyIdLen;
    pstCompSig->pucBuf = (WE_UCHAR *)pucBuf;
    pstCompSig->usBufLen = (WE_UINT16)iBufLen;

    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_WTLSCOMPSIG, (void*)pstCompSig);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstCompSig);
        return iRes;
    }
    
    return M_SEC_ERR_OK;    
}
/*=====================================================================================
FUNCTION: 
    Sec_MsgSetupCon
CREATE DATE: 
    2006-07-04
AUTHOR: 
    Kevin
DESCRIPTION:
    session connection is set up after this function is invoked.
ARGUMENTS PASSED:
    WE_HANDLE             hSecHandle[IN]: Global data handle.
    WE_INT32              iTargetID[IN]: the identity of the invoker input from the external interface.
    WE_INT32              iMasterSecretId[IN]: ID of the master secret.
    WE_INT32              iSecurityId[IN]: ID of the security.
    WE_INT32              iFullHandshake[IN]: the value of the full hand shake.
    St_SecSessionInfo     stSessionInfo[IN]: The value of the session information.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The status is ok.
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
WE_INT32 Sec_MsgSetupCon(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iMasterSecretId, 
                         WE_INT32 iSecurityId, WE_INT32 iFullHandshake,
                         St_SecSessionInfo stSessionInfo)
{
    WE_INT32 iRes = 0;
    St_SecSetupConnection * pstConEst = (St_SecSetupConnection *)WE_MALLOC((WE_ULONG)sizeof(St_SecSetupConnection));
    if(NULL == pstConEst)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }

    pstConEst->iTargetID = iTargetID;
    pstConEst->ucMasterSecretId = (WE_UINT8)iMasterSecretId;
    pstConEst->iSecurity_id = (WE_INT32)iSecurityId;
    pstConEst->ucFull_handshake = (WE_UINT8)iFullHandshake;
    pstConEst->stSessionInfo = stSessionInfo;


    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_SETUPCON, (void*)pstConEst);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstConEst);
        return iRes;
    }
    
    return M_SEC_ERR_OK;  
}

/*=====================================================================================
FUNCTION: 
    Sec_MsgStopCon
CREATE DATE: 
    2006-07-04
AUTHOR: 
    Kevin
DESCRIPTION:
    stop the session connection.
ARGUMENTS PASSED:
    WE_HANDLE     hSecHandle[IN]: Global data handle.
    WE_INT32      iTargetID[IN]: the identity of the invoker input from the external interface.
    WE_INT32      iSecurityId[IN]: ID of the security.
    WE_UINT8      ucConnectionType[IN]: the value of the connection type.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The status is ok.
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
WE_INT32 Sec_MsgStopCon (WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iSecurityId,
                         WE_UINT8 ucConnectionType)
{
    WE_INT32 iRes = 0;
    St_SecStopConnection * pstConTm = (St_SecStopConnection *)WE_MALLOC((WE_ULONG)sizeof(St_SecStopConnection));
    if(NULL == pstConTm)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }

    pstConTm->iTargetID = iTargetID;
    pstConTm->iSecurityId = (WE_INT32)iSecurityId;
    pstConTm->ucConnectionType = ucConnectionType;


    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_STOPCON, (void*)pstConTm);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstConTm);
        return iRes;
    }
    
    return M_SEC_ERR_OK;
}

/*=====================================================================================
FUNCTION: 
    Sec_MsgEnableSession
CREATE DATE: 
    2006-07-04
AUTHOR: 
    Kevin
DESCRIPTION:
    enable/disable the session slot. If ucIsActive 
    is equal to zero, the indicated session must be enabled. 
    or else, it must be disabled. 
ARGUMENTS PASSED:
    WE_HANDLE    hSecHandle[IN]:Global data handle.
    WE_INT32     iMasterSecretId[IN]:The ID of the master secret.
    WE_UINT8     ucIsActive[IN]:The flag to if it is enabled.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The status is ok.
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
WE_INT32 Sec_MsgEnableSession (WE_HANDLE hSecHandle, WE_INT32 iMasterSecretId, WE_UINT8 ucIsActive)
{
    WE_INT32 iRes = 0;
    St_SecEnableSession * pstSessActive = (St_SecEnableSession *)WE_MALLOC((WE_ULONG)sizeof(St_SecEnableSession));
    if(NULL == pstSessActive)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }

    pstSessActive->ucMasterSecretId = (WE_UINT8)iMasterSecretId;
    pstSessActive->ucIsActive = ucIsActive;

    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_ENVSESSION, (void*)pstSessActive);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstSessActive);
        return iRes;
    }
    
    return M_SEC_ERR_OK;  
  
}

/*=====================================================================================
FUNCTION: 
    Sec_MsgDisableSession
CREATE DATE: 
    2006-07-04
AUTHOR: 
    Kevin
DESCRIPTION:
    indicate the session of the given master secret as non-resumable. 
    If a session is as non-resumable, it must not be reused in later sessions.
ARGUMENTS PASSED:
    WE_HANDLE    hSecHandle[IN]:Global data handle.
    WE_INT32     iMasterSecretId[IN]: ID of the master secret.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The status is ok.
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
WE_INT32 Sec_MsgDisableSession (WE_HANDLE hSecHandle, WE_INT32 iMasterSecretId)
{
    WE_INT32 iRes = 0;
    St_SecDisableSession * pstSesInval = (St_SecDisableSession *)WE_MALLOC((WE_ULONG)sizeof(St_SecDisableSession));
    if(NULL == pstSesInval)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }

    pstSesInval->ucMasterSecretId = (WE_UINT8)iMasterSecretId;    


    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_DISSESSION, (void*)pstSesInval);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstSesInval);
        return iRes;
    }
    
    return M_SEC_ERR_OK;
}

/*=====================================================================================
FUNCTION: 
    Sec_MsgRemovePeer
CREATE DATE: 
    2006-07-04
AUTHOR: 
    Kevin
DESCRIPTION:
    Remove all peers that have a link to the master secret.
ARGUMENTS PASSED:
    WE_HANDLE    hSecHandle[IN]:Global data handle.
    WE_INT32     iMasterSecretId[IN]:The ID of the master secret.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The status is ok.
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
WE_INT32 Sec_MsgRemovePeer (WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iMasterSecretId)
{
    WE_INT32 iRes = 0;
    St_SecRemovePeer * pstPeerDelLink = (St_SecRemovePeer *)WE_MALLOC((WE_ULONG)sizeof(St_SecRemovePeer));
    if(NULL == pstPeerDelLink)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstPeerDelLink->iTargetID = iTargetID;
    pstPeerDelLink->ucMasterSecretId = (WE_UINT8)iMasterSecretId;


    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_DELPEER, (void*)pstPeerDelLink);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstPeerDelLink);
        return iRes;
    }
    
    return M_SEC_ERR_OK;
}

/*=====================================================================================
FUNCTION: 
    Sec_MsgAttachPeerToSession
CREATE DATE: 
    2006-07-04
AUTHOR: 
    Kevin
DESCRIPTION:
    attach a peer that links to the master secret. 
    If a peer exists, this function will overwrite the former peer.
ARGUMENTS PASSED:
    WE_HANDLE          hSecHandle[IN]:Global data handle.
    WE_UINT8           ucConnectionType[IN]: Type of the connection.
    const WE_UCHAR *   pucAddress[IN]: Pointer to the address.
    WE_INT32           iAddressLen[IN]: the length of the address.
    const WE_CHAR *    pcDomain[IN]: Pointer to the domain value.
    WE_UINT16          usPortnum[IN]:the number of the port.
    WE_INT32           iMasterSecretId[IN]: ID of the master secret.

RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The status is ok.
    
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
WE_INT32 Sec_MsgAttachPeerToSession (WE_HANDLE hSecHandle, WE_UINT8 ucConnectionType,
                                     const WE_UCHAR * pucAddress, WE_INT32 iAddressLen,
                                     const WE_CHAR * pcDomain, WE_UINT16 usPortnum,
                                     WE_INT32 iMasterSecretId)
{
    WE_INT32 iRes = 0;
    St_SecAttachPeer * pstPeerLinkSes = (St_SecAttachPeer *)WE_MALLOC((WE_ULONG)sizeof(St_SecAttachPeer));
    if(NULL == pstPeerLinkSes)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }

    pstPeerLinkSes->ucConnectionType = ucConnectionType;
    pstPeerLinkSes->pucAddress = (WE_UCHAR *)pucAddress;
    pstPeerLinkSes->usAddressLen = (WE_UINT16)iAddressLen;
    pstPeerLinkSes->usPortnum = usPortnum;
    pstPeerLinkSes->ucMasterSecretId = (WE_UINT8)iMasterSecretId;
    
    if(NULL == pcDomain)
    {    
        pstPeerLinkSes->pucDomain = NULL;
        pstPeerLinkSes->usDomainLen = 0;
    }
    else
    {
        pstPeerLinkSes->pucDomain = (WE_UCHAR*)pcDomain;
        pstPeerLinkSes->usDomainLen = (WE_UINT16)SEC_STRLEN(pcDomain);
    }    

    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_ATTACHPEER, (void*)pstPeerLinkSes);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstPeerLinkSes);
        return iRes;
    }
    
    return M_SEC_ERR_OK;    
}

/*=====================================================================================
FUNCTION: 
    Sec_MsgSearchPeer
CREATE DATE: 
    2006-07-04
AUTHOR: 
    Kevin
DESCRIPTION:
    search peer based on usPortnum&pucAddress.If there is a matching peer,
    it is not necessary to build up a new session.
ARGUMENTS PASSED:
    WE_HANDLE         hSecHandle[IN]: Global data handle.
    WE_INT32          iTargetID[IN]: the identity of the invoker input from the external interface.
    WE_UINT8          ucConnectionType[IN]:Type of the connection.
    const WE_UCHAR *  pucAddress[IN]:Pointer to the address.
    WE_INT32          iAddressLen[IN]:The length of the address.
    const WE_CHAR *   pcDomain[IN]:Pointer to the domain.
    WE_UINT16         usPortnum[IN]:The number of the port.

RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The status is ok.
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
WE_INT32 Sec_MsgSearchPeer (WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_UINT8 ucConnectionType,
                            const WE_UCHAR * pucAddress, WE_INT32 iAddressLen,
                            const WE_CHAR * pcDomain, WE_UINT16 usPortnum)
{
    WE_INT32 iRes = 0;
    St_SecSearchPeer * pstPeerLU = (St_SecSearchPeer *)WE_MALLOC((WE_ULONG)sizeof(St_SecSearchPeer));
    if(NULL == pstPeerLU)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }

    pstPeerLU->iTargetID = iTargetID;
    pstPeerLU->ucConnectionType = ucConnectionType;
    pstPeerLU->pucAddress = (WE_UCHAR *)pucAddress;
    pstPeerLU->usAddressLen = (WE_UINT16)iAddressLen;
    
    if(NULL == pcDomain)
    {    
        pstPeerLU->pucDomain = NULL;
        pstPeerLU->usDomainLen = 0;
    }
    else
    {
        pstPeerLU->pucDomain = (WE_UCHAR*)pcDomain;
        pstPeerLU->usDomainLen = (WE_UINT16)SEC_STRLEN(pcDomain);
    }
    
    pstPeerLU->usPortnum = usPortnum;


    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_SEARCHPEER, (void*)pstPeerLU);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstPeerLU);
        return iRes;
    }
    
    return M_SEC_ERR_OK;
}

/*=====================================================================================
FUNCTION: 
    Sec_MsgGetSessionInfo
CREATE DATE: 
    2006-07-04
AUTHOR: 
    Kevin
DESCRIPTION:
    get the contents of the indicated session.
ARGUMENTS PASSED:
    WE_HANDLE     hSecHandle[IN]:Global data handle.
    WE_INT32      iTargetID[IN]: the identity of the invoker input from the external interface.
    WE_INT32      iMasterSecretId[IN]:ID of the master secret.
RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
        The result is error.
    M_SEC_ERR_OK:
        The status is ok.
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
WE_INT32 Sec_MsgGetSessionInfo (WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iMasterSecretId)
{
    WE_INT32 iRes = 0;
    St_SecFetchSessionInfo * pstSesFetch = (St_SecFetchSessionInfo *)WE_MALLOC((WE_ULONG)sizeof(St_SecFetchSessionInfo));
    if(NULL == pstSesFetch)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }

    pstSesFetch->iTargetID = iTargetID;
    pstSesFetch->ucMasterSecretId = (WE_UINT8)iMasterSecretId;


    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_GETSESINFO, (void*)pstSesFetch);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstSesFetch);
        return iRes;
    }
    
    return M_SEC_ERR_OK;   
}

/*=====================================================================================
FUNCTION: 
    Sec_MsgRenewSession
CREATE DATE: 
    2006-07-04
AUTHOR: 
    Kevin
DESCRIPTION:
    update and store session info.
ARGUMENTS PASSED:
    WE_HANDLE     hSecHandle[IN]:Global data handle.
    WE_INT32      iMasterSecretId[IN]: ID of the master secret.
    WE_UINT8      ucSessionOptions[IN]: the value of the session options.
    const WE_UCHAR *  pucSessionId[IN]: Pointer to the session id.
    WE_UINT8      ucSessionIdLen[IN]: the length of the session id.
    WE_UINT8      ucCipherSuite[2][IN]:The array of the cipher suite.
    WE_UINT8      ucCompressionAlg[IN]: The value of the compression alg.
    const WE_UCHAR *  pucPrivateKeyId[IN]: Pointer to the private key id.
    WE_UINT32     uiCreationTime[IN]: The value of the creation time.

RETURN VALUE:
    M_SEC_ERR_INSUFFICIENT_MEMORY:
    M_SEC_ERR_INVALID_PARAMETER:
        The result is error.
    M_SEC_ERR_OK:
        The status is ok.
    
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
WE_INT32 Sec_MsgRenewSession (WE_HANDLE hSecHandle, WE_INT32 iMasterSecretId, 
                              WE_UINT8 ucSessionOptions, const WE_UCHAR * pucSessionId, 
                              WE_UINT8 ucSessionIdLen,  WE_UINT8 aucCipherSuite[2],
                              WE_UINT8 ucCompressionAlg, const WE_UCHAR * pucPrivateKeyId,
                              WE_UINT32 uiCreationTime)
{
    WE_INT32 iRes = 0;
    St_SecUpdateSession * pstSesUpd = NULL;  
    if(NULL == aucCipherSuite)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    pstSesUpd = (St_SecUpdateSession *)WE_MALLOC((WE_ULONG)sizeof(St_SecUpdateSession));
    if(NULL == pstSesUpd)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    pstSesUpd->ucMasterSecretId = (WE_UINT8)iMasterSecretId;
    pstSesUpd->ucSessionOptions = ucSessionOptions;
    pstSesUpd->pucSessionId = (WE_UCHAR *)pucSessionId;
    pstSesUpd->ucSessionIdLen = ucSessionIdLen;
    pstSesUpd->ucCipherSuite[0] = aucCipherSuite[0];
    pstSesUpd->ucCipherSuite[1] = aucCipherSuite[1];
    pstSesUpd->ucCompressionAlg = ucCompressionAlg;
    pstSesUpd->pucPrivateKeyId = (WE_UCHAR *)pucPrivateKeyId;
    pstSesUpd->uiCreationTime = uiCreationTime;


    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_UPSESSION, (void*)pstSesUpd);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstSesUpd);
        return iRes;
    }    
    
    return M_SEC_ERR_OK;    
}

