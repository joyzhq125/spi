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

/*! \file mauimmv.h
 */

#ifndef _MAUIMMV_H_
#define _MAUIMMV_H_

/******************************************************************************
 * Prototypes
 *****************************************************************************/

WE_BOOL msaUiMmvCreateMenu(const we_act_action_entry_t *actions, 
    WE_UINT32 actionCount);
WE_BOOL msaUiMmvPlayAudio(const MsaMoDataItem *mo);
WE_BOOL msaUiMmvAudioActive(void);
void msaUiMmvStopAudio(void);
void msaUiMmvDeleteMenu(void);
void msaMmvUiInit(void);

#endif
