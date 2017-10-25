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
 * uba_mmsg.h
 * 
 * Description:
 *    This file contains code to handle moving msg to user defined folders
 *
 */



#include "Uba_Mmsg.h"

#include "Uba_Main.h"
#include "Uba_Rce.h"
#include "Uba_Actn.h"
#include "Uba_Rc.h"
#include "We_Wid.h"

/************************************************************************/
/* Exported custom page variable                                          */
/************************************************************************/

const uba_custom_page_func_t uba_custom_page_move_msg_view = 
{
  uba_move_msg_view_init,
  uba_move_msg_view_release,
  uba_move_msg_view_handle_sig,
  uba_move_msg_view_handle_data_event,
  uba_move_msg_view_show
};

/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/

typedef struct {
    WeWindowHandle      window;
    WeActionHandle      accept;
    WeActionHandle      back;     
    WeScreenHandle      screen;
    uba_data_obj_t       data;
    uba_folder_t*        folders[UBA_FOLDER_COUNT-UBA_FIRST_USER_DEF_FOLDER];
    int                  folder_count;    
} uba_move_msg_view_t;


/************************************************************************/
/* Forwad declarations                                                  */
/************************************************************************/

static void
uba_move_msg_view_uppdate_items(uba_move_msg_view_t* dlg);

static int 
uba_move_msg_view_handle_widget_action (uba_custom_page_t   *p, 
                                        unsigned org_sig, 
                                        we_widget_action_t* action);


/************************************************************************/
/* External functions                                                    */
/************************************************************************/

/* Init the custom page, data must point to a folder object
 * return TRUE if successful 
 */
int
uba_move_msg_view_init (uba_custom_page_t *p, 
                    uba_data_obj_t    *data,
                    WeScreenHandle    screen,
                    int                i)
{
  
  uba_move_msg_view_t*  dlg;
  WeDisplayProperties  prop;
	WePosition	          pos =  {0, 0};

  UNREFERENCED_PARAMETER (i);

  

  p->usr_data = UBA_MEM_ALLOCTYPE(uba_move_msg_view_t);

  dlg = (uba_move_msg_view_t*)p->usr_data;

  dlg->screen       = screen;
  dlg->folder_count = 0;
  dlg->data         = *data;


  WE_WIDGET_DISPLAY_GET_PROPERTIES(&prop);

  dlg->accept = WE_WIDGET_ACTION_CREATE(WE_MODID_UBA, 
                                         WE_WIDGET_STRING_GET_PREDEFINED(UBA_STR_ID_MOVE),
                                         0, WeSelect);
  dlg->window = WE_WIDGET_MENU_CREATE (WE_MODID_UBA,
                                      WeImplicitChoice,
	  								                  &prop.displaySize,
                                      0, /*Element Position*/
                                      WE_CHOICE_ELEMENT_STRING_1, /*Bit Mask*/
                                      0, /* parent handle */
                                      0, /* parent index */
                                      0x8000, /*Property Mask*/
                                      0 /*Default Style*/);

  WE_WIDGET_ADD_ACTION (dlg->window, dlg->accept);
  
  WE_WIDGET_SET_TITLE (dlg->window, WE_WIDGET_STRING_GET_PREDEFINED(UBA_STR_ID_MOVE_TO_FOLDER), 0);


	
	dlg->back = WE_WIDGET_ACTION_CREATE(WE_MODID_UBA, 
                                       WE_WIDGET_STRING_GET_PREDEFINED(UBA_STR_ID_BACK),
                                       0, WeBack);
	
  uba_move_msg_view_uppdate_items(dlg);
  WE_WIDGET_ADD_ACTION (dlg->window, dlg->back);

  WE_WIDGET_SCREEN_ADD_WINDOW(screen, dlg->window, &pos);

  return TRUE;

}
/*
*/
/*
 *	Releases the allocated resources and widgets of p
 */
void 
uba_move_msg_view_release(uba_custom_page_t *p)
{
  uba_move_msg_view_t* dlg = (uba_move_msg_view_t*)p->usr_data;
 	WE_WIDGET_REMOVE  (dlg->screen, dlg->window); 
  WE_WIDGET_RELEASE (dlg->accept);
  WE_WIDGET_RELEASE (dlg->back);
  WE_WIDGET_RELEASE (dlg->window);
  UBA_MEM_FREE       (p->usr_data);
}

/*
 *	Offer the signal to this custom page, return TRUE if sig used
 */
int 
uba_move_msg_view_handle_sig (uba_custom_page_t   *p, 
                              uba_signal_t                *sig)
{
  switch (sig->type)
  {
  case UBA_MAIN_SIG_WIDGET_ACTION:
    return uba_move_msg_view_handle_widget_action (
                        p, 
                        sig->u_param1, 
                        (we_widget_action_t*)sig->p_param);
  default:
    return FALSE;
  }

}

/*
 *	Some data has changed
 */
void 
uba_move_msg_view_handle_data_event (uba_custom_page_t    *p, 
                                 const uba_data_obj_t *data,
                                 uba_data_event_t      event)
{
  uba_move_msg_view_t* dlg = (uba_move_msg_view_t*)p->usr_data;
  
  UNREFERENCED_PARAMETER(event);
  
  if (data->type == UBA_DATA_OBJECT_FOLDER){
    uba_move_msg_view_uppdate_items (dlg);
  }
  else if (uba_data_object_is_equal (&dlg->data, data)){
    
    uba_rce_remove_custom_page(p);
  }
}

/*
 *	View this custom page
 */
void 
uba_move_msg_view_show (uba_custom_page_t* p)
{
  uba_move_msg_view_t* dlg = (uba_move_msg_view_t*)p->usr_data;
  (void)WE_WIDGET_SET_IN_FOCUS (dlg->window, TRUE);    
}



/************************************************************************/
/* Local functions                                                      */
/************************************************************************/

static void
uba_move_msg_view_uppdate_items (uba_move_msg_view_t* dlg)
{
  int i;
  uba_folder_id_t fold_id;
  WeStringHandle we_str;
  const char*     str;

  while (dlg->folder_count--){
    WE_WIDGET_CHOICE_REMOVE_ELEMENT (dlg->window,0);
  }
 
  
  for ( fold_id=UBA_FIRST_USER_DEF_FOLDER, i =0; fold_id < UBA_FOLDER_COUNT; ++fold_id){    
    
    if (fold_id!=dlg->data.data.folder_item->item->folder_index){
      
      dlg->folders[i] = uba_data_get_folder (fold_id);
      str = uba_folder_get_custom_name (dlg->folders[i]);
      
      if (str){
        we_str = WE_WIDGET_STRING_CREATE (WE_MODID_UBA, str, WeUtf8, strlen(str)+1, 0);
      }
      else{
        we_str = WE_WIDGET_STRING_GET_PREDEFINED (
                                          uba_folder_get_name_str_rc_id (dlg->folders[i]));
      }
   
      WE_WIDGET_CHOICE_SET_ELEMENT (dlg->window, i, 0, we_str, 0, 0, 0, 0, 0 /* tag */, 0);
      WE_WIDGET_RELEASE            (we_str);
      ++i;
    }
  }
}

static int 
uba_move_msg_view_handle_widget_action (uba_custom_page_t   *p, 
                                        unsigned org_sig, 
                                        we_widget_action_t* action)
{
  uba_move_msg_view_t* dlg = (uba_move_msg_view_t*)p->usr_data;
  
  if (action->handle != dlg->window){
    return FALSE;
  }

  if (action->action == dlg->back) {
   uba_rce_remove_custom_page (p);
   WE_SIGNAL_DESTRUCT (WE_MODID_UBA, (WE_UINT16)org_sig, action);  
   return TRUE;
  }

  if (action->action == dlg->accept){
   int i = uba_util_get_selected_element (dlg->window);
   uba_action_ubs_change_UTF8_property (&dlg->data, UBS_MSG_KEY_FOLDER, 0,
                                        uba_folder_get_ubs_folder_name (dlg->folders[i]));
   uba_rce_remove_custom_page (p);
 
   WE_SIGNAL_DESTRUCT (WE_MODID_UBA, (WE_UINT16)org_sig, action);  
 
   return TRUE;

  }

  return FALSE;

 }


