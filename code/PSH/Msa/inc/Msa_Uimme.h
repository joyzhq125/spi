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

/* !\file mauimme.h
 */

#ifndef _MAUIMME_H_
#define _MAUIMME_H_

/******************************************************************************
 * Macros
 *****************************************************************************/

/******************************************************************************
 * Data-types
 *****************************************************************************/

/******************************************************************************
 * Prototype declarations
 *****************************************************************************/

void msaMmeUiInit(void);
void msaMmeUiSetMsg(MsaMessage *msg);

/* Navigation menu for the MME */
WE_BOOL msaUiMmeCreateNavMenu(int index, MmsFileType msgType);
void msaUiMmeDeleteNavMenu(void);
MsaMoDataItem *msaUiMmeGetSelectedMo(void);
WE_BOOL msaUiMmeCreateTextView(const char *text, const char *subject, 
    MsaStateMachine retFsm, int retSigOk, int retSigBack, int retSigMenu);
void msaUiMmeDeleteTextView(void);
char *msaUiMmeGetMessageText(void);
void msaUiMmePlayAudio(void);
void msaUiMmeStopAudio(void);

#endif /* _MAUIMME_H_ */
