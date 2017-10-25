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

/* !\file mauicols.h
 * \brief Displays a color selector to the End User.
 */

#ifndef _MAUICOLS_H_
#define _MAUICOLS_H_

/******************************************************************************
 * Constants
 *****************************************************************************/
#define MSA_COLOR_NOT_VISIBLE   0xff000000  /*!< Defines a "not used" color */

/******************************************************************************
 * Prototypes
 *****************************************************************************/

void msaColorSelectorInit(void);
void msaColorSelectorTerminate(void);
WE_BOOL msaCreateColorSelector(const WE_UINT32 *colors, unsigned int colorCount, 
    WE_UINT32 hideColor, WE_UINT32 selectedColor, MsaStateMachine fsm, 
    int signal);
void msaDeleteColorSelector(void);

#endif
