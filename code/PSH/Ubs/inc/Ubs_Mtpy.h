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









#ifndef UBS_MTPY_H
#define UBS_MTPY_H 




#ifndef UBS_HASH_H
#include "Ubs_Hash.h"
#endif

#ifndef UBS_JOB_H
#include "Ubs_Job.h"
#endif

#ifndef UBS_MSG_H
#include "Ubs_Msg.h"
#endif




typedef enum{
  UBS_MSG_TYPE_STARTING,
  UBS_MSG_TYPE_RUNNING
} ubs_msg_type_state_t;

typedef struct {
  int                               msg_type;
  ubs_hash_t                        msg_hash;
  unsigned long                     next_free_file_id;
  int                               file_handle;
  ubs_job_q_head_t                  job_queue;
  long                              file_size;
  long                              file_fragmentation_begins;
  int                               total_nbr_of_msgs;
  int                               nbr_of_changes;
  int                               nbr_of_deletions;
  int                               un_touched_count;
  ubs_msg_type_state_t              state;
  int                               ndx_file_exist;
} ubs_msg_type_t;






#define UBS_MSG_TYPE_ADD_SUB_JOB(msg_ptr,job_data,func,release_func,state) \
  ubs_jobQ_addFirst (&msg_ptr->job_queue, job_data, func, release_func, state)

#define UBS_MSG_TYPE_ADD_JOB(msg_ptr,job_data,func,release_func,state) \
  ubs_jobQ_add (&msg_ptr->job_queue, job_data, func, release_func, state)

#define UBS_MSG_TYPE_ADD_JOB_AND_RUN(msg_ptr,job_data, func, release_func, state)   \
  ubs_jobQ_add (&((msg_ptr)->job_queue), (job_data), (func), (release_func), (state));          \
  ubs_jobQ_process_job (&((msg_ptr)->job_queue))

#define UBS_MSG_TYPE_PROCESS_SIGNAL(msg_type, sig)            \
  ubs_jobQ_process (&msg_type->job_queue, sig)



#define UBS_MSG_GET_NEXT_FREE_FILE_ID(mtype) \
  ((mtype)->next_free_file_id = (((mtype)->next_free_file_id == 0x3FFFFF)? (1) :  \
    (++((mtype)->next_free_file_id))))












void  
ubs_msg_type_init_job (ubs_msg_type_t* mtype, 
                       int msg_type,
                       const char* file_name,
                       long file_size);

void
ubs_msg_type_release (ubs_msg_type_t* p);

int
ubs_msg_ready_to_terminate (ubs_msg_type_t* p);





int
ubs_msg_type_delete_msg_from_handle (ubs_msg_type_t* p, ubs_msg_handle_t *handle);

int 
ubs_msg_type_delete_msg (ubs_msg_type_t* p, ubs_msg_item_t* msg);







void
ubs_msg_type_write_del_msg_job (ubs_msg_type_t* p, ubs_msg_handle_t *handle, long *result);


int
ubs_msg_type_get_nbr_of_msgs (ubs_msg_type_t* p, ubs_key_value_array_t *filter);





ubs_msg_list_item_t*
ubs_msg_type_get_msg_list (ubs_msg_type_t* p, 
                           ubs_key_value_array_t *filter,
                           int srcModule,
                           int startFromMsg,
                           long maxResultMemSize,
                           int *result_nextMsg,
                           int *result_nbrOfMsgs);


ubs_msg_item_t*
ubs_msg_type_get_msg_from_handle (ubs_msg_type_t* p, ubs_msg_handle_t *handle);

void
ubs_msg_type_get_msg_hidden_properties_job (ubs_msg_type_t* p, 
                                            ubs_msg_handle_t *handle,
                                            int *result,
                                            ubs_key_value_array_t *result_array);

void
ubs_msg_type_get_msg_data_job (ubs_msg_type_t* p, 
                               ubs_msg_handle_t *handle,
                               int *result,
                               long *result_dataLen,
                               unsigned char **result_data);

void
ubs_msg_type_change_msg_job (ubs_msg_type_t* p, 
                             ubs_msg_handle_t *handle, 
                             ubs_key_value_array_t *changePropArray,
                             ubs_key_value_array_t *changeHiddenPropArray,
                             int changeData,
                             unsigned long dataLen,
                             unsigned char *data,
                             int *result);

void
ubs_msg_type_create_msg_job (ubs_msg_type_t* p, 
                             ubs_msg_handle_t *handle, 
                             ubs_key_value_array_t *propArray,
                             ubs_key_value_array_t *hiddenPropArray,
                             unsigned long dataLen,
                             unsigned char *data,
                             int *result);

#endif      /*UBS_MTPY_H*/
