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
 * uba_vmai.c
 * 
 * Description:
 *    This file contains code to set voice mail number
 *
 * Created by Kolbjörn Gripne, 2004/02/19.
 *
 * Revision history:
 *
 * 2004-03-03 (KOGR): Adapted to Widget API Q04A1
 *
 */

#include "Uba_Vmai.h"

#include "Uba_Main.h"
#include "Uba_Rce.h"
#include "Uba_Rc.h"
#include "Uba_Data.h"

#include "We_Wid.h"
#include "We_Log.h"

/************************************************************************/
/* Exported custom page variable                                          */
/************************************************************************/

const uba_custom_page_func_t uba_custom_page_voice_mail_set = 
{
  uba_voice_mail_set_init,             
  uba_voice_mail_set_release,          
  uba_voice_mail_set_handle_sig,       
  uba_voice_mail_set_handle_data_event,
  uba_voice_mail_set_show              
};

/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/

typedef struct {
    WeWindowHandle   window;
    WeActionHandle   accept;
    WeActionHandle   back;
    WeScreenHandle   screen;      
} uba_voice_mail_set_t;

/*
 *	Max number of UTF8 characters in voice mail number
 */
#define UBA_VOICE_MAIL_MAX_VOICE_MAIL_NUM_LENGTH  30

/*
 *  Widget definitions
 */
#define UBA_DEVICE_DEFAULT_PROPERTIES  0x8000
#define UBA_DEVICE_DEFAULT_STYLE       0

/************************************************************************/
/* Forward decl internal functions                                      */
/************************************************************************/
static int 
uba_voice_mail_set_handle_widget_action (uba_custom_page_t   *p, 
                                         unsigned             org_sig, 
                                         we_widget_action_t *action);

/************************************************************************/
/* Exported functions                                                    */
/************************************************************************/
/*
 * Init the custom page, data must point to a folder object
 * Return: TRUE = successful
 */
int
uba_voice_mail_set_init (uba_custom_page_t *p, 
                         uba_data_obj_t    *data,
                         WeScreenHandle    screen,
                         int                i)
{
  uba_voice_mail_set_t* dlg;
  WeDisplayProperties  prop;
	WePosition	          pos =  {0, 0};
  WeStringHandle       we_str;
  const char*           str;

  UNREFERENCED_PARAMETER (i);

  if (UBA_DATA_OBJECT_STATIC != data->type) {
    WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBA,
                  "UBA WARNING, uba_voice_mail_set_init: no data expected\n"));
  }

  p->usr_data = UBA_MEM_ALLOCTYPE(uba_voice_mail_set_t);

  dlg = (uba_voice_mail_set_t*)p->usr_data;
  dlg->screen = screen;

  WE_WIDGET_DISPLAY_GET_PROPERTIES(&prop);

  str = uba_data_get_voice_mail_num();
  
  if (NULL != str){
    we_str = WE_WIDGET_STRING_CREATE (WE_MODID_UBA, str, WeUtf8,
                                        strlen(str)+1, 0);
  }
  else{
    we_str = WE_WIDGET_STRING_GET_PREDEFINED (UBA_STR_ID_NO_STR);
  }

  dlg->window = WE_WIDGET_EDITOR_CREATE (WE_MODID_UBA,
                      WE_WIDGET_STRING_GET_PREDEFINED(UBA_STR_ID_SET_VOICE_MAIL),
                      we_str,
                      WeName,
                      0, 
                      0,
                      UBA_VOICE_MAIL_MAX_VOICE_MAIL_NUM_LENGTH,
                      TRUE,
                      NULL,
                      UBA_DEVICE_DEFAULT_PROPERTIES,
                      UBA_DEVICE_DEFAULT_STYLE);

  WE_WIDGET_RELEASE(we_str);

	dlg->accept = WE_WIDGET_ACTION_CREATE (WE_MODID_UBA, 
                                          WE_WIDGET_STRING_GET_PREDEFINED 
                                          (UBA_STR_ID_ACCEPT),
                                          0, WeSelect);
	dlg->back   = WE_WIDGET_ACTION_CREATE (WE_MODID_UBA, 
                                          WE_WIDGET_STRING_GET_PREDEFINED
                                          (UBA_STR_ID_BACK),
                                          0, WeBack);
	
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
uba_voice_mail_set_release(uba_custom_page_t *p)
{
  uba_voice_mail_set_t* dlg = (uba_voice_mail_set_t*)p->usr_data;
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
uba_voice_mail_set_handle_sig (uba_custom_page_t *p, 
                               uba_signal_t      *sig)
{
  switch (sig->type)
  {
  case UBA_MAIN_SIG_WIDGET_ACTION:
    return uba_voice_mail_set_handle_widget_action (p, sig->u_param1, 
                                            (we_widget_action_t*)sig->p_param);
  default:
    return FALSE;
  }
}

/*
 *	Some data has changed
 */
void 
uba_voice_mail_set_handle_data_event (uba_custom_page_t    *p, 
                                      const uba_data_obj_t *data,
                                      uba_data_event_t      event)
{
  /* Do nothing - no simultaneous change of the registry is expected, e.g. by
   * a user dialogue carried through by another module */
  UNREFERENCED_PARAMETER (p);
  UNREFERENCED_PARAMETER (data);
  UNREFERENCED_PARAMETER (event);
}

/*
 *	View this custom page
 */
void 
uba_voice_mail_set_show (uba_custom_page_t* p)
{
  uba_voice_mail_set_t* dlg = (uba_voice_mail_set_t*)p->usr_data;
  (void)WE_WIDGET_SET_IN_FOCUS (dlg->window, TRUE);  
}


/************************************************************************/
/* Internal functions                                                   */
/************************************************************************/

static int 
uba_voice_mail_set_handle_widget_action (uba_custom_page_t *p, 
                                         unsigned org_sig, 
                                         we_widget_action_t *action)
{
 uba_voice_mail_set_t* dlg = (uba_voice_mail_set_t*)p->usr_data;
 if(action->handle != dlg->window){
   return FALSE;
 }

 if(action->action == dlg->back){
   uba_rce_remove_custom_page (p);
   WE_SIGNAL_DESTRUCT (WE_MODID_UBA, (WE_UINT16)org_sig, action);  
   return TRUE;
 }

 if (action->action == dlg->accept){
   WeStringHandle we_str = WE_WIDGET_STRING_CREATE_TEXT (WE_MODID_UBA,
                                                            dlg->window);
   int             str_len = WE_WIDGET_STRING_GET_LENGTH (we_str, TRUE, WeUtf8);
   char           *str     = UBA_MEM_ALLOC (str_len);

   WE_WIDGET_STRING_GET_DATA (we_str, str, WeUtf8);
   uba_data_set_voice_mail_num (str, TRUE);

   UBA_MEM_FREE (str);
   WE_WIDGET_RELEASE (we_str);

   uba_rce_remove_custom_page (p);
   WE_SIGNAL_DESTRUCT (WE_MODID_UBA, (WE_UINT16)org_sig, action);
   return TRUE;
 }

 return FALSE;
}

