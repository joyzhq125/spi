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

/*!\file mauipm.h
 * \brief Progress meter.
 */

#ifndef _MAUIPM_H_
#define _MAUIPM_H_

/******************************************************************************
 * Prototypes
 *****************************************************************************/

void msaInitPm(void);
void msaTerminatePm(void);
WE_BOOL msaPmViewShow(WE_UINT32 messageStrId, WE_BOOL manualTick, 
    int allowCancel, int fsm, int signal);
void msaPmViewRemove(void);
void msaPmHandleTimer(void);
void msaPmSetProgress(unsigned int perCent, const char *str);

#endif
