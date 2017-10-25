/*==================================================================================================
    HEADER NAME : sec_resp.c
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
    
    
    GENERAL DESCRIPTION
        In this file,define the response function prototype. and all the response 
        function will invoke the callback registered by the client.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-07-07   Kevin Yang        None         Draft
==================================================================================================*/

/*******************************************************************************
*   Include File Section
*******************************************************************************/

#include "sec_comm.h" 

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/

#define M_SEC_WAP_CALLBACK_FUN              (((ISec *)hSecHandle)->hcbSecEvtFunc)
#define M_SEC_BRSA_CALLBACK_FUN             (((ISec *)hSecHandle)->hcbSecEvtFunc)

#define M_SEC_WAP_CB_PRIVATE_DATA           (((ISec *)hSecHandle)->hSecPrivDataCB)
#define M_SEC_BRSA_CB_PRIVATE_DATA          (((ISec *)hSecHandle)->hSecPrivDataCB)

#define M_SEC_AEECALLBACKK_RESP       (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->m_cb)
#define M_SEC_ISHELL_RESP                   0

#define M_SEC_RESPCALLBACK       0
#define M_SEC_RESPCALLBACKDATA  (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pvRespCBData)
#define M_SEC_RESPCALLBACKFD  (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->iFd)
#define M_SEC_RESPCALLBACKEVENT  (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->iRespEvent)

static WE_VOID Sec_RunRespCallBackFun(WE_VOID* pvData);
static WE_VOID Sec_FreeRespCBStruc(WE_INT32 iEvent,WE_VOID* pvData);

/*******************************************************************************
*   Function Define Section
*******************************************************************************/

/*==================================================================================================
FUNCTION: 
    Sec_SignTextResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    sign text reponse for browser application.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iSignId[IN]:ID of the sign input from the invoker.
    WE_INT32 iAlgorithm[IN]:the value of the algorithm.
    WE_CHAR * pucSignature[IN]:Pointer to the digital sign nature.
    WE_INT32 iSigLen[IN]:The length of the sign
    WE_CHAR * pucHashedKey[IN]:Pointer to the hashed key.
    WE_INT32 iHashedKeyLen[IN]:Length of the hashed key.
    WE_CHAR * pucCertificate[IN]:Pointer to certificate.
    WE_INT32 iCertificateLen[IN]:The length of certificate.
    WE_INT32 iCertificateType[IN]:The type of the certificate.
    WE_INT32 iErr[IN]:The value of the err.
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
#ifdef M_SEC_CFG_CAN_SIGN_TEXT
void Sec_SignTextResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iSignId, WE_INT32 iAlgorithm,
                      const WE_CHAR * pucSignature, WE_INT32 iSigLen,
                      const WE_CHAR * pucHashedKey, WE_INT32 iHashedKeyLen,
                      const WE_CHAR * pucCertificate, WE_INT32 iCertificateLen,
                      WE_INT32 iCertificateType, WE_INT32 iErr)
{
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeSignText(&puiMsg ,  iTargetID,  iSignId,  iAlgorithm,
                      pucSignature,  iSigLen,
                      pucHashedKey,  iHashedKeyLen,
                      pucCertificate, iCertificateLen,
                      iCertificateType, iErr);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);  
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }

}
#endif 
/*==================================================================================================
FUNCTION: 
    Sec_GetPrfResultResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    get prf result and return the result to the stk.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]: Global data handle.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]:The value of the result.
    WE_UCHAR * pucBuf[IN]:Pointer to the data buffer of prf result.
    WE_INT32 iBufLen[IN]:The length of the buffer.
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
void Sec_GetPrfResultResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult,
                              const WE_UCHAR * pucBuf, WE_INT32 iBufLen)
{
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeWtlsGetPrfResult(&puiMsg ,  iTargetID,  iResult,
                              pucBuf,  iBufLen);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }
}
/*==================================================================================================
FUNCTION: 
    Sec_WtlsVerifySveCertResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    verify gateway certificate chain and return the result.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]:The value of the result.
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
void Sec_WtlsVerifySvrCertResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult)
{    
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeWtlsVerifySvrCert(&puiMsg ,  iTargetID,  iResult);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }

}

/*==================================================================================================
FUNCTION: 
    Sec_WtlsGetUsrCertResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    get the user certificate and give the operation result.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]:The value of the result.
    WE_UCHAR * pucKeyId[IN]:A byte-encoded Identity which is used to 
                        recognize the client private key related with the cert 
                        in the buffer pucCert.
    WE_INT32 iKeyIdLen[IN]:The length of the keyID.
    WE_UCHAR * pucCert[IN]:Pointer to the cert content.
    WE_INT32 iCertLen[IN]:The length of the cert.
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
void Sec_WtlsGetUsrCertResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult,
                            const WE_UCHAR * pucKeyId, WE_INT32 iKeyIdLen,
                            const WE_UCHAR * pucCert, WE_INT32 iCertLen)
{
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeWtlsGetUsrCert(&puiMsg ,   iTargetID,  iResult,
                             pucKeyId,  iKeyIdLen,
                             pucCert,  iCertLen);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }

}

#ifdef M_SEC_CFG_TLS
void Sec_TlsGetUsrCertResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult,
                            const WE_UCHAR * pucPubkeyHash,  WE_UINT16   usPubKeyHashLen,
                            St_SecTlsAsn1Certificate *pstCert, WE_INT32 iNbrCerts)
{
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeTlsGetUsrCert(&puiMsg ,   iTargetID,  iResult,
                             pucPubkeyHash,  usPubKeyHashLen,
                             pstCert,  iNbrCerts);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;

    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }


}

/*VerifySvrCert:WTLS and Tls have same response structure*/
void Sec_TlsVerifySvrCertResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult)
{   
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeWtlsVerifySvrCert(&puiMsg ,  iTargetID,  iResult);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;

    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }


}

WE_VOID Sec_TlsGetCipherSuiteResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult,
                                const WE_UCHAR * pucCipherSuites, WE_INT32 iCipherSuitesLen)
{    
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeTlsGetCipherSuite(&puiMsg ,  iTargetID,  (WE_UINT16)iResult,
                                pucCipherSuites,  (WE_UINT16)iCipherSuitesLen);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }


}

void Sec_TlsCompSigResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult,
                         const WE_UCHAR * pucSignature, WE_INT32 iSignatureLen)
{    
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeWtlsCompSig(&puiMsg ,  iTargetID,  iResult,
                         pucSignature,  iSignatureLen);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }

}

void Sec_SSLGetMasterSecretResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult,
                         WE_UCHAR *pucMasterSecret)
{    
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeSSLGetMasterSecret(&puiMsg ,  iTargetID,  iResult,
                         pucMasterSecret,  M_SEC_TLS_MASTER_SECRET_LEN);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }

}

#endif

/*==================================================================================================
FUNCTION: 
    Sec_WtlsGetCipherSuiteResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    Get cipher suite, and return the result to the caller
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]:The value of the result.
    WE_UCHAR * pucCipherMethods[IN]:a sequence of elements of cipher suite.
    WE_INT32 iCipherMethodsLen[IN]:The length of the cipher method.
    WE_UCHAR * pucKeyExchangeIds[IN]:pointer including key exchange methods.
    WE_INT32 iKeyExchangeIdsLen[IN]:The length of the key exchange id.
    WE_UCHAR * pucTrustedKeyIds[IN]:Pointer to the trusted CA DN.
    WE_INT32 iTrustedKeyIdsLen[IN]:The length of the trusted key id.
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
void Sec_WtlsGetCipherSuiteResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult,
                                const WE_UCHAR * pucCipherMethods, WE_INT32 iCipherMethodsLen,
                                const WE_UCHAR * pucKeyExchangeIds, WE_INT32 iKeyExchangeIdsLen,
                                const WE_UCHAR * pucTrustedKeyIds, WE_INT32 iTrustedKeyIdsLen)
{    
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_WtlsGetCipherSuiteResp(iTargetID = %d, iResult = %d )\n",\
              iTargetID, iResult));
    iRes = Sec_EncodeWtlsGetCipherSuite(&puiMsg ,  iTargetID,  (WE_UINT16)iResult,
                                pucCipherMethods,  (WE_UINT16)iCipherMethodsLen,
                                pucKeyExchangeIds,  (WE_UINT16)iKeyExchangeIdsLen,
                                pucTrustedKeyIds,  (WE_UINT16)iTrustedKeyIdsLen);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }

}
/*==================================================================================================
FUNCTION: 
    Sec_KeyExchangeResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    get key exchange, ang return the result to the stk
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]:The value of the result.
    WE_INT32 iMasterSecretId[IN]:ID of the master secret.
    WE_CHAR * pucPublicValue[IN]:A public value computed by the key exchange 
                                method to be sent to the server side.
    WE_INT32 iPublicValueLen[IN]:The length of the public value.
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
void Sec_KeyExchangeResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult,
                             WE_INT32 iMasterSecretId, const WE_UCHAR * pucPublicValue,
                             WE_INT32 iPublicValueLen)
{    
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeWtlsKeyExchange(&puiMsg ,  iTargetID,  iResult,
                              iMasterSecretId, pucPublicValue,
                              iPublicValueLen);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }

}
/*==================================================================================================
FUNCTION: 
    Sec_WtlsCompSigResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    compute the signature and give the result.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]:The value of the result.
    WE_UCHAR * pucSignature[IN]:Pointer to the digital signature.
    WE_INT32 iSignatureLen[IN]:The length of the sign nature.
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
void Sec_WtlsCompSigResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult,
                         const WE_UCHAR * pucSignature, WE_INT32 iSignatureLen)
{    
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeWtlsCompSig(&puiMsg ,  iTargetID,  iResult,
                         pucSignature,  iSignatureLen);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }

}

/*==================================================================================================
FUNCTION: 
    Sec_SearchPeerResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    response for search peer operation.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]:The value of the result.
    WE_UINT8 ucConnection_type[IN]:the connection type.
    WE_INT32 iMasterSecretID[IN]:ID of the master secret.
    WE_INT32 iSecurityID[IN]:ID of the security.
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
void Sec_SearchPeerResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult,
                        WE_UINT8 ucConnectionType,WE_INT32 iMasterSecretID,
                        WE_INT32 iSecurityID)
{
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_SearchPeerResp(iTargetID = %d, iResult = %d, connectionType = %u, iMasterSecretID = %d )\n",\
              iTargetID, iResult, ucConnectionType, iMasterSecretID));
    iRes = Sec_EncodeSearchPeer(&puiMsg ,  iTargetID,  iResult,
                         ucConnectionType, iMasterSecretID,
                         iSecurityID);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }

}
/*==================================================================================================
FUNCTION: 
    Sec_SessionGetResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    response for get session information operation.
ARGUMENTS PASSED:
   WE_HANDLE hSecHandle[IN]:Global data handle.
   WE_INT32 iTargetID[IN]:The identity of the invoker.
   WE_INT32 iResult[IN]:The value of the result.
   WE_UINT8 ucSessionOptions[IN]:The value of the session option.
   WE_UCHAR * pucSessionId[IN]:Pointer to the ID of the session.
   WE_UINT8 ucSessionIdLen[IN]: Length of the session id.
   WE_UINT8 ucCipherSuite[2][IN]:array for cipher suite.
   WE_UINT8 ucCompressionAlg[IN]: The value of th alg.
   WE_UCHAR * pucPrivateKeyId[IN]:Pointer to the Id of the private key.
   WE_UINT32 uiCreationTime[IN]:The value of the creation time.
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
void Sec_SessionGetResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult,
                        WE_UINT8 ucSessionOptions, const WE_UCHAR * pucSessionId,
                        WE_UINT8 ucSessionIdLen, WE_UINT8 *ucCipherSuite,
                        WE_UINT8 ucCompressionAlg, const WE_UCHAR * pucPrivateKeyId,
                        WE_UINT32 uiCreationTime)
{
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
              "Sec_SessionGetResp(iTargetID = %d, iResult = %d, ucSessionOptions = %u, ucSessionIdLen = %d )\n",\
              iTargetID, iResult, ucSessionOptions, ucSessionIdLen));
    iRes = Sec_EncodeSessionGet(&puiMsg , iTargetID,  (WE_UINT16)iResult,
                         ucSessionOptions, pucSessionId,
                         (WE_UINT16)ucSessionIdLen, ucCipherSuite,
                         ucCompressionAlg, pucPrivateKeyId,
                         uiCreationTime);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }

}

/*==================================================================================================
FUNCTION: 
    Sec_SaveCertResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    response for save certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:The handle of the global data.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]:The value of the result.
   
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
void Sec_SaveCertResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult)
{
    WE_INT32 iRes = 0;
    St_SecSaveCertResp * pResp = NULL;
    
    if(NULL == hSecHandle)
    {
        return ;
    }

    pResp = WE_MALLOC((WE_ULONG)sizeof(St_SecSaveCertResp));
    if(NULL == pResp)
    {
        return;
    }
    
    pResp->iTargetID = iTargetID;
    pResp->usResult = (WE_UINT16)iResult;
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_CERTSAVE_RESP, (void*)pResp);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_SIGNAL_DESTRUCT(0, (WE_INT32)M_SEC_DP_MSG_STORE_CERT_RESP, (void*)pResp);
        return;
    }    
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_AEECALLBACKK_RESP,Sec_RunMainFlow,hSecHandle);

}
/*==================================================================================================
FUNCTION: 
    Sec_TrustedCAInfoResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    response for trusted ca information.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:The handle of the global data.
    WE_INT32 iWid[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]:The value of the result.
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
void Sec_TrustedCAInfoResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult)
{
    WE_INT32 iRes = 0;
    St_SecSaveCertResp * pResp = NULL;
    if(NULL == hSecHandle)
    {
        return ;
    }
    pResp = WE_MALLOC((WE_ULONG)sizeof(St_SecSaveCertResp));
    if(NULL == pResp)
    {
        return;
    }
 
    pResp->iTargetID = iTargetID;
    pResp->usResult = (WE_UINT16)iResult;
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_TRUSTEDCA_RESP, (void*)pResp);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_SIGNAL_DESTRUCT(0, (WE_INT32)M_SEC_DP_MSG_TRUSTED_CA_INFO_RESP, (void*)pResp);
        return;
    }    
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_AEECALLBACKK_RESP,Sec_RunMainFlow,hSecHandle);

}
/*==================================================================================================
FUNCTION: 
    Sec_CertDeliveryResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    delivery certificate response
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:The handle of the global data.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]:The value of the result.
    
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
void Sec_CertDeliveryResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult)
{
    WE_INT32 iRes = 0;
    St_SecSaveCertResp * pResp = NULL;
    if(NULL == hSecHandle)
    {
        return ;
    }

    pResp = WE_MALLOC((WE_ULONG)sizeof(St_SecSaveCertResp));
    if(NULL == pResp)
    {
        return;
    }
  
    pResp->iTargetID = iTargetID;
    pResp->usResult = (WE_UINT16)iResult;
    iRes = Sec_PostMsg(hSecHandle, (WE_INT32)E_SEC_MSG_DLVCERT_RESP, (void*)pResp);
    if (iRes != M_SEC_ERR_OK)
    {
        WE_SIGNAL_DESTRUCT(0, (WE_INT32)M_SEC_DP_MSG_DELVERY_CERT_RESP, (void*)pResp);
        return;
    }    
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_AEECALLBACKK_RESP,Sec_RunMainFlow,hSecHandle);    
}

/*==================================================================================================
FUNCTION: 
    Sec_GetCertNameListResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    response for view name list of certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:The handle of the global data.
    WE_INT32 iWid[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]:The value of the result.
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
void Sec_GetCertNameListResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult)
{         
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeGetCertNameList(&puiMsg , iTargetID,  iResult);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }
    
}

/*==================================================================================================
FUNCTION: 
    Sec_DeleteCertResponse
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    response for delete certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:The handle of the global data.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iCertId[IN]:The certificate id.
    WE_INT32 iResult[IN]:The value of the result.
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
void Sec_DeleteCertResponse(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iCertId, WE_INT32 iResult)
{
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeDeleteCert(&puiMsg ,  iTargetID,  iCertId,  iResult);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }

}

/*==================================================================================================
FUNCTION: 
    Sec_KeyPairGenResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    response for generate key pair.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]: The value of the reslut.
    WE_UINT8 ucKeyType[IN]: The value of the key type
    WE_UCHAR * pucPublicKey[IN]:Pointer to the public key.
    WE_INT32 iPublicKeyLen[IN]:The length of the public key.
    WE_UCHAR * pucSig[IN]:Pointer to the value of the sign
    WE_INT32 iSigLen[IN]:Length of the sign.
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
/* modify by Sam [070119] */
/* #if( defined(M_SEC_CFG_WTLS_CLASS_3) ||defined(M_SEC_CFG_CAN_SIGN_TEXT) ) */
void Sec_KeyPairGenResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult, WE_UINT8 ucKeyType, 
                        const WE_UCHAR * pucPublicKey, WE_INT32 iPublicKeyLen, 
                        const WE_UCHAR * pucSig, WE_INT32 iSigLen)
{
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeKeyPairGen(&puiMsg ,  iTargetID,  iResult,  ucKeyType, 
                        pucPublicKey, iPublicKeyLen, 
                        pucSig,  iSigLen);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }

}

/*==================================================================================================
FUNCTION: 
    Sec_GetPubKeyResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    get user public key response
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]: The value of the reslut.
    WE_UCHAR * pucPublicKey[IN]:Pointer to the public key.
    WE_INT32 iPublicKeyLen[IN]:The length of the public key.
    WE_UCHAR * pucSig[IN]:Pointer to the value of the sign
    WE_INT32 iSigLen[IN]:Length of the sign.
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
void Sec_GetPubKeyResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult, 
                      const WE_UCHAR * pucPublicKey, WE_INT32 iPublicKeyLen,
                      const WE_UCHAR * pucSig, WE_INT32 iSigLen)
{    
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeGetPubKey(&puiMsg ,  iTargetID,  iResult, 
                      pucPublicKey,  iPublicKeyLen,
                      pucSig, iSigLen);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }

}
/*==================================================================================================
FUNCTION: 
    Sec_ModifyPinResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    modify pin response.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]: The value of the reslut.
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
void Sec_ModifyPinResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult)
{    
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeModifyPin(&puiMsg ,  iTargetID,  iResult);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }

}
 
/* #endif */

/*==================================================================================================
FUNCTION: 
    Sec_ViewAndGetCertResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    response for view certificate information and get get certificate content
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]: The value of the reslut.
    WE_INT32 certId[IN]:ID of the cert.
    WE_UCHAR * pucCert[IN]:Pointer to the cert.
    WE_INT32 iCertLen[IN]:Length of the cert.
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
void Sec_ViewAndGetCertResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult, 
                            WE_INT32 certId, WE_UCHAR * pucCert, WE_INT32 iCertLen)
{    
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeViewAndGetCert(&puiMsg ,  iTargetID,  iResult, 
                             certId,  pucCert,  iCertLen);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }

}

/*==================================================================================================
FUNCTION: 
    Sec_GetCurSvrCertResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    view current used certificate information 
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]: The value of the reslut.
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
void Sec_GetCurSvrCertResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult)
{    
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeGetCurSvrCert(&puiMsg ,  iTargetID,  iResult);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }

}
/*==================================================================================================
FUNCTION: 
    Sec_GetSessionInfoResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    view the current session information
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]: The value of the reslut.
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
void Sec_GetSessionInfoResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult)
{       
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeGetSessionInfo(&puiMsg ,  iTargetID,  iResult);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }
}
/*==================================================================================================
FUNCTION: 
    Sec_WtlsCurClassResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    get current sec class response
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iSecClass[IN]:The value of the sec class.
    WE_INT32 iInfoAvailable[IN]:The flg of the info available.
    WE_INT32 iCertAvailable[IN]:The flg of the cert available.
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
void Sec_WtlsCurClassResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iSecClass,
                          WE_INT32 iInfoAvailable, WE_INT32 iCertAvailable)
{    
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeWtlsCurClass(&puiMsg ,   iTargetID,  iSecClass,
                           iInfoAvailable,  iCertAvailable);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }

}

/*==================================================================================================
FUNCTION: 
    Sec_ClrSessionResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    clear session response
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]: The value of the reslut.
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
void Sec_ClrSessionResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult)
{    
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeClrSession(&puiMsg ,  iTargetID,  iResult);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }
}

#if( (defined(M_SEC_CFG_CAN_SIGN_TEXT) && defined(M_SEC_CFG_STORE_CONTRACTS)))

/*==================================================================================================
FUNCTION: 
    Sec_GetContractsListResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    view contract content response
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]: The value of the reslut.
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
void Sec_GetContractsListResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult)
{    
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeGetContractsList(&puiMsg ,  iTargetID,  iResult);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }
}

/*==================================================================================================
FUNCTION: 
    Sec_RemoveContractResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    remove contract response
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:Global data handle.
    WE_INT32 iTargetID[IN]:The identity of the invoker.
    WE_INT32 iResult[IN]: The value of the reslut.
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
void Sec_RemoveContractResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult)
{    
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeRemoveContract(&puiMsg ,  iTargetID,  iResult);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }
    
}


#endif 


/*==================================================================================================
FUNCTION: 
    Sec_DelKeysUserCertsByTypeResponse
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle:Global data handle.
    WE_INT32 iTargetID:The identity of the invoker.
    WE_INT32 iResult:The value of the reslut.
    
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
void Sec_DelKeysUserCertsByTypeResponse(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult)
{
    hSecHandle =hSecHandle;
    iTargetID = iTargetID;
    iResult = iResult;
    return ;
}

/*==================================================================================================
FUNCTION: 
    Sec_UserCertReqResp
CREATE DATE:
    2006-11-08
AUTHOR:
    stone an
DESCRIPTION:
    handle the request message of user certificate.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle:Global data handle.
    WE_INT32 iTargetID:The identity of the invoker.
    WE_INT32 iResult:The value of the reslut.
    WE_UCHAR* pucCertReqMsg: request message.
    WE_UINT32 uiCertReqMsgLen: length of request message.    
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
void Sec_UserCertReqResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult, 
                                   WE_UCHAR* pucCertReqMsg, WE_UINT32 uiCertReqMsgLen)
{    
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeUserCertReq(&puiMsg ,   iTargetID,  iResult, 
                                    pucCertReqMsg,  uiCertReqMsgLen);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }
}
    

/*==================================================================================================
FUNCTION: 
    Sec_ChangeWTLSCertAbleResp
CREATE DATE:
    2006-11-08
AUTHOR:
    stone an
DESCRIPTION:
    handle the change WTLS certificate state.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle:Global data handle.
    WE_INT32 iTargetID:The identity of the invoker.
    WE_INT32 iResult:The value of the reslut.
    WE_UINT8 ucState: state of WTLS certificate.
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
void Sec_ChangeWTLSCertAbleResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, 
                                        WE_INT32 iResult, WE_UINT8 ucState)
{    
    WE_UINT8* puiMsg = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    iRes = Sec_EncodeChangeWTLSCertAble(&puiMsg ,   iTargetID,  iResult, ucState);
    if(iRes < 0)
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
        return;
    }
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)puiMsg;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    /*free data*/
    if(NULL != puiMsg)
    {
        WE_FREE(puiMsg);
    }
}
/*==================================================================================================
FUNCTION: 
    Sec_RunRespCallBackFun
CREATE DATE:
    2006-12-09
AUTHOR:
    Bird 
DESCRIPTION:
    callback function of response 
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

static WE_VOID Sec_RunRespCallBackFun(WE_VOID* hSecHandle)
{
    WE_VOID* pvCBData = NULL;
    if(NULL == hSecHandle)
    {
        return;
    }
    pvCBData = M_SEC_RESPCALLBACKDATA;
    M_SEC_RESPCALLBACKDATA = NULL;
    /*send data*/
    Sec_SendMsg(M_SEC_RESPCALLBACKFD,pvCBData);
    
}
/*==================================================================================================
FUNCTION: 
    Sec_ReleaseRespCB
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

WE_VOID Sec_ReleaseRespCB(WE_VOID* hSecHandle)
{

}
/*==================================================================================================
FUNCTION: 
    Sec_FreeRespCBStruc
CREATE DATE:
    2006-12-19
AUTHOR:
    Bird
DESCRIPTION:
    free response callback data when the callback is called
ARGUMENTS PASSED:
    WE_INT32 iEvent:event
    WE_VOID* pvData:data
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

static WE_VOID Sec_FreeRespCBStruc(WE_INT32 iEvent,WE_VOID* pvData)
{
    WE_LOG_MSG((0,0,"SEC:Sec_FreeCBStruc\n"));
    if(NULL == pvData)
    {
        return;
    }
    switch(iEvent)
    {
        case G_GETPRF_RESP:
            {
                St_SecGetPrfResp* pstData = (St_SecGetPrfResp*)pvData;
                M_SEC_SELF_FREE(pstData->pucBuf);
                
            }
            break;
        case G_VERIFYSVRCERT_RESP:
            /*
            St_SecVerifySvrCertResp;
            */
            break;
        case G_GETUSERCERT_RESP:
            {
                St_SecGetUserCertResp* pstData = (St_SecGetUserCertResp*)pvData;
                M_SEC_SELF_FREE(pstData->pucCert);
                M_SEC_SELF_FREE(pstData->pucKeyId);
            }
            break;
        case G_TLS_GETUSERCERT_RESP:
            {
                WE_INT32 iLoop = 0;
                St_SecTlsGetUserCertResp* pstData = (St_SecTlsGetUserCertResp*)pvData;
                M_SEC_SELF_FREE(pstData->pucPubKeyHash);
                for(iLoop = 0;iLoop < pstData->ucNbrCerts;iLoop ++)
                {                 
                    M_SEC_SELF_FREE(pstData->pstCert[iLoop].pucCert);
                }
                M_SEC_SELF_FREE(pstData->pstCert);
            }
            break;
        case G_GETCIPHERSUITE_RESP:
            {
                St_SecGetCipherSuiteResp* pstData = (St_SecGetCipherSuiteResp*)pvData;
                M_SEC_SELF_FREE(pstData->pucCipherMethods);
                M_SEC_SELF_FREE(pstData->pucKeyExchangeIds);
                M_SEC_SELF_FREE(pstData->pucTrustedKeyIds);
            }
            break;
        case G_TLS_GETCIPHERSUITE_RESP:
            {          
                St_SecTlsGetCipherSuitesResp* pstData = (St_SecTlsGetCipherSuitesResp*)pvData;
                M_SEC_SELF_FREE(pstData->pucCipherSuites);           
            }
            break;
        case G_KEYEXCH_RESP:
            {
                St_SecKeyExchResp* pstData = (St_SecKeyExchResp*)pvData;
                M_SEC_SELF_FREE(pstData->pucPreMsKey);
            }
            break;
        case G_COMPUTESIGN_RESP:
            {
                St_SecCompSignResp* pstData =(St_SecCompSignResp*)pvData ;
                M_SEC_SELF_FREE(pstData->pucSignature);
            }
            break;
        case G_SEARCHPEER_RESP:
            /*
            St_SecSearchPeerResp;
            */
            break;
        case G_SESSIONGET_RESP:
            {
                St_SecSessionGetResp* pstData = (St_SecSessionGetResp*)pvData ;
                M_SEC_SELF_FREE(pstData->pucPrivateKeyId);
                M_SEC_SELF_FREE(pstData->pucSessionId);
            }
            break;
        case G_GETCERTNAME_RESP:
            /*
            St_SecGetCertNameListResp;
            */
            break;
        case G_DELCERT_RESP:
            /*
            St_SecRemCertResp;
            */
            break;
        case G_GENKEYPAIR_RESP:
            {
                St_SecGenKeyPairResp* pstData = (St_SecGenKeyPairResp*)pvData;
                M_SEC_SELF_FREE(pstData->pucPKHashSig);
                M_SEC_SELF_FREE(pstData->pucPublicKey);
            }
            break;
        case G_GETPUBKEY_RESP:
            {
                St_GetPubkeyResp* pstData = (St_GetPubkeyResp*)pvData;
                M_SEC_SELF_FREE(pstData->pucPubKey);
                M_SEC_SELF_FREE(pstData->pucPubKeySig);
            }
            break;
        case G_MODIFYPIN_RESP:
            /*
            St_SecModifyPinResp;
            */
            break;
        case G_VIEWGETCERT_RESP:
            {
                St_SecGetCertContentResp* pstData = (St_SecGetCertContentResp*)pvData;
                M_SEC_SELF_FREE(pstData->pucCert);
            }
            break;
        case G_GETCURSVRCERT_RESP:
            /*
            St_SecGetCurSvrCertResp;
            */
            break;
        case G_GETSESSIONINFO_RESP:
            /*
            St_SecGetSessionInfoResp;
            */
            break;
        case G_WTLSCURCLASS_RESP:
            /*
            St_SecGetWtlsCurClassResp;
            */
            break;
        case G_SESSIONCLEAR_RESP:
            /*
            St_SecSessionClearResp;
            */
            break;
        case G_GETCONTRACT_RESP:
            /*
            St_SecGetContractsListResp;
            */
            break;
        case G_DELCONTRACT_RESP:
            /*
            St_SecDelContractResp;
            */
            break;
        case G_USERCERTREQ_RESP:
            {
                St_SecUserCertRequestResp* pstData = (St_SecUserCertRequestResp*)pvData;
                M_SEC_SELF_FREE(pstData->pucCertReqMsg);
            }
            break;
        case G_CHANGECERTSTATE_RESP:
            /*
            St_ChangeCertStateResp;
            */
            break;
        case G_SIGNTEXT_RESP:
            {
                St_SecSignTextResp* pstData = (St_SecSignTextResp*)pvData;
                M_SEC_SELF_FREE(pstData->pcCertificate);
                M_SEC_SELF_FREE(pstData->pcHashedKey);
                M_SEC_SELF_FREE(pstData->pcSignature);
            }
            break;
        default:
            WE_LOG_MSG((0,0,"SEC:Sec_FreeCBStruc, Unknown Event\n"));
            break;
                
    }
    M_SEC_SELF_FREE(pvData);
}
