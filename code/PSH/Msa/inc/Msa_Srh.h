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

/* !\file masrh.h
 * !\brief Handles the signal routing case where one interface function can be 
 *         called from multiple FSM's, .i.e., when the response signal needs to 
 *         be routed to more than one destination.
 */

#ifndef _MASRH_H_
#define _MASRH_H_

/******************************************************************************
 * Prototypes
 *****************************************************************************/
void msaSrhInit(void);
void msaSrhTerminate(void);

void msaSrhGetMessageInfo(MsaStateMachine fsm, int signal, MmsMsgId msgId);
void msaSrhGetMessageInfoRsp(const MmsListOfProperties *list);

void msaSrhMakeCall(MsaStateMachine fsm, int signal, const char *number);
void msaSrhMakeCallRsp(const we_tel_make_call_resp_t *callRsp);

#endif
