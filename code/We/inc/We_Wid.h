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
/*
 * we_wid.h
 *
 * Created by Anders Hansson
 *
 * Revision  history:
 * 
 *
 */

#ifndef _we_wid_h
#define _we_wid_h

#ifndef _we_int_h
#include "we_int.h"
#endif


/****************************************
 * Widget
 ****************************************/
#define WE_WIDGET_DISPLAY_GET_PROPERTIES   TPIa_widgetDisplayGetProperties
#define WE_WIDGET_SET_IN_FOCUS             TPIa_widgetSetInFocus
#define WE_WIDGET_HAS_FOCUS                TPIa_widgetHasFocus
#define WE_WIDGET_RELEASE                  TPIa_widgetRelease
#define WE_WIDGET_RELEASE_ALL              TPIa_widgetReleaseAll
#define WE_WIDGET_REMOVE                   TPIa_widgetRemove
#define WE_WIDGET_ADD_ACTION               TPIa_widgetAddAction
#define WE_WIDGET_HANDLE_WE_EVT           TPIa_widgetHandleWeEvt
#define WE_WIDGET_CHANGE_WE_EVT_MODULE    TPIa_widgetChangeWeEvtModule
#define WE_WIDGET_HANDLE_WE_NOTIFICATION  TPIa_widgetHandleWeNotification
#define WE_WIDGET_SET_TITLE                TPIa_widgetSetTitle
#define WE_WIDGET_SET_POSITION             TPIa_widgetSetPosition
#define WE_WIDGET_SET_SIZE                 TPIa_widgetSetSize
#define WE_WIDGET_SET_MARQUEE              TPIa_widgetSetMarquee
#define WE_WIDGET_GET_SIZE                 TPIa_widgetGetSize
#define WE_WIDGET_WE_EVENT_2_UTF8         TPIa_widgetWeEvent2Utf8
#define WE_WIDGET_WINDOW_ADD_GADGET        TPIa_widgetWindowAddGadget
#define WE_WIDGET_WINDOW_ADD_WINDOW        TPIa_widgetWindowAddWindow
#define WE_WIDGET_SCREEN_CREATE            TPIa_widgetScreenCreate
#define WE_WIDGET_DISPLAY_ADD_SCREEN       TPIa_widgetDisplayAddScreen
#define WE_WIDGET_SCREEN_ADD_WINDOW        TPIa_widgetScreenAddWindow
#define WE_WIDGET_WINDOW_SET_PROPERTIES    TPIa_widgetWindowSetProperties
#define WE_WIDGET_GET_INSIDE_AREA          TPIa_widgetGetInsideArea
#define WE_WIDGET_PAINTBOX_CREATE          TPIa_widgetPaintboxCreate
#define WE_WIDGET_FORM_CREATE              TPIa_widgetFormCreate
#define WE_WIDGET_TEXT_SET_TEXT            TPIa_widgetTextSetText
#define WE_WIDGET_EDITOR_CREATE            TPIa_widgetEditorCreate
#define WE_WIDGET_CHOICE_SET_ELEMENT       TPIa_widgetChoiceSetElement
#define WE_WIDGET_CHOICE_REMOVE_ELEMENT    TPIa_widgetChoiceRemoveElement
#define WE_WIDGET_CHOICE_GET_ELEM_STATE    TPIa_widgetChoiceGetElemState
#define WE_WIDGET_CHOICE_SET_ELEM_STATE    TPIa_widgetChoiceSetElemState
#define WE_WIDGET_CHOICE_GET_ELEM_TAG      TPIa_widgetChoiceGetElemTag
#define WE_WIDGET_CHOICE_SIZE              TPIa_widgetChoiceSize
#define WE_WIDGET_CHOICE_GET_SELECTED      TPIa_widgetChoiceGetSelected
#define WE_WIDGET_MENU_CREATE              TPIa_widgetMenuCreate
#define WE_WIDGET_DIALOG_CREATE            TPIa_widgetDialogCreate
#define WE_WIDGET_ACTION_CREATE            TPIa_widgetActionCreate
#define WE_WIDGET_ACTION_GET_TYPE          TPIa_widgetActionGetType
#define WE_WIDGET_GADGET_SET_PROPERTIES    TPIa_widgetGadgetSetProperties
#define WE_WIDGET_STRING_GADGET_CREATE     TPIa_widgetStringGadgetCreate
#define WE_WIDGET_STRING_GADGET_SET        TPIa_widgetStringGadgetSet
#define WE_WIDGET_TEXT_INPUT_CREATE        TPIa_widgetTextInputCreate
#define WE_WIDGET_SELECTGROUP_CREATE       TPIa_widgetSelectgroupCreate
#define WE_WIDGET_IMAGE_GADGET_CREATE      TPIa_widgetImageGadgetCreate
#define WE_WIDGET_IMAGE_GADGET_SET         TPIa_widgetImageGadgetSet
#define WE_WIDGET_BAR_CREATE               TPIa_widgetBarCreate
#define WE_WIDGET_BAR_SET_VALUES           TPIa_widgetBarSetValues
#define WE_WIDGET_BAR_GET_VALUES           TPIa_widgetBarGetValues
#define WE_WIDGET_SET_ACCESS_KEY           TPIa_widgetSetAccessKey
#define WE_WIDGET_REMOVE_ACCESS_KEY        TPIa_widgetRemoveAccessKey
#define WE_WIDGET_DRAW_LINE                TPIa_widgetDrawLine
#define WE_WIDGET_DRAW_RECT                TPIa_widgetDrawRect
#define WE_WIDGET_DRAW_CIRCLE              TPIa_widgetDrawCircle
#define WE_WIDGET_DRAW_STRING              TPIa_widgetDrawString
#define WE_WIDGET_DRAW_IMAGE               TPIa_widgetDrawImage
#define WE_WIDGET_DRAW_POLYGON             TPIa_widgetDrawPolygon
#define WE_WIDGET_DRAW_ACCESS_KEY          TPIa_widgetDrawAccessKey
#define WE_WIDGET_HOLD_DRAW                TPIa_widgetHoldDraw
#define WE_WIDGET_PERFORM_DRAW             TPIa_widgetPerformDraw
#define WE_WIDGET_GET_BRUSH                TPIa_widgetGetBrush
#define WE_WIDGET_SET_COLOR                TPIa_widgetSetColor
#define WE_WIDGET_SET_LINE_PROPERTY        TPIa_widgetSetLineProperty
#define WE_WIDGET_SET_TEXT_PROPERTY        TPIa_widgetSetTextProperty
#define WE_WIDGET_SET_PATTERN              TPIa_widgetSetPattern
#define WE_WIDGET_SET_FONT                 TPIa_widgetSetFont
#define WE_WIDGET_FONT_GET_FAMILY          TPIa_widgetFontGetFamily
#define WE_WIDGET_STRING_CREATE            TPIa_widgetStringCreate
#define WE_WIDGET_STRING_GET_PREDEFINED    TPIa_widgetStringGetPredefined
#define WE_WIDGET_STRING_CREATE_TEXT       TPIa_widgetStringCreateText
#define WE_WIDGET_STRING_CREATE_DATE_TIME  TPIa_widgetStringCreateDateTime
#define WE_WIDGET_STRING_GET_LENGTH        TPIa_widgetStringGetLength
#define WE_WIDGET_STRING_GET_DATA          TPIa_widgetStringGetData
#define WE_WIDGET_STRING_GET_WIDTH         TPIa_widgetStringGetWidth
#define WE_WIDGET_STRING_GET_NBR_OF_CHARS  TPIa_widgetStringGetNbrOfChars
#define WE_WIDGET_FONT_GET_VALUES          TPIa_widgetFontGetValues
#define WE_WIDGET_IMAGE_CREATE             TPIa_widgetImageCreate
#define WE_WIDGET_IMAGE_GET_PREDEFINED     TPIa_widgetImageGetPredefined
#define WE_WIDGET_IMAGE_CREATE_EMPTY       TPIa_widgetImageCreateEmpty
#define WE_WIDGET_IMAGE_CREATE_NATIVE      TPIa_widgetImageCreateNative
#define WE_WIDGET_STYLE_CREATE             TPIa_widgetStyleCreate
#define WE_WIDGET_STYLE_GET_DEFAULT        TPIa_widgetStyleGetDefault
#define WE_WIDGET_SOUND_CREATE             TPIa_widgetSoundCreate
#define WE_WIDGET_SOUND_PLAY               TPIa_widgetSoundPlay
#define WE_WIDGET_SOUND_GET_PREDEFINED     TPIa_widgetSoundGetPredefined
#define WE_WIDGET_SOUND_STOP               TPIa_widgetSoundStop

#endif
