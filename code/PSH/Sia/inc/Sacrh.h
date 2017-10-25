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

/*!\file macrh.h
 * \brief Content routing handler.
 */

#ifndef SACRH_H
#define SACRH_H

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! \enum SiaCrhResult
 *  Result codes 
 */
typedef enum
{
    SIA_CRH_OK,                     /* !< The operation completed successfully */
    SIA_CRH_ERROR,                  /* !< The operations failed */
    SIA_CRH_INTERNAL_TERMINATION    /* !< Only used internally by the CRH FSM */
}SiaCrhResult;

/******************************************************************************
 * Data-types
 *****************************************************************************/

/******************************************************************************
 * Function prototypes
 *****************************************************************************/

/* Set-up */
WE_BOOL smaCrhInit(void);
void smaCrhTerminate(void);

/* AFI functions */
void smaCrhGetActions(const MmsContentType *contentType, const char *drmContentType, 
    SiaStateMachine fsm, int signal);
void smaCrhSendContent(we_pck_attr_list_t *file_attrs, const char *fileName,
    SiaStateMachine fsm, int signal, const we_act_action_entry_t *actionEntry,
    MmsMsgId msgId, WE_UINT16 bodyPartIndex);

/* Interaction with external signal queue */
WE_BOOL smaCrhHandleSignal(WE_UINT16 signal, void *p);
void smaCrhHandleGetBpRsp(const MmsGetBodyPartReply *bodyPart);

/* Utils functions */
void smaFreeActionList(we_act_action_entry_t *actionsList, WE_UINT32 count);
we_act_action_entry_t *smaCopyActionList(
    const we_act_action_entry_t *actionsList, WE_UINT32 count);

#endif
