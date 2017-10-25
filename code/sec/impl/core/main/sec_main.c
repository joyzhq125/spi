/*==================================================================================================
    HEADER NAME : sec_main.c
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
    
    GENERAL DESCRIPTION
        this file is the main entry of sec module.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-07-07   Kevin Yang        None         Draft
    
==================================================================================================*/

/*==================================================================================================
*   Include File 
*=================================================================================================*/
#include "sec_comm.h"

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
#define SEC_MAIN_ISHELL_POINTER              (((ISec*)hSecHandle)->m_pIShell)
#define SEC_MAIN_WANTS_TO_RUN                (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->iWantsToRun)
#define SEC_MAIN_CALLBACK                    (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->m_cb)
#define SEC_BREW_CIPHERFACTORY_HANDLE        (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->hFactory)


/***************************************************************************************************
*   Function Define Section
***************************************************************************************************/
/*==================================================================================================
FUNCTION: 
    Sec_IsRun
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    Judge whether sec module needs to run.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Handle for global data used in sec module.
RETURN VALUE:
    True: the sec module needs to run.
    False: the sec module does not need to run.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_IsRun(WE_HANDLE hSecHandle)
{
    WE_INT32 iResult = 0;
    if(NULL == hSecHandle)
    {
        return FALSE;
    }
    
    iResult = (!Sec_MsgQueueState(hSecHandle));
    
    if (!iResult)
    {
        iResult = SEC_MAIN_WANTS_TO_RUN;
    }
    
    return iResult;
}
/*==================================================================================================
FUNCTION: 
    Sec_Run
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    Run the sec module.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Handle for global data used in sec module.
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
static void Sec_Run(WE_HANDLE hSecHandle)
{
    if(NULL == hSecHandle)
    {
        return;
    }
    if(SEC_MAIN_WANTS_TO_RUN)
    {
        Sec_DpHandleInternal(hSecHandle);
    }
    
    Sec_MsgProcess(hSecHandle);
}
/*==================================================================================================
FUNCTION: 
    Sec_StartEx
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    Start sec module. and this function will be invoked when the sec module is created.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Handle for global data used in sec module.
RETURN VALUE:
    M_SEC_ERR_OK
    M_SEC_ERR_GENERAL_ERROR
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_StartEx(WE_HANDLE hSecHandle)
{
    return Sec_DpHandleInit(hSecHandle);
}
/*==================================================================================================
FUNCTION: 
    Sec_Stop
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    Exit the sec module. and this function will be invoked when the sec module is released.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]: Handle for global data used in sec module.
RETURN VALUE:
    M_SEC_ERR_INVALID_PARAMETER
    M_SEC_ERR_GENERAL_ERROR
    M_SEC_ERR_OK
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 Sec_StopEx(WE_HANDLE hSecHandle)
{
    WE_INT32 iResult = 0;

    if(NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    iResult = Sec_DpHandleTerminate(hSecHandle);

    if(NULL != SEC_BREW_CIPHERFACTORY_HANDLE)
    {
        iResult = Sec_LibCryptTerminate(hSecHandle);
        if(M_SEC_ERR_OK != iResult)
        {
            return M_SEC_ERR_GENERAL_ERROR;
        }
    }
    return M_SEC_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    Sec_RunMainFlow
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    A callback function, and sec module run in this callback function.
ARGUMENTS PASSED:
    void * pHandle [IN]: Handle for global data used in sec module.
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
void Sec_RunMainFlow(WE_HANDLE hHandle)
{
    ISec * hSecHandle = NULL;
    if(NULL == hHandle)
    {
        return;
    }
    hSecHandle = (ISec*)hHandle;

    Sec_Run((WE_HANDLE)hSecHandle);

    if(Sec_IsRun((WE_HANDLE)hSecHandle))
    {
        SEC_REGCALLBACK(SEC_MAIN_ISHELL_POINTER,SEC_MAIN_CALLBACK,Sec_RunMainFlow,hSecHandle);
    } 
}

/*==================================================================================================
FUNCTION: 
    Sec_PostMsg
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    Post message to sec module's message queue.
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle [IN]:Handle for global data used in sec module.
    WE_UINT32 iEvent [IN]: event id.
    void* hData [IN]:event data body.
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
WE_INT32 Sec_PostMsg(WE_HANDLE hSecHandle, WE_INT32 iEvent, WE_HANDLE hData)
{ 
    WE_INT32 iResult = -100;
    switch (iEvent)
    {
        case E_SEC_MSG_CERTTRANSFER:       
            iResult = SEC_MSG_DISPATCH_IP (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN, 
                                        (WE_INT32)M_SEC_DP_MSG_EXECUTE_COMMAND, 0, hData );
            break;
        case E_SEC_MSG_USERCERTREQ:       
            iResult = SEC_MSG_DISPATCH_IP (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN, 
                                            (WE_INT32)M_SEC_DP_MSG_REQ_USER_CERT, 0, hData ); 
            break;
        case E_SEC_MSG_CERTSAVE:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                                    (WE_INT32)M_SEC_DP_MSG_STORE_CERT, hData);
            break;
        case E_SEC_MSG_CERTSAVE_RESP:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN, 
                                                (WE_INT32)M_SEC_DP_MSG_STORE_CERT_RESP, hData );
            break;
        case E_SEC_MSG_TRUSTEDCA:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                                 (WE_INT32)M_SEC_DP_MSG_TRUSTED_CA_INFO, hData );
            break;
        case E_SEC_MSG_TRUSTEDCA_RESP:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN, 
                                            (WE_INT32)M_SEC_DP_MSG_TRUSTED_CA_INFO_RESP, hData );
            break;
        case E_SEC_MSG_DLVCERT:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                                    (WE_INT32)M_SEC_DP_MSG_DELIVERY_CERT, hData );
            break;
        case E_SEC_MSG_DLVCERT_RESP:
            iResult = SEC_MSG_DISPATCH_P(hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN, 
                                                (WE_INT32)M_SEC_DP_MSG_DELVERY_CERT_RESP, hData ); 
            break;
        case E_SEC_MSG_SETUPCON:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                                (WE_INT32)M_SEC_DP_MSG_ESTABLISH_CONN,hData );
            break;
        case E_SEC_MSG_STOPCON:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                            (WE_INT32)M_SEC_DP_MSG_TERMINATE_CONN, hData );
            break;
        case E_SEC_MSG_DELPEER:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                        (WE_INT32)M_SEC_DP_MSG_RM_PEER_LINKS, hData );
            break;
        case E_SEC_MSG_ATTACHPEER:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                       (WE_INT32)M_SEC_DP_MSG_LINK_PEER_SESSION, hData );
            break;
        case E_SEC_MSG_SEARCHPEER:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                        (WE_INT32)M_SEC_DP_MSG_LOOKUP_PEER, hData );
            break;
        case E_SEC_MSG_ENVSESSION:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                        (WE_INT32)M_SEC_DP_MSG_ACTIVE_SESSION, hData );
            break;
        case E_SEC_MSG_DISSESSION:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                (WE_INT32)M_SEC_DP_MSG_INVALID_SESSION, hData );
            break;
        case E_SEC_MSG_GETSESINFO:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                        (WE_INT32)M_SEC_DP_MSG_FETCH_SESSION, hData );
            break;
        case E_SEC_MSG_UPSESSION:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                (WE_INT32)M_SEC_DP_MSG_UPDATE_SESSION, hData );  
            break;
        case E_SEC_MSG_GETCERTNAMELIST:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                    (WE_INT32)M_SEC_DP_MSG_VIEW_CERT_NAMES, hData ); 
            break;
        case E_SEC_MSG_VIEW_CURCERT:
            iResult = SEC_MSG_DISPATCH_IP (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                            (WE_INT32)M_SEC_DP_MSG_VIEW_CUR_CERT, 1, hData );
            break;
        case E_SEC_MSG_VIEW_INFOSESSION:
            iResult = SEC_MSG_DISPATCH_IP (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                    (WE_INT32)M_SEC_DP_MSG_VIEW_SESSION, 1, hData );
            break;
        case E_SEC_MSG_GET_CURCLASS:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                    (WE_INT32)M_SEC_DP_MSG_GET_CUR_SEC_CLASS, hData );
            break;
        case E_SEC_MSG_CLRSESSION:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                    (WE_INT32)M_SEC_DP_MSG_CLEANUP_SESSION, hData );
            break;
        case E_SEC_MSG_GET_CERTCONTENT:
            iResult = SEC_MSG_DISPATCH_IP (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                     (WE_INT32)M_SEC_DP_MSG_GET_CERT, FALSE, hData );
            break;
        case E_SEC_MSG_CHANGE_WTLS_CERT_ABLE:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                    (WE_INT32)M_SEC_DP_MSG_CHANGE_WTLS_CERT_ABLE, hData);
            break;
        case E_SEC_MSG_REMOVECERT:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                                (WE_INT32)M_SEC_DP_MSG_RM_CERT, hData );
            break;
        case E_SEC_MSG_WTLSGETCIPHERSUITE:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                        (WE_INT32)M_SEC_DP_MSG_GET_CIPHERSUITE, hData );
            break;
        case E_SEC_MSG_WTLSKEYEXCHANGE:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                        (WE_INT32)M_SEC_DP_MSG_EXCHANGE_WTLS_KEY, hData );
            break;
        case E_SEC_MSG_WTLSGETPRFRESULT: 
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                                (WE_INT32)M_SEC_DP_MSG_WTLS_PRF, hData );
            break;
        case E_SEC_MSG_WTLSVERIFYSVRCERT:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                (WE_INT32)M_SEC_DP_MSG_VERIFY_WTLS_CERT_CHAIN, hData );
            break;
        case E_SEC_MSG_WTLSGETUSRCERT: 
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                   (WE_INT32)M_SEC_DP_MSG_GET_WTLS_CLIENT_CERT, hData );
            break;
        case E_SEC_MSG_WTLSCOMPSIG: 
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                       (WE_INT32)M_SEC_DP_MSG_COMPUTE_WTLS_SIGN, hData );
            break;
        /*modify by bird 061114*/
        case E_SEC_MSG_ENCRYPT_PKC_RESP:      
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_WIM_MAIN, 
                                (WE_INT32)M_SEC_DP_MSG_ENCRYPT_PKC_RESP, hData );
            break;
        case E_SEC_MSG_DECRYPT_PKC_RESP:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_WIM_MAIN, 
                                (WE_INT32)M_SEC_DP_MSG_DECRYPT_PKC_RESP, hData );
            break;
        case E_SEC_MSG_COMPUTE_SIGN_RESP:   
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_WIM_MAIN, 
                                (WE_INT32)M_SEC_DP_MSG_COMPUTE_SIGN_RESP, hData );
            break;
        case E_SEC_MSG_GEN_KEYPAIR_RESP: 
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_WIM_MAIN, 
                                (WE_INT32)M_SEC_DP_MSG_GEN_KEYPAIR_RESP, hData );
            break;
        case E_SEC_MSG_VERIFY_SIGN_RESP:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_WIM_MAIN, 
                                (WE_INT32)M_SEC_DP_MSG_VERIFY_SIGN_RESP, hData );
            break;
        
#ifdef M_SEC_CFG_CAN_SIGN_TEXT
        case E_SEC_MSG_APP_SIGNTEXT:
        iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                    (WE_INT32)M_SEC_DP_MSG_SIGN_TEXT, hData );
        break;
#endif
#if((defined(M_SEC_CFG_CAN_SIGN_TEXT) && defined(M_SEC_CFG_STORE_CONTRACTS)))
        case E_SEC_MSG_GETCONTRACTINFOS:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                    (WE_INT32)M_SEC_DP_MSG_VIEW_CONTRACTS, hData );
            break;
        case E_SEC_MSG_REMOVECONTRACT:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN, 
                                (WE_INT32)M_SEC_DP_MSG_RM_CONTRACT, hData );
            break;
#endif 
/* modify by Sam [070124] */
/* #if(defined(M_SEC_CFG_WTLS_CLASS_3) || defined(M_SEC_CFG_CAN_SIGN_TEXT)) */
        case E_SEC_MSG_GENKEYPAIR:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                        (WE_INT32)M_SEC_DP_MSG_GEN_KEY_PAIR, hData );
            break;
        case E_SEC_MSG_GETPUBKEY:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                    (WE_INT32)M_SEC_DP_MSG_GET_USER_PUB_KEY, hData );
            break;
        case E_SEC_MSG_MODIFYPRIKEYPIN:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                            (WE_INT32)M_SEC_DP_MSG_CHANGE_PIN, hData );
            break;
/* #endif */
    /*added by bird 061221,action asynchronism*/    
        case E_SEC_MSG_SHOWCONTRACTLIST:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                (WE_INT32)M_SEC_DP_MSG_SHOWCONTRACTLIST, hData );  
            break;
        case E_SEC_MSG_SHOWCONTRACTCONTENT:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                (WE_INT32)M_SEC_DP_MSG_SHOWCONTRACTCONTENT, hData );
            break;
        case E_SEC_MSG_SHOWCERTLIST:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                        (WE_INT32)M_SEC_DP_MSG_SHOWCERTLIST, hData );
            break;
        case E_SEC_MSG_SHOWCERTCONTENT:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                        (WE_INT32)M_SEC_DP_MSG_SHOWCERTCONTENT, hData );
            break;
        case E_SEC_MSG_HASH:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                        (WE_INT32)M_SEC_DP_MSG_HASH, hData );
            break;
        case E_SEC_MSG_STORECERT:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                        (WE_INT32)M_SEC_DP_MSG_STORECERT, hData );
            break;
        case E_SEC_MSG_CONFIRM:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                        (WE_INT32)M_SEC_DP_MSG_CONFIRM, hData );
            break;
        case E_SEC_MSG_NAMECONFIRM:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                        (WE_INT32)M_SEC_DP_MSG_NAMECONFIRM, hData );
            break;
        case E_SEC_MSG_SELECTCERT:
            iResult = SEC_MSG_DISPATCH_P (hSecHandle, M_SEC_MSG_DST_HANDLE_MAIN,
                                        (WE_INT32)M_SEC_DP_MSG_SELECTCERT, hData );
            break;
   
        default:
            WE_LOG_MSG ((0, 0,"SEC: Received unknown msg (%04x)\n", iEvent));
            iResult = M_SEC_ERR_INVALID_PARAMETER;
            break;
    }
    return iResult;
}

