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
 * uba_rc.h
 *
 * Description:
 *		This file contain defines for all strings used in the UBA
 *    module. All strings are predefined.
 */

#ifndef _UBA_RC_H_
#define _UBA_RC_H_

#ifndef _we_cfg_h
#include "We_Cfg.h"
#endif

/************************************************************************/
/* Strings                                                              */
/************************************************************************/
/*--- Constants ---*/
#define UBA_NUMBER_OF_STRINGS 78

extern const char * const uba_strings[][UBA_NUMBER_OF_STRINGS+1];


/*--- Common language ---*/
#define UBA_STR_ID_BACK                     ((  1<<8) | WE_MODID_UBA) /*"Back"*/
#define UBA_STR_ID_CREATE_NEW_SMS           ((  2<<8) | WE_MODID_UBA) /*"Create new SMS"*/
#define UBA_STR_ID_CREATE_NEW_MMS           ((  3<<8) | WE_MODID_UBA) /*"Create new MMS"*/
#define UBA_STR_ID_CREATE_NEW_MSG           ((  4<<8) | WE_MODID_UBA) /*"Create new message"*/
#define UBA_STR_ID_CREATE_NEW_TEMPLATE      ((  5<<8) | WE_MODID_UBA) /*"Create new template"*/
#define UBA_STR_ID_DELETE                   ((  6<<8) | WE_MODID_UBA) /*"Delete"*/
#define UBA_STR_ID_DELETE_ALL               ((  7<<8) | WE_MODID_UBA) /*"Delete all"*/
#define UBA_STR_ID_RETRIEVE                 ((  8<<8) | WE_MODID_UBA) /*"Retrieve"*/
#define UBA_STR_ID_DRAFTS                   ((  9<<8) | WE_MODID_UBA) /*"Drafts"*/
#define UBA_STR_ID_EDIT                     (( 10<<8) | WE_MODID_UBA) /*"Edit"*/
#define UBA_STR_ID_EMS_MENU                 (( 11<<8) | WE_MODID_UBA) /*"SMS menu"*/
#define UBA_STR_ID_EMS_TEMPLATE_MENU        (( 12<<8) | WE_MODID_UBA) /*"SMS template menu"*/
#define UBA_STR_ID_FOLDER_ACTIONS           (( 13<<8) | WE_MODID_UBA) /*"Folder actions"*/
#define UBA_STR_ID_FORWARD                  (( 14<<8) | WE_MODID_UBA) /*"Forward"*/
#define UBA_STR_ID_INBOX                    (( 15<<8) | WE_MODID_UBA) /*"Inbox"*/
#define UBA_STR_ID_LOAD                     (( 16<<8) | WE_MODID_UBA) /*"Load"*/
#define UBA_STR_ID_MESSENGER_MENU           (( 17<<8) | WE_MODID_UBA) /*"Messenger menu"*/
#define UBA_STR_ID_MMS_DELIVERY_REPORT_MENU (( 18<<8) | WE_MODID_UBA) /*"MMS delivery report menu"*/
#define UBA_STR_ID_MMS_MSG_MENU             (( 19<<8) | WE_MODID_UBA) /*"MMS message menu"*/
#define UBA_STR_ID_MMS_NOTIFICATION_MENU    (( 20<<8) | WE_MODID_UBA) /*"MMS notification menu"*/
#define UBA_STR_ID_MMS_READ_REPORT_MENU     (( 21<<8) | WE_MODID_UBA) /*"MMS read report menu"*/
#define UBA_STR_ID_MMS_SETTINGS             (( 22<<8) | WE_MODID_UBA) /*"MMS settings"*/
#define UBA_STR_ID_MMS_TEMPLATE_MENU        (( 23<<8) | WE_MODID_UBA) /*"MMS template menu"*/
#define UBA_STR_ID_MOVE                     (( 24<<8) | WE_MODID_UBA) /*"Move"*/
#define UBA_STR_ID_OPEN                     (( 25<<8) | WE_MODID_UBA) /*"Open"*/
#define UBA_STR_ID_PUSH_SI_MENU             (( 26<<8) | WE_MODID_UBA) /*"Push SI menu"*/
#define UBA_STR_ID_PUSH_SL_MENU             (( 27<<8) | WE_MODID_UBA) /*"Push SL "*/
#define UBA_STR_ID_RENAME                   (( 28<<8) | WE_MODID_UBA) /*"Rename"*/
#define UBA_STR_ID_REPLY                    (( 29<<8) | WE_MODID_UBA) /*"Reply"*/
#define UBA_STR_ID_REPLY_ALL                (( 30<<8) | WE_MODID_UBA) /*"Reply to all"*/
#define UBA_STR_ID_REPLY_BY_MMS             (( 31<<8) | WE_MODID_UBA) /*"Reply by MMS"*/
#define UBA_STR_ID_REPLY_BY_SMS             (( 32<<8) | WE_MODID_UBA) /*"Reply by SMS"*/
#define UBA_STR_ID_SELECT                   (( 33<<8) | WE_MODID_UBA) /*"Select"*/
#define UBA_STR_ID_SEND                     (( 34<<8) | WE_MODID_UBA) /*"Send"*/
#define UBA_STR_ID_SENT                     (( 35<<8) | WE_MODID_UBA) /*"Sent"*/
#define UBA_STR_ID_SETTINGS                 (( 36<<8) | WE_MODID_UBA) /*"Settings"*/
#define UBA_STR_ID_SMS_SETTINGS             (( 37<<8) | WE_MODID_UBA) /*"SMS settings"*/
#define UBA_STR_ID_SORT                     (( 38<<8) | WE_MODID_UBA) /*"Sort"*/
#define UBA_STR_ID_TEMPLATES                (( 39<<8) | WE_MODID_UBA) /*"Templates"*/
#define UBA_STR_ID_VIEW                     (( 40<<8) | WE_MODID_UBA) /*"View"*/
#define UBA_STR_ID_EMPTY                    (( 41<<8) | WE_MODID_UBA) /*" "*/
#define UBA_STR_ID_MENU                     (( 42<<8) | WE_MODID_UBA) /*"Menu"*/
#define UBA_STR_ID_MMS_MSG_DRAFT_MENU       (( 43<<8) | WE_MODID_UBA) /*"MMS draft menu"*/
#define UBA_STR_ID_EMS_MSG_DRAFT_MENU       (( 44<<8) | WE_MODID_UBA) /*"SMS draft menu"*/
#define UBA_STR_ID_MMS_MSG_SENT_MENU        (( 45<<8) | WE_MODID_UBA) /*"MMS sent menu"*/
#define UBA_STR_ID_EMS_MSG_SENT_MENU        (( 46<<8) | WE_MODID_UBA) /*"SMS sent menu"*/
#define UBA_STR_ID_USER_FOLDER_0            (( 47<<8) | WE_MODID_UBA) /*"Friends"*/
#define UBA_STR_ID_USER_FOLDER_1            (( 48<<8) | WE_MODID_UBA) /*"Fun"*/
#define UBA_STR_ID_USER_FOLDER_2            (( 49<<8) | WE_MODID_UBA) /*"Important"*/
#define UBA_STR_ID_NO_STR                   (( 50<<8) | WE_MODID_UBA) /*" " Used in cfg to signal no str rc*/
#define UBA_STR_ID_FROM                     (( 51<<8) | WE_MODID_UBA) /*"From"*/
#define UBA_STR_ID_CREATED                  (( 52<<8) | WE_MODID_UBA) /*"Created"*/
#define UBA_STR_ID_EXPIRES                  (( 53<<8) | WE_MODID_UBA) /*"Expires"*/
#define UBA_STR_ID_URL                      (( 54<<8) | WE_MODID_UBA) /*"Url"*/
#define UBA_STR_ID_MESSAGE                  (( 55<<8) | WE_MODID_UBA) /*"Message"*/
#define UBA_STR_ID_ACCEPT                   (( 56<<8) | WE_MODID_UBA) /*"Accept"*/
#define UBA_STR_ID_RENAME_FOLDER            (( 57<<8) | WE_MODID_UBA) /*"Rename folder "*/
#define UBA_STR_ID_MOVE_TO_FOLDER           (( 58<<8) | WE_MODID_UBA) /*"Move to folder"*/
#define UBA_STR_ID_SORT_BY_DATE             (( 59<<8) | WE_MODID_UBA) /*"Sort by date"*/
#define UBA_STR_ID_SORT_BY_TYPE             (( 60<<8) | WE_MODID_UBA) /*"Sort by type"*/
#define UBA_STR_ID_SORT_BY_SENDER           (( 61<<8) | WE_MODID_UBA) /*"Sort by sender"*/
#define UBA_STR_ID_OK                       (( 62<<8) | WE_MODID_UBA) /*"Ok"*/
#define UBA_STR_ID_CANCEL                   (( 63<<8) | WE_MODID_UBA) /*"Cancel"*/
#define UBA_STR_ID_DELETE_ITEM              (( 64<<8) | WE_MODID_UBA) /*"Delete this item?"*/
#define UBA_STR_ID_DELETE_ALL_ITEMS         (( 65<<8) | WE_MODID_UBA) /*"Delete all items in this folder?"*/
#define UBA_STR_ID_CREATE_NEW_SMS_TEMPLATE  (( 66<<8) | WE_MODID_UBA) /*"Create new SMS template"*/
#define UBA_STR_ID_CREATE_NEW_MMS_TEMPLATE  (( 67<<8) | WE_MODID_UBA) /*"Create new MMS template"*/
#define UBA_STR_ID_CREATE_SMS_FROM_TEMPLATE (( 68<<8) | WE_MODID_UBA) /*"New SMS from template"*/
#define UBA_STR_ID_CREATE_MMS_FROM_TEMPLATE (( 69<<8) | WE_MODID_UBA) /*"New MMS from template"*/
#define UBA_STR_ID_SORT_BY_RECEIVER         (( 70<<8) | WE_MODID_UBA) /*"Sort by receiver"*/
#define UBA_STR_ID_RESEND                   (( 71<<8) | WE_MODID_UBA) /*"Resend"*/
#define UBA_STR_ID_EMS_STATUS_REPORT_MENU   (( 72<<8) | WE_MODID_UBA) /*"SMS status report menu"*/
#define UBA_STR_ID_EMS_VOICE_MAIL_MENU      (( 73<<8) | WE_MODID_UBA) /*"SMS voice mail menu"*/
#define UBA_STR_ID_EMS_DISC_VOICE_MAIL_MENU (( 74<<8) | WE_MODID_UBA) /*"SMS discard voice mail menu"*/
#define UBA_STR_ID_CALL_VOICE_MAIL          (( 75<<8) | WE_MODID_UBA) /*"Call voice mail"*/
#define UBA_STR_ID_VOICE_MAIL               (( 76<<8) | WE_MODID_UBA) /*"Voice mail"*/
#define UBA_STR_ID_SET_VOICE_MAIL           (( 77<<8) | WE_MODID_UBA) /*"Set voice mail"*/
#define UBA_STR_ID_EMS_INCOMP_MENU          (( 78<<8) | WE_MODID_UBA) /*"SMS incomplete menu"*/

/************************************************************************/
/* Images                                                               */
/************************************************************************/


#define UBA_NUMBER_OF_IMAGES 45


#define UBA_IMG_ID_EMPTY              ((  1<<8) | WE_MODID_UBA) /* An empty picture, DO NOT REMOVE */
#define UBA_IMG_ID_FOLDER             ((  2<<8) | WE_MODID_UBA) /* A folder image */
#define UBA_IMG_ID_MMS                ((  3<<8) | WE_MODID_UBA) /* A MMS */
#define UBA_IMG_ID_MMS_R              ((  4<<8) | WE_MODID_UBA) /* A MMS, read */
#define UBA_IMG_ID_MMS_H              ((  5<<8) | WE_MODID_UBA) /* A MMS, high priority */
#define UBA_IMG_ID_MMS_H_R            ((  6<<8) | WE_MODID_UBA) /* A MMS, high priority, read */
#define UBA_IMG_ID_MMS_L              ((  7<<8) | WE_MODID_UBA) /* A MMS, low priority */
#define UBA_IMG_ID_MMS_L_R            ((  8<<8) | WE_MODID_UBA) /* A MMS, low priority, read */
#define UBA_IMG_ID_MMS_LOCK           ((  9<<8) | WE_MODID_UBA) /* A MMS, forward locked */
#define UBA_IMG_ID_MMS_LOCK_R         (( 10<<8) | WE_MODID_UBA) /* A MMS, forward locked, read */
#define UBA_IMG_ID_MMS_LOCK_H         (( 11<<8) | WE_MODID_UBA) /* A MMS, forward locked, high priority */
#define UBA_IMG_ID_MMS_LOCK_H_R       (( 12<<8) | WE_MODID_UBA) /* A MMS, forward locked, high priority, read */
#define UBA_IMG_ID_MMS_LOCK_L         (( 13<<8) | WE_MODID_UBA) /* A MMS, forward locked, low priority */
#define UBA_IMG_ID_MMS_LOCK_L_R       (( 14<<8) | WE_MODID_UBA) /* A MMS, forward locked, low priority, read */
#define UBA_IMG_ID_MMS_OUTBOX         (( 15<<8) | WE_MODID_UBA) /* A MMS msg in the outbox */
#define UBA_IMG_ID_MMS_N              (( 16<<8) | WE_MODID_UBA) /* A MMS notification */
#define UBA_IMG_ID_MMS_RR             (( 17<<8) | WE_MODID_UBA) /* A MMS read report */
#define UBA_IMG_ID_MMS_RR_R           (( 18<<8) | WE_MODID_UBA) /* A MMS read report, read */
#define UBA_IMG_ID_MMS_DR             (( 19<<8) | WE_MODID_UBA) /* A MMS delivery report */
#define UBA_IMG_ID_MMS_DR_R           (( 20<<8) | WE_MODID_UBA) /* A MMS delivery report, read */
#define UBA_IMG_ID_EMS                (( 21<<8) | WE_MODID_UBA) /* A EMS */
#define UBA_IMG_ID_EMS_R              (( 22<<8) | WE_MODID_UBA) /* A EMS, read */
#define UBA_IMG_ID_EMS_H              (( 23<<8) | WE_MODID_UBA) /* A EMS, high priority */
#define UBA_IMG_ID_EMS_H_R            (( 24<<8) | WE_MODID_UBA) /* A EMS, high priority, read */
#define UBA_IMG_ID_EMS_L              (( 25<<8) | WE_MODID_UBA) /* A EMS, low priority */
#define UBA_IMG_ID_EMS_L_R            (( 26<<8) | WE_MODID_UBA) /* A EMS, low priority, read */
#define UBA_IMG_ID_EMS_OUTBOX         (( 27<<8) | WE_MODID_UBA) /* A EMS msg in the outbox */
#define UBA_IMG_ID_PUSH               (( 28<<8) | WE_MODID_UBA) /* A push */
#define UBA_IMG_ID_PUSH_R             (( 29<<8) | WE_MODID_UBA) /* A push, read */
#define UBA_IMG_ID_PUSH_H             (( 30<<8) | WE_MODID_UBA) /* A high priority push */
#define UBA_IMG_ID_PUSH_H_R           (( 31<<8) | WE_MODID_UBA) /* A read high priority push */
#define UBA_IMG_ID_PUSH_L             (( 32<<8) | WE_MODID_UBA) /* A low priority push */
#define UBA_IMG_ID_PUSH_L_R           (( 33<<8) | WE_MODID_UBA) /* A read low priority push  */
#define UBA_IMG_ID_EMS_SR             (( 34<<8) | WE_MODID_UBA) /* A EMS status report */
#define UBA_IMG_ID_EMS_SR_R           (( 35<<8) | WE_MODID_UBA) /* A EMS status report, read */
#define UBA_IMG_ID_EMS_VM             (( 36<<8) | WE_MODID_UBA) /* A EMS voice mail */
#define UBA_IMG_ID_EMS_VM_R           (( 37<<8) | WE_MODID_UBA) /* A EMS voice mail, read */
#define UBA_IMG_ID_EMS_DVM_R          (( 38<<8) | WE_MODID_UBA) /* A EMS discard voice mail */
#define UBA_IMG_ID_EMS_INCOMP         (( 39<<8) | WE_MODID_UBA) /* A EMS incomplete */
#define UBA_IMG_ID_EMS_LOCK_H_R       (( 40<<8) | WE_MODID_UBA) /* A EMS, forward locked, high priority, read */
#define UBA_IMG_ID_EMS_LOCK_L_R       (( 41<<8) | WE_MODID_UBA) /* A EMS, forward locked, low priority, read */
#define UBA_IMG_ID_EMS_LOCK_R         (( 42<<8) | WE_MODID_UBA) /* A EMS, forward locked, read */
#define UBA_IMG_ID_EMS_LOCK_H         (( 43<<8) | WE_MODID_UBA) /* A EMS, forward locked, high priority */
#define UBA_IMG_ID_EMS_LOCK_L         (( 44<<8) | WE_MODID_UBA) /* A EMS, forward locked, low priority */
#define UBA_IMG_ID_EMS_LOCK           (( 45<<8) | WE_MODID_UBA) /* A EMS, forward locked */

extern const unsigned char * const uba_images[UBA_NUMBER_OF_IMAGES+1];


#endif

