/*=====================================================================================
    FILE NAME : oem_secevent.h
    MODULE NAME : SEC

    GENERAL DESCRIPTION
        This file declare all event code for WEA module

    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2002 by TECHFAITH Software. All Rights Reserved.
=======================================================================================
    Revision History
    
    Modification Tracking
    Date       Author         Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-06-01  Alan            none        Initialization
    
=====================================================================================*/

/*******************************************************************************
*   multi-Include-Prevent Section
*******************************************************************************/
#ifndef __OEM_SECEVENT_H__
#define __OEM_SECEVENT_H__


/*******************************************************************************
*   macro Define Section
*******************************************************************************/

/*******************************************************************************
*   Type Define Section
*******************************************************************************/
typedef enum{
    E_WE_EVENT_NONE,

    /* file event can be select */
    E_WE_EVENT_FILE_READABLE,

    /* pipe event can be select */
    E_WE_EVENT_PIPE_READABLE,
    E_WE_EVENT_PIPE_WRITEABLE,

    /* socket event can be select */
    E_WE_EVENT_SOCKET_ACCEPT,
    E_WE_EVENT_SOCKET_BIND,
    E_WE_EVENT_SOCKET_CLOSED,    
    E_WE_EVENT_SOCKET_LISTEN,
    E_WE_EVENT_SOCKET_RECV,
    E_WE_EVENT_SOCKET_SEND,
    
    
    /* event cann't be select*/
    E_WE_EVENT_SOCKET_CONNECT_SUCCESS,
    E_WE_EVENT_SOCKET_CONNECT_FAILED,
    E_WE_EVENT_SOCKET_HOST_BY_NAME,
    E_WE_EVENT_SOCKET_SEND_SUCCESS,
    E_WE_EVENT_SOCKET_SEND_FAIL,
        
    
    /* event cann't be select*/
    E_WE_EVENT_TIMER_TIMEOUT,
    E_WE_EVENT_REG_KEYCHANGED,
    
    E_WE_EVENT_COUNT
}E_WE_EVENT;


#endif /*  */
