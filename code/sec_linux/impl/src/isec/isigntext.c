/*==================================================================================================
    HEADER NAME : isigntext.c
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
    ISignText_Handle
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
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
WE_UINT32 ISignText_Handle
(
    ISignText * pISec,
    WE_INT32 iTargetID,
    WE_INT32 iSignId,  
    WE_CHAR * pcText,
    WE_INT32 iKeyIdType, 
    WE_CHAR * pcKeyId,
    WE_INT32 iKeyIdLen,
    WE_CHAR * pcStringToSign,
    WE_INT32 iStringToSignLen,
    WE_INT32 iOptions
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
    iRet = Sec_EncodeHandle(&pvMsg,
                            iTargetID,
                            iSignId,  
                            pcText,
                            iKeyIdType, 
                            pcKeyId,
                            iKeyIdLen,
                            pcStringToSign,
                            iStringToSignLen,
                            iOptions);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pstISec,pvMsg);
    return uiRet;

}


/*==================================================================================================
FUNCTION: 
    ISignText_EvtSelectCertAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
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
WE_UINT32 ISignText_EvtSelectCertAction
(
    ISignText * pISec,
    WE_INT32 iTargetID,
    St_SelectCertAction stSelectCert
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
    iRet = Sec_EncodeEvtSelectCertAction(&pvMsg,
                                        iTargetID,
                                        stSelectCert);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pstISec,pvMsg);
    return uiRet;

}


#ifdef G_SEC_CFG_SHOW_PIN
/*==================================================================================================
FUNCTION: 
    ISignText_EvtPinAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
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
WE_UINT32 ISignText_EvtPinAction
(
    ISignText * pISec,
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
    iRet = SecS_EncodeEvtPinAction(&pvMsg,
                                  iTargetID,
                                  stPin);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pstISec,pvMsg);
    return uiRet;

}


#endif


