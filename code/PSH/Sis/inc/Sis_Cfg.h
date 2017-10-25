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

/*! \file Sis_cfg.h
 * \brief  configuration variables for SIS
 */
#ifndef SIS_CFG_H
#define SIS_CFG_H


/*--- Include files ---*/

/*--- Definitions/Declarations ---*/
/*! Initial size for parser buffer, NOT maximum. */
#define SIA_PARSE_BUFFER_INITIAL_SIZE   100

/*! Maximum number of slides allowed in a SMIL. */
#define MAX_NO_OF_SLIDES 100
#define MAX_ELEM_DEPTH   10

/*! Maximum length of text in <alt> attribute */
#define SIS_MAX_ALT_ATTR_LEN 40

/*! Maximum length of text in <fit> attribute */
#define SIS_SMIL_ATTR_FIT_MAX_LEN 10


/*! Define below if SIS shall be matching body-part through
    parameters. */
#define SIS_FIND_BODY_PART_BY_CT

/*! \enum SisMediaType
 *  \brief Defines the media type
 *
 * NOTE: Making changes here will probably require changes in mea_cfg.h and Sia_cfg.h.
 */
typedef enum 
{
    SIS_TEXT_PLAIN = 0x01,      /*!< Plain text */ 
    SIS_IMAGE_WBMP,             /*!< Wireless bitmap image */ 
    SIS_IMAGE_JPG,              /*!< JPG image */ 
    SIS_IMAGE_GIF,              /*!< GIF image */ 
/*    SIS_IMAGE_PNG,        */      /*!< PNG image */ 
    SIS_IMAGE_BMP,              /*!< BMP image */
/*    SIS_IMAGE_TIFF,       */      /*!< TIFF image */
    SIS_DRM_MESSAGE,            /*!< application/vnd.oma.drm.message */ 
	SIS_TEXT_X_VCARD,			/*!< V-Card */ 
	SIS_TEXT_X_VCAL,			/*!< V-Cal */ 
	SIS_VIDEO_3GP,              /*!< 3GP video */
	SIS_VIDEO_MP4,              /*!< MPEG4 video */
	SIS_VIDEO_H263,             /*!< H263 video */
/*    SIS_AUDIO_MP3,        */      /*!< MP3 audio */ 
    SIS_AUDIO_AMR,              /*!< AMR audio */ 
    SIS_AUDIO_WAV,              /*!< WAV audio */
    SIS_AUDIO_MIDI,             /*!< Midi audio */
    SIS_UNKNOWN,                /*!< Unknown media type */
    SIS_NOT_USED                /*!< Media is not used */ 
} SisMediaType;

/*! Supported media objects for the SIS module.
 *
 * Add or remove lines in the "table" below in order to configure the support
 * for different media types, the values in #SisMediaType does also needs to be
 * changed accordingly. The #SisMediaType enum is located in the source code 
 * file: Sis_if.h
 *
 * NOTE: Making changes here will probably require changes in mea_cfg.h and Sia_cfg.h.
 *
 * Format:
 *
 *  {#SisMediaType, MIME type, #MmsKnownMediaType, #SlsObjectType}
 *
 */
#define SIS_CFG_SUPPORTED_MEDIA_TYPES {\
    {SIS_TEXT_PLAIN,	"text/plain",       MMS_TYPE_TEXT_PLAIN,         SIS_OBJECT_TYPE_TEXT}, \
    \
    {SIS_IMAGE_WBMP,	"image/vnd.wap.wbmp", MMS_TYPE_IMAGE_VND_WAP_WBMP, SIS_OBJECT_TYPE_IMAGE},\
    {SIS_IMAGE_WBMP,	"image/vnd.wap.wbmp", MMS_VALUE_AS_STRING,       SIS_OBJECT_TYPE_IMAGE},\
    {SIS_IMAGE_JPG,		"image/jpeg",       MMS_TYPE_IMAGE_JPEG,         SIS_OBJECT_TYPE_IMAGE},\
    {SIS_IMAGE_JPG,		"image/jpeg",       MMS_VALUE_AS_STRING,         SIS_OBJECT_TYPE_IMAGE},\
/*    {SIS_IMAGE_GIF,     "image/gif",        MMS_TYPE_IMAGE_GIF,          SIS_OBJECT_TYPE_IMAGE},*/\
/*    {SIS_IMAGE_GIF,     "image/gif",        MMS_VALUE_AS_STRING,         SIS_OBJECT_TYPE_IMAGE},*/\
    {SIS_IMAGE_BMP,     "image/bmp",        MMS_VALUE_AS_STRING,         SIS_OBJECT_TYPE_IMAGE},\
    {SIS_IMAGE_BMP,		"image/bitmap",     MMS_VALUE_AS_STRING,         SIS_OBJECT_TYPE_IMAGE},\
/*    {SIS_IMAGE_PNG,     "image/png",        MMS_TYPE_IMAGE_PNG,          SIS_OBJECT_TYPE_IMAGE},*/\
/*    {SIS_IMAGE_PNG,		"image/png",        MMS_VALUE_AS_STRING,         SIS_OBJECT_TYPE_IMAGE},*/\
/*    {SIS_IMAGE_TIFF,    "image/tiff",       MMS_TYPE_IMAGE_TIFF,         SIS_OBJECT_TYPE_ATTACMENT},*/\
/*    {SIS_IMAGE_TIFF,	"image/tiff",       MMS_VALUE_AS_STRING,         SIS_OBJECT_TYPE_ATTACMENT},*/\
    \
	{SIS_VIDEO_MP4,		"video/mpeg",       MMS_VALUE_AS_STRING,         SIS_OBJECT_TYPE_VIDEO},\
	{SIS_VIDEO_MP4,		"video/mp4",        MMS_VALUE_AS_STRING,         SIS_OBJECT_TYPE_VIDEO},\
	{SIS_VIDEO_3GP,		"video/3gpp",       MMS_VALUE_AS_STRING,         SIS_OBJECT_TYPE_VIDEO},\
	{SIS_VIDEO_H263,	"video/h263-2000",  MMS_VALUE_AS_STRING,         SIS_OBJECT_TYPE_VIDEO},\
    \
    {SIS_AUDIO_AMR,		"audio/amr",        MMS_VALUE_AS_STRING,         SIS_OBJECT_TYPE_AUDIO},\
/*    {SIS_AUDIO_MP3,		"audio/mp3",        MMS_VALUE_AS_STRING,         SIS_OBJECT_TYPE_AUDIO},*/\
    {SIS_AUDIO_WAV,		"audio/wav",        MMS_VALUE_AS_STRING,         SIS_OBJECT_TYPE_AUDIO},\
    {SIS_AUDIO_WAV,		"audio/x-wav",      MMS_VALUE_AS_STRING,         SIS_OBJECT_TYPE_AUDIO},\
    {SIS_AUDIO_MIDI,    "audio/midi",       MMS_VALUE_AS_STRING,         SIS_OBJECT_TYPE_AUDIO},\
    \
    {SIS_DRM_MESSAGE,	"application/vnd.oma.drm.message", MMS_TYPE_APP_VND_OMA_DRM_MESSAGE, SIS_OBJECT_TYPE_DRM},\
	\
	{SIS_TEXT_X_VCAL,	"text/x-vcalendar",	MMS_TYPE_TEXT_X_VCALENDAR,	 SIS_OBJECT_TYPE_ATTACMENT},\
    {SIS_TEXT_X_VCARD,	"text/x-vcard",		MMS_TYPE_TEXT_X_VCARD,		 SIS_OBJECT_TYPE_ATTACMENT},\
    \
    {SIS_NOT_USED,      "",                 (MmsKnownMediaType)0,        (SlsMediaObjectType)0}}


/*! Supported character sets for the SIS. Add or remove items in order to change
 *  the supported types.
 *
 * Format
 *
 * {#MmsCharset, WE charset}
 *
 * Note that the last entry of the list must be present.
 */
#define SIS_CFG_SUPPORTED_CHARSET {             \
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


#define SIS_COLOR_SET {\
	{"white",		0xFFFFFF},\
	{"silver",		0xC0C0C0},\
	{"gray",		0x808080},\
	{"black",		0x000000},\
	{"yellow",		0xFFFF00},\
	{"fuchsia",		0xFF00FF},\
	{"red",			0xFF0000},\
	{"maroon",		0x800000},\
	{"lime",		0x00FF00},\
	{"olive",		0x808000},\
	{"green",		0x008000},\
	{"purple",		0x800080},\
	{"aqua",		0x00FFFF},\
	{"teal",		0x008080},\
	{"blue",		0x0000FF},\
	{"navy",		0x000080},\
	{NULL,			SIS_COLOR_NOT_USED}}
/*--- Types ---*/

/*--- Constants ---*/

/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

/*--- Global variables ---*/

/*--- Static variables ---*/

/*--- Prototypes ---*/
/******************************************************************************/

#endif /* SIS_CFG_H */
