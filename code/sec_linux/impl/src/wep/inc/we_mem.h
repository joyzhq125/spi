/*=====================================================================================

    HEADER NAME : we_mem.h
    MODULE NAME : WE Framework Memory Interface Define

    PRE-INCLUDE FILES DESCRIPTION

    GENERAL DESCRIPTION
        This file can be included by all WE Programmer.In this file all Memory-Interface
    is defined.
        These Info is  aptotic and is not changed by switching Project.

    TECHFAITH Wireless Confidential Proprietary
    (c) Copyright 2006 by TechSoft. All Rights Reserved.
=======================================================================================
    Revision History
   
    Modification                 Tracking
    Date           Author         Number        Description of changes
    ----------   ------------    ---------   --------------------------------------------


=====================================================================================*/


/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/
#ifndef WE_MEM_H
#define WE_MEM_H

/*******************************************************************************
*   Include File Section
*******************************************************************************/
/*Include System head file*/
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include <string.h>     /* memset */
/*Include Program Global head file*/

/*Include Module head file*/

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
#define WE_SPRINTF     sprintf
#define WE_MALLOC(len)  memset(malloc(len), 0, len)
#define WE_FREE(p)      {\
                           if (p)\
                           {\
                               free(p);\
                               p = NULL;\
                           }\
                       }
#define WE_MEMCHR      memchr
#define WE_MEMCPY      memcpy
#define WE_MEMSET      memset
#define WE_MEMMOVE     memmove
#define WE_MEMCMP      memcmp
#define WE_STRCAT      strcat
#define WE_STRRCHR     strrchr
#define WE_STRCHR      strchr
#define WE_STRCPY      strcpy
#define WE_STRNCPYLC   strncpy
#define WE_STRCMPNC    strcmp
#define WE_STRNCMPNC   strncmp
#define WE_STRDUP(src)   (NULL == src ? NULL : strdup(src))
#define WE_STRLEN      strlen
#define WE_ATOI        atoi
#define WE_STRSTR      strstr
#define WE_STRTOL      strtol
#define WE_STRNCPY     strncpy
#define WE_STRTOUL     strtoul
#define WE_STRLOWER    tolower
#define WE_ATOL        atol


//WE_VOID *We_Malloc(WE_INT32 iLen);
/*Conditional Compilation Directives start*/
/*Conditional Compilation Directives end*/

/*Define Constant Macro start*/
/*Define Constant Macro end*/

/*Define Macro Function start*/



/*******************************************************************************
*   Type Define Section
*******************************************************************************/

/*Define base type start*/
/*Define base type end*/

/*Define Enum type start*/
/*Define Enum type end*/

/*Define function type start*/
/*Define function type end*/

/*Define struct &  union type start*/
/*Define struct &  union type end*/

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/

#endif/*endif WE_MEM_H*/
/*--------------------------END-----------------------------*/
