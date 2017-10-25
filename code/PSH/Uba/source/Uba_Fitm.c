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
 * uba_fitm.c
 *
 * Created by Klas Hermodsson, 2003/08/29.
 *
 * Revision history:
 *
 */

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "Uba_Fitm.h"

#include "Uba_Rce.h"
#include "Uba_Data.h"

/************************************************************************/
/* Local typedefs                                                       */
/************************************************************************/

/************************************************************************/
/* Global variables                                                     */
/************************************************************************/

/************************************************************************/
/* Forward declarations local functions                                 */
/************************************************************************/

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/*
 *  Initiates a folder item.
 *  NOTE: The data item's folder_item member will be changed and set to p.
 *  This functions calls uba_rce_on_data_event of type UBA_DATA_EVENT_NEW.
 */
void
uba_folder_item_init (uba_folder_item_t *p, uba_data_item_t *item)
{
  uba_data_obj_t  data_obj;

  item->folder_item = p;
  p->item = item;
  p->next = NULL;

  data_obj.type = UBA_DATA_OBJECT_MSG;
  data_obj.data.folder_item = p;

  uba_rce_on_data_event (&data_obj, UBA_DATA_EVENT_NEW);
}


/*
 *  Releases the memory of a folder_item.
 *  This functions calls uba_rce_on_data_event of type UBA_DATA_EVENT_DELETE.
 */
void
uba_folder_item_release (uba_folder_item_t *p)
{
  uba_data_obj_t  data_obj;

  data_obj.type = UBA_DATA_OBJECT_MSG;
  data_obj.data.folder_item = p;

  uba_rce_on_data_event (&data_obj, UBA_DATA_EVENT_DELETE);
}


/*
 *  Returns the next folder item in the list.
 */
uba_folder_item_t*
uba_folder_item_get_next (uba_folder_item_t *p)
{
  return p->next;
}


/*
 *  Call this function when the data item that p points to changes so
 *  the folder item can send a change event to the RCE (resource engine, see uba_rce.h).
 *  This functions calls uba_rce_on_data_event of type UBA_DATA_EVENT_CHANGE.
 */
void
uba_folder_item_on_data_changed (uba_folder_item_t *p)
{
  uba_data_obj_t  data_obj;

  data_obj.type = UBA_DATA_OBJECT_MSG;
  data_obj.data.folder_item = p;

  uba_rce_on_data_event (&data_obj, UBA_DATA_EVENT_CHANGE);
}


/************************************************************************/
/* Local functions                                                      */
/************************************************************************/

