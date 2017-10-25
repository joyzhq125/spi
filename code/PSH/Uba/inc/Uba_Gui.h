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
 * uba_gui.h
 */

#ifndef UBA_GUI_H 
#define UBA_GUI_H

/************************************************************************/
/* Misc defines                                                         */
/************************************************************************/

/* Max number of items on a menu */
#define UBA_MENU_ITEM_COUNT 9

/* Max number of items on a page */
#define UBA_PAGE_ITEM_COUNT  10

/************************************************************************/
/* UBA Startup                                                          */
/************************************************************************/

#define UBA_GUI_STARTUP_ACTION        \
  UBA_ACTION(uba_action_view_page, UBA_GUI_MAIN_PAGE_ID,0,0, TRUE)

/* Uncomment the following define (UBA_GUI_STARTUP_FOLDER_DATA) and make it point
 * to a folder if you want the startup action to be initialized with a data object
 * pointing to a folder. Leave it undefined if you want to start with a static
 * data object
 */
/*
#define UBA_GUI_STARTUP_FOLDER_DATA    UBA_INBOX_FOLDER_ID
*/

/************************************************************************/
/* UBA GUI PAGE WID                                                      */
/************************************************************************/

#define UBA_GUI_MAIN_PAGE_ID                        0
#define UBA_GUI_FOLDER_IN_PAGE_ID                   1
#define UBA_GUI_FOLDER_OUT_PAGE_ID                  2
#define UBA_GUI_TEMPLATES_FOLDER_PAGE_ID            3

/************************************************************************/
/* UBA GUI CUSTOM PAGE WID                                               */
/************************************************************************/

#define UBA_CUSTOM_PAGE_PUSH_VIEW_ID                0
#define UBA_CUSTOM_PAGE_RENAME_FOLDER_VIEW_ID       1
#define UBA_CUSTOM_PAGE_MOVE_MSG_VIEW_ID            2
#define UBA_CUSTOM_PAGE_DEL_DLG_ID                  3
#define UBA_CUSTOM_PAGE_VOICE_MAIL_SET_ID           4

/************************************************************************/
/* UBA GUI MENU WID                                                      */
/************************************************************************/

#define UBA_GUI_MENU_NEW_MSG_ID                     0
#define UBA_GUI_MENU_FOLDER_IN_ID                   1
#define UBA_GUI_MENU_FOLDER_OUT_ID                  2
#define UBA_GUI_MENU_FOLDER_TEMPLATE_ID             3
#define UBA_GUI_MENU_INBOX_PUSH_SI_ID               4
#define UBA_GUI_MENU_INBOX_PUSH_SL_ID               5
#define UBA_GUI_MENU_INBOX_MMS_MSG_ID               6
#define UBA_GUI_MENU_INBOX_MMS_FWD_LOCK_MSG_ID      7
#define UBA_GUI_MENU_INBOX_MMS_NOTIF_ID             8
#define UBA_GUI_MENU_INBOX_MMS_RR_ID                9
#define UBA_GUI_MENU_INBOX_MMS_DR_ID                10
#define UBA_GUI_MENU_INBOX_EMS_MSG_ID               11
#define UBA_GUI_MENU_DRAFTS_MMS_MSG_ID              12
#define UBA_GUI_MENU_DRAFTS_EMS_MSG_ID              13
#define UBA_GUI_MENU_OUTBOX_MMS_MSG_ID              14
#define UBA_GUI_MENU_OUTBOX_EMS_MSG_ID              15
#define UBA_GUI_MENU_SENT_MMS_MSG_ID                16
#define UBA_GUI_MENU_SENT_EMS_MSG_ID                17
#define UBA_GUI_MENU_TEMPLATES_MMS_ID               18
#define UBA_GUI_MENU_TEMPLATES_EMS_ID               19
#define UBA_GUI_MENU_SETTINGS_ID                    20
#define UBA_GUI_MENU_PUSH_VIEW_SL_ID                21
#define UBA_GUI_MENU_PUSH_VIEW_SI_ID                22
#define UBA_GUI_MENU_USER_FOLDER_ID                 23
#define UBA_GUI_MENU_SORT_IN_MENU_ID                24
#define UBA_GUI_MENU_SORT_OUT_MENU_ID               25
#define UBA_GUI_MENU_SORT_TEMPLATES_ID              26
#define UBA_GUI_MENU_CREATE_TEMPLATE_ID             27
#define UBA_GUI_MENU_DELETE_ALL_ID                  28
#define UBA_GUI_MENU_INBOX_EMS_SR_ID                29
#define UBA_GUI_MENU_INBOX_EMS_VM_ID                30
#define UBA_GUI_MENU_INBOX_EMS_DVM_ID               31
#define UBA_GUI_MENU_INBOX_EMS_INCOMP_MSG_ID        32
#define UBA_GUI_MENU_INBOX_EMS_FWD_LOCK_MSG_ID      33
/*TR 17904*/
#define UBA_GUI_MENU_OUTBOX_MMS_RR_ID               34
#define UBA_GUI_MENU_DRAFTS_MMS_RR_ID               35
#endif      /*UBA_GUI_H*/
