/*=====================================================================================
    FILE NAME : sec_wimsp.c
    MODULE NAME : SEC
    
    GENERAL DESCRIPTION
        
    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2002-2006 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
    
    Modification Tracking
    Date       Author         Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-07-22 Bird     none      Init
    
=====================================================================================*/

/*==================================================================================================
*   Include File 
*=================================================================================================*/
#include"sec_comm.h"

/***************************************************************************************************
*   Macro Define Section
***************************************************************************************************/
#define SEC_PEER_CACHE       (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pstPeerCache)
#define SEC_SESSION_CACHE    (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pstSessionCache)
#define SEC_WE_HANDLE        (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->hWeHandle)
#define SEC_CALLER_TYPE      ((ISec*)hSecHandle)->iIFType


/***************************************************************************************************
*   Function Define Section
***************************************************************************************************/
/*==================================================================================================
FUNCTION: 
    Sec_WimPeerRemove
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    delete peer table that which mastersecreID is eque to the input value
    
ARGUMENTS PASSED:
    WE_INT32 iMasterSecretId[IN]: The id of the corresponding session slot
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_WimPeerRemove (WE_HANDLE hSecHandle,WE_INT32 iMasterSecretId)
{
    WE_INT32 iIndexI = 0;
    St_SecPeerRec* pstPeerCache = NULL;
    if(NULL == hSecHandle)
    {
        Sec_WimDeletePeerLinkSessionResponse(hSecHandle,M_SEC_ERR_INVALID_PARAMETER);
        return;
    }
    pstPeerCache = SEC_PEER_CACHE;
 
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimDeletePeerLinkSessionResponse(hSecHandle,M_SEC_ERR_WIM_NOT_INITIALISED);
        return;
    }
    if (iMasterSecretId >= M_SEC_SESSION_CACHE_SIZE)
    {
        Sec_WimDeletePeerLinkSessionResponse(hSecHandle,M_SEC_ERR_INVALID_PARAMETER);
        return;
    }
    for (iIndexI = 0; iIndexI < M_SEC_PEER_CACHE_SIZE; iIndexI++)
    {
        if (pstPeerCache[iIndexI].ucSlotInUse 
            && (pstPeerCache[iIndexI].iMasterSecretId == iMasterSecretId))
        {
            /*find  the peer entry that in use and the master secret ID is equal to the given master ID,delete it */
            Sec_WimClrPeerRec(&(pstPeerCache[iIndexI]));
        }
    }
    Sec_WimDeletePeerLinkSessionResponse(hSecHandle,M_SEC_ERR_OK);
}

/*==================================================================================================
FUNCTION: 
    Sec_WimPeerAttachSession
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
Adds a peer entry that links to the given master secret. If a peer entry already exists,
this function overwrites the old peer entry.

ARGUMENTS PASSED:
    WE_UINT8 uiConnectionType[IN]: This parameter can be SEC_CONNECTION_TYPE_TLS, SEC_CONNECTION_TYPE_SSL 
                                                    or M_SEC_WTLS_CONNECTION_MODE.
    WE_UCHAR *pucAddress[IN]:The server address
    WE_INT32 iAddressLen[IN]:The length of the address
    WE_UINT16 usPortNum[IN]:The port number of the connection.
    WE_INT32 iMasterSecretId[IN]:The id of the corresponding session slot. 
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_WimPeerAttachSession (WE_HANDLE   hSecHandle,WE_UINT8 uiConnectionType,
                               const WE_UCHAR *pucAddress, WE_INT32 iAddressLen,
                               WE_UINT16 usPortNum, WE_INT32 iMasterSecretId)
{
    WE_INT32 iIndexI = 0; 
    WE_INT32 iIndexJ = 0;
    WE_UINT32 uiTime = 0;
    WE_INT32 iMatch = 0;
    WE_INT32 iFound = 0;
    St_SecPeerRec* pstPeerCache = NULL;
    St_SecSessionRec* pstSessionCache = NULL;
    
    if((NULL == pucAddress)||(NULL == hSecHandle))
    {
        Sec_WimPeerLinkSessionResponse(hSecHandle,M_SEC_ERR_INVALID_PARAMETER);
        return;
    }
    pstPeerCache = SEC_PEER_CACHE;
    pstSessionCache =  SEC_SESSION_CACHE;
    
    
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimPeerLinkSessionResponse(hSecHandle,M_SEC_ERR_WIM_NOT_INITIALISED);
        return;
    }
    
    if (iMasterSecretId >= M_SEC_SESSION_CACHE_SIZE)
    {
        Sec_WimPeerLinkSessionResponse(hSecHandle,M_SEC_ERR_INVALID_PARAMETER);
        return;
    }
    /*in use*/
    for (iIndexI = 0; iIndexI < M_SEC_PEER_CACHE_SIZE; iIndexI++) 
    {  
        if (pstPeerCache[iIndexI].ucSlotInUse &&
            pstPeerCache[iIndexI].iConnectionType == uiConnectionType &&
            pstPeerCache[iIndexI].iAddrLen == iAddressLen &&
            pstPeerCache[iIndexI].usPortNum == usPortNum)
        { 
            iMatch = 1;
            /*compare address*/
            for (iIndexJ = 0; iIndexJ < iAddressLen; iIndexJ++) 
            {
                if (pucAddress[iIndexJ] != pstPeerCache[iIndexI].aucAddress[iIndexJ])
                {
                    iMatch = 0;
                    break;
                }
            }
            if (iMatch)
            {
                pstPeerCache[iIndexI].iMasterSecretId = iMasterSecretId;
                Sec_WimPeerLinkSessionResponse(hSecHandle,M_SEC_ERR_OK);
                return;
            }
        }
    }
    
    for (iIndexI = 0; iIndexI < M_SEC_PEER_CACHE_SIZE; iIndexI++) 
    {
        if (!pstPeerCache[iIndexI].ucSlotInUse)/*find not use*/
        { 
            iFound = 1; 
            break; 
        }
    }
    
    /*if all are in used ,look up which is in not active*/
    if (!iFound)
    {
        for (iIndexI = 0; iIndexI < M_SEC_PEER_CACHE_SIZE; iIndexI++) 
        {
            if (!pstSessionCache[pstPeerCache[iIndexI].iMasterSecretId].ucIsActive)
            { 
                iFound = 1; 
                break; 
            }
        }
    }
    /*if all are active ,look up the entry whose create time is less than the first entry,*/          
    if (!iFound) 
    {           
        iIndexJ=0;
        uiTime = pstSessionCache[pstPeerCache[0].iMasterSecretId].stInfo.uiCreationTime;
        for (iIndexI = 1; iIndexI < M_SEC_PEER_CACHE_SIZE;iIndexI++) 
        {
            if (pstSessionCache[pstPeerCache[iIndexI].iMasterSecretId].stInfo.uiCreationTime < uiTime)
            {
                uiTime = pstSessionCache[pstPeerCache[iIndexI].iMasterSecretId].stInfo.uiCreationTime;
                iIndexJ=iIndexI;
            } 

        }
        iIndexI=iIndexJ;
       
    }
    if (iAddressLen > M_SEC_ADDRESS_LEN)
    {
        iAddressLen = M_SEC_ADDRESS_LEN;
    }
    if(M_SEC_PEER_CACHE_SIZE <= iIndexI)
    {
        Sec_WimPeerLinkSessionResponse(hSecHandle,M_SEC_ERR_GENERAL_ERROR);
        return;
    }
    pstPeerCache[iIndexI].iConnectionType = (WE_INT32)uiConnectionType;
    for (iIndexJ=0; iIndexJ<iAddressLen; iIndexJ++)
    {
        pstPeerCache[iIndexI].aucAddress[iIndexJ] = pucAddress[iIndexJ];
    }
    pstPeerCache[iIndexI].iAddrLen = iAddressLen;
    pstPeerCache[iIndexI].usPortNum = usPortNum;
    pstPeerCache[iIndexI].iMasterSecretId = iMasterSecretId;
    pstPeerCache[iIndexI].ucSlotInUse = 1;
    Sec_WimPeerLinkSessionResponse(hSecHandle,M_SEC_ERR_OK);
}

/*==================================================================================================
FUNCTION: 
    Sec_WimPeerSearch
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    Checks if there is a peer with a matching address and port number. 
    If there is no such peer, the function tries to match only the address. 
    If there is a matching peer, it is not necessary to set up a new session.    
    
ARGUMENTS PASSED:
    WE_INT32 iWid[IN]:The identifier of this call
    WE_UINT8 ucConnectionType[IN]:This parameter can be SEC_CONNECTION_TYPE_TLS,
                                                    SEC_CONNECTION_TYPE_SSL or M_SEC_WTLS_CONNECTION_MODE.
    WE_UCHAR *pucAddress[IN]:The server address
    WE_INT32 iAddrLen[IN]:The length of the address.
    WE_UINT16 usPortNum[IN]:The port number of the connection.
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_WimPeerSearch (WE_HANDLE   hSecHandle, WE_INT32 iWid, WE_UINT8 ucConnectionType,
                        const WE_UCHAR *pucAddress, WE_INT32 iAddrLen,
                        WE_UINT16 usPortNum)
{
    WE_INT32 iIndexI = 0;
    WE_INT32 iIndexJ = 0;
    WE_INT32 iMatch = 0;
    WE_UINT32 iNow = 0;
    WE_UINT32 uiMaxAge = 0;
    WE_UINT32 uiTimeDiff = 0;    
    St_SecPeerRec* pstPeerCache = NULL;
    St_SecSessionRec* pstSessionCache = NULL;
    WE_INT32 iTmpID = -1;/* add by Sam [070126] */
    
    if((NULL == hSecHandle ) || (NULL == pucAddress))
    {
        Sec_WimPeerSearchResponse(hSecHandle,iWid, M_SEC_ERR_INVALID_PARAMETER, 
                                  ucConnectionType, 0, -1);
        return;
    }
    
    pstPeerCache = SEC_PEER_CACHE;
    pstSessionCache = SEC_SESSION_CACHE;
    uiMaxAge = 12*60*60;                
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimPeerSearchResponse(hSecHandle,iWid, M_SEC_ERR_WIM_NOT_INITIALISED, ucConnectionType, 0, -1);
        return;
    }
    iNow = SEC_CURRENT_TIME();    
    for (iIndexI = 0; iIndexI < M_SEC_PEER_CACHE_SIZE; iIndexI++)
    {
        if  ((usPortNum == pstPeerCache[iIndexI].usPortNum) 
            && (iAddrLen == pstPeerCache[iIndexI].iAddrLen) 
            && (ucConnectionType == pstPeerCache[iIndexI].iConnectionType))
        {
            /* modified by Bird 070208,only mach the address and port number,that mean find the correct entry*/
            iMatch = 1;
            for (iIndexJ = 0; iIndexJ < iAddrLen; iIndexJ++) 
            {   
                iMatch = iMatch && (pucAddress[iIndexJ] == pstPeerCache[iIndexI].aucAddress[iIndexJ]);
                if(0 == iMatch)
                {
                    break;
                }
            }
            /*port & address match,return master secret ID*/
            if (iMatch)
            {
                iTmpID = pstPeerCache[iIndexI].iMasterSecretId;
                uiTimeDiff = iNow - (pstSessionCache[iTmpID].stInfo.uiCreationTime);
                /*if the set up time is not more than 12 hours*/
                if (uiTimeDiff <= uiMaxAge)
                {

                    Sec_WimPeerSearchResponse(hSecHandle,iWid, M_SEC_ERR_OK, ucConnectionType, 
                                                  pstPeerCache[iIndexI].iMasterSecretId, -1);
                    return;
                }
                else
                {
                    Sec_WimClrSessionRec(&(pstSessionCache[iTmpID]));
                    Sec_WimClrPeerRec(&(pstPeerCache[iIndexI]));
                }

            }
        }
    }
    
    Sec_WimPeerSearchResponse(hSecHandle,iWid, M_SEC_ERR_NOT_FOUND, ucConnectionType, 0, -1);
}

/*==================================================================================================
FUNCTION: 
    Sec_WimSessionEnable
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
   This function tells if the session slot is active or not active. If "isActive" is equal to zero, 
   the indicated session slot is marked as not active. Otherwise, it is marked as active.
   A session slot that is "active" is not reused. TLS/WTLS should keep a session marked as "active" as long as 
   any connection based upon that session is active.
    
ARGUMENTS PASSED:
   WE_INT32 iMasterSecretId[IN]:The id of the corresponding session slot. 
   WE_UINT8 ucIsActive[IN]:Whether the active flag should be turned on (¡Ù 0) or off (= 0).
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_WimSessionEnable (WE_HANDLE hSecHandle,WE_INT32 iMasterSecretId, WE_UINT8 ucIsActive)
{    
    St_SecSessionRec* pstSessionCache = NULL;
    if(NULL == hSecHandle)
    {
        Sec_WimSessionActiveResponse(hSecHandle,M_SEC_ERR_INVALID_PARAMETER);
        return;
    }
    pstSessionCache = SEC_SESSION_CACHE;
    
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimSessionActiveResponse(hSecHandle,M_SEC_ERR_WIM_NOT_INITIALISED);
        return;
    }
    if (iMasterSecretId >= M_SEC_SESSION_CACHE_SIZE)
    {
        Sec_WimSessionActiveResponse(hSecHandle,M_SEC_ERR_INVALID_PARAMETER);
        return;
    }
    /*active*/
    pstSessionCache[iMasterSecretId].ucIsActive = ucIsActive;
    Sec_WimSessionActiveResponse(hSecHandle,M_SEC_ERR_OK);
}




/*==================================================================================================
FUNCTION: 
    Sec_WimSessionDisable
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    This function marks a session entry as non-resumable
    
ARGUMENTS PASSED:
    WE_INT32 iMasterSecretId[IN]: The id of the corresponding session slot. 
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_WimSessionDisable(WE_HANDLE hSecHandle,WE_INT32 iMasterSecretId)
{    
    St_SecSessionRec* pstSessionCache = NULL;
    if(NULL == hSecHandle)
    {
        Sec_WimSessionInvalidateResponse(hSecHandle,M_SEC_ERR_INVALID_PARAMETER);
        return;
    }
    pstSessionCache = SEC_SESSION_CACHE;
    
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimSessionInvalidateResponse(hSecHandle,M_SEC_ERR_WIM_NOT_INITIALISED);
        return;
    }
    if (iMasterSecretId >= M_SEC_SESSION_CACHE_SIZE)
    {
        Sec_WimSessionInvalidateResponse(hSecHandle,M_SEC_ERR_INVALID_PARAMETER);
        return;
    }
    /*non-resumable*/
    pstSessionCache[iMasterSecretId].stInfo.ucSessionOptions &= (0xff ^ G_SEC_SESSION_OPTIONS_RESUMABLE);
    Sec_WimSessionInvalidateResponse(hSecHandle,M_SEC_ERR_OK);
}




/*==================================================================================================
FUNCTION: 
    Sec_WimClrSession
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    Clear all session data    
ARGUMENTS PASSED:

RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_WimClrSession (WE_HANDLE hSecHandle)
{
    WE_INT32 iIndexI = 0;
    St_SecPeerRec* pstPeerCache = NULL;
    St_SecSessionRec* pstSessionCache = NULL;
    
    if(NULL == hSecHandle )
    {
        Sec_WimSessionCleanResponse(hSecHandle,M_SEC_ERR_INVALID_PARAMETER);
        return;
    }
    
    pstPeerCache = SEC_PEER_CACHE;
    pstSessionCache = SEC_SESSION_CACHE;
    
    
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimSessionCleanResponse(hSecHandle,M_SEC_ERR_WIM_NOT_INITIALISED);
        return;
    }
    
    for (iIndexI = 0; iIndexI < M_SEC_SESSION_CACHE_SIZE; iIndexI++)
    {
        Sec_WimClrSessionRec(&(pstSessionCache[iIndexI]));
        Sec_WimClrPeerRec(&(pstPeerCache[iIndexI]));
    }
    Sec_WimSessionCleanResponse(hSecHandle,M_SEC_ERR_OK);
}

/*==================================================================================================
FUNCTION: 
    Sec_WimSessionSlotClr
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    Clear session data  and peer data
    
ARGUMENTS PASSED:
    WE_INT32 iMasterSecretId: The id of the corresponding session slot
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_WimSessionSlotClr(WE_HANDLE hSecHandle, WE_INT32 iMasterSecretId)
{
    St_SecPeerRec* pstPeerCache = NULL;
    St_SecSessionRec* pstSessionCache = NULL;
    
    if(NULL == hSecHandle )
    {
        return;
    }
    
    pstPeerCache = SEC_PEER_CACHE;
    pstSessionCache = SEC_SESSION_CACHE;
    Sec_WimClrSessionRec(&(pstSessionCache[iMasterSecretId]));
    Sec_WimClrPeerRec(&(pstPeerCache[iMasterSecretId]));
}



/*==================================================================================================
FUNCTION: 
    Sec_WimFetchSession
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    Fetch the contents of the indicated session entry.     
ARGUMENTS PASSED:
    WE_INT32 iMasterSecretId: The id of the corresponding session slot
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_WimFetchSession (WE_HANDLE hSecHandle, WE_INT32 iWid, WE_INT32 iMasterSecretId)
{
    WE_UCHAR sessionId[M_SEC_SESSION_ID_LEN] = {0};
    WE_UCHAR privateKeyId[M_SEC_PRIVATE_KEY_ID_LEN] = {0};
    WE_INT32 iIndexI = 0;
    WE_UINT8 ucLen = 0;
    St_SecSessionRec* pstSessionCache = NULL;
    if(NULL == hSecHandle)
    {
        Sec_WimGetSessionMastResponse (hSecHandle,iWid, M_SEC_ERR_INVALID_PARAMETER, 0, NULL, 
                                                            0, 0, 0, NULL, 0);
        return;
    }
    pstSessionCache = SEC_SESSION_CACHE;
      
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimGetSessionMastResponse (hSecHandle,iWid, M_SEC_ERR_WIM_NOT_INITIALISED, 
            0, NULL, 0, 0, 0, NULL, 0);
        return;
    }
    
    if ((iMasterSecretId >= M_SEC_SESSION_CACHE_SIZE) || (!pstSessionCache[iMasterSecretId].ucSlotInUse))
    {
        Sec_WimGetSessionMastResponse (hSecHandle,iWid, M_SEC_ERR_NOT_FOUND, 0, NULL, 
                                                            0, 0, 0, NULL, 0);
    }
    else
    {
        ucLen = pstSessionCache[iMasterSecretId].stInfo.ucSessionIdLen;
        for (iIndexI = 0; iIndexI < ucLen; iIndexI++) 
        {
            sessionId[iIndexI] = pstSessionCache[iMasterSecretId].stInfo.aucSessionId[iIndexI];
        }
        for (iIndexI = 0; iIndexI < M_SEC_PRIVATE_KEY_ID_LEN; iIndexI++)
        {
            privateKeyId[iIndexI] = pstSessionCache[iMasterSecretId].stInfo.aucPrivateKeyId[iIndexI];
        }
        Sec_WimGetSessionMastResponse (hSecHandle,iWid, M_SEC_ERR_OK,
                                       pstSessionCache[iMasterSecretId].stInfo.ucSessionOptions,
                                       sessionId,
                                       pstSessionCache[iMasterSecretId].stInfo.ucSessionIdLen,
                                       pstSessionCache[iMasterSecretId].stInfo.aucCipherSuite,
                                       pstSessionCache[iMasterSecretId].stInfo.ucCompressionAlg,
                                       privateKeyId,
                                       pstSessionCache[iMasterSecretId].stInfo.uiCreationTime);
    }
}




/*==================================================================================================
FUNCTION: 
    Sec_WimUpdateSession
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    Store new values for a session entry.
    
ARGUMENTS PASSED:
    WE_INT32 iMasterSecretId[IN]:The id of the corresponding session slot. 
    WE_UINT8 ucSessionOptions[IN]:The session options defined in sec_if.h, 
                                                    SESSION_OPTIONS_RESUMABLE, SESSION_OPTIONS_SERVER_AUTH, 
                                                    or SESSION_OPTIONS_CLIENT_AUTH. 
    WE_UCHAR *pucSessionId[IN]:The session identifier
    WE_UINT8 ucSessionIdLen[IN]:The length of sessionId.
    WE_UINT8 ucCipherSuite[2][IN]: the cipher suites
    WE_UINT8 ucCompressionAlg[IN]: compression algorithm
    WE_UCHAR *pucPrivateKeyId[IN]: private Key identifier 
    WE_UINT32 uiCreationTime[IN]:The time that this session record was created
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_WimUpdateSession (WE_HANDLE hSecHandle, WE_INT32 iMasterSecretId, 
                           WE_UINT8 ucSessionOptions,
                           const WE_UCHAR *pucSessionId, WE_UINT8 ucSessionIdLen,
                           WE_UINT8 ucCipherSuite[2], WE_UINT8 ucCompressionAlg,
                           const WE_UCHAR *pucPrivateKeyId,
                           WE_UINT32 uiCreationTime)
{
    WE_INT32 iIndexI = 0;
    St_SecSessionRec* pstSessionCache = NULL; 
    
    if((NULL == hSecHandle) || (NULL == pucSessionId) 
       || (NULL == pucPrivateKeyId) || (NULL == ucCipherSuite))
    {
        Sec_WimSessionUpdateResponse(hSecHandle,M_SEC_ERR_INVALID_PARAMETER);
        return;
    }
    pstSessionCache = SEC_SESSION_CACHE;
    
    if (!Sec_WimIsInited(hSecHandle))
    {
        Sec_WimSessionUpdateResponse(hSecHandle,M_SEC_ERR_WIM_NOT_INITIALISED);
        return;
    }
    if (iMasterSecretId >= M_SEC_SESSION_CACHE_SIZE)
    {
        Sec_WimSessionUpdateResponse(hSecHandle,M_SEC_ERR_INVALID_PARAMETER);
        return;
    }
    if (ucSessionIdLen > M_SEC_SESSION_ID_LEN)
    {
        ucSessionIdLen = M_SEC_SESSION_ID_LEN;
    }
    pstSessionCache[iMasterSecretId].stInfo.ucSessionOptions = ucSessionOptions;
    for (iIndexI = 0; iIndexI < ucSessionIdLen; iIndexI++)
    {
        pstSessionCache[iMasterSecretId].stInfo.aucSessionId[iIndexI] = pucSessionId[iIndexI];
    }
    pstSessionCache[iMasterSecretId].stInfo.ucSessionIdLen = ucSessionIdLen;
    pstSessionCache[iMasterSecretId].stInfo.aucCipherSuite[0] = ucCipherSuite[0];
    pstSessionCache[iMasterSecretId].stInfo.aucCipherSuite[1] = ucCipherSuite[1];
    pstSessionCache[iMasterSecretId].stInfo.ucCompressionAlg = ucCompressionAlg;
    for (iIndexI=0; iIndexI<M_SEC_PRIVATE_KEY_ID_LEN; iIndexI++)
    {
        pstSessionCache[iMasterSecretId].stInfo.aucPrivateKeyId[iIndexI] = pucPrivateKeyId[iIndexI];
    }
    pstSessionCache[iMasterSecretId].stInfo.uiCreationTime = uiCreationTime;
    
    Sec_WimSessionUpdateResponse(hSecHandle,M_SEC_ERR_OK);
}



/*==================================================================================================
FUNCTION: 
    Sec_WimGetMastSecret
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    get mastersecret according master secret ID
    
ARGUMENTS PASSED:
    WE_INT32 iMasterSecretId[IN]: The id of the corresponding session slot. 
    WE_UCHAR *pucMasterSecret[OUT]:master secret
    WE_INT32 iMasterSecretLen[IN]:length fo master secret
    WE_INT32 *piRes[OUT]:result
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_WimGetMastSecret(WE_HANDLE hSecHandle, WE_INT32 iMasterSecretId, 
                          WE_UCHAR *pucMasterSecret, 
                          WE_INT32 iMasterSecretLen, WE_INT32 *piRes)
{
    WE_INT32 iIndexI = 0;
    St_SecSessionRec* pstSessionCache = NULL;
    
    if((NULL == hSecHandle) || (NULL == pucMasterSecret) || (NULL == piRes))
    {
        return;
    }
    pstSessionCache = SEC_SESSION_CACHE;
    
    if ((iMasterSecretId >= M_SEC_SESSION_CACHE_SIZE) || (!pstSessionCache[iMasterSecretId].ucSlotInUse))
    {
        *piRes = M_SEC_ERR_NOT_FOUND;
        return;
    }
    for (iIndexI = 0; iIndexI < iMasterSecretLen; iIndexI++)
    {
        pucMasterSecret[iIndexI] = pstSessionCache[iMasterSecretId].aucMasterSecret[iIndexI];
    }
    *piRes = M_SEC_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    Sec_WimClrSessionInfo
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    clear session data
    
ARGUMENTS PASSED:
    WE_INT32 iIndex[IN]: index 
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_WimClrSessionInfo(WE_HANDLE hSecHandle,WE_INT32 iIndex)
{
    St_SecSessionRec* pstSessionCache = NULL;

    if(NULL == hSecHandle)
    {
        return;
    }
    pstSessionCache = SEC_SESSION_CACHE;
    
    Sec_WimClrSessionRec(&(pstSessionCache[iIndex]));
}


/*==================================================================================================
FUNCTION: 
    Sec_WimClrPeerInfo
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    clear peer data
    
ARGUMENTS PASSED:
    WE_INT32 iIndex[IN]: index 
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_WimClrPeerInfo(WE_HANDLE hSecHandle, WE_INT32 iIndex)
{
    St_SecPeerRec* pstPeerCache = NULL;

    if(NULL == hSecHandle)
    {
        return;
    }
    
    pstPeerCache = SEC_PEER_CACHE;
    
    Sec_WimClrPeerRec(&(pstPeerCache[iIndex]));
}
/*==================================================================================================
FUNCTION: 
    Sec_WimClrSessionInfoRec
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    clear session data
    
ARGUMENTS PASSED:
    St_SecSession *pstInfo[out]:session data
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_WimClrSessionInfoRec(St_SecSession *pstInfo)
{
    WE_INT32 iIndexI = 0;
    if(NULL == pstInfo)
    {
        return;
    }
    pstInfo->ucSessionOptions = 0;
    for (iIndexI = 0; iIndexI < M_SEC_SESSION_ID_LEN; iIndexI++)
    {
        pstInfo->aucSessionId[iIndexI] = 0;
    }
    pstInfo->aucCipherSuite[0] = 0;
    pstInfo->aucCipherSuite[1] = 0;
    pstInfo->ucCompressionAlg = 0;
    for (iIndexI = 0; iIndexI < M_SEC_PRIVATE_KEY_ID_LEN; iIndexI++)
    {
        pstInfo->aucPrivateKeyId[iIndexI] = 0;
    }
    pstInfo->uiCreationTime = 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_WimClrSessionRec
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    clear session record
    
ARGUMENTS PASSED:
    St_SecSessionRec *pstRec[out]: session record data
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_WimClrSessionRec( St_SecSessionRec *pstRec )
{
    WE_INT32 iIndexI = 0;
    if(NULL == pstRec)
    {
        return;
    }
    Sec_WimClrSessionInfoRec(&(pstRec->stInfo));
    pstRec->ucIsActive = 0;
    for (iIndexI = 0; iIndexI < M_SEC_TLS_MASTER_SECRET_LEN; iIndexI++)
    {
        pstRec->aucMasterSecret[iIndexI] = 0;
    }
    pstRec->ucSlotInUse = 0;
}

/*==================================================================================================
FUNCTION: 
    Sec_WimClrPeerRec
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    clear peer record
    
ARGUMENTS PASSED:
    St_SecPeerRec *pstrec[out]: peer record data
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_WimClrPeerRec( St_SecPeerRec *pstrec )
{
    WE_INT32 iIndexI = 0;
    if(NULL == pstrec)
    {
        return;
    }
    for (iIndexI = 0; iIndexI < M_SEC_ADDRESS_LEN; iIndexI++)
    {
        pstrec->aucAddress[iIndexI] = 0;
    }
    pstrec->iAddrLen = 0;
    pstrec->usPortNum = 0;
    pstrec->iMasterSecretId = 0;
    pstrec->ucSlotInUse = 0;
}




/*==================================================================================================
FUNCTION: 
    Sec_WimSessionStart
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    initial,get session and peer data from file
    
ARGUMENTS PASSED:

RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_WimSessionStart (WE_HANDLE hSecHandle)
{
    WE_UINT16 usSLen=(M_SEC_SESSION_CACHE_SIZE*sizeof( St_SecSessionRec));
    WE_UINT16 usPLen=(M_SEC_PEER_CACHE_SIZE*sizeof( St_SecPeerRec));
    WE_UINT32 uiHeaderLen = 0;
    WE_INT32 iResult = 0;
    E_WE_ERROR eRes = E_WE_OK;
    WE_HANDLE  hFileHandle = NULL;
    WE_LONG lReadNum = 0;    
    St_SecPeerRec* pstPeerCache = NULL;
    St_SecSessionRec* pstSessionCache = NULL;
    WE_UINT8 *pucTmpPeer = NULL;
    WE_UINT8 *pucTmpSession = NULL;
    
    if(NULL == hSecHandle)
    {
        return;
    }
    pstPeerCache = SEC_PEER_CACHE;
    pstSessionCache = SEC_SESSION_CACHE;
    /* delete by Sam [070203] */
    /*
    for (iIndexI=0; iIndexI<M_SEC_SESSION_CACHE_SIZE; iIndexI++)
    {
        Sec_WimClrSessionRec(&(pstSessionCache[iIndexI]));
    }
    for (iIndexI=0; iIndexI<M_SEC_PEER_CACHE_SIZE; iIndexI++)
    {
        Sec_WimClrPeerRec(&(pstPeerCache[iIndexI]));
    }*/
    
    /*modify by bird 060807,save session,peer&'V' in one file,*/
    /*read S_H file*/
    if (M_SEC_ERR_OK == Sec_WimCheckFileExist(hSecHandle,M_SEC_SH_NAME))
    {
        WE_UINT32 uiSessionLen =0;
        WE_UINT32 uiPeerLen =0;
        WE_UINT32 uiViewLen =0;
        
        eRes = WE_FILE_OPEN(SEC_WE_HANDLE, M_SEC_SH_NAME, WE_FILE_SET_RDWR, &hFileHandle);
        if(eRes != E_WE_OK )
        {
            return;
        }
        
        /*get session and peer data length*/
        iResult = Sec_WimGetSessionLength(hSecHandle,hFileHandle,&uiSessionLen,
                                          &uiPeerLen,&uiViewLen,&lReadNum);
        if(iResult != 0)
        {
            eRes = WE_FILE_CLOSE(hFileHandle);
            return;
        }
        /*********************read session section************************************/
        uiHeaderLen += (WE_UINT32)lReadNum;
        pucTmpSession = (WE_UINT8*)WE_MALLOC(uiSessionLen * sizeof(WE_UINT8));
        (WE_VOID)WE_MEMSET(pucTmpSession, 0x00, uiSessionLen*sizeof(WE_UINT8));
        /*read session data*/
        iResult = Sec_WimDecryptDataFromFile(hSecHandle,hFileHandle, pucTmpSession, 
                                            (WE_INT32)uiHeaderLen, (WE_INT32)uiSessionLen,
                                            (WE_INT32*)&lReadNum);
        if (lReadNum < (WE_LONG)uiSessionLen ||(iResult != M_SEC_ERR_OK))
        {
            WE_FREE(pucTmpSession);
            eRes = WE_FILE_CLOSE(hFileHandle);
            return;
        }
        (WE_VOID)WE_MEMCPY(pstSessionCache, pucTmpSession, usSLen);
        WE_FREE(pucTmpSession);
        /*********************read peer section************************************/
        uiHeaderLen += (WE_UINT32)lReadNum ;
        pucTmpPeer = (WE_UINT8*)WE_MALLOC(uiPeerLen*sizeof(WE_UINT8));
        (WE_VOID)WE_MEMSET(pucTmpPeer, 0x00, uiPeerLen*sizeof(WE_UINT8));
        iResult = Sec_WimDecryptDataFromFile(hSecHandle,hFileHandle, pucTmpPeer, 
                                            (WE_INT32)uiHeaderLen, (WE_INT32)uiPeerLen,
                                            (WE_INT32*)&lReadNum);        
        if (lReadNum < (WE_LONG)uiPeerLen ||(iResult != M_SEC_ERR_OK))
        {
            WE_FREE(pucTmpPeer);
            eRes = WE_FILE_CLOSE(hFileHandle);
            return;
        }
        (WE_VOID)WE_MEMCPY(pstPeerCache, pucTmpPeer, usPLen);
        WE_FREE(pucTmpPeer);
        eRes = WE_FILE_CLOSE(hFileHandle);
    }
    eRes = eRes;
}
/*==================================================================================================
FUNCTION: 
    Sec_WimWriteCacheToFile
CREATE DATE:
    2007-02-08
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    write session and peer to file  
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IN/OUT]: can be M_SEC_SESSION_HEADER or M_SEC_PEER_HEADER
    WE_UINT32 uiSessionCount[IN]:session data length
    WE_UINT32 uiPeerCount[IN]: peer data length
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_WimWriteCacheToFile(WE_HANDLE hSecHandle,WE_UINT32 uiSessionCount, WE_UINT32 uiPeerCount)
{
    WE_UINT32 uiHeaderLen = 0;
    WE_UINT32 uiPos = 0;
    WE_INT32 iResult = 0;
    E_WE_ERROR eRes = E_WE_OK;
    /*file*/
    WE_HANDLE hFileHandle = NULL;
    WE_LONG lWriteNum = 0;

    WE_INT32 iNewPos = 0;
    St_SecSessionRec* pstSessionCache = NULL;
    St_SecPeerRec* pstPeerCache = NULL;

    if(NULL == hSecHandle) 
    {
        return;
    }
    pstSessionCache = SEC_SESSION_CACHE;
    pstPeerCache = SEC_PEER_CACHE;

    if(M_SEC_ERR_OK == Sec_WimCheckFileExist(hSecHandle,M_SEC_SH_NAME))
    {
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE,M_SEC_SH_NAME);
    }

    /*E_WE_ERROR by zhanghq 06/7/6*/
    /*create S_H file*/
    eRes = WE_FILE_OPEN(SEC_WE_HANDLE, M_SEC_SH_NAME, WE_FILE_SET_CREATE , &hFileHandle);
    if (eRes != E_WE_OK)
    {
        return;
    }
    eRes = WE_FILE_CLOSE(hFileHandle);    
    
    /*write head 'S'*/
    eRes = WE_FILE_OPEN(SEC_WE_HANDLE,M_SEC_SH_NAME, WE_FILE_SET_WRONLY, &hFileHandle);
    
    if(eRes != E_WE_OK)
    {
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE,M_SEC_SH_NAME);
        return;
    }
    
    uiPos += 3 * M_FILE_LENGTH_BITS;
    
    iResult = Sec_WimEncryptDataToFile(hSecHandle,hFileHandle, (void*)pstSessionCache,
        (WE_INT32)uiPos, (WE_INT32)uiSessionCount,(WE_INT32*)&lWriteNum);
    
    if ((lWriteNum < (WE_INT32)uiSessionCount) ||(iResult != M_SEC_ERR_OK))
    {    
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE,M_SEC_SH_NAME);
        return;
    }
    uiPos = (WE_UINT32)lWriteNum;
    
    /*write length file head */
    eRes = WE_FILE_SEEK(hFileHandle,  (WE_LONG)uiHeaderLen , WE_FILE_SEEK_SET,&iNewPos);    
    eRes = WE_FILE_WRITE(hFileHandle,&uiPos, M_FILE_LENGTH_BITS,&lWriteNum);
    if ((lWriteNum < (WE_INT32)M_FILE_LENGTH_BITS) || (eRes != E_WE_OK))
    {    
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE,M_SEC_SH_NAME);
        return;
    }

    /*write peer data*/
    /*get length of session,cal the peer pos at first*/
    uiPos = uiHeaderLen + M_FILE_LENGTH_BITS *3 + uiPos;
    /*write peer data*/
    iResult = Sec_WimEncryptDataToFile(hSecHandle,hFileHandle, (void*)pstPeerCache,
                                      (WE_INT32)uiPos, (WE_INT32)uiPeerCount,
                                      (WE_INT32*)&lWriteNum);
    if ((lWriteNum < (WE_INT32)uiPeerCount) ||(iResult != M_SEC_ERR_OK))
    {    
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE,M_SEC_SH_NAME);
        return;
    }
    /*write length file head */   
    uiPos = uiHeaderLen + M_FILE_LENGTH_BITS;
    eRes = WE_FILE_SEEK(hFileHandle,(WE_LONG)uiPos , WE_FILE_SEEK_SET,&iNewPos);  
    uiPos = (WE_UINT32)lWriteNum;
    eRes = WE_FILE_WRITE(hFileHandle,&uiPos, M_FILE_LENGTH_BITS,&lWriteNum);
    if ((lWriteNum < (WE_INT32)M_FILE_LENGTH_BITS) || (eRes != E_WE_OK))
    {    
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE,M_SEC_SH_NAME);
        return;
    }
    eRes = WE_FILE_CLOSE(hFileHandle);
}
/*==================================================================================================
FUNCTION: 
    Sec_WimWriteSessionCacheToFile
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    write session data to file
    
ARGUMENTS PASSED:
    WE_CHAR cHeader[IN]: can be M_SEC_SESSION_HEADER or M_SEC_PEER_HEADER
    WE_UINT32 uiCount[IN]:data length
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_WimWriteSessionCacheToFile(WE_HANDLE hSecHandle,WE_UINT32 uiCount)
{ 
    WE_UINT32 uiHeaderLen = 0;
    WE_UINT32 uiPos = 0;
    WE_INT32 iResult = 0;
    E_WE_ERROR eRes = E_WE_OK;
    /*file*/
    WE_HANDLE hFileHandle = NULL;
    WE_LONG lWriteNum = 0;

    WE_INT32 iNewPos = 0;
    St_SecSessionRec* pstSessionCache = NULL;
    
    if(NULL == hSecHandle) 
    {
        return;
    }
    pstSessionCache = SEC_SESSION_CACHE;
    
    if(M_SEC_ERR_OK == Sec_WimCheckFileExist(hSecHandle,M_SEC_SH_NAME))
    {
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE,M_SEC_SH_NAME);
    }

    /*E_WE_ERROR by zhanghq 06/7/6*/
    /*create S_H file*/
    eRes = WE_FILE_OPEN(SEC_WE_HANDLE, M_SEC_SH_NAME, WE_FILE_SET_CREATE , &hFileHandle);
    if (eRes != E_WE_OK)
    {
        return;
    }
    eRes = WE_FILE_CLOSE(hFileHandle);    
    
    /*write head 'S'*/
    eRes = WE_FILE_OPEN(SEC_WE_HANDLE,M_SEC_SH_NAME, WE_FILE_SET_WRONLY, &hFileHandle);
    
    if(eRes != E_WE_OK)
    {
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE,M_SEC_SH_NAME);
        return;
    }
    
    uiPos += 3 * M_FILE_LENGTH_BITS;
    
    iResult = Sec_WimEncryptDataToFile(hSecHandle,hFileHandle, (void*)pstSessionCache,
        (WE_INT32)uiPos, (WE_INT32)uiCount,(WE_INT32*)&lWriteNum);
    
    if ((lWriteNum < (WE_INT32)uiCount) ||(iResult != M_SEC_ERR_OK))
    {    
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE,M_SEC_SH_NAME);
        return;
    }
    uiPos = (WE_UINT32)lWriteNum;
    
    /*write length file head */
    eRes = WE_FILE_SEEK(hFileHandle,  (WE_LONG)uiHeaderLen , WE_FILE_SEEK_SET,&iNewPos);    
    eRes = WE_FILE_WRITE(hFileHandle,&uiPos, M_FILE_LENGTH_BITS,&lWriteNum);
    if ((lWriteNum < (WE_INT32)M_FILE_LENGTH_BITS) || (eRes != E_WE_OK))
    {    
        eRes = WE_FILE_CLOSE(hFileHandle);
        eRes = WE_FILE_REMOVE(SEC_WE_HANDLE,M_SEC_SH_NAME);
        return;
    }
    eRes = WE_FILE_CLOSE(hFileHandle);
}


/*==================================================================================================
FUNCTION: 
    Sec_WimWritePeerCacheToFile
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    write peer data from cach to file.    
ARGUMENTS PASSED:
    WE_UINT32 uiCount: data length.
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
void Sec_WimWritePeerCacheToFile(WE_HANDLE hSecHandle, WE_UINT32 uiCount)
{ 
    WE_UINT32 uiHeaderLen = 0;
    WE_UINT32 uiPos = 0;
    WE_INT32 iResult = 0;
    E_WE_ERROR eRes = E_WE_OK;
    
    /*file*/
    WE_HANDLE hFileHandle = NULL;
    WE_LONG lWriteNum = 0;
    WE_LONG lReadNum = 0;
    WE_INT32 iNewPos = 0;
    St_SecPeerRec* pstPeerCache = NULL;
    
    if(NULL == hSecHandle)
    {
        return;
    }
    pstPeerCache = SEC_PEER_CACHE;
    /*have find */
    if(M_SEC_ERR_OK == Sec_WimCheckFileExist(hSecHandle,M_SEC_SH_NAME))
    {
        WE_UINT32 uiSessionLen =0;
        WE_UINT32 uiPeerLen =0;
        WE_UINT32 uiViewLen =0;
        
        eRes = WE_FILE_OPEN(SEC_WE_HANDLE, M_SEC_SH_NAME, WE_FILE_SET_RDWR, &hFileHandle);
        if(eRes != E_WE_OK)
        {
            eRes = WE_FILE_CLOSE(hFileHandle);
            eRes = WE_FILE_REMOVE(SEC_WE_HANDLE,M_SEC_SH_NAME);
            return;
        }
        /*get session and peer data length*/
        iResult = Sec_WimGetSessionLength(hSecHandle,hFileHandle,&uiSessionLen,
                                          &uiPeerLen,&uiViewLen,&lReadNum);
        if(iResult != 0)
        {
            eRes = WE_FILE_CLOSE(hFileHandle);
            eRes = WE_FILE_REMOVE(SEC_WE_HANDLE,M_SEC_SH_NAME);
            return;
        }
        /*get length of session,cal the peer pos at first*/
        uiPos = uiHeaderLen + M_FILE_LENGTH_BITS *3 + uiSessionLen;
        /*write peer data*/
        iResult = Sec_WimEncryptDataToFile(hSecHandle,hFileHandle, (void*)pstPeerCache,
                                          (WE_INT32)uiPos, (WE_INT32)uiCount,
                                          (WE_INT32*)&lWriteNum);
        if ((lWriteNum < (WE_INT32)uiCount) ||(iResult != M_SEC_ERR_OK))
        {    
            eRes = WE_FILE_CLOSE(hFileHandle);
            eRes = WE_FILE_REMOVE(SEC_WE_HANDLE,M_SEC_SH_NAME);
            return;
        }
        /*write length file head */   
        uiPos = uiHeaderLen + M_FILE_LENGTH_BITS;
        eRes = WE_FILE_SEEK(hFileHandle,(WE_LONG)uiPos , WE_FILE_SEEK_SET,&iNewPos);  
        uiPos = (WE_UINT32)lWriteNum;
        eRes = WE_FILE_WRITE(hFileHandle,&uiPos, M_FILE_LENGTH_BITS,&lWriteNum);
        if ((lWriteNum < (WE_INT32)M_FILE_LENGTH_BITS) || (eRes != E_WE_OK))
        {    
            eRes = WE_FILE_CLOSE(hFileHandle);
            eRes = WE_FILE_REMOVE(SEC_WE_HANDLE,M_SEC_SH_NAME);
            return;
        }
        eRes = WE_FILE_CLOSE(hFileHandle);
    }
}


/*==================================================================================================
FUNCTION: 
    Sec_WimSessionStop
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    session close and write session data to file
    
ARGUMENTS PASSED:

RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_WimSessionStop (WE_HANDLE hSecHandle)
{
    WE_UINT32 uiSessionCount = 0;
    WE_UINT32 uiPeerCount = 0;
    WE_INT32 iSessionRecordSize = 0;
    WE_INT32 iPeerRecordSize = 0;
    WE_INT32 iIndexI = 0;
    St_SecSessionRec* pstSessionCache = NULL;
    St_SecPeerRec* pstPeerCache = NULL;
    WE_INT32 iRes = 0;
    if(NULL == hSecHandle)
    {
        return;
    }
    if ( (WE_INT32)E_SEC_WTLS == SEC_CALLER_TYPE) /*is wtls*/
    {
        iRes = Sec_WimCheckFileExist(hSecHandle, M_SEC_SH_NAME);
        if (iRes == M_SEC_ERR_OK)
        {
            iRes = (WE_INT32)WE_FILE_REMOVE(SEC_WE_HANDLE, M_SEC_SH_NAME);
        }
    }
    else /*not wtls*/
    {
        return;
    }
    pstSessionCache = SEC_SESSION_CACHE;
    pstPeerCache = SEC_PEER_CACHE;
    iSessionRecordSize = sizeof( St_SecSessionRec);
    /*modified by Bird 070208, to optimize the session writing*/
    uiSessionCount = (WE_UINT32)iSessionRecordSize * M_SEC_SESSION_CACHE_SIZE;    
    iPeerRecordSize = sizeof( St_SecPeerRec);
    uiPeerCount = (WE_UINT32)iPeerRecordSize * M_SEC_PEER_CACHE_SIZE;    
    Sec_WimWriteCacheToFile(hSecHandle, uiSessionCount,uiPeerCount);
    
    for (iIndexI = 0; iIndexI < M_SEC_SESSION_CACHE_SIZE; iIndexI++)
    {
        Sec_WimClrSessionRec(&(pstSessionCache[iIndexI]));
    }
    for (iIndexI = 0; iIndexI < M_SEC_PEER_CACHE_SIZE; iIndexI++)
    {
        Sec_WimClrPeerRec(&(pstPeerCache[iIndexI]));
    }
}
/*==================================================================================================
FUNCTION: 
    Sec_WimSaveMastSecret
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    find an entry to store master secret
    
ARGUMENTS PASSED:
    WE_UCHAR *pucMasterSecret[IN]:master secret
    WE_INT32 iMasterSecretLen[IN]:master secret lenght
    WE_INT32 *piMasterSecretId[OUT]:secret ID
    WE_INT32 *piRes[OUT]:result
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_WimSaveMastSecret(WE_HANDLE hSecHandle, WE_UCHAR *pucMasterSecret, 
                           WE_INT32 iMasterSecretLen, 
                           WE_INT32 *piMasterSecretId, WE_INT32 *piRes)
{
    WE_INT32 iIndexI;
    WE_INT32 iFound = 0;
    WE_UINT32 uiTime = 0;
    
    St_SecSessionRec* pstSessionCache = NULL;

    if(NULL == piRes)
    {
        return;
    }
    if((NULL == hSecHandle) || (NULL == pucMasterSecret) 
       || (NULL == piMasterSecretId))
    {
        *piRes = M_SEC_ERR_INVALID_PARAMETER;
        return;
    }
    pstSessionCache = SEC_SESSION_CACHE;
    
    /*find the entry that is not use*/
    for (iIndexI=0; iIndexI<M_SEC_SESSION_CACHE_SIZE; iIndexI++) 
    {
        if (!pstSessionCache[iIndexI].ucSlotInUse)
        { 
            iFound = 1; 
            *piMasterSecretId = iIndexI; 
            uiTime = pstSessionCache[iIndexI].stInfo.uiCreationTime; 
            break; 
        }
    }
        
    /*look up the entry that is not resumable and is not active */
    if (!iFound) 
    {
        for (iIndexI=0; iIndexI<M_SEC_SESSION_CACHE_SIZE; iIndexI++)
            if (((pstSessionCache[iIndexI].stInfo.ucSessionOptions & G_SEC_SESSION_OPTIONS_RESUMABLE) !=
                G_SEC_SESSION_OPTIONS_RESUMABLE) &&  (!pstSessionCache[iIndexI].ucIsActive))
            {
                if (!iFound)
                {
                    iFound = 1;
                    *piMasterSecretId = iIndexI;
                    uiTime = pstSessionCache[iIndexI].stInfo.uiCreationTime;
                }
                else
                {
                    if (pstSessionCache[iIndexI].stInfo.uiCreationTime < uiTime)
                    {
                        uiTime = pstSessionCache[iIndexI].stInfo.uiCreationTime;
                        *piMasterSecretId = iIndexI;
                    }
                } 
            }
    }
    /*resumable and not active*/
    if (!iFound)
    { 
        for (iIndexI=0; iIndexI<M_SEC_SESSION_CACHE_SIZE; iIndexI++)
            if (((pstSessionCache[iIndexI].stInfo.ucSessionOptions & G_SEC_SESSION_OPTIONS_RESUMABLE) == 
                G_SEC_SESSION_OPTIONS_RESUMABLE) && (!pstSessionCache[iIndexI].ucIsActive))
            {
                if (!iFound)
                {
                    iFound = 1;
                    *piMasterSecretId = iIndexI;
                    uiTime = pstSessionCache[iIndexI].stInfo.uiCreationTime;
                }
                else
                {
                    if (pstSessionCache[iIndexI].stInfo.uiCreationTime < uiTime)
                    {
                        uiTime = pstSessionCache[iIndexI].stInfo.uiCreationTime;
                        *piMasterSecretId = iIndexI;
                    }
                }
            }
    }
    /*not resumable and active*/
    if (!iFound)
    { 
        for (iIndexI=0; iIndexI<M_SEC_SESSION_CACHE_SIZE; iIndexI++)
            if (((pstSessionCache[iIndexI].stInfo.ucSessionOptions & G_SEC_SESSION_OPTIONS_RESUMABLE) != 
                G_SEC_SESSION_OPTIONS_RESUMABLE) &&  (pstSessionCache[iIndexI].ucIsActive))
            {
                if (!iFound)
                {
                    iFound = 1;
                    *piMasterSecretId = iIndexI;
                    uiTime = pstSessionCache[iIndexI].stInfo.uiCreationTime;
                }
                else
                {
                    if (pstSessionCache[iIndexI].stInfo.uiCreationTime < uiTime)
                    {
                        uiTime = pstSessionCache[iIndexI].stInfo.uiCreationTime;
                        *piMasterSecretId = iIndexI;
                    }
                }
            }
    }

    /*compare create time*/
    if (!iFound) 
    {
        *piMasterSecretId = 0;
        uiTime = pstSessionCache[0].stInfo.uiCreationTime;
        for (iIndexI = 1; iIndexI < M_SEC_SESSION_CACHE_SIZE; iIndexI++)
            if (pstSessionCache[iIndexI].stInfo.uiCreationTime < uiTime)
            {
                *piMasterSecretId = iIndexI;
                uiTime = pstSessionCache[iIndexI].stInfo.uiCreationTime;
            }
    }
    
    Sec_WimClrSessionRec(&(pstSessionCache[*piMasterSecretId]));
    for (iIndexI=0; iIndexI<iMasterSecretLen; iIndexI++) 
    {
        pstSessionCache[*piMasterSecretId].aucMasterSecret[iIndexI] = pucMasterSecret[iIndexI];
    }
    pstSessionCache[*piMasterSecretId].ucSlotInUse = 1; 
    *piRes = M_SEC_ERR_OK;
}
/*==================================================================================================
FUNCTION: 
    Sec_WimGetSessionLength
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    get session length
    
ARGUMENTS PASSED:
    WE_HANDLE hFileHandle[IN]:file handle
    WE_UINT32* puiSessionLength[OUT]:session lenght
    WE_UINT32* uiPeerLength[OUT]:peer length
    WE_UINT32* uiViewLength[OUT]:view length
    WE_LONG *plReadNum[OUT]:read length
RETURN VALUE:
    NULL
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Sec_WimGetSessionLength(WE_HANDLE hSecHandle,WE_HANDLE hFileHandle,
                                 WE_UINT32* puiSessionLength, WE_UINT32* puiPeerLength,
                                 WE_UINT32* puiViewLength,WE_LONG *plReadNum)
{
    WE_INT32 iNewPos = 0;
    E_WE_ERROR  eResult = E_WE_OK;
    WE_UCHAR* pucData = NULL;
    WE_UCHAR* pucTemp = NULL;
    WE_LONG  lReadNum = 0;
    WE_INT32 iDataLen = 3 * M_FILE_LENGTH_BITS;
    WE_UINT8 *pucSessionTemp = NULL;
    WE_UINT8 *pucPeerTemp = NULL;
    WE_UINT8 *pucViewTemp = NULL;

    pucSessionTemp = (WE_UCHAR* )puiSessionLength;
    pucPeerTemp = (WE_UCHAR* )puiPeerLength;
    pucViewTemp = (WE_UCHAR* )puiViewLength;
    
    /*malloc memory*/
    
    if((NULL == hSecHandle) || (NULL == puiSessionLength) ||(NULL == puiPeerLength) 
       || (NULL == puiViewLength) || (NULL == plReadNum))
    {
        return -1;
    }
    pucData = (WE_UCHAR*)WE_MALLOC((WE_UINT32)iDataLen);
    if(NULL== pucData)
    {
        return -1;
    }
    eResult = WE_FILE_SEEK(hFileHandle, 0, WE_FILE_SEEK_SET,&iNewPos);
    eResult = WE_FILE_READ(hFileHandle, pucData, iDataLen,&lReadNum);
    if(eResult != E_WE_OK || lReadNum < iDataLen)
    {
        WE_FREE(pucData);
        return -1;
    }
    *plReadNum = lReadNum;
     pucTemp = pucData;
    *pucSessionTemp++ = *(pucTemp++);
    *pucSessionTemp++ = *pucTemp++;
    *pucSessionTemp++ = *pucTemp++;
    *pucSessionTemp = *pucTemp++;

    *pucPeerTemp++ = *pucTemp++;
    *pucPeerTemp++ = *pucTemp++;
    *pucPeerTemp++ = *pucTemp++;
    *pucPeerTemp = *pucTemp++;

    *pucViewTemp++ = *pucTemp++;
    *pucViewTemp++ = *pucTemp++;
    *pucViewTemp++ = *pucTemp++;
    *pucViewTemp = *pucTemp;
    
    WE_FREE(pucData);
    return 0 ;
}

