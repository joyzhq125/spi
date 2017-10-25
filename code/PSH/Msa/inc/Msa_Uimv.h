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

/*! \file mauimv.h
 *  \brief The graphical user interface for the message property viewer.
 */

#ifndef _MAUIMV_H_
#define _MAUIMV_H_

/******************************************************************************
 * Constants
 *****************************************************************************/

/******************************************************************************
 * Data-types
 *****************************************************************************/

/******************************************************************************
 * Prototypes
 *****************************************************************************/

void msaMvUiInit(void);

WE_BOOL msaCreateMvPropView(MmsFileType fileType, const MsaPropItem *propItem, 
    int fsmOk, int sigOk, int fsmBack, int sigBack);
WE_BOOL msaSetMvProperties(const MsaPropItem *propItem);
void msaDeleteMvPropView(void);

WE_BOOL msaCreateMvRrDrView(WE_UINT32 titleStrId, const char *from, 
    WE_UINT32 statusStrId, const char *date);
void msaDeleteMvRrDrView(void);

WE_BOOL msaCreateMvNavMenu(MsaMessageType messageType);
void msaDeleteMvNavMenu(void);

#endif
