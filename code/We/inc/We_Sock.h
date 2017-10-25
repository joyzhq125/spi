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
 * We_Sock.h
 *
 * Created by Anders Edenwbandt, Tue Apr 30 16:41:33 2002.
 *
 * Revision  history:
 *   020905, AED: Added error codes
 *   021014, IPN: Added WE_SOCKET_CLOSE_ALL
 *
 */
#ifndef _we_sock_h
#define _we_sock_h

#ifndef _we_int_h
#include "We_Int.h"
#endif


/**********************************************************************
 * Constants
 **********************************************************************/

#define WE_SOCKET_DEFAULT_NID          -1

/* Socket types */
#define WE_SOCKET_TYPE_UDP             TPI_SOCKET_TYPE_UDP
#define WE_SOCKET_TYPE_TCP             TPI_SOCKET_TYPE_TCP
#define WE_SOCKET_TYPE_SMS_DATA        TPI_SOCKET_TYPE_SMS_DATA
#define WE_SOCKET_TYPE_SMS_RAW         TPI_SOCKET_TYPE_SMS_RAW

/* Bearers */
#define WE_SOCKET_BEARER_IP_ANY        TPI_SOCKET_BEARER_IP_ANY
#define WE_SOCKET_BEARER_GSM_SMS       TPI_SOCKET_BEARER_GSM_SMS
#define WE_SOCKET_BEARER_GSM_CSD       TPI_SOCKET_BEARER_GSM_CSD
#define WE_SOCKET_BEARER_GSM_GPRS      TPI_SOCKET_BEARER_GSM_GPRS
#define WE_SOCKET_BEARER_BT            TPI_SOCKET_BEARER_BT
#define WE_SOCKET_BEARER_ANY           TPI_SOCKET_BEARER_ANY

/* Notification types */
#define WE_SOCKET_EVENT_ACCEPT         TPI_SOCKET_EVENT_ACCEPT
#define WE_SOCKET_EVENT_CLOSED         TPI_SOCKET_EVENT_CLOSED
#define WE_SOCKET_EVENT_RECV           TPI_SOCKET_EVENT_RECV
#define WE_SOCKET_EVENT_SEND           TPI_SOCKET_EVENT_SEND

/* Error codes */
#define WE_SOCKET_ERROR_BAD_ID                   TPI_SOCKET_ERROR_BAD_ID
#define WE_SOCKET_ERROR_INVALID_PARAM            TPI_SOCKET_ERROR_INVALID_PARAM
#define WE_SOCKET_ERROR_MSG_SIZE                 TPI_SOCKET_ERROR_MSG_SIZE
#define WE_SOCKET_ERROR_DELAYED                  TPI_SOCKET_ERROR_DELAYED
#define WE_SOCKET_ERROR_RESOURCE_LIMIT           TPI_SOCKET_ERROR_RESOURCE_LIMIT
#define WE_SOCKET_ERROR_CONNECTION_FAILED        TPI_SOCKET_ERROR_CONNECTION_FAILED
#define WE_SOCKET_ERROR_HOST_NOT_FOUND           TPI_SOCKET_ERROR_HOST_NOT_FOUND
#define WE_SOCKET_ERROR_CLOSED                   TPI_SOCKET_ERROR_CLOSED


/**********************************************************************
 * API Functions
 **********************************************************************/

#define WE_SOCKET_CREATE               TPIa_SocketCreate
#define WE_SOCKET_CLOSE                TPIa_SocketClose
#define WE_SOCKET_ACCEPT               TPIa_SocketAccept
#define WE_SOCKET_BIND                 TPIa_SocketBind
#define WE_SOCKET_CONNECT              TPIa_SocketConnect
#define WE_SOCKET_GET_NAME             TPIa_SocketGetName
#define WE_SOCKET_LISTEN               TPIa_SocketListen
#define WE_SOCKET_RECV                 TPIa_SocketRecv
#define WE_SOCKET_RECV_FROM            TPIa_SocketRecvFrom
#define WE_SOCKET_SEND                 TPIa_SocketSend
#define WE_SOCKET_SEND_TO              TPIa_SocketSendTo
#define WE_SOCKET_SELECT               TPIa_SocketSelect
#define WE_SOCKET_CLOSE_ALL            TPIa_SocketCloseAll
#define WE_SOCKET_GET_HOST_BY_NAME     TPIa_SocketGetHostByName

#endif
