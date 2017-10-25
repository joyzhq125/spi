/*
 * Copyright (C) Techfaith, 2002-2005.
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
 
#ifndef SAUI_H
#define SAUI_H

#ifndef SATYPES_H
#error Satypes.h must be included berfore Saui.h!
#endif
#ifndef SIA_DEF_H
#error Sia_def.h must be included berfore Saui.h!
#endif
#ifndef SAUIFORM_H
#error Sauiform.h must be included berfore Saui.h!
#endif

/*--- Definitions/Declarations ---*/

/* Macros */
#define SIA_GET_STR_ID(strId)       WE_WIDGET_STRING_GET_PREDEFINED(strId)
#define SIA_GET_IMG_ID(imgId)       WE_WIDGET_IMAGE_GET_PREDEFINED(imgId, NULL)

/*--- Types ---*/

/* \enum SiaGadgetType Defines diferent gadgets for output */
typedef enum
{
	SIA_GADGET_ICON,	/* Gadget to use for icons */
    SIA_GADGET_IMAGE,	/* Gadget to use for image */
	SIA_GADGET_STRING,	/* Gadget to use for string text */
	SIA_GADGET_ALT		/* Gadget to use for alternative text attribute */
} SiaGadgetType;

/* \enum SiaDialogType Defines actions that have been requested */
typedef enum
{
    SIA_ACTION_STOP,
    SIA_ACTION_PLAY
} SiaActionType;


/*********************************************
 * Exported function
 *********************************************/

WE_BOOL smaShowStatusIcon(SiaActionType action);
void deleteStatusIconHandle(void);
void displayNextSlide(void);
WeImageZoom smaGetImageZoom(SisFit slsFitValue);
SiaResult widgetActionHandler(WE_UINT32 action, WE_UINT32 window);
void widgetDeleteScreen(void);
SiaResult createGUI(void);
void smaUiInit(void);
void smaUiTerminate(void);
void smaGetWidnowInsideArea(WeWindowHandle window, WeSize *size);
SiaResult showSlideMenu( void);
int widgetGetSelected(WE_UINT32 choiceHandle);
WeScreenHandle smaGetScreenHandle(void);
WePosition *smaGetPosLeftTop(void);
int smaBindAndShowWindow(WeWindowHandle windowHandle, 
	const WePosition *pos);
WE_INT32 smaGetSelectedChoiceIndex(WE_UINT32 choiceHandle);
WeStringHandle smaCreateString(const char *str);
WE_BOOL smaSetTitle(WeWindowHandle win, WeStringHandle strHandle, 
    int currentProperties);
char *smaGetStringFromHandle(WeStringHandle strHandle);
WE_BOOL smaIntegerToWeColor(WE_UINT32 color, WeColor *mColor);
char *smaGetStringBufferFromHandle(WeStringHandle strHandle);
#endif /* SAUI_H */


