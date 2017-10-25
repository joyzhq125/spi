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

/*!\file maph.h
 * \brief Packet handling.
 */

#ifndef _MAPH_H_
#define _MAPH_H_

/******************************************************************************
 * Constants
 *****************************************************************************/

typedef enum
{
    /* The result from a package operation
     *
     * p_param = The result
     */
    MSA_SIG_PH_READ_RESULT
}MsaPhSignal;

typedef enum
{
    MSA_AFI_READ_RESULT_OK,         /*!< The operation was successful */
    MSA_AFI_READ_RESULT_FAILED,     /*!< The operation failed */
    MSA_AFI_READ_RESULT_TO_LARGE,   /*!< The file was larger than the largest 
                                         file requested */
    MSA_AFI_READ_RESULT_INT_TERM    /*!< Only used internally */
}MsaAfiReadResult;

/******************************************************************************
 * Data-types
 *****************************************************************************/
typedef struct
{
    void        *data;
    WE_UINT32  size;
    char        *fileName;
    char        *mimeType;
    void        *callbackData;
}MsaAfiReadResultData;

/******************************************************************************
 * Prototypes 
 *****************************************************************************/

WE_BOOL msaPhInit(void);
void msaPhTerminate(void);
WE_BOOL msaPhHandleSignal(WE_UINT16, void *p);

WE_BOOL msaOpenDlgCreateMediaInput(MsaStateMachine fsm, int signal, 
    const char *mimeTypes);
void *msaOpenDlgMediaInputAbort(void);

WE_BOOL msaAfiLoadFromFile(MsaStateMachine fsm, int signal, 
    char *fileName, void *callbackData, WE_UINT32 maxSize);
void *msaAfiLoadFromFileAbort(void);
WE_UINT32 msaAfiGetFileSize(char *fileName);

void msaAfiLoadFreeData(MsaAfiReadResultData *data);


#endif

