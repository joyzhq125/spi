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









#ifndef _Pus_Msg_h
#define _Pus_Msg_h

#ifndef _we_core_h
#include "we_core.h"
#endif

#ifndef _Pus_Sig_h
#include "Pus_Sig.h"
#endif


#ifndef _Pus_Cfg_h
#include "Pus_Cfg.h"
#endif

#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES 
#ifndef _ubs_if_h
#include "ubs_if.h"
#endif
#endif

#ifndef _stk_if_h
#include "stk_if.h"
#endif





#define PUS_MSG_SIG_NEW_MESSAGE                    PUS_MODULE_MSG + 1
#define PUS_MSG_SIG_UBS_MESSAGE_DATA               PUS_MODULE_MSG + 2  











#define PUS_STATE_NEW_MSG                     0
#define PUS_STATE_WHITELIST_DONE              1
#define PUS_STATE_PRS_STARTED                 2
#define PUS_STATE_PARSING_SI                  3
#define PUS_STATE_PARSING_SL                  4
#define PUS_STATE_PARSING_CO                  5
#define PUS_STATE_READING_HEADER              6
#define PUS_STATE_READING_BODY                7


#define PUS_PRIO_LOW                             0
#define PUS_PRIO_MEDIUM                          1
#define PUS_PRIO_HIGH                            2
#define PUS_PRIO_DELETE                          3
#define PUS_PRIO_CACHE                           4
#define PUS_PRIO_NONE                            5







typedef struct pus_co_data_st {
  struct pus_co_data_st    *next;           
  int                       type;           
  char                     *text;           
} pus_co_data_t;

 


typedef struct pus_prsd_info_st {
  char                    *href;           
  int                      priority;         
  char                    *si_id;          
  char                    *text;           
  WE_UINT32               created;        
  WE_UINT32               expired;        
  pus_co_data_t           *co_data;        
} pus_prs_info_t;











void
Pus_Msg_init (void);

void
Pus_Msg_main (Pus_Signal_t* signal);

void
pus_handle_new_push (stk_recv_request_t *p);

void
pus_handle_content_send_ack (int wid, int status);

void 
Pus_Msg_delete_all_message_in_list (void);

#ifdef PUS_CONFIG_WAP_PUSH_MESSAGES 

void
pus_handle_parsing_aborted (int msg_id);

void
pus_handle_ubs_message_data (ubs_reply_msg_t *result);

#endif
#endif
