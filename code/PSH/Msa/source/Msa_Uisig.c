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

/* !\file mauisig.c
 *  \brief Signal handler for Widgets.
 */

/* WE */
#include "We_Int.h"
#include "We_Wid.h"
#include "We_Core.h"
#include "We_Log.h"
#include "We_Mem.h"
#include "We_Lib.h"

/* MMS */
#include "Mms_Def.h"

/* MSA */
#include "Msa_Rc.h"
#include "Msa_Def.h"
#include "Msa_Types.h"
#include "Msa_Intsig.h"
#include "Msa_Uisig.h"
#include "Msa_Uicmn.h"
#include "Msa_Core.h"
#include "Msa_Mem.h"
#include "Msa_Uicmn.h"

/******************************************************************************
 * Data-types
 *****************************************************************************/

/*! \struct MsaCallbackSignal
 *  Callback signals for Widget API are saved in the structure */
typedef struct
{
    MsaStateMachine     fsm;    /* !< Callback FSM */
    int                 signal; /* !< Callback signal */
}MsaCallbackSignal;

/*! \enum MsaEventType
 *  Type of event */
typedef enum
{
    MSA_NO_EVENT = 0,               /*!< No event defined */
    MSA_ACTION_EVENT,               /*!< Event is an action */
    MSA_NOTIFICATION_EVENT,         /*!< Event is a notification */
    MSA_USR_EVENT                   /*!< User events  */
}MsaEventType;

/*! \struct MsaEventResponse
 *  Callback/signal information for a response call */
typedef struct 
{
    unsigned char type;                 /*!< MSA_WIDGET_NA,
                                             MSA_WIDGET_CALLBACK or 
                                             MSA_WIDGET_SIGNAL */

    /*!\union callback Callback functions */
    union
    {
        MsaActionCallBack   func; /* !< Callback function */
        MsaCallbackSignal   sig;  /* !< Callback signal */
    }callback;
}MsaEventResponse;

/*! \struct MsaEventItem
 *  List item for action and notification handling */
typedef struct
{
    MsaEventType eventType;             /*!< Type of event (action/notif/
                                             user event)*/
    /*!\union identifier Identification of the action handle or notif type */
    union 
    {
        WeNotificationType notifType;      /*!< Type of notification */
        WeActionHandle     actionHandle;   /*!< Widget action handle */
        WeEventType        usrEventType;   /*!< User event */
    }identifier;
    MsaEventResponse response;          /*!< Response */
}MsaEventItem;

/*! \struct MsaWidgetItem
 *  List item for window/gadget and action/notification/usr event handling */
typedef struct
{
    WE_UINT32 handle; /* !< Widget window/gadget handle */
    MsaEventItem eventList[MSA_MAX_ACTIVE_ACTIONS]; /* !< a list of actions or
                                                            notifications or
                                                            user events
                                                            associated with the 
                                                            window */
}MsaWidgetItem;

/******************************************************************************
 * Static data
 *****************************************************************************/

/* Data-structure for signals originating from widget API */
static MsaWidgetItem *widgetList;
static unsigned int  widgetItemCount;

/******************************************************************************
 * Prototypes
 *****************************************************************************/

static MsaWidgetItem *findWidgetByHandle(WE_UINT32 widgetHandle);
static MsaEventItem *findEventByHandle(WE_UINT32 widgetHandle,
    MsaEventType eventType, WeActionHandle actionHandle, 
    WeNotificationType notifType, WeEventType usrEvent);
static WE_BOOL routeEvent(const MsaEventResponse *response,
                      WE_UINT32 widgetHandle);
static WE_BOOL handleWidgetAction(WeWindowHandle windowHandle, 
    WeActionHandle actionHandle);
static WE_BOOL handleWidgetNotification(WE_UINT32 widgetHandle, 
                       WeNotificationType notifType);
static WE_BOOL handleUsrEvent(WeWindowHandle handle, WeEventType usrEvent, 
    WeEventClass eventClass);
static int findEmptyEventSlot(const MsaEventItem *eventItems);
static void registerEventItem(MsaEventItem *eventItem, 
    MsaActionCallBack callback, MsaStateMachine fsm, int signal);
static void addEventToSlot(MsaEventItem *eventItem, MsaEventType eventType,
    WeActionHandle actionHandle, WeNotificationType notifType, 
    WeEventType usrEvent, MsaActionCallBack callback, MsaStateMachine fsm,
    int signal);

/******************************************************************************
 * Function implementation
 *****************************************************************************/


 /*!\brief Find a widget in widget list by its handle
 *
 * \param widgetHandle The widget handle of interest
 * \return Pointer to window list item or NULL if not found
 *****************************************************************************/
static MsaWidgetItem *findWidgetByHandle(WE_UINT32 widgetHandle)
{
    unsigned int i;

    for (i = 0; i < widgetItemCount; i++)
    {
        if (widgetHandle == widgetList[i].handle)
        {
            return &widgetList[i];
        }
    }
    return NULL;
}

 /*!\brief Find an event that matches the specified widget handle and
           action handle or notification type
 * \param widgetHandle The widget handle of interest (window/gadget)
 * \param eventType    The type of event (action, notif, or user event)
 * \param actionHandle The action handle of interest (only for actions)
 * \param notifType    The notification type of interest (only for notifs)
 * \param usrEvent     The user event type of interest (only for usr events)
 * \return Pointer to the event or NULL if not found
 *****************************************************************************/
static MsaEventItem *findEventByHandle(WE_UINT32 widgetHandle,
    MsaEventType eventType, WeActionHandle actionHandle, 
    WeNotificationType notifType, WeEventType usrEvent)
{
    unsigned int i;
    MsaWidgetItem *widgetItem = findWidgetByHandle(widgetHandle);
    MsaEventItem *eventItem;

    if (NULL == widgetItem)
    {
        return NULL;
    }

    for (i = 0; i < MSA_MAX_ACTIVE_ACTIONS; i++)
    {
        eventItem = &(widgetItem->eventList[i]);
        if (eventType == eventItem->eventType &&
            ((eventType == MSA_ACTION_EVENT && 
            actionHandle == eventItem->identifier.actionHandle) ||
            (eventType == MSA_NOTIFICATION_EVENT &&
            notifType == eventItem->identifier.notifType) ||
            (eventType == MSA_USR_EVENT &&
            usrEvent == eventItem->identifier.usrEventType)))
        {
            return eventItem;
        }
    }
    return NULL;
}

/*!\brief Route event through signal or callback
 *
 * \param response The information about the signal or callback
 * \param windowHandle The widget handle that was affected by the event
 * \return TRUE if the event was handled, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL routeEvent(const MsaEventResponse *response,
    WE_UINT32 widgetHandle)
{
    /* Signal or function call ?? */
    if (MSA_WIDGET_CALLBACK == response->type)
    {
        /* Call callback function */
        response->callback.func(widgetHandle);
        return TRUE;
    }
    else if (MSA_WIDGET_SIGNAL == response->type)
    {
        /* Send signal */
        (void)MSA_SIGNAL_SENDTO(
            (MsaStateMachine)response->callback.sig.fsm, 
            response->callback.sig.signal);
        return TRUE;
    }
    else
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA, 
            "(%s) (%d) unhandled event\n", __FILE__, 
            __LINE__));
    }
    return FALSE;
}


/*!\brief Signal handler for Widget actions.
 *
 * \param windowHandle The window that generated an action.
 * \param actionHandle The handle of the action that was generated.
 * \return TRUE if the action was handled, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL handleWidgetAction(WeWindowHandle windowHandle, 
    WeActionHandle actionHandle)
{
    MsaEventItem *eventItem;

    eventItem = findEventByHandle(windowHandle, MSA_ACTION_EVENT, actionHandle,
        (WeNotificationType)0, (WeEventType)0);

    if (NULL != eventItem)
    {
        return routeEvent(&eventItem->response, windowHandle);        
    }
    return FALSE;    
}

/*!\brief Signal handler for widget notifications.
 *
 * \param widgetHandle The widget that generated a notification.
 * \param notifType The type of notification that was generated.
 * \return TRUE if the notification was handled, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL handleWidgetNotification(WE_UINT32 widgetHandle, 
    WeNotificationType notifType)
{
    MsaEventItem *eventItem;

    if (NULL != (eventItem = findEventByHandle(widgetHandle,
        MSA_NOTIFICATION_EVENT, (WeNotificationType) 0, notifType, 
        (WeEventType)0)))
    {
        return routeEvent(&eventItem->response, widgetHandle);        
    }
    return FALSE;    
}

/*!\brief Signal handler for widget user events.
 *
 * \param widgetHandle The widget that generated a user event.
 * \param usrEvent The type of user event that was generated.
 * \param eventClass The even class, see #WeEventClass.
 * \return TRUE if the user event was handled, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL handleUsrEvent(WeWindowHandle handle, WeEventType usrEvent, 
    WeEventClass eventClass)
{
    MsaEventItem *eventItem;

    if ((eventClass == WePress) && (NULL != (eventItem = 
        findEventByHandle(handle, MSA_USR_EVENT, 0, (WeNotificationType)0, 
        usrEvent))))
    {
        return routeEvent(&eventItem->response, handle);
    }
    return FALSE;
}

/*!\brief Initiates data structes for the MH GUI.
 *
 *****************************************************************************/

void msaInitWidgetSignalHandler(void)
{
    /* Allocate signal handling buffer */
    widgetItemCount = MSA_MAX_ACTIVE_WIN;
    widgetList = MSA_ALLOC(widgetItemCount*sizeof(MsaWidgetItem));
    /* Set all data to zero */
    memset(widgetList, 0, sizeof(MsaWidgetItem)*widgetItemCount);
}

/*!\brief Initiates data structes for the MH GUI.
 *
 *****************************************************************************/

void msaTerminateWidgetSignalHandler(void)
{
    /* Allocate signal handling buffer */
    MSA_FREE(widgetList);
    widgetList = NULL;
    widgetItemCount = 0;
}


/*!\brief Delete a window and its actions.
 *
 * \param winHandle The handle of the window (with attached actions) to delete.
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaDeleteWindow(WeWindowHandle winHandle)
{
    unsigned int i;
    unsigned int j;

    if (0 == winHandle)
    {
        return TRUE;
    }
    for (i = 0; i < widgetItemCount; i++)
    {
        if (winHandle == widgetList[i].handle)
        {
            /* Delete window */
            (void)WE_WIDGET_REMOVE(msaGetScreenHandle(), 
                widgetList[i].handle);
            (void)WE_WIDGET_RELEASE(widgetList[i].handle);
            /* Delete all actions */
            for (j = 0; j < MSA_MAX_ACTIVE_ACTIONS; j++)
            {
                if (MSA_ACTION_EVENT == widgetList[i].eventList[j].eventType &&
                    0 != widgetList[i].eventList[j].identifier.actionHandle)
                {
                    (void)WE_WIDGET_RELEASE(
                        widgetList[i].eventList[j].identifier.actionHandle);
                }
            }
            /* Delete obsolete data*/
            memset(&widgetList[i], 0, sizeof(MsaWidgetItem));
            return TRUE;
        }
    }
    return FALSE;
}

/*!\brief Deletes a specific action.
 *
 * \param actionHandle The action to delete.
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaDeleteAction(WeActionHandle actionHandle)
{
    unsigned int i, j;
    for (i = 0; i < widgetItemCount; i++)
    {
        if (0 != widgetList[i].handle)
        {
            for (j = 0; j < MSA_MAX_ACTIVE_ACTIONS; j++)
            {
                if (MSA_ACTION_EVENT == widgetList[i].eventList[j].eventType &&
                    actionHandle == 
                    widgetList[i].eventList[j].identifier.actionHandle)
                {
                    (void)WE_WIDGET_RELEASE(
                        widgetList[i].eventList[j].identifier.actionHandle);
                    /* Delete obsolete data */
                    memset(&widgetList[i].eventList[j], 0, 
                        sizeof(MsaEventItem));
                    return TRUE;
                }

            }
        }
    }
    return FALSE;
}

/*!\brief Deletes requests for notifications for a specific widget
 *
 * \param widgetHandle   The widget for which notifications should no longer
                         be captured
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaDeleteNotification(WE_UINT32 widgetHandle)
{
    MsaWidgetItem *widget;
    int i;
    WE_BOOL hasOther;

    widget = findWidgetByHandle(widgetHandle);

    if (NULL == widget)
    {
        return FALSE;
    }

    /* clean all notification instances for this widget */
    hasOther = FALSE;
    for (i = 0; i < MSA_MAX_ACTIVE_ACTIONS; i++)
    {
        if (MSA_NOTIFICATION_EVENT == widget->eventList[i].eventType)
        {
            (void)WE_WIDGET_HANDLE_WE_NOTIFICATION(widgetHandle, 
                WE_MODID_MSA, widget->eventList[i].identifier.notifType, FALSE);
            memset(&widget->eventList[i], 0, sizeof(MsaEventItem));
        }
        else if (MSA_NO_EVENT != widget->eventList[i].eventType)
        {
            hasOther = TRUE;
        }
    }

    /* if there were only notification instances, clean the widget itself */
    if (!hasOther)
    {
        widget->handle = 0;
    }
    return TRUE;
}

/*!\brief Deletes requests for a registered user event
 *
 * \param winHandle The window for which the event should be removed
 * \param eventType The kind of event type to register, see #WeEventType.
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaDeleteUserEvent(WeWindowHandle winHandle, WeEventType eventType)
{
    MsaEventItem *event;
    
    event = findEventByHandle(winHandle, MSA_USR_EVENT, 0, 
        (WeNotificationType)0,  eventType);
      
    if (NULL == event)
    {
        return FALSE;
    }

    (void)WE_WIDGET_HANDLE_WE_EVT(winHandle, eventType, 0, 0); /* Q04A change */
    memset(event, 0, sizeof(MsaEventItem));
    return TRUE;
}


/*!\brief Finds a empty slot for inserting a new event.
 *
 * \param eventItems A pointer to a list (MSA_MAX_ACTIVE_ACTIONS items) of
 *                    actions.
 * \return The index of the empty slot or -1 if no slot could be found.
 *****************************************************************************/
static int findEmptyEventSlot(const MsaEventItem *eventItems)
{
    int i;
    for(i = 0; i < MSA_MAX_ACTIVE_ACTIONS; i++)
        if (MSA_NO_EVENT == eventItems[i].eventType)
            return i;
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA,
            "findEmptyEventSlot - no empty slots"));
    return -1;
}

/*!
 * \brief Registers an event into the widget event handler
 *
 * \param eventItem A pointer to a valid msaEventItem structure.
 * \param callback  A callback function that is called when an event is 
 *                  generated or NULL if no callback is used.
 * \param fsm       The fsm to send a signal. The fsm is only valid if callback
 *                  is NULL.
 * \param signal    The signal to send. The signal is only valid if callback is
 *                  NULL.
 *****************************************************************************/
static void registerEventItem(MsaEventItem *eventItem, 
    MsaActionCallBack callback, MsaStateMachine fsm, int signal)
{
    /* Check if callback should be used */
    if (NULL != callback)
    {
        /* Register callback func */
        eventItem->response.callback.func = callback;
        eventItem->response.type         = MSA_WIDGET_CALLBACK;
    }
    else
    {
        /* Register callback signal */
        eventItem->response.callback.sig.fsm    = fsm;
        eventItem->response.callback.sig.signal = signal;
        eventItem->response.type                = MSA_WIDGET_SIGNAL;
    }
}

/*!\brief Creates a new action, binds the action to a window and register the
 *        action in the signal dispatcher for widget actions.
 *
 * \param winHandle The window to bind the action to.
 * \param action    The action to bind.
 * \param callback  A callback function that is called when an action is 
 *                  generated or NULL if no callback is used.
 * \param fsm       The fsm to send a signal. The fsm is only valid if callback
 *                  is NULL.
 * \param signal    The signal to send. The signal is only valid if callback is 
 *                  NULL.
 * \param strId     The label for the action.
 * \return          The new action handle or 0 if something went wrong.
 *****************************************************************************/
WeActionHandle msaCreateAction(WeWindowHandle winHandle, 
    WeActionType action, MsaActionCallBack callback, MsaStateMachine fsm, 
    int signal, WE_UINT32 strId)
{
    WeActionHandle actionHandle;
    actionHandle = WE_WIDGET_ACTION_CREATE(WE_MODID_MSA,
        (strId ? MSA_GET_STR_ID(strId) : 0), 0, action); /* Q04A */
    if (0 == actionHandle)
    {
        return 0;
    }
    if (0 > WE_WIDGET_ADD_ACTION(winHandle, actionHandle))
    {
        (void)WE_WIDGET_RELEASE(actionHandle);
        return 0;
    }
    if (msaRegisterAction(winHandle, actionHandle, callback, fsm, signal))
    {
        return actionHandle;
    }
    else
    {
        (void)WE_WIDGET_RELEASE(actionHandle);
        return 0;
    }
}

/*!\brief Registers an event into the widget signal dispatcher.
 *
 * \param eventItem The event item to store in
 * \param eventType Type of event to dispatch (notification or action)
 * \param actionHandle A handle to the action to register. (0 for notifs)
 * \param notifType The notification type to register (0 for actions)
 * \param callback  A callback function that is called when an event is 
 *                  generated or NULL if no callback is used.
 * \param fsm The fsm to send a signal. 
 * \param signal The signal to send. 
 * \return TRUE if successful, otherwise FALSE..
 *****************************************************************************/
static void addEventToSlot(MsaEventItem *eventItem, MsaEventType eventType,
    WeActionHandle actionHandle, WeNotificationType notifType, 
    WeEventType usrEvent, MsaActionCallBack callback, MsaStateMachine fsm,
    int signal)
{
    eventItem->eventType = eventType;
    
    switch (eventType)
    {
    case MSA_ACTION_EVENT:
        eventItem->identifier.actionHandle = actionHandle;
        break;
    case MSA_NOTIFICATION_EVENT:
        eventItem->identifier.notifType = notifType;
        break;
    case MSA_USR_EVENT:
        eventItem->identifier.usrEventType = usrEvent;
        break;
    case MSA_NO_EVENT:
    default:
        break;
    }

    registerEventItem(eventItem, callback, fsm, signal);
}


/*!\brief Registers an event into the widget signal dispatcher.
 *
 * \param widgetHandle The widget to bind the event to.
 * \param eventType Type of event to dispatch (notification or action)
 * \param actionHandle A handle to the action to register. (only for actions)
 * \param notifType The notification type to register (only for notifs)
 * \param usrEvent The user event to register (only for usr events)
 * \param callback A callback function that is called when an event is 
 *                 generated or NULL if no callback is used.
 * \param fsm The fsm to send a signal. 
 * \param signal The signal to send. 
 * \return TRUE if successful, otherwise FALSE..
 *****************************************************************************/
static WE_BOOL msaRegisterEvent(WE_UINT32 widgetHandle, MsaEventType eventType,
    WeActionHandle actionHandle, WeNotificationType notifType,
    WeEventType usrEvent, MsaActionCallBack callback, MsaStateMachine fsm,
    int signal)
{
    MsaWidgetItem *list;
    unsigned int i;
    int sIndex;

    /* only add supported events */
    if ((eventType != MSA_ACTION_EVENT) && 
        (eventType != MSA_NOTIFICATION_EVENT) &&
        (eventType != MSA_USR_EVENT))
    {
        return FALSE;
    }

    /* Check if the widget is registered */
    for (i = 0; i < widgetItemCount; i++)
    {
        if (widgetHandle == widgetList[i].handle)
        {
            sIndex = findEmptyEventSlot(widgetList[i].eventList);
            if (sIndex >= 0)
            {
                addEventToSlot(&widgetList[i].eventList[sIndex],
                    eventType, actionHandle, notifType, usrEvent,
                    callback, fsm, signal);
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
    }
    /* The window needs to be registered */
    for (i = 0; i < widgetItemCount; i++)
    {
        /* Check if the slot is empty */
        if (0 == widgetList[i].handle)
        {
            widgetList[i].handle = widgetHandle;
            sIndex = findEmptyEventSlot(widgetList[i].eventList);
            if (sIndex >= 0)
            {
                addEventToSlot(&widgetList[i].eventList[sIndex],
                    eventType, actionHandle, notifType, usrEvent,
                    callback, fsm, signal);
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
    }
    /* If there is no empty slot get a larger buffer */
    list = MSA_ALLOC((widgetItemCount+1)*sizeof(MsaWidgetItem));
    memset(list, 0, (widgetItemCount+1)*sizeof(MsaWidgetItem));
    /* copy old window and action handles*/
    memcpy(list, widgetList, widgetItemCount*sizeof(MsaWidgetItem));
    MSA_FREE(widgetList);
    widgetList = list;
    ++widgetItemCount;
    /* register the new action */
    widgetList[i].handle = widgetHandle;
    addEventToSlot(&widgetList[i].eventList[0],
        eventType, actionHandle, notifType, usrEvent,
        callback, fsm, signal);  
    return TRUE;
}

/*!\brief Registers an action into the widget signal dispatcher.
 *
 * \param winHandle The window to bind the action to.
 * \param actionHandle A handle to the action to register.
 * \param callback A callback function that is called when an action is 
 *                 generated or NULL if no callback is used.
 * \param fsm The fsm to send a signal. 
 * \param signal The signal to send. 
 * \return TRUE if successful, otherwise FALSE..
 *****************************************************************************/
WE_BOOL msaRegisterAction(WeWindowHandle winHandle, 
    WeActionHandle actionHandle, MsaActionCallBack callback, 
    MsaStateMachine fsm, int signal)
{
    return msaRegisterEvent(winHandle, MSA_ACTION_EVENT, actionHandle, 
        (WeNotificationType)0, (WeEventType)0,  callback, fsm, signal);
}

/*!\brief Registers a notification into the widget signal dispatcher.
 *
 * \param winHandle The window to bind the notification to.
 * \param notifType The kind of notification to register. 
 * \param callback A callback function that is called when the notif is 
 *                 generated or NULL if no callback is used.
 * \param fsm The fsm to send a signal. 
 * \param signal The signal to send. 
 * \return TRUE if successful, otherwise FALSE..
 *****************************************************************************/
WE_BOOL msaRegisterNotification(WeWindowHandle winHandle, 
    WeNotificationType notifType, MsaActionCallBack callback, 
    MsaStateMachine fsm, int signal)
{
    /* Try to register notification */
    if (WE_WIDGET_HANDLE_WE_NOTIFICATION(winHandle, WE_MODID_MSA, 
        notifType, TRUE) < 0) 
    {
        return FALSE;
    }
    if (!msaRegisterEvent(winHandle, MSA_NOTIFICATION_EVENT, 0, notifType, 
        (WeEventType)0, callback, fsm, signal) < 0)
    {
        (void)WE_WIDGET_HANDLE_WE_NOTIFICATION(winHandle, WE_MODID_MSA, 
            notifType, FALSE); 
        return FALSE;
    }
    return TRUE;
}

/*!\brief Registers a User Event into the widget signal dispatcher.
 *
 * \param winHandle The window to bind the user event to.
 * \param eventType The kind of event type to register, see #WeEventType.
 * \param callback A callback function that is called when the event is 
 *                 generated or NULL if no callback is used.
 * \param fsm The fsm to send a signal. 
 * \param signal The signal to send. 
 * \return TRUE if successful, otherwise FALSE..
 *****************************************************************************/
WE_BOOL msaRegisterUserEvent(WeWindowHandle winHandle, 
    WeEventType eventType, MsaActionCallBack callback, MsaStateMachine fsm,
    int signal)
{
    if (WE_WIDGET_HANDLE_WE_EVT(winHandle, eventType, 1, 1) < 0) /* Q04A change */
        
    {
        return FALSE;
    }
    if (!msaRegisterEvent(winHandle, MSA_USR_EVENT, 0, (WeNotificationType)0, 
        eventType, callback, fsm, signal))
    {
        (void)WE_WIDGET_HANDLE_WE_EVT(winHandle, eventType, 0, 0);/* Q04A change */
        
        return FALSE;
    }
    return TRUE;
}

/*!
 * \brief The signal dispatcher for Widgets.
 *
 * When a new external signal is received it needs to be converted into a 
 * internal signal. This function converts an external signal to a internal
 * one.
 *
 * \param signal The type of external signal that was received.
 * \param p A pointer to the data attached to the signal, this is dependent on
 *          the signal.
 *****************************************************************************/
void msaWidgetSignalHandler(WE_UINT16 signal, void *p)
{
    we_widget_action_t *was;
    we_widget_notify_t *wns;
    we_widget_userevt_t *ues;
    switch(signal)
    {
    case WE_SIG_WIDGET_ACTION:
        was = (we_widget_action_t*) p;
        /* Convert the external action signal into a internal */
        if (handleWidgetAction(was->handle, was->action))
        {
            break;
        }
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "msaWidgetSignalHandler() - signal not handled\n"));
        break;
    case WE_SIG_WIDGET_NOTIFY:
        wns = (we_widget_notify_t*)p;
        /* First check if any handlers have been registered for this 
           window/notification type combo */
        if (handleWidgetNotification(wns->handle, wns->notificationType))
        {
            break;
        }
        break;
    case WE_SIG_WIDGET_USEREVT:
        ues = (we_widget_userevt_t *)p;
        if (!handleUsrEvent(ues->handle, ues->event.eventType, 
            ues->event.eventClass))
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) Unhandled widget user event signal!\n", __FILE__, 
                __LINE__));
        }
        break;
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "msaWidgetSignalHandler() Unhandled signal from WE Widget\n"));
        break;
    }
}


/*! \brief Checks if any window has focus
 *
 * \retrun TRUE if any window has focus, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaCheckWindowFocus(void)
{
    unsigned int i;
    unsigned int j;
    MsaWidgetItem *list = widgetList;

    if (NULL == list)
    {
        return FALSE;
    }
    /* Go through all window/gadgets */
    for (i = 0; i < widgetItemCount; i++)
    {
        if (0 != list[i].handle)
        {
            /* Check if there are any valid actions */
            for(j = 0; j < MSA_MAX_ACTIVE_ACTIONS; j++)
            {
                if (MSA_ACTION_EVENT == list[i].eventList[j].eventType)
                {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}
