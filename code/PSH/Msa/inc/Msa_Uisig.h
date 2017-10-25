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

/* !\file mauisig.h
 *  \brief Signal handler for Widgets.
 */

#ifndef _MAUBSIG_H_
#define _MAUBSIG_H_

/******************************************************************************
 * Constants
 *****************************************************************************/

#define MSA_MAX_ACTIVE_WIN      8   /*!< Default max no of windows */
#define MSA_MAX_ACTIVE_ACTIONS  10  /*!< WeOk, WeBack, WeMenu, WeCancel 
                                         and so on  */
#define MSA_WIDGET_NA           0x00 /* !< No callback function or signal is 
                                           available */
#define MSA_WIDGET_CALLBACK     0x01 /* !< The widget signal is converted into
                                           a function callback */
#define MSA_WIDGET_SIGNAL       0x02 /* !< The widget signal is converted into 
                                           an internal signal */

/******************************************************************************
 * Prototypes
 *****************************************************************************/

typedef void (*MsaActionCallBack)(WeWindowHandle winHandle);
WeActionHandle msaCreateAction(WeWindowHandle winHandle, 
    WeActionType action, MsaActionCallBack callback, MsaStateMachine fsm, 
    int signal, WE_UINT32 strId);
WE_BOOL msaDeleteAction(WeActionHandle actionHandle);
WE_BOOL msaDeleteNotification(WE_UINT32 widgetHandle);
WE_BOOL msaDeleteWindow(WeWindowHandle winHandle);
WE_BOOL msaDeleteUserEvent(WeWindowHandle winHandle, WeEventType eventType);
void msaInitWidgetSignalHandler(void);
WE_BOOL msaRegisterAction(WeWindowHandle winHandle, WeActionHandle actionHandle,
    MsaActionCallBack callback, MsaStateMachine fsm, int signal);
WE_BOOL msaRegisterNotification(WeWindowHandle winHandle, 
    WeNotificationType notifType, MsaActionCallBack callback, 
    MsaStateMachine fsm, int signal);
WE_BOOL msaRegisterUserEvent(WeWindowHandle winHandle, 
    WeEventType eventType, MsaActionCallBack callback, MsaStateMachine fsm,
    int signal);
WE_BOOL msaCheckWindowFocus(void);
void msaTerminateWidgetSignalHandler(void);
void msaWidgetSignalHandler(WE_UINT16 signal, void *p);

#endif

