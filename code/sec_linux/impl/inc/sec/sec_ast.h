/*==================================================================================================
    HEADER NAME : sec_ast.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION    
    
    GENERAL DESCRIPTION
        In this file,define the string function prototype.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-07-07   Kevin Yang        None         Draft
==================================================================================================*/

/*******************************************************************************
*   multi-Include-Prevent Section
*******************************************************************************/
#ifndef _SEC_AST_H
#define _SEC_AST_H


/*******************************************************************************
*   macro Define Section
*******************************************************************************/
#define WE_SIGNAL_DESTRUCT      Sec_FreeMsg
#define SEC_CURRENT_TIME        Sec_TimeGetCurrent
/*
#define BREW_PLATFORM
*/
#ifdef BREW_PLATFORM
#include "aeestdlib.h"

#define SEC_SPRINTF     SPRINTF
#define SEC_STRCAT      STRCAT
#define SEC_STRRCHR     STRRCHR
#define SEC_STRCHR      STRCHR
#define SEC_STRCPY      STRCPY
#define SEC_STRDUP      STRDUP
#define SEC_STRLEN      STRLEN
#define SEC_ATOI        ATOI
#define SEC_STRSTR      STRSTR
#define SEC_STRTOL      STRTOUL
#define SEC_STRCMP      STRCMP
#define SEC_STRNCMP     STRNCMP
#define SEC_STRNCPY     STRNCPY

#else
#include "stdlib.h"

#define SEC_SPRINTF     sprintf
#define SEC_STRCAT      strcat
#define SEC_STRRCHR     strrchr
#define SEC_STRCHR      strchr
#define SEC_STRCPY      strcpy
#define SEC_STRDUP      strdup
#define SEC_STRLEN      strlen
#define SEC_ATOI        atoi
#define SEC_STRSTR      strstr
#define SEC_STRTOL      strtol
#define SEC_STRCMP      strcmp
#define SEC_STRNCMP     strncmp
#define SEC_STRNCPY     strncpy

#endif

/*******************************************************************************
*   Type Define Section
*******************************************************************************/

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
void Sec_FreeMsg
(
    WE_UINT8    ucID, 
    WE_INT32    iMsg, 
    void        *pData
);

WE_UINT32 Sec_TimeGetCurrent(void);



#endif
