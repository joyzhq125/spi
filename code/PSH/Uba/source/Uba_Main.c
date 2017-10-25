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
 * uba_main.c
 *
 * This file contain the WE Browser Applications main functionality
 * such as Initialization and Termination functions. State handlers and the
 * internal signal receive funtion.
 *
 */

#include "Uba_Main.h"

#include "Uba_Sig.h"
#include "Uba_Vrsn.h"
#include "Uba_Cfg.h"
#include "Uba_Data.h"
#include "Uba_Rce.h"

#include "Ubs_If.h"

#include "We_Core.h"
#include "We_Log.h"
#include "We_Wid.h"

/************************************************************************/
/* Typdefs                                                              */
/************************************************************************/
typedef enum
{
  UBA_DELETE_MSG_STATE_WATING_FOR_RESPONSE,
  UBA_DELETE_MSG_STATE_WANTS_TO_RUN
} uba_delete_msg_state_t;

typedef struct uba_main_delete_msg_st{
  struct uba_main_delete_msg_st* next;
  uba_delete_msg_state_t         state;
  int                            msgType;
  ubs_msg_handle_t               handle;
}uba_main_delete_msg_t;
/**********************************************************************
 * Exported global variables
 **********************************************************************/

jmp_buf uba_jmp_buf;
int     uba_inside_run;
uba_state_t uba_main_state;


/************************************************************************/
/* Global variables                                                     */
/************************************************************************/

static uba_main_delete_msg_t* uba_main_delete_msg_queue     = NULL;
static uba_main_delete_msg_t* uba_main_delete_msg_queue_end = NULL;
/************************************************************************/
/* Macros                                                               */
/************************************************************************/

#define UBA_LOG_SIGNAL(signame)                      \
  WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBA, \
  "UBA: received signal " #signame "\n"))

/**********************************************************************
 * Forward declarations of local functions:
 **********************************************************************/

static void
uba_main (uba_signal_t *sig);

static void
uba_memory_exception_handler (void);

static void
uba_main_init (void);

static void
uba_main_terminate (void);

/**********************************************************************
 * External functions
 **********************************************************************/

/*
 *  Initiates the UBA module.
 *  Starts the components RCE and data.
 */
void
uba_init (void)
{
  WE_MEM_INIT (WE_MODID_UBA, uba_memory_exception_handler, FALSE);
  WE_SIGNAL_REGISTER_QUEUE (WE_MODID_UBA);
  WE_MODULE_IS_CREATED (WE_MODID_UBA, UBA_VERSION);

  UBSif_startInterface ();
  /* Initialize all parts of the UBA module */
  uba_signal_init ();
  uba_main_init();
  
  uba_signal_register_dst (UBA_MODULE_MAIN, uba_main);
  WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBA, "UBA: Initialized\n"));
  WE_MODULE_IS_ACTIVE (WE_MODID_UBA);
}


/*
 *  Starts the termination of the UBA module.
 */
void
uba_start_termination (void)
{
  uba_main_state = UBA_STATE_STOPPING;
  uba_rce_terminate();
}


/*
 *  Signal that the data component is ready with its initialisation.
 *  This will result in the next step of UBA startup.
 */
void
uba_data_init_done (void)
{
  uba_rce_init ();
  uba_rce_start ();
}
/*
 *  Signal that the RCE component is ready with its shutdown.
 *  This will result in the next step of UBA shutdown.
 */

void
uba_terminate_rce_done()
{
  uba_data_terminate();
}

/*
 *  Signal that the data component is ready with its shutdown.
 *  This will result in the next step of UBA shutdown.
 */
void
uba_terminate_data_done()
{
  uba_main_terminate();
}



void
uba_do_delete_ubs_msg( int msgType,
                       ubs_msg_handle_t *handle)
{
  uba_main_delete_msg_t* p = UBA_MEM_ALLOCTYPE(uba_main_delete_msg_t);
  p->msgType = msgType;
  p->next    = NULL;
  
  if (uba_main_delete_msg_queue)
  {
    uba_main_delete_msg_queue_end->next =p;  
    p->handle.handleLen = handle->handleLen;
    p->handle.handle    = UBA_MEM_ALLOC(handle->handleLen);
    memcpy(p->handle.handle,handle->handle, handle->handleLen);
    p->state = UBA_DELETE_MSG_STATE_WANTS_TO_RUN;
  }
  else
  {
    uba_main_delete_msg_queue = p;
    p->state = UBA_DELETE_MSG_STATE_WATING_FOR_RESPONSE;
    p->handle.handleLen = 0;
    p->handle.handle = NULL;
    UBSif_deleteMsg (WE_MODID_UBA,1,msgType,handle);
    
  }

  uba_main_delete_msg_queue_end = p;
  

}


/**********************************************************************
 * Local functions
 **********************************************************************/

static void
uba_terminate (void)
{
  uba_signal_terminate ();
  WE_SIGNAL_DEREGISTER_QUEUE (WE_MODID_UBA);
  WE_WIDGET_RELEASE_ALL(WE_MODID_UBA);
  WE_MEM_FREE_ALL (WE_MODID_UBA);
  WE_MODULE_IS_TERMINATED (WE_MODID_UBA);
  WE_KILL_TASK (WE_MODID_UBA);
  WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_UBA, "UBA: Terminated\n"));
}

 
static void
uba_main_init (void)
{
  uba_main_state =  UBA_STATE_STARTING;

  /* Start UBS */
  WE_MODULE_START(WE_MODID_UBA, WE_MODID_UBS, "", NULL, NULL);
  
  uba_main_state = UBA_STATE_STARTING_UBS;
}


static void
uba_main_terminate (void)
{
  uba_main_delete_msg_t* p;
  /* Delete msg queue */
  while (uba_main_delete_msg_queue)
  {
    p = uba_main_delete_msg_queue;
    uba_main_delete_msg_queue = uba_main_delete_msg_queue->next;
    UBA_MEM_FREE(p->handle.handle);
    UBA_MEM_FREE(p);
  }
  /* stop UBS */
  WE_MODULE_TERMINATE (WE_MODID_UBA, WE_MODID_UBS);

  UBA_SIGNAL_SENDTO(UBA_MODULE_MAIN, UBA_MAIN_SIG_TERMINATE);
}


static void
uba_memory_exception_handler (void)
{
  if (uba_inside_run) {
    longjmp (uba_jmp_buf, 1);
  }
}

/************************************************************************/
/* MSG handlers                                                         */
/************************************************************************/
static void
uba_handle_msg_moduleStatus (int srcModule, unsigned org_sig, we_module_status_t *p)
{
  UNREFERENCED_PARAMETER(srcModule);
  
  switch(uba_main_state){
  
  case UBA_STATE_STARTING_UBS:
    if (p->status == MODULE_STATUS_ACTIVE) {
      uba_main_state = UBA_STATE_STARTING_INIT_DATA;
      
      uba_data_init();
    }
    else {
      
    }
    break;
  
  default:
    
    break;
  }

  WE_SIGNAL_DESTRUCT (WE_MODID_UBA, (WE_UINT16)org_sig, p);
}

static void
uba_main_handle_delete_reply(int src_Module, unsigned org_sig, ubs_reply_delete_msg_t* p)
{
  UNREFERENCED_PARAMETER(src_Module);
  if (uba_main_delete_msg_queue && 
      uba_main_delete_msg_queue->state == UBA_DELETE_MSG_STATE_WATING_FOR_RESPONSE){

    uba_main_delete_msg_t* msg = uba_main_delete_msg_queue;
    uba_main_delete_msg_queue = msg->next;
   
    UBA_MEM_FREE(msg); 
    
    /* Check to send next signal */
    if (uba_main_delete_msg_queue){
      /* Signal exists send it */
      UBSif_deleteMsg (WE_MODID_UBA,1,uba_main_delete_msg_queue->msgType,
                       &uba_main_delete_msg_queue->handle);
      UBA_MEM_FREE(uba_main_delete_msg_queue->handle.handle);
      uba_main_delete_msg_queue->handle.handle = NULL;
      uba_main_delete_msg_queue->handle.handleLen = 0;
      uba_main_delete_msg_queue->state = UBA_DELETE_MSG_STATE_WATING_FOR_RESPONSE;
    }
    else{
      /* No more signals, set end pointer to NULL */
      uba_main_delete_msg_queue_end = NULL;
    }
  }

  WE_SIGNAL_DESTRUCT (WE_MODID_UBA, (WE_UINT16)org_sig, p);
}


static void
uba_main (uba_signal_t *sig)
{
  if (uba_rce_offer_custom_page_signal(sig))
  {
    uba_signal_delete (sig);
    return ;
  }
  switch (sig->type) {
  
  
  case UBA_MAIN_SIG_MODULE_STATUS:
    UBA_LOG_SIGNAL(UBA_MAIN_SIG_MODULE_STATUS);
    uba_handle_msg_moduleStatus (sig->i_param, sig->u_param1, 
                                 (we_module_status_t*)sig->p_param);
    break;

  case UBA_MAIN_SIG_REG_RESPONSE:
    UBA_LOG_SIGNAL(UBA_MAIN_SIG_REG_RESPONSE);
    uba_handle_data_msg_regResponse (sig->i_param, sig->u_param1, 
                                 (we_registry_response_t*)sig->p_param);
    break;

  case UBA_MAIN_SIG_REPLY_REGISTER:
    UBA_LOG_SIGNAL(UBA_MAIN_SIG_REPLY_REGISTER);
    uba_data_handle_msg_reply_register(sig->i_param, sig->u_param1, 
                                       (ubs_reply_register_t*)sig->p_param);
    break;

  case UBA_MAIN_SIG_WIDGET_ACTION:
    UBA_LOG_SIGNAL (UBA_MAIN_SIG_WIDGET_ACTION);
    uba_rce_handle_widget_action (sig->i_param, sig->u_param1, 
                                       (we_widget_action_t*)sig->p_param);
    break;

  case UBA_MAIN_SIG_REPLY_MSG_LIST:
    UBA_LOG_SIGNAL (UBA_MAIN_SIG_REPLY_MSG_LIST);
    uba_data_handle_msg_reply_list(sig->i_param, sig->u_param1,
      (ubs_reply_msg_list_t*) sig->p_param);
    break;
    
  case UBA_MAIN_SIG_NOTIFY_MSG_DELETED:
    UBA_LOG_SIGNAL (UBA_MAIN_SIG_NOTIFY_MSG_DELETED);
    uba_data_handle_notify_msg_deleted (sig->i_param, sig->u_param1,
      (ubs_notify_msg_deleted_t*) sig->p_param);
    break;
    
  case UBA_MAIN_SIG_NOTIFY_MSG_CHANGED:
    UBA_LOG_SIGNAL (UBA_MAIN_SIG_NOTIFY_MSG_CHANGED);
    uba_data_handle_notify_msg_changed (sig->i_param, sig->u_param1,
      (ubs_notify_msg_changed_t*) sig->p_param);
    break;
    
  case UBA_MAIN_SIG_NOTIFY_NEW_MSG:
    UBA_LOG_SIGNAL (UBA_MAIN_SIG_NOTIFY_NEW_MSG);
    uba_data_handle_notify_new_msg (sig->i_param, sig->u_param1,
      (ubs_notify_new_msg_t*) sig->p_param);
    break;

  case UBA_MAIN_SIG_TERMINATE:
    UBA_LOG_SIGNAL (UBA_MAIN_SIG_TERMINATE);
    uba_signal_delete (sig);
    if (uba_signal_queue_is_empty ()){
       uba_terminate ();
    }
    else{
       UBA_SIGNAL_SENDTO(UBA_MODULE_MAIN, UBA_MAIN_SIG_TERMINATE);
    }
    return;

  case UBS_MAIN_SIG_REPLY_DELETE_MSG:
    UBA_LOG_SIGNAL (UBS_MAIN_SIG_REPLY_DELETE_MSG);
    uba_main_handle_delete_reply(sig->i_param,sig->u_param1,
            (ubs_reply_delete_msg_t*)sig->p_param);
    break;
    
  default:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBA, 
                  "UBA: received unexpected signal: %d\n", sig->type));
    break;

  }

  uba_signal_delete (sig);
}

