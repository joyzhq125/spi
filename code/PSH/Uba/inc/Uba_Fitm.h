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
 * uba_fitm.h
 * 
 * This file contains the definition and functions that operate on a 
 * uba_folder_item_t.
 * 
 * A folder item is a list element in a folder that references a data item.
 * It does not own this data item and by releasing a folder item its data item is NOT
 * released.
 * 
 * A folder item is the data part of a message in a list in the GUI of the UBA. 
 * Therefore any change to the folder item or its data item must be signaled to the GUI. 
 * This is done by calling the uba_rce_on_data_event function (see uba_rce.h).
 * 
 *
 * Created by Klas Hermodsson, 2003/08/29.
 *
 * Revision history:
 *
 */
/*
 *	Handel a list of folder names and lists with msgs
 */

#ifndef UBA_FITM_H
#define UBA_FITM_H

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef UBS_IF_H
  #include "Ubs_If.h"
#endif

#ifndef UBA_DITM_H
  #include "Uba_Ditm.h"
#endif

/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/

/* The definition of a folder item */
typedef struct uba_folder_item_st
{
  uba_data_item_t             *item;  /* a reference to a data item (see uba_ditm.h) */
  struct uba_folder_item_st   *next;  /* The next folder item in the list */
}uba_folder_item_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/*
 *  Initiates a folder item.
 *  NOTE: The data item's folder_item member will be changed and set to p.
 *  This functions calls uba_rce_on_data_event of type UBA_DATA_EVENT_NEW.
 */
void
uba_folder_item_init (uba_folder_item_t *p, uba_data_item_t *item);

/*
 *  Releases the memory of a folder_item.
 *  This functions calls uba_rce_on_data_event of type UBA_DATA_EVENT_DELETE.
 */
void
uba_folder_item_release (uba_folder_item_t *p);

/*
 *  Returns the next folder item in the list.
 */
uba_folder_item_t*
uba_folder_item_get_next (uba_folder_item_t *p);

/*
 *  Call this function when the data item that p points to changes so
 *  the folder item can send a change event to the RCE (resource engine, see uba_rce.h).
 *  This functions calls uba_rce_on_data_event of type UBA_DATA_EVENT_CHANGE.
 */
void
uba_folder_item_on_data_changed (uba_folder_item_t *p);

#endif      /*UBA_FITM_H*/
