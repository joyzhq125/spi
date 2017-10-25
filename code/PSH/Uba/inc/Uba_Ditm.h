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
 * uba_ditm.h
 * 
 * This file contains the definition and functions that operate on a
 * uba_data_itemt.
 *
 * A data item is a container for a UBS message list item.
 *
 *
 * Created by Klas Hermodsson, 2003/09/01.
 *
 * Revision history:
 *
 */

#ifndef UBA_DITM_H
#define UBA_DITM_H

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef UBA_CFG_H
  #include "Uba_Cfg.h"
#endif

#ifndef UBS_IF_H
  #include "Ubs_If.h"
#endif

/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/

/* forward declaration needed due to cross referencing */
struct uba_folder_item_st;

/* the data item definition */
typedef struct uba_data_item_st
{
  WE_UINT32                hash;         /* a hash calculated from the message handle */
  int                       msg_type;     /* the msg type of the message */
  ubs_msg_list_item_t       msg;          /* the actual message as received from UBS */
  uba_folder_id_t           folder_index; /* the folder where the message belongs */
  struct uba_folder_item_st *folder_item; /* the folder item which points to this data item */
  struct uba_data_item_st   *next;        /* next data item in the list */
}uba_data_item_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/*
 *  Initiates a data item.
 *  NOTE: The data item will take over the handle and title list memory 
 *  of the msg and set the pointers in question to NULL.
 */
void
uba_data_item_init (uba_data_item_t *p, int msg_type, ubs_msg_list_item_t *msg);

/*
 *	Releases the memory for the data item.
 */
void
uba_data_item_release (uba_data_item_t *p);

/*
 *  Gets the next data item in the list.
 */
uba_data_item_t*
uba_data_item_get_next (const uba_data_item_t *p);

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
                               const ubs_msg_handle_t *handle);

/*
 *  Returns a string representation of the handle in hex form.
 *  Example: a handle of {0x00, 0x1a, 0x2d, 0x34} will be 
 *  returned as "001a2d34" (null terminated).
 */
char*
uba_data_item_get_handle_as_hex_str (const uba_data_item_t *p);

/*
 *  Returns the UBS_MSG_KEY_READ property value (TRUE/FALSE). If there is no such
 *  property FALSE is returned.
 */
int
uba_data_item_is_unread (uba_data_item_t *p);

/************************************************************************/
/* Util functions                                                       */
/************************************************************************/

/*
 *  Returns a 32 bit hash value from a handle
 */
WE_UINT32
uba_data_item_util_calc_hash (const ubs_msg_handle_t *handle);

#endif      /*UBA_DITM_H*/
