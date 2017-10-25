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
#include "sec_mod.h"
#include "oem_secmgr.h"
#include "isecw.h"

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
#define SEC_IAEECALLBACK_VAR    (((St_PublicData *)(((ISecW*)pMe)->hPrivateData))->m_cb)
#define SEC_IISHELL_POINTER     (((ISecW*)pMe)->m_pIShell)
#define SEC_PRIVATE_DATA        (((ISecW*)pMe)->hPrivateData)
#define SEC_SIINFO              (((St_PublicData *)(((ISecW*)pMe)->hPrivateData))->pstInfo)
#define SEC_PEER_CACHE          (((St_PublicData *)(((ISecW*)pMe)->hPrivateData))->pstPeerCache)
#define SEC_SESSION_CACHE       (((St_PublicData *)(((ISecW*)pMe)->hPrivateData))->pstSessionCache)
#define SEC_WIM_HAVELOADUSERPRIV        (((St_PublicData *)(((ISecW *)pMe)->hPrivateData))->pcHaveLoad)
/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*==================================================================================================
FUNCTION: 
    SecW_AddRef
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    sec add reference
ARGUMENTS PASSED:
    ISecW * pMe[IN]: pointer about an instance of ISecW    
RETURN VALUE:
    Counter number
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_UINT32 SecW_AddRef(ISecW *pMe)
{
   if(NULL == pMe)
   {
      return 0;
   }
   return (WE_UINT32)++(pMe->m_nRefs);
}
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
    ISecW * pMe[IN]: pointer about an instance of ISecW 
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
static WE_UINT32 SecW_Release (ISecW *pMe)
{
   WE_UINT32 uiRes = 0;
   
   if(NULL == pMe)
   {
      return 0;
   }
   /* Decrement reference count*/
   if( --pMe->m_nRefs != (WE_UINT32)0 )
   {
      return (WE_UINT32)pMe->m_nRefs;
   }

   /*Brs vtbl subtract*/
   ((SecMod*)(pMe->m_pIModule))->uiSecWapRefs--;

   /*added by bird 061113*/
   /*1.cancel callback*/
   SEC_CANCELCALLBACK(SEC_IAEECALLBACK_VAR);
   /*5.free the correlative global memory and initial global variable that control state*/
   Sec_DpResetVariable(pMe);   
   
   uiRes += Sec_StopEx(pMe);
   if(NULL != (St_PublicData*)(pMe->hPrivateData))
   {
     uiRes += (WE_UINT32)WeMgr_Terminate((((St_PublicData*)(pMe->hPrivateData))->hWeHandle));   
   }
   /*if free vtbl*/
   if((WE_UINT32)0 == ((SecMod*)(pMe->m_pIModule))->uiSecWapRefs)
   {
       FREE(((SecMod*)(pMe->m_pIModule))->pvSecWapVtbl);
   }

   if (SEC_PRIVATE_DATA != NULL)
   {
       FREE(SEC_PRIVATE_DATA);
       SEC_PRIVATE_DATA = NULL;
   }

   /*Release interfaces*/
   (void)ISHELL_Release(pMe->m_pIShell);
   (void)IMODULE_Release(pMe->m_pIModule);
   
   /*Free the object itself*/
   FREE(pMe);

   return uiRes;
}


/*==================================================================================================
FUNCTION: 
    SecW_WtlsGetCipherSuite
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    The function provides the information for wtls about the bulk cipher algorithm
    key exchange method and the trusted ca information.
ARGUMENTS PASSED:
    ISecW *      pMe[IN]: pointer of ISecW instance
    WE_INT32    iTargetID[IN]: the identity of the invoker ,input from the interface.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
    AEE_SUCCESS.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_UINT32 SecW_WtlsGetCipherSuite(ISecW * pMe, WE_INT32 iTargetID)
{
   WE_INT32 iResult = 0;

   if(NULL == pMe)
   {
      return EBADPARM;
   }

   iResult = Sec_MsgWtlsGetCipherSuite((WE_HANDLE)pMe,iTargetID);

   if(M_SEC_ERR_OK != iResult)
   {
      return ENOMEMORY; 
   }

   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);
   return AEE_SUCCESS;
}
/*==================================================================================================
FUNCTION: 
    SecW_WtlsKeyExchange
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    the interface will provide the function about the master secret key.
ARGUMENTS PASSED:
    ISecW *                      pMe[IN]: pointer of ISecW instance.
    WE_INT32                    iObjectId[IN]: the identity of the invoker ,input from the interface..
    WE_UINT8                    ucHashAlg[IN]:The value of the hash Algorithm.
    WE_UCHAR *            pucRandval[IN]:32 bytes of random data to create the master secret.
    St_SecWtlsKeyExchParams     stParam[IN]:The patam includs the public key and key exchange method.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
    AEE_SUCCESS.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_UINT32 SecW_WtlsKeyExchange(ISecW * pMe, WE_INT32 iTargetID, 
                                       St_SecWtlsKeyExchParams stParam, 
                                       WE_UINT8 ucHashAlg,  WE_UCHAR * pucRandval)
{
   WE_INT32 iResult = 0;
   St_SecWtlsKeyExchParams stParamMem = {0};
   WE_UCHAR * pucRandValMem = NULL;

   if(NULL == pMe)
   {
      return EBADPARM;
   }

   if(NULL != pucRandval)
   {
      pucRandValMem = WE_MALLOC((WE_ULONG)(32));
      if(NULL == pucRandValMem)
      {
         return ENOMEMORY;
      }
      (void)WE_MEMCPY(pucRandValMem,(WE_UCHAR *)pucRandval,(WE_UINT32)32);
   }

   stParamMem.ucKeyExchangeSuite = stParam.ucKeyExchangeSuite;

   switch(stParam.ucKeyExchangeSuite)
   {
      case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON:
      case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_512:
      case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_768:
         stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usExpLen = 
                                 stParam.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usExpLen;
         stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usModLen = 
                                 stParam.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usModLen;
         if(NULL != stParam.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent
            && stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usExpLen > 0)
         {
            stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent = 
            WE_MALLOC((WE_ULONG)stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usExpLen);

            if(NULL == stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent)
            {
               if(NULL != pucRandValMem)
               {
                  WE_FREE(pucRandValMem);
               }
               return ENOMEMORY;
            }

            (void)WE_MEMCPY(stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent,
                            stParam.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent,
                            (WE_UINT32)stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usExpLen);
         }

         if(NULL != stParam.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus
            && stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usModLen > 0)
         {
            stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus = 
            WE_MALLOC((WE_ULONG)stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usModLen);

            if(NULL == stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus)
            {
               if(NULL != pucRandValMem)
               {
                  WE_FREE(pucRandValMem);
               }
               if(NULL != stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent)
               {
                  WE_FREE(stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent);
               }
               return ENOMEMORY;
            }

            (void)WE_MEMCPY(stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus,
                            stParam.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus,
                            (WE_UINT32)stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usModLen);
         }

         if(255 == stParam.stParams.stKeyParam.stParamSpecifier.ucParameterIndex)
         {
            stParamMem.stParams.stKeyParam.stParamSpecifier.ucParameterIndex = 
            stParam.stParams.stKeyParam.stParamSpecifier.ucParameterIndex;
            if(stParam.stParams.stKeyParam.stParamSpecifier.usParamLen > 0)
            {
               stParamMem.stParams.stKeyParam.stParamSpecifier.usParamLen = 
               stParam.stParams.stKeyParam.stParamSpecifier.usParamLen;

               stParamMem.stParams.stKeyParam.stParamSpecifier.pucParams = 
                     WE_MALLOC((WE_ULONG)stParamMem.stParams.stKeyParam.stParamSpecifier.usParamLen);

               if(NULL == stParamMem.stParams.stKeyParam.stParamSpecifier.pucParams)
               {
                  if(NULL != pucRandValMem)
                  {
                     WE_FREE(pucRandValMem);
                  }
                  if(NULL != stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent)
                  {
                     WE_FREE(stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent);
                  }
                  if(NULL != stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus)
                  {
                     WE_FREE(stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus);
                  }
                  return ENOMEMORY;
               }

               (void)WE_MEMCPY(stParamMem.stParams.stKeyParam.stParamSpecifier.pucParams,
                               stParam.stParams.stKeyParam.stParamSpecifier.pucParams,
                               (WE_UINT32)stParamMem.stParams.stKeyParam.stParamSpecifier.usParamLen);
            }
         }
         break;
      case E_SEC_WTLS_KEYEXCHANGE_RSA:
      case E_SEC_WTLS_KEYEXCHANGE_RSA_512:
      case E_SEC_WTLS_KEYEXCHANGE_RSA_768:
      case E_SEC_WTLS_KEYEXCHANGE_ECDH_ECDSA:
         if(stParam.stParams.stCertificates.usBufLen > 0)
         {        
            stParamMem.stParams.stCertificates.usBufLen = 
            stParam.stParams.stCertificates.usBufLen;
            stParamMem.stParams.stCertificates.pucBuf = 
                           WE_MALLOC((WE_ULONG)stParamMem.stParams.stCertificates.usBufLen);

            if(NULL == stParamMem.stParams.stCertificates.pucBuf)
            {
               if(NULL != pucRandValMem)
               {
                  WE_FREE(pucRandValMem);
               }
               return ENOMEMORY;
            }

            (void)WE_MEMCPY(stParamMem.stParams.stCertificates.pucBuf,
                            stParam.stParams.stCertificates.pucBuf,
                            (WE_UINT32)stParamMem.stParams.stCertificates.usBufLen);
         }
         /*added by bird 070126,check the certificate*/
         else /*certificate buffer is null,return error immediatly*/
         {
              if(NULL != pucRandValMem)
              {
                 WE_FREE(pucRandValMem);
              }
              return EFAILED;
         }
         break;
      case E_SEC_WTLS_KEYEXCHANGE_SHARED_SECRET:
         if(stParam.stParams.stSecretKey.usIdLen > 0)
         {        
            stParamMem.stParams.stSecretKey.usIdLen = 
            stParam.stParams.stSecretKey.usIdLen;
            stParamMem.stParams.stSecretKey.pucIdentifier = 
                           WE_MALLOC((WE_ULONG)stParamMem.stParams.stSecretKey.usIdLen);

            if(NULL == stParamMem.stParams.stSecretKey.pucIdentifier)
            {
               if(NULL != pucRandValMem)
               {
                  WE_FREE(pucRandValMem);
               }
               return ENOMEMORY;
            }

            (void)WE_MEMCPY(stParamMem.stParams.stSecretKey.pucIdentifier,
                            stParam.stParams.stSecretKey.pucIdentifier,
                            (WE_UINT32)stParamMem.stParams.stSecretKey.usIdLen);
         }
         break;
      case E_SEC_WTLS_KEYEXCHANGE_NULL:
         (void)WE_MEMSET(&stParamMem,0,sizeof(St_SecWtlsKeyExchParams));
         break;
      default:
         break;
   }

   iResult = Sec_MsgWtlsKeyExchange((WE_HANDLE)pMe,iTargetID,stParamMem,ucHashAlg,pucRandValMem);

   if(M_SEC_ERR_OK != iResult)
   {
      if(NULL != pucRandValMem)
      {
         WE_FREE(pucRandValMem);
      }
      switch(stParam.ucKeyExchangeSuite)
      {
         case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON:
         case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_512:
         case E_SEC_WTLS_KEYEXCHANGE_RSA_ANON_768:
            if(NULL != stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent)
            {
               WE_FREE(stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent);
            }
            if(NULL != stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus)
            {
               WE_FREE(stParamMem.stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus);
            }
            if(255 == stParam.stParams.stKeyParam.stParamSpecifier.ucParameterIndex
               && NULL != stParamMem.stParams.stKeyParam.stParamSpecifier.pucParams)
            {
               WE_FREE(stParamMem.stParams.stKeyParam.stParamSpecifier.pucParams);
            }
            break;
         case E_SEC_WTLS_KEYEXCHANGE_RSA:
         case E_SEC_WTLS_KEYEXCHANGE_RSA_512:
         case E_SEC_WTLS_KEYEXCHANGE_RSA_768:
         case E_SEC_WTLS_KEYEXCHANGE_ECDH_ECDSA:
            if(NULL != stParamMem.stParams.stCertificates.pucBuf)
            {
               WE_FREE(stParamMem.stParams.stCertificates.pucBuf);
            }
            break;
         case E_SEC_WTLS_KEYEXCHANGE_SHARED_SECRET:
            if(NULL != stParamMem.stParams.stSecretKey.pucIdentifier)
            {
               WE_FREE(stParamMem.stParams.stSecretKey.pucIdentifier);
            }
            break;
         default :
            break;

      }
      return ENOMEMORY;  
   }

   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);
   return AEE_SUCCESS;
}
/*==================================================================================================
FUNCTION: 
    SecW_WtlsGetPrfResult
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    Get the Pseudo Random Function.If the master secret id is 
    to be used, "pucSecret" is NULL, and "iMasterSecretId" 
    denotes which master secret is to use. Or,"pucSecret" must be provided.
ARGUMENTS PASSED:
    ISecW *     pMe[IN]: pointer of ISecW instance.
    WE_INT32    iTargetID[IN]: the identity of the invoker ,input from the interface.
    WE_UINT8    ucAlg[IN]:The value of the Algorithm.
    WE_INT32    iMasterSecretId[IN]:ID of master secret.
    WE_INT32    iSecretLen[IN]:Length of the secret.
    WE_INT32    iSeedLen[IN]:Length of the seed.
    WE_INT32    iOutputLen[IN]:Length of the output.
    WE_UCHAR *  pucSecret[IN]:Pointer to the secret.
    WE_CHAR  *  pcLabel[IN]:Pointer to lable.
    WE_UCHAR *  pucSeed[IN]:Pointer to seed.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
    AEE_SUCCESS.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_UINT32 SecW_WtlsGetPrfResult(ISecW * pMe, WE_INT32 iTargetID, WE_UINT8 ucAlg, 
                                        WE_INT32 iMasterSecretId,  WE_UCHAR * pucSecret, 
                                        WE_INT32 iSecretLen,  WE_CHAR * pcLabel, 
                                         WE_UCHAR * pucSeed, WE_INT32 iSeedLen,
                                        WE_INT32 iOutputLen)
{
   WE_UCHAR * pucSecretMem = NULL;
   WE_CHAR  * pcLableMem = NULL;
   WE_UCHAR * pucSeedMem = NULL;
   WE_INT32   iResult = 0;

   if(NULL == pMe)
   {
      return EBADPARM;
   }


   if(NULL != pucSecret && iSecretLen > 0)
   {
      pucSecretMem = WE_MALLOC((WE_ULONG)iSecretLen);
      if(NULL == pucSecretMem)
      {
         return ENOMEMORY;
      }

      (void)WE_MEMCPY(pucSecretMem,pucSecret,(WE_UINT32)iSecretLen);
   }
   if(NULL != pucSeed && iSeedLen > 0)
   {
      pucSeedMem = WE_MALLOC((WE_ULONG)iSeedLen);
      if(!pucSeedMem)
      {
         if(NULL != pucSecretMem)
         {
            WE_FREE(pucSecretMem);
         }
         return ENOMEMORY;
      }
      (void)WE_MEMCPY(pucSeedMem, pucSeed, (WE_UINT32)iSeedLen);
   }
   if(NULL != pcLabel)
   {
      pcLableMem = SEC_STRDUP(pcLabel);
      if(NULL == pcLableMem)
      {
         if(NULL != pucSecretMem)
         {
            WE_FREE(pucSecretMem);
         }
         if(NULL != pucSeedMem)
         {
            WE_FREE(pucSeedMem);
         }
         return ENOMEMORY;
      }
   }

   iResult = Sec_MsgWtlsGetPrfResult((WE_HANDLE)pMe,iTargetID,ucAlg,iMasterSecretId,pucSecretMem,
                                     iSecretLen,pcLableMem,pucSeedMem,iSeedLen,iOutputLen);

   if(M_SEC_ERR_OK != iResult)
   {
      if(NULL != pucSecretMem)
      {
         WE_FREE(pucSecretMem);
      }
      if(NULL != pucSeedMem)
      {
         WE_FREE(pucSeedMem);
      }
      if(NULL != pcLableMem)
      {
         WE_FREE(pcLableMem);
      }
      return ENOMEMORY;  
   }

   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);

   return AEE_SUCCESS;
}
/*==================================================================================================
FUNCTION: 
    SecW_WtlsVerifySvrCertChain
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    the interface will provide the function to verify the certificate chain from the gateway.
ARGUMENTS PASSED:
    ISecW *      pMe[IN]: pointer of ISecW instance.
    WE_INT32    iTargetID[IN]: the identity of the invoker ,input from the interface.
    WE_INT32    iBufLen[IN]:Length of the buffer.
    WE_INT32    iAddrLen[IN]:Length of the address.
    WE_UCHAR *  pucBuf[IN]:Pointer to the buffer.
    WE_UCHAR *  pucAddr[IN]:Pointer to the address.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
    AEE_SUCCESS.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_UINT32 SecW_WtlsVerifySvrCertChain(ISecW * pMe, WE_INT32 iTargetID,
                                       WE_UCHAR * pucBuf, WE_INT32 iBufLen,
                                       WE_UCHAR * pucAddr, WE_INT32 iAddrLen)
{
   WE_UCHAR * pucBufMem = NULL;
   WE_UCHAR * pucAddrMem = NULL;
   WE_INT32   iResult = 0;

   if(NULL == pMe)
   {
      return EBADPARM;
   }    

   if(NULL != pucBuf && iBufLen > 0)
   {
      pucBufMem = WE_MALLOC((WE_ULONG)iBufLen);
      if(NULL == pucBufMem)
      {
         return ENOMEMORY;
      }
      (void)WE_MEMCPY(pucBufMem,pucBuf,(WE_UINT32)iBufLen);
   }
   if(NULL != pucAddr && iAddrLen > 0)
   {
      pucAddrMem = WE_MALLOC((WE_ULONG)iAddrLen);
      if(NULL == pucAddrMem)
      {
         if(NULL != pucBufMem)
         {
            WE_FREE(pucBufMem);
         }
         return ENOMEMORY;
      }
      (void)WE_MEMCPY(pucAddrMem,pucAddr,(WE_UINT32)iAddrLen);
   }

   iResult = Sec_MsgWtlsVerifySvrCert((WE_HANDLE)pMe,iTargetID,pucBufMem,iBufLen,
                                      pucAddrMem,iAddrLen,NULL);
   if(M_SEC_ERR_OK != iResult)
   {
      if(NULL != pucBufMem)
      {
         WE_FREE(pucBufMem);
      }
      if(NULL != pucAddrMem)
      {
         WE_FREE(pucAddrMem);
      }
      return ENOMEMORY;
   }

   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);

   return AEE_SUCCESS;
}
/*==================================================================================================
FUNCTION: 
    SecW_WtlsGetUserCert
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    the interface will provide the function to return the client certificate to the wtls.
ARGUMENTS PASSED:
    ISecW *     pMe[IN]: pointer of ISecW instance.
    WE_INT32    iTargetID[IN]: the identity of the invoker ,input from the interface.
    WE_INT32    iBufLen[IN]:Length of the buffer.
    WE_UCHAR *  pucBuf[IN]:Pointer to the buffer.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
    AEE_SUCCESS.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_UINT32 SecW_WtlsGetUserCert(ISecW * pMe, WE_INT32 iTargetID, 
                                         WE_UCHAR * pucBuf, WE_INT32 iBufLen)
{
   WE_UCHAR * pucBufMem = NULL;
   WE_INT32   iResult = 0;
/* add by Sam [070126] */
#ifndef M_SEC_CFG_WTLS_CLASS_3
   return EFAILED;
#endif
   if(NULL == pMe)
   {
      return EBADPARM;
   }
   if(NULL != pucBuf && iBufLen > 0)
   {
      pucBufMem = WE_MALLOC((WE_ULONG)iBufLen);
      if(NULL == pucBufMem)
      {
         return ENOMEMORY;
      }
      (void)WE_MEMCPY(pucBufMem,pucBuf,(WE_UINT32)iBufLen);
   }

   iResult = Sec_MsgWtlsGetUsrCert((WE_HANDLE)pMe,iTargetID,pucBufMem,iBufLen);

   if(M_SEC_ERR_OK != iResult)
   {
      if(NULL != pucBufMem)
      {
         WE_FREE(pucBufMem);
      }
      return ENOMEMORY; 
   }

   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);

   return AEE_SUCCESS;
}
/*==================================================================================================
FUNCTION: 
    SecW_WtlsCompSign
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    compute the signature for wtls.
ARGUMENTS PASSED:
    ISecW *       pMe[IN]: pointer of ISecW instance.
    WE_INT32      iTargetID[IN]: the identity of the invoker ,input from the interface.
    WE_UCHAR *    pucKeyId[IN]:Pointer to the key id .
    WE_INT32      iKeyIdLen[IN]:Length of the key id.
    WE_UCHAR *    pucBuf[IN]:Pointer to the buffer.
    WE_INT32      iBufLen[IN]:Length of the buffer.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
    AEE_SUCCESS.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_UINT32 SecW_WtlsCompSign(ISecW * pMe, WE_INT32 iTargetID,
                                     WE_UCHAR * pucKeyId, WE_INT32 iKeyIdLen,
                                     WE_UCHAR * pucBuf, WE_INT32 iBufLen)
{
   WE_UCHAR * pucKeyIdMem = NULL;
   WE_UCHAR * pucBufMem = NULL;
   WE_INT32   iResult = 0;
/* add by Sam [070126] */
#ifndef M_SEC_CFG_WTLS_CLASS_3
   return EFAILED;
#endif
   if(NULL == pMe)
   {
      return EBADPARM;
   }    

   if(NULL != pucKeyId && iKeyIdLen > 0)
   {
      pucKeyIdMem = WE_MALLOC((WE_ULONG)iKeyIdLen);
      if(NULL == pucKeyIdMem)
      {
         return ENOMEMORY;
      }
      (void)WE_MEMCPY(pucKeyIdMem,pucKeyId,(WE_UINT32)iKeyIdLen);
   }
   if(NULL != pucBuf && iBufLen > 0)
   {
      pucBufMem = WE_MALLOC((WE_ULONG)iBufLen);
      if(NULL == pucBufMem)
      {
         if(NULL != pucKeyIdMem)
         {
            WE_FREE(pucKeyIdMem);
         }
         return ENOMEMORY;
      }
      (void)WE_MEMCPY(pucBufMem,pucBuf,(WE_UINT32)iBufLen);
   }

   iResult = Sec_MsgWtlsCompSign((WE_HANDLE)pMe,iTargetID,pucKeyIdMem,iKeyIdLen,pucBufMem,iBufLen);


   if(M_SEC_ERR_OK != iResult)
   {
      if(NULL != pucKeyIdMem)
      {
         WE_FREE(pucKeyIdMem);
      }
      if(NULL != pucBufMem)
      {
         WE_FREE(pucBufMem);
      }
      return ENOMEMORY;  
   }

   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);

   return AEE_SUCCESS;
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
    ISecW *             pMe[IN]: pointer of ISecW instance.
    WE_INT32            iTargetID[IN]: the identity of the invoker ,input from the interface.
    WE_INT32            iMasterSecretId[IN]:ID of the master secret.
    WE_INT32            iSecurityId[IN]:ID of the security.
    WE_INT32            iFullHandshake[IN]:whether is full hand shake.
    St_SecSessionInformation    stSessionInfo[IN]:Information of the session.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
    AEE_SUCCESS.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_UINT32 SecW_SetupConnection(ISecW * pMe, WE_INT32 iTargetID, WE_INT32 iMasterSecretId, 
                                          WE_INT32 iSecurityId, WE_INT32 iFullHandshake,
                                          St_SecSessionInformation stSessionInfo)
{
   St_SecSessionInfo stInfo = {0};
   St_SecAsn1Certificate * pstCert = NULL;
   WE_INT32   iResult = 0;

   if(NULL == pMe)
   {
      return EBADPARM;
   }    

   /*add by birdzhang 061017*/
   if(stSessionInfo.ucNumCerts > 0)
   {
      pstCert = WE_MALLOC((WE_ULONG)sizeof(St_SecAsn1Certificate));
      if(NULL == pstCert)
      {
         return ENOMEMORY;
      }

      pstCert->ucFormat = stSessionInfo.ucCertFormat;
      pstCert->uiCertLen = stSessionInfo.uiCertLen;
      if(stSessionInfo.uiCertLen > (WE_UINT32)0 && NULL != stSessionInfo.pucCert)
      {    
         pstCert->pucCert = WE_MALLOC((WE_ULONG)stSessionInfo.uiCertLen);
         if(NULL == pstCert->pucCert)
         {          
            WE_FREE(pstCert);
            return ENOMEMORY;
         }
         (void)WE_MEMCPY(pstCert->pucCert,stSessionInfo.pucCert,stSessionInfo.uiCertLen);
      }
      else
      {
         pstCert->pucCert = NULL;
      }
   }
   stInfo.ucConnectionType = M_SEC_WTLS_CONNECTION_MODE;
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
      return ENOMEMORY; 
   }

   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);

   return AEE_SUCCESS;
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
    ISecW *         pMe[IN]: pointer of ISecW instance.
    WE_INT32        iTargetID[IN]: the identity of the invoker ,input from the interface.
    WE_INT32        iSecurityId[IN]:ID of the security.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
    AEE_SUCCESS.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_UINT32 SecW_StopConnection(ISecW * pMe, WE_INT32 iTargetID, WE_INT32 iSecurityId)
{
   WE_INT32   iResult = 0;

   if(NULL == pMe)
   {
      return EBADPARM;
   }    

   iResult = Sec_MsgStopCon((WE_HANDLE)pMe,iTargetID,iSecurityId,(WE_UCHAR)M_SEC_WTLS_CONNECTION_MODE);

   if(M_SEC_ERR_OK != iResult)
   {
      return ENOMEMORY; 
   }

   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);

   return AEE_SUCCESS;
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
    ISecW *         pMe[IN]: pointer of ISecW instance.
    WE_INT32        iMasterSecretId[IN]:ID of the master secret.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
    AEE_SUCCESS.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_UINT32 SecW_RemovePeer(ISecW * pMe, WE_INT32 iMasterSecretId)
{
   WE_INT32   iResult = 0;
   WE_INT32 iTargetId = 0; /*added by Bird 061123 for warning dialog to compile*/

   if(NULL == pMe)
   {
      return EBADPARM;
   }    

   iResult = Sec_MsgRemovePeer((WE_HANDLE)pMe,iTargetId,iMasterSecretId);

   if(M_SEC_ERR_OK != iResult)
   {
      return ENOMEMORY;
   }

   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);

   return AEE_SUCCESS;
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
    ISecW *       pMe[IN]: pointer of ISecW instance.
    WE_UCHAR *    pucAddress[IN]:Pointer to the address.
    WE_INT32      iAddressLen[IN]:Length of the address.
    WE_UINT16     usPortnum[IN]:Number of the port.
    WE_INT32      iMasterSecretId[IN]:ID of the master secret.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
    AEE_SUCCESS.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_UINT32 SecW_AttachPeerToSession(ISecW * pMe,  WE_UCHAR * pucAddress, WE_INT32 iAddressLen,
                                        WE_UINT16 usPortnum, WE_INT32 iMasterSecretId)
{
   WE_UCHAR * pucAddressMem = NULL;
   WE_INT32   iResult = 0;

   if(NULL == pMe)
   {
      return EBADPARM;
   }    

   if(NULL != pucAddress && iAddressLen > 0)
   {
      pucAddressMem = WE_MALLOC((WE_ULONG)iAddressLen);
      if(NULL == pucAddressMem)
      {
         return ENOMEMORY;
      }
      (void)WE_MEMCPY(pucAddressMem,pucAddress,(WE_UINT32)iAddressLen);
   }

   iResult = Sec_MsgAttachPeerToSession((WE_HANDLE)pMe,(WE_UCHAR)M_SEC_WTLS_CONNECTION_MODE,pucAddressMem,
                                        iAddressLen, NULL,usPortnum,iMasterSecretId);

   if(M_SEC_ERR_OK != iResult)
   {
      if(NULL != pucAddressMem)
      {
         WE_FREE(pucAddressMem);
      }
      return ENOMEMORY;  
   }

   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);

   return AEE_SUCCESS;
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
    ISecW *       pMe[IN]: pointer of ISecW instance.
    WE_UCHAR *    pucAddress[IN]:Pointer to the address.
    WE_INT32      iAddressLen[IN]:Length of the address.
    WE_UINT16     usPortnum[IN]:Number of the port.
    WE_INT32      iTargetID[IN]:ID of the object.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
    AEE_SUCCESS.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_UINT32 SecW_SearchPeer(ISecW * pMe, WE_INT32 iTargetID,  WE_UCHAR * pucAddress, 
                                  WE_INT32 iAddressLen, WE_UINT16 usPortnum)
{
   WE_UCHAR * pucAddressMem = NULL;
   WE_INT32   iResult = 0;

   if(NULL == pMe)
   {
      return EBADPARM;
   }    

   if(NULL != pucAddress && iAddressLen > 0)
   {
      pucAddressMem = (WE_UCHAR*)WE_MALLOC((WE_ULONG)iAddressLen);
      if(NULL == pucAddressMem)
      {
         return ENOMEMORY;
      }
      (void)WE_MEMCPY(pucAddressMem,pucAddress,(WE_UINT32)iAddressLen);
   }


   iResult = Sec_MsgSearchPeer((WE_HANDLE)pMe,iTargetID,(WE_UCHAR)M_SEC_WTLS_CONNECTION_MODE,pucAddressMem,iAddressLen,NULL,usPortnum);

   if(M_SEC_ERR_OK != iResult)
   {
      WE_FREE(pucAddressMem); 
      return ENOMEMORY; 
   }

   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);

   return AEE_SUCCESS;
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
    ISecW *     pMe[IN]: pointer of ISecW instance.
    WE_INT32    iMasterSecretId[IN]:ID of the master secret.
    WE_UINT8    ucIsActive[IN]:flg of the active.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
    AEE_SUCCESS.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_UINT32 SecW_EnableSession(ISecW * pMe, WE_INT32 iMasterSecretId, WE_UINT8 ucIsActive)
{
   WE_INT32   iResult = 0;

   if(NULL == pMe)
   {
      return EBADPARM;
   }    

   iResult = Sec_MsgEnableSession((WE_HANDLE)pMe,iMasterSecretId,ucIsActive);

   if(M_SEC_ERR_OK != iResult)
   {
      return ENOMEMORY; 
   } 

   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);

   return AEE_SUCCESS;
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
    ISecW *     pMe[IN]: pointer of ISecW instance.
    WE_INT32    iMasterSecretId[IN]:ID of the master secret.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
    AEE_SUCCESS.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_UINT32 SecW_DisableSession(ISecW * pMe, WE_INT32 iMasterSecretId)
{
   WE_INT32   iResult = 0;

   if(NULL == pMe)
   {
      return EBADPARM;
   }    

   iResult = Sec_MsgDisableSession((WE_HANDLE)pMe,iMasterSecretId);

   if(M_SEC_ERR_OK != iResult)
   {
      return ENOMEMORY; 
   }

   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);

   return AEE_SUCCESS;
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
    ISecW *     pMe[IN]: pointer of ISecW instance.
    WE_INT32    iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_INT32    iMasterSecretId[IN]:ID of the master secret.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
    AEE_SUCCESS.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_UINT32 SecW_SessionGet(ISecW * pMe, WE_INT32 iTargetID, WE_INT32 iMasterSecretId)
{
   WE_INT32   iResult = 0;

   if(NULL == pMe)
   {
      return EBADPARM;
   }    

   iResult = Sec_MsgGetSessionInfo((WE_HANDLE)pMe,iTargetID,iMasterSecretId);

   if(M_SEC_ERR_OK != iResult)
   {
      return ENOMEMORY; 
   } 

   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);

   return AEE_SUCCESS;
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
    ISecW *        pMe[IN]: pointer of ISecW instance.
    WE_INT32       iMasterSecretId[IN]:ID of the master secret.
    WE_UINT8       ucSessionOptions[IN]:The option of the session.
    WE_UCHAR *     pucSessionId[IN]:Pointer to the session ID.
    WE_UINT8       ucSessionIdLen[IN]:Length of the session id.
    WE_UINT8       aucCipherSuite[IN]:Array for cipher suite.
    WE_UINT8       ucCompressionAlg[IN]:The value of the compression Algorithm.
    WE_UCHAR *     pucPrivateKeyId[IN]:ID of the private key.
    WE_UINT32      uiCreationTime[IN]:the time of creation.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
    AEE_SUCCESS.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_UINT32 SecW_SessionRenew(ISecW * pMe, WE_INT32 iMasterSecretId, 
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
      return EBADPARM;
   }    

   if(NULL != pucSessionId && ucSessionIdLen > 0)
   {
      pucSessionIdMem = WE_MALLOC((WE_ULONG)ucSessionIdLen);
      if(NULL == pucSessionIdMem)
      {
         return ENOMEMORY;
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
         return ENOMEMORY;
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
      return ENOMEMORY;  
   }

   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);

   return AEE_SUCCESS;
}
/*==================================================================================================
FUNCTION: 
    SecW_RegClientEvtCB
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    register one callback for sec module, and through the callback, sec module can
    return some information to the invoker.
ARGUMENTS PASSED:
    ISec *      pMe[IN]: pointer about an instance of ISec   
    WE_HANDLE   hPrivData[IN]:Private data handle used in callback function.
    Fn_ISecEventHandle  pcbSecEvtFunc[IN]:pointer about The callback function .
RETURN VALUE:
    EBADPARM,
    AEE_SUCCESS
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_UINT32 SecW_RegClientEvtCB(ISecW * pMe,WE_HANDLE hPrivData,Fn_ISecEventHandle pcbSecEvtFunc)
{
    
   if(NULL == pMe || NULL == hPrivData || NULL == pcbSecEvtFunc)
   {
      return EBADPARM;
   }

   pMe->hcbSecEvtFunc = (WE_HANDLE)pcbSecEvtFunc;
   pMe->hSecPrivDataCB = hPrivData;

   return AEE_SUCCESS;
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
    ISecW *        pMe[IN]:pointer of ISecW instance.
    St_SelectCertAction  stChooseCerByName[IN]:The content of how to choose cert by name.
RETURN VALUE:
    EBADPARM,
    EFAILED,
    AEE_SUCCESS.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_UINT32 SecW_EvtSelectCertAction(ISecW * pMe,WE_INT32 iTargetID,St_SelectCertAction stChooseCerByName)
{
   WE_INT32   iResult = 0;

   if(NULL == pMe)
   {
      return EBADPARM;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"Sec_EvtSelectCertAction\n"));

   iResult = Sec_UeSelectAction((WE_HANDLE)pMe,iTargetID, stChooseCerByName);

   if(M_SEC_ERR_OK != iResult)
   {
      return EFAILED;
   }

   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);

   return AEE_SUCCESS;
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
    ISecB *             pMe[IN]:pointer of ISecB instance.
    St_ConfirmAction    stConfirm[IN]:The value of the confirm.
RETURN VALUE:
    EBADPARM,
    AEE_SUCCESS,
    EFAILED  
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_UINT32 SecW_EvtConfirmAction(ISecW * pMe,WE_INT32 iTargetID,St_ConfirmAction stConfirm)
{
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return EBADPARM;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"Sec_EvtConfirmAction\n"));
    
   iResult = Sec_UeConfirmAction((WE_HANDLE)pMe,iTargetID, stConfirm);
        
   if(M_SEC_ERR_OK != iResult)
   {
       return EFAILED;  
   }
    
   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);    
   return AEE_SUCCESS;
}
#endif

static WE_UINT32 SecW_EvtShowDlgAction(ISecW* pMe,WE_INT32 iTargetId,
                                                  WE_VOID* pvSrcData,WE_UINT32 uiLength)
{
    WE_INT32 iRes = 0;
    WE_INT32 iEvent = 0;
    WE_VOID* pvData = NULL;
    if((NULL == pMe) || (NULL == pvSrcData))
    {
        return EFAILED;
    }
    /*decode*/
    if(!Sec_DecodeEvtAction(pvSrcData,uiLength,&iEvent,&pvData))
    {
        if(NULL != pvData)
        {
            WE_FREE(pvData);
            pvData = NULL;
        }
        return EFAILED;
    }
    switch(iEvent)
    {
        case M_UE_SELECT_CET_EVENT:
        {
            St_SelectCertAction* pstSelCert = (St_SelectCertAction*)pvData;
            if(NULL == pstSelCert)
            {
                return ENOMEMORY;
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
                return ENOMEMORY;
            }
            
            iRes = (WE_INT32)SecW_EvtConfirmAction(pMe,iTargetId,*pstConfirm);

        }
        break;
#endif        
        default:
            iRes = EFAILED;
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
    ISecW_New
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    create one instance of ISecW interface.
ARGUMENTS PASSED:
    WE_UINT16         sSize[IN]:size of ISecW
    IShell *        pIShell[IN]:Pointer to the IShell interface.
    IModule*        pIModule[IN]:Pointer to the module.
    ISecW **        ppMod[IN]:Pointer to mod.
RETURN VALUE:
    ENOMEMORY,
    EFAILED,
    AEE_SUCCESS
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 ISecW_New(WE_UINT16 sSize, IShell *pIShell, IModule* pIModule, ISecW ** ppMod)
{
   ISecW * pMe = NULL;
   VTBL(ISecW) * modFuncs = NULL;

   if( !ppMod || !pIShell || !pIModule )
   {
      return EFAILED;
   }

   *ppMod = NULL;
   /* Allocate memory for the ExtensionCls object*/
   if( sSize < (WE_UINT16)sizeof(ISecW) )
   {
      sSize += (WE_UINT16)sizeof(ISecW);
   }
   /* Allocate the module's struct and initialize it. Note that the 
   modules and apps must not have any static data. 
   Hence, we need to allocate the vtbl as well.*/

   if((pMe = (ISecW*)MALLOC((WE_ULONG)(sSize))) == NULL )
   {
      return ENOMEMORY;
   }

   pMe->pvt = NULL;

   /*Initialize individual entries in the VTBL*/
   if(0 == ((SecMod*)pIModule)->uiSecWapRefs)
   {
      if(NULL == (((SecMod*)pIModule)->pvSecWapVtbl =
      (void*)MALLOC(sizeof(VTBL(ISecW)))))
      {
         FREE(pMe);
         return ENOMEMORY;
      }
      modFuncs = (VTBL(ISecW)*)(((SecMod*)pIModule)->pvSecWapVtbl);

      modFuncs->AddRef = SecW_AddRef;
      modFuncs->Release = SecW_Release;

      modFuncs->RegSecClientEvtCB = SecW_RegClientEvtCB;

      modFuncs->WtlsGetCipherSuite = SecW_WtlsGetCipherSuite;
      modFuncs->WtlsKeyExchange = SecW_WtlsKeyExchange;
      modFuncs->WtlsGetPrfResult = SecW_WtlsGetPrfResult;
      modFuncs->WtlsVerifySvrCertChain = SecW_WtlsVerifySvrCertChain;
      modFuncs->WtlsGetUserCert = SecW_WtlsGetUserCert;
      modFuncs->WtlsCompSign = SecW_WtlsCompSign;

      modFuncs->SetupConnection = SecW_SetupConnection;
      modFuncs->StopConnection = SecW_StopConnection;

      modFuncs->RemovePeer = SecW_RemovePeer;
      modFuncs->AttachPeerToSession = SecW_AttachPeerToSession;
      modFuncs->SearchPeer = SecW_SearchPeer;

      modFuncs->EnableSession = SecW_EnableSession;
      modFuncs->SessionGet = SecW_SessionGet;
      modFuncs->DisableSession = SecW_DisableSession;
      modFuncs->SessionRenew = SecW_SessionRenew;  
      modFuncs->EvtShowDlgAction =SecW_EvtShowDlgAction;
   }    

   if(NULL == (modFuncs = (VTBL(ISecW)*)(((SecMod*)pIModule)->pvSecWapVtbl)))
   {
      FREE(pMe);
      return EFAILED;
   }

   /* initialize the vtable*/
   INIT_VTBL(pMe, ISecW, *modFuncs);

   /* initialize the data members*/
   pMe->m_nRefs = 1;
   pMe->m_pIShell = pIShell;
   pMe->m_pIModule = pIModule;
   pMe->hcbSecEvtFunc = NULL;
   pMe->hSecPrivDataCB = NULL;
   /* initialize the data members*/   
   /*malloc memory for global variable*/
   if((pMe->hPrivateData = MALLOC((WE_ULONG)(sizeof(St_PublicData)))) == NULL)
   {
      FREE(pMe);
      return EFAILED;
   }   
   (void)MEMSET(pMe->hPrivateData,0,sizeof(St_PublicData));
   pMe->iIFType = (WE_INT32)E_SEC_WTLS;
   (pMe->hUserCertKeyPair) = ((SecMod*)pIModule)->astPubUserCertKeyPair;
   /*added by bird 070126*/
   SEC_SIINFO = &(((SecMod*)pIModule)->stInfo);
   /*added by bird 070202*/
   SEC_PEER_CACHE = ((SecMod*)pIModule)->astPeerCache;
   SEC_SESSION_CACHE = ((SecMod*)pIModule)->astSessionCache;
   SEC_WIM_HAVELOADUSERPRIV = &(((SecMod*)pIModule)->cHaveLoad);
   /*Note: this must be run after the former sentence*/
   if(SUCCESS != (WE_INT32)WeMgr_Initialize(pIShell, (IModule*)pIModule, &(((St_PublicData*)(pMe->hPrivateData))->hWeHandle)))
   { 
       FREE(pMe->hPrivateData);
       FREE(pMe);
       return EFAILED;
   }
   if(SUCCESS != Sec_StartEx((WE_HANDLE)pMe))
   {
       (void)WeMgr_Terminate(((St_PublicData*)(pMe->hPrivateData))->hWeHandle);
       FREE(pMe->hPrivateData);
       FREE(pMe);
       return EFAILED;
   }
   
   /* Add References and get IDisplay*/
   (void)ISHELL_AddRef(pIShell);
   (void)IMODULE_AddRef(pIModule);

   /* Set the pointer in the parameter*/
   *ppMod = (ISecW*)pMe;

   /*brs vtbl refernce add*/
   ((SecMod*)pIModule)->uiSecWapRefs++;

   return AEE_SUCCESS;
}
