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

/* \file mamcr.h
 * \brief Message Creation FSM
 */

#ifndef _MAMCR_H_
#define _MAMCR_H_

/******************************************************************************
* Constants
*****************************************************************************/

/*!\enum MsaMcrSignalId
 * Signals for the MCR FSM */
typedef enum 
{
    /*! Creates a message
     * p_param = pointer to a MsaCreateMessageSignal data-structure
     */
    MSA_SIG_MCR_CREATE_MESSAGE,

    /*! A delayed file operation can be retrieved again
     *
     */
    MSA_SIG_MCR_FILE_NOTIFY,

    /*! 
     *  Execution can continue again
     */
    MSA_SIG_MCR_CONTINUE,

    /*! An ASIO read operation has finished
     *  u_param1 = result, see #MsaAsioResult
     *  u_param2 = bytes read
     */
    MSA_SIG_MCR_ASIO_READ_RSP,

    /*! An ASIO write operation has finished
     *  u_param1 = result, see #MsaAsioResult
     */
    MSA_SIG_MCR_ASIO_WRITE_RSP,

    /*! Handling of "create message" response from the MMS Service
     *
     * u_param1 = result
     * u_param2 = msgId
     */
    MSA_SIG_MCR_CREATE_MSG_RESPONSE,

    /*! Handling of "create message done" response from the MMS Service
     *
     * u_param1 = result
     * u_param2 = msgId
     */
    MSA_SIG_MCR_CREATE_MSG_DONE_RESPONSE,

    /*! Response from the "get header" operation used for forward
     *
     * u_param1 = The result of the operation, see #MmsResult.
     * p_param = The #MmsGetHeader data-structure.
     */
    MSA_SIG_MCR_GET_HEADER_RSP,

    /*! Response from the "get header" operation used for forward
     *
     * u_param1 = The result of the operation, see #MmsResult.
     * p_param = The #MmsBodyInfoList data-structure.
     */
    MSA_SIG_MCR_GET_SKELETON_RSP

} MsaMcrSignalId;

/*!\enum MsaMcrResult 
 * Response signal data for message creation */
typedef enum
{
    MSA_MCR_OK,             /*!< The message was created successfully */
    MSA_MCR_FAILED,         /*!< Failed to create the message */
    MSA_MCR_CLEANUP_SILENT, /*!< Only used internally */
    MSA_MCR_WAIT_RSP,       /*!< Only used internally */
    MSA_MCR_DELAYED,        /*!< Only used internally */
    MSA_MCR_DONE            /*!< Only used internally */
}MsaMcrResult;

/******************************************************************************
 * Data-types
 *****************************************************************************/

/*!\struct MsaCreateMessageSignal
 * \brief Data-structure used when creating a message.
 */
typedef struct 
{
    MsaStateMachine fsm;         /*!< FSM to send response to */
    int             retSig;      /*!< The response signal */
    MsaMessage      *message;    /*!< The message to send */
    /* Information about original message */
    MmsFileType     origFileType;/*!< File type of original message */
    MmsMsgId        origMsgId;   /*!< The original msg if relevant */
    WE_BOOL        del;         /*!< Whether to delete the original message 
                                      after successful creation of a new one */
    MmsFolderType   folder;      /*!< The folder to save the message in */
    WE_BOOL        forwardMode; /*!< Tells if this is a forward operation */
}MsaCreateMessageSignal;

/******************************************************************************
 * Defines 
 *****************************************************************************/

/******************************************************************************
 * Prototype declarations 
 *****************************************************************************/

void msaMcrInit(void); 
void msaMcrTerminate(void);
void msaCreateMessage(MsaStateMachine fsm, unsigned int retSig, MsaMessage *msg, 
    MmsMsgId origId, WE_BOOL del, MmsFolderType folder, 
    MmsFileType origFileType, WE_BOOL forward);

#endif /* _MAMCR_H_ */
