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

/*! \file mauicmn.h
 *  \brief Common functions for the GUI.
 */

#ifndef _MAUICMN_H_
#define _MAUICMN_H_

/******************************************************************************
 * Macros
 *****************************************************************************/
#define MSA_GET_STR_ID(strId)   WE_WIDGET_STRING_GET_PREDEFINED(strId)
#define MSA_GET_IMG_ID(resId)   WE_WIDGET_IMAGE_GET_PREDEFINED(resId, NULL)

/*! The default width and height for gadgets.
 */
#define MSA_GADGET_DEF_WIDTH            240
#define MSA_GADGET_DEF_HEIGHT           40

#define MSA_GADGET_DEF_PADDING          (5)
#define MSA_CMN_BASELINE_SIZE           (1)

 /******************************************************************************
 * Types
*****************************************************************************/

/*! \enum MsaCmnStringGadgetData */
typedef struct 
{
    WeWindowHandle window;      /*!< Handle of the gadget window */
    WeGadgetHandle gadget;      /*!< Handle of the gadget */
    WE_BOOL hasLabel;           /*!< TRUE if gadget has a title */
    WE_BOOL multipleLines;      /*!< TRUE if gadget allows multiple lines */
    WeStringHandle gadgetString;/*!< A handle to the string in the gadget */
    WePosition gadgetPos;       /*!< The position the gadget will have */
    WeSize maxSize;             /*!< The max (default) size of the gadget */
    WeSize *gadgetSize;         /*!< Pointer to where the new optimized 
                                      size will be stored */
}MsaCmnStringGadgetData;

/*! \enum MsaSeverity */
typedef enum
{
    MSA_SEVERITY_OK,        /*!< Status OK */    
    MSA_SEVERITY_WARNING,   /*!< Status Warning */
    MSA_SEVERITY_ERROR      /*!< Status Error */
}MsaSeverity;

/******************************************************************************
 * Prototypes
 *****************************************************************************/

void msaInitGui(void);
void msaTerminateGui(void);
WeScreenHandle msaGetScreenHandle(void);
void msaGetDisplaySize(WeSize *ms);
int msaGetSelectedChoiceIndex(WE_UINT32 choiceHandle);
WeStringHandle msaGetEmptyStringHandle(void);
WePosition *msaGetPosLeftTop(void);
int msaDisplayWindow(WeWindowHandle windowHandle, 
    const WePosition *pos);
char *msaGetStringFromHandle(WeStringHandle strHandle);
char *msaGetTextFromGadget(WeGadgetHandle gadgetHandle);
WE_BOOL msaSetTextInputGadgetValue(WeGadgetHandle gadgetHandle, int maxLen, 
    const char *newStringValue);
WE_BOOL msaSetTitle(WeWindowHandle win, WeStringHandle strHandle, 
    int currentProperties);
WE_BOOL msaSetStringGadgetValue(WeGadgetHandle gadgetHandle,
    WeStringHandle strHandle, const char *str);
char *msaGetIntervalString(WE_UINT32 prefix, int minVal, 
    WE_UINT32 infix, int maxVal);
MsaSeverity msaShowSizeWarningDialog(WE_UINT32 newMsgSize, 
    WE_UINT32 objectSize, MsaMediaGroupType mediaGroupType,
    MsaStateMachine fsm, int signal);
void msaIntToWeColor(WE_UINT32 rgbColor, WeColor *weColor);
WE_UINT32 msaWeToIntColor(WeColor *weColor);
void msaCalculateStringGadgetSize(WeWindowHandle window, 
    WeGadgetHandle gadget, WeStringHandle strHandle, const WePosition *pos, 
    const WeSize *maxSize, WeSize *newSize, WE_BOOL multipleLines);

#endif
