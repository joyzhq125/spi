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
 * uba_fold.c
 *
 * Created by Martin Andersson, 2003/08/25.
 *
 * Revision history:
 *
 */

#include "Uba_Fold.h"

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "Uba_Main.h"
#include "Uba_Data.h"
#include "Uba_Cfg.h"
#include "Uba_Rce.h"
#include "Uba_Rc.h"

#include "We_Cmmn.h"

/************************************************************************/
/* constants                                                            */
/************************************************************************/


#define UBA_FOLDER_MAX_REG_KEY_LEN  20


/************************************************************************/
/* Typedefs                                                            */
/************************************************************************/
typedef struct  {
  WE_UINT32   folder_str_id;
  const char*  ubs_folder_name;
}uba_folder_name_t;


/************************************************************************/
/* Global variables                                                     */
/************************************************************************/

/*
 *  An array of string resource identifiers. By using the folder index as the 
 *  and index in the array, the string resource containing the folder's 
 *  name to be used in the GUI is retrieved.
 */
static const uba_folder_name_t uba_folder_names[] = UBA_FOLDER_NAMES;

static const char*              uba_folder_name_registry_path = "/UBA/folders";

/************************************************************************/
/* Forward declarations local functions                                 */
/************************************************************************/

static void
uba_folder_set_unread_count (uba_folder_t *p, unsigned val);

static void
uba_folder_recalc_unread_count (uba_folder_t *p);


static uba_folder_sort_func_t*
uba_folder_lookup_sort_func(uba_folder_sort_order_t sort_order);

static int
uba_folder_sort_cmp_time_stap(uba_data_item_t* a, uba_data_item_t* b);

static void
uba_folder_do_sort(uba_folder_t *p);

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/*
 *  Initiates a uba_folder_t struct.
 */
void
uba_folder_init (uba_folder_t *p, uba_folder_id_t folder_id)
{
  char  key[UBA_FOLDER_MAX_REG_KEY_LEN];

  p->folder_id = folder_id;
  p->items = NULL;
  p->count = 0;
  p->unread_count = 0;
  p->custom_name = NULL;
  p->sort_cmp = uba_folder_lookup_sort_func (UBA_FOLDER_SORT_DEFAULT);

  /* init of a folder results in a check in the registry to see if a custom name exists */
  sprintf(key, "%i", p->folder_id + 1); /* +1 because 0 is reserved */
  WE_REGISTRY_GET (WE_MODID_UBA, p->folder_id + 1, (char*)uba_folder_name_registry_path, key);
  /* the result of this query is delivered asynchronously to ubs_data.c where it will use
  uba_folder_set_custom_name to deliver it to this uba_folder_t */
}

/*
 *  Releases the memory of a uba_folder_t struct.
 *  This msans releasing each of the folder items in the list.
 */
void
uba_folder_release (uba_folder_t *p)
{
  uba_folder_item_t   *item;

  while (p->items) {
    item = p->items;
    p->items = p->items->next;
    uba_folder_item_release (item);
    UBA_MEM_FREE (item);
  }

  UBA_MEM_FREE (p->custom_name);
}


/*
 *  Returns the first folder item in the list.
 */
uba_folder_item_t*
uba_folder_get_first_item (uba_folder_t *p)
{
  return p->items;
}


/*
 *  Returns the number of folder items in the list.
 */
unsigned
uba_folder_get_nbr_of_items (uba_folder_t *p)
{
  return p->count;
}


/*
 *  Returns the number of unread folder items in the list.
 */
unsigned
uba_folder_get_nbr_of_unread_items (uba_folder_t *p)
{
  return p->unread_count;
}


/*
 *  Returns the custom name string, NULL if no such name exists.
 */
const char*
uba_folder_get_custom_name (const uba_folder_t *p)
{
  return p->custom_name;
}


/*
 *  Sets the custom name string. A copy is taken of the name string.
 *  If save_to_registry == TRUE then this new name will be stored in the
 *  registry.
 */
void
uba_folder_set_custom_name (uba_folder_t *p, const char *name, int save_to_registry)
{
  int               len;
  uba_data_obj_t    data_obj;

  if (p->custom_name) {
    UBA_MEM_FREE (p->custom_name);
    p->custom_name = NULL;
  }

  len = (name) ? (int)(strlen (name)) : (0);
  
  if (!len) {
    /* an empty name doesn't change the current name */
    return;
  }

  p->custom_name = we_cmmn_strndup (WE_MODID_UBA, name, len);

  data_obj.type = UBA_DATA_OBJECT_FOLDER;
  data_obj.data.folder = p;
  uba_rce_on_data_event (&data_obj, UBA_DATA_EVENT_CHANGE);

  /* If the custom name is to be saved it is written to a place in the registry */

  if (save_to_registry) {
    char    key[UBA_FOLDER_MAX_REG_KEY_LEN];

    sprintf(key, "%i", p->folder_id + 1); /* +1 because 0 is reserved */

    {
      void    *handle = WE_REGISTRY_SET_INIT (WE_MODID_UBA);

      WE_REGISTRY_SET_PATH (handle, (char*)uba_folder_name_registry_path);
      WE_REGISTRY_SET_ADD_KEY_STR (handle, key, (unsigned char*)(p->custom_name), 
                                    (WE_UINT16)(len + 1));
      WE_REGISTRY_SET_COMMIT (handle);
    }
  }
}


/*
 *  Returns the string resource identifier for the folder name of this folder
 */
unsigned long
uba_folder_get_name_str_rc_id (const uba_folder_t *p)
{
  return uba_folder_names[p->folder_id].folder_str_id;
}

/* 
 * Get the internal folder name for the folder, 
 * Returns NULL if no internal folder name exists
 */
const char*
uba_folder_get_ubs_folder_name(const uba_folder_t *p)
{
  return uba_folder_names[p->folder_id].ubs_folder_name;
}



/*
 *  Adds a data item to the list by creating a new folder item and inserts that
 *  folder item into the list.
 *  This functions calls uba_rce_on_data_event of type UBA_DATA_EVENT_ITEM_ADDED.
 */
void
uba_folder_add (uba_folder_t *p, uba_data_item_t *item)
{
  uba_folder_item_t *folder_item;
  uba_data_obj_t    data_obj;

  folder_item = UBA_MEM_ALLOCTYPE (uba_folder_item_t);
  uba_folder_item_init (folder_item, item);
  folder_item->next = p->items;
  p->items = folder_item;
  
  (p->count)++;

  uba_folder_do_sort (p); 

  if (uba_data_item_is_unread (item)) {
    uba_folder_set_unread_count (p, p->unread_count + 1);
  }

  data_obj.type = UBA_DATA_OBJECT_FOLDER;
  data_obj.data.folder = p;
  uba_rce_on_data_event (&data_obj, UBA_DATA_EVENT_ITEM_ADDED);
}


/*
 *  Removes and releases the folder item if found in the list of items.
 *  This functions calls uba_rce_on_data_event of type UBA_DATA_EVENT_ITEM_DELETED.
 */
void
uba_folder_delete (uba_folder_t *p, uba_folder_item_t *item)
{
  uba_folder_item_t *e = p->items;
  uba_data_obj_t    data_obj;

  if (!item) {
    return;
  }

  data_obj.type = UBA_DATA_OBJECT_FOLDER;
  data_obj.data.folder = p;

  if (e == item) {
    p->items = e->next;
    (p->count)--;
    uba_rce_on_data_event (&data_obj, UBA_DATA_EVENT_ITEM_DELETED);
    uba_folder_item_release (item);
    UBA_MEM_FREE (item);

    uba_folder_recalc_unread_count (p);
    return;
  }

  while (e && (e->next != item)) {
    e = e->next;
  }

  if (e) {
    e->next = item->next;
    (p->count)--;
    uba_rce_on_data_event (&data_obj, UBA_DATA_EVENT_ITEM_DELETED);
    uba_folder_item_release (item);
    UBA_MEM_FREE (item);

    uba_folder_recalc_unread_count (p);
  }
}


/*
 *  Call this function when a folder item has changed. Currently this results
 *  in a recalculation of number of unread items.
 */
void
uba_folder_on_folder_item_changed (uba_folder_t *p)
{
  uba_folder_recalc_unread_count (p);
  uba_folder_do_sort (p);
  
}

void
uba_folder_sort(uba_folder_t *p, uba_folder_sort_order_t sort)
{
   uba_data_obj_t    data_obj;    
   data_obj.type = UBA_DATA_OBJECT_FOLDER;
   data_obj.data.folder = p;

  p->sort_cmp = uba_folder_lookup_sort_func (sort);
  uba_folder_do_sort (p);
  uba_rce_on_data_event (&data_obj, UBA_DATA_EVENT_CHANGE);
}


/************************************************************************/
/* Local functions                                                      */
/************************************************************************/

static int
uba_folder_sort_cmp_type(uba_data_item_t* a, uba_data_item_t* b)
{
  return a->msg_type - b->msg_type;
}
static void
uba_folder_recalc_unread_count (uba_folder_t *p)
{
  unsigned  new_unread_count = 0;
  uba_folder_item_t *e;

  e = p->items;
  while (e) {
    if (uba_data_item_is_unread (e->item)) {
      new_unread_count++;
    }
    e = e->next;
  }

  uba_folder_set_unread_count (p, new_unread_count);
}

static int
uba_folder_sort_no_sort(uba_data_item_t* a, uba_data_item_t* b)
{
  UNREFERENCED_PARAMETER(a);
  UNREFERENCED_PARAMETER(b);
  return 0;
}

static int
uba_folder_sort_cmp_from(uba_data_item_t* a, uba_data_item_t* b)
{
  ubs_key_value_t* akvc = uba_data_util_lookup_msg_property(&a->msg,UBS_MSG_KEY_FROM);
  ubs_key_value_t* bkvc = uba_data_util_lookup_msg_property(&b->msg,UBS_MSG_KEY_FROM);


  if (!akvc && !bkvc){
    return 0;
  }

  if (!akvc){
    return 1;
  }

  if (!bkvc){
    return -1;
  }

  return uba_data_util_compare_key_value(akvc, bkvc);
}

static int
uba_folder_sort_cmp_to(uba_data_item_t* a, uba_data_item_t* b)
{
  ubs_key_value_t* akvc = uba_data_util_lookup_msg_property(&a->msg,UBS_MSG_KEY_TO);
  ubs_key_value_t* bkvc = uba_data_util_lookup_msg_property(&b->msg,UBS_MSG_KEY_TO);


  if (!akvc && !bkvc){
    return 0;
  }

  if (!akvc){
    return 1;
  }

  if (!bkvc){
    return -1;
  }

  return uba_data_util_compare_key_value(akvc, bkvc);
}

static uba_folder_sort_func_t*
uba_folder_lookup_sort_func(uba_folder_sort_order_t sort_order)
{
  switch (sort_order)
  {
  case UBA_FOLDER_SORT_TIME_STAMP:
    return &uba_folder_sort_cmp_time_stap;
  
  case UBA_FOLDER_SORT_TYPE:
    return &uba_folder_sort_cmp_type;
  
  case UBA_FOLDER_SORT_FROM:
    return &uba_folder_sort_cmp_from;
  
  case UBA_FOLDER_SORT_TO:
    return &uba_folder_sort_cmp_to;
  
  default:
    return &uba_folder_sort_no_sort;
  };
}

static void
uba_folder_set_unread_count (uba_folder_t *p, unsigned val)
{
  if (p->unread_count != val) {
    uba_data_obj_t    data_obj;
    
    p->unread_count = val;

    data_obj.type = UBA_DATA_OBJECT_FOLDER;
    data_obj.data.folder = p;
    uba_rce_on_data_event (&data_obj, UBA_DATA_EVENT_CHANGE);
  }
}



static int
uba_folder_sort_cmp_time_stap(uba_data_item_t* a, uba_data_item_t* b)
{
  ubs_key_value_t* akvc = uba_data_util_lookup_msg_property(&a->msg,UBS_MSG_KEY_TIMESTAMP);
  ubs_key_value_t* bkvc = uba_data_util_lookup_msg_property(&b->msg,UBS_MSG_KEY_TIMESTAMP);


  if (!akvc && !bkvc){
    return 0;
  }

  if (!akvc){
    return 1;
  }

  if (!bkvc){
    return -1;
  }

  return -uba_data_util_compare_key_value(akvc, bkvc);
}



static void
uba_folder_do_sort( uba_folder_t *p)
{
  uba_folder_item_t* it  = p->items;
  uba_folder_item_t* next;
  uba_folder_item_t* prev;
  int               element_swapped = TRUE;
  int               element_count;
  int               i;
  int               j;
  
  
  if (!it || !it->next )
  {
    /* empty list or one element only */
    return;
  }
  
  /* find end ptr */
  for(element_count = 1;it->next; it = it->next, ++element_count){
    /* empty */
  }

  
  for (i = 1; (i < element_count ) && element_swapped; ++i){
    prev = NULL;
    element_swapped = FALSE;
    it = p->items;
    /* One pass */
    for(j = element_count; j>i; --j){
      next = it->next;
      if (p->sort_cmp(it->item, next->item)>0){
        /* swap */
        if (prev){
          prev->next = next;
        }else {
          p->items = next;
        }
        it->next   = next->next;
        next->next = it;
        prev = next;
        element_swapped = TRUE;          
      } 
      else {
        /* Advance one step */
        prev = it;
        it = next;
      }
    };
  };
  
}
