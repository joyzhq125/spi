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

/*! \file Msa_Cfg.h
 *  Compile-time configurations for the MSA.
 */

#ifndef MSA_CFG_H
#define MSA_CFG_H

#include "Msa_Def.h"

/******************************************************************************
 * Defines 
 *****************************************************************************/

/*! The MSA can be invoked from the WBA or WBS when a mmsto:, mms: or mailto:
 *  scheme is found.
 *
 *  Use this define in order to enable cross triggering from the browser.
 */
#undef MSA_CFG_CT_BROWSER_SCHEMES /*#define MSA_CFG_CT_BROWSER_SCHEMES   "mms,mmsto,mailto"*/

/*! In order to use an external phone book this constant needs to be defined.
 */
#define MSA_CFG_USE_EXT_PB

/*! The MSA has support for several media types for audio, text and image
 * when creating (and edit) a multimedia message. Add or remove support by add
 * or remove items in the list, see #MsaMediaType for supported types.
 *
 * format:
 *
 * {#MsaMediaGroupType, mime-type, #SisMediaType, file extension}
 */ 
#define MSA_CFG_MEDIA_OBJECTS   {\
    {MSA_MEDIA_GROUP_IMAGE, "image/jpeg",         SIS_IMAGE_JPG, "jpg"},\
    {MSA_MEDIA_GROUP_IMAGE, "image/vnd.wap.wbmp", SIS_IMAGE_WBMP, "wbmp"},\
/*    {MSA_MEDIA_GROUP_IMAGE, "image/gif",          SIS_IMAGE_GIF, "gif"},*/\
/*    {MSA_MEDIA_GROUP_IMAGE, "image/png",          SIS_IMAGE_PNG, "png"},*/\
    {MSA_MEDIA_GROUP_IMAGE, "image/bmp",          SIS_IMAGE_BMP, "bmp"},\
    {MSA_MEDIA_GROUP_IMAGE, "image/bitmap",       SIS_IMAGE_BMP, "bmp"},\
    \
    /* Note, The default text-type must be first in the list of valid text-types */ \
    {MSA_MEDIA_GROUP_TEXT,  "text/plain",         SIS_TEXT_PLAIN, "txt"},\
    {MSA_MEDIA_GROUP_TEXT,  "application/text",   SIS_TEXT_PLAIN, "txt"},\
    {MSA_MEDIA_GROUP_TEXT,  "application/x-techfaith-bookmark", SIS_TEXT_PLAIN, "txt"},\
    \
/*    {MSA_MEDIA_GROUP_AUDIO, "audio/amr",          SIS_AUDIO_AMR, "amr"},*/\
    {MSA_MEDIA_GROUP_AUDIO, "audio/wav",          SIS_AUDIO_WAV, "wav"},\
    {MSA_MEDIA_GROUP_AUDIO, "audio/x-wav",        SIS_AUDIO_WAV, "wav"},\
    {MSA_MEDIA_GROUP_AUDIO, "audio/midi",         SIS_AUDIO_MIDI, "midi"},\
/*    {MSA_MEDIA_GROUP_AUDIO, "audio/mp3",          SIS_AUDIO_MP3, "mp3"},*/\
    \
    }

/*! This is the filter for opening media objects. MIME-types presented in this list
 *  is the ones that can be included in a message. Note that the 
 *  #MSA_CFG_MEDIA_OBJECTS needs to contain the same values in order to be able
 *  to actually add the object to the message.
 */
#define MSA_CFG_OPEN_IMAGE_TYPES "image/jpeg,image/vnd.wap.wbmp,image/bmp,image/bitmap"
#define MSA_CFG_OPEN_TEXT_TYPES  "text/plain,application/text"
#define MSA_CFG_OPEN_AUDIO_TYPES "audio/wav,audio/x-wav,audio/midi"

/*! The MSA uses UTF-8 for all internal strings. When editing messages created
 *  by other software a different character set may be used. Therefore the MSA
 *  must perform a conversion to UTF-8. The following definitions sets the 
 *  defines which character sets that are supported and how to convert between
 *  MMS and WE definitions.
 *	
 * format:
 *
 *    {Mms character set #MmsCharset, WE character set}
 */
#define MSA_CFG_SUPPORTED_CHARSETS  {{MMS_US_ASCII, WE_CHARSET_ASCII},\
    {MMS_UTF8, WE_CHARSET_UTF_8}, {MMS_UTF16BE, WE_CHARSET_UTF_16BE},\
    {MMS_UTF16LE, WE_CHARSET_UTF_16LE}, {MMS_UTF16, WE_CHARSET_UTF_16},\
    {MMS_ISO_8859_1, WE_CHARSET_ISO_8859_1}, {MMS_UCS2, WE_CHARSET_UCS_2},\
    }

/*! Exclude operations from object-actions. When viewing a multi-part mixed
 *  message and the user chooses to perform an action on the object, it is
 *  possible to block some of the possible operations. 
 *
 * Format: {"action 1", "action 2", "action n"}
 *
 * For information about which operations that are possible see, we_cfg.h and
 * we_act.h.
 */
#define MSA_CFG_EXCLUDE_ACTIONS     {"-mode new -i"}

/*! This defines if NULL termination to text/plain body-part is skipped/used.
 *  - A value of 0 => No NULL in body text (default)
 *  - A value of 1 => Use NULL in body text
 */
#define MSA_CFG_USE_BODY_TEXT_NULL  0

/*! The maximum size of a text to include into a SMIL presentation.
 *  Texts larger than the stated size is going to be rejected with
 *  an alert to the End User. Setting this variable to a value larger than 
 *  MSA_CFG_MESSAGE_SIZE_MAXIMUM or MSA_CFG_INSERT_MEDIA_OBJECT_MAX_SIZE 
 *  renders it useless. 
 */      
#define MSA_CFG_INSERT_TEXT_MAX_SIZE            29900  /* 30k */
 
/*! The maximum size of an image to include into a SMIL presentation.
 *  Images larger than the stated size is going to be rejected with
 *  an alert to the End User. Setting this variable to a value larger than 
 *  MSA_CFG_MESSAGE_SIZE_MAXIMUM or MSA_CFG_INSERT_MEDIA_OBJECT_MAX_SIZE 
 *  renders it useless. 
 */      
#define MSA_CFG_INSERT_IMAGE_MAX_SIZE           29900  /* 30k */

/*! The maximum size of a media object to include into a SMIL presentation.
 *  Media objects larger than the stated size is going to be rejected with
 *  an alert to the End User. Setting this variable to a value larger than 
 *  MSA_CFG_MESSAGE_SIZE_MAXIMUM renders it useless. 
 */
#define MSA_CFG_INSERT_MEDIA_OBJECT_MAX_SIZE    95000 /* 95k */

/*! The maximum size in total of an MM. It is not possible to create or view  
 *  larger messages than this value. Upon viewing, a warning is issued. 
 */
#define MSA_CFG_MESSAGE_SIZE_MAXIMUM            99000 /* 100k */

/*! The size of an MM that will generate a warning if surpassed that informs
 *	the user of the fact that this message may not be viewable by others.
 *  (i.e. it won't comply with MMS conformance specification)
 *  Setting this value above MSA_CFG_MESSAGE_SIZE_MAXIMUM and 
 *  MSA_CFG_INSERT_MEDIA_OBJECT_MAX_SIZE renders it useless.
 */
#define MSA_CFG_MESSAGE_SIZE_WARNING            39000 /* 39k */

/*! When setting any port number a verifications is performed against the
 *  limits defined below.
 */
#define MSA_CFG_MIN_PORT                        0
#define MSA_CFG_MAX_PORT                        65535

/*! When creating a SMIL presentation it is possible to limit the number of
 *  slides valid for one presentation.
 */
#define MSA_CFG_SLIDE_LIMIT                     200 /* Ramped up high enough to
                                                       be of no consequence */

/*! When multiple MMS addresses wid displayed to the End User or if the End User
 * wants to input multiple addresses the following separator is used.
 */
#define MSA_CFG_MMS_ADDRESS_SEPARATOR        ", "

/*! When displaying the To or Cc field of a received message and the adderss 
 * type is email and the "name" is provided, the MSA_EMAIL_BEGIN_STR and
 * MSA_EMAIL_END_STR separates the email address from the name, e.g.,
 * homer@simpson.com <Homer Simpson>
 */
#define MSA_CFG_EMAIL_BEGIN_STR                 " <"
#define MSA_CFG_EMAIL_END_STR                   ">"

/*! The maximum number of digits for duration 
 */
#define MSA_CFG_DURATION_SIZE                   2

/*! The maximum and minimum value for slide duration in seconds.
 *
 */
#define MSA_CFG_MAX_DURATION                    60
#define MSA_CFG_MIN_DURATION                    1

/*! The default duration for each slide, in milliseconds 
 */
#define MSA_CFG_DEFAULT_DURATION                5000

/*! The default setting for read report 0 is off and 1 is on.
 */
#define MSA_CFG_READ_REPORT                     0

/*! The default setting for delivery report 0 is off and 1 is on.
 */
#define MSA_CFG_DELIVERY_REPORT                 0

/*! The default setting for expiry time type. Relative time will be used default..
 */
#define MSA_CONF_EXPIRY_TIME_RELATIVE           TRUE

/*! The default setting for expiry time, see #MsaExpiryTimeType for valid values.
 */
#define MSA_CFG_EXPIRY_TIME                     MSA_EXPIRY_MAX

/*! The default setting for priority, see #MsaPriorityType for valid values.
 */
#define MSA_CFG_PRIORITY                        MSA_PRIORITY_NORMAL

/*! The default setting for signature
 */
#define MSA_CONFIG_SIGNATURE                  0

/*! The default setting for reply charging, see #MsaReplyChargingType for valid
 *  values
 */
#define MSA_CFG_REPLY_CHARGING                  MSA_REPLY_CHARGING_OFF

/*! The default setting for reply charging deadline, see
 *  #MsaReplyChargingDeadlineType for valid values
 */
#define MSA_CFG_REPLY_CHARGING_DEADLINE         MSA_REPLY_CHARGING_DEADLINE_1_DAY

/*! The default setting for reply charging size, see #MsaReplyChargingSizeType 
 *  for valid values
 */
#define MSA_CFG_REPLY_CHARGING_SIZE             MSA_REPLY_CHARGING_SIZE_1_KB

/*! The default setting for "save on send", 0 for removing a message after send
 *  1 for saving the message in the "sent" folder after send.
 */
#define MSA_CFG_SAVE_ON_SEND                    0

/*! The default setting for allowing PERSONAL messages. 0=no 1=yes */
#define MSA_CFG_ALLOW_PERSONAL                  1

/*! The default setting for allowing ADVERTISMENT messages. 0=no 1=yes */
#define MSA_CFG_ALLOW_ADVERT                    1

/*! The default setting for allowing INFORMATIONAL messages. 0=no 1=yes */
#define MSA_CFG_ALLOW_INFO                      1

/*! The default setting for allowing AUTO messages. 0=no 1=yes */
#define MSA_CFG_ALLOW_AUTO                      1

/*! The default setting for allowing anonymous senders. 0=no 1=yes */
#define MSA_CFG_ALLOW_ANONYM                    1

/*! The default setting for the treat as delayed flag. 0=no 1=yes */
#define MSA_CFG_TREAT_AS_DELAYED                1

/*! The default setting for allowing user defined string in message class. 
 *  ""=not used */
#define MSA_CFG_ALLOW_STRING                    ""

/*! The default setting for allowing immediate retrieval from only one server
 *  ""=not used */
#define MSA_CFG_IM_SERVER                       ""

/*! The default setting for MSISDN if nothing is implemented in /EXTERNAL */
#define MSA_CFG_MSISDN                          "+46701234567"

/*! The maximum number of digits to set for the immediate retrieval size */
#define MSA_CFG_MAX_IM_RETR_SIZE_DIGITS         7

/*! Minimum possible value of message size for immediate retrieval */
#define MSA_CFG_MIN_MSG_SIZE                    500

/*! Maximum possible value of message size for immediate retrieval */
#define MSA_CFG_MAX_MSG_SIZE                    4000000

/*! The maximum number of digits to set for the message retrieval size */
#define MSA_CFG_MAX_RETRIEVAL_SIZE_DIGITS       7

/*! Minimum possible value of retrieval size of a message. 0 is the largest 
    possible */
#define MSA_CFG_MIN_RETR_SIZE                   0

/*! Maximum possible value of retrieval size of a message */
#define MSA_CFG_MAX_RETR_SIZE                   5000000

/*! MMS version to use against MMSC. 0x10=MMS_VERSION_10 0x11=MMS_VERSION_11 */
#define MSA_CFG_MMSC_VERSION                    0x11

/*! The maximum number of addresses to add to a message during creation */
#define MSA_CFG_MAX_RECIPIENT_ADDRESSES         20

/*! The default background color for SMIL presentations. The color is RGB and
    number of bits per channel are 8. Example 0x00FF0000 for red color  */
#define MSA_CFG_DEFAULT_SMIL_BG_COLOR           0x00FFFFFF

/*! The default text color for SMIL presentations. The color is RGB and
 *  number of bits per channel are 8. Example 0x0000FF00 for green color
 *  Warning: Value other than SIS_COLOR_NOT_USED will create mutlipart/related
 *  PDUs with SMIL param element within text elements, which violates the 
 *  MMS SMIL Standard [OMA-MMS-CONF, 8.1.2.2]. */
#define MSA_CFG_DEFAULT_SMIL_TEXT_COLOR         SIS_COLOR_NOT_USED

/*! \brief Possible background colors for SMIL presentations
 *  
 * Format of the colors are: RGB 8-bits for each channel, i.e, 0xRRGGBB
 */
#define MSA_CFG_SMIL_BG_COLORS  {0x000000, \
                                 0x404040, 0x808080, 0xC0C0C0, 0xFFFFFF,\
                                 0x400000, 0x800000, 0xC00000, 0xFF0000,\
                                 0x004000, 0x008000, 0x00C000, 0x00FF00,\
                                 0x000040, 0x000080, 0x0000C0, 0x0000FF}

/*! \brief Possible text colors for SMIL presentations
 *  
 * Format of the colors are: RGB 8-bits for each channel, i.e, 0xRRGGBB
 */
#define MSA_CFG_SMIL_TEXT_COLORS   {0x000000, \
                                    0x404040, 0x808080, 0xC0C0C0, 0xFFFFFF,\
                                    0x400000, 0x800000, 0xC00000, 0xFF0000,\
                                    0x004000, 0x008000, 0x00C000, 0x00FF00,\
                                    0x000040, 0x000080, 0x0000C0, 0x0000FF}

/*! The default setting for Show number. For possible values see 
 * #MsaVisibilityType 
 */
#define MSA_CFG_SHOW_NUMBER                       MSA_SENDER_SHOW

/*! The default setting for Delivery report allowed. 0=Yes 1=No*/
#define MSA_CFG_DELIVERY_REPORT_ALLOWED           0

/*! The default setting for Send read report. 0=On request 1=Always 2=Never*/
#define MSA_CFG_SEND_READ_REPORT                  0

/*! The default setting for Delivery time. 0=Immediate 1=1h 2=12h 3=24h */
#define MSA_CFG_DELIVERY_TIME                     0

/*! The assumed default roaming status. 0 = not roaming, 1 = roaming.*/
#define MSA_CFG_DEFAULT_ROAMING_STATUS            0

/* The assumed default bearer status. 0 = no bearer, 1 = bearer. */
#define MSA_CFG_DEFAULT_BEARER_STATUS             1

/* Default retrieval setting for home network */
#define MSA_CFG_RETRIEVAL_MODE  MSA_RETRIEVAL_MODE_AUTOMATIC

/* Default retrieval setting for roaming network */
#define MSA_CFG_RETRIEVAL_MODE_ROAMING \
    MSA_RETRIEVAL_MODE_ROAMING_AUTOMATIC

/* MSA text alignment CR 17749 0 = left, 1 = center, 2 = right*/
#define MSA_CFG_TEXT_ALIGNMENT MSA_TEXT_ALIGNMENT_CENTER

/* Maximum number of characters for the text in one region/slide */
#define MSA_SE_TEXT_MAX_CHARACTERS  512

/* Treat messages that SIA fails to play as Multipart/Mixed */

/*#define MSA_BAD_SMIL_AS_MULTIPART_MIXED*/

#endif
