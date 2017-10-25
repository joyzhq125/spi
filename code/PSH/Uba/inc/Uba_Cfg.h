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
 * uba_cfg.h
 *
 *
 */

#ifndef UBA_CFG_H 
#define UBA_CFG_H

/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef UBS_IF_H
  #include "Ubs_If.h"
#endif

/************************************************************************/
/* Message types                                                        */
/************************************************************************/

/* 
Add or remove which message types whose messages will be fetched through the UBS API.
*/
#define UBA_HANDLED_MSG_TYPES             \
{                                         \
  UBA_MSG_TYPE (UBS_MSG_TYPE_PUSH_SI),    \
  UBA_MSG_TYPE (UBS_MSG_TYPE_PUSH_SL),    \
  UBA_MSG_TYPE (UBS_MSG_TYPE_MMS),        \
  UBA_MSG_TYPE (UBS_MSG_TYPE_EMS),        \
                                          \
  /* put msg types above this line */     \
  UBA_MSG_TYPE (UBA_END_MSG_TYPE_LIST)    \
}

/************************************************************************/
/* Folders                                                              */
/************************************************************************/

/* 
 * Enumeration of folders, add remove folders when needed, but keep in sync with
 * UBA_FOLDER_NAMES and make sure the first enum has the value 0 and that the rest of the 
 * folders are numbered from 0 to UBA_FOLDER_COUNT - 1 with no integers unassigned.
 */
typedef enum
{
  UBA_INBOX_FOLDER_ID = 0,
  UBA_DRAFTS_FOLDER_ID,
  UBA_SENT_FOLDER_ID,
  UBA_TEMPLATES_FOLDER_ID,
  /* User defined folders below */
  UBA_FIRST_USER_DEF_FOLDER,
  UBA_USER_DEF_FOLDER_0_ID = UBA_FIRST_USER_DEF_FOLDER,
  UBA_USER_DEF_FOLDER_1_ID,
  UBA_USER_DEF_FOLDER_2_ID,

  /* PLACE folder IDs above this line */
  UBA_FOLDER_COUNT,
  UBA_NO_FOLDER_ID = UBA_FOLDER_COUNT
}uba_folder_id_t;

/*
 *  By using the enumeration value for a folder as index
 *  into this array, the result is a string resource identifier
 *  which will be used as the GUI name. Second parameter if to make ite possible to
 *  lookup the ubs folder that represents this GUI folder.
 */
#define UBA_FOLDER_NAMES                                                                  \
{                                                                                         \
  { /*UBA_INBOX_FOLDER_ID*/      UBA_STR_ID_INBOX        , UBS_MSG_KEY_VALUE_FOLDER_INBOX             },\
  { /*UBA_DRAFTS_FOLDER_ID*/     UBA_STR_ID_DRAFTS       , UBS_MSG_KEY_VALUE_FOLDER_DRAFTS            },\
  { /*UBA_SENT_FOLDER_ID*/       UBA_STR_ID_SENT         , UBS_MSG_KEY_VALUE_FOLDER_SENT              },\
  { /*UBA_TEMPLATES_FOLDER_ID*/  UBA_STR_ID_TEMPLATES    , UBS_MSG_KEY_VALUE_FOLDER_TEMPLATES         },\
  { /*UBA_USER_DEF_FOLDER_0_ID*/ UBA_STR_ID_USER_FOLDER_0, UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_0        },\
  { /*UBA_USER_DEF_FOLDER_1_ID*/ UBA_STR_ID_USER_FOLDER_1, UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_1        },\
  { /*UBA_USER_DEF_FOLDER_2_ID*/ UBA_STR_ID_USER_FOLDER_2, UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_2        } \
}

/*
 *  This array maps the value of a UBS_MSG_KEY_FOLDER property to a folder in the UBA GUI
 *  The first value in each slot is the internal folder name.
 *  The second value in each slot is the corresponding folder index.
 *  This array makes it possible to map many internal folders to a less number of actual
 *  folders in the GUI. It is also possible to have them all end up in one GUI folder
 *  independently of the internal folder name. To do this kind of default mapping use
 *  NULL instead of a internal folder name.
 */
#define UBA_FOLDER_MAPPING                                                             \
{                                                                                      \
  {UBA_ALL_MESSAGE_TYPES, UBS_MSG_KEY_VALUE_FOLDER_INBOX     , UBA_INBOX_FOLDER_ID         },        \
  {UBA_ALL_MESSAGE_TYPES, UBS_MSG_KEY_VALUE_FOLDER_DRAFTS    , UBA_DRAFTS_FOLDER_ID        },        \
  {UBA_ALL_MESSAGE_TYPES, UBS_MSG_KEY_VALUE_FOLDER_SENT      , UBA_SENT_FOLDER_ID          },        \
  {UBA_ALL_MESSAGE_TYPES, UBS_MSG_KEY_VALUE_FOLDER_OUTBOX    , UBA_DRAFTS_FOLDER_ID        },        \
  {UBA_ALL_MESSAGE_TYPES, UBS_MSG_KEY_VALUE_FOLDER_TEMPLATES , UBA_TEMPLATES_FOLDER_ID     },        \
  {UBA_ALL_MESSAGE_TYPES, UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_0, UBA_USER_DEF_FOLDER_0_ID    },        \
  {UBA_ALL_MESSAGE_TYPES, UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_1, UBA_USER_DEF_FOLDER_1_ID    },        \
  {UBA_ALL_MESSAGE_TYPES, UBS_MSG_KEY_VALUE_FOLDER_USER_DEF_2, UBA_USER_DEF_FOLDER_2_ID    },        \
  /* Place folder mappings above */                                                    \
  {UBA_ALL_MESSAGE_TYPES, NULL                               , UBA_INBOX_FOLDER_ID}  /* Default folder wid if no match found*/\
}

/************************************************************************/
/* Registry related definitions                                         */
/************************************************************************/
#define UBA_REG_PATH_SETTINGS           "/UBA/settings"
#define UBA_REG_KEY_VOICE_MAIL_NUMBER   "vmailnum"

#define UBA_DEFAULT_VOICE_MAIL_NUMBER   ""

#endif      /*UBA_CFG_H*/
