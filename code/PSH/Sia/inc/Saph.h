/*
 * Copyright (C) Techfaith, 2002-2005.
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

#ifndef SAPH_H
#define SAPH_H

/******************************************************************************
 * Constants
 *****************************************************************************/

typedef enum
{
    /* The result from a package operation
     *
     * p_param = The result
     */
    SIA_SIG_PH_READ_RESULT
}SiaPhSignal;

typedef enum
{
    SIA_AFI_READ_RESULT_OK,         /*!< The operation was successful */
    SIA_AFI_READ_RESULT_FAILED,     /*!< The operation failed */
    SIA_AFI_READ_RESULT_TO_LARGE,   /*!< The file was larger than the largest 
                                         file requested */
    SIA_AFI_READ_RESULT_INT_TERM    /*!< Only used internally */
}SiaAfiReadResult;

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
}SiaAfiReadResultData;

/******************************************************************************
 * Prototypes 
 *****************************************************************************/

WE_BOOL smaPhInit(void);
void smaPhTerminate(void);
WE_BOOL smaPhHandleSignal(WE_UINT16, void *p);

WE_BOOL smaOpenDlgCreateMediaInput(SiaStateMachine fsm, int signal, 
    const char *mimeTypes);
void *smaOpenDlgMediaInputAbort(void);

WE_BOOL smaAfiLoadFromFile(SiaStateMachine fsm, int signal, 
    char *fileName, void *callbackData, WE_UINT32 maxSize);
void *smaAfiLoadFromFileAbort(void);
WE_UINT32 smaAfiGetFileSize(char *fileName);

void smaAfiLoadFreeData(SiaAfiReadResultData *data);


#endif

