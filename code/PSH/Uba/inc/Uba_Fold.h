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
 * uba_fold.h
 *
 * This file contains the definition and functions that operate on a
 * uba_folder_t.
 *
 * A folder contains a list of folder items (see uba_fitm.h).
 *
 * A folder is the data part of a folder in the GUI of the UBA. 
 * Therefore any change to the folder or its contents must be signaled to the GUI. 
 * This is done by calling the uba_rce_on_data_event function (see uba_rce.h).
 *
 * Created by Martin Andersson, 2003/08/25.
 *
 * Revision history:
 *
 */
/*
 *	Handel a list of folder names and lists with msgs
 */

#ifndef UBA_FOLD_H
#define UBA_FOLD_H

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef UBA_CFG_H
  #include "Uba_Cfg.h"
#endif

#ifndef UBA_FITM_H
  #include "Uba_Fitm.h"
#endif

#ifndef UBS_IF_H
  #include "Ubs_If.h"
#endif

#ifndef UBA_DITM_H
#include "Uba_Ditm.h"
#endif

/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/

typedef int uba_folder_sort_func_t(uba_data_item_t* a, uba_data_item_t* b);

/*
 *  The definition of a folder
 */
typedef struct {
  uba_folder_id_t         folder_id;    /* the index or wid of the folder */
  uba_folder_item_t      *items;        /* the list of folder items */
  unsigned                count;        /* the number of items in the list */
  unsigned                unread_count; /* the number of unread items in the list */
  char                   *custom_name;  /* a user given name if any */
  uba_folder_sort_func_t *sort_cmp;     /* Used for sorting and comparing */
}uba_folder_t;

/*
 *  The enumeration of sort orders. These are used as a parameter in the 
 *  UBA_ACTION_SORT_FOLDER action.
 *  One of these sort orders must be the default.
 *  
 *  NOTE! If a sort order is added, then code for performing this sort
 *  must also be added to the function uba_folder_sort.
 */
typedef enum {
  UBA_FOLDER_SORT_DEFAULT  = 0,     /* do not change this entry */
  UBA_FOLDER_SORT_TIME_STAMP = UBA_FOLDER_SORT_DEFAULT, 
  /* set the default sort order above by enumerating it and set its value to UBA_FOLDER_SORT_DEFAULT */
  UBA_FOLDER_SORT_TYPE,
  UBA_FOLDER_SORT_FROM,
  UBA_FOLDER_SORT_TO
  /* add non default sort orders above this line */
} uba_folder_sort_order_t;


/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/*
 *  Initiates a uba_folder_t struct.
 */
void
uba_folder_init (uba_folder_t *p, uba_folder_id_t folder_id);

/*
 *  Releases the memory of a uba_folder_t struct.
 *  This msans releasing each of the folder items in the list.
 */
void
uba_folder_release (uba_folder_t *p);

/*
 *  Returns the first folder item in the list.
 */
uba_folder_item_t*
uba_folder_get_first_item (uba_folder_t *p);

/*
 *  Returns the number of folder items in the list.
 */
unsigned
uba_folder_get_nbr_of_items (uba_folder_t *p);

/*
 *  Returns the number of unread folder items in the list.
 */
unsigned
uba_folder_get_nbr_of_unread_items (uba_folder_t *p);

/*
 *  Returns the custom name string, NULL if no such name exists.
 */
const char*
uba_folder_get_custom_name (const uba_folder_t *p);

/*
 *  Sets the custom name string. A copy is taken of the name string.
 *  If save_to_registry == TRUE then this new name will be stored in the
 *  registry.
 */
void
uba_folder_set_custom_name (uba_folder_t *p, const char *name, int save_to_registry);

/*
 *  Returns the string resource identifier for the folder name of this folder
 */
unsigned long
uba_folder_get_name_str_rc_id (const uba_folder_t *p);

/* 
 * Get the internal folder name for the folder
 */
const char*
uba_folder_get_ubs_folder_name(const uba_folder_t *p);

/*
 *  Adds a data item to the list by creating a new folder item and inserts that
 *  folder item into the list.
 *  This functions calls uba_rce_on_data_event of type UBA_DATA_EVENT_ITEM_ADDED.
 */
void
uba_folder_add (uba_folder_t *p, uba_data_item_t *item);

/*
 *  Removes and releases the folder item if found in the list of items.
 *  This functions calls uba_rce_on_data_event of type UBA_DATA_EVENT_ITEM_DELETED.
 */
void
uba_folder_delete (uba_folder_t *p, uba_folder_item_t *item);

/*
 *  Call this function when a folder item has changed. Currently this results
 *  in a recalculation of number of unread items.
 */
void
uba_folder_on_folder_item_changed (uba_folder_t *p);

void
uba_folder_sort(uba_folder_t *p, uba_folder_sort_order_t sort);

#endif      /*UBA_FOLD_H*/

