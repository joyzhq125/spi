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

/* !\file Sauisig.h
 *  \brief Signal handler for Widgets.
 */

#ifndef SAUISIG_H
#define SAUISIG_H

#ifndef SAINTSIG_H
#error Saintsig.h must be included berfore Sauisig.h!
#endif
/******************************************************************************
 * Constants
 *****************************************************************************/

#define SIA_MAX_ACTIVE_WIN      8   /*!< Default max no of windows */
#define SIA_MAX_ACTIVE_ACTIONS  10  /*!< WeOk, WeBack, WeMenu, WeCancel 
                                         and so on  */
#define SIA_WIDGET_NA           0x00 /* !< No callback function or signal is 
                                           available */
#define SIA_WIDGET_CALLBACK     0x01 /* !< The widget signal is converted into
                                           a function callback */
#define SIA_WIDGET_SIGNAL       0x02 /* !< The widget signal is converted into 
                                           an internal signal */


/******************************************************************************
 * Types
 *****************************************************************************/
/*! \enum SiaEventType
 *  Type of event */
typedef enum
{
    SIA_NO_EVENT = 0,               /*!< No event defined */
    SIA_ACTION_EVENT,               /*!< Event is an action */
    SIA_NOTIFICATION_EVENT,         /*!< Event is a notification */
    SIA_USR_EVENT                   /*!< User events  */
}SiaEventType;


/******************************************************************************
 * Prototypes
 *****************************************************************************/

typedef void (*SiaActionCallBack)(WeWindowHandle winHandle);
WeActionHandle smaCreateAction(WeWindowHandle winHandle, 
    WeActionType action, SiaActionCallBack callback, int fsm, int signal,
    WE_UINT32 strId);
WE_BOOL smaDeleteAction(WeActionHandle actionHandle);
WE_BOOL smaDeleteNotification(WE_UINT32 widgetHandle);
WE_BOOL smaDeleteWindow(WeWindowHandle winHandle);
WE_BOOL smaDeleteUserEvent(WeWindowHandle winHandle, WeEventType eventType);
void smaInitWidgetSignalHandler(void);
WE_BOOL smaRegisterAction(WeWindowHandle winHandle, WeActionHandle actionHandle,
    SiaActionCallBack callback, int fsm, int signal);
WE_BOOL smaRegisterNotification(WeWindowHandle winHandle, 
    WeNotificationType notifType, SiaActionCallBack callback, 
    SiaStateMachine fsm, int signal);
WE_BOOL smaRegisterUserEvent(WeWindowHandle winHandle, 
    WeEventType eventType, SiaActionCallBack callback, int fsm, int signal);
void smaTerminateWidgetSignalHandler(void);
void smaWidgetSignalHandler(WE_UINT16 signal, void *p);

WE_BOOL smaRegisterEvent(WE_UINT32 widgetHandle, SiaEventType eventType,
    WeActionHandle actionHandle, WeNotificationType notifType,
    WeEventType usrEvent, SiaActionCallBack callback, int fsm,
    int signal);
WE_BOOL smaCheckWindowFocus(void);

#endif

