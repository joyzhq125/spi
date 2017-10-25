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









#ifndef UBS_MAIN_H
#define UBS_MAIN_H

#ifndef _we_core_h
#include "We_Core.h"
#endif

#ifndef _we_mem_h
#include "We_Mem.h"
#endif

#ifndef _we_lib_h
#include "We_Lib.h"
#endif

#ifndef UBS_CFG_H
#include "Ubs_Cfg.h"
#endif

#ifndef UBS_IF_H
#include "Ubs_If.h"
#endif





#define UBS_MAIN_SIG_REGISTER                 (UBS_MODULE_MAIN + 1)
#define UBS_MAIN_SIG_REPLY_REGISTER           (UBS_MODULE_MAIN + 2)
#define UBS_MAIN_SIG_DEREGISTER               (UBS_MODULE_MAIN + 3)
#define UBS_MAIN_SIG_GET_NBR_OF_MSGS          (UBS_MODULE_MAIN + 4)
#define UBS_MAIN_SIG_REPLY_NBR_OF_MSGS        (UBS_MODULE_MAIN + 5)
#define UBS_MAIN_SIG_GET_MSG_LIST             (UBS_MODULE_MAIN + 6)
#define UBS_MAIN_SIG_REPLY_MSG_LIST           (UBS_MODULE_MAIN + 7)
#define UBS_MAIN_SIG_GET_MSG                  (UBS_MODULE_MAIN + 8)
#define UBS_MAIN_SIG_REPLY_MSG                (UBS_MODULE_MAIN + 9)
#define UBS_MAIN_SIG_GET_FULL_MSG             (UBS_MODULE_MAIN + 10)
#define UBS_MAIN_SIG_REPLY_FULL_MSG           (UBS_MODULE_MAIN + 11)
#define UBS_MAIN_SIG_DELETE_MSG               (UBS_MODULE_MAIN + 12)
#define UBS_MAIN_SIG_REPLY_DELETE_MSG         (UBS_MODULE_MAIN + 13)
#define UBS_MAIN_SIG_NOTIFY_MSG_DELETED       (UBS_MODULE_MAIN + 14)
#define UBS_MAIN_SIG_CHANGE_MSG               (UBS_MODULE_MAIN + 15)
#define UBS_MAIN_SIG_REPLY_CHANGE_MSG         (UBS_MODULE_MAIN + 16)
#define UBS_MAIN_SIG_NOTIFY_MSG_CHANGED       (UBS_MODULE_MAIN + 17)
#define UBS_MAIN_SIG_NOTIFY_NEW_MSG           (UBS_MODULE_MAIN + 18)
#define UBS_MAIN_SIG_CREATE_MSG               (UBS_MODULE_MAIN + 19)
#define UBS_MAIN_SIG_REPLY_CREATE_MSG         (UBS_MODULE_MAIN + 20)
#define UBS_MAIN_SIG_NOTIFY_UNREAD_MSG_COUNT  (UBS_MODULE_MAIN + 21)
#define UBS_MAIN_SIG_NOTIFY_INVALIDATE_ALL    (UBS_MODULE_MAIN + 22)
#define UBS_MAIN_SIG_SET_MSG_PROPERTY         (UBS_MODULE_MAIN + 23)
#define UBS_MAIN_SIG_REPLY_SET_MSG_PROPERTY   (UBS_MODULE_MAIN + 24)

#define UBS_MAIN_SIG_MODULE_STATUS            (UBS_MODULE_MAIN + 25)





extern jmp_buf ubs_jmp_buf;
extern int     ubs_inside_run;





void
ubs_init (void);

void
ubs_terminate (void);

void
ubs_start_termination (void);

void
ubs_terminate_msg_store_done (void);


int
ubs_main_deregisterTransaction (int transactionId, unsigned *pSrcTransactionId);

unsigned*
ubs_main_getTitlePropertiesDefinition (int msgType, int srcModule, int *result_nbrInArray);

void
ubs_main_free_msg_handle (ubs_msg_handle_t *p);

void
ubs_main_free_key_value (ubs_key_value_t *p);

void
ubs_main_free_key_value_array (ubs_key_value_t **p, int arraySize);

void
ubs_main_free_full_msg (ubs_full_msg_t *p);













void 
ubs_replyNbrOfMsgs (int dst, 
                          unsigned transactionId,
                          int msgType,
                          unsigned nbrOfMsgs);









void 
ubs_replyMsgList (int dst, 
                        unsigned transactionId,
                        int msgType,
                        unsigned msgListSize,
                        ubs_msg_list_item_t *msgList,
                        unsigned nextMsg);








void 
ubs_replyMsg (int dst, 
                    unsigned transactionId,
                    int msgType,
                    int result,
                    ubs_msg_t *msg);








void 
ubs_replyDeleteMsg (int dst,
                          unsigned transactionId,
                          int msgType,
                          ubs_msg_handle_t *handle,
                          int result);







void 
ubs_replyFullMsg (int dst,
                        unsigned transactionId,
                        int msgType,
                        int result,
                        ubs_full_msg_t *msg);








void 
ubs_replyChangeMsg (int dst, 
                          unsigned transactionId,
                          int msgType,
                          ubs_msg_handle_t *handle,
                          int result);








void 
ubs_replyCreateMsg (int dst, 
                          unsigned transactionId,
                          int msgType,
                          ubs_msg_handle_t *handle,
                          int result);









void
ubs_replySetMsgProperty (int dst,
                          unsigned transactionId,
                          int msgType,
                          ubs_msg_handle_t *handle,
                          int result);





void
ubs_handle_msg_main_notifyMsgDeleted (int srcModule, ubs_notify_msg_deleted_t *p);

void
ubs_handle_msg_main_notifyMsgChanged (int srcModule, ubs_notify_msg_changed_t *p);

void
ubs_handle_msg_main_notifyNewMsg (int srcModule, ubs_notify_new_msg_t *p);






#define UBS_MEM_ALLOC(x)      WE_MEM_ALLOC (WE_MODID_UBS, (x))
#define UBS_MEM_ALLOCTYPE(x)  WE_MEM_ALLOCTYPE (WE_MODID_UBS, x)
#define UBS_MEM_FREE(x)       WE_MEM_FREE (WE_MODID_UBS, (x))

#ifdef WE_LOG_MODULE

  #define UBS_LOG_FILE_IF(cond,func, fileop, res) \
  {if (cond){                                                        \
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_UBS,                  \
        "UBS: " #fileop " failed (result = %i) [" #func "]\n",res));      \
  }}

  #define UBS_LOG_FILE_ERROR(func,fileop,res) UBS_LOG_FILE_IF(((res)<0),func,fileop,res)
  #define UBS_LOG_FILE_ERROR_ASYNC(func,fileop,res) \
    UBS_LOG_FILE_IF( (res<0 && res!=WE_FILE_ERROR_DELAYED),func,fileop,res)

#else
  #define UBS_LOG_FILE_IF(cond,func, fileop, res) UNREFERENCED_PARAMETER(res)
  #define UBS_LOG_FILE_ERROR(func,fileop,res) UNREFERENCED_PARAMETER(res)
  #define UBS_LOG_FILE_ERROR_ASYNC(func,fileop,res) UNREFERENCED_PARAMETER(res)
#endif

#ifndef UNREFERENCED_PARAMETER
  #define UNREFERENCED_PARAMETER(P)          \
       \
      { \
          (P) = (P); \
      } \
      
#endif

#endif      /*UBS_MAIN_H*/
