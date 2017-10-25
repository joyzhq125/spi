/*=================================================================================================
    MODULE NAME : sec_wimpkcresp.c
    MODULE NAME : SEC

    GENERAL DESCRIPTION    
        This file offers several functions to realize the PKC response 
        with the asynchronism way. The Sec_HandleWimMain is the entrance 
        and process the message received. And some function is used to 
        manage the element queue list.
        
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by Techfaith Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                  Tracking
    Date          Author          Number      Description of changes
    ----------   --------------   ---------   --------------------------------------
    2006-07-11     Sam              None           Init  

==================================================================================================*/

/*=================================================================================================
    Include File Section
 *================================================================================================*/
#include "sec_comm.h"

/***************************************************************************************************
* Macro Define Section
***************************************************************************************************/
#define SEC_HEAD_ELEMENT            (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pstHeadElement)

/***************************************************************************************************
* Type Define Section
***************************************************************************************************/

/**************************************************************************************************
* Prototype Declare Section
***************************************************************************************************/
static void Sec_HandleWimMain( WE_HANDLE hSecHandle, St_SecMessage *pstMessage );

static void Sec_WimHandlePubKeyRSAEncryptResp
(
    WE_HANDLE                   hSecHandle, 
    St_SecCrptEncryptPkcResp    *pstParam 
);

static void Sec_WimHandleVerifyRSASignatureResp
(
    WE_HANDLE                       hSecHandle, 
    St_SecCrptVerifySignatureResp   *pstParam 
);

static void Sec_WimHandleComputeRSADigSignResp
(
    WE_HANDLE                       hSecHandle, 
    St_SecCrptComputeSignatureResp  *pstParam 
);

static void Sec_WimHandleGenerateRSAKeypairsResp
(
    WE_HANDLE                       hSecHandle, 
    St_SecCrptGenKeypairResp   *pstParam 
);

/*==================================================================================================
FUNCTION: 
    Sec_WimInit
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION:
    Initialise the WIM submodule and register the function pointer Sec_HandleWimMain.
ARGUMENTS PASSED: 
    hSecHandle[IO]: Global data handle.
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
void Sec_WimInit( WE_HANDLE hSecHandle )
{
    if (NULL == hSecHandle)
    {
        return ;
    }
    /* register function pointer: Sec_HandleWimMain */
    Sec_MsgRegister( hSecHandle, M_SEC_MSG_DST_WIM_MAIN, Sec_HandleWimMain );

    /* initialize element queue list */
    SEC_HEAD_ELEMENT = NULL;
}

/*==================================================================================================
FUNCTION: 
    Sec_WimGetSpecElement
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION:
    Get the element pointer of specific element from queue.
ARGUMENTS PASSED: 
    hSecHandle[IO]: Global data handle.
    iTargetID [IN]: kinds of ID, such as certID, object ID, and so on.
RETURN VALUE:
    A pointer to specific structure of St_SecWimElement.
USED GLOBAL VARIABLES: 
    None
USED STATIC VARIABLES:
    None
CALL BY: 
    Omit
IMPORTANT NOTES: 
    None
==================================================================================================*/
St_SecWimElement* Sec_WimGetSpecElement( WE_HANDLE hSecHandle, WE_INT32 iTargetID )
{
    #if 0
    St_SecWimElement *pstElement = NULL;
    St_SecWimElement *pstParentElement =  NULL;

    if (NULL == hSecHandle)
    {
        return NULL;
    }

    for (pstElement = SEC_HEAD_ELEMENT, 
         pstParentElement = SEC_HEAD_ELEMENT; 
         pstElement != NULL;
         pstElement = pstElement->pstNext)
    {/* judge accroding to the iTargetID */
        if (pstElement->iTargetID == iTargetID) 
        {
            if (pstElement == SEC_HEAD_ELEMENT)
            {/* head pointer to the next */
                SEC_HEAD_ELEMENT = pstElement->pstNext;
            }
            else
            {/* get the element pointer */ 
                pstParentElement->pstNext = pstElement->pstNext;
            }
            return pstElement;
        }
        pstParentElement = pstElement;
    }
    return NULL;
    #else
    St_SecWimElement *pstElement = NULL;
    iTargetID = iTargetID;
    pstElement = SEC_HEAD_ELEMENT;
    if(pstElement)
    {
        SEC_HEAD_ELEMENT = pstElement->pstNext;
    }
    return pstElement;
    #endif
}

/*==================================================================================================
FUNCTION: 
    Sec_WimAddNewElement
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION:
    Evaluat the new element and add  to the queue.
ARGUMENTS PASSED: 
    hSecHandle[IO]: Global data handle.
    iTargetID[IN] : kinds of ID, such as certID, object ID, and so on.
    iState[IN] : macro defined, express state.
    pvPointer[IN] : pointer to memory.
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
WE_INT32 Sec_WimAddNewElement
(
    WE_HANDLE   hSecHandle, 
    WE_INT32    iTargetID, 
    WE_INT32    iState, 
    void        *pvPointer 
)
{
    St_SecWimElement *pstElement = NULL;
    pstElement = WE_MALLOC(sizeof(St_SecWimElement));
    /* malloc failed */
    if (NULL == pstElement)
    {
        if (NULL != pvPointer)
        {
            WE_FREE(pvPointer);
            pvPointer = NULL;
        }
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    /* evaluate */
    pstElement->iTargetID = iTargetID;
    pstElement->iState = iState;
    pstElement->hItype = hSecHandle; /*add by bird 061114*/
    pstElement->pvPointer = pvPointer;
    /* insert */
    Sec_WimAddPreassignElement( hSecHandle, pstElement );
    return M_SEC_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    Sec_WimAddPreassignElement
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION:
    Add the element to the head of the queue.
ARGUMENTS PASSED: 
    hSecHandle[IO]: Global data handle.
    pstElement[IN]: pointer to the element structure.
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
void Sec_WimAddPreassignElement( WE_HANDLE hSecHandle, St_SecWimElement *pstElement )
{
    if ((NULL == hSecHandle) || (NULL == pstElement))
    {
        return ;
    }
    /* insert to the head of element queue */
    pstElement->pstNext = SEC_HEAD_ELEMENT;
    SEC_HEAD_ELEMENT = pstElement;
}
/*==================================================================================================
FUNCTION: 
    Sec_WimDelElementById
CREATE DATE:
    2006-11-14
AUTHOR:
    Bird
DESCRIPTION:
    delete node from list by interface id
ARGUMENTS PASSED: 
    hSecHandle[IO]: Global data handle.
    hItype[IN]: interface id
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

void Sec_WimDelElementById(WE_HANDLE hSecHandle)
{
    St_SecWimElement *pstElement = NULL;
    St_SecWimElement *pstParentElement =  NULL;
    WE_INT32 iFlag = 0;
    if (NULL == hSecHandle)
    {
        return ;
    }

    pstElement = SEC_HEAD_ELEMENT, 
    pstParentElement = SEC_HEAD_ELEMENT; 
    
    while(pstElement != NULL)
    {/* judge accroding to the hItype */
        if (pstElement->hItype == hSecHandle) 
        {
            if (pstElement == SEC_HEAD_ELEMENT)
            {/* head pointer to the next */
                SEC_HEAD_ELEMENT = pstElement->pstNext;
                iFlag = 1;
            }
            else
            {/* get the element pointer */ 
                pstParentElement->pstNext = pstElement->pstNext;
                iFlag= 0;
            }
            /*Free memory*/
            Sec_WimFreeRsaElement(hSecHandle, pstElement);
            pstElement = pstParentElement;
        }
        else
        {
              iFlag= 0;
        }
        if(1 == iFlag)
        {
            pstElement = SEC_HEAD_ELEMENT;
            pstParentElement = SEC_HEAD_ELEMENT; 

         }
        else
        {
            pstParentElement = pstElement;
            pstElement = pstElement->pstNext;
        }
    }
}

void Sec_WimRemoveElement(WE_HANDLE hSecHandle)
{
    if (NULL == hSecHandle)
    {
        return ;
    }

    while(NULL != SEC_HEAD_ELEMENT)
    {
        Sec_WimFreeRsaElement(hSecHandle, SEC_HEAD_ELEMENT);
        SEC_HEAD_ELEMENT = SEC_HEAD_ELEMENT->pstNext;
        
    }
}
/*==================================================================================================
FUNCTION: 
    Sec_HandleWimMain
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION:
    Processed the message received.
ARGUMENTS PASSED: 
    hSecHandle[IO]: Global data handle.
    pstMessage[IN]: pointer to the message which stored the state of response.
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
static void Sec_HandleWimMain( WE_HANDLE hSecHandle, St_SecMessage *pstMessage )
{
    if (NULL == pstMessage)
    {
        return ;
    }
    /* message type */
    switch (pstMessage->iType) 
    {
        case M_SEC_DP_MSG_ENCRYPT_PKC_RESP:
            WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, 0,
                  "SWWIM: received M_SEC_DP_MSG_ENCRYPT_PKC_RESP\n"));
            Sec_WimHandlePubKeyRSAEncryptResp( pstMessage->hItype, pstMessage->pvParam );
            break;

        case M_SEC_DP_MSG_VERIFY_SIGN_RESP: 
            WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, 0,
                  "SWWIM: received E_SEC_MSG_VERIFY_SIGN_RESP\n"));
            Sec_WimHandleVerifyRSASignatureResp( pstMessage->hItype, pstMessage->pvParam );
            break;
            
        case M_SEC_DP_MSG_COMPUTE_SIGN_RESP:  
            WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, 0,
                  "SWWIM: received E_SEC_MSG_COMPUTE_SIGN_RESP\n"));
            Sec_WimHandleComputeRSADigSignResp( pstMessage->hItype, pstMessage->pvParam );
            break;
            
        case M_SEC_DP_MSG_GEN_KEYPAIR_RESP:  
            WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, 0,
                  "SWWIM: received E_SEC_MSG_GEN_KEYPAIR_RESP\n"));
            Sec_WimHandleGenerateRSAKeypairsResp( pstMessage->hItype, pstMessage->pvParam );
            break;

        default :
            WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, 0,
                  "SWWIM: received unknown signal\n"));
            break;
    }
    /* free the resource */
    WE_SIGNAL_DESTRUCT (0, (WE_UINT16)pstMessage->iType, pstMessage->pvParam);
    pstMessage->pvParam = NULL;
    Sec_MsgRemove( hSecHandle, pstMessage );
}

/*==================================================================================================
FUNCTION: 
    Sec_WimHandlePubKeyRSAEncryptResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION:
    The response of public key enctyption.
ARGUMENTS PASSED: 
    hSecHandle[IO]: Global data handle.
    pstParam[IN]: pointer to the structure of St_SecCrptEncryptPkcResp.
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
static void Sec_WimHandlePubKeyRSAEncryptResp
(
    WE_HANDLE                   hSecHandle, 
    St_SecCrptEncryptPkcResp    *pstParam 
)
{
    WE_INT32 iTargetID = 0;
    St_SecWimElement *pstElement = NULL;

    /*modified by Bird 070118 */
    pstElement = Sec_WimGetSpecElement( hSecHandle, iTargetID );
    if (NULL == pstElement) 
    {    
        return;
    }

    if (M_SEC_WIM_WTLS_KEYEXCHANGE_RSA == pstElement->iState)
    {
        Sec_iWimWtlsKeyExRsaResp( hSecHandle, pstElement, pstParam);
    }
    else if(M_SEC_WIM_TLS_KEYEXCHANGE_RSA==pstElement->iState)
    {
        Sec_iWimTlsKeyExRsaResp( hSecHandle, pstElement, pstParam);
    }
}

/*==================================================================================================
FUNCTION: 
    Sec_WimHandleVerifyRSASignatureResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION:
    The response of verifing signature. According to the state.
ARGUMENTS PASSED: 
    hSecHandle[IO]: Global data handle.
    pstParam[IN]: pointer to the structure of St_SecCrptVerifySignatureResp.
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
static void Sec_WimHandleVerifyRSASignatureResp
(
    WE_HANDLE                       hSecHandle, 
    St_SecCrptVerifySignatureResp   *pstParam 
)
{
    WE_INT32  iTargetID = 0;
    St_SecWimElement  *pstElement = NULL;

    /* get the element pointer according to the iTargetID */
    pstElement = Sec_WimGetSpecElement( hSecHandle, iTargetID );

    if (NULL == pstElement) 
    {    
        return;
    }
    /* response according to the state */
    switch (pstElement->iState)
    {
        case M_SEC_WIM_FIND_MATCHING_PRIVATE_KEY_VERIFY:
            Sec_iWimGetFitedPrivKeyResp( hSecHandle, pstElement, pstParam );
            break;
            
        case M_SEC_WIM_VERIFY_ROOT_CERTIFICATE:
            Sec_iWimChkCACertResp( hSecHandle, pstElement, pstParam );
            break;
            
        case M_SEC_WIM_SIGNATURE_VERIFICATION:
            Sec_iWimVerifySignResp( hSecHandle, pstElement, pstParam );
            break;
            
        case M_SEC_WIM_VERIFY_CERT_CHAIN_INTERMEDIATE:
            Sec_iWimVerifyCertInterResp( hSecHandle, pstElement, pstParam );
            break;
            
        case M_SEC_WIM_VERIFY_CERT_CHAIN_ROOT:     
            Sec_iWimVerifyCertRootResp( hSecHandle, pstElement, pstParam );
            break;
            
        case M_SEC_WIM_CHECK_PRESTORED_ROOT_CERT:
            Sec_WimCheckNonVerifiedCertResp( hSecHandle, pstElement, pstParam );
            break;

        default :
            return;
    }
}

/*==================================================================================================
FUNCTION: 
    Sec_WimHandleComputeRSADigSignResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION:
    The response of computing signature. According to the state.
ARGUMENTS PASSED: 
    hSecHandle[IO]: Global data handle.
    pstParam[IN]: pointer to the structure of St_SecCrptComputeSignatureResp.
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
static void Sec_WimHandleComputeRSADigSignResp
(
    WE_HANDLE                       hSecHandle, 
    St_SecCrptComputeSignatureResp  *pstParam 
)
{
    WE_INT32  iTargetID = 0;
    St_SecWimElement  *pstElement = NULL;

    /* get the element pointer according to the iTargetID */
    pstElement = Sec_WimGetSpecElement( hSecHandle, iTargetID );

    if (NULL == pstElement) 
    {    
        return;
    }

    /* response according to the state */
    switch (pstElement->iState)
    {
        case M_SEC_WIM_FIND_MATCHING_PRIVATE_KEY_COMPUTE:
            Sec_iWimGetFitedPrivKeyResp( hSecHandle, pstElement, pstParam );
            break;
            
        case M_SEC_WIM_PRODUCE_PUBKEY_CERT:
            Sec_iWimKeyPairGenSignResp( hSecHandle, pstElement, pstParam );
            break;
            
        case M_SEC_WIM_SIGNTEXT:
            Sec_iWimSignTextResp( hSecHandle, pstElement, pstParam );
            break;
            
        case M_SEC_WIM_COMPUTE_SIGNATURE:
            Sec_iWimWtlsCalSignResp( hSecHandle, pstElement, pstParam );
            break;
            
        case M_SEC_WIM_GET_USER_CERT_AND_SIGNED_KEY:
            Sec_iWimGetUserCertAndPrivKeyResp( hSecHandle, pstElement, pstParam );
            break;
        case M_SEC_WIM_USER_CERT_REQ_COMPUTE_SIGNATURE:
            Sec_iWimUserCertReqResp( hSecHandle, pstElement, pstParam );
            break;
            
        default :
            return;
    }
}

/*==================================================================================================
FUNCTION: 
    Sec_WimHandleGenerateRSAKeypairsResp
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION:
    The response of generating key pairs. According to the state.
ARGUMENTS PASSED: 
    hSecHandle[IO]: Global data handle.
    pstParam[IN]: pointer to the structure of St_SecCrptGenKeypairResp.
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
static void Sec_WimHandleGenerateRSAKeypairsResp
(
    WE_HANDLE                   hSecHandle, 
    St_SecCrptGenKeypairResp    *pstParam 
)
{
    WE_INT32  iTargetID = 0;
    St_SecWimElement  *pstElement = NULL;
    
    /* get the element pointer according to the iTargetID */
    pstElement = Sec_WimGetSpecElement( hSecHandle, iTargetID );

    if (NULL == pstElement) 
    {    
        return;
    }

    if (M_SEC_WIM_GENERATE_KEYPAIR == pstElement->iState) 
    {
        Sec_iWimKeyPairGenResp( hSecHandle, pstElement, pstParam );
    }
}







