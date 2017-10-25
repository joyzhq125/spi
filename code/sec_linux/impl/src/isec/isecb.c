/*==================================================================================================
    HEADER NAME : isecb.c
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
        
    GENERAL DESCRIPTION
        In this file,define the interface function prototype of invoked by browser
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2007 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2007-03-21   Wang Shuhua        None         Draft
==================================================================================================*/
/**************************************************************************************************/
/* Include File Section                                                                           */
/**************************************************************************************************/
#include "stdio.h"
#include "we_def.h"

#include "sec.h"
#include "isecw.h"
#include "isecb.h"
#include "isigntext.h"

#include "sec_uecst.h"
#include "sec_edc.h"

/*==================================================================================================
* Macro Define Section
==================================================================================================*/
#define ISEC_HANDLE_MAGIC_NUM 0x499602D2
#define UNIXSTR_PATH       "/tmp/.isec"    
#define MSG_SIZE 10240
#define WE_LOGERROR  printf
/*==================================================================================================
* Type Define Section
==================================================================================================*/   
typedef struct tagSt_ISecHandle
{
    WE_INT32 iMagicNumer;    /* for struct pointer valid check */
    WE_INT iSocketFd;   /* communicate with SecD*/
    Fn_ISecEventHandle cbEvtHandler;    /* client event handle callback */
    WE_VOID *pvCbData;    /* callback function parameter client registered */
    WE_INT32 iIFType;
}St_ISecHandle;

/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*==================================================================================================
FUNCTION: 
    ISecB_GetCertNameList
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    get certificate name list saved on the current device.
ARGUMENTS PASSED:
    ISecB *     pMe[IN]:pointer of ISecB instance.
    WE_INT32    iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_INT32    iMasterSecretId:ID of the master secret.  
RETURN VALUE:
    G_SEC_OK: success
    other: error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecB_GetCertNameList
(
    ISecB * pISec, 
    WE_INT32 iTargetID,
    WE_INT32 iCertType
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_AttachPeerToSession()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    iRet = Sec_EncodeGetCertNameList(&pvMsg,iTargetID,iCertType);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pISec, pvMsg);
    return uiRet;

}
/*==================================================================================================
FUNCTION: 
    ISecB_GenKeyPair
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    generate one key pair for the device.
ARGUMENTS PASSED:
    ISecB *     pMe[IN]:pointer of ISecB instance.
    WE_INT32    iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_UINT8    ucKeyType[IN]:The value of the key type.
RETURN VALUE:
    G_SEC_OK: success
    other: error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecB_GenKeyPair
(
    ISecB * pISec, 
    WE_INT32 iTargetID, 
    WE_UINT8 ucKeyType
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_AttachPeerToSession()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    iRet = Sec_EncodeGenKeyPair(&pvMsg,iTargetID,ucKeyType);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pISec, pvMsg);
    return uiRet;

}
#ifdef G_SEC_CFG_SHOW_PIN
/*==================================================================================================
FUNCTION: 
    ISecB_PinModify
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    modify the pin code for private key.
ARGUMENTS PASSED:
    ISecB *         pMe[IN]:pointer of ISecB instance.
    WE_INT32        iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_UINT8        ucKeyType[IN]:The value of the key type.
RETURN VALUE:
    G_SEC_OK: success
    other: error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecB_PinModify
(
    ISecB * pISec, 
    WE_INT32 iTargetID,
    WE_UINT8 ucKeyType
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_AttachPeerToSession()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    iRet = Sec_EncodePinModify(&pvMsg,iTargetID,ucKeyType);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pISec, pvMsg);
    return uiRet;

}
/*==================================================================================================
FUNCTION: 
    ISecB_EvtChangePinAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    feedback about changing pin to sec module.
ARGUMENTS PASSED:
    ISecB *        pMe[IN]:pointer of ISecB instance.
    St_ChangePinAction  stChangePin[IN]:The content of the change pin.
RETURN VALUE:
    G_SEC_OK: success
    other: error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecB_EvtChangePinAction
(
    ISecB * pISec,
    WE_INT32 iTargetID,
    St_ChangePinAction stChangePin
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_AttachPeerToSession()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    iRet = Sec_EncodeEvtChangePinAction(&pvMsg,iTargetID,stChangePin);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pISec, pvMsg);
    return uiRet;
}
/*==================================================================================================
FUNCTION: 
    ISecB_EvtCreatePinAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    when pin is generated, this interface should be invoked.
ARGUMENTS PASSED:
    ISecB *               pMe[IN]:pointer of ISecB instance.
    St_CreatePinAction    stGenPin[IN]:The value of the GenPin
RETURN VALUE:
    G_SEC_OK: success
    other: error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecB_EvtCreatePinAction
(
    ISecB * pISec,
    WE_INT32 iTargetID,
    St_CreatePinAction stGenPin
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_AttachPeerToSession()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    iRet = Sec_EncodeEvtCreatePinAction(&pvMsg,iTargetID,stGenPin);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pISec, pvMsg);
    return uiRet;
}
/*==================================================================================================
FUNCTION: 
    ISecB_EvtPinAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    when a event is sent to invoker to ask a pin code to do specific something, this interface
    should be invoked to give a feedback to sec module.
ARGUMENTS PASSED:
    ISecB *         pMe[IN]:pointer of ISecB instance.
    St_PinAction    stPin[IN]:The value of the pin structure.
RETURN VALUE:
    G_SEC_OK: success
    other: error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecB_EvtPinAction
(
    ISecB * pISec,
    WE_INT32 iTargetID,
    St_PinAction stPin
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_AttachPeerToSession()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    iRet = Sec_EncodeEvtPinAction(&pvMsg,iTargetID,stPin);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pISec, pvMsg);
    return uiRet;
}

#endif
/*==================================================================================================
FUNCTION: 
    ISecB_GetCurSvrCert
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    get the detailed information about the current used certificate.
ARGUMENTS PASSED:
    ISecB *         pMe[IN]:pointer of ISecB instance.
    WE_INT32        iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_INT32        iSecID[IN]:ID of the security.
RETURN VALUE:
    G_SEC_OK: success
    other: error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecB_GetCurSvrCert
(
    ISecB * pISec, 
    WE_INT32 iTargetID, 
    WE_INT32 iSecID
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_AttachPeerToSession()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    iRet = Sec_EncodeGetCurSvrCert(&pvMsg,iTargetID,iSecID);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pISec, pvMsg);
    return uiRet;
}
/*==================================================================================================
FUNCTION: 
    ISecB_GetSessionInfo
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    get the current session information.
ARGUMENTS PASSED:
    ISecB *     pMe[IN]:pointer of ISecB instance.
    WE_INT32    iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_INT32    iSecID[IN]:ID of the security.
RETURN VALUE:
    G_SEC_OK: success
    other: error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecB_GetSessionInfo
(
    ISecB * pISec, 
    WE_INT32 iTargetID, 
    WE_INT32 iSecID
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_AttachPeerToSession()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    iRet = Sec_EncodeGetSessionInfo(&pvMsg,iTargetID,iSecID);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pISec, pvMsg);
    return uiRet;
}
/*==================================================================================================
FUNCTION: 
    ISecB_GetWtlsCurClass
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    get current sec class
ARGUMENTS PASSED:
    ISecB *     pMe[IN]:pointer of ISecB instance.
    WE_INT32    iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_INT32    iSecID[IN]:ID of the security.
RETURN VALUE:
    G_SEC_OK: success
    other: error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecB_GetWtlsCurClass
(
    ISecB * pISec, 
    WE_INT32 iTargetID, 
    WE_INT32 iSecID
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_AttachPeerToSession()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    iRet = Sec_EncodeGetWtlsCurClasss(&pvMsg,iTargetID,iSecID);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pISec, pvMsg);
    return uiRet;
}
/*==================================================================================================
FUNCTION: 
    ISecB_GetContractsList
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    get contract name list
ARGUMENTS PASSED:
    ISecB *     pMe[IN]:pointer of ISecB instance.
    WE_INT32    iTargetID[IN]:the identity of the invoker ,input from the interface.
RETURN VALUE:
    G_SEC_OK: success
    other: error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecB_GetContractsList
(
    ISecB * pISec, 
    WE_INT32 iTargetID
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_AttachPeerToSession()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    iRet = Sec_EncodeGetContractsList(&pvMsg,iTargetID);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pISec, pvMsg);
    return uiRet;
}
/*==================================================================================================
FUNCTION: 
    ISecB_TransferCert
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    save certificate. when one cert is got from client, this interface can be invoked and 
    certificate storage flow will be executed.
ARGUMENTS PASSED:
    ISecB *              pMe[IN]:pointer of ISecB instance.
    St_SecCertContent    stCertContent[IN]:The content of the cert. 
RETURN VALUE:
    G_SEC_OK: success
    other: error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecB_TransferCert
(
    ISecB * pISec,
    St_SecCertContent stCertContent
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_AttachPeerToSession()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    iRet = Sec_EncodeTransferCert(&pvMsg,stCertContent);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pISec, pvMsg);
    return uiRet;
}
/*==================================================================================================
FUNCTION: 
    ISecB_GetRequestUserCert
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Get request of user certificate in base64-DER/DER code. After key pairs are generated,
    this interface can be used to get the user cert using the key pairs.
ARGUMENTS PASSED:
    ISecB *                 pMe[IN]:pointer of ISecB instance.
    St_SecUserCertReqInfo   stUserCertReqInfo[IN]:The infomation of request about user cert.  
RETURN VALUE:
    G_SEC_OK: success
    other: error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecB_GetRequestUserCert
(
    ISecB * pISec,
    St_SecUserCertReqInfo stUserCertReqInfo
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_AttachPeerToSession()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    iRet = Sec_EncodeGetRequestUserCert(&pvMsg,stUserCertReqInfo);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pISec, pvMsg);
    return uiRet;
}
/*==================================================================================================
FUNCTION: 
    ISecB_EvtHashAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    this interface is used to input hash value for verifying when trusted ca info is saved.
ARGUMENTS PASSED:
    ISecB *        pMe[IN]:pointer of ISecB instance.
    St_HashAction   stHash[IN]:The hash action.  
RETURN VALUE:
    G_SEC_OK: success
    other: error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecB_EvtHashAction
(
    ISecB * pISec,
    WE_INT32 iTargetID,
    St_HashAction stHash
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_AttachPeerToSession()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    iRet = Sec_EncodeEvtHashAction(&pvMsg,iTargetID,stHash);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pISec, pvMsg);
    return uiRet;
}
/*==================================================================================================
FUNCTION: 
    ISecB_EvtShowCertContentAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    after detailed information about one certificate is showed, this interface can be invoked
    to do further operation, for example, delete certificate.
ARGUMENTS PASSED:
   ISecB *        pMe[IN]:pointer of ISecB instance.
   St_ShowCertContentAction stShowCer[IN]:Structure of the show cert action.
RETURN VALUE:
    G_SEC_OK: success
    other: error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecB_EvtShowCertContentAction
(
    ISecB * pISec,
    WE_INT32 iTargetID,
    St_ShowCertContentAction stShowCertContent
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_AttachPeerToSession()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    iRet = Sec_EncodeEvtShowCertContentAction(&pvMsg, iTargetID, stShowCertContent);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pISec, pvMsg);
    return uiRet;
}
/*==================================================================================================
FUNCTION: 
    ISecB_EvtShowCertListAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    after cert name list is transferred to client, this interface can be invoked to 
    view detailed information about certificate.
ARGUMENTS PASSED:
    ISecB *        pMe[IN]:pointer of ISecB instance.
    St_ShowCertListAction   stShowCertName[IN]:The structure of the show cert name.
RETURN VALUE:
    G_SEC_OK: success
    other: error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecB_EvtShowCertListAction
(
    ISecB * pISec,
    WE_INT32 iTargetID,
    St_ShowCertListAction stShowCertName
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_AttachPeerToSession()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    iRet = Sec_EncodeEvtShowCertListAction(&pvMsg,iTargetID,stShowCertName);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pISec, pvMsg);
    return uiRet;
}
/*==================================================================================================
FUNCTION: 
    ISecB_EvtShowContractsListAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    after getting the contract list, this interface should be run to give a feedback to sec
    module, for example, whether to view one contract information or not.
ARGUMENTS PASSED:
    ISecB *        pMe[IN]:pointer of ISecB instance.
    St_ShowContractsListAction    stStoredContracts[IN]:The structure of the stored contracts.
RETURN VALUE:
    G_SEC_OK: success
    other: error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecB_EvtShowContractsListAction
(
    ISecB * pISec,
    WE_INT32 iTargetID,
    St_ShowContractsListAction stStoredContracts
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_AttachPeerToSession()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    iRet = Sec_EncodeEvtShowContractsListAction(&pvMsg,iTargetID,stStoredContracts);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pISec, pvMsg);
    return uiRet;
}
/*==================================================================================================
FUNCTION: 
    ISecB_EvtStoreCertAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    before one certificate is saved, this interface should be invoked to give some information
    to sec module about name of certificate.
ARGUMENTS PASSED:
    ISecB *        pMe[IN]:pointer of ISecB instance.
    St_StoreCertAction  stStoreCert[IN]:The structure of the store cert.
RETURN VALUE:
    G_SEC_OK: success
    other: error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecB_EvtStoreCertAction
(
    ISecB * pISec,
    WE_INT32 iTargetID,
    St_StoreCertAction stStoredCert
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_AttachPeerToSession()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    iRet = Sec_EncodeEvtStoreCertAction(&pvMsg,iTargetID,stStoredCert);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pISec, pvMsg);
    return uiRet;
}
/*==================================================================================================
FUNCTION: 
    ISecB_EvtShowContractContentAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    after one contract information is showed, this function should be invoked to give 
    feedback to sec module.
ARGUMENTS PASSED:
    ISecB *        pMe[IN]:pointer of ISecB instance.
    St_ShowContractContentAction    stShowContract[IN]:The structure of the show contract.
RETURN VALUE:
    G_SEC_OK: success
    other: error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecB_EvtShowContractContentAction
(
    ISecB * pISec,
    WE_INT32 iTargetID,
    St_ShowContractContentAction stShowContract
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_AttachPeerToSession()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    iRet = Sec_EnocdeEvtShowContractContentAction(&pvMsg,iTargetID,stShowContract);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pISec, pvMsg);
    return uiRet;
}
/*==================================================================================================
FUNCTION: 
    ISecB_EvtConfirmAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    after the invoker receive the confirm event ,this function should be run to give a 
    feedback to sec module.
ARGUMENTS PASSED:
    ISecB *             pMe[IN]:pointer of ISecB instance.
    St_ConfirmAction    stConfirm[IN]:The value of the confirm.
RETURN VALUE:
    G_SEC_OK: success
    other: error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecB_EvtConfirmAction
(
    ISecB * pISec,
    WE_INT32 iTargetID,
    St_ConfirmAction stConfirm
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_AttachPeerToSession()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    iRet = Sec_EncodeEvtConfirmAction(&pvMsg,iTargetID,stConfirm);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pISec, pvMsg);
    return uiRet;
}
/*==================================================================================================
FUNCTION: 
    ISecB_EvtNameConfirmAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    when a confirm information is receive , the interface should be run to give the sec 
    module a feedback.
ARGUMENTS PASSED:
    ISecB *                 pMe[IN]:pointer of ISecB instance.
    St_NameConfirmAction    stConfName[IN]:The name of the confirm.
RETURN VALUE:
    G_SEC_OK: success
    other: error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecB_EvtNameConfirmAction
(
    ISecB * pISec,
    WE_INT32 iTargetID,
    St_NameConfirmAction stConfName
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_AttachPeerToSession()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    iRet = Sec_EncodeEvtNameConfirmAction(&pvMsg,iTargetID,stConfName);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pISec, pvMsg);
    return uiRet;
}

