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
 * uba_menu.c
 *
 * Created by Klas Hermodsson, 2003/08/22.
 *
 * Revision history:
 *
 */

#include "Uba_Menu.h"

#include "Uba_Rce.h"
#include "Uba_Text.h"
#include "Uba_Rc.h"

#include "We_Wid.h"

/************************************************************************/
/* Forward declaration                                                  */
/************************************************************************/

static void
uba_menu_handle_select_action (uba_menu_t* p);

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/*
 *	Init the menu and create the appropriate resources
 */
int
uba_menu_init (uba_menu_t* p, uba_data_obj_t* data, 
               WeScreenHandle screen, const uba_rce_menu_t* rc)
{
  WePosition                 we_pos = {0,0};
  WeDisplayProperties        prop;
  const uba_rce_menu_item_t*  item;
  uba_text_t                  text;
  int                         pos = 0;

  p->data  = *data;
  p->rc = rc;
  p->screen = screen;

  WE_WIDGET_DISPLAY_GET_PROPERTIES(&prop);


  /* Add the select action with its label */
  uba_text_init (&text, data, &(rc->ok));
  p->select = WE_WIDGET_ACTION_CREATE (WE_MODID_UBA, 
                                        UBA_TEXT_GET_HANDLE (&text), 
                                        0, WeSelect);
  uba_text_release (&text);

  
  p->window = WE_WIDGET_MENU_CREATE(WE_MODID_UBA,
                                     WeImplicitChoice,
	  								                 &prop.displaySize,
                                     0, /*Element Position*/
                                     WE_CHOICE_ELEMENT_STRING_1 , 
                                     0, 
                                     0, 
                                     0x8000, /*Property Mask*/
                                     0 /*Default Style*/);

  WE_WIDGET_ADD_ACTION (p->window, p->select);

  uba_text_init(&text, data, &(rc->title));
  WE_WIDGET_SET_TITLE (p->window, UBA_TEXT_GET_HANDLE(&(text)), 0);
  uba_text_release (&text);

  
  /* Add the back action with its label */
  uba_text_init (&text, data, &(rc->back));
  p->back = WE_WIDGET_ACTION_CREATE (WE_MODID_UBA, 
                                      UBA_TEXT_GET_HANDLE (&text), 
                                      0, WeBack);
  uba_text_release (&text);
  WE_WIDGET_ADD_ACTION (p->window, p->back);

  WE_WIDGET_WINDOW_SET_PROPERTIES (p->window, WE_WINDOW_PROPERTY_TITLE, 0);

  item = rc->items;
  while (item->str_id != UBA_STR_ID_NO_STR && pos < UBA_MENU_ITEM_COUNT)
  {
    WE_WIDGET_CHOICE_SET_ELEMENT (p->window, pos, 0,
                                   WE_WIDGET_STRING_GET_PREDEFINED(item->str_id), 
                                   0, 0, 0, 0, 0 /* tag */, 0);
    ++pos;
    ++item;
  }

  WE_WIDGET_SCREEN_ADD_WINDOW(screen, p->window, &we_pos);
  
  
  return TRUE; 
}


void
uba_menu_release (uba_menu_t* p)
{
  WE_WIDGET_REMOVE  (p->screen, p->window);
  WE_WIDGET_RELEASE (p->back);
  WE_WIDGET_RELEASE (p->select);
  WE_WIDGET_RELEASE (p->window);
}



void 
uba_menu_show (uba_menu_t* p)
{
  (void)WE_WIDGET_SET_IN_FOCUS (p->window, TRUE);
}


void
uba_menu_handle_widget_action (uba_menu_t* p, we_widget_action_t* action)
{
  if (action->action == p->select) {
    /* handle select action */
    uba_menu_handle_select_action (p);
  }
  else if (action->action == p->back) {
    /* handle back action */
    uba_rce_remove_menu (p);
  }
}


const uba_rce_action_t*
uba_menu_get_default_action (const uba_rce_menu_t* rc)
{
  if (rc->default_item == UBA_MENU_NO_DEFAULT_ITEM) {
    return NULL;
  }

  return &((rc->items[rc->default_item]).action_rc);
}


void
uba_menu_handle_data_event (uba_menu_t *p, const uba_data_obj_t *data, 
                            uba_data_event_t event)
{
  if (!uba_data_object_is_equal(&p->data, data))  {
    return;
  }
  switch (event)
  {
  case UBA_DATA_EVENT_CHANGE:
    
    if (data->type == UBA_DATA_OBJECT_MSG){
      uba_rce_remove_menu (p);
    }    
    break;
  
  case UBA_DATA_EVENT_DELETE:
    
    uba_rce_remove_menu (p);
    break;
  
  case UBA_DATA_EVENT_NEW:
    /* should never happen */
    break;
  
  case UBA_DATA_EVENT_ITEM_ADDED:
    /* No action */
  
  case UBA_DATA_EVENT_ITEM_DELETED:
    /* No action */
    break;
  }
}

/************************************************************************/
/* Local functions                                                      */
/************************************************************************/

static void
uba_menu_handle_select_action (uba_menu_t* p)
{
  int pos = uba_util_get_selected_element (p->window);

  if (pos < 0) {
    return;
  }

  uba_rce_do_action (&(p->rc->items[pos].action_rc), &(p->data), FALSE);
}

