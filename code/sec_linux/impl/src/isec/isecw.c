/*==================================================================================================
    HEADER NAME : isecw.c
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
        
    GENERAL DESCRIPTION
        In this file,define the interface function prototype of invoked by wap
    
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
#include "we_def.h"
#include "we_mem.h"

#include "sec.h"  /*Shared*/
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
    ISecW_SetUpConnection
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    set up session connection
ARGUMENTS PASSED:
    ISecW *             pMe[IN]: pointer of ISecW instance.
    WE_INT32            iTargetID[IN]: the identity of the invoker ,input from the interface.
    WE_INT32            iMasterSecretId[IN]:ID of the master secret.
    WE_INT32            iSecurityId[IN]:ID of the security.
    WE_INT32            iFullHandshake[IN]:whether is full hand shake.
    St_SecSessionInformation    stSessionInfo[IN]:Information of the session.  
    WE_UINT8        ucConnType: TLS or wtls    
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
WE_UINT32 ISecW_SetUpConnection
(
    ISecW * pISec, 
    WE_INT32 iTargetID,
    WE_INT32 iMasterSecretId, 
    WE_INT32 iSecId, 
    WE_INT32 iFullHandshake,
    St_SecSessionInformation stSessionInfo,
    WE_UINT8   ucConnType
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT        iFd = -1;; 
    WE_UINT32  uiRet = 0;
    WE_INT32 iRet = -1;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_SetUpConnection()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    if(-1 == (iFd = ISec_GetSocketFd(pstISec)))
    {
        WE_LOGERROR("ISecW_SetUpConnection()_____ invalid file description\r\n"); 
        return G_SEC_BAD_HANDLE;                                     
    }  
    iRet = Sec_EncodeSetUpConn(&pvMsg,
                         iTargetID,iMasterSecretId,iSecId,iFullHandshake,stSessionInfo,ucConnType);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pstISec,pvMsg);
    return uiRet;
}
/*==================================================================================================
FUNCTION: 
    ISecW_StopConnection
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    stop session connection.
ARGUMENTS PASSED:
    ISecW *         pMe[IN]: pointer of ISecW instance.
    WE_INT32        iTargetID[IN]: the identity of the invoker ,input from the interface.
    WE_INT32        iSecurityId[IN]:ID of the security.
    WE_UINT8        ucConnType: TLS or wtls
RETURN VALUE:
    G_SEC_OK : success
    Other : error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecW_StopConnection
(
    ISecW * pISec, 
    WE_INT32 iTargetID,
    WE_INT32 iSecId, 
    WE_UINT8   ucConnType
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT        iFd = -1;; 
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_StopConnection()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    if(-1 == (iFd = ISec_GetSocketFd(pstISec)))
    {
        WE_LOGERROR("ISecW_StopConnection()_____ invalid file description\r\n"); 
        return G_SEC_BAD_HANDLE;                                     
    }  
    iRet = Sec_EncodeStopConn(&pvMsg,
                         iTargetID,iSecId,ucConnType);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pstISec,pvMsg);
    return uiRet;
}
/*==================================================================================================
FUNCTION: 
    ISecW_RemovePeer
CREATE DATE:
    2007-03-21
AUTHOR:
    Wang shuhua
DESCRIPTION:
    remove peer from a session slot.
ARGUMENTS PASSED:
    ISecW *         pMe[IN]: pointer of ISecW instance.
    WE_INT32        iMasterSecretId[IN]:ID of the master secret.
RETURN VALUE:
    G_SEC_OK: success
    Other: error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecW_RemovePeer
(
    ISecW * pISec, 
    WE_INT32 iMasterSecretId
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT        iFd = -1;; 
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_RemovePeer()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    if(-1 == (iFd = ISec_GetSocketFd(pstISec)))
    {
        WE_LOGERROR("ISecW_RemovePeer()_____ invalid file description\r\n"); 
        return G_SEC_BAD_HANDLE;                                     
    }  
    iRet = Sec_EncodeRemovePeer(&pvMsg,iMasterSecretId);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pstISec,pvMsg);
 
    return uiRet;
}
/*==================================================================================================
FUNCTION: 
    ISecW_AttachPeerToSession
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    add one peer to an existing session.
ARGUMENTS PASSED:
    ISecW *       pMe[IN]: pointer of ISecW instance.
    WE_UCHAR *    pucAddress[IN]:Pointer to the address.
    WE_INT32      iAddressLen[IN]:Length of the address.
    WE_UINT16     usPortnum[IN]:Number of the port.
    WE_INT32      iMasterSecretId[IN]:ID of the master secret.
    WE_UINT8      ucConnType : TLS or WTLS
RETURN VALUE:
    G_SEC_OK  : success
    Other: error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecW_AttachPeerToSession
(
    ISecW * pISec, 
    WE_UCHAR * pucAddress, 
    WE_INT32 iAddressLen,
    WE_UINT16 usPortNum, 
    WE_INT32 iMasterSecretId,
    WE_UINT8   ucConnType
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT        iFd = -1;; 
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
    if(-1 == (iFd = ISec_GetSocketFd(pstISec)))
    {
        WE_LOGERROR("ISecW_AttachPeerToSession()_____ invalid file description\r\n"); 
        return G_SEC_BAD_HANDLE;                                     
    }  
    iRet = Sec_EncodeAttachPeerToSession(&pvMsg,
                                         pucAddress,
                                         iAddressLen,
                                         usPortNum,
                                         iMasterSecretId,
                                         ucConnType);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pstISec,pvMsg);
    return uiRet;
}
/*==================================================================================================
FUNCTION: 
    ISecW_SearchPeer
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    search peer in a session slot.
ARGUMENTS PASSED:
    ISecW *       pMe[IN]: pointer of ISecW instance.
    WE_UCHAR *    pucAddress[IN]:Pointer to the address.
    WE_INT32      iAddressLen[IN]:Length of the address.
    WE_UINT16     usPortnum[IN]:Number of the port.
    WE_INT32      iTargetID[IN]:ID of the object.
    WE_UINT8      ucConnType : TLS or WTLS
RETURN VALUE:
    G_SEC_OK :success
    Other : error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecW_SearchPeer
(
    ISecW * pISec, 
    WE_INT32 iTargetID, 
    WE_UCHAR * pucAddress,
    WE_INT32 iAddressLen,
    WE_UINT16 usPortNum,
    WE_UINT8   ucConnType
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT        iFd = -1;; 
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_SearchPeer()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    if(-1 == (iFd = ISec_GetSocketFd(pstISec)))
    {
        WE_LOGERROR("ISecW_SearchPeer()_____ invalid file description\r\n"); 
        return G_SEC_BAD_HANDLE;                                     
    }  
    iRet = Sec_EncodeSearchPeer(&pvMsg,
                                 iTargetID,
                                 pucAddress,
                                 iAddressLen,
                                 usPortNum,
                                 ucConnType);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pstISec,pvMsg);
    return uiRet;
}
/*==================================================================================================
FUNCTION: 
    ISecW_EnableSession
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    enable a session connection for wtls.
ARGUMENTS PASSED:
    ISecW *     pMe[IN]: pointer of ISecW instance.
    WE_INT32    iMasterSecretId[IN]:ID of the master secret.
    WE_UINT8    ucIsActive[IN]:flg of the active.
RETURN VALUE:
    G_SEC_OK : success
    Other : error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecW_EnableSession
(
    ISecW * pISec, 
    WE_INT32 iMasterSecId, 
    WE_UINT8   ucIsActive
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT        iFd = -1;; 
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;    
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_EnableSession()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    if(-1 == (iFd = ISec_GetSocketFd(pstISec)))
    {
        WE_LOGERROR("ISecW_EnableSession()_____ invalid file description\r\n"); 
        return G_SEC_BAD_HANDLE;                                     
    }  
    iRet = Sec_EncodeEnableSession(&pvMsg,
                                   iMasterSecId,
                                   ucIsActive
                                   );
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pstISec,pvMsg);
    return uiRet;

}
/*==================================================================================================
FUNCTION: 
    ISecW_DisableSession
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    disable one session for wtls.
ARGUMENTS PASSED:
    ISecW *     pMe[IN]: pointer of ISecW instance.
    WE_INT32    iMasterSecretId[IN]:ID of the master secret.
RETURN VALUE:
    G_SEC_OK : success
    Other : error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecW_DisableSession
(
    ISecW * pISec, 
    WE_INT32 iMasterSecId
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT        iFd = -1;; 
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return -1;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_DisableSession()_____ bad isec pointer!\r\n");
        return -1;
    }
    if(-1 == (iFd = ISec_GetSocketFd(pstISec)))
    {
        WE_LOGERROR("ISecW_DisableSession()_____ invalid file description\r\n"); 
        return -1;                                     
    }  
    iRet = Sec_EncodeDisableSession(&pvMsg,
                                   iMasterSecId
                                   );
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pstISec,pvMsg);
    return uiRet;

}
/*==================================================================================================
FUNCTION: 
    ISecW_SessionGet
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    fetch information of one session based on master secret id.
ARGUMENTS PASSED:
    ISecW *     pMe[IN]: pointer of ISecW instance.
    WE_INT32    iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_INT32    iMasterSecretId[IN]:ID of the master secret.
RETURN VALUE:
    G_SEC_OK : success
    Other : error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_UINT32 ISecW_SessionGet
(
    ISecW * pISec, 
    WE_INT32 iTargetID,
    WE_INT32 iMasterSecId
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT        iFd = -1;; 
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_SessionGet()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    if(-1 == (iFd = ISec_GetSocketFd(pstISec)))
    {
        WE_LOGERROR("ISecW_SessionGet()_____ invalid file description\r\n"); 
        return G_SEC_BAD_HANDLE;                                     
    }  
    iRet = Sec_EncodeSessionGet(&pvMsg,
                                iTargetID,
                                iMasterSecId
                                 );
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pstISec,pvMsg);
    return uiRet;

}
/*==================================================================================================
FUNCTION: 
    ISecW_SessionRenew
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    update one session information.
ARGUMENTS PASSED:
    ISecW *        pMe[IN]: pointer of ISecW instance.
    WE_INT32       iMasterSecretId[IN]:ID of the master secret.
    WE_UINT8       ucSessionOptions[IN]:The option of the session.
    WE_UCHAR *     pucSessionId[IN]:Pointer to the session ID.
    WE_UINT8       ucSessionIdLen[IN]:Length of the session id.
    WE_UINT8       aucCipherSuite[IN]:Array for cipher suite.
    WE_UINT8       ucCompressionAlg[IN]:The value of the compression Algorithm.
    WE_UCHAR *     pucPrivateKeyId[IN]:ID of the private key.
    WE_UINT32      uiCreationTime[IN]:the time of creation.
RETURN VALUE:
    G_SEC_OK : success
    Other : error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecW_SessionRenew
(
    ISecW * pISec, 
    WE_INT32 iMasterSecID,
    WE_UINT8 ucSessionOpts,
    WE_UCHAR *pucSessionId,
    WE_UINT8 ucSessionIdLen,
    WE_UINT8 *aucCipherSuite,
    WE_UINT8 ucCompressionAlg,
    WE_UCHAR *pucPrivateKeyId,
    WE_UINT32 uiCreationTime
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT        iFd = -1;; 
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_SessionRenew()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    if(-1 == (iFd = ISec_GetSocketFd(pstISec)))
    {
        WE_LOGERROR("ISecW_SessionRenew()_____ invalid file description\r\n"); 
        return G_SEC_BAD_HANDLE;                                     
    }  
    iRet = Sec_EncodeSessionRenew(&pvMsg,
                                   iMasterSecID,
                                   ucSessionOpts,
                                   pucSessionId,
                                   ucSessionIdLen,
                                   aucCipherSuite,
                                   ucCompressionAlg,
                                   pucPrivateKeyId,
                                   uiCreationTime
                                  );
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pstISec,pvMsg);
    return uiRet;

}
/*==================================================================================================
FUNCTION: 
    ISecW_GetPrfResult
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Get the Pseudo Random Function.If the master secret id is 
    to be used, "pucSecret" is NULL, and "iMasterSecretId" 
    denotes which master secret is to use. Or,"pucSecret" must be provided.
ARGUMENTS PASSED:
    ISecW *     pMe[IN]: pointer of ISecW instance.
    WE_INT32    iTargetID[IN]: the identity of the invoker ,input from the interface.
    WE_UINT8    ucAlg[IN]:The value of the Algorithm.
    WE_INT32    iMasterSecretId[IN]:ID of master secret.
    WE_INT32    iSecretLen[IN]:Length of the secret.
    WE_INT32    iSeedLen[IN]:Length of the seed.
    WE_INT32    iOutputLen[IN]:Length of the output.
    WE_UCHAR *  pucSecret[IN]:Pointer to the secret.
    WE_CHAR  *  pcLabel[IN]:Pointer to lable.
    WE_UCHAR *  pucSeed[IN]:Pointer to seed.
    WE_UINT8    ucConnType[IN]: TLS or WTLS
RETURN VALUE:
    G_SEC_OK : success
    Other : error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecW_GetPrfResult
(
    ISecW *pISec,
    WE_INT32 iTargetID, 
    WE_UINT8 ucAlg,
    WE_INT32 iMasterSecretId, 
    WE_UCHAR * pucSecret, 
    WE_INT32 iSecretLen, 
    WE_CHAR * pcLabel, 
    WE_UCHAR * pucSeed,
    WE_INT32 iSeedLen,
    WE_INT32 iOutputLen,
    WE_UINT8 ucConnType
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT        iFd = -1;; 
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0; 
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_GetPrfResult()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    if(-1 == (iFd = ISec_GetSocketFd(pstISec)))
    {
        WE_LOGERROR("ISecW_GetPrfResult()_____ invalid file description\r\n"); 
        return G_SEC_BAD_HANDLE;                                     
    }  
    iRet = Sec_EncodeGetPrfResult(&pvMsg,
                                   iTargetID, 
                                   ucAlg,
                                   iMasterSecretId, 
                                   pucSecret, 
                                   iSecretLen, 
                                   pcLabel, 
                                   pucSeed,
                                   iSeedLen,
                                   iOutputLen,
                                   ucConnType
                                 );
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pstISec,pvMsg);
    return uiRet;

}
/*==================================================================================================
FUNCTION: 
    ISecW_VerifySvrCertChain
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    the interface will provide the function to verify the certificate chain from the gateway.
ARGUMENTS PASSED:
    ISecW *      pMe[IN]: pointer of ISecW instance.
    WE_INT32    iTargetID[IN]: the identity of the invoker ,input from the interface.
    WE_INT32    iBufLen[IN]:Length of the buffer.
    WE_INT32    iAddrLen[IN]:Length of the address.
    WE_UCHAR *  pucBuf[IN]:Pointer to the buffer.
    WE_UCHAR *  pucAddr[IN]:Pointer to the address.
    WE_UINT8    ucConnType[IN]:
RETURN VALUE:
    G_SEC_OK : success
    Other : error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecW_VerifySvrCertChain
(
    ISecW *pISec,
    WE_INT32 iTargetID, 
    WE_VOID *pvBuf,
    WE_INT32 iBufLen,
    WE_UINT8 ucConnType
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT        iFd = -1;; 
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_VerifySvrCertChain()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    if(-1 == (iFd = ISec_GetSocketFd(pstISec)))
    {
        WE_LOGERROR("ISecW_VerifySvrCertChain()_____ invalid file description\r\n"); 
        return G_SEC_BAD_HANDLE;                                     
    }  
    iRet = Sec_EncodeVerifySvrCertChain(&pvMsg,
                                         iTargetID,
                                         pvBuf,
                                         iBufLen,
                                         ucConnType);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pstISec,pvMsg);
    return uiRet;

}
/*==================================================================================================
FUNCTION: 
    ISecW_GetCipherSuite
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    The function provides the information for wtls about the bulk cipher algorithm
    key exchange method and the trusted ca information.
ARGUMENTS PASSED:
    ISecW *      pMe[IN]: pointer of ISecW instance
    WE_INT32    iTargetID[IN]: the identity of the invoker ,input from the interface.
    WE_UINT8   ucConnType[IN]:
RETURN VALUE:
    G_SEC_OK : success
    Other : error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecW_GetCipherSuite
(
    ISecW *pISec,
    WE_INT32 iTargetID, 
    WE_UINT8 ucConnType
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT        iFd = -1;; 
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_GetCipherSuite()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    if(-1 == (iFd = ISec_GetSocketFd(pstISec)))
    {
        WE_LOGERROR("ISecW_GetCipherSuite()_____ invalid file description\r\n"); 
        return G_SEC_BAD_HANDLE;                                     
    }  
    iRet = Sec_EncodeGetCipherSuite(&pvMsg,
                                     iTargetID,
                                     ucConnType);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pstISec,pvMsg);
    return uiRet;

}
/*==================================================================================================
FUNCTION: 
    ISecW_KeyExchange
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    the interface will provide the function about the master secret key.
ARGUMENTS PASSED:
    ISecW *        pMe[IN]: pointer of ISecW instance.
    WE_INT32       iObjectId[IN]: the identity of the invoker ,input from the interface..
    WE_UINT8       ucAlg[IN]:Wtls:The value of the hash Algorithm.
                             Tls:The Algorithm of key Exchange.
    WE_VOID *      pvParam[IN]:if the type of connection is wtls,it is point to the structure St_SecWtlsKeyExchParams,which includs the public key and key exchange method.
                   if the type of connection is tls,which point to the structure of St_SecTlsKeyExchangeParams
    WE_UCHAR *     pucRandval[IN]:random data to create the master secret,if the type of connetion is wtls, 
                   it's length is 32;else if tls ,it is 64(rfc2246).
    WE_UINT8       ucConnType[IN]:the type of connectin:wtls,ssl or tls   
RETURN VALUE:
    G_SEC_OK : success
    Other : error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecW_KeyExchange
(
    ISecW *pISec,
    WE_INT32 iTargetID, 
    WE_VOID *pvParam,
    WE_UINT8 ucAlg,
    WE_UCHAR *pucRandval,
    WE_UINT8 ucConnType
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT        iFd = -1;; 
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_KeyExchange()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    if(-1 == (iFd = ISec_GetSocketFd(pstISec)))
    {
        WE_LOGERROR("ISecW_KeyExchange()_____ invalid file description\r\n"); 
        return G_SEC_BAD_HANDLE;                                     
    }  
    iRet = Sec_EncodeKeyExchange(&pvMsg,
                                 iTargetID,
                                 pvParam,
                                 ucAlg,
                                 pucRandval,
                                 ucConnType);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pstISec,pvMsg);
    return uiRet;

}
/*==================================================================================================
FUNCTION: 
    ISecW_GetUserCert
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    the interface will provide the function to return the client certificate to the wtls.
ARGUMENTS PASSED:
    ISecW *     pMe[IN]: pointer of ISecW instance.
    WE_INT32    iTargetID[IN]: the identity of the invoker ,input from the interface.
    WE_INT32    iBufLen[IN]:Length of the buffer.
    WE_UCHAR *  pucBuf[IN]:Pointer to the buffer.
    WE_UINT8   ucConnType:The type of connection:wtls,tls or ssl.
RETURN VALUE:
    G_SEC_OK : success
    Other : error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecW_GetUserCert
(
    ISecW *pISec,
    WE_INT32 iTargetID, 
    WE_VOID *pvBuf,
    WE_INT32 iBufLen,
    WE_UINT8 ucConnType
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT        iFd = -1;; 
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;    
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_GetUserCert()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    if(-1 == (iFd = ISec_GetSocketFd(pstISec)))
    {
        WE_LOGERROR("ISecW_GetUserCert()_____ invalid file description\r\n"); 
        return G_SEC_BAD_HANDLE;                                     
    }  
    iRet = Sec_EncodeGetUserCert(&pvMsg,
                                 iTargetID,
                                 pvBuf,
                                 iBufLen,
                                 ucConnType);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pstISec,pvMsg);
    return uiRet;

}
/*==================================================================================================
FUNCTION: 
    ISecW_CompSign
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    compute the signature for wtls.
ARGUMENTS PASSED:
    ISecW *       pMe[IN]: pointer of ISecW instance.
    WE_INT32      iTargetID[IN]: the identity of the invoker ,input from the interface.
    WE_UCHAR *    pucKeyId[IN]:Pointer to the key id .
    WE_INT32      iKeyIdLen[IN]:Length of the key id.
    WE_UCHAR *    pucBuf[IN]:Pointer to the buffer.
    WE_INT32      iBufLen[IN]:Length of the buffer.
    WE_UINT8      ucConnType[IN]:
RETURN VALUE:
    G_SEC_OK : success
    Other : error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecW_CompSign
(
    ISecW *pISec,
    WE_INT32 iTargetID, 
    WE_UCHAR *pucKeyId,
    WE_INT32 iKeyIdLen,
    WE_UCHAR *pucBuf,
    WE_INT32 iBufLen,
    WE_UINT8 ucConnType
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT        iFd = -1;; 
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_CompSign()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    if(-1 == (iFd = ISec_GetSocketFd(pstISec)))
    {
        WE_LOGERROR("ISecW_CompSign()_____ invalid file description\r\n"); 
        return G_SEC_BAD_HANDLE;                                     
    }  
    iRet = Sec_EncodeCompSign(&pvMsg,
                              iTargetID,
                              pucKeyId,
                              iKeyIdLen,
                              pucBuf,
                              iBufLen,
                              ucConnType);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pstISec,pvMsg);
    return uiRet;

}
/*==================================================================================================
FUNCTION: 
    ISecW_SslHashUpdateWMasterSec
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    ssl hash compute
ARGUMENTS PASSED:
    ISecW *       pISec[IN]: pointer of ISecW instance.
    WE_INT32      iMasterSecId[IN]:master secret id.
RETURN VALUE:
    G_SEC_OK : success
    Other : error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecW_SslHashUpdateWMasterSec
(
    ISecW *pISec,
    WE_INT32 iMasterSecId
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT        iFd = -1;; 
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;    
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_SslHashUpdateWMasterSec()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    if(-1 == (iFd = ISec_GetSocketFd(pstISec)))
    {
        WE_LOGERROR("ISecW_SslHashUpdateWMasterSec()_____ invalid file description\r\n"); 
        return G_SEC_BAD_HANDLE;                                     
    }  
    iRet = Sec_EncodeSSLHashUpdateWMasterSec(&pvMsg,
                                              iMasterSecId);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pstISec,pvMsg);
    return uiRet;

}
/*==================================================================================================
FUNCTION: 
    ISecW_EvtShowDlgAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    ssl hash compute
ARGUMENTS PASSED:
    ISecW *       pISec[IN]: pointer of ISecW instance.
    WE_INT32    iTargetID[IN]:the identity of the invoker ,input from the interface.
    WE_VOID*    pvData [IN] : data 
    WE_UINT32   uiLength[IN]: data length

RETURN VALUE:
    G_SEC_OK : success
    Other : error code
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_UINT32 ISecW_EvtShowDlgAction
(
    ISecW *pISec,
    WE_INT32 iTargetID, 
    WE_VOID *pvData,
    WE_UINT32 uiLength
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT        iFd = -1;; 
    WE_INT32 iRet = -1;
    WE_UINT32  uiRet = 0;    
    WE_VOID      *pvMsg = NULL;

    if (NULL == pstISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_EvtShowDlgAction()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    if(-1 == (iFd = ISec_GetSocketFd(pstISec)))
    {
        WE_LOGERROR("ISecW_EvtShowDlgAction()_____ invalid file description\r\n"); 
        return G_SEC_BAD_HANDLE;                                     
    }  
    iRet = Sec_EncodeEvtShowDlgAction(&pvMsg,
                                      iTargetID,
                                      pvData,
                                      uiLength);
    if ( -1 == iRet)
    {
        return G_SEC_ERROR_OTHER;
    }
    uiRet = ISec_SendToSecD(pstISec,pvMsg);
    return uiRet;

}




