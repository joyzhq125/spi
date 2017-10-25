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
 * We_Mime.h
 *
 * File extension to MIME type conversion.
 *
 */


#ifndef _we_mime_h
#define _we_mime_h

#ifndef _we_def_h
#include "We_Def.h"
#endif
#ifndef _we_cmmn_h
#include "We_Cmmn.h"
#endif

/************************************************************ 
 * Constants
 ************************************************************/

/*
 * Media types that have a pre-defined integer representation.
 * OMNA "Well-Known Values".
 */
#define WE_MIME_TYPE_ANY_ANY                                    0x00
#define WE_MIME_TYPE_TEXT_ANY                                   0x01
#define WE_MIME_TYPE_TEXT_HTML                                  0x02
#define WE_MIME_TYPE_TEXT_PLAIN                                 0x03
#define WE_MIME_TYPE_TEXT_X_HDML                                0x04
#define WE_MIME_TYPE_TEXT_X_TTML                                0x05
#define WE_MIME_TYPE_TEXT_X_VCALENDAR                           0x06
#define WE_MIME_TYPE_TEXT_X_VCARD                               0x07
#define WE_MIME_TYPE_TEXT_VND_WAP_WML                           0x08
#define WE_MIME_TYPE_TEXT_VND_WAP_WMLSCRIPT                     0x09
#define WE_MIME_TYPE_TEXT_VND_WAP_CHANNEL                       0x0a
#define WE_MIME_TYPE_MULTIPART_ANY                              0x0b
#define WE_MIME_TYPE_MULTIPART_MIXED                            0x0c
#define WE_MIME_TYPE_MULTIPART_FORM_DATA                        0x0d
#define WE_MIME_TYPE_MULTIPART_BYTERANGES                       0x0e
#define WE_MIME_TYPE_MULTIPART_ALTERNATIVE                      0x0f
#define WE_MIME_TYPE_APPLICATION_ANY                            0x10
#define WE_MIME_TYPE_APPLICATION_JAVA_VM                        0x11
#define WE_MIME_TYPE_APPLICATION_X_WWW_FORM_URLENCODED          0x12
#define WE_MIME_TYPE_APPLICATION_X_HDMLC                        0x13
#define WE_MIME_TYPE_APPLICATION_VND_WAP_WMLC                   0x14
#define WE_MIME_TYPE_APPLICATION_VND_WAP_WMLSCRIPTC             0x15
#define WE_MIME_TYPE_APPLICATION_VND_WAP_CHANNELC               0x16
#define WE_MIME_TYPE_APPLICATION_VND_WAP_UAPROF                 0x17
#define WE_MIME_TYPE_APPLICATION_VND_WAP_WTLS_CA_CERTIFICATE    0x18
#define WE_MIME_TYPE_APPLICATION_VND_WAP_WTLS_USER_CERTIFICATE  0x19
#define WE_MIME_TYPE_APPLICATION_X_X509_CA_CERT                 0x1a
#define WE_MIME_TYPE_APPLICATION_X_X509_USER_CERT               0x1b
#define WE_MIME_TYPE_IMAGE_ANY                                  0x1c
#define WE_MIME_TYPE_IMAGE_GIF                                  0x1d
#define WE_MIME_TYPE_IMAGE_JPEG                                 0x1e
#define WE_MIME_TYPE_IMAGE_TIFF                                 0x1f
#define WE_MIME_TYPE_IMAGE_PNG                                  0x20
#define WE_MIME_TYPE_IMAGE_VND_WAP_WBMP                         0x21
#define WE_MIME_TYPE_APPLICATION_VND_WAP_MULTIPART_ANY          0x22
#define WE_MIME_TYPE_APPLICATION_VND_WAP_MULTIPART_MIXED        0x23
#define WE_MIME_TYPE_APPLICATION_VND_WAP_MULTIPART_FORM_DATA    0x24
#define WE_MIME_TYPE_APPLICATION_VND_WAP_MULTIPART_BYTERANGES   0x25
#define WE_MIME_TYPE_APPLICATION_VND_WAP_MULTIPART_ALTERNATIVE  0x26
#define WE_MIME_TYPE_APPLICATION_XML                            0x27
#define WE_MIME_TYPE_TEXT_XML                                   0x28
#define WE_MIME_TYPE_APPLICATION_VND_WAP_WBXML                  0x29
#define WE_MIME_TYPE_APPLICATION_X_X968_CROSS_CERT              0x2a
#define WE_MIME_TYPE_APPLICATION_X_X968_CA_CERT                 0x2b
#define WE_MIME_TYPE_APPLICATION_X_X968_USER_CERT               0x2c
#define WE_MIME_TYPE_TEXT_VND_WAP_SI                            0x2d
#define WE_MIME_TYPE_APPLICATION_VND_WAP_SIC                    0x2e
#define WE_MIME_TYPE_TEXT_VND_WAP_SL                            0x2f
#define WE_MIME_TYPE_APPLICATION_VND_WAP_SLC                    0x30
#define WE_MIME_TYPE_TEXT_VND_WAP_CO                            0x31
#define WE_MIME_TYPE_APPLICATION_VND_WAP_COC                    0x32
#define WE_MIME_TYPE_APPLICATION_VND_WAP_MULTIPART_RELATED      0x33
#define WE_MIME_TYPE_APPLICATION_VND_WAP_SIA                    0x34
#define WE_MIME_TYPE_TEXT_VND_WAP_CONNECTIVITY_XML              0x35
#define WE_MIME_TYPE_APPLICATION_VND_WAP_CONNECTIVITY_WBXML     0x36
#define WE_MIME_TYPE_APPLICATION_PKCS7_MIME                     0x37
#define WE_MIME_TYPE_APPLICATION_VND_WAP_HASHED_CERTIFICATE     0x38
#define WE_MIME_TYPE_APPLICATION_VND_WAP_SIGNED_CERTIFICATE     0x39
#define WE_MIME_TYPE_APPLICATION_VND_WAP_CERT_RESPONSE          0x3a
#define WE_MIME_TYPE_APPLICATION_XHTML_XML                      0x3b
#define WE_MIME_TYPE_APPLICATION_WML_XML                        0x3c
#define WE_MIME_TYPE_TEXT_CSS                                   0x3d
#define WE_MIME_TYPE_APPLICATION_VND_WAP_MMS_MESSAGE            0x3e
#define WE_MIME_TYPE_APPLICATION_VND_WAP_ROLLOVER_CERTIFICATE   0x3f
#define WE_MIME_TYPE_APPLICATION_VND_WAP_LOCC_WBXML             0x40
#define WE_MIME_TYPE_APPLICATION_VND_WAP_LOC_XML                0x41
#define WE_MIME_TYPE_APPLICATION_VND_SYNCML_DM_WBXML            0x42
#define WE_MIME_TYPE_APPLICATION_VND_SYNCML_DM_XML              0x43
#define WE_MIME_TYPE_APPLICATION_VND_SYNCML_NOTIFICATION        0x44
#define WE_MIME_TYPE_APPLICATION_VND_WAP_XHTML_XML              0x45
#define WE_MIME_TYPE_APPLICATION_VND_WV_CSP_CIR                 0x46
#define WE_MIME_TYPE_APPLICATION_VND_OMA_DD_XML                 0x47
#define WE_MIME_TYPE_APPLICATION_VND_OMA_DRM_MESSAGE            0x48
#define WE_MIME_TYPE_APPLICATION_VND_OMA_DRM_CONTENT            0x49
#define WE_MIME_TYPE_APPLICATION_VND_OMA_DRM_RIGHTS_XML         0x4a
#define WE_MIME_TYPE_APPLICATION_VND_OMA_DRM_RIGHTS_WBXML       0x4b
#define WE_MIME_TYPE_APPLICATION_VND_WV_CSP_XML                 0x4c
#define WE_MIME_TYPE_APPLICATION_VND_WV_CSP_WBXML               0x4d

/*
 * OMNA "Registered Values"
 */
#define WE_MIME_TYPE_APPLICATION_VND_UPLANET_CACHEOP_WBXML      0x0201
#define WE_MIME_TYPE_APPLICATION_VND_UPLANET_SIGNAL             0x0202
#define WE_MIME_TYPE_APPLICATION_VND_UPLANET_ALERT_WBXML        0x0203
#define WE_MIME_TYPE_APPLICATION_VND_UPLANET_LIST_WBXML         0x0204
#define WE_MIME_TYPE_APPLICATION_VND_UPLANET_LISTCMD_WBXML      0x0205
#define WE_MIME_TYPE_APPLICATION_VND_UPLANET_CHANNEL_WBXML      0x0206
#define WE_MIME_TYPE_APPLICATION_VND_UPLANET_PROVISIONING_STATUS_URI  0x0207
#define WE_MIME_TYPE_X_WAP_MULTIPART_VND_UPLANET_HEADER_SET     0x0208
#define WE_MIME_TYPE_APPLICATION_VND_UPLANET_BEARER_CHOICE_WBXML  0x0209
#define WE_MIME_TYPE_APPLICATION_VND_PHONECOM_MMC_WBXML         0x020a
#define WE_MIME_TYPE_APPLICATION_VND_NOKIA_SYNCSET_WBXML        0x020b
#define WE_MIME_TYPE_IMAGE_X_UP_WPNG                            0x020c
#define WE_MIME_TYPE_APPLICATION_IOTA_MMC_WBXML                 0x0300
#define WE_MIME_TYPE_APPLICATION_IOTA_MMC                       0x0301

/*
 * Other media types that have a pre-defined integer representation.
 * These are pre-defined internally by WE and used to enable the 
 * parser to handle these media types.
 */
#define WE_MIME_TYPE_AUDIO_ANY                                  0xa0
#define WE_MIME_TYPE_AUDIO_MID                                  0xa1
#define WE_MIME_TYPE_AUDIO_MPEG                                 0xa2
#define WE_MIME_TYPE_AUDIO_X_WAV                                0xa3
#define WE_MIME_TYPE_APPLICATION_X_VND_MMS_TEMPLATE_TEXTS       0xa4
#define WE_MIME_TYPE_APPLICATION_X_WAP_PROV_BROWSER_SETTINGS    0xa5
#define WE_MIME_TYPE_APPLICATION_X_WAP_PROV_BROWSER_BOOKMARKS   0xa6
#define WE_MIME_TYPE_APPLICATION_X_WAP_PROV_SYNCSET_XML         0xa7
#define WE_MIME_TYPE_APPLICATION_X_WAP_PROV_SYNCSET_WBXML       0xa8
#define WE_MIME_TYPE_APPLICATION_SMIL                           0xa9

#define WE_MIME_TYPE_IMAGE_SVG                                  0xaa
#define WE_MIME_TYPE_IMAGE_BMP                                  0xab
#define WE_MIME_TYPE_VIDEO_MPEG                                 0xac
#define WE_MIME_TYPE_APPLICATION_VND_SIAF                       0xad
#define WE_MIME_TYPE_APPLICATION_X_SIAF                         0xae
#define WE_MIME_TYPE_AUDIO_AMR                                  0xaf
#define WE_MIME_TYPE_APPLICATION_X_SHOCKWAVE_FLASH              0xb0
#define WE_MIME_TYPE_APPLICATION_ZIP                            0xb1
#define WE_MIME_TYPE_AUDIO_MIDI                                 0xb2
#define WE_MIME_TYPE_AUDIO_X_MIDI                               0xb3
#define WE_MIME_TYPE_AUDIO_SP_MIDI                              0xb4
#define WE_MIME_TYPE_AUDIO_IMELODY                              0xb5
#define WE_MIME_TYPE_TEXT_X_IMELODY                             0xb6
#define WE_MIME_TYPE_VIDEO_3GPP                                 0xb7 /* Video support*/
#define WE_MIME_TYPE_AUDIO_3GPP                                 0xb8 /* Video support*/
#define WE_MIME_TYPE_VIDEO_MP4                                  0xb9 /* Video support*/
#define WE_MIME_TYPE_AUDIO_MP4                                  0xba /* Video support*/

#define WE_MIME_TYPE_UNKNOWN                                    0xff

/************************************************************ 
 * Exported Variables
 ************************************************************/

extern const we_strtable_info_t we_mime_types;

/************************************************************ 
 * Exported functions
 ************************************************************/

/*
 * Returns the corresponding mime type for a file extension 
 */
const char*
we_mime_ext_to_mime (const char *ext);

/*
 * Returns the corresponding file extension for a mime type
 */
const char*
we_mime_to_ext (const char *mime);

/*
 * Returns the integer representation for the mime type
 */
WE_INT32 
we_mime_to_int (const char *mime);

/*
 * Returns the mime type given an integer representation.
 */
const char *
we_int_to_mime (WE_INT32 known_value);

/*
 * Returns the integer representation for the file extension
 * Note! that it is the integer representation of the mime type
 * corresponding to the extension that is returned.
 */
WE_INT32 
we_mime_ext_to_int (const char *ext);

#endif
