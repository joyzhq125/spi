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
 * uba_del.c
 * 
 * Created by Klas Hermodsson, 2003/09/24.
 *
 * Revision history:
 *
 */

#include "Uba_Del.h"

#include "Uba_Main.h"
#include "Uba_Rce.h"
#include "Uba_Actn.h"
#include "Uba_Rc.h"

#include "We_Wid.h"

/************************************************************************/
/* Exported custom page variable                                          */
/************************************************************************/

const uba_custom_page_func_t uba_custom_page_del_dlg = 
{
  uba_delete_msg_view_init,
  uba_delete_msg_view_release,
  uba_delete_msg_view_handle_sig,
  uba_delete_msg_view_handle_data_event,
  uba_delete_msg_view_show
};

/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/

/* The user data for this delete custom page */
typedef struct {
    WeWindowHandle  window;
    WeActionHandle  accept;
    WeActionHandle  back;
    WeScreenHandle  screen;
    uba_data_obj_t   data;      
} uba_delete_msg_view_t;

/************************************************************************/
/* Forward decl internal functions                                      */
/************************************************************************/

static int 
uba_delete_msg_view_handle_widget_action (uba_custom_page_t   *p, 
                                          unsigned             org_sig, 
                                          we_widget_action_t *action);

/************************************************************************/
/* Exported functions                                                    */
/************************************************************************/
/* Init the custom page, data must point to a folder object
 * return TRUE if successful 
 */
int
uba_delete_msg_view_init (uba_custom_page_t *p, 
                          uba_data_obj_t    *data,
                          WeScreenHandle    screen,
                          int                i)
{
  uba_delete_msg_view_t   *dlg;
  WeStringHandle         dlg_str;
	WePosition	            pos =  {0, 0};

  UNREFERENCED_PARAMETER (i);

  

  p->usr_data = UBA_MEM_ALLOCTYPE(uba_delete_msg_view_t);

  dlg = (uba_delete_msg_view_t*)p->usr_data;

  dlg->data = *data;
  dlg->screen = screen;

  if (data->type == UBA_DATA_OBJECT_MSG) {
    dlg_str = WE_WIDGET_STRING_GET_PREDEFINED (UBA_STR_ID_DELETE_ITEM);
  }
  else if (data->type == UBA_DATA_OBJECT_FOLDER) {
    dlg_str = WE_WIDGET_STRING_GET_PREDEFINED (UBA_STR_ID_DELETE_ALL_ITEMS);
  }
  else {
    
    dlg_str = WE_WIDGET_STRING_GET_PREDEFINED (UBA_STR_ID_EMPTY);
  }


  dlg->window = WE_WIDGET_DIALOG_CREATE (WE_MODID_UBA, dlg_str, WeConfirmation, 
                                          0);

	dlg->accept = WE_WIDGET_ACTION_CREATE(WE_MODID_UBA, 
                                         WE_WIDGET_STRING_GET_PREDEFINED(UBA_STR_ID_OK),
                                         0, WeOk);
	dlg->back = WE_WIDGET_ACTION_CREATE(WE_MODID_UBA, 
                                       WE_WIDGET_STRING_GET_PREDEFINED(UBA_STR_ID_CANCEL),
                                       0, WeCancel);
	
  WE_WIDGET_ADD_ACTION (dlg->window, dlg->back);
	WE_WIDGET_ADD_ACTION (dlg->window, dlg->accept);

  pos.x = 0;
  pos.y = 0;
  WE_WIDGET_SCREEN_ADD_WINDOW(screen, dlg->window, &pos);

  return TRUE;
}


/*
 *	Releases the allocated resources and widgets of p
 */
void 
uba_delete_msg_view_release(uba_custom_page_t *p)
{
  uba_delete_msg_view_t* dlg = (uba_delete_msg_view_t*)p->usr_data;
 	WE_WIDGET_REMOVE (dlg->screen, dlg->window); 
  WE_WIDGET_RELEASE(dlg->accept);
  WE_WIDGET_RELEASE(dlg->back);
  WE_WIDGET_RELEASE(dlg->window);
  UBA_MEM_FREE(p->usr_data);
}


/*
 *	Offer the signal to this custom page, return TRUE if sig used
 */
int 
uba_delete_msg_view_handle_sig (uba_custom_page_t   *p, 
                          uba_signal_t                *sig)
{
  switch (sig->type)
  {
  case UBA_MAIN_SIG_WIDGET_ACTION:
    return uba_delete_msg_view_handle_widget_action (p, sig->u_param1, 
                                                     (we_widget_action_t*)sig->p_param);
  default:
    return FALSE;
  }
}


/*
 *	Some data has changed
 */
void 
uba_delete_msg_view_handle_data_event (uba_custom_page_t *p, 
                                       const uba_data_obj_t *data,
                                       uba_data_event_t event)
{
  if (!uba_data_object_is_equal(data, &((uba_delete_msg_view_t*)p->usr_data)->data))
  {
    return;
  }

  switch(event)
  {
  case UBA_DATA_EVENT_DELETE:
    uba_rce_remove_custom_page (p); 
    break;
  }
}


/*
 *	View this custom page
 */
void 
uba_delete_msg_view_show (uba_custom_page_t* p)
{
   uba_delete_msg_view_t* dlg = (uba_delete_msg_view_t*)p->usr_data;
  (void)WE_WIDGET_SET_IN_FOCUS(dlg->window, TRUE);  
}

/************************************************************************/
/* Internal functions                                                   */
/************************************************************************/

static int 
uba_delete_msg_view_handle_widget_action (uba_custom_page_t   *p, 
                                             unsigned org_sig, 
                                             we_widget_action_t* action)
{
  uba_delete_msg_view_t* dlg = (uba_delete_msg_view_t*)p->usr_data;
  uba_data_obj_t  data_obj = dlg->data;

  if (action->handle != dlg->window) {
    return FALSE;
  }

  if (action->action == dlg->back) {
    uba_rce_remove_custom_page(p);
    WE_SIGNAL_DESTRUCT (WE_MODID_UBA, (WE_UINT16)org_sig, action);  
    return TRUE;
  }

  if (action->action == dlg->accept) {

    uba_rce_remove_custom_page(p);

    if (data_obj.type == UBA_DATA_OBJECT_MSG) {
      uba_action_ubs_delete_msg (&(data_obj), 0, 0, NULL);
    }
    else if (data_obj.type == UBA_DATA_OBJECT_FOLDER) {
      uba_action_top_page_delete_all (&(data_obj), 0, 0, NULL);
    }

    WE_SIGNAL_DESTRUCT (WE_MODID_UBA, (WE_UINT16)org_sig, action);  

    return TRUE;

  }

  return FALSE;
}

