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









#include "Ubs_Mtpy.h"
#include "Ubs_Msg.h"
#include "Ubs_Main.h"
#include "Ubs_Msin.h"
#include "Ubs_Ndx.h"
#include "We_Dcvt.h"
#include "We_File.h"
#include "We_Cmmn.h"
#include "We_Log.h"
#include "Ubs_A.h"




extern int total_nbr_of_push_si_and_sl;






#define UBS_MSG_TYPE_MAX_FRAGMENTATION 200





typedef struct{
  ubs_msg_type_t  *mtype;
  void            *data;
  long             result;
} ubs_msg_type_rw_file_t;

typedef struct {
  int                   start;
  ubs_msg_type_t*       msg_type;
  ubs_msg_item_t*       msg;
  long                  dataLen;
  unsigned char         *data;
  int                   *result;
  ubs_key_value_array_t *result_array;
  long                  subjob_result;
} ubs_msg_type_get_msg_hidden_properties_job_t;

typedef struct {
  int                   start;
  ubs_msg_type_t*       msg_type;
  ubs_msg_item_t*       msg;
  int                   *result;
  long                  *result_data_len;
  unsigned char         **result_data;
  long                  subjob_result;
} ubs_msg_type_get_msg_data_job_t;

typedef struct {
  ubs_msg_type_t*       msg_type;
  int                   *result;
  ubs_msg_item_t*       msg;
  ubs_key_value_array_t *propArray;
  ubs_key_value_array_t *hiddenPropArray;
  int                   changeData;
  long                  dataLen;
  unsigned char         *data;
  long                  hidden_prop_data_len;
  long                  subjob_result;
} ubs_msg_type_change_msg_job_t;

typedef struct {
  ubs_msg_type_t*       msg_type;
  int                   *result;
  ubs_msg_item_t*       msg;
  long                  dataLen;
  unsigned char         *data;
  ubs_key_value_array_t *hiddenPropArray;
  long                  hidden_prop_data_len;
  long                  subjob_result;
} ubs_msg_type_create_msg_job_t;







static int
ubs_msg_type_read_items(ubs_msg_type_t* mtype,
                        void *data,
                        long datasize);

static ubs_job_signal_t 
ubs_msg_type_read_index_file (ubs_job_q_head_t *jobQ, 
                              ubs_job_q_element_t *p, 
                              ubs_signal_t *sig);

static void
ubs_msg_type_rw_index_file_release(ubs_msg_type_rw_file_t* data );

static void
ubs_msg_type_createJob_defrag_index_file(ubs_msg_type_t* mtyp);

static ubs_job_signal_t
ubs_msg_type_defrag_index_file(ubs_job_q_head_t *jobQ, 
                                ubs_job_q_element_t *p, 
                                ubs_signal_t *sig);


static ubs_msg_item_t*
ubs_msg_type_get_msg (ubs_msg_type_t* p, ubs_msg_item_t *msg);

static ubs_msg_item_t*
ubs_msg_type_change_msg (ubs_msg_type_t *mtype,
                         ubs_msg_item_t *msg);

static void
ubs_msg_type_releaseJob_createMsg (ubs_msg_type_create_msg_job_t  *job);


static ubs_job_signal_t
ubs_msg_type_execJob_getHiddenProp (struct ubs_job_q_head_st *jobQ, 
                                    ubs_job_q_element_t      *p, 
                                    ubs_signal_t             *sig);

static ubs_job_signal_t
ubs_msg_type_execJob_getData (struct ubs_job_q_head_st *jobQ, 
                              ubs_job_q_element_t      *p, 
                              ubs_signal_t             *sig);

static void
ubs_msg_type_releaseJob_getData (ubs_msg_type_get_msg_data_job_t  *job);

static ubs_job_signal_t
ubs_msg_type_exec_job_change_msg_data (struct ubs_job_q_head_st *jobQ, 
                                       ubs_job_q_element_t *p, 
                                       ubs_signal_t *sig);

static ubs_job_signal_t
ubs_msg_type_exec_job_change_msg_end (struct ubs_job_q_head_st *jobQ, 
                                      ubs_job_q_element_t      *p, 
                                      ubs_signal_t             *sig);

static ubs_job_signal_t
ubs_msg_type_exec_job_change_msg_ndx (struct ubs_job_q_head_st *jobQ, 
                                      ubs_job_q_element_t      *p, 
                                      ubs_signal_t             *sig);

static ubs_job_signal_t
ubs_msg_type_exec_job_change_msg_hidden_prop (struct ubs_job_q_head_st *jobQ, 
                                              ubs_job_q_element_t      *p, 
                                              ubs_signal_t             *sig);

static void
ubs_msg_type_release_job_change_msg (ubs_msg_type_create_msg_job_t *job);

static ubs_job_signal_t
ubs_msg_type_execJob_createMsg_end (struct ubs_job_q_head_st *jobQ, 
                                    ubs_job_q_element_t      *p, 
                                    ubs_signal_t             *sig);

static ubs_job_signal_t
ubs_msg_type_execJob_createMsg_ndx (struct ubs_job_q_head_st *jobQ, 
                                    ubs_job_q_element_t      *p, 
                                    ubs_signal_t             *sig);
static ubs_job_signal_t
ubs_msg_type_execJob_createMsg_data (struct ubs_job_q_head_st *jobQ, 
                                     ubs_job_q_element_t      *p, 
                                     ubs_signal_t             *sig);
static ubs_job_signal_t
ubs_msg_type_execJob_createMsg_hidden_prop (struct ubs_job_q_head_st *jobQ, 
                                            ubs_job_q_element_t      *p, 
                                            ubs_signal_t             *sig);


static void
uba_msg_type_notify_un_touched_count_changed(ubs_msg_type_t* mtype);

static int
ubs_msg_type_add_msg(ubs_msg_type_t* mtype, ubs_msg_item_t* msg, int called_from_ndx_file_reading);







void  
ubs_msg_type_init_job (ubs_msg_type_t* mtype,                      
                            int msg_type,
                            const char* file_name,
                            long file_size)

{
  mtype->msg_type = msg_type;
  mtype->file_size = file_size;
  mtype->file_fragmentation_begins = file_size;
  mtype->next_free_file_id = 1;
  mtype->nbr_of_changes = 0;
  mtype->nbr_of_deletions = 0;
  mtype->un_touched_count = 0;
  mtype->state = UBS_MSG_TYPE_STARTING;
  
  ubs_jobQ_init (&mtype->job_queue);
  ubs_hash_init (&mtype->msg_hash, 
                 WE_MODID_UBS, 
                 15, 
                 (ubs_hash_func_t*)ubs_msg_hash,
                 (ubs_hash_func_compare_t*) ubs_msg_compare); 


  mtype->file_handle = WE_FILE_OPEN (WE_MODID_UBS, 
                                      file_name, 
                                      WE_FILE_SET_RDWR | WE_FILE_SET_CREATE, 
                                      0);

 
  UBS_LOG_FILE_ERROR(ubs_msg_type_init_job, WE_FILE_OPEN, mtype->file_handle);

  

  
  
  if (file_size > 0) {
    
    ubs_msg_type_rw_file_t* p = UBS_MEM_ALLOCTYPE (ubs_msg_type_rw_file_t);
    p->data  = NULL;
    p->mtype = mtype;

	mtype->ndx_file_exist = TRUE;  

    UBS_MSG_TYPE_ADD_JOB_AND_RUN (mtype,
                  p,
                  ubs_msg_type_read_index_file,
                  (ubs_release_job_func_t*) ubs_msg_type_rw_index_file_release,
                  UBS_JOB_STATE_WANTS_TO_RUN);
  }
  else{
	  mtype->ndx_file_exist = FALSE;  

  }
  
}

void
ubs_msg_type_release(ubs_msg_type_t* p)
{
  
  ubs_hash_iterator_t it  = ubs_hash_begin (&p->msg_hash);                      
  ubs_hash_iterator_t end = ubs_hash_end   (&p->msg_hash);             
  ubs_msg_item_t     *msg;
  
  for ( ; UBS_HASH_ITERATOR_NOT_EQUAL(it,end); it = ubs_hash_erase(it)) {                                                                 
     msg =  UBS_HASH_ITERATOR_GET_TYPE(it, ubs_msg_item_t);
     
     ubs_msg_release (msg);
     
     UBS_MEM_FREE (msg);
  }

  ubs_hash_release (&p->msg_hash);
  ubs_jobQ_release (&p->job_queue);
  if (p->file_handle >=0 )
  {
    int res = WE_FILE_CLOSE (p->file_handle);
    UBS_LOG_FILE_ERROR(ubs_msg_type_release, WE_FILE_CLOSE, res);

  }
}


int
ubs_msg_ready_to_terminate (ubs_msg_type_t* p)
{
  return !ubs_jobQ_has_running_jobs (&p->job_queue);
}


int
ubs_msg_type_delete_msg_from_handle (ubs_msg_type_t   *p, 
                                     ubs_msg_handle_t *handle)
{
  ubs_msg_item_t item;
  ubs_msg_init(&item);
  item.handle = *handle;
  item.handle_hash = 0;
  
  return ubs_msg_type_delete_msg(p, &item);
}

int 
ubs_msg_type_delete_msg(ubs_msg_type_t* p, ubs_msg_item_t* msg)
{
  ubs_hash_iterator_t it = ubs_hash_find (&p->msg_hash,msg);
  ubs_hash_iterator_t end;
  UBS_HASH_END(&(p->msg_hash), end);
  
  if(UBS_HASH_ITERATOR_EQUAL (it,end))
  {
    return FALSE;
  }
  total_nbr_of_push_si_and_sl--;
  p->total_nbr_of_msgs--;
  p->nbr_of_deletions++;
  msg = UBS_HASH_ITERATOR_GET (it);

  if (!UBS_MSG_IS_TOUCHED(msg)){
    --p->un_touched_count;
    uba_msg_type_notify_un_touched_count_changed (p);
  }
  
  ubs_hash_erase(it);
  ubs_msg_release(msg);
  UBS_MEM_FREE(msg);
  
  return TRUE;
}

ubs_msg_item_t*
ubs_msg_type_get_msg_from_handle(ubs_msg_type_t* p, ubs_msg_handle_t *handle)
{
  ubs_msg_item_t msg;
  memset(&msg,0, sizeof(ubs_msg_item_t));
  msg.handle.handle= handle->handle;
  msg.handle.handleLen = handle->handleLen;
  
  return ubs_msg_type_get_msg(p, &msg);

}



int
ubs_msg_type_get_nbr_of_msgs (ubs_msg_type_t* p, ubs_key_value_array_t *filter)
{
  if (!filter->propertyArraySize) {
    return UBS_HASH_SIZE (&(p->msg_hash));
  }

  {
    ubs_hash_iterator_t     it  = ubs_hash_begin (&p->msg_hash);                      
    ubs_hash_iterator_t     end = ubs_hash_end   (&p->msg_hash);             
    ubs_msg_item_t          *msg;
    int                     result = 0;

    for ( ; UBS_HASH_ITERATOR_NOT_EQUAL (it,end); UBS_HASH_ITERATOR_NEXT (it)) {

      msg =  UBS_HASH_ITERATOR_GET_TYPE (it, ubs_msg_item_t);

      if (ubs_key_value_array_is_subset ((UBS_MSG_GET_PROPERTY_ARRAY (msg)), filter)) {
        result++;
      }
    }

    return result;
  }
}

void
ubs_msg_type_write_del_msg_job (ubs_msg_type_t* p, ubs_msg_handle_t *handle, long *result)
{
  ubs_ndx_item_t item;
  UBS_NDX_INIT_DELETE(&item, handle);
  ubs_ndx_write_item_job (&p->job_queue, p->msg_type, p->file_handle, &item, result);
}






ubs_msg_list_item_t*
ubs_msg_type_get_msg_list (ubs_msg_type_t* p, 
                           ubs_key_value_array_t *filter,
                           int srcModule,
                           int startFromMsg,
                           long maxResultMemSize,
                           int *result_nextMsg,
                           int *result_nbrOfMsgs)
{
  int                   maxMsgArraySize;
  int                   msgArraySize = 0;
  ubs_msg_list_item_t   *msgArray = NULL;
  ubs_hash_iterator_t   it;
  ubs_hash_iterator_t   end;
  long                  msgArrayMemSize = 0;
  ubs_msg_item_t        *msg;
  int                   titlePropKeyListSize;
  unsigned              *titlePropKeyList;
  ubs_key_value_array_t propArray;
  we_dcvt_t            cvt_obj;

  titlePropKeyList = ubs_main_getTitlePropertiesDefinition (p->msg_type, 
                                                            srcModule, 
                                                            &titlePropKeyListSize);
  
  
  maxMsgArraySize = UBS_HASH_SIZE (&(p->msg_hash)) - startFromMsg;

  
  msgArray = UBS_MEM_ALLOC (sizeof (ubs_msg_list_item_t) * maxMsgArraySize);
  
  memset(msgArray,0, sizeof (ubs_msg_list_item_t) * maxMsgArraySize);

  it = ubs_hash_iterator_at (&p->msg_hash, startFromMsg);
  UBS_HASH_END(&(p->msg_hash), end);

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, 0, 0, WE_MODID_UBS);

  while (UBS_HASH_ITERATOR_NOT_EQUAL (it, end) && (msgArrayMemSize < maxResultMemSize)) {
    msg = UBS_HASH_ITERATOR_GET_TYPE (it, ubs_msg_item_t);

    if (ubs_key_value_array_is_subset (UBS_MSG_GET_PROPERTY_ARRAY (msg), filter)) {

      propArray.propertyArray = UBS_MEM_ALLOC 
                                        (sizeof (ubs_key_value_t) * titlePropKeyListSize);

      ubs_key_value_array_get_subset (UBS_MSG_GET_PROPERTY_ARRAY (msg),
                                      titlePropKeyListSize, titlePropKeyList,
                                      &propArray);
      msgArray[msgArraySize].handle = msg->handle;
      msgArray[msgArraySize].titleListSize = propArray.propertyArraySize;
      msgArray[msgArraySize].titleList = propArray.propertyArray;

      ubs_cvt_msg_list_item (&cvt_obj, &(msgArray[msgArraySize]));
      msgArrayMemSize += cvt_obj.pos;

      msgArraySize++;
    }
    UBS_HASH_ITERATOR_NEXT (it);
  }

  if (msgArrayMemSize <= maxResultMemSize) {
    
    
    *result_nextMsg = 0;
  }
  else {
    *result_nextMsg = (--msgArraySize) + startFromMsg;
    UBS_MEM_FREE (msgArray[msgArraySize].titleList);
  }

  UBS_MEM_FREE (titlePropKeyList);

  *result_nbrOfMsgs = msgArraySize;
  return msgArray;
}




void
ubs_msg_type_releaseJob_getHiddenProp (ubs_msg_type_get_msg_hidden_properties_job_t  *job)
{
  UBS_MEM_FREE (job->data);
  UBS_MEM_FREE (job);
}


void
ubs_msg_type_get_msg_hidden_properties_job (ubs_msg_type_t* p, 
                                            ubs_msg_handle_t *handle,
                                            int *result,
                                            ubs_key_value_array_t *result_array)
{
  ubs_msg_type_get_msg_hidden_properties_job_t  *job;
  ubs_msg_item_t  *msg = ubs_msg_type_get_msg_from_handle (p, handle);

  if (!msg) {
    *result = FALSE;
    return;
  }

  if (!UBS_MSG_HAS_HIDDEN_PROPERTIES_FILE(msg)) {
    result_array->propertyArraySize = 0;
    result_array->propertyArray = NULL;
    *result = TRUE;
    return;
  }

  job = UBS_MEM_ALLOCTYPE (ubs_msg_type_get_msg_hidden_properties_job_t);
  job->start = TRUE;
  job->data = NULL;
  job->result = result;
  job->result_array = result_array;
  job->msg_type = p;
  job->msg = msg;
  job->subjob_result = 0;

  UBS_MSG_TYPE_ADD_SUB_JOB (p, job,
                        ubs_msg_type_execJob_getHiddenProp, 
                        (ubs_release_job_func_t*) ubs_msg_type_releaseJob_getHiddenProp,
                        UBS_JOB_STATE_WANTS_TO_RUN);
}




void
ubs_msg_type_get_msg_data_job (ubs_msg_type_t* p, 
                               ubs_msg_handle_t *handle,
                               int *result,
                               long *result_dataLen,
                               unsigned char **result_data)
{
  ubs_msg_type_get_msg_data_job_t  *job;
  ubs_msg_item_t  *msg = ubs_msg_type_get_msg_from_handle (p, handle);
  char            fileName[UBS_MSG_STORE_MAX_PATH_LEN];

  *result_dataLen = 0;

  if (!msg) {
    *result = FALSE;
    return;
  }

  if (!UBS_MSG_HAS_DATA_FILE(msg)) {
    *result_data = NULL;
    *result = TRUE;
    return;
  }

  sprintf (fileName, UBS_MSG_STORE_MSG_DATA_FILE, p->msg_type, msg->fileId);

  job = UBS_MEM_ALLOCTYPE (ubs_msg_type_get_msg_data_job_t);
  job->start = TRUE;
  job->result = result;
  job->result_data_len = result_dataLen;
  job->result_data = result_data;
  job->subjob_result = 0;
  job->msg_type = p;
  job->msg = msg;

  UBS_MSG_TYPE_ADD_SUB_JOB (p, job,
                        ubs_msg_type_execJob_getData, 
                        (ubs_release_job_func_t*)ubs_msg_type_releaseJob_getData,
                        UBS_JOB_STATE_WANTS_TO_RUN);
}





void
ubs_msg_type_change_msg_job (ubs_msg_type_t* p, 
                             ubs_msg_handle_t *handle, 
                             ubs_key_value_array_t *changePropArray,
                             ubs_key_value_array_t *changeHiddenPropArray,
                             int changeData,
                             unsigned long dataLen,
                             unsigned char *data,
                             int *result)
{
  ubs_msg_type_change_msg_job_t *job;
  ubs_msg_item_t* msg = ubs_msg_type_get_msg_from_handle (p, handle);

  if (!msg) {
    *result = FALSE;
    return;
  }

  job = UBS_MEM_ALLOCTYPE (ubs_msg_type_change_msg_job_t);

  job->msg_type = p;
  job->msg = msg;
  job->result = result;
  job->propArray = changePropArray;
  job->hiddenPropArray = changeHiddenPropArray;
  job->changeData = changeData;
  job->dataLen = dataLen;
  job->data = data;
  job->hidden_prop_data_len = 0;
  job->subjob_result = 0;
  
  UBS_MSG_TYPE_ADD_SUB_JOB (p, job,
                        ubs_msg_type_exec_job_change_msg_hidden_prop, 
                        (ubs_release_job_func_t *)ubs_msg_type_release_job_change_msg,
                        UBS_JOB_STATE_WANTS_TO_RUN);
}









static ubs_job_signal_t
ubs_msg_type_execJob_createMsg_hidden_prop (struct ubs_job_q_head_st *jobQ, 
                                            ubs_job_q_element_t      *p, 
                                            ubs_signal_t             *sig)
{
  ubs_msg_type_create_msg_job_t *job = (ubs_msg_type_create_msg_job_t*)p->job;
  we_dcvt_t      cvt_obj;
  unsigned char   *hiddenData;
  char            fileName[UBS_MSG_STORE_MAX_PATH_LEN];

  UNREFERENCED_PARAMETER (sig);

  if (job->hiddenPropArray->propertyArraySize) {
    UBS_MSG_SET_FILE_ID (job->msg, UBS_MSG_GET_NEXT_FREE_FILE_ID (job->msg_type));

    
    UBS_MSG_SET_HAS_HIDDEN_PROPERTIES_FILE (job->msg);

    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, 0, 0, WE_MODID_UBS);
    ubs_cvt_key_value_array (job->hiddenPropArray, &cvt_obj);
    job->hidden_prop_data_len = cvt_obj.pos;
    hiddenData = UBS_MEM_ALLOC (job->hidden_prop_data_len);
    
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, hiddenData, 
                   job->hidden_prop_data_len, WE_MODID_UBS);
    ubs_cvt_key_value_array (job->hiddenPropArray, &cvt_obj);

    sprintf (fileName, UBS_MSG_STORE_MSG_HIDDEN_PROP_FILE, job->msg_type->msg_type, 
             UBS_MSG_GET_FILE_ID (job->msg));

    ubs_file_open_rewrite_mem_close_job (jobQ, job->msg_type->msg_type, fileName, 
                                         (void**)&hiddenData, job->hidden_prop_data_len, 
                                         &(job->subjob_result));
  }
  else {
    job->subjob_result = TRUE;
  }

  p->execJob = ubs_msg_type_execJob_createMsg_data;
  return UBS_JOB_SIGNAL_NOT_USED;
}




void
ubs_msg_type_create_msg_job (ubs_msg_type_t* p, 
                             ubs_msg_handle_t *handle, 
                             ubs_key_value_array_t *propArray,
                             ubs_key_value_array_t *hiddenPropArray,
                             unsigned long dataLen,
                             unsigned char *data,
                             int *result)
{  
  ubs_msg_item_t* msg = UBS_MEM_ALLOCTYPE(ubs_msg_item_t);
  ubs_msg_type_create_msg_job_t *job;

  ubs_msg_init (msg);
  ubs_msg_set_handle (msg, handle);

  if (!ubs_msg_type_add_msg(p, msg, FALSE)) {
    ubs_msg_release (msg);
    UBS_MEM_FREE (msg);
    *result = FALSE;
    return;
  }

  ubs_key_value_update (UBS_MSG_GET_PROPERTY_ARRAY(msg), propArray);

  job = UBS_MEM_ALLOCTYPE (ubs_msg_type_create_msg_job_t);

  job->msg_type = p;
  job->result = result;
  job->hiddenPropArray = hiddenPropArray;
  job->dataLen = dataLen;
  job->data = data;
  job->msg = msg;
  job->hidden_prop_data_len = 0;
  job->subjob_result = 0;
  
  UBS_MSG_TYPE_ADD_SUB_JOB (p, job,
                        ubs_msg_type_execJob_createMsg_hidden_prop, 
                        (ubs_release_job_func_t*)ubs_msg_type_releaseJob_createMsg,
                        UBS_JOB_STATE_WANTS_TO_RUN);
}







static void
ubs_msg_type_releaseJob_createMsg (ubs_msg_type_create_msg_job_t  *job)
{
  UBS_MEM_FREE (job);
}






ubs_job_signal_t 
ubs_msg_type_read_index_file (ubs_job_q_head_t *jobQ, 
                              ubs_job_q_element_t *p, 
                              ubs_signal_t *sig)
{ 
  ubs_msg_type_rw_file_t* data = (ubs_msg_type_rw_file_t*) p->job;
 
  UNREFERENCED_PARAMETER(jobQ);
  UNREFERENCED_PARAMETER(sig);


  if (data->data == NULL){
    data->result = 0;
    
    data->data = UBS_MEM_ALLOC (data->mtype->file_size);
    ubs_file_read_job (&data->mtype->job_queue, 
								   data->mtype->msg_type,
                                   data->mtype->file_handle, 
                                   data->data,
                                   data->mtype->file_size,
                                   &data->result);
    return UBS_JOB_SIGNAL_NOT_USED;
  }

  

  if (data->result<1) {
    
    
  }
  else {
    ubs_msg_type_read_items (data->mtype, data->data, data->result);
  }
      
  
  p->state = UBS_JOB_STATE_DONE;

  data->mtype->state = UBS_MSG_TYPE_RUNNING;
  uba_msg_type_notify_un_touched_count_changed(data->mtype);
  
  return UBS_JOB_SIGNAL_NOT_USED;

}

static
void ubs_msg_type_rw_index_file_release(ubs_msg_type_rw_file_t* data )
{
  UBS_MEM_FREE(data->data);
  UBS_MEM_FREE(data);
}


#define UBS_MSG_TYPE_UPDATE_FRAGMENTATION(mtype, cvt_obj)               \
  ((mtype)->file_fragmentation_begins =  \
    MIN((mtype)->file_fragmentation_begins,(cvt_obj)->pos))
  

static int
ubs_msg_type_read_items(ubs_msg_type_t* mtype,
                                     void *data,
                                     long datasize)
{
  we_dcvt_t    cvt_obj;
  we_dcvt_t    cvt_obj_release;
  ubs_ndx_item_t item;
  int res;
  long file_res;
  int file_size = 0;
  
  we_dcvt_init (&cvt_obj, WE_DCVT_DECODE, data, datasize, WE_MODID_UBS);
  we_dcvt_init (&cvt_obj_release, WE_DCVT_FREE, NULL, 0, WE_MODID_UBS);
 

  do {
    file_size = cvt_obj.pos; 
    
    res =ubs_ndx_item_init_and_cvt (&cvt_obj, &item);
    if (res){    
      switch (item.header){
      case UBS_NDX_NEW_MSG:
        res = ubs_msg_type_add_msg(mtype, item.data.new_msg, TRUE);
        if (res) {
          item.data.new_msg = NULL;
          item.header = UBS_NDX_PADDING;
        }
        break;

      case UBS_NDX_DEL_MSG:
        res =  ubs_msg_type_delete_msg_from_handle (mtype, item.data.del_msg);
        UBS_MSG_TYPE_UPDATE_FRAGMENTATION (mtype, &cvt_obj);
        break;

      case UBS_NDX_CHANGE_MSG:
        res = ubs_msg_type_change_msg  (mtype, item.data.new_msg) != NULL;
        UBS_MSG_TYPE_UPDATE_FRAGMENTATION (mtype, &cvt_obj);
        break;

      default:
        res = FALSE;
      }
      ubs_cvt_ndx_item(&cvt_obj_release,&item); 
    }
      
    


      
    
  } while (res);

  
  mtype->file_size = file_size;
  mtype->file_fragmentation_begins = MIN(file_size, mtype->file_fragmentation_begins);
  
  file_res = WE_FILE_SETSIZE(mtype->file_handle,file_size);
  UBS_LOG_FILE_ERROR(ubs_msg_type_read_items,WE_FILE_SETSIZE,file_res);
  
  res = WE_FILE_SEEK (mtype->file_handle, file_size, WE_FILE_SEEK_SET);
  UBS_LOG_FILE_ERROR(ubs_msg_type_read_items,WE_FILE_SEEK,file_res);
  return res;
}

static void
ubs_msg_type_write_items(ubs_msg_type_t *mtype,
                         we_dcvt_t     *cvt_obj)
{
  ubs_hash_iterator_t it = ubs_hash_begin(&mtype->msg_hash);
  ubs_hash_iterator_t end = ubs_hash_end(&mtype->msg_hash);
  ubs_ndx_item_t      item;

  ubs_ndx_item_init (&item);
  item.header = UBS_NDX_NEW_MSG;

  for(;UBS_HASH_ITERATOR_NOT_EQUAL(it,end); UBS_HASH_ITERATOR_NEXT(it)){
    item.data.new_msg = UBS_HASH_ITERATOR_GET (it);
    ubs_cvt_ndx_item (cvt_obj, &item);
  }
  
}




static void
ubs_msg_type_createJob_defrag_index_file (ubs_msg_type_t* mtype)
{
  ubs_msg_type_rw_file_t* p;
  
	if (((mtype->nbr_of_changes + mtype->nbr_of_deletions) > (UBS_DEFRAG_CONSTANT_1 * UBS_DEFRAG_TRIGG)) ||
		  ((mtype->nbr_of_changes + mtype->nbr_of_deletions) > (UBS_DEFRAG_CONSTANT_2 * mtype->total_nbr_of_msgs))) {
		p = UBS_MEM_ALLOCTYPE (ubs_msg_type_rw_file_t);
		p->data  = NULL;
		p->mtype = mtype;
		UBS_MSG_TYPE_ADD_JOB (mtype,
				        p,
						ubs_msg_type_defrag_index_file,
						(ubs_release_job_func_t*)ubs_msg_type_rw_index_file_release,
						UBS_JOB_STATE_WANTS_TO_RUN);
	}
}

static ubs_job_signal_t
ubs_msg_type_defrag_index_file(ubs_job_q_head_t *jobQ, 
                                ubs_job_q_element_t *p, 
                                ubs_signal_t *sig)
{
  ubs_msg_type_rw_file_t* data = (ubs_msg_type_rw_file_t*) p->job;
  int file_res;
  UNREFERENCED_PARAMETER(sig);
  UNREFERENCED_PARAMETER(jobQ);
  
  if (data->data==0)
  {
    
    we_dcvt_t cvt_obj;
    
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL,0, WE_MODID_UBS);
    ubs_msg_type_write_items (data->mtype, &cvt_obj);

    data->data = UBS_MEM_ALLOC(cvt_obj.pos);
    data->result = cvt_obj.pos;
    data->mtype->file_size = data->result;
    data->mtype->file_fragmentation_begins = data->mtype->file_size;

    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, data->data, data->result, WE_MODID_UBS);
    ubs_msg_type_write_items (data->mtype, &cvt_obj);

    file_res = WE_FILE_SEEK (data->mtype->file_handle,0, WE_FILE_SEEK_SET);
    UBS_LOG_FILE_ERROR(ubs_msg_type_defrag_index_file,WE_FILE_SEEK,file_res);
    
    if ( file_res < 0)
    {
      
      WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,  
      "UBS: Defrag index file failed (seek failed = %i) [ubs_msg_type_defrag_index_file]\n",
      file_res));
      p->state = UBS_JOB_STATE_DONE;
      return UBS_JOB_SIGNAL_NOT_USED;
    }

    ubs_file_write_job (&data->mtype->job_queue, 
	  data->mtype->msg_type,
      data->mtype->file_handle,
      data->data, data->result,
      &data->result);

    return UBS_JOB_SIGNAL_NOT_USED;
  }

  
  p->state = UBS_JOB_STATE_DONE;
  
  if (data->result < 0 ){
    
   WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,  
   "UBS: Defrag index file failed (write job failed = %i) [ubs_msg_type_defrag_index_file]\n", 
   data->result));
  }
  else{
    
    file_res = WE_FILE_SETSIZE(data->mtype->file_handle, data->mtype->file_size);
    
    UBS_LOG_FILE_ERROR(ubs_msg_type_defrag_index_file,WE_FILE_SETSIZE,file_res);

  }
  
  return UBS_JOB_SIGNAL_NOT_USED;
}







static ubs_msg_item_t*
ubs_msg_type_get_msg (ubs_msg_type_t* p, ubs_msg_item_t *msg)
{
  ubs_hash_iterator_t it = ubs_hash_find (&p->msg_hash,msg);
  ubs_hash_iterator_t end = ubs_hash_end (&p->msg_hash);
  return (UBS_HASH_ITERATOR_EQUAL (it,end)) ? NULL: UBS_HASH_ITERATOR_GET(it);
  
}
static ubs_msg_item_t*
ubs_msg_type_change_msg (ubs_msg_type_t *mtype,
                         ubs_msg_item_t *msg)
{
  ubs_msg_item_t *org = ubs_msg_type_get_msg (mtype,msg);
  int             org_touched;
  if (!org){
    return NULL;
  }
  
  mtype->nbr_of_changes++;
  org_touched = UBS_MSG_IS_TOUCHED(msg);
  ubs_msg_change(org,msg);

  if (!org_touched && UBS_MSG_IS_TOUCHED(org)){
    --mtype->un_touched_count;
    uba_msg_type_notify_un_touched_count_changed (mtype);
  }
  
  return org;
}


static ubs_job_signal_t
ubs_msg_type_execJob_getHiddenProp (struct ubs_job_q_head_st *jobQ, 
                                 ubs_job_q_element_t *p, 
                                 ubs_signal_t *sig)
{
  ubs_msg_type_get_msg_hidden_properties_job_t  *job = 
                (ubs_msg_type_get_msg_hidden_properties_job_t*)p->job;
  char          fileName[UBS_MSG_STORE_MAX_PATH_LEN];

  UNREFERENCED_PARAMETER (sig);

  if (job->start) {
    job->start = FALSE;

    sprintf (fileName, UBS_MSG_STORE_MSG_HIDDEN_PROP_FILE, 
             job->msg_type->msg_type, UBS_MSG_GET_FILE_ID (job->msg));

    ubs_file_open_read_close_job (jobQ, job->msg_type->msg_type, fileName, (void**)&(job->data), &(job->dataLen), 
                                   &job->subjob_result);
  }
  else {
    we_dcvt_t  cvt_obj;

    p->state = UBS_JOB_STATE_DONE;

    if (job->subjob_result < 0) {
      *job->result = FALSE;
      return UBS_JOB_SIGNAL_NOT_USED;
    }

    we_dcvt_init (&cvt_obj, WE_DCVT_DECODE, job->data, job->dataLen, WE_MODID_UBS);

    *job->result = ubs_cvt_key_value_array (job->result_array, &cvt_obj);
  }

  return UBS_JOB_SIGNAL_NOT_USED;
}


static ubs_job_signal_t
ubs_msg_type_execJob_getData (struct ubs_job_q_head_st *jobQ, 
                                 ubs_job_q_element_t *p, 
                                 ubs_signal_t *sig)
{
  ubs_msg_type_get_msg_data_job_t  *job = (ubs_msg_type_get_msg_data_job_t*)p->job;
  char          fileName[UBS_MSG_STORE_MAX_PATH_LEN];

  UNREFERENCED_PARAMETER (sig);

  if (job->start) {
    job->start = FALSE;

    sprintf (fileName, UBS_MSG_STORE_MSG_DATA_FILE, 
             job->msg_type->msg_type, UBS_MSG_GET_FILE_ID (job->msg));

    ubs_file_open_read_close_job (jobQ, job->msg_type->msg_type, fileName, (void**)job->result_data, job->result_data_len, 
                                   &job->subjob_result);
  }
  else {
    p->state = UBS_JOB_STATE_DONE;

    if (job->subjob_result < 0) {
      *job->result = FALSE;
      return UBS_JOB_SIGNAL_NOT_USED;
    }
  }

  return UBS_JOB_SIGNAL_NOT_USED;
}


static void
ubs_msg_type_releaseJob_getData (ubs_msg_type_get_msg_data_job_t  *job)
{
  UBS_MEM_FREE (job);
}



static ubs_job_signal_t
ubs_msg_type_exec_job_change_msg_data (struct ubs_job_q_head_st *jobQ, 
                                 ubs_job_q_element_t *p, 
                                 ubs_signal_t *sig)
{
  ubs_msg_type_change_msg_job_t *job = (ubs_msg_type_change_msg_job_t*)p->job;
  char            fileName[UBS_MSG_STORE_MAX_PATH_LEN];

  UNREFERENCED_PARAMETER (sig);

  if (job->subjob_result <= 0) {
    p->execJob = ubs_msg_type_exec_job_change_msg_end;
    return UBS_JOB_SIGNAL_NOT_USED;
  }

  if (!job->changeData) {
    p->execJob = ubs_msg_type_exec_job_change_msg_ndx;
    return UBS_JOB_SIGNAL_NOT_USED;
  }

  if (job->dataLen) {
    if (!UBS_MSG_GET_FILE_ID (job->msg)) {
      WE_UINT32 file_id = UBS_MSG_GET_NEXT_FREE_FILE_ID (job->msg_type);
      UBS_MSG_SET_FILE_ID (job->msg, file_id);
    }

    
    UBS_MSG_SET_HAS_DATA_FILE (job->msg);

    sprintf (fileName, UBS_MSG_STORE_MSG_DATA_FILE, job->msg_type->msg_type, 
             UBS_MSG_GET_FILE_ID (job->msg));

    ubs_file_open_rewrite_close_job (jobQ, job->msg_type->msg_type, fileName, 
										job->data, job->dataLen, &(job->subjob_result));
  }
  else if (UBS_MSG_HAS_DATA_FILE (job->msg)) {
    

    sprintf (fileName, UBS_MSG_STORE_MSG_DATA_FILE, job->msg_type->msg_type, 
             UBS_MSG_GET_FILE_ID (job->msg));

    job->subjob_result = WE_FILE_REMOVE (fileName);
    UBS_LOG_FILE_ERROR(ubs_msg_type_exec_job_change_msg_data, 
                        WE_FILE_REMOVE, job->subjob_result);

    if (job->subjob_result < 0) {
      p->execJob = ubs_msg_type_exec_job_change_msg_end;
      return UBS_JOB_SIGNAL_NOT_USED;
    }

    UBS_MSG_SET_HAS_NO_DATA_FILE (job->msg);
  }

  p->execJob = ubs_msg_type_exec_job_change_msg_ndx;
  return UBS_JOB_SIGNAL_NOT_USED;
}






static ubs_job_signal_t
ubs_msg_type_exec_job_change_msg_end (struct ubs_job_q_head_st *jobQ, 
                                 ubs_job_q_element_t *p, 
                                 ubs_signal_t *sig)
{
  ubs_msg_type_change_msg_job_t *job = (ubs_msg_type_change_msg_job_t*)p->job;

  UNREFERENCED_PARAMETER (sig);
  UNREFERENCED_PARAMETER (jobQ);

  *job->result = (job->subjob_result > 0);
  p->state = UBS_JOB_STATE_DONE;
  return UBS_JOB_SIGNAL_NOT_USED;
}


static ubs_job_signal_t
ubs_msg_type_exec_job_change_msg_ndx (struct ubs_job_q_head_st *jobQ, 
                                 ubs_job_q_element_t *p, 
                                 ubs_signal_t *sig)
{
  ubs_msg_type_change_msg_job_t *job = (ubs_msg_type_change_msg_job_t*)p->job;
  ubs_ndx_item_t  ndx_job_item;
  
  UNREFERENCED_PARAMETER (sig);

  if (job->subjob_result > 0) {
    





    ubs_key_value_array_t tmp_store = job->msg->propertyArray;  
    
    job->msg->propertyArray = *(job->propArray);
    
    UBS_NDX_INIT_CHANGE (&ndx_job_item, job->msg);

    ubs_ndx_write_item_job (jobQ, job->msg_type->msg_type, job->msg_type->file_handle, &ndx_job_item, 
                            &(job->subjob_result));
    
    job->msg->propertyArray = tmp_store;
    
    if (job->propArray) {
      
      ubs_msg_item_t item;
        
      ubs_msg_init(&item);
      item.handle = job->msg->handle;
      item.handle_hash = 0;
      item.propertyArray = *(job->propArray);
      item.flags = job->msg->flags;

      
      ubs_msg_type_change_msg (job->msg_type, &item);
      
      
      *(job->propArray) = item.propertyArray; 
    }
  }

  p->execJob = ubs_msg_type_exec_job_change_msg_end;
  ubs_msg_type_createJob_defrag_index_file (job->msg_type);
  return UBS_JOB_SIGNAL_NOT_USED;
}




static ubs_job_signal_t
ubs_msg_type_exec_job_change_msg_hidden_prop (struct ubs_job_q_head_st *jobQ, 
                                 ubs_job_q_element_t *p, 
                                 ubs_signal_t *sig)
{
  ubs_msg_type_change_msg_job_t *job = (ubs_msg_type_change_msg_job_t*)p->job;
  we_dcvt_t      cvt_obj;
  unsigned char   *hiddenData;
  char            fileName[UBS_MSG_STORE_MAX_PATH_LEN];

  UNREFERENCED_PARAMETER (sig);

  if (job->hiddenPropArray && job->hiddenPropArray->propertyArraySize) {

    if (!UBS_MSG_GET_FILE_ID (job->msg)) {
      UBS_MSG_SET_FILE_ID (job->msg, UBS_MSG_GET_NEXT_FREE_FILE_ID (job->msg_type));
    }

    
    UBS_MSG_SET_HAS_HIDDEN_PROPERTIES_FILE (job->msg);

    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, 0, 0, WE_MODID_UBS);
    ubs_cvt_key_value_array (job->hiddenPropArray, &cvt_obj);
    job->hidden_prop_data_len = cvt_obj.pos;
    hiddenData = UBS_MEM_ALLOC (job->hidden_prop_data_len);
    
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, hiddenData, 
                   job->hidden_prop_data_len, WE_MODID_UBS);
    ubs_cvt_key_value_array (job->hiddenPropArray, &cvt_obj);

    sprintf (fileName, UBS_MSG_STORE_MSG_HIDDEN_PROP_FILE, job->msg_type->msg_type, 
             UBS_MSG_GET_FILE_ID (job->msg));

    ubs_file_open_rewrite_mem_close_job (jobQ, 
										 job->msg_type->msg_type, 
                                         fileName, 
                                         (void**)&hiddenData,  
                                         job->hidden_prop_data_len, 
                                         &(job->subjob_result));
  }
  else {
    job->subjob_result = TRUE;
  }

  p->execJob = ubs_msg_type_exec_job_change_msg_data;
  return UBS_JOB_SIGNAL_NOT_USED;
}


static void
ubs_msg_type_release_job_change_msg (ubs_msg_type_create_msg_job_t  *job)
{
  UBS_MEM_FREE (job);
}

static ubs_job_signal_t
ubs_msg_type_execJob_createMsg_end (struct ubs_job_q_head_st *jobQ, 
                                    ubs_job_q_element_t      *p, 
                                    ubs_signal_t             *sig)
{
  ubs_msg_type_create_msg_job_t *job = (ubs_msg_type_create_msg_job_t*)p->job;

  UNREFERENCED_PARAMETER (sig);
  UNREFERENCED_PARAMETER (jobQ);

  if (job->subjob_result <= 0) {
    
    ubs_msg_type_delete_msg (job->msg_type, job->msg);
    job->msg = NULL;
  }

  *job->result = (job->subjob_result > 0);
  p->state = UBS_JOB_STATE_DONE;
  return UBS_JOB_SIGNAL_NOT_USED;
}


static ubs_job_signal_t
ubs_msg_type_execJob_createMsg_ndx (struct ubs_job_q_head_st *jobQ, 
                                    ubs_job_q_element_t      *p, 
                                    ubs_signal_t             *sig)
{
  ubs_msg_type_create_msg_job_t *job = (ubs_msg_type_create_msg_job_t*)p->job;
  ubs_ndx_item_t  ndx_job_item;

  UNREFERENCED_PARAMETER (sig);

  if (job->subjob_result > 0) {
    UBS_NDX_INIT_NEW (&ndx_job_item, job->msg);

    ubs_ndx_write_item_job (jobQ, job->msg_type->msg_type, job->msg_type->file_handle, &ndx_job_item, 
                            &(job->subjob_result));
  }

  p->execJob = ubs_msg_type_execJob_createMsg_end;
  return UBS_JOB_SIGNAL_NOT_USED;
}

static ubs_job_signal_t
ubs_msg_type_execJob_createMsg_data (struct ubs_job_q_head_st *jobQ, 
                                     ubs_job_q_element_t      *p, 
                                     ubs_signal_t             *sig)
{
  ubs_msg_type_create_msg_job_t *job = (ubs_msg_type_create_msg_job_t*)p->job;
  char            fileName[UBS_MSG_STORE_MAX_PATH_LEN];

  UNREFERENCED_PARAMETER (sig);

  if (job->subjob_result > 0) {

    if (job->dataLen) {
      if (!UBS_MSG_GET_FILE_ID (job->msg)) {
        UBS_MSG_SET_FILE_ID (job->msg, UBS_MSG_GET_NEXT_FREE_FILE_ID (job->msg_type));
      }

      
      UBS_MSG_SET_HAS_DATA_FILE (job->msg);

      sprintf (fileName, UBS_MSG_STORE_MSG_DATA_FILE, job->msg_type->msg_type, 
               UBS_MSG_GET_FILE_ID (job->msg));

      ubs_file_open_rewrite_close_job (jobQ, job->msg_type->msg_type, fileName, job->data,
										job->dataLen, &(job->subjob_result));
    }

    p->execJob = ubs_msg_type_execJob_createMsg_ndx;
  }
  else {
    p->execJob = ubs_msg_type_execJob_createMsg_end;
  }
  
  return UBS_JOB_SIGNAL_NOT_USED;
}


static void
uba_msg_type_notify_un_touched_count_changed(ubs_msg_type_t* mtype)
{
  

  if ((mtype->ndx_file_exist == FALSE) && (mtype->state == UBS_MSG_TYPE_STARTING)) {
    UBSa_notifyUnreadMsgCount (mtype->msg_type, mtype->un_touched_count);

    return;
  }

  if ((mtype->ndx_file_exist == TRUE) && (mtype->state == UBS_MSG_TYPE_RUNNING)) {
    UBSa_notifyUnreadMsgCount (mtype->msg_type, mtype->un_touched_count);
  }

  
}


static int
ubs_msg_type_add_msg(ubs_msg_type_t* mtype, ubs_msg_item_t* msg, int called_from_ndx_file_reading)
{
  int res = ubs_hash_insert (&mtype->msg_hash, msg);
  
  
  if (res) { 
    mtype->total_nbr_of_msgs++;
    total_nbr_of_push_si_and_sl++;
  }

  if (res && !UBS_MSG_IS_TOUCHED(msg)){
    ++mtype->un_touched_count;
    uba_msg_type_notify_un_touched_count_changed(mtype);
  }

  if (!called_from_ndx_file_reading) {
    if (total_nbr_of_push_si_and_sl >= UBS_MAX_NBR_OF_MSGS) {
      UBSa_error (UBS_RESULT_NBR_OF_MESSAGES_EXCEEDS_MAX_ERROR);
    }
  }

  return res;
}
