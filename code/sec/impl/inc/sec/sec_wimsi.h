/*=====================================================================================
    FILE NAME : sec_wimsi.h
    MODULE NAME : SEC
    
    GENERAL DESCRIPTION
        In this file, define the structure and functions used in the SEC module.
        
    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2002-2006 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
    
    Modification Tracking
    Date       Author         Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-07-22 Bird             none      Init
    
=====================================================================================*/

/***************************************************************************************************
*   multi-Include-Prevent Section
***************************************************************************************************/
#ifndef SEC_WIMSI_H
#define SEC_WIMSI_H

/***************************************************************************************************
*   macro Define Section
***************************************************************************************************/
#define M_SEC_SECURITYID (5 * M_SEC_SESSION_CACHE_SIZE)
#define M_SEC_IDTOSLOT(id) (id % M_SEC_SECURITYID)

/***************************************************************************************************
*   Type Define Section
***************************************************************************************************/
typedef struct tagSt_SecSecurityIds
{
    WE_INT32    iSecurityId;
    WE_UINT8    ucMasterSecretId;
} St_SecSecurityIds;

typedef struct tagSt_SecInfo
{
    WE_INT32            iCurSecurityId;
    St_SecSessionInfo   astInfo[M_SEC_SESSION_CACHE_SIZE];
    St_SecSecurityIds   astIds[M_SEC_SECURITYID];
} St_SecInfo;


/***************************************************************************************************
*   Prototype Declare Section
***************************************************************************************************/
WE_INT32 Sec_WimStartInfo(WE_HANDLE hSecHandle,WE_UCHAR *pucData, WE_INT32 iLen);

WE_INT32 Sec_WimStopInfo(WE_HANDLE hSecHandle);

WE_INT32 Sec_WimGetNewSecIdInfo(WE_HANDLE hSecHandle);

WE_INT32 Sec_WimAddInfo(WE_HANDLE hSecHandle,WE_INT32 iMasterSecretId, WE_INT32 iSecurityId,
                        WE_INT32 iFullHandhake, St_SecSessionInfo *pstFromInfo);

WE_INT32 Sec_WimGetInfo(WE_HANDLE hSecHandle,WE_INT32 iSecurityId, St_SecSessionInfo **ppstInfo);

WE_INT32 Sec_WimGetDataInfo(WE_HANDLE hSecHandle,WE_UCHAR **ppucData, WE_INT32 *piLen);

#endif

