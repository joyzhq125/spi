#ifndef __WE_EVENT_H__
#define __WE_EVENT_H__


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
    E_WE_EVENT_SOCKET_SETUP_FAIL,
    
    
    /* event cann't be select*/
    E_WE_EVENT_SOCKET_CONNECT_SUCCESS,
    E_WE_EVENT_SOCKET_CONNECT_FAILED,
    E_WE_EVENT_SOCKET_HOST_BY_NAME,
    E_WE_EVENT_SOCKET_SEND_SUCCESS,
    E_WE_EVENT_SOCKET_SEND_FAIL,
    E_WE_EVENT_SOCKET_SEND_BUSY,
        
    
    /* event cann't be select*/
    E_WE_EVENT_TIMER_TIMEOUT,
    E_WE_EVENT_REG_KEYCHANGED,
    
    E_WE_EVENT_TLS_NOGOTIATE_OK,
    E_WE_EVENT_TLS_NOGOTIATE_ERROR,
 
    E_WE_EVENT_COUNT
}E_WE_EVENT;


#endif
