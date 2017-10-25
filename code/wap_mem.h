/*=================================================================================
HEADER NAME : wap_mem.h
MODULE NAME : WAP
PRE-INCLUDE FILES DESCRIPTION : 
GENERAL DESCRIPTION : 
this file includes some selfdefined memory operation function. 
TECHFAITH Wireless Confidential Proprietary
(c) Copyright 2002 by TECHFAITH Wireless. All Rights Reserved.
===================================================================================
Revision History Modification Tracking
Date Author Number Description of changes
---------- ------------ -----------------------------------------------------
2006-10-20 Zhongbin.Xiong, create
2006-11-16 ZHongbin.Xiong, add strdup and stat. peak value
==================================================================================*/
#ifndef _WAP_MEM_HEADER_FILE_
#define _WAP_MEM_HEADER_FILE_
/*******************************************************************************
* Include File Section
*******************************************************************************/
#include "we_def.h"
#ifndef AEE_SIMULATOR
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
#else
    #include <AEEStdLib.h>
#endif /* AEE_SIMULATOR */

/*******************************************************************************
* Macro Define Section
*******************************************************************************/
#ifndef AEE_SIMULATOR
    #define MALLOC      malloc
    #define FREE        free
    #define MEMSET      memset
    #define MEMCPY      memcpy
    #define MEMCMP      memcmp
    #define WE_LOGINFO  printf
    #define STRRCHR     strrchr
    #define STRLEN      strlen
#endif /* AEE_SIMULATOR */

#ifndef WE_LOGINFO
#define WE_LOGINFO  DBGPRINTF
#endif
/* a macro to Allocates memory blocks. */
#define WAP_MALLOC(size)    WAP_MemMalloc(__FILE__, __LINE__, (size))
/* a macro to free memory blocks. */
#define WAP_FREE(p)   WAP_MemFree(__FILE__, __LINE__, (p))
/* a macro to Compare characters in two buffers. */
#define WAP_MEMCMP(p1, p2, size)   WAP_MemCmp(__FILE__, __LINE__, (p1), (p2), (size))
/* a macro to Copies characters between buffers. */
#define WAP_MEMCPY(dest, src, count) \
                WAP_MemTestbourn(__FILE__, __LINE__, (dest), (count));\
                WAP_MemTestbourn(__FILE__, __LINE__, (src), (count));\
                MEMCPY((dest), (src), (count));
/* a macro to Sets buffers to a specified character. */
#define WAP_MEMSET(dest, character, count) \
                WAP_MemTestbourn(__FILE__, __LINE__, (dest), (count));\
                MEMSET((dest), (character), (count));
/* a macro to Moves one buffer to another. */
#define WAP_MEMMOVE(dest, src, count) \
                WAP_MemTestbourn(__FILE__, __LINE__, (dest), (count));\
                WAP_MemTestbourn(__FILE__, __LINE__, (src), (count));\
                MEMMOVE((dest), (src), (count));

/* Duplicate a string*/
#define WAP_STRDUP(p)       WAP_Strndup(__FILE__, __LINE__, (p), STRLEN( (p) ) )
/* Duplicate characters of a string*/
#define WAP_STRNDUP(p, n)   WAP_Strndup(__FILE__, __LINE__, (p), (n))

/* a macro to print memory leaked*/
#define WAP_MEMPRINTMEMLEAKED() WAP_MemPrintMemLeaked()

#ifdef __cplusplus
extern "C"
{ 
#endif
    /*******************************************************************************
    * Prototype Declare Section
    *******************************************************************************/
    /* Allocates memory blocks*/
    WE_VOID *WAP_MemMalloc(const  WE_CHAR *pcFileName, WE_INT32 iLine, WE_INT32 iSize);
    /* free memory blocks*/
    WE_VOID WAP_MemFree(const WE_CHAR *pcFileName, WE_INT32 iLine, WE_VOID *pstMemBlock);
    /* Compare characters in two buffers*/
    WE_INT32 WAP_MemCmp(const  WE_CHAR *pcFileName, WE_INT32 iLine, 
        const WE_VOID *pBuf1, const WE_VOID *pBuf2, WE_UINT32 iSize);
    /* test the pointer is out of bourn or not*/
    WE_INT32 WAP_MemTestbourn(const WE_CHAR *pcFileName, WE_INT32 iLine, 
        const WE_VOID *pvDest, WE_UINT32 iSize);
    /* Duplicate characters of a string*/
    WE_CHAR *WAP_Strndup(const WE_CHAR *pcFileName, WE_INT32 iLine,
                     const WE_CHAR *pcString, WE_INT iCount);
    /* print memory leaked*/
    WE_VOID WAP_MemPrintMemLeaked( );
#ifdef __cplusplus
}
#endif
#endif /* _WAP_MEM_HEADER_FILE_ */
