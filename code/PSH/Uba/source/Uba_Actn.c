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
 * uba_actn.c
 *
 * Created by Martin Andersson, 2003/08/28.
 *
 * Revision history:
 *
 */

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "Uba_Actn.h"

#include "Uba_Rce.h"
#include "Uba_Main.h"
#include "Uba_Data.h"
#include "Uba_Ditm.h"
#include "Uba_Page.h"

#include "We_Core.h"
#include "We_Tel.h"
#include "We_Log.h"

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

void
uba_action_view_page(uba_data_obj_t* p, int i, int i2, const char* str)
{
  UNREFERENCED_PARAMETER (str);
  UNREFERENCED_PARAMETER (i2);
  uba_rce_show_page (i, p);
}


void
uba_action_view_menu(uba_data_obj_t* p, int i, int i2, const char* str)
{
  UNREFERENCED_PARAMETER (str);
  UNREFERENCED_PARAMETER (i2);
  uba_rce_show_menu (i, p);
}

void
uba_action_view_custom_page(uba_data_obj_t* p, int i, int i2, const char* str)
{
  UNREFERENCED_PARAMETER (str);
  UNREFERENCED_PARAMETER (i2);
  uba_rce_show_custom_page (i, p, i2);
}

void
uba_action_static_content_routing(uba_data_obj_t* p, int i, int i2, const char* str)
{
  we_content_data_t  contentData; 
  memset(&contentData, 0, sizeof(we_content_data_t));

  UNREFERENCED_PARAMETER(p);
  UNREFERENCED_PARAMETER (i2);

  WE_CONTENT_SEND(WE_MODID_UBA, (WE_UINT8)i, (char*)str, &contentData, FALSE, 0, TRUE);
}


void
uba_action_content_routing_with_handle_as_hex_string (uba_data_obj_t* p, 
                                                      int i, int i2, const char* str)
{
  we_content_data_t    contentData;
  char                  *cr_str;
  uba_data_item_t       *item;
  char                  *handle_hex_str;

  UNREFERENCED_PARAMETER (i2);

  if ((p->type != UBA_DATA_OBJECT_MSG) || (p->data.folder_item == NULL) ||
      (p->data.folder_item->item == NULL)) {
    
    return;
  }

  memset(&contentData, 0, sizeof(we_content_data_t));
  item = p->data.folder_item->item;
  handle_hex_str = uba_data_item_get_handle_as_hex_str (item);

  cr_str = UBA_MEM_ALLOC (strlen (str) + strlen (handle_hex_str) + 1);
  sprintf (cr_str, str, handle_hex_str);

  WE_CONTENT_SEND(WE_MODID_UBA, (WE_UINT8)i, cr_str, &contentData, FALSE, 0, TRUE);

  UBA_MEM_FREE (cr_str);
  UBA_MEM_FREE (handle_hex_str);
}


/*	Sends a content routing to module wid i with
 *  sprintfs str together with the property key i2 (must be have a UTF8 property value)
 *  NOTE that p must be of the type UBA_DATA_OBJECT_MSG!
 *
 *  Example: 
 *   i = WE_MODID_MSA
 *   i2= UBS_MSG_KEY_FROM
 *   str "-mode new -to %s"
 *
 */
void
uba_action_content_routing_with_UTF8_property (uba_data_obj_t* p, 
                                               int i, int i2, const char* str)
{
  













  we_content_data_t    contentData;
  char                  *cr_str;
  uba_data_item_t       *item;
  ubs_key_value_t       *theProperty;

  UNREFERENCED_PARAMETER (i2);

  if ((p->type != UBA_DATA_OBJECT_MSG) || (p->data.folder_item == NULL) ||
      (p->data.folder_item->item == NULL)) {
    
    return;
  }

  memset(&contentData, 0, sizeof(we_content_data_t));
  item = p->data.folder_item->item;

  theProperty = uba_data_util_lookup_msg_property (&(item->msg), i2);

  if (!theProperty || (theProperty->valueType != UBS_VALUE_TYPE_UTF8) ||
     (!(theProperty->value))) {
    
    return;
  }

  cr_str = UBA_MEM_ALLOC (strlen (str) + strlen ((char*)(theProperty->value)) + 1);
  sprintf (cr_str, str, theProperty->value);

  WE_CONTENT_SEND(WE_MODID_UBA, (WE_UINT8)i, cr_str, &contentData, FALSE, 0, TRUE);

  UBA_MEM_FREE (cr_str);
}


void
uba_action_content_routing_with_URL (uba_data_obj_t* p, int i, int i2, const char* str)
{
  we_content_data_t    contentData;
  uba_data_item_t       *item;
  ubs_key_value_t       *url_property;

  UNREFERENCED_PARAMETER (str);
  UNREFERENCED_PARAMETER (i2);

  if ((p->type != UBA_DATA_OBJECT_MSG) || (p->data.folder_item == NULL) ||
      (p->data.folder_item->item == NULL)) {
    
    return;
  }

  memset(&contentData, 0, sizeof(we_content_data_t));
  item = p->data.folder_item->item;

  url_property = uba_data_util_lookup_msg_property (&(item->msg), UBS_MSG_KEY_URL);

  if (!url_property || !url_property->value) {
    
    return;
  }

  contentData.contentDataType = WE_CONTENT_DATA_NONE;
  contentData.contentUrl = (char*)(url_property->value);

  WE_CONTENT_SEND(WE_MODID_UBA, (WE_UINT8)i, NULL, &contentData, FALSE, 0, TRUE);
}


void
uba_action_call_del_voice_mail (uba_data_obj_t* p, int i, int i2, const char* str)
{
  UNREFERENCED_PARAMETER (i);
  UNREFERENCED_PARAMETER (i2);
  UNREFERENCED_PARAMETER (str);

  if ((p->type != UBA_DATA_OBJECT_MSG) || (p->data.folder_item == NULL) ||
      (p->data.folder_item->item == NULL)) {
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBA,
           "UBA ERROR, uba_action_call_del_voice_mail: illegal parametrers\n"));
    return;
  }

  WE_TEL_MAKE_CALL (WE_MODID_UBA, 1, uba_data_get_voice_mail_num());

  uba_action_ubs_delete_msg (p, 0, 0, NULL);
}


void
uba_action_ubs_delete_msg (uba_data_obj_t* p, int i, int i2, const char* str)
{
  UNREFERENCED_PARAMETER (i);
  UNREFERENCED_PARAMETER (i2);
  UNREFERENCED_PARAMETER (str);

  if (!p || (p->type != UBA_DATA_OBJECT_MSG)) {
    return;
  }

  uba_do_delete_ubs_msg (p->data.folder_item->item->msg_type, 
                   &(p->data.folder_item->item->msg.handle));
}


/*
 *  Calls UBSif_chnageMsg on the msg indicated by p by setting its property key i
 *  to str.
 */
void
uba_action_ubs_change_UTF8_property (uba_data_obj_t* p, int i, int i2, const char* str)
{
  uba_data_item_t*  item;
  ubs_key_value_t   msg_property;

  UNREFERENCED_PARAMETER (i2);

  if (!str || !p || (p->type != UBA_DATA_OBJECT_MSG)) {
    return;
  }

  item = p->data.folder_item->item;

  if (!item) {
    return;
  }


  msg_property.key = (WE_UINT16)i;
  msg_property.valueType = UBS_VALUE_TYPE_UTF8;
  msg_property.valueLen = (WE_UINT16)(strlen (str) + 1);
  msg_property.value = (unsigned char*)str;

  
  UBSif_setMsgProperty(WE_MODID_UBA, 0, item->msg_type, &item->msg.handle, &msg_property);
}


void
uba_action_top_page_delete_all (uba_data_obj_t* p, int i, int i2, const char* str)
{
  uba_page_t  *page;

  UNREFERENCED_PARAMETER (p);
  UNREFERENCED_PARAMETER (i);
  UNREFERENCED_PARAMETER (i2);
  UNREFERENCED_PARAMETER (str);

  page = uba_rce_get_top_page ();

  if (!page) {
    return;
  }

  uba_page_do_delete_all (page);
}


/*
 *  Performs a sorting of the folder pointed to by p. The sort order is in i.
 */
void
uba_action_sort_folder (uba_data_obj_t* p, int i, int i2, const char* str)
{
  UNREFERENCED_PARAMETER (i2);
  UNREFERENCED_PARAMETER (str);

  if (!p || (p->type != UBA_DATA_OBJECT_FOLDER) || !(p->data.folder)) {
    
    return;
  }

  uba_folder_sort (p->data.folder, i);
}


/*
 *  Starts the module with the module wid i.
 */
void
uba_action_start_module (uba_data_obj_t* p, int i, int i2, const char* str)
{
  UNREFERENCED_PARAMETER (p);
  UNREFERENCED_PARAMETER (i2);
  UNREFERENCED_PARAMETER (str);

  if (i >= 0) {
    WE_MODULE_START (WE_MODID_UBA, (WE_UINT8)i, NULL, NULL, NULL);
  }
}


