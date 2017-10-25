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
 * uba_rc.c
 * 
 * Description: This file implement the recources needed by the 
 *              UBA module.
 *    
 */

#include "Uba_Rc.h"
#include "Uba_Img.h"

/* String Resources */

const char * const uba_strings[][UBA_NUMBER_OF_STRINGS+1] = 
{ /*English*/
  {
    "Back",                       /*UBA_STR_ID_BACK*/
    "Create new SMS",             /*UBA_STR_ID_CREATE_NEW_SMS*/
    "Create new MMS",             /*UBA_STR_ID_CREATE_NEW_MMS*/
    "Create new message",         /*UBA_STR_ID_CREATE_NEW_MSG*/
    "Create new template",        /*UBA_STR_ID_CREATE_NEW_TEMPLATE*/
    "Delete",                     /*UBA_STR_ID_DELETE*/
    "Delete all",                 /*UBA_STR_ID_DELETE_ALL*/
    "Retrieve",                   /*UBA_STR_ID_RETRIEVE*/
    "Drafts",                     /*UBA_STR_ID_DRAFTS*/
    "Edit",                       /*UBA_STR_ID_EDIT*/
    "SMS menu",                   /*UBA_STR_ID_EMS_MENU*/
    "SMS template menu",          /*UBA_STR_ID_EMS_TEMPLATE_MENU*/
    "Folder actions",             /*UBA_STR_ID_FOLDER_ACTIONS*/
    "Forward",                    /*UBA_STR_ID_FORWARD*/
    "Inbox",                      /*UBA_STR_ID_INBOX*/
    "Load",                       /*UBA_STR_ID_LOAD*/
    "Messenger menu",             /*UBA_STR_ID_MESSENGER_MENU*/
    "MMS delivery report menu",   /*UBA_STR_ID_MMS_DELIVERY_REPORT_MENU*/
    "MMS message menu",           /*UBA_STR_ID_MMS_MSG_MENU*/
    "MMS notification menu",      /*UBA_STR_ID_MMS_NOTIFICATION_MENU*/
    "MMS read report menu",       /*UBA_STR_ID_MMS_READ_REPORT_MENU*/
    "MMS settings",               /*UBA_STR_ID_MMS_SETTINGS*/
    "MMS template menu",          /*UBA_STR_ID_MMS_TEMPLATE_MENU*/
    "Move",                       /*UBA_STR_ID_MOVE*/
    "Open",                       /*UBA_STR_ID_OPEN*/
    "Push SI menu",               /*UBA_STR_ID_PUSH_SI_MENU*/
    "Push SL ",                   /*UBA_STR_ID_PUSH_SL_MENU*/
    "Rename",                     /*UBA_STR_ID_RENAME*/
    "Reply",                      /*UBA_STR_ID_REPLY*/
    "Reply to all",               /*UBA_STR_ID_REPLY_ALL*/
    "Reply by MMS",               /*UBA_STR_ID_REPLY_BY_MMS*/
    "Reply by SMS",               /*UBA_STR_ID_REPLY_BY_SMS*/
    "Select",                     /*UBA_STR_ID_SELECT*/
    "Send",                       /*UBA_STR_ID_SEND*/
    "Sent",                       /*UBA_STR_ID_SENT*/
    "Settings",                   /*UBA_STR_ID_SETTINGS*/
    "SMS settings",               /*UBA_STR_ID_SMS_SETTINGS*/
    "Sort",                       /*UBA_STR_ID_SORT*/
    "Templates",                  /*UBA_STR_ID_TEMPLATES*/
    "View",                       /*UBA_STR_ID_VIEW*/
    " ",                          /*UBA_STR_ID_EMPTY*/
    "Menu",                       /*UBA_STR_ID_MENU*/
    "MMS draft menu",             /*UBA_STR_ID_MMS_MSG_DRAFT_MENU*/
    "SMS draft menu",             /*UBA_STR_ID_EMS_MSG_DRAFT_MENU*/
    "MMS sent menu",              /*UBA_STR_ID_MMS_MSG_SENT_MENU*/
    "SMS sent menu",              /*UBA_STR_ID_EMS_MSG_SENT_MENU*/
    "Friends",                    /*UBA_STR_ID_USER_FOLDER_0*/
    "Fun",                        /*UBA_STR_ID_USER_FOLDER_1*/
    "Important",                  /*UBA_STR_ID_USER_FOLDER_2*/
    "",                           /*UBA_STR_ID_NO_STR*/
    "From",                       /*UBA_STR_ID_FROM*/
    "Created",                    /*UBA_STR_ID_CREATED*/
    "Expires",                    /*UBA_STR_ID_EXPIRES*/
    "Url",                        /*UBA_STR_ID_URL*/
    "Message",                    /*UBA_STR_ID_MESSAGE*/
    "Accept",                     /*UBA_STR_ID_ACCEPT*/
    "Rename folder ",             /*UBA_STR_ID_RENAME_FOLDER*/ 
    "Move to folder",             /*UBA_STR_ID_MOVE_TO_FOLDER*/
    "Sort by date",               /*UBA_STR_ID_SORT_BY_DATE*/
    "Sort by type",               /*UBA_STR_ID_SORT_BY_TYPE*/
    "Sort by sender",             /*UBA_STR_ID_SORT_BY_SENDER*/
    "Ok",                         /*UBA_STR_ID_OK*/
    "Cancel",                     /*UBA_STR_ID_CANCEL*/
    "Delete this item?",          /*UBA_STR_ID_DELETE_ITEM*/
    "Delete all items in this folder?", /*UBA_STR_ID_DELETE_ALL_ITEMS*/
    "Create new SMS template",    /*UBA_STR_ID_CREATE_NEW_SMS_TEMPLATE*/
    "Create new MMS template",    /*UBA_STR_ID_CREATE_NEW_MMS_TEMPLATE*/
    "New SMS from template",      /*UBA_STR_ID_CREATE_SMS_FROM_TEMPLATE*/
    "New MMS from template",      /*UBA_STR_ID_CREATE_MMS_FROM_TEMPLATE*/
    "Sort by receiver",           /*UBA_STR_ID_SORT_BY_RECEIVER*/
    "Resend",                     /*UBA_STR_ID_RESEND*/
    "SMS status report menu",     /*UBA_STR_ID_EMS_STATUS_REPORT_MENU*/
    "SMS voice mail menu",        /*UBA_STR_ID_EMS_VOICE_MAIL_MENU*/
    "SMS discard voice mail menu",/*UBA_STR_ID_EMS_DISC_VOICE_MAIL_MENU*/
    "Call voice mail",            /*UBA_STR_ID_CALL_VOICE_MAIL*/
    "Voice mail",                 /*UBA_STR_ID_VOICE_MAIL*/
    "Set voice mail number ",     /*UBA_STR_ID_SET_VOICE_MAIL*/
    "SMS incomplete menu",        /*UBA_STR_ID_EMS_INCOMP_MENU*/
    NULL /*The table is NULL-terminated*/
  }
  /*Other language*/
 /*{
     Etc.
   }*/
};


const unsigned char * const uba_images[UBA_NUMBER_OF_IMAGES+1] = 
  {
    ubaImgEmpty,
    ubaImgFolder,
    ubaImgMms,
    ubaImgMmsRead,
    ubaImgMmsHigh,
    ubaImgMmsHighRead,
    ubaImgMmsLow,
    ubaImgMmsLowRead,
    ubaImgMmsLock,
    ubaImgMmsLockRead,
    ubaImgMmsLockHigh,
    ubaImgMmsLockHighRead,
    ubaImgMmsLockLow,
    ubaImgMmsLockLowRead,
    ubaImgMmsOutbox,
    ubaImgMmsNotify,
    ubaImgMmsDeliveryReport,
    ubaImgMmsDeliveryReportRead,
    ubaImgMmsReadReport,
    ubaImgMmsReadReportRead,
    ubaImgEms,
    ubaImgEmsRead,
    ubaImgEmsHigh,
    ubaImgEmsHighRead,
    ubaImgEmsLow,
    ubaImgEmsLowRead,
    ubaImgEmsOutbox,
    ubaImgPush,
    ubaImgPushRead,
    ubaImgPushHigh,
    ubaImgPushHighRead,
    ubaImgPushLow,
    ubaImgPushLowRead,
    ubaImgEmsStatusReport,
    ubaImgEmsStatusReportRead,
    ubaImgEmsVoiceMail,
    ubaImgEmsVoiceMailRead,
    ubaImgEmsDiscVoiceMail,
    ubaImgEmsIncomplete,
    ubaImgEmsLockHighRead,
    ubaImgEmsLockLowRead,
    ubaImgEmsLockRead,
    ubaImgEmsLockHigh,
    ubaImgEmsLockLow,
    ubaImgEmsLock,
    NULL /*The table is NULL-terminated*/
  };
