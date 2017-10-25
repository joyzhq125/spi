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
 * uba_push.c
 * 
 * Description:
 *    This file contains code to handle Push messages(s)
 *
 *
 * Created by Martin Andersson, 2003/09/16.
 *
 * Revision history:
 *
 */


#include "Uba_Cfg.h"

#include "Uba_Main.h"
#include "Uba_Sig.h"
#include "Uba_Rc.h"
#include "Uba_Rce.h"
#include "Uba_Rc.h"
#include "Uba_Gui.h"


#include "We_Wid.h"
#include "We_Cmmn.h"

#include "We_Lib.h"

#include "Uba_Push.h"

#ifndef WE_MODID_WBA
  #define WE_MODID_WBA WE_MODID_UBA
#endif

/************************************************************************/
/* Exported custom page variable                                          */
/************************************************************************/

const uba_custom_page_func_t uba_custom_page_push_view = 
{
  uba_push_view_init,             
  uba_push_view_release,          
  uba_push_view_handle_sig,       
  uba_push_view_handle_data_event,
  uba_push_view_show              
};

/************************************************************************/
/* Defines                                                              */
/************************************************************************/

#define	UBA_STRCONV(a)		WE_WIDGET_STRING_CREATE(WE_MODID_UBA, a, WeUtf8, strlen(a)+1, 0)

/************************************************************
 * Local Types 
 ************************************************************/
/* menu type*/

typedef struct {
    WeWindowHandle      window;
    WeActionHandle      select;
    WeActionHandle      back;
    WeActionHandle      menu;
    WeGadgetHandle      from_s;
    WeGadgetHandle      from;
    WeGadgetHandle      created_s;
    WeGadgetHandle      created;
    WeGadgetHandle      expired_s;
    WeGadgetHandle      expired;
    WeGadgetHandle      message_s;
    WeGadgetHandle      message;
    WeGadgetHandle      url_s;
    WeGadgetHandle      url;
    char                *url_str;
    WeScreenHandle	     screen;

    uba_data_obj_t      data;
      
} uba_push_x_t;

/************************************************************
 * Defines
 ************************************************************/

#define UBA_PUSH_METHOD_LOAD 0x01
#define UBA_PUSH_METHOD_VIEW 0x02

#define UBA_PUSH_VIEW_GET_FULL_TRANSACTION_ID 1234
#define UBA_PUSH_VIEW_SET_READ_TRANSACTION_ID 1234

/************************************************************
 * Function Prototypes
 ************************************************************/

/************************************************************
 * Internal Function
 ************************************************************/

int
uba_push_find_key(ubs_key_value_t *propertyList, int listSize, WE_UINT16 key)
{
  int i = 0;

  while ((i < listSize) && (propertyList[i].key != key))
    i++;

  if (i == listSize)
    i = -1;

  return i;
}

static void 
uba_push_date_transform (char* s)
{
  char *p;

  p = s;

  /*Skip past "day, "*/
  while (*p++ != ',')
    /* empty */;

  p++;

  while (*p != '\0')
  {
    *s = *p;
    p++;
    s++;
  }

  /*Remove 'GMT'*/
  while (*s-- != ' ')
    /*empty */;

  *(s + 1) = '\0';
}


int
uba_push_view_render(uba_push_x_t*         dlg,
                     ubs_key_value_t       *propertyList,
                     int                    propertyListSize)
{
  
  WeStringHandle       ms;
  char                  created_str[30];
  char                  expired_str[30];
 
  int                   idx;

 
  
  
  
  created_str[0] = 0;
  if ((idx = uba_push_find_key(propertyList, propertyListSize, UBS_MSG_KEY_CREATED)) != -1) {
    if(propertyList[idx].value != NULL) {
      we_cmmn_time2str (*((WE_UINT32*)propertyList[idx].value), created_str);
      uba_push_date_transform (created_str);
    }
  }

  expired_str[0] = 0;
  if ((idx = uba_push_find_key(propertyList, propertyListSize, UBS_MSG_KEY_EXPIRES)) != -1) {
    if(propertyList[idx].value != NULL) {
      we_cmmn_time2str (*((WE_UINT32*)propertyList[idx].value), expired_str);
      uba_push_date_transform (expired_str);
    }
  }




  if ((idx = uba_push_find_key(propertyList, propertyListSize, UBS_MSG_KEY_FROM)) != -1) {
 	
    if(propertyList[idx].value != NULL) {
      ms = UBA_STRCONV ((const char*)propertyList[idx].value);
      WE_WIDGET_STRING_GADGET_SET(dlg->from, ms, TRUE);
      WE_WIDGET_RELEASE (ms);
    }
  }


  
  ms = UBA_STRCONV (created_str);
  WE_WIDGET_STRING_GADGET_SET(dlg->created,ms,TRUE);
	WE_WIDGET_RELEASE (ms);


  
  ms = UBA_STRCONV (expired_str);
  WE_WIDGET_STRING_GADGET_SET(dlg->expired,ms,TRUE);
	WE_WIDGET_RELEASE (ms);
 

  if ((idx = uba_push_find_key(propertyList, propertyListSize, UBS_MSG_KEY_URL)) != -1) {  
    if(propertyList[idx].value != NULL) {
      ms = UBA_STRCONV ((const char*)propertyList[idx].value);
      WE_WIDGET_STRING_GADGET_SET(dlg->url,ms,TRUE);  	 
      WE_WIDGET_RELEASE (ms);
    }
  }


  if ((idx = uba_push_find_key(propertyList, propertyListSize, UBS_MSG_KEY_SUBJECT)) != -1) {
    if(propertyList[idx].value != NULL) {
      ms = UBA_STRCONV ((const char*)propertyList[idx].value);
      WE_WIDGET_STRING_GADGET_SET(dlg->message,ms,FALSE);
      WE_WIDGET_RELEASE (ms);
    }
  }


  return 1;
}


void
uba_push_view_re_render(uba_push_x_t*         dlg,
                        ubs_key_value_t       *propertyList,
                        int                   propertyListSize)
{
  uba_push_view_render (dlg, propertyList, propertyListSize);

}

/*  
 *  This function handles actions for the push window 
 *  
 *
 *  Parameters:
 *
 *  action    : Action to be handled
 *  window    : Window to send the action to
 *
 *  Return    : 1 if handled else 0
 */

static int 
uba_push_view_handle_widget_action (uba_custom_page_t   *p, 
                                    unsigned org_sig, 
                                    we_widget_action_t* action)

{
  uba_push_x_t*         dlg = (uba_push_x_t*)p->usr_data;
  
  we_content_data_t  contentData;
  int index;

  if ((dlg == NULL) || (action->handle != dlg->window))
    return FALSE;

  if (action->action == dlg->select) {
    /*And reset the url in the window sturct so that it's not destroyed*/
    dlg->url_str = NULL;
    /*Close all windows*/
    
    
    /*GoTo URL*/
    index = uba_push_find_key(dlg->data.data.folder_item->item->msg.titleList,
                              dlg->data.data.folder_item->item->msg.titleListSize, UBS_MSG_KEY_URL);
    if(index<0)
    {
      
      uba_rce_remove_custom_page (p);
    }
    else {
      memset(&contentData, 0, sizeof(we_content_data_t));
      contentData.contentUrl = (char*)dlg->data.data.folder_item->item->msg.titleList[index].value;
      contentData.contentDataType = WE_CONTENT_DATA_NONE;

      uba_rce_remove_custom_page (p);

      WE_MODULE_START(WE_MODID_UBA, WE_MODID_WBA, NULL, &contentData, NULL);
  
    }
    goto lblSigHandled;
  }
  
  if(action->action == dlg->back)
  {
    uba_rce_remove_custom_page (p);
    goto lblSigHandled;
  }

  if (action->action == dlg->menu)
  {
    uba_rce_show_menu (dlg->data.data.folder_item->item->msg_type == UBS_MSG_TYPE_PUSH_SI ? 
                          UBA_GUI_MENU_PUSH_VIEW_SI_ID : UBA_GUI_MENU_PUSH_VIEW_SL_ID,
                       &dlg->data);
    goto lblSigHandled;
  }
  
  return FALSE;

lblSigHandled:
  WE_SIGNAL_DESTRUCT (WE_MODID_UBA, (WE_UINT16)org_sig, action);  
  return TRUE;

}

static int
uba_push_view_handle_reply_get_full_msg(uba_custom_page_t   *p, 
                                        unsigned org_sig, 
                                        ubs_reply_full_msg_t  *reply)
{
  uba_push_x_t*         dlg = (uba_push_x_t*)p->usr_data;
  WE_BOOL              msg_read =TRUE;
  ubs_key_value_t       read = { UBS_MSG_KEY_READ, 
                                 UBS_VALUE_TYPE_BOOL, 
                                 sizeof(WE_BOOL), 
                                 NULL
                                };
  
  if (reply->transactionId != UBA_PUSH_VIEW_GET_FULL_TRANSACTION_ID){
    return FALSE;
  }
 
  if (reply->msg.handle.handleLen != dlg->data.data.folder_item->item->msg.handle.handleLen){
    return FALSE;
  }
  
  if (memcmp(reply->msg.handle.handle, 
             dlg->data.data.folder_item->item->msg.handle.handle,
             reply->msg.handle.handleLen) != 0 ){
    return FALSE;
  }

  /* my signal */
  uba_push_view_re_render(dlg, reply->msg.propertyList, reply->msg.propertyListSize);

  read.value = (unsigned char*) &msg_read;
  

  UBSif_setMsgProperty (WE_MODID_UBA, UBA_PUSH_VIEW_SET_READ_TRANSACTION_ID, 
                        reply->msgType, &reply->msg.handle, &read);
  WE_SIGNAL_DESTRUCT (WE_MODID_UBA, (WE_UINT16)org_sig, reply);  
  return TRUE;

}



/************************************************************
 * External Functions
 ************************************************************/


int
uba_push_view_init (uba_custom_page_t *p, 
                    uba_data_obj_t    *data,
                    WeScreenHandle    screen,
                    int                i)
{

  WeSize			          win_size;
  WeDisplayProperties  prop;
	WePosition	          pos =  {0, 20};
  uba_push_x_t*         dlg;
  WePosition	          str_pos;
  WeSize			          str_size;
  /* Check msg type */
  

  UNREFERENCED_PARAMETER (i);


  p->usr_data = UBA_MEM_ALLOCTYPE (uba_push_x_t);
  /*Set all members to 0*/
  memset (p->usr_data, 0, sizeof (uba_push_x_t));
  dlg = (uba_push_x_t*)p->usr_data;
  dlg->data = *data;
  dlg->screen = screen;

   WE_WIDGET_DISPLAY_GET_PROPERTIES(&prop);
   win_size.width  = (WE_INT16)(prop.displaySize.width);
   win_size.height = (WE_INT16)(prop.displaySize.height - 20);

   /*Create Form*/
	dlg->window = WE_WIDGET_FORM_CREATE (WE_MODID_UBA, &win_size, 0x8000, 0);
  




  /* Draw page */
	str_pos.x = 5;
	str_pos.y = 5;
	str_size.height = 20;
	str_size.width = 65;

	dlg->from_s = WE_WIDGET_STRING_GADGET_CREATE(WE_MODID_UBA, WE_WIDGET_STRING_GET_PREDEFINED(UBA_STR_ID_FROM), &str_size, 0, 1, 0x8000, 0);
	WE_WIDGET_WINDOW_ADD_GADGET(dlg->window, dlg->from_s, &str_pos);



	str_pos.x = 5;
	str_pos.y = 25;
	str_size.height = 20;
	str_size.width = 65;

	dlg->created_s = WE_WIDGET_STRING_GADGET_CREATE(WE_MODID_UBA, WE_WIDGET_STRING_GET_PREDEFINED(UBA_STR_ID_CREATED), &str_size, 0, 1, 0x8000, 0);
	WE_WIDGET_WINDOW_ADD_GADGET(dlg->window, dlg->created_s, &str_pos);



  str_pos.x = 5;
	str_pos.y = 45;
	str_size.height = 20;
	str_size.width = 65;

	dlg->expired_s = WE_WIDGET_STRING_GADGET_CREATE(WE_MODID_UBA, WE_WIDGET_STRING_GET_PREDEFINED(UBA_STR_ID_EXPIRES), &str_size, 0, 1, 0x8000, 0);
	WE_WIDGET_WINDOW_ADD_GADGET(dlg->window, dlg->expired_s, &str_pos);


  str_pos.x = 5;
	str_pos.y = 65;
	str_size.height = 20;
	str_size.width = 65;

	dlg->url_s = WE_WIDGET_STRING_GADGET_CREATE(WE_MODID_UBA, WE_WIDGET_STRING_GET_PREDEFINED(UBA_STR_ID_URL), &str_size, 0, 1, 0x8000, 0);
	WE_WIDGET_WINDOW_ADD_GADGET(dlg->window, dlg->url_s, &str_pos);


  str_pos.x = 5;
	str_pos.y = 85;
	str_size.height = 20;
	str_size.width = 65;

	dlg->message_s = WE_WIDGET_STRING_GADGET_CREATE(WE_MODID_UBA, WE_WIDGET_STRING_GET_PREDEFINED(UBA_STR_ID_MESSAGE), &str_size, 0, 1, 0x8000, 0);
	WE_WIDGET_WINDOW_ADD_GADGET(dlg->window, dlg->message_s, &str_pos);





 	str_pos.x = 70;
	str_pos.y = 5;
	str_size.height = 20;
	str_size.width = (WE_INT16)(prop.displaySize.width - 70 - 5);

	dlg->from= WE_WIDGET_STRING_GADGET_CREATE(WE_MODID_UBA, WE_WIDGET_STRING_GET_PREDEFINED(UBA_STR_ID_EMPTY), &str_size, 0, 1, 0x8000, 0);
	WE_WIDGET_WINDOW_ADD_GADGET(dlg->window, dlg->from, &str_pos);


  str_pos.x = 70;
	str_pos.y = 25;
	str_size.height = 20;
	str_size.width = (WE_INT16)(prop.displaySize.width - 70 - 5);

	dlg->created = WE_WIDGET_STRING_GADGET_CREATE(WE_MODID_UBA, WE_WIDGET_STRING_GET_PREDEFINED(UBA_STR_ID_EMPTY), &str_size, 0, 1, 0x8000, 0);
	WE_WIDGET_WINDOW_ADD_GADGET(dlg->window, dlg->created, &str_pos);


  str_pos.x = 70;
	str_pos.y = 45;
	str_size.height = 20;
	str_size.width = (WE_INT16)(prop.displaySize.width - 70 - 5);

	dlg->expired = WE_WIDGET_STRING_GADGET_CREATE(WE_MODID_UBA, WE_WIDGET_STRING_GET_PREDEFINED(UBA_STR_ID_EMPTY), &str_size, 0, 1, 0x8000, 0);
	WE_WIDGET_WINDOW_ADD_GADGET(dlg->window, dlg->expired, &str_pos);
 

  str_pos.x = 70;
	str_pos.y = 65;
	str_size.height = 20;
	str_size.width = (WE_INT16)(prop.displaySize.width - 70 - 5);

  dlg->url= WE_WIDGET_STRING_GADGET_CREATE(WE_MODID_UBA, WE_WIDGET_STRING_GET_PREDEFINED(UBA_STR_ID_EMPTY), &str_size, 0, 1, 0x8000, 0);
  WE_WIDGET_WINDOW_ADD_GADGET(dlg->window, dlg->url, &str_pos);


  str_pos.x = 5;
	str_pos.y = 105;
	str_size.height = (WE_INT16)(prop.displaySize.height - 40 - 105);
	str_size.width  = (WE_INT16)(prop.displaySize.width - 10);

  dlg->message= WE_WIDGET_STRING_GADGET_CREATE(WE_MODID_UBA, WE_WIDGET_STRING_GET_PREDEFINED(UBA_STR_ID_EMPTY), &str_size, 0, 1, 0x8000, 0);
  WE_WIDGET_WINDOW_ADD_GADGET(dlg->window, dlg->message, &str_pos);

  
  uba_push_view_render (dlg, 
                        data->data.folder_item->item->msg.titleList,
                        data->data.folder_item->item->msg.titleListSize);



	dlg->select = WE_WIDGET_ACTION_CREATE(WE_MODID_UBA, 
                                         WE_WIDGET_STRING_GET_PREDEFINED(UBA_STR_ID_SELECT),
                                         0, WeOk);
	dlg->back = WE_WIDGET_ACTION_CREATE(WE_MODID_UBA, 
                                       WE_WIDGET_STRING_GET_PREDEFINED(UBA_STR_ID_BACK),
                                       0, WeBack);
  dlg->menu = WE_WIDGET_ACTION_CREATE(WE_MODID_UBA, 
                                       WE_WIDGET_STRING_GET_PREDEFINED(UBA_STR_ID_MENU),
                                       0, WeMenu);
	
  WE_WIDGET_ADD_ACTION (dlg->window, dlg->back);
  WE_WIDGET_ADD_ACTION (dlg->window, dlg->menu);
  WE_WIDGET_ADD_ACTION (dlg->window, dlg->select);

  WE_WIDGET_SCREEN_ADD_WINDOW(screen, dlg->window, &pos);




  

  UBSif_getFullMsg (WE_MODID_UBA, UBA_PUSH_VIEW_GET_FULL_TRANSACTION_ID, 
                    data->data.folder_item->item->msg_type, 
                    &data->data.folder_item->item->msg.handle,
                    TRUE, FALSE, FALSE);




  return TRUE; 
}



void 
uba_push_view_release(uba_custom_page_t *p)
{
  uba_push_x_t*         dlg = (uba_push_x_t*)p->usr_data;

 	WE_WIDGET_REMOVE (dlg->screen, dlg->window); 

	WE_WIDGET_RELEASE (dlg->window); 
  WE_WIDGET_RELEASE (dlg->select);
  WE_WIDGET_RELEASE (dlg->back);
  WE_WIDGET_RELEASE (dlg->menu);
  WE_WIDGET_RELEASE (dlg->from_s);
  WE_WIDGET_RELEASE (dlg->from);
  WE_WIDGET_RELEASE (dlg->created_s);
  WE_WIDGET_RELEASE (dlg->created);
  WE_WIDGET_RELEASE (dlg->expired_s);
  WE_WIDGET_RELEASE (dlg->expired);
  WE_WIDGET_RELEASE (dlg->message_s);
  WE_WIDGET_RELEASE (dlg->message);
  WE_WIDGET_RELEASE (dlg->url_s);
  WE_WIDGET_RELEASE (dlg->url);

  UBA_MEM_FREE (dlg->url_str);
  UBA_MEM_FREE (dlg);
}






void 
uba_push_view_show (uba_custom_page_t* p)
{
   uba_push_x_t*         dlg = (uba_push_x_t*)p->usr_data;
  (void)WE_WIDGET_SET_IN_FOCUS(dlg->window, TRUE);
}

void 
uba_push_view_handle_data_event (uba_custom_page_t    *p, 
                                 const uba_data_obj_t *data,
                                 uba_data_event_t      event)
{
  
  
  if (!uba_data_object_is_equal(data, &(( uba_push_x_t*)p->usr_data)->data))
  {
    return;
  }
  switch(event)
  {
  case UBA_DATA_EVENT_DELETE:
    uba_rce_remove_custom_page (p);
    break;
  case UBA_DATA_EVENT_CHANGE:
    uba_push_view_re_render ((uba_push_x_t*)p->usr_data,
                             data->data.folder_item->item->msg.titleList,
                             data->data.folder_item->item->msg.titleListSize);
    break;
  }
}


int 
uba_push_view_handle_sig (uba_custom_page_t   *p, 
                          uba_signal_t                *sig)
{
  switch (sig->type)
  {
  case UBA_MAIN_SIG_WIDGET_ACTION:
    return uba_push_view_handle_widget_action (p, 
                                               sig->u_param1, 
                                               (we_widget_action_t*)sig->p_param);
  case UBS_MAIN_SIG_REPLY_FULL_MSG:
    return uba_push_view_handle_reply_get_full_msg(
                                               p,
                                               sig->u_param1, 
                                               (ubs_reply_full_msg_t*)sig->p_param);

  default:
    return FALSE;
  }


}





