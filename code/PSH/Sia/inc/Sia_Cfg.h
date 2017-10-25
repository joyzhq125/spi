/*
 * Copyright (C) Techfaith, 2002-2005.
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

/*! \file Sia_cfg.h
 * \brief  configuration variables for SMA
 */

#ifndef SIA_CFG_H
#define SIA_CFG_H

/*--- Include files ---*/

/*--- Definitions/Declarations ---*/

/* ! Maximum number of items in a context dependent menu
 */
#define SIA_MAX_CONTEXT_LIST_ITEMS      20

/*! 
 *	Default display time for an SMA dialog (in ms)
 */
#define SIA_DIALOG_DEFAULT_TIME_OUT     3000
 
/*!
 *	Display time, in 10th of a second, for action images.
 *  Minimum value is 1000, i.e. 1 second 
 */
#define SIA_ACTION_IMAGE_TIME 2000


/*!
 *  Minimum duration in milliseconds for a slide 
 */
#define SIA_MIN_SLIDE_DURATION 5000

/*!
 *  Use this define in order to support cross-triggering, where the SMA starts
 *  the BRA with a specific URL. The default schemes supported are http:// and
 *  https://. To support more schemes or for example; "www.", then add a 
 *  additional entry to the list:
 *
 *    {"www.", sizeof("www.") - 1},\
 *
 */
#define SIA_CONFIG_CROSS_TRIGGERING_SCHEMES  \
    {{"http://", sizeof("http://") - 1},\
    {"https://", sizeof("https://") - 1},\
    {NULL, 0}}


/*! Supported media types for the SMA.
 * Add or remove items in the list in order to provide support for specific
 * formats. Please note that the SIS module should be configured accordingly,
 * i.e., the SIS data-types used in the list must be configured in the SIS
 * module.
 *
 *  Format:
 *
 * SMA media type
 *
 * {mime type, #SlsMediaObjectType}
 */
#define SIA_CFG_SUPPORTED_MEDIA_TYPES {\
    {"text/plain", SIS_OBJECT_TYPE_TEXT}, \
    \
    {"image/vnd.wap.wbmp", SIS_OBJECT_TYPE_IMAGE}, \
    {"image/jpeg", SIS_OBJECT_TYPE_IMAGE}, \
    {"image/bmp", SIS_OBJECT_TYPE_IMAGE}, \
    {"image/bitmap", SIS_OBJECT_TYPE_IMAGE}, \
    {"image/gif", SIS_OBJECT_TYPE_IMAGE}, \
    {"image/png", SIS_OBJECT_TYPE_IMAGE}, \
    \
    {"audio/amr", SIS_OBJECT_TYPE_AUDIO}, \
    {"audio/wav", SIS_OBJECT_TYPE_AUDIO}, \
    {"audio/midi", SIS_OBJECT_TYPE_AUDIO}, \
    \
    {"video/mpeg", SIS_OBJECT_TYPE_VIDEO}, \
    {"video/3gpp", SIS_OBJECT_TYPE_VIDEO}, \
    {"video/h263-2000", SIS_OBJECT_TYPE_VIDEO}}



/*! Exclude operations from object-actions. 
 * Format: {"action 1", "action 2", "action n"}
 *
 * For information about which operations that are possible see, We_Cfg.h and
 * We_Act.h.
 */
#define SIA_CFG_EXCLUDE_ACTIONS     {"-mode new -i"}


/*! Supported attachment types for the SMA.
 * Add or remove items in the list in order to provide support for specific
 * formats. Please note that the SIS module should be configured accordingly,
 * i.e., the SIS data-types used in the list must be configured in the SIS
 * module.
 *
 *  Format:
 *
 * SMA media type
 *
 * {#SisMediaType, mime type, file extension}
 */
#define SIA_CFG_ATTACHMENT_MEDIA_TYPES {\
    {SIS_IMAGE_WBMP,    "image/vnd.wap.wbmp",   "wbm"},     \
    {SIS_IMAGE_JPG,		"image/jpeg",           "jpg"},     \
    {SIS_IMAGE_BMP,		"image/bmp",            "bmp"},     \
	                                                        \
    {SIS_AUDIO_AMR,		"audio/amr",            "amr"},     \
    {SIS_AUDIO_WAV,		"audio/wav",            "wav"},	    \
                                                            \
    {SIS_TEXT_X_VCARD,	"text/x-vcard",         "vcd"},     \
    {SIS_TEXT_X_VCARD,	"text/x-vcalendar",     "vcl"},     \
                                                            \
    {SIS_NOT_USED,		NULL,                   0}}



/*! Supported character sets for the SMA. Add or remove items in order to change
 *  the supported types.
 *
 * Format
 *
 * {#MmsCharset, WE charset}
 *
 * Note that the last entry of the list must be present.
 */
#define SIA_CFG_SUPPORTED_CHARSET {             \
    {MMS_US_ASCII,      WE_CHARSET_ASCII},     \
    {MMS_UTF8,          WE_CHARSET_UTF_8},     \
    {MMS_ISO_8859_1,    WE_CHARSET_ISO_8859_1},\
    {MMS_ISO_8859_1,    WE_CHARSET_LATIN_1},   \
    {MMS_UCS2,          WE_CHARSET_UCS_2},     \
    {MMS_UTF16BE,       WE_CHARSET_UTF_16BE},  \
    {MMS_UTF16LE,       WE_CHARSET_UTF_16LE},  \
    {MMS_UTF16,         WE_CHARSET_UTF_16},    \
                                                \
    /* The last entry must be present */        \
    {MMS_UNKNOWN_CHARSET, WE_CHARSET_UNKNOWN}}


#endif /* SIA_CFG_H */
