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
#include "oem_secmgr.h"
#include "isecb.h"

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
#define SEC_IAEECALLBACK_VAR    (((St_PublicData *)(((ISec*)pMe)->hPrivateData))->m_cb)
#define SEC_IISHELL_POINTER     (((ISec*)pMe)->m_pIShell)
#define SEC_PRIVATE_DATA        (((ISec*)pMe)->hPrivateData)
#define SEC_SIINFO              (((St_PublicData *)(((ISec*)pMe)->hPrivateData))->pstInfo)
#define SEC_PEER_CACHE          (((St_PublicData *)(((ISec*)pMe)->hPrivateData))->pstPeerCache)
#define SEC_SESSION_CACHE       (((St_PublicData *)(((ISec*)pMe)->hPrivateData))->pstSessionCache)
#define SEC_WIM_HAVELOADUSERPRIV        (((St_PublicData *)(((WE_HANDLE)pMe)->hPrivateData))->pcHaveLoad)

/*******************************************************************************
*   Function Define Section
*******************************************************************************/

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
    WE_HANDLE     pMe[IN]:pointer of ISec instance.
    WE_INT32    iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_INT32    iMasterSecretId:ID of the master secret.
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
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
WE_UINT32 SecB_GetCertNameList(WE_HANDLE pMe, WE_INT32 iTargetID, WE_INT32 iCertType)
{
   WE_INT32 iResult = 0;

   if(NULL == pMe)
   {
       return G_SEC_INVALID_PARAMETER;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"Sec_GetCertNames\n"));
    
   iResult = Sec_MsgGetCertNameList((WE_HANDLE)pMe,iTargetID,iCertType);
    
   if(M_SEC_ERR_OK != iResult)
   {
       return G_SEC_NOTENOUGH_MEMORY; 
   }  

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
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
    WE_HANDLE     pMe[IN]:pointer of ISec instance.
    WE_INT32    iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_UINT8    ucKeyType[IN]:The value of the key type.
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
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
WE_UINT32 SecB_GenKeyPair(WE_HANDLE pMe, WE_INT32 iTargetID, WE_UINT8 ucKeyType)
{
   WE_INT32 iResult = 0;
    
   if(NULL == pMe)
   {
       return G_SEC_INVALID_PARAMETER;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_GenKeyPair\n"));

   iResult = Sec_MsgKeyPairGen((WE_HANDLE)pMe,iTargetID,ucKeyType,M_SEC_SP_RSA);
    
   if(M_SEC_ERR_OK != iResult)
   {
       return G_SEC_NOTENOUGH_MEMORY; 
   }    

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
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
    WE_HANDLE         pMe[IN]:pointer of ISec instance.
    WE_INT32        iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_UINT8        ucKeyType[IN]:The value of the key type.
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
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
WE_UINT32 SecB_PinModify(WE_HANDLE pMe, WE_INT32 iTargetID, WE_UINT8 ucKeyType)
{
   WE_INT32 iResult = 0;

   if(NULL == pMe)
   {
       return G_SEC_INVALID_PARAMETER;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_PinModify\n"));    

   iResult = Sec_MsgModifyPin((WE_HANDLE)pMe,iTargetID,ucKeyType);
    
   if(M_SEC_ERR_OK != iResult)
   {
       return G_SEC_NOTENOUGH_MEMORY;  
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
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
    WE_HANDLE         pMe[IN]:pointer of ISec instance.
    WE_INT32        iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_INT32        iSecID[IN]:ID of the security.
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
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
WE_UINT32 SecB_GetCurSvrCert(WE_HANDLE pMe, WE_INT32 iTargetID, WE_INT32 iSecID)
{
   WE_INT32 iResult = 0;

   if(NULL == pMe)
   {
       return G_SEC_INVALID_PARAMETER;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_GetCurSvrCert\n"));
    
   iResult = Sec_MsgViewCurCert((WE_HANDLE)pMe,iTargetID,iSecID);
    
   if(M_SEC_ERR_OK != iResult)
   {
       return G_SEC_NOTENOUGH_MEMORY; 
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
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
    WE_HANDLE     pMe[IN]:pointer of ISec instance.
    WE_INT32    iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_INT32    iSecID[IN]:ID of the security.
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
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
WE_UINT32 SecB_GetSessionInfo(WE_HANDLE pMe, WE_INT32 iTargetID, WE_INT32 iSecID)
{
   WE_INT32 iResult = 0;

   if(NULL == pMe)
   {
       return G_SEC_INVALID_PARAMETER;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_GetSessionInfo\n"));
    
   iResult = Sec_MsgViewInfoSession((WE_HANDLE)pMe,iTargetID,iSecID);
    
   if(M_SEC_ERR_OK != iResult)
   {
       return G_SEC_NOTENOUGH_MEMORY;  
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
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
    WE_HANDLE     pMe[IN]:pointer of ISec instance.
    WE_INT32    iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_INT32    iSecID[IN]:ID of the security.
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
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
WE_UINT32 SecB_GetWtlsCurClasss(WE_HANDLE pMe, WE_INT32 iTargetID, WE_INT32 iSecID)
{
   WE_INT32 iResult = 0;

   if(NULL == pMe)
   {
       return G_SEC_INVALID_PARAMETER;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_GetWtlsCurClasss\n"));
    
   iResult = Sec_MsgGetSecCurClass((WE_HANDLE)pMe,iTargetID,iSecID);
    
   if(M_SEC_ERR_OK != iResult)
   {
       return G_SEC_NOTENOUGH_MEMORY; 
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
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
    WE_HANDLE     pMe[IN]:pointer of ISec instance.
    WE_INT32    iTargetID[IN]:the identity of the invoker ,input from the interface.
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
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
WE_UINT32 SecB_GetContractsList(WE_HANDLE pMe, WE_INT32 iTargetID)
{
   WE_INT32 iResult = 0;

   if(NULL == pMe)
   {
       return G_SEC_INVALID_PARAMETER;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_GetContractsList\n"));    
  
   iResult = Sec_MsgGetContractsInfos((WE_HANDLE)pMe,iTargetID);
   if(M_SEC_ERR_OK != iResult)
   {
       return G_SEC_NOTENOUGH_MEMORY;  
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
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
    WE_HANDLE              pMe[IN]:pointer of ISec instance.
    St_SecCertContent    stCertContent[IN]:The content of the cert.   
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
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
WE_UINT32 SecB_TransferCert(WE_HANDLE pMe,St_SecCertContent stCertContent)
{
   St_SecCertContent * pstCertContent = NULL;
   WE_INT32 iRes = 0;
    
   if(NULL == pMe)
   {
       return G_SEC_INVALID_PARAMETER;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_TransferCert\n"));
    
   pstCertContent = (St_SecCertContent *)WE_MALLOC((WE_ULONG)sizeof(St_SecCertContent));
   if(NULL == pstCertContent)
   {
       return G_SEC_NOTENOUGH_MEMORY;
   }

   (void)WE_MEMSET(pstCertContent,0,sizeof(St_SecCertContent));
    
   /*MIME*/
   if(NULL == stCertContent.pcMime)
   {
       WE_FREE(pstCertContent);
       return G_SEC_INVALID_PARAMETER;
   }
   else
   {
       pstCertContent->pcMime = SEC_STRDUP(stCertContent.pcMime);
       if(NULL == pstCertContent->pcMime)
       {
           WE_FREE(pstCertContent);
           return G_SEC_NOTENOUGH_MEMORY;
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
               return G_SEC_NOTENOUGH_MEMORY;
           }
           (void)WE_MEMCPY(pstCertContent->pucCertData,stCertContent.pucCertData,pstCertContent->uiCertLen);
           pstCertContent->pucCertData[pstCertContent->uiCertLen] = '\0';
   }
   else
   {
       WE_FREE(pstCertContent->pcMime);
       WE_FREE(pstCertContent);
       return G_SEC_INVALID_PARAMETER;
   }
    
   iRes = Sec_PostMsg((WE_HANDLE)pMe, (WE_INT32)E_SEC_MSG_CERTTRANSFER, (void*)pstCertContent);
   if (iRes != M_SEC_ERR_OK)
   {
       WE_SIGNAL_DESTRUCT(0, (WE_INT32)M_SEC_DP_MSG_EXECUTE_COMMAND, (void*)pstCertContent);
       return G_SEC_NOTENOUGH_MEMORY;
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
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
    WE_HANDLE                 pMe[IN]:pointer of ISec instance.
    St_SecUserCertReqInfo   stUserCertReqInfo[IN]:The infomation of request about user cert.   
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
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
WE_UINT32 SecB_GetRequestUserCert(WE_HANDLE pMe, St_SecUserCertReqInfo stUserCertReqInfo)
{
   St_SecUserCertReqInfo * pstUserCertReqInfo = NULL;
   WE_INT32 iRes = 0;
    
   if(NULL == pMe)
   {
       return G_SEC_INVALID_PARAMETER;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_GetRequestUserCert\n"));
   
   pstUserCertReqInfo = (St_SecUserCertReqInfo *)WE_MALLOC((WE_ULONG)sizeof(St_SecUserCertReqInfo));
   if(NULL == pstUserCertReqInfo)
   {
       return G_SEC_NOTENOUGH_MEMORY;
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
           return G_SEC_NOTENOUGH_MEMORY;
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
           return G_SEC_NOTENOUGH_MEMORY;
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
           return G_SEC_NOTENOUGH_MEMORY;
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
           return G_SEC_NOTENOUGH_MEMORY;
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
           return G_SEC_NOTENOUGH_MEMORY;
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
           return G_SEC_NOTENOUGH_MEMORY;
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
           return G_SEC_NOTENOUGH_MEMORY;
       }
       (WE_VOID)WE_MEMCPY(pstUserCertReqInfo->pucCountry, stUserCertReqInfo.pucCountry, 
                       pstUserCertReqInfo->usCountryLen);
   }
    
   iRes = Sec_PostMsg((WE_HANDLE)pMe, (WE_INT32)E_SEC_MSG_USERCERTREQ, (WE_VOID*)pstUserCertReqInfo);
   if (iRes != M_SEC_ERR_OK)
   {
       WE_SIGNAL_DESTRUCT(0, (WE_INT32)M_SEC_DP_MSG_REQ_USER_CERT, (void*)pstUserCertReqInfo);
       return G_SEC_NOTENOUGH_MEMORY;
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
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
    WE_HANDLE        pMe[IN]:pointer of ISec instance.
    St_ChangePinAction  stChangePin[IN]:The content of the change pin.
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
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
WE_UINT32 SecB_EvtChangePinAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_ChangePinAction stChangePin)
{
   St_ChangePinAction stChangeP = {0};
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return G_SEC_INVALID_PARAMETER;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_EvtPinAction\n"));
    
   stChangeP.bIsOk = stChangePin.bIsOk;
    
   if(NULL != stChangePin.pcNewPinAgainValue)
   {
       stChangeP.pcNewPinAgainValue = SEC_STRDUP(stChangePin.pcNewPinAgainValue);
       if(NULL == stChangeP.pcNewPinAgainValue)
       {
           return G_SEC_NOTENOUGH_MEMORY;
       }
   }
   if(NULL != stChangePin.pcNewPinValue)
   {
       stChangeP.pcNewPinValue = SEC_STRDUP(stChangePin.pcNewPinValue);
       if(NULL == stChangeP.pcNewPinValue)
       {
           return G_SEC_NOTENOUGH_MEMORY;
       }
   }
   if(NULL != stChangePin.pcOldPinValue)
   {
       stChangeP.pcOldPinValue = SEC_STRDUP(stChangePin.pcOldPinValue);
       if(NULL == stChangeP.pcOldPinValue)
       {
           return G_SEC_NOTENOUGH_MEMORY;
       }       
   }
    
   iResult = Sec_UeChangePinAction((WE_HANDLE)pMe,iTargetID,stChangeP);
        
   if(M_SEC_ERR_OK != iResult)
   {
       WE_FREE(stChangeP.pcNewPinAgainValue);
       WE_FREE(stChangeP.pcNewPinValue);
       WE_FREE(stChangeP.pcOldPinValue);
       return G_SEC_GENERAL_ERROR;  
   }

   Sec_RunMainFlow(pMe);    
   return G_SEC_OK;
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
    WE_HANDLE               pMe[IN]:pointer of ISec instance.
    St_CreatePinAction    stGenPin[IN]:The value of the GenPin
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
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
WE_UINT32 SecB_EvtCreatePinAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_CreatePinAction stGenPin)
{
   St_CreatePinAction stGenP = {0};
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return G_SEC_INVALID_PARAMETER;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_EvtCreatePinAction\n"));
    
   stGenP.bIsOk = stGenPin.bIsOk;

   if(NULL != stGenPin.pcPinValue)
   {
       stGenP.pcPinValue = SEC_STRDUP(stGenPin.pcPinValue);
       if(NULL == stGenP.pcPinValue)
       {
           return G_SEC_NOTENOUGH_MEMORY;
       }
   }
    
   iResult = Sec_UeCreatePinAction((WE_HANDLE)pMe,iTargetID,stGenP);
        
   if(M_SEC_ERR_OK != iResult)
   {
       if(NULL != stGenPin.pcPinValue)
       {
           WE_FREE(stGenPin.pcPinValue);
       }
       return G_SEC_GENERAL_ERROR; 
   }    

   Sec_RunMainFlow(pMe);    
   return G_SEC_OK;
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
    WE_HANDLE        pMe[IN]:pointer of ISec instance.
    St_HashAction   stHash[IN]:The hash action.
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
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
WE_UINT32 SecB_EvtHashAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_HashAction stHash)
{
   St_HashAction stHashVal = {0};
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return G_SEC_INVALID_PARAMETER;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_EvtHashAction\n"));
   
   stHashVal.bIsOk = stHash.bIsOk;
    
   if(NULL != stHash.pcHashValue)
   {
       stHashVal.pcHashValue = SEC_STRDUP(stHash.pcHashValue);
       if(NULL == stHashVal.pcHashValue)
       {
           return G_SEC_NOTENOUGH_MEMORY;
       }
   }
    
   iResult = Sec_MsgEvtHashAction((WE_HANDLE)pMe,iTargetID,stHashVal);
        
   if(M_SEC_ERR_OK != iResult)
   {
       if(NULL != stHashVal.pcHashValue)
       {
           WE_FREE(stHashVal.pcHashValue);
       }
       return G_SEC_GENERAL_ERROR;  
   }    

   Sec_RunMainFlow(pMe);    
   return G_SEC_OK;
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
   WE_HANDLE        pMe[IN]:pointer of ISec instance.
   St_ShowCertContentAction stShowCer[IN]:Structure of the show cert action.
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
WE_UINT32 SecB_EvtShowCertContentAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_ShowCertContentAction stShowCer )
{
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return G_SEC_INVALID_PARAMETER;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_EvtShowCertContentAction\n"));
    
   iResult = Sec_MsgEvtShowCertsContentAction((WE_HANDLE)pMe,iTargetID, stShowCer);
        
   if(M_SEC_ERR_OK != iResult)
   {
       return G_SEC_GENERAL_ERROR; 
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
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
    WE_HANDLE        pMe[IN]:pointer of ISec instance.
    St_ShowCertListAction   stShowCertName[IN]:The structure of the show cert name.
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
WE_UINT32 SecB_EvtShowCertListAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_ShowCertListAction stShowCertName)
{
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return G_SEC_INVALID_PARAMETER;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_EvtShowCertListAction\n"));
    
   iResult = Sec_MsgEvtShowCertListAction((WE_HANDLE)pMe,iTargetID, stShowCertName);
        
   if(M_SEC_ERR_OK != iResult)
   {
       return G_SEC_GENERAL_ERROR;  
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
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
    WE_HANDLE        pMe[IN]:pointer of ISec instance.
    St_StoredContractsAction    stStoredContracts[IN]:The structure of the stored contracts.
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
WE_UINT32 SecB_EvtShowContractsListAction(WE_HANDLE pMe,WE_INT32 iTargetId,St_ShowContractsListAction stStoredContracts)
{
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return G_SEC_INVALID_PARAMETER;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_EvtShowContractsListAction\n"));
    
   iResult = Sec_MsgEvtShowContractsListAction((WE_HANDLE)pMe,iTargetId, stStoredContracts);
        
   if(M_SEC_ERR_OK != iResult)
   {
       return G_SEC_GENERAL_ERROR; 
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
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
    WE_HANDLE        pMe[IN]:pointer of ISec instance.
    St_StoreCertAction  stStoreCert[IN]:The structure of the store cert.
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
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
WE_UINT32 SecB_EvtStoreCertAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_StoreCertAction stStoreCert)
{
   St_StoreCertAction stStoreCe = {0};
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return G_SEC_INVALID_PARAMETER;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_EvtStoreCertAction\n"));
    
   stStoreCe.bIsOk = stStoreCert.bIsOk;

   if(NULL != stStoreCert.pcText)
   {
       stStoreCe.pcText = SEC_STRDUP(stStoreCert.pcText);
       if(NULL == stStoreCe.pcText)
       {
           return G_SEC_NOTENOUGH_MEMORY;
       }
   }
    
   iResult = Sec_MsgEvtStoreCertAction((WE_HANDLE)pMe,iTargetID,stStoreCe);
        
   if(M_SEC_ERR_OK != iResult)
   {
       WE_FREE(stStoreCe.pcText);
       return G_SEC_GENERAL_ERROR;  
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
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
    WE_HANDLE        pMe[IN]:pointer of ISec instance.
    St_ShowContractContentAction    stShowContract[IN]:The structure of the show contract.
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
WE_UINT32 SecB_EvtShowContractContentAction(WE_HANDLE pMe,WE_INT32 iTargetID,
                                                   St_ShowContractContentAction stShowContract)
{
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return G_SEC_INVALID_PARAMETER;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_EvtShowContractContentAction\n"));
        
   iResult = Sec_MsgEvtShowContractsContentAction((WE_HANDLE)pMe,iTargetID,stShowContract);
        
   if(M_SEC_ERR_OK != iResult)
   {
       return G_SEC_GENERAL_ERROR;  
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
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
    WE_HANDLE             pMe[IN]:pointer of ISec instance.
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
WE_UINT32 SecB_EvtConfirmAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_ConfirmAction stConfirm)
{
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return G_SEC_INVALID_PARAMETER;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"Sec_EvtConfirmAction\n"));
    
   iResult = Sec_MsgEvtConfirmAction((WE_HANDLE)pMe,iTargetID, stConfirm);
        
   if(M_SEC_ERR_OK != iResult)
   {
       return G_SEC_GENERAL_ERROR;  
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
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
    WE_HANDLE                 pMe[IN]:pointer of ISec instance.
    St_NameConfirmAction    stConfName[IN]:The name of the confirm.
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
WE_UINT32 SecB_EvtNameConfirmAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_NameConfirmAction stConfName)
{
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return G_SEC_INVALID_PARAMETER;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"Sec_EvtNameConfirmAction\n"));
    
   iResult = Sec_MsgEvtNameConfirmAction((WE_HANDLE)pMe,iTargetID, stConfName);
        
   if(M_SEC_ERR_OK != iResult)
   {
       return G_SEC_GENERAL_ERROR; 
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
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
    WE_HANDLE         pMe[IN]:pointer of ISec instance.
    St_PinAction    stPin[IN]:The value of the pin structure.
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
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
WE_UINT32 SecB_EvtPinAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_PinAction stPin)
{
   St_PinAction stP = {0};
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return G_SEC_INVALID_PARAMETER;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"SecB_EvtPinAction\n"));

    
   stP.bIsOk = stPin.bIsOk;
    
   if(NULL != stPin.pcPinValue)
   {
       stP.pcPinValue = SEC_STRDUP(stPin.pcPinValue);
       if(NULL == stP.pcPinValue)
       {
           return G_SEC_NOTENOUGH_MEMORY;
       }
   }
    
   iResult = Sec_UePinAction((WE_HANDLE)pMe,iTargetID, stP);
        
   if(M_SEC_ERR_OK != iResult)
   {
        WE_FREE(stP.pcPinValue);
         return G_SEC_GENERAL_ERROR;  
   }

   Sec_RunMainFlow(pMe);    
   return G_SEC_OK;
}
#endif

