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

#include "Msa_Rc.h"

/*! \file msa_rc.c
 *  \brief Language definitions for the MSA application.
 */

/******************************************************************************
 * Constants
 *****************************************************************************/

/* String Resources */

const char * const msa_strings[][MSA_NUMBER_OF_STRINGS + 1] = 
{ /*English*/
  {
    "",                                             /* MSA_STR_ID_EMPTY_STRING */
    "Back",                                         /* MSA_STR_ID_BACK */
    "Cancel",                                       /* MSA_STR_ID_CANCEL */
    "OK",                                           /* MSA_STR_ID_OK */
    "Select",                                       /* MSA_STR_ID_SELECT */
    "On",                                           /* MSA_STR_ID_ON */
    "Off",                                          /* MSA_STR_ID_OFF */
    "View",                                         /* MSA_STR_ID_VIEW */
    "Next",                                         /* MSA_STR_ID_NEXT */
    "Save",                                         /* MSA_STR_ID_SAVE */
    "From: ",                                       /* MSA_STR_ID_FROM */
    "Cc: ",                                         /* MSA_STR_ID_CC */
    "To: ",                                         /* MSA_STR_ID_TO */
    "Subject: ",                                    /* MSA_STR_ID_SUBJECT */
    "kB",                                           /* MSA_STR_ID_KB */
    "Remove object",                                /* MSA_STR_ID_REM_MEDIA_OBJ */
    "Options",                                      /* MSA_STR_ID_OPTIONS */
    "Yes",                                          /* MSA_STR_ID_YES */
    "No",                                           /* MSA_STR_ID_NO */
    "Edit",                                         /* MSA_STR_ID_EDIT */
    "View message",                                 /* MSA_STR_ID_VIEW_TITLE */
    "View attachments",                             /* MSA_STR_ID_VIEW_ATTACHMENTS */
    "View multimedia presentation",                 /* MSA_STR_ID_VIEW_SMIL */
    "Read report",                                  /* MSA_STR_ID_READ_REPORT */
    "Delivery report",                              /* MSA_STR_ID_DELIV_REPORT */
    "Status:",                                      /* MSA_STR_ID_STATUS */
    "Date:",                                        /* MSA_STR_ID_DATE */
    "Expired.",                                     /* MSA_STR_ID_EXPIRED */
    "Retrieved.",                                   /* MSA_STR_ID_RETRIEVED */
    "Rejected.",                                    /* MSA_STR_ID_REJECTED */
    "Deferred.",                                    /* MSA_STR_ID_DEFERRED */
    "Unrecognized.",                                /* MSA_STR_ID_UNRECOGNIZED */
    "Indeterminate.",                               /* MSA_STR_ID_INDETERMINATE */
    "Forwarded.",                                   /* MSA_STR_ID_FORWARDED */
    "Unknown.",                                     /* MSA_STR_ID_UNKOWN */
    "Read.",                                        /* MSA_STR_ID_READ */
    "Deleted, not read.",                           /* MSA_STR_ID_NOT_READ */
    "Sending",                                      /* MSA_STR_ID_CONFIG_SENDING */
    "Filters",                                      /* MSA_STR_ID_CONFIG_MSG_FILTERS */
    "Edit content",                                 /* MSA_STR_ID_EDIT_CONTENT */
    "Edit text",                                    /* MSA_STR_ID_EDIT_TEXT */
    "Message editor",                               /* MSA_STR_ID_ME_TITLE */
    "Add Image",                                    /* MSA_STR_ID_ADD_IMAGE */
    "Preview",                                      /* MSA_STR_ID_PREVIEW_SMIL */
    "Insert new page",                              /* MSA_STR_ID_INSERT_SLIDE */
    "Delete page",                                  /* MSA_STR_ID_DELETE_SLIDE */
    "Next page",                                    /* MSA_STR_ID_NEXT_SLIDE */
    "Previous page",                                /* MSA_STR_ID_PREVIOUS_SLIDE */
    "Add media object",                             /* MSA_STR_ID_ADD_MEDIA_OBJECT */
    "Add text",                                     /* MSA_STR_ID_ADD_TEXT */
    "Page timing",                                  /* MSA_STR_ID_DURATION */
    "Treat as delayed",                             /* MSA_STR_ID_CONFIG_TREAT_AS_DEL */
    "Immediate limit",                              /* MSA_STR_ID_CONFIG_IM_SIZE */
    "Anonymous senders",                            /* MSA_STR_ID_CONFIG_ANONYM */
    "Retrieval",                                    /* MSA_STR_ID_CONFIG_MSG_RETRIEVAL */
    "Immediate server",                             /* MSA_STR_ID_CONFIG_IM_SERVER */
    "Settings",                                     /* MSA_STR_ID_CONFIG */
    "Network account",                              /* MSA_STR_ID_NETWORK_ACCOUNT */
    "Connection type",                              /* MSA_STR_ID_CONN_TYPE */
    "HTTP",                                         /* MSA_STR_ID_CONN_TYPE_HTTP */
    "WSP CL",                                       /* MSA_STR_ID_CONN_TYPE_WSP_CL */
    "WSP CO",                                       /* MSA_STR_ID_CONN_TYPE_WSP_CO */
    "User",                                         /* MSA_STR_ID_CONFIG_USER */
    "MMSC",                                         /* MSA_STR_ID_CONFIG_MMSC */
    "Network",                                      /* MSA_STR_ID_CONFIG_NETWORK */
    "Login",                                        /* MSA_STR_ID_LOGIN */
    "Password",                                     /* MSA_STR_ID_PASSWD */
    "Realm",                                        /* MSA_STR_ID_REALM */
    "Secure port",                                  /* MSA_STR_ID_SEC_PORT */
    "HTTP port",                                    /* MSA_STR_ID_HTTP_PORT */
    "WAP gateway",                                  /* MSA_STR_ID_WAP_GATEWAY */
    "Automatic",                                    /* MSA_STR_ID_IM_RETRIEVAL */
    "Always ask",                                   /* MSA_STR_ID_DEL_RETRIEVAL */
    "Auto save",                                    /* MSA_STR_ID_CONFIG_SOS */
    "Read report",                                  /* MSA_STR_ID_CONFIG_RR */
    "Delivery report",                              /* MSA_STR_ID_CONFIG_DR */
    "Failed to open the originating message since it contains unsupported header fields!", /* MSA_STR_ID_UNSUPPORTED_HEADER_FIELDS */
    "Validity period",                              /* MSA_STR_ID_CONFIG_VAL_PER */
    "Priority",                                     /* MSA_STR_ID_CONFIG_PRIORITY */
    "Allow",                                        /* MSA_STR_ID_ALLOW */
    "Reject",                                       /* MSA_STR_ID_REJECT */
    "Downloading message.",                         /* MSA_STR_ID_DOWNLOADING_MM */
    "Generating message.",                          /* MSA_STR_ID_GENERATING_MESSAGE */
    "Sending message.",                             /* MSA_STR_ID_SENDING_MSG */
    " bytes received.",                             /* MSA_STR_ID_BYTES_RECEIVED */
    "Downloading message. Status: ",                /* MSA_STR_ID_DOWNLOAD_STATUS */
    "Sending message. Status: ",                    /* MSA_STR_ID_SEND_STATUS */
    " bytes sent.",                                 /* MSA_STR_ID_BYTES_SENT */
    "connecting.",                                  /* MSA_STR_ID_CONNECTING */
    "Delete",                                       /* MSA_STR_ID_DELETE */
    "Send",                                         /* MSA_STR_ID_SEND */
    "Forward",                                      /* MSA_STR_ID_FORWARD */
    "Reply",                                        /* MSA_STR_ID_REPLY */
    "1 Hour",                                       /* MSA_STR_ID_CONFIG_1_HOUR */
    "12 Hours",                                     /* MSA_STR_ID_CONFIG_12_HOURS */
    "1 Day",                                        /* MSA_STR_ID_CONFIG_1_DAY */
    "1 Week",                                       /* MSA_STR_ID_CONFIG_1_WEEK */
    "Max",                                          /* MSA_STR_ID_CONFIG_MAX */
    "Low",                                          /* MSA_STR_ID_CONFIG_LOW */
    "Normal",                                       /* MSA_STR_ID_CONFIG_NORMAL */
    "High",                                         /* MSA_STR_ID_CONFIG_HIGH */
    "Save as template",                             /* MSA_STR_ID_SAVE_AS_TEMPLATE */
    "Menu",                                         /* MSA_STR_ID_MENU */
    "Message is invalid!",                          /* MSA_STR_ID_INVALID_MESSAGE */
    "Not enough message storage. Delete messages to free storage space.", /* MSA_STR_ID_NO_PERSISTENT_STORAGE */
    "Not enough memory to complete this operation!", /* MSA_STR_ID_NOT_ENOUGH_MEM */
    "Failed to display the message!",               /* MSA_STR_ID_FAILED_TO_DISPLAY_MM */
    "Error",                                        /* MSA_STR_ID_ERROR */
    "Internal error. The application is going to close.", /* MSA_STR_ID_PANIC */
    "Invalid from address!",                        /* MSA_STR_ID_INVALID_ADDRESS */
    "Invalid proxy address!",                       /* MSA_STR_ID_INVALID_PROXY_HOST */
    "Invalid proxy port!",                          /* MSA_STR_ID_INVALID_PROXY_PORT */
    "Invalid WAP gateway!",                         /* MSA_STR_ID_INVALID_WAP_GATEWAY */
    "Invalid proxy scheme!",                        /* MSA_STR_ID_INVALID_PROXY_SCHEME */
    "Message download failed!",                     /* MSA_STR_ID_DOWNLOAD_FAILED */
    "Failed to send message!",                      /* MSA_STR_ID_FAILED_TO_SEND */
    "Do you want to delete the message?",           /* MSA_STR_ID_DELETE_MSG_QUEST */
    "Message was sent successfully.",               /* MSA_STR_ID_MESSAGE_SENT */
    "A read report has been requested for this message. Do you want to send a read report?", /* MSA_STR_ID_READ_REPORT_SEND */
    "Reading message...",                           /* MSA_STR_ID_READING_MSG */
    "Failed to save message!",                      /* MSA_STR_ID_FAILED_TO_SAVE */
    "Unsupported message type!",                    /* MSA_STR_ID_WRONG_MESSAGE_TYPE */
    "Unable to show attachment!",                   /* MSA_STR_ID_ATTACH_ERROR */
    "Maximum number of pages reached!",             /* MSA_STR_ID_SLIDE_LIMIT_REACHED */
    "The configuration is not valid. Please check \"From\" address and the MMSC version number.", /* MSA_STR_ID_ME_INVALID_CONFIG */
    "Invalid input value!",                         /* MSA_STR_ID_INVALID_VALUE */
    "An error occured when trying to read the image to attach!", /* MSA_STR_ID_CRR_FAILED_TO_READ */
    "Failed to edit the message!",                  /* MSA_STR_ID_FAILED_TO_EDIT */
    "At least one address field must be filled in.", /* MSA_STR_ID_ADDRESS_FIELD_MISSING */
    "The media object could not be added to the message!", /* MSA_STR_ID_FAILED_TO_READ_MO */
    "Erroneous notification!",                      /* MSA_STR_ID_FAILED_TO_DISPLAY_NOTIF */
    "Do you want to download the message with size ", /* MSA_STR_ID_DOWNLOAD_REQ1 */
    " bytes?",                                      /* MSA_STR_ID_DOWNLOAD_REQ2 */
    "The read report could not be sent!",           /* MSA_STR_ID_FAILED_TO_SEND_RR */
    "Invalid value, please enter a value between ", /* MSA_STR_ID_INTERVAL_PREFIX */
    " and ",                                        /* MSA_STR_ID_INTERVAL_INFIX */
    "A too large message was blocked",              /* MSA_STR_ID_RETR_SIZE_EXCEEDED */
    "The message content is invalid!",              /* MSA_STR_ID_PLY_SMIL_FAILED */
    "Message creation failed. System resources exhausted!", /* MSA_STR_ID_EDIT_FAILED_RESOURCES_EXHAUSTED */
    "PERSONAL messages",                            /* MSA_STR_ID_CONFIG_PERS */
    "ADVERTISEMENT messages",                       /* MSA_STR_ID_CONFIG_ADVERT */
    "INFORMATIONAL messages",                       /* MSA_STR_ID_CONFIG_INFO */
    "AUTO messages",                                /* MSA_STR_ID_CONFIG_AUTO */
    "STRING messages",                              /* MSA_STR_ID_CONFIG_USER_STRING */
    "Phone book",                                   /* MSA_STR_ID_PHONE_BOOK */
    "Too many recipients.",                         /* MSA_STR_ID_TO_MANY_RECIPIENTS */
    "Invalid recipient address!",                   /* MSA_STR_ID_INVALID_RECIP_ADDRESS */
    "Recipients:",                                  /* MSA_STR_ID_RECIPIENTS */
    "Add recipient?",                               /* MSA_STR_ID_ADD_RECIPIENT_QUEST */
    " recipients.",                                 /* MSA_STR_ID_NO_OF_RECIPIENTS */
    "The message contains locked information and connot be edited!", /* MSA_STR_ID_FAILED_FORWARD_LOCK */
    "Max retrieval size",                           /* MSA_STR_ID_MAX_RETR_SIZE */
    "RE: ",                                         /* MSA_STR_ID_RE */
    "FW: ",                                         /* MSA_STR_ID_FW */
    "Maximum message size reached",                 /* MSA_STR_ID_MESSAGE_SIZE_MAXIMUM_REACHED */
    "Recipients may not be able to view messages this large", /* MSA_STR_ID_MESSAGE_SIZE_WARNING */
    "Reply charging",                               /* MSA_STR_ID_REPLY_CHARGING */
    "Reply charging deadline",                      /* MSA_STR_ID_REPLY_CHARGING_DEADLINE */
    "Reply charging size",                          /* MSA_STR_ID_REPLY_CHARGING_SIZE */
    "Text only",                                    /* MSA_STR_ID_TEXT_ONLY */
    "1 KB",                                         /* MSA_STR_ID_CONFIG_1_KB */
    "10 KB",                                        /* MSA_STR_ID_CONFIG_10_KB */
    "Select color",                                 /* MSA_STR_ID_SELECT_COLOR */
    "Background color",                             /* MSA_STR_ID_BG_COLOR */
    "Home network",                                 /* MSA_STR_ID_CONFIG_MSG_RETR_HOME_MODE */
    "Roaming network",                              /* MSA_STR_ID_CONFIG_MSG_RETR_ROAM_MODE */
    "Automatic",                                    /* MSA_STR_ID_AUTO_RETRIEVAL_ROAM */
    "Always ask",                                   /* MSA_STR_ID_ASK_RETRIEVAL_ROAM */
    "Restricted",                                   /* MSA_STR_ID_RES_RETRIEVAL_ROAM */
    "Show number",                                  /* MSA_STR_ID_CONFIG_SEND_VISIBILITY */
    "Yes",                                          /* MSA_STR_ID_SHOW_NUMBER */
    "No",                                           /* MSA_STR_ID_HIDE_NUMBER */
    "Send read report",                             /* MSA_STR_ID_CONFIG_SEND_READ_REPORT */
    "Always",                                       /* MSA_STR_ID_CONFIG_ALWAYS */
    "On request",                                   /* MSA_STR_ID_CONFIG_ON_REQUEST */
    "Never",                                        /* MSA_STR_ID_CONFIG_NEVER */
    "Delivery time",                                /* MSA_STR_ID_CONFIG_DELIVERY_TIME */
    "Immediate",                                    /* MSA_STR_ID_CONFIG_IMMEDIATE */
    "In one hour",                                  /* MSA_STR_ID_CONFIG_IN_ONE_HOUR */
    "In 12 hours",                                  /* MSA_STR_ID_CONFIG_IN_12_HOURS */
    "In 24 hours",                                  /* MSA_STR_ID_CONFIG_IN_24_HOURS */
    "Play sound",                                   /* MSA_STR_ID_PLAY_SOUND */
    "Delivery report allowed",                      /* MSA_STR_ID_CONFIG_DELIVERY_REPORT_ALLOWED */
    "Add audio",                                    /* MSA_STR_ID_ADD_AUDIO */
    "Text color",                                   /* MSA_STR_ID_TEXT_COLOR */
    "Text Colour, All Slides",                                   /* MSA_STR_ID_TEXT_COLOR_ALL */
    "Bcc: ",                                        /* MSA_STR_ID_BCC */
    "The text can not be displayed",                /* MSA_STR_ID_TEXT_MISSING */
    "There are media objects in this message that might be too large for some recipients.", /* MSA_STR_ID_MULTIPLE_OBJECTS_SIZE_WARNING */
    "Recipients may not be able to view media objects of this size", /* MSA_STR_ID_SINGLE_OBJECT_SIZE_WARNING */
    "You are currently not in your home network and there might be additional charges on transactions. Do you want to download the message with size ", /* MSA_STR_ID_ROAM_DOWNLOAD_REQ1 */
    " bytes?",                                      /* MSA_STR_ID_ROAM_DOWNLOAD_REQ2 */
    "You are currently not in your home network and there might be additional charges on transactions. Do you want to send the message with size ", /* MSA_STR_ID_ROAM_SEND_REQ1 */
    " bytes?",                                      /* MSA_STR_ID_ROAM_SEND_REQ2 */
    "Your network settings are invalid. Please check MMSC and WAP settings.", /* MSA_STR_ID_INVALID_NETWORK_SETTINGS */
    "The called party is busy",                     /* MSA_STR_ID_CALL_BUSY */
    "The network is not available",                 /* MSA_STR_ID_CALL_NO_NETWORK */
    "The called party did not answer",              /* MSA_STR_ID_CALL_NO_ANSWER */
    "The number is not a phone-number",             /* MSA_STR_ID_CALL_NOT_A_NUMBER */
    "Could not place call",                         /* MSA_STR_ID_CALL_ERROR */
    "Failed to open original message",              /* MSA_STR_ID_FAILED_TO_GET_MESSAGE_INFO */
    "Failed to open window",                        /* MSA_STR_ID_FAILED_TO_OPEN_WINDOW */
    "Start time: ",                                 /* MSA_STR_ID_OBJ_START */
    "Duration: ",                                   /* MSA_STR_ID_OBJ_DURATION */
    "Item timing",                                  /* MSA_STR_ID_ITEM_TIMING */
    " seconds",                                     /* MSA_STR_ID_SECONDS */
    "Start time",                                   /* MSA_STR_ID_START_TIME */
    "maximum",                                      /* MSA_STR_ID_MAXIMUM */
    "The item timing is changed in order to be within page duration bounds!", /* MSA_STR_ID_TIMING_OUT_OF_BOUNDS */
    "page:",                                        /* MSA_STR_ID_PAGE_COLON */
    "s",                                            /* MSA_STR_ID_SHORT_SECOND */
    "Item duration",                                /* MSA_STR_ID_OBJ_OB_DURATION */
    "The message was not accepted by the server due to size, media type, copyrights or some other reason.", /* MSA_STR_ID_FAILED_TO_SEND_REFUSED */
    "Unable to connect to server!",                 /* MSA_STR_ID_SERVER_ERROR */
    "Connection timeout!",                          /* MSA_STR_ID_COMM_TIMEOUT */
    "Invalid MMS version!",                         /* MSA_STR_ID_INVALID_MMS_VERSION */
    "This MMS Client doesn't support handling of DRM-protected content!",                         /* MSA_STR_ID_NO_DRM_SUPPORT */
    "No Content exist in mms file !",                                                                                 /* MSA_STR_ID_MMS_EMPTY */                          
    "Save template",                                /* MSA_STR_ID_SAVE_TEMPLATE */
    "Signature",                                    /* MSA_STR_ID_CONFIG_SIGNATURE */
    "Use signature",                                /* MSA_STR_ID_CONFIG_USE_SIGNATURE */
    "Edit signature",                               /* MSA_STR_ID_CONFIG_EDIT_SIGNATURE */
    NULL /*The table is NULL-terminated*/
  }
  /*Other language*/
 /*{
     Etc.
   }*/
};

/******************************************************************************
 * Image data
 *****************************************************************************/

/*! BMP  */
static const unsigned char msaBrokenImg[]={
    0x42, 0x4d, 0x2a, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x76,
    0x0, 0x0, 0x0, 0x28, 0x0, 0x0, 0x0, 0x15, 0x0, 0x0, 0x0, 0xf,
    0x0, 0x0, 0x0, 0x1, 0x0, 0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0xb4,
    0x0, 0x0, 0x0, 0xc4, 0xe, 0x0, 0x0, 0xc4, 0xe, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x80, 0x0, 0x0, 0x80, 0x0, 0x0, 0x0, 0x80, 0x80, 0x0, 0x80,
    0x0, 0x0, 0x0, 0x80, 0x0, 0x80, 0x0, 0x80, 0x80, 0x0, 0x0, 0x80,
    0x80, 0x80, 0x0, 0xc0, 0xc0, 0xc0, 0x0, 0x0, 0x0, 0xff, 0x0, 0x0,
    0xff, 0x0, 0x0, 0x0, 0xff, 0xff, 0x0, 0xff, 0x0, 0x0, 0x0, 0xff,
    0x0, 0xff, 0x0, 0xff, 0xff, 0x0, 0x0, 0xff, 0xff, 0xff, 0x0, 0x99,
    0x99, 0x99, 0x9f, 0xff, 0xff, 0xff, 0x99, 0x99, 0x99, 0x90, 0x0, 0xf9,
    0x99, 0x99, 0x99, 0xff, 0xff, 0xf9, 0x99, 0x99, 0x99, 0xf0, 0x0, 0xff,
    0x99, 0x99, 0x99, 0x9f, 0xff, 0x99, 0x99, 0x99, 0x9f, 0xf0, 0x0, 0xff,
    0xf9, 0x99, 0x99, 0x99, 0xf9, 0x99, 0x99, 0x99, 0xff, 0xf0, 0x0, 0xff,
    0xff, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x9f, 0xff, 0xf0, 0x0, 0xff,
    0xff, 0xf9, 0x99, 0x99, 0x99, 0x99, 0x99, 0xff, 0xff, 0xf0, 0x0, 0xff,
    0xff, 0xff, 0x99, 0x99, 0x99, 0x99, 0x9f, 0xff, 0xff, 0xf0, 0x0, 0xff,
    0xff, 0xff, 0xf9, 0x99, 0x99, 0x99, 0xff, 0xff, 0xff, 0xf0, 0x0, 0xff,
    0xff, 0xff, 0x99, 0x99, 0x99, 0x99, 0x9f, 0xff, 0xff, 0xf0, 0x0, 0xff,
    0xff, 0xf9, 0x99, 0x99, 0x99, 0x99, 0x99, 0xff, 0xff, 0xf0, 0x0, 0xff,
    0xff, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x9f, 0xff, 0xf0, 0x0, 0xff,
    0xf9, 0x99, 0x99, 0x99, 0xf9, 0x99, 0x99, 0x99, 0xff, 0xf0, 0x0, 0xff,
    0x99, 0x99, 0x99, 0x9f, 0xff, 0x99, 0x99, 0x99, 0x9f, 0xf0, 0x0, 0xf9,
    0x99, 0x99, 0x99, 0xff, 0xff, 0xf9, 0x99, 0x99, 0x99, 0xf0, 0x0, 0x99,
    0x99, 0x99, 0x9f, 0xff, 0xff, 0xff, 0x99, 0x99, 0x99, 0x90, 0x0};

/*! BMP  */
static const unsigned char msaAudio[]={
    0x42, 0x4d, 0xf6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x76,
    0x0, 0x0, 0x0, 0x28, 0x0, 0x0, 0x0, 0x10, 0x0, 0x0, 0x0, 0x10,
    0x0, 0x0, 0x0, 0x1, 0x0, 0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x80,
    0x0, 0x0, 0x0, 0xc4, 0xe, 0x0, 0x0, 0xc4, 0xe, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x80, 0x0, 0x0, 0x80, 0x0, 0x0, 0x0, 0x80, 0x80, 0x0, 0x80,
    0x0, 0x0, 0x0, 0x80, 0x0, 0x80, 0x0, 0x80, 0x80, 0x0, 0x0, 0x80,
    0x80, 0x80, 0x0, 0xc0, 0xc0, 0xc0, 0x0, 0x0, 0x0, 0xff, 0x0, 0x0,
    0xff, 0x0, 0x0, 0x0, 0xff, 0xff, 0x0, 0xff, 0x0, 0x0, 0x0, 0xff,
    0x0, 0xff, 0x0, 0xff, 0xff, 0x0, 0x0, 0xff, 0xff, 0xff, 0x0, 0x88,
    0x88, 0x88, 0x80, 0x8, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x3, 0x7,
    0x88, 0x88, 0x88, 0x88, 0x88, 0x80, 0x30, 0x88, 0x78, 0x88, 0x78, 0x88,
    0x88, 0x3, 0x77, 0xff, 0x78, 0x87, 0x88, 0x88, 0x80, 0x3b, 0x8, 0xff,
    0x78, 0x78, 0x88, 0x80, 0x3, 0xbf, 0x8, 0xff, 0x78, 0x88, 0x88, 0x3,
    0x7b, 0xfb, 0x0, 0xff, 0x78, 0x88, 0x88, 0x3b, 0x8f, 0xbf, 0x7, 0xf,
    0x78, 0x77, 0x77, 0x3b, 0x8b, 0xfb, 0x8, 0xf, 0x78, 0x88, 0x88, 0x3f,
    0x8f, 0xbf, 0x0, 0xff, 0x78, 0x88, 0x88, 0x83, 0x3b, 0xfb, 0x8, 0xff,
    0x78, 0x78, 0x88, 0x88, 0x83, 0xbf, 0x8, 0xff, 0x78, 0x87, 0x88, 0x88,
    0x88, 0x3b, 0x77, 0x8f, 0x78, 0x88, 0x78, 0x88, 0x88, 0x83, 0xb0, 0x88,
    0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x37, 0x7, 0x88, 0x88, 0x88, 0x88,
    0x88, 0x88, 0x80, 0x8, 0x88, 0x88, 0x88};

const unsigned char * const msa_images[MSA_NUMBER_OF_IMAGES + 1] = 
{
    msaBrokenImg, 
    msaAudio, 
    NULL /*The table is NULL-terminated*/
};
