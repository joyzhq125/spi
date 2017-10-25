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

/*!\file macrh.h
 * \brief Content routing handler.
 */

#ifndef _MACRH_H_
#define _MACRH_H_

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! \enum MsaCrhResult
 *  Result codes 
 */
typedef enum
{
    MSA_CRH_OK,                     /* !< The operation completed successfully */
    MSA_CRH_ERROR,                  /* !< The operations failed */
    MSA_CRH_INTERNAL_TERMINATION    /* !< Only used internally by the CRH FSM */
}MsaCrhResult;

/******************************************************************************
 * Data-types
 *****************************************************************************/

/******************************************************************************
 * Function prototypes
 *****************************************************************************/

/* Set-up */
WE_BOOL msaCrhInit(void);
void msaCrhTerminate(void);

/* AFI functions */
void msaCrhGetActions(MmsContentType *contentType, const char *drmContentType, 
    MsaStateMachine fsm, int signal);
void msaCrhSendContent(we_pck_attr_list_t *file_attrs, char *fileName,
    MsaStateMachine fsm, int signal, const we_act_action_entry_t *actionEntry,
    MmsMsgId msgId, WE_UINT16 bodyPartIndex);

/* Interaction with external signal queue */
WE_BOOL msaCrhHandleSignal(WE_UINT16 signal, void *p);
void msaCrhHandleGetBpRsp(const MmsGetBodyPartReply *bodyPart);

/* Utils functions */
void msaFreeActionList(we_act_action_entry_t *actionsList, WE_UINT32 count);
we_act_action_entry_t *msaCopyActionList(
    const we_act_action_entry_t *actionsList, WE_UINT32 count);

#endif
