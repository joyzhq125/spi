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

/*! \file mauimob.h
 *  \brief Media object browser UI logic (headers)
 */

#ifndef _MAUIMOB_H_
#define _MAUIMOB_H_

/******************************************************************************
 * Prototypes
 *****************************************************************************/

WE_BOOL msaUiMobAddSpecialOp(MsaMobSpecialOp op);
WE_BOOL msaUiMobAddWidget(const MsaMoDataItem *mo);
WE_BOOL msaUiInitializeForm(void);
WE_BOOL msaUiMobFinalizeForm(WE_UINT32 winTitle);
int msaUiMobCheckFocus(void);
MsaMobSpecialOp msaUiMobGetSpecialOp(void);
void msaUiMobDeleteAll(void);
void msaUiMobInit(void);
void msaUiMobTerminate(void);    
MtrLinkEntry *msaUiMobGetLink(int num);

#endif
