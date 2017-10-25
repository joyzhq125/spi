/*==================================================================================================
    FILE NAME : Sec_sic.c
    MODULE NAME : Sec_SIC
    
    
    GENERAL DESCRIPTION
       Encode/Decode the message.
   
      
    Techfaith Software Confidential Proprietary
    (c) Copyright 2007 by Techfaith Software. All Rights Reserved.
====================================================================================================
    Revision History
        
    Modification                    Tracking
    Date            Author            Number       Description of changes
    ----------   --------------     ---------   ----------------------------
    2007-03-22      Bird Zhang          None             draft
    
    Self-documenting Code
    
==================================================================================================*/ 
/***************************************************************************************************
Include File Section
***************************************************************************************************/
#include "sec_comm.h"
#include "isec.h"
#include "sec_eds.h"
#include "sec_sicmsg.h"
#include "sec_sic.h"
#include "sec_ecdr.h"
#include "sec_icmsg.h"

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
static WE_INT32 SecSIc_SigntextEvtPinAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_SigntextEvtSelectCertAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_SigntextHandle(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_EvtNameConfirmAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_EvtConfirmAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_EvtShowContractContentAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_EvtStoreCertAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_EvtShowContractsListAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_EvtShowCertlistAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_EvtShowCertContentAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_EvtHashAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
#ifdef G_SEC_CFG_SHOW_PIN
static WE_INT32 SecSIc_EvtPinAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_EvtCreatePinAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_EvtChangePinAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_PinModify(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
#endif
static WE_INT32 SecSIc_GetRequestUserCert(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_TransferCert(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_GetContractList(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_GetWtlsCurClass(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_GetSessionInfo(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_GetCurSvrCert(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_GenKeyPairs(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_GetCertNameList(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_ShowDlgAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_SslGetMasterSecret(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_CompSign(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_GetUserCert(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_KeyExchange(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_VerifySvrCertChain(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_GetPrfResult(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_SessionRenew(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_SessionGet(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_DisableSession(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_EnableSession(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_SearchPeer(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_AttachPeerToSession(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_RemovePeer(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_StopConn(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_SetUpConn(WE_HANDLE hSecHandle, WE_VOID *pvMsg);
static WE_INT32 SecSIc_GetCipherSuite(WE_HANDLE hSecHandle, WE_VOID *pvMsg);

/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*==================================================================================================
FUNCTION: 
    SecSIc_ReadProc                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    read data from client and process correponsding operation 
ARGUMENTS PASSED:
    WE_INT32 iFd[IN]: socket id
    WE_VOID* pvData[IN]: input data
    
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 SecSIc_ReadProc( WE_INT32 iFd ,WE_VOID* pvData)
{
    WE_UINT32 ucMsgType = 0;
    WE_INT32 iRet = 0;    
    WE_HANDLE hSecHandle = NULL;
    if(NULL == pvData)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    ucMsgType = Sec_GetMsgType( pvData );
    /*find handle by ifd*/
    if (M_SEC_CONN_TYPE != ucMsgType)
    {
        iRet = Secd_FindHandle(iFd, &hSecHandle);
        if(iRet != G_SEC_OK)
        {
            WE_LOG_MSG((0,0,"Not find match hSecHandle\n"));
            return G_SEC_GENERAL_ERROR;
        }
    }
    WE_LOG_MSG((0,0,"Message Type is %d\n",ucMsgType));
    switch (ucMsgType)
    {
        /*intial a new instance*/
        case M_SEC_CONN_TYPE:
           {
                WE_INT32 iType = 0;
                /*get interface type ,such as wap,brs and so on*/
                iRet = Sec_DecodeConnType(pvData,&iType);
                if(iRet != 0)
                {
                    WE_LOG_MSG((0,0,"Get Connect Interface type failed\n"));
                    break;
                }
                WE_LOG_MSG((0,0,"Connect Interface type is %d\n",iType));

                /*isec_new*/
                iRet = Sec_New(iType,iFd,&hSecHandle);
                if(iRet != G_SEC_OK)
                {
                    WE_LOG_MSG((0,0,"New instance failed\n",iType));
                    break;
                }
                /*add handle*/
                iRet = Secd_AddHandle(iFd,hSecHandle);
                if(iRet != G_SEC_OK)
                {
                    WE_LOG_MSG((0,0,"Add handle failed\n",iType));
                    break;
                }
           }
            break;
        case M_SEC_W_GET_CIPHER_SUITE:
            {
                iRet = SecSIc_GetCipherSuite(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_W_SETUP_CONN:
            {
                iRet = SecSIc_SetUpConn(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_W_STOP_CONN:
            {
                iRet = SecSIc_StopConn(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_W_REMOVE_PEER:
            {
                iRet = SecSIc_RemovePeer(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_W_ATTACH_PEER_TO_SESSION:
            {
                iRet = SecSIc_AttachPeerToSession(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_W_SEARCH_PEER:
            {
                iRet = SecSIc_SearchPeer(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_W_ENABLE_SESSION:
            {
                iRet = SecSIc_EnableSession(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_W_DISABLE_SESSION:
            {
                iRet = SecSIc_DisableSession(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_W_SESSION_GET:
            {
                iRet = SecSIc_SessionGet(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_W_SESSION_RENEW:
            {
                iRet = SecSIc_SessionRenew(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_W_GET_PRF_RESULT:
            {
                iRet = SecSIc_GetPrfResult(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_W_VERIFY_SVR_CERT_CHAIN:
            {
                iRet = SecSIc_VerifySvrCertChain(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_W_KEY_EXCHANGE:
            {
                iRet = SecSIc_KeyExchange(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_W_GET_USER_CERT:
            {
                iRet = SecSIc_GetUserCert(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_W_COMP_SIGN:
            {
                iRet = SecSIc_CompSign(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_W_SSL_HASH_UPDATE:
            {
                iRet = SecSIc_SslGetMasterSecret(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_W_EVT_SHOW_DLG_ACTION:
            {
                iRet = SecSIc_ShowDlgAction(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_B_GET_CERT_NAME_LIST:
            {
                iRet = SecSIc_GetCertNameList(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_B_GEN_KEY_PAIR:
            {
                iRet = SecSIc_GenKeyPairs(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        #ifdef G_SEC_CFG_SHOW_PIN
        case M_SEC_B_PIN_MODIFY:
            {
                iRet = SecSIc_PinModify(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        #endif
        case M_SEC_B_GET_CUR_SVR_CERT:
            {
                iRet = SecSIc_GetCurSvrCert(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_B_GET_SESSION_INFO:
            {
                iRet = SecSIc_GetSessionInfo(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_B_GET_WTLS_CUR_CLASS:
            {
                iRet = SecSIc_GetWtlsCurClass(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_B_GET_CONTRACT_LIST:
            {
                iRet = SecSIc_GetContractList(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_B_TRANSFER_CERT:
            {
                iRet = SecSIc_TransferCert(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_B_GET_REQUEST_USER_CERT:
            {
                iRet = SecSIc_GetRequestUserCert(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        #ifdef G_SEC_CFG_SHOW_PIN
        case M_SEC_B_EVT_CHANGE_PIN_ACTION:
            {
                iRet = SecSIc_EvtChangePinAction(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_B_EVT_CREATE_PIN_ACTION:
            {
                iRet = SecSIc_EvtCreatePinAction(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_B_EVT_PIN_ACTION:
            {
                iRet = SecSIc_EvtPinAction(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        #endif
        case M_SEC_B_EVT_HASH_ACTION:
            {
                iRet = SecSIc_EvtHashAction(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_B_EVT_SHOW_CERT_CONTENT_ACTION:
            {
                iRet = SecSIc_EvtShowCertContentAction(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_B_EVT_SHOW_CERT_LIST_ACTION:
            {
                iRet = SecSIc_EvtShowCertlistAction(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_B_EVT_SHOW_CONTRACTS_LIST_ACTION:
            {
                iRet = SecSIc_EvtShowContractsListAction(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_B_EVT_STORE_CERT_ACTION:
            {
                iRet = SecSIc_EvtStoreCertAction(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_B_EVT_SHOW_CONTRACT_CONTENT_ACTION:
            {
                iRet = SecSIc_EvtShowContractContentAction(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_B_EVT_CONFIRM_ACTION:
            {
                iRet = SecSIc_EvtConfirmAction(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_B_EVT_NAME_CONFIRM_ACTION:
            {
                iRet = SecSIc_EvtNameConfirmAction(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_S_HANDLE:
            {
                iRet = SecSIc_SigntextHandle(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        case M_SEC_S_EVT_SELECT_CERT_ACTION:
            {
                iRet = SecSIc_SigntextEvtSelectCertAction(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        #ifdef G_SEC_CFG_SHOW_PIN
        case M_SEC_S_EVT_PIN_ACTION:
            {
                iRet = SecSIc_SigntextEvtPinAction(hSecHandle, pvData);
                if(iRet != G_SEC_OK)
                {
                    return iRet;
                }
                break;
            }
        #endif
        /*other interface*/
        default:
            break;
    }
    return iRet;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_SigntextEvtPinAction                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/

#ifdef G_SEC_CFG_SHOW_PIN
static WE_INT32 SecSIc_SigntextEvtPinAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{   
    WE_INT32 iTargetID = 0;
    St_PinAction stPin = {0};
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_SigntextEvtPinAction\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( SecS_DecodeEvtPinAction(pvMsg, &iTargetID, &stPin) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_SigntextEvtPinAction\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SignText_EvtPinAction(hSecHandle, iTargetID, stPin);
    WE_FREE(stPin.pcPinValue);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
#endif
/*==================================================================================================
FUNCTION: 
    SecSIc_SigntextEvtSelectCertAction                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 SecSIc_SigntextEvtSelectCertAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{ 
    WE_INT32 iTargetID = 0;
    St_SelectCertAction stSelectCert = {0};
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_SigntextEvtSelectCertAction\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeEvtSelectCertAction(pvMsg, &iTargetID, &stSelectCert) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_SigntextEvtSelectCertAction\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SignText_EvtSelectCertAction(hSecHandle, iTargetID, stSelectCert);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_SigntextHandle                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 SecSIc_SigntextHandle(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{   
    WE_INT32 iTargetID = 0;
    WE_INT32 iSignId = 0;
    WE_INT32 iKeyIdType = 0;
    WE_INT32 iKeyIdLen = 0;
    WE_INT32 iStringToSignLen = 0;
    WE_INT32 iOptions = 0;
    WE_CHAR  *pcText = NULL;
    WE_CHAR  *pcKeyId = NULL;
    WE_CHAR  *pcStringToSign = NULL;
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_SigntextHandle\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeHandle(pvMsg, &iTargetID, &iSignId, &pcText, &iKeyIdType, &pcKeyId,
                        &iKeyIdLen, &pcStringToSign, &iStringToSignLen, &iOptions) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_SigntextHandle\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SignText_Handle(hSecHandle, iTargetID, iSignId, pcText,
                                 iKeyIdType, pcKeyId, iKeyIdLen,
                                 pcStringToSign, iStringToSignLen, iOptions);
    WE_FREE(pcText);
    WE_FREE(pcStringToSign);
    WE_FREE(pcKeyId);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_EvtNameConfirmAction                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 SecSIc_EvtNameConfirmAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{   
    WE_INT32 iTargetID = 0;
    St_NameConfirmAction stConfName = {0};
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_EvtNameConfirmAction\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeEvtNameConfirmAction(pvMsg, &iTargetID, &stConfName) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_EvtNameConfirmAction\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecB_EvtNameConfirmAction(hSecHandle, iTargetID, stConfName);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_EvtConfirmAction                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 SecSIc_EvtConfirmAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{   
    WE_INT32 iTargetID = 0;
    St_ConfirmAction stConfirm = {0};
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_EvtConfirmAction\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeEvtConfirmAction(pvMsg, &iTargetID, &stConfirm) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_EvtConfirmAction\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecB_EvtConfirmAction(hSecHandle, iTargetID, stConfirm);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_EvtShowContractContentAction                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 SecSIc_EvtShowContractContentAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{   
    WE_INT32 iTargetID = 0;
    St_ShowContractContentAction stShowContract = {0};
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_EvtShowContractContentAction\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DeocdeEvtShowContractContentAction(pvMsg, &iTargetID, &stShowContract) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_EvtShowContractContentAction\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecB_EvtShowContractContentAction(hSecHandle, iTargetID, stShowContract);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_EvtStoreCertAction                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 SecSIc_EvtStoreCertAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{   
    WE_INT32 iTargetID = 0;
    St_StoreCertAction stStoreCert = {0};
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_EvtStoreCertAction\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeEvtStoreCertAction(pvMsg, &iTargetID, &stStoreCert) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_EvtStoreCertAction\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecB_EvtStoreCertAction(hSecHandle, iTargetID, stStoreCert);
    WE_FREE(stStoreCert.pcText);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_EvtShowContractsListAction                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 SecSIc_EvtShowContractsListAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{   
    WE_INT32 iTargetID = 0;
    St_ShowContractsListAction stStoredContracts = {0};
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_EvtShowContractsListAction\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeEvtShowContractsListAction(pvMsg, &iTargetID, &stStoredContracts) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_EvtShowContractsListAction\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecB_EvtShowContractsListAction(hSecHandle, iTargetID, stStoredContracts);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_EvtShowCertlistAction                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 SecSIc_EvtShowCertlistAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{   
    WE_INT32 iTargetID = 0;
    St_ShowCertListAction stShowCertName = {0};
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_EvtShowCertlistAction\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeEvtShowCertListAction(pvMsg, &iTargetID, &stShowCertName) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_EvtShowCertlistAction\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecB_EvtShowCertListAction(hSecHandle, iTargetID, stShowCertName);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_EvtShowCertContentAction                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 SecSIc_EvtShowCertContentAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{   
    WE_INT32 iTargetID = 0;
    St_ShowCertContentAction stShowCertContent = {0};
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_EvtShowCertContentAction\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeEvtShowCertContentAction(pvMsg, &iTargetID, &stShowCertContent) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_EvtShowCertContentAction\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecB_EvtShowCertContentAction(hSecHandle, iTargetID, stShowCertContent);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_EvtHashAction                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 SecSIc_EvtHashAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{   
    WE_INT32 iTargetID = 0;
    St_HashAction stHash = {0};
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_EvtHashAction\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeEvtHashAction(pvMsg, &iTargetID,&stHash) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_EvtHashAction\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecB_EvtHashAction(hSecHandle, iTargetID, stHash);
    WE_FREE(stHash.pcHashValue);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_EvtPinAction                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/

#ifdef G_SEC_CFG_SHOW_PIN
static WE_INT32 SecSIc_EvtPinAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{   
    WE_INT32 iTargetID = 0;
    St_PinAction stPin = {0};
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_EvtPinAction\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeEvtPinAction(pvMsg, &iTargetID, &stPin) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_EvtPinAction\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecB_EvtPinAction(hSecHandle, iTargetID, stPin);
    WE_FREE(stPin.pcPinValue);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_EvtCreatePinAction                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 SecSIc_EvtCreatePinAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{   
    WE_INT32 iTargetID = 0;
    St_CreatePinAction stGenPin = {0};
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_EvtCreatePinAction\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeEvtCreatePinAction(pvMsg, &iTargetID, &stGenPin) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_EvtCreatePinAction\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecB_EvtCreatePinAction(hSecHandle, iTargetID, stGenPin);
    WE_FREE(stGenPin.pcPinValue);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_EvtChangePinAction                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 SecSIc_EvtChangePinAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{   
    WE_INT32 iTargetID = 0;
    St_ChangePinAction stChangePin = {0};
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_EvtChangePinAction\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeEvtChangePinAction(pvMsg, &iTargetID, &stChangePin) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_EvtChangePinAction\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecB_EvtChangePinAction(hSecHandle, iTargetID, stChangePin);
    WE_FREE(stChangePin.pcNewPinAgainValue);
    WE_FREE(stChangePin.pcNewPinValue);
    WE_FREE(stChangePin.pcOldPinValue);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
#endif
/*==================================================================================================
FUNCTION: 
    SecSIc_GetRequestUserCert                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 SecSIc_GetRequestUserCert(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{   
    St_SecUserCertReqInfo stUserCertReqInfo = {0};
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_GetRequestUserCert\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeGetRequestUserCert(pvMsg, &stUserCertReqInfo) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_GetRequestUserCert\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecB_GetRequestUserCert(hSecHandle, stUserCertReqInfo);
    WE_FREE(stUserCertReqInfo.pucCity);
    WE_FREE(stUserCertReqInfo.pucCompany);
    WE_FREE(stUserCertReqInfo.pucCountry);
    WE_FREE(stUserCertReqInfo.pucDepartment);
    WE_FREE(stUserCertReqInfo.pucEMail);
    WE_FREE(stUserCertReqInfo.pucName);
    WE_FREE(stUserCertReqInfo.pucProvince);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_TransferCert                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 SecSIc_TransferCert(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_INT32  iTargetID = 0;
    St_SecCertContent stCertContent = {0};
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_TransferCert\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeTransferCert(pvMsg, &stCertContent) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_TransferCert\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecB_TransferCert(hSecHandle, stCertContent);
    WE_FREE(stCertContent.pcMime);
    WE_FREE(stCertContent.pucCertData);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_GetContractList                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 SecSIc_GetContractList(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_INT32  iTargetID = 0;
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_GetContractList\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeGetContractsList(pvMsg, &iTargetID) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_GetContractList\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecB_GetContractsList(hSecHandle, iTargetID);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_GetWtlsCurClass                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 SecSIc_GetWtlsCurClass(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_INT32  iTargetID = 0;
    WE_INT32  iSecID = 0;
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_GetWtlsCurClass\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeGetWtlsCurClasss(pvMsg, &iTargetID, &iSecID) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_GetWtlsCurClass\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecB_GetWtlsCurClasss(hSecHandle, iTargetID, iSecID);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_GetSessionInfo                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 SecSIc_GetSessionInfo(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_INT32  iTargetID = 0;
    WE_INT32  iSecID = 0;
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_GetSessionInfo\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeGetSessionInfo(pvMsg, &iTargetID, &iSecID) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_GetSessionInfo\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecB_GetSessionInfo(hSecHandle, iTargetID, iSecID);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_GetCurSvrCert                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 SecSIc_GetCurSvrCert(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_INT32  iTargetID = 0;
    WE_INT32  iSecID = 0;
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_GetCurSvrCert\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeGetCurSvrCert(pvMsg, &iTargetID, &iSecID) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_GetCurSvrCert\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecB_GetCurSvrCert(hSecHandle, iTargetID, iSecID);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_PinModify                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/
#ifdef G_SEC_CFG_SHOW_PIN
static WE_INT32 SecSIc_PinModify(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_INT32  iTargetID = 0;
    WE_UINT8  ucKeyType = 0;
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_PinModify\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodePinModify(pvMsg, &iTargetID, &ucKeyType) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_PinModify\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecB_PinModify(hSecHandle, iTargetID, ucKeyType);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}

#endif
/*==================================================================================================
FUNCTION: 
    SecSIc_GenKeyPairs                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 SecSIc_GenKeyPairs(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_INT32  iTargetID = 0;
    WE_UINT8  ucKeyType = 0;
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_GenKeyPairs\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeGenKeyPair(pvMsg, &iTargetID, &ucKeyType) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_GenKeyPairs\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecB_GenKeyPair(hSecHandle, iTargetID, ucKeyType);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_GetCertNameList                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 SecSIc_GetCertNameList(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_INT32  iTargetID = 0;
    WE_INT32  iCertType = 0;
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_GetCertNameList\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeGetCertNameList(pvMsg, &iTargetID, &iCertType) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_GetCertNameList\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecB_GetCertNameList(hSecHandle, iTargetID, iCertType);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_ShowDlgAction                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 SecSIc_ShowDlgAction(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_INT32  iTargetID = 0;
    WE_VOID   *pvSrcData = NULL;
    WE_UINT32 uiLength = 0;
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_ShowDlgAction\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeEvtShowDlgAction(pvMsg, &iTargetID, &pvSrcData, &uiLength) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_ShowDlgAction\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecW_EvtShowDlgAction(hSecHandle, iTargetID, pvSrcData, uiLength);
    WE_FREE(pvSrcData);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_SslGetMasterSecret                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 SecSIc_SslGetMasterSecret(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_HANDLE hHashHandle = NULL;
    WE_INT32  iMasterSecretID = 0;
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_SslGetMasterSecret\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeSSLGetWMasterSec(pvMsg, &iMasterSecretID) < 0) //?
    {
        WE_LOG_MSG((0,0,"SecSIc_SslGetMasterSecret\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecW_SslGetWMasterSec(hSecHandle, iMasterSecretID);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_CompSign                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 SecSIc_CompSign(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_INT32  iTargetID = 0;
    WE_UCHAR  *pucKeyId = NULL;
    WE_INT32  iKeyIdLen = 0;
    WE_UCHAR  *pucBuf = NULL;
    WE_INT32  iBufLen = 0;
    WE_UINT8  ucConnType = 0;
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_CompSign\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeCompSign(pvMsg, &iTargetID, &pucKeyId, &iKeyIdLen, &pucBuf, &iBufLen, &ucConnType) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_CompSign\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecW_CompSign(hSecHandle, iTargetID, pucKeyId, iKeyIdLen,
                               pucBuf, iBufLen, ucConnType);
    WE_FREE(pucKeyId);
    WE_FREE(pucBuf);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_GetUserCert                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 SecSIc_GetUserCert(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_INT32  iTargetID = 0;
    WE_VOID   *pvBuf = NULL;
    WE_INT32  iBufLen = 0;
    WE_UINT8  ucConnType = 0;
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_GetUserCert\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeGetUserCert(pvMsg, &iTargetID, &pvBuf, &iBufLen, &ucConnType) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_GetUserCert\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecW_GetUserCert(hSecHandle, iTargetID, pvBuf, iBufLen, ucConnType);
    WE_FREE(pvBuf);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_KeyExchange                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 SecSIc_KeyExchange(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_INT32  iTargetID = 0;
    WE_VOID   *pvParam = NULL;
    WE_UINT8  ucAlg = 0;
    WE_UCHAR  *pucRandVal = NULL;
    WE_UINT8  ucConnType = 0;
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_KeyExchange\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeKeyExchange(pvMsg, &iTargetID, &pvParam, &ucAlg,
                                &pucRandVal, &ucConnType) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_KeyExchange\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecW_KeyExchange(hSecHandle, iTargetID, pvParam, ucAlg, &pucRandVal, ucConnType);
    WE_FREE(pvParam);
    WE_FREE(pucRandVal);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_VerifySvrCertChain                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 SecSIc_VerifySvrCertChain(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_INT32  iTargetID = 0;
    WE_VOID   *pvBuf = NULL;
    WE_INT32  iBufLen = 0;
    WE_UINT8  ucConnType = 0;
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_VerifySvrCertChain\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if ( Sec_DecodeVerifySvrCertChain(pvMsg, &iTargetID, &pvBuf, &iBufLen, &ucConnType) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_VerifySvrCertChain\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecW_VerifySvrCertChain(hSecHandle, iTargetID,
                                          pvBuf, iBufLen, ucConnType);
    WE_FREE(pvBuf);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_GetPrfResult                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 SecSIc_GetPrfResult(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_INT32  iTargetID = 0;
    WE_UINT8  ucAlg = 0;
    WE_INT32  iMasterSecretId = 0;
    WE_UCHAR  *pucSecret = NULL;
    WE_INT32  iSecretLen = 0;
    WE_CHAR   *pcLabel = NULL;
    WE_UCHAR  *pucSeed = NULL;
    WE_INT32  iSeedLen = 0;
    WE_INT32  iOutputLen = 0;
    WE_UINT8  ucConnType = 0;
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_GetPrfResult\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if (Sec_DecodeGetPrfResult(pvMsg, &iTargetID, &ucAlg, &iMasterSecretId, &pucSecret, 
                            &iSecretLen, &pcLabel, &pucSeed, &iSeedLen, &iOutputLen, &ucConnType) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_GetPrfResult\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecW_GetPrfResult(hSecHandle, iTargetID, ucAlg, iMasterSecretId, pucSecret, 
                                  iSecretLen, pcLabel, pucSeed, iSeedLen, iOutputLen, ucConnType);
    WE_FREE(pucSecret);
    WE_FREE(pcLabel);
    WE_FREE(pucSeed);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_SessionRenew                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 SecSIc_SessionRenew(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_UINT8  ucSessionOptions = 0;
    WE_INT32  iMasterSecretId = 0;
    WE_UCHAR  *pucSessionId = NULL;
    WE_UCHAR  ucSessionIdLen = 0;
    WE_UINT8  *pucCipherSuite = NULL;
    WE_UINT8  ucCompressionAlg = 0;
    WE_UCHAR  *pucPrivateKeyId = NULL;
    WE_UINT32 uiCreationTime = 0;
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_SessionRenew\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if (Sec_DecodeSessionRenew(pvMsg, &iMasterSecretId, &ucSessionOptions, &pucSessionId,
                         &ucSessionIdLen, &pucCipherSuite, &ucCompressionAlg,
                         &pucPrivateKeyId, &uiCreationTime) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_SessionRenew\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecW_SessionRenew(hSecHandle, iMasterSecretId, ucSessionOptions, pucSessionId, 
                                    ucSessionIdLen, pucCipherSuite, ucCompressionAlg, pucPrivateKeyId,
                                    uiCreationTime);
    WE_FREE(pucSessionId);
    WE_FREE(pucCipherSuite);
    WE_FREE(pucPrivateKeyId);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_SessionGet                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 SecSIc_SessionGet(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_INT32 iTargetID = 0;
    WE_INT32 iMasterSecretId = 0;
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_SessionGet\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if (Sec_DecodeSessionGet(pvMsg, &iTargetID, &iMasterSecretId) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_SessionGet\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecW_SessionGet(hSecHandle, iTargetID, iMasterSecretId);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_DisableSession                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 SecSIc_DisableSession(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_INT32  iMasterSecretId = 0;
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_DisableSession\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if (Sec_DecodeDisableSession(pvMsg, &iMasterSecretId) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_DisableSession\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecW_DisableSession(hSecHandle, iMasterSecretId);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_EnableSession                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 SecSIc_EnableSession(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_INT32  iMasterSecretId = 0;
    WE_UINT8  ucIsActive = 0;
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_EnableSession\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if (Sec_DecodeEnableSession(pvMsg, &iMasterSecretId, &ucIsActive) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_EnableSession\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecW_EnableSession(hSecHandle, iMasterSecretId, ucIsActive);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_SearchPeer                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 SecSIc_SearchPeer(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_UCHAR  *pucAddress = NULL;
    WE_INT32  iAddressLen = 0;
    WE_INT32  iTargetID = 0;
    WE_UINT16 usPortNum = 0;
    WE_UINT8  ucConnType = 0;
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_SearchPeer\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if (Sec_DecodeSearchPeer(pvMsg, &iTargetID, &pucAddress, &iAddressLen, &usPortNum, &ucConnType) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_SearchPeer\n"));
        return G_SEC_GENERAL_ERROR;
    }     
    
    uiResult = SecW_SearchPeer(hSecHandle, iTargetID, pucAddress, 
                                  iAddressLen, usPortNum, ucConnType);
    WE_FREE(pucAddress);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_AttachPeerToSession                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 SecSIc_AttachPeerToSession(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_UCHAR  *pucAddress = NULL;
    WE_INT32  iAddressLen = 0;
    WE_UINT16 usPortnum = 0;
    WE_INT32  iMasterSecretId = 0;
    WE_UINT8  ucConnType = 0;
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_AttachPeerToSession\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if (Sec_DecodeAttachPeerToSession(pvMsg, &pucAddress, &iAddressLen, &usPortnum, 
                                     &iMasterSecretId, &ucConnType) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_AttachPeerToSession\n"));
        return G_SEC_GENERAL_ERROR;
    } 
    
    uiResult = SecW_AttachPeerToSession(hSecHandle, pucAddress, iAddressLen,
                                        usPortnum, iMasterSecretId, ucConnType);
    WE_FREE(pucAddress);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_RemovePeer                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 SecSIc_RemovePeer(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_INT32 iMasterSecretId = 0;
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_RemovePeer\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if (Sec_DecodeRemovePeer(pvMsg, &iMasterSecretId) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_RemovePeer\n"));
        return G_SEC_GENERAL_ERROR;
    } 
    
    uiResult = SecW_RemovePeer(hSecHandle, iMasterSecretId);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_StopConn                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/

static WE_INT32 SecSIc_StopConn(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_INT32 iTargetID = 0;
    WE_UINT8 ucConnType = 0;
    WE_INT32 iSecurityId = 0;
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_StopConn\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if (Sec_DecodeStopConn(pvMsg, &iTargetID, &iSecurityId, &ucConnType) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_StopConn\n"));
        return G_SEC_GENERAL_ERROR;
    } 
    
    uiResult = SecW_StopConnection(hSecHandle, iTargetID, iSecurityId, ucConnType);
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_SetUpConn                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 SecSIc_SetUpConn(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_UINT8 ucConnType = 0;
    WE_INT32 iTargetID = 0;
    WE_INT32 iMasterSecretId = 0;
    WE_INT32 iSecurityId = 0;
    WE_INT32 iFullHandshake = 0;
    St_SecSessionInformation stSessionInfo = {0};
    WE_UINT32 uiResult = 0;
    
    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_SetUpConn\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    
    if (Sec_DecodeSetUpConn(pvMsg, &iTargetID, &iMasterSecretId, 
                    &iSecurityId, &iFullHandshake, &stSessionInfo, &ucConnType) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_SetUpConn\n"));
        return G_SEC_GENERAL_ERROR;
    } 
    
    uiResult = SecW_SetupConnection(hSecHandle, iTargetID, iMasterSecretId, 
                           iSecurityId, iFullHandshake, stSessionInfo, ucConnType);
    WE_FREE( stSessionInfo.pucCert );
    if (G_SEC_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   
    
    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION: 
    SecSIc_GetCipherSuite                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode data from client  and use the decoded data as invoked interface's parameter .  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN]:handle.
    WE_VOID *pvMsg[IN]:  encoded data from client.  
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 SecSIc_GetCipherSuite(WE_HANDLE hSecHandle, WE_VOID *pvMsg)
{
    WE_INT32  iTargetID = 0;
    WE_UINT8  ucConnType = 0;
    WE_UINT32 uiResult = 0;

    if (NULL == hSecHandle)
    {
        WE_LOG_MSG((0,0,"SecSIc_GetCipherSuite\n"));
        return G_SEC_INVALID_PARAMETER;
    }
    if (Sec_DecodeGetCipherSuite(pvMsg, &iTargetID, &ucConnType) < 0)
    {
        WE_LOG_MSG((0,0,"SecSIc_GetCipherSuite\n"));
        return G_SEC_GENERAL_ERROR;
    } 

    uiResult = SecW_GetCipherSuite(hSecHandle, iTargetID, ucConnType);
    if (M_SEC_ERR_OK != uiResult)
    {
       return G_SEC_GENERAL_ERROR; 
    }   

    return G_SEC_OK;
}

