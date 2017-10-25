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
 * uba_rce.h
 *
 *
 * The resource engine.
 * 
 *
 *
 * Revision history:
 *
 */

#ifndef UBA_RCE_H
#define UBA_RCE_H

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef UBA_CFG_H
  #include "Uba_Cfg.h"
#endif 

#ifndef _we_core_h
  #include "We_Core.h"
#endif

#ifndef UBA_DATA_H
  #include "Uba_Data.h"
#endif

#ifndef UBA_SIG_H
  #include "Uba_Sig.h"
#endif

#ifndef UBA_GUI_H
  #include "Uba_Gui.h"
#endif

/************************************************************************/
/* Forward declarations                                                 */
/************************************************************************/

/* To present uba_rce_remove_page(struct uba_page_st* p); */
struct uba_page_st;

struct uba_menu_st;

struct uba_custom_page_st;

/************************************************************************/
/* GUI RC help macros                                                   */
/************************************************************************/

/******************************************
 *  Text helpers
 */
/* Use this to define a text */
#define UBA_TEXT_COMPLETE(type,wid) {type, wid}

/* A text from a resource string */
#define UBA_TEXT_RC(wid)                             UBA_TEXT_COMPLETE(UBA_TEXT_TYPE_RC, wid)
/* 
 * Text that takes the name from the message data, for this to work, the texts
 * data object must be of the type UBA_DATA_OBJECT_MSG
 */
#define UBA_TEXT_MSG                                UBA_TEXT_COMPLETE(UBA_TEXT_TYPE_MSG, UBA_STR_ID_NO_STR)
/* 
 * Text that takes the name from the folder name, for this to work, the texts
 * data object must be of the type UBA_DATA_OBJECT_FOLDER
 */
#define UBA_TEXT_FOLDER_NAME                        UBA_TEXT_COMPLETE(UBA_TEXT_TYPE_FOLDER, UBA_STR_ID_NO_STR)
#define UBA_TEXT_FOLDER_NAME_PLUS_TOTAL             UBA_TEXT_COMPLETE(UBA_TEXT_TYPE_FOLDER_PLUS_TOTAL_COUNT, UBA_STR_ID_NO_STR)
#define UBA_TEXT_FOLDER_NAME_PLUS_UNREAD            UBA_TEXT_COMPLETE(UBA_TEXT_TYPE_FOLDER_PLUS_UNREAD_COUNT, UBA_STR_ID_NO_STR)
#define UBA_TEXT_FOLDER_NAME_PLUS_UNREAD_AND_TOTAL  UBA_TEXT_COMPLETE(UBA_TEXT_TYPE_FOLDER_PLUS_UNREAD_AND_TOTAL_COUNT, UBA_STR_ID_NO_STR)
/* No text is used	*/
#define UBA_TEXT_NO_TEXT                            UBA_TEXT_COMPLETE(UBA_TEXT_TYPE_RC, UBA_STR_ID_NO_STR)

/******************************************
 *  Action helpers
 */
/* 
 * Action parameters
 * actionid  function pointer to an action function
 * intparam  integer param to be sent to the action
 * intparam2 second integer param to be sent to the action
 * strparam  string param to be sent to the action
 * popmenu   TRUE if the menu should be popped else FALSE
 */
#define UBA_ACTION(action, intparam, intparam2, strparam, popmenu) \
  { action, intparam, intparam2, strparam, popmenu }

 
/******************************************
 *  Menu helpers
 */
/* Defines the title, text MUST be on the form of UBA_TEXT_COMPLETE */
#define UBA_MENU_TITLE(text)            text
/* Defines the label for an action on the menu */
#define UBA_MENU_ACTION_LABEL(text)     text
/* Defines the index of the default menu item */
#define UBA_MENU_DEFAULT_ITEM(index)    (index)
#define UBA_MENU_NO_DEFAULT_ITEM        (-1)
/*
 * A menu item.
 * str_id is a predefined string resource
 * Action is the full action as in the macro UBA_ACTION
 */
#define UBA_MENU_ITEM(str_id, action)   {str_id, action}
/*
 *  Defines an empty non selectable menu item
 */
#define UBA_MENU_ITEM_EMPTY             {UBA_STR_ID_NO_STR, NULL }

/******************************************
 *  Page helpers
 */
/* Defines the title, text MUST be on the form of UBA_TEXT_COMPLETE */
#define UBA_PAGE_TITLE(text)            text
/* Defines the label for an action on the page */
#define UBA_PAGE_ACTION_LABEL(text)     text

/* A full page item with all parameters exposed */
#define UBA_PAGE_ITEM_COMPLETE( type, text, menu_id, icon_id, int_val)  \
  { type, text, menu_id, icon_id, int_val }

/* Normal page item */
#define UBA_PAGE_ITEM(text, menu_id, icon_id) \
  { UBA_PAGE_ITEM_TYPE_STATIC,text,menu_id,icon_id,0 }

/* Define an page item of the type folder */
#define UBA_PAGE_ITEM_FOLDER(text, menu_id, icon_id, folder_id) \
  { UBA_PAGE_ITEM_TYPE_FOLDER, text, menu_id, icon_id, folder_id }

/* Defines that this page contains/handles msgs of msg_type */
#define UBA_PAGE_ITEM_MSG_LIST \
  { UBA_PAGE_ITEM_TYPE_MSG_LIST, UBA_TEXT_MSG, 0, UBA_IMG_ID_EMPTY, 0 }

/* Page item to fill up the page list to UBA_PAGE_ITEM_COUNT */
#define UBA_PAGE_ITEM_EMPTY \
  { UBA_PAGE_ITEM_TYPE_EMPTY ,UBA_TEXT_NO_TEXT,-1,UBA_IMG_ID_EMPTY,-1 }

/************************************************************************/
/* Typedefs                                                             */
/************************************************************************/

/* Enumerates diffrent types of page items */
typedef enum
{
  UBA_PAGE_ITEM_TYPE_STATIC,    /* Static page item, not data dependant */
  UBA_PAGE_ITEM_TYPE_FOLDER,    /* This page item represents an folder */
  UBA_PAGE_ITEM_TYPE_MSG_LIST,   /* This page item represents a placeholder to insert a msg list */
  /*add new type here*/
  UBA_PAGE_ITEM_TYPE_EMPTY       /* TR 18670 to repalce -1 in UBA_PAGE_ITEM_EMPTY*/
}uba_rce_page_item_type_t;

/* Defines available methods to extract/produce a GUI text */
typedef enum
{
  UBA_TEXT_TYPE_RC = 0,                             /* Static text from resource */
  UBA_TEXT_TYPE_MSG,                                /* Use special text type for msg */
  UBA_TEXT_TYPE_FOLDER,                             /* Use special text type for folder */
  UBA_TEXT_TYPE_FOLDER_PLUS_TOTAL_COUNT,            /* Use special text type for folder */
  UBA_TEXT_TYPE_FOLDER_PLUS_UNREAD_COUNT,           /* Use special text type for folder */
  UBA_TEXT_TYPE_FOLDER_PLUS_UNREAD_AND_TOTAL_COUNT  /* Use special text type for folder */
}uba_text_type_t;

/* Struct for the text rc */
typedef struct {
  const uba_text_type_t text_type; /* Type of text */
  const WE_UINT32      text_rc;   /* Str resource pointer if 
                                      text_type = UBA_TEXT_TYPE_RC*/
}uba_rce_text_t;


/* Function template for the action routine, every action is on this form */
typedef void uba_rce_action_func_t (uba_data_obj_t *p, 
                                    int             i,
                                    int             i2,
                                    const char     *str);

/* Struct for the action resource */
typedef struct {
  uba_rce_action_func_t      *action;    /* The action*/
  const int                   i;         /* The int param */
  const int                   i2;        /* The second int param */
  const char                 *str;       /* The str param */
  const int                   pop_menu;  /* True if this action should do a pop menu */
}uba_rce_action_t;

/* Struct for menu item resource */
typedef struct  {
  const WE_UINT32               str_id;   /* WE string ide to be displayed */
  const uba_rce_action_t         action_rc;/* Action to perfor if this item is selected */
}uba_rce_menu_item_t;

/*
 *	Struct containing the resources needed to render on page item
 */
typedef struct  {
  const uba_rce_page_item_type_t type; /* The type of item */
  const uba_rce_text_t           text_rc;  /* Text object to be used for generating the 
                                              gui text */
  const int                      menu_id;  /* Menu wid if the user presses menu on this item 
                                              display this menu */
  const WE_UINT32               icon_id;  /* Show this icon */
  const int                      int_data; /* Special data to the item */
}uba_rce_page_item_t;

typedef struct  {
  uba_rce_text_t        title;
  uba_rce_menu_item_t   items[UBA_MENU_ITEM_COUNT];
  uba_rce_text_t        ok;
  uba_rce_text_t        back;
  int                   default_item;
}uba_rce_menu_t;

typedef struct  {
  uba_rce_text_t       title;
  uba_rce_page_item_t  items[UBA_PAGE_ITEM_COUNT];
  uba_rce_text_t       default_ok;
  uba_rce_text_t       back;
}uba_rce_page_t;

/************************************************************************/
/* Exported functions                                                   */
/************************************************************************/

/*
 * Init the resource engine.
 */
void
uba_rce_init (void);

/*
 *	Deallocate resource engine resources
 */
void
uba_rce_release (void);

/* 
 * Start termination of this sub module. Called from main.
 * After terminate done this function cales uba_terminate_rce_done
 */
void
uba_rce_terminate (void);

/*
 *	Start the GUI application according to the resource configuration
 *  returns: TRUE if ok
 */
int
uba_rce_start (void);

void
uba_rce_handle_widget_action (int src_module, unsigned org_sig, we_widget_action_t* p);

/*
 *	Show a page and add it to the window history
 */
void
uba_rce_show_page (int page_id, uba_data_obj_t *data);

/*
 *	Show a menu and add it to the window history
 */
void
uba_rce_show_menu (int menu_id, uba_data_obj_t *data);

void
uba_rce_show_menu_rc(const uba_rce_menu_t* rc, uba_data_obj_t* data);

/*
 *	Show a custom page
 */
void
uba_rce_show_custom_page(int custom_page_id, uba_data_obj_t* data, int i);



/* removes the page p from the page stack*/
void
uba_rce_remove_page (struct uba_page_st* p);

/*
 *	Removes the menu p form the page stack
 */
void
uba_rce_remove_menu (struct uba_menu_st* p);

/*
 *	Removes the custom page p from the page stack
 */
void
uba_rce_remove_custom_page(struct uba_custom_page_st* p);


/*
 *	Executes the action with action_id and forwards the parameters
 *  data, i and str
 */
void
uba_rce_do_action(const uba_rce_action_t *action, 
                  uba_data_obj_t* data, 
                  int is_default_action);


/*
 *	Gets the menu struct from a wid
 */
const uba_rce_menu_t*
uba_rce_get_menu (int menu_id);

struct uba_page_st*
uba_rce_get_top_page (void);

/*
 *	Call this to notify the rce of a change in the data mode
 */
void
uba_rce_on_data_event (const uba_data_obj_t *data, uba_data_event_t event);

/* Offers the signal to the custom page, returns true if the cp uses the signal */
int
uba_rce_offer_custom_page_signal(uba_signal_t *sig);

/************************************************************************/
/* Util functions                                                       */
/************************************************************************/

/*
 *	Returns the index of the selected item in a widget choice group, -1 
 *  if no item is selected or if the group is empty or if error
 */
int
uba_util_get_selected_element (WeWindowHandle choice_handle);

#endif      /*UBA_RCE_H*/
