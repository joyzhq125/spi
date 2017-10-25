/*==================================================================================================
    HEADER NAME : sec_main.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
    
    
    GENERAL DESCRIPTION
        In this file,define the main function of sec module.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-07-07   Kevin Yang        None         Draft
==================================================================================================*/

#ifndef SEC_MAIN_H
#define SEC_MAIN_H

WE_INT32 Sec_StartEx(WE_HANDLE hSecHandle);
WE_UINT32 Sec_StopEx(WE_HANDLE hSecHandle);

void Sec_RunMainFlow(WE_HANDLE hHandle);
WE_INT32 Sec_PostMsg(WE_HANDLE hSecHandle, WE_INT32 iEvent, WE_HANDLE hData);

#endif
