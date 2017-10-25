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








#ifndef UBS_MSG_H
#define UBS_MSG_H




#ifndef UBS_MAIN_H
#include "Ubs_Main.h"
#endif

#ifndef UBS_KEYV_H
#include "Ubs_Keyv.h"
#endif





#define UBS_MSG_ITEM_HAS_DATA_FILE_MASK                0x01
#define UBS_MSG_ITEM_HAS_HIDDEN_PROPERTIES_FILE_MASK   0x02
#define UBA_MSG_ITEM_IS_TOUCED_FILE_MASK               0x04

typedef struct  {
  ubs_msg_handle_t                    handle;
  ubs_key_value_array_t               propertyArray;
  WE_UINT32                          fileId;
  WE_UINT8                           flags;
  
  
  int                                 handle_hash; 
} ubs_msg_item_t;







void 
ubs_msg_handle_init (ubs_msg_handle_t *p);


int
ubs_msg_handle_init_and_cvt(we_dcvt_t *obj, ubs_msg_handle_t *p);

void
ubs_msg_handle_release(ubs_msg_handle_t *p);

int
ubs_cvt_msg_handle (we_dcvt_t *obj, ubs_msg_handle_t *p);









#define UBS_MSG_HAS_DATA_FILE(p) \
  ((p)->flags & UBS_MSG_ITEM_HAS_DATA_FILE_MASK)


#define UBS_MSG_HAS_HIDDEN_PROPERTIES_FILE(p) \
  ((p)->flags & UBS_MSG_ITEM_HAS_HIDDEN_PROPERTIES_FILE_MASK)


#define UBS_MSG_IS_TOUCHED(p) \
  ((p)->flags & UBA_MSG_ITEM_IS_TOUCED_FILE_MASK)

#define UBS_MSG_SET_FILE_ID(p, value) \
  ((p)->fileId = (value))

#define UBS_MSG_GET_FILE_ID(p) \
  ((p)->fileId)

#define UBS_MSG_SET_HAS_DATA_FILE(p) \
  ((p)->flags |= UBS_MSG_ITEM_HAS_DATA_FILE_MASK) 

#define UBS_MSG_SET_HAS_NO_DATA_FILE(p) \
  ((p)->flags &= ~UBS_MSG_ITEM_HAS_DATA_FILE_MASK) 
  
#define UBS_MSG_SET_HAS_HIDDEN_PROPERTIES_FILE(p) \
 ((p)->flags |= UBS_MSG_ITEM_HAS_HIDDEN_PROPERTIES_FILE_MASK) 

#define UBS_MSG_TOUCH(p) \
  ((p)->flags |= UBA_MSG_ITEM_IS_TOUCED_FILE_MASK) 


#define UBS_MSG_GET_PROPERTY_ARRAY(p) \
  (&((p)->propertyArray))


#define UBS_MSG_ITEM_CHANGE_HANDLE(msg, h) \
  UBS_MSG_HANDLE_SWAP(&((msg)->handle), (h))







void
ubs_msg_init (ubs_msg_item_t *p);


int 
ubs_msg_init_and_cvt(we_dcvt_t *obj, ubs_msg_item_t *p);


void 
ubs_msg_release (ubs_msg_item_t *p);


void
ubs_msg_set_handle (ubs_msg_item_t *p, const ubs_msg_handle_t *handle);


void 
ubs_msg_change (ubs_msg_item_t *org, ubs_msg_item_t *msg);


int 
ubs_msg_hash (ubs_msg_item_t* data);


int 
ubs_msg_compare (ubs_msg_item_t* key_a, ubs_msg_item_t* key_b);

int
ubs_cvt_msg_item (we_dcvt_t *obj, ubs_msg_item_t *p);




int
ubs_cvt_msg_list_item (we_dcvt_t *obj, ubs_msg_list_item_t *p);

#endif      /*UBS_MSG_H*/
