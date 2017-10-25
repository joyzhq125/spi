/*
 * Copyright (C) Techfaith 2002-2005.
 * All rights reserved.
 *
 * This software is covered by the license agreement between
 * the end user and Techfaith, and may be 
 * used and copied only in accordance with the terms of the 
 * said agreement.
 *
 * Techfaith assumes no responsibility or 
 * liability for any errors or inaccuracies in this software, 
 * or any consequential, incidental or indirect damage arising
 * out of the use of the software.
 *
 */
 
#ifndef _AU_PIPE_H_
#define _AU_PIPE_H_


/*---------- Global Definitions and Declarations ---------------*/

/*---------- Constant / Macro Definitions ----------------------*/

/*---------- Type Declarations ---------------------------------*/
typedef struct {
    WE_UINT8    iValid;                      /* 0: invalid; 1: valid */
    WE_UINT8    iDeleted;                    /* 0: hasn't been deleted; 1: has been deleted; */
    WE_UINT8    iWriteModId;                 /* Write end module    ID    */
    WE_UINT8    iReadModId;                  /* Read end module    ID    */
    char*       pStrPipeName;                /* Null terminated string */
    int         iWriteHandle;                /* 0: close; others: open */            
    int         iReadHandle;                 /* 0: close; others: open */
    WE_UINT8    iReadEndPolled;              /* 0: Read end hasn't polled; 1: ... has polled*/
    WE_UINT8    iWriteEndPolled;             /* Refer to last line */
    WE_UINT8*   pDataBufBase;                /* Data tunnel base address*/
    long        iDataBufSize;                /* Size of data buffer size */
    long        iValidDataPosIdx;            /* Valid data position offset from data buffer base */
    long        iValidDataLen;               /* valid data length */
}PipeStruct;
/*---------- Variable Declarations -----------------------------*/

/*---------- function Declarations -----------------------------*/

int    PipeCreate (WE_UINT8 uiModId, const char* pcName);
int    PipeOpen (WE_UINT8 uiModId, const char *pcName);
int    PipeClose (int iHandle);
int    PipeDelete (const char* pcName);
long   PipeRead (int iHandle, void* pvBuf, long lBufSize);
long   PipeWrite (int iHandle, void* pvBuf, long lBufSize);
int    PipePoll (int iHandle);
int    PipeStatus (int iHandle, int *piIsOpen, long *plAvailable);
void   PipeCloseAll (WE_UINT8 uiModId);


/*------------------END-----------------------------------------*/

#endif 
