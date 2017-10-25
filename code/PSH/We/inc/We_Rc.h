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


#ifndef _we_rc_h
#define _we_rc_h

#ifndef _we_cfg_h 
#include "We_Cfg.h"
#endif

#define WE_NUMBER_OF_STRINGS 59

extern const char * const we_strings[][WE_NUMBER_OF_STRINGS+1];

#define WE_STR_ID_ROOT_FOLDER_PICTURE         (WE_MODID_FRW + 0x0100)  /*"Pictures"*/
#define WE_STR_ID_ROOT_FOLDER_SOUND           (WE_MODID_FRW + 0x0200)  /*"Sounds"*/
#define WE_STR_ID_ROOT_FOLDER_VIDEO           (WE_MODID_FRW + 0x0300)  /*"Videos"*/
#define WE_STR_ID_ROOT_FOLDER_APP             (WE_MODID_FRW + 0x0400)  /*"Applications"*/
#define WE_STR_ID_ROOT_FOLDER_ANIM            (WE_MODID_FRW + 0x0500)  /*"Animations"*/
#define WE_STR_ID_ROOT_FOLDER_EXT             (WE_MODID_FRW + 0x0600)  /*"External"*/
#define WE_STR_ID_DLG_ACTION_OK               (WE_MODID_FRW + 0x0700)  /*"Ok"*/
#define WE_STR_ID_DLG_ACTION_BACK             (WE_MODID_FRW + 0x0800)  /*"Back"*/
#define WE_STR_ID_DLG_ACTION_CANCEL           (WE_MODID_FRW + 0x0900)  /*"Cancel"*/
#define WE_STR_ID_DLG_ACTION_SAVE             (WE_MODID_FRW + 0x0a00)  /*"Save"*/
#define WE_STR_ID_DLG_TEXT_EDIT_FILENAME      (WE_MODID_FRW + 0x0b00)  /*"Edit filename"*/
#define WE_STR_ID_DLG_TEXT_ROOT_FOLDER        (WE_MODID_FRW + 0x0c00)  /*"Root folder"*/
#define WE_STR_ID_DLG_TEXT_SAVE_HERE          (WE_MODID_FRW + 0x0d00)  /*"Save here"*/
#define WE_STR_ID_DLG_EXISTS_TEXT1            (WE_MODID_FRW + 0x0e00)  /*""*/
#define WE_STR_ID_DLG_EXISTS_TEXT2            (WE_MODID_FRW + 0x0f00)  /*" already exists. Do you want to replace it?"*/
#define WE_STR_ID_DLG_EXISTS_TITLE            (WE_MODID_FRW + 0x1000)  /*"File exists"*/
#define WE_STR_ID_DLG_NO_REPLACE_TEXT1        (WE_MODID_FRW + 0x1100)  /*""*/
#define WE_STR_ID_DLG_NO_REPLACE_TEXT2        (WE_MODID_FRW + 0x1200)  /*" already exists and cannot be replaced."*/
#define WE_STR_ID_DLG_NO_REPLACE_TITLE        (WE_MODID_FRW + 0x1300)  /*"File exists"*/
#define WE_STR_ID_DLG_SAVE_ERROR              (WE_MODID_FRW + 0x1400)  /*"Save Error"*/
#define WE_STR_ID_DLG_SAVE_NOT_ALLOWED        (WE_MODID_FRW + 0x1500)  /*"This file type may not be saved."*/
#define WE_STR_ID_USER_FOLDER_NATURE          (WE_MODID_FRW + 0x1600)  /*"Nature pictures"*/
#define WE_STR_ID_USER_FOLDER_WORK            (WE_MODID_FRW + 0x1700)  /*"Work pictures"*/
#define WE_STR_ID_USER_FOLDER_VACATION        (WE_MODID_FRW + 0x1800)  /*"Vacation pictures"*/
#define WE_STR_ID_USER_FOLDER_MUSIC           (WE_MODID_FRW + 0x1900)  /*"My music"*/
#define WE_STR_ID_USER_FOLDER_SOUNDS          (WE_MODID_FRW + 0x1a00)  /*"My sounds"*/
#define WE_STR_ID_SAS_BAR_TITLE               (WE_MODID_FRW + 0x1b00)  /*"Saving..."*/
#define WE_STR_ID_SAS_ACTION_CANCEL           (WE_MODID_FRW + 0x1c00)  /*"Cancel"*/
#define WE_STR_ID_SAS_ERROR_TITLE             (WE_MODID_FRW + 0x1d00)  /*"Unexpected error"*/
#define WE_STR_ID_SAS_ERROR_FULL              (WE_MODID_FRW + 0x1e00)  /*"The memory is full"*/
#define WE_STR_ID_SAS_ERROR_SIZE              (WE_MODID_FRW + 0x1f00)  /*"File size exceeds the file system maximum file size"*/
#define WE_STR_ID_MIME_ACTION_SAVE            (WE_MODID_FRW + 0x2000)  /*"Save As"*/
#define WE_STR_ID_MIME_ACTION_SEND_AS_MMS     (WE_MODID_FRW + 0x2100)  /*"Send as MMS"*/
#define WE_STR_ID_MIME_ACTION_SET             (WE_MODID_FRW + 0x2200)  /*"Set as..."*/
#define WE_STR_ID_MIME_ACTION_SET_SCREENSAVER (WE_MODID_FRW + 0x2300)  /*"Set screensaver"*/
#define WE_STR_ID_MIME_ACTION_SET_GREETING    (WE_MODID_FRW + 0x2400)  /*"Set greeting"*/
#define WE_STR_ID_MIME_ACTION_SET_WALLPAPER   (WE_MODID_FRW + 0x2500)  /*"Set wallpaper"*/
#define WE_STR_ID_DLG_ACTION_CREATE_DIR       (WE_MODID_FRW + 0x2600)  /*"Create folder"*/
#define WE_STR_ID_DLG_TITLE_CREATE_DIR        (WE_MODID_FRW + 0x2700)  /*"Create new folder"*/
#define WE_STR_ID_DLG_ERROR_CREATE_DIR        (WE_MODID_FRW + 0x2800)  /*"Could not create folder with the specified name."*/
#define WE_STR_ID_DLG_COMMON_ERROR            (WE_MODID_FRW + 0x2900)  /*"Error"*/
#define WE_STR_ID_DLG_ERROR_NAME              (WE_MODID_FRW + 0x2a00)  /*"The specified name is not valid."*/
#define WE_STR_ID_SAS_PERCENT                 (WE_MODID_FRW + 0x2b00)  /*" % done."*/
#define WE_STR_ID_SAS_BYTES_SAVED             (WE_MODID_FRW + 0x2c00)  /*" bytes saved. "*/
#define WE_STR_ID_DLG_CANNOT_SAVE             (WE_MODID_FRW + 0x2d00)  /*"Cannot save this file type!"*/
#define WE_STR_ID_PROPERTIES_SIZE2            (WE_MODID_FRW + 0x2e00)  /*"."*/
#define WE_STR_ID_PROPERTIES_SIZE3            (WE_MODID_FRW + 0x2f00)  /*" kBytes"*/
#define WE_STR_ID_PROPERTIES_SIZE4            (WE_MODID_FRW + 0x3000)  /*" MBytes"*/
#define WE_STR_ID_PROPERTIES_SIZE5            (WE_MODID_FRW + 0x3100)  /*" Bytes"*/
#define WE_STR_ID_DLG_MIME_ACTION_NONE        (WE_MODID_FRW + 0x3200)  /*"Unsupported content."*/
#define WE_STR_ID_MIME_ACTION_SET_RINGTONE    (WE_MODID_FRW + 0x3300)  /*"Set ringtone"*/
#define WE_STR_ID_DLG_ERROR_REPLACING_FILE    (WE_MODID_FRW + 0x3400)  /*"Unable to replace file"*/ 
#define WE_STR_ID_DLG_DIR_NOT_ALLOWED         (WE_MODID_FRW + 0x3500)  /*"Sub folders not allowed"*/
#define WE_STR_ID_ROOT_FOLDER_BKM             (WE_MODID_FRW + 0x3600)  /*"My Bookmarks"*/
#define WE_STR_ID_ROOT_FOLDER_OFLN_PGS        (WE_MODID_FRW + 0x3700)  /*"Offline Pages"*/
#define WE_STR_ID_ROOT_FOLDER_BKM_PERS        (WE_MODID_FRW + 0x3800)  /*"Persistent Bookmarks"*/

#define WE_STR_ID_MIME_ACTION_MT1_TEST1       (WE_MODID_FRW + 0x3900)  /*"MT1 obj-act test1"*/
#define WE_STR_ID_MIME_ACTION_MT1_TEST2       (WE_MODID_FRW + 0x3a00)  /*"MT1 obj-act test2"*/
#define WE_STR_ID_MIME_ACTION_MT1_TEST3       (WE_MODID_FRW + 0x3b00)  /*"MT1 obj-act test3"*/
/*Predefined images*/
#define WE_NUMBER_OF_IMAGES 11

extern const unsigned char * const we_images[WE_NUMBER_OF_IMAGES+1];

#define WE_IMAGE_ID_ICON_IMAGE_ANY            (WE_MODID_FRW + 0x0100)
#define WE_IMAGE_ID_ICON_IMAGE_JPEG           (WE_MODID_FRW + 0x0200) 
#define WE_IMAGE_ID_ICON_ANY                  (WE_MODID_FRW + 0x0300) 
#define WE_IMAGE_ID_ICON_AUDIO_ANY            (WE_MODID_FRW + 0x0400) 
#define WE_IMAGE_ID_ICON_MESSAGE_ALER         (WE_MODID_FRW + 0x0500)
#define WE_IMAGE_ID_ICON_MESSAGE_WARNING      (WE_MODID_FRW + 0x0600)
#define WE_IMAGE_ID_ICON_MESSAGE_ERROR        (WE_MODID_FRW + 0x0700)
#define WE_IMAGE_ID_ICON_MESSAGE_INFO         (WE_MODID_FRW + 0x0800)
#define WE_IMAGE_ID_ICON_MESSAGE_CONFIRM      (WE_MODID_FRW + 0x0900)
#define WE_IMAGE_ID_ICON_FOLDER               (WE_MODID_FRW + 0x0a00)
#define WE_IMAGE_ID_ICON_FILE                 (WE_MODID_FRW + 0x0b00)

#endif
