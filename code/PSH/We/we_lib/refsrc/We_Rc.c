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
 * We_Rc.h
 * 
 * Description: This file implement the Common resources needed 
 *              by FRW, Packages, and other modules.
 *    
 */

#include "We_Rc.h"

/* String Resources */

const char * const we_strings[][WE_NUMBER_OF_STRINGS+1] = 
{ /*English*/
  {
    "My Pictures",                                    /*WE_STR_ID_ROOT_FOLDER_PICTURE    */
    "My Sounds",                                      /*WE_STR_ID_ROOT_FOLDER_SOUND      */
    "My Videos",                                      /*WE_STR_ID_ROOT_FOLDER_VIDEO      */
    "My Applications",                                /*WE_STR_ID_ROOT_FOLDER_APP        */
    "My Animations",                                  /*WE_STR_ID_ROOT_FOLDER_ANIM       */
    "Memory Card",                                    /*WE_STR_ID_ROOT_FOLDER_EXT*/
    "Ok",                                             /*WE_STR_ID_DLG_ACTION_OK              */
    "Back",                                           /*WE_STR_ID_DLG_ACTION_BACK            */
    "Cancel",                                         /*WE_STR_ID_DLG_ACTION_CANCEL          */
    "Save",                                           /*WE_STR_ID_DLG_ACTION_SAVE            */
    "Edit filename",                                  /*WE_STR_ID_DLG_TEXT_EDIT_FILENAME     */
    "My Content",                                     /*WE_STR_ID_DLG_TEXT_ROOT_FOLDER       */
    "Save here",                                      /*WE_STR_ID_DLG_TEXT_SAVE_HERE         */
    "",                                               /*WE_STR_ID_DLG_EXISTS_TEXT1 */
    " already exists. Do you want to replace it?",    /*WE_STR_ID_DLG_EXISTS_TEXT2 */
    "File exists",                                    /*WE_STR_ID_DLG_EXISTS_TITLE */
    "",                                               /*WE_STR_ID_DLG_NO_REPLACE_TEXT1 */
    " already exists and cannot be replaced.",        /*WE_STR_ID_DLG_NO_REPLACE_TEXT2 */
    "File exists",                                    /*WE_STR_ID_DLG_NO_REPLACE_TITLE */
    "Save Error",                                     /*WE_STR_ID_DLG_SAVE_ERROR*/
    "This file type may not be saved.",               /*WE_STR_ID_DLG_SAVE_NOT_ALLOWED*/
    "Nature pictures",                                /*WE_STR_ID_USER_FOLDER_NATURE*/
    "Work pictures",                                  /*WE_STR_ID_USER_FOLDER_WORK*/
    "Vacation pictures",                              /*WE_STR_ID_USER_FOLDER_VACATION*/
    "My music",                                       /*WE_STR_ID_USER_FOLDER_MUSIC*/
    "My sounds",                                      /*WE_STR_ID_USER_FOLDER_SOUNDS*/
    "Saving...",                                      /*WE_STR_ID_SAS_BAR_TITLE*/
    "Cancel",                                         /*WE_STR_ID_SAS_ACTION_CANCEL*/
    "Unexpected error",                               /*WE_STR_ID_SAS_ERROR_TITLE */
    "The memory is full",                             /*WE_STR_ID_SAS_ERROR_FULL */
    "File size exceeds the file system maximum file size", /*WE_STR_ID_SAS_ERROR_SIZE*/
    "Save As",                                         /*WE_STR_ID_MIME_ACTION_SAVE*/
    "Send as MMS",                                     /*WE_STR_ID_MIME_ACTION_SEND_AS_MMS*/
    "Set as...",                                       /*WE_STR_ID_MIME_ACTION_SET*/
    "Set screensaver",                                 /*WE_STR_ID_MIME_ACTION_SET_SCREENSAVER*/
    "Set greeting",                                    /*WE_STR_ID_MIME_ACTION_SET_GREETING*/
    "Set wallpaper",                                   /*WE_STR_ID_MIME_ACTION_SET_WALLPAPER*/
    "Create folder",                                   /*WE_STR_ID_DLG_ACTION_CREATE_DIR*/
    "Create new folder",                               /*WE_STR_ID_DLG_TITLE_CREATE_DIR*/
    "Cannot create folder with the specified name.",   /*WE_STR_ID_DLG_ERROR_CREATE_DIR*/
    "Error",                                           /*WE_STR_ID_DLG_COMMON_ERROR*/
    "The specified name is invalid.",                  /*WE_STR_ID_DLG_ERROR_NAME*/
    " % done.",                                        /*WE_STR_ID_SAS_PERCENT*/
    " bytes saved.\r\n",                               /*WE_STR_ID_SAS_BYTES_SAVED*/
    "Cannot save this file type!",                     /*WE_STR_ID_DLG_CANNOT_SAVE*/
    ".",                                               /*WE_STR_ID_PROPERTIES_SIZE2*/
    " kBytes",                                         /*WE_STR_ID_PROPERTIES_SIZE3*/
    " MBytes",                                         /*WE_STR_ID_PROPERTIES_SIZE4*/
    " Bytes",                                          /*WE_STR_ID_PROPERTIES_SIZE5*/
    "Unsupported content.",                            /*WE_STR_ID_DLG_MIME_ACTION_NONE*/
    "Set ringtone",                                    /*WE_STR_ID_MIME_ACTION_SET_RINGTONE*/
    "Unable to replace file",                          /*WE_STR_ID_DLG_ERROR_REPLACING_FILE*/
    "Sub folders not allowed",                         /*WE_STR_ID_DLG_DIR_NOT_ALLOWED*/
    "My Bookmarks",                                    /*WE_STR_ID_ROOT_FOLDER_BOOKMARK*/
    "Offline Pages",                                   /*WE_STR_ID_ROOT_FOLDER_OFLN_PGS*/
    "Persistent Bookmarks",                            /*WE_STR_ID_ROOT_FOLDER_BKM_PERS*/

    "MT1 obj-act test1",                               /*WE_STR_ID_MIME_ACTION_MT1_TEST1*/
    "MT1 obj-act test2",                               /*WE_STR_ID_MIME_ACTION_MT1_TEST2*/
    "MT1 obj-act test3",                               /*WE_STR_ID_MIME_ACTION_MT1_TEST3*/

    NULL /*The table is NULL-terminated*/
  }
  /*Other language*/
 /*{
     Etc.
   }*/
};

static const unsigned  char we_icon_image_any[] = {
#include "image_any.inc"
};

static const unsigned  char we_icon_image_jpeg[] = {
#include "image_jpeg.inc"
};
static const unsigned  char we_icon_any[] = {
#include "icon_any.inc"
};
static const unsigned  char we_icon_audio_any[] = {
#include "audio_any.inc"
};

static const unsigned char we_icon_dlg_alert[] = {
#include "dlg_alert.inc"
};
static const unsigned char we_icon_dlg_warning[] = {
#include "dlg_warning.inc"
};
static const unsigned char we_icon_dlg_info[] = {
#include "dlg_info.inc"
};
static const unsigned char we_icon_dlg_error[] = {
#include "dlg_error.inc"
};
static const unsigned char we_icon_dlg_confirm[] = {
#include "dlg_confirm.inc"
};

static const unsigned char we_icon_dlg_folder[] = {
#include "dlg_folder.inc"
};

static const unsigned char we_icon_dlg_file[] = {
#include "dlg_file.inc"
};


const unsigned char * const we_images[WE_NUMBER_OF_IMAGES+1] = 
  {
    we_icon_image_any,
    we_icon_image_jpeg,
    we_icon_any,
    we_icon_audio_any,
    we_icon_dlg_alert,
    we_icon_dlg_warning,
    we_icon_dlg_info,
    we_icon_dlg_error,
    we_icon_dlg_confirm,
    we_icon_dlg_folder,
    we_icon_dlg_file,
    NULL /*The table is NULL-terminated*/
  };
