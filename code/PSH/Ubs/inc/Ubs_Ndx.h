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













#ifndef UBS_NDX_H
#define UBS_NDX_H

#ifndef UBS_MSG_H
  #include "Ubs_Msg.h"
#endif

#ifndef UBS_JOB_H
  #include "Ubs_Job.h"
#endif





 



#define UBS_NDX_PADDING     0x00

#define UBS_NDX_NEW_MSG     0x01

#define UBS_NDX_DEL_MSG     0x02

#define UBS_NDX_CHANGE_MSG  0x03





#define UBS_NDX_INIT_DELETE(item, handle)   \
  (item)->header = UBS_NDX_DEL_MSG;         \
  (item)->data.del_msg = (handle)

#define UBS_NDX_INIT_NEW(item, msg)   \
  (item)->header = UBS_NDX_NEW_MSG;   \
  (item)->data.new_msg = (msg)

#define UBS_NDX_INIT_CHANGE(item, msg)  \
  (item)->header = UBS_NDX_CHANGE_MSG;  \
  (item)->data.change_msg = (msg)





typedef struct {
  WE_UINT8  header;
  union {
    ubs_msg_item_t* new_msg;
    ubs_msg_item_t* change_msg;
    ubs_msg_handle_t* del_msg;
  }data;
} ubs_ndx_item_t;





void 
ubs_ndx_item_init (ubs_ndx_item_t *p);

int
ubs_ndx_item_init_and_cvt (we_dcvt_t *obj, ubs_ndx_item_t *p);

int 
ubs_cvt_ndx_item (we_dcvt_t *obj, ubs_ndx_item_t *p);








void 
ubs_ndx_write_item_job (ubs_job_q_head_t* queue, int msgType, int file_id, ubs_ndx_item_t *item, 
                        long* result);



#endif      /*UBS_NDX_H*/
