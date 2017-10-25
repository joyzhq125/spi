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

/* !\file mauise.h
 */

#ifndef _MAUBSE_H_
#define _MAUBSE_H_

/******************************************************************************
 * Type definitions
 *****************************************************************************/

/*! \enum MsaSeGadgetDef
 *	
 */
typedef enum
{
    MSA_SE_GADGET_NONE = 0,     /*!< No gadget is active */
    MSA_SE_GADGET_INP_AUDIO,    /*!< The audio icon is active */
    MSA_SE_GADGET_INP_TEXT,     /*!< The text region is active */
    MSA_SE_GADGET_INP_IMAGE,    /*!< The image regin is active */
    MSA_SE_GADGET_AUDIO,        /*!< The audio icon is active */
    MSA_SE_GADGET_TEXT,         /*!< The text part is active */
    MSA_SE_GADGET_IMAGE         /*!< The image part is active */
}MsaSeGadgetDef;

/******************************************************************************
 * Data-types
 *****************************************************************************/

/******************************************************************************
 * Prototype declarations
 *****************************************************************************/

void msaSeUiInit(void);

/* Slide edit view */
WE_BOOL msaSeCreateEditorView(void);
WE_BOOL msaSeShowSlide(const MsaSmilInfo *smilInfo, 
    const MsaPropItem *propItem);

void msaSeSetBusy(WE_BOOL busy);
void msaSeDeleteEditorView(void);

/* Active region */
MsaSeGadgetDef msaSeGetActiveGadget(void);
MsaMediaGroupType msaSeGetActiveObjType(void);
void msaSeSetActiveObj(void);

/* Text editor */
WE_BOOL msaSeCreateTextView(char *text, const char *title);

/* User interaction */
void msaSeHandleEvent(WeEventType event);
void msaSeHandleNotif(WeNotificationType notif);

/* Item timing */
WE_BOOL msaSeCreateItemTiming(WE_UINT32 start, WE_UINT32 dur, 
    WE_UINT32 slideDur, MsaMediaGroupType gType);
void msaSeUpdateItemTiming(WE_UINT32 objStart, WE_UINT32 objDuration, 
    WE_UINT32 slideDuration);
void msaSeDeleteItemTiming(void);

#endif
