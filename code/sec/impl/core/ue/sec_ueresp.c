/*==============================================================================
     FILE NAME :
        sec_ueresp.c
    MODULE NAME :
        sec
    GENERAL DESCRIPTION
    In this file,define the Dialog response.
        
    Techfaith Software Confidential Proprietary(c)
    Copyright 2002-2006 by Techfaith Software. All Rights Reserved.
================================================================================
    Revision History
    
    Modification              Tracking
    Date       Author         Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-07-13 Tang           None      Init

============================================================================== */

 
/************************************************************************/
/*      Include File Section                                                                     */
/************************************************************************/

#include "sec_comm.h"
 
/*****************************************************************************************
        Macro Define Section
******************************************************************************************/
typedef St_PublicData                       St_SecGlobalData;

#define  M_SEC_VAITING_FOR_TI_RESULT         (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->iWaitingForUe)
#define  M_SEC_CURRENT_TI_RESPONSE_VALUES    (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->pvCurUeResp)
#define  M_SEC_WANTS_TO_RUN                  (((St_SecGlobalData *)(((ISec*)hSecHandle)->hPrivateData))->iWantsToRun)

/************************************************************************/
/*      Function Define Section                                                                     */
/************************************************************************/

/*====================================================================================
FUNCTION: 
    Sec_UeConfirmDialogResp  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
     response of Confirm Dialog . 
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:   Global data handle
    ucAnswer[in]:         answer.       
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
=====================================================================================*/
void Sec_UeConfirmDialogResp(WE_HANDLE hSecHandle,WE_UINT8 ucAnswer)
{      
    St_SecTlConfirmDlgResp *pstP={0};
    WE_LOG_MSG ((WE_LOG_DETAIL_LOW, 0, 
                    "SEC: Sec_UeConfirmDialogResp(answer = %u)\n", ucAnswer));
    if(!hSecHandle)
    {
        return ;
    }
    
    if (!(M_SEC_VAITING_FOR_TI_RESULT)) 
    {
        M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
        return;        
    }
    M_SEC_VAITING_FOR_TI_RESULT = 0;
    pstP= ( St_SecTlConfirmDlgResp *)WE_MALLOC(sizeof( St_SecTlConfirmDlgResp));
    if (!pstP)
    {
        M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
        return;
    }
    pstP->eType = E_SEC_DP_CONFIRM_DLG;
    pstP->ucAnswer = ucAnswer;
    M_SEC_CURRENT_TI_RESPONSE_VALUES = ( St_SecTlUeFunctionId *)pstP;
    M_SEC_WANTS_TO_RUN = 1;
    }

/*====================================================================================
FUNCTION: 
    Sec_UeExtConfirmDialogResp  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    response of ExtConfirm Dialog.
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:   Global data handle.
    ucAnswer[IN]:     answer.          
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
=====================================================================================*/
void Sec_UeExtConfirmDialogResp(WE_HANDLE hSecHandle,WE_UINT8 ucAnswer)
{     
    St_SecTlConfirmDlgResp *pstP={0};
    WE_LOG_MSG((WE_LOG_DETAIL_LOW, 0,
               "SEC: Sec_UeExtConfirmDialogResp(answer = %u)\n", ucAnswer));
    if(!hSecHandle)
    {
        return ;
    }

    if (!(M_SEC_VAITING_FOR_TI_RESULT))
    {
        M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_VAITING_FOR_TI_RESULT = 0;
    pstP = WE_MALLOC(sizeof( St_SecTlConfirmDlgResp));
    if (!pstP)
    {
        M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
        return;
    }
    pstP->eType = E_SEC_DP_CONFIRM_DLG;
    pstP->ucAnswer = ucAnswer;
    M_SEC_CURRENT_TI_RESPONSE_VALUES = ( St_SecTlUeFunctionId *)pstP;
    M_SEC_WANTS_TO_RUN = 1;
}

/*====================================================================================
FUNCTION: 
    Sec_UeConfirmDispNameResp  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    response of Confirm Displayed Name.
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:   Global data handle.
    ucAnswer[IN]:     answer.             
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
=====================================================================================*/
void Sec_UeConfirmDispNameResp(WE_HANDLE hSecHandle,WE_UINT8 ucAnswer)
{    
    St_SecTlConfirmDlgResp *pstP={0};
    WE_LOG_MSG((WE_LOG_DETAIL_LOW, 0,
               "Sec_UeConfirmDispNameResp(answer = %u)\n", ucAnswer));
    if(!hSecHandle)
    {
        return ;
    }

    if (!(M_SEC_VAITING_FOR_TI_RESULT))                
    {
        M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_VAITING_FOR_TI_RESULT = 0;
    pstP = ( St_SecTlConfirmDlgResp *)WE_MALLOC(sizeof( St_SecTlConfirmDlgResp));
    if (!pstP)
    {
        M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
        return;
    }
    pstP->eType = E_SEC_DP_CONFIRM_DLG;
    pstP->ucAnswer = ucAnswer;
    M_SEC_CURRENT_TI_RESPONSE_VALUES = ( St_SecTlUeFunctionId *)pstP;
    M_SEC_WANTS_TO_RUN = 1;
}

/*====================================================================================
FUNCTION: 
    Sec_UeChooseCertByNameResp  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    response of Choose Cert By Name  . 
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:   Global data handle.
    iCertId[IN]:    certificate id.        
    eResult[IN]:   result.            
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    none
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
void Sec_UeChooseCertByNameResp(WE_HANDLE hSecHandle, WE_INT32 iCertId, E_SecUeReturnVal eResult)
{
   St_SecTlChooseCertResp *pstP={0};
   WE_LOG_MSG((WE_LOG_DETAIL_LOW, 0, 
              "SEC: Sec_UeChooseCertByNameResp(certId = %d, result = %d)\n", iCertId, eResult));
    if(!hSecHandle)
    {
        return ;
    }

   if (!(M_SEC_VAITING_FOR_TI_RESULT))                
   {
       M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
       return;
   }
   M_SEC_VAITING_FOR_TI_RESULT = 0;
   pstP = ( St_SecTlChooseCertResp *)WE_MALLOC(sizeof( St_SecTlChooseCertResp));
   if (!pstP)
   {
       M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
       return;
   }
   pstP->eType = E_SEC_DP_CHOOSE_CERT;
   pstP->iCertId = iCertId;
   pstP->eResult = eResult;
    
   M_SEC_CURRENT_TI_RESPONSE_VALUES = ( St_SecTlUeFunctionId *)pstP;
   M_SEC_WANTS_TO_RUN = 1;
}
/*====================================================================================
FUNCTION: 
    Sec_UeStoreCertDialogResp  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    response of Store Certificate Dialog .   
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:   Global data handle.
    ucAnswer[IN]:        the value of answer.   
    pvFriendlyName[IN]:     freindly name.
    usFriendlyNameLen[IN]:  the lenght of pvFriendlyName.
    usCharSet[IN]:            charset.
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
=====================================================================================*/
void Sec_UeStoreCertDialogResp(WE_HANDLE hSecHandle,WE_UINT8 ucAnswer, 
                               void *pvFriendlyName, WE_UINT16 usFriendlyNameLen, 
                               WE_INT16 usCharSet)
{
   St_SecTlStoreCertDlgResp *pstP={0};
   WE_INT32 iLoop=0;
   WE_UINT8 *pucQ = (WE_UINT8 *)pvFriendlyName;

   WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,
               "SEC: Sec_UeStoreCertDialogResp(answer = %u, friendlyName = %p, friendlyNameLen = %u, \
               charSet = &d)\n", ucAnswer, pvFriendlyName, usFriendlyNameLen, usCharSet));
    if(!hSecHandle)
    {
        return ;
    }

   if (!(M_SEC_VAITING_FOR_TI_RESULT))                
   {
       M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
       return;
   }
   M_SEC_VAITING_FOR_TI_RESULT = 0;
   pstP = ( St_SecTlStoreCertDlgResp *)WE_MALLOC(sizeof(St_SecTlStoreCertDlgResp));
   if (!pstP)
   {
      M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
      return;
   }
   if (pvFriendlyName)
   {
      pstP->usFriendlyNameLen = usFriendlyNameLen;
   }
   else 
   {
       pstP->usFriendlyNameLen = 0;
   }
   if (pstP->usFriendlyNameLen == 0)
   {
      pstP->pucCertFriendlyName = NULL;
   }
   else
   {      
       pstP->pucCertFriendlyName = (WE_UINT8 *)WE_MALLOC(pstP->usFriendlyNameLen*sizeof(WE_UINT8)+1);
       if (!(pstP->pucCertFriendlyName))
       {
           M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
           WE_FREE(pstP);
           return;
       }
       if(!pucQ)
       {
           M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
           WE_FREE(pstP->pucCertFriendlyName);
           WE_FREE(pstP);
           return ;
       }
       for (iLoop=0; iLoop<pstP->usFriendlyNameLen; iLoop++)
            pstP->pucCertFriendlyName[iLoop] = pucQ[iLoop];
       pstP->pucCertFriendlyName[iLoop] = 0;
   }
   pstP->sCharSet = usCharSet;
   pstP->eType = E_SEC_DP_STORE_CERT_DLG;
   pstP->ucAnswer = ucAnswer;
   M_SEC_CURRENT_TI_RESPONSE_VALUES = ( St_SecTlUeFunctionId *)pstP;
   M_SEC_WANTS_TO_RUN = 1;
}
/*====================================================================================
FUNCTION: 
    Sec_UePinDialogResp  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Response of Pin Dialog.  
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:   Global data handle.
    eResult[IN]:          result.     
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
=====================================================================================*/
void Sec_UePinDialogResp(WE_HANDLE hSecHandle,E_SecUeReturnVal eResult)
{
    St_SecTlPinDlgResult *pstP={0};
    WE_LOG_MSG((WE_LOG_DETAIL_LOW, 0,
               "SEC: Sec_UePinDialogResp(result %d)\n", eResult));
    if(!hSecHandle)
    {
        return ;
    }
    if (!(M_SEC_VAITING_FOR_TI_RESULT))                
    {
        M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_VAITING_FOR_TI_RESULT = 0;
    pstP = ( St_SecTlPinDlgResult *)WE_MALLOC(sizeof( St_SecTlPinDlgResult));
    if (!pstP)
    {
        M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
        return;
    }
    pstP->eType = E_SEC_DP_PIN_DLG;
    pstP->eResult = eResult;
    M_SEC_CURRENT_TI_RESPONSE_VALUES = ( St_SecTlUeFunctionId *)pstP;
  M_SEC_WANTS_TO_RUN = 1;
}

/*====================================================================================
FUNCTION: 
    Sec_UeVerifyPin  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    verify pin.  
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:   Global data handle.
    ucKeyType[IN]:        key type.  
    pcPin[IN]:            pin.     
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
=====================================================================================*/
void Sec_UeVerifyPin(WE_HANDLE hSecHandle,WE_UINT8 ucKeyType, const WE_CHAR *pcPin)
{
    St_SecTlVerifyPin *pstP={0};
    WE_INT32 iLoop = 0;
    WE_UINT16 usPinLen = 0;
    WE_LOG_MSG((WE_LOG_DETAIL_LOW, 0,"SEC: Sec_UeVerifyPin(keyType = %u, pin =", ucKeyType));
    if(!hSecHandle)
    {
        return ;
    }
    if(pcPin != NULL)
    {
        while(pcPin[usPinLen] != 0)
        {
            usPinLen++;
        }
    }
    else
    {
        usPinLen = 0;
    }
    WE_LOG_DATA (WE_LOG_DETAIL_LOW, 0, (const WE_UINT8 *)pcPin, usPinLen);
    M_SEC_VAITING_FOR_TI_RESULT = 0;
    pstP = ( St_SecTlVerifyPin *)WE_MALLOC(sizeof( St_SecTlVerifyPin));
    if (!pstP)
    {
        M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
        return;
    }
    pstP->eType = E_SEC_DP_VERIFY_PIN;
    pstP->ucKeyType = ucKeyType;
    pstP->pcPin = NULL;
    M_SEC_CURRENT_TI_RESPONSE_VALUES = ( St_SecTlUeFunctionId *)pstP;
    if(pcPin != NULL)
    {
        pstP->pcPin = (WE_CHAR *)WE_MALLOC(usPinLen+1);
        if (!(pstP->pcPin))
        {
            M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
            WE_FREE(pstP);
            return;
        }
        
        for  (iLoop=0; iLoop<usPinLen; iLoop++)
        {
            pstP->pcPin[iLoop] = pcPin[iLoop];
        }
        pstP->pcPin[iLoop] = 0;
    }
    M_SEC_WANTS_TO_RUN = 1;
}

/*====================================================================================
FUNCTION: 
    Sec_UeVerifyHash  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    Verify Hash .
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:   Global data handle.
    pucHash[IN]:        the value of hash.       
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
=====================================================================================*/
void Sec_UeVerifyHash(WE_HANDLE hSecHandle,WE_UINT8 *pucHash)
{
    St_SecTlVerifyHash *pstP={0};
    WE_INT32 iLoop=0;
    WE_UINT16 usHashLen = 30;

    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 0,"Sec_UeVerifyHash(hash = %u\n", pucHash));
    if(!hSecHandle)
    {
        return ;
    }
    M_SEC_VAITING_FOR_TI_RESULT = 0;
    pstP = ( St_SecTlVerifyHash *)WE_MALLOC(sizeof( St_SecTlVerifyHash));
    if (!pstP)
    {
        M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
        return;
    }
    pstP->eType = E_SEC_DP_VERIFY_HASH;
    pstP->pucHash = NULL;
    M_SEC_CURRENT_TI_RESPONSE_VALUES = ( St_SecTlUeFunctionId *)pstP;
    if(pucHash != NULL)
    {
        pstP->pucHash = (WE_UINT8 *)WE_MALLOC((usHashLen+1)*sizeof(WE_UINT8));
        if (!(pstP->pucHash))
        {
            M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
            WE_FREE(pstP);
            return;
        }
        
        for (iLoop=0; iLoop<usHashLen; iLoop++)
            pstP->pucHash[iLoop] = pucHash[iLoop];
    }
    M_SEC_WANTS_TO_RUN = 1;
}

/*====================================================================================
FUNCTION: 
    Sec_UeHashDialogResp  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    response of hash dialog.   
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:   Global data handle.
    eResult[IN]:     result.         
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
=====================================================================================*/
void Sec_UeHashDialogResp(WE_HANDLE hSecHandle,E_SecUeReturnVal eResult)
{
    St_SecTlPinDlgResult *pstP={0};
    WE_LOG_MSG((WE_LOG_DETAIL_LOW, 0,"Sec_UeHashDialogResp(result %d)\n", eResult));
    if(!hSecHandle)
    {
        return ;
    }

    if (!(M_SEC_VAITING_FOR_TI_RESULT))                
    {
        M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_VAITING_FOR_TI_RESULT = 0;
    pstP = ( St_SecTlPinDlgResult *)WE_MALLOC(sizeof( St_SecTlPinDlgResult));
    if (!pstP)
    {
        M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
        return;
    }
    pstP->eType = E_SEC_DP_HASH_DLG;
    pstP->eResult = eResult;
    M_SEC_CURRENT_TI_RESPONSE_VALUES = ( St_SecTlUeFunctionId *)pstP;
    M_SEC_WANTS_TO_RUN = 1;
}

/*====================================================================================
FUNCTION: 
    Sec_UeChangePinDialogResp  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    response of change pin dialog.   
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:   Global data handle.
    eResult[IN]:         result.       
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
=====================================================================================*/
void Sec_UeChangePinDialogResp(WE_HANDLE hSecHandle,E_SecUeReturnVal eResult)
{
   St_SecTlPinDlgResult *pstP={0};
   WE_LOG_MSG((WE_LOG_DETAIL_LOW, 0,"SEC: Sec_UeChangePinDialogResp(result = %d)\n", eResult));
    if(!hSecHandle)
    {
        return ;
    }

   if (!(M_SEC_VAITING_FOR_TI_RESULT))                
   {
       M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
       return;
   }
   M_SEC_VAITING_FOR_TI_RESULT = 0;
   pstP = ( St_SecTlPinDlgResult *)WE_MALLOC(sizeof( St_SecTlPinDlgResult));
   if (!pstP)
   {
       M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
       return;
   }
   pstP->eType = E_SEC_DP_CHANGE_PIN_DLG;
   pstP->eResult = eResult;
   M_SEC_CURRENT_TI_RESPONSE_VALUES = ( St_SecTlUeFunctionId *)pstP;
   M_SEC_WANTS_TO_RUN = 1;
}

/*====================================================================================
FUNCTION: 
    Sec_UeChangePin  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    change pin .   
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:   Global data handle.
    ucKeyType[IN]: key type.          
    pcOldPin[in]:  old pin.         
    pcNewPin [in]:  new pin.             
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
=====================================================================================*/
void Sec_UeChangePin(WE_HANDLE hSecHandle,WE_UINT8 ucKeyType, const WE_CHAR *pcOldPin, 
                     const WE_CHAR *pcNewPin)
{
   St_SecTlChangePin *pstP={0};
   WE_INT32 iLoop=0;
   WE_UINT16 usOldPinLen=0;
   WE_UINT16 usNewPinLen=0;

   WE_LOG_MSG((WE_LOG_DETAIL_LOW, 0,"SEC: Sec_UeChangePin(keyType = %u", ucKeyType));
    if(!hSecHandle)
    {
        return ;
    }
   
   if(pcOldPin != NULL)  
   {
       while(pcOldPin[usOldPinLen] != 0)
       {
           usOldPinLen++;
       }
   }
   else        
   {
      usOldPinLen = 0;
   }
   if(pcNewPin != NULL)  
   {
       while(pcNewPin[usNewPinLen] != 0)
       {
           usNewPinLen++;
       }
   }
   else
   {
       usNewPinLen = 0;
   }
   
   WE_LOG_MSG((WE_LOG_DETAIL_LOW, 0,"oldPinLen = %d, oldPin =\n", usOldPinLen));
   WE_LOG_DATA (WE_LOG_DETAIL_LOW, 0, (const WE_UINT8 *)pcOldPin, usOldPinLen);
   WE_LOG_MSG((WE_LOG_DETAIL_LOW, 0,"newPinLen = %d, newPin =\n", usNewPinLen));
   WE_LOG_DATA (WE_LOG_DETAIL_LOW, 0, (const WE_UINT8 *)pcNewPin, usNewPinLen);

   M_SEC_VAITING_FOR_TI_RESULT = 0;
   pstP = ( St_SecTlChangePin *)WE_MALLOC(sizeof( St_SecTlChangePin));
   if (!pstP)
   {
        M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
        return;
   }
   pstP->eType = E_SEC_DP_CHANGE_PIN;
   pstP->ucKeyType = ucKeyType;
   pstP->pcOldPin = NULL;
   pstP->pcNewPin = NULL;
   M_SEC_CURRENT_TI_RESPONSE_VALUES = ( St_SecTlUeFunctionId *)pstP;
   if( (pcOldPin != NULL) && (pcNewPin != NULL) )
   {
        pstP->pcOldPin = (WE_CHAR *)WE_MALLOC(usOldPinLen+1);
        pstP->pcNewPin = (WE_CHAR *)WE_MALLOC(usNewPinLen+1);
        if ( !(pstP->pcOldPin) || !(pstP->pcNewPin) )
        {
            M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
            if(pstP->pcOldPin)
            {
                WE_FREE(pstP->pcOldPin);
            }
            if(pstP->pcNewPin)
            {
                WE_FREE(pstP->pcNewPin);
            }  
            WE_FREE(pstP);
            return;
        }
        
        for (iLoop=0; iLoop<usOldPinLen; iLoop++)
            pstP->pcOldPin[iLoop] = pcOldPin[iLoop];
        pstP->pcOldPin[iLoop] = 0;
        
        for (iLoop=0; iLoop<usNewPinLen; iLoop++)
            pstP->pcNewPin[iLoop] = pcNewPin[iLoop];
        pstP->pcNewPin[iLoop] = 0;
   }
   M_SEC_WANTS_TO_RUN = 1;
}

/*====================================================================================
FUNCTION: 
    Sec_UeGenPinResp  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    response of generate pin. 
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:   Global data handle.
    eResult[IN]: result.           
    pcPin [IN] : pin.              
RETURN VALUE:
    None
USED GLOBAL VARIABLES:
    None.  
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
=====================================================================================*/
void Sec_UeGenPinResp(WE_HANDLE hSecHandle, E_SecUeReturnVal eResult, const WE_CHAR *pcPin)
{
    St_SecTlGenPinResp *pstP={0};
    WE_INT32 iLoop=0;
    WE_UINT16 usPinLen = 0;

    WE_LOG_MSG((WE_LOG_DETAIL_LOW, 0,"SEC: Sec_UeGenPinResp(result = %d", eResult));
    if(!hSecHandle)
    {
        return ;
    }
    if(pcPin != NULL)  
    {
        while(pcPin[usPinLen] != 0)
        {
            usPinLen++;
        }
    }
    else
    {
        usPinLen = 0;
    }

    WE_LOG_MSG((WE_LOG_DETAIL_LOW, 0,"pinLen = %d, pin =\n", usPinLen));
    WE_LOG_DATA(WE_LOG_DETAIL_LOW, 0, (const WE_UINT8 *)pcPin, usPinLen);

    if (!(M_SEC_VAITING_FOR_TI_RESULT))
    {
        M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
        return;
    }
    M_SEC_VAITING_FOR_TI_RESULT = 0;
    pstP = ( St_SecTlGenPinResp *)WE_MALLOC(sizeof( St_SecTlGenPinResp));
    if (!pstP)
    {
        M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
        return;
    }
    pstP->eType = E_SEC_DP_GEN_PIN;
    pstP->eResult = eResult;
    pstP->pcPin = NULL;
    M_SEC_CURRENT_TI_RESPONSE_VALUES = ( St_SecTlUeFunctionId *)pstP;
    if( (eResult == E_SEC_UE_OK) && (pcPin != NULL) )
    {
        pstP->pcPin = (WE_CHAR *)WE_MALLOC(usPinLen+1);
        if (!(pstP->pcPin))
        {
            M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
            WE_FREE(pstP);
            return;
        }
        
        iLoop=0;
        while(pcPin[iLoop] != 0)
        {
            pstP->pcPin[iLoop] = pcPin[iLoop];
            iLoop++;
        }
        pstP->pcPin[iLoop] = 0;
    }
  M_SEC_WANTS_TO_RUN = 1;
}

/*====================================================================================
FUNCTION: 
    Sec_UeAllowDelKeysResp  
CREATE DATE: 
    2006-7-21
AUTHOR: 
    Tang  
DESCRIPTION:
    response of allow delete key.
ARGUMENTS PASSED:
    hSecHandle[IN/OUT]:   Global data handle.
    eResult[IN]: result.              
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
=====================================================================================*/
void Sec_UeAllowDelKeysResp(WE_HANDLE hSecHandle,E_SecUeReturnVal eResult)
{
   St_SecTlPinDlgResult *pstP={0};
   WE_LOG_MSG((WE_LOG_DETAIL_LOW, 0,"SEC: Sec_UeAllowDelKeysResp(result = %d)\n", eResult));
    if(!hSecHandle)
    {
        return ;
    }
    
   if (!(M_SEC_VAITING_FOR_TI_RESULT))                
   {
        M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
        return;
   }
   M_SEC_VAITING_FOR_TI_RESULT = 0;
   pstP = ( St_SecTlPinDlgResult *)WE_MALLOC(sizeof( St_SecTlPinDlgResult));
   if (!pstP)
   {
        M_SEC_CURRENT_TI_RESPONSE_VALUES = NULL;
        return;
   }
   pstP->eType = E_SEC_DP_RM_KEYS_AND_CERTS;
   pstP->eResult = eResult;
   M_SEC_CURRENT_TI_RESPONSE_VALUES = ( St_SecTlUeFunctionId *)pstP;
   M_SEC_WANTS_TO_RUN = 1;
}


