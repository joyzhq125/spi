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
 * Pus_If.c
 *
 * Created by Kent Olsson, Wed July 03 18:23:35 2001.
 *
 * Revision history:
 *
 */
#include "Pus_If.h"
#include "we_cfg.h"
#include "we_core.h"
#include "we_mem.h"
#include "we_log.h"



/**********************************************************************
 * API invoked from other modules
 **********************************************************************/

void
PUSif_startInterface (void)
{
  WE_SIGNAL_REG_FUNCTIONS (WE_MODID_PUS, pus_convert, pus_destruct);
}


/**********************************************************************
 * Conversion routines for all signal parameter structs
 **********************************************************************/

int
pus_cvt_open_push_connection (we_dcvt_t *obj, pus_open_push_connection_t* p)
{
  if (!we_dcvt_int16 (obj, &(p->handle)) ||
      !we_dcvt_uint16 (obj, &(p->portNumber)) ||
      !we_dcvt_int16 (obj, &(p->connectionType)) ||
      !we_dcvt_int32 (obj, &(p->networkAccountId))) {
    return FALSE;
  }

  return TRUE;
}

int
pus_cvt_close_push_connection (we_dcvt_t *obj, pus_close_push_connection_t* p)
{
  if (!we_dcvt_int16 (obj, &(p->handle))) {
    return FALSE;
  }

  return TRUE;
}

int
pus_cvt_connection_opened (we_dcvt_t *obj, Pus_Connection_opened_t *p)
{
  if (!we_dcvt_int16 (obj, &(p->handle)) ||
      !we_dcvt_int16 (obj, &(p->errorCode))) {
    return FALSE;
  }

  return TRUE;
}

int
pus_cvt_connection_closed (we_dcvt_t *obj, Pus_Connection_closed_t *p)
{
  if (!we_dcvt_int16 (obj, &(p->handle)) ||
      !we_dcvt_int16 (obj, &(p->errorCode))) {
    return FALSE;
  }

  return TRUE;
}

/**********************************************************************
 * Signal-sending functions
 **********************************************************************/

/* For opening of push connections a PUSif is defined. This interface can be used 
 * to set up new push connections or close existing connections.
 * Push connectins can also be open when starting the push module. 
 * This is handle by CFG parameters in psh_cfg.h. For each of these parameters that
 * is defined as TRUE, a connection will be set up at start up. It's only the 
 * default push connections that can be started at start up of the module.
 * The following three connections are opened by the
 * pus module according to default values in Pus_Cfg.h. 
 * 
 * These connections is only opened if values is stored in the registry at
 * the following places. These valus must be stored in the registry before the PUS
 * module is started!
 *
 * If a value is stored in PUS/DefaultConnections/SMS_NetworkAccount, the following
 * connection will be set up. When asked for bearer for this network account the 
 * answer must be GSM_SMS:
 *  
 * handle = x, portNumber = 2948, connectionType = WSP_CL
 *
 * If a value is stored in PUS/DefaultConnections/UDP_NetworkAccount, the following
 * connection will be set up:
 *  
 * handle = x, portNumber = 2948, connectionType = WSP_CL
 *
 * If a value is stored in PUS/DefaultConnections/HTTP_NetworkAccount, the following
 * connection will be set up:
 *  
 * handle = x, portNumber = 4035, connectionType = HTTP, 
 * 
 */

/*
 * Open a new push connection, that is, listening for push messages 
 * on a new socket, either as a HTTP Server or WSP (UDP or SMS)
 * Parameters are:
 * - modId: the module that sent this signal to the pus-module
 * - handle: Id used to identify this connection (connection ids 253 - 255
 *                 are reserved for internal use by the push application and can
 *                 not be used)
 * - portNumber: the portnumber the terminal should be listening at
 * - connectionType: the type of socket to be set up (UDP, HTTP or SMS)
 * - networkAccountId: Id used to identify teh network account used for this 
 *                     connection.       
 */
 /*
 *port Number:端口号
 *connection type:连接类型UDP HTTP SMS
 *network account:网络号
*/
void
PUSif_openPushConnection (WE_UINT8 modId, int handle,
                          WE_UINT16 portNumber, int connectionType,
                          WE_INT32 networkAccountId)
{
  pus_open_push_connection_t    cpc;
  we_dcvt_t                    cvt_obj;
  void                         *signal_buffer, *user_data;
  WE_UINT16                    length;

  cpc.handle = (WE_INT16)handle;
  cpc.portNumber = portNumber;
  cpc.connectionType  = (WE_INT16)connectionType;
  cpc.networkAccountId  = (WE_INT32)networkAccountId;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  pus_cvt_open_push_connection (&cvt_obj, &cpc);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (PUS_SIG_OPEN_PUSH_CONNECTION, modId,
                                     WE_MODID_PUS, length);
  if (signal_buffer != NULL) 
  {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
    pus_cvt_open_push_connection (&cvt_obj, &cpc);

    WE_SIGNAL_SEND (signal_buffer);
  }
 
}

/*
 * Close a push connection.
 * Parameters are:
 * - modId: the module that sent this signal to the pus-module
 * - handle: Id used to identify this connection. This is the same
 *                 handle that earlier was used in PUSif_createPushConnection
 *                 or one of the default values (253, 254, 255).
 */
void
PUSif_closePushConnection (WE_UINT8 modId, int handle)
{
  pus_close_push_connection_t  cpc;
  we_dcvt_t                   cvt_obj;
  void                        *signal_buffer, *user_data;
  WE_UINT16                   length;

  cpc.handle = (WE_INT16)handle;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  pus_cvt_close_push_connection (&cvt_obj, &cpc);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (PUS_SIG_CLOSE_PUSH_CONNECTION, modId,
                                     WE_MODID_PUS, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, 0);
    pus_cvt_close_push_connection (&cvt_obj, &cpc);

    WE_SIGNAL_SEND (signal_buffer);
  }
}

/*
 * Convert signals owned by the PUS module.
 * Conversion is from signal buffer to signal structs (see above).
 */
void*
pus_convert (WE_UINT8 module, WE_UINT16 signal, void* buffer)
{
  we_dcvt_t  cvt_obj;
  void       *user_data;
  WE_UINT16  length;

  if (buffer == NULL)
    return NULL;

  user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
  if ((user_data == NULL) || (length == 0))
    return NULL;

  we_dcvt_init (&cvt_obj, WE_DCVT_DECODE, user_data, length, module);

  switch (signal){
  case PUS_SIG_OPEN_PUSH_CONNECTION:
    {
      pus_open_push_connection_t *p;

      p = WE_MEM_ALLOCTYPE (module, pus_open_push_connection_t);
      pus_cvt_open_push_connection (&cvt_obj, p);
      
      return (void *)p;
    }
  
  case PUS_SIG_CLOSE_PUSH_CONNECTION:
    {
      pus_close_push_connection_t *p;

      p = WE_MEM_ALLOCTYPE (module, pus_close_push_connection_t);
      pus_cvt_close_push_connection (&cvt_obj, p);
      
      return (void *)p;
    }

  case PUS_SIG_CONNECTION_OPENED:
    {
      Pus_Connection_opened_t *p;

      p = WE_MEM_ALLOCTYPE (module, Pus_Connection_opened_t);
      pus_cvt_connection_opened (&cvt_obj, p);
      
      return (void *)p;
    }

  case PUS_SIG_CONNECTION_CLOSED:
    {
      Pus_Connection_closed_t *p;

      p = WE_MEM_ALLOCTYPE (module, Pus_Connection_closed_t);
      pus_cvt_connection_closed (&cvt_obj, p);
      
      return (void *)p;
    }
  }

  return NULL;
}

void
pus_destruct (WE_UINT8 module, WE_UINT16 signal, void* p)
{
  we_dcvt_t cvt_obj;

  if (p == NULL)
    return;

  we_dcvt_init (&cvt_obj, WE_DCVT_FREE, NULL, 0, module);

  switch (signal){
  case PUS_SIG_OPEN_PUSH_CONNECTION:
    pus_cvt_open_push_connection (&cvt_obj, p);
    break;

  case PUS_SIG_CLOSE_PUSH_CONNECTION:
    pus_cvt_close_push_connection (&cvt_obj, p);
    break;

  case PUS_SIG_CONNECTION_OPENED:
    pus_cvt_connection_opened (&cvt_obj, p);
    break;

  case PUS_SIG_CONNECTION_CLOSED:
    pus_cvt_connection_closed (&cvt_obj, p);
    break;

  }

  WE_MEM_FREE (module, p);
}


/**********************************************************************
 * Logging functions
 **********************************************************************/

