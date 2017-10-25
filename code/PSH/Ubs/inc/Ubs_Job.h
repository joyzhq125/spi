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









#ifndef UBS_JOB_H
#define UBS_JOB_H

#ifndef UBS_LIST_H
#include "Ubs_List.h"
#endif

#ifndef UBS_SIG_H
#include "Ubs_Sig.h"
#endif







typedef struct ubs_job_q_head_st {
  ubs_list_t joblist;
} ubs_job_q_head_t;


struct ubs_job_q_element_st;

typedef enum
{
  UBS_JOB_SIGNAL_USED, 
  UBS_JOB_SIGNAL_NOT_USED 
} ubs_job_signal_t;


typedef ubs_job_signal_t ubs_execute_job_func_t (ubs_job_q_head_t *jobQ, 
                                    struct ubs_job_q_element_st *p, 
                                    ubs_signal_t *sig);
typedef void ubs_release_job_func_t (void *job);

typedef enum
{
  UBS_JOB_TYPE_NORMAL, 
  UBS_JOB_TYPE_RUNING  
} ubs_job_type_t;

typedef enum
{
  UBS_JOB_STATE_WANTS_TO_RUN,
  UBS_JOB_STATE_WAITING_FOR_SIGNAL, 
  UBS_JOB_STATE_DONE           
} ubs_job_state_t;




typedef struct ubs_job_q_element_st {
  void                          *job;     
  ubs_execute_job_func_t        *execJob; 
  ubs_release_job_func_t        *releaseJob; 
  ubs_job_state_t               state;    
  
  
  ubs_job_type_t                job_type;  
} ubs_job_q_element_t;




  




void
ubs_jobQ_init(ubs_job_q_head_t *jobQ);

void 
ubs_jobQ_release(ubs_job_q_head_t *jobQ);

ubs_job_signal_t
ubs_jobQ_process (ubs_job_q_head_t *jobQ, ubs_signal_t *sig);

void 
ubs_jobQ_process_job (ubs_job_q_head_t *jobQ);

void
ubs_jobQ_add (ubs_job_q_head_t *jobQ,
              void *job,
              ubs_execute_job_func_t *func,
              ubs_release_job_func_t *release,
              ubs_job_state_t state);

void
ubs_jobQ_addFirst (ubs_job_q_head_t *jobQ, 
                   void *job,
                   ubs_execute_job_func_t *func,
                   ubs_release_job_func_t *release,
                   ubs_job_state_t state);

void
ubs_jobQ_removeFirst (ubs_job_q_head_t *jobQ);


int
ubs_jobQ_has_running_jobs (ubs_job_q_head_t *jobQ);





typedef struct {
  int           msgType;
  int           fileHandle;
  unsigned char *data;
  long          data_size;
  long          *result;
} ubs_job_file_t;


void
ubs_file_read_job (ubs_job_q_head_t* jobQ,
				               int msgType,
                               int fileHandle, 
                               void *data, 
                               long size,
                               long *result);

void 
ubs_file_write_job (ubs_job_q_head_t* jobQ,
								int msgType,
                                int fileHandle, 
                                void *data, 
                                long size,
                                long *result);





void
ubs_file_write_mem_job (ubs_job_q_head_t* jobQ,
									int msgType,
                                    int fileHandle, 
                                    void **data, 
                                    long size,
                                    long *result);


void
ubs_file_open_read_close_job (ubs_job_q_head_t* jobQ,
								      int msgType,
                               const char *file_name, 
                               void **data, 
                               long *size,
                               long *result);

void 
ubs_file_open_rewrite_close_job (ubs_job_q_head_t* jobQ,
								 int msgType,
                                const char *file_name, 
                                void *data, 
                                long size,
                                long *result);

void
ubs_file_open_rewrite_mem_close_job (ubs_job_q_head_t* jobQ,
											int msgType,
                                    const char *file_name, 
                                    void **data, 
                                    long size,
                                    long *result);


#endif      /*UBS_JOB_H*/
