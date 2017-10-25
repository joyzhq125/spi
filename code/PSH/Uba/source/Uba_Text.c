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
 * uba_text.c
 *
 * Created by Martin Andersson, 2003/08/22.
 *
 * Revision history:
 *
 */

#include "Uba_Text.h"

#include "Uba_Data.h"
#include "Ubs_If.h"
#include "Uba_Pitm.h"
#include "Uba_Main.h"
#include "Uba_Rc.h"

#include "We_Wid.h"
#include "We_Lib.h"
#include "We_Cmmn.h"

/************************************************************************/
/* Forwad declartions of local functions                                */
/************************************************************************/

static void
uba_text_init_text_for_msg (uba_text_t* p, uba_data_obj_t* data);

static void
uba_text_init_text_for_folder (uba_text_t* p, uba_folder_t* folder, 
                               uba_text_type_t text_type);

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/* Init p with the data object data and rc */
int
uba_text_init (uba_text_t* p, uba_data_obj_t* data, const uba_rce_text_t* rc)
{  
  p->type = rc->text_type;
 
  switch (rc->text_type)
  {
  case UBA_TEXT_TYPE_RC:
    p->text = WE_WIDGET_STRING_GET_PREDEFINED(rc->text_rc);
    break;
  case UBA_TEXT_TYPE_MSG:
    uba_text_init_text_for_msg (p, data); /* complex case use special function */
    break;

  /* Folders */
  case UBA_TEXT_TYPE_FOLDER:
     /*lint -fallthrough*/
     /* fall through case */
  case UBA_TEXT_TYPE_FOLDER_PLUS_UNREAD_COUNT:
     /*lint -fallthrough*/
     /* fall through case */
  case UBA_TEXT_TYPE_FOLDER_PLUS_UNREAD_AND_TOTAL_COUNT:
     /*lint -fallthrough*/
     /* fall through case */
  case UBA_TEXT_TYPE_FOLDER_PLUS_TOTAL_COUNT:
    if (data->type != UBA_DATA_OBJECT_FOLDER) {
      
      p->text = WE_WIDGET_STRING_GET_PREDEFINED (UBA_STR_ID_EMPTY);
    }
    else{
      uba_text_init_text_for_folder(p, data->data.folder, rc->text_type);
    }
    break;
  default:
    
    ;

  }
  

  return (p->text!= 0); 
}


/* Release p and ps resources */
void
uba_text_release (uba_text_t* p)
{
  WE_WIDGET_RELEASE (p->text);
}

/************************************************************************/
/* Local functions                                                      */
/************************************************************************/

static void
uba_text_init_text_for_msg (uba_text_t* p, uba_data_obj_t* data)
{
  const uba_page_item_icon_menu_t *rc_msg;
  uba_data_item_t                 *msg ;
  ubs_key_value_t                 *kv;
  
  if (data->type != UBA_DATA_OBJECT_MSG){
    goto ErrLbl;
  }
  
  msg = data->data.folder_item->item;
  rc_msg = uba_page_item_get_icon_menu_msg (msg);
  
  if (!rc_msg){
    goto ErrLbl;
  }

  kv = uba_data_util_lookup_msg_property (&msg->msg, rc_msg->text_property_id);

  if (!kv || (kv->valueType != UBS_VALUE_TYPE_UTF8) || (kv->valueLen <= 1)){
    kv = uba_data_util_lookup_msg_property (&msg->msg, rc_msg->text_property_id_2);
  }
    
  if (!kv || (kv->valueType != UBS_VALUE_TYPE_UTF8) || (kv->valueLen <= 1)){
    goto ErrLbl;
  }
 

  p->text = WE_WIDGET_STRING_CREATE (WE_MODID_UBA, (const char*)kv->value, 
                                      WeUtf8, kv->valueLen,0);
  return;

ErrLbl:
   
   
   p->text = WE_WIDGET_STRING_GET_PREDEFINED (UBA_STR_ID_EMPTY);
      

}


/*
 *	Init the WE text for a uba_text_t with data object pointing to a folder
 *  and one of the following types 
 *    UBA_TEXT_TYPE_FOLDER, 
 *    UBA_TEXT_TYPE_FOLDER_PLUS_UNREAD_COUNT,
 *    UBA_TEXT_TYPE_FOLDER_PLUS_UNREAD_AND_TOTAL_COUNT,
 *    UBA_TEXT_TYPE_FOLDER_PLUS_TOTAL_COUNT
 */
static void
uba_text_init_text_for_folder (uba_text_t* p, uba_folder_t* folder, 
                               uba_text_type_t text_type)
{
  const char*       folder_name = uba_folder_get_custom_name (folder);
  char*             str  = NULL;
  char*             str2 = NULL;
  char              str_nbr[20];
  unsigned          count = uba_folder_get_nbr_of_items (folder);
  unsigned          unread_count = uba_folder_get_nbr_of_unread_items (folder);
  int               folder_name_str_len;


  /* To prevent the adding of (0) or (0/0) to a empty folder */
  if( count == 0 ) {
    text_type = UBA_TEXT_TYPE_FOLDER;
  }
  
  if (!folder_name) {
    /* No custom name */

    /* Get folder we string */
    WeStringHandle we_str = WE_WIDGET_STRING_GET_PREDEFINED (
                                    uba_folder_get_name_str_rc_id (folder));
    
    if(text_type == UBA_TEXT_TYPE_FOLDER ){
      /* dont add any text */
      p->text = we_str;
      /* No more to do exit */
      return;
    }
    
    folder_name_str_len = WE_WIDGET_STRING_GET_LENGTH (we_str, TRUE, WeUtf8); 
    str = UBA_MEM_ALLOC (folder_name_str_len+1);
    WE_WIDGET_STRING_GET_DATA (we_str, str, WeUtf8);
    /* No need to free we_str predefined */
    folder_name = str;
  } 
  
  
  switch (text_type)
  {
    case UBA_TEXT_TYPE_FOLDER:
      break;
    
    case UBA_TEXT_TYPE_FOLDER_PLUS_UNREAD_COUNT:
      sprintf (str_nbr, " (%u)", unread_count);
      folder_name = str2 = we_cmmn_strcat (WE_MODID_UBA, folder_name, str_nbr);
      break;

    case UBA_TEXT_TYPE_FOLDER_PLUS_UNREAD_AND_TOTAL_COUNT:
      sprintf (str_nbr, " (%u/%u)", unread_count, count);
      folder_name = str2 = we_cmmn_strcat (WE_MODID_UBA, folder_name, str_nbr);
      break;

    case UBA_TEXT_TYPE_FOLDER_PLUS_TOTAL_COUNT:
      sprintf (str_nbr, " (%u)", count);
      folder_name = str2 = we_cmmn_strcat (WE_MODID_UBA, folder_name, str_nbr);
      break;
    
    default:
      
      break;
  }

  p->text = WE_WIDGET_STRING_CREATE (WE_MODID_UBA, folder_name, WeUtf8, 
                                      strlen (folder_name)+1, 0);

  UBA_MEM_FREE (str);
  UBA_MEM_FREE (str2);
}

