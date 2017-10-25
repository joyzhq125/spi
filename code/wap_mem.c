/*=====================================================================================
FILE NAME  :wap_mem.c
MODULE NAME:WAP_MEM
GENERAL DESCRIPTION : implement memory functions be selfdefined
TECHFAITH Software Confidential Proprietary(c) Copyright 2006 by TECHFAITH Software. 
All Rights Reserved.
=======================================================================================
Revision History Modification Tracking
Date       Author         Number    Description of changes
---------- -------------- --------- --------------------------------------
2006-05-24 ChenZhifeng    none      Init
2006-08-02 ChenZhifeng    none      Add MEM_TEST   
2006-11-08 Zhongbin.Xiong none      clean up
2006-11-16 ZHongbin.Xiong none      add strdup and stat. peak value
=====================================================================================*/

/**************************************************************************************
*   Include File Section
**************************************************************************************/
#include "wap_mem.h"

/******************************************************************************
* Macro Define Section
******************************************************************************/
/* a macro defines size of the array of memory items have malloced.*/
#define WAP_MEM_ARRAY_MAX_SIZE      256

#ifndef _Min
#define _Min(a,b)                    (((a) < (b)) ? (a) : (b))
#endif /* _Min*/
/******************************************************************************
* Type Define Section
******************************************************************************/
typedef struct tagSt_MEMItem
{
    WE_CHAR     acFileName[16]; /* source file name*/
    WE_INT32    iLine;          /* Code line*/
    
    WE_INT32    iSize;          /* Bytes to allocate*/
    WE_VOID     *pvMemBlock;    /* Pointer to allocate*/
} St_MEMItem;
/******************************************************************************
* Prototype Declare Section
******************************************************************************/


/******************************************************************************
* Global Variable Declare Section
******************************************************************************/


/******************************************************************************
* File Static Variable Define Section
****************************************************************************/
/* Defines a array of memory items have malloced.*/
static St_MEMItem      g_WAP_MEM_astInfo[WAP_MEM_ARRAY_MAX_SIZE] = {0};
/* Defines the count of the array of memory items have malloced.*/
static WE_INT32        g_WAP_MEM_iArrayCount  = 0;
/* total used Currenttly*/
static WE_INT32        g_WAP_MEM_iTotalValue = 0;
/* peak value */
static WE_INT32        g_WAP_MEM_iPeakValue  = 0;

/******************************************************************************
*FUNCTION	: WAP_MemMalloc
*CREATE DATE: 2006-11-09
*AUTHOR		: 
*DESCRIPTION: Allocates memory blocks
*ARGUMENTS PASSED: 
*   WE_CHAR *pcFileName -- Pointer to source file name, __FILE__ usually.
*   WE_INT32 iLine      -- Code line, __LINE__, usually.
*   WE_INT32 iSize      -- Bytes to allocate.
*RETURN VALUE 	 : 
*    returns a void pointer to the allocated space
*    or NULL if there is insufficient memory available.
*USED GLOBAL VARIABLES :
*USED STATIC VARIABLES :
*CALL BY               :
*IMPORTANT NOTES: 
*******************************************************************************/
WE_VOID *WAP_MemMalloc(const  WE_CHAR *pcFileName, WE_INT32 iLine, WE_INT32 iSize)
{
    St_MEMItem* pstItem     = NULL;
    WE_VOID*    pvMemBlock  = NULL;
    
    /* try to malloc a invalid number of memory*/
    if(iSize <= 0)
    {
        WE_LOGINFO( "WAP_MALLOC:-------------------------------------------------\n" );
        WE_LOGINFO( "WAP_MALLOC:You wanna to malloc invalid size.\n" );
        WE_LOGINFO( "WAP_MALLOC:%s:%d, Size:%d \n", pcFileName ? pcFileName : "", iLine, iSize );
        WE_LOGINFO( "WAP_MALLOC:-------------------------------------------------\n" );
        return NULL;
    }
    
    /* error, array is full.*/
    if(g_WAP_MEM_iArrayCount >= WAP_MEM_ARRAY_MAX_SIZE)
    {
        WE_LOGINFO( "WAP_MALLOC:-------------------------------------------------\n" );
        WE_LOGINFO( "WAP_MALLOC:Array is full, won't malloc anymore.\n" );
        WE_LOGINFO( "WAP_MALLOC:%s:%d, Size:%d.\n", pcFileName ? pcFileName : "", iLine, iSize );
        WE_LOGINFO( "WAP_MALLOC:-------------------------------------------------\n" );
        return NULL;
    }
    
    pvMemBlock = MALLOC( iSize );
    if( NULL == pvMemBlock )
    {
        WE_LOGINFO( "WAP_MALLOC:-------------------------------------------------\n" );
        WE_LOGINFO( "WAP_MALLOC:insufficient memory, malloc failed.\n" );
        WE_LOGINFO( "WAP_MALLOC:%s:%d, Size:%d.\n", pcFileName ? pcFileName : "", iLine, iSize );
        WE_LOGINFO( "WAP_MALLOC:-------------------------------------------------\n" );
        return NULL;
    }
    
    /* add it into array*/
    pstItem = g_WAP_MEM_astInfo + g_WAP_MEM_iArrayCount;
    MEMSET(pstItem, 0, sizeof(St_MEMItem));
    
    if(pcFileName)
    {
        /* save file name */
        const WE_CHAR  *rChr = STRRCHR( pcFileName, '\\' );
        if( NULL == rChr )
        {
            rChr = STRRCHR( pcFileName, '/' );
            if(rChr == NULL)
            {
                rChr = pcFileName - 1;
            }
        }
        
        rChr ++;   
        MEMCPY( pstItem->acFileName, rChr, _Min(STRLEN(rChr) , sizeof(pstItem->acFileName) - 1) );
    }
    
    pstItem->iLine      = iLine;
    pstItem->iSize      = iSize;
    pstItem->pvMemBlock = pvMemBlock;
    
    WE_LOGINFO( "WAP_MALLOC:%s:%d,Size:%d,0x%x, No.%d\n",  pcFileName ? pcFileName : "", 
        pstItem->iLine, pstItem->iSize, pstItem->pvMemBlock, g_WAP_MEM_iArrayCount );
    
    g_WAP_MEM_iArrayCount ++;
    
    g_WAP_MEM_iTotalValue += iSize;
    if(g_WAP_MEM_iTotalValue > g_WAP_MEM_iPeakValue)
    {
        g_WAP_MEM_iPeakValue = g_WAP_MEM_iTotalValue;
    }
    return pvMemBlock;
}
/******************************************************************************
*FUNCTION	: WAP_MemFree
*CREATE DATE: 2006-11-09
*AUTHOR		: 
*DESCRIPTION: free memory blocks
*ARGUMENTS PASSED: 
*   WE_CHAR *pcFileName -- Pointer to source file name, __FILE__ usually.
*   WE_INT32 iLine      -- Code line, __LINE__, usually.
*   void *pstMemBlock   -- Pointer to allocate.
*RETURN VALUE 	 : 
*  void
*USED GLOBAL VARIABLES :
*USED STATIC VARIABLES :
*CALL BY               :
*IMPORTANT NOTES: 
*******************************************************************************/
WE_VOID WAP_MemFree(const WE_CHAR *pcFileName, WE_INT32 iLine, WE_VOID *pstMemBlock)
{
    WE_INT32    iCount;
    
    if( NULL == pstMemBlock )
    {
        WE_LOGINFO( "WAP_FREE:-------------------------------------------------\n" );
        WE_LOGINFO( "WAP_FREE:You wanted to free a NULL memory\n" );
        WE_LOGINFO( "WAP_FREE:%s:%d\n", pcFileName ? pcFileName : "", iLine);
        WE_LOGINFO( "WAP_FREE:-------------------------------------------------\n" );
        return;
    }
    
    /* Search it*/
    for( iCount = 0; iCount < g_WAP_MEM_iArrayCount; iCount++ )
    {
        if( g_WAP_MEM_astInfo[iCount].pvMemBlock == pstMemBlock )
        {
            WE_LOGINFO( "WAP_FREE:%s:%d,Size:%d,0x%x,malloc by %s:%d No.%d\n",  
                pcFileName ? pcFileName : "" , iLine, 
                g_WAP_MEM_astInfo[iCount].iSize, pstMemBlock, 
                g_WAP_MEM_astInfo[iCount].acFileName,
                g_WAP_MEM_astInfo[iCount].iLine, iCount);
            
            g_WAP_MEM_iTotalValue -= g_WAP_MEM_astInfo[iCount].iSize;
            
            g_WAP_MEM_iArrayCount -- ;
            if(g_WAP_MEM_iArrayCount != iCount)
            {                
                MEMCPY(g_WAP_MEM_astInfo + iCount, 
                    g_WAP_MEM_astInfo + g_WAP_MEM_iArrayCount  ,sizeof(St_MEMItem));
            }
            
            
            FREE( pstMemBlock );
            return;
        }
    }
    
    WE_LOGINFO( "WAP_FREE:-------------------------------------------------\n" );
    WE_LOGINFO( "WAP_FREE:You wanted to free a unmalloc memory\n" );
    WE_LOGINFO( "WAP_FREE:%s:%d,0x%x\n", pcFileName ? pcFileName : "", iLine, pstMemBlock );
    WE_LOGINFO( "WAP_FREE:-------------------------------------------------\n" );
    
    
}

/******************************************************************************
*FUNCTION	: WAP_Strndup
*CREATE DATE: 2006-11-16
*AUTHOR		: Zhongbin.Xoing
*DESCRIPTION: Duplicate characters of a string
*ARGUMENTS PASSED: 
*   WE_CHAR *pcFileName -- Pointer to source file name, __FILE__ usually.
*   WE_INT32 iLine      -- Code line, __LINE__, usually.
*   const void *pvDest  -- Pointer to destination string .
*   WE_UINT32 iCount    -- Count of destination string
*RETURN VALUE 	 : Each of these functions returns a pointer to the storage 
location for the copied string or NULL if storage cannot be allocated
*USED GLOBAL VARIABLES :
*USED STATIC VARIABLES :
*CALL BY               :
*IMPORTANT NOTES: 
*******************************************************************************/
WE_CHAR *WAP_Strndup(const WE_CHAR *pcFileName, WE_INT32 iLine,
                     const WE_CHAR *pcString, WE_INT iCount)
{
    WE_CHAR * pNew;
    if(pcString == NULL)
    {
        return NULL;
    }
    
    iCount = _Min(iCount, (WE_INT)STRLEN(pcString));
    if(iCount <= 0)
    {
        return NULL;
    }    
    
    WAP_MemTestbourn(pcFileName, iLine, pcString, iCount);
    
    pNew = WAP_MemMalloc(pcFileName, iLine, iCount + 1);
    if(pNew)
    {
        pNew[iCount] = '\0';
        MEMCPY(pNew, pcString, iCount);
    }
    return pNew;
}
/******************************************************************************
*FUNCTION	: WAP_MemTestbourn
*CREATE DATE: 2006-11-09
*AUTHOR		: 
*DESCRIPTION: test the pointer is out of bourn or not
*ARGUMENTS PASSED: 
*   WE_CHAR *pcFileName -- Pointer to source file name, __FILE__ usually.
*   WE_INT32 iLine      -- Code line, __LINE__, usually.
*   const void *pvDest  -- Pointer to buffer.
*   WE_UINT32 iSize     -- Size of [pvDest] buffer
*RETURN VALUE 	 : 
*    -1 : out of bourn
*    0  : ok
*USED GLOBAL VARIABLES :
*USED STATIC VARIABLES :
*CALL BY               :
*IMPORTANT NOTES: 
*******************************************************************************/
WE_INT32 WAP_MemTestbourn(const WE_CHAR *pcFileName, WE_INT32 iLine, 
                          const WE_VOID *pvDest, WE_UINT32 iSize)
{
    WE_INT32 iCount = 0;
    /* Search it*/
    for( ; iCount < g_WAP_MEM_iArrayCount; iCount++ )
    {
        if( (WE_INT8*)pvDest >= (WE_INT8*)g_WAP_MEM_astInfo[iCount].pvMemBlock 
            && (WE_INT8*)pvDest < (WE_INT8*)g_WAP_MEM_astInfo[iCount].pvMemBlock + g_WAP_MEM_astInfo[iCount].iSize )
        {
            if((WE_INT8*)pvDest + iSize > (WE_INT8*)g_WAP_MEM_astInfo[iCount].pvMemBlock + g_WAP_MEM_astInfo[iCount].iSize)
            {
                WE_LOGINFO( "WAP_MEMTESTBOURN:-------------------------------------------------\n" );
                WE_LOGINFO( "WAP_MEMTESTBOURN:Out of bourn\n" );
                WE_LOGINFO( "WAP_MEMTESTBOURN:%s:%d,Size:%d,0x%x,malloc by %s:%d No.%d, test:0x%x, %d\n",  
                    pcFileName ? pcFileName : "" , iLine, 
                    g_WAP_MEM_astInfo[iCount].iSize, g_WAP_MEM_astInfo[iCount].pvMemBlock, 
                    g_WAP_MEM_astInfo[iCount].acFileName, g_WAP_MEM_astInfo[iCount].iLine, iCount,
                    pvDest, iSize);
                WE_LOGINFO( "WAP_MEMTESTBOURN:-------------------------------------------------\n" );
                return -1;
            }
            
            return 0;
        }
    }
    return -1;
}
/******************************************************************************
*FUNCTION	: WAP_MemCmp
*CREATE DATE: 2006-11-09
*AUTHOR		: 
*DESCRIPTION: Compare characters in two buffers
*ARGUMENTS PASSED:  
*   WE_CHAR *pcFileName -- Pointer to source file name, __FILE__ usually.
*   WE_INT32 iLine      -- Code line, __LINE__, usually.
*   const WE_VOID *pBuf1-- First buffer
*   const WE_VOID *pBuf2-- Second buffer
*   WE_UINT32 iSize     -- Number of characters
*RETURN VALUE 	 :  
*   < 0 buf1 less than buf2 
*   0 buf1 identical to buf2 
*   > 0 buf1 greater than buf2 
*USED GLOBAL VARIABLES :
*USED STATIC VARIABLES :
*CALL BY               :
*IMPORTANT NOTES: 
*******************************************************************************/
WE_INT32 WAP_MemCmp(const  WE_CHAR *pcFileName, WE_INT32 iLine, 
                    const WE_VOID *pBuf1, const WE_VOID *pBuf2, WE_UINT32 iSize)
{
    WAP_MemTestbourn(pcFileName, iLine, pBuf1, iSize);
    WAP_MemTestbourn(pcFileName, iLine, pBuf2, iSize);
    return MEMCMP(pBuf1, pBuf2, iSize);
}
/******************************************************************************
*FUNCTION	: WAP_MemPrintMemLeaked
*CREATE DATE: 2006-11-09
*AUTHOR		: 
*DESCRIPTION: print memory leaked
*ARGUMENTS PASSED:  none
*RETURN VALUE 	 :  none
*USED GLOBAL VARIABLES :
*USED STATIC VARIABLES :
*CALL BY               :
*IMPORTANT NOTES: 
*******************************************************************************/
WE_VOID WAP_MemPrintMemLeaked( )
{
    WE_INT32    iLoop;    
    for( iLoop = 0; iLoop < g_WAP_MEM_iArrayCount; iLoop++ )
    {
        WE_LOGINFO( "WAP_PRINTMEMINFO:-------------------------------------------------\n" );
        WE_LOGINFO( "WAP_PRINTMEMINFO:[%d] %s: %d, %d, 0x%x\n", 
            iLoop, g_WAP_MEM_astInfo[iLoop].acFileName, g_WAP_MEM_astInfo[iLoop].iLine, 
            g_WAP_MEM_astInfo[iLoop].iSize, g_WAP_MEM_astInfo[iLoop].pvMemBlock );
        WE_LOGINFO( "WAP_PRINTMEMINFO:-------------------------------------------------\n" );
    }
    WE_LOGINFO( "WAP_PRINTMEMINFO:Wasted size %d bytes [%d], peak value is %d bytes  \n", 
        g_WAP_MEM_iTotalValue, g_WAP_MEM_iArrayCount, g_WAP_MEM_iPeakValue );
}

/* The end*/
