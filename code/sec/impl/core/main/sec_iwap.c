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
    St_SecWtlsGetPrfResult * pstWtlsPrf = (St_SecWtlsGetPrfResult *)WE_MALLOC((WE_ULONG)sizeof(St_SecWtlsGetPrfResult));

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
                                   const WE_UCHAR *pucBuf, WE_INT32 iBufLen,
                                   const WE_UCHAR *pucAddr, WE_INT32 iAddrLen,
                                   const WE_CHAR * pcDomain)
{
    WE_INT32 iRes = 0;
    St_SecWtlsVerifySvrCert * pstVerCertChain = (St_SecWtlsVerifySvrCert*)WE_MALLOC((WE_ULONG)sizeof(St_SecWtlsVerifySvrCert));
    if(NULL == pstVerCertChain)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }

    pstVerCertChain->iTargetID = iTargetID;
    pstVerCertChain->pucBuf = (WE_UCHAR *)pucBuf;
    pstVerCertChain->usBufLen = (WE_UINT16)iBufLen;
    pstVerCertChain->pucAddr = (WE_UCHAR *)pucAddr;
    pstVerCertChain->usAddrLen = (WE_UINT16)iAddrLen;
    
    if(NULL == pcDomain)
    {    
        pstVerCertChain->pucDomain = NULL;
        pstVerCertChain->usDomainLen = 0;
    }
    else
    {
        pstVerCertChain->pucDomain = (WE_UCHAR *)pcDomain;
        pstVerCertChain->usDomainLen = (WE_UINT16)SEC_STRLEN(pcDomain);
    }

    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_WTLSVERIFYSVRCERT, (void*)pstVerCertChain);
    if (iRes != M_SEC_ERR_OK)
    {
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
                                 St_SecWtlsKeyExchParams stParam, WE_UINT8 ucHashAlg, 
                                 const WE_UCHAR * pucRandval)
{
    WE_INT32 iRes = 0;
    St_SecWtlsKeyExchange * pstKeyExch = (St_SecWtlsKeyExchange*)WE_MALLOC((WE_ULONG)sizeof(St_SecWtlsKeyExchange));

    if(NULL == pstKeyExch)
    {
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    
    pstKeyExch->iTargetID = iTargetID;
    pstKeyExch->stParam = stParam;
    pstKeyExch->ucHashAlg = ucHashAlg;
    pstKeyExch->pucRandval = (WE_UCHAR*)pucRandval;

    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_WTLSKEYEXCHANGE, (void*)pstKeyExch);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_FREE(pstKeyExch);
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

