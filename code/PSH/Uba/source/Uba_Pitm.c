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
 * uba_pitm.c
 *
 * Created by Martin Andersson, 2003/08/25.
 *
 * Revision history:
 *
 */

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "Uba_Pitm.h"

#include "Uba_Main.h"
#include "Uba_Cfg.h"


#include "We_Wid.h"
/************************************************************************/
/* Typefs                                                               */
/************************************************************************/




/************************************************************************/
/* Local variables                                                      */
/************************************************************************/ 

/* The lookup map to define icons and menus for msgs */
extern const uba_page_item_icon_menu_t uba_page_item_icon_menu_map[];
/* The criteria array used to define one search criteria */
extern const uba_page_item_key_value_criteria_t uba_page_item_key_value_criterias[];

/************************************************************************/
/* Forward decl local functions                                         */
/************************************************************************/

static int
uba_page_item_check_criteria(const uba_page_item_key_value_criteria_t* criteria, 
                             const ubs_msg_list_item_t* msg);

static int
uba_page_item_check_criterias (uba_page_criteria_t criteria_mask, 
                               const ubs_msg_list_item_t* msg);

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void
uba_page_item_init (      uba_page_item_t     *p, 
                          uba_data_obj_t      *data, 
                    const uba_rce_page_item_t *rc)
{
  switch(rc->type){
  case UBA_PAGE_ITEM_TYPE_MSG_LIST:
     /*lint -fallthrough*/
     /* fall through case */
  case UBA_PAGE_ITEM_TYPE_STATIC:
    p->data = *data;
    break;
  case UBA_PAGE_ITEM_TYPE_FOLDER:
    p->data.type = UBA_DATA_OBJECT_FOLDER;
    p->data.data.folder = uba_data_get_folder ((uba_folder_id_t)rc->int_data);
    break;
  }
  
  uba_text_init (&p->text1, &(p->data), &rc->text_rc);  
  p->rc = rc;
  
}

void
uba_page_item_release (uba_page_item_t *p)
{  
  uba_text_release (&p->text1);
}


int
uba_page_item_get_menu_id (const uba_page_item_t *p)
{
  if (p->data.type == UBA_DATA_OBJECT_MSG){
    return uba_page_item_get_icon_menu_msg (p->data.data.folder_item->item)->menu_id;
  }
  
  return p->rc->menu_id;
}

WeImageHandle
uba_page_item_get_icon( const uba_page_item_t *p )
{
  WE_UINT32 h = p->rc->icon_id;
  if (p->data.type == UBA_DATA_OBJECT_MSG){
    h = uba_page_item_get_icon_menu_msg (p->data.data.folder_item->item)->icon_id;
  }
  
  return WE_WIDGET_IMAGE_GET_PREDEFINED(h,NULL);
}


const uba_page_item_icon_menu_t*
uba_page_item_get_icon_menu_msg (const uba_data_item_t* msg)
{
  const uba_page_item_icon_menu_t* it = uba_page_item_icon_menu_map;
  
  while (it->criteria_mask != UBA_END_KVC_MASK) {
    if (it->msg_type == msg->msg_type && 
        uba_page_item_check_criterias(it->criteria_mask, &msg->msg)){
      return it;
    }
    ++it;
  }
  return it; /* default menu item */
}

/************************************************************************/
/* Local functions                                                      */
/************************************************************************/

/*
 *	Checks if the msg fulfills the criteria criteria and return TRUE if so
 *  else FALSE
 */
static int
uba_page_item_check_criteria (const uba_page_item_key_value_criteria_t *criteria, 
                              const ubs_msg_list_item_t                *msg)
{
  const ubs_key_value_t* it  = msg->titleList;
  const ubs_key_value_t* end = it + msg->titleListSize;
  int i;
  
  
  /* Look for criteria key = msg.property key*/
  for(; it != end && it->key != criteria->key ; ++it){
    /* empty */
  }

 
  if (it == end) {
    /* key not found in properties list */
    return FALSE;
  }
  
  if (it->valueType != criteria->valueType){
    /* No the same key type -> criteria not met */
    return FALSE;
  }

  /* Compare a value */
  switch(criteria->valueType){
  case UBS_VALUE_TYPE_UINT32:
    {
      WE_UINT32 u = criteria->value[0];
      for (i = 1; i<criteria->valueLen; ++i) {
        u<<=8;
        u|= criteria->value[i];
      }
      return (it->valueLen == 4) && 
             (memcmp (&u, it->value, 4) == 0);
    }
  case UBS_VALUE_TYPE_INT32:
    {
      /* for sign extend */
      WE_INT32 v = (signed char) criteria->value[0];
      for (i = 1;i<criteria->valueLen;++i){
        v<<=8;
        v|= criteria->value[i];
      }
      return (it->valueLen == 4) && 
             (memcmp (&v, it->value, 4) == 0);
    }
 
  case UBS_VALUE_TYPE_BOOL:
    {
      int v=0;
      int mb=0;
      for (i = 0;i<criteria->valueLen;++i){
        v|=criteria->value[i];
      }
      for (i = 0;i<it->valueLen;++i){
        mb|=it->value[i];
      };
      /* return boolean comparison v == mb */
      return v ? mb : !mb;
    }
  case UBS_VALUE_TYPE_UTF8:
  case UBS_VALUE_TYPE_BINARY:
  default:
    /* return true if equal length and buffers are identical */
    return (it->valueLen == criteria->valueLen) && 
           (memcmp (criteria->value, it->value, it->valueLen) == 0);

  }
}

/*
 *	Checks all criterias in the criteria mask
 *  Returns : TRUE if all criteria are met
 *            FALSE if not
 */
static int
uba_page_item_check_criterias (      uba_page_criteria_t  criteria_mask, 
                               const ubs_msg_list_item_t *msg)
{
  const uba_page_item_key_value_criteria_t* it = uba_page_item_key_value_criterias;
  uba_page_criteria_t i =1;
  
  for(;it->key!=UBA_END_KEY ;++it, i<<=1) {
    if ((i & criteria_mask) && !uba_page_item_check_criteria (it, msg)){
        return FALSE;
    }
  }
  return TRUE;
}


