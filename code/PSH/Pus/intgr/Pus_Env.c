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
 * Pus_Env.c
 *
 * Created by Kent Olsson, Wed July 03 18:23:35 2001.
 *
 * Revision history:
 *
 */

#include "we_cfg.h"
#include "we_core.h"
#include "we_mem.h"
#include "we_cmmn.h"
#include "we_log.h"
#include "we_def.h"
#include "we_errc.h"
#include "we_pipe.h"
#include "we_file.h"

#include "Pus_Env.h"
#include "Pus_Main.h"
#include "Pus_If.h"
#include "Pus_Sig.h"

#include "stk_if.h"
#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES 
#include "ubs_if.h"
#endif

/**********************************************************************
 * Function declarations
 **********************************************************************/

static void
pus_receive (WE_UINT8 src_module, WE_UINT16 signal, void* p);


/**********************************************************************
 * Function definitions
 **********************************************************************/

void
PUSc_start (void)
{
  pus_inside_run = FALSE;
  pus_init ();
  WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_PUS, "PUS: Initialised\n"));
}

void
PUSc_run (void)
{
  if (setjmp (pus_jmp_buf) == 0) {
    pus_inside_run = TRUE;
    while (WE_SIGNAL_RETRIEVE (WE_MODID_PUS, pus_receive));

    Pus_Signal_process ();
    pus_inside_run = FALSE;
  }
  else {
    /* A longjmp from pus_exception_handler. If we are here there is no more
     * memory, and the situation could not be worse.
     * Send out an error and terminate.
     */
    WE_ERROR (WE_MODID_PUS, WE_ERR_MODULE_OUT_OF_MEMORY);
    WE_PIPE_CLOSE_ALL (WE_MODID_PUS);
    WE_FILE_CLOSE_ALL (WE_MODID_PUS);
    /* WE_SOCKET_CLOSE_ALL (WE_MODID_PUS); PUS open sconenctions, but use sockets via STK 
                                             should this be called or not ????*/
    WE_MEM_FREE_ALL (WE_MODID_PUS);
    WE_MODULE_IS_TERMINATED (WE_MODID_PUS);
    WE_SIGNAL_DEREGISTER_QUEUE (WE_MODID_PUS);
    WE_KILL_TASK (WE_MODID_PUS);
  }
}

int
PUSc_wantsToRun (void)
{
  return ((WE_SIGNAL_QUEUE_LENGTH (WE_MODID_PUS) > 0) ||
          !Pus_Signal_queue_is_empty ());
}

void
PUSc_kill (void)
{
  /*WE_PIPE_CLOSE_ALL (WE_MODID_PUS);         
  WE_FILE_CLOSE_ALL (WE_MODID_PUS);        
  WE_WIDGET_RELEASE_ALL (WE_MODID_PUS);     */
  WE_MEM_FREE_ALL (WE_MODID_PUS);
  WE_KILL_TASK (WE_MODID_PUS);
}


/*
 * Place a received external signal in the internal queue
 * of the appropriate module.
 */
static void
pus_receive (WE_UINT8 srcModule, WE_UINT16 signal, void* p)
{
  switch (signal) 
  {

/**********************************************************************
 * Signals from the WE
 **********************************************************************/
  case WE_SIG_MODULE_EXECUTE_COMMAND:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,
                 "PUS: Received WE_SIG_MODULE_EXECUTE_COMMAND\n"));
    PUS_MEM_FREE (p);
    break;


  case WE_SIG_MODULE_TERMINATE:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,
                 "PUS: Received WE_SIG_MODULE_TERMINATE\n"));
    PUS_MEM_FREE (p);
    pus_terminate ();
    break;
      
  case WE_SIG_MODULE_STATUS:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,
                 "PUS: Received WE_SIG_MODULE_STATUS\n"));
    pus_stk_status (p);
    break;
    
  case WE_SIG_CONTENT_SEND_ACK:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,
                 "PUS: Received WE_SIG_CONTENT_SEND_ACK\n"));
    PUS_SIGNAL_SENDTO_P (PUS_MODULE_MAIN, PUS_MAIN_SIG_RESPONSE_FROM_CONTENT_ROUTING, p);
    PUS_MEM_FREE (p);
    break;

  /* Registry */
  case WE_SIG_REG_RESPONSE:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,
                "PUS: Received WE_SIG_REG_RESPONSE\n"));
    pus_reg_response(p);
    break;

  case WE_SIG_FILE_NOTIFY:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,
                  "PUS: Received WE_SIG_FILE_NOTIFY\n"));
    pus_handle_file_notification (((we_file_notify_t *)p)->fileHandle,
                                  ((we_file_notify_t *)p)->eventType);
    PUS_MEM_FREE (p);
    break;

  case WE_SIG_PIPE_NOTIFY:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,
                  "PUS: Received WE_SIG_PIPE_NOTIFY\n"));
    pus_handle_pipe_notification (((we_pipe_notify_t *)p)->handle,
                                  ((we_pipe_notify_t *)p)->eventType);
    PUS_MEM_FREE (p);
    break;

  case WE_SIG_WIDGET_ACTION:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,
                  "PUS: Received WE_SIG_WIDGET_ACTION\n"));
    PUS_SIGNAL_SENDTO_P (PUS_MODULE_MAIN, PUS_MAIN_SIG_WIDGET_ACTION, p);
    break;

  /* The respons to a DNS lookup */
  case WE_SIG_SOCKET_HOST_BY_NAME:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,
                  "PUS: Received WE_SIG_SOCKET_HOST_BY_NAME\n"));
    PUS_SIGNAL_SENDTO_P (PUS_MODULE_MAIN, PUS_MAIN_SIG_HOST_BY_NAME, p);
    break;

  /* The SIR lockout timer has expired */
  case WE_SIG_TIMER_EXPIRED:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,
                  "PUS: Received WE_SIG_TIMER_EXPIRED\n"));
    sir_lockout_timer = FALSE;
    break;

/**********************************************************************
 * Signals corresponding to the Signal API
 **********************************************************************/      
  case PUS_SIG_OPEN_PUSH_CONNECTION:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,
                 "PUS: Received PUS_SIG_OPEN_PUSH_CONNECTION\n"));
    PUS_SIGNAL_SENDTO_UP (PUS_MODULE_MAIN, PUS_MAIN_SIG_OPEN_PUSH_CONNECTION,
                          srcModule, p);
    break;

  case PUS_SIG_CLOSE_PUSH_CONNECTION:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,
                 "PUS: Received PUS_SIG_CLOSE_PUSH_CONNECTION\n"));
    PUS_SIGNAL_SENDTO_IU (PUS_MODULE_MAIN, PUS_MAIN_SIG_CLOSE_PUSH_CONNECTION,
                          ((pus_close_push_connection_t*)p)->handle, srcModule);
    PUS_MEM_FREE (p);
    break;
    
/**********************************************************************
 * Signals from the STK
 **********************************************************************/
  case STK_SIG_CONNECTION_CREATED:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,
                 "PUS: Received STK_SIG_CONNECTION_CREATED\n"));
    PUS_SIGNAL_SENDTO_P (PUS_MODULE_MAIN, PUS_MAIN_SIG_STK_CONNECTION_CREATED, p);
    break;

  case STK_SIG_CONNECTION_CLOSED:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,
                 "PUS: Received STK_SIG_CONNECTION_CLOSED\n"));
    PUS_SIGNAL_SENDTO_P (PUS_MODULE_MAIN, PUS_MAIN_SIG_STK_CONNECTION_CLOSED, p);
    break;

  case STK_SIG_RECV_REQUEST:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,
                 "PUS: Received STK_SIG_RECV_REQUEST\n"));
    PUS_SIGNAL_SENDTO_P (PUS_MODULE_MAIN, PUS_MAIN_SIG_RECV_REQUEST, p);
    break;

  case STK_SIG_HTTP_REPLY: /* only used for SL Cache, when url should be loaded into cache */
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,
                 "PUS: Received STK_SIG_HTTP_REPLY\n"));
    PUS_SIGNAL_SENDTO_P (PUS_MODULE_MAIN, PUS_MAIN_SIG_HTTP_REPLY, p);
    break;
    
/**********************************************************************
 * Signals from the Inbox (UBS)
 **********************************************************************/
#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES 
  case UBS_SIG_REPLY_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,
                 "PUS: Received UBS_SIG_REPLY_MSG\n"));
    PUS_SIGNAL_SENDTO_P (PUS_MODULE_MSG, PUS_MSG_SIG_UBS_MESSAGE_DATA, p);
    break;
#endif
    
  default:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_PUS,
                 "PUS: Received unknown signal (%04x)\n", signal));
    /* This was not a signal that we are interested in */
    WE_SIGNAL_DESTRUCT (WE_MODID_PUS, signal, p);
    break;
  }
}
