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
 * uba_main.h
 *
 * This file contains the handling of initiation and termination for UBA.
 * It also contains the internal signal receive function.
 *
 * The UBA consists of a RCE (resource engine) and data component. These
 * components are not pure submodules but have their own init and termination routines.
 *
 * Startup order:
 *  1. Starts the UBS module
 *  2. Starts the data component (uba_data_init)
 *  3. Starts the RCE component (uba_rce_init and uba_rce_start)
 *
 * Shutdown order
 *  1. Shutdown of RCE (uba_rce_terminate)
 *  2. Shutdown of data (uba_data_terminate)
 *  3. Stops the UBS module
 *
 * For RCE, see uba_rce.h
 * For data, see uba_data.h
 */

#ifndef UBA_MAIN_H
#define UBA_MAIN_H

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef UBA_CFG_H
  #include "Uba_Cfg.h"
#endif

#ifndef _we_core_h
  #include "We_Core.h"
#endif

#ifndef _we_mem_h
  #include "We_Mem.h"
#endif

#ifndef _we_lib_h
  #include "We_Lib.h"
#endif

/************************************************************************/
/* Memory macros                                                        */
/************************************************************************/

/* Memory macros used in the UBA module */
#define UBA_MEM_ALLOC(s)      WE_MEM_ALLOC(WE_MODID_UBA,s)
#define UBA_MEM_ALLOCTYPE(t)  WE_MEM_ALLOCTYPE(WE_MODID_UBA,t)
#define UBA_MEM_FREE(p)       WE_MEM_FREE(WE_MODID_UBA,p)

/************************************************************************/
/* Type declarations                                                    */
/************************************************************************/

/* Enumeration of the UBA states */
typedef enum {
  UBA_STATE_STARTING,

  UBA_STATE_STARTING_UBS,
  UBA_STATE_STARTING_INIT_DATA,
  
  UBA_STATE_STARTED,

  UBA_STATE_STOPPING,
  UBA_STATE_STOPPED
} uba_state_t;

/***********************************************************
 *   Internal Signals used in the UBA Module:              *
 ***********************************************************/

#define UBA_MAIN_SIG_MODULE_STATUS	  		      (UBA_MODULE_MAIN + 1)
#define UBA_MAIN_SIG_REPLY_REGISTER             (UBA_MODULE_MAIN + 2)
#define UBA_MAIN_SIG_WIDGET_ACTION              (UBA_MODULE_MAIN + 3)
#define UBA_MAIN_SIG_REPLY_MSG_LIST             (UBA_MODULE_MAIN + 4)
#define UBA_MAIN_SIG_NOTIFY_MSG_DELETED         (UBA_MODULE_MAIN + 5)
#define UBA_MAIN_SIG_NOTIFY_MSG_CHANGED         (UBA_MODULE_MAIN + 6)
#define UBA_MAIN_SIG_NOTIFY_NEW_MSG             (UBA_MODULE_MAIN + 7)
#define UBS_MAIN_SIG_REPLY_DELETE_MSG           (UBA_MODULE_MAIN + 8)
#define UBS_MAIN_SIG_REPLY_FULL_MSG             (UBA_MODULE_MAIN + 9)
#define UBA_MAIN_SIG_REG_RESPONSE               (UBA_MODULE_MAIN + 10)

#define UBA_MAIN_SIG_INTERNAL_SIGNALS           (UBA_MODULE_MAIN +20) /* Internal signals start after this */
#define UBA_MAIN_SIG_TERMINATE                  (UBA_MAIN_SIG_INTERNAL_SIGNALS +1)

/************************************************************
 * Exported global variables
 ************************************************************/

extern jmp_buf      uba_jmp_buf;
extern int          uba_inside_run;
extern uba_state_t  uba_main_state;

/**********************************************************************
 * Function declarations
 **********************************************************************/

/*
 *  Initiates the UBA module.
 *  Starts the components RCE and data.
 */
void
uba_init (void);

/*
 *  Starts the termination of the UBA module.
 */
void
uba_start_termination (void);

/*
 *  Signal that the data component is ready with its initialisation.
 *  This will result in the next step of UBA startup.
 */
void
uba_data_init_done (void);

/*
 *  Signal that the RCE component is ready with its shutdown.
 *  This will result in the next step of UBA shutdown.
 */
void
uba_terminate_rce_done (void);

/*
 *  Signal that the data component is ready with its shutdown.
 *  This will result in the next step of UBA shutdown.
 */
void
uba_terminate_data_done (void);


/*
 *	Use this function to delete a ubs msg
 */

void
uba_do_delete_ubs_msg (int msgType, ubs_msg_handle_t *handle);

/**************************************************
 * Macros
 **************************************************/

#ifndef UNREFERENCED_PARAMETER
  #define UNREFERENCED_PARAMETER(P)          \
      /*lint -e527 -e530 */ \
      { \
          (P) = (P); \
      } \
      /*lint +e527 +e530 */
#endif


#endif      /*UBA_MAIN_H*/
