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









#include "We_Mem.h"
#include "We_Core.h"
#include "We_Log.h"
#include "We_Cfg.h"
#include "We_File.h"

#include "Ubs_Main.h"
#include "Ubs_Msin.h"
#include "Ubs_Sig.h"
#include "Ubs_Vrsn.h"
#include "Ubs_Cfg.h"
#include "Ubs_A.h"






#define UBS_STATE_NOT_STARTED            1
#define UBS_STATE_ASKED_TO_START         2
#define UBS_STATE_STARTED                3
#define UBS_STATE_ASKED_TO_REGISTER      4
#define UBS_STATE_REGISTERED             5
#define UBS_STATE_ASKED_TO_STOP          6





typedef struct ubs_module_st  {
  int                   modId;
  int                   state;
  struct ubs_module_st  *next;
} ubs_module_t;

typedef struct ubs_transaction_st {
  int                       wid;
  int                       modId;
  unsigned                  srcTransactionId;
  int                       msgType;
  struct ubs_transaction_st *next;
} ubs_transaction_t;





jmp_buf ubs_jmp_buf;
int     ubs_inside_run;





ubs_module_t          ownerViewerList[UBS_NBR_OF_MSG_TYPES];
ubs_transaction_t     *transactionList;




static void
ubs_main (ubs_signal_t *sig);

static void
ubs_memory_exception_handler (void);

static void
ubs_main_init (void);


static void 
ubs_notifyNewMsg (int dst, 
                        int msgType,
                        ubs_msg_list_item_t *msg);

static void 
ubs_notifyMsgDeleted (int dst, 
                            int msgType,
                            ubs_msg_handle_t *handle);

static void 
ubs_notifyMsgChanged (int dst, 
                            int msgType,
                            ubs_msg_list_item_t *msg);









void
ubs_init (void)
{
  WE_MEM_INIT (WE_MODID_UBS, ubs_memory_exception_handler, FALSE);
  WE_SIGNAL_REG_FUNCTIONS (WE_MODID_UBS, ubs_convert, ubs_destruct);
  WE_SIGNAL_REGISTER_QUEUE (WE_MODID_UBS);
  WE_MODULE_IS_CREATED (WE_MODID_UBS, UBS_VERSION);

  
  ubs_signal_init ();
  ubs_msg_store_init ();
  ubs_main_init();
  
  ubs_signal_register_dst (UBS_MODULE_MAIN, ubs_main);
  WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS, "UBS: Initialized\n"));
  WE_MODULE_IS_ACTIVE (WE_MODID_UBS);
}

void
ubs_start_termination (void)
{
  ubs_msg_store_terminate ();
}

void
ubs_main_terminate (void)
{
  
  int i;

  for (i = 0; i < UBS_NBR_OF_MSG_TYPES; i++) {
    if ((ownerViewerList[i].modId != WE_MODID_UBS) &&
        (ownerViewerList[i].state != UBS_STATE_NOT_STARTED) && 
        (ownerViewerList[i].state != UBS_STATE_ASKED_TO_STOP)) 
    {
      WE_MODULE_TERMINATE (WE_MODID_UBS, (WE_UINT8)ownerViewerList[i].modId);
      ownerViewerList[i].state = UBS_STATE_ASKED_TO_STOP;
      ownerViewerList[i].modId = -1;
    }
  }

  for (i = 0; i < UBS_NBR_OF_MSG_TYPES; i++) 
  {
    while (ownerViewerList[i].next) {
      ubs_module_t *e = ownerViewerList[i].next;
      ownerViewerList[i].next = e->next;
      UBS_MEM_FREE (e);
    }
  }
  
  while (transactionList) 
  {
    ubs_transaction_t  *e = transactionList;
    transactionList = e->next;
    UBS_MEM_FREE (e);
  }

  ubs_terminate ();
}



void
ubs_terminate_msg_store_done (void)
{
  ubs_main_terminate ();
}




void
ubs_terminate (void)
{
  ubs_signal_terminate ();
  WE_SIGNAL_DEREGISTER_QUEUE (WE_MODID_UBS);
  WE_FILE_CLOSE_ALL (WE_MODID_UBS);
  WE_MEM_FREE_ALL (WE_MODID_UBS);
  WE_MODULE_IS_TERMINATED (WE_MODID_UBS);
  WE_KILL_TASK (WE_MODID_UBS);
  WE_LOG_MSG ((WE_LOG_DETAIL_LOW, WE_MODID_UBS, "UBS: Terminated\n"));
}





int
ubs_main_deregisterTransaction (int transactionId, unsigned *pSrcTransactionId)
{
  ubs_transaction_t   *p = transactionList;
  ubs_transaction_t   *pre_p = NULL;
  int                 result = -1;

  while (p && (p->wid != transactionId)) {
    pre_p = p;
    p = p->next;
  }

  if (p) {
    result = p->modId;
    if (!pre_p) {
      transactionList = p->next;
    }
    else {
      pre_p->next = p->next;
    }
    *pSrcTransactionId = p->srcTransactionId;

    UBS_MEM_FREE (p);
  }

  return result;
}


unsigned*
ubs_main_getTitlePropertiesDefinition (int msgType, int srcModule, int *result_nbrInArray)
{
  unsigned  *result_Array = UBS_MEM_ALLOC (sizeof (unsigned) * UBS_NBR_OF_KEYS);
  int       i;
  ubs_title_property_array_element_t    
            titlePropArray[UBS_NBR_OF_KEYS] = UBS_TITLE_PROPERTY_ARRAY;

  UNREFERENCED_PARAMETER (msgType);
  UNREFERENCED_PARAMETER (srcModule);

  *result_nbrInArray = 0;

  for (i = 0; i < UBS_NBR_OF_KEYS; i++) {
    if (titlePropArray[i].isTitleProperty) {
      result_Array[*result_nbrInArray] = i;
      (*result_nbrInArray)++;
    }
  }

  return result_Array;
}


void
ubs_main_free_msg_handle (ubs_msg_handle_t *p)
{
  if (p && (p->handle)) {
    UBS_MEM_FREE (p->handle);
  }
}


void
ubs_main_free_key_value (ubs_key_value_t *p)
{
  if (p && (p->value)) {
    UBS_MEM_FREE (p->value);
  }
}


void
ubs_main_free_key_value_array (ubs_key_value_t **p, int arraySize)
{
  if (*p) 
  {
    int i;
    for (i = 0; i < arraySize; i++ ) {
      ubs_main_free_key_value (&((*p)[i]));
    }
    UBS_MEM_FREE (*p);
  }
}


void
ubs_main_free_full_msg (ubs_full_msg_t *p)
{
  int i;

  if (p) 
  {
    ubs_main_free_msg_handle (&(p->handle));

    if (p->propertyList)
    {
      for (i = 0; i < (p->propertyListSize); i++) {
        ubs_main_free_key_value (&(p->propertyList[i]));
      }
      UBS_MEM_FREE (p->propertyList);
    }
    if (p->hiddenPropertyList)
    {
      for (i = 0; i < (p->hiddenPropertyListSize); i++) {
        ubs_main_free_key_value (&(p->hiddenPropertyList[i]));
      }
      UBS_MEM_FREE (p->hiddenPropertyList);
    }

    if (p->data) {
      UBS_MEM_FREE (p->data);
    }
  }
}


void
ubs_handle_msg_main_notifyNewMsg (int srcModule, ubs_notify_new_msg_t *p)
{
  ubs_module_t  *e = ownerViewerList[p->msgType].next;

  srcModule = srcModule; 


  while (e) {
    if (e->state == UBS_STATE_REGISTERED) {
      ubs_notifyNewMsg (e->modId, p->msgType, &(p->msg));
      e = e->next;
    }
  }

  
}


void
ubs_handle_msg_main_notifyMsgDeleted (int srcModule, ubs_notify_msg_deleted_t *p)
{
  ubs_module_t  *e = ownerViewerList[p->msgType].next;

  srcModule = srcModule; 


  while (e) {
    if (e->state == UBS_STATE_REGISTERED) {
      ubs_notifyMsgDeleted (e->modId, p->msgType, &(p->handle));
      e = e->next;
    }
  }

  
}


void
ubs_handle_msg_main_notifyMsgChanged (int srcModule, ubs_notify_msg_changed_t *p)
{
  ubs_module_t  *e = ownerViewerList[p->msgType].next;

  srcModule = srcModule; 


  while (e) {
    if (e->state == UBS_STATE_REGISTERED) {
      ubs_notifyMsgChanged (e->modId, p->msgType, &(p->msg));
      e = e->next;
    }
  }

  
}






static void 
ubs_register (int dst, int msgType)
{
  ubs_register_t        param;
  we_dcvt_t            cvt_obj;
  WE_UINT16            length;
  void                  *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(ubs_register)
  WE_LOG_FC_UINT8((WE_UINT8)dst, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  WE_LOG_FC_PRE_IMPL

  param.msgType = (WE_UINT8)msgType;
  param.transactionId = 0; 
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_register (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_REGISTER, WE_MODID_UBS,
                                     (WE_UINT8)dst, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)dst);
    ubs_cvt_register (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


static void 
ubs_deregister (int dst, int msgType)
{
  ubs_deregister_t      param;
  we_dcvt_t            cvt_obj;
  WE_UINT16            length;
  void                  *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(ubs_deregister)
  WE_LOG_FC_UINT8((WE_UINT8)dst, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  WE_LOG_FC_PRE_IMPL

  param.msgType = (WE_UINT8)msgType;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_deregister (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_DEREGISTER, WE_MODID_UBS,
                                     (WE_UINT8)dst, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)dst);
    ubs_cvt_deregister (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


static void 
ubs_getNbrOfMsgs (int dst,
                        unsigned transactionId,
                        int msgType, 
                        unsigned filterListSize, 
                        ubs_key_value_t *filterList)
{
  ubs_get_nbr_of_msgs_t param;
  we_dcvt_t            cvt_obj;
  WE_UINT16            length;
  void                  *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(ubs_getNbrOfMsgs)
  WE_LOG_FC_UINT8((WE_UINT8)dst, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)filterListSize, NULL)
  
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.filterListSize = (WE_UINT16)filterListSize;
  param.filterList = filterList;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_get_nbr_of_msgs (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_GET_NBR_OF_MSGS, WE_MODID_UBS,
                                     (WE_UINT8)dst, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)dst);
    ubs_cvt_get_nbr_of_msgs (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


static void 
ubs_getMsgList (int dst,
                      unsigned transactionId,
                      int msgType, 
                      unsigned filterListSize, 
                      ubs_key_value_t *filterList,
                      unsigned startFromMsg)
{
  ubs_get_msg_list_t    param;
  we_dcvt_t            cvt_obj;
  WE_UINT16            length;
  void                  *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(ubs_getMsgList)
  WE_LOG_FC_UINT8((WE_UINT8)dst, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)filterListSize, NULL)
  
  WE_LOG_FC_UINT16((WE_UINT16)startFromMsg, NULL)
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.filterListSize = (WE_UINT16)filterListSize;
  param.filterList = filterList;
  param.startFromMsg = (WE_UINT16)startFromMsg;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_get_msg_list (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_GET_MSG_LIST, WE_MODID_UBS,
                                     (WE_UINT8)dst, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)dst);
    ubs_cvt_get_msg_list (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


static void 
ubs_getMsg (int dst, 
                  unsigned transactionId,
                  int msgType, 
                  ubs_msg_handle_t *handle)
{
  ubs_get_msg_t         param;
  we_dcvt_t            cvt_obj;
  WE_UINT16            length;
  void                  *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(ubs_getMsg)
  WE_LOG_FC_UINT8((WE_UINT8)dst, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.handle = *handle;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_get_msg (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_GET_MSG, WE_MODID_UBS,
                                     (WE_UINT8)dst, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)dst);
    ubs_cvt_get_msg (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


static void 
ubs_deleteMsg (int dst,
                     unsigned transactionId,
                     int msgType,
                     ubs_msg_handle_t *handle)
{
  ubs_delete_msg_t      param;
  we_dcvt_t            cvt_obj;
  WE_UINT16            length;
  void                  *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(ubs_deleteMsg)
  WE_LOG_FC_UINT8((WE_UINT8)dst, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.handle = *handle;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_delete_msg (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_DELETE_MSG, WE_MODID_UBS,
                                     (WE_UINT8)dst, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)dst);
    ubs_cvt_delete_msg (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}




void
ubs_replyRegister (int dst, 
                        unsigned transactionId,
                        int msgType,
                        int result)
{
  ubs_reply_register_t    param;
  we_dcvt_t              cvt_obj;
  WE_UINT16              length;
  void                    *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(ubs_replyRegister)
  WE_LOG_FC_UINT8((WE_UINT8)dst, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  WE_LOG_FC_INT8((WE_INT8)result, NULL)
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.result = (WE_INT8)result;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_reply_register (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_REPLY_REGISTER, WE_MODID_UBS,
                                     (WE_UINT8)dst, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)dst);
    ubs_cvt_reply_register (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


void 
ubs_replyNbrOfMsgs (int dst, 
                          unsigned transactionId,
                          int msgType,
                          unsigned nbrOfMsgs)
{
  ubs_reply_nbr_of_msgs_t param;
  we_dcvt_t              cvt_obj;
  WE_UINT16              length;
  void                    *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(ubs_replyNbrOfMsgs)
  WE_LOG_FC_UINT8((WE_UINT8)dst, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)nbrOfMsgs, NULL)
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.nbrOfMsgs = (WE_UINT16)nbrOfMsgs;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_reply_nbr_of_msgs (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_REPLY_NBR_OF_MSGS, WE_MODID_UBS,
                                     (WE_UINT8)dst, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)dst);
    ubs_cvt_reply_nbr_of_msgs (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


void 
ubs_replyMsgList (int dst, 
                        unsigned transactionId,
                        int msgType,
                        unsigned msgListSize,
                        ubs_msg_list_item_t *msgList,
                        unsigned nextMsg)
{
  ubs_reply_msg_list_t    param;
  we_dcvt_t              cvt_obj;
  WE_UINT16              length;
  void                    *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(ubs_replyMsgList)
  WE_LOG_FC_UINT8((WE_UINT8)dst, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)msgListSize, NULL)
  
  WE_LOG_FC_UINT16((WE_UINT16)nextMsg, NULL)
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.msgListSize = (WE_UINT16)msgListSize;
  param.msgList = msgList;
  param.nextMsg = (WE_UINT16)nextMsg;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_reply_msg_list (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_REPLY_MSG_LIST, WE_MODID_UBS,
                                     (WE_UINT8)dst, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)dst);
    ubs_cvt_reply_msg_list (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


void 
ubs_replyMsg (int dst, 
                    unsigned transactionId,
                    int msgType,
                    int result,
                    ubs_msg_t *msg)
{
  ubs_reply_msg_t         param;
  we_dcvt_t              cvt_obj;
  WE_UINT16              length;
  void                    *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(ubs_replyMsg)
  WE_LOG_FC_UINT8((WE_UINT8)dst, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  WE_LOG_FC_INT8((WE_INT8)result, NULL)
  
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.result = (WE_INT8)result;
  param.msg = *msg;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_reply_msg (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_REPLY_MSG, WE_MODID_UBS,
                                     (WE_UINT8)dst, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)dst);
    ubs_cvt_reply_msg (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


void 
ubs_replyDeleteMsg (int dst,
                          unsigned transactionId,
                          int msgType,
                          ubs_msg_handle_t *handle,
                          int result)
{
  ubs_reply_delete_msg_t  param;
  we_dcvt_t              cvt_obj;
  WE_UINT16              length;
  void                    *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(ubs_replyDeleteMsg)
  WE_LOG_FC_UINT8((WE_UINT8)dst, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  
  WE_LOG_FC_INT8((WE_INT8)result, NULL)
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.handle = *handle;
  param.result = (WE_INT8)result;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_reply_delete_msg (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_REPLY_DELETE_MSG, WE_MODID_UBS,
                                     (WE_UINT8)dst, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)dst);
    ubs_cvt_reply_delete_msg (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


void
ubs_replySetMsgProperty (int dst,
                          unsigned transactionId,
                          int msgType,
                          ubs_msg_handle_t *handle,
                          int result)
{
  ubs_reply_set_msg_property_t  param;
  we_dcvt_t                    cvt_obj;
  WE_UINT16                    length;
  void                          *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(ubs_replySetMsgProperty)
  WE_LOG_FC_UINT8((WE_UINT8)dst, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  
  WE_LOG_FC_INT8((WE_INT8)result, NULL)
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.handle = *handle;
  param.result = (WE_INT8)result;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_reply_set_msg_property (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_REPLY_SET_MSG_PROPERTY, WE_MODID_UBS,
                                     (WE_UINT8)dst, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)dst);
    ubs_cvt_reply_set_msg_property (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


static void 
ubs_notifyMsgDeleted (int dst, 
                            int msgType,
                            ubs_msg_handle_t *handle)
{
  ubs_notify_msg_deleted_t  param;
  we_dcvt_t                cvt_obj;
  WE_UINT16                length;
  void                      *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(ubs_notifyMsgDeleted)
  WE_LOG_FC_UINT8((WE_UINT8)dst, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  
  WE_LOG_FC_PRE_IMPL

  param.msgType = (WE_UINT8)msgType;
  param.handle = *handle;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_notify_msg_deleted (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_NOTIFY_MSG_DELETED, WE_MODID_UBS,
                                     (WE_UINT8)dst, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)dst);
    ubs_cvt_notify_msg_deleted (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


static void 
ubs_notifyMsgChanged (int dst, 
                            int msgType,
                            ubs_msg_list_item_t *msg)
{
  ubs_notify_msg_changed_t  param;
  we_dcvt_t                cvt_obj;
  WE_UINT16                length;
  void                      *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(ubs_notifyMsgChanged)
  WE_LOG_FC_UINT8((WE_UINT8)dst, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  
  WE_LOG_FC_PRE_IMPL

  param.msgType = (WE_UINT8)msgType;
  param.msg = *msg;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_notify_msg_changed (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_NOTIFY_MSG_CHANGED, WE_MODID_UBS,
                                     (WE_UINT8)dst, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)dst);
    ubs_cvt_notify_msg_changed (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


static void 
ubs_notifyNewMsg (int dst, 
                        int msgType,
                        ubs_msg_list_item_t *msg)
{
  ubs_notify_new_msg_t      param;
  we_dcvt_t                cvt_obj;
  WE_UINT16                length;
  void                      *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(ubs_notifyNewMsg)
  WE_LOG_FC_UINT8((WE_UINT8)dst, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  
  WE_LOG_FC_PRE_IMPL

  param.msgType = (WE_UINT8)msgType;
  param.msg = *msg;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_notify_new_msg (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_NOTIFY_NEW_MSG, WE_MODID_UBS,
                                     (WE_UINT8)dst, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)dst);
    ubs_cvt_notify_new_msg (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


static void 
ubs_notifyInvalidateAll (int dst, int msgType)
{
  ubs_notify_invalidate_all_t     param;
  we_dcvt_t                      cvt_obj;
  WE_UINT16                      length;
  void                            *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(ubs_notifyInvalidateAll)
  WE_LOG_FC_UINT8((WE_UINT8)dst, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  WE_LOG_FC_PRE_IMPL

  param.msgType = (WE_UINT8)msgType;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_notify_invalidate_all (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_NOTIFY_INVALIDATE_ALL, WE_MODID_UBS,
                                     (WE_UINT8)dst, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)dst);
    ubs_cvt_notify_invalidate_all (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


static void 
ubs_setMsgProperty (int dst,
                      unsigned transactionId,
                      int msgType,
                      ubs_msg_handle_t *handle,
                      ubs_key_value_t *propertyValue)
{
  ubs_set_msg_property_t  param;
  we_dcvt_t              cvt_obj;
  WE_UINT16              length;
  void                    *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(ubs_setMsgProperty)
  WE_LOG_FC_UINT8((WE_UINT8)dst, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  
  
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.handle = *handle;
  param.propertyValue = *propertyValue;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_set_msg_property (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_SET_MSG_PROPERTY, WE_MODID_UBS,
                                     (WE_UINT8)dst, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)dst);
    ubs_cvt_set_msg_property (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}




void 
ubs_replyFullMsg (int dst,
                        unsigned transactionId,
                        int msgType,
                        int result,
                        ubs_full_msg_t *msg)
{
  ubs_reply_full_msg_t    param;
  we_dcvt_t              cvt_obj;
  WE_UINT16              length;
  void                    *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(ubs_replyFullMsg)
  WE_LOG_FC_UINT8((WE_UINT8)dst, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  WE_LOG_FC_INT8((WE_INT8)result, NULL)
  
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.result = (WE_INT8)result;
  param.msg = *msg;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_reply_full_msg (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_REPLY_FULL_MSG, WE_MODID_UBS,
                                     (WE_UINT8)dst, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)dst);
    ubs_cvt_reply_full_msg (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


void 
ubs_replyChangeMsg (int dst, 
                          unsigned transactionId,
                          int msgType,
                          ubs_msg_handle_t *handle,
                          int result)
{
  ubs_reply_change_msg_t  param;
  we_dcvt_t              cvt_obj;
  WE_UINT16              length;
  void                    *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(ubs_replyChangeMsg)
  WE_LOG_FC_UINT8((WE_UINT8)dst, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  
  WE_LOG_FC_INT8((WE_INT8)result, NULL)
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.handle = *handle;
  param.result = (WE_INT8)result;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_reply_change_msg (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_REPLY_CHANGE_MSG, WE_MODID_UBS,
                                     (WE_UINT8)dst, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)dst);
    ubs_cvt_reply_change_msg (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}


void 
ubs_replyCreateMsg (int dst, 
                          unsigned transactionId,
                          int msgType,
                          ubs_msg_handle_t *handle,
                          int result)
{
  ubs_reply_create_msg_t      param;
  we_dcvt_t                  cvt_obj;
  WE_UINT16                  length;
  void                        *signal_buffer, *user_data;

  WE_LOG_FC_BEGIN(ubs_replyCreateMsg)
  WE_LOG_FC_UINT8((WE_UINT8)dst, NULL)
  WE_LOG_FC_UINT16((WE_UINT16)transactionId, NULL)
  WE_LOG_FC_UINT8((WE_UINT8)msgType, NULL)
  
  WE_LOG_FC_INT8((WE_INT8)result, NULL)
  WE_LOG_FC_PRE_IMPL

  param.transactionId = (WE_UINT16)transactionId;
  param.msgType = (WE_UINT8)msgType;
  param.handle = *handle;
  param.result = (WE_INT8)result;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  ubs_cvt_reply_create_msg (&cvt_obj, &param);
  length = (WE_UINT16)cvt_obj.pos;

  signal_buffer = WE_SIGNAL_CREATE (UBS_SIG_REPLY_CREATE_MSG, WE_MODID_UBS,
                                     (WE_UINT8)dst, length);
  if (signal_buffer != NULL) {
    user_data = WE_SIGNAL_GET_USER_DATA (signal_buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, (WE_UINT8)dst);
    ubs_cvt_reply_create_msg (&cvt_obj, &param);

    WE_SIGNAL_SEND (signal_buffer);
  }
  WE_LOG_FC_END
}





static void
ubs_main_free_msg_list_item (ubs_msg_list_item_t *p)
{
  int i;

  if (p) 
  {
    ubs_main_free_msg_handle (&(p->handle));

    if (p->titleList)
    {
      for (i = 0; i < (p->titleListSize); i++) {
        ubs_main_free_key_value (&(p->titleList[i]));
      }
      UBS_MEM_FREE (p->titleList);
    }
  }
}


static void
ubs_main_free_msg_array (ubs_msg_list_item_t **p, int arraySize)
{
  if (*p) 
  {
    int i;
    for (i = 0; i < arraySize; i++ ) {
      ubs_main_free_msg_list_item (&((*p)[i]));
    }
    UBS_MEM_FREE (*p);
  }
}


static void
ubs_main_free_msg (ubs_msg_t *p)
{
  int i;

  if (p) 
  {
    ubs_main_free_msg_handle (&(p->handle));

    if (p->propertyList)
    {
      for (i = 0; i < (p->propertyListSize); i++) {
        ubs_main_free_key_value (&(p->propertyList[i]));
      }
      UBS_MEM_FREE (p->propertyList);
    }
  }
}
 
 



static void
ubs_memory_exception_handler (void)
{
  if (ubs_inside_run) {
    longjmp (ubs_jmp_buf, 1);
  }
}


static void
ubs_main_init (void)
{
  int i;
  int msgTypeOwnerArray[UBS_NBR_OF_MSG_TYPES] = UBS_MSG_TYPE_OWNER_ARRAY;

  
  for (i = 0; i < UBS_NBR_OF_MSG_TYPES; i++)
  {
    ownerViewerList[i].modId = msgTypeOwnerArray[i];
    if (ownerViewerList[i].modId == WE_MODID_UBS) {
      ownerViewerList[i].state = UBS_STATE_REGISTERED;
    }
    else {
      ownerViewerList[i].state = UBS_STATE_NOT_STARTED;
    }
    ownerViewerList[i].next = NULL;
  }

  
  transactionList = NULL;
}


static int
ubs_main_registerTransaction (int src, unsigned srcTransactionId, int msgType)
{
  static int        currentTransactionId = 1;
  ubs_transaction_t *newTransaction;


  if (currentTransactionId == 0x7FFF) {
    currentTransactionId = 1;
  }
  else {
    currentTransactionId++;
  }

  newTransaction = UBS_MEM_ALLOCTYPE (ubs_transaction_t);
  newTransaction->wid = currentTransactionId;
  newTransaction->modId = src;
  newTransaction->srcTransactionId = srcTransactionId;
  newTransaction->msgType = msgType;
  newTransaction->next = transactionList;
  transactionList = newTransaction;

  return currentTransactionId;
}


static void
ubs_main_deregisterModuleTransactions (int src, int msgType)
{
  ubs_transaction_t   *p = transactionList;
  ubs_transaction_t   *pre_p = NULL;

  while (p) 
  {
    if ((p->modId != src) || (p->msgType != msgType)) 
    {
      pre_p = p;
      p = p->next;
    }
    else 
    {
      if (!pre_p) {
        transactionList = p->next;
      }
      else {
        pre_p->next = p->next;
      }

      UBS_MEM_FREE (p);

      if (!pre_p) {
        p = transactionList;
      }
      else {
        p = pre_p->next;
      }
    }
  }
}






static void
ubs_handle_msg_moduleStatus (int srcModule, we_module_status_t *p)
{
  int bRegister = FALSE;
  int i;
  ubs_module_t  *e;

  srcModule = srcModule; 

  switch (p->status) {
  case MODULE_STATUS_ACTIVE:
    
    i = 0;
    while ((i < UBS_NBR_OF_MSG_TYPES) && (ownerViewerList[i].modId != p->modId)) {
      i++;
    }
    if (ownerViewerList[i].modId == p->modId) {
      
      ownerViewerList[i].state = UBS_STATE_STARTED;

      e = ownerViewerList[i].next;
      while (e) {
        bRegister = (e->state == UBS_STATE_ASKED_TO_REGISTER);
        e = e->next;
      }
      if (bRegister) {
        ubs_register (p->modId, i);
        ownerViewerList[i].state = UBS_STATE_ASKED_TO_REGISTER;
      }
    }
    break;

  case MODULE_STATUS_TERMINATED:
    
    i = 0;
    while ((i < UBS_NBR_OF_MSG_TYPES) && (ownerViewerList[i].modId != p->modId)) {
      i++;
    }
    if (ownerViewerList[i].modId == p->modId) 
    {
      
      ownerViewerList[i].state = UBS_STATE_NOT_STARTED;

      e = ownerViewerList[i].next;
      while (e) {
        bRegister = (e->state == UBS_STATE_ASKED_TO_REGISTER);
        e = e->next;
      }
      
      if (bRegister) 
      {
        WE_MODULE_START(WE_MODID_UBS, 
                          (WE_UINT8)(ownerViewerList[i].modId), "", NULL, NULL);
        ownerViewerList[i].state = UBS_STATE_ASKED_TO_START;
      }
    }
    break;
  }

  UBS_MEM_FREE (p->modVersion);
  UBS_MEM_FREE (p);
}


static void
ubs_handle_msg_main_register (int srcModule, ubs_register_t *p)
{
  ubs_module_t  *viewer;

  if ((p->msgType < UBS_NBR_OF_MSG_TYPES) && ownerViewerList[p->msgType].modId != -1)
  {
    
    viewer = UBS_MEM_ALLOCTYPE (ubs_module_t);
    viewer->modId = srcModule;

    switch (ownerViewerList[p->msgType].state) {
    case UBS_STATE_REGISTERED:
      viewer->state = UBS_STATE_REGISTERED;
      ubs_replyRegister (srcModule, p->transactionId, p->msgType, UBS_RESULT_SUCCESS);
      break;

    case UBS_STATE_STARTED:
      ubs_register (ownerViewerList[p->msgType].modId, p->msgType);
      viewer->state = UBS_STATE_ASKED_TO_REGISTER;
      ownerViewerList[p->msgType].state = UBS_STATE_ASKED_TO_REGISTER;
      break;

    case UBS_STATE_NOT_STARTED:
      
      WE_MODULE_START(WE_MODID_UBS, 
                        (WE_UINT8)(ownerViewerList[p->msgType].modId),"", NULL, NULL);
      ownerViewerList[p->msgType].state = UBS_STATE_ASKED_TO_START;
      
      
    case UBS_STATE_ASKED_TO_STOP:
      
      
    case UBS_STATE_ASKED_TO_START:
      
      
    case UBS_STATE_ASKED_TO_REGISTER:
      viewer->state = UBS_STATE_ASKED_TO_REGISTER;
      break;
    }

    
    viewer->next = ownerViewerList[p->msgType].next;
    ownerViewerList[p->msgType].next = viewer;
  }
  else
  {
    ubs_replyRegister (srcModule, p->transactionId, p->msgType, UBS_RESULT_FAILURE);
  }

  UBS_MEM_FREE (p);
}


static void
ubs_handle_msg_main_replyRegister (int srcModule, ubs_reply_register_t *p)
{
  ubs_module_t  *e = ownerViewerList[p->msgType].next;

  srcModule = srcModule; 

  while (e) {
    if (e->state == UBS_STATE_ASKED_TO_REGISTER) {
      
      e->state = UBS_STATE_REGISTERED; 
      ubs_replyRegister (e->modId, 0, p->msgType, p->result);
      e = e->next;
    }
  }

  if (p->result == UBS_RESULT_FAILURE) {
    
    while (ownerViewerList[p->msgType].next) {
      e = ownerViewerList[p->msgType].next;
      ownerViewerList[p->msgType].next = e->next;
      UBS_MEM_FREE (e);
    }
  }
  else {
    
    ownerViewerList[p->msgType].state = UBS_STATE_REGISTERED;
  }

  UBS_MEM_FREE (p);
}


static void
ubs_handle_msg_main_deregister  (int srcModule, ubs_deregister_t *p)
{
  ubs_module_t  *e = ownerViewerList[p->msgType].next;
  ubs_module_t  *pre_e = &ownerViewerList[p->msgType];

  if (ownerViewerList[p->msgType].modId != -1)
  {
    while (e && (e->modId != srcModule)) {
      pre_e = e;
      e = e->next;
    }

    if (e) {
      pre_e->next = e->next;
      UBS_MEM_FREE (e);
    }

    if (ownerViewerList[p->msgType].modId != WE_MODID_UBS) {
      if (ownerViewerList[p->msgType].next == NULL) {
        
        ubs_deregister (ownerViewerList[p->msgType].modId, p->msgType);
        WE_MODULE_TERMINATE (WE_MODID_UBS, (WE_UINT8)ownerViewerList[p->msgType].modId);
        ownerViewerList[p->msgType].state = UBS_STATE_ASKED_TO_STOP;
      }
    }
  }

  
  ubs_main_deregisterModuleTransactions (srcModule, p->msgType);

  UBS_MEM_FREE (p);
}


static void
ubs_handle_msg_main_getNbrOfMsgs (int srcModule, ubs_get_nbr_of_msgs_t *p)
{
  p->transactionId = (WE_UINT16)ubs_main_registerTransaction (srcModule, 
                                            p->transactionId, p->msgType);
  
  

  if (ownerViewerList[p->msgType].modId != WE_MODID_UBS) {
    
    ubs_getNbrOfMsgs (ownerViewerList[p->msgType].modId, p->transactionId, p->msgType, 
        p->filterListSize, p->filterList);

    ubs_main_free_key_value_array (&(p->filterList), p->filterListSize);
    UBS_MEM_FREE (p);
  }
  else {
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MSG_STORE, UBS_MSG_STORE_SIG_GET_NBR_OF_MSGS, 
      srcModule, p);
  }
}


static void
ubs_handle_msg_main_replyNbrOfMsgs (int srcModule, ubs_reply_nbr_of_msgs_t *p)
{
  unsigned srcTransactionId;
  int dstModule = ubs_main_deregisterTransaction (p->transactionId, &srcTransactionId);

  srcModule = srcModule; 

  if (dstModule >= 0) {
    ubs_replyNbrOfMsgs (dstModule, srcTransactionId, p->msgType, p->nbrOfMsgs);
  }

  UBS_MEM_FREE (p);
}


static void
ubs_handle_msg_main_getMsgList (int srcModule, ubs_get_msg_list_t *p)
{
  p->transactionId = (WE_UINT16)ubs_main_registerTransaction (srcModule, 
                                            p->transactionId, p->msgType);
  
  


  if (ownerViewerList[p->msgType].modId != WE_MODID_UBS) {
    
    ubs_getMsgList (ownerViewerList[p->msgType].modId, p->transactionId, p->msgType, 
        p->filterListSize, p->filterList, p->startFromMsg);

    ubs_main_free_key_value_array (&(p->filterList), p->filterListSize);
    UBS_MEM_FREE (p);
  }
  else {
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MSG_STORE, UBS_MSG_STORE_SIG_GET_MSG_LIST, 
      srcModule, p);
  }
}


static void
ubs_handle_msg_main_replyMsgList (int srcModule, ubs_reply_msg_list_t *p)
{
  unsigned srcTransactionId;
  int dstModule = ubs_main_deregisterTransaction (p->transactionId, &srcTransactionId);

  srcModule = srcModule; 

  if (dstModule >= 0) {
    ubs_replyMsgList (dstModule, srcTransactionId, p->msgType, p->msgListSize, 
        p->msgList, p->nextMsg);
  }

  ubs_main_free_msg_array (&(p->msgList), p->msgListSize);
  UBS_MEM_FREE (p);
}


static void
ubs_handle_msg_main_getMsg (int srcModule, ubs_get_msg_t *p)
{
  p->transactionId = (WE_UINT16)ubs_main_registerTransaction (srcModule, 
                                            p->transactionId, p->msgType);
  
  


  if (ownerViewerList[p->msgType].modId != WE_MODID_UBS) {
    
    ubs_getMsg (ownerViewerList[p->msgType].modId, p->transactionId, 
                p->msgType, &(p->handle));

    ubs_main_free_msg_handle (&(p->handle));
    UBS_MEM_FREE (p);
  }
  else {
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MSG_STORE, UBS_MSG_STORE_SIG_GET_MSG, 
      srcModule, p);
  }
}


static void
ubs_handle_msg_main_replyMsg (int srcModule, ubs_reply_msg_t *p)
{
  unsigned srcTransactionId;
  int dstModule = ubs_main_deregisterTransaction (p->transactionId, &srcTransactionId);

  srcModule = srcModule; 

  if (dstModule >= 0) {
    ubs_replyMsg (dstModule, srcTransactionId, p->msgType, p->result, 
        &(p->msg));
  }

  ubs_main_free_msg (&(p->msg));
  UBS_MEM_FREE (p);
}


static void
ubs_handle_msg_main_getFullMsg (int srcModule, ubs_get_full_msg_t *p)
{
  p->transactionId = (WE_UINT16)ubs_main_registerTransaction (srcModule, 
                                            p->transactionId, p->msgType);
  
  


  if (ownerViewerList[p->msgType].modId == WE_MODID_UBS) {
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MSG_STORE, UBS_MSG_STORE_SIG_GET_FULL_MSG, 
      srcModule, p);
  }
  else {
    
    ubs_main_free_msg_handle (&(p->handle));
    UBS_MEM_FREE (p);
  }
}


static void
ubs_handle_msg_main_replyFullMsg (int srcModule, ubs_reply_full_msg_t *p)
{
  unsigned srcTransactionId;
  int dstModule = ubs_main_deregisterTransaction (p->transactionId, &srcTransactionId);

  srcModule = srcModule; 

  if (dstModule >= 0) {
    ubs_replyFullMsg (dstModule, srcTransactionId, p->msgType, p->result, 
        &(p->msg));
  }

  ubs_main_free_full_msg (&(p->msg));
  UBS_MEM_FREE (p);
}


static void
ubs_handle_msg_main_deleteMsg (int srcModule, ubs_delete_msg_t *p)
{
  p->transactionId = (WE_UINT16)ubs_main_registerTransaction (srcModule, 
                                            p->transactionId, p->msgType);
  
  

  if (ownerViewerList[p->msgType].modId != WE_MODID_UBS) {
    
    ubs_deleteMsg (ownerViewerList[p->msgType].modId, p->transactionId, 
                    p->msgType, &(p->handle));

    ubs_main_free_msg_handle (&(p->handle));
    UBS_MEM_FREE (p);
  }
  else {
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MSG_STORE, UBS_MSG_STORE_SIG_DELETE_MSG, 
      srcModule, p);
  }
}


static void
ubs_handle_msg_main_replyDeleteMsg (int srcModule, ubs_reply_delete_msg_t *p)
{
  unsigned srcTransactionId;
  int dstModule = ubs_main_deregisterTransaction (p->transactionId, &srcTransactionId);

  srcModule = srcModule; 

  if (dstModule >= 0) {
    ubs_replyDeleteMsg (dstModule, srcTransactionId, p->msgType, &(p->handle), p->result);
  }

  ubs_main_free_msg_handle (&(p->handle));
  UBS_MEM_FREE (p);
}


static void
ubs_handle_msg_main_changeMsg (int srcModule, ubs_change_msg_t *p)
{
  p->transactionId = (WE_UINT16)ubs_main_registerTransaction (srcModule, 
                                            p->transactionId, p->msgType);
  
  

  if (ownerViewerList[p->msgType].modId == WE_MODID_UBS) 
  {
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MSG_STORE, UBS_MSG_STORE_SIG_CHANGE_MSG, 
      srcModule, p);
  }
  else {
    
    ubs_main_free_full_msg (&(p->msg));
    UBS_MEM_FREE (p);
  }
}


static void
ubs_handle_msg_main_replyChangeMsg (int srcModule, ubs_reply_change_msg_t *p)
{
  unsigned srcTransactionId;
  int dstModule = ubs_main_deregisterTransaction (p->transactionId, &srcTransactionId);

  srcModule = srcModule; 

  if (dstModule >= 0) {
    ubs_replyChangeMsg (dstModule, srcTransactionId, p->msgType, &(p->handle), p->result);
  }

  ubs_main_free_msg_handle (&(p->handle));
  UBS_MEM_FREE (p);
}


static void
ubs_handle_msg_main_createMsg (int srcModule, ubs_create_msg_t *p)
{
  p->transactionId = (WE_UINT16)ubs_main_registerTransaction (srcModule, 
                                            p->transactionId, p->msgType);
  
  

  if (ownerViewerList[p->msgType].modId == WE_MODID_UBS) 
  {
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MSG_STORE, UBS_MSG_STORE_SIG_CREATE_MSG, 
      srcModule, p);
  }
  else {
    
    ubs_main_free_full_msg (&(p->msg));
    UBS_MEM_FREE (p);
  }
}


static void
ubs_handle_msg_main_replyCreateMsg (int srcModule, ubs_reply_create_msg_t *p)
{
  unsigned srcTransactionId;
  int dstModule = ubs_main_deregisterTransaction (p->transactionId, &srcTransactionId);

  srcModule = srcModule; 

  if (dstModule >= 0) {
    ubs_replyCreateMsg (dstModule, srcTransactionId, p->msgType, &(p->handle), p->result);
  }

  ubs_main_free_msg_handle (&(p->handle));
  UBS_MEM_FREE (p);
}


static void
ubs_handle_msg_main_notifyUnreadMsgCount (int srcModule, ubs_notify_unread_msg_count_t *p)
{
  UNREFERENCED_PARAMETER (srcModule);

  UBSa_notifyUnreadMsgCount (p->msgType, p->nbrOfUnreadMsgs);

  UBS_MEM_FREE (p);
}


static void
ubs_handle_msg_notifyInvalidateAll (int srcModule, ubs_notify_invalidate_all_t *p)
{
  ubs_module_t  *e = ownerViewerList[p->msgType].next;

  UNREFERENCED_PARAMETER (srcModule);

  while (e) {
    if (e->state == UBS_STATE_REGISTERED) {
      ubs_notifyInvalidateAll (e->modId, p->msgType);
      e = e->next;
    }
  }

  
}


static void
ubs_handle_msg_setMsgProperty (int srcModule, ubs_set_msg_property_t *p)
{
  p->transactionId = (WE_UINT16)ubs_main_registerTransaction (srcModule, 
                                            p->transactionId, p->msgType);
  
  

  if (ownerViewerList[p->msgType].modId != WE_MODID_UBS) {
    
    ubs_setMsgProperty (ownerViewerList[p->msgType].modId, p->transactionId, 
                    p->msgType, &(p->handle), &(p->propertyValue));

    ubs_main_free_msg_handle (&(p->handle));
    ubs_main_free_key_value (&(p->propertyValue));
    UBS_MEM_FREE (p);
  }
  else {
    UBS_SIGNAL_SENDTO_UP (UBS_MODULE_MSG_STORE, UBS_MSG_STORE_SIG_SET_MSG_PROPERTY, 
      srcModule, p);
  }
}


static void
ubs_handle_msg_replySetMsgProperty (int srcModule, ubs_reply_set_msg_property_t *p)
{
  unsigned srcTransactionId;
  int dstModule = ubs_main_deregisterTransaction (p->transactionId, &srcTransactionId);

  UNREFERENCED_PARAMETER (srcModule);

  if (dstModule >= 0) {
    ubs_replySetMsgProperty (dstModule, srcTransactionId, p->msgType, &(p->handle), p->result);
  }

  ubs_main_free_msg_handle (&(p->handle));
  UBS_MEM_FREE (p);
}











static void
ubs_main (ubs_signal_t *sig)
{
  switch (sig->type) {
  
  case UBS_MAIN_SIG_REGISTER:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_REGISTER\n"));
    ubs_handle_msg_main_register (sig->u_param, (ubs_register_t*)sig->p_param);
    break;

  case UBS_MAIN_SIG_REPLY_REGISTER:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_REPLY_REGISTER\n"));
    ubs_handle_msg_main_replyRegister (sig->u_param, (ubs_reply_register_t*)sig->p_param);
    break;

  case UBS_MAIN_SIG_DEREGISTER:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_DEREGISTER\n"));
    ubs_handle_msg_main_deregister (sig->u_param, (ubs_deregister_t*)sig->p_param);
    break;

  case UBS_MAIN_SIG_GET_NBR_OF_MSGS:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_GET_NBR_OF_MSGS\n"));
    ubs_handle_msg_main_getNbrOfMsgs (sig->u_param, (ubs_get_nbr_of_msgs_t*)sig->p_param);
    break;

  case UBS_MAIN_SIG_REPLY_NBR_OF_MSGS:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_REPLY_NBR_OF_MSGS\n"));
    ubs_handle_msg_main_replyNbrOfMsgs (sig->u_param, (ubs_reply_nbr_of_msgs_t*)sig->p_param);
    break;

  case UBS_MAIN_SIG_GET_MSG_LIST:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_GET_MSG_LIST\n"));
    ubs_handle_msg_main_getMsgList (sig->u_param, (ubs_get_msg_list_t*)sig->p_param);
    break;

  case UBS_MAIN_SIG_REPLY_MSG_LIST:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_REPLY_MSG_LIST\n"));
    ubs_handle_msg_main_replyMsgList (sig->u_param, (ubs_reply_msg_list_t*)sig->p_param);
    break;

  case UBS_MAIN_SIG_GET_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_GET_MSG\n"));
    ubs_handle_msg_main_getMsg (sig->u_param, (ubs_get_msg_t*)sig->p_param);
    break;

  case UBS_MAIN_SIG_REPLY_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_REPLY_MSG\n"));
    ubs_handle_msg_main_replyMsg (sig->u_param, (ubs_reply_msg_t*)sig->p_param);
    break;

  case UBS_MAIN_SIG_GET_FULL_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_GET_FULL_MSG\n"));
    ubs_handle_msg_main_getFullMsg (sig->u_param, (ubs_get_full_msg_t*)sig->p_param);
    break;

  case UBS_MAIN_SIG_REPLY_FULL_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_REPLY_FULL_MSG\n"));
    ubs_handle_msg_main_replyFullMsg (sig->u_param, (ubs_reply_full_msg_t*)sig->p_param);
    break;

  case UBS_MAIN_SIG_DELETE_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_DELETE_MSG\n"));
    ubs_handle_msg_main_deleteMsg (sig->u_param, (ubs_delete_msg_t*)sig->p_param);
    break;

  case UBS_MAIN_SIG_REPLY_DELETE_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_REPLY_DELETE_MSG\n"));
    ubs_handle_msg_main_replyDeleteMsg (sig->u_param, (ubs_reply_delete_msg_t*)sig->p_param);
    break;

  case UBS_MAIN_SIG_NOTIFY_MSG_DELETED:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_NOTIFY_MSG_DELETED\n"));
    ubs_handle_msg_main_notifyMsgDeleted (sig->u_param, (ubs_notify_msg_deleted_t*)sig->p_param);
    ubs_main_free_msg_handle (&(((ubs_notify_msg_deleted_t*)sig->p_param)->handle));
    UBS_MEM_FREE (sig->p_param);
    break;

  case UBS_MAIN_SIG_CHANGE_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_CHANGE_MSG\n"));
    ubs_handle_msg_main_changeMsg (sig->u_param, (ubs_change_msg_t*)sig->p_param);
    break;

  case UBS_MAIN_SIG_REPLY_CHANGE_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_REPLY_CHANGE_MSG\n"));
    ubs_handle_msg_main_replyChangeMsg (sig->u_param, (ubs_reply_change_msg_t*)sig->p_param);
    break;

  case UBS_MAIN_SIG_NOTIFY_MSG_CHANGED:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_NOTIFY_MSG_CHANGED\n"));
    ubs_handle_msg_main_notifyMsgChanged (sig->u_param, (ubs_notify_msg_changed_t*)sig->p_param);
    ubs_main_free_msg_list_item (&(((ubs_notify_msg_changed_t*)sig->p_param)->msg));
    UBS_MEM_FREE (sig->p_param);
    break;

  case UBS_MAIN_SIG_NOTIFY_NEW_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_NOTIFY_NEW_MSG\n"));
    ubs_handle_msg_main_notifyNewMsg (sig->u_param, (ubs_notify_new_msg_t*)sig->p_param);
    ubs_main_free_msg_list_item (&(((ubs_notify_new_msg_t*)sig->p_param)->msg));
    UBS_MEM_FREE (sig->p_param);
    break;

  case UBS_MAIN_SIG_CREATE_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_CREATE_MSG\n"));
    ubs_handle_msg_main_createMsg (sig->u_param, (ubs_create_msg_t*)sig->p_param);
    break;

  case UBS_MAIN_SIG_REPLY_CREATE_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_REPLY_CREATE_MSG\n"));
    ubs_handle_msg_main_replyCreateMsg (sig->u_param, (ubs_reply_create_msg_t*)sig->p_param);
    break;
    
  case UBS_MAIN_SIG_NOTIFY_UNREAD_MSG_COUNT:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_NOTIFY_UNREAD_MSG_COUNT\n"));
    ubs_handle_msg_main_notifyUnreadMsgCount (sig->u_param, (ubs_notify_unread_msg_count_t*)sig->p_param);
    break;

  case UBS_MAIN_SIG_MODULE_STATUS:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_MODULE_STATUS\n"));
    ubs_handle_msg_moduleStatus (sig->u_param, (we_module_status_t*)sig->p_param);
    break;

  case UBS_MAIN_SIG_NOTIFY_INVALIDATE_ALL:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_NOTIFY_INVALIDATE_ALL\n"));
    ubs_handle_msg_notifyInvalidateAll (sig->u_param, (ubs_notify_invalidate_all_t*)sig->p_param);
    UBS_MEM_FREE (sig->p_param);
    break;

  case UBS_MAIN_SIG_SET_MSG_PROPERTY:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_SET_MSG_PROPERTY\n"));
    ubs_handle_msg_setMsgProperty (sig->u_param, (ubs_set_msg_property_t*)sig->p_param);
    break;

  case UBS_MAIN_SIG_REPLY_SET_MSG_PROPERTY:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MAIN_SIG_REPLY_SET_MSG_PROPERTY\n"));
    ubs_handle_msg_replySetMsgProperty (sig->u_param, (ubs_reply_set_msg_property_t*)sig->p_param);
    break;

  default:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS, 
                  "UBS: received unexpected signal: %d\n", sig->type));
    break;

  }

  ubs_signal_delete (sig);
}

