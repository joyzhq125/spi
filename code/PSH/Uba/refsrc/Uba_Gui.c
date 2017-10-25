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
 * uba_gui.c
 */

#include "Uba_Gui.h"

#include "Uba_Rce.h"
#include "Uba_Cust.h"
#include "Uba_Rc.h"
#include "Uba_Actn.h"

/************************************************************************/
/* Defines that avoid build errors in Techfaith without all modules         */
/************************************************************************/

#ifndef WE_MODID_MSA
  #define WE_MODID_MSA WE_MODID_UBA
#endif

#ifndef WE_MODID_EMA
  #define WE_MODID_EMA WE_MODID_UBA
#endif

#ifndef WE_MODID_WBA
  #define WE_MODID_WBA WE_MODID_UBA
#endif

/************************************************************************/
/* Helpers for content routing defines                                  */
/************************************************************************/

#define UBA_EXE_STR_NEW               "-mode new"
#define UBA_EXE_STR_NEW_TEMPLATE      "-mode new_template"
#define UBA_EXE_STR_NEW_TO            "-mode new -to %s"
#define UBA_EXE_STR_NEW_FROM_TEMPLATE "-mode new -t %s"
#define UBA_EXE_STR_VIEW              "-mode view -m %s"
#define UBA_EXE_STR_REPLY             "-mode reply -m %s"
#define UBA_EXE_STR_REPLY_ALL         "-mode reply_to_all -m %s"
#define UBA_EXE_STR_FORWARD           "-mode forward -m %s"
#define UBA_EXE_STR_EDIT              "-mode edit -m %s"
#define UBA_EXE_STR_SEND              "-mode send -m %s"
#define UBA_EXE_STR_DOWNLOAD          "-mode download -m %s"
#define UBA_EXE_STR_SETTINGS          "-mode settings"

/************************************************************************/
/* UBA GUI Pages                                                        */
/************************************************************************/

static const uba_rce_page_t uba_gui_main_page = 
{
  UBA_PAGE_TITLE(UBA_TEXT_RC(UBA_STR_ID_MESSENGER_MENU)),
  {
      UBA_PAGE_ITEM(UBA_TEXT_RC(UBA_STR_ID_CREATE_NEW_MSG),
                            UBA_GUI_MENU_NEW_MSG_ID, UBA_IMG_ID_EMPTY),
      UBA_PAGE_ITEM_FOLDER(UBA_TEXT_FOLDER_NAME_PLUS_UNREAD_AND_TOTAL,
                            UBA_GUI_MENU_FOLDER_IN_ID,
                            UBA_IMG_ID_FOLDER,
                            UBA_INBOX_FOLDER_ID),
      UBA_PAGE_ITEM_FOLDER(UBA_TEXT_FOLDER_NAME_PLUS_TOTAL,
                            UBA_GUI_MENU_FOLDER_OUT_ID,
                            UBA_IMG_ID_FOLDER,
                            UBA_DRAFTS_FOLDER_ID),
      UBA_PAGE_ITEM_FOLDER(UBA_TEXT_FOLDER_NAME_PLUS_TOTAL,
                            UBA_GUI_MENU_FOLDER_OUT_ID,
                            UBA_IMG_ID_FOLDER,
                            UBA_SENT_FOLDER_ID),
      UBA_PAGE_ITEM_FOLDER(UBA_TEXT_FOLDER_NAME_PLUS_TOTAL,
                            UBA_GUI_MENU_FOLDER_TEMPLATE_ID,
                            UBA_IMG_ID_FOLDER,
                            UBA_TEMPLATES_FOLDER_ID),
      UBA_PAGE_ITEM_FOLDER(UBA_TEXT_FOLDER_NAME_PLUS_TOTAL,
                            UBA_GUI_MENU_USER_FOLDER_ID,
                            UBA_IMG_ID_FOLDER,
                            UBA_USER_DEF_FOLDER_0_ID),
      UBA_PAGE_ITEM_FOLDER(UBA_TEXT_FOLDER_NAME_PLUS_TOTAL,
                            UBA_GUI_MENU_USER_FOLDER_ID,
                            UBA_IMG_ID_FOLDER,
                            UBA_USER_DEF_FOLDER_1_ID),
      UBA_PAGE_ITEM_FOLDER(UBA_TEXT_FOLDER_NAME_PLUS_TOTAL,
                            UBA_GUI_MENU_USER_FOLDER_ID,
                            UBA_IMG_ID_FOLDER,
                            UBA_USER_DEF_FOLDER_2_ID),
      UBA_PAGE_ITEM(UBA_TEXT_RC(UBA_STR_ID_SETTINGS),
                    UBA_GUI_MENU_SETTINGS_ID,UBA_IMG_ID_EMPTY),
      UBA_PAGE_ITEM_EMPTY
  },
  UBA_PAGE_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)),
  UBA_PAGE_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK))
};

static const uba_rce_page_t uba_gui_folder_in_page = 
{ 
  UBA_PAGE_TITLE(UBA_TEXT_FOLDER_NAME), 
  { 
    UBA_PAGE_ITEM_MSG_LIST, 
    UBA_PAGE_ITEM(UBA_TEXT_RC(UBA_STR_ID_DELETE_ALL), 
                  UBA_GUI_MENU_DELETE_ALL_ID,UBA_IMG_ID_EMPTY), 
    UBA_PAGE_ITEM(UBA_TEXT_RC(UBA_STR_ID_SORT), 
                  UBA_GUI_MENU_SORT_IN_MENU_ID,UBA_IMG_ID_EMPTY), 
    UBA_PAGE_ITEM_EMPTY
  }, 
  UBA_PAGE_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_PAGE_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)) 
};

static const uba_rce_page_t uba_gui_folder_out_page = 
{ 
  UBA_PAGE_TITLE(UBA_TEXT_FOLDER_NAME), 
  { 
    UBA_PAGE_ITEM_MSG_LIST, 
    UBA_PAGE_ITEM(UBA_TEXT_RC(UBA_STR_ID_DELETE_ALL), 
                  UBA_GUI_MENU_DELETE_ALL_ID,UBA_IMG_ID_EMPTY), 
    UBA_PAGE_ITEM(UBA_TEXT_RC(UBA_STR_ID_SORT), 
                  UBA_GUI_MENU_SORT_OUT_MENU_ID,UBA_IMG_ID_EMPTY), 
    UBA_PAGE_ITEM_EMPTY
  }, 
  UBA_PAGE_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_PAGE_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)) 
};

static const uba_rce_page_t uba_gui_folder_templates_page = 
{ 
  UBA_PAGE_TITLE(UBA_TEXT_FOLDER_NAME), 
  { 
    UBA_PAGE_ITEM(UBA_TEXT_RC(UBA_STR_ID_CREATE_NEW_TEMPLATE), 
                 UBA_GUI_MENU_CREATE_TEMPLATE_ID,UBA_IMG_ID_EMPTY), 
    UBA_PAGE_ITEM_MSG_LIST, 
    UBA_PAGE_ITEM(UBA_TEXT_RC(UBA_STR_ID_DELETE_ALL), 
                 UBA_GUI_MENU_DELETE_ALL_ID,UBA_IMG_ID_EMPTY), 
    UBA_PAGE_ITEM(UBA_TEXT_RC(UBA_STR_ID_SORT), 
                 UBA_GUI_MENU_SORT_TEMPLATES_ID,UBA_IMG_ID_EMPTY), 
    UBA_PAGE_ITEM_EMPTY 
  }, 
  UBA_PAGE_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_PAGE_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)) 
};

/************************************************************************/
/* UBA GUI Menus                                                        */
/************************************************************************/

static const uba_rce_menu_t uba_gui_menu_new_msg =
{
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_CREATE_NEW_MSG)),
  {              
    UBA_MENU_ITEM(UBA_STR_ID_CREATE_NEW_SMS, 
                  UBA_ACTION(uba_action_static_content_routing, 
                             WE_MODID_EMA,0,UBA_EXE_STR_NEW, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_CREATE_NEW_MMS, 
                  UBA_ACTION(uba_action_static_content_routing, 
                             WE_MODID_MSA,0,UBA_EXE_STR_NEW, TRUE )), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_NO_DEFAULT_ITEM
};

static const uba_rce_menu_t uba_gui_menu_folder_in =
{               
  UBA_MENU_TITLE(UBA_TEXT_FOLDER_NAME), 
  {                              
    UBA_MENU_ITEM(UBA_STR_ID_OPEN, 
                  UBA_ACTION(uba_action_view_page, UBA_GUI_FOLDER_IN_PAGE_ID,0,0, TRUE)), 
    UBA_MENU_ITEM_EMPTY    
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};


static const uba_rce_menu_t uba_gui_menu_folder_out =
{               
  UBA_MENU_TITLE(UBA_TEXT_FOLDER_NAME), 
  {                              
    UBA_MENU_ITEM(UBA_STR_ID_OPEN, 
                  UBA_ACTION(uba_action_view_page, UBA_GUI_FOLDER_OUT_PAGE_ID,0,0, TRUE)), 
    UBA_MENU_ITEM_EMPTY    
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};


static const uba_rce_menu_t uba_gui_menu_inbox_push_si =
{                                   
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_PUSH_SI_MENU)), 
  {              
    UBA_MENU_ITEM(UBA_STR_ID_VIEW, 
                  UBA_ACTION(uba_action_view_custom_page, 
                  UBA_CUSTOM_PAGE_PUSH_VIEW_ID, 0, 0, TRUE)), 
    UBA_MENU_ITEM(UBA_STR_ID_LOAD, 
                  UBA_ACTION(uba_action_content_routing_with_URL, 
                             WE_MODID_WBA,0,0, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_MOVE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                  UBA_CUSTOM_PAGE_MOVE_MSG_VIEW_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM(UBA_STR_ID_DELETE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                  UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};


static const uba_rce_menu_t uba_gui_menu_inbox_push_sl =
{                                   
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_PUSH_SL_MENU)), 
  {              
    UBA_MENU_ITEM(UBA_STR_ID_VIEW, 
                  UBA_ACTION(uba_action_view_custom_page, 
                  UBA_CUSTOM_PAGE_PUSH_VIEW_ID, 0, 0, TRUE)), 
    UBA_MENU_ITEM(UBA_STR_ID_LOAD, 
                  UBA_ACTION(uba_action_content_routing_with_URL, 
                             WE_MODID_WBA,0,0, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_MOVE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                  UBA_CUSTOM_PAGE_MOVE_MSG_VIEW_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM(UBA_STR_ID_DELETE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                  UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};


static const uba_rce_menu_t uba_gui_menu_push_view_si =
{                                   
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_PUSH_SI_MENU)), 
  {              
    UBA_MENU_ITEM(UBA_STR_ID_LOAD, 
                  UBA_ACTION(uba_action_content_routing_with_URL, 
                             WE_MODID_WBA,0,0, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_MOVE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                  UBA_CUSTOM_PAGE_MOVE_MSG_VIEW_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM(UBA_STR_ID_DELETE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                  UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};

static const uba_rce_menu_t uba_gui_menu_push_view_sl =
{                                   
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_PUSH_SL_MENU)), 
  {              
    UBA_MENU_ITEM(UBA_STR_ID_LOAD, 
                  UBA_ACTION(uba_action_content_routing_with_URL, 
                             WE_MODID_WBA,0,0, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_MOVE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                             UBA_CUSTOM_PAGE_MOVE_MSG_VIEW_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM(UBA_STR_ID_DELETE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                             UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};


static const uba_rce_menu_t uba_gui_menu_inbox_mms_msg =
{                                   
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_MMS_MSG_MENU)), 
  {              
    UBA_MENU_ITEM(UBA_STR_ID_VIEW, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                             WE_MODID_MSA,0,UBA_EXE_STR_VIEW, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_REPLY, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                             WE_MODID_MSA,0,UBA_EXE_STR_REPLY, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_REPLY_ALL, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                              WE_MODID_MSA,0,UBA_EXE_STR_REPLY_ALL, TRUE )),
    UBA_MENU_ITEM(UBA_STR_ID_REPLY_BY_SMS, 
                  UBA_ACTION(uba_action_content_routing_with_UTF8_property, 
                             WE_MODID_EMA, UBS_MSG_KEY_FROM, 
                             UBA_EXE_STR_NEW_TO, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_FORWARD, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                             WE_MODID_MSA,0,UBA_EXE_STR_FORWARD, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_EDIT, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                             WE_MODID_MSA,0,UBA_EXE_STR_EDIT, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_MOVE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                             UBA_CUSTOM_PAGE_MOVE_MSG_VIEW_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM(UBA_STR_ID_DELETE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                             UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};


static const uba_rce_menu_t uba_gui_menu_inbox_mms_fwd_lock_msg =
{                                   
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_MMS_MSG_MENU)), 
  {              
    UBA_MENU_ITEM(UBA_STR_ID_VIEW, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                             WE_MODID_MSA,0,UBA_EXE_STR_VIEW, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_REPLY, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                             WE_MODID_MSA,0,UBA_EXE_STR_REPLY, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_REPLY_ALL, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                              WE_MODID_MSA,0,UBA_EXE_STR_REPLY_ALL, TRUE )),
    UBA_MENU_ITEM(UBA_STR_ID_REPLY_BY_SMS, 
                  UBA_ACTION(uba_action_content_routing_with_UTF8_property, 
                             WE_MODID_EMA, UBS_MSG_KEY_FROM, 
                             UBA_EXE_STR_NEW_TO, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_MOVE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                             UBA_CUSTOM_PAGE_MOVE_MSG_VIEW_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM(UBA_STR_ID_DELETE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                             UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};


static const uba_rce_menu_t uba_gui_menu_inbox_mms_notif = 
{                                   
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_MMS_NOTIFICATION_MENU)), 
  {              
    UBA_MENU_ITEM(UBA_STR_ID_VIEW, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                             WE_MODID_MSA,0,UBA_EXE_STR_VIEW, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_RETRIEVE, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                             WE_MODID_MSA,0,UBA_EXE_STR_DOWNLOAD, TRUE )),
    UBA_MENU_ITEM(UBA_STR_ID_DELETE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                             UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};


static const uba_rce_menu_t uba_gui_menu_inbox_mms_rr = 
{                                   
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_MMS_READ_REPORT_MENU)), 
  {              
    UBA_MENU_ITEM(UBA_STR_ID_VIEW, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                             WE_MODID_MSA, 0, UBA_EXE_STR_VIEW, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_DELETE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                             UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};

/*TR17904*/
static const uba_rce_menu_t uba_gui_menu_outbox_mms_rr =
{                                   
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_MMS_READ_REPORT_MENU)), 
  {              
    UBA_MENU_ITEM(UBA_STR_ID_VIEW, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                             WE_MODID_MSA, 0, UBA_EXE_STR_VIEW, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_DELETE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                             UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};
static const uba_rce_menu_t uba_gui_menu_drafts_mms_rr =
{                                   
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_MMS_READ_REPORT_MENU)), 
  {              
    UBA_MENU_ITEM(UBA_STR_ID_VIEW, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                             WE_MODID_MSA, 0, UBA_EXE_STR_VIEW, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_DELETE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                             UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};
/***********************/

static const uba_rce_menu_t uba_gui_menu_inbox_mms_dr = 
{                                   
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_MMS_DELIVERY_REPORT_MENU)), 
  {              
    UBA_MENU_ITEM(UBA_STR_ID_VIEW, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                             WE_MODID_MSA, 0, UBA_EXE_STR_VIEW, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_DELETE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                             UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};


static const uba_rce_menu_t uba_gui_menu_inbox_ems_msg = 
{                                   
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_EMS_MENU)), 
  {              
    UBA_MENU_ITEM(UBA_STR_ID_VIEW, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                             WE_MODID_EMA,0,UBA_EXE_STR_VIEW, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_REPLY, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                             WE_MODID_EMA, 0, UBA_EXE_STR_REPLY, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_REPLY_BY_MMS, 
                  UBA_ACTION(uba_action_content_routing_with_UTF8_property, 
                             WE_MODID_MSA, UBS_MSG_KEY_FROM, 
                             UBA_EXE_STR_NEW_TO, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_FORWARD, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                             WE_MODID_EMA, 0, UBA_EXE_STR_FORWARD, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_EDIT, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                     WE_MODID_EMA,0,UBA_EXE_STR_EDIT, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_MOVE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                             UBA_CUSTOM_PAGE_MOVE_MSG_VIEW_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM(UBA_STR_ID_DELETE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                             UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};


static const uba_rce_menu_t uba_gui_menu_drafts_mms_msg = 
{                                   
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_MMS_MSG_DRAFT_MENU)), 
  {              
    UBA_MENU_ITEM(UBA_STR_ID_EDIT, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                     WE_MODID_MSA,0,UBA_EXE_STR_EDIT, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_DELETE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                  UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};


static const uba_rce_menu_t uba_gui_menu_drafts_ems_msg =
{                                   
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_EMS_MSG_DRAFT_MENU)), 
  {              
    UBA_MENU_ITEM(UBA_STR_ID_EDIT, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                     WE_MODID_EMA,0,UBA_EXE_STR_EDIT, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_DELETE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                  UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};


static const uba_rce_menu_t uba_gui_menu_outbox_mms_msg = 
{                                   
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_MMS_MSG_DRAFT_MENU)), 
  {              
    UBA_MENU_ITEM(UBA_STR_ID_EDIT, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                     WE_MODID_MSA,0,UBA_EXE_STR_EDIT, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_SEND, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                     WE_MODID_MSA,0,UBA_EXE_STR_SEND, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_DELETE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                  UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};


static const uba_rce_menu_t uba_gui_menu_outbox_ems_msg =
{                                   
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_EMS_MSG_DRAFT_MENU)), 
  {              
    UBA_MENU_ITEM(UBA_STR_ID_EDIT, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                     WE_MODID_EMA,0,UBA_EXE_STR_EDIT, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_SEND, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                     WE_MODID_EMA,0,UBA_EXE_STR_SEND, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_DELETE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                  UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};


static const uba_rce_menu_t uba_gui_menu_sent_mms_msg = 
{                                   
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_MMS_MSG_SENT_MENU)), 
  {              
    UBA_MENU_ITEM(UBA_STR_ID_VIEW, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                             WE_MODID_MSA,0,UBA_EXE_STR_VIEW, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_FORWARD, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                          WE_MODID_MSA,0,UBA_EXE_STR_FORWARD, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_RESEND, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                          WE_MODID_MSA,0,UBA_EXE_STR_SEND, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_DELETE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                  UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};


static const uba_rce_menu_t uba_gui_menu_sent_ems_msg = 
{                                   
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_EMS_MSG_SENT_MENU)), 
  {              
    UBA_MENU_ITEM(UBA_STR_ID_VIEW, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                             WE_MODID_EMA,0,UBA_EXE_STR_VIEW, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_FORWARD, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                          WE_MODID_EMA,0,UBA_EXE_STR_FORWARD, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_RESEND, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                          WE_MODID_EMA,0,UBA_EXE_STR_SEND, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_DELETE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                  UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};


static const uba_rce_menu_t uba_gui_menu_templates_mms = 
{                                   
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_MMS_TEMPLATE_MENU)), 
  {              
    UBA_MENU_ITEM(UBA_STR_ID_CREATE_MMS_FROM_TEMPLATE, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                          WE_MODID_MSA,0,UBA_EXE_STR_NEW_FROM_TEMPLATE, TRUE )),
    UBA_MENU_ITEM(UBA_STR_ID_EDIT, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                          WE_MODID_MSA,0,UBA_EXE_STR_EDIT, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_DELETE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                  UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};


static const uba_rce_menu_t uba_gui_menu_templates_ems = 
{                                   
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_EMS_TEMPLATE_MENU)), 
  {              
    UBA_MENU_ITEM(UBA_STR_ID_CREATE_SMS_FROM_TEMPLATE, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                          WE_MODID_EMA,0,UBA_EXE_STR_NEW_FROM_TEMPLATE, TRUE )),
    UBA_MENU_ITEM(UBA_STR_ID_EDIT, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                          WE_MODID_EMA,0,UBA_EXE_STR_EDIT, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_DELETE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                  UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};


static const uba_rce_menu_t uba_gui_menu_settings = 
{                                   
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_SETTINGS)), 
  {              
    UBA_MENU_ITEM(UBA_STR_ID_SMS_SETTINGS, 
                  UBA_ACTION(uba_action_static_content_routing, 
                        WE_MODID_EMA,0,UBA_EXE_STR_SETTINGS, FALSE )), 
    UBA_MENU_ITEM(UBA_STR_ID_MMS_SETTINGS, 
                  UBA_ACTION(uba_action_static_content_routing, 
                        WE_MODID_MSA,0,UBA_EXE_STR_SETTINGS, FALSE )), 
    UBA_MENU_ITEM(UBA_STR_ID_VOICE_MAIL,
                  UBA_ACTION(uba_action_view_custom_page,
                  UBA_CUSTOM_PAGE_VOICE_MAIL_SET_ID, 0, 0, TRUE)),
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_NO_DEFAULT_ITEM
};


static const uba_rce_menu_t uba_gui_menu_user_folder = 
{               
  UBA_MENU_TITLE(UBA_TEXT_FOLDER_NAME), 
  {                              
    UBA_MENU_ITEM(UBA_STR_ID_OPEN, 
                  UBA_ACTION(uba_action_view_page, UBA_GUI_FOLDER_IN_PAGE_ID,0,0, FALSE)), 
    UBA_MENU_ITEM(UBA_STR_ID_RENAME, 
                  UBA_ACTION(uba_action_view_custom_page, 
                  UBA_CUSTOM_PAGE_RENAME_FOLDER_VIEW_ID, 0, 0, TRUE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};


static const uba_rce_menu_t uba_gui_menu_sort_in_menu = 
{               
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_SORT)), 
  {                              
    UBA_MENU_ITEM(UBA_STR_ID_SORT_BY_DATE, 
                  UBA_ACTION(uba_action_sort_folder, 
                        UBA_FOLDER_SORT_TIME_STAMP, 0, 0, TRUE)), 
    UBA_MENU_ITEM(UBA_STR_ID_SORT_BY_TYPE, 
                  UBA_ACTION(uba_action_sort_folder, 
                        UBA_FOLDER_SORT_TYPE, 0, 0, TRUE)), 
    UBA_MENU_ITEM(UBA_STR_ID_SORT_BY_SENDER, 
                  UBA_ACTION(uba_action_sort_folder, 
                        UBA_FOLDER_SORT_FROM, 0, 0, TRUE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_NO_DEFAULT_ITEM
};


static const uba_rce_menu_t uba_gui_menu_sort_out_menu = 
{               
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_SORT)), 
  {                              
    UBA_MENU_ITEM(UBA_STR_ID_SORT_BY_DATE, 
                  UBA_ACTION(uba_action_sort_folder, 
                        UBA_FOLDER_SORT_TIME_STAMP, 0, 0, TRUE)), 
    UBA_MENU_ITEM(UBA_STR_ID_SORT_BY_TYPE, 
                  UBA_ACTION(uba_action_sort_folder, 
                        UBA_FOLDER_SORT_TYPE, 0, 0, TRUE)), 
    UBA_MENU_ITEM(UBA_STR_ID_SORT_BY_RECEIVER, 
                  UBA_ACTION(uba_action_sort_folder, 
                        UBA_FOLDER_SORT_TO, 0, 0, TRUE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_NO_DEFAULT_ITEM
};


static const uba_rce_menu_t uba_gui_menu_sort_templates_menu = 
{               
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_SORT)), 
  {                              
    UBA_MENU_ITEM(UBA_STR_ID_SORT_BY_DATE, 
                  UBA_ACTION(uba_action_sort_folder, 
                        UBA_FOLDER_SORT_TIME_STAMP, 0, 0, TRUE)), 
    UBA_MENU_ITEM(UBA_STR_ID_SORT_BY_TYPE, 
                  UBA_ACTION(uba_action_sort_folder, 
                        UBA_FOLDER_SORT_TYPE, 0, 0, TRUE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_NO_DEFAULT_ITEM
};


static const uba_rce_menu_t uba_gui_menu_create_template = 
{               
  UBA_MENU_TITLE(UBA_TEXT_FOLDER_NAME), 
  {                              
    UBA_MENU_ITEM(UBA_STR_ID_CREATE_NEW_SMS_TEMPLATE, 
                  UBA_ACTION(uba_action_static_content_routing, 
                             WE_MODID_EMA,0,UBA_EXE_STR_NEW_TEMPLATE, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_CREATE_NEW_MMS_TEMPLATE, 
                  UBA_ACTION(uba_action_static_content_routing, 
                             WE_MODID_MSA,0,UBA_EXE_STR_NEW_TEMPLATE, TRUE )), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_NO_DEFAULT_ITEM
};


static const uba_rce_menu_t uba_gui_menu_delete_all =
{               
  UBA_MENU_TITLE(UBA_TEXT_FOLDER_NAME), 
  {                              
    UBA_MENU_ITEM(UBA_STR_ID_DELETE_ALL, 
                  UBA_ACTION(uba_action_view_custom_page, 
                  UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, TRUE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0)
};


static const uba_rce_menu_t uba_gui_menu_inbox_ems_sr = 
{
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_EMS_STATUS_REPORT_MENU)), 
  {
    UBA_MENU_ITEM(UBA_STR_ID_VIEW, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                             WE_MODID_EMA, 0, UBA_EXE_STR_VIEW, TRUE)), 
    UBA_MENU_ITEM(UBA_STR_ID_DELETE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                             UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM_EMPTY
  },
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};


static const uba_rce_menu_t uba_gui_menu_inbox_ems_vm =
{
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_EMS_VOICE_MAIL_MENU)),
  {
    UBA_MENU_ITEM(UBA_STR_ID_VIEW,
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string,
                             WE_MODID_EMA, 0, UBA_EXE_STR_VIEW, TRUE)),
    UBA_MENU_ITEM(UBA_STR_ID_DELETE,
                  UBA_ACTION(uba_action_view_custom_page,
                             UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)),
    UBA_MENU_ITEM_EMPTY
  },
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)),
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)),
  UBA_MENU_DEFAULT_ITEM(0)
};


static const uba_rce_menu_t uba_gui_menu_inbox_ems_dvm =
{
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_EMS_DISC_VOICE_MAIL_MENU)),
  {
    UBA_MENU_ITEM(UBA_STR_ID_CALL_VOICE_MAIL,
                  UBA_ACTION(uba_action_call_del_voice_mail,
                             0, 0, 0, TRUE)),
    UBA_MENU_ITEM(UBA_STR_ID_DELETE,
                  UBA_ACTION(uba_action_view_custom_page,
                             UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)),
    UBA_MENU_ITEM_EMPTY
  },
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)),
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)),
  UBA_MENU_DEFAULT_ITEM(0)
};


static const uba_rce_menu_t uba_gui_menu_inbox_ems_incomp_msg =
{
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_EMS_INCOMP_MENU)),
  {
    UBA_MENU_ITEM(UBA_STR_ID_VIEW,
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string,
                             WE_MODID_EMA,0,UBA_EXE_STR_VIEW, TRUE )),
    UBA_MENU_ITEM(UBA_STR_ID_REPLY,
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string,
                             WE_MODID_EMA, 0, UBA_EXE_STR_REPLY, TRUE )),
    UBA_MENU_ITEM(UBA_STR_ID_REPLY_BY_MMS,
                  UBA_ACTION(uba_action_content_routing_with_UTF8_property,
                             WE_MODID_MSA, UBS_MSG_KEY_FROM,
                             UBA_EXE_STR_NEW_TO, TRUE )),
    UBA_MENU_ITEM(UBA_STR_ID_FORWARD,
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string,
                             WE_MODID_EMA, 0, UBA_EXE_STR_FORWARD, TRUE )),
    UBA_MENU_ITEM(UBA_STR_ID_EDIT,
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string,
                     WE_MODID_EMA,0,UBA_EXE_STR_EDIT, TRUE )),
    UBA_MENU_ITEM(UBA_STR_ID_DELETE,
                  UBA_ACTION(uba_action_view_custom_page,
                             UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)),
    UBA_MENU_ITEM_EMPTY
  },
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)),
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)),
  UBA_MENU_DEFAULT_ITEM(0)
};


static const uba_rce_menu_t uba_gui_menu_inbox_ems_fwd_lock_msg = 
{                                   
  UBA_MENU_TITLE(UBA_TEXT_RC(UBA_STR_ID_EMS_MENU)), 
  {              
    UBA_MENU_ITEM(UBA_STR_ID_VIEW, 
                  UBA_ACTION(uba_action_content_routing_with_handle_as_hex_string, 
                             WE_MODID_EMA,0,UBA_EXE_STR_VIEW, TRUE )), 
    UBA_MENU_ITEM(UBA_STR_ID_MOVE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                             UBA_CUSTOM_PAGE_MOVE_MSG_VIEW_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM(UBA_STR_ID_DELETE, 
                  UBA_ACTION(uba_action_view_custom_page, 
                             UBA_CUSTOM_PAGE_DEL_DLG_ID, 0, 0, FALSE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0) 
};


static const uba_rce_menu_t uba_gui_menu_folder_template = 
{               
  UBA_MENU_TITLE(UBA_TEXT_FOLDER_NAME), 
  {                              
    UBA_MENU_ITEM(UBA_STR_ID_OPEN, 
        UBA_ACTION(uba_action_view_page, UBA_GUI_TEMPLATES_FOLDER_PAGE_ID,0,0, TRUE)), 
    UBA_MENU_ITEM_EMPTY
  }, 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_SELECT)), 
  UBA_MENU_ACTION_LABEL(UBA_TEXT_RC(UBA_STR_ID_BACK)), 
  UBA_MENU_DEFAULT_ITEM(0)
};


/************************************************************************/
/* UBA GUI Page array                                                   */
/************************************************************************/

const uba_rce_page_t * uba_rce_pages[] = 
{
  &uba_gui_main_page,             /*UBA_GUI_MAIN_PAGE_ID*/
  &uba_gui_folder_in_page,        /*UBA_GUI_FOLDER_IN_PAGE_ID*/
  &uba_gui_folder_out_page,       /*UBA_GUI_FOLDER_OUT_PAGE_ID*/
  &uba_gui_folder_templates_page  /*UBA_GUI_FOLDER_TEMPLATES_PAGE_ID*/
};

/************************************************************************/
/* UBA GUI Custom page array                                            */
/************************************************************************/

extern const uba_custom_page_func_t uba_custom_page_push_view;
extern const uba_custom_page_func_t uba_custom_page_rename_folder_view;
extern const uba_custom_page_func_t uba_custom_page_move_msg_view;
extern const uba_custom_page_func_t uba_custom_page_del_dlg;
extern const uba_custom_page_func_t uba_custom_page_voice_mail_set;

const uba_custom_page_func_t * uba_custom_pages[] = 
{
  &uba_custom_page_push_view,          /*UBA_CUSTOM_PAGE_PUSH_VIEW_ID*/         
  &uba_custom_page_rename_folder_view, /*UBA_CUSTOM_PAGE_RENAME_FOLDER_VIEW_ID*/
  &uba_custom_page_move_msg_view,      /*UBA_CUSTOM_PAGE_MOVE_MSG_VIEW_ID*/     
  &uba_custom_page_del_dlg,            /*UBA_CUSTOM_PAGE_DEL_DLG_ID*/      
  &uba_custom_page_voice_mail_set      /*UBA_CUSTOM_PAGE_VOICE_MAIL_SET_ID*/
};

/************************************************************************/
/* UBA GUI Menu array                                                   */
/************************************************************************/

const uba_rce_menu_t * uba_rce_menus[] = 
{
  &uba_gui_menu_new_msg,                /*UBA_GUI_MENU_NEW_MSG_ID*/
  &uba_gui_menu_folder_in,              /*UBA_GUI_MENU_FOLDER_IN_ID*/
  &uba_gui_menu_folder_out,             /*UBA_GUI_MENU_FOLDER_OUT_ID*/
  &uba_gui_menu_folder_template,        /*UBA_GUI_MENU_FOLDER_TEMPLATE_ID*/
  &uba_gui_menu_inbox_push_si,          /*UBA_GUI_MENU_INBOX_PUSH_SI_ID*/
  &uba_gui_menu_inbox_push_sl,          /*UBA_GUI_MENU_INBOX_PUSH_SL_ID*/
  &uba_gui_menu_inbox_mms_msg,          /*UBA_GUI_MENU_INBOX_MMS_MSG_ID*/
  &uba_gui_menu_inbox_mms_fwd_lock_msg, /*UBA_GUI_MENU_INBOX_MMS_FWD_LOCK_MSG_ID*/
  &uba_gui_menu_inbox_mms_notif,        /*UBA_GUI_MENU_INBOX_MMS_NOTIF_ID*/
  &uba_gui_menu_inbox_mms_rr,           /*UBA_GUI_MENU_INBOX_MMS_RR_ID*/
  &uba_gui_menu_inbox_mms_dr,           /*UBA_GUI_MENU_INBOX_MMS_DR_ID*/
  &uba_gui_menu_inbox_ems_msg,          /*UBA_GUI_MENU_INBOX_EMS_MSG_ID*/
  &uba_gui_menu_drafts_mms_msg,         /*UBA_GUI_MENU_DRAFTS_MMS_MSG_ID*/
  &uba_gui_menu_drafts_ems_msg,         /*UBA_GUI_MENU_DRAFTS_EMS_MSG_ID*/
  &uba_gui_menu_outbox_mms_msg,         /*UBA_GUI_MENU_OUTBOX_MMS_MSG_ID*/
  &uba_gui_menu_outbox_ems_msg,         /*UBA_GUI_MENU_OUTBOX_EMS_MSG_ID*/
  &uba_gui_menu_sent_mms_msg,           /*UBA_GUI_MENU_SENT_MMS_MSG_ID*/
  &uba_gui_menu_sent_ems_msg,           /*UBA_GUI_MENU_SENT_EMS_MSG_ID*/
  &uba_gui_menu_templates_mms,          /*UBA_GUI_MENU_TEMPLATES_MMS_ID*/
  &uba_gui_menu_templates_ems,          /*UBA_GUI_MENU_TEMPLATES_EMS_ID*/
  &uba_gui_menu_settings,               /*UBA_GUI_MENU_SETTINGS_ID*/
  &uba_gui_menu_push_view_sl,           /*UBA_GUI_MENU_PUSH_VIEW_SL_ID*/
  &uba_gui_menu_push_view_si,           /*UBA_GUI_MENU_PUSH_VIEW_SI_ID*/
  &uba_gui_menu_user_folder,            /*UBA_GUI_MENU_USER_FOLDER_ID*/
  &uba_gui_menu_sort_in_menu,           /*UBA_GUI_MENU_SORT_IN_MENU_ID*/
  &uba_gui_menu_sort_out_menu,          /*UBA_GUI_MENU_SORT_OUT_MENU_ID*/
  &uba_gui_menu_sort_templates_menu,    /*UBA_GUI_MENU_SORT_TEMPLATES_ID*/
  &uba_gui_menu_create_template,        /*UBA_GUI_MENU_CREATE_TEMPLATE_ID*/
  &uba_gui_menu_delete_all,             /*UBA_GUI_MENU_DELETE_ALL_ID*/
  &uba_gui_menu_inbox_ems_sr,           /*UBA_GUI_MENU_INBOX_EMS_SR_ID*/
  &uba_gui_menu_inbox_ems_vm,           /*UBA_GUI_MENU_INBOX_EMS_VM_ID*/
  &uba_gui_menu_inbox_ems_dvm,          /*UBA_GUI_MENU_INBOX_EMS_DVM_ID*/
  &uba_gui_menu_inbox_ems_incomp_msg,   /*UBA_GUI_MENU_INBOX_EMS_INCOMP_MSG_ID*/
  &uba_gui_menu_inbox_ems_fwd_lock_msg,  /*UBA_GUI_MENU_INBOX_EMS_FWD_LOCK_MSG_ID*/
  &uba_gui_menu_outbox_mms_rr,           /*UBA_GUI_MENU_OUTBOX_MMS_RR_ID TR 17904*/ 
  &uba_gui_menu_drafts_mms_rr           /*UBA_GUI_MENU_DRAFTS_MMS_RR_ID TR 17904*/ 
};

