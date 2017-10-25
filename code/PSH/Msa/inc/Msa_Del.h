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

/* \file madel.h
 * \brief Handling of message deletion.
 */

#ifndef _MADEL_H_
#define _MADEL_H_

/******************************************************************************
 * Prototype declarations
 *****************************************************************************/

void msaDelInit(void);
void msaDelTerminate(void);
void msaDeleteMsg(MmsMsgId msgId, WE_BOOL confirm);
void msaDelHandleDeleteRsp(MmsResult result);

#endif /* _MADEL_H_ */
