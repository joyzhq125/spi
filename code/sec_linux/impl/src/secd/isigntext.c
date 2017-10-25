/*==================================================================================================
    HEADER NAME : isigntext.c
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
#include "isignText.h"

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
#define SEC_IAEECALLBACK_VAR    (((St_PublicData *)(((ISec*)pMe)->hPrivateData))->m_cb)
#define SEC_IISHELL_POINTER     (((ISec*)pMe)->m_pIShell)
#define SEC_PRIVATE_DATA        (((ISec*)pMe)->hPrivateData)
#define SEC_SIINFO              (((St_PublicData *)(((ISec*)pMe)->hPrivateData))->pstInfo)
#define SEC_PEER_CACHE          (((St_PublicData *)(((ISec*)pMe)->hPrivateData))->pstPeerCache)
#define SEC_SESSION_CACHE       (((St_PublicData *)(((ISec*)pMe)->hPrivateData))->pstSessionCache)
#define SEC_WIM_HAVELOADUSERPRIV        (((St_PublicData *)(((ISec *)pMe)->hPrivateData))->pcHaveLoad)

/*==================================================================================================
FUNCTION: 
    SignText_SignText
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    provide sign text function for browser engine.
ARGUMENTS PASSED:
    ISignText * pMe[IN]:pointer of ISignText instance.
    WE_INT32 iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_INT32 iSignId[IN]: ID of the sign.
    WE_CHAR * pcText[IN]:Pointer to the text.
    WE_CHAR * pcData[IN]: Pointer to the data.
    WE_INT32 iDataLen[IN]:Length of data.
    WE_INT32 iKeyIdType[IN]: The type of the key id.
    WE_CHAR * pcKeyId[IN]:Pointer to the key id.
    WE_INT32 iKeyIdLen[IN]: Length of key id.
    WE_INT32 iOptions[IN]:The value of option.
RETURN VALUE:
    G_SEC_INVALID_PARAMETER,
    G_SEC_NOTENOUGH_MEMORY,
    G_SEC_OK,
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 SignText_Handle(WE_HANDLE pMe, WE_INT32 iTargetID, WE_INT32 iSignId,  WE_CHAR * pcText,
                                 WE_INT32 iKeyIdType, WE_CHAR * pcKeyId, WE_INT32 iKeyIdLen,
                                 WE_CHAR * pcStringToSign, WE_INT32 iStringToSignLen, WE_INT32 iOptions)
{
   WE_CHAR * pcTextMem = NULL;
   WE_CHAR * pcDataMem = NULL;
   WE_CHAR * pcKeyIdMem = NULL;
   WE_INT32 iResult = 0;
    
   if(NULL == pMe)
   {
       return G_SEC_INVALID_PARAMETER;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"SignText_SignText\n"));
   if(NULL != pcText)
   {
       pcTextMem = SEC_STRDUP(pcText);
       if(NULL == pcTextMem)
       {
           return G_SEC_NOTENOUGH_MEMORY;            
       }        
   }
   if(NULL != pcStringToSign)
   {
       pcDataMem = WE_MALLOC((WE_ULONG)(iStringToSignLen+1));
       if(NULL == pcDataMem)
       {
           WE_FREE(pcTextMem);
           return G_SEC_NOTENOUGH_MEMORY;
       }
       (void)WE_MEMCPY(pcDataMem,pcStringToSign,(WE_UINT32)iStringToSignLen);
       pcDataMem[iStringToSignLen] = '\0';        
   }
   if(NULL != pcKeyId)
   {
       pcKeyIdMem = WE_MALLOC((WE_ULONG)iKeyIdLen+1);
       if(NULL == pcKeyIdMem)
       {
           WE_FREE(pcDataMem);
           WE_FREE(pcTextMem);
           return G_SEC_NOTENOUGH_MEMORY;
       }
       (void)WE_MEMCPY(pcKeyIdMem,pcKeyId,(WE_UINT32)iKeyIdLen);
       pcKeyIdMem[iKeyIdLen] = '\0';
   }
    
   iResult = Sec_MsgSignText((WE_HANDLE)pMe,iTargetID,iSignId,pcTextMem,pcDataMem,\
       iStringToSignLen,iKeyIdType,pcKeyIdMem,iKeyIdLen,iOptions);
   if(M_SEC_ERR_OK != iResult)
   {
       WE_FREE(pcDataMem);
       WE_FREE(pcTextMem);
       WE_FREE(pcKeyIdMem);
       return G_SEC_NOTENOUGH_MEMORY;  
   }
   
   Sec_RunMainFlow(pMe);   
   return G_SEC_OK;
}

/*==================================================================================================
FUNCTION: 
    SignText_EvtSelectCertAction
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    after get cert name list, the invoker should run this function to give a feedback
    to sec module.
ARGUMENTS PASSED:
    ISignText *        pMe[IN]:pointer of ISignText instance.
    St_SelectCertAction  stSelectCert[IN]:The content of how to choose cert by name.
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
WE_UINT32 SignText_EvtSelectCertAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_SelectCertAction stSelectCert)
{
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return G_SEC_INVALID_PARAMETER;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"Sec_EvtSelectCertAction\n"));
    
   iResult = Sec_MsgEvtSelCertAction((WE_HANDLE)pMe, iTargetID,stSelectCert);
    
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
    SignText_EvtPinAction
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    when a event is sent to invoker to ask a pin code to do specific something, this interface
    should be invoked to give a feedback to sec module.
ARGUMENTS PASSED:
    ISignText *         pMe[IN]:pointer of ISignText instance.
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
WE_UINT32 SignText_EvtPinAction(WE_HANDLE pMe,WE_INT32 iTargetID,St_PinAction stPin)
{
   WE_INT32   iResult = 0;

   if(NULL == pMe)
   {
      return G_SEC_INVALID_PARAMETER;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"Sec_EvtPinAction\n"));


   iResult = Sec_UePinAction((WE_HANDLE)pMe, iTargetID,stPin);

   if(M_SEC_ERR_OK != iResult)
   {
      return G_SEC_GENERAL_ERROR;  
   }

   Sec_RunMainFlow(pMe);
   return G_SEC_OK;
}

#endif
#if 0
static WE_UINT32 
SignText_EvtShowDlgAction(ISignText* pMe,WE_INT32 iTargetId,
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
#ifdef G_SEC_CFG_SHOW_PIN        
        case M_UE_PIN_EVENT:
        {
            St_PinAction* pstPin = (St_PinAction*)pvData;
            if(NULL == pstPin)
            {
                return G_SEC_NOTENOUGH_MEMORY;
            }
            /*call fuction*/
            iRes = SignText_EvtPinAction(pMe,iTargetId,*pstPin);
        }
        break;
#endif        
        case M_UE_SELECT_CET_EVENT:
        {
            St_SelectCertAction* pstSelCert = (St_SelectCertAction*)pvData;
            if(NULL == pstSelCert)
            {
                return G_SEC_NOTENOUGH_MEMORY;
            }            
            /*call function*/
            iRes = (WE_INT32)(SignText_EvtSelectCertAction(pMe,iTargetId,*pstSelCert));
        }
        break;
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
#endif

