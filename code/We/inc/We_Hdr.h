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
 * We_Hdr.h
 *
 * Created by Anders Edenwbandt, Mon Sep 09 08:13:35 2002.
 *
 * Revision history:
 */
#ifndef _we_hdr_h
#define _we_hdr_h

#ifndef _we_def_h
#include "We_Def.h"
#endif
#ifndef _we_cmmn_h
#include "We_Cmmn.h"
#endif
#ifndef _we_mime_h
#include "We_Mime.h"
#endif


/************************************************************ 
 * Constants
 ************************************************************/

/* 
 * Supported header fields.
 * The numeric representation is the one used by WSP.
 * No suffix msans that the code belongs to encoding version 1.1-1.2.
 */
#define WE_HDR_ACCEPT                            0x00
#define WE_HDR_ACCEPT_CHARSET                    0x01
#define WE_HDR_ACCEPT_ENCODING                   0x02
#define WE_HDR_ACCEPT_LANGUAGE                   0x03
#define WE_HDR_ACCEPT_RANGES                     0x04
#define WE_HDR_AGE                               0x05
#define WE_HDR_ALLOW                             0x06
#define WE_HDR_AUTHORIZATION                     0x07
#define WE_HDR_CACHE_CONTROL                     0x08
#define WE_HDR_CONNECTION                        0x09
#define WE_HDR_CONTENT_BASE                      0x0a
#define WE_HDR_CONTENT_ENCODING                  0x0b
#define WE_HDR_CONTENT_LANGUAGE                  0x0c
#define WE_HDR_CONTENT_LENGTH                    0x0d
#define WE_HDR_CONTENT_LOCATION                  0x0e
#define WE_HDR_CONTENT_MD5                       0x0f
#define WE_HDR_CONTENT_RANGE                     0x10
#define WE_HDR_CONTENT_TYPE                      0x11
#define WE_HDR_DATE                              0x12
#define WE_HDR_ETAG                              0x13
#define WE_HDR_EXPIRES                           0x14
#define WE_HDR_FROM                              0x15
#define WE_HDR_HOST                              0x16
#define WE_HDR_IF_MODIFIED_SINCE                 0x17
#define WE_HDR_IF_MATCH                          0x18
#define WE_HDR_IF_NONE_MATCH                     0x19
#define WE_HDR_IF_RANGE                          0x1a
#define WE_HDR_IF_UNMODIFIED_SINCE               0x1b
#define WE_HDR_LOCATION                          0x1c
#define WE_HDR_LAST_MODIFIED                     0x1d
#define WE_HDR_MAX_FORWARDS                      0x1e
#define WE_HDR_PRAGMA                            0x1f
#define WE_HDR_PROXY_AUTHENTICATE                0x20
#define WE_HDR_PROXY_AUTHORIZATION               0x21
#define WE_HDR_PUBLIC                            0x22
#define WE_HDR_RANGE                             0x23
#define WE_HDR_REFERER                           0x24
#define WE_HDR_RETRY_AFTER                       0x25
#define WE_HDR_SERVER                            0x26
#define WE_HDR_TRANSFER_ENCODING                 0x27
#define WE_HDR_UPGRADE                           0x28
#define WE_HDR_USER_AGENT                        0x29
#define WE_HDR_VARY                              0x2a
#define WE_HDR_VIA                               0x2b
#define WE_HDR_WARNING                           0x2c
#define WE_HDR_WWW_AUTHENTICATE                  0x2d
#define WE_HDR_CONTENT_DISPOSITION               0x2e
#define WE_HDR_X_WAP_APPLICATION_ID              0x2f
#define WE_HDR_X_WAP_CONTENT_URI                 0x30
#define WE_HDR_X_WAP_INITIATOR_URI               0x31
#define WE_HDR_ACCEPT_APPLICATION                0x32
#define WE_HDR_BEARER_INDICATION                 0x33
#define WE_HDR_PUSH_FLAG                         0x34
#define WE_HDR_PROFILE                           0x35
#define WE_HDR_PROFILE_DIFF                      0x36
#define WE_HDR_PROFILE_WARNING                   0x37
#define WE_HDR_EXPECT                            0x38
#define WE_HDR_TE                                0x39
#define WE_HDR_TRAILER                           0x3a
#define WE_HDR_ACCEPT_CHARSET_1_3                0x3b
#define WE_HDR_ACCEPT_ENCODING_1_3               0x3c
#define WE_HDR_CACHE_CONTROL_1_3                 0x3d
#define WE_HDR_CONTENT_RANGE_1_3                 0x3e
#define WE_HDR_X_WAP_TOD                         0x3f
#define WE_HDR_CONTENT_ID                        0x40
#define WE_HDR_SET_COOKIE                        0x41
#define WE_HDR_COOKIE                            0x42
#define WE_HDR_ENCODING_VERSION                  0x43
#define WE_HDR_PROFILE_WARNING_1_4               0x44
#define WE_HDR_CONTENT_DISPOSITION_1_4           0x45
#define WE_HDR_X_WAP_SECURITY                    0x46
#define WE_HDR_CACHE_CONTROL_1_4                 0x47
#define WE_HDR_EXPECT_1_5                        0x48
#define WE_HDR_X_WAP_LOC_INVOCATION              0x49
#define WE_HDR_X_WAP_LOC_DELIVERY                0x4a

/* The following headers do not have any WSP values. */
#define WE_HDR_X_WAP_CPITAG                      0x6e
#define WE_HDR_X_WAP_PUSH_PROVURL                0x6f
#define WE_HDR_X_WAP_PUSH_ACCEPT                 0x70
#define WE_HDR_X_WAP_PUSH_ACCEPT_CHARSET         0x71
#define WE_HDR_X_WAP_PUSH_ACCEPT_ENCODING        0x72
#define WE_HDR_X_WAP_PUSH_ACCEPT_LANGUAGE        0x73
#define WE_HDR_X_WAP_PUSH_ACCEPT_APP_ID          0x74
#define WE_HDR_X_WAP_PUSH_MSG_SIZE               0x75 
#define WE_HDR_X_WAP_PUSH_ACCEPT_MAX_PUSH_REQ    0x76 
#define WE_HDR_X_WAP_TERMINAL_ID                 0x77
#define WE_HDR_X_WAP_PUSH_INFO                   0x78
#define WE_HDR_X_WAP_PUSH_STATUS                 0x79
#define WE_HDR_X_WAP_PUSH_OTA_VERSION            0x7a
#define WE_HDR_X_WAP_BEARER_INDICATION           0x7b
#define WE_HDR_X_WAP_AUTHENTICATE                0x7c
#define WE_HDR_X_WAP_AUTHORIZATION               0x7d
#define WE_HDR_X_WAP_PROFILE                     0x7e
#define WE_HDR_X_WAP_PROFILE_DIFF                0x7f
#define WE_HDR_X_WAP_PROFILE_WARNING             0x80
#define WE_HDR_X_UP_TPD_E2E_HEADERS              0x81
#define WE_HDR_X_FORWARDED_FOR                   0x82
#define WE_HDR_CONTENT_TRANSFER_ENCODING         0x83
#define WE_HDR_REFRESH                           0x84
#define WE_HDR_X_DRM                             0x85
#define WE_HDR_SET_COOKIE2                       0x86                     
#define WE_HDR_COOKIE2                           0x87
#define WE_HDR_X_VODAFONE_3GPDP_CONTEXT          0x88
#define WE_HDR_X_OMA_DRM_SEPARATE_DELIVERY       0x89
#define WE_HDR_RIGHTS_ISSUER                     0x8a
#define WE_HDR_CONTENT_NAME                      0x8b
#define WE_HDR_CONTENT_DESCRIPTION               0x8c
#define WE_HDR_CONTENT_VENDOR                    0x8d
#define WE_HDR_ICON_URI                          0x8e
#define WE_HDR_ENCRYPTION_METHOD                 0x8f
#define WE_HDR_X_VODAFONE_CONTENT                0x90
#define WE_HDR_UNKNOWN                           0x91



/* Languages */
#define WE_LANGUAGE_AA                 0x01
#define WE_LANGUAGE_AB                 0x02
#define WE_LANGUAGE_AF                 0x03
#define WE_LANGUAGE_AM                 0x04
#define WE_LANGUAGE_AR                 0x05
#define WE_LANGUAGE_AS                 0x06
#define WE_LANGUAGE_AY                 0x07
#define WE_LANGUAGE_AZ                 0x08
#define WE_LANGUAGE_BA                 0x09
#define WE_LANGUAGE_BE                 0x0A
#define WE_LANGUAGE_BG                 0x0B
#define WE_LANGUAGE_BH                 0x0C
#define WE_LANGUAGE_BI                 0x0D
#define WE_LANGUAGE_BN                 0x0E
#define WE_LANGUAGE_BO                 0x0F
#define WE_LANGUAGE_BR                 0x10
#define WE_LANGUAGE_CA                 0x11
#define WE_LANGUAGE_CO                 0x12
#define WE_LANGUAGE_CS                 0x13
#define WE_LANGUAGE_CY                 0x14
#define WE_LANGUAGE_DA                 0x15
#define WE_LANGUAGE_DE                 0x16
#define WE_LANGUAGE_DZ                 0x17
#define WE_LANGUAGE_EL                 0x18
#define WE_LANGUAGE_EN                 0x19
#define WE_LANGUAGE_EO                 0x1A
#define WE_LANGUAGE_ES                 0x1B
#define WE_LANGUAGE_ET                 0x1C
#define WE_LANGUAGE_EU                 0x1D
#define WE_LANGUAGE_FA                 0x1E
#define WE_LANGUAGE_FI                 0x1F
#define WE_LANGUAGE_FJ                 0x20
#define WE_LANGUAGE_FO                 0x82
#define WE_LANGUAGE_FR                 0x22
#define WE_LANGUAGE_FY                 0x83
#define WE_LANGUAGE_GA                 0x24
#define WE_LANGUAGE_GD                 0x25
#define WE_LANGUAGE_GL                 0x26
#define WE_LANGUAGE_GN                 0x27
#define WE_LANGUAGE_GU                 0x28
#define WE_LANGUAGE_HA                 0x29
#define WE_LANGUAGE_HE                 0x2A
#define WE_LANGUAGE_HI                 0x2B
#define WE_LANGUAGE_HR                 0x2C
#define WE_LANGUAGE_HU                 0x2D
#define WE_LANGUAGE_HY                 0x2E
#define WE_LANGUAGE_IA                 0x84
#define WE_LANGUAGE_ID                 0x30
#define WE_LANGUAGE_IE                 0x86
#define WE_LANGUAGE_IK                 0x87
#define WE_LANGUAGE_IS                 0x33
#define WE_LANGUAGE_IT                 0x34
#define WE_LANGUAGE_IU                 0x89
#define WE_LANGUAGE_JA                 0x36
#define WE_LANGUAGE_JW                 0x37
#define WE_LANGUAGE_KA                 0x38
#define WE_LANGUAGE_KK                 0x39
#define WE_LANGUAGE_KL                 0x8A
#define WE_LANGUAGE_KM                 0x3B
#define WE_LANGUAGE_KN                 0x3C
#define WE_LANGUAGE_KO                 0x3D
#define WE_LANGUAGE_KS                 0x3E
#define WE_LANGUAGE_KU                 0x3F
#define WE_LANGUAGE_KY                 0x40
#define WE_LANGUAGE_LA                 0x8B
#define WE_LANGUAGE_LN                 0x42
#define WE_LANGUAGE_LO                 0x43
#define WE_LANGUAGE_LT                 0x44
#define WE_LANGUAGE_LV                 0x45
#define WE_LANGUAGE_MG                 0x46
#define WE_LANGUAGE_MI                 0x47
#define WE_LANGUAGE_MK                 0x48
#define WE_LANGUAGE_ML                 0x49
#define WE_LANGUAGE_MN                 0x4A
#define WE_LANGUAGE_MO                 0x4B
#define WE_LANGUAGE_MR                 0x4C
#define WE_LANGUAGE_MS                 0x4D
#define WE_LANGUAGE_MT                 0x4E
#define WE_LANGUAGE_MY                 0x4F
#define WE_LANGUAGE_NA                 0x81
#define WE_LANGUAGE_NE                 0x51
#define WE_LANGUAGE_NL                 0x52
#define WE_LANGUAGE_NO                 0x53
#define WE_LANGUAGE_OC                 0x54
#define WE_LANGUAGE_OM                 0x55
#define WE_LANGUAGE_OR                 0x56
#define WE_LANGUAGE_PA                 0x57
#define WE_LANGUAGE_PO                 0x58
#define WE_LANGUAGE_PS                 0x59
#define WE_LANGUAGE_PT                 0x5A
#define WE_LANGUAGE_QU                 0x5B
#define WE_LANGUAGE_RM                 0x8C
#define WE_LANGUAGE_RN                 0x5D
#define WE_LANGUAGE_RO                 0x5E
#define WE_LANGUAGE_RU                 0x5F
#define WE_LANGUAGE_RW                 0x60
#define WE_LANGUAGE_SA                 0x61
#define WE_LANGUAGE_SD                 0x62
#define WE_LANGUAGE_SG                 0x63
#define WE_LANGUAGE_SH                 0x64
#define WE_LANGUAGE_SI                 0x65
#define WE_LANGUAGE_SK                 0x66
#define WE_LANGUAGE_SL                 0x67
#define WE_LANGUAGE_SM                 0x68
#define WE_LANGUAGE_SN                 0x69
#define WE_LANGUAGE_SO                 0x6A
#define WE_LANGUAGE_SQ                 0x6B
#define WE_LANGUAGE_SR                 0x6C
#define WE_LANGUAGE_SS                 0x6D
#define WE_LANGUAGE_ST                 0x6E
#define WE_LANGUAGE_SU                 0x6F
#define WE_LANGUAGE_SV                 0x70
#define WE_LANGUAGE_SW                 0x71
#define WE_LANGUAGE_TA                 0x72
#define WE_LANGUAGE_TE                 0x73
#define WE_LANGUAGE_TG                 0x74
#define WE_LANGUAGE_TH                 0x75
#define WE_LANGUAGE_TI                 0x76
#define WE_LANGUAGE_TK                 0x77
#define WE_LANGUAGE_TL                 0x78
#define WE_LANGUAGE_TN                 0x79
#define WE_LANGUAGE_TO                 0x7A
#define WE_LANGUAGE_TR                 0x7B
#define WE_LANGUAGE_TS                 0x7C
#define WE_LANGUAGE_TT                 0x7D
#define WE_LANGUAGE_TW                 0x7E
#define WE_LANGUAGE_UG                 0x7F
#define WE_LANGUAGE_UK                 0x50
#define WE_LANGUAGE_UR                 0x21
#define WE_LANGUAGE_UZ                 0x23
#define WE_LANGUAGE_VI                 0x2F
#define WE_LANGUAGE_VO                 0x85
#define WE_LANGUAGE_WO                 0x31
#define WE_LANGUAGE_XH                 0x32
#define WE_LANGUAGE_YI                 0x88
#define WE_LANGUAGE_YO                 0x35
#define WE_LANGUAGE_ZA                 0x3A
#define WE_LANGUAGE_ZH                 0x41
#define WE_LANGUAGE_ZU                 0x5C
#define WE_LANGUAGE_UNKNOWN            0xff


/* Application wid, well known values */
#define WE_X_WAP_APPLICATION                     0x00
#define WE_X_WAP_APPLICATION_PUSH_SIA            0x01
#define WE_X_WAP_APPLICATION_WML_UA              0x02
#define WE_X_WAP_APPLICATION_WTA_UA              0x03
#define WE_X_WAP_APPLICATION_MMS_UA              0x04
#define WE_X_WAP_APPLICATION_PUSH_SYNCML         0x05
#define WE_X_WAP_APPLICATION_LOC_UA              0x06

/* Application wid, registered values */
#define WE_X_WAP_MICROSOFT_LOCALCONTENT_UA       0x8000
#define WE_X_WAP_MICROSOFT_IMCLIENT_UA           0x8001
#define WE_X_WAP_DOCOMO_IMODE_MAIL_UA            0x8002
#define WE_X_WAP_DOCOMO_IMODE_MR_UA              0x8003
#define WE_X_WAP_DOCOMO_IMODE_MF_UA              0x8004


/**************************************************
 * Type definitions 
 ***************************************************/

typedef struct we_hdr_st we_hdr_t;


/************************************************************ 
 * Exported Variables
 ************************************************************/

extern const we_strtable_info_t we_hdr_names;
extern const we_strtable_info_t we_languages;


/************************************************************ 
 * Exported functions
 ************************************************************/

/*
 * Create a new, empty header table
 */ 
we_hdr_t*
we_hdr_create (WE_UINT8 module);

/* 
 * Deallocate a header table and all its content 
 */
void
we_hdr_delete (we_hdr_t* hdr);

/*
 * Copy a header table 
 */ 
we_hdr_t* 
we_hdr_copy (we_hdr_t* hdr);

/*
 * Delete all fields of the specified kind.
 */
void
we_hdr_delete_field (we_hdr_t *hdr, int field_name);


/************************************************************
 * Routines to add information to a header table. 
 ************************************************************/

/*
 * Add a string to the header table.
 * The string is copied into the table, and will be deallocated
 * when the header table is removed.
 * Returns TRUE on success and FALSE on error.
 */
int
we_hdr_add_string (we_hdr_t *hdr, int field_name, const char* value);

/*
 * Add a parameter value to a header that is already in the table.
 * The string is copied, and will be deallocated when the header
 * table is deleted.
 * Returns TRUE on success, or FALSE on error.
 */
int
we_hdr_add_parameter (we_hdr_t *hdr, int field_name,
                       const char *parameter, const char* value);

/*
 * Parses one line of text as an HTTP header, and adds
 * the relevant content to the header table.
 * The parameter 'str' is a null-terminated character string holding
 * one line of text, and 'len' is the number of characters in 'str'.
 * Returns TRUE on success and FALSE on error.
 */
int
we_hdr_add_header_from_text_line (we_hdr_t *hdr, const char *str, int len);

/*
 * Given a text string that holds a number of lines, each
 * terminated by <CR><LF> (except perhaps the last one),
 * this function reads these lines and adds the header value to the table.
 * Returns TRUE on success and FALSE on error.
 */
int
we_hdr_add_headers_from_text (we_hdr_t *hdr, const char *text);

/**********************************************************************
 * Routines to retrieve information from a header table. 
 **********************************************************************/

/* 
 * Make a string out of the contents of a header structure.
 * Returns TRUE on success and FALSE on error.
 */
int
we_hdr_make_string (we_hdr_t *hdr, char **str);

/*
 * Get a WE_UINT32 value from the first element with the given header name.
 * Returns TRUE if a header element was found, and FALSE otherwise.
 */
int
we_hdr_get_uint32 (we_hdr_t *hdr, int field_name, WE_UINT32 *value);

/*
 * Get a string value from the first element with the given header name.
 * Returns NULL if the requested header element is not in the table.
 * NOTE: the caller MUST NOT deallocate the returned string!
 */
const char *
we_hdr_get_string (we_hdr_t *hdr, int field_name);

/*
 * Get a parameter value from the first element with the given field name.
 * Returns NULL if the requested header element is not in the table,
 * or the header field does not have a parameter with the given name.
 * NOTE: it is the responsibility of the caller to deallocate the string!
 */
char *
we_hdr_get_parameter (we_hdr_t *hdr, int field_name, const char *parameter);


/**********************************************************************
 * Routines to build up and parse header parameter strings 
 **********************************************************************/

/* 
 *    Converts the strings param_name and param_value parameters to be 
 *  uniquely identified according to the HTTP/1.1 header encoding. 
 *  This implies that the name parameters are token strings, as defined 
 *  by HTTP/1.1, i.e. no presence of CTL's (US-ASCII 0x00 - 0x1F, 0x7F) 
 *  or reserved separators below: 
 *  
 *  '('  ')'  '<'  '>'  '@'  
 *  ','  ';'  ':'  '\'  '"'  
 *  '/'  '['  ']'  '?'  '='
 *  '{'  '}'  SPACE (0x20)  HTAB (0x09)
 *  
 *
 *  IF A FORBIDDEN CHARACTER IS ENCOUNTERED INSIDE A NAME ATTRIBUTE 
 *  THE WHOLE PARAMETER PAIR IS DISCARDED!
 *
 *  This restriction does not apply to the strings in the value array. 
 *  Values are transformed to quoted strings ( val(ue -> "val(ue" ) 
 *  with quoted pairs ( val"ue -> "val\"ue" ) if required.
 *    Returns:
 *        A string with the parameter pairs concatenated in consecutive order, 
 *    
 *    e.g param_name[0] = name1, param_value[0] = NULL
 *        param_name[1] = name2, param_value[1] = val(ue2
 *
 *    returns  name1;name2="val(ue2"
 *
 *  Note: The caller is responsible for for deallocating the returned
 *  memory buffer.
 */
char *
we_hdr_build_param_list (WE_UINT8 module, int num_params, 
                          const char *param_name[], const char *param_value[]);


/*
 *    Unpacks the parameter list into a name array and a corresponding value array.
 *  If a parameter name's corresponding value pair is not encountered its value 
 *  string pointer will point at NULL. This is the reverse function of
 *  we_hdr_build_parameter_string(). See its description header for more details.
 *
 *    Returns:
 *        The number of parameters that were found in the string, if this number 
 *    exceeds the max_params only the given number of parameters was retrieved. 
 *    the operation have to be redone with array sizes the returned int tells
 *    in order to retrieve all parameters. 
 *
 *  
 *  Note 1: The caller is responsible for for deallocating the
 *  memory buffers attached to the param_name and param_value arrays after the 
 *  function returns. 
 *
 *  Note 2: The arrays should not contain any pointers when used in this function.
 *  All array pointers will be reset by the function, if an array slot points out a
 *  memory buffer it might result in a leak!
 */
int
we_hdr_get_params (WE_UINT8 module, const char *param_list, 
                    int max_params, char *param_name[], char *param_value[]);

#endif
