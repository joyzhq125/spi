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













#include "Ubs_Ndx.h"
#include "We_Log.h"





void 
ubs_ndx_item_init(ubs_ndx_item_t *p)
{
  p->header = UBS_NDX_PADDING;
  p->data.change_msg = NULL;
}


int
ubs_ndx_item_init_and_cvt(we_dcvt_t *obj, ubs_ndx_item_t *p)
{
  ubs_ndx_item_init (p);
  
  if (!we_dcvt_uint8(obj,&p->header)){
    return FALSE;
  }
  
  switch (p->header){
    case UBS_NDX_PADDING:
      return TRUE;
    case UBS_NDX_NEW_MSG:
    case UBS_NDX_CHANGE_MSG:
      return we_dcvt_array 
              (obj, 
               sizeof(ubs_msg_item_t), 
               1, 
              (void**)&p->data.change_msg, 
              (we_dcvt_element_t *)ubs_msg_init_and_cvt );            
    case UBS_NDX_DEL_MSG:
      return we_dcvt_array 
              (obj, 
              sizeof(ubs_msg_handle_t), 
              1, 
              (void**)&p->data.del_msg, 
              (we_dcvt_element_t*)ubs_msg_handle_init_and_cvt);
    default:
      return FALSE;
  }
}


int 
ubs_cvt_ndx_item (we_dcvt_t *obj, ubs_ndx_item_t *p)
{
  
  if (!we_dcvt_uint8(obj,&p->header)){
    return FALSE;
  }
  
  switch (p->header){
    case UBS_NDX_PADDING:
      return TRUE;
    case UBS_NDX_NEW_MSG:
    case UBS_NDX_CHANGE_MSG:
      return we_dcvt_array 
                (obj, 
                sizeof(ubs_msg_t), 
                1, 
                (void**)&p->data.change_msg, 
                (we_dcvt_element_t*)ubs_cvt_msg_item); 
    case UBS_NDX_DEL_MSG:
      return we_dcvt_array 
                (obj, 
                sizeof(ubs_msg_handle_t), 
                1,
                (void**)&p->data.del_msg, 
                (we_dcvt_element_t*)ubs_cvt_msg_handle);
    default:
      return FALSE;
  }
}








void
ubs_ndx_write_item_job(ubs_job_q_head_t* queue, int msgType, int file_id, ubs_ndx_item_t *item, 
                       long* result)
{

  we_dcvt_t                cvt_obj;
  long                      dataLen = 0;
  void                      *data = NULL;

  
  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  *result = ubs_cvt_ndx_item (&cvt_obj,item);
  if (!*result){
    WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,                 
      "UBS: calc buffer size cvt failed [ubs_ndx_write_item_job]\n")); 
    return;
  }
  
  dataLen = cvt_obj.pos;
  data = UBS_MEM_ALLOC (dataLen);

  if (data) {
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, data, dataLen, WE_MODID_UBS);
    *result = ubs_cvt_ndx_item(&cvt_obj,item);
    
    if (!*result){
      WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBS,                 
                  "UBS: cvt failed [ubs_ndx_write_item_job]\n")); 
      UBS_MEM_FREE(data);
      return;
    }

    ubs_file_write_mem_job (queue, msgType, file_id, &data, dataLen, result);
  }
  else {
    *result = FALSE;
    return;
  }
}

