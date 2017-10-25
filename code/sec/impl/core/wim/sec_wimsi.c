/*=====================================================================================
    FILE NAME : sec_wimsi.c
    MODULE NAME : SEC
    
    GENERAL DESCRIPTION
        this file defined several functions to manage and deal with 
        the session information.

    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2002-2006 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
    
    Modification Tracking
    Date       Author         Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-07-22 Bird             none      Init
    
=====================================================================================*/

/*==================================================================================================
*   Include File 
*=================================================================================================*/
#include"sec_comm.h"

/***************************************************************************************************
*   Macro Define Section
***************************************************************************************************/
/*if save server's certficates or not ,0 is not save ,other wise is save*/
#define M_SEC_SAVE_SERVERCERT 0

/*GLOBAL VARIABLE*/
#define SEC_SIINFO (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pstInfo)

/***************************************************************************************************
*   Prototype Declare Section
***************************************************************************************************/
#if(M_SEC_SAVE_SERVERCERT == 1)
static WE_BOOL 
Sec_WimInfoGetInfo(St_SecInfo *pstSecInfo,WE_UCHAR * pucData);

static WE_BOOL 
Sec_WimInfoSetData(WE_UCHAR*pucData, St_SecInfo *pstSecInfo);

static WE_BOOL 
Sec_WimInfoGetSize(St_SecInfo *pstSecInfo,WE_INT32 * piLen);
#endif

static WE_BOOL 
Sec_WimInfoFreeInfo(St_SecInfo *pstSecInfo);


/***************************************************************************************************
*   Function Define Section
***************************************************************************************************/
/*==================================================================================================
FUNCTION: 
    Sec_WimInfoGetMasterSecretId
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    get master secret ID according to security ID
    
ARGUMENTS PASSED:
    WE_INT32 iSecurityId[IN]:The security id number
    WE_UINT8 *ucMasterSecretId[out]:master secret ID
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
static WE_INT32 Sec_WimInfoGetMasterSecretId(WE_HANDLE hSecHandle,WE_INT32 iSecurityId, 
                                             WE_UINT8 *ucMasterSecretId)
{
    if((NULL == hSecHandle) || (NULL == ucMasterSecretId))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    if (iSecurityId >= 0) 
    {
        WE_INT32 iSlot = M_SEC_IDTOSLOT(iSecurityId);
        
        if (SEC_SIINFO->astIds[iSlot].iSecurityId == iSecurityId) 
        {
            *ucMasterSecretId = SEC_SIINFO->astIds[iSlot].ucMasterSecretId;
            return M_SEC_ERR_OK;
        }
    }
    
    return M_SEC_ERR_NOT_FOUND;
}

/*==================================================================================================
FUNCTION: 
    Sec_WimStartInfo
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    fill St_SecInfo strucure ,using the input data from file
    
ARGUMENTS PASSED:
    WE_UCHAR *pucData[IN]:input data
    WE_INT32 iLen[IN]:length
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
WE_INT32 Sec_WimStartInfo(WE_HANDLE hSecHandle, WE_UCHAR *pucData, WE_INT32 iLen)
{
    WE_INT32 iIndex = 0;
    
    if(NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    /* add by Sam [070203] */
    if (pucData == NULL || iLen <= 0)
    {
        return M_SEC_ERR_OK;
    }
    
    for (iIndex=0; iIndex<M_SEC_SECURITYID; ++iIndex) 
    {
        SEC_SIINFO->astIds[iIndex].iSecurityId = -1;
        SEC_SIINFO->astIds[iIndex].ucMasterSecretId = 0;
    }
    
    SEC_SIINFO->iCurSecurityId = 0;
    (void)WE_MEMCPY(SEC_SIINFO,pucData,(WE_UINT32)iLen);    
    /* delete by Sam [070203] */
#if 0    
    if (pucData != NULL) 
    {
        /*modify by bird 060807,for reading data to struct correspond with how to save data to file */
        (void)WE_MEMCPY(SEC_SIINFO,pucData,(WE_UINT32)iLen);    
#if  0  
        if (!Sec_WimInfoGetInfo(&SEC_SIINFO,pucData))
            return M_SEC_ERR_GENERAL_ERROR;
#endif
    }
#endif    
    
    return M_SEC_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    Sec_WimGetDataInfo
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    get data from St_SecInfo 
ARGUMENTS PASSED:
    WE_UCHAR **ppucData[OUT]:data get from St_SecInfo
    WE_INT32 *piLen[out]:data length
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
WE_INT32 Sec_WimGetDataInfo(WE_HANDLE hSecHandle, WE_UCHAR **ppucData, WE_INT32 *piLen)
{
    St_SecInfo* pstSecInfo = NULL; 
    WE_BOOL bValue = TRUE;
    if((NULL == hSecHandle) ||(NULL == ppucData) || (NULL == piLen))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    pstSecInfo = SEC_SIINFO;
    /*get size*/
    /*modify by bird 060807,for not saving server's certificates to file */
    /*
    Sec_WimInfoGetSize(pstSecInfo,piLen);
    */
    *piLen = sizeof(St_SecInfo);
    *ppucData = WE_MALLOC((WE_UINT32)*piLen);
    if (NULL == *ppucData )
    {
        *piLen  = 0;
        return M_SEC_ERR_INSUFFICIENT_MEMORY;
    }
    (void)WE_MEMSET(*ppucData,0,(WE_UINT32)*piLen);
    
    /*copy data*/
    /*set certificate number is 0,because it does not store is file by bird 061109*/
    bValue = Sec_WimInfoFreeInfo(SEC_SIINFO);
    if(!bValue)
    {
        WE_FREE(*ppucData);
        return M_SEC_ERR_GENERAL_ERROR;
    }

    (void)WE_MEMCPY(*ppucData,pstSecInfo,(WE_UINT32)*piLen);
    /*
    if (!Sec_WimInfoSetData(*ppucData, &SEC_SIINFO))
    return M_SEC_ERR_GENERAL_ERROR;
    */
    return M_SEC_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    Sec_WimStopInfo
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    FREE St_SecInfo
    
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
WE_INT32 Sec_WimStopInfo(WE_HANDLE hSecHandle)
{  
    WE_BOOL bResult = TRUE;
    if(NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    bResult = Sec_WimInfoFreeInfo(SEC_SIINFO);
    if (bResult != TRUE)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    
    return Sec_WimStartInfo(hSecHandle,NULL, 0);
}

/*==================================================================================================
FUNCTION: 
    Sec_WimGetNewSecIdInfo
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    get secret ID
    
ARGUMENTS PASSED:
    WE_HANDLE hSecHandle[IO]: handle of data.
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
WE_INT32 Sec_WimGetNewSecIdInfo(WE_HANDLE hSecHandle)
{
    if(NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    return SEC_SIINFO->iCurSecurityId;
}

/*==================================================================================================
FUNCTION: 
    Sec_WimAddInfo
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
   add new entry to St_SecInfo
    
ARGUMENTS PASSED:
    WE_INT32 iMasterSecretId[IN]:master secret ID
    WE_INT32 iSecurityId[IN]:The security id number
    WE_INT32 iFullHandhake[IN]:if fullhandhake or not
    St_SecSessionInfo *pstFromInfo[IN]:session info data
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
WE_INT32 Sec_WimAddInfo(WE_HANDLE hSecHandle,WE_INT32 iMasterSecretId, WE_INT32 iSecurityId,
                        WE_INT32 iFullHandhake, St_SecSessionInfo *pstFromInfo)
{
    WE_INT32 iSlot = 0;
    St_SecSessionInfo *pstToInfo = NULL;    
    WE_INT32 iIndex = 0;
    WE_LOG_MSG((0,0,"SEC:Sec_WimAddInfo\n"));
    if((NULL == hSecHandle ) || (NULL == pstFromInfo)
        ||(iMasterSecretId >= M_SEC_SESSION_CACHE_SIZE))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    if (iFullHandhake) 
    {
        for (iIndex=0; iIndex<M_SEC_SECURITYID; ++iIndex) 
        {
            if (SEC_SIINFO->astIds[iIndex].ucMasterSecretId == iMasterSecretId) 
            {
                SEC_SIINFO->astIds[iIndex].ucMasterSecretId = 0;
                SEC_SIINFO->astIds[iIndex].iSecurityId = -1;
            }
        }
    }
    
    
    iSlot = M_SEC_IDTOSLOT(iSecurityId);
    SEC_SIINFO->astIds[iSlot].iSecurityId = iSecurityId;
    SEC_SIINFO->astIds[iSlot].ucMasterSecretId = (WE_UINT8) iMasterSecretId;
    
    
    if (iFullHandhake) 
    {
        pstToInfo = &SEC_SIINFO->astInfo[iMasterSecretId];
        
        if (pstToInfo->ucNumCerts > 0 || pstToInfo->pstCerts) 
        {
            for (iIndex=0; iIndex<pstToInfo->ucNumCerts; iIndex++)
            {
                M_SEC_SELF_FREE(pstToInfo->pstCerts[iIndex].pucCert);
            }
            WE_FREE(pstToInfo->pstCerts);
        }
        
        *pstToInfo = *pstFromInfo;
        /* add by Sam [070130] */
        if (pstFromInfo->ucNumCerts > 0)
        {
            pstToInfo->ucNumCerts = pstFromInfo->ucNumCerts;
            pstToInfo->pstCerts = (St_SecAsn1Certificate*)WE_MALLOC(pstToInfo->ucNumCerts*sizeof(St_SecAsn1Certificate));
            if (!pstToInfo->pstCerts)
            {
                return M_SEC_ERR_INSUFFICIENT_MEMORY;
            }
            (WE_VOID)WE_MEMSET(pstToInfo->pstCerts, 0x00, pstToInfo->ucNumCerts*sizeof(St_SecAsn1Certificate));
            for (iIndex = 0; iIndex < pstToInfo->ucNumCerts; iIndex++)
            {
                pstToInfo->pstCerts[iIndex].ucFormat = pstFromInfo->pstCerts[iIndex].ucFormat;
                if (pstFromInfo->pstCerts[iIndex].uiCertLen)
                {
                    pstToInfo->pstCerts[iIndex].uiCertLen = pstFromInfo->pstCerts[iIndex].uiCertLen;
                    pstToInfo->pstCerts[iIndex].pucCert = (WE_UINT8 *)WE_MALLOC(pstToInfo->pstCerts[iIndex].uiCertLen * sizeof(WE_UINT8));
                    if (!pstToInfo->pstCerts[iIndex].pucCert)
                    {
                        WE_INT32 iIndexJ = 0;
                        for (iIndexJ = 0; iIndexJ < iIndex; iIndexJ++)
                        {
                            WE_FREE(pstToInfo->pstCerts[iIndexJ].pucCert);
                        }
                        WE_FREE(pstToInfo->pstCerts);
                        return M_SEC_ERR_INSUFFICIENT_MEMORY;
                    }
                    (WE_VOID)WE_MEMSET(pstToInfo->pstCerts[iIndex].pucCert, 0x00, pstToInfo->pstCerts[iIndex].uiCertLen);
                    (WE_VOID)WE_MEMCPY(pstToInfo->pstCerts[iIndex].pucCert, 
                                       pstFromInfo->pstCerts[iIndex].pucCert, 
                                       pstToInfo->pstCerts[iIndex].uiCertLen);
                }
            }
        }
        pstToInfo->usEncryptionKeyLen *= 8;
        pstToInfo->usKeyExchangeKeyLen *= 8;

        /*delete by birdzhang 061017 for memory leak*/
     /*   
        if (pstFromInfo->ucNumCerts > 0) 
        {
            pstToInfo->pstCerts = WE_MALLOC(sizeof(St_SecAsn1Certificate) *
                pstFromInfo->ucNumCerts);
            if(NULL == pstToInfo->pstCerts)
            {
                return M_SEC_ERR_INSUFFICIENT_MEMORY;
            }
            for (iIndex=0; iIndex<pstFromInfo->ucNumCerts; ++iIndex) 
            {
                pstToInfo->pstCerts[iIndex].ucFormat = pstFromInfo->pstCerts[iIndex].ucFormat;
                pstToInfo->pstCerts[iIndex].uiCertLen = pstFromInfo->pstCerts[iIndex].uiCertLen;
                pstToInfo->pstCerts[iIndex].pucCert = WE_MALLOC(pstFromInfo->pstCerts[iIndex].uiCertLen);
                if (!pstToInfo->pstCerts[iIndex].pucCert)
                {
                    WE_FREE(pstToInfo->pstCerts);
                    return M_SEC_ERR_INSUFFICIENT_MEMORY;
                }
                (void)WE_MEMCPY(pstToInfo->pstCerts[iIndex].pucCert, pstFromInfo->pstCerts[iIndex].pucCert, pstFromInfo->pstCerts[iIndex].uiCertLen);
            }
        }
     */
    }
    
    
    ++SEC_SIINFO->iCurSecurityId;
    if (SEC_SIINFO->iCurSecurityId < 0)
    {
        SEC_SIINFO->iCurSecurityId = 0;
    }    
    return M_SEC_ERR_OK;
}

/*==================================================================================================
FUNCTION: 
    Sec_WimGetInfo
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    get session info data ,according security ID
    
ARGUMENTS PASSED:
    WE_INT32 iSecurityId[IN]:The security id number
    St_SecSessionInfo **ppstInfo[out]:session info data
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
WE_INT32 Sec_WimGetInfo(WE_HANDLE hSecHandle,WE_INT32 iSecurityId, St_SecSessionInfo **ppstInfo)
{
    WE_UINT8 ucMasterSecretId = 0;
    WE_INT32 iResult = 0;
    if((NULL == hSecHandle) || (NULL == ppstInfo))
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    iResult = Sec_WimInfoGetMasterSecretId(hSecHandle,iSecurityId, &ucMasterSecretId);
    
    if (iResult == M_SEC_ERR_OK)
    {
        *ppstInfo = &(SEC_SIINFO->astInfo[ucMasterSecretId]);
    }
    else
    {
        *ppstInfo = NULL;
    }    
    return iResult;
}

/*==================================================================================================
FUNCTION: 
    Sec_WimInfoFreeInfo
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    free memroy
    
ARGUMENTS PASSED:
    St_SecInfo *pstSecInfo[IN]:St_SecInfo structure
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
static WE_BOOL Sec_WimInfoFreeInfo(St_SecInfo *pstSecInfo)
{
    WE_INT32 iIndexI = 0;
    WE_INT8  cNumCerts = 0;
    if(NULL == pstSecInfo)
    {
        return FALSE;
    }
    /*2*/
    /*astInfo*/
    /*modify by Bird 061109*/
    for(iIndexI = 0; iIndexI < M_SEC_SESSION_CACHE_SIZE; iIndexI++)
    {
        /*have certificate or not?*/
        cNumCerts = (WE_INT8)(pstSecInfo->astInfo[iIndexI].ucNumCerts);
        if(cNumCerts > 0)
        {
            /*delete by bird 070126*/
            /* add by Sam [070130] */
            WE_INT iIndexJ = 0;
            for(iIndexJ = 0; iIndexJ < cNumCerts; iIndexJ++)
            {
                if(NULL != pstSecInfo->astInfo[iIndexI].pstCerts[iIndexJ].pucCert)
                {
                    WE_FREE(pstSecInfo->astInfo[iIndexI].pstCerts[iIndexJ].pucCert);
                }
                pstSecInfo->astInfo[iIndexI].pstCerts[iIndexJ].pucCert = NULL;
            }
            WE_FREE(pstSecInfo->astInfo[iIndexI].pstCerts);
            pstSecInfo->astInfo[iIndexI].pstCerts = NULL;
        }
        pstSecInfo->astInfo[iIndexI].ucNumCerts = 0;     
    }
    return TRUE;
  
}

#if(M_SEC_SAVE_SERVERCERT == 1)
/************************************************************************
*  sec info data storage
*
*  _ _ _ _ _ _ _ _ _ _ _ __ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ __ _ _ _ _ _ _ _ _ _ _ _
*  |sizeof(St_SecInfo) |+ | certNum*(sizeof(St_SecAsn1Certificate) + certNameLen)  |
*  |-------------------------------------------------------------------------
*  
*  sizeof(St_SecSecurityIds)
*
* by BirdZhang 06/07/12
*
*************************************************************************/
/*==================================================================================================
FUNCTION: 
    Sec_WimInfoGetInfo
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    convert data from file to St_SecInfo structure
    
ARGUMENTS PASSED:
    St_SecInfo *pstSecInfo[OUT]:St_SecInfo structure
    WE_UCHAR * pucData[IN]:source data
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

static WE_BOOL
Sec_WimInfoGetInfo(St_SecInfo *pstSecInfo,WE_UCHAR * pucData)
{

    WE_INT32 iIndexI = 0;
    WE_INT32 iIndexJ = 0;
    WE_INT32 iIndexK = 0;
    WE_INT32 iLen = 0;
    WE_INT8 cNumCerts = 0;
    WE_INT8 cCertLen = 0;
    void*   pvData = NULL;
    WE_UCHAR* pucTempData = NULL;

    
    if((NULL == pucData) || (NULL == pstSecInfo))
    {
        return FALSE;
    }
    
    pucTempData = pucData;
    /*1*/
    pstSecInfo->iCurSecurityId = ((St_SecInfo*)pucTempData)->iCurSecurityId;
    pucTempData += sizeof(pstSecInfo->iCurSecurityId);

    /*2*/
    /*astInfo*/
    for(iIndexI = 0; iIndexI < M_SEC_SESSION_CACHE_SIZE; iIndexI++)
    {
        /*2.1*/
        pstSecInfo->astInfo[iIndexI].ucConnectionType = ((St_SecSessionInfo*)pucTempData)->ucConnectionType;
        pstSecInfo->astInfo[iIndexI].ucSecurityClass = ((St_SecSessionInfo*)pucTempData)->ucSecurityClass;
        pstSecInfo->astInfo[iIndexI].ucKeyExchangeAlg = ((St_SecSessionInfo*)pucTempData)->ucKeyExchangeAlg;
        pstSecInfo->astInfo[iIndexI].usKeyExchangeKeyLen = ((St_SecSessionInfo*)pucTempData)->usKeyExchangeKeyLen;
        pstSecInfo->astInfo[iIndexI].ucHmacAlg = ((St_SecSessionInfo*)pucTempData)->ucHmacAlg;
        pstSecInfo->astInfo[iIndexI].usHmacLen = ((St_SecSessionInfo*)pucTempData)->usHmacLen;
        pstSecInfo->astInfo[iIndexI].ucEncryptionAlg = ((St_SecSessionInfo*)pucTempData)->ucEncryptionAlg;
        pstSecInfo->astInfo[iIndexI].usEncryptionKeyLen =((St_SecSessionInfo*)pucTempData)->usEncryptionKeyLen;

        /*certificate*/
        pstSecInfo->astInfo[iIndexI].ucNumCerts = ((St_SecSessionInfo*)pucTempData)->ucNumCerts;
        cNumCerts = (WE_INT8)pstSecInfo->astInfo[iIndexI].ucNumCerts;
        iLen = sizeof(St_SecAsn1Certificate) * cNumCerts;
        
        pucTempData += sizeof(St_SecSessionInfo);

        /*malloc memory*/
        if(0 != iLen && (NULL ==(pvData = (void*)WE_MALLOC((WE_UINT32)iLen))))     
        {
            for(iIndexK = iIndexI -1; iIndexK >=0; iIndexK --)
            {
                WE_FREE(pstSecInfo->astInfo[iIndexK].pstCerts);
            }
            return FALSE;
        }
        pstSecInfo->astInfo[iIndexI].pstCerts = (St_SecAsn1Certificate*)pvData;

        /* sizeof(St_SecInfo) +  certNum*(sizeof(St_SecAsn1Certificate) + certNameLen) */
        /*2.2*/
        for(iIndexJ = 0; iIndexJ < cNumCerts; iIndexJ++)
        {
            pstSecInfo->astInfo[iIndexI].pstCerts[iIndexJ].ucFormat = 
                                    ((St_SecAsn1Certificate*)pucTempData)->ucFormat ;
            pstSecInfo->astInfo[iIndexI].pstCerts[iIndexJ].uiCertLen = 
                                    ((St_SecAsn1Certificate*)pucTempData )->uiCertLen;
            
            cCertLen = (WE_INT8)(pstSecInfo->astInfo[iIndexI].pstCerts[iIndexJ].uiCertLen);

             pucTempData  += sizeof(St_SecAsn1Certificate);
            /*cert name*/
            if((0!=cCertLen) && (NULL == (pvData = (void*)WE_MALLOC((WE_UINT32)cCertLen))))
            {
                for(iIndexK = iIndexI; iIndexK >=0; iIndexK --)
                {
                    WE_FREE(pstSecInfo->astInfo[iIndexK].pstCerts);
                }
                return FALSE;
            }                
            pstSecInfo->astInfo[iIndexI].pstCerts[iIndexJ].pucCert = (WE_UCHAR*)pvData;

            /*2.3*/
            (void)WE_MEMCPY(pstSecInfo->astInfo[iIndexI].pstCerts[iIndexJ].pucCert,
                            pucTempData,cCertLen);
            pucTempData  +=  cCertLen;
        }            
    /* pstTempInfo = (St_SecInfo*)pucTempData;*/
    }

    /*3*/
    /*astIds*/
    for(iIndexI = 0 ;iIndexI < M_SEC_SECURITYID; iIndexI ++)
    {
        pstSecInfo->astIds[iIndexI].iSecurityId =  ((St_SecSecurityIds*)pucTempData)->iSecurityId; 
        pstSecInfo->astIds[iIndexI].ucMasterSecretId =((St_SecSecurityIds*)pucTempData)->ucMasterSecretId;
        
        pucTempData += sizeof(St_SecSecurityIds); 
        /* pstTempInfo = (St_SecInfo*)pucTempData; */   
    }
    return TRUE;
}
    
/*==================================================================================================
FUNCTION: 
    Sec_WimInfoSetData
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    convert St_SecInfo structure to data that can be save to file
    
ARGUMENTS PASSED:
    WE_UCHAR * pucData[OUT]: data for save to file
    St_SecInfo *pstSecInfo[IN]:St_SecInfo structure

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
static WE_BOOL Sec_WimInfoSetData(WE_UCHAR*pucData, St_SecInfo *pstSecInfo)
{
    WE_INT32 iIndexI = 0;
    WE_INT32 iIndexJ = 0;
    WE_INT8 cNumCerts = 0;
    WE_INT8 cCertLen = 0;

    WE_UCHAR* pucTempData = pucData;
/*    St_SecInfo* pstTempInfo = (St_SecInfo*)pucTempData;*/

    if((NULL == pucData) || (NULL == pstSecInfo))
    {
        return FALSE;
    }
    /*1*/
    ((St_SecInfo*)pucTempData)->iCurSecurityId = pstSecInfo->iCurSecurityId;
     pucTempData += sizeof(pstSecInfo->iCurSecurityId);

    /*2*/
    /*astInfo*/
     for(iIndexI = 0; iIndexI < M_SEC_SESSION_CACHE_SIZE; iIndexI++)
     {
         /*2.1*/
         ((St_SecSessionInfo*)pucTempData)->ucConnectionType =pstSecInfo->astInfo[iIndexI].ucConnectionType;
         ((St_SecSessionInfo*)pucTempData)->ucSecurityClass = pstSecInfo->astInfo[iIndexI].ucSecurityClass;
         ((St_SecSessionInfo*)pucTempData)->ucKeyExchangeAlg = pstSecInfo->astInfo[iIndexI].ucKeyExchangeAlg ;
         ((St_SecSessionInfo*)pucTempData)->usKeyExchangeKeyLen = pstSecInfo->astInfo[iIndexI].usKeyExchangeKeyLen ;
         ((St_SecSessionInfo*)pucTempData)->ucHmacAlg = pstSecInfo->astInfo[iIndexI].ucHmacAlg  ;
         ((St_SecSessionInfo*)pucTempData)->usHmacLen = pstSecInfo->astInfo[iIndexI].usHmacLen  ;
         ((St_SecSessionInfo*)pucTempData)->ucEncryptionAlg = pstSecInfo->astInfo[iIndexI].ucEncryptionAlg ;
         ((St_SecSessionInfo*)pucTempData)->usEncryptionKeyLen = pstSecInfo->astInfo[iIndexI].usEncryptionKeyLen;
         
         /*certificate*/
         ((St_SecSessionInfo*)pucTempData)->ucNumCerts = pstSecInfo->astInfo[iIndexI].ucNumCerts;
         cNumCerts =  pstSecInfo->astInfo[iIndexI].ucNumCerts;
         
         /*  
         *sizeof(St_SecInfo) +  certNum*(sizeof(St_SecAsn1Certificate) + certNameLen)
         */
         /*2.2*/
         pucTempData += sizeof(St_SecSessionInfo);
         for(iIndexJ = 0; iIndexJ < cNumCerts; iIndexJ++)
         {
             ((St_SecAsn1Certificate*)pucTempData)->ucFormat =
                            pstSecInfo->astInfo[iIndexI].pstCerts[iIndexJ].ucFormat ;
             ((St_SecAsn1Certificate*)pucTempData )->uiCertLen =
                            pstSecInfo->astInfo[iIndexI].pstCerts[iIndexJ].uiCertLen ;
             
             cCertLen = pstSecInfo->astInfo[iIndexI].pstCerts[iIndexJ].uiCertLen;
             
             
             /*2.3*/
             pucTempData  += sizeof(St_SecAsn1Certificate);
             (void)WE_MEMCPY(pucTempData,
                             pstSecInfo->astInfo[iIndexI].pstCerts[iIndexJ].pucCert,
                             cCertLen);
             pucTempData  +=  cCertLen;
         }
         
/*         pstTempInfo = (St_SecInfo*)pucTempData;*/
         
     }

    /*3*/
    /*astIds*/
    for(iIndexI = 0 ;iIndexI < M_SEC_SECURITYID; iIndexI ++)
    {
        ((St_SecSecurityIds*)pucTempData)->iSecurityId = pstSecInfo->astIds[iIndexI].iSecurityId ; 
        ((St_SecSecurityIds*)pucTempData)->ucMasterSecretId = pstSecInfo->astIds[iIndexI].ucMasterSecretId ;
        
        pucTempData += sizeof(St_SecSecurityIds); 
       /*pstTempInfo = (St_SecInfo*)pucTempData;*/        
    }
    return TRUE;

    
}

/*==================================================================================================
FUNCTION: 
    Sec_WimInfoGetSize
CREATE DATE:
    2006-07-22
AUTHOR:
    Bird Zhang
DESCRIPTION: 
    get St_SecInfo size
    
ARGUMENTS PASSED:
    St_SecInfo *pstSecInfo[IN]:St_SecInfo data
    WE_INT32 * piLen[out]:length
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

static WE_BOOL 
Sec_WimInfoGetSize(St_SecInfo *pstSecInfo,WE_INT32 * piLen)
{
    WE_INT32 iIndexI = 0;
    WE_INT32 iIndexJ = 0;
    WE_INT32 iLen = 0;
    WE_INT8 cNumCerts = 0;
    WE_INT8 cCertLen = 0;

    if(NULL == piLen)
    {
        return FALSE;
    }
    if(NULL == pstSecInfo)
    {
        *piLen = 0; 
        return FALSE;
    }
    
    for(iIndexI = 0; iIndexI < M_SEC_SESSION_CACHE_SIZE; iIndexI++)
    {
        cNumCerts =  pstSecInfo->astInfo[iIndexI].ucNumCerts;
        
        iLen += sizeof(St_SecSessionInfo);
        for(iIndexJ = 0; iIndexJ < cNumCerts; iIndexJ++)
        {
            
            cCertLen = pstSecInfo->astInfo[iIndexI].pstCerts[iIndexJ].uiCertLen;
            iLen += sizeof(St_SecAsn1Certificate);
            iLen += cCertLen;
            
        }
        
    }
    
    iLen += M_SEC_SECURITYID * sizeof(St_SecSecurityIds);    
    *piLen = iLen;
    return TRUE;
}

#endif
