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
 * ubs_env.c
 *
 * Created by Klas Hermodsson, Tue 03 June 2003.
 *
 * Revision history:
 *
 */

#include "We_Cfg.h"
#include "We_Core.h"
#include "We_Mem.h"
#include "We_Cmmn.h"
#include "We_Log.h"
#include "We_Def.h"
#include "We_Errc.h"
#include "We_File.h"

#include "Ubs_Env.h"
#include "Ubs_Main.h"
#include "Ubs_Msin.h"
#include "Ubs_If.h"
#include "Ubs_Sig.h"


/**********************************************************************
 * Function declarations
 **********************************************************************/

static void
ubs_receive (WE_UINT8 src_module, WE_UINT16 signal, void* p);


/**********************************************************************
 * Function definitions
 **********************************************************************/

void
UBSc_start (void)
{
  WE_LOG_FC_BEGIN(UBSc_start)
  WE_LOG_FC_PRE_IMPL

  ubs_inside_run = FALSE;
  ubs_init ();

  WE_LOG_FC_END
}

int
UBSc_wantsToRun (void)
{
  int returnVal;
  WE_LOG_FC_BEGIN(UBSc_wantsToRun)
  WE_LOG_FC_PRE_IMPL

  returnVal = ((WE_SIGNAL_QUEUE_LENGTH (WE_MODID_UBS) > 0) ||
          !ubs_signal_queue_is_empty ());
  
  WE_LOG_FC_INT(returnVal, NULL)
  WE_LOG_FC_END

  return returnVal;
}

void
UBSc_run (void)
{
  WE_LOG_FC_BEGIN(UBSc_run)
  WE_LOG_FC_PRE_IMPL

  if (setjmp (ubs_jmp_buf) == 0) {
    ubs_inside_run = TRUE;
    while (WE_SIGNAL_RETRIEVE (WE_MODID_UBS, ubs_receive))
    { /* empty*/ };

    ubs_signal_process ();
    ubs_inside_run = FALSE;
  }
  else {
    /* A longjmp from ubs_exception_handler. If we are here there is no more
     * memory, and the situation could not be worse.
     * Send out an error and terminate.
     */
    WE_ERROR (WE_MODID_UBS, WE_ERR_MODULE_OUT_OF_MEMORY);
    WE_FILE_CLOSE_ALL (WE_MODID_UBS);
    WE_MEM_FREE_ALL (WE_MODID_UBS);
    WE_MODULE_IS_TERMINATED (WE_MODID_UBS);
    WE_SIGNAL_DEREGISTER_QUEUE (WE_MODID_UBS);
    WE_KILL_TASK (WE_MODID_UBS);
  }

  WE_LOG_FC_END
}

void
UBSc_kill (void)
{
  WE_LOG_FC_BEGIN(UBSc_kill)
  WE_LOG_FC_PRE_IMPL

   
  WE_FILE_CLOSE_ALL (WE_MODID_UBS);
  WE_MEM_FREE_ALL (WE_MODID_UBS);
  WE_KILL_TASK (WE_MODID_UBS);

  WE_LOG_FC_END
}


/*
 * Place a received external signal in the internal queue
 * of the appropriate module.
 */
static void
ubs_receive (WE_UINT8 srcModule, WE_UINT16 signal, void* p)
{
  switch (signal) {

  case WE_SIG_MODULE_EXECUTE_COMMAND:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
                 "UBS: Received WE_SIG_MODULE_EXECUTE_COMMAND\n"));
    UBS_MEM_FREE (p);
    break;

  case WE_SIG_MODULE_TERMINATE:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
                 "UBS: Received WE_SIG_MODULE_TERMINATE\n"));
    UBS_MEM_FREE (p);
    ubs_start_termination ();
    break;

  case WE_SIG_MODULE_STATUS:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
                 "UBS: Received WE_SIG_MODULE_STATUS\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_MODULE_STATUS, srcModule, p);
    break;
    
  case WE_SIG_FILE_NOTIFY:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
                 "UBS: Received WE_SIG_FILE_NOTIFY\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MSG_STORE, UBS_MSG_STORE_SIG_FILE_NOTIFY, srcModule, p);
    break;
    
  case UBS_SIG_REGISTER:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_REGISTER\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_REGISTER, srcModule, p);
    break;

  case UBS_SIG_REPLY_REGISTER:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_REPLY_REGISTER\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_REPLY_REGISTER, srcModule, p);
    break;

  case UBS_SIG_DEREGISTER:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_DEREGISTER\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_DEREGISTER, srcModule, p);
    break;

  case UBS_SIG_GET_NBR_OF_MSGS:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_GET_NBR_OF_MSGS\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_GET_NBR_OF_MSGS, srcModule, p);
    break;

  case UBS_SIG_REPLY_NBR_OF_MSGS:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_REPLY_NBR_OF_MSGS\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_REPLY_NBR_OF_MSGS, srcModule, p);
    break;

  case UBS_SIG_GET_MSG_LIST:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_GET_MSG_LIST\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_GET_MSG_LIST, srcModule, p);
    break;

  case UBS_SIG_REPLY_MSG_LIST:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_REPLY_MSG_LIST\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_REPLY_MSG_LIST, srcModule, p);
    break;

  case UBS_SIG_GET_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_GET_MSG\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_GET_MSG, srcModule, p);
    break;

  case UBS_SIG_REPLY_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_REPLY_MSG\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_REPLY_MSG, srcModule, p);
    break;

  case UBS_SIG_GET_FULL_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_GET_FULL_MSG\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_GET_FULL_MSG, srcModule, p);
    break;

  case UBS_SIG_REPLY_FULL_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_REPLY_FULL_MSG\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_REPLY_FULL_MSG, srcModule, p);
    break;

  case UBS_SIG_DELETE_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_DELETE_MSG\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_DELETE_MSG, srcModule, p);
    break;

  case UBS_SIG_REPLY_DELETE_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_REPLY_DELETE_MSG\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_REPLY_DELETE_MSG, srcModule, p);
    break;

  case UBS_SIG_NOTIFY_MSG_DELETED:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_NOTIFY_MSG_DELETED\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_NOTIFY_MSG_DELETED, srcModule, p);
    break;

  case UBS_SIG_CHANGE_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_CHANGE_MSG\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_CHANGE_MSG, srcModule, p);
    break;

  case UBS_SIG_REPLY_CHANGE_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_REPLY_CHANGE_MSG\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_REPLY_CHANGE_MSG, srcModule, p);
    break;

  case UBS_SIG_NOTIFY_MSG_CHANGED:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_NOTIFY_MSG_CHANGED\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_NOTIFY_MSG_CHANGED, srcModule, p);
    break;

  case UBS_SIG_NOTIFY_NEW_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_NOTIFY_NEW_MSG\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_NOTIFY_NEW_MSG, srcModule, p);
    break;

  case UBS_SIG_CREATE_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_CREATE_MSG\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_CREATE_MSG, srcModule, p);
    break;

  case UBS_SIG_REPLY_CREATE_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_REPLY_CREATE_MSG\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_REPLY_CREATE_MSG, srcModule, p);
    break;
    
  case UBS_SIG_NOTIFY_UNREAD_MSG_COUNT:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_NOTIFY_UNREAD_MSG_COUNT\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_NOTIFY_UNREAD_MSG_COUNT, srcModule, p);
    break;

  case UBS_SIG_NOTIFY_INVALIDATE_ALL:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_NOTIFY_INVALIDATE_ALL\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_NOTIFY_INVALIDATE_ALL, srcModule, p);
    break;

  case UBS_SIG_SET_MSG_PROPERTY:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_SET_MSG_PROPERTY\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_SET_MSG_PROPERTY, srcModule, p);
    break;

  case UBS_SIG_REPLY_SET_MSG_PROPERTY:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Received UBS_SIG_REPLY_SET_MSG_PROPERTY\n"));
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MAIN, UBS_MAIN_SIG_REPLY_SET_MSG_PROPERTY, srcModule, p);
    break;

  default:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
                 "UBS: Received unknown signal (%04x)\n", signal));
    /* This was not a signal that we are interested in */
    WE_SIGNAL_DESTRUCT (WE_MODID_UBS, signal, p);
    break;
  }
}
