/*=====================================================================================
    FILE NAME : oem_sechandle.h
    MODULE NAME : SEC
    
    GENERAL DESCRIPTION
        This file declare structure used in file module.

    TECHFAITH Software Confidential Proprietary(c)
        Copyright 2006 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
    
    Modification Tracking
    Date       Author         Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-06-01 Alan              none        Initialization
    
=====================================================================================*/

/*******************************************************************************
*   multi-Include-Prevent Section
*******************************************************************************/
#ifndef __OEM_SECHANDLE_H__
#define __OEM_SECHANDLE_H__

/*******************************************************************************
*   include File Section
*******************************************************************************/
#include "oem_secmgr.h"
#include "AEEShell.h"
#include "AEEFile.h"
#include "AEENet.h"
#include "AEEFIFO.h"

/*******************************************************************************
*   macro Define Section
*******************************************************************************/
#define WE_HANDLE_MAGIC_NUM         0x34322423
#define WE_FILE_HANDLE_MAGIC_NUM    0x499602D2
#define WE_SOCKET_HANDLE_MAGIC_NUM  0x04303029
#define WE_TIMER_HANDLE_MAGIC_NUM   0x3ADE68B1
#define WE_PIPE_HANDLE_MAGIC_NUM    0x49631902

#define OPEN_FILE_MAX_NUM           20
#define OPEN_SOCKET_MAX_NUM         16
#define OPEN_TIMER_MAX_NUM          20
#define OPEN_PIPE_MAX_NUM           20

/*******************************************************************************
*   Type Define Section
*******************************************************************************/
typedef struct tagSt_WeFileHandle
{
    WE_UINT32   iMagicNum;
    WE_HANDLE   hWeHandle;    
    WE_INT8     iFileHandleID;
    IFile       *pIFile;
}St_WeFileHandle;


typedef struct tagSt_WeHandle
{
    WE_UINT32   iMagicNum;
    IShell      *pIShell;
    IModule     *pIModule;
    IFileMgr    *pIFileMgr;

    Fn_WeCallback cbWeCallback;
    void *pvPrivData;

    WE_HANDLE aphFileList[OPEN_FILE_MAX_NUM];

}St_WeHandle;

#endif /* __WE_HANDLE_H__ */


