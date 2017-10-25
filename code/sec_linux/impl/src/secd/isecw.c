/*==================================================================================================
    HEADER NAME : isecw.c
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
        
    GENERAL DESCRIPTION
        In this file,define the interface function prototype of sec module.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-07-07   Kevin Yang        None         Draft
==================================================================================================*/

/*=====================================================================================
*   Include File Section
*=====================================================================================*/
#include "sec_comm.h" 
#include "oem_secmgr.h"
#include "isecw.h"

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
#define SEC_IAEECALLBACK_VAR        (((St_PublicData *)(((ISec*)pMe)->hPrivateData))->m_cb)
#define SEC_IISHELL_POINTER         (((ISec*)pMe)->m_pIShell)
#define SEC_PRIVATE_DATA            (((ISec*)pMe)->hPrivateData)
#define SEC_SIINFO                  (((St_PublicData *)(((ISec*)pMe)->hPrivateData))->pstInfo)
#define SEC_PEER_CACHE              (((St_PublicData *)(((ISec*)pMe)->hPrivateData))->pstPeerCache)
#define SEC_SESSION_CACHE           (((St_PublicData *)(((ISec*)pMe)->hPrivateData))->pstSessionCache)
#define SEC_WIM_HAVELOADUSERPRIV    (((St_PublicData *)(((ISec*)pMe)->hPrivateData))->pcHaveLoad)

/*******************************************************************************
*   Global Variable Declare Section
*******************************************************************************/
/*global data move from sec_mod.h*/
St_WimUCertKeyPairInfo   astPubUserCertKeyPair[M_SEC_USER_CERT_MAX_SIZE] = {0}; 
St_SecInfo               stInfo = {0};
St_SecSessionRec         astSessionCache[M_SEC_SESSION_CACHE_SIZE] = {0};    
St_SecPeerRec            astPeerCache[M_SEC_PEER_CACHE_SIZE] = {0}; 
/* add for user_priv file load number */
WE_CHAR                  cHaveLoad = 0;

/*******************************************************************************
*   Function Define Section
*******************************************************************************/

/*==================================================================================================
FUNCTION: 
    SecW_Release
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    sec release
ARGUMENTS PASSED:
    WE_HANDLE pMe[IN]: pointer about an instance of ISec 
RETURN VALUE:
    referrence counter
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 Sec_Release (WE_HANDLE pMe)
{
   WE_UINT32 uiRes = 0;
   
   if (NULL == pMe)
   {
      return G_SEC_INVALID_PARAMETER;
   }

   Sec_DpResetVariable(pMe);   
   
   uiRes += Sec_StopEx(pMe);
   if (NULL != (St_PublicData*)(((ISec*)pMe)->hPrivateData))
   {
     uiRes += (WE_UINT32)WeMgr_Terminate((((St_PublicData*)(((ISec*)pMe)->hPrivateData))->hWeHandle));   
   }
   if (SEC_PRIVATE_DATA != NULL)
   {
       WE_FREE(SEC_PRIVATE_DATA);
       SEC_PRIVATE_DATA = NULL;
   }  
   /*Free the object itself*/
   WE_FREE(pMe);

   return uiRes;
}


/*==================================================================================================
FUNCTION: 
    SecW_GetCipherSuite
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    The function provides the information for wtls about the bulk cipher algorithm
    key exchange method and the trusted ca information.
ARGUMENTS PASSED:
    WE_HANDLE      pMe[IN]: pointer of ISec instance
    WE_INT32       iTargetID[IN]: the identity of the invoker ,input from the interface.
    WE_UINT8       ucConnType[IN]:the type of connection:wtls,tls or ssl
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
    G_SEC_OK.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 SecW_GetCipherSuite (WE_HANDLE pMe, WE_INT32 iTargetID,WE_UINT8   ucConnType)
{
   WE_INT32 iResult = 0;

   if (NULL == pMe)
   {
      return G_SEC_INVALID_PARAMETER;
   }
   if (M_SEC_WTLS_CONNECTION_TYPE==ucConnType)
   {
   iResult = Sec_MsgWtlsGetCipherSuite((WE_HANDLE)pMe,iTargetID);
   }
   else if (M_SEC_TLS_CONNECTION_TYPE==ucConnType)
   {      
      iResult = Sec_MsgTlsGetCipherSuite((WE_HANDLE)pMe,iTargetID);
   }
   else
   {
      return G_SEC_INVALID_PARAMETER;
   }
   if (M_SEC_ERR_OK != iResult)
   {
      return G_SEC_NOTENOUGH_MEMORY; 
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecW_KeyExchange
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    the interface will provide the function about the master secret key.
ARGUMENTS PASSED:
    ISecW *        pMe[IN]: pointer of ISecW instance.
    WE_INT32       iObjectId[IN]: the identity of the invoker ,input from the interface..
    WE_UINT8       ucAlg[IN]:Wtls:The value of the hash Algorithm.
                             Tls:The Algorithm of key Exchange.
    WE_VOID *      pvParam[IN]:if the type of connection is wtls,it is point to the structure St_SecWtlsKeyExchParams,which includs the public key and key exchange method.
                   if the type of connection is tls,which point to the structure of St_SecTlsKeyExchangeParams
    WE_UCHAR *     pucRandval[IN]:random data to create the master secret,if the type of connetion is wtls, 
                   it's length is 32;else if tls ,it is 64(rfc2246).
    WE_UINT8       ucConnType[IN]:the type of connection:wtls,ssl or tls   
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
    G_SEC_OK.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
 WE_UINT32 SecW_KeyExchange(WE_HANDLE pMe, WE_INT32 iTargetID, WE_VOID *pvParam,
                                 WE_UINT8 ucAlg,WE_UCHAR * pucRandval,WE_UINT8 ucConnType)
{
   WE_INT32 iResult = 0;
    
   if (NULL == pMe)
   {
      return G_SEC_INVALID_PARAMETER;
   }  
   
   if (M_SEC_WTLS_CONNECTION_TYPE==ucConnType)
   {
      iResult = Sec_MsgWtlsKeyExchange ((WE_HANDLE)pMe,iTargetID,pvParam,ucAlg,pucRandval);
   }
   else if (M_SEC_TLS_CONNECTION_TYPE==ucConnType)
   {      
      iResult = Sec_MsgTlsKeyExchange ((WE_HANDLE)pMe,iTargetID,pvParam,ucAlg,pucRandval);
   }
   else
   {
      return G_SEC_INVALID_PARAMETER;
   }
   if (M_SEC_ERR_OK != iResult)
   {
      return G_SEC_NOTENOUGH_MEMORY; 
   }  

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecW_GetPrfResult
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    Get the Pseudo Random Function.If the master secret id is 
    to be used, "pucSecret" is NULL, and "iMasterSecretId" 
    denotes which master secret is to use. Or,"pucSecret" must be provided.
ARGUMENTS PASSED:
    WE_HANDLE     pMe[IN]: pointer of ISec instance.
    WE_INT32    iTargetID[IN]: the identity of the invoker ,input from the interface.
    WE_UINT8    ucAlg[IN]:The value of the Algorithm.
    WE_INT32    iMasterSecretId[IN]:ID of master secret.
    WE_INT32    iSecretLen[IN]:Length of the secret.
    WE_INT32    iSeedLen[IN]:Length of the seed.
    WE_INT32    iOutputLen[IN]:Length of the output.
    WE_UCHAR *  pucSecret[IN]:Pointer to the secret.
    WE_CHAR  *  pcLabel[IN]:Pointer to lable.
    WE_UCHAR *  pucSeed[IN]:Pointer to seed.
    WE_UINT8    ucConnType[IN]:the type of connection:wtls,ssl or tls   
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
    G_SEC_OK.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 SecW_GetPrfResult(WE_HANDLE pMe, WE_INT32 iTargetID, WE_UINT8 ucAlg, 
                                        WE_INT32 iMasterSecretId,  WE_UCHAR * pucSecret, 
                                        WE_INT32 iSecretLen,  WE_CHAR * pcLabel, 
                                         WE_UCHAR * pucSeed, WE_INT32 iSeedLen,
                                       WE_INT32 iOutputLen,WE_UINT8 ucConnType)
{
   WE_UCHAR * pucSecretMem = NULL;
   WE_CHAR  * pcLableMem = NULL;
   WE_UCHAR * pucSeedMem = NULL;
   WE_INT32   iResult = 0;

   if (NULL == pMe)
   {
      return G_SEC_INVALID_PARAMETER;
   }


   if (NULL != pucSecret && iSecretLen > 0)
   {
      pucSecretMem = WE_MALLOC((WE_ULONG)iSecretLen);
      if (NULL == pucSecretMem)
      {
         return G_SEC_NOTENOUGH_MEMORY;
      }

      (void)WE_MEMCPY(pucSecretMem,pucSecret,(WE_UINT32)iSecretLen);
   }
   if (NULL != pucSeed && iSeedLen > 0)
   {
      pucSeedMem = WE_MALLOC((WE_ULONG)iSeedLen);
      if (!pucSeedMem)
      {
         if (NULL != pucSecretMem)
         {
            WE_FREE(pucSecretMem);
         }
         return G_SEC_NOTENOUGH_MEMORY;
      }
      (void)WE_MEMCPY(pucSeedMem, pucSeed, (WE_UINT32)iSeedLen);
   }
   if (NULL != pcLabel)
   {
      pcLableMem = SEC_STRDUP(pcLabel);
      if (NULL == pcLableMem)
      {
         if (NULL != pucSecretMem)
         {
            WE_FREE(pucSecretMem);
         }
         if (NULL != pucSeedMem)
         {
            WE_FREE(pucSeedMem);
         }
         return G_SEC_NOTENOUGH_MEMORY;
      }
   }
   if (M_SEC_WTLS_CONNECTION_TYPE==ucConnType)
   {
      iResult = Sec_MsgWtlsGetPrfResult ((WE_HANDLE)pMe,iTargetID,ucAlg,iMasterSecretId,pucSecretMem,
                                     iSecretLen,pcLableMem,pucSeedMem,iSeedLen,iOutputLen);
   }
   else if (M_SEC_TLS_CONNECTION_TYPE==ucConnType)
   {      
      iResult = Sec_MsgTlsGetPrfResult ((WE_HANDLE)pMe,iTargetID,ucAlg,iMasterSecretId,pucSecretMem,
                                     iSecretLen,pcLableMem,pucSeedMem,iSeedLen,iOutputLen);
   }
   else
   {
      if (NULL != pcLableMem)
      {
         WE_FREE(pcLableMem);
      }
      return G_SEC_INVALID_PARAMETER;
   }


   if (M_SEC_ERR_OK != iResult)
   {
      if (NULL != pucSecretMem)
      {
         WE_FREE(pucSecretMem);
      }
      if (NULL != pucSeedMem)
      {
         WE_FREE(pucSeedMem);
      }
      if (NULL != pcLableMem)
      {
         WE_FREE(pcLableMem);
      }
      return G_SEC_NOTENOUGH_MEMORY;  
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecW_VerifySvrCertChain
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    the interface will provide the function to verify the certificate chain from the gateway.
ARGUMENTS PASSED:
    WE_HANDLE      pMe[IN]: pointer of ISec instance.
    WE_INT32    iTargetID[IN]: the identity of the invoker ,input from the interface.
    WE_UCHAR *  pvBuf[IN]:Pointer to the parameter.
    WE_UCHAR *  iBufLen[IN]:length of the buffer.
    WE_UINT8    ucConnType[IN]:the type of connection:wtls,ssl or tls   
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
    G_SEC_OK.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 SecW_VerifySvrCertChain(WE_HANDLE pMe, WE_INT32 iTargetID,
                                          WE_VOID *pvBuf,  WE_INT32 iBufLen,
                                          WE_UINT8 ucConnType)
{
   WE_UCHAR * pucBufMem = NULL;
   WE_INT32   iResult = 0;

   if (NULL == pMe||NULL==pvBuf)
   {
      return G_SEC_INVALID_PARAMETER;
   } 
   if (M_SEC_WTLS_CONNECTION_TYPE==ucConnType)
   {
      iResult = Sec_MsgWtlsVerifySvrCert ((WE_HANDLE)pMe,iTargetID,pvBuf,iBufLen);
   }
   else if (M_SEC_TLS_CONNECTION_TYPE==ucConnType)
   {      
      iResult = Sec_MsgTlsVerifyCertChain ((WE_HANDLE)pMe,iTargetID,pvBuf,iBufLen);
   }
   else
   {
      return G_SEC_INVALID_PARAMETER;
   }

   if (M_SEC_ERR_OK != iResult)
   {
      return G_SEC_NOTENOUGH_MEMORY;
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecW_GetUserCert
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    the interface will provide the function to return the client certificate to the wtls.
ARGUMENTS PASSED:
    WE_HANDLE     pMe[IN]: pointer of ISec instance.
    WE_INT32    iTargetID[IN]: the identity of the invoker ,input from the interface.
    WE_VOID *   pvBuf[IN]:Pointer to the parameter.
    WE_INT32   iBufLen[IN]:length of the buffer.
    WE_UINT8   ucConnType[IN]:The type of connection:wtls,tls or ssl.
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
    G_SEC_OK.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 SecW_GetUserCert(WE_HANDLE pMe, WE_INT32 iTargetID, 
                                 WE_VOID *  pvBuf, WE_INT32 iBufLen,
                                 WE_UINT8   ucConnType)
{
   WE_VOID   *pvBufMem = NULL;
   WE_INT32   iResult = 0;

   if(NULL == pMe)
   {
      return G_SEC_INVALID_PARAMETER;
   }

   if(M_SEC_WTLS_CONNECTION_TYPE==ucConnType)
   {
       if(NULL != pvBuf && iBufLen > 0)
       {
          pvBufMem = WE_MALLOC((WE_ULONG)iBufLen);
          if(NULL == pvBufMem)
          {
             return G_SEC_NOTENOUGH_MEMORY;
          }
          (void)WE_MEMCPY(pvBufMem,pvBuf,(WE_UINT32)iBufLen);
       }
      iResult = Sec_MsgWtlsGetUsrCert((WE_HANDLE)pMe,iTargetID,pvBufMem,iBufLen);
   }
   else if(M_SEC_TLS_CONNECTION_TYPE==ucConnType)
   {      
      if(NULL==pvBuf)
      {
          return G_SEC_INVALID_PARAMETER;
      }
      iResult = Sec_MsgTlsGetUsrCert((WE_HANDLE)pMe,iTargetID,pvBuf,iBufLen);
   }
   else
   {
      return G_SEC_INVALID_PARAMETER;
   }
   if(M_SEC_ERR_OK != iResult)
   {
      if(NULL != pvBufMem)
      {
         WE_FREE(pvBufMem);
      }
      return G_SEC_NOTENOUGH_MEMORY; 
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecW_CompSign
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    compute the signature for wtls.
ARGUMENTS PASSED:
    WE_HANDLE       pMe[IN]: pointer of ISec instance.
    WE_INT32      iTargetID[IN]: the identity of the invoker ,input from the interface.
    WE_UCHAR *    pucKeyId[IN]:Pointer to the key id .
    WE_INT32      iKeyIdLen[IN]:Length of the key id.
    WE_UCHAR *    pucBuf[IN]:Pointer to the buffer.
    WE_INT32      iBufLen[IN]:Length of the buffer.
    WE_UINT8   ucConnType[IN]:The type of connection:wtls,tls or ssl.   
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
    G_SEC_OK.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 SecW_CompSign(WE_HANDLE pMe, WE_INT32 iTargetID,
                               WE_UCHAR * pucKeyId, WE_INT32 iKeyIdLen,
                               WE_UCHAR * pucBuf, WE_INT32 iBufLen,
                               WE_UINT8 ucConnType)
{
   WE_UCHAR * pucKeyIdMem = NULL;
   WE_UCHAR * pucBufMem = NULL;
   WE_INT32   iResult = 0;

   if (NULL == pMe)
   {
      return G_SEC_INVALID_PARAMETER;
   }    

   if (NULL != pucKeyId && iKeyIdLen > 0)
   {
      pucKeyIdMem = WE_MALLOC((WE_ULONG)iKeyIdLen);
      if (NULL == pucKeyIdMem)
      {
         return G_SEC_NOTENOUGH_MEMORY;
      }
      (void)WE_MEMCPY(pucKeyIdMem,pucKeyId,(WE_UINT32)iKeyIdLen);
   }
   if (NULL != pucBuf && iBufLen > 0)
   {
      pucBufMem = WE_MALLOC((WE_ULONG)iBufLen);
      if (NULL == pucBufMem)
      {
         if (NULL != pucKeyIdMem)
         {
            WE_FREE(pucKeyIdMem);
         }
         return G_SEC_NOTENOUGH_MEMORY;
      }
      (void)WE_MEMCPY(pucBufMem,pucBuf,(WE_UINT32)iBufLen);
   }
   if (M_SEC_WTLS_CONNECTION_TYPE==ucConnType)
   {
      iResult = Sec_MsgWtlsCompSign((WE_HANDLE)pMe,iTargetID,pucKeyIdMem,iKeyIdLen,pucBufMem,iBufLen);
   }
   else if (M_SEC_TLS_CONNECTION_TYPE==ucConnType)
   {      
      iResult = Sec_MsgTlsCompSign((WE_HANDLE)pMe,iTargetID,pucKeyIdMem,iKeyIdLen,pucBufMem,iBufLen,M_SEC_SP_RSA);
   }
   else
   {
      return G_SEC_INVALID_PARAMETER;
   }

   if (M_SEC_ERR_OK != iResult)
   {
      if (NULL != pucKeyIdMem)
      {
         WE_FREE(pucKeyIdMem);
      }
      if (NULL != pucBufMem)
      {
         WE_FREE(pucBufMem);
      }
      return G_SEC_NOTENOUGH_MEMORY;  
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecW_SetupConnection
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    set up session connection .
ARGUMENTS PASSED:
    WE_HANDLE             pMe[IN]: pointer of ISec instance.
    WE_INT32            iTargetID[IN]: the identity of the invoker ,input from the interface.
    WE_INT32            iMasterSecretId[IN]:ID of the master secret.
    WE_INT32            iSecurityId[IN]:ID of the security.
    WE_INT32            iFullHandshake[IN]:whether is full hand shake.
    St_SecSessionInformation    stSessionInfo[IN]:Information of the session.
    WE_UINT8   ucConnType[IN]:The type of connection:wtls,tls or ssl.   
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
    G_SEC_OK.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 SecW_SetupConnection(WE_HANDLE pMe, WE_INT32 iTargetID, WE_INT32 iMasterSecretId, 
                                          WE_INT32 iSecurityId, WE_INT32 iFullHandshake,
                                          St_SecSessionInformation stSessionInfo,WE_UINT8 ucConnType)
{
   St_SecSessionInfo stInfo = {0};
   St_SecAsn1Certificate * pstCert = NULL;
   WE_INT32   iResult = 0;

   if(NULL == pMe)
   {
      return G_SEC_INVALID_PARAMETER;
   }    

   /*add by birdzhang 061017*/
   if(stSessionInfo.ucNumCerts > 0)
   {
      pstCert = WE_MALLOC((WE_ULONG)sizeof(St_SecAsn1Certificate));
      if(NULL == pstCert)
      {
         return G_SEC_NOTENOUGH_MEMORY;
      }

      pstCert->ucFormat = stSessionInfo.ucCertFormat;
      pstCert->uiCertLen = stSessionInfo.uiCertLen;
      if(stSessionInfo.uiCertLen > (WE_UINT32)0 && NULL != stSessionInfo.pucCert)
      {    
         pstCert->pucCert = WE_MALLOC((WE_ULONG)stSessionInfo.uiCertLen);
         if(NULL == pstCert->pucCert)
         {          
            WE_FREE(pstCert);
            return G_SEC_NOTENOUGH_MEMORY;
         }
         (void)WE_MEMCPY(pstCert->pucCert,stSessionInfo.pucCert,stSessionInfo.uiCertLen);
      }
      else
      {
         pstCert->pucCert = NULL;
      }
   }
   stInfo.ucConnectionType = ucConnType;
   stInfo.ucSecurityClass = stSessionInfo.ucSecurityClass ;
   stInfo.ucKeyExchangeAlg = stSessionInfo.ucKeyExchangeAlg;
   stInfo.usKeyExchangeKeyLen = stSessionInfo.usKeyExchangeKeyLen;
   stInfo.ucHmacAlg = stSessionInfo.ucHmacAlg;
   stInfo.usHmacLen = stSessionInfo.usHmacLen;
   stInfo.ucEncryptionAlg = stSessionInfo.ucEncryptionAlg;
   stInfo.usEncryptionKeyLen = stSessionInfo.usEncryptionKeyLen;
   stInfo.ucNumCerts = stSessionInfo.ucNumCerts;
   stInfo.pstCerts = pstCert;

   iResult = Sec_MsgSetupCon((WE_HANDLE)pMe,iTargetID,iMasterSecretId,iSecurityId,
                             iFullHandshake,stInfo);

   if(M_SEC_ERR_OK != iResult)
   {
      if (NULL != pstCert)
      {
         if(NULL != pstCert->pucCert)
         {
            WE_FREE(pstCert->pucCert);
         }       
         WE_FREE(pstCert);
      }
      return G_SEC_NOTENOUGH_MEMORY; 
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecW_StopConnection
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    stop session connection.
ARGUMENTS PASSED:
    WE_HANDLE         pMe[IN]: pointer of ISec instance.
    WE_INT32        iTargetID[IN]: the identity of the invoker ,input from the interface.
    WE_INT32        iSecurityId[IN]:ID of the security.
    WE_UINT8   ucConnType[IN]:The type of connection:wtls,tls or ssl.   
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
    G_SEC_OK.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
 WE_UINT32 SecW_StopConnection(WE_HANDLE pMe, WE_INT32 iTargetID, WE_INT32 iSecurityId,WE_UINT8   ucConnType)
{
   WE_INT32   iResult = 0;

   if(NULL == pMe)
   {
      return G_SEC_INVALID_PARAMETER;
   }    

   iResult = Sec_MsgStopCon((WE_HANDLE)pMe,iTargetID,iSecurityId,(WE_UCHAR)M_SEC_WTLS_CONNECTION_TYPE);

   if(M_SEC_ERR_OK != iResult)
   {
      return G_SEC_NOTENOUGH_MEMORY; 
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecW_ClearPeer
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    remove peer from a session slot.
ARGUMENTS PASSED:
    WE_HANDLE         pMe[IN]: pointer of ISec instance.
    WE_INT32        iMasterSecretId[IN]:ID of the master secret.
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
    G_SEC_OK.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 SecW_RemovePeer(WE_HANDLE pMe, WE_INT32 iMasterSecretId)
{
   WE_INT32   iResult = 0;
   WE_INT32 iTargetId = 0; /*added by Bird 061123 for warning dialog to compile*/

   if(NULL == pMe)
   {
      return G_SEC_INVALID_PARAMETER;
   }    

   iResult = Sec_MsgRemovePeer((WE_HANDLE)pMe,iTargetId,iMasterSecretId);

   if(M_SEC_ERR_OK != iResult)
   {
      return G_SEC_NOTENOUGH_MEMORY;
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecW_AttachPeerToSession
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    add one peer to an existing session.
ARGUMENTS PASSED:
    WE_HANDLE       pMe[IN]: pointer of ISec instance.
    WE_UCHAR *    pucAddress[IN]:Pointer to the address.
    WE_INT32      iAddressLen[IN]:Length of the address.
    WE_UINT16     usPortnum[IN]:Number of the port.
    WE_INT32      iMasterSecretId[IN]:ID of the master secret.
    WE_UINT8   ucConnType[IN]:The type of connection:wtls,tls or ssl.   
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
    G_SEC_OK.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 SecW_AttachPeerToSession(WE_HANDLE pMe,  WE_UCHAR * pucAddress, WE_INT32 iAddressLen,
                                        WE_UINT16 usPortnum, WE_INT32 iMasterSecretId,WE_UINT8   ucConnType)
{
   WE_UCHAR * pucAddressMem = NULL;
   WE_INT32   iResult = 0;

   if(NULL == pMe)
   {
      return G_SEC_INVALID_PARAMETER;
   }    

   if(NULL != pucAddress && iAddressLen > 0)
   {
      pucAddressMem = WE_MALLOC((WE_ULONG)iAddressLen);
      if(NULL == pucAddressMem)
      {
         return G_SEC_NOTENOUGH_MEMORY;
      }
      (void)WE_MEMCPY(pucAddressMem,pucAddress,(WE_UINT32)iAddressLen);
   }

   iResult = Sec_MsgAttachPeerToSession((WE_HANDLE)pMe,ucConnType,pucAddressMem,
                                        iAddressLen, NULL,usPortnum,iMasterSecretId);

   if(M_SEC_ERR_OK != iResult)
   {
      if(NULL != pucAddressMem)
      {
         WE_FREE(pucAddressMem);
      }
      return G_SEC_NOTENOUGH_MEMORY;  
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecW_SearchPeer
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    search peer in a session slot.
ARGUMENTS PASSED:
    WE_HANDLE       pMe[IN]: pointer of ISec instance.
    WE_UCHAR *    pucAddress[IN]:Pointer to the address.
    WE_INT32      iAddressLen[IN]:Length of the address.
    WE_UINT16     usPortnum[IN]:Number of the port.
    WE_INT32      iTargetID[IN]:ID of the object.
    WE_UINT8   ucConnType[IN]:The type of connection:wtls,tls or ssl.   
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
    G_SEC_OK.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 SecW_SearchPeer(WE_HANDLE pMe, WE_INT32 iTargetID,  WE_UCHAR * pucAddress, 
                                  WE_INT32 iAddressLen, WE_UINT16 usPortnum,WE_UINT8   ucConnType)
{
   WE_UCHAR * pucAddressMem = NULL;
   WE_INT32   iResult = 0;

   if(NULL == pMe)
   {
      return G_SEC_INVALID_PARAMETER;
   }    

   if(NULL != pucAddress && iAddressLen > 0)
   {
      pucAddressMem = (WE_UCHAR*)WE_MALLOC((WE_ULONG)iAddressLen);
      if(NULL == pucAddressMem)
      {
         return G_SEC_NOTENOUGH_MEMORY;
      }
      (void)WE_MEMCPY(pucAddressMem,pucAddress,(WE_UINT32)iAddressLen);
   }


   iResult = Sec_MsgSearchPeer((WE_HANDLE)pMe,iTargetID,ucConnType,pucAddressMem,iAddressLen,NULL,usPortnum);

   if(M_SEC_ERR_OK != iResult)
   {
      WE_FREE(pucAddressMem); 
      return G_SEC_NOTENOUGH_MEMORY; 
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecW_EnableSession
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    enable a session connection for wtls.
ARGUMENTS PASSED:
    WE_HANDLE     pMe[IN]: pointer of ISec instance.
    WE_INT32    iMasterSecretId[IN]:ID of the master secret.
    WE_UINT8    ucIsActive[IN]:flg of the active.
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
    G_SEC_OK.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 SecW_EnableSession(WE_HANDLE pMe, WE_INT32 iMasterSecretId, WE_UINT8 ucIsActive)
{
   WE_INT32   iResult = 0;

   if(NULL == pMe)
   {
      return G_SEC_INVALID_PARAMETER;
   }    

   iResult = Sec_MsgEnableSession((WE_HANDLE)pMe,iMasterSecretId,ucIsActive);

   if(M_SEC_ERR_OK != iResult)
   {
      return G_SEC_NOTENOUGH_MEMORY; 
   } 

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecW_DisableSession
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    disable one session for wtls.
ARGUMENTS PASSED:
    WE_HANDLE     pMe[IN]: pointer of ISec instance.
    WE_INT32    iMasterSecretId[IN]:ID of the master secret.
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
    G_SEC_OK.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 SecW_DisableSession(WE_HANDLE pMe, WE_INT32 iMasterSecretId)
{
   WE_INT32   iResult = 0;

   if(NULL == pMe)
   {
      return G_SEC_INVALID_PARAMETER;
   }    

   iResult = Sec_MsgDisableSession((WE_HANDLE)pMe,iMasterSecretId);

   if(M_SEC_ERR_OK != iResult)
   {
      return G_SEC_NOTENOUGH_MEMORY; 
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecW_SessionGet
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    fetch information of one session based on master secret id.
ARGUMENTS PASSED:
    WE_HANDLE     pMe[IN]: pointer of ISec instance.
    WE_INT32    iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_INT32    iMasterSecretId[IN]:ID of the master secret.
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
    G_SEC_OK.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 SecW_SessionGet(WE_HANDLE pMe, WE_INT32 iTargetID, WE_INT32 iMasterSecretId)
{
   WE_INT32   iResult = 0;

   if(NULL == pMe)
   {
      return G_SEC_INVALID_PARAMETER;
   }    

   iResult = Sec_MsgGetSessionInfo((WE_HANDLE)pMe,iTargetID,iMasterSecretId);

   if(M_SEC_ERR_OK != iResult)
   {
      return G_SEC_NOTENOUGH_MEMORY; 
   } 

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecW_SessionRenew
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    update one session information.
ARGUMENTS PASSED:
    WE_HANDLE        pMe[IN]: pointer of ISec instance.
    WE_INT32       iMasterSecretId[IN]:ID of the master secret.
    WE_UINT8       ucSessionOptions[IN]:The option of the session.
    WE_UCHAR *     pucSessionId[IN]:Pointer to the session ID.
    WE_UINT8       ucSessionIdLen[IN]:Length of the session id.
    WE_UINT8       aucCipherSuite[IN]:Array for cipher suite.
    WE_UINT8       ucCompressionAlg[IN]:The value of the compression Algorithm.
    WE_UCHAR *     pucPrivateKeyId[IN]:ID of the private key.
    WE_UINT32      uiCreationTime[IN]:the time of creation.
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
    G_SEC_OK.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 SecW_SessionRenew(WE_HANDLE pMe, WE_INT32 iMasterSecretId, 
                                    WE_UINT8 ucSessionOptions,  WE_UCHAR * pucSessionId, 
                                    WE_UINT8 ucSessionIdLen,  WE_UINT8 *aucCipherSuite,
                                    WE_UINT8 ucCompressionAlg,  WE_UCHAR * pucPrivateKeyId,
                                    WE_UINT32 uiCreationTime)
{
   WE_UCHAR * pucSessionIdMem = NULL;
   WE_UCHAR * pucPrivateKeyIdMem = NULL;
   WE_INT32   iResult = 0;

   if(NULL == pMe)
   {
      return G_SEC_INVALID_PARAMETER;
   }    

   if(NULL != pucSessionId && ucSessionIdLen > 0)
   {
      pucSessionIdMem = WE_MALLOC((WE_ULONG)ucSessionIdLen);
      if(NULL == pucSessionIdMem)
      {
         return G_SEC_NOTENOUGH_MEMORY;
      }
      (void)WE_MEMCPY(pucSessionIdMem,pucSessionId,(WE_UINT32)ucSessionIdLen);
   }
   if(NULL != pucPrivateKeyId)
   {
      pucPrivateKeyIdMem = WE_MALLOC((WE_ULONG)M_SEC_PRIVATE_KEY_ID_LEN);
      if(NULL == pucPrivateKeyIdMem)
      {
         if(NULL != pucSessionIdMem)
         {
            WE_FREE(pucSessionIdMem);
         }
         return G_SEC_NOTENOUGH_MEMORY;
      }
      (void)WE_MEMCPY(pucPrivateKeyIdMem,pucPrivateKeyId,(WE_UINT32)M_SEC_PRIVATE_KEY_ID_LEN);
   }

   iResult = Sec_MsgRenewSession((WE_HANDLE)pMe,iMasterSecretId,ucSessionOptions,pucSessionIdMem,ucSessionIdLen,
                                 aucCipherSuite,ucCompressionAlg,pucPrivateKeyIdMem,uiCreationTime);

   if(M_SEC_ERR_OK != iResult)
   {
      if(NULL != pucSessionIdMem)
      {
         WE_FREE(pucSessionIdMem);
      }
      if(NULL != pucPrivateKeyIdMem)
      {
         WE_FREE(pucPrivateKeyIdMem);
      }
      return G_SEC_NOTENOUGH_MEMORY;  
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
}

/*==================================================================================================
FUNCTION: 
    SecW_EvtSelectCertAction
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    after get cert name list, the invoker should run this function to give a feedback
    to sec module.
ARGUMENTS PASSED:
    WE_HANDLE        pMe[IN]:pointer of ISec instance.
    St_SelectCertAction  stChooseCerByName[IN]:The content of how to choose cert by name.
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_GENERAL_ERROR,
    G_SEC_OK.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 SecW_EvtSelectCertAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_SelectCertAction stChooseCerByName)
{
   WE_INT32   iResult = 0;

   if(NULL == pMe)
   {
      return G_SEC_INVALID_PARAMETER;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"Sec_EvtSelectCertAction\n"));

   iResult = Sec_UeSelectAction((WE_HANDLE)pMe,iTargetID, stChooseCerByName);

   if(M_SEC_ERR_OK != iResult)
   {
      return G_SEC_GENERAL_ERROR;
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
}

#ifndef M_SEC_WTLS_NO_CONFIRM_UE 
/*==================================================================================================
FUNCTION: 
    SecW_EvtConfirmAction
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    after the invoker receive the confirm event ,this function should be run to give a 
    feedback to sec module.
ARGUMENTS PASSED:
    ISecB *             pMe[IN]:pointer of ISec instance.
    WE_INT32 iTargetID[IN]:the identity of the invoker ,input from the interface.
    St_ConfirmAction    stConfirm[IN]:The value of the confirm.
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_OK,
    G_SEC_GENERAL_ERROR  
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 SecW_EvtConfirmAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_ConfirmAction stConfirm)
{
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return G_SEC_INVALID_PARAMETER;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"Sec_EvtConfirmAction\n"));
    
   iResult = Sec_UeConfirmAction((WE_HANDLE)pMe,iTargetID, stConfirm);
        
   if(M_SEC_ERR_OK != iResult)
   {
       return G_SEC_GENERAL_ERROR;  
   }    

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
}
#endif
/*==================================================================================================
FUNCTION: 
    SecW_EvtShowDlgAction
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    after the invoker receive the confirm event ,this function should be run to give a 
    feedback to sec module.
ARGUMENTS PASSED:
    WE_HANDLE pMe[IN]:pointer of ISec instance.
    WE_INT32 iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_VOID* pvSrcData[IN]:Pointer to the dada.
    WE_UINT32 uiLength[IN]:Length of data.
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_OK,
    G_SEC_GENERAL_ERROR  
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 SecW_EvtShowDlgAction(WE_HANDLE pMe,WE_INT32 iTargetId,
                                WE_VOID* pvSrcData,WE_UINT32 uiLength)
{
    WE_INT32 iRes = 0;
    WE_INT32 iEvent = 0;
    WE_VOID* pvData = NULL;
    if((NULL == pMe) || (NULL == pvSrcData))
    {
        return G_SEC_GENERAL_ERROR;
    }
    /*decode*/
    if(!Sec_DecodeEvtAction(pvSrcData,uiLength,&iEvent,&pvData))
    {
        if(NULL != pvData)
        {
            WE_FREE(pvData);
            pvData = NULL;
        }
        return G_SEC_GENERAL_ERROR;
    }
    switch(iEvent)
    {
        case M_UE_SELECT_CET_EVENT:
        {
            St_SelectCertAction* pstSelCert = (St_SelectCertAction*)pvData;
            if(NULL == pstSelCert)
            {
                return G_SEC_NOTENOUGH_MEMORY;
            }            
            /*call function*/
            iRes = (WE_INT32)SecW_EvtSelectCertAction(pMe,iTargetId,*pstSelCert);
        }
        break;
        
#ifndef M_SEC_WTLS_NO_CONFIRM_UE  /*have confirmation dialog*/
        case M_UE_CONFIRM_EVENT:
        {
            St_ConfirmAction* pstConfirm = (St_ConfirmAction*)pvData;
            if(NULL == pstConfirm)
            {
                return G_SEC_NOTENOUGH_MEMORY;
            }
            
            iRes = (WE_INT32)SecW_EvtConfirmAction(pMe,iTargetId,*pstConfirm);

        }
        break;
#endif        
        default:
            iRes = G_SEC_GENERAL_ERROR;
            break;
    }
    /*Free memory*/
    if(NULL != pvData)
    {
        WE_FREE(pvData);
        pvData = NULL;
    }
    return (WE_UINT32)iRes;
}
/*==================================================================================================
FUNCTION: 
    SecW_EvtShowDlgAction
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    after the invoker receive the confirm event ,this function should be run to give a 
    feedback to sec module.
ARGUMENTS PASSED:
    WE_HANDLE pMe[IN]:pointer of ISec instance.
    WE_INT32 iMasterSecretID[IN]:Master Secret ID
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_OK,
    G_SEC_GENERAL_ERROR  
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecW_SslGetWMasterSec(WE_HANDLE pMe, WE_INT32 iMasterSecretID)
{
     Sec_iWimGetMasterSecret ((WE_HANDLE)pMe,iMasterSecretID);
     return G_SEC_OK ;
} 
/*==================================================================================================
FUNCTION: 
    ISec_New
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    create one instance of ISec interface.
ARGUMENTS PASSED:
    WE_UINT16         sSize[IN]:size of ISec
    IShell *        pIShell[IN]:Pointer to the IShell interface.
    IModule*        pIModule[IN]:Pointer to the module.
    WE_HANDLE*        ppMod[IN]:Pointer to mod.
RETURN VALUE:
    G_SEC_NOTENOUGH_MEMORY,
    G_SEC_GENERAL_ERROR,
    G_SEC_OK
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_New( WE_INT32 iType, WE_INT32 iFd,WE_HANDLE* phHandle)
{
   ISec* pMe = NULL;

   if( !phHandle )
   {
      return G_SEC_INVALID_PARAMETER;
   }

   *phHandle = NULL;
   if((pMe = (ISec*)WE_MALLOC((WE_ULONG)(sizeof(ISec)))) == NULL )
   {
      return G_SEC_NOTENOUGH_MEMORY;
   }
   /*malloc memory for global variable*/
   if((pMe->hPrivateData = WE_MALLOC((WE_ULONG)(sizeof(St_PublicData)))) == NULL)
   {
      WE_FREE(pMe);
      return G_SEC_NOTENOUGH_MEMORY;
   }   
   (void)WE_MEMSET(pMe->hPrivateData,0,sizeof(St_PublicData));
   
   /* initialize the data members*/
   pMe->iIFType = (WE_INT32)iType;
   /*socket id*/
   ((St_PublicData *)(((ISec*)pMe)->hPrivateData))->iFd =  iFd;
   (pMe->pstUserCertKeyPair) = astPubUserCertKeyPair;
   /*added by bird 070126*/
   SEC_SIINFO = &stInfo;
   /*added by bird 070202*/
   SEC_PEER_CACHE = astPeerCache;
   SEC_SESSION_CACHE = astSessionCache;
   SEC_WIM_HAVELOADUSERPRIV = &cHaveLoad;
   /*Note: this must be run after the former sentence*/
   if(SUCCESS != (WE_INT32)WeMgr_Initialize(&(((St_PublicData*)(pMe->hPrivateData))->hWeHandle)))
   { 
       WE_FREE(pMe->hPrivateData);
       WE_FREE(pMe);
       return G_SEC_GENERAL_ERROR;
   }
   if(SUCCESS != Sec_StartEx((WE_HANDLE)pMe))
   {
       (void)WeMgr_Terminate(((St_PublicData*)(pMe->hPrivateData))->hWeHandle);
       WE_FREE(pMe->hPrivateData);
       WE_FREE(pMe);
       return G_SEC_GENERAL_ERROR;
   }
   
   /* Set the pointer in the parameter*/
   *phHandle = (ISec*)pMe;

   return G_SEC_OK;
}

