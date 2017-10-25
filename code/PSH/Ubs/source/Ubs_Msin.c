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









#include "Ubs_Msin.h"
#include "Ubs_Job.h"
#include "Ubs_Main.h"
#include "Ubs_List.h"
#include "Ubs_Mtpy.h"

#include "We_File.h"
#include "We_Log.h"


#undef UBS_MSG_TYPE_ADD_JOB






#define UBS_MSG_STORE_PATH_SEPARATOR        "/"

#define UBS_MSG_STORE_FOLDER                "/ubs"

#define UBS_MSG_STORE_MSG_TYPE_INDEX_FILE   "%i.ndx"

#define UBS_MSG_STORE_FOLDER_MSG_TYPE_INDEX_FILE "/ubs/%i.ndx"

#define UBS_MSG_STORE_MSG_TYPE_FOLDER       "/ubs/%i"





typedef struct {
  int                   start;
  ubs_get_full_msg_t    *sig;
  ubs_full_msg_t        result_msg;
  int                   subjob_hiddenProp_result;
  ubs_key_value_array_t subjob_hiddenProp_result_propArray;
  int                   subjob_data_result;
  long                  subjob_data_result_dataLen;
  unsigned char         *subjob_data_result_data;
} ubs_msg_store_execJob_getFullMsg_t;

typedef struct {
  int                   start;
  ubs_delete_msg_t      *sig;
  long                   subjob_result;
} ubs_msg_store_execJob_deleteMsg_t;

typedef struct {
  int                     start;
  ubs_change_msg_t        *sig;
  ubs_set_msg_property_t  *sig2;
  ubs_key_value_array_t   propArray;
  ubs_key_value_array_t   hiddenPropArray;
  int                     subjob_result;
} ubs_msg_store_execJob_changeMsg_t;

typedef struct {
  int                     start;
  ubs_set_msg_property_t  *sig;
  ubs_key_value_array_t   propArray;
  int                     subjob_result;
} ubs_msg_store_execJob_setMsgProperty_t;

typedef struct {
  int                   start;
  ubs_create_msg_t      *sig;
  ubs_key_value_array_t propArray;
  ubs_key_value_array_t hiddenPropArray;
  int                   subjob_result;
} ubs_msg_store_execJob_createMsg_t;

typedef enum {
  ubs_msg_store_state_running,
  ubs_msg_store_state_stopping,
  ubs_msg_store_state_stopped
} ubs_msg_store_state_t;












ubs_msg_store_state_t   msg_store_state;
ubs_list_t              msgTypeList;
int                     total_nbr_of_push_si_and_sl;





static void
ubs_msg_store_main (ubs_signal_t *sig);


static int
ubs_msg_store_initUbsFileStructure (void);

static void
ubs_msg_store_readIndexFiles (void);


static void
ubs_msg_store_deallocMemory (void);

static void
ubs_msg_store_clear_inactive_jobs (void);








void
ubs_msg_store_init (void)
{
  total_nbr_of_push_si_and_sl = 0; 
  msg_store_state = ubs_msg_store_state_running;

  ubs_list_init(&msgTypeList, WE_MODID_UBS);

  ubs_msg_store_initUbsFileStructure ();
  ubs_msg_store_readIndexFiles ();

  ubs_signal_register_dst (UBS_MODULE_MSG_STORE, ubs_msg_store_main);
}




void
ubs_msg_store_terminate (void)
{
  total_nbr_of_push_si_and_sl = 0; 
  msg_store_state = ubs_msg_store_state_stopping;
  ubs_msg_store_clear_inactive_jobs ();
}


void
ubs_msg_store_terminate_done (void)
{
  ubs_msg_store_deallocMemory ();
  msg_store_state = ubs_msg_store_state_stopped;
  ubs_terminate_msg_store_done ();
}






static void
ubs_msg_store_clear_inactive_jobs (void)
{
  ubs_list_iterator_t it = ubs_list_begin (&msgTypeList);
  ubs_list_iterator_t end = ubs_list_end (&msgTypeList);
  ubs_msg_type_t      *type;

  while (UBS_LIST_ITERATOR_NOT_EQUAL(it,end)){
    type = (ubs_msg_type_t*)(UBS_LIST_ITERATOR_GET(it));

    if (ubs_msg_ready_to_terminate (type)) {
      it = ubs_list_erase (it);
      ubs_msg_type_release (type);
      UBS_MEM_FREE (type);
    }
    else {
      UBS_LIST_ITERATOR_NEXT(it);
    }
  }

  if (UBS_LIST_EMPTY (&msgTypeList)) {
    
    ubs_msg_store_terminate_done ();
  }
}


static ubs_msg_type_t*
ubs_msg_store_findMsgType (int msgType)
{
  ubs_list_iterator_t it = ubs_list_begin (&msgTypeList);
  ubs_list_iterator_t end = ubs_list_end (&msgTypeList);
  ubs_msg_type_t* type;
  for(;UBS_LIST_ITERATOR_NOT_EQUAL(it,end);UBS_LIST_ITERATOR_NEXT(it)){
    type = (ubs_msg_type_t*)(UBS_LIST_ITERATOR_GET(it));
    if (type->msg_type == msgType) {
      return type;
    }
  }
  return NULL;
}


static ubs_msg_type_t*
ubs_msg_store_newMsgType (int msgType, const char* file_name, long size)
{
  ubs_msg_type_t *item = UBS_MEM_ALLOCTYPE (ubs_msg_type_t);
  ubs_list_push_back (&msgTypeList, item);
  ubs_msg_type_init_job (item, msgType, file_name, size); 
 
  return item;
}


static ubs_msg_type_t*
ubs_msg_store_find_create_MsgType (int msgType)
{
  ubs_msg_type_t* item = ubs_msg_store_findMsgType (msgType);

  if (!item) {
    char  fileName[UBS_MSG_STORE_MAX_PATH_LEN];

    
    sprintf (fileName, UBS_MSG_STORE_FOLDER_MSG_TYPE_INDEX_FILE, msgType);
    
    
    item = ubs_msg_store_newMsgType (msgType, fileName, 0);
  }
  return item;
}


static void
ubs_msg_store_visitDirItem (const char *folderName, 
                            const char *fileName, 
                            int type, long size)
{
  int msgType = -1;

  
  UNREFERENCED_PARAMETER(folderName);

  if (type == WE_FILE_FILETYPE && 
      sscanf (fileName, UBS_MSG_STORE_MSG_TYPE_INDEX_FILE, &msgType) == 1) 
  {
    char file[UBS_MSG_STORE_MAX_PATH_LEN];
    strcpy (file, folderName);
    strcat (file, UBS_MSG_STORE_PATH_SEPARATOR);
    strcat (file, fileName);
    ubs_msg_store_newMsgType (msgType, file, size);
  }
}


static void
ubs_msg_store_traverseDir (const char *folderName)
{
  int     dirSize;
  int     i;
  char    fileName[UBS_MSG_STORE_MAX_PATH_LEN];
  int     type;
  long    size;

  
  dirSize = WE_FILE_GETSIZE_DIR (folderName);
  UBS_LOG_FILE_ERROR(ubs_msg_store_traverseDir, WE_FILE_GETSIZE_DIR, dirSize);
  
  if (dirSize > 0) {
    int res;
    for (i = 0; i < dirSize; i++)
    {
      res = WE_FILE_READ_DIR (folderName, i, fileName, UBS_MSG_STORE_MAX_PATH_LEN, &type, 
            &size ); 
      if (res == WE_FILE_OK) 
      {
        ubs_msg_store_visitDirItem (folderName, fileName, type, size);
      }
      UBS_LOG_FILE_ERROR(ubs_msg_store_traverseDir, WE_FILE_READ_DIR, res);
 
    }
  }
}


static int
ubs_msg_store_initUbsFileStructure (void)
{
  int result = WE_FILE_MKDIR (UBS_MSG_STORE_FOLDER);
  UBS_LOG_FILE_IF((!(result == WE_FILE_OK) || (result == WE_FILE_ERROR_EXIST)),
    ubs_msg_store_initUbsFileStructure, WE_FILE_MKDIR,result);
  return (result == WE_FILE_OK) || (result == WE_FILE_ERROR_EXIST);
}


static void
ubs_msg_store_readIndexFiles (void)
{
  ubs_msg_store_traverseDir (UBS_MSG_STORE_FOLDER);

}


static void
ubs_msg_store_deallocMemory (void)
{
  ubs_list_iterator_t it  = ubs_list_begin (&msgTypeList);                      
  ubs_list_iterator_t end = ubs_list_end   (&msgTypeList);             
  ubs_msg_type_t     *msg;
  
  for ( ; UBS_LIST_ITERATOR_NOT_EQUAL(it,end); it = ubs_list_erase(it)) {                                                                 
     msg =  UBS_LIST_ITERATOR_GET_TYPE(it, ubs_msg_type_t);
     ubs_msg_type_release (msg);
     UBS_MEM_FREE (msg);
  }
}


static ubs_job_signal_t
ubs_msg_store_execJob_getNbrOfMsgs (ubs_job_q_head_t *jobQ, 
												            ubs_job_q_element_t *p, 
												            ubs_signal_t *sig)
{
  int                     dstModule;
  unsigned                srcTransactionId;
  ubs_get_nbr_of_msgs_t   *sigStruct = (ubs_get_nbr_of_msgs_t*)p->job;
  ubs_msg_type_t          *msgType = ubs_msg_store_findMsgType (sigStruct->msgType);
  ubs_key_value_array_t   filter;
  int                     result;

  UNREFERENCED_PARAMETER(sig);
  UNREFERENCED_PARAMETER(jobQ);

  filter.propertyArraySize = sigStruct->filterListSize;
  filter.propertyArray = sigStruct->filterList;

  result = ubs_msg_type_get_nbr_of_msgs (msgType, &filter);

  dstModule = ubs_main_deregisterTransaction (sigStruct->transactionId, &srcTransactionId);
  ubs_replyNbrOfMsgs (dstModule, srcTransactionId, sigStruct->msgType, result);

  p->state = UBS_JOB_STATE_DONE;
  return UBS_JOB_SIGNAL_NOT_USED;
}


static void 
ubs_msg_store_releaseJob_getNbrOfMsgs (void *job)
{
  WE_SIGNAL_DESTRUCT (WE_MODID_UBS, UBS_SIG_GET_NBR_OF_MSGS, job);
}


static void
ubs_msg_store_createJob_getNbrOfMsgs (ubs_get_nbr_of_msgs_t *p)
{
  ubs_msg_type_t  *msgType = ubs_msg_store_findMsgType (p->msgType);

  if (!msgType) {
    unsigned  srcTransactionId;
    int       dstModule = ubs_main_deregisterTransaction (p->transactionId, 
                                                          &srcTransactionId);

    ubs_replyNbrOfMsgs (dstModule, srcTransactionId, p->msgType, 0);

    ubs_main_free_key_value_array (&(p->filterList), p->filterListSize);
    UBS_MEM_FREE (p);
    return;
  }

  UBS_MSG_TYPE_ADD_JOB_AND_RUN (msgType, p, 
                        ubs_msg_store_execJob_getNbrOfMsgs, 
                        ubs_msg_store_releaseJob_getNbrOfMsgs,
                        UBS_JOB_STATE_WANTS_TO_RUN);
}


static ubs_job_signal_t 
ubs_msg_store_execJob_getMsgList (struct ubs_job_q_head_st *jobQ, 
												            struct ubs_job_q_element_st *p, 
												            ubs_signal_t *sig)
{
  int                     dstModule;
  unsigned                srcTransactionId;
  ubs_get_msg_list_t      *sigStruct = (ubs_get_msg_list_t*)p->job;
  ubs_msg_type_t          *msgType = ubs_msg_store_findMsgType (sigStruct->msgType);
  ubs_msg_list_item_t     *resultArray = NULL;
  int                     resultArraySize = 0;
  int                     nextMsg;
  ubs_key_value_array_t   filter;
  int                     i;

  UNREFERENCED_PARAMETER(sig);
  UNREFERENCED_PARAMETER(jobQ);

  dstModule = ubs_main_deregisterTransaction (sigStruct->transactionId, &srcTransactionId);

  filter.propertyArraySize = sigStruct->filterListSize;
  filter.propertyArray = sigStruct->filterList;

  resultArray = ubs_msg_type_get_msg_list (msgType, 
                                      &filter,
                                      dstModule,
                                      sigStruct->startFromMsg, 
                                      UBS_MAX_SIGNAL_SIZE - sizeof (ubs_reply_msg_list_t),
                                      &nextMsg,
                                      &resultArraySize);

  ubs_replyMsgList (dstModule, srcTransactionId, sigStruct->msgType, 
                    resultArraySize, resultArray, nextMsg);

  
  

  if (resultArray) {
    for (i = 0; i < resultArraySize; i++) {
      if (resultArray[i].titleList) {
        UBS_MEM_FREE (resultArray[i].titleList);
      }
    }
    UBS_MEM_FREE (resultArray);
  }

  p->state = UBS_JOB_STATE_DONE;
  return UBS_JOB_SIGNAL_NOT_USED;
}


static void 
ubs_msg_store_releaseJob_getMsgList (void *job)
{
  WE_SIGNAL_DESTRUCT (WE_MODID_UBS, UBS_SIG_GET_MSG_LIST, job);
}


static void
ubs_msg_store_createJob_getMsgList (ubs_get_msg_list_t *p)
{
  ubs_msg_type_t  *msgType = ubs_msg_store_findMsgType (p->msgType);

  if (!msgType) {
    unsigned  srcTransactionId;
    int       dstModule = ubs_main_deregisterTransaction (p->transactionId, 
                                                          &srcTransactionId);

    ubs_replyMsgList (dstModule, srcTransactionId, p->msgType, 0, NULL, 0);

    ubs_main_free_key_value_array (&(p->filterList), p->filterListSize);
    UBS_MEM_FREE (p);
    return;
  }

  UBS_MSG_TYPE_ADD_JOB_AND_RUN (msgType, p, 
                        ubs_msg_store_execJob_getMsgList, 
                        ubs_msg_store_releaseJob_getMsgList,
                        UBS_JOB_STATE_WANTS_TO_RUN);
}


static ubs_job_signal_t
ubs_msg_store_execJob_getMsg (struct ubs_job_q_head_st *jobQ, 
												      struct ubs_job_q_element_st *p, 
												      ubs_signal_t *sig)
{
  int                     dstModule;
  unsigned                srcTransactionId;
  ubs_get_msg_t           *sigStruct = (ubs_get_msg_t*)p->job;
  ubs_msg_type_t          *msgType = ubs_msg_store_findMsgType (sigStruct->msgType);
  ubs_msg_t               msg;
  int                     result;
  ubs_msg_item_t*         msgItem;

  UNREFERENCED_PARAMETER(sig);
  UNREFERENCED_PARAMETER(jobQ);

  dstModule = ubs_main_deregisterTransaction (sigStruct->transactionId, &srcTransactionId);

  msg.handle = sigStruct->handle;
  msg.propertyListSize = 0;
  msg.propertyList = NULL;

  msgItem = ubs_msg_type_get_msg_from_handle (msgType, &(sigStruct->handle));

  if (msgItem) {
    msg.propertyListSize = msgItem->propertyArray.propertyArraySize;
    msg.propertyList = msgItem->propertyArray.propertyArray;
    result = UBS_RESULT_SUCCESS;
  }
  else {
    result = UBS_RESULT_FAILURE;
  }
  
  ubs_replyMsg (dstModule, srcTransactionId, sigStruct->msgType, result, &msg);

  p->state = UBS_JOB_STATE_DONE;
  return UBS_JOB_SIGNAL_NOT_USED;
}


static void 
ubs_msg_store_releaseJob_getMsg (void *job)
{
  WE_SIGNAL_DESTRUCT (WE_MODID_UBS, UBS_SIG_GET_MSG, job);
}


static void
ubs_msg_store_createJob_getMsg (ubs_get_msg_t *p)
{
  ubs_msg_type_t  *msgType = ubs_msg_store_findMsgType (p->msgType);

  if (!msgType) {
    unsigned  srcTransactionId;
    int       dstModule = ubs_main_deregisterTransaction (p->transactionId, 
                                                          &srcTransactionId);
    ubs_msg_t msg;

    msg.handle = p->handle;
    msg.propertyListSize = 0;
    msg.propertyList = NULL;

    ubs_replyMsg (dstModule, srcTransactionId, p->msgType, UBS_RESULT_FAILURE, &msg);

    ubs_main_free_msg_handle (&(p->handle));
    UBS_MEM_FREE (p);
    return;
  }

  UBS_MSG_TYPE_ADD_JOB_AND_RUN (msgType, p, 
                        ubs_msg_store_execJob_getMsg, 
                        ubs_msg_store_releaseJob_getMsg,
                        UBS_JOB_STATE_WANTS_TO_RUN);
}


static ubs_job_signal_t 
ubs_msg_store_execJob_getFullMsg (struct ubs_job_q_head_st *jobQ, 
												          struct ubs_job_q_element_st *p, 
												          ubs_signal_t *sig)
{
  int                 dstModule;
  unsigned            srcTransactionId;
  ubs_msg_store_execJob_getFullMsg_t *job = (ubs_msg_store_execJob_getFullMsg_t*)p->job;

  UNREFERENCED_PARAMETER(sig);
  UNREFERENCED_PARAMETER(jobQ);

  if (job->start) {
    ubs_msg_type_t        *msgType = ubs_msg_store_findMsgType (job->sig->msgType);
    ubs_msg_item_t*       msgItem;

    job->result_msg.handle = job->sig->handle;

    msgItem = ubs_msg_type_get_msg_from_handle(msgType, &(job->sig->handle));

    if (!msgItem) {
      

      dstModule = ubs_main_deregisterTransaction (job->sig->transactionId, 
                                                  &srcTransactionId);
      ubs_replyFullMsg (dstModule, srcTransactionId, job->sig->msgType, 
                        UBS_RESULT_FAILURE, &(job->result_msg));

      p->state = UBS_JOB_STATE_DONE;
      return UBS_JOB_SIGNAL_NOT_USED;
    }

    if (job->sig->getPropertyList) {
      job->result_msg.propertyListSize = msgItem->propertyArray.propertyArraySize;
      job->result_msg.propertyList = msgItem->propertyArray.propertyArray;
    }

    if (!(job->sig->getHiddenPropertyList || job->sig->getData)) {
      

      dstModule = ubs_main_deregisterTransaction (job->sig->transactionId,
                                                  &srcTransactionId);
      ubs_replyFullMsg (dstModule, srcTransactionId, job->sig->msgType, UBS_RESULT_SUCCESS, 
                        &job->result_msg);

      p->state = UBS_JOB_STATE_DONE;
      return UBS_JOB_SIGNAL_NOT_USED;
    }
    else {
      

      if (job->sig->getHiddenPropertyList) {
        ubs_msg_type_get_msg_hidden_properties_job (msgType, &(job->sig->handle), 
                                         &(job->subjob_hiddenProp_result),
                                         &(job->subjob_hiddenProp_result_propArray));
      }

      if (job->sig->getData) {
        ubs_msg_type_get_msg_data_job (msgType, &(job->sig->handle), 
                                       &(job->subjob_data_result), 
                                       &(job->subjob_data_result_dataLen),
                                       &(job->subjob_data_result_data));
      }

      job->start = FALSE;
      return UBS_JOB_SIGNAL_NOT_USED;      
    }
  }
  else {
    
    int totalResult = (job->subjob_hiddenProp_result && job->subjob_data_result)? 
                        (UBS_RESULT_SUCCESS) : (UBS_RESULT_FAILURE);

    if (totalResult == UBS_RESULT_SUCCESS) {
      job->result_msg.hiddenPropertyListSize = 
                            job->subjob_hiddenProp_result_propArray.propertyArraySize;
      job->result_msg.hiddenPropertyList = 
                                job->subjob_hiddenProp_result_propArray.propertyArray;

      
      job->result_msg.dataLen = job->subjob_data_result_dataLen;
      job->result_msg.data = job->subjob_data_result_data;
    }
    else {
      
      job->result_msg.propertyListSize = 0;
      job->result_msg.propertyList = NULL;
    }

    dstModule = ubs_main_deregisterTransaction (job->sig->transactionId, &srcTransactionId);
    ubs_replyFullMsg (dstModule, srcTransactionId, job->sig->msgType, totalResult, 
                      &job->result_msg);

    p->state = UBS_JOB_STATE_DONE;
    return UBS_JOB_SIGNAL_NOT_USED;
  }
}


static void 
ubs_msg_store_releaseJob_getFullMsg (void *job)
{
  ubs_msg_store_execJob_getFullMsg_t  *theJob = (ubs_msg_store_execJob_getFullMsg_t*)job;

  WE_SIGNAL_DESTRUCT (WE_MODID_UBS, UBS_SIG_GET_FULL_MSG, theJob->sig);

  ubs_key_value_array_release (&(theJob->subjob_hiddenProp_result_propArray));
  UBS_MEM_FREE (theJob->subjob_data_result_data);

  UBS_MEM_FREE (job);
}


static void
ubs_msg_store_createJob_getFullMsg (ubs_get_full_msg_t *p)
{
  ubs_msg_type_t  *msgType = ubs_msg_store_findMsgType (p->msgType);
  ubs_msg_store_execJob_getFullMsg_t  *job;

  if (!msgType) {
    unsigned  srcTransactionId;
    int       dstModule = ubs_main_deregisterTransaction (p->transactionId, 
                                                          &srcTransactionId);
    ubs_full_msg_t msg;

    msg.handle = p->handle;
    msg.propertyListSize = 0;
    msg.propertyList = NULL;
    msg.hiddenPropertyListSize = 0;
    msg.hiddenPropertyList = NULL;
    msg.dataLen = 0;
    msg.data = NULL;

    ubs_replyFullMsg (dstModule, srcTransactionId, p->msgType, UBS_RESULT_FAILURE, &msg);

    ubs_main_free_msg_handle (&(p->handle));
    UBS_MEM_FREE (p);
    return;
  }

  job = UBS_MEM_ALLOCTYPE (ubs_msg_store_execJob_getFullMsg_t);

  job->start = TRUE;
  job->sig = p;
  job->result_msg.handle.handleLen = 0;
  job->result_msg.handle.handle = NULL;
  job->result_msg.propertyListSize = 0;
  job->result_msg.propertyList = NULL;
  job->result_msg.hiddenPropertyListSize = 0;
  job->result_msg.hiddenPropertyList = NULL;
  job->result_msg.dataLen = 0;
  job->result_msg.data = NULL;
  job->subjob_hiddenProp_result = 0;
  job->subjob_hiddenProp_result_propArray.propertyArraySize = 0;
  job->subjob_hiddenProp_result_propArray.propertyArray = NULL;
  job->subjob_data_result = 0;
  job->subjob_data_result_dataLen = 0;
  job->subjob_data_result_data = NULL;

  UBS_MSG_TYPE_ADD_JOB_AND_RUN (msgType, job, 
                        ubs_msg_store_execJob_getFullMsg,
                        ubs_msg_store_releaseJob_getFullMsg,
                        UBS_JOB_STATE_WANTS_TO_RUN);
}


static ubs_job_signal_t
ubs_msg_store_execJob_deleteMsg (struct ubs_job_q_head_st *jobQ, 
												         struct ubs_job_q_element_st *p, 
												         ubs_signal_t *sig)
{
  int       dstModule;
  unsigned  srcTransactionId;
  ubs_msg_store_execJob_deleteMsg_t *job = (ubs_msg_store_execJob_deleteMsg_t*)p->job;

  UNREFERENCED_PARAMETER(sig);
  UNREFERENCED_PARAMETER(jobQ);

  if (job->start) {
    ubs_msg_type_t        *msgType = ubs_msg_store_findMsgType (job->sig->msgType);
    ubs_msg_item_t*       msgItem;

    msgItem = ubs_msg_type_get_msg_from_handle(msgType, &(job->sig->handle));

    if (!msgItem) {
      

      dstModule = ubs_main_deregisterTransaction (job->sig->transactionId, 
                                                  &srcTransactionId);
      ubs_replyDeleteMsg (dstModule, srcTransactionId, job->sig->msgType, 
                          &(job->sig->handle), UBS_RESULT_FAILURE);

      p->state = UBS_JOB_STATE_DONE;
      return UBS_JOB_SIGNAL_NOT_USED;
    }

    ubs_msg_type_delete_msg_from_handle (msgType, &(job->sig->handle));
    ubs_msg_type_write_del_msg_job (msgType, &(job->sig->handle), &(job->subjob_result));

    job->start = FALSE;
    return UBS_JOB_SIGNAL_NOT_USED;
  }
  else {
    

    dstModule = ubs_main_deregisterTransaction (job->sig->transactionId, &srcTransactionId);
    ubs_replyDeleteMsg (dstModule, srcTransactionId, job->sig->msgType, 
                        &(job->sig->handle), job->subjob_result);

    if (job->subjob_result > 0) {
      
      ubs_notify_msg_deleted_t  notify;

      notify.handle = job->sig->handle;
      notify.msgType = job->sig->msgType;

      ubs_handle_msg_main_notifyMsgDeleted (dstModule, &notify);
    }

    p->state = UBS_JOB_STATE_DONE;
    return UBS_JOB_SIGNAL_NOT_USED;
  }
}


static void 
ubs_msg_store_releaseJob_deleteMsg (void *job)
{
  ubs_msg_store_execJob_deleteMsg_t  *theJob = (ubs_msg_store_execJob_deleteMsg_t*)job;

  WE_SIGNAL_DESTRUCT (WE_MODID_UBS, UBS_SIG_DELETE_MSG, theJob->sig);
  UBS_MEM_FREE (job);
}


static void
ubs_msg_store_createJob_deleteMsg (ubs_delete_msg_t *p)
{
  ubs_msg_type_t  *msgType = ubs_msg_store_findMsgType (p->msgType);
  ubs_msg_store_execJob_deleteMsg_t   *job;

  if (!msgType) {
    unsigned  srcTransactionId;
    int       dstModule = ubs_main_deregisterTransaction (p->transactionId, 
                                                          &srcTransactionId);

    ubs_replyDeleteMsg (dstModule, srcTransactionId, p->msgType, &(p->handle), 
                        UBS_RESULT_FAILURE);

    ubs_main_free_msg_handle (&(p->handle));
    UBS_MEM_FREE (p);
    return;
  }

  job = UBS_MEM_ALLOCTYPE (ubs_msg_store_execJob_deleteMsg_t);

  job->start = TRUE;
  job->sig = p;
  job->subjob_result = -1;

  UBS_MSG_TYPE_ADD_JOB_AND_RUN (msgType, job, 
                        ubs_msg_store_execJob_deleteMsg, 
                        ubs_msg_store_releaseJob_deleteMsg,
                        UBS_JOB_STATE_WANTS_TO_RUN);
}


static ubs_job_signal_t 
ubs_msg_store_execJob_changeMsg (struct ubs_job_q_head_st *jobQ, 
												         struct ubs_job_q_element_st *p, 
												         ubs_signal_t *sig)
{
  int               dstModule;
  unsigned          srcTransactionId;
  ubs_msg_store_execJob_changeMsg_t *job = (ubs_msg_store_execJob_changeMsg_t*)p->job;
  ubs_msg_type_t    *msgType = ubs_msg_store_findMsgType (job->sig->msgType);

  UNREFERENCED_PARAMETER(sig);
  UNREFERENCED_PARAMETER(jobQ);

  if (job->start) {
    ubs_msg_item_t*       msgItem;
    ubs_key_value_array_t *propArray = NULL;
    ubs_key_value_array_t *hiddenPropArray = NULL;

    msgItem = ubs_msg_type_get_msg_from_handle(msgType, &(job->sig->msg.handle));

    if (!msgItem) {
      

      dstModule = ubs_main_deregisterTransaction (job->sig->transactionId, 
                                                  &srcTransactionId);
      ubs_replyChangeMsg (dstModule, srcTransactionId, job->sig->msgType, 
                          &(job->sig->msg.handle), UBS_RESULT_FAILURE);

      p->state = UBS_JOB_STATE_DONE;

      return UBS_JOB_SIGNAL_NOT_USED;
    }

    if (job->sig->changePropertyList) {
      job->propArray.propertyArraySize = job->sig->msg.propertyListSize;
      job->propArray.propertyArray = job->sig->msg.propertyList;
      propArray = &job->propArray;
    }

    if (job->sig->changeHiddenPropertyList) {
      job->hiddenPropArray.propertyArraySize = job->sig->msg.hiddenPropertyListSize;
      job->hiddenPropArray.propertyArray = job->sig->msg.hiddenPropertyList;
      hiddenPropArray = &job->hiddenPropArray;
    }

    ubs_msg_type_change_msg_job (msgType, &(job->sig->msg.handle),
                                 propArray, hiddenPropArray,
                                 job->sig->changeData,
                                 job->sig->msg.dataLen, job->sig->msg.data,
                                 &(job->subjob_result));

    job->start = FALSE;
    return UBS_JOB_SIGNAL_NOT_USED;
  }
  else {
    

    dstModule = ubs_main_deregisterTransaction (job->sig->transactionId, &srcTransactionId);
    ubs_replyChangeMsg (dstModule, srcTransactionId, job->sig->msgType, 
                        &(job->sig->msg.handle), job->subjob_result);

    if (job->subjob_result) {
      
      ubs_notify_msg_changed_t  notify;
      ubs_msg_item_t*           msgItem;

      msgItem = ubs_msg_type_get_msg_from_handle(msgType, &(job->sig->msg.handle));

      if (msgItem) {
        notify.msg.handle = job->sig->msg.handle;
        notify.msg.titleListSize = msgItem->propertyArray.propertyArraySize;
        notify.msg.titleList = msgItem->propertyArray.propertyArray;
        notify.msgType = job->sig->msgType;

        ubs_handle_msg_main_notifyMsgChanged (dstModule, &notify);
      }
      else {
        
        
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS, 
          "UBS: msgItem not found/removed [ubs_msg_store_execJob_changeMsg]\n"));
      }
    }

    p->state = UBS_JOB_STATE_DONE;
    return UBS_JOB_SIGNAL_NOT_USED;
  }
}


static void 
ubs_msg_store_releaseJob_changeMsg (void *job)
{
  ubs_msg_store_execJob_changeMsg_t  *theJob = (ubs_msg_store_execJob_changeMsg_t*)job;

  WE_SIGNAL_DESTRUCT (WE_MODID_UBS, UBS_SIG_CHANGE_MSG, theJob->sig);
  UBS_MEM_FREE (theJob);
}


void
ubs_msg_store_createJob_changeMsg (ubs_change_msg_t *p)
{
  ubs_msg_type_t  *msgType = ubs_msg_store_findMsgType (p->msgType);
  ubs_msg_store_execJob_changeMsg_t   *job;

  if (!msgType) {
    unsigned  srcTransactionId;
    int       dstModule = ubs_main_deregisterTransaction (p->transactionId, 
                                                          &srcTransactionId);

    ubs_replyChangeMsg (dstModule, srcTransactionId, p->msgType, 
                        &(p->msg.handle), UBS_RESULT_FAILURE);

    ubs_main_free_full_msg (&(p->msg));
    UBS_MEM_FREE (p);
    return;
  }

  job = UBS_MEM_ALLOCTYPE (ubs_msg_store_execJob_changeMsg_t);

  job->start = TRUE;
  job->sig = p;
  ubs_key_value_array_init (&job->propArray);
  ubs_key_value_array_init (&job->hiddenPropArray);
  job->subjob_result = -1;

  UBS_MSG_TYPE_ADD_JOB_AND_RUN (msgType, job, 
                        ubs_msg_store_execJob_changeMsg,
                        ubs_msg_store_releaseJob_changeMsg,
                        UBS_JOB_STATE_WANTS_TO_RUN);
}


static ubs_job_signal_t 
ubs_msg_store_execJob_setMsgProperty (struct ubs_job_q_head_st *jobQ, 
												         struct ubs_job_q_element_st *p, 
												         ubs_signal_t *sig)
{
  int               dstModule;
  unsigned          srcTransactionId;
  ubs_msg_store_execJob_setMsgProperty_t *job = 
                                           (ubs_msg_store_execJob_setMsgProperty_t*)p->job;
  ubs_msg_type_t    *msgType = ubs_msg_store_findMsgType (job->sig->msgType);

  UNREFERENCED_PARAMETER(sig);
  UNREFERENCED_PARAMETER(jobQ);

  if (job->start) {
    ubs_msg_item_t*       msgItem;
    ubs_key_value_array_t *propArray = NULL;

    msgItem = ubs_msg_type_get_msg_from_handle(msgType, &(job->sig->handle));

    if (!msgItem) {
      

      dstModule = ubs_main_deregisterTransaction (job->sig->transactionId, 
                                                  &srcTransactionId);
      ubs_replySetMsgProperty (dstModule, srcTransactionId, job->sig->msgType, 
                          &(job->sig->handle), UBS_RESULT_FAILURE);

      p->state = UBS_JOB_STATE_DONE;

      return UBS_JOB_SIGNAL_NOT_USED;
    }

    job->propArray.propertyArraySize = 1;
    job->propArray.propertyArray = &(job->sig->propertyValue);
    propArray = &job->propArray;

    ubs_msg_type_change_msg_job (msgType, &(job->sig->handle), propArray, 
                                 NULL, FALSE, 0, NULL, &(job->subjob_result));

    job->start = FALSE;
    return UBS_JOB_SIGNAL_NOT_USED;
  }
  else {
    

    dstModule = ubs_main_deregisterTransaction (job->sig->transactionId, &srcTransactionId);
    ubs_replySetMsgProperty (dstModule, srcTransactionId, job->sig->msgType, 
                        &(job->sig->handle), job->subjob_result);

    if (job->subjob_result) {
      
      ubs_notify_msg_changed_t  notify;
      ubs_msg_item_t*           msgItem;

      msgItem = ubs_msg_type_get_msg_from_handle(msgType, &(job->sig->handle));

      if (msgItem) {
        notify.msg.handle = job->sig->handle;
        notify.msg.titleListSize = msgItem->propertyArray.propertyArraySize;
        notify.msg.titleList = msgItem->propertyArray.propertyArray;
        notify.msgType = job->sig->msgType;

        ubs_handle_msg_main_notifyMsgChanged (dstModule, &notify);
      }
      else {
        
        
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS, 
          "UBS: msgItem not found/removed [ubs_msg_store_execJob_setMsgProperty]\n"));
      }
    }

    p->state = UBS_JOB_STATE_DONE;
    return UBS_JOB_SIGNAL_NOT_USED;
  }
}


static void 
ubs_msg_store_releaseJob_setMsgProperty (void *job)
{
  ubs_msg_store_execJob_setMsgProperty_t  *theJob = 
                                              (ubs_msg_store_execJob_setMsgProperty_t*)job;

  WE_SIGNAL_DESTRUCT (WE_MODID_UBS, UBS_SIG_SET_MSG_PROPERTY, theJob->sig);
  UBS_MEM_FREE (theJob);
}


void
ubs_msg_store_createJob_setMsgProperty (ubs_set_msg_property_t *p)
{
  ubs_msg_type_t  *msgType = ubs_msg_store_findMsgType (p->msgType);
  ubs_msg_store_execJob_setMsgProperty_t   *job;

  if (!msgType) {
    unsigned  srcTransactionId;
    int       dstModule = ubs_main_deregisterTransaction (p->transactionId, 
                                                          &srcTransactionId);

    ubs_replySetMsgProperty (dstModule, srcTransactionId, p->msgType, 
                        &(p->handle), UBS_RESULT_FAILURE);

    ubs_main_free_msg_handle (&(p->handle));
    ubs_main_free_key_value (&(p->propertyValue));
    UBS_MEM_FREE (p);
    return;
  }

  job = UBS_MEM_ALLOCTYPE (ubs_msg_store_execJob_setMsgProperty_t);

  job->start = TRUE;
  job->sig = p;
  ubs_key_value_array_init (&job->propArray);
  job->subjob_result = -1;

  UBS_MSG_TYPE_ADD_JOB_AND_RUN (msgType, job, 
                        ubs_msg_store_execJob_setMsgProperty,
                        ubs_msg_store_releaseJob_setMsgProperty,
                        UBS_JOB_STATE_WANTS_TO_RUN);
}


static ubs_job_signal_t 
ubs_msg_store_execJob_createMsg (struct ubs_job_q_head_st *jobQ, 
												          struct ubs_job_q_element_st *p, 
												          ubs_signal_t *sig)
{
  int                 dstModule;
  unsigned            srcTransactionId;
  ubs_msg_store_execJob_createMsg_t *job = (ubs_msg_store_execJob_createMsg_t*)p->job;
  ubs_msg_type_t      *msgType = ubs_msg_store_findMsgType (job->sig->msgType);

  UNREFERENCED_PARAMETER(sig);
  UNREFERENCED_PARAMETER(jobQ);

  if (job->start) {
    ubs_msg_item_t*       msgItem;

    msgItem = ubs_msg_type_get_msg_from_handle(msgType, &(job->sig->msg.handle));

    if (msgItem) {
      

      dstModule = ubs_main_deregisterTransaction (job->sig->transactionId, 
                                                  &srcTransactionId);
      ubs_replyCreateMsg (dstModule, srcTransactionId, job->sig->msgType, 
                          &(job->sig->msg.handle), UBS_RESULT_FAILURE);

      p->state = UBS_JOB_STATE_DONE;
      return UBS_JOB_SIGNAL_NOT_USED;
    }

    job->propArray.propertyArraySize = job->sig->msg.propertyListSize;
    job->propArray.propertyArray = job->sig->msg.propertyList;

    job->hiddenPropArray.propertyArraySize = job->sig->msg.hiddenPropertyListSize;
    job->hiddenPropArray.propertyArray = job->sig->msg.hiddenPropertyList;

    ubs_msg_type_create_msg_job (msgType, &(job->sig->msg.handle),
                                 &job->propArray, &job->hiddenPropArray,     
                                 job->sig->msg.dataLen, job->sig->msg.data, 
                                 &(job->subjob_result));

    job->start = FALSE;
    return UBS_JOB_SIGNAL_NOT_USED;
  }
  else {
    

    dstModule = ubs_main_deregisterTransaction (job->sig->transactionId, &srcTransactionId);

    job->sig->msg.propertyListSize = job->propArray.propertyArraySize;
    job->sig->msg.propertyList = job->propArray.propertyArray;

    job->sig->msg.hiddenPropertyListSize = job->hiddenPropArray.propertyArraySize;
    job->sig->msg.hiddenPropertyList = job->hiddenPropArray.propertyArray;

    ubs_replyCreateMsg (dstModule, srcTransactionId, job->sig->msgType, 
                        &(job->sig->msg.handle), job->subjob_result);

    if (job->subjob_result) {
      
      ubs_notify_new_msg_t    notify;
      ubs_msg_item_t*         msgItem;

      msgItem = ubs_msg_type_get_msg_from_handle(msgType, &(job->sig->msg.handle));

      if (msgItem) {
        notify.msg.handle = job->sig->msg.handle;
        notify.msg.titleListSize = msgItem->propertyArray.propertyArraySize;
        notify.msg.titleList = msgItem->propertyArray.propertyArray;
        notify.msgType = job->sig->msgType;

        ubs_handle_msg_main_notifyNewMsg (dstModule, &notify);
      }
      else {
        
       WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS, 
         "UBS: msgItem not found/removed [ubs_msg_store_execJob_createMsg]\n"));
      }
    }

    p->state = UBS_JOB_STATE_DONE;
    return UBS_JOB_SIGNAL_NOT_USED;
  }
}


static void 
ubs_msg_store_releaseJob_createMsg (void *job)
{
  ubs_msg_store_execJob_createMsg_t  *theJob = (ubs_msg_store_execJob_createMsg_t*)job;

  WE_SIGNAL_DESTRUCT (WE_MODID_UBS, UBS_SIG_CREATE_MSG, theJob->sig);
  UBS_MEM_FREE (theJob);
}


static void
ubs_msg_store_createJob_createMsg (ubs_create_msg_t *p)
{
  ubs_msg_type_t         *msgType = ubs_msg_store_find_create_MsgType (p->msgType);
  ubs_msg_store_execJob_createMsg_t *job;

  if (!msgType) {
    unsigned  srcTransactionId;
    int       dstModule = ubs_main_deregisterTransaction (p->transactionId, 
                                                          &srcTransactionId);

    ubs_replyCreateMsg (dstModule, srcTransactionId, p->msgType, 
                        &(p->msg.handle), UBS_RESULT_FAILURE);

    ubs_main_free_full_msg (&(p->msg));
    UBS_MEM_FREE (p);
    return;
  }

  job = UBS_MEM_ALLOCTYPE (ubs_msg_store_execJob_createMsg_t);

  job->sig = p;
  job->start = TRUE;
  ubs_key_value_array_init (&job->propArray);
  ubs_key_value_array_init (&job->hiddenPropArray);
  job->subjob_result = -1;

  UBS_MSG_TYPE_ADD_JOB_AND_RUN (msgType, job, 
                        ubs_msg_store_execJob_createMsg, 
                        ubs_msg_store_releaseJob_createMsg,
                        UBS_JOB_STATE_WANTS_TO_RUN);
}









static void
ubs_msg_store_main (ubs_signal_t *sig)
{
  
  ubs_list_iterator_t it = ubs_list_begin(&msgTypeList);
  ubs_list_iterator_t end = ubs_list_end(&msgTypeList);
  int                 isHandled = FALSE;
  ubs_msg_type_t      *msg_type;

  while((msg_store_state != ubs_msg_store_state_stopped) && 
        !isHandled && UBS_LIST_ITERATOR_NOT_EQUAL(it,end))
  {
    msg_type = UBS_LIST_ITERATOR_GET_TYPE (it, ubs_msg_type_t);
    isHandled = UBS_MSG_TYPE_PROCESS_SIGNAL(msg_type, sig) == UBS_JOB_SIGNAL_USED;
    UBS_LIST_ITERATOR_NEXT(it);
  }

  if (isHandled) {

    if (msg_store_state == ubs_msg_store_state_stopping) {
      ubs_msg_store_clear_inactive_jobs ();
    }
    ubs_signal_delete (sig);
    return;
  }


  if (msg_store_state != ubs_msg_store_state_running) {
    

    switch (sig->type) {
  
    case UBS_MSG_STORE_SIG_GET_NBR_OF_MSGS:
      WE_SIGNAL_DESTRUCT (WE_MODID_UBS, UBS_SIG_GET_NBR_OF_MSGS, sig->p_param);
      break;

    case UBS_MSG_STORE_SIG_GET_MSG_LIST:
      WE_SIGNAL_DESTRUCT (WE_MODID_UBS, UBS_SIG_GET_MSG_LIST, sig->p_param);
      break;

    case UBS_MSG_STORE_SIG_GET_MSG:
      WE_SIGNAL_DESTRUCT (WE_MODID_UBS, UBS_SIG_GET_MSG, sig->p_param);
      break;

    case UBS_MSG_STORE_SIG_GET_FULL_MSG:
      WE_SIGNAL_DESTRUCT (WE_MODID_UBS, UBS_SIG_GET_FULL_MSG, sig->p_param);
      break;

    case UBS_MSG_STORE_SIG_DELETE_MSG:
      WE_SIGNAL_DESTRUCT (WE_MODID_UBS, UBS_SIG_DELETE_MSG, sig->p_param);
      break;

    case UBS_MSG_STORE_SIG_CHANGE_MSG:
      WE_SIGNAL_DESTRUCT (WE_MODID_UBS, UBS_SIG_CHANGE_MSG, sig->p_param);
      break;

    case UBS_MSG_STORE_SIG_CREATE_MSG:
      WE_SIGNAL_DESTRUCT (WE_MODID_UBS, UBS_SIG_CREATE_MSG, sig->p_param);
      break;

    case UBS_MSG_STORE_SIG_FILE_NOTIFY:
      WE_SIGNAL_DESTRUCT (WE_MODID_UBS, WE_SIG_FILE_NOTIFY, sig->p_param);
      break;
    }
    ubs_signal_delete (sig);
    return;
  }

  switch (sig->type) {
  
  case UBS_MSG_STORE_SIG_GET_NBR_OF_MSGS:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MSG_STORE_SIG_GET_NBR_OF_MSGS\n"));
    ubs_msg_store_createJob_getNbrOfMsgs ((ubs_get_nbr_of_msgs_t*)sig->p_param);
    break;

  case UBS_MSG_STORE_SIG_GET_MSG_LIST:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MSG_STORE_SIG_GET_MSG_LIST\n"));
    ubs_msg_store_createJob_getMsgList ((ubs_get_msg_list_t*)sig->p_param);
    break;

  case UBS_MSG_STORE_SIG_GET_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MSG_STORE_SIG_GET_MSG\n"));
    ubs_msg_store_createJob_getMsg ((ubs_get_msg_t*)sig->p_param);
    break;

  case UBS_MSG_STORE_SIG_GET_FULL_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MSG_STORE_SIG_GET_FULL_MSG\n"));
    ubs_msg_store_createJob_getFullMsg ((ubs_get_full_msg_t*)sig->p_param);
    break;

  case UBS_MSG_STORE_SIG_DELETE_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MSG_STORE_SIG_DELETE_MSG\n"));
    ubs_msg_store_createJob_deleteMsg ((ubs_delete_msg_t*)sig->p_param);
    break;

  case UBS_MSG_STORE_SIG_CHANGE_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MSG_STORE_SIG_CHANGE_MSG\n"));
    ubs_msg_store_createJob_changeMsg ((ubs_change_msg_t*)sig->p_param);
    break;

  case UBS_MSG_STORE_SIG_CREATE_MSG:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MSG_STORE_SIG_CREATE_MSG\n"));
    ubs_msg_store_createJob_createMsg ((ubs_create_msg_t*)sig->p_param);
    break;
    
  case UBS_MSG_STORE_SIG_SET_MSG_PROPERTY:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: received signal UBS_MSG_STORE_SIG_SET_MSG_PROPERTY\n"));
    ubs_msg_store_createJob_setMsgProperty ((ubs_set_msg_property_t*)sig->p_param);
    break;

  case UBS_MSG_STORE_SIG_FILE_NOTIFY:
      WE_SIGNAL_DESTRUCT (WE_MODID_UBS, WE_SIG_FILE_NOTIFY, sig->p_param);
      break;
  default:
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS, 
      "UBS_MODULE_MSG_STORE: received unexpected signal: %d\n", sig->type));
    break;

  }

  ubs_signal_delete (sig);
}

