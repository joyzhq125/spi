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
/*
 * uba_ditm.c
 *
 * Created by Klas Hermodsson, 2003/09/01.
 *
 * Revision history:
 *
 */

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "Uba_Ditm.h"

#include "Uba_Main.h"
#include "We_Cmmn.h"
#include "Uba_Data.h"

/************************************************************************/
/* Local typedefs                                                       */
/************************************************************************/

/************************************************************************/
/* Global varaiables                                                     */
/************************************************************************/

/************************************************************************/
/* Forward declarations local functions                                 */
/************************************************************************/

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/*
 *  Initiates a data item.
 *  NOTE: The data item will take over the handle and title list memory 
 *  of the msg and set the pointers in question to NULL.
 */
void
uba_data_item_init (uba_data_item_t *p, int msg_type, ubs_msg_list_item_t *msg)
{
  p->next = NULL;
  p->msg_type = msg_type;
  p->msg = *msg;
  p->folder_index = UBA_NO_FOLDER_ID;
  p->folder_item = NULL;

  /* the memory of msg is taken over by this data item */
  msg->handle.handle = NULL;
  msg->handle.handleLen = 0;
  msg->titleList = NULL;
  msg->titleListSize = 0;

  p->hash = uba_data_item_util_calc_hash (&(p->msg.handle));
}


/*
 *	Releases the memory for the data item.
 */
void
uba_data_item_release (uba_data_item_t *p)
{
  int i;

  UBA_MEM_FREE (p->msg.handle.handle);

  for (i = 0; i < p->msg.titleListSize; ++i) {
    UBA_MEM_FREE (p->msg.titleList[i].value);
  }
  UBA_MEM_FREE (p->msg.titleList);
}


/*
 *  Gets the next data item in the list.
 */
uba_data_item_t*
uba_data_item_get_next (const uba_data_item_t *p)
{
  return p->next;
}


/*
 *  Checks if the data item's handle in conjunction with msg type is the same 
 *  as the other parameters.
 *  The msg type, hash and handle must all be the same (by value) for the
 *  the match to be correct.
 *  Returns TRUE if the handle matches, else FALSE.
 */
int
uba_data_item_has_same_handle (const uba_data_item_t *p, 
                               WE_UINT32 hash,
                               int msg_type, 
                               const ubs_msg_handle_t *handle)
{
  if (p->msg_type != msg_type) {
    return FALSE;
  }

  if (p->hash != hash) {
    return FALSE;
  }

  if (p->msg.handle.handleLen != handle->handleLen) {
    return FALSE;
  }

  return ((memcmp(p->msg.handle.handle, handle->handle, handle->handleLen) == 0) 
          ? (TRUE) : (FALSE));
}


/*
 *  Returns a string representation of the handle in hex form.
 *  Example: a handle of {0x00, 0x1a, 0x2d, 0x34} will be 
 *  returned as "001a2d34" (null terminated).
 */
char*
uba_data_item_get_handle_as_hex_str (const uba_data_item_t *p)
{
  char  *str = (char*) UBA_MEM_ALLOC (p->msg.handle.handleLen * 2 + 1);
  int   i;

  for (i = 0; i < p->msg.handle.handleLen; ++i) {
    we_cmmn_byte2hex (p->msg.handle.handle[i], &(str[i*2]));
  }
  /* null terminate str */
  str[p->msg.handle.handleLen * 2] = 0;

  return str;
}


/*
 *  Returns the UBS_MSG_KEY_READ property value (TRUE/FALSE). If there is no such
 *  property FALSE is returned.
 */
int
uba_data_item_is_unread (uba_data_item_t *p)
{
  ubs_key_value_t   *prop;
  WE_BOOL          read_val;
  we_dcvt_t        cvt_obj;

  if (!p->msg.titleList) {
    return FALSE;
  }

  prop = uba_data_util_lookup_msg_property (&(p->msg), UBS_MSG_KEY_READ);

  if (!prop || (prop->valueType != UBS_VALUE_TYPE_BOOL)) {
    return FALSE;
  }

  we_dcvt_init (&cvt_obj, WE_DCVT_DECODE, prop->value, prop->valueLen, WE_MODID_UBA);
  if (!we_dcvt_bool (&cvt_obj, &read_val)) {
    return FALSE;
  }

  return !read_val;
}


/************************************************************************/
/* Util functions                                                       */
/************************************************************************/

/*
 *  Returns a 32 bit hash value from a handle
 */
WE_UINT32
uba_data_item_util_calc_hash (const ubs_msg_handle_t *handle)
{
  return we_cmmn_strhash ((char*)(handle->handle), handle->handleLen);
}

/************************************************************************/
/* Local functions                                                      */
/************************************************************************/



