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

/*! \file msa_rc.h
 *  \brief Language definitions for the MSA application.
 */

#ifndef _MSA_RC_H_
#define _MSA_RC_H_

#include "We_Cfg.h"

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! The total number of strings defined for the MSA application.
 *
 */
#define MSA_NUMBER_OF_STRINGS  222

/*! The strings table for the MSA application.
 *
 */
extern const char * const msa_strings[][MSA_NUMBER_OF_STRINGS + 1];

/* "" */
#define MSA_STR_ID_EMPTY_STRING                             ((1<<8) | WE_MODID_MSA)
/* "Back" */
#define MSA_STR_ID_BACK                                     ((2<<8) | WE_MODID_MSA)
/* "Cancel" */
#define MSA_STR_ID_CANCEL                                   ((3<<8) | WE_MODID_MSA)
/* "OK" */
#define MSA_STR_ID_OK                                       ((4<<8) | WE_MODID_MSA)
/* "Select" */
#define MSA_STR_ID_SELECT                                   ((5<<8) | WE_MODID_MSA)
/* "On" */
#define MSA_STR_ID_ON                                       ((6<<8) | WE_MODID_MSA)
/* "Off" */
#define MSA_STR_ID_OFF                                      ((7<<8) | WE_MODID_MSA)
/* "View" */
#define MSA_STR_ID_VIEW                                     ((8<<8) | WE_MODID_MSA)
/* "Next" */
#define MSA_STR_ID_NEXT                                     ((9<<8) | WE_MODID_MSA)
/* "Save" */
#define MSA_STR_ID_SAVE                                     ((10<<8) | WE_MODID_MSA)
/* "From: " */
#define MSA_STR_ID_FROM                                     ((11<<8) | WE_MODID_MSA)
/* "Cc: " */
#define MSA_STR_ID_CC                                       ((12<<8) | WE_MODID_MSA)
/* "To: " */
#define MSA_STR_ID_TO                                       ((13<<8) | WE_MODID_MSA)
/* "Subject: " */
#define MSA_STR_ID_SUBJECT                                  ((14<<8) | WE_MODID_MSA)
/* "kB" */
#define MSA_STR_ID_KB                                       ((15<<8) | WE_MODID_MSA)
/* "Remove object" */
#define MSA_STR_ID_REM_MEDIA_OBJ                            ((16<<8) | WE_MODID_MSA)
/* "Options" */
#define MSA_STR_ID_OPTIONS                                  ((17<<8) | WE_MODID_MSA)
/* "Yes" */
#define MSA_STR_ID_YES                                      ((18<<8) | WE_MODID_MSA)
/* "No" */
#define MSA_STR_ID_NO                                       ((19<<8) | WE_MODID_MSA)
/* "Edit" */
#define MSA_STR_ID_EDIT                                     ((20<<8) | WE_MODID_MSA)
/* "View message" */
#define MSA_STR_ID_VIEW_TITLE                               ((21<<8) | WE_MODID_MSA)
/* "View attachments" */
#define MSA_STR_ID_VIEW_ATTACHMENTS                         ((22<<8) | WE_MODID_MSA)
/* "View multimedia presentation" */
#define MSA_STR_ID_VIEW_SMIL                                ((23<<8) | WE_MODID_MSA)
/* "Read report" */
#define MSA_STR_ID_READ_REPORT                              ((24<<8) | WE_MODID_MSA)
/* "Delivery report" */
#define MSA_STR_ID_DELIV_REPORT                             ((25<<8) | WE_MODID_MSA)
/* "Status:" */
#define MSA_STR_ID_STATUS                                   ((26<<8) | WE_MODID_MSA)
/* "Date:" */
#define MSA_STR_ID_DATE                                     ((27<<8) | WE_MODID_MSA)
/* "Expired." */
#define MSA_STR_ID_EXPIRED                                  ((28<<8) | WE_MODID_MSA)
/* "Retrieved." */
#define MSA_STR_ID_RETRIEVED                                ((29<<8) | WE_MODID_MSA)
/* "Rejected." */
#define MSA_STR_ID_REJECTED                                 ((30<<8) | WE_MODID_MSA)
/* "Deferred." */
#define MSA_STR_ID_DEFERRED                                 ((31<<8) | WE_MODID_MSA)
/* "Unrecognized." */
#define MSA_STR_ID_UNRECOGNIZED                             ((32<<8) | WE_MODID_MSA)
/* "Indeterminate." */
#define MSA_STR_ID_INDETERMINATE                            ((33<<8) | WE_MODID_MSA)
/* "Forwarded." */
#define MSA_STR_ID_FORWARDED                                ((34<<8) | WE_MODID_MSA)
/* "Unknown." */
#define MSA_STR_ID_UNKOWN                                   ((35<<8) | WE_MODID_MSA)
/* "Read." */
#define MSA_STR_ID_READ                                     ((36<<8) | WE_MODID_MSA)
/* "Deleted, not read." */
#define MSA_STR_ID_NOT_READ                                 ((37<<8) | WE_MODID_MSA)
/* "Sending" */
#define MSA_STR_ID_CONFIG_SENDING                           ((38<<8) | WE_MODID_MSA)
/* "Filters" */
#define MSA_STR_ID_CONFIG_MSG_FILTERS                       ((39<<8) | WE_MODID_MSA)
/* "Edit content" */
#define MSA_STR_ID_EDIT_CONTENT                             ((40<<8) | WE_MODID_MSA)
/* "Edit text" */
#define MSA_STR_ID_EDIT_TEXT                                ((41<<8) | WE_MODID_MSA)
/* "Message editor" */
#define MSA_STR_ID_ME_TITLE                                 ((42<<8) | WE_MODID_MSA)
/* "Add Image" */
#define MSA_STR_ID_ADD_IMAGE                                ((43<<8) | WE_MODID_MSA)
/* "Preview" */
#define MSA_STR_ID_PREVIEW_SMIL                             ((44<<8) | WE_MODID_MSA)
/* "Insert new page" */
#define MSA_STR_ID_INSERT_SLIDE                             ((45<<8) | WE_MODID_MSA)
/* "Delete page" */
#define MSA_STR_ID_DELETE_SLIDE                             ((46<<8) | WE_MODID_MSA)
/* "Next page" */
#define MSA_STR_ID_NEXT_SLIDE                               ((47<<8) | WE_MODID_MSA)
/* "Previous page" */
#define MSA_STR_ID_PREVIOUS_SLIDE                           ((48<<8) | WE_MODID_MSA)
/* "Add media object" */
#define MSA_STR_ID_ADD_MEDIA_OBJECT                         ((49<<8) | WE_MODID_MSA)
/* "Add text" */
#define MSA_STR_ID_ADD_TEXT                                 ((50<<8) | WE_MODID_MSA)
/* "Page timing" */
#define MSA_STR_ID_DURATION                                 ((51<<8) | WE_MODID_MSA)
/* "Treat as delayed" */
#define MSA_STR_ID_CONFIG_TREAT_AS_DEL                      ((52<<8) | WE_MODID_MSA)
/* "Immediate limit" */
#define MSA_STR_ID_CONFIG_IM_SIZE                           ((53<<8) | WE_MODID_MSA)
/* "Anonymous senders" */
#define MSA_STR_ID_CONFIG_ANONYM                            ((54<<8) | WE_MODID_MSA)
/* "Retrieval" */
#define MSA_STR_ID_CONFIG_MSG_RETRIEVAL                     ((55<<8) | WE_MODID_MSA)
/* "Immediate server" */
#define MSA_STR_ID_CONFIG_IM_SERVER                         ((56<<8) | WE_MODID_MSA)
/* "Settings" */
#define MSA_STR_ID_CONFIG                                   ((57<<8) | WE_MODID_MSA)
/* "Network account" */
#define MSA_STR_ID_NETWORK_ACCOUNT                          ((58<<8) | WE_MODID_MSA)
/* "Connection type" */
#define MSA_STR_ID_CONN_TYPE                                ((59<<8) | WE_MODID_MSA)
/* "HTTP" */
#define MSA_STR_ID_CONN_TYPE_HTTP                           ((60<<8) | WE_MODID_MSA)
/* "WSP CL" */
#define MSA_STR_ID_CONN_TYPE_WSP_CL                         ((61<<8) | WE_MODID_MSA)
/* "WSP CO" */
#define MSA_STR_ID_CONN_TYPE_WSP_CO                         ((62<<8) | WE_MODID_MSA)
/* "User" */
#define MSA_STR_ID_CONFIG_USER                              ((63<<8) | WE_MODID_MSA)
/* "MMSC" */
#define MSA_STR_ID_CONFIG_MMSC                              ((64<<8) | WE_MODID_MSA)
/* "Network" */
#define MSA_STR_ID_CONFIG_NETWORK                           ((65<<8) | WE_MODID_MSA)
/* "Login" */
#define MSA_STR_ID_LOGIN                                    ((66<<8) | WE_MODID_MSA)
/* "Password" */
#define MSA_STR_ID_PASSWD                                   ((67<<8) | WE_MODID_MSA)
/* "Realm" */
#define MSA_STR_ID_REALM                                    ((68<<8) | WE_MODID_MSA)
/* "Secure port" */
#define MSA_STR_ID_SEC_PORT                                 ((69<<8) | WE_MODID_MSA)
/* "HTTP port" */
#define MSA_STR_ID_HTTP_PORT                                ((70<<8) | WE_MODID_MSA)
/* "WAP gateway" */
#define MSA_STR_ID_WAP_GATEWAY                              ((71<<8) | WE_MODID_MSA)
/* "Automatic" */
#define MSA_STR_ID_IM_RETRIEVAL                             ((72<<8) | WE_MODID_MSA)
/* "Always ask" */
#define MSA_STR_ID_DEL_RETRIEVAL                            ((73<<8) | WE_MODID_MSA)
/* "Auto save" */
#define MSA_STR_ID_CONFIG_SOS                               ((74<<8) | WE_MODID_MSA)
/* "Read report" */
#define MSA_STR_ID_CONFIG_RR                                ((75<<8) | WE_MODID_MSA)
/* "Delivery report" */
#define MSA_STR_ID_CONFIG_DR                                ((76<<8) | WE_MODID_MSA)
/* "Failed to open the originating message since it contains unsupported header fields!" */
#define MSA_STR_ID_UNSUPPORTED_HEADER_FIELDS                ((77<<8) | WE_MODID_MSA)
/* "Validity period" */
#define MSA_STR_ID_CONFIG_VAL_PER                           ((78<<8) | WE_MODID_MSA)
/* "Priority" */
#define MSA_STR_ID_CONFIG_PRIORITY                          ((79<<8) | WE_MODID_MSA)
/* "Allow" */
#define MSA_STR_ID_ALLOW                                    ((80<<8) | WE_MODID_MSA)
/* "Reject" */
#define MSA_STR_ID_REJECT                                   ((81<<8) | WE_MODID_MSA)
/* "Downloading message." */
#define MSA_STR_ID_DOWNLOADING_MM                           ((82<<8) | WE_MODID_MSA)
/* "Generating message." */
#define MSA_STR_ID_GENERATING_MESSAGE                       ((83<<8) | WE_MODID_MSA)
/* "Sending message." */
#define MSA_STR_ID_SENDING_MSG                              ((84<<8) | WE_MODID_MSA)
/* " bytes received." */
#define MSA_STR_ID_BYTES_RECEIVED                           ((85<<8) | WE_MODID_MSA)
/* "Downloading message. Status: " */
#define MSA_STR_ID_DOWNLOAD_STATUS                          ((86<<8) | WE_MODID_MSA)
/* "Sending message. Status: " */
#define MSA_STR_ID_SEND_STATUS                              ((87<<8) | WE_MODID_MSA)
/* " bytes sent." */
#define MSA_STR_ID_BYTES_SENT                               ((88<<8) | WE_MODID_MSA)
/* "connecting." */
#define MSA_STR_ID_CONNECTING                               ((89<<8) | WE_MODID_MSA)
/* "Delete" */
#define MSA_STR_ID_DELETE                                   ((90<<8) | WE_MODID_MSA)
/* "Send" */
#define MSA_STR_ID_SEND                                     ((91<<8) | WE_MODID_MSA)
/* "Forward" */
#define MSA_STR_ID_FORWARD                                  ((92<<8) | WE_MODID_MSA)
/* "Reply" */
#define MSA_STR_ID_REPLY                                    ((93<<8) | WE_MODID_MSA)
/* "1 Hour" */
#define MSA_STR_ID_CONFIG_1_HOUR                            ((94<<8) | WE_MODID_MSA)
/* "12 Hours" */
#define MSA_STR_ID_CONFIG_12_HOURS                          ((95<<8) | WE_MODID_MSA)
/* "1 Day" */
#define MSA_STR_ID_CONFIG_1_DAY                             ((96<<8) | WE_MODID_MSA)
/* "1 Week" */
#define MSA_STR_ID_CONFIG_1_WEEK                            ((97<<8) | WE_MODID_MSA)
/* "Max" */
#define MSA_STR_ID_CONFIG_MAX                               ((98<<8) | WE_MODID_MSA)
/* "Low" */
#define MSA_STR_ID_CONFIG_LOW                               ((99<<8) | WE_MODID_MSA)
/* "Normal" */
#define MSA_STR_ID_CONFIG_NORMAL                            ((100<<8) | WE_MODID_MSA)
/* "High" */
#define MSA_STR_ID_CONFIG_HIGH                              ((101<<8) | WE_MODID_MSA)
/* "Save as template" */
#define MSA_STR_ID_SAVE_AS_TEMPLATE                         ((102<<8) | WE_MODID_MSA)
/* "Menu" */
#define MSA_STR_ID_MENU                                     ((103<<8) | WE_MODID_MSA)
/* "Message is invalid!" */
#define MSA_STR_ID_INVALID_MESSAGE                          ((104<<8) | WE_MODID_MSA)
/* "Not enough message storage. Delete messages to free storage space." */
#define MSA_STR_ID_NO_PERSISTENT_STORAGE                    ((105<<8) | WE_MODID_MSA)
/* "Not enough memory to complete this operation!" */
#define MSA_STR_ID_NOT_ENOUGH_MEM                           ((106<<8) | WE_MODID_MSA)
/* "Failed to display the message!" */
#define MSA_STR_ID_FAILED_TO_DISPLAY_MM                     ((107<<8) | WE_MODID_MSA)
/* "Error" */
#define MSA_STR_ID_ERROR                                    ((108<<8) | WE_MODID_MSA)
/* "Internal error. The application is going to close." */
#define MSA_STR_ID_PANIC                                    ((109<<8) | WE_MODID_MSA)
/* "Invalid from address!" */
#define MSA_STR_ID_INVALID_ADDRESS                          ((110<<8) | WE_MODID_MSA)
/* "Invalid proxy address!" */
#define MSA_STR_ID_INVALID_PROXY_HOST                       ((111<<8) | WE_MODID_MSA)
/* "Invalid proxy port!" */
#define MSA_STR_ID_INVALID_PROXY_PORT                       ((112<<8) | WE_MODID_MSA)
/* "Invalid WAP gateway!" */
#define MSA_STR_ID_INVALID_WAP_GATEWAY                      ((113<<8) | WE_MODID_MSA)
/* "Invalid proxy scheme!" */
#define MSA_STR_ID_INVALID_PROXY_SCHEME                     ((114<<8) | WE_MODID_MSA)
/* "Message download failed!" */
#define MSA_STR_ID_DOWNLOAD_FAILED                          ((115<<8) | WE_MODID_MSA)
/* "Failed to send message!" */
#define MSA_STR_ID_FAILED_TO_SEND                           ((116<<8) | WE_MODID_MSA)
/* "Do you want to delete the message?" */
#define MSA_STR_ID_DELETE_MSG_QUEST                         ((117<<8) | WE_MODID_MSA)
/* "Message was sent successfully." */
#define MSA_STR_ID_MESSAGE_SENT                             ((118<<8) | WE_MODID_MSA)
/* "A read report has been requested for this message. Do you want to send a read report?" */
#define MSA_STR_ID_READ_REPORT_SEND                         ((119<<8) | WE_MODID_MSA)
/* "Reading message..." */
#define MSA_STR_ID_READING_MSG                              ((120<<8) | WE_MODID_MSA)
/* "Failed to save message!" */
#define MSA_STR_ID_FAILED_TO_SAVE                           ((121<<8) | WE_MODID_MSA)
/* "Unsupported message type!" */
#define MSA_STR_ID_WRONG_MESSAGE_TYPE                       ((122<<8) | WE_MODID_MSA)
/* "Unable to show attachment!" */
#define MSA_STR_ID_ATTACH_ERROR                             ((123<<8) | WE_MODID_MSA)
/* "Maximum number of pages reached!" */
#define MSA_STR_ID_SLIDE_LIMIT_REACHED                      ((124<<8) | WE_MODID_MSA)
/* "The configuration is not valid. Please check \"From\" address and the MMSC version number." */
#define MSA_STR_ID_ME_INVALID_CONFIG                        ((125<<8) | WE_MODID_MSA)
/* "Invalid input value!" */
#define MSA_STR_ID_INVALID_VALUE                            ((126<<8) | WE_MODID_MSA)
/* "An error occured when trying to read the image to attach!" */
#define MSA_STR_ID_CRR_FAILED_TO_READ                       ((127<<8) | WE_MODID_MSA)
/* "Failed to edit the message!" */
#define MSA_STR_ID_FAILED_TO_EDIT                           ((128<<8) | WE_MODID_MSA)
/* "At least one address field must be filled in." */
#define MSA_STR_ID_ADDRESS_FIELD_MISSING                    ((129<<8) | WE_MODID_MSA)
/* "The media object could not be added to the message!" */
#define MSA_STR_ID_FAILED_TO_READ_MO                        ((130<<8) | WE_MODID_MSA)
/* "Erroneous notification!" */
#define MSA_STR_ID_FAILED_TO_DISPLAY_NOTIF                  ((131<<8) | WE_MODID_MSA)
/* "Do you want to download the message with size " */
#define MSA_STR_ID_DOWNLOAD_REQ1                            ((132<<8) | WE_MODID_MSA)
/* " bytes?" */
#define MSA_STR_ID_DOWNLOAD_REQ2                            ((133<<8) | WE_MODID_MSA)
/* "The read report could not be sent!" */
#define MSA_STR_ID_FAILED_TO_SEND_RR                        ((134<<8) | WE_MODID_MSA)
/* "Invalid value, please enter a value between " */
#define MSA_STR_ID_INTERVAL_PREFIX                          ((135<<8) | WE_MODID_MSA)
/* " and " */
#define MSA_STR_ID_INTERVAL_INFIX                           ((136<<8) | WE_MODID_MSA)
/* "A too large message was blocked" */
#define MSA_STR_ID_RETR_SIZE_EXCEEDED                       ((137<<8) | WE_MODID_MSA)
/* "The message content is invalid!" */
#define MSA_STR_ID_PLY_SMIL_FAILED                          ((138<<8) | WE_MODID_MSA)
/* "Message creation failed. System resources exhausted!" */
#define MSA_STR_ID_EDIT_FAILED_RESOURCES_EXHAUSTED          ((139<<8) | WE_MODID_MSA)
/* "PERSONAL messages" */
#define MSA_STR_ID_CONFIG_PERS                              ((140<<8) | WE_MODID_MSA)
/* "ADVERTISEMENT messages" */
#define MSA_STR_ID_CONFIG_ADVERT                            ((141<<8) | WE_MODID_MSA)
/* "INFORMATIONAL messages" */
#define MSA_STR_ID_CONFIG_INFO                              ((142<<8) | WE_MODID_MSA)
/* "AUTO messages" */
#define MSA_STR_ID_CONFIG_AUTO                              ((143<<8) | WE_MODID_MSA)
/* "STRING messages" */
#define MSA_STR_ID_CONFIG_USER_STRING                       ((144<<8) | WE_MODID_MSA)
/* "Phone book" */
#define MSA_STR_ID_PHONE_BOOK                               ((145<<8) | WE_MODID_MSA)
/* "Too many recipients." */
#define MSA_STR_ID_TO_MANY_RECIPIENTS                       ((146<<8) | WE_MODID_MSA)
/* "Invalid recipient address!" */
#define MSA_STR_ID_INVALID_RECIP_ADDRESS                    ((147<<8) | WE_MODID_MSA)
/* "Recipients:" */
#define MSA_STR_ID_RECIPIENTS                               ((148<<8) | WE_MODID_MSA)
/* "Add recipient?" */
#define MSA_STR_ID_ADD_RECIPIENT_QUEST                      ((149<<8) | WE_MODID_MSA)
/* " recipients." */
#define MSA_STR_ID_NO_OF_RECIPIENTS                         ((150<<8) | WE_MODID_MSA)
/* "The message contains locked information and connot be edited!" */
#define MSA_STR_ID_FAILED_FORWARD_LOCK                      ((151<<8) | WE_MODID_MSA)
/* "Max retrieval size" */
#define MSA_STR_ID_MAX_RETR_SIZE                            ((152<<8) | WE_MODID_MSA)
/* "RE: " */
#define MSA_STR_ID_RE                                       ((153<<8) | WE_MODID_MSA)
/* "FW: " */
#define MSA_STR_ID_FW                                       ((154<<8) | WE_MODID_MSA)
/* "Maximum message size reached" */
#define MSA_STR_ID_MESSAGE_SIZE_MAXIMUM_REACHED             ((155<<8) | WE_MODID_MSA)
/* "Recipients may not be able to view messages this large" */
#define MSA_STR_ID_MESSAGE_SIZE_WARNING                     ((156<<8) | WE_MODID_MSA)
/* "Reply charging" */
#define MSA_STR_ID_REPLY_CHARGING                           ((157<<8) | WE_MODID_MSA)
/* "Reply charging deadline" */
#define MSA_STR_ID_REPLY_CHARGING_DEADLINE                  ((158<<8) | WE_MODID_MSA)
/* "Reply charging size" */
#define MSA_STR_ID_REPLY_CHARGING_SIZE                      ((159<<8) | WE_MODID_MSA)
/* "Text only" */
#define MSA_STR_ID_TEXT_ONLY                                ((160<<8) | WE_MODID_MSA)
/* "1 KB" */
#define MSA_STR_ID_CONFIG_1_KB                              ((161<<8) | WE_MODID_MSA)
/* "10 KB" */
#define MSA_STR_ID_CONFIG_10_KB                             ((162<<8) | WE_MODID_MSA)
/* "Select color" */
#define MSA_STR_ID_SELECT_COLOR                             ((163<<8) | WE_MODID_MSA)
/* "Background color" */
#define MSA_STR_ID_BG_COLOR                                 ((164<<8) | WE_MODID_MSA)
/* "Home network" */
#define MSA_STR_ID_CONFIG_MSG_RETR_HOME_MODE                ((165<<8) | WE_MODID_MSA)
/* "Roaming network" */
#define MSA_STR_ID_CONFIG_MSG_RETR_ROAM_MODE                ((166<<8) | WE_MODID_MSA)
/* "Automatic" */
#define MSA_STR_ID_AUTO_RETRIEVAL_ROAM                      ((167<<8) | WE_MODID_MSA)
/* "Always ask" */
#define MSA_STR_ID_ASK_RETRIEVAL_ROAM                       ((168<<8) | WE_MODID_MSA)
/* "Restricted" */
#define MSA_STR_ID_RES_RETRIEVAL_ROAM                       ((169<<8) | WE_MODID_MSA)
/* "Show number" */
#define MSA_STR_ID_CONFIG_SEND_VISIBILITY                   ((170<<8) | WE_MODID_MSA)
/* "Yes" */
#define MSA_STR_ID_SHOW_NUMBER                              ((171<<8) | WE_MODID_MSA)
/* "No" */
#define MSA_STR_ID_HIDE_NUMBER                              ((172<<8) | WE_MODID_MSA)
/* "Send read report" */
#define MSA_STR_ID_CONFIG_SEND_READ_REPORT                  ((173<<8) | WE_MODID_MSA)
/* "Always" */
#define MSA_STR_ID_CONFIG_ALWAYS                            ((174<<8) | WE_MODID_MSA)
/* "On request" */
#define MSA_STR_ID_CONFIG_ON_REQUEST                        ((175<<8) | WE_MODID_MSA)
/* "Never" */
#define MSA_STR_ID_CONFIG_NEVER                             ((176<<8) | WE_MODID_MSA)
/* "Delivery time" */
#define MSA_STR_ID_CONFIG_DELIVERY_TIME                     ((177<<8) | WE_MODID_MSA)
/* "Immediate" */
#define MSA_STR_ID_CONFIG_IMMEDIATE                         ((178<<8) | WE_MODID_MSA)
/* "In one hour" */
#define MSA_STR_ID_CONFIG_IN_ONE_HOUR                       ((179<<8) | WE_MODID_MSA)
/* "In 12 hours" */
#define MSA_STR_ID_CONFIG_IN_12_HOURS                       ((180<<8) | WE_MODID_MSA)
/* "In 24 hours" */
#define MSA_STR_ID_CONFIG_IN_24_HOURS                       ((181<<8) | WE_MODID_MSA)
/* "Play sound" */
#define MSA_STR_ID_PLAY_SOUND                               ((182<<8) | WE_MODID_MSA)
/* "Delivery report allowed" */
#define MSA_STR_ID_CONFIG_DELIVERY_REPORT_ALLOWED           ((183<<8) | WE_MODID_MSA)
/* "Add audio" */
#define MSA_STR_ID_ADD_AUDIO                                ((184<<8) | WE_MODID_MSA)
/* "Text color" */
#define MSA_STR_ID_TEXT_COLOR                               ((185<<8) | WE_MODID_MSA)
/* "Text color All" */
#define MSA_STR_ID_TEXT_COLOR_ALL                           ((186<<8) | WE_MODID_MSA)
/* "Bcc: " */
#define MSA_STR_ID_BCC                                      ((187<<8) | WE_MODID_MSA)
/* "The text can not be displayed" */
#define MSA_STR_ID_TEXT_MISSING                             ((188<<8) | WE_MODID_MSA)
/* "There are media objects in this message that might be too large for some recipients." */
#define MSA_STR_ID_MULTIPLE_OBJECTS_SIZE_WARNING            ((189<<8) | WE_MODID_MSA)
/* "Recipients may not be able to view media objects of this size" */
#define MSA_STR_ID_SINGLE_OBJECT_SIZE_WARNING               ((190<<8) | WE_MODID_MSA)
/* "You are currently not in your home network and there might be additional charges on transactions. Do you want to download the message with size " */
#define MSA_STR_ID_ROAM_DOWNLOAD_REQ1                       ((191<<8) | WE_MODID_MSA)
/* " bytes?" */
#define MSA_STR_ID_ROAM_DOWNLOAD_REQ2                       ((192<<8) | WE_MODID_MSA)
/* "You are currently not in your home network and there might be additional charges on transactions. Do you want to send the message with size " */
#define MSA_STR_ID_ROAM_SEND_REQ1                           ((193<<8) | WE_MODID_MSA)
/* " bytes?" */
#define MSA_STR_ID_ROAM_SEND_REQ2                           ((194<<8) | WE_MODID_MSA)
/* "Your network settings are invalid. Please check MMSC and WAP settings." */
#define MSA_STR_ID_INVALID_NETWORK_SETTINGS                 ((195<<8) | WE_MODID_MSA)
/* "The called party is busy" */
#define MSA_STR_ID_CALL_BUSY                                ((196<<8) | WE_MODID_MSA)
/* "The network is not available" */
#define MSA_STR_ID_CALL_NO_NETWORK                          ((197<<8) | WE_MODID_MSA)
/* "The called party did not answer" */
#define MSA_STR_ID_CALL_NO_ANSWER                           ((198<<8) | WE_MODID_MSA)
/* "The number is not a phone-number" */
#define MSA_STR_ID_CALL_NOT_A_NUMBER                        ((199<<8) | WE_MODID_MSA)
/* "Could not place call" */
#define MSA_STR_ID_CALL_ERROR                               ((200<<8) | WE_MODID_MSA)
/* "Failed to open original message" */
#define MSA_STR_ID_FAILED_TO_GET_MESSAGE_INFO               ((201<<8) | WE_MODID_MSA)
/* "Failed to open window" */
#define MSA_STR_ID_FAILED_TO_OPEN_WINDOW                    ((202<<8) | WE_MODID_MSA)
/* "Start time: " */
#define MSA_STR_ID_OBJ_START                                ((203<<8) | WE_MODID_MSA)
/* "Duration: " */
#define MSA_STR_ID_OBJ_DURATION                             ((204<<8) | WE_MODID_MSA)
/* "Item timing" */
#define MSA_STR_ID_ITEM_TIMING                              ((205<<8) | WE_MODID_MSA)
/* " seconds" */
#define MSA_STR_ID_SECONDS                                  ((206<<8) | WE_MODID_MSA)
/* "Start time" */
#define MSA_STR_ID_START_TIME                               ((207<<8) | WE_MODID_MSA)
/* "maximum" */
#define MSA_STR_ID_MAXIMUM                                  ((208<<8) | WE_MODID_MSA)
/* "The item timing is changed in order to be within page duration bounds!" */
#define MSA_STR_ID_TIMING_OUT_OF_BOUNDS                     ((209<<8) | WE_MODID_MSA)
/* "page:" */
#define MSA_STR_ID_PAGE_COLON                               ((210<<8) | WE_MODID_MSA)
/* "s" */
#define MSA_STR_ID_SHORT_SECOND                             ((211<<8) | WE_MODID_MSA)
/* "Item duration" */
#define MSA_STR_ID_OBJ_OB_DURATION                          ((212<<8) | WE_MODID_MSA)
/* "The message was not accepted by the server due to size, media type, copyrights or some other reason." */
#define MSA_STR_ID_FAILED_TO_SEND_REFUSED                   ((213<<8) | WE_MODID_MSA)
/* "Unable to connect to server!" */
#define MSA_STR_ID_SERVER_ERROR                             ((214<<8) | WE_MODID_MSA)
/* "Connection timeout!" */
#define MSA_STR_ID_COMM_TIMEOUT                             ((215<<8) | WE_MODID_MSA)
/* "Invalid MMS version!" */
#define MSA_STR_ID_INVALID_MMS_VERSION                      ((216<<8) | WE_MODID_MSA)
/* "DRM not supported" */
#define MSA_STR_ID_NO_DRM_SUPPORT                           ((217<<8) | WE_MODID_MSA)
/* "No Content exist in mms file !" */
#define MSA_STR_ID_MMS_EMPTY                                ((218<<8) | WE_MODID_MSA)
/* "Save template" */
#define MSA_STR_ID_SAVE_TEMPLATE                            ((219<<8) | WE_MODID_MSA)
/* "Signature" */
#define MSA_STR_ID_CONFIG_SIGNATURE                         ((220<<8) | WE_MODID_MSA)
/* "Use signature" */
#define MSA_STR_ID_USE_SIGNATURE                            ((221<<8) | WE_MODID_MSA)
/* "Edit signature" */
#define MSA_STR_ID_EDIT_SIGNATURE                           ((222<<8) | WE_MODID_MSA)




/*Predefined images*/
#define MSA_NUMBER_OF_IMAGES 2

extern const unsigned char * const msa_images[MSA_NUMBER_OF_IMAGES + 1];

/*!< A SMIL presentation with one region/slide */
#define MSA_IMG_BROKEN_IMAGE   ((  1<<8) | WE_MODID_MSA)
/*!< An indication that audio valid */
#define MSA_IMG_AUDIO          ((  2<<8) | WE_MODID_MSA)

#endif
