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
#include "sec_mod.h"
#include "oem_secmgr.h"
#include "ISignText.h"

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
#define SEC_IAEECALLBACK_VAR    (((St_PublicData *)(((ISignText*)pMe)->hPrivateData))->m_cb)
#define SEC_IISHELL_POINTER     (((ISignText*)pMe)->m_pIShell)

#define SEC_PRIVATE_DATA        (((ISignText*)pMe)->hPrivateData)
#define SEC_SIINFO              (((St_PublicData *)(((ISignText*)pMe)->hPrivateData))->pstInfo)
#define SEC_PEER_CACHE          (((St_PublicData *)(((ISignText*)pMe)->hPrivateData))->pstPeerCache)
#define SEC_SESSION_CACHE       (((St_PublicData *)(((ISignText*)pMe)->hPrivateData))->pstSessionCache)
#define SEC_WIM_HAVELOADUSERPRIV        (((St_PublicData *)(((ISignText *)pMe)->hPrivateData))->pcHaveLoad)

/*******************************************************************************
*   Function Define Section
*******************************************************************************/

/*==================================================================================================
FUNCTION: 
    SignText_AddRef
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    SignText add reference
ARGUMENTS PASSED:
    ISignText * pMe[IN]: pointer about an instance of ISignText    
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
static WE_UINT32 SignText_AddRef(ISignText *pMe)
{
   if(NULL == pMe)
   {
       return 0;
   }
   return (WE_UINT32)++(pMe->m_nRefs);
}
/*==================================================================================================
FUNCTION: 
    SignText_Release
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    ISignText release
ARGUMENTS PASSED:
    ISignText * pMe[IN]: pointer about an instance of ISignText 
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
static WE_UINT32 SignText_Release (ISignText *pMe)
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
    ((SecMod*)(pMe->m_pIModule))->uiSecSignTextRefs--;
    
    /*added by bird 061113*/
    /*1.cancel callback*/
    SEC_CANCELCALLBACK(SEC_IAEECALLBACK_VAR);
 
    /*5.free the correlative global memory and initial global variable that control state*/
    Sec_DpResetVariable(pMe);   
 
    uiRes += Sec_StopEx(pMe);
    if(NULL != (St_PublicData*)(pMe->hPrivateData))
    {
        uiRes += (WE_UINT32)(WeMgr_Terminate((((St_PublicData*)(pMe->hPrivateData))->hWeHandle)));   
    } 
    /*if free vtbl*/
    if((WE_UINT32)0 == ((SecMod*)(pMe->m_pIModule))->uiSecSignTextRefs)
    {
        FREE(((SecMod*)(pMe->m_pIModule))->pvSecSignTextVtbl);
    }
 
    if ( SEC_PRIVATE_DATA != NULL )
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
    EBADPARM,
    ENOMEMORY,
    AEE_SUCCESS,
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_UINT32 SignText_Handle(ISignText * pMe, WE_INT32 iTargetID, WE_INT32 iSignId,  WE_CHAR * pcText,
                                 WE_INT32 iKeyIdType, WE_CHAR * pcKeyId, WE_INT32 iKeyIdLen,
                                 WE_CHAR * pcStringToSign, WE_INT32 iStringToSignLen, WE_INT32 iOptions)
{
   WE_CHAR * pcTextMem = NULL;
   WE_CHAR * pcDataMem = NULL;
   WE_CHAR * pcKeyIdMem = NULL;
   WE_INT32 iResult = 0;
    
   if(NULL == pMe)
   {
       return EBADPARM;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"SignText_SignText\n"));
   if(NULL != pcText)
   {
       pcTextMem = SEC_STRDUP(pcText);
       if(NULL == pcTextMem)
       {
           return ENOMEMORY;            
       }        
   }
   if(NULL != pcStringToSign)
   {
       pcDataMem = WE_MALLOC((WE_ULONG)(iStringToSignLen+1));
       if(NULL == pcDataMem)
       {
           WE_FREE(pcTextMem);
           return ENOMEMORY;
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
           return ENOMEMORY;
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
       return ENOMEMORY;  
   }
   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);
   
   return AEE_SUCCESS;
}
/*==================================================================================================
FUNCTION: 
    SignText_RegClientEvtCB
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    register one callback for sec module, and through the callback, sec module can
    return some information to the invoker.
ARGUMENTS PASSED:
    ISignText *      pMe[IN]: pointer about an instance of ISignText   
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
static WE_UINT32 SignText_RegClientEvtCB(ISignText * pMe,WE_HANDLE hPrivData,Fn_ISecEventHandle pcbSecEvtFunc)
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
static WE_UINT32 SignText_EvtSelectCertAction(ISignText * pMe,WE_INT32 iTargetID,St_SelectCertAction stSelectCert)
{
   WE_INT32   iResult = 0;
    
   if(NULL == pMe)
   {
       return EBADPARM;
   }    
   WE_LOG_MSG((0,(WE_UCHAR)0,"Sec_EvtSelectCertAction\n"));
    
   iResult = Sec_MsgEvtSelCertAction((WE_HANDLE)pMe, iTargetID,stSelectCert);
    
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
static WE_UINT32 SignText_EvtPinAction(ISignText * pMe,WE_INT32 iTargetID,St_PinAction stPin)
{
   WE_INT32   iResult = 0;

   if(NULL == pMe)
   {
      return EBADPARM;
   }
   WE_LOG_MSG((0,(WE_UCHAR)0,"Sec_EvtPinAction\n"));


   iResult = Sec_UePinAction((WE_HANDLE)pMe, iTargetID,stPin);

   if(M_SEC_ERR_OK != iResult)
   {
      return EFAILED;  
   }

   SEC_REGCALLBACK(SEC_IISHELL_POINTER,SEC_IAEECALLBACK_VAR,Sec_RunMainFlow,pMe);

   return AEE_SUCCESS;
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
#ifdef G_SEC_CFG_SHOW_PIN        
        case M_UE_PIN_EVENT:
        {
            St_PinAction* pstPin = (St_PinAction*)pvData;
            if(NULL == pstPin)
            {
                return ENOMEMORY;
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
                return ENOMEMORY;
            }            
            /*call function*/
            iRes = (WE_INT32)(SignText_EvtSelectCertAction(pMe,iTargetId,*pstSelCert));
        }
        break;
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
#endif
/*==================================================================================================
FUNCTION: 
    ISignText_New
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    create one instance of ISignText interface.
ARGUMENTS PASSED:
    WE_UINT16          sSize[IN]:the size of need malloc space
    IShell *           pIShell[IN]:Pointer to the IShell interface.
    IModule*           pIModule[IN]:Pointer to the module.
    ISignText **       ppMod[IN]:Pointer to mod.
RETURN VALUE:
    ENOMEMORY,
    EFAILED,
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
WE_INT32 ISignText_New(WE_UINT16 sSize, IShell *pIShell, IModule* pIModule, ISignText ** ppMod)
{
   ISignText * pMe = NULL;
   VTBL(ISignText) * modFuncs = NULL;
    
   if( !ppMod || !pIShell || !pIModule )
   {
       return EFAILED;
   }
    
   *ppMod = NULL;
   /* Allocate memory for the ExtensionCls object*/
   if( sSize < (WE_UINT16)sizeof(ISignText) )
   {
       sSize += (WE_UINT16)sizeof(ISignText);
   }
   /* Allocate the module's struct and initialize it. Note that the 
   modules and apps must not have any static data. 
   Hence, we need to allocate the vtbl as well.*/
    
   if((pMe = (ISignText*)MALLOC((WE_ULONG)(sSize))) == NULL )
   {
       return ENOMEMORY;
   }
    
   pMe->pvt = NULL;
    
   /*Initialize individual entries in the VTBL*/
   if(0 == ((SecMod*)pIModule)->uiSecSignTextRefs)
   {
       if(NULL == (((SecMod*)pIModule)->pvSecSignTextVtbl = (void*)MALLOC(sizeof(VTBL(ISignText)))))
       {
           FREE(pMe);
           return ENOMEMORY;            
       }
       modFuncs = (VTBL(ISignText)*)(((SecMod*)pIModule)->pvSecSignTextVtbl);
        
       modFuncs->AddRef = SignText_AddRef;
       modFuncs->Release = SignText_Release;    
       modFuncs->RegSecClientEvtCB = SignText_RegClientEvtCB;
       modFuncs->Handle = SignText_Handle;
       modFuncs->EvtSelCertAction = SignText_EvtSelectCertAction;
   }    
    
   if(NULL == (modFuncs = (VTBL(ISignText)*)(((SecMod*)pIModule)->pvSecSignTextVtbl)))
   {
       FREE(pMe);
       return EFAILED;
   }
    
   /* initialize the vtable*/
   INIT_VTBL(pMe, ISignText, *modFuncs);
    
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
   pMe->iIFType = (WE_INT32)E_SEC_SIGNTEXT;
   (pMe->hUserCertKeyPair) = ((SecMod*)pIModule)->astPubUserCertKeyPair;
   /*added by bird 070126*/
   SEC_SIINFO = &(((SecMod*)pIModule)->stInfo);
   /*added by bird 070202*/
   SEC_PEER_CACHE = ((SecMod*)pIModule)->astPeerCache;
   SEC_SESSION_CACHE = ((SecMod*)pIModule)->astSessionCache;
   SEC_WIM_HAVELOADUSERPRIV = &(((SecMod*)pIModule)->cHaveLoad);
   /*Note: this must be run after the former sentence*/   
   if(SUCCESS != (WE_INT32)(WeMgr_Initialize(pIShell, (IModule*)pIModule, &(((St_PublicData*)(pMe->hPrivateData))->hWeHandle))))
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
   *ppMod = (ISignText*)pMe;
   /*brs vtbl refernce add*/
   ((SecMod*)pIModule)->uiSecSignTextRefs++;
   return AEE_SUCCESS;
}

