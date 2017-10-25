/*==================================================================================================
    HEADER NAME : sec_edc.c
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
        
    GENERAL DESCRIPTION
        In this file,encode data will send to socket, decode data received from socket
    
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
#include "we_mem.h"
#include "sec_ecdr.h"
#include "sec_icmsg.h"

#include "sec.h"
#include "isecb.h"
#include "isecw.h"
#include "isigntext.h"

#include "oem_seclog.h"
#include "sec_resp.h"
#include "sec_uecst.h"

#include "sec_edc.h"

/* #define G_SEC_CFG_SHOW_PIN */

typedef struct tagSt_SecModifyPinResp
{
   WE_INT32  iTargetID;
   WE_UINT16 usResult;
}St_SecModifyPinResp;

#define WE_LOGERROR  printf


/***************************************************************************************************
*   Type Define Section
***************************************************************************************************/

/**************************************************************************************************
* Prototype Declare Section
***************************************************************************************************/
static St_SecEcdrEncoder *Sec_SetMsgDataEncode(WE_UINT8 ucMsgId, WE_INT32 iDataLength );
static St_SecEcdrDecoder* Sec_GetMsgDecoder(WE_VOID* pvBuf);
static WE_INT32 Sec_GetPrfResp(WE_VOID *pvMsg,WE_VOID **ppvResp);
static WE_INT32 Sec_GetSignTextResp(WE_VOID *pvMsg,WE_VOID** ppvResp);
static WE_UINT8 Sec_GetMsgType(WE_VOID *pvBuf);
static WE_INT32 Sec_GetVerifySvrCertResp(WE_VOID *pvMsg,WE_VOID **ppvResp);
static WE_INT32 Sec_GetGetUsrCertResp(WE_VOID *pvMsg,WE_VOID **ppvResp);
static WE_INT32 Sec_GetGetCipherSuiteResp(WE_VOID *pvMsg,WE_VOID **ppvResp);
static WE_INT32 Sec_GetKeyExchangeResp(WE_VOID *pvMsg,WE_VOID **ppvResp);
static WE_INT32 Sec_GetCompSignResp(WE_VOID *pvMsg,WE_VOID **ppvResp);
static WE_INT32 Sec_GetSearchPeerResp(WE_VOID *pvMsg,WE_VOID **ppvResp);
static WE_INT32 Sec_GetSessionGetResp(WE_VOID *pvMsg,WE_VOID **ppvResp);
static WE_INT32 Sec_GetGetCertNameListResp(WE_VOID *pvMsg,WE_VOID **ppvResp);
static WE_INT32 Sec_GetDelCertResp(WE_VOID *pvMsg,WE_VOID **ppvResp);
static WE_INT32 Sec_GetKeyPairGenResp(WE_VOID *pvMsg,WE_VOID **ppvResp);
static WE_INT32 Sec_GetGetPubKeyResp(WE_VOID *pvMsg,WE_VOID **ppvResp);
#ifdef G_SEC_CFG_SHOW_PIN
static WE_INT32 Sec_GetModifyPinResp(WE_VOID *pvMsg,WE_VOID **ppvResp);
#endif
static WE_INT32 Sec_GetViewAndCertResp(WE_VOID *pvMsg,WE_VOID **ppvResp);
static WE_INT32 Sec_GetGetCurSvrCertResp(WE_VOID *pvMsg,WE_VOID **ppvResp);
static WE_INT32 Sec_GetGetSessionInfoResp(WE_VOID *pvMsg,WE_VOID **ppvResp);
static WE_INT32 Sec_GetWtlsCurClassResp(WE_VOID *pvMsg,WE_VOID **ppvResp);
static WE_INT32 Sec_GetClrSessionResp(WE_VOID *pvMsg, WE_VOID** ppvData);
static WE_INT32 Sec_GetGetContractsListResp(WE_VOID *pvMsg, WE_VOID** ppvData);
static WE_INT32 Sec_GetRemoveContractResp(WE_VOID *pvMsg, WE_VOID** ppvData);
static WE_INT32 Sec_GetUsrCertReqResp (WE_VOID *pvMsg, WE_VOID** ppvData);
static WE_INT32 Sec_GetChangeCertStateResp (WE_VOID *pvMsg, WE_VOID** ppvData);
static WE_INT32 Sec_GetTlsUsrCertResp (WE_VOID *pvMsg, WE_VOID** ppvData);
static WE_INT32 Sec_GetTlsCipherSuiteResp (WE_VOID *pvMsg, WE_VOID** ppvData);
static WE_INT32 Sec_GetSSLMasterSecretResp(WE_VOID *pvMsg,WE_VOID **ppvResp);

/*==================================================================================================
FUNCTION: 
    Sec_EncodeConnType
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode interface type 
ARGUMENTS PASSED:
    ppvMsg: address of data to send
    iType: interface type
RETURN VALUE:
    success: 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeConnType
(
    WE_VOID **ppvMsg,
    WE_INT32 iType
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;
    
    if (NULL == ppvMsg) 
    {
        return -1;
    }
    iLen = sizeof(WE_INT32);
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_CONN_TYPE, iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iType);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }

    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;
    
}

#ifndef WAP
/*==================================================================================================
FUNCTION: 
    Sec_EncodeGetCipherSuite
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters to get cipher suite 
ARGUMENTS PASSED:
    ppvMsg: address of data to send
    iTargetID: target id
    ucConnType: connection type
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeGetCipherSuite
(
    WE_VOID **ppvMsg,
    WE_INT32 iTargetID,
    WE_UINT8 ucConnType
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;

    iLen = sizeof(WE_INT32) + sizeof(WE_UINT8);
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_W_GET_CIPHER_SUITE, iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    iRet = SecEcdr_EncodeUint8(pstEncoder, &ucConnType);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    
    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeSetUpConn
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters to set up connection
ARGUMENTS PASSED:
    ppvMsg: address of data to send
    iTargetID: target id
    iMasterSecretId: master secret id
    iSecId : security id
    iFullHandshake: whether is full handshake
    stSessionInfo: session information
    ucConnType: connection type
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeSetUpConn
(
    WE_VOID **ppvMsg,
    WE_INT32 iTargetID,
    WE_INT32 iMasterSecretId, 
    WE_INT32 iSecId, 
    WE_INT32 iFullHandshake,
    St_SecSessionInformation stSessionInfo,
    WE_UINT8   ucConnType
)
{  
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;

    iLen = 4 * sizeof(WE_INT32) + sizeof(St_SecSessionInformation) + stSessionInfo.uiCertLen 
                                 + sizeof(WE_UINT8);
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_W_SETUP_CONN, iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }

    iRet = SecEcdr_EncodeInt32(pstEncoder, &iTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iMasterSecretId);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iSecId);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iFullHandshake);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint32(pstEncoder, &(stSessionInfo.uiCertLen));
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    if (stSessionInfo.uiCertLen > 0)
    {
        iRet = SecEcdr_EncodeOctets(pstEncoder, stSessionInfo.pucCert,stSessionInfo.uiCertLen);
        if (iRet < 0)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }        
    }    
    iRet = SecEcdr_EncodeUint16(pstEncoder, &(stSessionInfo.usKeyExchangeKeyLen));
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    iRet = SecEcdr_EncodeUint16(pstEncoder, &(stSessionInfo.usHmacLen));
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint16(pstEncoder, &(stSessionInfo.usEncryptionKeyLen));
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint8(pstEncoder, &(stSessionInfo.ucSecurityClass));
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint8(pstEncoder, &(stSessionInfo.ucKeyExchangeAlg));
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint8(pstEncoder, &(stSessionInfo.ucHmacAlg));
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint8(pstEncoder, &(stSessionInfo.ucEncryptionAlg));
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint8(pstEncoder, &(stSessionInfo.ucNumCerts));
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint8(pstEncoder, &(stSessionInfo.ucCertFormat));
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint8(pstEncoder, &ucConnType);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    
    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeStopConn
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters to stop connction
ARGUMENTS PASSED:
    ppvMsg: address of data to send
    iTargetID: target id
    iSecId : security id
    ucConnType: connection type
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeStopConn
(
    WE_VOID **ppvMsg,
    WE_INT32 iTargetID,
    WE_INT32 iSecId, 
    WE_UINT8   ucConnType
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;

    iLen = 2 *sizeof(WE_INT32) + sizeof(WE_UINT8);
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_W_STOP_CONN, iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iSecId);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    iRet = SecEcdr_EncodeUint8(pstEncoder, &ucConnType);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    
    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeRemovePeer
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters to remove peer
ARGUMENTS PASSED:
    ppvMsg: address of data to send
    iMasterSecId: master secret id
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeRemovePeer
(
    WE_VOID** ppvMsg,
    WE_INT32 iMasterSecId
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;

    iLen = sizeof(WE_INT32);
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_W_REMOVE_PEER, iLen);    
    if (NULL == pstEncoder)
    {
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iMasterSecId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg,pstEncoder->pucData,pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;
    
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeAttachPeerToSession
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters for attach peer to session
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeAttachPeerToSession
(
    WE_VOID **ppvMsg,
    WE_UCHAR *pucAddress, 
    WE_INT32 iAddressLen,
    WE_UINT16 usPortnum, 
    WE_INT32 iMasterSecretId,
    WE_UINT8   ucConnType
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;

    iLen = iAddressLen + 2* sizeof(WE_INT32) + sizeof(WE_UINT16) + sizeof(WE_UINT8);
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_W_ATTACH_PEER_TO_SESSION, iLen);    
    if (NULL == pstEncoder)
    {
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iAddressLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    iRet = SecEcdr_EncodeOctets(pstEncoder,pucAddress,iAddressLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    iRet = SecEcdr_EncodeUint16(pstEncoder,&usPortnum);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeInt32(pstEncoder,&iMasterSecretId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint8(pstEncoder, &ucConnType);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg,pstEncoder->pucData,pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;
    
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeSearchPeer
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters for search peer
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeSearchPeer
(
    WE_VOID **ppvMsg,
    WE_INT32 iTargetID,
    WE_UCHAR *pucAddress, 
    WE_INT32 iAddressLen,
    WE_UINT16 usPortNum, 
    WE_UINT8   ucConnType
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;

    iLen = iAddressLen + 2* sizeof(WE_INT32) + sizeof(WE_UINT16) + sizeof(WE_UINT8);
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_W_SEARCH_PEER, iLen);    
    if (NULL == pstEncoder)
    {
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iAddressLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeOctets(pstEncoder,pucAddress,iAddressLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    iRet = SecEcdr_EncodeUint16(pstEncoder,&usPortNum);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint8(pstEncoder, &ucConnType);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg,pstEncoder->pucData,pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;
    
}  
/*==================================================================================================
FUNCTION: 
    Sec_EncodeEnableSession
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters for enable session
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeEnableSession
(
    WE_VOID **ppvMsg,
    WE_INT32 iMasterSecId,
    WE_UINT8 ucIsActive
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;

    iLen = sizeof(WE_INT32) + sizeof(WE_UINT8);
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_W_ENABLE_SESSION, iLen); 
    if (NULL == pstEncoder)
    {
        return -1;
    }

    iRet = SecEcdr_EncodeInt32(pstEncoder, &iMasterSecId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    iRet = SecEcdr_EncodeUint8(pstEncoder,&ucIsActive);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeDisableSession
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters for disable session
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeDisableSession
(
    WE_VOID **ppvMsg,
    WE_INT32 iMasterSecId
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;

    iLen = sizeof(WE_INT32);
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_W_DISABLE_SESSION, iLen); 
    if (NULL == pstEncoder)
    {
        return -1;
    }

    iRet = SecEcdr_EncodeInt32(pstEncoder, &iMasterSecId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }

    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeSessionGet
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters for get session
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeSessionGet
(
    WE_VOID **ppvMsg,
    WE_INT32 iTargetID,
    WE_INT32 iMasterSecID
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;

    iLen = 2 * sizeof(WE_INT32);
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_W_SESSION_GET, iLen); 
    if (NULL == pstEncoder)
    {
        return -1;
    }

    iRet = SecEcdr_EncodeInt32(pstEncoder, &iTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iMasterSecID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeSessionRenew
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters for renew session
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeSessionRenew
(
    WE_VOID **ppvMsg,
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
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;
    WE_INT32 iPrivateKeyIdLen = 0;
    WE_INT32 iDataLenTmp = 0;

    if (pucPrivateKeyId != NULL)
    {
        iPrivateKeyIdLen = WE_STRLEN(pucPrivateKeyId) + 1;
    }
    if (aucCipherSuite != NULL)
    {
        iDataLenTmp = sizeof(aucCipherSuite);
    }

    iLen = 4* sizeof(WE_INT32) + 3* sizeof(WE_UINT8) + ucSessionIdLen + iDataLenTmp + iPrivateKeyIdLen ;
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_W_SESSION_RENEW,iLen);
    
    if (NULL == pstEncoder)
    {
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iMasterSecID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint8(pstEncoder, &ucSessionOpts);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint8(pstEncoder, &ucSessionIdLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeOctets(pstEncoder,pucSessionId, ucSessionIdLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeInt32( pstEncoder,&iDataLenTmp);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeOctets(pstEncoder,aucCipherSuite, iDataLenTmp);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint8( pstEncoder, &ucCompressionAlg);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeInt32(pstEncoder,&iPrivateKeyIdLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeOctets(pstEncoder, pucPrivateKeyId, iPrivateKeyIdLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint32(pstEncoder, &uiCreationTime);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData,pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeGetPrfResult
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters for get prf result
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeGetPrfResult
(
    WE_VOID **ppvMsg,
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
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;
    WE_INT32 iLableLen = 0;

    if (pcLabel != NULL)
    {
        iLableLen = WE_STRLEN(pcLabel) + 1;
    }
    iLen = 7* sizeof(WE_INT32) + sizeof(WE_UINT8) + iSecretLen + iLableLen + iSeedLen;

    pstEncoder = Sec_SetMsgDataEncode( M_SEC_W_GET_PRF_RESULT, iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }
    iRet = SecEcdr_EncodeInt32( pstEncoder,&iTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint8(pstEncoder, &ucAlg);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeInt32(pstEncoder,&iMasterSecretId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iSecretLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeOctets(pstEncoder, pucSecret, iSecretLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iLableLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }   
    if (iLableLen > 1)
    {
        iRet = SecEcdr_EncodeOctets(pstEncoder, pcLabel, iLableLen);
        if ( -1 == iRet)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iSeedLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeOctets(pstEncoder, pucSeed, iSeedLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeInt32( pstEncoder,&iOutputLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeUint8(pstEncoder, &ucConnType); 
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }

    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        if ( -1 == iRet)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeVerifySvrCertChain
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters for verify server certificate chain
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeVerifySvrCertChain
(
    WE_VOID **ppvMsg,
    WE_INT32 iTargetId,
    WE_VOID *pvBuf,
    WE_INT32 iBufLen,
    WE_UINT8 ucConnType
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;    

    iLen = 2 * sizeof(WE_INT32) + iBufLen + sizeof(WE_UINT8);
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_W_VERIFY_SVR_CERT_CHAIN, iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder,&iTargetId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iBufLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }   
    iRet = SecEcdr_EncodeOctets(pstEncoder,pvBuf,iBufLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    } 
    iRet = SecEcdr_EncodeUint8(pstEncoder, &ucConnType);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }  
    
    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;           
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeKeyExchange
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters for key exchange
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeKeyExchange
(
    WE_VOID **ppvMsg,
    WE_INT32 iTargetId,
    WE_VOID *pvParam,
    WE_UINT8 ucAlg,
    WE_UCHAR *pucRandVal,
    WE_UINT8 ucConnType
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iRet = -1;   
    WE_INT32 iRandValLen = 0;
    WE_INT32 iLen = 0;
    WE_INT32 iParamLen = 0;
    St_SecTlsKeyExchangeParams *pstTlsParams = NULL;
    WE_INT32 iRSARsaLen = 0;
    WE_INT32 iRSASigLen = 0;
    WE_INT32 iRSALen = 0;
    WE_INT32 iDHLen = 0;
    WE_INT32 iDHDhLen = 0;
    WE_INT32 iDHSigLen = 0;
    WE_INT32 iCertLen = 0;
    St_SecWtlsKeyExchParams *pstWtlsParams = NULL;

printf("Sec_edc.c:........into Sec_EncodeKeyExchange....\n");
   if (ucConnType == G_SEC_TLS_CONNECTION_TYPE)
   {
       pstTlsParams = (St_SecTlsKeyExchangeParams*)pvParam;
       if (NULL == pstTlsParams)
       {
          return -1;
       }
       /* stRSAParam*/
       iRSARsaLen = pstTlsParams->stRSAParam.stTlsRSAParam.usModLen + pstTlsParams->stRSAParam.stTlsRSAParam.usExpLen
                   + 2 *sizeof(WE_UINT16);

       iRSASigLen = 2*sizeof(WE_UINT16) + pstTlsParams->stRSAParam.stTlsSignature.usSignLen ;
       iRSALen = iRSARsaLen + iRSASigLen;

       /*stDHParam*/
       iDHDhLen = pstTlsParams->stDHParam.stTlsDHParam.usDHGLen
                    + pstTlsParams->stDHParam.stTlsDHParam.usDHPLen
                    + pstTlsParams->stDHParam.stTlsDHParam.usDHYsLen
                    + 4 * sizeof(WE_UINT16);
       iDHSigLen = pstTlsParams->stDHParam.stTlsSignature.usSignLen + 2* sizeof(WE_UINT16);
       iDHLen = iDHDhLen + iDHSigLen;

       /*stSecCert*/
       iCertLen = pstTlsParams->stSecCert.usBufLen + sizeof(WE_UINT16);

       iParamLen = iRSALen + iDHLen + iCertLen + sizeof(WE_INT32);
   }
   else if(ucConnType == G_SEC_WTLS_CONNECTION_TYPE)
   {
       pstWtlsParams = (St_SecWtlsKeyExchParams*)pvParam;
       if (NULL == pstWtlsParams)
       {
          return -1;
       }   
       iParamLen = pstWtlsParams->stParams.stCertificates.usBufLen + sizeof(WE_UINT16)
                 + pstWtlsParams->stParams.stKeyParam.stParamSpecifier.usParamLen + sizeof(WE_UINT16) + sizeof(WE_UINT8)
                 + pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usExpLen + 
                           pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usModLen
                                + 2*sizeof(WE_UINT16)
                 + pstWtlsParams->stParams.stSecretKey.usIdLen + sizeof(WE_UINT16)
                 + sizeof(WE_UINT8);
                 
   }
   else
   {
         return -1;
   }
   if (pucRandVal != NULL)
   {
       iRandValLen = WE_STRLEN(pucRandVal) + 1;
   }   
   iLen =  2*sizeof(iTargetId) + 2 *sizeof(WE_UINT8)+ iRandValLen + iParamLen;
   
   pstEncoder = Sec_SetMsgDataEncode(M_SEC_W_KEY_EXCHANGE, iLen);
   if (NULL == pstEncoder)
   {
       return -1;
   }
   iRet = SecEcdr_EncodeUint8(pstEncoder, &ucConnType);
   if ( -1 == iRet)
   {
       SecEcdr_DestroyEncoder(pstEncoder);
       return -1;
   }    
   iRet = SecEcdr_EncodeInt32(pstEncoder,&iTargetId);
   if ( -1 == iRet)
   {
       SecEcdr_DestroyEncoder(pstEncoder);
       return -1;
   }   
   iRet = SecEcdr_EncodeUint8(pstEncoder, &ucAlg);
   if ( -1 == iRet)
   {
       SecEcdr_DestroyEncoder(pstEncoder);
       return -1;
   }  
   iRet = SecEcdr_EncodeInt32(pstEncoder,&iRandValLen);    
   if ( -1 == iRet)
   {
       SecEcdr_DestroyEncoder(pstEncoder);
       return -1;
   }   

   if (iRandValLen > 0)
   {
   iRet = SecEcdr_EncodeOctets(pstEncoder,pucRandVal,iRandValLen);
   if ( -1 == iRet)
   {
       SecEcdr_DestroyEncoder(pstEncoder);
       return -1;
        }
   }
   if (ucConnType == G_SEC_TLS_CONNECTION_TYPE)     
   {
        /*stRSAParam*/
        iRet = SecEcdr_EncodeUint16(pstEncoder,&pstTlsParams->stRSAParam.stTlsRSAParam.usModLen);
        if ( -1 == iRet)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }            
        if (pstTlsParams->stRSAParam.stTlsRSAParam.usModLen > 0)
        {
            iRet = SecEcdr_EncodeOctets(pstEncoder,
                                        pstTlsParams->stRSAParam.stTlsRSAParam.pucModulus,
                                        pstTlsParams->stRSAParam.stTlsRSAParam.usModLen);  
           if ( -1 == iRet)
           {
               SecEcdr_DestroyEncoder(pstEncoder);
               return -1;
           }                
        }

        iRet = SecEcdr_EncodeUint16(pstEncoder,&pstTlsParams->stRSAParam.stTlsRSAParam.usExpLen);
        if ( -1 == iRet)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }         
        if (pstTlsParams->stRSAParam.stTlsRSAParam.usExpLen > 0)
        {
            iRet = SecEcdr_EncodeOctets(pstEncoder,
                                        pstTlsParams->stRSAParam.stTlsRSAParam.pucExponent,
                                        pstTlsParams->stRSAParam.stTlsRSAParam.usExpLen); 
            if ( -1 == iRet)
            {
                SecEcdr_DestroyEncoder(pstEncoder);
                return -1;
            }             
        }    

        iRet = SecEcdr_EncodeUint16(pstEncoder,&pstTlsParams->stRSAParam.stTlsSignature.usSignLen);
        if ( -1 == iRet)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }         
        if (pstTlsParams->stRSAParam.stTlsSignature.usSignLen > 0)
        {
            iRet = SecEcdr_EncodeOctets(pstEncoder,
                                        pstTlsParams->stRSAParam.stTlsSignature.pucSignature,
                                        pstTlsParams->stRSAParam.stTlsSignature.usSignLen);  
            if ( -1 == iRet)
            {
                SecEcdr_DestroyEncoder(pstEncoder);
                return -1;
            }             
        }
        iRet = SecEcdr_EncodeUint16(pstEncoder,&pstTlsParams->stRSAParam.stTlsSignature.usReserved);
        if ( -1 == iRet)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }         
        /*stDHParam*/
        iRet = SecEcdr_EncodeUint16(pstEncoder,&pstTlsParams->stDHParam.stTlsDHParam.usDHGLen);
        if ( -1 == iRet)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }         
        if (pstTlsParams->stDHParam.stTlsDHParam.usDHGLen > 0)
        {
            iRet = SecEcdr_EncodeOctets(pstEncoder,
                                        pstTlsParams->stDHParam.stTlsDHParam.pucDHG,
                                        pstTlsParams->stDHParam.stTlsDHParam.usDHGLen);  
            if ( -1 == iRet)
            {
                SecEcdr_DestroyEncoder(pstEncoder);
                return -1;
            }             
        }
        iRet = SecEcdr_EncodeUint16(pstEncoder,&pstTlsParams->stDHParam.stTlsDHParam.usDHPLen);
        if ( -1 == iRet)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }         
        if (pstTlsParams->stDHParam.stTlsDHParam.usDHPLen> 0)
        {
            iRet = SecEcdr_EncodeOctets(pstEncoder,
                                        pstTlsParams->stDHParam.stTlsDHParam.pucDHP,
                                        pstTlsParams->stDHParam.stTlsDHParam.usDHPLen); 
            if ( -1 == iRet)
            {
                SecEcdr_DestroyEncoder(pstEncoder);
                return -1;
            }             
        }
        iRet = SecEcdr_EncodeUint16(pstEncoder,&pstTlsParams->stDHParam.stTlsDHParam.usDHYsLen);
        if ( -1 == iRet)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }         
        if (pstTlsParams->stDHParam.stTlsDHParam.usDHYsLen> 0)
        {
            iRet = SecEcdr_EncodeOctets(pstEncoder,
                                        pstTlsParams->stDHParam.stTlsDHParam.pucDHYs,
                                        pstTlsParams->stDHParam.stTlsDHParam.usDHYsLen);  
            if ( -1 == iRet)
            {
                SecEcdr_DestroyEncoder(pstEncoder);
                return -1;
            }             
        }
        iRet = SecEcdr_EncodeUint16(pstEncoder,&pstTlsParams->stDHParam.stTlsDHParam.usReserved1);
        if ( -1 == iRet)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }         
        /*stSecCert*/
        iRet = SecEcdr_EncodeUint16(pstEncoder,&pstTlsParams->stSecCert.usBufLen);
        if ( -1 == iRet)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }         
        if (pstTlsParams->stSecCert.usBufLen > 0)
        {
            
            iRet = SecEcdr_EncodeOctets(pstEncoder,
                                        pstTlsParams->stSecCert.pucBuf,
                                        pstTlsParams->stSecCert.usBufLen);  
            if ( -1 == iRet)
            {
                SecEcdr_DestroyEncoder(pstEncoder);
                return -1;
            }             
        }
        iRet = SecEcdr_EncodeInt32(pstEncoder,&pstTlsParams->iOptions);
        if ( -1 == iRet)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }         
   }
   else if (ucConnType == G_SEC_WTLS_CONNECTION_TYPE)
   {    
        /*stParams*/
        iRet = SecEcdr_EncodeUint16(pstEncoder,&pstWtlsParams->stParams.stCertificates.usBufLen);
        if ( -1 == iRet)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }         
        if (pstWtlsParams->stParams.stCertificates.usBufLen > 0)
        {
            iRet = SecEcdr_EncodeOctets(pstEncoder,
                                        pstWtlsParams->stParams.stCertificates.pucBuf,
                                        pstWtlsParams->stParams.stCertificates.usBufLen);    
            if ( -1 == iRet)
            {
                SecEcdr_DestroyEncoder(pstEncoder);
                return -1;
            }             
        }    
        iRet = SecEcdr_EncodeUint16(pstEncoder,&pstWtlsParams->stParams.stKeyParam.stParamSpecifier.usParamLen);
        if ( -1 == iRet)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        } 

        if (pstWtlsParams->stParams.stKeyParam.stParamSpecifier.usParamLen > 0)
        {
            iRet = SecEcdr_EncodeOctets(pstEncoder,
                                        pstWtlsParams->stParams.stKeyParam.stParamSpecifier.pucParams,
                                        pstWtlsParams->stParams.stKeyParam.stParamSpecifier.usParamLen);            
            if ( -1 == iRet)
            {
                SecEcdr_DestroyEncoder(pstEncoder);
                return -1;
            } 

        }           
        iRet = SecEcdr_EncodeUint8(pstEncoder,&pstWtlsParams->stParams.stKeyParam.stParamSpecifier.ucParameterIndex);
        if ( -1 == iRet)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        } 

        iRet = SecEcdr_EncodeUint16(pstEncoder,&pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usExpLen);
        if ( -1 == iRet)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        } 

        if (pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usExpLen > 0)
        {
            iRet = SecEcdr_EncodeOctets(pstEncoder,
                                        pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucExponent,
                                        pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usExpLen );             
            if ( -1 == iRet)
            {
                SecEcdr_DestroyEncoder(pstEncoder);
                return -1;
            } 

        }
        iRet = SecEcdr_EncodeUint16(pstEncoder,&pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usModLen);
        if ( -1 == iRet)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        } 

        if (pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usModLen > 0)
        {
            iRet = SecEcdr_EncodeOctets(pstEncoder,
                                        pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.pucModulus,
                                        pstWtlsParams->stParams.stKeyParam.stPubKey.stPubKey.stRsaKey.usModLen );             
            if ( -1 == iRet)
            {
                SecEcdr_DestroyEncoder(pstEncoder);
                return -1;
            } 

        }   
        /*stParams.stSecretKey*/
        iRet = SecEcdr_EncodeUint16(pstEncoder,&pstWtlsParams->stParams.stSecretKey.usIdLen);
        if ( -1 == iRet)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        } 

        if (pstWtlsParams->stParams.stSecretKey.usIdLen > 0)
        {
            iRet = SecEcdr_EncodeOctets(pstEncoder,
                                        pstWtlsParams->stParams.stSecretKey.pucIdentifier,
                                        pstWtlsParams->stParams.stSecretKey.usIdLen );   
            if ( -1 == iRet)
            {
                SecEcdr_DestroyEncoder(pstEncoder);
                return -1;
            }             
        }   
        /*ucKeyExchangeSuite*/
        iRet = SecEcdr_EncodeUint8(pstEncoder,&(pstWtlsParams->ucKeyExchangeSuite));
        if ( -1 == iRet)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }         
   }
    else
    {
        return -1;
    }
   
   *ppvMsg = WE_MALLOC(pstEncoder->iLength);
   if (NULL == *ppvMsg)
   {
       SecEcdr_DestroyEncoder(pstEncoder);
       return -1;           
   }
   WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
   SecEcdr_DestroyEncoder(pstEncoder);
   return 0; 
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeGetUserCert
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters for get user cert
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeGetUserCert
(
    WE_VOID **ppvMsg,
    WE_INT32 iTargetId,
    WE_VOID *pvBuf,
    WE_INT32 iBufLen,
    WE_UINT8 ucConnType
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;    

    iLen = 2 * sizeof(WE_INT32) + iBufLen + sizeof(WE_UINT8);
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_W_GET_USER_CERT, iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder,&iTargetId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iBufLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }   
    iRet = SecEcdr_EncodeOctets(pstEncoder,pvBuf,iBufLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    } 
    iRet = SecEcdr_EncodeUint8(pstEncoder, &ucConnType);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }  
    
    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;           
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeCompSign
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters for compute sign
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeCompSign
(
    WE_VOID **ppvMsg,
    WE_INT32 iTargetId,
    WE_UCHAR *pucKeyId,
    WE_INT32 iKeyIdLen,
    WE_UCHAR *pucBuf,
    WE_INT32 iBufLen,
    WE_UINT8 ucConnType
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;    

    iLen = 3 * sizeof(WE_INT32) + iKeyIdLen + iBufLen + sizeof(WE_UINT8);

    pstEncoder = Sec_SetMsgDataEncode(M_SEC_W_COMP_SIGN, iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder,&iTargetId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iKeyIdLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }  
    iRet = SecEcdr_EncodeOctets(pstEncoder,pucKeyId,iKeyIdLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }  
    iRet = SecEcdr_EncodeInt32(pstEncoder,&iBufLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }  
    iRet = SecEcdr_EncodeOctets(pstEncoder,pucBuf,iBufLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }  
    iRet = SecEcdr_EncodeUint8(pstEncoder, &ucConnType);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }  
    
    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;           
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeSSLHashUpdateWMasterSec
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters for ssl hash update
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeSSLHashUpdateWMasterSec
(
    WE_VOID **ppvMsg,
    WE_INT32 iMasterSecId
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1; 

    iLen = sizeof(iMasterSecId);
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_W_SSL_HASH_UPDATE,iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }
  
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iMasterSecId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;        
    }

    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;        
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeEvtShowDlgAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters for show dialog
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeEvtShowDlgAction
(
    WE_VOID **ppvMsg,
    WE_INT32 iTargetId,
    WE_VOID *pvData,
    WE_UINT32 uiLen
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;

    iLen = sizeof(WE_INT32) + sizeof(WE_UINT32) + uiLen;
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_W_EVT_SHOW_DLG_ACTION,iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder,&iTargetId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    iRet = SecEcdr_EncodeUint32(pstEncoder,&uiLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    iRet = SecEcdr_EncodeOctets(pstEncoder, pvData,uiLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;   
    
}
#endif

#ifndef BROWSER
/*==================================================================================================
FUNCTION: 
    Sec_EncodeGetCertNameList
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters for get cert name list
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeGetCertNameList
(
    WE_VOID **ppvMsg,
    WE_INT32 iTargetID,
    WE_INT32 iCertType
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;

    iLen = 2* sizeof(WE_INT32);
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_B_GET_CERT_NAME_LIST, iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iCertType);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    
    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeGenKeyPair
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters for generate key pair
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeGenKeyPair
(
    WE_VOID **ppvMsg,
    WE_INT32 iTargetID,
    WE_UINT8 ucKeyType    
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;

    iLen = sizeof(WE_INT32) + sizeof(WE_UINT8);
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_B_GEN_KEY_PAIR, iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    iRet = SecEcdr_EncodeUint8(pstEncoder, &ucKeyType);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    
    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;
}
#ifdef G_SEC_CFG_SHOW_PIN
/*==================================================================================================
FUNCTION: 
    Sec_EncodePinModify
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters for modify the pin code for private key.
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodePinModify
(
    WE_VOID **ppvMsg,
    WE_INT32 iTargetID,
    WE_UINT8 ucKeyType    
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;

    iLen = sizeof(WE_INT32) + sizeof(WE_UINT8);
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_B_PIN_MODIFY, iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    iRet = SecEcdr_EncodeUint8(pstEncoder, &ucKeyType);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    
    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeEvtChangePinAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters 
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeEvtChangePinAction
(
    WE_VOID **ppvMsg,
    WE_INT32 iTargetID, 
    St_ChangePinAction stChangePin
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;
    WE_INT32 iLenOld = 0;
    WE_INT32 iLenNew = 0;
    WE_INT32 iLenAgain = 0;
    
    if (stChangePin.pcOldPinValue != NULL)
    {
        iLenOld = WE_STRLEN(stChangePin.pcOldPinValue) + 1;
    }
    if (stChangePin.pcNewPinValue != NULL)
    {
        iLenNew = WE_STRLEN(stChangePin.pcNewPinValue) + 1;
    }
    if (stChangePin.pcNewPinValue != NULL)
    {
        iLenNew = WE_STRLEN(stChangePin.pcNewPinAgainValue) + 1;
    }    
    iLen = sizeof(WE_INT32) + sizeof(St_ChangePinAction) + iLenOld + iLenNew + iLenAgain;
    
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_B_EVT_CHANGE_PIN_ACTION, iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iLenOld);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    if (iLenOld > 1)
    {        
        iRet = SecEcdr_EncodeOctets(pstEncoder, stChangePin.pcOldPinValue, iLenOld);
        if (iRet < 0)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }        
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iLenNew);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    if (iLenNew > 1)
    {
        iRet = SecEcdr_EncodeOctets(pstEncoder, stChangePin.pcNewPinValue, iLenNew);
        if (iRet < 0)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder,&iLenAgain);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }    
    if (iLenAgain > 1)
    {
        iRet = SecEcdr_EncodeOctets(pstEncoder, stChangePin.pcNewPinAgainValue,iLenAgain);
        if (iRet < 0)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }        
    }
    iRet = SecEcdr_EncodeInt8(pstEncoder, (WE_INT8 *)&stChangePin.bIsOk);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }

    
    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;
}  
/*==================================================================================================
FUNCTION: 
    Sec_EncodeEvtCreatePinAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters 
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeEvtCreatePinAction
(
    WE_VOID **ppvMsg,
    WE_INT32 iTargetID, 
    St_CreatePinAction stGenPin
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;
    WE_INT32 iPinValueLen = 0;

    if (stGenPin.pcPinValue != NULL)
    {
        iPinValueLen = WE_STRLEN(stGenPin.pcPinValue) + 1;
    }
    iLen = sizeof(WE_INT32) + sizeof(St_CreatePin) + iPinValueLen;

    pstEncoder = Sec_SetMsgDataEncode(M_SEC_B_EVT_CREATE_PIN_ACTION, iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }

    iRet = SecEcdr_EncodeInt32(pstEncoder, &iTargetID);
    if (iRet < 0)
    {
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iPinValueLen);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    if (iPinValueLen > 1)
    {
        iRet = SecEcdr_EncodeOctets(pstEncoder,stGenPin.pcPinValue,iPinValueLen);
        if (iRet < 0)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }
    }
    iRet = SecEcdr_EncodeInt8(pstEncoder, &stGenPin.bIsOk);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }

    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;    
} 
/*==================================================================================================
FUNCTION: 
    Sec_EncodeEvtPinAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters 
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeEvtPinAction
(
    WE_VOID **ppvMsg,
    WE_INT32 iTargetID, 
    St_PinAction stPin
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;
    WE_INT32 iPinValueLen = 0;

    if (stPin.pcPinValue!= NULL)
    {
        iPinValueLen = WE_STRLEN(stPin.pcPinValue) + 1;
    }
    iLen = sizeof(WE_INT32) + sizeof(St_CreatePin) + iPinValueLen;

    pstEncoder = Sec_SetMsgDataEncode(M_SEC_B_EVT_PIN_ACTION, iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }

    iRet = SecEcdr_EncodeInt32(pstEncoder, &iTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);        
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iPinValueLen);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    if (iPinValueLen > 1)
    {
        iRet = SecEcdr_EncodeOctets(pstEncoder,stPin.pcPinValue,iPinValueLen);
        if (iRet < 0)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }
    }
    iRet = SecEcdr_EncodeInt8(pstEncoder, &stPin.bIsOk);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }

    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;    
}  
#endif
/*==================================================================================================
FUNCTION: 
    Sec_EncodeGetCurSvrCert
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters 
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeGetCurSvrCert
(
    WE_VOID **ppvMsg,
    WE_INT32 iTargetID, 
    WE_INT32 iSecId
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;

    iLen = 2* sizeof(WE_INT32);
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_B_GET_CUR_SVR_CERT, iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iSecId);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    
    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeGetSessionInfo
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters 
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeGetSessionInfo
(
    WE_VOID **ppvMsg,
    WE_INT32 iTargetID, 
    WE_INT32 iSecId
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;

    iLen = 2* sizeof(WE_INT32);
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_B_GET_SESSION_INFO, iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iSecId);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    
    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeGetWtlsCurClasss
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters 
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeGetWtlsCurClasss
(
    WE_VOID **ppvMsg,
    WE_INT32 iTargetID,
    WE_INT32 iSecId
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;

    iLen = 2* sizeof(WE_INT32);
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_B_GET_WTLS_CUR_CLASS, iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iSecId);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    
    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeGetContractsList
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters for renew session
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeGetContractsList
(
    WE_VOID **ppvMsg,
    WE_INT32 iTargetID
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;

    //iLen = 2* sizeof(WE_INT32);
    iLen = sizeof(WE_INT32);
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_B_GET_CONTRACT_LIST, iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    
    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;
}
/*==================================================================================================
FUNCTION: 
   Sec_EncodeTransferCert
CREATE DATE:
   2007-03-21
AUTHOR:
   wang shuhua
DESCRIPTION:
   Encode parameters 
ARGUMENTS PASSED:
   ppvMsg: address of data to send
RETURN VALUE:
   success : 0
   fail : -1
USED GLOBAL VARIABLES:
   None
USED STATIC VARIABLES:
   None
CALL BY:
   Omit
IMPORTANT NOTES:
   None
==================================================================================================*/
   /*for transferring certificate*/
WE_INT32 Sec_EncodeTransferCert
(    
    WE_VOID **ppvMsg, 
    St_SecCertContent stCertContent
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = -1;
    WE_INT32 iMimeLen = 0;

    if (stCertContent.pcMime != NULL)
    {
        iMimeLen = WE_STRLEN(stCertContent.pcMime) + 1;
    }

    iLen = 2* sizeof(WE_INT32) + stCertContent.uiCertLen + iMimeLen;
    pstEncoder = Sec_SetMsgDataEncode(M_SEC_B_TRANSFER_CERT, iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }
    iRet = SecEcdr_EncodeUint32(pstEncoder, &stCertContent.uiCertLen);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    if (stCertContent.uiCertLen > 0 && stCertContent.pucCertData != NULL)
    {
        iRet = SecEcdr_EncodeOctets(pstEncoder,stCertContent.pucCertData,stCertContent.uiCertLen);
        if (iRet < 0)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iMimeLen);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    if (iMimeLen > 1)
    {
        iRet = SecEcdr_EncodeOctets(pstEncoder,stCertContent.pcMime,iMimeLen);
        if (iRet < 0)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }
    }
    
    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeGetRequestUserCert
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters for renew session
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
/* generate request of user certificate in Base64 DER code. */
WE_INT32 Sec_EncodeGetRequestUserCert
(   
    WE_VOID **ppvMsg, 
    St_SecUserCertReqInfo stShowContractContent
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = 0;

    iLen = sizeof(WE_INT32) + 7 * sizeof(WE_UINT16) + stShowContractContent.usCountryLen 
        + stShowContractContent.usProvinceLen + stShowContractContent.usCityLen 
        + stShowContractContent.usCompanyLen + stShowContractContent.usDepartmentLen 
        + stShowContractContent.usNameLen + stShowContractContent.usEMailLen + 3*sizeof(WE_UCHAR);

    pstEncoder = Sec_SetMsgDataEncode(M_SEC_B_GET_REQUEST_USER_CERT, iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }

    iRet = SecEcdr_EncodeInt32(pstEncoder, &stShowContractContent.iTargetID);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    iRet = SecEcdr_EncodeUint16(pstEncoder, &stShowContractContent.usCountryLen);
    iRet = SecEcdr_EncodeOctets(pstEncoder, stShowContractContent.pucCountry, stShowContractContent.usCountryLen);
    iRet = SecEcdr_EncodeUint16(pstEncoder, &stShowContractContent.usProvinceLen);
    iRet = SecEcdr_EncodeOctets(pstEncoder, stShowContractContent.pucProvince, stShowContractContent.usProvinceLen);
    iRet = SecEcdr_EncodeUint16(pstEncoder, &stShowContractContent.usCityLen);
    iRet = SecEcdr_EncodeOctets(pstEncoder, stShowContractContent.pucCity, stShowContractContent.usCityLen); 
    iRet = SecEcdr_EncodeUint16(pstEncoder, &stShowContractContent.usCompanyLen);
    iRet = SecEcdr_EncodeOctets(pstEncoder, stShowContractContent.pucCompany, stShowContractContent.usCompanyLen);     
    iRet = SecEcdr_EncodeUint16(pstEncoder, &stShowContractContent.usDepartmentLen);
    iRet = SecEcdr_EncodeOctets(pstEncoder, stShowContractContent.pucDepartment, stShowContractContent.usDepartmentLen); 
    iRet = SecEcdr_EncodeUint16(pstEncoder, &stShowContractContent.usNameLen);
    iRet = SecEcdr_EncodeOctets(pstEncoder, stShowContractContent.pucName, stShowContractContent.usNameLen);
    iRet = SecEcdr_EncodeUint16(pstEncoder, &stShowContractContent.usEMailLen);
    iRet = SecEcdr_EncodeOctets(pstEncoder, stShowContractContent.pucEMail, stShowContractContent.usEMailLen); 

    iRet = SecEcdr_EncodeUint8(pstEncoder,&stShowContractContent.ucCertUsage);
    iRet = SecEcdr_EncodeUint8(pstEncoder,&stShowContractContent.ucKeyUsage);
    iRet = SecEcdr_EncodeUint8(pstEncoder,&stShowContractContent.ucSignType); 
    
    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeEvtConfirmAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters 
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
/*user event response interface*/
WE_INT32 Sec_EncodeEvtConfirmAction
(
   WE_VOID **ppvMsg,
   WE_INT32 iTargetID,
   St_ConfirmAction stConfirm
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = 0;

    iLen = sizeof(WE_INT32) + sizeof(St_HashAction);

    pstEncoder = Sec_SetMsgDataEncode(M_SEC_B_EVT_CONFIRM_ACTION,iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }

    iRet = SecEcdr_EncodeInt32(pstEncoder,&iTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      
    iRet = SecEcdr_EncodeUint8(pstEncoder,&stConfirm.bIsOk);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      

    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeEvtNameConfirmAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters 
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeEvtNameConfirmAction
(   
    WE_VOID **ppvMsg,
    WE_INT32 iTargetID,
    St_NameConfirmAction stNameConfirm
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = 0;

    iLen = sizeof(WE_INT32) + sizeof(St_HashAction);

    pstEncoder = Sec_SetMsgDataEncode(M_SEC_B_EVT_NAME_CONFIRM_ACTION,iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }

    iRet = SecEcdr_EncodeInt32(pstEncoder,&iTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      
    iRet = SecEcdr_EncodeUint8(pstEncoder,&stNameConfirm.bIsOk);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      

    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeEvtHashAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters for renew session
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeEvtHashAction
(   
    WE_VOID **ppvMsg,
    WE_INT32 iTargetID,
    St_HashAction stHash
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = 0;
    WE_INT32 iTextLen = 0;

    if (stHash.pcHashValue!= NULL)
    {
        iTextLen = WE_STRLEN(stHash.pcHashValue) + 1;
    }

    iLen = sizeof(WE_INT32) + sizeof(St_HashAction) + iTextLen;

    pstEncoder = Sec_SetMsgDataEncode(M_SEC_B_EVT_STORE_CERT_ACTION,iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }

    iRet = SecEcdr_EncodeInt32(pstEncoder,&iTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }     
    iRet = SecEcdr_EncodeInt32(pstEncoder,&iTextLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }   
    if (iTextLen > 1)
    {
        iRet = SecEcdr_EncodeOctets(pstEncoder,stHash.pcHashValue,iTextLen);
        if (iRet < 0)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }
    }
    iRet = SecEcdr_EncodeUint8(pstEncoder,&stHash.bIsOk);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      

    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeEvtShowCertContentAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters 
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeEvtShowCertContentAction
(    
    WE_VOID **ppvMsg,
    WE_INT32 iTargetID,
    St_ShowCertContentAction stShowCertContent
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = 0;

    iLen = sizeof(WE_INT32) + sizeof(St_ShowCertContentAction);

    pstEncoder = Sec_SetMsgDataEncode(M_SEC_B_EVT_SHOW_CERT_CONTENT_ACTION,iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }

    iRet = SecEcdr_EncodeInt32(pstEncoder,&iTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      
    iRet = SecEcdr_EncodeUint8(pstEncoder,&stShowCertContent.bIsOk);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      

    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeEvtShowCertListAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters 
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeEvtShowCertListAction
(    
    WE_VOID **ppvMsg,
    WE_INT32 iTargetID,
    St_ShowCertListAction stShowCertList
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = 0;

    iLen = 3* sizeof(WE_INT32) + sizeof(WE_UINT8);

    pstEncoder = Sec_SetMsgDataEncode(M_SEC_B_EVT_SHOW_CERT_LIST_ACTION,iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }

    iRet = SecEcdr_EncodeInt32(pstEncoder,&iTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      
    iRet = SecEcdr_EncodeInt32(pstEncoder,&stShowCertList.iSelId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }     
    iRet = SecEcdr_EncodeInt32(pstEncoder, (WE_INT32 *)(&(stShowCertList.eRKeyType)));
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    } 
    iRet = SecEcdr_EncodeUint8(pstEncoder,&stShowCertList.bIsOk);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      

    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeEvtShowContractsListAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters 
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeEvtShowContractsListAction
(    
    WE_VOID **ppvMsg,
    WE_INT32 iTargetID,
    St_ShowContractsListAction stShowContractsList
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = 0;

    iLen = sizeof(WE_INT32) + sizeof(St_SelectCertAction);

    pstEncoder = Sec_SetMsgDataEncode(M_SEC_B_EVT_SHOW_CONTRACTS_LIST_ACTION,iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }

    iRet = SecEcdr_EncodeInt32(pstEncoder,&iTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      
    iRet = SecEcdr_EncodeInt32(pstEncoder,&stShowContractsList.iSelId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }     

    iRet = SecEcdr_EncodeUint8(pstEncoder,&stShowContractsList.bIsOk);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      

    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeEvtStoreCertAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters 
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeEvtStoreCertAction
(    
    WE_VOID **ppvMsg,
    WE_INT32 iTargetID,
    St_StoreCertAction stStoreCert
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = 0;
    WE_INT32 iTextLen = 0;

    if (stStoreCert.pcText != NULL)
    {
        iTextLen = WE_STRLEN(stStoreCert.pcText) + 1;
    }

    iLen = sizeof(WE_INT32) + sizeof(St_StoreCertAction) + iTextLen;

    pstEncoder = Sec_SetMsgDataEncode(M_SEC_B_EVT_STORE_CERT_ACTION,iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }

    iRet = SecEcdr_EncodeInt32(pstEncoder,&iTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }     
    iRet = SecEcdr_EncodeInt32(pstEncoder,&iTextLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }   
    if (iTextLen > 1)
    {
        iRet = SecEcdr_EncodeOctets(pstEncoder,stStoreCert.pcText,iTextLen);
        if (iRet < 0)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }
    }
    iRet = SecEcdr_EncodeUint8(pstEncoder,&stStoreCert.bIsOk);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      

    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    Sec_EnocdeEvtShowContractContentAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters 
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EnocdeEvtShowContractContentAction
(
    WE_VOID **ppvMsg,
    WE_INT32 iTargetID,
    St_ShowContractContentAction stShowContractContent
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = 0;

    iLen = sizeof(WE_INT32) + sizeof(St_ShowContractContentAction);

    pstEncoder = Sec_SetMsgDataEncode(M_SEC_B_EVT_SHOW_CONTRACT_CONTENT_ACTION,iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }

    iRet = SecEcdr_EncodeInt32(pstEncoder,&iTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      
    iRet = SecEcdr_EncodeUint8(pstEncoder,&stShowContractContent.bIsOk);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      

    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;    
}
#endif

#ifndef SIGNTEXT
/*==================================================================================================
FUNCTION: 
    Sec_EncodeHandle
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters for renew session
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeHandle
(
    WE_VOID **ppvMsg,
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
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = 0;
    WE_INT32 iTextLen = 0;

    if ( pcText != NULL)
    {
        iTextLen = WE_STRLEN(pcText) + 1;
    }
    iLen = 7* sizeof(WE_INT32) + iTextLen + iKeyIdLen + iStringToSignLen;

    pstEncoder = Sec_SetMsgDataEncode(M_SEC_S_HANDLE,iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }

    iRet = SecEcdr_EncodeInt32(pstEncoder,&iTargetID);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      
    iRet = SecEcdr_EncodeInt32(pstEncoder,&iSignId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      
    iRet = SecEcdr_EncodeInt32(pstEncoder,&iTextLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      
    iRet = SecEcdr_EncodeOctets(pstEncoder,pcText,iTextLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      
    iRet = SecEcdr_EncodeInt32(pstEncoder,&iKeyIdType);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      
    iRet = SecEcdr_EncodeInt32(pstEncoder,&iKeyIdLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      
    iRet = SecEcdr_EncodeOctets(pstEncoder,pcKeyId,iKeyIdLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      
    iRet = SecEcdr_EncodeInt32(pstEncoder,&iStringToSignLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      
    iRet = SecEcdr_EncodeOctets(pstEncoder,pcStringToSign,iStringToSignLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      
    iRet = SecEcdr_EncodeInt32(pstEncoder, &iOptions);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      

    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg,pstEncoder->pucData,pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;
    
}
/*==================================================================================================
FUNCTION: 
    Sec_EncodeEvtSelectCertAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_EncodeEvtSelectCertAction
(
    WE_VOID **ppvMsg,
    WE_INT32 iTargetId,
    St_SelectCertAction stSelectCert
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iRet = 0;

    iLen = sizeof(WE_INT32) + sizeof(St_SelectCertAction);

    pstEncoder = Sec_SetMsgDataEncode(M_SEC_S_EVT_SELECT_CERT_ACTION,iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }

    iRet = SecEcdr_EncodeInt32(pstEncoder,&iTargetId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      
    iRet = SecEcdr_EncodeInt32(pstEncoder,&stSelectCert.iSelId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }     

    iRet = SecEcdr_EncodeUint8(pstEncoder,&stSelectCert.bIsOk);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      

    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;    
}
/*==================================================================================================
FUNCTION: 
    SecS_EncodeEvtPinAction
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Encode parameters 
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 SecS_EncodeEvtPinAction
(
    WE_VOID **ppvMsg,
    WE_INT32 iTargetId,
    St_PinAction stPin
)
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iLen = 0;
    WE_INT32 iPinValueLen = 0;
    WE_INT32 iRet = 0;

    if (stPin.pcPinValue != NULL)
    {
        iPinValueLen = WE_STRLEN(stPin.pcPinValue) + 1;
    }

    iLen = sizeof(WE_INT32) + sizeof(St_PinAction) + iPinValueLen;

    pstEncoder = Sec_SetMsgDataEncode(M_SEC_S_EVT_PIN_ACTION,iLen);
    if (NULL == pstEncoder)
    {
        return -1;
    }

    iRet = SecEcdr_EncodeInt32(pstEncoder,&iTargetId);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      
    iRet = SecEcdr_EncodeInt32(pstEncoder,&iPinValueLen);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }     
    if (iPinValueLen > 1)
    {
        iRet = SecEcdr_EncodeOctets(pstEncoder,stPin.pcPinValue,iPinValueLen);
        if ( -1 == iRet)
        {
            SecEcdr_DestroyEncoder(pstEncoder);
            return -1;
        }    
    }
    iRet = SecEcdr_EncodeUint8(pstEncoder,&stPin.bIsOk);
    if ( -1 == iRet)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }      

    *ppvMsg = WE_MALLOC(pstEncoder->iLength);
    if (NULL == *ppvMsg)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return -1;
    }
    WE_MEMCPY(*ppvMsg, pstEncoder->pucData, pstEncoder->iLength);
    SecEcdr_DestroyEncoder(pstEncoder);
    return 0;    
}
#endif
/*==================================================================================================
FUNCTION: 
    Sec_GetMsgLen
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    get message lenght contain header length
ARGUMENTS PASSED:
    pvBuf: message 
RETURN VALUE:
    success : length of message
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Sec_GetMsgLen(WE_VOID *pvBuf)
{
    WE_UINT8 ucType = 0;
    WE_INT32 iLen = 0;
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pvBuf)
    {
        return -1;
    }

    pstDecoder = SecEcdr_CreateDecoder( (WE_UCHAR *)pvBuf, SECD_MSG_HEADER_LENGTH );
    if (NULL == pstDecoder)
    {
        return -1;
    }
    if (-1 == SecEcdr_DecodeUint8( pstDecoder, &ucType ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, &iLen))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    SecEcdr_DestroyDecoder( pstDecoder );

    return iLen + SECD_MSG_HEADER_LENGTH;
} 
/*==================================================================================================
FUNCTION: 
    Sec_SetMsgDataEncode
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    create encoder by message
ARGUMENTS PASSED:
    ppvMsg: address of data to send
RETURN VALUE:
    success : pointer of encode
    fail : null
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static St_SecEcdrEncoder *Sec_SetMsgDataEncode(WE_UINT8 ucMsgId, WE_INT32 iDataLength )
{
    St_SecEcdrEncoder *pstEncoder = NULL;
    WE_INT32 iRet = 0;
    
    pstEncoder = SecEcdr_CreateEncoder(SECD_MSG_HEADER_LENGTH + iDataLength);
    if (NULL == pstEncoder)
    {
        return NULL;
    }
    iRet = SecEcdr_EncodeUint8(pstEncoder, &ucMsgId);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return NULL;
    }
    iRet = SecEcdr_EncodeInt32(pstEncoder,&iDataLength);
    if (iRet < 0)
    {
        SecEcdr_DestroyEncoder(pstEncoder);
        return NULL;
    }
    return pstEncoder;
    
}
/*==================================================================================================
FUNCTION: 
    Sec_GetMsgDecoder
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Create decoder
ARGUMENTS PASSED:
    pvBuf: message
RETURN VALUE:
    success : pointer of decoder
    fail : null
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static St_SecEcdrDecoder* Sec_GetMsgDecoder(WE_VOID* pvBuf)
{
    St_SecEcdrDecoder* pstDecodrTmp = NULL;
    St_SecEcdrDecoder *pstDecodr = NULL;
    WE_INT32 iMsgLen = 0;
    WE_INT32 iRet = 0;

    if (NULL == pvBuf)
    {
        return NULL;
    }

    pstDecodrTmp = SecEcdr_CreateDecoder((WE_UCHAR*)pvBuf, SECD_MSG_HEADER_LENGTH);
    if (NULL == pstDecodrTmp)
    {
        return NULL;
    }
    iRet = SecEcdr_ChangeDecodePosition(pstDecodrTmp,SEC_MSG_TYPE_LENGTH);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecodrTmp);
        return NULL;
    }
    iRet = SecEcdr_DecodeInt32(pstDecodrTmp,&iMsgLen);
    if (iRet < 0)
    {
        SecEcdr_DestroyDecoder(pstDecodrTmp);
        return NULL;
    }    
    SecEcdr_DestroyDecoder(pstDecodrTmp);

    pstDecodr = SecEcdr_CreateDecoder((WE_UCHAR*)pvBuf + SECD_MSG_HEADER_LENGTH, iMsgLen);
    if (NULL == pstDecodr)
    {
        return NULL;
    }
    return pstDecodr;
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeSecDResp
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    Decode message from secd
ARGUMENTS PASSED:
    pvMsg: meesage
    piEvent: address of message type
    ppvResp: address of messge content
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
/*wsh add *ppvResp free by app*/
WE_INT32 Sec_DecodeSecDResp
(
    WE_VOID *pvMsg,
    WE_INT32 *piEvent,
    WE_VOID **ppvResp
)
{
     WE_INT32 iRet = -1;
     WE_UINT8 ucMsgType = 0;

     if ((NULL == pvMsg) || (NULL == piEvent) || (NULL == ppvResp))
     {
        return -1;
     }

     ucMsgType = Sec_GetMsgType(pvMsg);
     *piEvent = (WE_INT32)ucMsgType;
     switch (ucMsgType)
     {
        /*Signtext*/
        case G_SIGNTEXT_RESP:
            Sec_GetSignTextResp(pvMsg,ppvResp);
            break;
        /*Secw*/
        case G_SEARCHPEER_RESP:
            Sec_GetSearchPeerResp(pvMsg,ppvResp);
            break;
        case G_SESSIONGET_RESP:
            Sec_GetSessionGetResp(pvMsg,ppvResp);
            break;
        case G_VERIFYSVRCERT_RESP:
            Sec_GetVerifySvrCertResp(pvMsg,ppvResp);
            break;
        case G_GETPRF_RESP:
            iRet = Sec_GetPrfResp(pvMsg,ppvResp);
            break;   
        case G_GETUSERCERT_RESP:
            iRet = Sec_GetGetUsrCertResp(pvMsg,ppvResp);
            break;
        case G_COMPUTESIGN_RESP:
            iRet = Sec_GetCompSignResp(pvMsg,ppvResp);
            break;
        case G_GETCIPHERSUITE_RESP:
            iRet = Sec_GetGetCipherSuiteResp(pvMsg,ppvResp);
            break;
        case G_TLS_GETCIPHERSUITE_RESP:
            iRet = Sec_GetTlsCipherSuiteResp(pvMsg,ppvResp);
            break;
        case G_TLS_GETUSERCERT_RESP:
            iRet = Sec_GetTlsUsrCertResp(pvMsg,ppvResp);
            break;
        case G_TLS_GETMASTERSECRET_RESP:
            iRet = Sec_GetSSLMasterSecretResp(pvMsg,ppvResp);
            break;
        case G_KEYEXCH_RESP:
            iRet = Sec_GetKeyExchangeResp(pvMsg,ppvResp);
            break;
        /*browser*/
        case G_GENKEYPAIR_RESP:
            iRet = Sec_GetKeyPairGenResp(pvMsg,ppvResp);
            break;
        case G_WTLSCURCLASS_RESP:
            iRet = Sec_GetWtlsCurClassResp(pvMsg,ppvResp);
            break;
        case G_VIEWGETCERT_RESP:
            iRet = Sec_GetViewAndCertResp(pvMsg,ppvResp);
            break;
        case G_DELCERT_RESP:
            iRet = Sec_GetDelCertResp(pvMsg,ppvResp);
            break;
        case G_GETCERTNAME_RESP:
            iRet = Sec_GetGetCertNameListResp(pvMsg,ppvResp);
            break;
        case G_GETSESSIONINFO_RESP:
            iRet = Sec_GetGetSessionInfoResp(pvMsg,ppvResp);
            break;
        case G_GETCURSVRCERT_RESP:
            iRet = Sec_GetGetCurSvrCertResp(pvMsg,ppvResp);
            break;
        case G_GETCONTRACT_RESP:
            iRet = Sec_GetGetContractsListResp(pvMsg,ppvResp);
            break;
        case G_DELCONTRACT_RESP:
            iRet = Sec_GetRemoveContractResp(pvMsg,ppvResp);
            break;
        case G_USERCERTREQ_RESP:
            iRet = Sec_GetUsrCertReqResp(pvMsg,ppvResp);
            break;
        case G_CHANGECERTSTATE_RESP:
            iRet = Sec_GetChangeCertStateResp(pvMsg,ppvResp);
            break;
        /*sec_resp*/
        case G_SESSIONCLEAR_RESP:
            iRet = Sec_GetClrSessionResp(pvMsg,ppvResp);
            break;
        case G_GETPUBKEY_RESP:
            iRet = Sec_GetGetPubKeyResp(pvMsg,ppvResp);
            break;
        /*sec_ueh*/
        case G_Evt_CONFIRM:
            iRet = Sec_DecodeConfirm(pvMsg,ppvResp);
            break;
        case G_Evt_CHANGEPIN:
            iRet = Sec_DecodeChangePin(pvMsg,ppvResp);
            break;
        case G_Evt_SELECTCERT:
            iRet = Sec_DecodeSelCert(pvMsg,ppvResp);
            break;
        case G_Evt_NAMECONFIRM:
            iRet = Sec_DecodeNameConfirm(pvMsg,ppvResp);
            break;
        case G_Evt_SIGNTEXTCONFIRM:
            iRet = Sec_DecodeSigntextConfirm(pvMsg,ppvResp);
            break;
        case G_Evt_CREATEPIN:
            iRet = Sec_DecodeCreatePin(pvMsg,ppvResp);
            break;
        case G_Evt_HASH:
            iRet = Sec_DecodeHash(pvMsg,ppvResp);
            break;
        case G_Evt_PIN:
            iRet = Sec_DecodePin(pvMsg,ppvResp);
            break;
        case G_Evt_SHOWCERTCONTENT:
            iRet = Sec_DecodeShowCertContent(pvMsg,ppvResp);
            break;
        case G_Evt_SHOWCERTLIST:
            iRet = Sec_DecodeShowCertList(pvMsg,ppvResp);
            break;
        case G_Evt_SHOWSESSIONCONTENT:
            iRet = Sec_DecodeShowSessionContent(pvMsg,ppvResp);
            break;
        case G_Evt_SHOWCONTRACTLIST:
            iRet = Sec_DecodeShowContractList(pvMsg,ppvResp);
            break;
        case G_Evt_STORECERT:
            iRet = Sec_DecodeStoreCert(pvMsg,ppvResp);
            break;
        case G_Evt_WARNING:
            iRet = Sec_DecodeWarning(pvMsg,ppvResp);
            break;
        case G_Evt_SHOWCONTRACTCONTENT:
            iRet = Sec_DecodeStoreContract(pvMsg,ppvResp);
            break;
        case G_Evt_SHOWCURSVRCERT:
            iRet = Sec_DecodeCurSvrCert(pvMsg,ppvResp);
            break;
        default:
            WE_LOGERROR("unknown message type");
            break;
     }
     return iRet;
}

/*==================================================================================================
FUNCTION: 
    Sec_GetMsgType
CREATE DATE:
    2007-03-21
AUTHOR:
    wang shuhua
DESCRIPTION:
    get message type
ARGUMENTS PASSED:
    pvBuf: message
RETURN VALUE:
    success : 0
    fail : -1
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_UINT8 Sec_GetMsgType(WE_VOID *pvBuf)
{
    WE_UINT8 ucType = 0;
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (!pvBuf)
    {
        return 0;
    }

    pstDecoder = SecEcdr_CreateDecoder( (WE_UCHAR *)pvBuf, SEC_MSG_TYPE_LENGTH );
    if (!pstDecoder)
    {
        return 0;
    }
    if (-1 == SecEcdr_DecodeUint8( pstDecoder, &ucType ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return 0;
    }
    SecEcdr_DestroyDecoder( pstDecoder );

    return ucType;
}
/*==================================================================================================
FUNCTION: 
    Sec_GetSignTextResp
CREATE DATE:
    2007-03-16
AUTHOR:
    wangshuhua
DESCRIPTION:
    Generate get sign text response from bytes stream
ARGUMENTS PASSED:
    pvMsg: [in] bytes stream
    ppvResp:[out] address response structure
RETURN VALUE:
    0: success.
    -1: fail.
==================================================================================================*/
static WE_INT32 Sec_GetSignTextResp(WE_VOID *pvMsg,WE_VOID** ppvResp)
{
    St_SecSignTextResp *pstSignTextResp = NULL;
    WE_INT32 iRet = 0;

    pstSignTextResp = (St_SecSignTextResp*)WE_MALLOC(sizeof(St_SecSignTextResp));
    if (NULL == pstSignTextResp)
    {
        return -1;
    }
    iRet = Sec_DecodeSignTextResp(pvMsg, 
                           &(pstSignTextResp->iTargetID),
                           &(pstSignTextResp->iSignId),
                           (WE_INT32 *)(&(pstSignTextResp->ucAlgorithm)),
                           &(pstSignTextResp->pcSignature),
                           (WE_INT32 *)(&(pstSignTextResp->usSigLen)),
                           &(pstSignTextResp->pcHashedKey),
                           (WE_INT32 *)(&(pstSignTextResp->usHashedKeyLen)),
                           &(pstSignTextResp->pcCertificate),
                           (WE_INT32 *)(&(pstSignTextResp->usCertificateLen)),
                           (WE_INT32 *)(&(pstSignTextResp->ucCertificateType)),
                           (WE_INT32 *)(&(pstSignTextResp->ucErr))
                           );
    if (iRet < 0)
    {
        WE_FREE(pstSignTextResp);
        return -1;
    }
    *ppvResp = (WE_VOID*)pstSignTextResp;
    return 0;
    
                           
}

/*server end decode*/
/*1. response*/
WE_INT32 Sec_DecodeSignTextResp(
                        WE_UINT8  *pcBuffer, WE_INT32 *piTargetID,
                        WE_INT32 *piSignId, WE_INT32 *piAlgorithm,
                         WE_CHAR **ppcSignature, WE_INT32 *piSigLen,
                         WE_CHAR **ppcHashedKey, WE_INT32 *piHashedKeyLen,
                         WE_CHAR **ppcCertificate, WE_INT32 *piCertificateLen,
                        WE_INT32 *piCertificateType, WE_INT32 *piErr)
{
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piSignId )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piAlgorithm )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piSigLen )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piHashedKeyLen )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piCertificateLen )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piCertificateType )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piErr ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (*piSigLen > 0)
    {
        *ppcSignature = (WE_CHAR *)WE_MALLOC((WE_ULONG)(*piSigLen) * sizeof(WE_CHAR));
        if (NULL == *ppcSignature)
        {
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
        if (SecEcdr_DecodeOctets( pstDecoder, *ppcSignature, *piSigLen ) < 0)
        {
            WE_FREE(*ppcSignature);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    }
    
    if (*piHashedKeyLen > 0)
    {
        *ppcHashedKey = (WE_CHAR *)WE_MALLOC((WE_ULONG)(*piHashedKeyLen) * sizeof(WE_CHAR));
        if (NULL == *ppcHashedKey)
        {
            WE_FREE(*ppcSignature);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
        if (SecEcdr_DecodeOctets( pstDecoder, *ppcHashedKey, *piHashedKeyLen ) < 0)
        {
            WE_FREE(*ppcHashedKey);
            WE_FREE(*ppcSignature);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    }
    
    if (*piCertificateLen > 0)
    {
        *ppcCertificate = (WE_CHAR *)WE_MALLOC((WE_ULONG)(*piCertificateLen) * sizeof(WE_CHAR));
        if (NULL == *ppcCertificate)
        {
            WE_FREE(*ppcHashedKey);
            WE_FREE(*ppcSignature);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
        if (SecEcdr_DecodeOctets( pstDecoder, *ppcCertificate, *piCertificateLen ) < 0)
        {
            WE_FREE(*ppcCertificate);
            WE_FREE(*ppcHashedKey);
            WE_FREE(*ppcSignature);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    }
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_GetPrfResp
CREATE DATE:
    2007-03-16
AUTHOR:
    wangshuhua
DESCRIPTION:
    Generate get prf response from bytes stream
ARGUMENTS PASSED:
    pvMsg: [in] bytes stream
    ppvResp:[out] response structure
RETURN VALUE:
    0: success.
    -1: fail.
==================================================================================================*/
static WE_INT32 Sec_GetPrfResp(WE_VOID *pvMsg,WE_VOID **ppvResp)
{
    St_SecGetPrfResp *pstResp = NULL;
    WE_INT32 iRet = 0;

    pstResp = (St_SecGetPrfResp*)WE_MALLOC(sizeof(St_SecGetPrfResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeGetPrfResultResp(pvMsg,
                                   &(pstResp->iTargetID),
                                   (WE_INT32 *)(&(pstResp->usResult)),
                                   &(pstResp->pucBuf),
                                   (WE_INT32 *)&(pstResp->usBufLen));
    if (iRet < 0)
    {
        WE_FREE(pstResp);
        return -1;
    }
    *ppvResp = (WE_VOID*)pstResp;
    return 0;
       
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeGetPrfResultResp
CREATE DATE:
    2007-03-19
AUTHOR:
    Bird
DESCRIPTION:
    decode prf result.
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piResult[OUT]:the result of get prf.
    WE_UCHAR **ppucResBuf[OUT]:the value of prf.
    WE_INT32 *piBufLen[OUT]:Length of prf.
RETURN VALUE:
    0: success.
    -1: fail.
==================================================================================================*/
WE_INT32 Sec_DecodeGetPrfResultResp(WE_UINT8  *pcBuffer,
                              WE_INT32 *piTargetID, WE_INT32 *piResult,
                              WE_UCHAR **ppucResBuf, WE_INT32 *piBufLen)
{
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piResult )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piBufLen ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (*piBufLen > 0)
    {
        *ppucResBuf = (WE_UCHAR *)WE_MALLOC((WE_ULONG)(*piBufLen) * sizeof(WE_UCHAR));
        if (NULL == *ppucResBuf)
        {
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
        if (SecEcdr_DecodeOctets( pstDecoder, *ppucResBuf, *piBufLen ) < 0)
        {
            WE_FREE(*ppucResBuf);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    }
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_GetVerifySvrCertResp
CREATE DATE:
    2007-03-19
AUTHOR:
    wangshuhua
DESCRIPTION:
    Generate get prf response from bytes stream
ARGUMENTS PASSED:
    pvMsg: [in] bytes stream
    ppvResp:[out] response structure
RETURN VALUE:
    0: success.
    -1: fail.
==================================================================================================*/
static WE_INT32 Sec_GetVerifySvrCertResp(WE_VOID *pvMsg,WE_VOID **ppvResp)
{
    St_SecVerifySvrCertResp *pstResp = NULL;
    WE_INT32 iRet = 0;

    pstResp = (St_SecVerifySvrCertResp*)WE_MALLOC(sizeof(St_SecVerifySvrCertResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeWtlsVerifySvrCertResp(
                                           pvMsg,
                                           &(pstResp->iTargetID),
                                           (WE_INT32 *)&(pstResp->usResult)
                                           );
    if (iRet < 0)
    {
        WE_FREE(pstResp);
        return -1;
    }
    *ppvResp = (WE_VOID*)pstResp;
    return 0;
       
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeWtlsVerifySvrCertResp                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode encoded data and get wtls verify svr cert response.
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piResult[OUT]:the result of verify svr cert.
RETURN VALUE:
    0: success.
    -1: fail.
IMPORTANT NOTES:
    None.
==================================================================================================*/

WE_INT32 Sec_DecodeWtlsVerifySvrCertResp(WE_UINT8  *pcBuffer,
                              WE_INT32 *piTargetID, WE_INT32 *piResult)
{
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piResult ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}
static WE_INT32 Sec_GetSSLMasterSecretResp(WE_VOID *pvMsg,WE_VOID **ppvResp)
{
    St_SecSSLGetMasterSecretResp *pstResp = NULL;
    WE_INT32 iRet = -1;

    if (NULL == ppvResp)
    {
        return -1;
    }

    pstResp = (St_SecSSLGetMasterSecretResp*)WE_MALLOC(sizeof(St_SecSSLGetMasterSecretResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeSSLGetMasterSecretResp(pvMsg,
                                            &(pstResp->iTargetID),
                                            (WE_INT32 *)&(pstResp->usResult),
                                            &(pstResp->pucMasterSecret),
                                            &(pstResp->iMasterSecretLen));
    
    if (iRet < 0)
    {
        WE_FREE(pstResp);
        return -1;
    }
    *ppvResp = (WE_VOID*)pstResp;
    return 0;    
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodeSSLGetMasterSecretResp
CREATE DATE:
    2007-03-19
AUTHOR:
    tang
DESCRIPTION:
    get master secret for ssl hash updata.
ARGUMENTS PASSED:
    pcBuffer: [in] bytes stream
    piTargetID:[out] response structure
    piResult:[out]:result
    ppucMasterSecret:[out]:master secret 
    piMasterSecretLen:[out]:lenght of master secret .
RETURN VALUE:
    0: success.
    -1: fail.
==================================================================================================*/
WE_INT32 Sec_DecodeSSLGetMasterSecretResp(WE_UINT8  *pcBuffer, WE_INT32 *piTargetID, WE_INT32 *piResult,
                         WE_UCHAR **ppucMasterSecret,WE_INT32 *piMasterSecretLen)
{
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piResult )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piMasterSecretLen ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (*piMasterSecretLen > 0)
    {
        *ppucMasterSecret = (WE_UCHAR *)WE_MALLOC((WE_ULONG)(*piMasterSecretLen) * sizeof(WE_UCHAR));
        if (NULL == *ppucMasterSecret)
        {
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
        if (SecEcdr_DecodeOctets( pstDecoder, *ppucMasterSecret, *piMasterSecretLen ) < 0)
        {
            WE_FREE(*ppucMasterSecret);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    }
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_GetGetUsrCertResp
CREATE DATE:
    2007-03-19
AUTHOR:
    wangshuhua
DESCRIPTION:
    Generate get prf response from bytes stream
ARGUMENTS PASSED:
    pvMsg: [in] bytes stream
    ppvResp:[out] response structure
RETURN VALUE:
    0: success.
    -1: fail.
==================================================================================================*/
static WE_INT32 Sec_GetGetUsrCertResp(WE_VOID *pvMsg,WE_VOID **ppvResp)
{
    St_SecGetUserCertResp *pstResp = NULL;
    WE_INT32 iRet = 0;

    pstResp = (St_SecGetUserCertResp*)WE_MALLOC(sizeof(St_SecGetUserCertResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeWtlsGetUsrCertResp(pvMsg,
                                        &(pstResp->iTargetID),
                                        (WE_INT32 *)&(pstResp->usResult),
                                        &(pstResp->pucKeyId),
                                        (WE_INT32 *)&(pstResp->usKeyIdLen),
                                        &(pstResp->pucCert),
                                        (WE_INT32 *)&(pstResp->usCertLen));
    if (iRet < 0)
    {
        WE_FREE(pstResp);
        return -1;
    }
    *ppvResp = (WE_VOID*)pstResp;
    return 0;
       
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeWtlsGetUsrCertResp                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode encoded data and get wtls user cert response.
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piResult[OUT]:the result of verify svr cert.
    WE_UCHAR **ppucKeyId[OUT]:key id.
    WE_INT32 *piKeyIdLen[OUT]:length of key id.
    WE_UCHAR **ppucCert[OUT]:certificate.
    WE_INT32 *piCertLen[OUT]:length of certificate.
RETURN VALUE:
    0: success.
    -1: fail.
IMPORTANT NOTES:
    None.
==================================================================================================*/
WE_INT32 Sec_DecodeWtlsGetUsrCertResp(WE_UINT8  *pcBuffer, 
                            WE_INT32 *piTargetID, WE_INT32 *piResult,
                             WE_UCHAR **ppucKeyId, WE_INT32 *piKeyIdLen,
                             WE_UCHAR **ppucCert, WE_INT32 *piCertLen)
{
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piResult )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piKeyIdLen )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piCertLen ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (*piKeyIdLen > 0)
    {
        *ppucKeyId = (WE_UCHAR *)WE_MALLOC((WE_ULONG)(*piKeyIdLen) * sizeof(WE_UCHAR));
        if (NULL == *ppucKeyId)
        {
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
        if (SecEcdr_DecodeOctets( pstDecoder, *ppucKeyId, *piKeyIdLen ) < 0)
        {
            WE_FREE(*ppucKeyId);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    }
    
    if (*piCertLen > 0)
    {
        *ppucCert = (WE_UCHAR *)WE_MALLOC((WE_ULONG)(*piCertLen) * sizeof(WE_UCHAR));
        if (NULL == *ppucCert)
        {
            WE_FREE(*ppucKeyId);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
        if (SecEcdr_DecodeOctets( pstDecoder, *ppucCert, *piCertLen ) < 0)
        {
            WE_FREE(*ppucCert);
            WE_FREE(*ppucKeyId);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    }
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_GetGetCipherSuiteResp
CREATE DATE:
    2007-03-19
AUTHOR:
    wangshuhua
DESCRIPTION:
    Generate get prf response from bytes stream
ARGUMENTS PASSED:
    pvMsg: [in] bytes stream
    ppvResp:[out] response structure
RETURN VALUE:
    0: success.
    -1: fail.
==================================================================================================*/
static WE_INT32 Sec_GetGetCipherSuiteResp(WE_VOID *pvMsg,WE_VOID **ppvResp)
{
    St_SecGetCipherSuiteResp *pstResp = NULL;
    WE_INT32 iRet = 0;

    pstResp = (St_SecGetCipherSuiteResp*)WE_MALLOC(sizeof(St_SecGetCipherSuiteResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeWtlsGetCipherSuiteResp(pvMsg,
                                        &(pstResp->iTargetID),
                                        &(pstResp->usResult),
                                        &(pstResp->pucCipherMethods),
                                        &(pstResp->usCipherMethodsLen),
                                        &(pstResp->pucKeyExchangeIds),
                                        &(pstResp->usKeyExchangeIdsLen),
                                        &(pstResp->pucTrustedKeyIds),
                                        &(pstResp->usTrustedKeyIdsLen));
    if (iRet < 0)
    {
        WE_FREE(pstResp);
        return -1;
    }
    *ppvResp = (WE_VOID*)pstResp;
    return 0;
       
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeWtlsGetCipherSuiteResp                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode encoded data and get wtls CipherSuite response.
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piResult[OUT]:the result.
    WE_UCHAR **ppucCipherMethods[OUT]:ciphermethod.
    WE_INT32 *piCipherMethodsLen[OUT]:length of cipher suit method.
    WE_UCHAR **ppucKeyExchangeIds[OUT]:pointer to the key exchange id.
    WE_INT32 *piKeyExchangeIdsLen[OUT]:length of key exchange id.
    WE_UCHAR **ppucTrustedKeyIds[OUT]:pointer to the trusted key id.
    WE_INT32 *piTrustedKeyIdsLen[OUT]:Pointer to the length of trusted key id.
RETURN VALUE:
    0: success.
    -1: fail.
IMPORTANT NOTES:
    None.
==================================================================================================*/
WE_INT32 Sec_DecodeWtlsGetCipherSuiteResp(WE_UINT8  *pcBuffer,  
                                WE_INT32 *piTargetID, WE_UINT16 *pusResult,
                                 WE_UCHAR **ppucCipherMethods, WE_UINT16 *pusCipherMethodsLen,
                                 WE_UCHAR **ppucKeyExchangeIds, WE_UINT16 *pusKeyExchangeIdsLen,
                                 WE_UCHAR **ppucTrustedKeyIds, WE_UINT16 *pusTrustedKeyIdsLen)
{
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeUint16( pstDecoder, pusResult )
        || -1 == SecEcdr_DecodeUint16( pstDecoder, pusCipherMethodsLen )
        || -1 == SecEcdr_DecodeUint16( pstDecoder, pusKeyExchangeIdsLen )
        || -1 == SecEcdr_DecodeUint16( pstDecoder, pusTrustedKeyIdsLen ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (*pusCipherMethodsLen > 0)
    {
        *ppucCipherMethods = (WE_UCHAR *)WE_MALLOC((WE_ULONG)(*pusCipherMethodsLen) * sizeof(WE_UCHAR));
        if (NULL == *ppucCipherMethods)
        {
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
        if (SecEcdr_DecodeOctets( pstDecoder, *ppucCipherMethods, *pusCipherMethodsLen ) < 0)
        {
            WE_FREE(*ppucCipherMethods);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    }
    
    if (*pusKeyExchangeIdsLen > 0)
    {
        *ppucKeyExchangeIds = (WE_UCHAR *)WE_MALLOC((WE_ULONG)(*pusKeyExchangeIdsLen) * sizeof(WE_UCHAR));
        if (NULL == *ppucKeyExchangeIds)
        {
            WE_FREE(*ppucCipherMethods);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
        if (SecEcdr_DecodeOctets( pstDecoder, *ppucKeyExchangeIds, *pusKeyExchangeIdsLen ) < 0)
        {
            WE_FREE(*ppucKeyExchangeIds);
            WE_FREE(*ppucCipherMethods);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    }    
    
    if (*pusTrustedKeyIdsLen > 0)
    {
        *ppucTrustedKeyIds = (WE_UCHAR *)WE_MALLOC((WE_ULONG)(*pusTrustedKeyIdsLen) * sizeof(WE_UCHAR));
        if (NULL == *ppucTrustedKeyIds)
        {
            WE_FREE(*ppucKeyExchangeIds);
            WE_FREE(*ppucCipherMethods);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
        if (SecEcdr_DecodeOctets( pstDecoder, *ppucTrustedKeyIds, *pusTrustedKeyIdsLen ) < 0)
        {
            WE_FREE(*ppucTrustedKeyIds);
            WE_FREE(*ppucKeyExchangeIds);
            WE_FREE(*ppucCipherMethods);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    }
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_GetGetCipherSuiteResp
CREATE DATE:
    2007-03-19
AUTHOR:
    wangshuhua
DESCRIPTION:
    Generate get prf response from bytes stream
ARGUMENTS PASSED:
    pvMsg: [in] bytes stream
    ppvResp:[out] response structure
RETURN VALUE:
    0: success.
    -1: fail.
==================================================================================================*/
static WE_INT32 Sec_GetKeyExchangeResp(WE_VOID *pvMsg,WE_VOID **ppvResp)
{
    St_SecKeyExchResp *pstResp = NULL;
    WE_INT32 iRet = 0;

    pstResp = (St_SecKeyExchResp*)WE_MALLOC(sizeof(St_SecKeyExchResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeKeyExchangeResp(pvMsg,
                                        &(pstResp->iTargetID),
                                        (WE_INT32 *)&(pstResp->usResult),
                                        (WE_INT32 *)&(pstResp->ucMasterSecretId),
                                        &(pstResp->pucPreMsKey),
                                        (WE_INT32 *)&(pstResp->usPreMsKeyLen)
                                          );
    if (iRet < 0)
    {
        WE_FREE(pstResp);
        return -1;
    }
    *ppvResp = (WE_VOID*)pstResp;
    return 0;
       
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodeKeyExchangeResp                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode encoded data and get key exchange response.
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piResult[OUT]:the result.
    WE_INT32 *piMasterSecretId[OUT]:master secret id.
    WE_UCHAR **ppucPublicValue[OUT]:Pointer to the public value.
    WE_INT32 *piPublicValueLen[OUT]:length of public value.
RETURN VALUE:
    0: success.
    -1: fail.
IMPORTANT NOTES:
    None.
==================================================================================================*/
WE_INT32 Sec_DecodeKeyExchangeResp(WE_UINT8  *pcBuffer,  
                             WE_INT32 *piTargetID, WE_INT32 *piResult, WE_INT32 *piMasterSecretId, 
                             WE_UCHAR **ppucPublicValue, WE_INT32 *piPublicValueLen)
{
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piResult )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piMasterSecretId )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piPublicValueLen ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (*piPublicValueLen > 0)
    {
        *ppucPublicValue = (WE_UCHAR *)WE_MALLOC((WE_ULONG)(*piPublicValueLen) * sizeof(WE_UCHAR));
        if (NULL == *ppucPublicValue)
        {
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
        if (SecEcdr_DecodeOctets( pstDecoder, *ppucPublicValue, *piPublicValueLen ) < 0)
        {
            WE_FREE(*ppucPublicValue);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    }    
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_GetCompSignResp
CREATE DATE:
    2007-03-19
AUTHOR:
    wangshuhua
DESCRIPTION:
    Generate get comp sign response from bytes stream
ARGUMENTS PASSED:
    pvMsg: [in] bytes stream
    ppvResp:[out] response structure
RETURN VALUE:
    0: success.
    -1: fail.
==================================================================================================*/
static WE_INT32 Sec_GetCompSignResp(WE_VOID *pvMsg,WE_VOID **ppvResp)
{
    St_SecCompSignResp *pstResp = NULL;
    WE_INT32 iRet = 0;
    WE_INT32 iRes = 0;
    WE_INT32 iSignatureLen = 0;

    pstResp = (St_SecCompSignResp*)WE_MALLOC(sizeof(St_SecCompSignResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeCompSigResp(pvMsg,
                                        &(pstResp->iTargetID), &(iRes),
                                        &(pstResp->pucSignature),
                                        &iSignatureLen);
    if (iRet < 0)
    {
        WE_FREE(pstResp);
        return -1;
    }
    pstResp->usResult = (WE_UINT16)iRes;
    pstResp->usSignatureLen = (WE_UINT16)iSignatureLen;
    *ppvResp = (WE_VOID*)pstResp;
    return 0;
       
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodeCompSigResp                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode encoded data and get computer signature response.
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piResult[OUT]:the result.
    WE_UCHAR **ppucSignature[OUT]:Pointer to the signature.
    WE_INT32 *piSignatureLen[OUT]:length of signature.
RETURN VALUE:
    0: success.
    -1: fail.
IMPORTANT NOTES:
    None.
==================================================================================================*/
WE_INT32 Sec_DecodeCompSigResp(WE_UINT8  *pcBuffer,
                         WE_INT32 *piTargetID, WE_INT32 *piResult,
                          WE_UCHAR **ppucSignature, WE_INT32 *piSignatureLen)
{
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piResult )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piSignatureLen ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (*piSignatureLen > 0)
    {
        *ppucSignature = (WE_UCHAR *)WE_MALLOC((WE_ULONG)(*piSignatureLen) * sizeof(WE_UCHAR));
        if (NULL == *ppucSignature)
        {
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
        if (SecEcdr_DecodeOctets( pstDecoder, *ppucSignature, *piSignatureLen ) < 0)
        {
            WE_FREE(*ppucSignature);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    }    
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_GetSearchPeerResp
CREATE DATE:
    2007-03-19
AUTHOR:
    wangshuhua
DESCRIPTION:
    Generate get search peer response from bytes stream
ARGUMENTS PASSED:
    pvMsg: [in] bytes stream
    ppvResp:[out] response structure
RETURN VALUE:
    0: success.
    -1: fail.
==================================================================================================*/
static WE_INT32 Sec_GetSearchPeerResp(WE_VOID *pvMsg,WE_VOID **ppvResp)
{
    St_SecSearchPeerResp *pstResp = NULL;
    WE_INT32 iRet = 0;
    WE_INT32 iRes = 0;
    WE_INT32 iMasterSecretId = 0;

    pstResp = (St_SecSearchPeerResp*)WE_MALLOC(sizeof(St_SecSearchPeerResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeSearchPeerResp(pvMsg,
                                        &(pstResp->iTargetID), &iRes,
                                        &(pstResp->ucConnectionType),
                                        &iMasterSecretId,
                                        &(pstResp->iSecId)
                                          );
    if (iRet < 0)
    {
        WE_FREE(pstResp);
        return -1;
    }
    pstResp->usResult = (WE_UINT16)iRes;
    pstResp->ucMasterSecretId = (WE_UINT8)iMasterSecretId;
    *ppvResp = (WE_VOID*)pstResp;
    return 0;
       
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeSearchPeerResp                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode encoded data and get search peer response.
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piResult[OUT]:the result.
    WE_UINT8 *pucConnectionType[OUT]:type of connection.
    WE_INT32 *piMasterSecretID[OUT]:master secret id.
    WE_INT32 *piSecurityID[OUT]:security id.
RETURN VALUE:
    0: success.
    -1: fail.
IMPORTANT NOTES:
    None.
==================================================================================================*/
WE_INT32 Sec_DecodeSearchPeerResp(WE_UINT8  *pcBuffer,
                         WE_INT32 *piTargetID, WE_INT32 *piResult,
                         WE_UINT8 *pucConnectionType, WE_INT32 *piMasterSecretID,
                         WE_INT32 *piSecurityID)
{
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piResult )
        || -1 == SecEcdr_DecodeUint8( pstDecoder, pucConnectionType )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piMasterSecretID )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piSecurityID ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_GetSessionGetResp
CREATE DATE:
    2007-03-19
AUTHOR:
    wangshuhua
DESCRIPTION:
    Generate get session response from bytes stream
ARGUMENTS PASSED:
    pvMsg: [in] bytes stream
    ppvResp:[out] response structure
RETURN VALUE:
    0: success.
    -1: fail.
==================================================================================================*/
static WE_INT32 Sec_GetSessionGetResp(WE_VOID *pvMsg,WE_VOID **ppvResp)
{
    St_SecSessionGetResp *pstResp = NULL;
    WE_INT32 iRet = 0;

    pstResp = (St_SecSessionGetResp*)WE_MALLOC(sizeof(St_SecSessionGetResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeSessionGetResp(pvMsg,
                                    &(pstResp->iTargetID),
                                    &(pstResp->usResult),
                                    &(pstResp->ucSessionOptions),
                                    &(pstResp->pucSessionId),
                                    &(pstResp->usSessionIdLen),
                                    pstResp->aucCipherSuite,
                                    &(pstResp->ucCompressionAlg),
                                    &(pstResp->pucPrivateKeyId),
                                    &(pstResp->uiCreationTime));
    if (iRet < 0)
    {
        WE_FREE(pstResp);
        return -1;
    }
    *ppvResp = (WE_VOID*)pstResp;
    return 0;
       
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodeSessionGetResp                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode encoded data and get search peer response.
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piResult[OUT]:the result.
    WE_UINT8 *pucSessionOptions[OUT]:session option.
    WE_UCHAR **ppucSessionId[OUT]:session id.
    WE_UINT8 *pucSessionIdLen[OUT]: length of session id.
    WE_UINT8 **ppucCipherSuite[OUT}:Pointer to the cpher suite.
    WE_UINT8 *pucCompressionAlg[OUT]:Compression alg.
    WE_UCHAR **ppucPrivateKeyId[OUT]: Private key id.
    WE_UINT32 *puiCreationTime[OUT]:Pointer to the creation time.
RETURN VALUE:
    0: success.
    -1: fail.
IMPORTANT NOTES:
    None.
==================================================================================================*/
WE_INT32 Sec_DecodeSessionGetResp(WE_UINT8  *pcBuffer,
                         WE_INT32 *piTargetID, WE_UINT16 *pusResult, 
                         WE_UINT8 *pucSessionOptions, WE_UCHAR **ppucSessionId,
                         WE_UINT16 *pusSessionIdLen, WE_UINT8 *pucCipherSuite,
                         WE_UINT8 *pucCompressionAlg, WE_UCHAR **ppucPrivateKeyId,
                         WE_UINT32 *puiCreationTime)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32          iPrivateKeyIdLen = 0;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeUint16( pstDecoder, pusResult )
        || -1 == SecEcdr_DecodeUint8( pstDecoder, pucSessionOptions )
        || -1 == SecEcdr_DecodeUint16( pstDecoder, pusSessionIdLen )
        || -1 == SecEcdr_DecodeUint8( pstDecoder, pucCompressionAlg )
        || -1 == SecEcdr_DecodeUint32( pstDecoder, puiCreationTime )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, &iPrivateKeyIdLen ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
        
    if (*pusSessionIdLen > 0)
    {
        *ppucSessionId = (WE_UCHAR *)WE_MALLOC((WE_ULONG)(*pusSessionIdLen) * sizeof(WE_UCHAR));
        if (NULL == *ppucSessionId)
        {
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
        if (SecEcdr_DecodeOctets( pstDecoder, *ppucSessionId, *pusSessionIdLen ) < 0)
        {
            WE_FREE(*ppucSessionId);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    }    
    if (SecEcdr_DecodeOctets( pstDecoder, pucCipherSuite, 2 ) < 0)
    {
        WE_FREE(*ppucSessionId);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }

    if (iPrivateKeyIdLen > 0)
    {
        *ppucPrivateKeyId = (WE_UCHAR *)WE_MALLOC((WE_ULONG)(iPrivateKeyIdLen) * sizeof(WE_UCHAR));
        if (NULL == *ppucPrivateKeyId)
        {
            WE_FREE(*ppucSessionId);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
        if (SecEcdr_DecodeOctets( pstDecoder, *ppucPrivateKeyId, iPrivateKeyIdLen ) < 0)
        {
            WE_FREE(*ppucSessionId);
            WE_FREE(*ppucPrivateKeyId);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    } 
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_GetSessionGetResp
CREATE DATE:
    2007-03-19
AUTHOR:
    wangshuhua
DESCRIPTION:
    Generate get session response from bytes stream
ARGUMENTS PASSED:
    pvMsg: [in] bytes stream
    ppvResp:[out] response structure
RETURN VALUE:
    0: success.
    -1: fail.
==================================================================================================*/
static WE_INT32 Sec_GetGetCertNameListResp(WE_VOID *pvMsg,WE_VOID **ppvResp)
{
    St_SecGetCertNameListResp *pstResp = NULL;
    WE_INT32 iRet = 0;
    WE_INT32 iRes = 0;

    pstResp = (St_SecGetCertNameListResp*)WE_MALLOC(sizeof(St_SecGetCertNameListResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeGetCertNameListResp(pvMsg,
                                    &(pstResp->iTargetID),
                                    &(iRes));
    if (iRet < 0)
    {
        WE_FREE(pstResp);
        return -1;
    }
    pstResp->usResult = (WE_UINT16)iRes;
    *ppvResp = (WE_VOID*)pstResp;
    return 0;
       
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeGetCertNameListResp                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode encoded data and get cert name list response.
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piResult[OUT]:the result.
RETURN VALUE:
    0: success.
    -1: fail.
IMPORTANT NOTES:
    None.
==================================================================================================*/
WE_INT32 Sec_DecodeGetCertNameListResp(WE_UINT8  *pcBuffer,
                              WE_INT32 *piTargetID, WE_INT32 *piResult)
{
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piResult ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_GetDelCertResp
CREATE DATE:
    2007-03-19
AUTHOR:
    wangshuhua
DESCRIPTION:
    Generate delete cert response from bytes stream
ARGUMENTS PASSED:
    pvMsg: [in] bytes stream
    ppvResp:[out] response structure
RETURN VALUE:
    0: success.
    -1: fail.
==================================================================================================*/
static WE_INT32 Sec_GetDelCertResp(WE_VOID *pvMsg,WE_VOID **ppvResp)
{
    St_SecRemCertResp *pstResp = NULL;
    WE_INT32 iRet = 0;
    WE_INT32 iRes = 0;
    WE_INT32 iCertId = 0;

    pstResp = (St_SecRemCertResp*)WE_MALLOC(sizeof(St_SecRemCertResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeDeleteCertResponse(pvMsg,
                                    &(pstResp->iTargetID),
                                    &(iCertId), &(iRes));
    if (iRet < 0)
    {
        WE_FREE(pstResp);
        return -1;
    }
    pstResp->ucCertId = (WE_UINT8)iCertId;
    pstResp->usResult = (WE_UINT16)iRes;
    *ppvResp = (WE_VOID*)pstResp;
    return 0;
       
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeDeleteCertResponse                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode encoded data and get delete certificate response.
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piCertId[OUT]:certificate id.
    WE_INT32 *piResult[OUT]:the result.
RETURN VALUE:
    0: success.
    -1: fail.
IMPORTANT NOTES:
    None.
==================================================================================================*/
WE_INT32 Sec_DecodeDeleteCertResponse(WE_UINT8  *pcBuffer,
                              WE_INT32 *piTargetID, WE_INT32 *piCertId, WE_INT32 *piResult)
{
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piCertId )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piResult ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_GetKeyPairGenResp
CREATE DATE:
    2007-03-19
AUTHOR:
    wangshuhua
DESCRIPTION:
    Generate response from bytes stream
ARGUMENTS PASSED:
    pvMsg: [in] bytes stream
    ppvResp:[out] response structure
RETURN VALUE:
    0: success.
    -1: fail.
==================================================================================================*/
static WE_INT32 Sec_GetKeyPairGenResp(WE_VOID *pvMsg,WE_VOID **ppvResp)
{
    St_SecGenKeyPairResp *pstResp = NULL;
    WE_INT32 iRet = 0;
    WE_INT32 iRes = 0;
    WE_INT32 iPubkeyLen = 0;
    WE_INT32 iPKHashSigLen = 0;

    pstResp = (St_SecGenKeyPairResp*)WE_MALLOC(sizeof(St_SecGenKeyPairResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeKeyPairGenResp(pvMsg,
                                    &(pstResp->iTargetID),
                                    &iRes,
                                    &(pstResp->ucKeyType),
                                    &(pstResp->pucPublicKey),
                                    &iPubkeyLen,
                                    &(pstResp->pucPKHashSig),
                                    &iPKHashSigLen);
    if (iRet < 0)
    {
        WE_FREE(pstResp);
        return -1;
    }
    pstResp->usResult =  (WE_UINT16)iRes;
    pstResp->usPublicKeyLen =  (WE_UINT16)iPubkeyLen;
    pstResp->usPKHashSigLen =  (WE_UINT16)iPKHashSigLen;
    *ppvResp = (WE_VOID*)pstResp;
    return 0;
       
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeKeyPairGenResp                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode encoded data and get generate key pair response.
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piResult[OUT]:the result.
    WE_UINT8 *pucKeyType[OUT]:key type.
    WE_UCHAR **ppucPublicKey[OUT]:Pointer to the public key.
    WE_INT32 *piPublicKeyLen[OUT]:length of public key.
    WE_UCHAR **ppucSig[OUT]:Pointer to the signature.
    WE_INT32 *piSigLen[OUT]:length of signature.
RETURN VALUE:
    0: success.
    -1: fail.
IMPORTANT NOTES:
    None.
==================================================================================================*/

WE_INT32 Sec_DecodeKeyPairGenResp(WE_UINT8  *pcBuffer,
                        WE_INT32 *piTargetID, WE_INT32 *piResult,  WE_UINT8 *pucKeyType, 
                         WE_UCHAR **ppucPublicKey, WE_INT32 *piPublicKeyLen, 
                         WE_UCHAR **ppucSig, WE_INT32 *piSigLen)
{
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piResult )
        || -1 == SecEcdr_DecodeUint8( pstDecoder, pucKeyType )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piPublicKeyLen )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piSigLen ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (*piPublicKeyLen > 0)
    {
        *ppucPublicKey = (WE_UCHAR *)WE_MALLOC((WE_ULONG)(*piPublicKeyLen) * sizeof(WE_UCHAR));
        if (NULL == *ppucPublicKey)
        {
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
        if (SecEcdr_DecodeOctets( pstDecoder, *ppucPublicKey, *piPublicKeyLen ) < 0)
        {
            WE_FREE(*ppucPublicKey);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    }
    
    if (*piSigLen > 0)
    {
        *ppucSig = (WE_UCHAR *)WE_MALLOC((WE_ULONG)(*piSigLen) * sizeof(WE_UCHAR));
        if (NULL == *ppucSig)
        {
            WE_FREE(*ppucPublicKey);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
        if (SecEcdr_DecodeOctets( pstDecoder, *ppucSig, *piSigLen ) < 0)
        {
            WE_FREE(*ppucSig);
            WE_FREE(*ppucPublicKey);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    }
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_GetGetPubKeyResp
CREATE DATE:
    2007-03-19
AUTHOR:
    wangshuhua
DESCRIPTION:
    Generate response from bytes stream
ARGUMENTS PASSED:
    pvMsg: [in] bytes stream
    ppvResp:[out] response structure
RETURN VALUE:
    0: success.
    -1: fail.
==================================================================================================*/
static WE_INT32 Sec_GetGetPubKeyResp(WE_VOID *pvMsg,WE_VOID **ppvResp)
{
    St_GetPubkeyResp *pstResp = NULL;
    WE_INT32 iRet = 0;
    WE_INT32 iRes = 0;
    WE_INT32 iPubKeyLen = 0;
    WE_INT32 iPubKeySigLen = 0;
    
    pstResp = (St_GetPubkeyResp*)WE_MALLOC(sizeof(St_GetPubkeyResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeGetPubKeyResp(pvMsg,
                                   &(pstResp->iTargetID), &iRes,
                                   &(pstResp->pucPubKey),
                                   &iPubKeyLen,
                                   &(pstResp->pucPubKeySig),
                                   &iPubKeySigLen);
    if (iRet < 0)
    {
        WE_FREE(pstResp);
        return -1;
    }
    pstResp->usResult =  (WE_UINT16)iRes;
    pstResp->usPubKeyLen = (WE_UINT16)iPubKeyLen;
    pstResp->usPubKeySigLen = (WE_UINT16)iPubKeySigLen;
    *ppvResp = (WE_VOID*)pstResp;
    return 0;
       
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeGetPubKeyResp                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode encoded data and get public key response.
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piResult[OUT]:the result.
    WE_UINT8 *pucKeyType[OUT]:key type.
    WE_UCHAR **ppucPublicKey[OUT]:Pointer to the public key.
    WE_INT32 *piPublicKeyLen[OUT]:length of public key.
    WE_UCHAR **ppucSig[OUT]:Pointer to the signature.
    WE_INT32 *piSigLen[OUT]:length of signature.
RETURN VALUE:
    0: success.
    -1: fail.
IMPORTANT NOTES:
    None.
==================================================================================================*/

WE_INT32 Sec_DecodeGetPubKeyResp(WE_UINT8  *pcBuffer,
                        WE_INT32 *piTargetID, WE_INT32 *piResult,
                        WE_UCHAR **ppucPublicKey, WE_INT32 *piPublicKeyLen, 
                        WE_UCHAR **ppucSig, WE_INT32 *piSigLen)
{
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piResult )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piPublicKeyLen )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piSigLen ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (*piPublicKeyLen > 0)
    {
        *ppucPublicKey = (WE_UCHAR *)WE_MALLOC((WE_ULONG)(*piPublicKeyLen) * sizeof(WE_UCHAR));
        if (NULL == *ppucPublicKey)
        {
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
        if (SecEcdr_DecodeOctets( pstDecoder, *ppucPublicKey, *piPublicKeyLen ) < 0)
        {
            WE_FREE(*ppucPublicKey);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    }
    
    if (*piSigLen > 0)
    {
        *ppucSig = (WE_UCHAR *)WE_MALLOC((WE_ULONG)(*piSigLen) * sizeof(WE_UCHAR));
        if (NULL == *ppucSig)
        {
            WE_FREE(*ppucPublicKey);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
        if (SecEcdr_DecodeOctets( pstDecoder, *ppucSig, *piSigLen ) < 0)
        {
            WE_FREE(*ppucSig);
            WE_FREE(*ppucPublicKey);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    }
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}

#ifdef G_SEC_CFG_SHOW_PIN
/*==================================================================================================
FUNCTION: 
    Sec_GetGetPubKeyResp
CREATE DATE:
    2007-03-19
AUTHOR:
    wangshuhua
DESCRIPTION:
    Generate response from bytes stream
ARGUMENTS PASSED:
    pvMsg: [in] bytes stream
    ppvResp:[out] response structure
RETURN VALUE:
    0: success.
    -1: fail.
==================================================================================================*/
static WE_INT32 Sec_GetModifyPinResp(WE_VOID *pvMsg,WE_VOID **ppvResp)
{
    St_SecModifyPinResp *pstResp = NULL;
    WE_INT32 iRet = 0;
    WE_INT32 iRes = 0;

    pstResp = (St_SecModifyPinResp*)WE_MALLOC(sizeof(St_SecModifyPinResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeModifyPinResp(pvMsg,
                                   &(pstResp->iTargetID), &iRes);
    if (iRet < 0)
    {
        WE_FREE(pstResp);
        return -1;
    }
    pstResp->usResult =  (WE_UINT16)iRes;
    *ppvResp = (WE_VOID*)pstResp;
    return 0;
       
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodeModifyPinResp                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode encoded data and get modify pin response.
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piResult[OUT]:the result.
RETURN VALUE:
    0: success.
    -1: fail.
IMPORTANT NOTES:
    None.
==================================================================================================*/

WE_INT32 Sec_DecodeModifyPinResp(WE_UINT8  *pcBuffer,
                              WE_INT32 *piTargetID, WE_INT32 *piResult)
{
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piResult ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}
#endif

/*==================================================================================================
FUNCTION: 
    Sec_GetViewAndCertResp
CREATE DATE:
    2007-03-19
AUTHOR:
    wangshuhua
DESCRIPTION:
    Generate response from bytes stream
ARGUMENTS PASSED:
    pvMsg: [in] bytes stream
    ppvResp:[out] response structure
RETURN VALUE:
    0: success.
    -1: fail.
==================================================================================================*/
static WE_INT32 Sec_GetViewAndCertResp(WE_VOID *pvMsg,WE_VOID **ppvResp)
{
    St_SecGetCertContentResp *pstResp = NULL;
    WE_INT32 iRet = 0;
    WE_INT32 iRes = 0;
    WE_INT32 iCertId = 0;
    WE_INT32 iCertLen = 0;

    pstResp = (St_SecGetCertContentResp*)WE_MALLOC(sizeof(St_SecGetCertContentResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeViewAndGetCertResp(pvMsg,
                                   &(pstResp->iTargetID), &iRes,
                                   &iCertId,  &(pstResp->pucCert),
                                   &iCertLen);
    if (iRet < 0)
    {
        WE_FREE(pstResp);
        return -1;
    }
    pstResp->usResult =  (WE_UINT16)iRes;
    pstResp->ucCertId = (WE_UINT8)iCertId;
    pstResp->usCertLen = (WE_UINT16)iCertLen;
    *ppvResp = (WE_VOID*)pstResp;
    return 0;
       
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeViewAndGetCertResp                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode encoded data and get view and get certificate response.
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piResult[OUT]:the result.
    WE_INT32 *piCertId[OUT]:certificate id.
    WE_UCHAR **ppucCert[OUT]:Pointer to the certificate.
    WE_INT32 *piCertLen[OUT]:length of certificate.
RETURN VALUE:
    0: success.
    -1: fail.
IMPORTANT NOTES:
    None.
==================================================================================================*/
WE_INT32 Sec_DecodeViewAndGetCertResp(WE_UINT8  *pcBuffer,
                            WE_INT32 *piTargetID, WE_INT32 *piResult, 
                            WE_INT32 *piCertId, WE_UCHAR **ppucCert, WE_INT32 *piCertLen)
{
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piResult )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piCertId )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piCertLen ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (*piCertLen > 0)
    {
        *ppucCert = (WE_UCHAR *)WE_MALLOC((WE_ULONG)(*piCertLen) * sizeof(WE_UCHAR));
        if (NULL == *ppucCert)
        {
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
        if (SecEcdr_DecodeOctets( pstDecoder, *ppucCert, *piCertLen ) < 0)
        {
            WE_FREE(*ppucCert);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    }
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_GetGetCurSvrCertResp
CREATE DATE:
    2007-03-19
AUTHOR:
    wangshuhua
DESCRIPTION:
    Generate response from bytes stream
ARGUMENTS PASSED:
    pvMsg: [in] bytes stream
    ppvResp:[out] response structure
RETURN VALUE:
    0: success.
    -1: fail.
==================================================================================================*/
static WE_INT32 Sec_GetGetCurSvrCertResp(WE_VOID *pvMsg,WE_VOID **ppvResp)
{
    St_SecGetCurSvrCertResp *pstResp = NULL;
    WE_INT32 iRet = 0;
    WE_INT32 iRes = 0;

    pstResp = (St_SecGetCurSvrCertResp*)WE_MALLOC(sizeof(St_SecGetCurSvrCertResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeGetCurSvrCertResp(pvMsg,
                                   &(pstResp->iTargetID), &iRes);
    if (iRet < 0)
    {
        WE_FREE(pstResp);
        return -1;
    }
    pstResp->usResult =  (WE_UINT16)iRes;
    *ppvResp = (WE_VOID*)pstResp;
    return 0;
       
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeGetCurSvrCertResp                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode encoded data and get current server certificate response.
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piResult[OUT]:the result.
RETURN VALUE:
    0: success.
    -1: fail.
IMPORTANT NOTES:
    None.
==================================================================================================*/
WE_INT32 Sec_DecodeGetCurSvrCertResp(WE_UINT8  *pcBuffer,
                              WE_INT32 *piTargetID, WE_INT32 *piResult)
{
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piResult ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_GetGetSessionInfoResp
CREATE DATE:
    2007-03-19
AUTHOR:
    wangshuhua
DESCRIPTION:
    Generate response from bytes stream
ARGUMENTS PASSED:
    pvMsg: [in] bytes stream
    ppvResp:[out] response structure
RETURN VALUE:
    0: success.
    -1: fail.
==================================================================================================*/
static WE_INT32 Sec_GetGetSessionInfoResp(WE_VOID *pvMsg,WE_VOID **ppvResp)
{
    St_SecGetSessionInfoResp *pstResp = NULL;
    WE_INT32 iRet = 0;
    WE_INT32 iRes = 0;

    pstResp = (St_SecGetSessionInfoResp*)WE_MALLOC(sizeof(St_SecGetSessionInfoResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeGetSessionInfoResp(pvMsg,
                                   &(pstResp->iTargetID), &iRes);
    if (iRet < 0)
    {
        WE_FREE(pstResp);
        return -1;
    }
    pstResp->usResult =  (WE_UINT16)iRes;
    *ppvResp = (WE_VOID*)pstResp;
    return 0;
       
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeGetSessionInfoResp                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode encoded data and get the response of get session information.
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piResult[OUT]:the result.
RETURN VALUE:
    0: success.
    -1: fail.
IMPORTANT NOTES:
    None.
==================================================================================================*/
WE_INT32 Sec_DecodeGetSessionInfoResp(WE_UINT8  *pcBuffer,
                              WE_INT32 *piTargetID, WE_INT32 *piResult)
{
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piResult ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_GetGetSessionInfoResp
CREATE DATE:
    2007-03-19
AUTHOR:
    wangshuhua
DESCRIPTION:
    Generate response from bytes stream
ARGUMENTS PASSED:
    pvMsg: [in] bytes stream
    ppvResp:[out] response structure
RETURN VALUE:
    0: success.
    -1: fail.
==================================================================================================*/
static WE_INT32 Sec_GetWtlsCurClassResp(WE_VOID *pvMsg,WE_VOID **ppvResp)
{
    St_SecGetWtlsCurClassResp *pstResp = NULL;
    WE_INT32 iRet = 0;
    WE_INT32 iSecClass = 0;
    WE_INT32 iSessionInfoReady = 0;
    WE_INT32 iCertReady = 0;

    pstResp = (St_SecGetWtlsCurClassResp*)WE_MALLOC(sizeof(St_SecGetWtlsCurClassResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeWtlsCurClassResp(pvMsg,
                                   &(pstResp->iTargetID), &iSecClass,
                                   &iSessionInfoReady, &iCertReady);
    if (iRet < 0)
    {
        WE_FREE(pstResp);
        return -1;
    }
    pstResp->ucSecClass=  (WE_UINT8)iSecClass;
    pstResp->ucSessionInfoReady=  (WE_UINT8)iSessionInfoReady;
    pstResp->ucCertReady=  (WE_UINT8)iCertReady;
    *ppvResp = (WE_VOID*)pstResp;
    return 0;
       
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeWtlsCurClassResp                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode encoded data and get the response of wtls current class.
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piResult[OUT]:the result.
    WE_INT32 *piInfoAvailable[OUT]:the information of available.
    WE_INT32 *piCertAvailable[OUT]:the avialable of certificate.
RETURN VALUE:
    0: success.
    -1: fail.
IMPORTANT NOTES:
    None.
==================================================================================================*/
WE_INT32 Sec_DecodeWtlsCurClassResp(WE_UINT8  *pcBuffer,
                              WE_INT32 *piTargetID, WE_INT32 *piResult,
                              WE_INT32 *piInfoAvailable, WE_INT32 *piCertAvailable)
{
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piResult )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piInfoAvailable )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piCertAvailable ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}
static WE_INT32 Sec_GetClrSessionResp(WE_VOID *pvMsg, WE_VOID** ppvData)
{
    St_SecSessionClearResp *pstResp = NULL;
    WE_INT32 iRet = -1;
    WE_INT32 iRes = -1;
    
    if ((NULL == pvMsg) || (NULL == ppvData))
    {
        return -1;
    }

    pstResp = (St_SecSessionClearResp*)WE_MALLOC(sizeof(St_SecSessionClearResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeClrSessionResp(pvMsg,
                                    &(pstResp->iTargetID), &iRes);

    pstResp->usResult =  (WE_UINT16)iRes;
    *(ppvData) = (WE_VOID*)pstResp;
    return iRet;
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeClrSessionResp                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode encoded data and get the response of clear session.
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piResult[OUT]:the result.
RETURN VALUE:
    0: success.
    -1: fail.
IMPORTANT NOTES:
    None.
==================================================================================================*/
WE_INT32 Sec_DecodeClrSessionResp(WE_UINT8  *pcBuffer,
                              WE_INT32 *piTargetID, WE_INT32 *piResult)
{
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piResult ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}
static WE_INT32 Sec_GetGetContractsListResp(WE_VOID *pvMsg, WE_VOID** ppvData)
{
    St_SecGetContractsListResp *pstResp = NULL;
    WE_INT32 iRet = -1;
    WE_INT32 iResult = -1;

    if ((NULL == pvMsg) || (NULL == ppvData))
    {
        return -1;
    }

    pstResp = (St_SecGetContractsListResp *)WE_MALLOC(sizeof(St_SecGetContractsListResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeGetContractsListResp(pvMsg,
                                         &(pstResp->iTargetID), &(iResult));
    if (iRet != 0)
    {
        WE_FREE(pstResp);
        return -1;
    }
    pstResp->usResult = (WE_UINT16)iResult;
    
    *(ppvData) = (WE_VOID*)pstResp;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodeGetContractsListResp                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode encoded data and get the response of get contracts list.
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piResult[OUT]:the result.
RETURN VALUE:
    0: success.
    -1: fail.
IMPORTANT NOTES:
    None.
==================================================================================================*/
WE_INT32 Sec_DecodeGetContractsListResp(WE_UINT8  *pcBuffer,
                              WE_INT32 *piTargetID, WE_INT32 *piResult)
{
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piResult ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}

static WE_INT32 Sec_GetRemoveContractResp(WE_VOID *pvMsg, WE_VOID** ppvData)
{
    St_SecDelContractResp *pstResp = NULL;
    WE_INT32 iRet = -1;
    WE_INT32 iRes = -1;

    if ((NULL == pvMsg) || (NULL == ppvData))
    {
        return -1;
    }

    pstResp = (St_SecDelContractResp*)WE_MALLOC(sizeof(St_SecDelContractResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeRemoveContractResp(pvMsg,
                                     &(pstResp->iTargetID), &iRes);

    pstResp->usResult =  (WE_UINT16)iRes;
    *(ppvData) = (WE_VOID*)pstResp;
    return iRet;
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeRemoveContractResp                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode encoded data and get the response of remove contracts .
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piResult[OUT]:the result.
RETURN VALUE:
    0: success.
    -1: fail.
IMPORTANT NOTES:
    None.
==================================================================================================*/

WE_INT32 Sec_DecodeRemoveContractResp(WE_UINT8  *pcBuffer,
                              WE_INT32 *piTargetID, WE_INT32 *piResult)
{
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piResult ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}
static WE_INT32 Sec_GetUsrCertReqResp (WE_VOID *pvMsg, WE_VOID** ppvData)
{
    St_SecUserCertRequestResp *pstResp = NULL;
    WE_INT32 iRet = -1;

    if ((NULL == pvMsg) || (NULL == ppvData))
    {
        return -1;
    }

    pstResp = (St_SecUserCertRequestResp*)WE_MALLOC(sizeof(St_SecUserCertRequestResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeUserCertReqResp(pvMsg,
                                     &(pstResp->iTargetID),
                                     &(pstResp->iResult),
                                     &(pstResp->pucCertReqMsg),
                                     &(pstResp->uiCertReqMsgLen));
    *(ppvData) = (WE_VOID*)pstResp;
    return iRet;
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodeRemoveContractResp                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode encoded data and get the response of user certificate request .
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piResult[OUT]:the result.
RETURN VALUE:
    0: success.
    -1: fail.
IMPORTANT NOTES:
    None.
==================================================================================================*/
WE_INT32 Sec_DecodeUserCertReqResp(WE_UINT8  *pcBuffer,
                         WE_INT32 *piTargetID, WE_INT32 *piResult,
                         WE_UCHAR **ppucCertReqMsg, WE_UINT32 *puiCertReqMsgLen)
{
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piResult )
        || -1 == SecEcdr_DecodeUint32( pstDecoder, puiCertReqMsgLen ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (*puiCertReqMsgLen > 0)
    {
        *ppucCertReqMsg = (WE_UCHAR *)WE_MALLOC((WE_ULONG)(*puiCertReqMsgLen) * sizeof(WE_UCHAR));
        if (NULL == *ppucCertReqMsg)
        {
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
        if (SecEcdr_DecodeOctets( pstDecoder, *ppucCertReqMsg, *puiCertReqMsgLen ) < 0)
        {
            WE_FREE(*ppucCertReqMsg);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    }    
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}

static WE_INT32 Sec_GetChangeCertStateResp (WE_VOID *pvMsg, WE_VOID** ppvData)
{   
    St_ChangeCertStateResp* pstResp = NULL; 
    WE_INT32 iRet = -1;

    if (NULL == pvMsg)
    {
        return -1;
    }
    /*malloc structure*/
    pstResp= (St_ChangeCertStateResp*)WE_MALLOC(sizeof(St_ChangeCertStateResp));
    if(NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeChangeWTLSCertAbleResp(pvMsg,
                                            &(pstResp->iTargetID),
                                            &(pstResp->iResult),
                                            &(pstResp->ucState));
    *(ppvData) = (WE_VOID*)pstResp;
    return iRet;
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeChangeWTLSCertAbleResp                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode encoded data and get the response of change wtls cert able. .
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piResult[OUT]:the result.
    WE_UINT8 *pucState[OUT]:state.
RETURN VALUE:
    0: success.
    -1: fail.
IMPORTANT NOTES:
    None.
==================================================================================================*/
WE_INT32 Sec_DecodeChangeWTLSCertAbleResp(WE_UINT8  *pcBuffer,
                              WE_INT32 *piTargetID, WE_INT32 *piResult,  WE_UINT8 *pucState)
{
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piResult )
        || -1 == SecEcdr_DecodeUint8( pstDecoder, pucState ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}

/*2. UE*/
/*==================================================================================================
FUNCTION: 
    Sec_DecodeConfirm
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    handle the change WTLS certificate state.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Broswer.
    WE_INT32 iTargetID:The identity of the invoker.
    WE_INT32 iResult:The value of the reslut.
    WE_UINT8 ucState: state of WTLS certificate.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DecodeConfirm(WE_UINT8 *pucBuffer, WE_VOID** ppvData)
{   
    St_Confirm* pstTemp = NULL; 
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_DecodeConfirm\n"));
    if (NULL == pucBuffer)
    {
        return -1;
    }
    pstDecoder = Sec_GetMsgDecoder( pucBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    /*malloc structure*/
    pstTemp = (St_Confirm*)WE_MALLOC(sizeof(St_Confirm));
    if(NULL == pstTemp)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->iConfirmStrId)))
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }   
    SecEcdr_DestroyDecoder( pstDecoder );
    *ppvData = (WE_VOID*)pstTemp;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodeChangePin
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    handle the change WTLS certificate state.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Broswer.
    WE_INT32 iTargetID:The identity of the invoker.
    WE_INT32 iResult:The value of the reslut.
    WE_UINT8 ucState: state of WTLS certificate.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DecodeChangePin(WE_UINT8 *pucBuffer,  WE_VOID** ppvData)
{   
    St_ChangePin* pstTemp = NULL; 
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_DecodeConfirm\n"));
    if (NULL == pucBuffer)
    {
        return -1;
    }
    pstDecoder = Sec_GetMsgDecoder( pucBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    /*malloc structure*/
    pstTemp = (St_ChangePin*)WE_MALLOC(sizeof(St_ChangePin));
    if(NULL == pstTemp)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->iNewPinId))
        -1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->iNewPinIdAfter )))
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }   
    SecEcdr_DestroyDecoder( pstDecoder );
    *ppvData = (WE_VOID*)pstTemp;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodeSelCert
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    handle the change WTLS certificate state.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Broswer.
    WE_INT32 iTargetID:The identity of the invoker.
    WE_INT32 iResult:The value of the reslut.
    WE_UINT8 ucState: state of WTLS certificate.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DecodeSelCert(WE_UINT8 *pucBuffer, WE_VOID** ppvData)
{   
    St_SelectCert* pstTemp = NULL; 
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32       iIndex = 0;
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_DecodeConfirm\n"));
    if (NULL == pucBuffer)
    {
        return -1;
    }
    pstDecoder = Sec_GetMsgDecoder( pucBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    /*malloc structure*/
    pstTemp = (St_SelectCert*)WE_MALLOC(sizeof(St_SelectCert));
    if(NULL == pstTemp)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->nbrOfCerts)))
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }   
    pstTemp->ppcCertName = (WE_CHAR**)WE_MALLOC(pstTemp->nbrOfCerts * sizeof(WE_CHAR*));
    if(NULL == pstTemp->ppcCertName)
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    for(iIndex = 0;iIndex < pstTemp->nbrOfCerts; iIndex ++)
    {
        if ( -1 == SecEcdr_DecodeString( pstDecoder, &(pstTemp->ppcCertName[iIndex])))
        {
            WE_FREE(pstTemp->ppcCertName);
            WE_FREE(pstTemp);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    }    
    SecEcdr_DestroyDecoder( pstDecoder );
    *ppvData = (WE_VOID*)pstTemp;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodeNameConfirm
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    handle the change WTLS certificate state.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Broswer.
    WE_INT32 iTargetID:The identity of the invoker.
    WE_INT32 iResult:The value of the reslut.
    WE_UINT8 ucState: state of WTLS certificate.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DecodeNameConfirm(WE_UINT8 *pucBuffer, WE_VOID** ppvData)
{   
    St_NameConfirm* pstTemp = NULL; 
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_DecodeConfirm\n"));
    if (NULL == pucBuffer)
    {
        return -1;
    }
    pstDecoder = Sec_GetMsgDecoder( pucBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    /*malloc structure*/
    pstTemp = (St_NameConfirm*)WE_MALLOC(sizeof(St_NameConfirm));
    if(NULL == pstTemp)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, (WE_INT32 *)&(pstTemp->pcCertName)))
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }   
    SecEcdr_DestroyDecoder( pstDecoder );
    *ppvData = (WE_VOID*)pstTemp;
    return 0;
}


/*==================================================================================================
FUNCTION: 
    Sec_DecodeSigntextConfirm
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    handle the change WTLS certificate state.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Broswer.
    WE_INT32 iTargetID:The identity of the invoker.
    WE_INT32 iResult:The value of the reslut.
    WE_UINT8 ucState: state of WTLS certificate.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DecodeSigntextConfirm(WE_UINT8 *pucBuffer, WE_VOID** ppvData)
{   
    St_SignTextConfirm* pstTemp = NULL; 
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_DecodeConfirm\n"));
    if (NULL == pucBuffer)
    {
        return -1;
    }
    pstDecoder = Sec_GetMsgDecoder( pucBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    /*malloc structure*/
    pstTemp = (St_SignTextConfirm*)WE_MALLOC(sizeof(St_SignTextConfirm));
    if(NULL == pstTemp)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->iInfoId)))
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }   
    if (-1 == SecEcdr_DecodeString( pstDecoder, &(pstTemp->pcText)))
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }   
    SecEcdr_DestroyDecoder( pstDecoder );
    *ppvData = (WE_VOID*)pstTemp;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodeCreatePin
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    handle the change WTLS certificate state.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Broswer.
    WE_INT32 iTargetID:The identity of the invoker.
    WE_INT32 iResult:The value of the reslut.
    WE_UINT8 ucState: state of WTLS certificate.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DecodeCreatePin(WE_UINT8 *pucBuffer, WE_VOID** ppvData)
{   
    St_CreatePin* pstTemp = NULL; 
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_DecodeConfirm\n"));
    if (NULL == pucBuffer)
    {
        return -1;
    }
    pstDecoder = Sec_GetMsgDecoder( pucBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    /*malloc structure*/
    pstTemp = (St_CreatePin*)WE_MALLOC(sizeof(St_CreatePin));
    if(NULL == pstTemp)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->iInfoId)) ||
        -1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->iInfoIdAfter)))
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }   
    SecEcdr_DestroyDecoder( pstDecoder );
    *ppvData = (WE_VOID*)pstTemp;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodeHash
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    handle the change WTLS certificate state.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Broswer.
    WE_INT32 iTargetID:The identity of the invoker.
    WE_INT32 iResult:The value of the reslut.
    WE_UINT8 ucState: state of WTLS certificate.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DecodeHash(WE_UINT8 *pucBuffer, WE_VOID** ppvData)
{   
    St_Hash* pstTemp = NULL; 
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_DecodeHash\n"));
    if (NULL == pucBuffer)
    {
        return -1;
    }
    pstDecoder = Sec_GetMsgDecoder( pucBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    /*malloc structure*/
    pstTemp = (St_Hash *)WE_MALLOC(sizeof(St_Hash) * sizeof(WE_UCHAR));
    if(NULL == pstTemp)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->iInfoId)))
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }   
    if (-1 == SecEcdr_DecodeString( pstDecoder, &(pstTemp->pcDisplayName)))
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }   
    SecEcdr_DestroyDecoder( pstDecoder );
    *ppvData = (WE_VOID*)pstTemp;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodePin
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    handle the change WTLS certificate state.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Broswer.
    WE_INT32 iTargetID:The identity of the invoker.
    WE_INT32 iResult:The value of the reslut.
    WE_UINT8 ucState: state of WTLS certificate.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DecodePin(WE_UINT8 *pucBuffer, WE_VOID** ppvData)
{   
    St_Pin* pstTemp = NULL; 
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_DecodeConfirm\n"));
    if (NULL == pucBuffer)
    {
        return -1;
    }
    pstDecoder = Sec_GetMsgDecoder( pucBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    /*malloc structure*/
    pstTemp = (St_Pin*)WE_MALLOC(sizeof(St_Pin));
    if(NULL == pstTemp)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->iInfoId)) ||
       -1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->iInfoIdAfer)) )
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }   
    SecEcdr_DestroyDecoder( pstDecoder );
    *ppvData = (WE_VOID*)pstTemp;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodeShowCertContent
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    handle the change WTLS certificate state.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Broswer.
    WE_INT32 iTargetID:The identity of the invoker.
    WE_INT32 iResult:The value of the reslut.
    WE_UINT8 ucState: state of WTLS certificate.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DecodeShowCertContent(WE_UINT8 *pucBuffer, WE_VOID** ppvData)
{   
    St_ShowCertContent* pstTemp = NULL; 
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_DecodeConfirm\n"));
    if (NULL == pucBuffer)
    {
        return -1;
    }
    pstDecoder = Sec_GetMsgDecoder( pucBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    /*malloc structure*/
    pstTemp = (St_ShowCertContent*)WE_MALLOC(sizeof(St_ShowCertContent));
    if(NULL == pstTemp)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->iNotAftMonth)) ||
        -1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->iNotBefMonth)))
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }   
    if ( -1 == SecEcdr_DecodeString( pstDecoder, &(pstTemp->pcIssuerStr )) ||
        -1 == SecEcdr_DecodeString( pstDecoder, &(pstTemp->pcNotAftStr ))||
        -1 == SecEcdr_DecodeString( pstDecoder, &(pstTemp->pcNotBefStr ))||
        -1 == SecEcdr_DecodeString( pstDecoder, &(pstTemp->pcSubjectStr ))||
        -1 == SecEcdr_DecodeString( pstDecoder, (WE_CHAR **)&(pstTemp->pucAlg ))||
        -1 == SecEcdr_DecodeString( pstDecoder, (WE_CHAR **)&(pstTemp->pucPubKey ))||
        -1 == SecEcdr_DecodeString( pstDecoder, (WE_CHAR **)&(pstTemp->pucSerialNumber )))
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
    }

    SecEcdr_DestroyDecoder( pstDecoder );
    *ppvData = (WE_VOID*)pstTemp;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodeShowCertList
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    handle the change WTLS certificate state.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Broswer.
    WE_INT32 iTargetID:The identity of the invoker.
    WE_INT32 iResult:The value of the reslut.
    WE_UINT8 ucState: state of WTLS certificate.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DecodeShowCertList(WE_UINT8 *pucBuffer, WE_VOID** ppvData)
{   
    St_ShowCertList* pstTemp = NULL; 
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32           iIndex = 0;
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_DecodeShowCertList\n"));
    if (NULL == pucBuffer)
    {
        return -1;
    }
    pstDecoder = Sec_GetMsgDecoder( pucBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    /*malloc structure*/
    pstTemp = (St_ShowCertList*)WE_MALLOC(sizeof(St_ShowCertList));
    if(NULL == pstTemp)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->nbrOfCerts)))
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }   
    pstTemp->ppcCertName = (WE_CHAR**)WE_MALLOC(pstTemp->nbrOfCerts * sizeof(WE_CHAR*));
    if(NULL == pstTemp->ppcCertName)
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    for(iIndex = 0;iIndex < pstTemp->nbrOfCerts; iIndex ++)
    {
        if ( -1 == SecEcdr_DecodeString( pstDecoder, &(pstTemp->ppcCertName[iIndex])))
        {
            WE_FREE(pstTemp->ppcCertName);
            WE_FREE(pstTemp);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    }    

    pstTemp->pucAble = (WE_UINT8*)WE_MALLOC(pstTemp->nbrOfCerts *sizeof(WE_UINT8));
    if(NULL == pstTemp->pucAble)
    {
        WE_FREE(pstTemp->ppcCertName);
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
   if ( -1 == SecEcdr_DecodeOctets( pstDecoder, (pstTemp->pucAble ),pstTemp->nbrOfCerts))
   {
        WE_FREE(pstTemp->pucAble);
        WE_FREE(pstTemp->ppcCertName);
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
   }
    pstTemp->pueCertType = (E_CertType*)WE_MALLOC(pstTemp->nbrOfCerts *sizeof(E_CertType));
    if(NULL == pstTemp->pueCertType)
    {
        WE_FREE(pstTemp->pucAble);
        WE_FREE(pstTemp->ppcCertName);
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
   if ( -1 == SecEcdr_DecodeOctets( pstDecoder, (WE_CHAR *)(pstTemp->pueCertType ), 
                                (pstTemp->nbrOfCerts) * sizeof(E_CertType)))
   {
        WE_FREE(pstTemp->pueCertType);
        WE_FREE(pstTemp->pucAble);
        WE_FREE(pstTemp->ppcCertName);
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
   }

    SecEcdr_DestroyDecoder( pstDecoder );
    *ppvData = (WE_VOID*)pstTemp;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodeShowSessionContent
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    handle the change WTLS certificate state.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Broswer.
    WE_INT32 iTargetID:The identity of the invoker.
    WE_INT32 iResult:The value of the reslut.
    WE_UINT8 ucState: state of WTLS certificate.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DecodeShowSessionContent(WE_UINT8 *pucBuffer, WE_VOID** ppvData)
{   
    St_ShowSessionContent* pstTemp = NULL; 
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_DecodeShowSessionContent\n"));
    if (NULL == pucBuffer)
    {
        return -1;
    }
    pstDecoder = Sec_GetMsgDecoder( pucBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    /*malloc structure*/
    pstTemp = (St_ShowSessionContent*)WE_MALLOC(sizeof(St_ShowSessionContent));
    if(NULL == pstTemp)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    if ( -1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->iConnTypeId )) ||
        -1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->iEncralgId ))||
        -1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->iHmacId ))||
        -1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->iKeyExchId )))
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }

    if ( -1 == SecEcdr_DecodeString( pstDecoder, &(pstTemp->pcEncrKeyLenStr)) ||
        -1 == SecEcdr_DecodeString( pstDecoder, &(pstTemp->pcHmacKeyLenStr ))||
        -1 == SecEcdr_DecodeString( pstDecoder, &(pstTemp->pcKeyExchKeyLenStr )))
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    SecEcdr_DestroyDecoder( pstDecoder );
    *ppvData = (WE_VOID*)pstTemp;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodeShowContractList
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    handle the change WTLS certificate state.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Broswer.
    WE_INT32 iTargetID:The identity of the invoker.
    WE_INT32 iResult:The value of the reslut.
    WE_UINT8 ucState: state of WTLS certificate.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DecodeShowContractList(WE_UINT8 *pucBuffer, WE_VOID** ppvData)
{   
    St_ShowContractsList* pstTemp = NULL; 
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32                iIndex = 0;
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_DecodeShowContractList\n"));
    if (NULL == pucBuffer)
    {
        return -1;
    }
    pstDecoder = Sec_GetMsgDecoder( pucBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    /*malloc structure*/
    pstTemp = (St_ShowContractsList*)WE_MALLOC(sizeof(St_ShowContractsList));
    if(NULL == pstTemp)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    if ( -1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->nbrOfContract )))
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    pstTemp->ppcTime = (WE_CHAR**)WE_MALLOC(pstTemp->nbrOfContract * sizeof(WE_CHAR*));
    if(NULL == pstTemp->ppcTime)
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    for(iIndex = 0;iIndex < pstTemp->nbrOfContract; iIndex ++)
    {
        if ( -1 == SecEcdr_DecodeString( pstDecoder, &(pstTemp->ppcTime[iIndex])))
        {
            WE_FREE(pstTemp->ppcTime);
            WE_FREE(pstTemp);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }    
        if ( -1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->piMonthID[iIndex] )))
        {
            WE_FREE(pstTemp->ppcTime);
            WE_FREE(pstTemp);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    }    
    printf("Sec_DecodeShowContractList6\n");
    /*
    if ( -1 == SecEcdr_DecodeOctets( pstDecoder, (WE_CHAR *)(pstTemp->piMonthID), pstTemp->nbrOfContract * sizeof(WE_INT32)))
    {
            WE_FREE(pstTemp->ppcTime);
            WE_FREE(pstTemp);
            SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }*/
    SecEcdr_DestroyDecoder( pstDecoder );
    *ppvData = (WE_VOID*)pstTemp;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodeStoreCert
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    handle the change WTLS certificate state.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Broswer.
    WE_INT32 iTargetID:The identity of the invoker.
    WE_INT32 iResult:The value of the reslut.
    WE_UINT8 ucState: state of WTLS certificate.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DecodeStoreCert(WE_UINT8 *pucBuffer, WE_VOID** ppvData)
{   
    St_StoreCert* pstTemp = NULL; 
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_DecodeConfirm\n"));
    if (NULL == pucBuffer)
    {
        return -1;
    }
    pstDecoder = Sec_GetMsgDecoder( pucBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    /*malloc structure*/
    pstTemp = (St_StoreCert*)WE_MALLOC(sizeof(St_StoreCert));
    if(NULL == pstTemp)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    if ( -1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->iNotAftMonth )) ||
        -1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->iNotBefMonth )))
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }

    if ( -1 == SecEcdr_DecodeString( pstDecoder, &(pstTemp->pcIssuerStr )) ||
        -1 == SecEcdr_DecodeString( pstDecoder, &(pstTemp->pcNotAftStr ))||
        -1 == SecEcdr_DecodeString( pstDecoder, &(pstTemp->pcNotBefStr ))||
        -1 == SecEcdr_DecodeString( pstDecoder, &(pstTemp->pcSubjectStr )))
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    SecEcdr_DestroyDecoder( pstDecoder );
    *ppvData = (WE_VOID*)pstTemp;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodeWarning
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    handle the change WTLS certificate state.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Broswer.
    WE_INT32 iTargetID:The identity of the invoker.
    WE_INT32 iResult:The value of the reslut.
    WE_UINT8 ucState: state of WTLS certificate.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DecodeWarning(WE_UINT8 *pucBuffer, WE_VOID** ppvData)
{   
    St_Warning* pstTemp = NULL; 
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_DecodeConfirm\n"));
    if (NULL == pucBuffer)
    {
        return -1;
    }
    pstDecoder = Sec_GetMsgDecoder( pucBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    /*malloc structure*/
    pstTemp = (St_Warning*)WE_MALLOC(sizeof(St_Warning));
    if(NULL == pstTemp)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->iWarningContent)))
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }   
    SecEcdr_DestroyDecoder( pstDecoder );
    *ppvData = (WE_VOID*)pstTemp;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodeStoreContract
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    handle the change WTLS certificate state.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Broswer.
    WE_INT32 iTargetID:The identity of the invoker.
    WE_INT32 iResult:The value of the reslut.
    WE_UINT8 ucState: state of WTLS certificate.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DecodeStoreContract(WE_UINT8 *pucBuffer, WE_VOID** ppvData)
{   
    St_ShowContractContent* pstTemp = NULL; 
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_DecodeStoreContract\n"));
    if (NULL == pucBuffer)
    {
        return -1;
    }
    pstDecoder = Sec_GetMsgDecoder( pucBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    /*malloc structure*/
    pstTemp = (St_ShowContractContent*)WE_MALLOC(sizeof(St_ShowContractContent));
    if(NULL == pstTemp)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    if ( -1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->iMonthID )))
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if ( -1 == SecEcdr_DecodeString( pstDecoder, &(pstTemp->pcContractStr )) ||
        -1 == SecEcdr_DecodeString( pstDecoder, &(pstTemp->pcSignatureStr )) ||
        -1 == SecEcdr_DecodeString( pstDecoder, &(pstTemp->pcTimeStr )))
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    SecEcdr_DestroyDecoder( pstDecoder );
    *ppvData = (WE_VOID*)pstTemp;
    return 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodeCurSvrCert
CREATE DATE:
    2007-03-15
AUTHOR:
    bird zhang
DESCRIPTION:
    handle the change WTLS certificate state.
ARGUMENTS PASSED:
    WE_UINT8 **ppucBuffer[OUT]: the data which will be sent to Broswer.
    WE_INT32 iTargetID:The identity of the invoker.
    WE_INT32 iResult:The value of the reslut.
    WE_UINT8 ucState: state of WTLS certificate.
RETURN VALUE:
    0: success.
    -1: fail.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_DecodeCurSvrCert(WE_UINT8 *pucBuffer, WE_VOID** ppvData)
{   
    St_ShowCertContent* pstTemp = NULL; 
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_LOG_MSG((0,(WE_UINT8)0,"SEC:....Sec_DecodeConfirm\n"));
    if (NULL == pucBuffer)
    {
        return -1;
    }
    pstDecoder = Sec_GetMsgDecoder( pucBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    /*malloc structure*/
    pstTemp = (St_ShowCertContent*)WE_MALLOC(sizeof(St_ShowCertContent));
    if(NULL == pstTemp)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    if ( -1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->iNotAftMonth )) ||
        -1 == SecEcdr_DecodeInt32( pstDecoder, &(pstTemp->iNotBefMonth )))
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if ( -1 == SecEcdr_DecodeString( pstDecoder, &(pstTemp->pcIssuerStr )) ||
        -1 == SecEcdr_DecodeString( pstDecoder, &(pstTemp->pcNotAftStr ))||
        -1 == SecEcdr_DecodeString( pstDecoder, &(pstTemp->pcNotBefStr ))||
        -1 == SecEcdr_DecodeString( pstDecoder, &(pstTemp->pcSubjectStr ))||
        -1 == SecEcdr_DecodeString( pstDecoder, (WE_CHAR **)&(pstTemp->pucAlg ))||
        -1 == SecEcdr_DecodeString( pstDecoder, (WE_CHAR **)&(pstTemp->pucPubKey ))||
        -1 == SecEcdr_DecodeString( pstDecoder, (WE_CHAR **)&(pstTemp->pucSerialNumber )))
    {
        WE_FREE(pstTemp);
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    SecEcdr_DestroyDecoder( pstDecoder );
    *ppvData = (WE_VOID*)pstTemp;
    return 0;
}

static WE_INT32 Sec_GetTlsUsrCertResp (WE_VOID *pvMsg, WE_VOID** ppvData)
{
    St_SecTlsGetUserCertResp *pstResp = NULL;
    WE_INT32 iRet = -1;

    if ((NULL == pvMsg) || (NULL == ppvData))
    {
        return -1;
    }

    pstResp = (St_SecTlsGetUserCertResp*)WE_MALLOC(sizeof(St_SecTlsGetUserCertResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    pstResp->pstCert = (St_SecTlsAsn1Certificate*)WE_MALLOC(sizeof(St_SecTlsAsn1Certificate));
    if (NULL == pstResp->pstCert)
    {
        WE_FREE(pstResp);
        return -1;
    }
    iRet = Sec_DecodeTlsGetUsrCertResp(pvMsg,
                                     &(pstResp->iTargetID),
                                     &(pstResp->usResult),
                                     &(pstResp->pucPubKeyHash),
                                     &(pstResp->usPubKeyHashLen),
                                     &(pstResp->pstCert),
                                     &(pstResp->ucNbrCerts));
    *(ppvData) = (WE_VOID*)pstResp;
    return iRet;
}

/*==================================================================================================
FUNCTION: 
    Sec_DecodeTlsGetUsrCertResp                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode encoded data and get the response of tls get user certificate .
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piResult[OUT]:the result.
    WE_UCHAR **ppucPubkeyHash[OUT]:Pointer to the user certificate hash.
    WE_INT32 *pusPubKeyHashLen[OUT]:Length of public hash.
    St_SecTlsAsn1Certificate **ppstCert[OUT]:Pointer to the structure os cert.
    WE_INT32 *piNbrCerts[OUT]:number of certificate.
RETURN VALUE:
    0: success.
    -1: fail.
IMPORTANT NOTES:
    None.
==================================================================================================*/
WE_INT32 Sec_DecodeTlsGetUsrCertResp(WE_UINT8  *pcBuffer, 
                            WE_INT32 *piTargetID, WE_UINT16 *pusResult,
                             WE_UCHAR **ppucPubkeyHash, WE_UINT16 *pusPubKeyHashLen,
                             St_SecTlsAsn1Certificate **ppstCert, WE_INT32 *piNbrCerts)
{
    St_SecEcdrDecoder *pstDecoder = NULL;
    WE_INT32          iLoop = 0;
    WE_INT32          iKLoop = 0;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeUint16( pstDecoder, pusResult )
        || -1 == SecEcdr_DecodeUint16( pstDecoder, pusPubKeyHashLen )
        || -1 == SecEcdr_DecodeInt32( pstDecoder, piNbrCerts ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (*piNbrCerts > 0)
    {
        *ppstCert = (St_SecTlsAsn1Certificate *)WE_MALLOC((*piNbrCerts)*sizeof(St_SecTlsAsn1Certificate ));
        if (NULL == *ppstCert)
        {
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
        for(iLoop = 0;iLoop < *piNbrCerts ;iLoop++)
        {
            if (-1 == SecEcdr_DecodeInt8( pstDecoder, &((*ppstCert)[iLoop].ucFormat) )
                || -1 == SecEcdr_DecodeInt32( pstDecoder, &((*ppstCert)[iLoop].uiCertLen) ))
            {
                for( iKLoop = 0;iKLoop < iLoop;iKLoop ++)
                {
                    WE_FREE((*ppstCert)[iLoop].pucCert);
                }
                WE_FREE( *ppstCert );
                SecEcdr_DestroyDecoder( pstDecoder );
                return -1;
            }
            
            if ((*ppstCert)[iLoop].uiCertLen > 0)
            {
                (*ppstCert)[iLoop].pucCert = (WE_UCHAR *)WE_MALLOC((WE_ULONG)((*ppstCert)[iLoop].uiCertLen ) * sizeof(WE_UCHAR));
                if (NULL == (*ppstCert)[iLoop].pucCert)
                {  
                    for( iKLoop = 0;iKLoop < iLoop;iKLoop ++)
                    {
                        WE_FREE((*ppstCert)[iLoop].pucCert);
                    }
                    WE_FREE( *ppstCert );
                    SecEcdr_DestroyDecoder( pstDecoder );
                    return -1;
                }
                if (SecEcdr_DecodeOctets( pstDecoder, (*ppstCert)[iLoop].pucCert , (*ppstCert)[iLoop].uiCertLen  ) < 0)
                {
                    for( iKLoop = 0;iKLoop < iLoop;iKLoop ++)
                    {
                        WE_FREE((*ppstCert)[iLoop].pucCert);
                    }
                    WE_FREE( *ppstCert );
                    SecEcdr_DestroyDecoder( pstDecoder );
                    return -1;
                }
            }
        }
    }
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}
static WE_INT32 Sec_GetTlsCipherSuiteResp (WE_VOID *pvMsg, WE_VOID** ppvData)
{
    St_SecTlsGetCipherSuitesResp *pstResp = NULL;
    WE_INT32 iRet = -1;

    if ((NULL == pvMsg) || (NULL == ppvData))
    {
        return -1;
    }
    pstResp = (St_SecTlsGetCipherSuitesResp*)WE_MALLOC(sizeof(St_SecTlsGetCipherSuitesResp));
    if (NULL == pstResp)
    {
        return -1;
    }
    iRet = Sec_DecodeTlsGetCipherSuiteResp(pvMsg,
                                     &(pstResp->iTargetID),
                                     &(pstResp->usResult),
                                     &(pstResp->pucCipherSuites),
                                     &(pstResp->usCipherSuitesLen));
    if(iRet < 0)
    {
        WE_FREE(pstResp);
        return -1;            
    }
    
    *(ppvData) = (WE_VOID*)pstResp;    
     return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_DecodeTlsGetCipherSuiteResp                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    decode encoded data and get the response of tls get cipher suite .
ARGUMENTS PASSED:
    WE_UINT8  *pcBuffer[IN]:encoded data.
    WE_INT32 *piTargetID[OUT]:the identity of the invoker.
    WE_INT32 *piResult[OUT]:the result.
    WE_UCHAR ** ppucCipherSuites[OUT]:Pointer to the cipher suite.
    WE_INT32 *piCipherSuitesLen[OUT]:Length of cipher suite.
RETURN VALUE:
    0: success.
    -1: fail.
IMPORTANT NOTES:
    None.
==================================================================================================*/
WE_INT32 Sec_DecodeTlsGetCipherSuiteResp(WE_UINT8  *pcBuffer,  
                                        WE_INT32 *piTargetID, WE_UINT16 *pusResult,
                                        WE_UCHAR ** ppucCipherSuites, 
                                        WE_UINT16 *pusCipherSuitesLen)
{    
    St_SecEcdrDecoder *pstDecoder = NULL;

    if (NULL == pcBuffer)
    {
        return -1;
    }
    
    pstDecoder = Sec_GetMsgDecoder( pcBuffer );
    if (NULL == pstDecoder)
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (-1 == SecEcdr_DecodeInt32( pstDecoder, piTargetID )
        || -1 == SecEcdr_DecodeUint16( pstDecoder, pusResult )
        || -1 == SecEcdr_DecodeUint16( pstDecoder, pusCipherSuitesLen ))
    {
        SecEcdr_DestroyDecoder( pstDecoder );
        return -1;
    }
    
    if (*pusCipherSuitesLen > 0)
    {
        *ppucCipherSuites = (WE_UCHAR *)WE_MALLOC((WE_ULONG)(*pusCipherSuitesLen) * sizeof(WE_UCHAR));
        if (NULL == *ppucCipherSuites)
        {
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
        if (SecEcdr_DecodeOctets( pstDecoder, *ppucCipherSuites, *pusCipherSuitesLen ) < 0)
        {
            WE_FREE(*ppucCipherSuites);
            SecEcdr_DestroyDecoder( pstDecoder );
            return -1;
        }
    }   
    
    SecEcdr_DestroyDecoder( pstDecoder );
    return 0;
}

