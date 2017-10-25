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

#include "We_Dprc.h"
#include "We_Dlg.h"
#include "We_Rc.h"

#include "We_Def.h"
#include "We_Mem.h"
#include "We_Wid.h"


#include "We_Cmmn.h"
#include "We_Core.h"
#include "We_Lib.h"
#include "We_Wid.h"

/**********************************************************************
 * Constants
 **********************************************************************/

/* Number of handles used by different dialog types: */
#define WE_DLG_MESSAGE_ALERT_HANDLE      2
#define WE_DLG_MESSAGE_WARNING_HANDLE    3
#define WE_DLG_MESSAGE_INFO_HANDLE       4
#define WE_DLG_MESSAGE_ERROR_HANDLE      5
#define WE_DLG_CONFIRM_HANDLE            6
#define WE_DLG_PROMPT_HANDLE             7
#define WE_DLG_PROGRESS_BAR_HANDLE       8
#define WE_DLG_LOGIN_HANDLE              9

/* Indices into the array of handles */
#define WE_DLG_SCREEN_HANDLE             0
#define WE_DLG_DIALOG_HANDLE             1

/*Actions */
#define WE_DLG_OK_ACTION_HANDLE          10
#define WE_DLG_CANCEL_ACTION_HANDLE      11
#define WE_DLG_MENU_ACTION_HANDLE        12

#define WE_DLG_MAX_NR_OF_HANDLES         13

/**********************************************************************
 * Dialog apparents settings
 **********************************************************************/
/*change defined values to customise the dialogs*/
#define WE_DLG_MINIMUM_DIALOG_HEIGHT 30               /*per cent of display height*/ 

#define WE_DLG_PKG_MESSAGE_WIDTH 90                   /*per cent of display width*/
#define WE_DLG_PKG_MESSAGE_SPACE_BETWEEN_MESSAGES 5   /*per cent of display height*/ 
#define WE_DLG_PKG_MESSAGE_HEIGHT 120                 /*Height of the form per cent of the gadgets height*/
#define WE_DLG_PKG_MESSAGE_GADGET_WIDTH 70            /*per cent of form width*/
#define WE_DLG_PKG_MESSAGE_ICON_X_POS 2               /*pixels*/
#define WE_DLG_PKG_MESSAGE_ICON_Y_POS 2               /*pixels*/

#define WE_DLG_PKG_CONFIRM_WIDTH 90                   /*per cent of display width*/
#define WE_DLG_PKG_CONFIRM_SPACE_BETWEEN_MESSAGES 5   /*per cent of display height*/
#define WE_DLG_PKG_CONFIRM_HEIGHT 120                 /*per cent of the gadgets height*/
#define WE_DLG_PKG_CONFIRM_GADGET_WIDTH 70            /*per cent of form width*/

#define WE_DLG_PKG_PROMPT_WIDTH 90                    /*per cent of display width*/
#define WE_DLG_PKG_PROMPT_SPACE_BETWEEN_INPUTFIELDS 5 /*per cent of display height*/
#define WE_DLG_PKG_PROMPT_HEIGHT 120                  /*per cent of the gadgets height*/
#define WE_DLG_PKG_PROMPT_GADGET_WIDTH 70             /*per cent of form width*/

#define WE_DLG_PKG_PROGRESSBAR_WIDTH 90               /*per cent of display width*/
#define WE_DLG_PKG_PROGRESSBAR_HEIGHT 140             /*per cent of the gadgets height*/
#define WE_DLG_PKG_BAR_HEIGHT 10                      /*per cent of display height*/
#define WE_DLG_PKG_BAR_WIDTH 90                       /*per cent of form width*/
#define WE_DLG_PKG_PROGRESSBAR_ONLY_ONE_WIDTH 90      /*per cent of progressbar width*/
#define WE_DLG_PKG_PROGRESSBAR_TEXTONE_WIDTH 40       /*per cent of progressbar width*/
#define WE_DLG_PKG_PROGRESSBAR_TEXTTWO_WIDTH 30       /*per cent of progressbar width*/
#define WE_DLG_PKG_PROGRESSBAR_TEXT_ONE_X_POS 5       /*pixels from right*/
#define WE_DLG_PKG_PROGRESSBAR_TEXT_TWO_X_POS 5       /*pixels from left*/

#define WE_DLG_PKG_LOGIN_WIDTH 90                     /*per cent of display width*/
#define WE_DLG_PKG_LOGIN_SPACE_BETWEEN_INPUTFIELDS 5  /*per cent of the gadgets height*/
#define WE_DLG_PKG_LOGIN_HEIGHT 120                   /*per cent of the gadgets height*/
#define WE_DLG_PKG_LOGIN_GADGET_WIDTH 70              /*per cent of form width*/

#define WE_DLG_PKG_MENU_HEIGHT 80                     /*per cent of display width*/                            
#define WE_DLG_PKG_MENU_WIDTH 90                      /*per cent of display width*/     

#define WE_DLG_PKG_SELECTGROUP_HEIGHT 80              /*per cent of display width*/                            
#define WE_DLG_PKG_SELECTGROUP_WIDTH 90               /*per cent of display width*/     

/**********************************************************************
 * Type definitions
 **********************************************************************/

struct we_dprc_st {
  WE_UINT8   client_modid;
  int         type;
  int         response;
  WE_UINT32  handle[WE_DLG_MAX_NR_OF_HANDLES]; /* handle must be 0 if not used*/
  int         nbr_of_gadgets;
  WE_UINT32  *gadgets;
  WePosition bar_text_pos;
};


/**********************************************************************
 * Local functions
 **********************************************************************/
static we_dprc_t *
we_dprc_dlg_create (WE_UINT8 client_modid, WE_UINT32 screen_handle, int dialog_type);

static int
we_dprc_dlg_create_message (we_dprc_t *dlg, we_dprc_message_t* dlg_data);

static int
we_dprc_dlg_create_confirm (we_dprc_t *dlg, we_dprc_confirm_t* dlg_data);

static int
we_dprc_dlg_create_prompt (we_dprc_t *dlg, we_dprc_prompt_t* dlg_data);

static int
we_dprc_get_prompt_result (we_dprc_t *dlg, we_dprc_prompt_result_t* response_data);

static int
we_dprc_dlg_create_progressbar (we_dprc_t *dlg, we_dprc_progress_bar_t* dlg_data);

static int
we_dprc_dlg_create_login (we_dprc_t *dlg, we_dprc_login_t* dlg_data);

static int
we_dprc_dlg_login_result (we_dprc_t *dlg, we_dprc_login_result_t* response_data);

static int
we_dprc_dlg_create_menu (we_dprc_t *dlg, we_dprc_menu_t* dlg_data);

static int
we_dprc_dlg_menu_result (we_dprc_t *dlg, we_dprc_menu_result_t* response_data);

static int
we_dprc_dlg_create_file (we_dprc_t *dlg, we_dprc_file_t* dlg_data);

static int
we_dprc_dlg_file_result (we_dprc_t *dlg, we_dprc_file_result_t *response_data);


static WE_UINT32
we_dprc_set_title_int (WE_UINT32 handle, WeStringHandle title);


/**********************************************************************
 * Exported functions
 **********************************************************************/

/*
 * Create a dialog.
 * Returns a pointer to a struct we_dprc_st, or NULL if error.
 */
we_dprc_t *
we_dprc_create (WE_UINT8 client_modid, WeScreenHandle screenHandle, int dialog_type, void *dlg_data)
{
  we_dprc_t *dlg = NULL;

  switch (dialog_type) {
  case WE_DLG_MESSAGE_ALERT:
  case WE_DLG_MESSAGE_WARNING:
  case WE_DLG_MESSAGE_INFO:
    /* Fall through */
  case WE_DLG_MESSAGE_ERROR:
    if (((we_dprc_message_t*)dlg_data)->numStrId < 1 || 
        ((we_dprc_message_t*)dlg_data)->str_ids == NULL) {
      return NULL;
    }
    dlg = we_dprc_dlg_create (client_modid, screenHandle, dialog_type);
    we_dprc_dlg_create_message (dlg, (we_dprc_message_t*)dlg_data);
    break;
  case WE_DLG_CONFIRM:
    if (((we_dprc_confirm_t*)dlg_data)->numStrId < 1 || 
        ((we_dprc_confirm_t*)dlg_data)->str_ids == NULL) {
      return NULL;
    }
    dlg = we_dprc_dlg_create (client_modid, screenHandle, dialog_type); 
    we_dprc_dlg_create_confirm (dlg, dlg_data);
    break;
  case WE_DLG_PROMPT:
    if (((we_dprc_prompt_t*)dlg_data)->nbr_of_input_fields < 1 || 
        ((we_dprc_prompt_t*)dlg_data)->input_fields == NULL) {
      return NULL;
    }
    dlg = we_dprc_dlg_create (client_modid, screenHandle, dialog_type); 
    we_dprc_dlg_create_prompt (dlg, dlg_data);    
    break;
  case WE_DLG_PROGRESS_BAR:
    dlg = we_dprc_dlg_create (client_modid, screenHandle, dialog_type); 
    we_dprc_dlg_create_progressbar (dlg, dlg_data); 
   break;
  case WE_DLG_LOGIN:
    if (((we_dprc_login_t*)dlg_data)->input_fields == NULL) {
      return NULL;
    }
    dlg = we_dprc_dlg_create (client_modid, screenHandle, dialog_type); 
    we_dprc_dlg_create_login (dlg, dlg_data);
    break;
  case WE_DLG_MENU:
    if (((we_dprc_menu_t*)dlg_data)->items == NULL) {
      return NULL;
    }
    dlg = we_dprc_dlg_create (client_modid, screenHandle, dialog_type); 
    we_dprc_dlg_create_menu (dlg, dlg_data);
    break;
  case WE_DLG_FILE:
    dlg = we_dprc_dlg_create (client_modid, screenHandle, dialog_type); 
    we_dprc_dlg_create_file (dlg, dlg_data);
    break;
  default:
    break;
  }

  if (dlg != NULL) {
    WE_WIDGET_SET_IN_FOCUS (dlg->handle[WE_DLG_SCREEN_HANDLE], 1);
    WE_WIDGET_SET_IN_FOCUS (dlg->handle[WE_DLG_DIALOG_HANDLE], 1);
  }
  return dlg;
}

/*
 * Delete a dialog. Should be called when the user
 * has responded and all information has been extracted.
 */
void
we_dprc_delete (we_dprc_t *dlg)
{
  int i = 0;

  /* NOTE: first the Dialog window must be removed from the Screen.
   * Then we release the Dialog window. This will cause all other
   * widgets that are attached to the Dialog to be removed from
   * the Dialog window.
   * Also, we must take care not to delete the Screen Handle! */

  for (i = 0; i < dlg->nbr_of_gadgets;i++) {
    WE_WIDGET_REMOVE (dlg->handle[WE_DLG_DIALOG_HANDLE], dlg->gadgets[i]);
    WE_WIDGET_RELEASE (dlg->gadgets[i]);
  }
  WE_MEM_FREE (dlg->client_modid, dlg->gadgets);

  for (i = WE_DLG_MAX_NR_OF_HANDLES - 1; i > 1; i--) {
    if (dlg->handle[i] != 0) {
      WE_WIDGET_REMOVE (dlg->handle[WE_DLG_DIALOG_HANDLE], dlg->handle[i]);
      WE_WIDGET_RELEASE (dlg->handle[i]);
    }
  }
  WE_WIDGET_REMOVE (dlg->handle[WE_DLG_SCREEN_HANDLE],
                      dlg->handle[WE_DLG_DIALOG_HANDLE]);

  WE_WIDGET_RELEASE (dlg->handle[WE_DLG_DIALOG_HANDLE]);
  WE_MEM_FREE(dlg->client_modid, dlg);
}

/*
 * Inform the dialog of the Action notification that was received.
 * 'action' is the WeActionHandle.
 */
void
we_dprc_action (we_dprc_t *dlg, WE_UINT32 action)
{
  if (dlg != NULL) {
    if (dlg->handle[WE_DLG_OK_ACTION_HANDLE] == action) {
      dlg->response = WE_DLG_RESPONSE_POSITIVE;
    }
    else if (dlg->handle[WE_DLG_CANCEL_ACTION_HANDLE] == action) {
      dlg->response = WE_DLG_RESPONSE_CANCEL;
    }
    else if (dlg->handle[WE_DLG_MENU_ACTION_HANDLE] == action) {
      dlg->response = WE_DLG_RESPONSE_MENU;
    }
  }
}

/*
 * Inform the dialog of the user event that was received.
 * 'event' is the WeEvent.
 */
void
we_dprc_user_event (we_dprc_t *dlg, WE_UINT32 event)
{
  WE_UNUSED_PARAMETER (dlg);
  WE_UNUSED_PARAMETER (event); 
}

/*
 * Return true if the handle is the dialog 
 * Returns -1 on error. 
 */
int
we_dprc_check_handle (we_dprc_t *dlg, WE_UINT32 handle)
{
  if (dlg == NULL) {
    return -1;
  }
  return (dlg->handle[WE_DLG_DIALOG_HANDLE] == handle);
}

/*
 * Return the Dialog Handle associated with any action 
 * sent from the specified dialog.
 * Returns 0 on error. 
 */
WE_UINT32
we_dprc_get_dialog_handle (we_dprc_t *dlg)
{
  if (dlg == NULL) {
    return 0;
  }
  return dlg->handle[WE_DLG_DIALOG_HANDLE];
}

/*
 * Return the response that this dialog has received
 * (one of the Response types above).
 * Returns 0 if no response has been registered (by calling we_dprc_action).
 */
int
we_dprc_get_response (we_dprc_t *dlg, void* response_data)
{
  if (dlg == NULL) {
    return 0;
  }

  if (response_data && dlg->response != WE_DLG_RESPONSE_MENU) {
    switch (dlg->type) {
    case WE_DLG_PROMPT:
      we_dprc_get_prompt_result (dlg, response_data);
      break;
    case WE_DLG_LOGIN:
      we_dprc_dlg_login_result (dlg, response_data);
      break;
    case WE_DLG_MENU:
      we_dprc_dlg_menu_result (dlg, response_data);
      break;
    case WE_DLG_FILE:
      we_dprc_dlg_file_result (dlg, response_data);
      break;
    default:
      break;
    }
  }
  return dlg->response;
}

void
we_dprc_set_in_focus(we_dprc_t *dlg)
{
  if (dlg != NULL) {
    WE_WIDGET_SET_IN_FOCUS (dlg->handle[WE_DLG_SCREEN_HANDLE], 1);
    WE_WIDGET_SET_IN_FOCUS (dlg->handle[WE_DLG_DIALOG_HANDLE], 1);
  }
}

void
we_dprc_set_title (we_dprc_t *dlg, WeStringHandle title)
{
  if (dlg != NULL) {
    we_dprc_set_title_int (dlg->handle[WE_DLG_DIALOG_HANDLE], title);
    if (title) {
      WE_WIDGET_WINDOW_SET_PROPERTIES (dlg->handle[WE_DLG_DIALOG_HANDLE], 
                                        WE_WINDOW_PROPERTY_TITLE, 1);
    }
  }
}

void
we_dprc_set_background_image (we_dprc_t *dlg, WeImageHandle image)
{
  WePattern  pattern;

  if (image != 0) {
    pattern.image = image;
    pattern.alignment.horisontalPos = WeCenter;
    pattern.alignment.verticalPos = WeMiddle;
    pattern.padding = WeNoPadding;
  
    WE_WIDGET_SET_PATTERN (dlg->handle[WE_DLG_DIALOG_HANDLE], &pattern, 1);
  }
  else {
    WE_WIDGET_SET_PATTERN (dlg->handle[WE_DLG_DIALOG_HANDLE], NULL, 1);
  }
}

void
we_dprc_event_subscribe (we_dprc_t *dlg,int event_type, int subscribe)
{
  WE_WIDGET_HANDLE_WE_EVT (dlg->handle[WE_DLG_DIALOG_HANDLE], 
                             event_type, TRUE, subscribe);
}


/**********************************************************************
 * Local functions
 **********************************************************************/

static WeSize 
get_device_display_size (void)
{
  WeDisplayProperties display_prop;
  WE_WIDGET_DISPLAY_GET_PROPERTIES (&display_prop);
  return display_prop.displaySize;
}

static int
we_dprc_get_property (we_dprc_pkg_type_t pakage_type)
{
  if (pakage_type == we_dprc_pkg_type_secure) {
    return WE_WINDOW_PROPERTY_SECURE;
  }
  return 0;
}

/*
 * Set title to handle.
 * handle: Widget to set title on
 * title: title to set on widget
 * Return: property to set in widget;
 */
static WE_UINT32
we_dprc_set_title_int (WE_UINT32 handle, WeStringHandle title)
{
  if (title) {
    WE_WIDGET_SET_TITLE (handle, title, 0);
    return WE_WINDOW_PROPERTY_TITLE;
  }
  return 0;
}

static int
we_dprc_dlg_add_action (we_dprc_t *dlg, WE_UINT32 dialog_handle, 
                         WeActionType action, WeStringHandle string)
{
  WeActionHandle action_handle = 0;
  WeStringHandle action_string = string;

  switch (action) {
  case WeOk:
    if (action_string == 0) {
        action_string = WE_WIDGET_STRING_GET_PREDEFINED(WE_STR_ID_DLG_ACTION_OK);
    }
    action_handle = WE_WIDGET_ACTION_CREATE (dlg->client_modid,
                                              action_string, 
                                              0, action);
    WE_WIDGET_ADD_ACTION (dialog_handle, action_handle);
    dlg->handle[WE_DLG_OK_ACTION_HANDLE] = action_handle;
    break;
  case WeCancel:
    if (action_string == 0) {
        action_string = WE_WIDGET_STRING_GET_PREDEFINED(WE_STR_ID_DLG_ACTION_CANCEL);
    }
    action_handle = WE_WIDGET_ACTION_CREATE (dlg->client_modid,
                                              action_string, 
                                              0, action);
    WE_WIDGET_ADD_ACTION (dialog_handle, action_handle);
    dlg->handle[WE_DLG_CANCEL_ACTION_HANDLE] = action_handle;
    break;
  case WeMenu:
    action_handle = WE_WIDGET_ACTION_CREATE (dlg->client_modid,
                                              0, 0, action);
    WE_WIDGET_ADD_ACTION (dialog_handle, action_handle);
    dlg->handle[WE_DLG_MENU_ACTION_HANDLE] = action_handle;
    break;
  default:
    return FALSE;
  }
  return TRUE;
}

static we_dprc_t *
we_dprc_dlg_create (WE_UINT8 client_modid, WE_UINT32 screen_handle, int dialog_type)
{
  int i = 0;

  we_dprc_t *dlg = WE_MEM_ALLOCTYPE (client_modid, we_dprc_t);

  for (i = 0; i < WE_DLG_MAX_NR_OF_HANDLES; i++) {
    dlg->handle[i] = 0;
  }

  dlg->client_modid = client_modid;
  dlg->type = dialog_type;
  dlg->response = 0;
  dlg->handle[WE_DLG_SCREEN_HANDLE] = screen_handle;
  dlg->nbr_of_gadgets = 0;
  dlg->gadgets = NULL;
  dlg->bar_text_pos.x = 0;
  dlg->bar_text_pos.y = 0;
  return dlg;
}

static int
we_dprc_dlg_create_single_message (we_dprc_t *dlg, WeDialogType dlg_type, 
                                    we_dprc_message_t* dlg_data)
{
  WE_UINT32 dialog_handle = 0;
  WePosition null_pos = {0, 0};

  dialog_handle = WE_WIDGET_DIALOG_CREATE (dlg->client_modid, (dlg_data->str_ids)[0], dlg_type, 
                                            we_dprc_get_property(dlg_data->pakage_type));
  
  WE_WIDGET_SET_TITLE (dialog_handle, dlg_data->title, 0);

  /*add ok action*/
  we_dprc_dlg_add_action (dlg, dialog_handle, WeOk, dlg_data->ok_string);

  dlg->handle[WE_DLG_DIALOG_HANDLE] = dialog_handle;

  WE_WIDGET_SCREEN_ADD_WINDOW (dlg->handle[WE_DLG_SCREEN_HANDLE], dialog_handle, &null_pos);
  
  return TRUE; 
}

static int
we_dprc_dlg_create_message (we_dprc_t *dlg, we_dprc_message_t* dlg_data)
{
  WE_UINT32 dialog_handle = 0;
  WePosition gadget_pos = {0, 0};
  WePosition form_pos = {0, 0};
  WePosition icon_pos = {WE_DLG_PKG_MESSAGE_ICON_X_POS, WE_DLG_PKG_MESSAGE_ICON_Y_POS};
  WePosition spec_pos = {0, 0};
  WeSize icon_size = {0, 0};
  WeSize form_size = {0, 0};
  WeSize spec_size = {0, 0};
  WeSize gadget_size = {0, 0};
  WeSize device_size = get_device_display_size ();
  WeDialogType dlg_type = WeInfo;
  WE_UINT32 prop_mask = 0;
  WeImageHandle image_handle = 0;
  WeGadgetHandle image_gadget_handle = 0;
  int i = 0;
  int height_of_gadgets = 0;
  int width_of_gadget = 0;
  int icon_type = 0;
  int space_between_gadgets = 0;

  switch(dlg->type)
  {
  case WE_DLG_MESSAGE_ALERT:
    icon_type = WE_IMAGE_ID_ICON_MESSAGE_ALER;
    dlg_type = WeAlert;
    break;
  case WE_DLG_MESSAGE_WARNING:
    icon_type = WE_IMAGE_ID_ICON_MESSAGE_WARNING;
    dlg_type = WeWarning;
    break;
  case WE_DLG_MESSAGE_INFO:
    icon_type = WE_IMAGE_ID_ICON_MESSAGE_INFO;
    dlg_type = WeInfo;
    break;
  case WE_DLG_MESSAGE_ERROR:
    icon_type = WE_IMAGE_ID_ICON_MESSAGE_ERROR;
    dlg_type = WeError;
    break;
  default:
    return FALSE;
    break;
  }
  
  if (dlg_data->singel_dlg == TRUE)
  {
    /*use a dialog widget*/
    return we_dprc_dlg_create_single_message (dlg, dlg_type, dlg_data);
  }
  space_between_gadgets = (device_size.height * WE_DLG_PKG_MESSAGE_SPACE_BETWEEN_MESSAGES)/100;

  image_handle = WE_WIDGET_IMAGE_GET_PREDEFINED (icon_type, NULL);
  WE_WIDGET_GET_SIZE(image_handle, &icon_size);
  image_gadget_handle = WE_WIDGET_IMAGE_GADGET_CREATE (dlg->client_modid, image_handle, 
                                                        &icon_size, WeImageZoom100, 0, 0);
  WE_WIDGET_RELEASE(image_handle);
  prop_mask = we_dprc_get_property(dlg_data->pakage_type);
  dialog_handle = WE_WIDGET_FORM_CREATE (dlg->client_modid, NULL, prop_mask, 0);
  prop_mask |= we_dprc_set_title_int (dialog_handle, dlg_data->title);
  WE_WIDGET_WINDOW_SET_PROPERTIES (dialog_handle, prop_mask, 0);
  /*Alloc memory for strings and icon*/
  dlg->gadgets = WE_MEM_ALLOC (dlg->client_modid, sizeof(WeGadgetHandle)*(dlg_data->numStrId+1));

  form_size.width = (WE_INT16)((device_size.width * WE_DLG_PKG_MESSAGE_WIDTH)/100);

  width_of_gadget = (WE_INT16)((form_size.width * WE_DLG_PKG_MESSAGE_GADGET_WIDTH) / 100);

  /*Create all string gadgets and calculate form hight*/
  for (i = 0; i<dlg_data->numStrId;i++) {
    (dlg->gadgets)[i] = WE_WIDGET_STRING_GADGET_CREATE (dlg->client_modid, dlg_data->str_ids[i],
                                                         NULL, (WE_INT16)width_of_gadget, FALSE,
                                                         0, 0);
    WE_WIDGET_GET_SIZE (dlg->gadgets[i], &gadget_size);
    height_of_gadgets += gadget_size.height; 
  }
  
  height_of_gadgets += (i-1) * space_between_gadgets;
  /*set height of the form*/
  form_size.height = (WE_INT16) MAX (((height_of_gadgets  * WE_DLG_PKG_MESSAGE_HEIGHT) / 100),
                                      ((device_size.height * WE_DLG_MINIMUM_DIALOG_HEIGHT)/100));

  gadget_pos.y = (WE_INT16)(((form_size.height-height_of_gadgets)/2) + icon_size.height);

  form_size.height = (WE_INT16)(form_size.height +icon_size.height);
  WE_WIDGET_SET_SIZE (dialog_handle, &form_size);

  /*add all string gadgets to form*/
  for (i = 0; i < dlg_data->numStrId; i++) {
    WE_WIDGET_GET_SIZE (dlg->gadgets[i], &gadget_size);
    gadget_pos.x = (WE_INT16)((form_size.width-gadget_size.width)/2);
    WE_WIDGET_WINDOW_ADD_GADGET (dialog_handle, (dlg->gadgets)[i], &gadget_pos);

    gadget_pos.y = (WE_INT16)(gadget_pos.y + gadget_size.height + space_between_gadgets); 
  }
  /*add icon to list*/
  (dlg->gadgets)[i] = image_gadget_handle;
  WE_WIDGET_WINDOW_ADD_GADGET (dialog_handle, (dlg->gadgets)[i], &icon_pos);

  dlg->nbr_of_gadgets = i+1;

  /*Check real size*/
  WE_WIDGET_GET_INSIDE_AREA (dialog_handle, &spec_pos, &spec_size);
  if (form_size.height != spec_size.height ) {
      form_size.height =  (WE_INT16)(form_size.height + (form_size.height-spec_size.height));
  }
  WE_WIDGET_SET_SIZE (dialog_handle, &form_size);
    
  if (form_size.height > device_size.height) {
    prop_mask |= WE_WINDOW_PROPERTY_SCROLLBARVER;
    WE_WIDGET_WINDOW_SET_PROPERTIES (dialog_handle, prop_mask, 0);
    /*Check real size*/
    WE_WIDGET_GET_INSIDE_AREA (dialog_handle, &spec_pos, &spec_size);
    if (form_size.width != spec_size.width ) {
        form_size.width =  (WE_INT16)(form_size.width + (form_size.width-spec_size.width));
    }
    WE_WIDGET_SET_SIZE (dialog_handle, &form_size);
    form_pos.y = 0;
  }
  else {
    form_pos.y = (WE_INT16)((device_size.height-form_size.height)/2);
  }
  form_pos.x = (WE_INT16)((device_size.width-form_size.width)/2);

  /*add ok action*/
  we_dprc_dlg_add_action (dlg, dialog_handle, WeOk, dlg_data->ok_string);
 
  dlg->handle[WE_DLG_DIALOG_HANDLE] = dialog_handle;

  WE_WIDGET_SCREEN_ADD_WINDOW (dlg->handle[WE_DLG_SCREEN_HANDLE],
                                dialog_handle, &form_pos);
 
  return TRUE;
}

static int
we_dprc_dlg_create_singel_confirm (we_dprc_t *dlg, we_dprc_confirm_t* dlg_data)
{
  WE_UINT32 dialog_handle = 0;
  WePosition null_pos = {0, 0};
  WE_UINT32 prop_mask = 0;

  prop_mask = we_dprc_get_property(dlg_data->pakage_type);
  dialog_handle = WE_WIDGET_DIALOG_CREATE (dlg->client_modid, dlg_data->str_ids[0], 
                                            WeConfirmation, prop_mask);

  prop_mask |= we_dprc_set_title_int (dialog_handle, dlg_data->title);
  WE_WIDGET_WINDOW_SET_PROPERTIES (dialog_handle, prop_mask, 0);

  /*add ok action*/
  we_dprc_dlg_add_action (dlg, dialog_handle, WeOk, dlg_data->ok_string);
  /*add cancel action*/
  we_dprc_dlg_add_action (dlg, dialog_handle, WeCancel, dlg_data->cancel_string);

  dlg->handle[WE_DLG_DIALOG_HANDLE] = dialog_handle;

  WE_WIDGET_SCREEN_ADD_WINDOW (dlg->handle[WE_DLG_SCREEN_HANDLE], dialog_handle, &null_pos);
  return TRUE; 
}

static int
we_dprc_dlg_create_confirm (we_dprc_t *dlg, we_dprc_confirm_t* dlg_data)
{
  WE_UINT32 dialog_handle = 0;
  WePosition gadget_pos = {0,0};
  WePosition form_pos = {0, 0};
  WePosition icon_pos = {WE_DLG_PKG_MESSAGE_ICON_X_POS, WE_DLG_PKG_MESSAGE_ICON_Y_POS};
  WePosition spec_pos = {0, 0};
  WeSize icon_size = {0, 0};
  WeSize spec_size = {0, 0};
  WeSize form_size = {0, 0};
  WeSize gadget_size = {0, 0};
  WeSize device_size = get_device_display_size ();
  WE_UINT32 prop_mask = 0;
  WeImageHandle image_handle = 0;
  WeGadgetHandle image_gadget_handle = 0;
  int i = 0;
  int height_of_gadgets = 0;
  int width_of_gadget = 0;

  if (dlg_data->singel_dlg == TRUE) {
    /*Only one message use a dialog widget*/
    return we_dprc_dlg_create_singel_confirm (dlg, dlg_data);
  }

  image_handle = WE_WIDGET_IMAGE_GET_PREDEFINED (WE_IMAGE_ID_ICON_MESSAGE_CONFIRM, NULL);
  WE_WIDGET_GET_SIZE(image_handle, &icon_size);
  image_gadget_handle = WE_WIDGET_IMAGE_GADGET_CREATE (dlg->client_modid, image_handle, 
                                                        &icon_size, WeImageZoom100, 0, 0);
  WE_WIDGET_RELEASE(image_handle);

  /*multiple message*/
  prop_mask = we_dprc_get_property(dlg_data->pakage_type);
  dialog_handle = WE_WIDGET_FORM_CREATE (dlg->client_modid, NULL, prop_mask, 0);
  prop_mask |= we_dprc_set_title_int (dialog_handle, dlg_data->title);
  WE_WIDGET_WINDOW_SET_PROPERTIES (dialog_handle, prop_mask, 0);

  /*Alloc memory for strings and icon*/
  dlg->gadgets = WE_MEM_ALLOC (dlg->client_modid, sizeof(WeGadgetHandle)*(dlg_data->numStrId+1));

  /*set size of form*/
  form_size.width = (WE_INT16)((device_size.width * WE_DLG_PKG_CONFIRM_WIDTH) / 100);

  width_of_gadget = (WE_INT16)((form_size.width * WE_DLG_PKG_CONFIRM_GADGET_WIDTH) / 100);
  /*Create all string gadgets and calculate hight of the gadgets*/
  for (i = 0; i<dlg_data->numStrId; i++) {

    (dlg->gadgets)[i] = WE_WIDGET_STRING_GADGET_CREATE (dlg->client_modid, dlg_data->str_ids[i],
                                                         NULL, (WE_INT16)width_of_gadget, FALSE,
                                                         0, 0);
    WE_WIDGET_GET_SIZE (dlg->gadgets[i], &gadget_size);
    height_of_gadgets += gadget_size.height; 
  }

  /*add spaces between the gadgets*/
  height_of_gadgets += ( (i-1) * (WE_DLG_PKG_CONFIRM_SPACE_BETWEEN_MESSAGES * 
                                  device_size.height) / 100);

  /*set height of the form*/
  form_size.height = (WE_INT16) MAX (((height_of_gadgets  * WE_DLG_PKG_CONFIRM_HEIGHT) / 100),
                                      ((device_size.height * WE_DLG_MINIMUM_DIALOG_HEIGHT)/100));

  gadget_pos.y = (WE_INT16)(((form_size.height-height_of_gadgets)/2) + icon_size.height);

  form_size.height = (WE_INT16)(form_size.height + icon_size.height);
  WE_WIDGET_SET_SIZE (dialog_handle, &form_size);

  /*add all string gadgets to form*/
  for (i = 0; i < dlg_data->numStrId; i++) {
    WE_WIDGET_GET_SIZE (dlg->gadgets[i], &gadget_size);
    gadget_pos.x = (WE_INT16)((form_size.width-gadget_size.width)/2);
    WE_WIDGET_WINDOW_ADD_GADGET (dialog_handle, (dlg->gadgets)[i], &gadget_pos);

    gadget_pos.y = (WE_INT16)(gadget_pos.y + gadget_size.height + 
                   (WE_DLG_PKG_CONFIRM_SPACE_BETWEEN_MESSAGES * device_size.height) / 100); 
  }
  /*add icon to list*/
  (dlg->gadgets)[i] = image_gadget_handle;
  WE_WIDGET_WINDOW_ADD_GADGET (dialog_handle, (dlg->gadgets)[i], &icon_pos);
  dlg->nbr_of_gadgets = i+1;

  /*Check real size*/
  WE_WIDGET_GET_INSIDE_AREA (dialog_handle, &spec_pos, &spec_size);
  if (form_size.height != spec_size.height ) {
      form_size.height =  (WE_INT16)(form_size.height + (form_size.height-spec_size.height));
  }
  WE_WIDGET_SET_SIZE (dialog_handle, &form_size);
  
  /*position and properties for the form.*/ 
  if ( form_size.height > device_size.height) {
    prop_mask |= WE_WINDOW_PROPERTY_SCROLLBARVER;
    WE_WIDGET_WINDOW_SET_PROPERTIES (dialog_handle, prop_mask, 0);
    /*Check real size*/
    WE_WIDGET_GET_INSIDE_AREA (dialog_handle, &spec_pos, &spec_size);
    if (form_size.width != spec_size.width ) {
        form_size.width =  (WE_INT16)(form_size.width + (form_size.width-spec_size.width));
    }
    WE_WIDGET_SET_SIZE (dialog_handle, &form_size);
    form_pos.y = 0;
  }
  else {
    form_pos.y = (WE_INT16)((device_size.height-form_size.height)/2);
  }
  form_pos.x = (WE_INT16)((device_size.width-form_size.width)/2);

  /*add ok action*/
  we_dprc_dlg_add_action (dlg, dialog_handle, WeOk, dlg_data->ok_string);
  /*add cancel action*/
  we_dprc_dlg_add_action (dlg, dialog_handle, WeCancel, dlg_data->cancel_string);

  dlg->handle[WE_DLG_DIALOG_HANDLE] = dialog_handle;

  WE_WIDGET_SCREEN_ADD_WINDOW (dlg->handle[WE_DLG_SCREEN_HANDLE], dialog_handle, &form_pos);
  return TRUE;
}

static int
we_dprc_dlg_create_prompt (we_dprc_t *dlg, we_dprc_prompt_t* dlg_data)
{
  WE_UINT32 dialog_handle = 0;
  WePosition gadget_pos = {0,0};
  WePosition form_pos = {0, 0};
  WePosition spec_pos = {0, 0};
  WeSize form_size = {0, 0};
  WeSize spec_size = {0, 0};
  WeSize gadget_size = {0, 0};
  WeSize device_size = get_device_display_size ();
  WE_UINT32 prop_mask = 0;
  int i = 0;
  int index = 0;
  int height_of_gadgets = 0;
  int width_of_gadget = 0;
 
  prop_mask = we_dprc_get_property(dlg_data->pakage_type);
  dialog_handle = WE_WIDGET_FORM_CREATE (dlg->client_modid, NULL, prop_mask, 0);
  prop_mask |= we_dprc_set_title_int (dialog_handle, dlg_data->title);
  WE_WIDGET_WINDOW_SET_PROPERTIES (dialog_handle, prop_mask, 0);
  /*number of input fields + message text + position used to check if message text is present*/
  dlg->gadgets = WE_MEM_ALLOC (dlg->client_modid, sizeof(WeGadgetHandle) * (dlg_data->nbr_of_input_fields + 2));

  /*set width of form*/
  form_size.width = (WE_INT16)((device_size.width * WE_DLG_PKG_PROMPT_WIDTH) / 100);
  width_of_gadget = (WE_UINT16)((WE_DLG_PKG_PROMPT_GADGET_WIDTH*form_size.width)/100);
  /*Set last in gadget list to 0. Is used to check if message test is present*/
  /*when the result struct is created.                                       */
  dlg->gadgets[dlg_data->nbr_of_input_fields] = 0;
  /*Create string gadget to use for message string*/
  if (dlg_data->message != 0) {
    dlg->gadgets[index++] = WE_WIDGET_STRING_GADGET_CREATE (dlg->client_modid, dlg_data->message,
                                                             NULL, (WE_INT16)width_of_gadget, FALSE, 0, 0);
    WE_WIDGET_GET_SIZE (dlg->gadgets[i], &gadget_size);
    height_of_gadgets += gadget_size.height;
    /*Set last in gadget list to 1. Is used to check if message test is present*/
    /*when the result struct is created.                                       */
    dlg->gadgets[dlg_data->nbr_of_input_fields + 1] = 1;
  }
  /*Create all input gadgets and calculate height of all inputfields*/
  for (i = 0; i<dlg_data->nbr_of_input_fields;i++) {
    /*Any labels for the input fields? */
    dlg->gadgets[index] = WE_WIDGET_TEXT_INPUT_CREATE (dlg->client_modid, dlg_data->input_fields[i].label,
                                                    dlg_data->input_fields[i].default_text,
                                                    dlg_data->input_fields[i].type, 0, 0, 
                                                    dlg_data->input_fields[i].max_size, TRUE, NULL, 
                                                    WE_GADGET_PROPERTY_FOCUS | WE_GADGET_PROPERTY_BORDER, 0);
    WE_WIDGET_GET_SIZE (dlg->gadgets[index], &gadget_size);
    gadget_size.width = (WE_UINT16)width_of_gadget;
    WE_WIDGET_SET_SIZE (dlg->gadgets[index++], &gadget_size);
    height_of_gadgets += gadget_size.height;
  }
  dlg->nbr_of_gadgets = index;
  /*add spaces between the inputfields*/
  height_of_gadgets += ( (index-1) * (WE_DLG_PKG_PROMPT_SPACE_BETWEEN_INPUTFIELDS * device_size.height) / 100);

  /*set size to form*/
  form_size.height = (WE_INT16) MAX (((height_of_gadgets  * WE_DLG_PKG_PROMPT_HEIGHT) / 100),
                                      ((device_size.height * WE_DLG_MINIMUM_DIALOG_HEIGHT)/100));

  gadget_pos.y = (WE_INT16)((form_size.height-height_of_gadgets)/2);
  /*add gadgets to form*/
  for (i = 0; i < dlg->nbr_of_gadgets; i++) {
    /* Set text gadget in center */
    WE_WIDGET_GET_SIZE (dlg->gadgets[i], &gadget_size);
    gadget_pos.x = (WE_INT16)((form_size.width-gadget_size.width)/2);
    /*Add gadget to form*/
    WE_WIDGET_WINDOW_ADD_GADGET (dialog_handle, (dlg->gadgets)[i], &gadget_pos);
    /*position of next gadget*/
    gadget_pos.y = (WE_INT16)(gadget_pos.y + gadget_size.height + (WE_DLG_PKG_PROMPT_SPACE_BETWEEN_INPUTFIELDS * device_size.height) / 100); 
  }
  dlg->nbr_of_gadgets = i;

  WE_WIDGET_SET_SIZE (dialog_handle, &form_size);
  /*Check real size*/
  WE_WIDGET_GET_INSIDE_AREA (dialog_handle, &spec_pos, &spec_size);
  if (form_size.height != spec_size.height ) {
      form_size.height =  (WE_INT16)(form_size.height + (form_size.height-spec_size.height));
  }
  WE_WIDGET_SET_SIZE (dialog_handle, &form_size);

  /*position and properties for the form.*/ 
  if ( form_size.height > device_size.height) {
    prop_mask |= WE_WINDOW_PROPERTY_SCROLLBARVER;
    WE_WIDGET_WINDOW_SET_PROPERTIES (dialog_handle, prop_mask, 0);
    /*Check real size*/
    WE_WIDGET_GET_INSIDE_AREA (dialog_handle, &spec_pos, &spec_size);
    if (form_size.width != spec_size.width ) {
        form_size.width =  (WE_INT16)(form_size.width + (form_size.width-spec_size.width));
    }
    WE_WIDGET_SET_SIZE (dialog_handle, &form_size);
    form_pos.y = 0;
  }
  else {
    form_pos.y = (WE_INT16)((device_size.height-form_size.height)/2);
  }
  form_pos.x = (WE_INT16)((device_size.width-form_size.width)/2);

  /*add ok action*/
  we_dprc_dlg_add_action (dlg, dialog_handle, WeOk, dlg_data->ok_string);
  /*add cancel action*/
  we_dprc_dlg_add_action (dlg, dialog_handle, WeCancel, dlg_data->cancel_string);

  dlg->handle[WE_DLG_DIALOG_HANDLE] = dialog_handle;

  WE_WIDGET_SCREEN_ADD_WINDOW (dlg->handle[WE_DLG_SCREEN_HANDLE], dialog_handle, &form_pos);
  return TRUE;
}

static int
we_dprc_get_prompt_result (we_dprc_t *dlg, we_dprc_prompt_result_t* response_data)
{
  int i = 0;
  int offset = 0;
  /*Check if message text is in gadget list*/
  if (dlg->gadgets[dlg->nbr_of_gadgets] == 1)
  {
     offset = 1;
  }
  response_data->string_list = WE_MEM_ALLOC (dlg->client_modid, 
                                              sizeof (WeStringHandle) * dlg->nbr_of_gadgets-offset);
  for (i = offset; i < dlg->nbr_of_gadgets; i++) {
    (response_data->string_list)[i-offset] =  WE_WIDGET_STRING_CREATE_TEXT (dlg->client_modid, dlg->gadgets[i]);
  }
  response_data->num_of_strings = i-offset;
  return TRUE;
}

static WeStringHandle
we_dprc_dlg_get_count_string( WE_UINT8 modid, we_dprc_progress_bar_t* dlg_data)
{
  char* text = NULL;
  char* nbr_and_text = NULL;
  int nbr = 0;
  int nbr_of_bytes = 0;
  WeStringHandle count_string = 0;

  if (dlg_data->propmask & WE_DLG_PROGRESS_SHOW_TEXT_TWO && dlg_data->text_two) {
    nbr_of_bytes = WE_WIDGET_STRING_GET_LENGTH (dlg_data->text_two,
                                                 TRUE, WeUtf8) + 2;/*+1 for null term and +1 for space */
    text = WE_MEM_ALLOC (modid, nbr_of_bytes);
    text[0] = ' ';
    WE_WIDGET_STRING_GET_DATA (dlg_data->text_two, &text[1], WeUtf8);
    
  }
  else {
    text = WE_MEM_ALLOC (modid, 1);
    text[0] = '\0';  
  }
  nbr_and_text = WE_MEM_ALLOC (modid, nbr_of_bytes + 32);/*+32 for counter*/

  if (dlg_data->propmask & WE_DLG_PROGRESS_PER_CENT) {
    if (dlg_data->max_value) {
      nbr = (dlg_data->value*100) / dlg_data->max_value;
    }
    nbr = MIN(100,nbr);
    sprintf (nbr_and_text, "%d%s", nbr, text);
    count_string = WE_WIDGET_STRING_CREATE (modid, nbr_and_text, WeUtf8, 
                                             strlen(nbr_and_text)+1, 0);
  }   
  else if (dlg_data->propmask & WE_DLG_PROGRESS_NUMBER) {
    sprintf(nbr_and_text, "%d%s", dlg_data->value, text);
    count_string = WE_WIDGET_STRING_CREATE (modid, nbr_and_text, WeUtf8, 
                                             strlen(nbr_and_text)+1, 0);
  }
  else if (dlg_data->propmask & WE_DLG_PROGRESS_SHARE_OF) {
    sprintf(nbr_and_text, "%d/%d%s", dlg_data->value, dlg_data->max_value, text);
    count_string = WE_WIDGET_STRING_CREATE (modid, nbr_and_text, WeUtf8, 
                                            strlen(nbr_and_text)+1, 0);
  }
  else {

    /* This string copy is just to be consistent with the other if statements. */

    WE_WIDGET_STRING_GET_DATA(dlg_data->text_two, nbr_and_text, WeUtf8); 
    count_string = WE_WIDGET_STRING_CREATE (modid, nbr_and_text, WeUtf8, 
                                            strlen(nbr_and_text)+1, 0);
  }
  WE_MEM_FREE (modid, text);
  WE_MEM_FREE (modid, nbr_and_text);
  return count_string;
}

static int
we_dprc_dlg_create_progressbar (we_dprc_t *dlg, we_dprc_progress_bar_t* dlg_data)
{
  WE_UINT32 dialog_handle = 0;
  WeStringHandle count_string = 0;
  WePosition gadget_pos = {0, 0};
  WePosition form_pos = {0, 0};
  WePosition spec_pos = {0, 0};
  WeSize gadget_size = {0, 0};
  WeSize spec_size ={0, 0};
  WeSize bar_size = {0, 0};
  WeSize text_one_size = {0, 0};
  WeSize text_two_size = {0, 0};
  WeSize form_size = {0, 0};
  WeSize device_size = get_device_display_size ();
  WE_UINT32 prop_mask = 0;
  int i = 0;
  int j = 0;
  
  if (!dlg_data->text_one && dlg_data->propmask & WE_DLG_PROGRESS_SHOW_TEXT_ONE ||
      !dlg_data->text_two && dlg_data->propmask & WE_DLG_PROGRESS_SHOW_TEXT_TWO) {
    return FALSE;
  }
  
  form_size.width = (WE_INT16)((device_size.width * WE_DLG_PKG_PROGRESSBAR_WIDTH)/100);

  bar_size.height = (WE_INT16)((device_size.height * WE_DLG_PKG_BAR_HEIGHT) / 100);
  bar_size.width = (WE_INT16)((form_size.width * WE_DLG_PKG_BAR_WIDTH) / 100);

  /*get size for text gadgets*/
  if (dlg_data->text_one && dlg_data->propmask & WE_DLG_PROGRESS_SHOW_TEXT_ONE) {
    if (dlg_data->propmask == WE_DLG_PROGRESS_SHOW_TEXT_ONE) {
      /*Only one string to show set full width*/
      text_one_size.width = (WE_INT16)((form_size.width * 
                                         WE_DLG_PKG_PROGRESSBAR_ONLY_ONE_WIDTH) / 100);
    }
    else {
      text_one_size.width = (WE_INT16)((form_size.width * 
                                         WE_DLG_PKG_PROGRESSBAR_TEXTONE_WIDTH) / 100);
    }
  }
  if (dlg_data->text_two && dlg_data->propmask & WE_DLG_PROGRESS_SHOW_TEXT_TWO) {
    text_two_size.width = (WE_INT16)((form_size.width * 
                                       WE_DLG_PKG_PROGRESSBAR_TEXTTWO_WIDTH) / 100);
  }
  /*create and set property to form*/
  prop_mask = we_dprc_get_property(dlg_data->pakage_type);
  dialog_handle = WE_WIDGET_FORM_CREATE (dlg->client_modid, NULL, prop_mask, 0);
  prop_mask |= we_dprc_set_title_int (dialog_handle, dlg_data->title);
  WE_WIDGET_WINDOW_SET_PROPERTIES (dialog_handle, prop_mask, 0);
  
  dlg->nbr_of_gadgets = 0;
  gadget_size.height = (WE_INT16)(bar_size.height);
  if (dlg_data->propmask & WE_DLG_PROGRESS_SHOW_TEXT_ONE) {
    dlg->nbr_of_gadgets++;
  }
  if (dlg_data->propmask & (WE_DLG_PROGRESS_PER_CENT | 
                           WE_DLG_PROGRESS_NUMBER | 
                           WE_DLG_PROGRESS_SHARE_OF |
                           WE_DLG_PROGRESS_SHOW_TEXT_TWO) ) {
    dlg->nbr_of_gadgets++;
  }

  dlg->nbr_of_gadgets++;
  dlg->gadgets = WE_MEM_ALLOC (dlg->client_modid, sizeof(WeGadgetHandle) * dlg->nbr_of_gadgets);

  if (dlg_data->propmask & WE_DLG_PROGRESS_SHOW_TEXT_ONE) {
    if (!dlg_data->text_one) {
      return FALSE;
    }
    dlg->gadgets[i] = WE_WIDGET_STRING_GADGET_CREATE (dlg->client_modid, dlg_data->text_one,
                                                       NULL, text_one_size.width, FALSE, 0, 0);
    WE_WIDGET_GET_SIZE (dlg->gadgets[i], &text_one_size);
    j = i+1;
  }

  if (dlg_data->propmask & (WE_DLG_PROGRESS_PER_CENT | 
                            WE_DLG_PROGRESS_NUMBER | 
                            WE_DLG_PROGRESS_SHARE_OF | 
                            WE_DLG_PROGRESS_SHOW_TEXT_TWO)) {
    int temp_value = 0;

    temp_value = dlg_data->value;
    dlg_data->value = dlg_data->max_value;
    /* Get max string to calculate size for biggest string*/
    count_string = we_dprc_dlg_get_count_string (dlg->client_modid, dlg_data);
    dlg->gadgets[j] = WE_WIDGET_STRING_GADGET_CREATE (dlg->client_modid, count_string, 
                                                       NULL, text_two_size.width,
                                                       FALSE, 0, 0);
    WE_WIDGET_RELEASE (count_string);
    WE_WIDGET_GET_SIZE (dlg->gadgets[j], &text_two_size);
    /*set start count string to show*/
    dlg_data->value = temp_value;
    count_string = we_dprc_dlg_get_count_string (dlg->client_modid, dlg_data);
    WE_WIDGET_STRING_GADGET_SET (dlg->gadgets[j], count_string, FALSE);
    WE_WIDGET_RELEASE (count_string);
  }

  gadget_size.height = (WE_INT16)(gadget_size.height + 
                                   MAX (text_one_size.height, text_two_size.height));

  if (dlg->nbr_of_gadgets > 1) {
     
    gadget_size.height = (WE_INT16)(gadget_size.height + 10);
  }
  /*set height of the form*/
  form_size.height = (WE_INT16) MAX (((gadget_size.height * WE_DLG_PKG_PROGRESSBAR_HEIGHT) / 100),
                                      ((device_size.height * WE_DLG_MINIMUM_DIALOG_HEIGHT)/100));
  /* Y position of text gadgets.*/ 
  gadget_pos.y = (WE_INT16)((form_size.height-gadget_size.height)/2);
  /*Add text one*/
  if (dlg_data->propmask & WE_DLG_PROGRESS_SHOW_TEXT_ONE) {
    if (!dlg_data->text_one) {
      return FALSE;
    }
    gadget_pos.x = (WE_INT16)(WE_DLG_PKG_PROGRESSBAR_TEXT_ONE_X_POS);
    if (dlg_data->propmask == WE_DLG_PROGRESS_SHOW_TEXT_ONE) {
      gadget_pos.x = (WE_INT16)((form_size.width-text_one_size.width)/2);
    }
    WE_WIDGET_WINDOW_ADD_GADGET (dialog_handle, dlg->gadgets[i++], &gadget_pos);
  }
  /*Add text two and counter*/
  if (dlg_data->propmask & (WE_DLG_PROGRESS_PER_CENT | 
                            WE_DLG_PROGRESS_NUMBER | 
                            WE_DLG_PROGRESS_SHARE_OF | 
                            WE_DLG_PROGRESS_SHOW_TEXT_TWO)) {
    
    gadget_pos.x = (WE_INT16)((form_size.width - text_two_size.width-10));
    /*this gadget has to be added to dlg->gadgets[] second last*/
    WE_WIDGET_WINDOW_ADD_GADGET (dialog_handle, dlg->gadgets[i], &gadget_pos);
    dlg->bar_text_pos = gadget_pos;
    i++;
  }

  /*Create bar gadget this gadget has to be added to dlg->gadgets[] last*/
  dlg->gadgets[i] = WE_WIDGET_BAR_CREATE (dlg->client_modid, WeProgressBar, dlg_data->max_value, 0,
                                             dlg_data->value, &bar_size, 0, 0);
  /* Position of bar gadget.*/
  gadget_pos.y = (WE_INT16)( gadget_pos.y + MAX(text_one_size.height,text_two_size.height));
  if (dlg->nbr_of_gadgets > 1) {
     
    gadget_pos.y = (WE_INT16)( gadget_pos.y + 10);
  }
  gadget_pos.x = (WE_INT16)((form_size.width-bar_size.width)/2);
  WE_WIDGET_WINDOW_ADD_GADGET (dialog_handle, dlg->gadgets[i++], &gadget_pos);

  /*Check real size*/
  WE_WIDGET_SET_SIZE (dialog_handle, &form_size);
  WE_WIDGET_GET_INSIDE_AREA (dialog_handle, &spec_pos, &spec_size);
  if (form_size.height != spec_size.height ) {
      form_size.height =  (WE_INT16)(form_size.height + (form_size.height-spec_size.height));
  }
  WE_WIDGET_SET_SIZE (dialog_handle, &form_size);

  /*position and properties for the form.*/ 
  if ( form_size.height > device_size.height) {
    prop_mask |= WE_WINDOW_PROPERTY_SCROLLBARVER;
    WE_WIDGET_WINDOW_SET_PROPERTIES (dialog_handle, prop_mask, 0);
    /*Check real size*/
    WE_WIDGET_GET_INSIDE_AREA (dialog_handle, &spec_pos, &spec_size);
    if (form_size.width != spec_size.width ) {
        form_size.width =  (WE_INT16)(form_size.width + (form_size.width-spec_size.width));
    }
    WE_WIDGET_SET_SIZE (dialog_handle, &form_size);
    form_pos.y = 0;
  }
  else {
    form_pos.y = (WE_INT16)((device_size.height-form_size.height)/2);
  }
  form_pos.x = (WE_INT16)((device_size.width-form_size.width)/2);
  
  if (!(dlg_data->propmask & WE_DLG_PROGRESS_NOT_CANCEL)) {
    /*add cancel action*/
    we_dprc_dlg_add_action (dlg, dialog_handle, WeCancel, dlg_data->cancel_string);
  }
  dlg->handle[WE_DLG_DIALOG_HANDLE] = dialog_handle;

  WE_WIDGET_SCREEN_ADD_WINDOW (dlg->handle[WE_DLG_SCREEN_HANDLE], dialog_handle, &form_pos);

  return TRUE;
}

int
we_dprc_update_progressbar (we_dprc_t* dlg, we_dprc_progress_bar_t* dlg_data)
{
  WeStringHandle count_string = 0;

  if (dlg_data->propmask & (WE_DLG_PROGRESS_PER_CENT | WE_DLG_PROGRESS_NUMBER | 
                            WE_DLG_PROGRESS_SHARE_OF)) {
    WeSize default_size = {0, 0};
    WeSize new_size = {0, 0};
    int delta_x = 0;
    
    WE_WIDGET_GET_SIZE (dlg->gadgets[dlg->nbr_of_gadgets-2], &default_size);
    count_string = we_dprc_dlg_get_count_string (dlg->client_modid, dlg_data);

    WE_WIDGET_STRING_GADGET_SET (dlg->gadgets[dlg->nbr_of_gadgets-2], count_string, FALSE);    
    WE_WIDGET_RELEASE (count_string);
    WE_WIDGET_GET_SIZE (dlg->gadgets[dlg->nbr_of_gadgets-2], &new_size);
    delta_x = new_size.width - default_size.width;

    dlg->bar_text_pos.x = (WE_INT16)(dlg->bar_text_pos.x - delta_x);

    /* Reset text_pos.x to 0 if negative */
    if (dlg->bar_text_pos.x < 0) {
      dlg->bar_text_pos.x = 0;
    }

    WE_WIDGET_SET_POSITION (dlg->gadgets[dlg->nbr_of_gadgets-2], &dlg->bar_text_pos);
  }
  if (dlg_data->value > dlg_data->max_value) {
    dlg_data->value = dlg_data->max_value;
  }
  return WE_WIDGET_BAR_SET_VALUES (dlg->gadgets[dlg->nbr_of_gadgets-1], 
                                    dlg_data->value, dlg_data->max_value, 0);
}

static int
we_dprc_dlg_create_login (we_dprc_t *dlg, we_dprc_login_t* dlg_data)
{
  WE_UINT32 dialog_handle = 0;
  WePosition gadget_pos = {0, 0};
  WePosition form_pos = {0, 0};
  WePosition spec_pos = {0, 0};
  WeSize spec_size = {0, 0};
  WeSize gadget_size = {0, 0};
  WeSize form_size = {0, 0};
  WeSize device_size = get_device_display_size ();
  WE_UINT32 prop_mask = 0;
  int i = 0;
  int height_of_gadgets = 0;
  WE_UINT16 gadget_width = 0;
  
  prop_mask = we_dprc_get_property(dlg_data->pakage_type);
  dialog_handle = WE_WIDGET_FORM_CREATE (dlg->client_modid, NULL, prop_mask, 0);
  prop_mask |= we_dprc_set_title_int (dialog_handle, dlg_data->title);
  WE_WIDGET_WINDOW_SET_PROPERTIES (dialog_handle, prop_mask, 0);

  /*set size of form*/
  form_size.width = (WE_INT16)((device_size.width * WE_DLG_PKG_LOGIN_WIDTH) / 100);

  /*username, password and selection group*/
  dlg->gadgets = WE_MEM_ALLOC (dlg->client_modid, sizeof(WeGadgetHandle) * 3);
  dlg->nbr_of_gadgets = 3;

  /*First text input*/
  dlg->gadgets[0] = WE_WIDGET_TEXT_INPUT_CREATE (dlg->client_modid, dlg_data->input_fields[0].label, 
                                                  dlg_data->input_fields[0].default_text,
                                                  dlg_data->input_fields[0].type,
                                                  0, 0,
                                                  dlg_data->input_fields[0].max_size,
                                                  TRUE, NULL, 
                                                  WE_GADGET_PROPERTY_BORDER | WE_GADGET_PROPERTY_FOCUS,
                                                  0);
  /*Second text input*/
  dlg->gadgets[1] = WE_WIDGET_TEXT_INPUT_CREATE (dlg->client_modid, dlg_data->input_fields[1].label,
                                                  dlg_data->input_fields[1].default_text,
                                                  dlg_data->input_fields[1].type,
                                                  0, 0,
                                                  dlg_data->input_fields[1].max_size,
                                                  TRUE, NULL, 
                                                  WE_GADGET_PROPERTY_BORDER | WE_GADGET_PROPERTY_FOCUS,
                                                  0);
  /*Selection boxes*/
  if (dlg_data->nbr_of_selection_boxes > 0) {
    dlg->gadgets[2] = WE_WIDGET_SELECTGROUP_CREATE (dlg->client_modid, WeMultipleChoice, 0, 
                                                     0, 
                                                     WE_CHOICE_ELEMENT_STRING_1 | WE_CHOICE_ELEMENT_ICON, 
                                                     WE_GADGET_PROPERTY_FOCUS | WE_GADGET_PROPERTY_BORDER,
                                                     0);
  }

  for (i = 0; i < dlg_data->nbr_of_selection_boxes; i++) {
    WE_WIDGET_CHOICE_SET_ELEMENT (dlg->gadgets[2], i, 0, 
                                   dlg_data->selectboxes[i].select_str_ids, 
                                   0, 0, 0, 0, 0, 0);
    WE_WIDGET_CHOICE_SET_ELEM_STATE (dlg->gadgets[2], i,
                                      dlg_data->selectboxes[i].select_value);
  }
  gadget_width =(WE_UINT16)((WE_DLG_PKG_LOGIN_GADGET_WIDTH*form_size.width)/100); 
  
  WE_WIDGET_GET_SIZE (dlg->gadgets[0], &gadget_size);
  gadget_size.width = gadget_width;
  WE_WIDGET_SET_SIZE (dlg->gadgets[0], &gadget_size);
  height_of_gadgets += gadget_size.height + 
                      (WE_DLG_PKG_LOGIN_SPACE_BETWEEN_INPUTFIELDS * device_size.height) / 100;

  WE_WIDGET_GET_SIZE (dlg->gadgets[1], &gadget_size);
  gadget_size.width = gadget_width;
  WE_WIDGET_SET_SIZE (dlg->gadgets[1], &gadget_size);
  /*space between gadgets*/
  height_of_gadgets += gadget_size.height + 
                      (WE_DLG_PKG_LOGIN_SPACE_BETWEEN_INPUTFIELDS * device_size.height) / 100;

  WE_WIDGET_GET_SIZE (dlg->gadgets[2], &gadget_size);
  gadget_size.width = gadget_width;
  WE_WIDGET_SET_SIZE (dlg->gadgets[2], &gadget_size);
  height_of_gadgets += gadget_size.height;

  form_size.height = (WE_INT16) MAX (((height_of_gadgets * WE_DLG_PKG_LOGIN_HEIGHT) / 100),
                                      ((device_size.height * WE_DLG_MINIMUM_DIALOG_HEIGHT)/100));
  WE_WIDGET_SET_SIZE (dialog_handle, &form_size);

  gadget_pos.y = (WE_INT16)((form_size.height-height_of_gadgets)/2);
  
  for (i = 0; i < 3; i++) {
    /* Set text gadget in center */
    WE_WIDGET_GET_SIZE (dlg->gadgets[i], &gadget_size);
    gadget_pos.x = (WE_INT16)((form_size.width-gadget_size.width)/2);
    /*Add gadget to form*/
    WE_WIDGET_WINDOW_ADD_GADGET (dialog_handle, (dlg->gadgets)[i], &gadget_pos);
    /*position of next gadget*/
    gadget_pos.y = (WE_INT16)(gadget_pos.y + gadget_size.height + 
                              (WE_DLG_PKG_LOGIN_SPACE_BETWEEN_INPUTFIELDS * device_size.height) / 100); 
  }

  /*Check real size*/
  WE_WIDGET_GET_INSIDE_AREA (dialog_handle, &spec_pos, &spec_size);
  if (form_size.height != spec_size.height ) {
      form_size.height =  (WE_INT16)(form_size.height + (form_size.height-spec_size.height));
  }
  WE_WIDGET_SET_SIZE (dialog_handle, &form_size);
  
  if ( form_size.height > device_size.height) {
    prop_mask |= WE_WINDOW_PROPERTY_SCROLLBARVER;
    WE_WIDGET_WINDOW_SET_PROPERTIES (dialog_handle, prop_mask, 0);
    /*Check real size*/
    WE_WIDGET_GET_INSIDE_AREA (dialog_handle, &spec_pos, &spec_size);
    if (form_size.width != spec_size.width ) {
        form_size.width =  (WE_INT16)(form_size.width + (form_size.width-spec_size.width));
    }
    WE_WIDGET_SET_SIZE (dialog_handle, &form_size);
    form_pos.y = 0;
  }
  else {
    form_pos.y = (WE_INT16)((device_size.height-form_size.height)/2);
  }
  form_pos.x = (WE_INT16)((device_size.width - form_size.width) / 2);

  /*add ok action*/
  we_dprc_dlg_add_action (dlg, dialog_handle, WeOk, dlg_data->ok_string);
  /*add cancel action*/
  we_dprc_dlg_add_action (dlg, dialog_handle, WeCancel, dlg_data->cancel_string);

  dlg->handle[WE_DLG_DIALOG_HANDLE] = dialog_handle;

  WE_WIDGET_SCREEN_ADD_WINDOW (dlg->handle[WE_DLG_SCREEN_HANDLE], dialog_handle, &form_pos);

  return TRUE;
}

static int
we_dprc_dlg_login_result (we_dprc_t *dlg, we_dprc_login_result_t* response_data)
{
  int i = 0;
  int nbr_of_elements = 0;
  
  response_data->string_list[0] =  WE_WIDGET_STRING_CREATE_TEXT (dlg->client_modid, dlg->gadgets[0]);
  response_data->string_list[1] =  WE_WIDGET_STRING_CREATE_TEXT (dlg->client_modid, dlg->gadgets[1]);

  nbr_of_elements = WE_WIDGET_CHOICE_SIZE (dlg->gadgets[2]);

  response_data->select_value = WE_MEM_ALLOC ( dlg->client_modid, sizeof (int) * nbr_of_elements);

  response_data->nbr_of_select_value = nbr_of_elements;

  for (i = 0; i < nbr_of_elements; i++) {
    (response_data->select_value)[i] = WE_WIDGET_CHOICE_GET_ELEM_STATE (dlg->gadgets[2], i);
  }
  return TRUE;
}

static int
we_dprc_dlg_create_menu (we_dprc_t *dlg, we_dprc_menu_t* dlg_data)
{
  WeWindowHandle  dialog_handle = 0;
  int              i = 0;
  WeSize          menu_size = {0, 0};
  WeSize          display_size = get_device_display_size ();
  WePosition      menu_pos = {0,0};
  
  menu_size.height = (short) ((display_size.height * WE_DLG_PKG_SELECTGROUP_HEIGHT) / 100); 
  menu_size.width = (short) ((display_size.width * WE_DLG_PKG_SELECTGROUP_WIDTH) / 100);


  dialog_handle =  WE_WIDGET_MENU_CREATE(dlg->client_modid,
                                          dlg_data->type,
                                          &menu_size,
                                          0, /*Element Position*/
                                          WE_CHOICE_ELEMENT_STRING_1, /*Bit Mask*/
                                          0,/* parent menu handle */
                                          0,/* element index in parent menu */
                                          0x8000, /*Property Mask*/
                                          0 /*Default Style*/);
                                          
  if(dlg_data->pakage_type == we_dprc_pkg_type_secure) {
    WE_WIDGET_WINDOW_SET_PROPERTIES(dialog_handle, WE_WINDOW_PROPERTY_SECURE, 0);
  }
  WE_WIDGET_WINDOW_SET_PROPERTIES (dialog_handle, WE_WINDOW_PROPERTY_TITLE, 1);
  WE_WIDGET_SET_TITLE (dialog_handle, dlg_data->title, 0);


  for (i = 0; i < dlg_data->nbr_of_items; i++)
  {
    WE_WIDGET_CHOICE_SET_ELEMENT (dialog_handle, i, 0, 
                                   dlg_data->items[i].string1,
                                   dlg_data->items[i].string2,
                                   dlg_data->items[i].image1,
                                   dlg_data->items[i].image2,
                                   dlg_data->items[i].tooltip_string,
                                   0, 0);
  }

  /*add ok action*/
  we_dprc_dlg_add_action (dlg, dialog_handle, WeOk, dlg_data->ok_string);
  /*add cancel action*/
  we_dprc_dlg_add_action (dlg, dialog_handle, WeCancel, dlg_data->cancel_string);

  menu_pos.y = (WE_INT16)((display_size.height-menu_size.height)/2);
  menu_pos.x = (WE_INT16)((display_size.width-menu_size.width)/2);
  
  dlg->handle[WE_DLG_DIALOG_HANDLE] = dialog_handle;
  WE_WIDGET_SCREEN_ADD_WINDOW (dlg->handle[WE_DLG_SCREEN_HANDLE], dialog_handle, &menu_pos);
  return TRUE;
}

static int
we_dprc_dlg_menu_result (we_dprc_t *dlg, we_dprc_menu_result_t *response_data)
{
  int i = 0;
  int nbr_of_items = 0;
  
  nbr_of_items = WE_WIDGET_CHOICE_SIZE (dlg->handle[WE_DLG_DIALOG_HANDLE]);

  response_data->select_values = WE_MEM_ALLOC (dlg->client_modid, sizeof (int) * nbr_of_items);
  response_data->nbr_of_select_values = nbr_of_items;

  for (i = 0; i < response_data->nbr_of_select_values; i++) {
    (response_data->select_values)[i] = WE_WIDGET_CHOICE_GET_ELEM_STATE (
                                          dlg->handle[WE_DLG_DIALOG_HANDLE], i);
  }
  return TRUE;
}


static int
we_dprc_dlg_create_file (we_dprc_t *dlg, we_dprc_file_t* dlg_data)
{
  WeWindowHandle dialog_handle = 0;
  int             i = 0;
  WeSize         form_size = {0, 0};
  WeSize         display_size = get_device_display_size ();
  WeSize         gadget_size = {0, 0};
  WePosition     gadget_pos = {0, 0};
  WePosition     form_pos = {0, 0};
  WE_UINT32      prop_mask = 0;

  form_size.height = (short) ((display_size.height * WE_DLG_PKG_SELECTGROUP_HEIGHT) / 100); 
  form_size.width = (short) ((display_size.width * WE_DLG_PKG_SELECTGROUP_WIDTH) / 100);

  prop_mask = we_dprc_get_property(dlg_data->pakage_type);
  dialog_handle = WE_WIDGET_FORM_CREATE (dlg->client_modid, &form_size, 
                                          prop_mask, 0);
  prop_mask |= we_dprc_set_title_int (dialog_handle, dlg_data->title);
  WE_WIDGET_WINDOW_SET_PROPERTIES(dialog_handle, prop_mask, 0);

  WE_WIDGET_GET_INSIDE_AREA(dialog_handle, &gadget_pos, &gadget_size);
 
  dlg->gadgets = WE_MEM_ALLOC (dlg->client_modid, sizeof(WeGadgetHandle) * 1);
  dlg->nbr_of_gadgets = 1;
  dlg->gadgets[0] = WE_WIDGET_SELECTGROUP_CREATE (dlg->client_modid,
                                                   dlg_data->type, 
                                                   &gadget_size, 
                                                   0, 
                                                   WE_CHOICE_ELEMENT_STRING_1 | 
                                                   WE_CHOICE_ELEMENT_STRING_2 | 
                                                   WE_CHOICE_ELEMENT_IMAGE_1 |
                                                   WE_CHOICE_ELEMENT_IMAGE_2, 
                                                   0x8000, 
                                                   0);    
  WE_WIDGET_WINDOW_ADD_GADGET (dialog_handle, dlg->gadgets[0], &gadget_pos);

  for (i = 0; i < dlg_data->nbr_of_items; i++)
  {
    WE_WIDGET_CHOICE_SET_ELEMENT (dlg->gadgets[0], i, 0, 
                                   dlg_data->items[i].string1,
                                   dlg_data->items[i].string2,
                                   dlg_data->items[i].image1,
                                   dlg_data->items[i].image2,
                                   dlg_data->items[i].tooltip_string,
                                   0, 0);
  }

  /*add ok action*/
  we_dprc_dlg_add_action (dlg, dialog_handle, WeOk, dlg_data->ok_string);
  /*add cancel action*/
  we_dprc_dlg_add_action (dlg, dialog_handle, WeCancel, dlg_data->cancel_string);
  /*add menu action if a save as dialog*/
  if (!dlg_data->is_open_dialog) {
    we_dprc_dlg_add_action (dlg, dialog_handle, WeMenu, 0);
  }

  form_pos.y = (WE_INT16)((display_size.height-form_size.height)/2);
  form_pos.x = (WE_INT16)((display_size.width-form_size.width)/2);

  dlg->handle[WE_DLG_DIALOG_HANDLE] = dialog_handle;
  WE_WIDGET_SCREEN_ADD_WINDOW (dlg->handle[WE_DLG_SCREEN_HANDLE], dialog_handle, 
                                &form_pos);
  return TRUE;
}

static int
we_dprc_dlg_file_result (we_dprc_t *dlg, we_dprc_file_result_t *response_data)
{
  int i = -1;
  int nbr_of_items = -1;

  if (dlg == NULL) {
    return -1;
  }

  nbr_of_items = WE_WIDGET_CHOICE_SIZE (dlg->gadgets[0]);
  response_data->nbr_of_items = nbr_of_items;
  
  for (i = 0; i < nbr_of_items; i++) {
    if (WE_WIDGET_CHOICE_GET_ELEM_STATE(dlg->gadgets[0], i) &
        WE_CHOICE_ELEMENT_FOCUSED) {
        response_data->selected_item = i;
      return TRUE;
    }
  }
  return FALSE;
}

int
we_dprc_add_item_to_file_dlg (we_dprc_t *dlg, we_dprc_menu_item_t* item_data,
                               int index)
{
  if (dlg == NULL || item_data == NULL) {
    return -1;
  }
  return WE_WIDGET_CHOICE_SET_ELEMENT (dlg->gadgets[0], index, 0, 
                                        item_data->string1,
                                        item_data->string2,
                                        item_data->image1,
                                        item_data->image2,
                                        item_data->tooltip_string,
                                        0, 0);
}

void
we_dprc_clear_file_dlg (we_dprc_t *dlg)
{
  if (dlg) {
    int i = 0;
  
    for (i = WE_WIDGET_CHOICE_SIZE (dlg->gadgets[0])-1; i >= 0; i--) {
      WE_WIDGET_CHOICE_REMOVE_ELEMENT (dlg->gadgets[0],i);  
    }      
  }
}

void
we_dprc_set_file_dlg_item_state (we_dprc_t *dlg, int index, int state)
{
  if (dlg) {
    WE_WIDGET_CHOICE_SET_ELEM_STATE (dlg->gadgets[0], index, state);
  }
}
