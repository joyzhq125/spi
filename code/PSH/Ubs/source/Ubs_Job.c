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










#include "Ubs_Job.h"
#include "Ubs_Main.h"
#include "Ubs_Msin.h" 
#include "We_File.h"
#include "We_Log.h"
#include "Ubs_A.h"

void
ubs_jobQ_init (ubs_job_q_head_t *jobQ)
{
	ubs_list_init (&jobQ->joblist, WE_MODID_UBS);
}

void 
ubs_jobQ_release (ubs_job_q_head_t *jobQ)
{
	ubs_list_iterator_t end;
	ubs_list_iterator_t it;
  ubs_job_q_element_t *job;

  UBS_LIST_END( &jobQ->joblist, end);
  UBS_LIST_BEGIN( &jobQ->joblist ,it);
	 
  while (!UBS_LIST_ITERATOR_EQUAL (it, end) ) {
		


    job = UBS_LIST_ITERATOR_GET(it);
    job->releaseJob( job->job);
		
    UBS_MEM_FREE ( job );
		it = ubs_list_erase (it);
	}
	
  ubs_list_release (&jobQ->joblist);
}




ubs_job_signal_t
ubs_jobQ_process (ubs_job_q_head_t *jobQ, ubs_signal_t *sig)
{
  ubs_job_signal_t     retval = UBS_JOB_SIGNAL_NOT_USED;
  ubs_job_q_element_t *job ;
	
  
	if (UBS_LIST_SIZE (&jobQ->joblist) == 0) {
		return retval;
	}
  
  
	job = UBS_LIST_FRONT(&jobQ->joblist);
	if (job->state != UBS_JOB_STATE_WAITING_FOR_SIGNAL){
		return retval;
	}

  retval = (*job->execJob) (jobQ, job, sig);
  job->job_type = UBS_JOB_TYPE_RUNING; 
  

  ubs_jobQ_process_job (jobQ);
	
  return retval;
}

void 
ubs_jobQ_process_job (ubs_job_q_head_t *jobQ)
{
  ubs_job_q_element_t *job ;
  
  while (UBS_LIST_SIZE(&jobQ->joblist) != 0)
  {
    job = UBS_LIST_FRONT(&jobQ->joblist);
    
    switch (job->state){
    
    case UBS_JOB_STATE_WANTS_TO_RUN:
      job->execJob(jobQ, job, NULL);
      job->job_type = UBS_JOB_TYPE_RUNING; 
      break;

    case UBS_JOB_STATE_WAITING_FOR_SIGNAL:
      return;

    case UBS_JOB_STATE_DONE:
      job->releaseJob( job->job );
      ubs_list_pop_front (&jobQ->joblist );
      UBS_MEM_FREE(job);
      break;

    default:
      WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,
        "UBS: Unkown job state (state =  %i) [ubs_jobQ_process_job]\n",job->state));
      job->releaseJob( job->job );
      ubs_list_pop_front (&jobQ->joblist );
      UBS_MEM_FREE(job);
    }
  }
	
}


ubs_job_q_element_t* 
ubs_jobQ_create_element(void *job, 
                        ubs_execute_job_func_t *func, 
                        ubs_release_job_func_t *release, 
                        ubs_job_state_t state, 
                        ubs_job_type_t job_type){
	ubs_job_q_element_t* element = UBS_MEM_ALLOCTYPE(ubs_job_q_element_t);
	element->execJob = func;
  element->releaseJob = release;
	element->job = job;
	element->state = state;
  element->job_type = job_type;
	return element;
}

void
ubs_jobQ_add (ubs_job_q_head_t *jobQ, 									 
							void *job,
							ubs_execute_job_func_t *func,
              ubs_release_job_func_t *release,
							ubs_job_state_t state)
{
	ubs_job_q_element_t* element = ubs_jobQ_create_element (job, func, release, 
                                                          state, UBS_JOB_TYPE_NORMAL);
  ubs_list_push_back(&jobQ->joblist, element);

}


int
ubs_jobQ_has_running_jobs (ubs_job_q_head_t *jobQ)
{
  ubs_list_iterator_t it;
  ubs_list_iterator_t end;
  UBS_LIST_BEGIN (&jobQ->joblist, it);
  UBS_LIST_END   (&jobQ->joblist, end);

  if (UBS_LIST_ITERATOR_EQUAL(it,end)){
    return FALSE;
  }

  return 
   (UBS_LIST_ITERATOR_GET_TYPE (it, ubs_job_q_element_t))->job_type == UBS_JOB_TYPE_RUNING;
}

void
ubs_jobQ_addFirst (ubs_job_q_head_t *jobQ, 									 
									 void *job,
									 ubs_execute_job_func_t *func,
                   ubs_release_job_func_t *release,
									 ubs_job_state_t state)
{
	ubs_job_q_element_t* 
        element = ubs_jobQ_create_element (job, func, release, state, UBS_JOB_TYPE_RUNING);
  ubs_list_push_front(&jobQ->joblist, element);
}


void
ubs_jobQ_removeFirst (ubs_job_q_head_t *jobQ)
{
	ubs_list_t* list = &jobQ->joblist;
	
	if (UBS_LIST_SIZE (list) > 0){
		UBS_MEM_FREE (UBS_LIST_FRONT(list));
		ubs_list_pop_front (list);
	}		
}








static ubs_job_signal_t 
ubs_file_read_exec_job ( ubs_job_q_head_t    *jobQ, 
									       ubs_job_q_element_t *element, 
									       ubs_signal_t        *sig )
{
	
  ubs_job_file_t   *job = ( ubs_job_file_t* ) element->job;
	unsigned char    *data;
	long             bytes_to_read;
	long             result;
	ubs_job_signal_t func_res = UBS_JOB_SIGNAL_NOT_USED;

  UNREFERENCED_PARAMETER(jobQ);

	if (element->state == UBS_JOB_STATE_WAITING_FOR_SIGNAL ) {
		we_file_notify_t *notify;
		
		if (sig->type != UBS_MSG_STORE_SIG_FILE_NOTIFY) {
			return UBS_JOB_SIGNAL_NOT_USED;
    }
		
    notify = (we_file_notify_t*) sig->p_param;
		if (notify->eventType != WE_FILE_READ_READY || 
			  notify->fileHandle != job->fileHandle) {
			return UBS_JOB_SIGNAL_NOT_USED;
		}
    
    WE_SIGNAL_DESTRUCT (WE_MODID_UBS, WE_SIG_FILE_NOTIFY, sig->p_param);
		func_res = UBS_JOB_SIGNAL_USED;
		element->state = UBS_JOB_STATE_WANTS_TO_RUN;
	}

	
	
	do {
		data          = job->data + *(job->result);
    bytes_to_read = job->data_size - *job->result;
	
    
    result = WE_FILE_READ (job->fileHandle, data, bytes_to_read);
		UBS_LOG_FILE_ERROR_ASYNC(ubs_file_read_exec_job, WE_FILE_READ, result);
	
    
    if (result>0){
			*(job->result) += result;

		}
	} while ( (*(job->result) < job->data_size) && result >0 );

	if (result == WE_FILE_ERROR_DELAYED ){
		
		WE_FILE_SELECT( job->fileHandle, WE_FILE_EVENT_READ );
		element->state = UBS_JOB_STATE_WAITING_FOR_SIGNAL;
		return func_res;
	}

	
  element->state = UBS_JOB_STATE_DONE;
	return func_res;
}

static ubs_job_signal_t
ubs_file_write_exec_job ( ubs_job_q_head_t   *jobQ, 
									        struct ubs_job_q_element_st *element, 
							            ubs_signal_t        *sig )
{
	ubs_job_file_t  *job = (ubs_job_file_t*) element->job;
	unsigned char   *data;
	long             bytes_to_write;
	long             result;
	ubs_job_signal_t func_res = UBS_JOB_SIGNAL_NOT_USED;

  UNREFERENCED_PARAMETER(jobQ);

	if (element->state == UBS_JOB_STATE_WAITING_FOR_SIGNAL ) {
		we_file_notify_t *notify;
		
		if (sig->type != UBS_MSG_STORE_SIG_FILE_NOTIFY) {
			return UBS_JOB_SIGNAL_NOT_USED;
    }
		
    notify = (we_file_notify_t*) sig->p_param;
		if (notify->eventType != WE_FILE_WRITE_READY || 
			  notify->fileHandle != job->fileHandle) {
			return UBS_JOB_SIGNAL_NOT_USED;
		}
    WE_SIGNAL_DESTRUCT (WE_MODID_UBS, WE_SIG_FILE_NOTIFY, sig->p_param);
		func_res = UBS_JOB_SIGNAL_USED;
		element->state = UBS_JOB_STATE_WANTS_TO_RUN;
	}

	
	
	do {
		data          = job->data + *(job->result);
		bytes_to_write = job->data_size - *job->result;
	
    result = WE_FILE_WRITE (job->fileHandle, data, bytes_to_write);
    UBS_LOG_FILE_ERROR_ASYNC(ubs_file_write_exec_job, WE_FILE_WRITE, result);

		if (result>0){
			*(job->result) += result;

		}
	} while ( (*(job->result) < job->data_size) && result > 0 );

  if (result > 0 ) {
    





    WE_FILE_FLUSH (job->fileHandle);
  }
  else if (result == WE_FILE_ERROR_DELAYED ){
		
		WE_FILE_SELECT( job->fileHandle, WE_FILE_EVENT_WRITE );
		element->state = UBS_JOB_STATE_WAITING_FOR_SIGNAL;
		return func_res;
	}

  
  {
    char file_name[UBS_MSG_STORE_MAX_PATH_LEN];
    int file_size = 0; 

    sprintf (file_name, UBS_MSG_STORE_FOLDER_MSG_TYPE_INDEX_FILE, job->msgType);
    
    file_size = WE_FILE_GETSIZE (file_name);

    if (file_size > UBS_MAX_INDEX_FILE_SIZE) {
      UBSa_error (UBS_RESULT_INDEX_FILE_MAX_SIZE_ERROR);
    }
  }

	
  element->state = UBS_JOB_STATE_DONE;
	return func_res;
}

ubs_job_file_t*  ubs_file_create_rw_data(int msgType, 
										 int fileHandle, 
																	        void *data, 
																	        long size,
																	        long *result
																	      )
{
	ubs_job_file_t      *fdata = UBS_MEM_ALLOCTYPE( ubs_job_file_t );
	fdata->data            = data;
	fdata->msgType         = msgType;
	fdata->fileHandle      = fileHandle;
	fdata->result          = result;
	fdata->data_size			 = size;

	return fdata;
}

void
ubs_file_release_rw_data(void * data){
  UBS_MEM_FREE(data);
}

static void
ubs_file_release_rw_data_and_buf (ubs_job_file_t* data)
{
  UBS_MEM_FREE(data->data);
  UBS_MEM_FREE(data);
}

static void
ubs_file_release_rw_data_close_file(ubs_job_file_t* data)
{
  int result = WE_FILE_CLOSE (data->fileHandle);
  UBS_LOG_FILE_ERROR_ASYNC (ubs_file_release_rw_data_close_file, WE_FILE_CLOSE, result);
  UBS_MEM_FREE (data);
}

void
ubs_file_release_rw_data_close_file_and_buf(ubs_job_file_t* data)
{
  int result = WE_FILE_CLOSE (data->fileHandle);
  UBS_LOG_FILE_ERROR(ubs_file_release_rw_data_close_file_and_buf, WE_FILE_CLOSE, result);
  UBS_MEM_FREE (data->data);
  UBS_MEM_FREE (data);
}



#define UBS_ADD_NEW_FILE_JOB(jobQ, msgType, fileHandle, data, size, result,func) \
	ubs_jobQ_addFirst(																										\
		jobQ,																																\
		ubs_file_create_rw_data (msgType, fileHandle, data, size, result),			    \
		func,                                                               \
    ubs_file_release_rw_data,																					  \
		UBS_JOB_STATE_WANTS_TO_RUN)








void
ubs_file_read_job (ubs_job_q_head_t* jobQ,
				   int msgType,
									 int fileHandle, 
									 void *data, 
									 long size,
									 long *result)
{
	UBS_ADD_NEW_FILE_JOB (jobQ, msgType, fileHandle, data, size, result,ubs_file_read_exec_job);
}


void
ubs_file_write_job (ubs_job_q_head_t* jobQ,
					int msgType,
										int fileHandle, 
										void *data, 
										long size,
										long *result)
{
  *result = 0;
	UBS_ADD_NEW_FILE_JOB (jobQ, msgType, fileHandle, data, size, result, 
                        ubs_file_write_exec_job);
                        

}

void
ubs_file_write_mem_job( ubs_job_q_head_t* jobQ, int msgType, int fileHandle, void **data, 
												long size, long *result)
{
  *result = 0;
  ubs_jobQ_addFirst(																										
		jobQ,																																
		ubs_file_create_rw_data(msgType, fileHandle, *data, size, result),			
		  ubs_file_write_exec_job, 
      (ubs_release_job_func_t*)ubs_file_release_rw_data_and_buf,																		
		  UBS_JOB_STATE_WANTS_TO_RUN );
  
  *data = NULL;
}

void
ubs_file_open_read_close_job (ubs_job_q_head_t* jobQ,
							  int msgType,
															 const char *file_name, 
															 void **data, 
															 long *size,
															 long *result)
{
  int file_handle = WE_FILE_OPEN(WE_MODID_UBS, file_name,
                                 WE_FILE_SET_RDONLY, 0);
  if (file_handle<1)
  {
    UBS_LOG_FILE_ERROR(ubs_file_open_read_close_job, WE_FILE_OPEN, file_handle);
    
    *result = file_handle;
    *size = 0;
    *data = NULL;
    return;
  }

  *size = WE_FILE_GETSIZE(file_name);
  if (*size<0)
  {
    UBS_LOG_FILE_ERROR(ubs_file_open_read_close_job, WE_FILE_GETSIZE, *size);
    *result = *size;
    *size = 0;
    *data = NULL;
    return;
  }
 
  if (*size == 0)
  {
    *result = TRUE;
    *data =NULL;
    return;
  }

  *data = UBS_MEM_ALLOC( *size );
  
  ubs_jobQ_addFirst(																										
	  jobQ,																																
	  ubs_file_create_rw_data( msgType, file_handle, *data, *size, result),			
	  ubs_file_read_exec_job, 
    (ubs_release_job_func_t *)ubs_file_release_rw_data_close_file,																		
	  UBS_JOB_STATE_WANTS_TO_RUN );
}

void 
ubs_file_open_rewrite_close_job (ubs_job_q_head_t* jobQ,
								 int msgType,
   														  const char *file_name, 
																void *data, 
																long size,
																long *result)
{
  int file_handle = WE_FILE_OPEN( WE_MODID_UBS, file_name,
                                  WE_FILE_SET_WRONLY| WE_FILE_SET_CREATE,
                                  size);
  if (file_handle<1)
  {
    UBS_LOG_FILE_ERROR(ubs_file_open_rewrite_close_job, WE_FILE_OPEN, file_handle);
    
    *result = file_handle;
    return;
  }

  *result = WE_FILE_SETSIZE(file_handle, size);
  if (*result != size)
  {
    UBS_LOG_FILE_ERROR(ubs_file_open_rewrite_close_job, WE_FILE_SETSIZE, *result);
    return;
  }
 
  if (*result==0)
  {
    *result = TRUE;
    return;
  }
  
  ubs_jobQ_addFirst(																										
	  jobQ,																																
	  ubs_file_create_rw_data( msgType, file_handle, data, size, result),			
	  ubs_file_write_exec_job, 
    (ubs_release_job_func_t *)ubs_file_release_rw_data_close_file,																		
	  UBS_JOB_STATE_WANTS_TO_RUN );
}

void
ubs_file_open_rewrite_mem_close_job (ubs_job_q_head_t* jobQ,
									 int msgType,
																    const char* file_name, 
																    void **data, 
																    long size,
																    long *result)
{
  int file_handle = WE_FILE_OPEN( WE_MODID_UBS, file_name,
                                  WE_FILE_SET_WRONLY| WE_FILE_SET_CREATE,
                                  size);
  if (file_handle<1)
  {
    UBS_LOG_FILE_ERROR(ubs_file_open_rewrite_mem_close_job, WE_FILE_OPEN, file_handle);

    
    *result = file_handle;
    UBS_MEM_FREE(*data);
    *data = NULL;
    return;
  }

  *result = WE_FILE_SETSIZE(file_handle, size);
  if (*result != size)
  {
    UBS_LOG_FILE_ERROR(ubs_file_open_rewrite_mem_close_job, WE_FILE_SETSIZE, *result);
    UBS_MEM_FREE(*data);
    *data = NULL;
    return;
  }
 
  

  
  ubs_jobQ_addFirst(																										
	  jobQ,																																
	  ubs_file_create_rw_data( msgType, file_handle, *data, size, result),			
	  ubs_file_write_exec_job, 
    (ubs_release_job_func_t *)ubs_file_release_rw_data_close_file_and_buf,																		
	  UBS_JOB_STATE_WANTS_TO_RUN );

  *data = NULL;
}
