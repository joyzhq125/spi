/*==================================================================================================
    HEADER NAME : isecb.c
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

/***************************************************************************************************
*   Include File Section
***************************************************************************************************/

#include "sec_comm.h"
#include "sec_mod.h"
#include "oem_secmgr.h"
#include "isecb.h"

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
#define SEC_IAEECALLBACK_VAR    (((St_PublicData *)(((ISecB*)pMe)->hPrivateData))->m_cb)
#define SEC_IISHELL_POINTER     (((ISecB*)pMe)->m_pIShell)

#define SEC_PRIVATE_DATA        (((ISecB*)pMe)->hPrivateData)
#define SEC_SIINFO              (((St_PublicData *)(((ISecB*)pMe)->hPrivateData))->pstInfo)
#define SEC_PEER_CACHE          (((St_PublicData *)(((ISecB*)pMe)->hPrivateData))->pstPeerCache)
#define SEC_SESSION_CACHE       (((St_PublicData *)(((ISecB*)pMe)->hPrivateData))->pstSessionCache)
#define SEC_WIM_HAVELOADUSERPRIV        (((St_PublicData *)(((ISecB *)pMe)->hPrivateData))->pcHaveLoad)



/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*==================================================================================================
FUNCTION: 
    SecB_AddRef
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    sec add reference
ARGUMENTS PASSED:
    ISecB * pMe[IN]: pointer about an instance of ISecB    
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
static WE_UINT32 SecB_AddRef(ISecB *pMe)
{
   if(NULL == pMe)
   {
      return 0;
   }
   return (WE_UINT32)++(pMe->m_nRefs);
}
/*==================================================================================================
FUNCTION: 
    SecB_Release
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    sec release
ARGUMENTS PASSED:
    ISecB * pMe[IN]: pointer about an instance of ISecB 
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
static WE_UINT32 SecB_Release (ISecB *pMe)
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
   ((SecMod*)(pMe->m_pIModule))->uiSecBrsRefs--;
   
   /*added by bird 061113*/
   /*1.cancel callback*/
   SEC_CANCELCALLBACK(SEC_IAEECALLBACK_VAR);
   /*modified by Bird 061205*/
#if 0 
   /*2.free the first message list*/
   Sec_MsgDelById(pMe);
   /*3. free the second message list*/
   Sec_DpHandleQueueDelById(pMe);
   /*4.free the pkc resp message list*/
   Sec_WimDelElementById(pMe);
#endif
   /*2.free the correlative global memory and initial global variable that control state*/
   Sec_DpResetVariable(pMe);
   
   /*if stop file operation*/   
   uiRes += Sec_StopEx(pMe);
   if(NULL != (St_PublicData*)(pMe->hPrivateData))
   {
        uiRes += (WE_UINT32)WeMgr_Terminate((((St_PublicData*)(pMe->hPrivateData))->hWeHandle));
   }
   
   /*if free vtbl*/
   if((WE_UINT32)0 == ((SecMod*)(pMe->m_pIModule))->uiSecBrsRefs)
   {
       FREE(((SecMod*)(pMe->m_pIModule))->pvSecBrsVtbl);
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
    SecB_GetCertNameList
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    get certificate name list saved on the current device.
ARGUMENTS PASSED:
    ISecB *     pMe[IN]:pointer of ISecB instance.
    WE_INT32    iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_INT32    iMasterSecretId:ID of the master secret.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
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
static WE_UINT32 SecB_GetCertNameList(ISecB * pMe, WE_INT32 iTargetID, WE_INT32 iCertType)
{
   WE_INT32 iResult = 0;

   if(NULL == pMe)
   {
       return EBADPARM;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"Sec_GetCertNames\n"));
    
   iResult = Sec_MsgGetCertNameList((WE_HANDLE)pMe,iTargetID,iCertType);
    
   if(M_SEC_ERR_OK != iResult)
   {
       return ENOMEMORY; 
   }  

   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);

   return AEE_SUCCESS;
}
/*==================================================================================================
FUNCTION: 
    SecB_GenKeyPair
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    generate one key pair for the device.
ARGUMENTS PASSED:
    ISecB *     pMe[IN]:pointer of ISecB instance.
    WE_INT32    iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_UINT8    ucKeyType[IN]:The value of the key type.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
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
static WE_UINT32 SecB_GenKeyPair(ISecB * pMe, WE_INT32 iTargetID, WE_UINT8 ucKeyType)
{
   WE_INT32 iResult = 0;
    
   if(NULL == pMe)
   {
       return EBADPARM;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_GenKeyPair\n"));


   iResult = Sec_MsgKeyPairGen((WE_HANDLE)pMe,iTargetID,ucKeyType,M_SEC_SP_RSA);
    
   if(M_SEC_ERR_OK != iResult)
   {
       return ENOMEMORY; 
   }
    
   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);
   return AEE_SUCCESS;
}

#ifdef G_SEC_CFG_SHOW_PIN
/*==================================================================================================
FUNCTION: 
    SecB_PinModify
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    modify the pin code for private key.
ARGUMENTS PASSED:
    ISecB *         pMe[IN]:pointer of ISecB instance.
    WE_INT32        iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_UINT8        ucKeyType[IN]:The value of the key type.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
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
static WE_UINT32 SecB_PinModify(ISecB * pMe, WE_INT32 iTargetID, WE_UINT8 ucKeyType)
{
   WE_INT32 iResult = 0;

   if(NULL == pMe)
   {
       return EBADPARM;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_PinModify\n"));
    

   iResult = Sec_MsgModifyPin((WE_HANDLE)pMe,iTargetID,ucKeyType);
    
   if(M_SEC_ERR_OK != iResult)
   {
       return ENOMEMORY;  
   }
    
   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);
   return AEE_SUCCESS;
}
#endif
/*==================================================================================================
FUNCTION: 
    SecB_GetCurSvrCert
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    get the detailed information about the current used certificate.
ARGUMENTS PASSED:
    ISecB *         pMe[IN]:pointer of ISecB instance.
    WE_INT32        iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_INT32        iSecID[IN]:ID of the security.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
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
static WE_UINT32 SecB_GetCurSvrCert(ISecB * pMe, WE_INT32 iTargetID, WE_INT32 iSecID)
{
   WE_INT32 iResult = 0;

   if(NULL == pMe)
   {
       return EBADPARM;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_GetCurSvrCert\n"));
    
   iResult = Sec_MsgViewCurCert((WE_HANDLE)pMe,iTargetID,iSecID);
    
   if(M_SEC_ERR_OK != iResult)
   {
       return ENOMEMORY; 
   }
    
   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);
   return AEE_SUCCESS;
}
/*==================================================================================================
FUNCTION: 
    SecB_GetSessionInfo
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    get the current session information.
ARGUMENTS PASSED:
    ISecB *     pMe[IN]:pointer of ISecB instance.
    WE_INT32    iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_INT32    iSecID[IN]:ID of the security.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
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
static WE_UINT32 SecB_GetSessionInfo(ISecB * pMe, WE_INT32 iTargetID, WE_INT32 iSecID)
{
   WE_INT32 iResult = 0;

   if(NULL == pMe)
   {
       return EBADPARM;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_GetSessionInfo\n"));
    
   iResult = Sec_MsgViewInfoSession((WE_HANDLE)pMe,iTargetID,iSecID);
    
   if(M_SEC_ERR_OK != iResult)
   {
       return ENOMEMORY;  
   }
    
   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);
   return AEE_SUCCESS;
}
/*==================================================================================================
FUNCTION: 
    SecB_GetWtlsCurClasss
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    get current sec class
ARGUMENTS PASSED:
    ISecB *     pMe[IN]:pointer of ISecB instance.
    WE_INT32    iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_INT32    iSecID[IN]:ID of the security.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
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
static WE_UINT32 SecB_GetWtlsCurClasss(ISecB * pMe, WE_INT32 iTargetID, WE_INT32 iSecID)
{
   WE_INT32 iResult = 0;

   if(NULL == pMe)
   {
       return EBADPARM;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_GetWtlsCurClasss\n"));
    
   iResult = Sec_MsgGetSecCurClass((WE_HANDLE)pMe,iTargetID,iSecID);
    
   if(M_SEC_ERR_OK != iResult)
   {
       return ENOMEMORY; 
   }
    
   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);
   return AEE_SUCCESS;
}
/*==================================================================================================
FUNCTION: 
    SecB_GetContractsList
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    get contract name list
ARGUMENTS PASSED:
    ISecB *     pMe[IN]:pointer of ISecB instance.
    WE_INT32    iTargetID[IN]:the identity of the invoker ,input from the interface.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
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
static WE_UINT32 SecB_GetContractsList(ISecB * pMe, WE_INT32 iTargetID)
{
   WE_INT32 iResult = 0;

   if(NULL == pMe)
   {
       return EBADPARM;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_GetContractsList\n"));    
  
   iResult = Sec_MsgGetContractsInfos((WE_HANDLE)pMe,iTargetID);
   if(M_SEC_ERR_OK != iResult)
   {
       return ENOMEMORY;  
   }
    
   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);
   return AEE_SUCCESS;
}

/*==================================================================================================
FUNCTION: 
    SecB_TransferCert
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    save certificate. when one cert is got from client, this interface can be invoked and 
    certificate storage flow will be executed.
ARGUMENTS PASSED:
    ISecB *              pMe[IN]:pointer of ISecB instance.
    St_SecCertContent    stCertContent[IN]:The content of the cert.   
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
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
static WE_UINT32 SecB_TransferCert(ISecB * pMe,St_SecCertContent stCertContent)
{
   St_SecCertContent * pstCertContent = NULL;
   WE_INT32 iRes = 0;
    
   if(NULL == pMe)
   {
       return EBADPARM;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_TransferCert\n"));
    
   pstCertContent = (St_SecCertContent *)WE_MALLOC((WE_ULONG)sizeof(St_SecCertContent));
   if(NULL == pstCertContent)
   {
       return ENOMEMORY;
   }

   (void)WE_MEMSET(pstCertContent,0,sizeof(St_SecCertContent));
    
   /*MIME*/
   if(NULL == stCertContent.pcMime)
   {
       WE_FREE(pstCertContent);
       return EBADPARM;
   }
   else
   {
       pstCertContent->pcMime = SEC_STRDUP(stCertContent.pcMime);
       if(NULL == pstCertContent->pcMime)
       {
           WE_FREE(pstCertContent);
           return ENOMEMORY;
       }
   }

   /*certificate data*/
   if((NULL != stCertContent.pucCertData) && (stCertContent.uiCertLen > (WE_UINT32)0))
   {
           pstCertContent->uiCertLen = stCertContent.uiCertLen;
            
           pstCertContent->pucCertData = WE_MALLOC((WE_ULONG)(pstCertContent->uiCertLen + (WE_UINT32)1)*(WE_ULONG)sizeof(WE_CHAR));	
           if (NULL == pstCertContent->pucCertData) 
           {
               WE_FREE(pstCertContent->pcMime);
               WE_FREE(pstCertContent);
               return ENOMEMORY;
           }
           (void)WE_MEMCPY(pstCertContent->pucCertData,stCertContent.pucCertData,pstCertContent->uiCertLen);
           pstCertContent->pucCertData[pstCertContent->uiCertLen] = '\0';
   }
   else
   {
       WE_FREE(pstCertContent->pcMime);
       WE_FREE(pstCertContent);
       return EBADPARM;
   }
    
   iRes = Sec_PostMsg((WE_HANDLE)pMe, (WE_INT32)E_SEC_MSG_CERTTRANSFER, (void*)pstCertContent);
   if (iRes != M_SEC_ERR_OK)
   {
       WE_SIGNAL_DESTRUCT(0, (WE_INT32)M_SEC_DP_MSG_EXECUTE_COMMAND, (void*)pstCertContent);
       return ENOMEMORY;
   }

   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);

   return AEE_SUCCESS;
}
/*==================================================================================================
FUNCTION: 
    SecB_GetRequestUserCert
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    Get request of user certificate in base64-DER/DER code. After key pairs are generated,
    this interface can be used to get the user cert using the key pairs.
ARGUMENTS PASSED:
    ISecB *                 pMe[IN]:pointer of ISecB instance.
    St_SecUserCertReqInfo   stUserCertReqInfo[IN]:The infomation of request about user cert.   
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
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
static WE_UINT32 SecB_GetRequestUserCert(ISecB * pMe, St_SecUserCertReqInfo stUserCertReqInfo)
{
   St_SecUserCertReqInfo * pstUserCertReqInfo = NULL;
   WE_INT32 iRes = 0;
    
   if(NULL == pMe)
   {
       return EBADPARM;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_GetRequestUserCert\n"));
   
   pstUserCertReqInfo = (St_SecUserCertReqInfo *)WE_MALLOC((WE_ULONG)sizeof(St_SecUserCertReqInfo));
   if(NULL == pstUserCertReqInfo)
   {
       return ENOMEMORY;
   }
   (WE_VOID)WE_MEMSET(pstUserCertReqInfo, 0, sizeof(St_SecUserCertReqInfo)); 
   
   pstUserCertReqInfo->iTargetID= stUserCertReqInfo.iTargetID;   
   pstUserCertReqInfo->ucCertUsage = stUserCertReqInfo.ucCertUsage;
   pstUserCertReqInfo->ucKeyUsage= stUserCertReqInfo.ucKeyUsage;
   pstUserCertReqInfo->ucSignType= stUserCertReqInfo.ucSignType;
   
   if((NULL != stUserCertReqInfo.pucName) && (stUserCertReqInfo.usNameLen > 0))
   {
       pstUserCertReqInfo->usNameLen = stUserCertReqInfo.usNameLen;
       
       pstUserCertReqInfo->pucName = WE_MALLOC((pstUserCertReqInfo->usNameLen)*sizeof(WE_CHAR));
       if (NULL == pstUserCertReqInfo->pucName) 
       {
           WE_FREE(pstUserCertReqInfo);
           return ENOMEMORY;
       }
       (WE_VOID)WE_MEMCPY(pstUserCertReqInfo->pucName, stUserCertReqInfo.pucName, pstUserCertReqInfo->usNameLen);
   }

   if((NULL != stUserCertReqInfo.pucEMail) && (stUserCertReqInfo.usEMailLen > 0))
   {
       pstUserCertReqInfo->usEMailLen = stUserCertReqInfo.usEMailLen;
       
       pstUserCertReqInfo->pucEMail = WE_MALLOC((pstUserCertReqInfo->usEMailLen)*sizeof(WE_CHAR));
       if (NULL == pstUserCertReqInfo->pucEMail) 
       {
           if (pstUserCertReqInfo->pucName != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucName);
           }
           WE_FREE(pstUserCertReqInfo);
           return ENOMEMORY;
       }
       (WE_VOID)WE_MEMCPY(pstUserCertReqInfo->pucEMail, stUserCertReqInfo.pucEMail, pstUserCertReqInfo->usEMailLen);
   }

   if((NULL != stUserCertReqInfo.pucDepartment) && (stUserCertReqInfo.usDepartmentLen > 0))
   {
       pstUserCertReqInfo->usDepartmentLen = stUserCertReqInfo.usDepartmentLen;
       
       pstUserCertReqInfo->pucDepartment = WE_MALLOC((pstUserCertReqInfo->usDepartmentLen)*sizeof(WE_CHAR));
       if (NULL == pstUserCertReqInfo->pucDepartment) 
       {
           if (pstUserCertReqInfo->pucName != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucName);
           }
           if (pstUserCertReqInfo->pucEMail != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucEMail);
           }
           WE_FREE(pstUserCertReqInfo);
           return ENOMEMORY;
       }
       (WE_VOID)WE_MEMCPY(pstUserCertReqInfo->pucDepartment, stUserCertReqInfo.pucDepartment, 
                       pstUserCertReqInfo->usDepartmentLen);
   }

   if((NULL != stUserCertReqInfo.pucCompany) && (stUserCertReqInfo.usCompanyLen > 0))
   {
       pstUserCertReqInfo->usCompanyLen = stUserCertReqInfo.usCompanyLen;
       
       pstUserCertReqInfo->pucCompany = WE_MALLOC((pstUserCertReqInfo->usCompanyLen)*sizeof(WE_CHAR));
       if (NULL == pstUserCertReqInfo->pucCompany) 
       {
           if (pstUserCertReqInfo->pucName != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucName);
           }
           if (pstUserCertReqInfo->pucEMail != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucEMail);
           }
           if (pstUserCertReqInfo->pucDepartment != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucDepartment);
           }
           WE_FREE(pstUserCertReqInfo);
           return ENOMEMORY;
       }
       (WE_VOID)WE_MEMCPY(pstUserCertReqInfo->pucCompany, stUserCertReqInfo.pucCompany, 
                       pstUserCertReqInfo->usCompanyLen);
   }

   if((NULL != stUserCertReqInfo.pucCity) && (stUserCertReqInfo.usCityLen > 0))
   {
       pstUserCertReqInfo->usCityLen = stUserCertReqInfo.usCityLen;
       
       pstUserCertReqInfo->pucCity = WE_MALLOC((pstUserCertReqInfo->usCityLen)*sizeof(WE_CHAR));
       if (NULL == pstUserCertReqInfo->pucCity) 
       {
           if (pstUserCertReqInfo->pucName != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucName);
           }
           if (pstUserCertReqInfo->pucEMail != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucEMail);
           }
           if (pstUserCertReqInfo->pucDepartment != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucDepartment);
           }
           if (pstUserCertReqInfo->pucCompany != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucCompany);
           }
           WE_FREE(pstUserCertReqInfo);
           return ENOMEMORY;
       }
       (WE_VOID)WE_MEMCPY(pstUserCertReqInfo->pucCity, stUserCertReqInfo.pucCity, 
                       pstUserCertReqInfo->usCityLen);
   }

   if((NULL != stUserCertReqInfo.pucProvince) && (stUserCertReqInfo.usProvinceLen > 0))
   {
       pstUserCertReqInfo->usProvinceLen = stUserCertReqInfo.usProvinceLen;
       
       pstUserCertReqInfo->pucProvince = WE_MALLOC((pstUserCertReqInfo->usProvinceLen)*sizeof(WE_CHAR));
       if (NULL == pstUserCertReqInfo->pucProvince) 
       {
           if (pstUserCertReqInfo->pucName != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucName);
           }
           if (pstUserCertReqInfo->pucEMail != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucEMail);
           }
           if (pstUserCertReqInfo->pucDepartment != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucDepartment);
           }
           if (pstUserCertReqInfo->pucCompany != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucCompany);
           }
           if (pstUserCertReqInfo->pucCity != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucCity);
           }
           WE_FREE(pstUserCertReqInfo);
           return ENOMEMORY;
       }
       (WE_VOID)WE_MEMCPY(pstUserCertReqInfo->pucProvince, stUserCertReqInfo.pucProvince, 
                       pstUserCertReqInfo->usProvinceLen);
   }

   if((NULL != stUserCertReqInfo.pucCountry) && (stUserCertReqInfo.usCountryLen > 0))
   {
       pstUserCertReqInfo->usCountryLen = stUserCertReqInfo.usCountryLen;
       
       pstUserCertReqInfo->pucCountry = WE_MALLOC((pstUserCertReqInfo->usCountryLen)*sizeof(WE_CHAR));
       if (NULL == pstUserCertReqInfo->pucCountry) 
       {
           if (pstUserCertReqInfo->pucName != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucName);
           }
           if (pstUserCertReqInfo->pucEMail != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucEMail);
           }
           if (pstUserCertReqInfo->pucDepartment != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucDepartment);
           }
           if (pstUserCertReqInfo->pucCompany != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucCompany);
           }
           if (pstUserCertReqInfo->pucCity != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucCity);
           }
           if (pstUserCertReqInfo->pucProvince != NULL)
           {
               WE_FREE(pstUserCertReqInfo->pucProvince);
           }
           WE_FREE(pstUserCertReqInfo);
           return ENOMEMORY;
       }
       (WE_VOID)WE_MEMCPY(pstUserCertReqInfo->pucCountry, stUserCertReqInfo.pucCountry, 
                       pstUserCertReqInfo->usCountryLen);
   }
    
   iRes = Sec_PostMsg((WE_HANDLE)pMe, (WE_INT32)E_SEC_MSG_USERCERTREQ, (WE_VOID*)pstUserCertReqInfo);
   if (iRes != M_SEC_ERR_OK)
   {
       WE_SIGNAL_DESTRUCT(0, (WE_INT32)M_SEC_DP_MSG_REQ_USER_CERT, (void*)pstUserCertReqInfo);
       return ENOMEMORY;
   }

   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);

   return AEE_SUCCESS;
}

#ifdef G_SEC_CFG_SHOW_PIN
/*==================================================================================================
FUNCTION: 
    SecB_EvtChangePinAction
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    feedback about changing pin to sec module.
ARGUMENTS PASSED:
    ISecB *        pMe[IN]:pointer of ISecB instance.
    St_ChangePinAction  stChangePin[IN]:The content of the change pin.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
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
static WE_UINT32 SecB_EvtChangePinAction(ISecB * pMe,WE_INT32 iTargetID,St_ChangePinAction stChangePin)
{
   St_ChangePinAction stChangeP = {0};
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return EBADPARM;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_EvtPinAction\n"));
    
   stChangeP.bIsOk = stChangePin.bIsOk;
    
   if(NULL != stChangePin.pcNewPinAgainValue)
   {
       stChangeP.pcNewPinAgainValue = SEC_STRDUP(stChangePin.pcNewPinAgainValue);
       if(NULL == stChangeP.pcNewPinAgainValue)
       {
           return ENOMEMORY;
       }
   }
   if(NULL != stChangePin.pcNewPinValue)
   {
       stChangeP.pcNewPinValue = SEC_STRDUP(stChangePin.pcNewPinValue);
       if(NULL == stChangeP.pcNewPinValue)
       {
           return ENOMEMORY;
       }
   }
   if(NULL != stChangePin.pcOldPinValue)
   {
       stChangeP.pcOldPinValue = SEC_STRDUP(stChangePin.pcOldPinValue);
       if(NULL == stChangeP.pcOldPinValue)
       {
           return ENOMEMORY;
       }       
   }
    
   iResult = Sec_UeChangePinAction((WE_HANDLE)pMe,iTargetID,stChangeP);
        
   if(M_SEC_ERR_OK != iResult)
   {
       WE_FREE(stChangeP.pcNewPinAgainValue);
       WE_FREE(stChangeP.pcNewPinValue);
       WE_FREE(stChangeP.pcOldPinValue);
       return EFAILED;  
   }
    
   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);
    
   return AEE_SUCCESS;
}

/*==================================================================================================
FUNCTION: 
    SecB_EvtCreatePinAction
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    when pin is generated, this interface should be invoked.
ARGUMENTS PASSED:
    ISecB *               pMe[IN]:pointer of ISecB instance.
    St_CreatePinAction    stGenPin[IN]:The value of the GenPin
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
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
static WE_UINT32 SecB_EvtCreatePinAction(ISecB * pMe,WE_INT32 iTargetID,St_CreatePinAction stGenPin)
{
   St_CreatePinAction stGenP = {0};
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return EBADPARM;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_EvtCreatePinAction\n"));
    
   stGenP.bIsOk = stGenPin.bIsOk;

   if(NULL != stGenPin.pcPinValue)
   {
       stGenP.pcPinValue = SEC_STRDUP(stGenPin.pcPinValue);
       if(NULL == stGenP.pcPinValue)
       {
           return ENOMEMORY;
       }
   }
    
   iResult = Sec_UeCreatePinAction((WE_HANDLE)pMe,iTargetID,stGenP);
        
   if(M_SEC_ERR_OK != iResult)
   {
       if(NULL != stGenPin.pcPinValue)
       {
           WE_FREE(stGenPin.pcPinValue);
       }
       return EFAILED; 
   }
    
   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);
    
   return AEE_SUCCESS;
}

#endif

/*==================================================================================================
FUNCTION: 
    SecB_EvtHashAction
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    this interface is used to input hash value for verifying when trusted ca info is saved.
ARGUMENTS PASSED:
    ISecB *        pMe[IN]:pointer of ISecB instance.
    St_HashAction   stHash[IN]:The hash action.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
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
static WE_UINT32 SecB_EvtHashAction(ISecB * pMe,WE_INT32 iTargetID,St_HashAction stHash)
{
   St_HashAction stHashVal = {0};
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return EBADPARM;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_EvtHashAction\n"));
   
   stHashVal.bIsOk = stHash.bIsOk;
    
   if(NULL != stHash.pcHashValue)
   {
       stHashVal.pcHashValue = SEC_STRDUP(stHash.pcHashValue);
       if(NULL == stHashVal.pcHashValue)
       {
           return ENOMEMORY;
       }
   }
    
   iResult = Sec_MsgEvtHashAction((WE_HANDLE)pMe,iTargetID,stHashVal);
        
   if(M_SEC_ERR_OK != iResult)
   {
       if(NULL != stHashVal.pcHashValue)
       {
           WE_FREE(stHashVal.pcHashValue);
       }
       return EFAILED;  
   }
    
   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);
    
   return AEE_SUCCESS;
}
/*==================================================================================================
FUNCTION: 
    SecB_EvtShowCertContentAction
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    after detailed information about one certificate is showed, this interface can be invoked
    to do further operation, for example, delete certificate.
ARGUMENTS PASSED:
   ISecB *        pMe[IN]:pointer of ISecB instance.
   St_ShowCertContentAction stShowCer[IN]:Structure of the show cert action.
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
static WE_UINT32 SecB_EvtShowCertContentAction(ISecB * pMe,WE_INT32 iTargetID,St_ShowCertContentAction stShowCer )
{
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return EBADPARM;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_EvtShowCertContentAction\n"));
    
   iResult = Sec_MsgEvtShowCertsContentAction((WE_HANDLE)pMe,iTargetID, stShowCer);
        
   if(M_SEC_ERR_OK != iResult)
   {
       return EFAILED; 
   }
    
   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);
    
   return AEE_SUCCESS;
}
/*==================================================================================================
FUNCTION: 
    SecB_EvtShowCertListAction
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    after cert name list is transferred to client, this interface can be invoked to 
    view detailed information about certificate.
ARGUMENTS PASSED:
    ISecB *        pMe[IN]:pointer of ISecB instance.
    St_ShowCertListAction   stShowCertName[IN]:The structure of the show cert name.
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
static WE_UINT32 SecB_EvtShowCertListAction(ISecB * pMe,WE_INT32 iTargetID,St_ShowCertListAction stShowCertName)
{
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return EBADPARM;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_EvtShowCertListAction\n"));
    
   iResult = Sec_MsgEvtShowCertListAction((WE_HANDLE)pMe,iTargetID, stShowCertName);
        
   if(M_SEC_ERR_OK != iResult)
   {
       return EFAILED;  
   }
    
   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);
    
   return AEE_SUCCESS;
}
/*==================================================================================================
FUNCTION: 
    SecB_EvtShowContractsListAction
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    after getting the contract list, this interface should be run to give a feedback to sec
    module, for example, whether to view one contract information or not.
ARGUMENTS PASSED:
    ISecB *        pMe[IN]:pointer of ISecB instance.
    St_StoredContractsAction    stStoredContracts[IN]:The structure of the stored contracts.
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
static WE_UINT32 SecB_EvtShowContractsListAction(ISecB * pMe,WE_INT32 iTargetId,St_ShowContractsListAction stStoredContracts)
{
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return EBADPARM;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_EvtShowContractsListAction\n"));
    
   iResult = Sec_MsgEvtShowContractsListAction((WE_HANDLE)pMe,iTargetId, stStoredContracts);
        
   if(M_SEC_ERR_OK != iResult)
   {
       return EFAILED; 
   }
        
   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);
    
   return AEE_SUCCESS;
}
/*==================================================================================================
FUNCTION: 
    SecB_EvtStoreCertAction
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    before one certificate is saved, this interface should be invoked to give some information
    to sec module about name of certificate.
ARGUMENTS PASSED:
    ISecB *        pMe[IN]:pointer of ISecB instance.
    St_StoreCertAction  stStoreCert[IN]:The structure of the store cert.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
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
static WE_UINT32 SecB_EvtStoreCertAction(ISecB * pMe,WE_INT32 iTargetID,St_StoreCertAction stStoreCert)
{
   St_StoreCertAction stStoreCe = {0};
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return EBADPARM;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_EvtStoreCertAction\n"));
    
   stStoreCe.bIsOk = stStoreCert.bIsOk;

   if(NULL != stStoreCert.pcText)
   {
       stStoreCe.pcText = SEC_STRDUP(stStoreCert.pcText);
       if(NULL == stStoreCe.pcText)
       {
           return ENOMEMORY;
       }
   }
    
   iResult = Sec_MsgEvtStoreCertAction((WE_HANDLE)pMe,iTargetID,stStoreCe);
        
   if(M_SEC_ERR_OK != iResult)
   {
       WE_FREE(stStoreCe.pcText);
       return EFAILED;  
   }
    
   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);
    
   return AEE_SUCCESS;
}
/*==================================================================================================
FUNCTION: 
    SecB_EvtShowContractContentAction
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    after one contract information is showed, this function should be invoked to give 
    feedback to sec module.
ARGUMENTS PASSED:
    ISecB *        pMe[IN]:pointer of ISecB instance.
    St_ShowContractContentAction    stShowContract[IN]:The structure of the show contract.
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
static WE_UINT32 SecB_EvtShowContractContentAction(ISecB * pMe,WE_INT32 iTargetID,
                                                   St_ShowContractContentAction stShowContract)
{
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return EBADPARM;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_EvtShowContractContentAction\n"));
        
   iResult = Sec_MsgEvtShowContractsContentAction((WE_HANDLE)pMe,iTargetID,stShowContract);
        
   if(M_SEC_ERR_OK != iResult)
   {
       return EFAILED;  
   }
    
   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);
    
   return AEE_SUCCESS;
}
/*==================================================================================================
FUNCTION: 
    RegSecClientEvtCB
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    register one callback for sec module, and through the callback, sec module can
    return some information to the invoker.
ARGUMENTS PASSED:
    ISecB *      pMe[IN]: pointer about an instance of ISec   
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
static WE_UINT32 SecB_RegClientEvtCB(ISecB * pMe,WE_HANDLE hPrivData,Fn_ISecEventHandle pcbSecEvtFunc)
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
    Sec_EvtConfirmAction
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
static WE_UINT32 SecB_EvtConfirmAction(ISecB * pMe,WE_INT32 iTargetID,St_ConfirmAction stConfirm)
{
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return EBADPARM;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"Sec_EvtConfirmAction\n"));
    
   iResult = Sec_MsgEvtConfirmAction((WE_HANDLE)pMe,iTargetID, stConfirm);
        
   if(M_SEC_ERR_OK != iResult)
   {
       return EFAILED;  
   }
    
   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);    
   return AEE_SUCCESS;
}
/*==================================================================================================
FUNCTION: 
    Sec_EvtNameConfirmAction
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    when a confirm information is receive , the interface should be run to give the sec 
    module a feedback.
ARGUMENTS PASSED:
    ISecB *                 pMe[IN]:pointer of ISecB instance.
    St_NameConfirmAction    stConfName[IN]:The name of the confirm.
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
static WE_UINT32 SecB_EvtNameConfirmAction(ISecB * pMe,WE_INT32 iTargetID,St_NameConfirmAction stConfName)
{
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return EBADPARM;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"Sec_EvtNameConfirmAction\n"));
    
   iResult = Sec_MsgEvtNameConfirmAction((WE_HANDLE)pMe,iTargetID, stConfName);
        
   if(M_SEC_ERR_OK != iResult)
   {
       return EFAILED; 
   }
    
   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);    
   return AEE_SUCCESS;
}

#ifdef G_SEC_CFG_SHOW_PIN
/*==================================================================================================
FUNCTION: 
    Sec_EvtPinAction
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    when a event is sent to invoker to ask a pin code to do specific something, this interface
    should be invoked to give a feedback to sec module.
ARGUMENTS PASSED:
    ISecB *         pMe[IN]:pointer of ISecB instance.
    St_PinAction    stPin[IN]:The value of the pin structure.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
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
static WE_UINT32 SecB_EvtPinAction(ISecB * pMe,WE_INT32 iTargetID,St_PinAction stPin)
{
   St_PinAction stP = {0};
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return EBADPARM;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_EvtPinAction\n"));

    
   stP.bIsOk = stPin.bIsOk;
    
   if(NULL != stPin.pcPinValue)
   {
       stP.pcPinValue = SEC_STRDUP(stPin.pcPinValue);
       if(NULL == stP.pcPinValue)
       {
           return ENOMEMORY;
       }
   }
    
   iResult = Sec_UePinAction((WE_HANDLE)pMe,iTargetID, stP);
        
   if(M_SEC_ERR_OK != iResult)
   {
        WE_FREE(stP.pcPinValue);
         return EFAILED;  
   }
   
   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);
    
   return AEE_SUCCESS;
}
#endif

/*==================================================================================================
FUNCTION: 
    ISecB_New
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    create one instance of ISecB interface.
ARGUMENTS PASSED:
    WE_UINT16         sSize[IN]:size of ISecB.
    IShell *        pIShell[IN]:Pointer to the IShell interface.
    IModule*        pIModule[IN]:Pointer to the module.
    ISecB **        ppMod[IN]:Pointer to mod.
RETURN VALUE:
    EBADPARM,
    ENOMEMORY,
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
WE_INT32 ISecB_New(WE_UINT16 sSize, IShell *pIShell, IModule* pIModule, ISecB ** ppMod)
{
   ISecB * pMe = NULL;
   VTBL(ISecB) * modFuncs = NULL;
    
   if( !ppMod || !pIShell || !pIModule )
   {
       return EFAILED;
   }    
   *ppMod = NULL;
   /* Allocate memory for the ExtensionCls object*/
   if( sSize < (WE_UINT16)sizeof(ISecB) )
   {
       sSize += (WE_UINT16)sizeof(ISecB);
   }
   /* Allocate the module's struct and initialize it. Note that the 
      modules and apps must not have any static data. 
      Hence, we need to allocate the vtbl as well.*/
    
   if((pMe = (ISecB*)MALLOC((WE_ULONG)(sSize))) == NULL )
   {
      return ENOMEMORY;
   }        
   pMe->pvt = NULL;

   /*Initialize individual entries in the VTBL*/
   if(0 == ((SecMod*)pIModule)->uiSecBrsRefs)
   {
       /*at first malloc vtbl*/
        
       if(NULL == (((SecMod*)pIModule)->pvSecBrsVtbl = 
           (void*)MALLOC(sizeof(VTBL(ISecB)))))
       {
           FREE(pMe);
           return ENOMEMORY;
       }
       modFuncs = (VTBL(ISecB)*)(((SecMod*)pIModule)->pvSecBrsVtbl);

       modFuncs->AddRef = SecB_AddRef;
       modFuncs->Release = SecB_Release;
        
       modFuncs->RegSecClientEvtCB = SecB_RegClientEvtCB;
       
       modFuncs->GetCertNameList = SecB_GetCertNameList;        
       modFuncs->GenKeyPair = SecB_GenKeyPair;
       modFuncs->GetCurSvrCert = SecB_GetCurSvrCert;
       modFuncs->GetSessionInfo = SecB_GetSessionInfo;
       modFuncs->GetWtlsCurClasss = SecB_GetWtlsCurClasss;
       modFuncs->GetContractList = SecB_GetContractsList;
        
       modFuncs->TransferCert = SecB_TransferCert;

       modFuncs->GetRequestUserCert = SecB_GetRequestUserCert;
       
       modFuncs->EvtConfirmAction = SecB_EvtConfirmAction;
       modFuncs->EvtNameConfirmAction = SecB_EvtNameConfirmAction;
       modFuncs->EvtHashAction = SecB_EvtHashAction;
       modFuncs->EvtStoreCertAction = SecB_EvtStoreCertAction;
       modFuncs->EvtShowCertListAction = SecB_EvtShowCertListAction;
       modFuncs->EvtShowContractsListAction = SecB_EvtShowContractsListAction;
       modFuncs->EvtShowCertContentAction = SecB_EvtShowCertContentAction;
       modFuncs->EvtShowContractContentAction = SecB_EvtShowContractContentAction;
   } 
    
   if(NULL == (modFuncs = (VTBL(ISecB)*)(((SecMod*)pIModule)->pvSecBrsVtbl)))
   {
       FREE(((SecMod*)pIModule)->pvSecBrsVtbl);
       FREE(pMe);
       return EFAILED;
   }
   /* initialize the vtable*/
   INIT_VTBL(pMe, ISecB, *modFuncs);
        
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
   pMe->iIFType = (WE_INT32)E_SEC_BRS;
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
   *ppMod = (ISecB*)pMe;

   /*brs vtbl refernce add*/
   ((SecMod*)pIModule)->uiSecBrsRefs++;

   return AEE_SUCCESS;
}

