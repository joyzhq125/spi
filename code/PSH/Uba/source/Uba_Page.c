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
 * uba_page.c
 *
 * Created by Martin Andersson, 2003/08/22.
 *
 * Revision history:
 * 2003-11-06 (KLHE): TR13466. Now the first page item is highlighted
 *                    when the page is first rendered.
 *
 */

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "Uba_Page.h"

#include "Uba_Menu.h"
#include "Uba_Rce.h"
#include "Uba_Text.h"
#include "Uba_Main.h"
#include "Uba_Fold.h"
#include "Uba_Actn.h"

#include "We_Wid.h"
#include "We_Log.h"

/************************************************************************/
/* Forward declarations                                                 */
/************************************************************************/

static void
uba_page_handle_select_action (uba_page_t* p);

static void
uba_page_render(uba_page_t* p);

static void
uba_page_render_item(uba_page_t* p, int index);

static void
uba_page_re_render(uba_page_t* p);

static void
uba_page_re_render_item(uba_page_t* p, int index);

static void
uba_page_clean_widgets(uba_page_t* p);

static void 
uba_page_attach_to_screen (uba_page_t* p, WeScreenHandle screen);


/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/*
 *	Init the struct p with the data object data and the resource pointer rc.
 *  This will create the GUI representation of rc and data
 */
int
uba_page_init (uba_page_t* p, uba_data_obj_t* data, WeScreenHandle screen,
               const uba_rce_page_t* rc)
{
  int                         elem_state;
  WeDisplayProperties        prop;
  uba_text_t                  a_text;
 
  memset(p,0,sizeof(uba_page_t));

  p->rc = rc;
  p->data = *data;
  
  WE_WIDGET_DISPLAY_GET_PROPERTIES(&prop);

  /* Add the select action with its label */
  uba_text_init (&(p->default_ok_label), &p->data, &(p->rc->default_ok));
  p->select = WE_WIDGET_ACTION_CREATE (WE_MODID_UBA, UBA_TEXT_GET_HANDLE (&(p->default_ok_label)), 
                                        0, WeSelect);
  uba_text_release (&p->default_ok_label);

  /* Add the back action with its label */
  uba_text_init (&a_text, &p->data, &(p->rc->back));
  p->back = WE_WIDGET_ACTION_CREATE (WE_MODID_UBA, UBA_TEXT_GET_HANDLE (&a_text), 
                                      0, WeBack);
  uba_text_release (&a_text);

  p->window = WE_WIDGET_MENU_CREATE (WE_MODID_UBA,
                                      WeImplicitChoice,
	  								                  &prop.displaySize,
                                      0, /*Element Position*/
                                      WE_CHOICE_ELEMENT_STRING_1 | WE_CHOICE_ELEMENT_IMAGE_1, /*Bit Mask*/
                                      0,
                                      0,
                                      0x8000, /*Property Mask*/
                                      0 /*Default Style*/);
  
  WE_WIDGET_ADD_ACTION (p->window, p->select);
  WE_WIDGET_ADD_ACTION (p->window, p->back);

  uba_page_attach_to_screen (p, screen);

  if (data->type == UBA_DATA_OBJECT_FOLDER){
    uba_folder_sort(data->data.folder, UBA_FOLDER_SORT_DEFAULT);
  }
  
  uba_page_render(p);
  
  /* Highlight the first page item */
  if (WE_WIDGET_CHOICE_SIZE (p->window) > 0) {
    elem_state = WE_WIDGET_CHOICE_GET_ELEM_STATE (p->window, 0);
    elem_state |= WE_CHOICE_ELEMENT_SELECTED;
    WE_WIDGET_CHOICE_SET_ELEM_STATE (p->window, 0, elem_state);
  }
  
  p->is_up_and_running = TRUE;
  
  return TRUE; 
}


/*
 *	Releases the resources allocated by p 
 */
void
uba_page_release (uba_page_t* p)
{
  uba_page_clean_widgets (p);
  WE_WIDGET_RELEASE (p->select);
  p->select = 0;
  WE_WIDGET_RELEASE (p->back);
  p->back = 0;
  WE_WIDGET_REMOVE(p->screen, p->window);
  WE_WIDGET_RELEASE (p->window);
  p->window = 0;
}




/*
 *	Set focus to p 
 */
void 
uba_page_show (const uba_page_t* p)
{
  (void)WE_WIDGET_SET_IN_FOCUS (p->window, TRUE);
}


/*
 *	Called from uba_rce to notify widget actions
 */
void
uba_page_handle_widget_action (uba_page_t* p, we_widget_action_t* action)
{
  if (action->action == p->select) {
    /* handle select action */
    uba_page_handle_select_action (p);
  }
  else if (action->action == p->back) {
    /* handle back action */
    uba_rce_remove_page (p);
  }
}

/*
 *	Called for uba_rce to notify that the user presed menu key 
 */
void
uba_page_handle_menu_action (uba_page_t* p)
{
  int   pos = uba_util_get_selected_element (p->window);
  int   menu_id;

  if (pos < 0) {
    return;
  }

  menu_id = uba_page_item_get_menu_id (&(p->items[pos]));
  uba_rce_show_menu (menu_id, UBA_PAGE_ITEM_GET_DATA (&(p->items[pos])));
}

/*
 *	Called from rce to notify that the data data has changed.
 *  event = the type of change
 */
void
uba_page_handle_data_event (uba_page_t *p, const uba_data_obj_t *data, 
                            uba_data_event_t event)
{
  int i            = 0;
  int nbr_of_items ;;
  
  if(!p->is_up_and_running){
    return;
  }
 

  /* Check this item */
  if (uba_data_object_is_equal(&p->data, data))  {
    switch (event)
    {
    case UBA_DATA_EVENT_CHANGE:
      
      uba_page_re_render (p);
      return;
    case UBA_DATA_EVENT_DELETE:
      
      uba_rce_remove_page (p);
      return;
    case UBA_DATA_EVENT_NEW:
      /* should never happen */
      return;
    case UBA_DATA_EVENT_ITEM_ADDED:
      /* rerender page */
      uba_page_re_render (p);
      return;
    case UBA_DATA_EVENT_ITEM_DELETED:
      /* handled in subitems iteration do nothing */
      return;
    }
  }

   /* Check sub items */
  for (nbr_of_items = WE_WIDGET_CHOICE_SIZE (p->window); i < nbr_of_items; ++i) {
    if (uba_data_object_is_equal (&p->items[i].data, data)) {
      switch (event)
      {
      case UBA_DATA_EVENT_CHANGE:
        
        uba_page_re_render_item (p, i);
        return;
      case UBA_DATA_EVENT_DELETE:
        
        uba_page_re_render (p);
        return;
      case UBA_DATA_EVENT_NEW:
        /* should never happen */
        return;
      case UBA_DATA_EVENT_ITEM_ADDED:
        /* rerender item */
        uba_page_re_render_item (p, i);
        return;
      case UBA_DATA_EVENT_ITEM_DELETED:     
        /* rerender item */
        uba_page_re_render (p);
        return;
      }
    }
  }
}


/*
 *	Performs delete action menu item for all page items
 */
void
uba_page_do_delete_all (uba_page_t *p)
{
  int                     i;
  int                     size = WE_WIDGET_CHOICE_SIZE (p->window);

  WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBA, 
  "UBA: page delete all begin\n"));

  for (i = size - 1; i >= 0; --i) {
    if (p->items[i].data.type == UBA_DATA_OBJECT_MSG) {
      uba_action_ubs_delete_msg (&(p->items[i].data), 0, 0, NULL);
    }
  }

  WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_UBA, 
  "UBA: page delete all end\n"));
}


/************************************************************************/
/* Local functions                                                      */
/************************************************************************/

static void 
uba_page_attach_to_screen (uba_page_t* p, WeScreenHandle screen)
{
  WePosition pos = {0,0};
  WE_WIDGET_SCREEN_ADD_WINDOW (screen, p->window, &pos);
  p->screen = screen;
}

static void
uba_page_handle_select_action (uba_page_t* p)
{
  int                     pos = uba_util_get_selected_element (p->window);
  int                     menu_id;
  const uba_rce_action_t  *action;

  if (pos < 0) {
    return;
  }

  menu_id = uba_page_item_get_menu_id (&(p->items[pos]));
  action = uba_menu_get_default_action (uba_rce_get_menu (menu_id));

  if (action) {
    /* if default action exists then select triggers it */
    uba_rce_do_action (action, &(p->items[pos].data), TRUE);
  }
  else {
    /* if no default action exists then select shows the menu */
    uba_rce_show_menu (menu_id, &(p->items[pos].data));
  }
}


static void
uba_page_render(uba_page_t* p)
{
  
  const uba_rce_page_item_t* item;
  const uba_rce_page_item_t* item_end; /* points to one beyound the valid items list */
  unsigned int               pos = 0;
  unsigned int               item_count;

  uba_text_init(&p->title, &p->data, &(p->rc->title));  
  
  WE_WIDGET_SET_TITLE (p->window, UBA_TEXT_GET_HANDLE (&(p->title)), 0);


  WE_WIDGET_WINDOW_SET_PROPERTIES (p->window, WE_WINDOW_PROPERTY_TITLE, 0);

  
  /* Init the page items */
  /* pass 1 count nber of items */

  item = p->rc->items;
  item_count = 0;
  
  while ((item->type != UBA_PAGE_ITEM_TYPE_EMPTY ) && (item_count < UBA_PAGE_ITEM_COUNT)) {
    if (item->type == UBA_PAGE_ITEM_TYPE_MSG_LIST) {
      /* Special case list of msgs */
      
      pos+= uba_folder_get_nbr_of_items (p->data.data.folder);
      
    }
    else {
      ++pos;
    }
    ++item_count;
    ++item;
  }

  
  /* Pass 2 create and insert items */
  
  
  item_end = item; 
  p->items = UBA_MEM_ALLOC (sizeof(uba_page_item_t) * pos );
  pos  = 0;
  
  for (item = p->rc->items; item != item_end;++item) { 

    if (item->type == UBA_PAGE_ITEM_TYPE_MSG_LIST) {
      uba_folder_item_t* msg = uba_folder_get_first_item (p->data.data.folder);
      uba_data_obj_t folder_data;
    
      folder_data.type = UBA_DATA_OBJECT_MSG;
      
      for( ; msg; msg = uba_folder_item_get_next (msg), ++pos) {
        folder_data.data.folder_item = msg;
        uba_page_item_init (&p->items[pos], &folder_data, item);
        uba_page_render_item (p, pos);
      }
  
    }
    else {
      uba_page_item_init (&p->items[pos], &p->data, item);
      uba_page_render_item (p, pos );
      ++pos;
    }
  }  
}

static void
uba_page_re_render(uba_page_t* p)
{
  uba_page_clean_widgets (p);
  uba_page_render(p);
}

static void
uba_page_clean_widgets(uba_page_t* p)
{
  int nbr_of_items = WE_WIDGET_CHOICE_SIZE (p->window);
  int i;

  WE_WIDGET_HOLD_DRAW (p->window);
  
  for (i = 0; i < nbr_of_items; ++i )
  {
    uba_page_item_release (&p->items[i]);
    WE_WIDGET_CHOICE_REMOVE_ELEMENT (p->window,0);
  }

  uba_text_release (&p->title);
  UBA_MEM_FREE (p->items);
  p->items = 0;

  WE_WIDGET_PERFORM_DRAW (p->window);
}


static void
uba_page_re_render_item (uba_page_t* p, int index)
{
  uba_page_item_t           *pi   = &p->items [index];
  const uba_rce_page_item_t *rc   =  UBA_PAGE_ITEM_GET_RC (pi);
  uba_data_obj_t             data = *UBA_PAGE_ITEM_GET_DATA (pi);

  uba_page_item_release (pi);
  uba_page_item_init(pi, &data, rc);
  uba_page_render_item(p, index);


}

static void
uba_page_render_item (uba_page_t* p, int index)
{
 
  WE_WIDGET_CHOICE_SET_ELEMENT (p->window, index, 0,
                               UBA_PAGE_ITEM_GET_TEXT1_HANDLE (&p->items[index]), 0, 
                               uba_page_item_get_icon (&p->items[index]), 0, 
                               0, 0 /* tag */, 0);
}

