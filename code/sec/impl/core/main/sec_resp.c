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
#define M_SEC_ISHELL_RESP                   (((ISec*)hSecHandle)->m_pIShell)

#define M_SEC_RESPCALLBACK       (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->m_SecRespCB)
#define M_SEC_RESPCALLBACKDATA  (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pvRespCBData)
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
    St_SecSignTextResp*    pstSignText = NULL;
    if(NULL == hSecHandle)
    {    
        return ;
    }
    pstSignText = (St_SecSignTextResp*)WE_MALLOC(sizeof(St_SecSignTextResp));
    if((NULL == pstSignText) )
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    pstSignText->iTargetID = iTargetID;
    pstSignText->iSignId = iSignId;
    pstSignText->ucAlgorithm = (WE_UINT8)iAlgorithm;
    
    pstSignText->pcSignature = (WE_CHAR *)WE_MALLOC((WE_ULONG)iSigLen);
    if((NULL == pstSignText->pcSignature) && (0 != iSigLen))
    {
        M_SEC_SELF_FREE(pstSignText);        
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    (WE_VOID)WE_MEMCPY(pstSignText->pcSignature,pucSignature,(WE_UINT32)iSigLen);
    pstSignText->usSigLen = (WE_UINT16)iSigLen;
    
    pstSignText->pcHashedKey = (WE_CHAR *)WE_MALLOC((WE_ULONG)iHashedKeyLen);
    if((NULL == pstSignText->pcHashedKey) && (0 != iHashedKeyLen))
    {
        M_SEC_SELF_FREE(pstSignText->pcSignature);
        M_SEC_SELF_FREE(pstSignText);        
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    (WE_VOID)WE_MEMCPY(pstSignText->pcSignature,pucHashedKey,(WE_UINT32)iHashedKeyLen);
    pstSignText->usHashedKeyLen = (WE_UINT16)iHashedKeyLen;
    
    pstSignText->pcCertificate = (WE_CHAR *)WE_MALLOC((WE_ULONG)iCertificateLen);
    if((NULL == pstSignText->pcCertificate) && (0 != iCertificateLen))
    {
        M_SEC_SELF_FREE(pstSignText->pcSignature);
        M_SEC_SELF_FREE(pstSignText->pcHashedKey);
        M_SEC_SELF_FREE(pstSignText);        
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    (WE_VOID)WE_MEMCPY(pstSignText->pcCertificate,pucCertificate,(WE_UINT32)iCertificateLen);
    pstSignText->usCertificateLen = (WE_UINT16)iCertificateLen;
    
    pstSignText->ucCertificateType = (WE_UINT8)iCertificateType;
    pstSignText->ucErr = (WE_UINT8)iErr;

    /*fill the struct*/
    M_SEC_RESPCALLBACKEVENT = G_SIGNTEXT_RESP;
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)pstSignText;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   

}
#endif 
/*==================================================================================================
FUNCTION: 
    Sec_WtlsGetPrfResultResp
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
void Sec_WtlsGetPrfResultResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult,
                              const WE_UCHAR * pucBuf, WE_INT32 iBufLen)
{
    St_SecGetPrfResp*   pstPrf    = NULL;
    if(NULL == hSecHandle)
    {        
        return ;
    }
    pstPrf = (St_SecGetPrfResp*)WE_MALLOC(sizeof(St_SecGetPrfResp));
    if((NULL == pstPrf) )
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    pstPrf->iTargetID = iTargetID;
    pstPrf->usResult = (WE_UINT16)iResult;
    
    pstPrf->pucBuf = (WE_UCHAR*)WE_MALLOC((WE_ULONG)iBufLen);
    if((NULL == pstPrf->pucBuf) && (0 != iBufLen))
    {
        M_SEC_SELF_FREE(pstPrf);        
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    (WE_VOID)WE_MEMCPY(pstPrf->pucBuf,pucBuf,(WE_UINT32)iBufLen);
    pstPrf->usBufLen = (WE_UINT16)iBufLen;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_WtlsGetPrfResultResp\n"));
    
    /*fill the struct*/
    M_SEC_RESPCALLBACKEVENT = G_GETPRF_RESP;
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)pstPrf;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);

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
    St_SecVerifySvrCertResp*  pstVerSvrCert = NULL;
    if(NULL == hSecHandle)
    {        
        return ;
    }
    pstVerSvrCert = (St_SecVerifySvrCertResp*)WE_MALLOC(sizeof(St_SecVerifySvrCertResp));
    if((NULL == pstVerSvrCert) )
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }   
    pstVerSvrCert->iTargetID = iTargetID;
    pstVerSvrCert->usResult = (WE_UINT16)iResult;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_WtlsVerifySvrCertResp\n"));
    
    /*fill the struct*/
    M_SEC_RESPCALLBACKEVENT = G_VERIFYSVRCERT_RESP;
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)pstVerSvrCert;

    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   

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
    St_SecGetUserCertResp*    pstGetUserCert = NULL;
    if(NULL == hSecHandle)
    {        
        return ;
    }
    pstGetUserCert = (St_SecGetUserCertResp*)WE_MALLOC(sizeof(St_SecGetUserCertResp));
    if((NULL == pstGetUserCert) )
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }    
    pstGetUserCert->iTargetID = iTargetID;
    pstGetUserCert->usResult = (WE_UINT16)iResult;
    
    pstGetUserCert->pucKeyId = (WE_UCHAR*)WE_MALLOC((WE_ULONG)iKeyIdLen);
    if((NULL == pstGetUserCert->pucKeyId) && (0 != iKeyIdLen))
    {
        M_SEC_SELF_FREE(pstGetUserCert);        
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    (WE_VOID)WE_MEMCPY(pstGetUserCert->pucKeyId ,pucKeyId,(WE_UINT32)iKeyIdLen);
    pstGetUserCert->usKeyIdLen = (WE_UINT16)iKeyIdLen;
    
    pstGetUserCert->pucCert = (WE_UCHAR*)WE_MALLOC((WE_ULONG)iCertLen);
    if((NULL == pstGetUserCert->pucCert) && (0 != iCertLen))
    {
        M_SEC_SELF_FREE(pstGetUserCert->pucKeyId);
        M_SEC_SELF_FREE(pstGetUserCert);        
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    (WE_VOID)WE_MEMCPY(pstGetUserCert->pucCert,pucCert,(WE_UINT32)iCertLen);
    pstGetUserCert->usCertLen = (WE_UINT16)iCertLen;

    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_WtlsGetUsrCertResp\n"));
    /*fill the struct*/
    M_SEC_RESPCALLBACKEVENT = G_GETUSERCERT_RESP;
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)pstGetUserCert;
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);    

}

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
    St_SecGetCipherSuiteResp*  pstGetCipherSuite = NULL;
    if(NULL == hSecHandle)
    {        
        return ;
    }
    pstGetCipherSuite = (St_SecGetCipherSuiteResp*)WE_MALLOC(sizeof(St_SecGetCipherSuiteResp));
    if((NULL == pstGetCipherSuite) )
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }    
    pstGetCipherSuite->iTargetID = iTargetID;
    pstGetCipherSuite->usResult = (WE_UINT16)iResult;
    
    pstGetCipherSuite->pucCipherMethods = (WE_UCHAR *)WE_MALLOC((WE_ULONG)iCipherMethodsLen);
    if((NULL == pstGetCipherSuite->pucCipherMethods) && (0 != iCipherMethodsLen))
    {
        M_SEC_SELF_FREE(pstGetCipherSuite);
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    (WE_VOID)WE_MEMCPY(pstGetCipherSuite->pucCipherMethods,pucCipherMethods,(WE_UINT32)iCipherMethodsLen);
    pstGetCipherSuite->usCipherMethodsLen = (WE_UINT16)iCipherMethodsLen;
    
    pstGetCipherSuite->pucKeyExchangeIds = (WE_UCHAR *)WE_MALLOC((WE_ULONG)iKeyExchangeIdsLen);
    if((NULL == pstGetCipherSuite->pucKeyExchangeIds) && (0 != iKeyExchangeIdsLen ))
    {
        M_SEC_SELF_FREE(pstGetCipherSuite->pucCipherMethods);
        M_SEC_SELF_FREE(pstGetCipherSuite);
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    (WE_VOID)WE_MEMCPY(pstGetCipherSuite->pucKeyExchangeIds,pucKeyExchangeIds,(WE_UINT32)iKeyExchangeIdsLen);
    pstGetCipherSuite->usKeyExchangeIdsLen = (WE_UINT16)iKeyExchangeIdsLen;
    
    pstGetCipherSuite->pucTrustedKeyIds = (WE_UCHAR *)WE_MALLOC((WE_ULONG)iTrustedKeyIdsLen);
    if((NULL == pstGetCipherSuite->pucTrustedKeyIds) && (0 != iTrustedKeyIdsLen))
    {
        M_SEC_SELF_FREE(pstGetCipherSuite->pucKeyExchangeIds);
        M_SEC_SELF_FREE(pstGetCipherSuite->pucCipherMethods);
        M_SEC_SELF_FREE(pstGetCipherSuite);
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    pstGetCipherSuite->usTrustedKeyIdsLen = (WE_UINT16)iTrustedKeyIdsLen;
    (WE_VOID)WE_MEMCPY( pstGetCipherSuite->pucTrustedKeyIds,pucTrustedKeyIds,(WE_UINT32)iTrustedKeyIdsLen );

    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_WtlsGetCipherSuiteResp\n"));
    /*fill the struct*/
    M_SEC_RESPCALLBACKEVENT= G_GETCIPHERSUITE_RESP;
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)pstGetCipherSuite;
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   

}
/*==================================================================================================
FUNCTION: 
    Sec_WtlsKeyExchangeResp
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
void Sec_WtlsKeyExchangeResp(WE_HANDLE hSecHandle, WE_INT32 iTargetID, WE_INT32 iResult,
                             WE_INT32 iMasterSecretId, const WE_UCHAR * pucPublicValue,
                             WE_INT32 iPublicValueLen)
{    
    St_SecKeyExchResp*   pstKeyExch = NULL;
    if(NULL == hSecHandle)
    {
        return ;
    }
    pstKeyExch = (St_SecKeyExchResp*)WE_MALLOC(sizeof(St_SecKeyExchResp));
    if((NULL == pstKeyExch) )
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }        
    pstKeyExch->iTargetID = iTargetID;
    pstKeyExch->usResult = (WE_UINT16)iResult;
    pstKeyExch->ucMasterSecretId = (WE_UINT8)iMasterSecretId;
    
    pstKeyExch->pucPreMsKey = (WE_UCHAR *)WE_MALLOC((WE_ULONG)iPublicValueLen);
    if((NULL == pstKeyExch->pucPreMsKey) && (0 != iPublicValueLen))
    {
        M_SEC_SELF_FREE(pstKeyExch);
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    (WE_VOID)WE_MEMCPY(pstKeyExch->pucPreMsKey,pucPublicValue,(WE_UINT32)iPublicValueLen);
    pstKeyExch->usPreMsKeyLen = (WE_UINT16)iPublicValueLen;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_WtlsKeyExchangeResp\n"));
    
    /*fill the struct*/
    M_SEC_RESPCALLBACKEVENT = G_KEYEXCH_RESP;
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)pstKeyExch;
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);    

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
    St_SecCompSignResp*  pstCompSig = NULL;	
    if(NULL == hSecHandle)
    {
        return ;
    }
    pstCompSig = (St_SecCompSignResp*)WE_MALLOC(sizeof(St_SecCompSignResp));
    if((NULL == pstCompSig) )
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }        
    pstCompSig->iTargetID = iTargetID;
    pstCompSig->usResult = (WE_UINT16)iResult;
    
    pstCompSig->pucSignature = (WE_UCHAR*)WE_MALLOC((WE_ULONG)iSignatureLen);
    if((NULL == pstCompSig->pucSignature) && (0 != iSignatureLen))
    {
        M_SEC_SELF_FREE(pstCompSig);
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    (WE_VOID)WE_MEMCPY(pstCompSig->pucSignature,pucSignature,(WE_UINT32)iSignatureLen);
    pstCompSig->usSignatureLen = (WE_UINT16)iSignatureLen;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_WtlsCompSigResp\n"));
    /*fill the struct*/
    M_SEC_RESPCALLBACKEVENT = G_COMPUTESIGN_RESP;

    M_SEC_RESPCALLBACKDATA = (WE_VOID*)pstCompSig;
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);    

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
    St_SecSearchPeerResp*  pstSearchPeer = NULL;
    if(NULL == hSecHandle)
    {
        return ;
    }
    pstSearchPeer = (St_SecSearchPeerResp*)WE_MALLOC(sizeof(St_SecSearchPeerResp));
    if((NULL == pstSearchPeer) )
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    pstSearchPeer->iTargetID = iTargetID;
    pstSearchPeer->usResult = (WE_UINT16)iResult;
    pstSearchPeer->ucConnectionType = ucConnectionType;
    pstSearchPeer->ucMasterSecretId = (WE_UINT8)iMasterSecretID;
    pstSearchPeer->iSecId = iSecurityID;

    /*fill the struct*/
    M_SEC_RESPCALLBACKEVENT = G_SEARCHPEER_RESP;
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)pstSearchPeer;

    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);

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
{/* modify by Sam [070126] */
    St_SecSessionGetResp*   pstGetSession = NULL;
    if(NULL == hSecHandle)
    {
        return ;
    }
    pstGetSession = (St_SecSessionGetResp*)WE_MALLOC(sizeof(St_SecSessionGetResp));
    if((NULL == pstGetSession) )
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    } 
    (WE_VOID)WE_MEMSET(pstGetSession, 0x00, sizeof(St_SecSessionGetResp));
    pstGetSession->iTargetID = iTargetID;
    pstGetSession->usResult = (WE_UINT16)iResult;
    if (iResult == M_SEC_ERR_OK)
    {    
        pstGetSession->ucSessionOptions = ucSessionOptions;
        
        pstGetSession->pucSessionId = (WE_UCHAR*)WE_MALLOC((WE_ULONG)ucSessionIdLen);
        if((NULL == pstGetSession->pucSessionId) && (0 != ucSessionIdLen))
        {
            WE_LOG_MSG((0,0,"SEC:Sec_SessionGetResp Session ID Malloc Error!\n"));
            M_SEC_SELF_FREE(pstGetSession);
            SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
            return;
        }
        (WE_VOID)WE_MEMCPY(pstGetSession->pucSessionId,pucSessionId,ucSessionIdLen);
        pstGetSession->usSessionIdLen = (WE_UINT16)ucSessionIdLen;
        if (NULL == ucCipherSuite)
        {
            pstGetSession->aucCipherSuite[0] = 0xff;
            pstGetSession->aucCipherSuite[1] = 0xff;
        }
        else
        {
            pstGetSession->aucCipherSuite[0] = ucCipherSuite[0];
            pstGetSession->aucCipherSuite[1] = ucCipherSuite[1];
        }
        pstGetSession->ucCompressionAlg = ucCompressionAlg;
        
        pstGetSession->pucPrivateKeyId = (WE_UCHAR*)SEC_STRDUP((WE_CHAR*)pucPrivateKeyId);
        if(NULL == pstGetSession->pucPrivateKeyId)
        {
            WE_LOG_MSG((0,0,"SEC:Sec_SessionGetResp PrivateKeyId Malloc Error!\n"));
            M_SEC_SELF_FREE(pstGetSession->pucSessionId);
            M_SEC_SELF_FREE(pstGetSession);
            SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
            return;
        }
        pstGetSession->uiCreationTime = uiCreationTime;
    }
    /*fill the struct*/
    M_SEC_RESPCALLBACKEVENT = G_SESSIONGET_RESP;

    M_SEC_RESPCALLBACKDATA = (WE_VOID*)pstGetSession;
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);

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
    St_SecGetCertNameListResp*   pstGetCertNamesList = NULL;
    if(NULL == hSecHandle)
    {
        return ;
    }
    pstGetCertNamesList = (St_SecGetCertNameListResp*)WE_MALLOC(sizeof(St_SecGetCertNameListResp));
    if((NULL == pstGetCertNamesList) )
    {
          SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    pstGetCertNamesList->iTargetID = iTargetID;
    pstGetCertNamesList->usResult = (WE_UINT16)iResult;
    
    /*fill the struct*/
    M_SEC_RESPCALLBACKEVENT = G_GETCERTNAME_RESP;
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)pstGetCertNamesList;
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);
    
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
    St_SecRemCertResp*  pstDelCert = NULL;
    if(NULL == hSecHandle)
    {
        return ;
    }
    pstDelCert = (St_SecRemCertResp*)WE_MALLOC(sizeof(St_SecRemCertResp));
    if((NULL == pstDelCert) )
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    pstDelCert->iTargetID = iTargetID;
    pstDelCert->usResult = (WE_UINT16)iResult;
    pstDelCert->ucCertId = (WE_UINT8)iCertId;

    /*fill the struct*/
    M_SEC_RESPCALLBACKEVENT = G_DELCERT_RESP;

    M_SEC_RESPCALLBACKDATA = (WE_VOID*)pstDelCert;
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);    

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
    St_SecGenKeyPairResp*  pstKeyPair = NULL;
    if(NULL == hSecHandle)
    {
        return ;
    }
    pstKeyPair = (St_SecGenKeyPairResp*)WE_MALLOC(sizeof(St_SecGenKeyPairResp));
    if((NULL == pstKeyPair) )
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    pstKeyPair->iTargetID = iTargetID;
    
    pstKeyPair->pucPublicKey = (WE_UCHAR *)WE_MALLOC((WE_ULONG)iPublicKeyLen);
    if((NULL == pstKeyPair->pucPublicKey) && (0 != iPublicKeyLen))
    {
        M_SEC_SELF_FREE(pstKeyPair);
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    (WE_VOID)WE_MEMCPY(pstKeyPair->pucPublicKey,pucPublicKey,(WE_UINT32)iPublicKeyLen);
    pstKeyPair->usPublicKeyLen = (WE_UINT16)iPublicKeyLen;
    
    pstKeyPair->pucPKHashSig = (WE_UCHAR *)WE_MALLOC((WE_ULONG)iSigLen);
    if((NULL == pstKeyPair->pucPKHashSig) && (0 != iSigLen))
    {
        M_SEC_SELF_FREE(pstKeyPair->pucPublicKey);
        M_SEC_SELF_FREE(pstKeyPair);
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    (WE_VOID)WE_MEMCPY(pstKeyPair->pucPKHashSig,pucSig,(WE_UINT32)iSigLen);
    pstKeyPair->usPKHashSigLen = (WE_UINT16)iSigLen;
    
    pstKeyPair->ucKeyType = ucKeyType;
    pstKeyPair->usResult = (WE_UINT16)iResult;

    /*fill the struct*/
    M_SEC_RESPCALLBACKEVENT = G_GENKEYPAIR_RESP;
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)pstKeyPair;

    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);

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
    St_GetPubkeyResp*   pstGetPubKey = NULL;
    if(NULL == hSecHandle)
    {
        return ;
    }
    pstGetPubKey = (St_GetPubkeyResp*)WE_MALLOC(sizeof(St_GetPubkeyResp));
    if((NULL == pstGetPubKey) )
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    pstGetPubKey->iTargetID = iTargetID;
    pstGetPubKey->pucPubKey = (WE_UCHAR *)WE_MALLOC((WE_ULONG)iPublicKeyLen);
    if((NULL == pstGetPubKey->pucPubKey) && (0 != iPublicKeyLen))
    {
        M_SEC_SELF_FREE(pstGetPubKey);
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    (WE_VOID)WE_MEMCPY(pstGetPubKey->pucPubKey,pucPublicKey,(WE_UINT32)iPublicKeyLen);
    pstGetPubKey->usPubKeyLen = (WE_UINT16)iPublicKeyLen;
    
    pstGetPubKey->pucPubKeySig = (WE_UCHAR *)WE_MALLOC((WE_ULONG)iSigLen);
    if((NULL == pstGetPubKey->pucPubKeySig) && (0 != iSigLen))
    {
        M_SEC_SELF_FREE(pstGetPubKey->pucPubKey);
        M_SEC_SELF_FREE(pstGetPubKey);
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    (WE_VOID)WE_MEMCPY(pstGetPubKey->pucPubKeySig,pucSig,(WE_UINT32)iSigLen);
    pstGetPubKey->usPubKeySigLen = (WE_UINT16)iSigLen;
    
    pstGetPubKey->usResult = (WE_UINT16)iResult;
    /*fill the struct*/
    M_SEC_RESPCALLBACKEVENT = G_GETPUBKEY_RESP;
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)pstGetPubKey;

    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);

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
    St_SecModifyPinResp*   pstModifyPin = NULL;
    if(NULL == hSecHandle)
    {
        return ;
    }
    pstModifyPin = (St_SecModifyPinResp*)WE_MALLOC(sizeof(St_SecModifyPinResp));
    if((NULL == pstModifyPin) )
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    pstModifyPin->iTargetID = iTargetID;
    pstModifyPin->usResult = (WE_UINT16)iResult;
    
    /*fill the struct*/
    M_SEC_RESPCALLBACKEVENT = G_MODIFYPIN_RESP;
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)pstModifyPin;

    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);    

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
    St_SecGetCertContentResp*   pstGetCertContent = NULL;
    if(NULL == hSecHandle)
    {        
        return ;
    }
    pstGetCertContent = (St_SecGetCertContentResp*)WE_MALLOC(sizeof(St_SecGetCertContentResp));
    if((NULL == pstGetCertContent) )
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    pstGetCertContent->iTargetID = iTargetID;
    
    pstGetCertContent->pucCert = (WE_UCHAR*)WE_MALLOC((WE_ULONG)iCertLen);
    if((NULL == pstGetCertContent->pucCert) && (0 != iCertLen))
    {
        M_SEC_SELF_FREE(pstGetCertContent);
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    (WE_VOID)WE_MEMCPY(pstGetCertContent->pucCert,pucCert,(WE_UINT32)iCertLen);
    pstGetCertContent->usCertLen = (WE_UINT16)iCertLen;
    
    pstGetCertContent->ucCertId = (WE_UINT8)certId;
    pstGetCertContent->usResult = (WE_UINT16)iResult;
    
    /*fill the struct*/
    M_SEC_RESPCALLBACKEVENT = G_VIEWGETCERT_RESP;
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)pstGetCertContent;

    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);

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
    St_SecGetCurSvrCertResp* pstGetCurCert = NULL;
    if(NULL == hSecHandle)
    {
        return ;
    }
    pstGetCurCert = (St_SecGetCurSvrCertResp*)WE_MALLOC(sizeof(St_SecGetCurSvrCertResp));
    if((NULL == pstGetCurCert) )
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    pstGetCurCert->iTargetID = iTargetID;
    pstGetCurCert->usResult = (WE_UINT16)iResult;
    
    /*fill the struct*/
    M_SEC_RESPCALLBACKEVENT= G_GETCURSVRCERT_RESP;
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)pstGetCurCert;

    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);    /*modify by zhanghuanqing 06/10/10*/

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
    St_SecGetSessionInfoResp* pstGetSessionInfo = NULL;
    if(NULL == hSecHandle)
    {
        return ;
    }
    pstGetSessionInfo = (St_SecGetSessionInfoResp*)WE_MALLOC(sizeof(St_SecGetSessionInfoResp));
    if((NULL == pstGetSessionInfo) )
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    pstGetSessionInfo->iTargetID = iTargetID;
    pstGetSessionInfo->usResult = (WE_UINT16)iResult;
    /*fill the struct*/
    M_SEC_RESPCALLBACKEVENT = G_GETSESSIONINFO_RESP;
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)pstGetSessionInfo;

    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);

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
    St_SecGetWtlsCurClassResp*   pstCurSecCls = NULL;
    if(NULL == hSecHandle)
    {
        return ;
    }
    pstCurSecCls = (St_SecGetWtlsCurClassResp*)WE_MALLOC(sizeof(St_SecGetWtlsCurClassResp));
    if((NULL == pstCurSecCls) )
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    pstCurSecCls->iTargetID = iTargetID;
    pstCurSecCls->ucCertReady = (WE_UINT8)iCertAvailable;
    pstCurSecCls->ucSessionInfoReady = (WE_UINT8)iInfoAvailable;
    pstCurSecCls->ucSecClass = (WE_UINT8)iSecClass;    
    /*fill the struct*/
    M_SEC_RESPCALLBACKEVENT = G_WTLSCURCLASS_RESP;
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)pstCurSecCls;

    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);

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
    St_SecSessionClearResp*   pstSessionClr = NULL;
    if(NULL == hSecHandle)
    {
        return ;
    }
    pstSessionClr = (St_SecSessionClearResp*)WE_MALLOC(sizeof(St_SecSessionClearResp));
    if((NULL == pstSessionClr) )
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    pstSessionClr->iTargetID = iTargetID;
    pstSessionClr->usResult = (WE_UINT16)iResult;   
    /*fill the struct*/
    M_SEC_RESPCALLBACKEVENT = G_SESSIONCLEAR_RESP;
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)pstSessionClr;

    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);
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
    St_SecGetContractsListResp*  pstGetContractList = NULL;
    if(NULL == hSecHandle)
    {
        return ;
    }
    pstGetContractList = (St_SecGetContractsListResp*)WE_MALLOC(sizeof(St_SecGetContractsListResp));
    if((NULL == pstGetContractList) )
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    pstGetContractList->iTargetID = iTargetID;
    pstGetContractList->usResult = (WE_UINT16)iResult;   
    
    /*fill the struct*/
    M_SEC_RESPCALLBACKEVENT = G_GETCONTRACT_RESP;
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)pstGetContractList;

    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
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
    St_SecDelContractResp*    pstDelContract = NULL;
    if(NULL == hSecHandle)
    {
        return ;
    }
    pstDelContract = (St_SecDelContractResp*)WE_MALLOC(sizeof(St_SecDelContractResp));
    if((NULL == pstDelContract) )
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    pstDelContract->iTargetID = iTargetID;
    pstDelContract->usResult = (WE_UINT16)iResult;    
    /*fill the struct*/
    M_SEC_RESPCALLBACKEVENT = G_DELCONTRACT_RESP;
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)pstDelContract;

    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);   
    
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
    St_SecUserCertRequestResp*   pstUCertReq = NULL;
    if(NULL == hSecHandle)
    {
        return ;
    }
    pstUCertReq = (St_SecUserCertRequestResp*)WE_MALLOC(sizeof(St_SecUserCertRequestResp));
    if((NULL == pstUCertReq) )
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    pstUCertReq->iTargetID = iTargetID;
    pstUCertReq->iResult = iResult;   
    pstUCertReq->pucCertReqMsg = (WE_UCHAR*)WE_MALLOC((WE_ULONG)uiCertReqMsgLen);
    if((NULL == pstUCertReq->pucCertReqMsg) && (0 != uiCertReqMsgLen))
    {
        M_SEC_SELF_FREE(pstUCertReq);
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    (WE_VOID)WE_MEMCPY(pstUCertReq->pucCertReqMsg,pucCertReqMsg,uiCertReqMsgLen);
    pstUCertReq->uiCertReqMsgLen = uiCertReqMsgLen;
    
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_UserCertReqResp\n"));
    /*fill the struct*/
    M_SEC_RESPCALLBACKEVENT = G_USERCERTREQ_RESP;
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)pstUCertReq;

    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);    
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
    St_ChangeCertStateResp*   pstChangeCertState = NULL;
    if(NULL == hSecHandle)
    {
        return ;
    }
    pstChangeCertState = (St_ChangeCertStateResp*)WE_MALLOC(sizeof(St_ChangeCertStateResp));
    if((NULL == pstChangeCertState) )
    {
        SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle); 
        return;
    }
    pstChangeCertState->iTargetID = iTargetID;
    pstChangeCertState->iResult = iResult;   
    pstChangeCertState->ucState = ucState;
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_ChangeWTLSCertAbleResp\n"));
    /*fill the struct*/
    M_SEC_RESPCALLBACKEVENT = G_CHANGECERTSTATE_RESP;
    M_SEC_RESPCALLBACKDATA = (WE_VOID*)pstChangeCertState;
    
    SEC_REGCALLBACK(M_SEC_ISHELL_RESP,M_SEC_RESPCALLBACK,Sec_RunRespCallBackFun,hSecHandle);
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
    WE_INT32 iEvent = 0;
    WE_VOID* pvCBData = NULL;
    if(NULL == hSecHandle)
    {
        return;
    }
    iEvent = M_SEC_RESPCALLBACKEVENT;
    pvCBData = M_SEC_RESPCALLBACKDATA;
    /*set to null before call back*/
    M_SEC_RESPCALLBACKDATA = NULL;
    if(NULL != M_SEC_BRSA_CALLBACK_FUN)
    {
        ((Fn_ISecEventHandle)(M_SEC_BRSA_CALLBACK_FUN))(iEvent,
            (void *)(pvCBData),M_SEC_BRSA_CB_PRIVATE_DATA);
    }
    Sec_FreeRespCBStruc(iEvent,pvCBData);
    
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
    WE_INT32 iEvent = 0;
    WE_VOID* pvCBData = NULL;
    if( NULL == hSecHandle)
    {
        return;
    }
    iEvent = M_SEC_RESPCALLBACKEVENT;
    pvCBData = M_SEC_RESPCALLBACKDATA;
    Sec_FreeRespCBStruc(iEvent,pvCBData);
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
        case G_GETCIPHERSUITE_RESP:
            {
                St_SecGetCipherSuiteResp* pstData = (St_SecGetCipherSuiteResp*)pvData;
                M_SEC_SELF_FREE(pstData->pucCipherMethods);
                M_SEC_SELF_FREE(pstData->pucKeyExchangeIds);
                M_SEC_SELF_FREE(pstData->pucTrustedKeyIds);
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
