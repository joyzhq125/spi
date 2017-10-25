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
 * uba_env.c
 *
 * Created by Anders Evertsson, Thu Jun 5 2003.
 *
 * Revision history:
 *
 */
#include "Uba_Cfg.h"
#include "Uba_Main.h"
#include "Uba_Sig.h"
#include "We_Cfg.h"
#include "We_Core.h"
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Wid.h"
#include "We_Errc.h"


/**********************************************************************
 * Function declarations
 **********************************************************************/

static void
uba_receive (WE_UINT8 src_module, WE_UINT16 signal, void* p);


/**********************************************************************
 * Function definitions
 **********************************************************************/

void
UBAc_start (void)
{
  WE_LOG_FC_BEGIN(UBAc_start)
  WE_LOG_FC_PRE_IMPL

  WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_UBA, "UBA: UBAc_start\n"));
  uba_init ();
  WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_UBA, "UBA: Initialised\n"));

  WE_LOG_FC_END
}

void
UBAc_kill (void)
{
    WE_WIDGET_RELEASE_ALL (WE_MODID_UBA);     /* Only if the module use widgets. */
    WE_MEM_FREE_ALL (WE_MODID_UBA);
    WE_KILL_TASK (WE_MODID_UBA);
}
void
UBAc_run (void)
{
  WE_LOG_FC_BEGIN(UBAc_run)
    WE_LOG_FC_PRE_IMPL
    
    WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_UBA, "UBA: UBAc_run\n"));
  /* Set the global variable, the module is now inside the Run function. */
  uba_inside_run = 1;
  
  /* Make a setjmp, so the module can jump back to this point if the memory runs
     out. */
  if (setjmp (uba_jmp_buf) == 0) {
   /*
    * Get external signals and process one internal signal.
    */
    while (WE_SIGNAL_RETRIEVE (WE_MODID_UBA, uba_receive)){ 
      /*empty */
    };
    uba_signal_process ();
  }
  else {
   /*
    * A longjmp from xxx_exception_handler. If we are here there is no more
    * memory, and the situation could not be worse.
    * Send out an error and terminate as the figure above.
    */
    WE_ERROR (WE_MODID_UBA, WE_ERR_MODULE_OUT_OF_MEMORY);
    WE_WIDGET_RELEASE_ALL (WE_MODID_UBA);     /* Only if the module use widgets. */
    WE_MEM_FREE_ALL (WE_MODID_UBA);
    WE_MODULE_IS_TERMINATED (WE_MODID_UBA);
    WE_SIGNAL_DEREGISTER_QUEUE (WE_MODID_UBA);
    WE_KILL_TASK (WE_MODID_UBA);
  }
  
  /* Clear the global variable, the module is now outside the Run function. */
  uba_inside_run = 0;
  
  WE_LOG_FC_END
}

int
UBAc_wantsToRun (void)
{
  int returnVal = 0;
  WE_LOG_FC_BEGIN(UBAc_wantsToRun)
  WE_LOG_FC_PRE_IMPL

  WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_UBA, "UBA: UBAc_wantsToRun\n"));
  if ((WE_SIGNAL_QUEUE_LENGTH (WE_MODID_UBA) > 0) || !uba_signal_queue_is_empty ())
  {
    returnVal = 1;
  }

  WE_LOG_FC_INT32(returnVal, NULL);
  WE_LOG_FC_END
  return returnVal;
}

/*
 * Place a received external signal in the internal queue
 * of the appropriate module.
 */
static void
uba_receive (WE_UINT8 src_module, WE_UINT16 signal, void* p)
{
  
  switch (signal) {

  case WE_SIG_MODULE_TERMINATE:
		UBA_MEM_FREE (p);
    uba_start_termination ();
    
		break;


  case WE_SIG_MODULE_STATUS:
    UBA_SIGNAL_SENDTO_IUP (UBA_MODULE_MAIN, UBA_MAIN_SIG_MODULE_STATUS,
      src_module, signal, p);      
    break;

  case WE_SIG_REG_RESPONSE:
    UBA_SIGNAL_SENDTO_IUP (UBA_MODULE_MAIN, UBA_MAIN_SIG_REG_RESPONSE,
      src_module, signal, p);      
    break;

  case UBS_SIG_REPLY_REGISTER:
    UBA_SIGNAL_SENDTO_IUP (UBA_MODULE_MAIN, UBA_MAIN_SIG_REPLY_REGISTER,
      src_module, signal, p);
    break;

  case WE_SIG_WIDGET_ACTION:
    UBA_SIGNAL_SENDTO_IUP (UBA_MODULE_MAIN, UBA_MAIN_SIG_WIDGET_ACTION,
      src_module, signal, p);
    break;

  case UBS_SIG_REPLY_MSG_LIST:
    UBA_SIGNAL_SENDTO_IUP (UBA_MODULE_MAIN, UBA_MAIN_SIG_REPLY_MSG_LIST,
      src_module, signal, p);
    break;

  case UBS_SIG_NOTIFY_MSG_DELETED:
    UBA_SIGNAL_SENDTO_IUP (UBA_MODULE_MAIN, UBA_MAIN_SIG_NOTIFY_MSG_DELETED,
      src_module, signal, p);
    break;

  case UBS_SIG_NOTIFY_MSG_CHANGED:
    UBA_SIGNAL_SENDTO_IUP (UBA_MODULE_MAIN, UBA_MAIN_SIG_NOTIFY_MSG_CHANGED,
      src_module, signal, p);
    break;

  case UBS_SIG_NOTIFY_NEW_MSG:
    UBA_SIGNAL_SENDTO_IUP (UBA_MODULE_MAIN, UBA_MAIN_SIG_NOTIFY_NEW_MSG,
      src_module, signal, p);
    break;

  case UBS_SIG_REPLY_DELETE_MSG:
     UBA_SIGNAL_SENDTO_IUP (UBA_MODULE_MAIN, UBS_MAIN_SIG_REPLY_DELETE_MSG,
      src_module, signal, p);
    break;

   case UBS_SIG_REPLY_FULL_MSG:
     UBA_SIGNAL_SENDTO_IUP (UBA_MODULE_MAIN, UBS_MAIN_SIG_REPLY_FULL_MSG,
      src_module, signal, p);
   
    break;













  default:
    /* This was not a signal that we are interested in */
    WE_SIGNAL_DESTRUCT (WE_MODID_UBA, signal, p);
    break;
  }
}


#ifdef WE_LOG_FC
WE_LOG_FC_DISPATCH_MAP_BEGIN(UBA)

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(UBAc_start)
UBAc_start();
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(UBAc_wantsToRun)
UBAc_wantsToRun();
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(UBAc_run)
UBAc_run();
WE_LOG_FC_DISPATCH_MAP_ENTRY_END

WE_LOG_FC_DISPATCH_MAP_END
#endif 
