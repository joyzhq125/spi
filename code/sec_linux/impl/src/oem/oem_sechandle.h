/*==================================================================================================
    FILE NAME:
        oem_sechandle.h
    MODULE NAME:
        iwap
    GENERAL DESCRIPTION        
        In this file,define the function prototype         
    Techfaith Software Confidential Proprietary.
    (c) Copyright 2006 by Techfaith Software. All Rights Reserved.
====================================================================================================
    Revision History

    Modification Tracking
    Date             Author               Number       Description of changes
    --------------------------------------------------------------------------------
    2007-03-12  Zhanghuanqing   None         Initialize   
==================================================================================================*/

/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/

#ifndef __WE_HANDLE_H__
#define __WE_HANDLE_H__

/***************************************************************************************
    *   Include File Section
****************************************************************************************/
#include "oem_secfile.h"
#include "oem_secmgr.h"

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
#define WE_HANDLE_MAGIC_NUM             0x34322423
#define WE_FILE_HANDLE_MAGIC_NUM        0x499602D2
#define WE_SOCKET_HANDLE_MAGIC_NUM      0x04303029
#define WE_PIPE_HANDLE_MAGIC_NUM        0x49631902

#define OPEN_FILE_MAX_NUM               30
#define OPEN_SOCKET_MAX_NUM             16
#define OPEN_PIPE_MAX_NUM               40

#ifdef ISec_BREW_PLATFROM
#undef ISec_BREW_PLATFROM
#endif

/*******************************************************************************
*   Type Define Section
*******************************************************************************/
typedef struct tagSt_WeFileHandle
{
    WE_INT          iFd;      
    WE_INT32        iMagicNum;
    WE_HANDLE       hWeHandle;    
    WE_INT32        iFileHandleID;
}St_WeFileHandle;

typedef struct tagSt_WeHandle
{    
    WE_INT32            iMagicNum;    
    Fn_WeCallback       cbWeCallback;    
#ifdef WE_ALG_SEC
    Fn_ISecEventHandle  cbSecCallback;
#endif
    void                *pvPrivData;
    /*---------Socket struct------------------------*/
    WE_CHAR             *pcLocalAddr;
    WE_HANDLE           aphWeSocketInfo[OPEN_SOCKET_MAX_NUM];    
    WE_HANDLE           aphFileList[OPEN_FILE_MAX_NUM];
    WE_HANDLE           hTimerHandle;
    WE_HANDLE           aphPipeList[OPEN_PIPE_MAX_NUM];
   /*-----------------------TLS------------------------------*/
    WE_HANDLE           aphWeSecSocketInfo[OPEN_SOCKET_MAX_NUM];
#ifdef WE_ALG_SEC
    WE_HANDLE           hAlgHandle;
    WE_INT32            iAlgRefs;
    WE_HANDLE           hSecHandle;
    WE_INT32            iSecRefs;
#endif
}St_WeHandle;

#endif /* __WE_HANDLE_H__ */


