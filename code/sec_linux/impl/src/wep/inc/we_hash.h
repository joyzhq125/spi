/*==================================================================================================
    HEADER NAME : we_hash.h
    MODULE NAME : WE

    PRE-INCLUDE FILES DESCRIPTION
        
    GENERAL DESCRIPTION
        In this file,define the struct and function prototype.
        
    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2002-2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
    
    Modification              Tracking
    Date            Author      Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-11-03       Sam         none      Init
  
==================================================================================================*/
/***************************************************************************************************
*   Multi-Include-Prevent Section
***************************************************************************************************/
#ifndef _WE_HASH_H 
#define _WE_HASH_H


#define M_HASH_BLOCK_SIZE   64
/***************************************************************************************************
*   Type Define Section
***************************************************************************************************/
/* structure of hash result. */
typedef struct tagSt_WeHashValue
{
    WE_UINT32   uiSize;
    WE_UINT8    *pucData;
}St_WeHashValue;

typedef struct tagSt_WeLibHashDigest
{
    WE_UINT8    aucHashValue[20];
    WE_UINT32   uiHashSize;
}St_WeLibHashDigest;

/***************************************************************************************************
*   Prototype Declare Section
***************************************************************************************************/
/************************************************************************
*   HASH-SHA
************************************************************************/
WE_UINT32 We_LibHashStartSha(WE_HANDLE *phHandle);

WE_UINT32 We_LibHashAppendSha
(
    WE_UINT8    *pucMessage, 
    WE_UINT32   uiNBits, 
    WE_VOID*    pvHashID
);

WE_UINT32 We_LibHashFinishSha( WE_VOID* pvHashID, St_WeLibHashDigest *pstHashDigest);

/************************************************************************
*   HASH-MD2
************************************************************************/
WE_UINT32 We_LibHashStartMd2(WE_HANDLE *hHandle);

WE_UINT32 We_LibHashAppendMd2
(
    WE_UINT8    *pucData, 
    WE_UINT32   uiNbits, 
    WE_VOID     *pvEid
);

WE_UINT32 We_LibHashFinishMd2(WE_VOID *pvEid, St_WeLibHashDigest *pstHashDigest);


/************************************************************************
*   HASH-MD5
************************************************************************/
WE_UINT32 We_LibHashStartMd5(WE_HANDLE *hHandle);

WE_UINT32 We_LibHashAppendMd5
(
    WE_UINT8    *pucMessage, 
    WE_UINT32   uiNBits, 
    WE_VOID     *pvHashID
);

WE_UINT32 We_LibHashFinishMd5( WE_VOID* pvHashID, St_WeLibHashDigest *pstHashDigest);


/************************************************************************
*   HMAC-MD2
************************************************************************/
WE_UINT32 We_LibHashStartHmacMd2
(
    const WE_UINT8    *pucKey, 
    WE_UINT32   uiKeyLen, 
    WE_HANDLE   *hHMACHandle
);


WE_UINT32 We_LibHashAppendHmacMd2
(
    WE_UINT8    *pucData, 
    WE_UINT32   uiDataLengthInBits, 
    WE_VOID     *pvHid
);

WE_UINT32 We_LibHashFinishHmacMd2( WE_VOID * pvHid, St_WeLibHashDigest *pstHashDigest);


/************************************************************************
*   HMAC-MD5
************************************************************************/
WE_UINT32 We_LibHashStartHmacMd5
(
    const WE_UINT8    *pucKey, 
    WE_UINT32   uiKeyLen, 
    WE_HANDLE   *hHMACHandle
);

WE_UINT32 We_LibHashAppendHmacMd5
(
    WE_UINT8    *pucData, 
    WE_UINT32   uiDataLengthInBits, 
    WE_VOID     *pvHid
);

WE_UINT32 We_LibHashFinishHmacMd5( WE_VOID * pvHid, St_WeLibHashDigest *pstHashDigest);


/************************************************************************
*   HMAC-SHA
************************************************************************/
WE_UINT32  We_LibHashStartHmacSha
( 
    const WE_UINT8   *pucKey, 
    WE_UINT32  uiKeyLen, 
    WE_HANDLE  *hHMACHandle
);

WE_UINT32 We_LibHashAppendHmacSha
(
    WE_UINT8    *pucData, 
    WE_UINT32   uiDataLengthInBits, 
    WE_VOID     *pvHid
);

WE_UINT32 We_LibHashFinishHmacSha( WE_VOID * pvHid, St_WeLibHashDigest *pstHashDigest );



void We_LibRemoveHmac( WE_VOID * pvHmacid );


#endif /* endif WE_LIBHASH_H*/
