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
 * We_Cfg.h
 *
 * Created by
 *
 * Revision history:
 *
 *
 */
#ifndef _we_cfg_h
#define _we_cfg_h

#include "We_Def.h"

/**************************************************
 * Startup policy
 **************************************************/
#define WE_STARTUP_INTERNAL     1
#define WE_STARTUP_EXTERNAL     2
 
/**************************************************
 * Modules in this configuration
 **************************************************/
#define WE_MODID_FRW            0x00
#define WE_MODID_STK            0x01
#define WE_MODID_WBS            0x02
#define WE_MODID_PUS            0x03
#define WE_MODID_SEC            0x04
#define WE_MODID_WBA            0x05
#define WE_MODID_PRS            0x06
#define WE_MODID_MMS           0x07
#define WE_MODID_SIA            0x08
#define WE_MODID_UBS            0x09
#define WE_MODID_UBA            0x0A
#define WE_MODID_SIS            0x0B
#define WE_MODID_MSA           0x0C
#define WE_NUMBER_OF_MODULES    13



#define WE_MODULES_STARTUP { WE_STARTUP_INTERNAL, "FRW" ,    \
                              WE_STARTUP_INTERNAL, "STK" ,    \
                              WE_STARTUP_INTERNAL, "WBS" ,    \
                              WE_STARTUP_INTERNAL, "PUS" ,    \
                              WE_STARTUP_INTERNAL, "SEC" ,    \
                              WE_STARTUP_INTERNAL, "WBA" ,    \
                              WE_STARTUP_INTERNAL, "PRS" ,    \
                              WE_STARTUP_INTERNAL, "MMS" ,    \
                              WE_STARTUP_INTERNAL, "SIA" ,    \
                              WE_STARTUP_INTERNAL, "UBS" ,    \
                              WE_STARTUP_INTERNAL, "UBA" ,    \
                              WE_STARTUP_INTERNAL, "SIS" ,    \
                              WE_STARTUP_INTERNAL, "MSA" ,    \
                            }

/*Total solution module has signal queue, widget, file and other features*/
#define FRW_IS_TOTAL_SOLUTION_MODULE(mod_id) ((mod_id >= WE_MODID_FRW && mod_id < WE_NUMBER_OF_MODULES)?TRUE:FALSE)

/*****************************************************
 * Digital Rights Management Extension Package support
 *  - Define WE_CONFIG_DRE if DRE should be used
 *****************************************************/
/*#define WE_CONFIG_DRE*/

/**************************************************
 * Parsing
 **************************************************/
/* Flag to use SGML grammar for WML */
/*#define WE_CFG_EXTENDED_WML_PARSING */

/**************************************************
 * Logging
 **************************************************/

/* This flag decides whether the WE_LOG_MSG and WE_LOG_DATA macros will be compiled in */ 
/*(enabled). */
/* If set calls to TPIa_logMsg and TPIa_logData will be made. */
#define WE_LOG_MODULE

/* This flag decides whether The WE_LOG_SIG... macros will be compiled in */ 
/*(enabled). */
/* If set a call to TPIa_logSignal will be made for each signal send/recv */
/* that uses those macros. */
#define WE_LOG_SIGNALS



/**************************************************
 * Memory allocation
 **************************************************/
#if defined WE_CONFIG_INTERNAL_MALLOC
/* Define this to help find memory problems */
/* #define WE_CFG_DEBUG_INTERNAL_MALLOC */
#endif

/**************************************************
 * Temporary Folder - Must NOT be terminated by a slash
 **************************************************/
#define WE_CFG_TEMPORARY_FOLDER_PATH "/temp"

/**************************************************
 * Virtual Max Filename length (Extension included)
 * (this is the filename displayed to the user internally)
 **************************************************/
#define WE_CFG_VIRTUAL_FILE_NAME_LENGTH 40

/**************************************************
 * Timer defines
 *
 * Update definition of WE_TICKS_PER_SECOND.
 * It must be a value between 1 - 1000.
 **************************************************/
#define WE_TICKS_PER_SECOND 1000

/**************************************************
 * This flag sets the type of WeDisplayProperties::numberOfColors 
 * to WE_UINT32. 
 **************************************************/
/*
#define WE_CFG_DISP_NBR_OF_COLORS_UINT32
*/


/**********************************************************************
 * User Root Folders
 *
 * The User Root Folder are a set of folders that packages and Techfaith modules
 * use to store files in an orderly and convenient manner for the user.
 * It also allows for user-friendly, localized folder names.
 *
 * The structure must be as follows:
 *
 * Pathname            : The physical pathname to where the folder content
 *                       should be stored.
 * String Resource wid  : A resource wid to the string that will be displayed
 *                       to the user, e.g. the folder "/usr_data/picture/"
 *                       will be called "Mina Bilder" if Swedish were
 *                       selected as  current language.
 * mime type           : The mime types that will be possible to store in
 *                       this folder.
 * max directory depth : The maximum depth of the directory structure that
 *                       may be created. 1 indicates that no subdirectories
 *                       may be created under the root. 2 would indicate that
 *                       any number of subdirectories may be created
 *                       as long as they are the child of the root directory.
 * external              This is used to indicate if this User Root Folder is
 *                       mapped to an external memory device such as memory
 *                       card etc. 1 msans external and 0 internal.
 **********************************************************************/

/* A typical User Root Folder configuration may be defined as below:
*/
#define WE_NBR_OF_USER_ROOT_FOLDERS 6

#define WE_USER_ROOT_FOLDERS \
{ "/usr_data/picture/",    WE_STR_ID_ROOT_FOLDER_PICTURE, "image/*", 3, 0,\
  "/usr_data/sound/",      WE_STR_ID_ROOT_FOLDER_SOUND  , "audio/*", 3, 0,\
  "/usr_data/video/",      WE_STR_ID_ROOT_FOLDER_VIDEO  , "video/*", 1, 0,\
  "/usr_data/app/",        WE_STR_ID_ROOT_FOLDER_APP    , "application/*,application/x-techfaith-midlet-wid", 2, 0,\
  "/usr_data/anim/",       WE_STR_ID_ROOT_FOLDER_ANIM   , "application/x-techfaith-ems-animation", 1, 0, \
  "/external/c:usr_data/", WE_STR_ID_ROOT_FOLDER_EXT    , "*/*", 2, 1,\
}

/* A typical User Folder configuration may be defined as below:
 * Note: If user folders are nested, define the parent user folders ordered first.
 */
#define WE_NBR_OF_USER_FOLDERS 5

#define WE_USER_FOLDERS \
{ WE_STR_ID_USER_FOLDER_NATURE,   "/usr_data/picture/nature",    \
  WE_STR_ID_USER_FOLDER_WORK,     "/usr_data/picture/work",      \
  WE_STR_ID_USER_FOLDER_VACATION, "/usr_data/picture/vacation",  \
  WE_STR_ID_USER_FOLDER_MUSIC,    "/usr_data/sound/music",       \
  WE_STR_ID_USER_FOLDER_SOUNDS,   "/usr_data/sound/sounds",      \
}


/**********************************************************************
 * Mime Actions
 *
 * The Mime Actions stores actions. An action has a list of mime
 * types. The list records for which kind of files the action is applicable..
 * For each action, there is also an associated action name, intended to
 * be displayed in a menu to the user.
 *   . 
 *
 * The structure must be as follows:
 *
 * Mime types          : Mime types separated by comma.
 * Action command      : A string that identifies the action. For actions
 *                       executed internally, it may also be a command
 *                       string.
 *                        WE_PACKET_OPERATION_ID_SAS : command string
 *                            for save as operation.
 * Module wid           : The module to which an internal action will be
 *                       forwarded to for execution.
 *                       Use 0 if not applicable, e.g. for external actions
 *                       (see Action type below).
 *                       For packet and category type, use 0.
 *                       WE_MIME_ACTION_TYPE_INTERNAL actions, uses content
 *                       routing. Use a non-zero wid in these cases.
 *                       The non-zero wid implies direct content routing.
 *                       (WE_MIME_ACTION_TYPE_INTERNAL actions may be used
 *                       with zero wid, which implies that FRW will try to find a 
 *                       receiver module based on MIME registry information 
 *                       for content routing. Zero module wid for
 *                       WE_MIME_ACTION_TYPE_INTERNAL actions, may only be used
 *                       if it is certain that FRW will find a match in its
 *                       content routing descision.)
 * Action Name wid      : A resource wid to the localized string that will be  
 *                       displayed to the user.
 * Action type         : The type of action indicating if the action is 
 *                       realized internally, as an internal packet, externally
 *                       or if it is a category (see below).
 *                          WE_MIME_ACTION_TYPE_EXTERNAL
 *                          WE_MIME_ACTION_TYPE_INTERNAL
 *                          WE_MIME_ACTION_TYPE_PACKET  
 *                          WE_MIME_ACTION_TYPE_CATEGORY  
 * Category action     : The category to which this action belongs to.
 *                       Actions may be grouped into action categories.
 *                       In this case the category is represented by an
 *                       WE_MIME_ACTION_TYPE_CATEGORY action and a contained
 *                       action (sub-action) must in this field reference the
 *                       category. The reference is expressed as the index
 *                       (row nr - 1) into this table where the category action
 *                       is recorded. If the action does not belong to any catogory
 *                       this field must be set to -1.
 *                       Category actions are regarded as internal but may
 *                       contain external actions. Only WE_MIME_ACTION_TYPE_EXTERNAL 
 *                       actions are passed on externally..
 *                       NOTE: When adding or removing rows in the table,
 *                       remember to check/update this field for ALL remaining
 *                       rows. Also note, that sub-actions may not be category 
 *                       actions in turn, i.e. the category structure is only 
 *                       one level deep.
 * Blocked permissions : The denied or blocked DRM permissions (bits)
 *                       for the action. When a DRM permission is required, 
 *                       this field is checked wether it blocks (denies) 
 *                       the permission or not.
 *                       Use WE_PERMISSION_NONE_BLOCKED if all permissions
 *                       are granted. Use WE_PERMISSION_FORWARD_BLOCKED
 *                       if DRM forward permission is denied for the action.
 *                       Note, this field is only applicable when
 *                       permissions are required.
 * Action capability   : The capabilities (bits) of the action. For some
 *                       content there might be requirements on the
 *                       capabilities of the action, to regard the action
 *                       as applicable for the content. This field holds
 *                       such capabilities of the action. Use either
 *                       WE_ACTION_CAPABILITY_DRM if the action can
 *                       handle DRM content. Otherwise use
 *                       WE_ACTION_CAPABILITY_NONE. This field is
 *                       ignored for category actions. For such actions,
 *                       set this field to WE_ACTION_CAPABILITY_NONE.
 *
 **********************************************************************/

/* A typical Mime Action configuration may be defined as below */
#define WE_NBR_OF_MIME_ACTIONS 6

#define WE_MIME_ACTIONS  \
{ { "*/*", WE_PACKET_OPERATION_ID_SAS, 0, WE_STR_ID_MIME_ACTION_SAVE,                         \
    WE_MIME_ACTION_TYPE_PACKET,    -1, WE_PERMISSION_NONE_BLOCKED, WE_ACTION_CAPABILITY_NONE \
  },                                                                                            \
  { "", "set",                       0, WE_STR_ID_MIME_ACTION_SET,                             \
    WE_MIME_ACTION_TYPE_CATEGORY,  -1, WE_PERMISSION_NONE_BLOCKED, WE_ACTION_CAPABILITY_NONE \
  },                                                                                            \
  { "image/jpeg", "set screensaver", 0, WE_STR_ID_MIME_ACTION_SET_SCREENSAVER,                 \
    WE_MIME_ACTION_TYPE_EXTERNAL,   1, WE_PERMISSION_NONE_BLOCKED, WE_ACTION_CAPABILITY_NONE \
  },                                                                                            \
  { "image/jpeg", "set greeting",    0, WE_STR_ID_MIME_ACTION_SET_GREETING,                    \
    WE_MIME_ACTION_TYPE_EXTERNAL,   1, WE_PERMISSION_NONE_BLOCKED, WE_ACTION_CAPABILITY_NONE \
  },                                                                                            \
  { "image/jpeg", "set wallpaper",   0, WE_STR_ID_MIME_ACTION_SET_WALLPAPER,                   \
    WE_MIME_ACTION_TYPE_EXTERNAL,   1, WE_PERMISSION_NONE_BLOCKED, WE_ACTION_CAPABILITY_NONE \
  },                                                                                            \
  { "audio/*", "set ringtone",       0, WE_STR_ID_MIME_ACTION_SET_RINGTONE,                    \
    WE_MIME_ACTION_TYPE_EXTERNAL,   1, WE_PERMISSION_NONE_BLOCKED, WE_ACTION_CAPABILITY_NONE \
  }                                                                                             \
}

/* Example of an WE_MIME_ACTIONS record using an internal command to send as mms:
  { "image/jpeg", "-mode new -i", WE_MODID_MSA, WE_STR_ID_MIME_ACTION_SEND_AS_MMS,              \
    WE_MIME_ACTION_TYPE_INTERNAL, -1, WE_PERMISSION_FORWARD_BLOCKED, WE_ACTION_CAPABILITY_NONE \
  }                                                                                               \
*/

/**********************************************************************
 * Icon Table
 *
 * In this table it is possible to configure which icon to be associated
 * with at specific (or general) mime type. To ensure that all files 
 * always are associated with an icon the * / * type should be defined.
 * If both a string resource wid and a integer resource wid is defined 
 * for one item the string resource will be used.
 *
 * Note that the last element int the table must be NULL
 * 
 *
 * The structure must be as follows:
 *
 * Mime type          : Mime type to associate this icon with.
 * 
 * resource_type      : The type of resource defined. There are 
 *                      two types of icon resources.
 *                      WE_ICON_RESOURCE_TYPE_PREDEFINED
 *                      WE_ICON_RESOURCE_TYPE_FILE                            
 * resource_str_id    : This could be either a resource wid to
 *                      a predefined image or it could be a 
 *                      filename.
 * resource_int_id    : image resource identifier
 * 
 **********************************************************************/


#define WE_ICON_MIME_MAP_TABLE \
{ {"image/*"    ,WE_ICON_RESOURCE_TYPE_PREDEFINED, NULL , WE_IMAGE_ID_ICON_IMAGE_ANY  },\
  {"audio/*"    ,WE_ICON_RESOURCE_TYPE_PREDEFINED, NULL , WE_IMAGE_ID_ICON_AUDIO_ANY  },\
  {"image/jpeg" ,WE_ICON_RESOURCE_TYPE_PREDEFINED, NULL , WE_IMAGE_ID_ICON_IMAGE_JPEG },\
  {"*/*"        ,WE_ICON_RESOURCE_TYPE_PREDEFINED, NULL , WE_IMAGE_ID_ICON_ANY        },\
  {NULL         ,0                                , NULL , 0                            },\
}

#endif
