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
 * uba_rce.c
 *
 * Created by Martin Andersson, 2003/08/22.
 *
 * Revision history:
 * 2004-02-18 (KLHE): CR14015. Code changed to prevent flickering.
 *
 */

#include "Uba_Rce.h"

#include "Uba_Cfg.h"
#include "Uba_Rc.h"
#include "Uba_Page.h"
#include "Uba_Menu.h"
#include "Uba_Main.h"
#include "Uba_Cust.h"
#include "Uba_Actn.h"

#include "We_Int.h"
#include "We_Wid.h"

/************************************************************************/
/* Constants                                                            */
/************************************************************************/

/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/


 
/* Difrent types of pages/menus */
typedef enum {
  uba_rce_window_type_page,
  uba_rce_window_type_menu,
  uba_rce_window_type_custom_page
} uba_rce_window_type_t;

/* One page/menu struct */
typedef struct uba_rce_window_stack_item_st{
  uba_rce_window_type_t window_type;
  union {
    uba_page_t page;
    uba_menu_t menu;
    uba_custom_page_t custom_page;
  } window;
  struct uba_rce_window_stack_item_st* next;
}uba_rce_window_stack_item_t;

/************************************************************************/
/* Global variables                                                     */
/************************************************************************/

/* Defines the start data for the first page */
static uba_data_obj_t    uba_rce_static_data = {UBA_DATA_OBJECT_STATIC, NULL};
/* Defines the page resources */
extern const uba_rce_page_t * uba_rce_pages[];

/* Defines the menu resources */
extern const uba_rce_menu_t * uba_rce_menus[];


/* The main screen */
static WeScreenHandle uba_rce_screen;
static WeActionHandle uba_rce_menu_action;

/* the window stack the first object is the topmost object */
static uba_rce_window_stack_item_t* uba_rce_window_stack;

/************************************************************************/
/* Forward declarations                                                 */
/************************************************************************/

static void
uba_rce_free_item (uba_rce_window_stack_item_t* p);

static void
uba_rce_remove_item (uba_rce_window_type_t type, const void* item_address);

static void
uba_rce_show_topmost (void);

static int
uba_rce_pop_menu (void);


/************************************************************************/
/* Macros                                                               */
/************************************************************************/

/*
 *	Custom page  macros 
 *  Inparams of the type uba_rce_window_stack_item_t where type = custom page
 */
#define UBA_RCE_CP_SHOW(p) \
  ((p)->window.custom_page.operation->show(&(p)->window.custom_page))

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/*
 * Init the resource engine.
 */
void
uba_rce_init (void)
{
  uba_rce_screen       = 0;
  uba_rce_menu_action  = 0;
  uba_rce_window_stack = NULL;
}

/*
 *	Deallocate resource engine resources
 */
void
uba_rce_release (void)
{
  WE_WIDGET_RELEASE (uba_rce_menu_action);
  WE_WIDGET_RELEASE (uba_rce_screen);
  
}


/* 
 * Start termination of this sub module. Called from main.
 * After terminate done this function cales uba_terminate_rce_done
 */
void
uba_rce_terminate (void)
{
  uba_rce_window_stack_item_t *p; 

  while (uba_rce_window_stack) {
    p = uba_rce_window_stack;
    uba_rce_window_stack = p->next;
    uba_rce_free_item (p);
  }

  uba_rce_release ();
  uba_terminate_rce_done ();
}


/*
 *	Start the gui application according to the resource configuration
 */
int
uba_rce_start (void)
{
#ifndef UBA_GUI_STARTUP_ACTION
  #error UBA_GUI_STARTUP_ACTION must be defined!
#endif

  uba_rce_action_t  startup_action = UBA_GUI_STARTUP_ACTION;
  
  
  uba_rce_screen = WE_WIDGET_SCREEN_CREATE (WE_MODID_UBA, 0);
  WE_WIDGET_DISPLAY_ADD_SCREEN (uba_rce_screen);

  if (!uba_rce_screen) {
    return FALSE;
  }

  uba_rce_menu_action = WE_WIDGET_ACTION_CREATE (WE_MODID_UBA, 
                                        WE_WIDGET_STRING_GET_PREDEFINED (UBA_STR_ID_MENU), 
                                        0, WeMenu);
  WE_WIDGET_ADD_ACTION (uba_rce_screen, uba_rce_menu_action);

#if defined UBA_GUI_STARTUP_FOLDER_DATA
    uba_rce_static_data.type = UBA_DATA_OBJECT_FOLDER;
    uba_rce_static_data.data.folder = uba_data_get_folder (UBA_GUI_STARTUP_FOLDER_DATA);
#endif

  WE_WIDGET_SET_IN_FOCUS (uba_rce_screen, TRUE);

  (startup_action.action) (&uba_rce_static_data, startup_action.i, startup_action.i2,
                            startup_action.str);

  return TRUE;
}


void
uba_rce_handle_widget_action (int src_module, unsigned org_sig, we_widget_action_t* p)
{
  uba_rce_window_stack_item_t *w = uba_rce_window_stack;
  UNREFERENCED_PARAMETER(src_module);
  
  if (!w) {
    WE_SIGNAL_DESTRUCT (WE_MODID_UBA, (WE_UINT16)org_sig, p);
    return;
  }

  if (((w->window_type == uba_rce_window_type_page) && 
       (w->window.page.window == p->handle))) {
    uba_page_handle_widget_action (&w->window.page, p);
  }
  
  else if (((w->window_type == uba_rce_window_type_menu) && 
            (w->window.menu.window == p->handle))) {
    uba_menu_handle_widget_action (&w->window.menu, p);
  }
  
  else if (p->handle == uba_rce_screen && (p->action == uba_rce_menu_action)) {
    if (w->window_type == uba_rce_window_type_page) {
      uba_page_handle_menu_action (&(w->window.page));
    }
  }
  /* Dont offer widget action to custom pages, cp gets signals via handle signal function*/

  WE_SIGNAL_DESTRUCT (WE_MODID_UBA, (WE_UINT16)org_sig, p);
}


/*
 *	Show a page and add it to the window history
 */
void
uba_rce_show_page (int page_id, uba_data_obj_t *data)
{
  uba_rce_window_stack_item_t *page;

  page                 = UBA_MEM_ALLOCTYPE (uba_rce_window_stack_item_t);
  page->window_type    = uba_rce_window_type_page;
  page->next           = uba_rce_window_stack;
  uba_rce_window_stack = page;
  
  uba_page_init (&page->window.page, data, uba_rce_screen, uba_rce_pages[page_id]);
  uba_page_show (&page->window.page);
}


/*
 *	Show a menu and add it to the window history
 */
void
uba_rce_show_menu (int menu_id, uba_data_obj_t *data)
{  
  uba_rce_show_menu_rc (uba_rce_menus[menu_id], data );  
}

void
uba_rce_show_menu_rc (const uba_rce_menu_t* rc, uba_data_obj_t* data)
{
  uba_rce_window_stack_item_t *menu;
  
  menu                 = UBA_MEM_ALLOCTYPE (uba_rce_window_stack_item_t);
  menu->window_type    = uba_rce_window_type_menu;
  menu->next           = uba_rce_window_stack;
  uba_rce_window_stack = menu;

  uba_menu_init (&menu->window.menu, data, uba_rce_screen,rc);
  uba_menu_show (&menu->window.menu);
}

/*
 *	Show a custom page
 */
void
uba_rce_show_custom_page (int custom_page_id, uba_data_obj_t* data, int i)
{
  
  uba_rce_window_stack_item_t *cp;
  cp = UBA_MEM_ALLOCTYPE (uba_rce_window_stack_item_t);
  cp->window_type = uba_rce_window_type_custom_page;
  cp->next = uba_rce_window_stack;
  uba_rce_window_stack = cp;
  uba_custom_page_init (&cp->window.custom_page, custom_page_id, data, uba_rce_screen, i);
  
  
  UBA_RCE_CP_SHOW (cp); 
}



/* removes the page p from the page stack*/
void
uba_rce_remove_page (struct uba_page_st* p)
{
  uba_rce_remove_item (uba_rce_window_type_page, p); 
}

/*
 *	Removes the menu p form the page stack
 */
void
uba_rce_remove_menu (struct uba_menu_st* p)
{
  uba_rce_remove_item (uba_rce_window_type_menu, p);
}

void
uba_rce_remove_custom_page (uba_custom_page_t* p)
{
  uba_rce_remove_item (uba_rce_window_type_custom_page, p);
}



/*
 *	Executes the action with action_id and forwards the parameters
 *  data, i and str
 */
void
uba_rce_do_action (const uba_rce_action_t *action, 
                   uba_data_obj_t         *data, 
                   int                     is_default_action)
{
  uba_data_obj_t tmp;
 
  if (!action) {
    return;
  }

  if (action->pop_menu && !is_default_action){
    /* may invalidate the data object pointer so make a new one */
    tmp  = *data;
    data = &tmp;
    if (uba_rce_pop_menu ()>0) {
      /* sett focus to topmost i menus poped*/
      uba_rce_show_topmost ();
    }
  }
  
  if (action->action) {
    action->action(data, action->i, action->i2, action->str);
  }
}



/*
 *	Gets the menu struct from a wid
 */
const uba_rce_menu_t*
uba_rce_get_menu (int menu_id)
{
  return uba_rce_menus[menu_id];
}


struct uba_page_st*
uba_rce_get_top_page (void)
{
  uba_rce_window_stack_item_t *p = uba_rce_window_stack;

  while (p && (p->window_type != uba_rce_window_type_page)) {
    p = p->next;
  }

  return (p) ? (&(p->window.page)) : (NULL);
}


void
uba_rce_on_data_event (const uba_data_obj_t *data, uba_data_event_t event)
{
  uba_rce_window_stack_item_t* item = uba_rce_window_stack;
  uba_rce_window_stack_item_t* next;
  
  while (item){
    /* Save next pointer before offering data evet as the evet may trigger the 
     * page/menu/cp to remove itself
     */
    next = item->next;
  
    switch(item->window_type){
    
    case uba_rce_window_type_page:
      uba_page_handle_data_event (&item->window.page, data, event);
      break;
    
    case uba_rce_window_type_menu:
      uba_menu_handle_data_event (&item->window.menu, data, event);
      break;
    
    case uba_rce_window_type_custom_page:
      item->window.custom_page.operation->data_event(&item->window.custom_page, data, event);
      break;
    }
    item = next;
  }
}


/* Offers the signal to the custom page, returns true if the cp uses the signal */
int
uba_rce_offer_custom_page_signal (uba_signal_t *sig)
{
  uba_rce_window_stack_item_t *item = uba_rce_window_stack;
  
  while (item) {
   if (item->window_type == uba_rce_window_type_custom_page && 
       item->window.custom_page.operation->handle_signal(&item->window.custom_page, sig)){
     return TRUE;
   }
   item = item->next;
  }

  return FALSE;
}


/************************************************************************/
/* Util functions (Exported functions)                                  */
/************************************************************************/

int
uba_util_get_selected_element (WeWindowHandle choice_handle)
{
  int i = 0;
  int nbr_of_items = WE_WIDGET_CHOICE_SIZE (choice_handle);

  if (nbr_of_items <= 0) {
    return -1;
  }

  while ( (i < nbr_of_items) && 
          (!(WE_WIDGET_CHOICE_GET_ELEM_STATE (choice_handle, i) & 
          WE_CHOICE_ELEMENT_SELECTED))) {
    ++i;
  }
  return (i < nbr_of_items) ? (i) : (-1);
}

/************************************************************************/
/* Local functions                                                      */
/************************************************************************/

/*
 *	Release the item p points to and frees the memory p points to
 */
static void
uba_rce_free_item (uba_rce_window_stack_item_t* p)
{
  switch (p->window_type)
  {
  case uba_rce_window_type_menu:
    uba_menu_release (&p->window.menu);
    break;
  
  case uba_rce_window_type_page:
    uba_page_release (&p->window.page);
    break;
  
  case uba_rce_window_type_custom_page:
    p->window.custom_page.operation->release(&p->window.custom_page);
    break;
  }
  
  UBA_MEM_FREE (p);
}


static void
uba_rce_remove_item(uba_rce_window_type_t type, const void* item_address)
{
  uba_rce_window_stack_item_t* item = uba_rce_window_stack;
  uba_rce_window_stack_item_t* prev = NULL; /* to avoid null check */
  
  while (item) {
    if ((item->window_type == type) && (&item->window == item_address)){
      /* item found remove from stack */
      if (prev) {
        /* Page is not first */
        prev->next = item->next;
        uba_rce_free_item (item);
      }
      else {
        /* topmost item */
        uba_rce_window_stack = item->next;
        
        if (uba_rce_window_stack){
          uba_rce_show_topmost ();
          /* CR14015
          Free item after showing the item to be on top prevents the screen from 
          flickering when the widgets on the removed item are released */
          uba_rce_free_item (item);
        }
        else{
          /* NOTE! The item must be freed before termination is started */
          uba_rce_free_item (item);
          uba_start_termination ();
        }
      }   
      /* item removed no need to continue return */
      return;
    }
    prev = item;
    item = item->next;
  }
}

static void
uba_rce_show_topmost()
{
  switch(uba_rce_window_stack->window_type) {

  case uba_rce_window_type_page:
    uba_page_show (&uba_rce_window_stack->window.page);
    break;

  case uba_rce_window_type_menu:
    uba_menu_show (&uba_rce_window_stack->window.menu);
    break;
  
  case uba_rce_window_type_custom_page:
    UBA_RCE_CP_SHOW(uba_rce_window_stack);
    break;
  }
}

/*
 * Pops topmost menus and return the number of menus poped
 */
static int
uba_rce_pop_menu (void)
{
  int i = 0;
  uba_rce_window_stack_item_t* item;
  while (uba_rce_window_stack && 
         uba_rce_window_stack->window_type == uba_rce_window_type_menu){
    
    item = uba_rce_window_stack;
    ++i;
    uba_rce_window_stack = item->next;
    uba_rce_free_item (item);
  }
  return i;
}

