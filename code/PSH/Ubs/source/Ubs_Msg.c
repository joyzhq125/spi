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









#include "Ubs_Msg.h"
#include "We_Cmmn.h"






#define UBS_MSG_GET_HANDLE(msg) \
  (&(msg)->handle)






void 
ubs_msg_handle_init (ubs_msg_handle_t *p)
{
  p->handleLen = 0;
  p->handle = NULL;
 
}

int
ubs_msg_handle_init_and_cvt(we_dcvt_t *obj, ubs_msg_handle_t *p)
{
  ubs_msg_handle_init (p);
  return ubs_cvt_msg_handle (obj,p);
}

void
ubs_msg_handle_release (ubs_msg_handle_t *p)
{
  UBS_MEM_FREE (p->handle);
}








void
ubs_msg_init (ubs_msg_item_t *p)
{
  
  p->fileId = 0;
  p->flags = 0;
  p->handle_hash = 0;
  ubs_msg_handle_init (&p->handle);
  ubs_key_value_array_init (UBS_MSG_GET_PROPERTY_ARRAY(p));
}

int 
ubs_msg_init_and_cvt(we_dcvt_t *obj, ubs_msg_item_t *p)
{
  ubs_msg_init(p);
  return ubs_cvt_msg_item (obj,p);
}


int 
ubs_cvt_msg_item (we_dcvt_t* obj, ubs_msg_item_t *p)
{
  if (!ubs_cvt_msg_handle (obj, &(p->handle)) ||
      !ubs_cvt_key_value_array (&(p->propertyArray),obj) ||
      !we_dcvt_uintvar (obj, &(p->fileId))  ||
      !we_dcvt_uint8 (obj,&(p->flags))){
    return FALSE;
  }
  return TRUE;
  
}

void 
ubs_msg_release (ubs_msg_item_t *p)
{
  we_dcvt_t obj;
  we_dcvt_init (&obj, WE_DCVT_FREE, NULL, 0, WE_MODID_UBS);
  ubs_cvt_msg_item (&obj, p);
}



void
ubs_msg_set_handle (ubs_msg_item_t *p, const ubs_msg_handle_t *handle)
{
  ubs_msg_handle_release (&(p->handle));
  p->handle.handleLen = handle->handleLen;
  p->handle.handle = UBS_MEM_ALLOC (p->handle.handleLen);
  memcpy (p->handle.handle, handle->handle, p->handle.handleLen);
}


void 
ubs_msg_change (ubs_msg_item_t *org, ubs_msg_item_t *msg)
{
  
  UBS_MSG_TOUCH(msg);
  org->flags  = msg->flags;
  org->fileId = msg->fileId;
  
  
  ubs_key_value_update (UBS_MSG_GET_PROPERTY_ARRAY(org), 
                        UBS_MSG_GET_PROPERTY_ARRAY(msg));        
}

int 
ubs_msg_hash (ubs_msg_item_t* data)
{
  if (data->handle_hash == 0) {
    data->handle_hash = (int) we_cmmn_strhash ((char*)data->handle.handle, 
                                                data->handle.handleLen);
  }
  
  return data->handle_hash;
}


int 
ubs_msg_compare (ubs_msg_item_t* key_a, ubs_msg_item_t* key_b)
{
  ubs_msg_handle_t *h1;
  ubs_msg_handle_t *h2;
  if (ubs_msg_hash (key_a)!= ubs_msg_hash(key_b)){
    
    return FALSE;
  }
  
  h1 = UBS_MSG_GET_HANDLE (key_a);
  h2 = UBS_MSG_GET_HANDLE (key_b);

  return (h1->handleLen == h2->handleLen) && 
    (0 == memcmp (h1->handle, h2->handle, h1->handleLen));
}

