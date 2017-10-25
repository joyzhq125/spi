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








#include "Pus_Cfg.h"

#ifdef PUS_CONFIG_MULTIPART

#include "Pus_Main.h"
#include "we_def.h"
#include "we_dcvt.h"
#include "we_hdr.h"
#include "we_chrt.h"
#include "we_chrs.h"




#define PUS_HDR_NBR_OF_HEADERS              72
#define PUS_HDR_NO_CACHE                        128 

#define PUS_HDR_SHIFT_DELIMITER             127
#define PUS_HDR_UNLIMITED                   10000
#define PUS_HDR_ENC_VER_HDR_NAME_LEN        17
#define PUS_HDR_ENCODE                      0
#define PUS_HDR_DECODE                      1


#define PUS_CC_NO_CACHE                     128
#define PUS_CC_NO_STORE                     129
#define PUS_CC_MAX_AGE                      130
#define PUS_CC_MAX_STALE                    131
#define PUS_CC_MIN_FRESH                    132
#define PUS_CC_ONLY_IF_CACHED               133
#define PUS_CC_PUBLIC                       134
#define PUS_CC_PRIVATE                      135
#define PUS_CC_NO_TRANSFORM                 136
#define PUS_CC_MUST_REVALIDATE              137
#define PUS_CC_PROXY_REVALIDATE             138
#define PUS_CC_S_MAXAGE                     139

#define PUS_NO_CACHE_FLAG                   0x8000
#define PUS_NO_STORE_FLAG                   0x4000
#define PUS_MAX_AGE_FLAG                    0x2000
#define PUS_MAX_STALE_FLAG                  0x1000
#define PUS_MIN_FRESH_FLAG                  0x0800
#define PUS_NO_TRANSFORM_FLAG               0x0400
#define PUS_ONLY_IF_CACHED_FLAG             0x0200
#define PUS_PUBLIC_FLAG                     0x0100
#define PUS_PRIVATE_FLAG                    0x0080
#define PUS_MUST_REVALIDATE_FLAG            0x0040
#define PUS_PROXY_REVALIDATE_FLAG           0x0020
#define PUS_S_MAX_AGE_FLAG                  0x0010

#define PUS_UTYPE_WE_UINT8                     1
#define PUS_UTYPE_WE_UINT32                    2
#define PUS_UTYPE_STRING_LIT                3  
#define PUS_UTYPE_STRING                    4  
#define PUS_UTYPE_DATE                      5  
#define PUS_UTYPE_DATA_STRING               6
#define PUS_UTYPE_NONE                      9

#define PUS_HDR_CVT_LENGTH_QUOTE            31
#define PUS_HDR_CVT_QUOTE                   127
#define PUS_HDR_CVT_DATESTRING_LENGTH       29
#define PUS_HDR_CVT_SPACE                   0x20
#define PUS_HDR_CVT_BASIC                   128

#define PUS_HDR_CVT_BASIC                   128
#define PUS_HDR_CVT_NO_STRING_EXISTS        10000
#define PUS_HDR_CVT_TRAILERS                129
#define PUS_HDR_CVT_NO_VALUE                0
#define PUS_HDR_CVT_STRING_QUOTE            34
#define PUS_HDR_CVT_ABSOLUTE_TIME           128
#define PUS_HDR_CVT_RELATIVE_TIME           129




#define ENCODING_VERSION_1_2            0x12
#define ENCODING_VERSION_1_3            0x13
#define ENCODING_VERSION_1_4            0x14


#define X_WAP_APPLICATION               0x00
#define X_WAP_APPLICATION_PUSH_SIA      0x01
#define X_WAP_APPLICATION_WML_UA        0x02
#define X_WAP_APPLICATION_WTA_UA        0x03
#define X_WAP_APPLICATION_MMS_UA        0x04
#define X_WAP_APPLICATION_PUSH_SYNCML   0x05
#define X_WAP_APPLICATION_LOC_UA        0x06


#define X_WAP_MICROSOFT_LOCALCONTENT_UA 0x8000
#define X_WAP_MICROSOFT_IMCLIENT_UA     0x8001
#define X_WAP_DOCOMO_IMODE_MAIL_UA      0x8002
#define X_WAP_DOCOMO_IMODE_MR_UA        0x8003
#define X_WAP_DOCOMO_IMODE_MF_UA        0x8004


#define PUS_PARAM_Q                         0x00
#define PUS_PARAM_CHARSET                   0x01
#define PUS_PARAM_LEVEL                     0x02
#define PUS_PARAM_TYPE                      0x03
#define PUS_PARAM_NAME                      0x05
#define PUS_PARAM_FILE_NAME                 0x06
#define PUS_PARAM_DIFFERENCES               0x07
#define PUS_PARAM_PADDING                   0x08
#define PUS_PARAM_TYPE_MP                   0x09
#define PUS_PARAM_START                     0x0A
#define PUS_PARAM_START_INFO                0x0B
#define PUS_PARAM_COMMENT                   0x0C
#define PUS_PARAM_DOMAIN                    0x0D
#define PUS_PARAM_MAX_AGE                   0x0E
#define PUS_PARAM_PATH                      0x0F
#define PUS_PARAM_SECURE                    0x10
#define PUS_PARAM_SEC                       0x11
#define PUS_PARAM_MAC                       0x12
#define PUS_PARAM_CREATION_DATE             0x13
#define PUS_PARAM_MODIFICATION_DATE         0x14
#define PUS_PARAM_READ_DATE                 0x15
#define PUS_PARAM_SIZE                      0x16
#define PUS_PARAM_NAME_1_4                  0x17
#define PUS_PARAM_FILE_NAME_1_4             0x18
#define PUS_PARAM_START_1_4                 0x19
#define PUS_PARAM_START_INFO_1_4            0x1A
#define PUS_PARAM_COMMENT_1_4               0x1B
#define PUS_PARAM_DOMAIN_1_4                0x1C
#define PUS_PARAM_PATH_1_4                  0x1D

#define PUS_PARAMETERS_HASH_TABLE_SIZE      43
#define PUS_PARAMETERS_STRING_TABLE_SIZE    29




static we_strtable_info_t pus_params;




static const unsigned char pus_parameters_hash_table[] = {
  255, 13, 21, 255, 28, 255, 20, 255, 255, 5,
  24, 2, 255, 8, 15, 11, 255, 255, 255, 255,
  6, 14, 23, 255, 255, 26, 17, 9, 25, 10,
  255, 27, 18, 255, 19, 3, 4, 12, 22, 1,
  7, 16, 0
};

static const we_strtable_entry_t pus_parameters_strings[] = {
  {"q", PUS_PARAM_Q},
  {"charset", PUS_PARAM_CHARSET},
  {"level", PUS_PARAM_LEVEL},
  {"type", PUS_PARAM_TYPE},
  {"name", PUS_PARAM_NAME},
  {"filename", PUS_PARAM_FILE_NAME},
  {"differences", PUS_PARAM_DIFFERENCES},
  {"padding", PUS_PARAM_PADDING},
  {"type_mp", PUS_PARAM_TYPE_MP},
  {"start", PUS_PARAM_START},
  {"start-info", PUS_PARAM_START_INFO},
  {"comment", PUS_PARAM_COMMENT},
  {"domain", PUS_PARAM_DOMAIN},
  {"max-age", PUS_PARAM_MAX_AGE},
  {"path", PUS_PARAM_PATH},
  {"secure", PUS_PARAM_SECURE},
  {"SEC", PUS_PARAM_SEC},
  {"MAC", PUS_PARAM_MAC},
  {"creation-date", PUS_PARAM_CREATION_DATE},
  {"modification-date", PUS_PARAM_MODIFICATION_DATE},
  {"read-date", PUS_PARAM_READ_DATE},
  {"size", PUS_PARAM_SIZE},
  {"name_1_4", PUS_PARAM_NAME_1_4},
  {"filename_1_4", PUS_PARAM_FILE_NAME_1_4},
  {"start_1_4", PUS_PARAM_START_1_4},
  {"start-info_1_4", PUS_PARAM_START_INFO_1_4},
  {"comment_1_4", PUS_PARAM_COMMENT_1_4},
  {"domain_1_4", PUS_PARAM_DOMAIN_1_4},
  {"path_1_4", PUS_PARAM_PATH_1_4}
};

static we_strtable_info_t pus_params = {
  pus_parameters_hash_table,
  PUS_PARAMETERS_HASH_TABLE_SIZE,
  WE_TABLE_SEARCH_BINARY,
  pus_parameters_strings,
  PUS_PARAMETERS_STRING_TABLE_SIZE
};

static const unsigned char pus_application_hash_table[] = {
  1, 11, 255, 3, 8, 7, 5, 255, 255, 2,
  255, 255, 6, 255, 0, 9, 10, 4, 255
};

static const we_strtable_entry_t pus_application_strings[] = {
  {"x-wap-application:*",             X_WAP_APPLICATION},
  {"x-wap-application:push.sia",      X_WAP_APPLICATION_PUSH_SIA},
  {"x-wap-application:wml.ua",        X_WAP_APPLICATION_WML_UA},
  {"x-wap-application:wta.ua",        X_WAP_APPLICATION_WTA_UA},
  {"x-wap-application:mms.ua",        X_WAP_APPLICATION_MMS_UA},
  {"x-wap-application:push.syncml",   X_WAP_APPLICATION_PUSH_SYNCML},
  {"x-wap-application:loc.ua",        X_WAP_APPLICATION_LOC_UA},
  {"x-wap-microsoft:localcontent.ua", X_WAP_MICROSOFT_LOCALCONTENT_UA},
  {"x-wap-microsoft:IMclient.ua",     X_WAP_MICROSOFT_IMCLIENT_UA},
  {"x-wap-docomo:imode.mail.ua",      X_WAP_DOCOMO_IMODE_MAIL_UA},
  {"x-wap-docomo:imode.mr.ua",        X_WAP_DOCOMO_IMODE_MR_UA},
  {"x-wap-docomo:imode.mf.ua",        X_WAP_DOCOMO_IMODE_MF_UA}
};

const we_strtable_info_t pus_applications = {
  pus_application_hash_table,
  19,
  WE_TABLE_SEARCH_LINEAR,
  pus_application_strings,
  12
};





 
typedef struct Pus_Hdr_element_st {
  struct Pus_Hdr_element_st *next;

  int field_name;
  int utype; 
  union {
    char   *str;
    WE_UINT8   uint8;
    WE_UINT32  uint32;
  } _u;
  void   *extras;
} Pus_Hdr_element_t;

typedef struct {
  Pus_Hdr_element_t *hdr_fields;
  Pus_Hdr_element_t *last_added; 
} Pus_Hdr_table_t;

typedef struct {
  WE_UINT16  flags;
  WE_UINT32  secage;
  WE_UINT32  secstale;
  WE_UINT32  secfresh;
  WE_UINT32  secS_maxage;
} Pus_Hdr_cachecontrol_t;







typedef struct Pus_Hdr_params_st {
  struct Pus_Hdr_params_st *next;

  char *name;
  char *value;
} Pus_Hdr_params_t;






static int 
Pus_Hdr_add_binary_headers (Pus_Hdr_table_t *table, unsigned char *headers, int headers_len, 
                            int is_connect_headers);

static int
Pus_Hdr_build_textual (Pus_Hdr_table_t *table, char **headers);

static Pus_Hdr_element_t *
Pus_Hdr_element_new (int field_name);

static void
Pus_Hdr_add_element (Pus_Hdr_table_t* table, Pus_Hdr_element_t *hdr_element);

static void
Pus_Hdr_delete_element (Pus_Hdr_element_t* hdr_element);

static void
Pus_Hdr_delete_params_list (Pus_Hdr_params_t *params_list);

static void
Pus_Hdr_delete_params (Pus_Hdr_params_t* params);

void
Pus_Hdr_delete_table (Pus_Hdr_table_t* table);

static Pus_Hdr_element_t *
Pus_Hdr_element_find (Pus_Hdr_table_t *table, int field_name);

static void
Pus_Hdr_add_parameter_to_element (Pus_Hdr_element_t *elt,
                                  const char *param_name,
                                  const char *param_value);
static Pus_Hdr_params_t *
Pus_Hdr_new_params (void);

static int
Pus_Hdr_cvt_short_int (we_dcvt_t* obj, WE_UINT8* p);

static int
Pus_Hdr_cvt_uint32var (we_dcvt_t *obj, int length, WE_UINT32 *p);

static int 
Pus_Hdr_cvt_string_length (we_dcvt_t* obj);

static int
Pus_Hdr_cvt_authenticate (we_dcvt_t* obj, Pus_Hdr_element_t* elt);

static int 
Pus_Hdr_cvt_encoding_version (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version);

static int
Pus_Hdr_cvt_quoted_string (we_dcvt_t* obj, char** p);

static int 
Pus_Hdr_cvt_retry_after (we_dcvt_t *obj, Pus_Hdr_element_t *elt, int encoding_version);

static int
Pus_Hdr_cvt_content_md5 (we_dcvt_t *obj, Pus_Hdr_element_t *elt, int encoding_version);

static int
Pus_Hdr_cvt_cachecontrol (we_dcvt_t* obj, Pus_Hdr_cachecontrol_t* p);

static int
Pus_Hdr_cvt_uint32_len (WE_UINT32 num);

static int
Pus_Hdr_cvt_int_val (we_dcvt_t* obj, WE_UINT32* p);

static void
Pus_Hdr_uint322string (WE_UINT32 i, char **s);

static void
Pus_Hdr_string2uint32 (char *s, WE_UINT32 *i);

static int
Pus_Hdr_get_uint32_string_len (WE_UINT32 i);

static void
Pus_Hdr_cvt_levelint_to_str (int level, char *s);

static int
Pus_Hdr_cvt_long_int (we_dcvt_t *obj, WE_UINT32 *p);

int
Pus_Hdr_cvt_string (we_dcvt_t* obj, char** p);

static int 
Pus_Hdr_cvt_text_value (we_dcvt_t* obj, char** p);

static int
Pus_Hdr_cvt_content_disposition (we_dcvt_t *obj, Pus_Hdr_element_t *elt, int encoding_version);

static int
Pus_Hdr_cvt_cookie (we_dcvt_t *obj, Pus_Hdr_element_t *elt, int encoding_version);

static int
Pus_Hdr_cvt_profilewarning (we_dcvt_t *obj, Pus_Hdr_element_t *elt, int encoding_version);

static int
Pus_Hdr_cvt_version_value (char** version_string, WE_UINT8* version_value);



static int
Pus_Hdr_encode_decode_int (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version);

static int
Pus_Hdr_encode_decode_long_int (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version);

static int
Pus_Hdr_cvt_contenttype (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version);

static int
Pus_Hdr_encode_decode_string (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version);

static int
Pus_Hdr_cvt_parameters (we_dcvt_t* obj, int* paramslength, Pus_Hdr_element_t *elt);

static int
Pus_Hdr_decode_authenticate (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version);

static int
Pus_Hdr_encode_decode_short_int (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version);

static int
Pus_Hdr_encode_decode_encoding_version (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version);

static int
Pus_Hdr_encode_decode_cache_control (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version);

static int
Pus_Hdr_decode_text_and_int (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version);

static int
Pus_Hdr_decode_text_and_short_int (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version);

static int
Pus_Hdr_encode_decode_quoted_string (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version);

static int
Pus_Hdr_decode_value_length (we_dcvt_t *obj, int *value_length);

static int
Pus_Hdr_decode_junk (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version);

static int
Pus_Hdr_decode_pragma (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version);

static int
pus_encode_one_cookie (we_dcvt_t *obj, char **cookies, WE_UINT8 *version);

static int
pus_length_relevant_parameters (Pus_Hdr_params_t* params_list);

static int
pus_add_relevant_parameters (Pus_Hdr_params_t* params_list, char** string);

static int
pus_add_to_string (char *headerstring, int str_index,
               const char *str_first, const char *str_after);

static void
Pus_Hdr_make_header_name (char *dst, const char *src);

static int
pus_create_content_type_string (Pus_Hdr_element_t *elt, char *headerstring,
                                int str_index);

static int
pus_create_cache_control_string (Pus_Hdr_cachecontrol_t *cc, char *headerstring,
                                int str_index);

static int
pus_add_crlf_to_string (char *headerstring, int str_index);

static int
Pus_Hdr_txt_calculate_length (Pus_Hdr_table_t* table);

static void
Pus_Hdr_txt_create_string (Pus_Hdr_table_t* table, char *headerstring);

static int
pus_get_parameter_length (Pus_Hdr_params_t *param);

static int
pus_get_cache_control_length (Pus_Hdr_cachecontrol_t *cc);

static int
pus_get_int_length (WE_UINT32 n);

typedef int (*function_pointer)(we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version);

static const
function_pointer function_vector[PUS_HDR_NBR_OF_HEADERS] =
{
  Pus_Hdr_cvt_contenttype, 
  0, 
  0, 
  0, 
  Pus_Hdr_decode_text_and_short_int, 
  Pus_Hdr_encode_decode_int, 
  Pus_Hdr_encode_decode_short_int, 
  0, 
  Pus_Hdr_encode_decode_cache_control, 
  Pus_Hdr_decode_text_and_short_int, 
  Pus_Hdr_encode_decode_string, 
  Pus_Hdr_decode_text_and_short_int, 
  Pus_Hdr_decode_text_and_int, 
  Pus_Hdr_encode_decode_int, 
  Pus_Hdr_encode_decode_string, 
  Pus_Hdr_cvt_content_md5, 
  Pus_Hdr_decode_junk, 
  Pus_Hdr_cvt_contenttype, 
  Pus_Hdr_encode_decode_long_int, 
  Pus_Hdr_encode_decode_string,
  Pus_Hdr_encode_decode_long_int, 
  0, 
  0, 
  0, 
  0, 
  0, 
  0, 
  0,         
  Pus_Hdr_encode_decode_string, 
  Pus_Hdr_encode_decode_long_int,
  0, 
  Pus_Hdr_decode_pragma, 
  Pus_Hdr_decode_authenticate, 
  0, 
  Pus_Hdr_decode_text_and_short_int, 
  0, 
  0,       
  Pus_Hdr_cvt_retry_after, 
  Pus_Hdr_encode_decode_string, 
  Pus_Hdr_decode_text_and_short_int,
  Pus_Hdr_encode_decode_string, 
  0, 
  Pus_Hdr_decode_text_and_short_int, 
  Pus_Hdr_encode_decode_string, 
  Pus_Hdr_decode_junk,  
  Pus_Hdr_decode_authenticate, 
  Pus_Hdr_cvt_content_disposition, 
  Pus_Hdr_decode_text_and_int, 
  Pus_Hdr_encode_decode_string, 
  Pus_Hdr_encode_decode_string,
  0, 
  0, 
  Pus_Hdr_encode_decode_short_int, 
  0, 
  0, 
  Pus_Hdr_cvt_profilewarning, 
  0, 
  0, 
  Pus_Hdr_decode_text_and_int, 
  0,
  0, 
  Pus_Hdr_encode_decode_cache_control, 
  Pus_Hdr_decode_junk,  
  Pus_Hdr_encode_decode_long_int, 
  Pus_Hdr_encode_decode_quoted_string, 
  Pus_Hdr_cvt_cookie,  
  0,  
  Pus_Hdr_encode_decode_encoding_version, 
  Pus_Hdr_cvt_profilewarning, 
  Pus_Hdr_cvt_content_disposition, 
  0, 
  Pus_Hdr_encode_decode_cache_control 
};

int
Pus_Hdr_create_hdr_string_from_bin_hdrs (we_hdr_t *table, unsigned char *headers, int headers_len)
{
  int    result;
  char  *header_string;
  




  Pus_Hdr_table_t *pus_table = PUS_MEM_ALLOCTYPE (Pus_Hdr_table_t);
  pus_table->hdr_fields = NULL;
  result = Pus_Hdr_add_binary_headers (pus_table, headers, headers_len, FALSE);
  Pus_Hdr_build_textual (pus_table, &header_string);
  Pus_Hdr_delete_table(pus_table);
  we_hdr_add_headers_from_text (table, header_string);
  PUS_MEM_FREE (header_string);
  return result;
}



static int 
Pus_Hdr_add_binary_headers (Pus_Hdr_table_t *table, unsigned char *headers, int headers_len, 
                            int is_connect_headers)
{  
  Pus_Hdr_element_t     *elt;
  WE_UINT8              temp;
  WE_UINT8              length8;
  WE_UINT32             length32;
  we_dcvt_t             obj;
  WE_UINT8              codepage = 1;
  function_pointer       function;
  
  
  if (headers_len == 0) 
    return TRUE;
  
  we_dcvt_init (&obj, WE_DCVT_DECODE, headers, headers_len, WE_MODID_PUS);
  

  if (!is_connect_headers) {
    if (obj.data[0] == 0) {
      if (!we_dcvt_change_pos (&obj, 1))
        return FALSE;
    }
    else {
      elt = Pus_Hdr_element_new (WE_HDR_CONTENT_TYPE);
      if (!Pus_Hdr_cvt_contenttype (&obj, elt, 0))
        return FALSE;
      Pus_Hdr_add_element (table, elt);
    }
  }
  
  while (obj.pos < obj.length) { 
    
    if ((obj.data[obj.pos] == PUS_HDR_SHIFT_DELIMITER) ||
        ((obj.data[obj.pos] >= 2) && (obj.data[obj.pos] <= 31))) {     
      if (obj.data[obj.pos] == PUS_HDR_SHIFT_DELIMITER) {
        if (!we_dcvt_change_pos (&obj, 1) ||
            !we_dcvt_uint8 (&obj, &codepage))
          return FALSE;
      }
      else if (!we_dcvt_uint8 (&obj, &codepage))
        return FALSE;
    }
    
    if (codepage != 1)
      
      return FALSE;
    
    if ((obj.data[obj.pos] >= 128) && (obj.data[obj.pos] <= 199)) {
      if (!Pus_Hdr_cvt_short_int (&obj, &temp))
        return FALSE;
      function = function_vector [temp];
      if (function == NULL) {
        if (obj.data[obj.pos] <= 30)
          we_dcvt_change_pos (&obj, obj.data[obj.pos]);
        else if (obj.data[obj.pos] == 31)
          we_dcvt_uintvar (&obj, &length32);
        else if ((obj.data[obj.pos] >= 32) && (obj.data[obj.pos] <= 127))
          we_dcvt_change_pos (&obj, Pus_Hdr_cvt_string_length (&obj));
        else
          we_dcvt_change_pos (&obj, 1);
      }
      else {
        if ((temp == WE_HDR_CACHE_CONTROL) || (temp == WE_HDR_CACHE_CONTROL_1_3) ||
            (temp == WE_HDR_CACHE_CONTROL_1_4)) {
          temp = WE_HDR_CACHE_CONTROL;
          if ((elt = Pus_Hdr_element_find (table, WE_HDR_CACHE_CONTROL)) == NULL) {
            Pus_Hdr_cachecontrol_t *cc = PUS_MEM_ALLOCTYPE (Pus_Hdr_cachecontrol_t);
            elt = Pus_Hdr_element_new (WE_HDR_CACHE_CONTROL);
            elt->utype = PUS_UTYPE_NONE;
            cc->flags = 0;
            cc->secage = cc->secfresh = cc->secstale = 0;
            elt->extras = cc;
            Pus_Hdr_add_element (table, elt);
          }
        }
        else {
          elt = Pus_Hdr_element_new (temp);
        }
        if (!function (&obj, elt, 0))
          return FALSE;
        if ((temp != WE_HDR_CACHE_CONTROL) && (function != Pus_Hdr_decode_junk))
          
          Pus_Hdr_add_element (table, elt);      
      }
    }
    
    else if ((obj.data[obj.pos] >= 32) && (obj.data[obj.pos] <= 127)) {
      we_dcvt_change_pos (&obj, Pus_Hdr_cvt_string_length (&obj));
       
      if ((obj.data[obj.pos] >= 32) && (obj.data[obj.pos] <= 127))
        we_dcvt_change_pos (&obj, Pus_Hdr_cvt_string_length (&obj));
      else if ((obj.data[obj.pos]) & 0x80)
        we_dcvt_change_pos (&obj, 1);
      else if (obj.data[obj.pos] <= 31) {
        we_dcvt_uint8 (&obj, &length8);
        if (length8 == PUS_HDR_CVT_LENGTH_QUOTE) {
          we_dcvt_uintvar (&obj, &length32);
          length8 = (WE_UINT8)length32;
        }
        we_dcvt_change_pos (&obj, length8);
      }
    }
    
    else if (obj.data[obj.pos] == 0) {
      if (!we_dcvt_change_pos (&obj, 1))
        return FALSE;
    } 
    else
      return FALSE;
  }
  return TRUE;
}




static Pus_Hdr_element_t *
Pus_Hdr_element_new (int field_name)
{
  Pus_Hdr_element_t *elt = PUS_MEM_ALLOCTYPE (Pus_Hdr_element_t);

  elt->next = NULL;
  
  if (field_name == WE_HDR_CACHE_CONTROL_1_3 || field_name == WE_HDR_CACHE_CONTROL_1_4)
    elt->field_name = WE_HDR_CACHE_CONTROL;
  else if (field_name == WE_HDR_CONTENT_RANGE_1_3)
    elt->field_name = WE_HDR_CONTENT_RANGE;
  else if (field_name == WE_HDR_PROFILE_WARNING_1_4)
    elt->field_name = WE_HDR_PROFILE_WARNING;
  else if (field_name == WE_HDR_CONTENT_DISPOSITION_1_4)
    elt->field_name = WE_HDR_CONTENT_DISPOSITION;
  else
    elt->field_name = field_name;
  elt->extras = NULL;
  elt->utype = PUS_UTYPE_NONE;

  return elt;
}







static void
Pus_Hdr_add_element (Pus_Hdr_table_t* table, Pus_Hdr_element_t *hdr_element)
{
  Pus_Hdr_element_t *elt, *pred;

  if ((hdr_element == NULL) || (table == NULL))
    return;

  for (pred = NULL, elt = table->hdr_fields; elt != NULL;
       pred = elt, elt = elt->next) {
    if (elt->field_name == hdr_element->field_name)
      break;
  }

  if (elt != NULL) {
    pred = elt;
    for (; (elt != NULL) && (elt->field_name == hdr_element->field_name);
         pred = elt, elt = elt->next);
    hdr_element->next = pred->next;
    pred->next = hdr_element;
  }
  else {
    if (pred != NULL)
      pred->next = hdr_element;
    else
      table->hdr_fields = hdr_element;
    hdr_element->next = NULL;
  }
}

static void
Pus_Hdr_delete_params (Pus_Hdr_params_t* params)
{
  if (params != NULL) {
    PUS_MEM_FREE (params->name);
    PUS_MEM_FREE(params->value);
    PUS_MEM_FREE (params);
  }
}




static void
Pus_Hdr_delete_params_list (Pus_Hdr_params_t *params_list)
{
  Pus_Hdr_params_t *temp;

  if (params_list != NULL) {
    while (params_list != NULL) {
      temp = params_list;
      params_list = params_list->next;
      Pus_Hdr_delete_params (temp);
    }
  }
}




static void
Pus_Hdr_delete_element (Pus_Hdr_element_t* hdr_element)
{
  if (hdr_element->utype == PUS_UTYPE_STRING)
    PUS_MEM_FREE (hdr_element->_u.str);

  if (hdr_element->field_name == WE_HDR_CACHE_CONTROL) {
    PUS_MEM_FREE (hdr_element->extras);
  }
  else if (hdr_element->extras != NULL) {
    if (hdr_element->utype == PUS_UTYPE_DATA_STRING)
      PUS_MEM_FREE (hdr_element->extras);
    else
      Pus_Hdr_delete_params_list ((Pus_Hdr_params_t *)hdr_element->extras);
  }

  PUS_MEM_FREE (hdr_element);
}



static void
Pus_Hdr_add_parameter_to_element (Pus_Hdr_element_t *elt,
                                  const char *param_name,
                                  const char *param_value)
{
  Pus_Hdr_params_t *params, *pr;

  params = Pus_Hdr_new_params ();
  params->name = we_cmmn_strdup (WE_MODID_PUS, param_name);
  params->value = we_cmmn_strdup (WE_MODID_PUS, param_value);

  if (elt->extras == NULL) {
    elt->extras = params;
  }
  else {
    for (pr = elt->extras; pr->next != NULL; pr = pr->next);
    pr->next = params;
  }
}

static Pus_Hdr_params_t *
Pus_Hdr_new_params (void)
{
  Pus_Hdr_params_t *params = PUS_MEM_ALLOCTYPE (Pus_Hdr_params_t);

  params->next = NULL;
  params->name = NULL;
  params->value = NULL;

  return params;
}




static Pus_Hdr_element_t *
Pus_Hdr_element_find (Pus_Hdr_table_t *table, int field_name)
{
  Pus_Hdr_element_t *elt;

  if (table == NULL)
    return NULL;

  for (elt = table->hdr_fields; elt != NULL; elt = elt->next) {
    if (elt->field_name == field_name)
      break;
  }

  return elt;
}




static int
Pus_Hdr_cvt_short_int (we_dcvt_t* obj, WE_UINT8* p)
{
  if ((obj == NULL) || (p == NULL)) 
    return FALSE;

  switch (obj->operation) {
  case WE_DCVT_DECODE:
    if ((WE_DCVT_REM_LENGTH(obj) < 1) || !(obj->data[obj->pos] & 0x80))
      return FALSE;
    *p = (WE_UINT8)(0x7f & WE_DCVT_GET_BYTE(obj)); 
    break;

  case WE_DCVT_ENCODE:
    if (WE_DCVT_REM_LENGTH(obj) < 1)
      return FALSE;
    if (*p < 128)
      WE_DCVT_PUT_BYTE(obj, 0x80 | (*p));
    else
      return FALSE;
    break;

  case WE_DCVT_ENCODE_SIZE:
    if (!we_dcvt_change_pos (obj, 1))
      return FALSE;
    break;

  default:
    return FALSE;
  }
  return TRUE;
}

static int
Pus_Hdr_cvt_uint32var (we_dcvt_t *obj, int length, WE_UINT32 *p)
{
  WE_UINT32 temp = 0;
  int    i = 0;

  if ((obj == NULL) || (p == NULL))
    return FALSE;

  switch (obj->operation) {
  case WE_DCVT_DECODE:
    if (WE_DCVT_REM_LENGTH(obj) < length)
      return FALSE;
    for (i = 0; i < length; i++) 
      temp = (temp << 8) | WE_DCVT_GET_BYTE(obj);
    *p = temp; 
    break;

  case WE_DCVT_ENCODE:
    if (WE_DCVT_REM_LENGTH(obj) < length)
      return FALSE;
    for (i = 1; i <= length; i++) 
      WE_DCVT_PUT_BYTE(obj, (WE_UINT8)(*p >> (length - i) * 8));
    break;

  case WE_DCVT_ENCODE_SIZE:
    obj->pos += Pus_Hdr_cvt_uint32_len (*p);
    break;

  default:
    return FALSE;
  }
  return TRUE;
}





static int 
Pus_Hdr_cvt_string_length (we_dcvt_t* obj)
{
  int pos_copy;
  
  if (obj == NULL)
    return FALSE;

  pos_copy = obj->pos;
 
  while ((obj->data[pos_copy] != '\0') && (pos_copy < obj->length))
    pos_copy++;

  if ((pos_copy == obj->length) || (obj->data[pos_copy] != '\0'))
    return -1; 

  
  return (pos_copy - obj->pos + 1);
}

static void
Pus_Hdr_uint322string (WE_UINT32 i, char **s)
{
  int len;

  len = Pus_Hdr_get_uint32_string_len (i);
  if (len > 0)
  {
    *s = PUS_MEM_ALLOC (len + 1);
    sprintf (*s, "%lu", i);
  }
  else
    *s = NULL;
}

static int
Pus_Hdr_get_uint32_string_len (WE_UINT32 i)
{
  char temp[35];
  int  len;

  len = sprintf (temp, "%lu", i);
  return len;
}

static void
Pus_Hdr_string2uint32 (char *s, WE_UINT32 *i)
{
  if (s != NULL) {
    *i = atoi (s);
    PUS_MEM_FREE (s);
  }
  else
    *i = 0;
}





static int
Pus_Hdr_cvt_int_val (we_dcvt_t* obj, WE_UINT32* p)
{
  WE_UINT8 pcopy = 0; 

  if ((obj == NULL) || (p == NULL)) 
    return FALSE;

  switch (obj->operation) {
  case WE_DCVT_DECODE:
    if (obj->data[obj->pos] & 128) {
      if (!Pus_Hdr_cvt_short_int (obj, &pcopy))
        return FALSE;
      *p = (WE_UINT32)pcopy;
    }
    else {
      if (!Pus_Hdr_cvt_long_int (obj, p))
        return FALSE;
    }
    break;

  case WE_DCVT_ENCODE:  
    if (*p <= 127)   { 
      pcopy = (WE_UINT8)*p;
      if (!Pus_Hdr_cvt_short_int (obj, &pcopy))
        return FALSE;
    }
    else {
      if (!Pus_Hdr_cvt_long_int (obj, p))
        return FALSE;
    }
    break;

  case WE_DCVT_ENCODE_SIZE:
    if (*p <= 127) {  
      if (!Pus_Hdr_cvt_short_int (obj, &pcopy))
        return FALSE;
    }
    else {
      if (!Pus_Hdr_cvt_long_int (obj, p))
        return FALSE;
    }
    break;

  default:
    return FALSE;
  }
  return TRUE;
}

static void
Pus_Hdr_cvt_levelint_to_str (int level, char *s)
{
  s[0] = (char)('0' + ((level & 0x70) >> 4));
  s[1] = '.';
  if ((level & 0x0f) == 15)
    s[2] = '0';
  else
    s[2] = (char)('0' + (level & 0x0f));
  s[3] = '\0';
}




static int
Pus_Hdr_cvt_long_int (we_dcvt_t *obj, WE_UINT32 *p)
{
  WE_UINT8 length = 0;
  int check;

  if ((obj == NULL) || (p == NULL)) 
    return FALSE;

  if (obj->operation == WE_DCVT_DECODE) { 
    check = obj->data[obj->pos];
    if (!(check <= 30))
      return FALSE;
  }
  if (obj->operation == WE_DCVT_ENCODE_SIZE) {
    
    we_dcvt_change_pos (obj, Pus_Hdr_cvt_uint32_len (*p) + 1);
    return TRUE;
  }
  if (obj->operation == WE_DCVT_ENCODE)
    length = (WE_UINT8)Pus_Hdr_cvt_uint32_len (*p);
  if ((!we_dcvt_uint8 (obj, &length)) ||
      (!Pus_Hdr_cvt_uint32var (obj, length, p)))
    return FALSE;

  return TRUE;
}




int
Pus_Hdr_cvt_string (we_dcvt_t* obj, char** p)
{
  WE_UINT8 temp;

  if (obj == NULL) 
    return FALSE;

  switch (obj->operation) {
  case WE_DCVT_DECODE: 
    if(!we_dcvt_uint8 (obj, &temp))
      return FALSE;
    if (temp == PUS_HDR_CVT_QUOTE) {
      

      if(!we_dcvt_uchar_vector (obj, Pus_Hdr_cvt_string_length (obj),(unsigned char**) p))
        return FALSE;
    }
    else {      
      if (!we_dcvt_change_pos (obj, -1)||
        !we_dcvt_uchar_vector (obj, Pus_Hdr_cvt_string_length (obj), (unsigned char**) p))
        return FALSE;
    }
    break;

  case WE_DCVT_ENCODE:
    if (*p == NULL)
      return FALSE;
    temp = PUS_HDR_CVT_QUOTE;    
    if (**p & 0x80) { 
      if (!we_dcvt_uint8 (obj, &temp))
        return FALSE;
    }    
    
    if (!we_dcvt_uchar_vector (obj, strlen (*p) + 1, (unsigned char**) p))
      return FALSE;
    break;

  case WE_DCVT_ENCODE_SIZE:
    if (*p == NULL)
      return FALSE;
    if(**p & 0x80) {
      if (!we_dcvt_change_pos (obj, 1))
        return FALSE;
    }
    if (!we_dcvt_change_pos (obj, strlen(*p) + 1))
      return FALSE;
    break;

  default:
    return FALSE;
  }
  return TRUE;
}

static int
Pus_Hdr_cvt_authenticate (we_dcvt_t* obj, Pus_Hdr_element_t* elt)
{
  WE_UINT32 length = 0;
  WE_UINT8  lengthcopy = 0;
  WE_UINT8 temp = 0;
  unsigned char* buffer;
  int            i;
  char* temp_pointer;

  if ((obj == NULL) || elt == NULL || (obj->operation != WE_DCVT_DECODE))
    return FALSE;

  if (!we_dcvt_uint8 (obj, &lengthcopy))
    return FALSE;
  length = (WE_UINT32)lengthcopy;
  if (length == PUS_HDR_CVT_LENGTH_QUOTE) { 
    if (!we_dcvt_uintvar (obj, &length))
      return FALSE;
  }
  if (!we_dcvt_uint8 (obj, &temp))
    return FALSE;
  if (temp == PUS_HDR_CVT_BASIC) { 
    if (!Pus_Hdr_cvt_string (obj, &(elt->_u.str)))
      return FALSE;
  }
  else {
    if (!we_dcvt_change_pos (obj, -1))
      return FALSE;
    if (!we_dcvt_uchar_vector (obj, length, &buffer))
      return FALSE;
    
    for (i = 0; i < (int) (length - 1); i++) {
      if (buffer[i] == '\0')
        buffer[i] = ',';
    }
    

    temp_pointer = (char*) buffer;
    while (temp_pointer != NULL) {
      temp_pointer = strchr (temp_pointer, '"');
      temp_pointer = strchr (temp_pointer, ',');
      if (temp_pointer != NULL)
        temp_pointer[0] = '"';
    }

    

    temp_pointer = strstr ((char*) buffer, "algorithm");
    temp_pointer = strchr (temp_pointer, ',');
    temp_pointer[0] = '=';
    temp_pointer = strstr ((char*) buffer, "stale");
    temp_pointer = strchr (temp_pointer, ',');
    if (temp_pointer != NULL)
      temp_pointer[0] = '=';
    elt->_u.str = (char*) buffer;
  }
  return TRUE;
}




static int 
Pus_Hdr_cvt_encoding_version (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version)
{
  char  *ONE_TWO = "1.2";
  char  *ONE_THREE = "1.3";
  char  *ONE_FOUR = "1.4";
  WE_UINT8  length8;
  WE_UINT32 length32;

  
  if (obj->operation == WE_DCVT_ENCODE) {    
    if (encoding_version <= ENCODING_VERSION_1_2) {
      switch ((elt->_u.uint8)) {
      case ENCODING_VERSION_1_2:
        if (!Pus_Hdr_cvt_string (obj, &ONE_TWO))
          return FALSE;
        break;

      case ENCODING_VERSION_1_3:
        if (!Pus_Hdr_cvt_string (obj, &ONE_THREE))
          return FALSE;
        break;

      case ENCODING_VERSION_1_4:
        if (!Pus_Hdr_cvt_string (obj, &ONE_FOUR))
          return FALSE;
        break;

      default:
        return FALSE;
      }
    }
    else if (!Pus_Hdr_cvt_short_int (obj, &(elt->_u.uint8))) {
      return FALSE;
    }
  }

  else if (obj->operation == WE_DCVT_ENCODE_SIZE) {
    if (encoding_version <= ENCODING_VERSION_1_2) {
    if (!we_dcvt_change_pos (obj, 4))
        return FALSE;
    }
    else if (!Pus_Hdr_cvt_short_int (obj, &(elt->_u.uint8))) {
      return FALSE;
    }
  }
  



  else if (obj->operation == WE_DCVT_DECODE) {
    if (Pus_Hdr_cvt_short_int (obj, &(elt->_u.uint8))) 
      elt->utype = PUS_UTYPE_WE_UINT8; 
    else if (obj->data[obj->pos] <= 31) {
      if (!we_dcvt_uint8 (obj, &length8))
        return FALSE;
      length32 = (WE_UINT32)length8;    
      if (length32 == PUS_HDR_CVT_LENGTH_QUOTE) { 
        if (!we_dcvt_uintvar (obj, &length32))
          return FALSE;
      }
      if (!we_dcvt_change_pos (obj, length32))
        return FALSE;     
    } 
    else if (Pus_Hdr_cvt_string (obj, &(elt->_u.str)))
        elt->utype = PUS_UTYPE_STRING;
    else 
      return FALSE;
  }
  else
    return FALSE;
  return TRUE;
}

static int
Pus_Hdr_cvt_quoted_string (we_dcvt_t* obj, char** p)
{
  WE_UINT8 temp;

  if ((obj == NULL) || (*p == NULL)) 
    return FALSE;

  switch (obj->operation) {
  case WE_DCVT_DECODE: 
    if (!we_dcvt_uint8 (obj, &temp))
      return FALSE;
    if (temp == PUS_HDR_CVT_STRING_QUOTE) {
      if (!Pus_Hdr_cvt_string (obj, p))
        return FALSE;
    }
    else 
      return FALSE;      
    break;

  case WE_DCVT_ENCODE:
    WE_DCVT_PUT_BYTE(obj, PUS_HDR_CVT_STRING_QUOTE);
    if (!Pus_Hdr_cvt_string (obj, p))
      return FALSE;
    break;

  case WE_DCVT_ENCODE_SIZE:
    if (!we_dcvt_change_pos (obj, strlen(*p) + 2)) 
      return FALSE;
    break;

  default:
    return FALSE;
  }
  return TRUE;
}



static int 
Pus_Hdr_cvt_retry_after (we_dcvt_t *obj, Pus_Hdr_element_t *elt, int encoding_version)
{
  WE_UINT32 temp = 0;
  WE_UINT8  tempcopy = 0;

  if ((obj == NULL) || (elt == NULL))
    return FALSE;

  encoding_version = encoding_version; 

  if (obj->operation != WE_DCVT_DECODE) 
    return FALSE;  
  if (!we_dcvt_uint8 (obj, &tempcopy))
      return FALSE;  
  if (tempcopy == PUS_HDR_CVT_LENGTH_QUOTE) { 
    if (!we_dcvt_uintvar (obj, &temp))
      return FALSE;
  }
  

  if (!we_dcvt_uint8 (obj, &tempcopy))
    return FALSE;
  if (tempcopy == PUS_HDR_CVT_ABSOLUTE_TIME) {
    if (!Pus_Hdr_cvt_long_int (obj, &(elt->_u.uint32)))
      return FALSE;
    else
      elt->utype = PUS_UTYPE_DATE;
  }
  else if (tempcopy == PUS_HDR_CVT_RELATIVE_TIME) {
    if (!Pus_Hdr_cvt_int_val (obj, &(elt->_u.uint32)))
      return FALSE;
    else
      elt->utype = PUS_UTYPE_WE_UINT32;
  }
  else 
    return FALSE;
  return TRUE;
}







static int
Pus_Hdr_cvt_content_md5 (we_dcvt_t *obj, Pus_Hdr_element_t *elt, int encoding_version)
{
  WE_UINT8  length8;
  WE_UINT32 length32;
  int    len, slen;

  if ((obj == NULL) || (elt == NULL))
    return FALSE;

  encoding_version = encoding_version; 

  switch (obj->operation) {
  case WE_DCVT_DECODE:
    
    if (!we_dcvt_uint8 (obj, &length8))
      return FALSE;
    length32 = (WE_UINT32)length8;    
    if (length32 == PUS_HDR_CVT_LENGTH_QUOTE) { 
      if (!we_dcvt_uintvar (obj, &length32))
        return FALSE;
    }
    if (length32 != 16) {
      return FALSE;
    }
    len = we_cmmn_base64_encode_len (16) + 1;
    elt->_u.str = PUS_MEM_ALLOC (len);
    if ((we_cmmn_base64_encode ((const char *)(obj->data + obj->pos), 16,
                            elt->_u.str, &len) < 0) ||
        !we_dcvt_change_pos (obj, 16)) {
      PUS_MEM_FREE (elt->_u.str);
      return FALSE;
    }
    elt->extras = NULL;
    elt->utype = PUS_UTYPE_STRING;
    break;

  case WE_DCVT_ENCODE:
    WE_DCVT_PUT_BYTE (obj, 16);
    len = WE_DCVT_REM_LENGTH (obj);
    slen = (elt->_u.str != NULL ? strlen (elt->_u.str) : 0);
    if ((we_cmmn_base64_decode (elt->_u.str, slen,
                             (char *)(obj->data + obj->pos), &len) < 0) ||
        !we_dcvt_change_pos (obj, len)) {
      return FALSE;
    }
    break;

  case WE_DCVT_ENCODE_SIZE:
    slen = (elt->_u.str != NULL ? strlen (elt->_u.str) : 0);
    len = we_cmmn_base64_decode_len (elt->_u.str, slen);
    if (!we_dcvt_change_pos (obj, 1 + len)) {
        
      return FALSE;
    }
    break;

  default:
    return FALSE;
  }
  return TRUE;
}




static int
Pus_Hdr_cvt_cachecontrol (we_dcvt_t* obj, Pus_Hdr_cachecontrol_t* p)
{
  WE_UINT8 temp = 0;
  WE_UINT32 length = 0;

  if ((obj == NULL) || (p == NULL))
    return FALSE;
  switch (obj->operation) {
  case WE_DCVT_DECODE:
    if (!we_dcvt_uint8 (obj, &temp))
      return FALSE;
    switch (temp) {
    case  PUS_CC_NO_CACHE:
      p->flags |= PUS_NO_CACHE_FLAG;
      break;

    case  PUS_CC_NO_STORE: 
      p->flags |= PUS_NO_STORE_FLAG;
      break;

    case  PUS_CC_MAX_STALE: 
      p->flags |= PUS_MAX_STALE_FLAG;
      break;

    case  PUS_CC_ONLY_IF_CACHED:
      p->flags |= PUS_ONLY_IF_CACHED_FLAG;
      break;

    case  PUS_CC_PRIVATE: 
      p->flags |= PUS_PRIVATE_FLAG;
      break;

    case  PUS_CC_PUBLIC:
      p->flags |= PUS_PUBLIC_FLAG;
      break;

    case  PUS_CC_NO_TRANSFORM: 
      p->flags |= PUS_NO_TRANSFORM_FLAG;
      break;

    case  PUS_CC_MUST_REVALIDATE: 
      p->flags |= PUS_MUST_REVALIDATE_FLAG;
      break;

    case  PUS_CC_PROXY_REVALIDATE: 
      p->flags |= PUS_PROXY_REVALIDATE_FLAG;
      break;

    default:      
      if ((temp >= 32) && (temp <= 127)) { 
        if (!we_dcvt_change_pos (obj, Pus_Hdr_cvt_string_length (obj)))
          return FALSE;
      }
      
      else if (temp <= 31) {
        if (temp == PUS_HDR_CVT_LENGTH_QUOTE) {
          if (!we_dcvt_uintvar (obj, &length))
            return FALSE;
        }
        else
         length = temp;
        if (!we_dcvt_uint8 (obj, &temp))
          return FALSE;
        switch (temp) {
        case PUS_CC_NO_CACHE:
          p->flags |= PUS_NO_CACHE_FLAG;
          if (!we_dcvt_change_pos (obj, (length - 1)))
            return FALSE;
          break;

        case PUS_CC_MAX_AGE:
          p->flags |= PUS_MAX_AGE_FLAG;
          if (!Pus_Hdr_cvt_int_val (obj, &(p->secage)))
            return FALSE;
          break;

        case PUS_CC_MAX_STALE:
          p->flags |= PUS_MAX_STALE_FLAG;
          if (!Pus_Hdr_cvt_int_val (obj, &(p->secstale)))
            return FALSE;
          break;

        case PUS_CC_MIN_FRESH:
          p->flags |= PUS_MIN_FRESH_FLAG;
          if (!Pus_Hdr_cvt_int_val (obj, &(p->secfresh)))
            return FALSE;
          break;

        case PUS_CC_PRIVATE:
          p->flags |= PUS_PRIVATE_FLAG;
          if (!we_dcvt_change_pos (obj, (length - 1)))
            return FALSE;
          break;

        case PUS_CC_S_MAXAGE:
          p->flags |= PUS_S_MAX_AGE_FLAG;
          if (!Pus_Hdr_cvt_int_val (obj, &(p->secS_maxage)))
            return FALSE;
          break;

        default:          
          if ((temp >= 32) && (temp <= 127)) { 
            if (!we_dcvt_change_pos (obj, (length - 1))) 
              return FALSE;
          }
          else
            return FALSE;
        }
      }
      else 
        return FALSE;       
    }
    break;

  case WE_DCVT_ENCODE_SIZE:
  case WE_DCVT_ENCODE: {
    
    WE_UINT32 delta_sec_val;
    WE_UINT32 value_length;

    value_length = 0;
    if ((delta_sec_val = p->secage) == 0xffffffff)
      if ((delta_sec_val = p->secfresh) == 0xffffffff)
        if ((delta_sec_val = p->secstale) == 0xffffffff)
          if ((delta_sec_val = p->secS_maxage) == 0xffffffff)
            delta_sec_val = 0xffffffff;

    
    if (delta_sec_val != 0xffffffff) {
      value_length = Pus_Hdr_cvt_uint32_len (delta_sec_val);
      if (value_length != 1)
        value_length++; 
      value_length++;
    }

    if (obj->operation == WE_DCVT_ENCODE_SIZE) {
      if (delta_sec_val == 0xffffffff) { 
        if (!we_dcvt_change_pos (obj, 1)) 
          return FALSE;
      }
      else { 
        if (!we_dcvt_change_pos (obj, value_length + 1)) 
          return FALSE;
        if (value_length > 30) {
          if (!we_dcvt_change_pos (obj, we_dcvt_uintvar_len (value_length))) 
            return FALSE;
        }
      }
            
    }
    else if (obj->operation == WE_DCVT_ENCODE) {
      if (delta_sec_val != 0xffffffff) { 
        if (value_length < 31) 
          WE_DCVT_PUT_BYTE(obj, value_length);
        else {
          WE_DCVT_PUT_BYTE(obj, PUS_HDR_CVT_LENGTH_QUOTE);
          if (!we_dcvt_uintvar (obj, &value_length))
            return FALSE;
        }

        if (p->flags == PUS_MAX_AGE_FLAG)
          WE_DCVT_PUT_BYTE(obj, PUS_CC_MAX_AGE);
        else if (p->flags == PUS_MAX_STALE_FLAG)
          WE_DCVT_PUT_BYTE(obj, PUS_CC_MAX_STALE);
        else if (p->flags == PUS_MIN_FRESH_FLAG)
          WE_DCVT_PUT_BYTE(obj, PUS_CC_MIN_FRESH);
        else if (p->flags == PUS_S_MAX_AGE_FLAG)
          WE_DCVT_PUT_BYTE(obj, PUS_CC_S_MAXAGE);
        else 
          return FALSE;

        if (!Pus_Hdr_cvt_int_val (obj, &delta_sec_val))
          return FALSE;
      }
      else { 
        switch (p->flags) {
        case PUS_NO_CACHE_FLAG:
          WE_DCVT_PUT_BYTE(obj, PUS_CC_NO_CACHE);
          break;
        case PUS_NO_STORE_FLAG:
          WE_DCVT_PUT_BYTE(obj, PUS_CC_NO_STORE);
          break;
        case PUS_MAX_AGE_FLAG:
          WE_DCVT_PUT_BYTE(obj, PUS_CC_MAX_AGE);
          break;
        case PUS_MAX_STALE_FLAG:
          WE_DCVT_PUT_BYTE(obj, PUS_CC_MAX_STALE);
          break;
        case PUS_MIN_FRESH_FLAG:
          WE_DCVT_PUT_BYTE(obj, PUS_CC_MIN_FRESH);
          break;
        case PUS_NO_TRANSFORM_FLAG:
          WE_DCVT_PUT_BYTE(obj, PUS_CC_ONLY_IF_CACHED);
          break;
        case PUS_ONLY_IF_CACHED_FLAG:
          WE_DCVT_PUT_BYTE(obj, PUS_CC_PUBLIC);
          break;
        case PUS_PUBLIC_FLAG:
          WE_DCVT_PUT_BYTE(obj, PUS_CC_PRIVATE);
          break;
        case PUS_PRIVATE_FLAG:
          WE_DCVT_PUT_BYTE(obj, PUS_CC_NO_TRANSFORM);
          break;
        case PUS_MUST_REVALIDATE_FLAG:
          WE_DCVT_PUT_BYTE(obj, PUS_CC_MUST_REVALIDATE);
          break;
        case PUS_PROXY_REVALIDATE_FLAG:
          WE_DCVT_PUT_BYTE(obj, PUS_CC_PROXY_REVALIDATE);
          break;
        case PUS_S_MAX_AGE_FLAG:
          WE_DCVT_PUT_BYTE(obj, PUS_CC_S_MAXAGE);
          break;
        default:
          return FALSE;
        }
      }

    }
    }
    break;

    default:
      return FALSE;
  }  
  return TRUE;
}

static int
Pus_Hdr_cvt_uint32_len (WE_UINT32 num)
{
  if (num <= 0xff) 
    return 1;
  else if (num <= 0xffff)
    return 2;
  else if (num <= 0xffffff)
    return 3;
  else  
    return 4;
}




static int
Pus_Hdr_encode_decode_int (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version)
{
  encoding_version = encoding_version; 
  if (!Pus_Hdr_cvt_int_val (obj, &(elt->_u.uint32)))
    return FALSE;
  if (obj->operation == WE_DCVT_DECODE)
    elt->utype = PUS_UTYPE_WE_UINT32; 
  return TRUE;
}

static int
Pus_Hdr_encode_decode_long_int (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version)
{
  encoding_version = encoding_version; 
  if (!Pus_Hdr_cvt_long_int (obj, &(elt->_u.uint32)))
    return FALSE;
  if (obj->operation == WE_DCVT_DECODE)
    elt->utype = PUS_UTYPE_DATE; 
  return TRUE;

}




static int
Pus_Hdr_cvt_contenttype (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version)
{
  int           oldpos;
  WE_UINT8     temp;
  WE_UINT32    temp32;
  int           length;
  WE_UINT32    castcopy1;
  WE_UINT8     castcopy2;
  int           hash_result;
  WE_UINT32    value_length;
  int           params_length = 0;
  int           content_type_length = 0;
  int           back_to_string = FALSE;
  char         *string;
  
  if ((obj == NULL) || (elt == NULL))
    return FALSE;

  switch (obj->operation) {
  case WE_DCVT_DECODE:
    elt->extras = NULL;
    if (!we_dcvt_uint8 (obj, &temp))
      return FALSE;

    if (temp >= 128) {      
      if ((!we_dcvt_change_pos (obj, -1)) ||
          (!Pus_Hdr_cvt_short_int (obj, &temp)))
        return FALSE;
      elt->_u.uint8 = temp;
      elt->utype = PUS_UTYPE_WE_UINT8;
    }
    else if ((temp >= 32) && (temp <= 127)) {
      if ((!we_dcvt_change_pos (obj, -1)) ||
          (!Pus_Hdr_cvt_string (obj, &(elt->_u.str))))
        return FALSE;
      elt->utype = PUS_UTYPE_STRING;
    }
    else { 
      if (temp == 31) {
        if (!we_dcvt_uintvar (obj, &castcopy1))
          return FALSE;
        temp = (WE_UINT8)castcopy1;
      }
      length = temp;
      oldpos = obj->pos; 

      
      if (Pus_Hdr_cvt_long_int (obj, &temp32)) {
        elt->_u.uint32 = temp32;
        elt->utype = PUS_UTYPE_WE_UINT32;
      }
      else if (Pus_Hdr_cvt_short_int (obj, &temp)) {
        elt->_u.uint32 = (WE_UINT32)temp;
        elt->utype = PUS_UTYPE_WE_UINT32;
      }
      else if (Pus_Hdr_cvt_string (obj, &(elt->_u.str))) {
        elt->utype = PUS_UTYPE_STRING;
      }
      else {
        return FALSE;
      }

      params_length = length - (obj->pos - oldpos);
      if (params_length > 0) { 
        if (!Pus_Hdr_cvt_parameters (obj, &params_length, elt)) 
          return FALSE; 
      }
    }
    break;
  
  case WE_DCVT_ENCODE:
  case WE_DCVT_ENCODE_SIZE:
    







    hash_result = 0;
    value_length = 0;
    if (elt->extras != NULL) {
      if (obj->operation == WE_DCVT_ENCODE) {
       obj->operation = WE_DCVT_ENCODE_SIZE;
       if (!Pus_Hdr_cvt_parameters (obj, &params_length, elt))   
         return FALSE;
       obj->operation = WE_DCVT_ENCODE;
      }
      else {
        if (!Pus_Hdr_cvt_parameters (obj, &params_length, elt))   
          return FALSE;
      }
      content_type_length = params_length;
    }
    if (elt->utype == PUS_UTYPE_WE_UINT8) {
      if (elt->_u.uint8 >= WE_MIME_TYPE_APPLICATION_X_WAP_PROV_BROWSER_SETTINGS)
        back_to_string = TRUE;
      else if ((encoding_version == ENCODING_VERSION_1_2) && 
        (elt->_u.uint8 > WE_MIME_TYPE_APPLICATION_VND_WAP_SIA)) {
        back_to_string = TRUE;
      }
      else if ((encoding_version == ENCODING_VERSION_1_3) && 
        (elt->_u.uint8 > WE_MIME_TYPE_APPLICATION_VND_WAP_CONNECTIVITY_WBXML)) {
        back_to_string = TRUE;
      }

      if (back_to_string) {
        content_type_length += strlen (we_cmmn_int2str ((int)elt->_u.uint8,
                                                         &we_mime_types)) + 1;
      }
      else 
        content_type_length++; 
    }
    else if (elt->utype == PUS_UTYPE_WE_UINT32) {
      if (elt->_u.uint32 >= WE_MIME_TYPE_APPLICATION_X_WAP_PROV_BROWSER_SETTINGS)
        back_to_string = TRUE;
      else if ((encoding_version == ENCODING_VERSION_1_2) && 
        (elt->_u.uint32 > WE_MIME_TYPE_APPLICATION_VND_WAP_SIA)) {
      
        back_to_string = TRUE;
      }
      else if ((encoding_version == ENCODING_VERSION_1_3) && 
        (elt->_u.uint32 > WE_MIME_TYPE_APPLICATION_VND_WAP_CONNECTIVITY_WBXML)) {

        back_to_string = TRUE;
      }
      if (back_to_string)
        content_type_length += strlen (we_cmmn_int2str ((int)elt->_u.uint32,
                                                         &we_mime_types)) + 1;
      else 
        content_type_length++; 
    }
    else if (elt->utype == PUS_UTYPE_STRING || elt->utype == PUS_UTYPE_STRING_LIT) {
      hash_result = we_cmmn_str2int (elt->_u.str, strlen(elt->_u.str), &we_mime_types);
      if (hash_result >= WE_MIME_TYPE_APPLICATION_X_WAP_PROV_BROWSER_SETTINGS) {
        hash_result = -1; 
      }
      else if ((encoding_version == ENCODING_VERSION_1_2) && 
               (hash_result > WE_MIME_TYPE_APPLICATION_VND_WAP_SIA)) {
        hash_result = -1; 
      }
      else if ((encoding_version == ENCODING_VERSION_1_3) && 
               (hash_result > WE_MIME_TYPE_APPLICATION_VND_WAP_CONNECTIVITY_WBXML)) {
        hash_result = -1; 
      }
      if (hash_result != -1) 
        content_type_length++;
      else 
        content_type_length += strlen (elt->_u.str) + 1;
    }
    if (params_length != 0) {
      value_length = content_type_length;
      if (value_length < 31)
        content_type_length++; 
      else
        content_type_length += 1 + we_dcvt_uintvar_len (value_length);
    }

    if (obj->operation == WE_DCVT_ENCODE_SIZE) {
      if (!we_dcvt_change_pos (obj, content_type_length))
        return FALSE;
    }
    else { 
      if (params_length != 0) {
        if (value_length < 31) 
          WE_DCVT_PUT_BYTE(obj, value_length); 
        else {
          WE_DCVT_PUT_BYTE(obj, PUS_HDR_CVT_LENGTH_QUOTE); 
          if (!we_dcvt_uintvar (obj, &value_length))
            return FALSE;
        }
      }
      if (elt->utype == PUS_UTYPE_WE_UINT8) {
        if (back_to_string) {
          string = we_cmmn_strdup (WE_MODID_PUS,
                                    we_cmmn_int2str ((int)elt->_u.uint8,
                                                      &we_mime_types));
          if (!Pus_Hdr_cvt_string (obj, &string)) {
            PUS_MEM_FREE (string);
            return FALSE;
          }
          PUS_MEM_FREE (string);
        }
        else {
          if (!Pus_Hdr_cvt_short_int (obj, &(elt->_u.uint8)))
            return FALSE;
        }
      }
      else if (elt->utype == PUS_UTYPE_WE_UINT32) {
        if (back_to_string) {
          string = we_cmmn_strdup (WE_MODID_PUS,
                                    we_cmmn_int2str ((int)elt->_u.uint32,
                                                      &we_mime_types));
          if (!Pus_Hdr_cvt_string (obj, &string)) {
            PUS_MEM_FREE (string);
            return FALSE;
          }
          PUS_MEM_FREE (string);
        }
        else {
          castcopy2 = (WE_UINT8) elt->_u.uint32;
          if (!Pus_Hdr_cvt_short_int (obj, &castcopy2))
            return FALSE;
        }
      }
      else if (elt->utype == PUS_UTYPE_STRING || elt->utype == PUS_UTYPE_STRING_LIT) {
        if (hash_result != -1) { 
          castcopy2 = (WE_UINT8) hash_result;
          if (!Pus_Hdr_cvt_short_int (obj, &castcopy2))
            return FALSE;
        }
        else {
          if (!Pus_Hdr_cvt_string (obj, &(elt->_u.str)))
            return FALSE;
        }
      }
      if (params_length != 0) {
        if (!Pus_Hdr_cvt_parameters (obj, &params_length, elt))   
          return FALSE;
      }
    } 
    break;

  default:
    return FALSE;  
  }
  return TRUE;
}

static int
Pus_Hdr_encode_decode_string (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version)
{
  encoding_version = encoding_version; 
  if (!Pus_Hdr_cvt_string (obj, &(elt->_u.str)))
    return FALSE;
  if (obj->operation == WE_DCVT_DECODE)
    elt->utype = PUS_UTYPE_STRING; 
  return TRUE;
}




static int
Pus_Hdr_cvt_parameters (we_dcvt_t* obj, int* paramslength, Pus_Hdr_element_t *elt)
{
  Pus_Hdr_params_t* param_struct;
  char*         str_value = NULL;
  WE_UINT8         tempArray[2];
  WE_UINT32        uint32_copy = 0;
  int           charset_value = 0;
  int           params_len = 0;
  int           params_len_count = 0;
  int           pos_before = 0;
  int           pos_after = 0;
  WE_UINT8         temp = 0;
  WE_UINT8         uint8_copy = 0;
  char          buf[8];
  
  if ((obj == NULL) || (elt == NULL)) 
    return FALSE;
  switch(obj->operation) {
  case WE_DCVT_DECODE:
    while (params_len_count < *paramslength) {
      if (Pus_Hdr_cvt_short_int (obj, &temp)) { 
        params_len_count++;
        pos_before = obj->pos;
        switch (temp) {
        case PUS_PARAM_CHARSET:
          if (!Pus_Hdr_cvt_int_val (obj, &uint32_copy))
            return FALSE;
          else {
            const char* s = we_charset_int2str (uint32_copy);
            if (s != NULL) {
              Pus_Hdr_add_parameter_to_element (elt, "charset", s);
            }
          }
          break;

        case PUS_PARAM_LEVEL:
          if (!Pus_Hdr_cvt_short_int (obj, &uint8_copy)) {
            if (!Pus_Hdr_cvt_string  (obj, &str_value))
              return FALSE;
          }
          else {
            Pus_Hdr_cvt_levelint_to_str (uint8_copy, str_value); 
          }
          Pus_Hdr_add_parameter_to_element (elt, "level", str_value);
          PUS_MEM_FREE (str_value);
          break;

        case PUS_PARAM_DIFFERENCES:
          if (!Pus_Hdr_cvt_short_int (obj, &uint8_copy)) {
            if (!Pus_Hdr_cvt_string (obj, &str_value))
              return FALSE;
          }
          else {
            
          }  
          if (str_value != NULL)
            Pus_Hdr_add_parameter_to_element (elt, "differences", str_value); 
          PUS_MEM_FREE (str_value);
          break;

        case PUS_PARAM_SECURE:
          if (!we_dcvt_change_pos (obj, 1))
            return FALSE;
          Pus_Hdr_add_parameter_to_element (elt, "secure", NULL);
          break;

        case PUS_PARAM_Q:
          if (!we_dcvt_uintvar (obj, &uint32_copy))
            return FALSE;
          

          break;

        case PUS_PARAM_SIZE:
        case PUS_PARAM_TYPE:
        case PUS_PARAM_MAX_AGE:
          if (!Pus_Hdr_cvt_int_val (obj, &uint32_copy))
            return FALSE;
          Pus_Hdr_uint322string (uint32_copy, &str_value);
          Pus_Hdr_add_parameter_to_element (elt, we_cmmn_int2str (temp, &pus_params),
                                        str_value);
          PUS_MEM_FREE (str_value);
          break;

        case PUS_PARAM_FILE_NAME:
        case PUS_PARAM_NAME:
        case PUS_PARAM_START:
        case PUS_PARAM_START_INFO:
        case PUS_PARAM_COMMENT:
        case PUS_PARAM_DOMAIN:
        case PUS_PARAM_PATH:        
          if (!Pus_Hdr_cvt_string (obj, &str_value))
            return FALSE;
          Pus_Hdr_add_parameter_to_element (elt, we_cmmn_int2str (temp, &pus_params), str_value);
          PUS_MEM_FREE (str_value);
          break;

        case PUS_PARAM_CREATION_DATE:
        case PUS_PARAM_MODIFICATION_DATE:
        case PUS_PARAM_READ_DATE:
          if (!Pus_Hdr_cvt_long_int (obj, &uint32_copy))
            return FALSE;
          we_cmmn_time2str (uint32_copy, str_value);
          Pus_Hdr_add_parameter_to_element (elt, we_cmmn_int2str (temp, &pus_params), str_value);
          PUS_MEM_FREE (str_value);
          break;
                
        case PUS_PARAM_NAME_1_4:
        case PUS_PARAM_FILE_NAME_1_4:
        case PUS_PARAM_START_1_4:
        case PUS_PARAM_START_INFO_1_4:
        case PUS_PARAM_COMMENT_1_4:
        case PUS_PARAM_DOMAIN_1_4:
        case PUS_PARAM_PATH_1_4:
        case PUS_PARAM_MAC:
          if (!Pus_Hdr_cvt_text_value (obj, &str_value))
            return FALSE;
          Pus_Hdr_add_parameter_to_element (elt, we_cmmn_int2str (temp, &pus_params), str_value);
          PUS_MEM_FREE (str_value);
          break;

        case PUS_PARAM_PADDING:
        case PUS_PARAM_SEC:
          if (!Pus_Hdr_cvt_short_int (obj, &uint8_copy))
            return FALSE;
          sprintf (buf, "%u", (unsigned int)uint8_copy);
          Pus_Hdr_add_parameter_to_element (elt, we_cmmn_int2str (temp, &pus_params), buf);
          break;

        case PUS_PARAM_TYPE_MP:
          if (!Pus_Hdr_cvt_short_int (obj, &uint8_copy)) {
            if (!Pus_Hdr_cvt_string (obj, &str_value))
              return FALSE;
          }
          else {
            

          }
          if (str_value != NULL)
            Pus_Hdr_add_parameter_to_element (elt, "Type_mp", str_value);
          PUS_MEM_FREE (str_value);
          break;

        default:
          return FALSE;
        }
      }

      else {  
        pos_before = obj->pos;
        

        if (!Pus_Hdr_cvt_string (obj, &str_value))
          return FALSE;
        if (!Pus_Hdr_cvt_int_val (obj, &uint32_copy))
          if (!Pus_Hdr_cvt_text_value (obj, &str_value))
            return FALSE;
      }
      pos_after = obj->pos;
      params_len_count += pos_after-pos_before;
    }
    break;      

  case WE_DCVT_ENCODE:
  case WE_DCVT_ENCODE_SIZE:
    *paramslength = 0;
    for (param_struct = (Pus_Hdr_params_t*) elt->extras; param_struct != NULL; 
         param_struct = param_struct->next) {
      if (!strcmp (param_struct->name, "charset")) {
        charset_value = we_charset_str2int (param_struct->value, 
                                             strlen (param_struct->value));
        if (charset_value != -1) {
          if (charset_value >= 128 ) { 


            params_len = 1;      
            if (charset_value < 256 ) { 

              tempArray[0] = (WE_UINT8) (charset_value & 0xff);
              params_len++;
            }
            else {
              tempArray[0] = (WE_UINT8) ((charset_value >> 8) & 0xff);
              tempArray[1] = (WE_UINT8) (charset_value & 0xff);
              params_len += 2;
            }
          }
          else if (charset_value > -1 )   
            params_len = 1; 
          else 
            return FALSE;
        }    
        else 
          params_len = strlen (param_struct->value) + 1;
      }
      else
        params_len = strlen (param_struct->value) + 1;
        
      if (obj->operation == WE_DCVT_ENCODE_SIZE) {      			
        if (!strcmp (param_struct->name, "charset"))
          *paramslength += 1 + params_len; 
        else
          *paramslength += strlen (param_struct->name) + 1 + params_len;
      }
      else {  
        if (!strcmp (param_struct->name, "charset")) {           
          WE_DCVT_PUT_BYTE(obj, PUS_PARAM_CHARSET | 0x80);       
          if (charset_value == -1) { 
            if (!Pus_Hdr_cvt_string (obj, &(param_struct->value)))
              return FALSE;
          }
          else if (params_len == 1) {  
            WE_DCVT_PUT_BYTE(obj, (WE_UINT8)(charset_value) | 0x80); 

          }
          else {      
           WE_DCVT_PUT_BYTE(obj, (WE_UINT8)(params_len - 1));
           WE_DCVT_PUT_BYTE(obj, tempArray[0]);		
           if (params_len == 3) 
             WE_DCVT_PUT_BYTE(obj, tempArray[1]);
          }
        }
        else { 
          if (!Pus_Hdr_cvt_string (obj, &(param_struct->name)) || 
              !Pus_Hdr_cvt_string (obj, &(param_struct->value)))
            return FALSE;          
        } 
        *paramslength = params_len;
      }
    }
    break;     

    default:
      return FALSE;
  }
    return TRUE;
}




static int 
Pus_Hdr_cvt_text_value (we_dcvt_t* obj, char** p) 
{

  WE_UINT8 temp;

  if ((obj == NULL) || (p == NULL)) 
    return FALSE;

  switch (obj->operation) {
  case WE_DCVT_DECODE: 
    if(!we_dcvt_uint8 (obj, &temp))
      return FALSE;
    if (temp == PUS_HDR_CVT_STRING_QUOTE) {
      
      if(!Pus_Hdr_cvt_string (obj, p))
        return FALSE;
    }
    else if (temp == PUS_HDR_CVT_NO_VALUE) { 
      
      *p = NULL;
    }    
    else if (temp <= 127) {
      
      if (!we_dcvt_change_pos (obj, -1) || !Pus_Hdr_cvt_string (obj, p))
        return FALSE;
    }
    else 
      return FALSE;
    break;

  case WE_DCVT_ENCODE:
    
    break;

  case WE_DCVT_ENCODE_SIZE:
    
    break;

  default:
    return FALSE;
  }
  return TRUE;
}




static int
Pus_Hdr_cvt_content_disposition (we_dcvt_t *obj, Pus_Hdr_element_t *elt, int encoding_version)
{
  WE_UINT8  length8;
  WE_UINT32 length32;
  int    oldpos;
  int    params_length = 0;
  int    content_disp_length = 0;
  WE_UINT32 value_length;

  if ((obj == NULL) || (elt == NULL)) 
    return FALSE;

  encoding_version = encoding_version; 

  switch (obj->operation) {
  case WE_DCVT_DECODE: 
    if (!we_dcvt_uint8 (obj, &length8))
      return FALSE;
    length32 = (WE_UINT32) length8;    
    if (length32 == PUS_HDR_CVT_LENGTH_QUOTE) { 
      if (!we_dcvt_uintvar (obj, &length32))
        return FALSE;
    }
    oldpos = obj->pos;
    if (Pus_Hdr_cvt_short_int (obj, &(elt->_u.uint8)))
      elt->utype = PUS_UTYPE_WE_UINT8;
    else if (Pus_Hdr_cvt_string (obj, &(elt->_u.str)))
      elt->utype = PUS_UTYPE_STRING;
    else 
      return FALSE;
    params_length = length32 - (obj->pos - oldpos);
    if (params_length > 0) { 
      if (!Pus_Hdr_cvt_parameters (obj, &params_length, elt)) 
        return FALSE; 
    }
    break;

  case WE_DCVT_ENCODE:    
  case WE_DCVT_ENCODE_SIZE:
    

    if (elt->extras != NULL) {
      if (obj->operation == WE_DCVT_ENCODE) {
       obj->operation = WE_DCVT_ENCODE_SIZE;
       if (!Pus_Hdr_cvt_parameters (obj, &params_length, elt))   
         return FALSE;
       obj->operation = WE_DCVT_ENCODE;
      }
      else {
        if (!Pus_Hdr_cvt_parameters (obj, &params_length, elt))   
          return FALSE;
      }
      content_disp_length = params_length;
    }
    if (elt->utype == PUS_UTYPE_WE_UINT8 || elt->utype == PUS_UTYPE_WE_UINT32) {
      content_disp_length++;     
    }
    else if (elt->utype == PUS_UTYPE_STRING || elt->utype == PUS_UTYPE_STRING_LIT) {
      content_disp_length += 1 + strlen (elt->_u.str);
    }
    value_length = content_disp_length;
    if (value_length < 31)
      content_disp_length++; 
    else 
      content_disp_length += 1 + we_dcvt_uintvar_len (value_length);
    
    if (obj->operation == WE_DCVT_ENCODE_SIZE) {
      if (!we_dcvt_change_pos (obj, content_disp_length))
        return FALSE;
    }
    else { 
      if (value_length < 31) 
        WE_DCVT_PUT_BYTE(obj, value_length); 
      else {
        WE_DCVT_PUT_BYTE(obj, PUS_HDR_CVT_LENGTH_QUOTE); 
        if (!we_dcvt_uintvar (obj, &value_length))
          return FALSE;
      }
      if (elt->utype == PUS_UTYPE_WE_UINT8) {
        if (!Pus_Hdr_cvt_short_int (obj, &(elt->_u.uint8)))
          return FALSE;
      }
      else if (elt->utype == PUS_UTYPE_WE_UINT32) {
        length8 = (WE_UINT8) elt->_u.uint32;
        if (!Pus_Hdr_cvt_short_int (obj, &length8))
          return FALSE;
      }
      else if (elt->utype == PUS_UTYPE_STRING || elt->utype == PUS_UTYPE_STRING_LIT) {
        if (!Pus_Hdr_cvt_string (obj, &(elt->_u.str)))
          return FALSE;        
      }
      if (params_length != 0) {
        if (!Pus_Hdr_cvt_parameters (obj, &params_length, elt))   
          return FALSE;
      }
    } 
    break;

  default:
    return FALSE;
  }
  return TRUE;
}




static int
Pus_Hdr_cvt_cookie (we_dcvt_t *obj, Pus_Hdr_element_t *elt, int encoding_version)
{  
  WE_UINT32  value_length;
  WE_UINT8   length8;
  int     old_pos;
  WE_UINT32  length32;
  int     length_cookie_string;
  WE_UINT8   version_value;
  char   *version_string; 
  char   *name;
  char   *value;
  int     params_length;
  int     i; 
  char   *pos_in_string;
  char   *string;
  char       *tmp_cookie;
  we_dcvt_t  tmp_obj;
  int         tmp_pos;
  
  if ((obj == NULL) || (elt == NULL))
    return FALSE;

  encoding_version = encoding_version; 

  switch (obj->operation) {
  case WE_DCVT_DECODE:
    
    if (!we_dcvt_uint8 (obj, &length8))
      return FALSE;
    length32 = (WE_UINT32) length8;
    if (length32 == 0) {
      elt->_u.str = we_cmmn_strdup (WE_MODID_PUS, "");
      elt->utype = PUS_UTYPE_STRING;
      return TRUE;
    }
    if (length32 == PUS_HDR_CVT_LENGTH_QUOTE) { 
      if (!we_dcvt_uintvar (obj, &length32))
        return FALSE;
    }
    old_pos = obj->pos;
    
    if (!Pus_Hdr_cvt_short_int (obj, &version_value)) {
      if (!Pus_Hdr_cvt_string (obj, &version_string))
        return FALSE;
    }
    else { 
      version_string = NULL;
      if (!Pus_Hdr_cvt_version_value (&version_string, &version_value))
        return FALSE;
    }
    
    if (!Pus_Hdr_cvt_string (obj, &name))
      return FALSE;
    
    if (!Pus_Hdr_cvt_string (obj, &value))
        return FALSE;
    params_length = length32 - (obj->pos - old_pos );
     
    Pus_Hdr_cvt_parameters (obj, &params_length, elt);
    
    
    length_cookie_string = strlen (name) + strlen (value) + 2; 
    length_cookie_string += 10;
    length_cookie_string += pus_length_relevant_parameters ((Pus_Hdr_params_t*) elt->extras);
    
    string = PUS_MEM_ALLOC (length_cookie_string + 20);
    
    
    
    if ((value[0]=='"') && (value[strlen(value)-1]!='"'))    
    {
      i = sprintf (string, "%s=%s\";Version=%s;", name, value, version_string);
      length_cookie_string++;
    }
    else
      i = sprintf (string, "%s=%s;Version=%s;", name, value, version_string);
    
    pos_in_string = string + i;
    pus_add_relevant_parameters ((Pus_Hdr_params_t*) elt->extras, &pos_in_string);
    string[length_cookie_string - 1] = '\0'; 
    elt->_u.str = string;
    elt->utype = PUS_UTYPE_STRING;
    PUS_MEM_FREE (name);
    PUS_MEM_FREE (value);
    PUS_MEM_FREE (version_string);
    break;

  case WE_DCVT_ENCODE:
  case WE_DCVT_ENCODE_SIZE:
    
    tmp_cookie = elt->_u.str;
    tmp_obj = *obj;
    tmp_obj.operation = WE_DCVT_ENCODE_SIZE;
    tmp_pos = tmp_obj.pos;
    value_length = 0;
    version_value = 0;

    while (pus_encode_one_cookie (&tmp_obj, &tmp_cookie, &version_value)) {
      value_length += tmp_obj.pos - tmp_pos;
      tmp_pos = tmp_obj.pos;
    }
    value_length++; 

    if (obj->operation == WE_DCVT_ENCODE_SIZE) {
      if (value_length < 31) {
        if (!we_dcvt_change_pos (obj, 1 + value_length))
          return FALSE;
      }
      else {
        if (!we_dcvt_change_pos (obj, 1 + value_length +
                                  we_dcvt_uintvar_len (value_length)))
          return FALSE;
      }
    }
    else { 
      if (value_length < 31) 
        WE_DCVT_PUT_BYTE (obj, value_length);
      else {
        WE_DCVT_PUT_BYTE (obj, PUS_HDR_CVT_LENGTH_QUOTE);
        if (!we_dcvt_uintvar (obj, &value_length))
          return FALSE;
      }

      
      if (!Pus_Hdr_cvt_short_int (obj, &version_value))
        return FALSE;

      tmp_cookie = elt->_u.str;
      while (pus_encode_one_cookie (obj, &tmp_cookie, NULL));
    }
    break;

  default:
    return FALSE;
  }
  return TRUE;
}





static int
Pus_Hdr_cvt_profilewarning (we_dcvt_t *obj, Pus_Hdr_element_t *elt, int encoding_version)
{
  WE_UINT8   temp8;
  WE_UINT8   length8;
  WE_UINT32  date;  
  WE_UINT32  length32;
  int     old_pos;
  char    warn_code[4];
  char   *warn_target;
  char   *date_string = NULL;
  char   *result;
  int     i;
  int     dates_length;

  encoding_version = encoding_version; 

  if ((obj == NULL) || (elt == NULL))
    return FALSE;
  if (obj->operation != WE_DCVT_DECODE) 
    return FALSE;  
  
  if (Pus_Hdr_cvt_short_int (obj, &(elt->_u.uint8))) 
    elt->utype = PUS_UTYPE_WE_UINT8;
  else { 
    if (!we_dcvt_uint8 (obj, &length8))
      return FALSE;
    length32 = (WE_UINT32) length8;    
    if (length32 == PUS_HDR_CVT_LENGTH_QUOTE) { 
      if (!we_dcvt_uintvar (obj, &length32))
        return FALSE;
    }
    old_pos = obj->pos;
    if (Pus_Hdr_cvt_short_int (obj, &temp8)) 
      return FALSE;
    

    if (temp8 & 0x10) {
      if (temp8 & 0x01) 
        sprintf (warn_code, "%d",101);
      else if (temp8 & 0x02)
        sprintf (warn_code, "%d",102);
      else
        sprintf (warn_code, "%d",100);
    }
    else {
      if (temp8 & 0x01) {
        if (temp8 & 0x02)
          sprintf (warn_code, "%d",203);
        else 
          sprintf (warn_code, "%d",201);
      }
      else if (temp8 & 0x02)
        sprintf (warn_code, "%d",202);
      else
        sprintf (warn_code, "%d",200);  
    }
    
    if (!Pus_Hdr_cvt_string (obj, &warn_target))
      return FALSE;
    
    dates_length = length32 - (old_pos - obj->pos);
    if (dates_length > 0) {
      if (!Pus_Hdr_cvt_long_int (obj, &date))
        return FALSE;
      date_string = PUS_MEM_ALLOC (PUS_HDR_CVT_DATESTRING_LENGTH + 1);
      we_cmmn_time2str (date, date_string);
      dates_length = length32 - (old_pos - obj->pos);
      if (!we_dcvt_change_pos (obj, dates_length))
        return FALSE;
    }
    
    result = we_cmmn_strcat (WE_MODID_PUS, warn_code, warn_target);
    if (date_string != NULL)
      result = we_cmmn_strcat (WE_MODID_PUS, result, date_string);
    result = we_cmmn_strcat (WE_MODID_PUS, result, '\0');

    
    for (i = 0; i < (int) strlen (result); i++) {
      if (result[i] == '\0')
        result[i] = PUS_HDR_CVT_SPACE;
    }
    elt->utype = PUS_UTYPE_STRING;
    elt->_u.str = we_cmmn_strdup (WE_MODID_PUS, result);
    PUS_MEM_FREE (warn_target);
    PUS_MEM_FREE (result);
    PUS_MEM_FREE (date_string);
  }
  return TRUE;
}

static int
Pus_Hdr_cvt_version_value (char** version_string, WE_UINT8* version_value)
{
  WE_UINT32 temp_value;
  WE_UINT8  temp_major, temp_minor;
  char  *temp_string;

  if (*version_string == NULL) { 
    temp_minor = (WE_UINT8) (*version_value & 0x0f); 
    temp_major = (WE_UINT8) (*version_value & 0x70);
    temp_major = (WE_UINT8) (temp_major >> 4);
    if (temp_minor == 15)  
      Pus_Hdr_uint322string (temp_major, version_string);
    else {
      *version_string = PUS_MEM_ALLOC (Pus_Hdr_get_uint32_string_len (temp_major) + 
                                       Pus_Hdr_get_uint32_string_len (temp_minor) + 2);
      sprintf (*version_string, "%d.%d", temp_major, temp_minor);    
    }
  }
  else { 
    if (*version_string == NULL)
      return FALSE;
    if (strlen (*version_string) == 1) {
      Pus_Hdr_string2uint32 (*version_string, &temp_value);
      *version_value = (WE_UINT8) temp_value;   
      *version_value = (WE_UINT8)((*version_value) << 4);
      *version_value = (WE_UINT8)(*version_value | 0x0f);
    }
    else if (strlen (*version_string) == 3|| strlen (*version_string) == 4) {
      temp_string = we_cmmn_strdup (WE_MODID_PUS, *version_string);
      temp_string[1] = '\0';
      Pus_Hdr_string2uint32 (temp_string, &temp_value);
      temp_major = (WE_UINT8) temp_value;
      temp_string = we_cmmn_strdup (WE_MODID_PUS, (*version_string) + 2); 
      Pus_Hdr_string2uint32 (temp_string, &temp_value);
      temp_minor = (WE_UINT8) temp_value;
      *version_value = temp_major;
      *version_value = (WE_UINT8) ((*version_value) << 4);
      *version_value = (WE_UINT8) (*version_value | temp_minor);
      PUS_MEM_FREE (*version_string);
    }
    else 
      return FALSE;
  }
  return TRUE;
}

static int
Pus_Hdr_decode_authenticate (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version)
{
  encoding_version = encoding_version; 
  if (!Pus_Hdr_cvt_authenticate (obj, elt))
    return FALSE;
  elt->utype = PUS_UTYPE_STRING; 
  return TRUE;
}

static int
Pus_Hdr_encode_decode_short_int (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version)
{
  encoding_version = encoding_version; 
  if (!Pus_Hdr_cvt_short_int (obj, &(elt->_u.uint8)))
    return FALSE;
  if (obj->operation == WE_DCVT_DECODE)
    elt->utype = PUS_UTYPE_WE_UINT8; 
  return TRUE;
}

static int
Pus_Hdr_encode_decode_encoding_version (we_dcvt_t* obj, Pus_Hdr_element_t *elt,
                                        int encoding_version)
{
  if (!Pus_Hdr_cvt_encoding_version (obj, elt, encoding_version))
    return FALSE;
  return TRUE;
}

static int
Pus_Hdr_encode_decode_cache_control (we_dcvt_t* obj, Pus_Hdr_element_t *elt, 
                                     int encoding_version)
{
  encoding_version = encoding_version; 
  
  if (!Pus_Hdr_cvt_cachecontrol (obj, (Pus_Hdr_cachecontrol_t*) elt->extras)) {
    return FALSE;
  }
  return TRUE;
}


static int
Pus_Hdr_decode_text_and_int (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version)
{
  encoding_version = encoding_version; 
  if (!Pus_Hdr_cvt_int_val (obj, &(elt->_u.uint32))) {
    if (!Pus_Hdr_cvt_string (obj, &(elt->_u.str)))
      return FALSE;
    else
      elt->utype = PUS_UTYPE_STRING; 
  }
  else
    elt->utype = PUS_UTYPE_WE_UINT32; 
  return TRUE;
}


static int
Pus_Hdr_decode_text_and_short_int (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version)
{
  encoding_version = encoding_version; 
  if (!Pus_Hdr_cvt_short_int (obj, &(elt->_u.uint8))) {
    if (!Pus_Hdr_cvt_string (obj, &(elt->_u.str)))
      return FALSE;
    else
      elt->utype = PUS_UTYPE_STRING; 
  }
  else
    elt->utype = PUS_UTYPE_WE_UINT8;
  return TRUE;
}


static int
Pus_Hdr_encode_decode_quoted_string (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version)
{
  encoding_version = encoding_version; 
  if (!Pus_Hdr_cvt_quoted_string (obj, &(elt->_u.str)))
    return FALSE;
  if (obj->operation == WE_DCVT_DECODE)
    elt->utype = PUS_UTYPE_STRING;
  return TRUE;
}

static int
Pus_Hdr_decode_value_length (we_dcvt_t *obj, int *value_length)
{
  WE_UINT8 tmp8;

  *value_length = 0;

  if (!we_dcvt_uint8 (obj, &tmp8))
    return FALSE;

  if (tmp8 <= 30) {
    *value_length = (int)tmp8;
  }
  else if (tmp8 == PUS_HDR_CVT_LENGTH_QUOTE) {
    WE_UINT32 tmp32;

    if (!we_dcvt_uintvar (obj, &tmp32))
      return FALSE;
    *value_length = (int)tmp32;
  }
  else {
    we_dcvt_change_pos (obj, -1);
    return FALSE;
  }

  return TRUE;
}


static int
Pus_Hdr_decode_junk (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version)
{
  WE_UINT8 length8;
  WE_UINT32 length32;

  encoding_version = encoding_version; 
  
  if ((obj->data[obj->pos] >= 32) && (obj->data[obj->pos] <= 127))
    we_dcvt_change_pos (obj, Pus_Hdr_cvt_string_length (obj));
  else if ((obj->data[obj->pos]) & 0x80)
    we_dcvt_change_pos (obj, 1);
  else if (obj->data[obj->pos] <= 31) {
    if (!we_dcvt_uint8 (obj, &length8))
      return FALSE;
    length32 = (WE_UINT32)length8;    
    if (length32 == PUS_HDR_CVT_LENGTH_QUOTE) { 
      if (!we_dcvt_uintvar (obj, &length32))
        return FALSE;
    }
    we_dcvt_change_pos (obj, length32);
  }

  Pus_Hdr_delete_element (elt); 
  return TRUE;
}


static int
Pus_Hdr_decode_pragma (we_dcvt_t* obj, Pus_Hdr_element_t *elt, int encoding_version)
{
  int len;
  int r = TRUE;

  encoding_version = encoding_version;

  if (Pus_Hdr_cvt_short_int (obj, &(elt->_u.uint8))) {
    elt->utype = PUS_UTYPE_WE_UINT8;
  }
  else if (Pus_Hdr_decode_value_length (obj, &len)) {
    elt->_u.str = "";
    elt->utype = PUS_UTYPE_STRING_LIT;
    r = Pus_Hdr_cvt_parameters (obj, &len, elt);
  }
  else {
    r = FALSE;
  }

  return r;
}

static int
pus_encode_one_cookie (we_dcvt_t *obj, char **cookies, WE_UINT8 *version)
{
  int    len1, len2;
  char  *s;
  char  *tmp1, *tmp2, *tmp3;
  char  *start_version = NULL, *start_name = NULL, *start_value = NULL;
  char  *start_path = NULL, *start_domain = NULL;
  int    len_version = 0, len_name = 0, len_value = 0, len_path = 0, len_domain = 0;
  WE_UINT32 cookie_length;
  WE_UINT8  null_byte = 0;

  if ((cookies == NULL) || (*cookies == NULL) || (obj == NULL))
    return FALSE;

  s = *cookies;
  for (;;) {
    s = we_cmmn_skip_blanks (s);
    if (s[0] == '\0') {
      break;
    }
    tmp1 = strchr (s, '=');
    if (tmp1 == NULL) {
      break;
    }
    len1 = we_cmmn_skip_trailing_blanks (s, tmp1 - s);

    tmp2 = we_cmmn_skip_blanks (tmp1 + 1);
    tmp3 = strchr (tmp2, ';');
    if (tmp3 != NULL) {
      len2 = we_cmmn_skip_trailing_blanks (tmp2, tmp3 - tmp2);
    }
    else {
      len2 = strlen (tmp2);
      len2 = we_cmmn_skip_trailing_blanks (tmp2, len2);
    }

    if (tmp2[0] == '"') {
      if ((len2 <= 0) || (tmp2[len2 - 1] != '"')) {
        return FALSE;
      }
      tmp2++;
      len2 -= 2;
    }

    if ((len1 == 8) && (we_cmmn_strncmp_nc (s, "$version", 8) == 0)) {
      if (start_version != NULL) {
        break;
      }
      start_version = tmp2;
      len_version = len2;
    }
    else if ((len1 == 5) && (we_cmmn_strncmp_nc (s, "$path", 5)) == 0) {
      if (start_path != NULL) {
        break;
      }
      start_path = tmp2;
      len_path = len2;
    }
    else if ((len1 == 7) && (we_cmmn_strncmp_nc (s, "$domain", 7)) == 0) {
      if (start_domain != NULL) {
        break;
      }
      start_domain = tmp2;
      len_domain = len2;
    }
    else if (s[0] != '$') {
      if (start_name != NULL) {
        break;
      }
      start_name = s;
      len_name = len1;
      start_value = tmp2;
      len_value = len2;
    }

    if (tmp3 != NULL)
      s = tmp3 + 1;
    else {
      s += strlen (s);
      break;
    }
  }
  *cookies = s;
  if ((len_name == 0) || (len_value == 0)) {
    return FALSE;
  }

  cookie_length = len_name + 1 + len_value + 1;
  if ((len_path > 0) || (len_domain > 0)) {
    cookie_length += len_path + 1;
  }
  if (len_domain > 0) {
    cookie_length += len_domain + 1;
  }

  if ((version != NULL) && (start_version != NULL) && (len_version > 0)) {
    int val = atoi (start_version);
    *version = (WE_UINT8)val;
  }

  
  if (!we_dcvt_uintvar (obj, &cookie_length))
    return FALSE;  

  
  if (!we_dcvt_uchar_vector (obj, len_name, (unsigned char **)&start_name) ||
      !we_dcvt_uint8 (obj, &null_byte)) {
    return FALSE;
  }

  
  if (!we_dcvt_uchar_vector (obj, len_value, (unsigned char **)&start_value) ||
      !we_dcvt_uint8 (obj, &null_byte)) {
    return FALSE;
  }

  if ((len_domain > 0) || (len_path > 0)) {
    
    if (!we_dcvt_uchar_vector (obj, len_path, (unsigned char **)&start_path) ||
      !we_dcvt_uint8 (obj, &null_byte)) {
      return FALSE;
    }
  }

  
  if (len_domain > 0) {
    if (!we_dcvt_uchar_vector (obj, len_domain, (unsigned char **)&start_domain) ||
      !we_dcvt_uint8 (obj, &null_byte)) {
      return FALSE;
    }
  }

  return TRUE;
}

static int
pus_length_relevant_parameters (Pus_Hdr_params_t* params_list)
{
  Pus_Hdr_params_t *p;
  int params_length = 0;

    for (p = params_list; p != NULL; p = p->next) {
      if (!strcmp (p->name, "domain") || !strcmp (p->name, "path") || !strcmp (p->name, "max-age")) {
        params_length += strlen (p->name) + strlen (p->value) + 2; 
      }
    }
  return params_length;
}

static int
pus_add_relevant_parameters (Pus_Hdr_params_t* params_list, char** string)
{
  Pus_Hdr_params_t *p;
  int          i = 0; 

  for (p = params_list; p != NULL; p = p->next) {
      if (!strcmp (p->name, "domain") || !strcmp (p->name, "path") || !strcmp (p->name, "max-age")) {
        i += sprintf((*string + i), "%s=%s;", p->name, p->value);
      }
  }
  return TRUE;
}









static int
Pus_Hdr_build_textual (Pus_Hdr_table_t *table, char **headers)
{
  int len;
  
  len = Pus_Hdr_txt_calculate_length (table);
  *headers = (char*) PUS_MEM_ALLOC (len + 1);  
  
  if (len == 0) {
    (*headers)[len] = 0;
    return TRUE;
  }
  
  Pus_Hdr_txt_create_string (table, *headers);
  
  return TRUE;
}





static int
Pus_Hdr_txt_calculate_length (Pus_Hdr_table_t* table)
{
  int            len = 0;
  int            prev_field_name = -1;
  Pus_Hdr_element_t *elt;
  const char    *s;
  const char    *hname;
  int            header_type;
  int            n;
  
  for (elt = table->hdr_fields; elt != NULL; elt = elt->next) {
    header_type = elt->field_name;

    if (header_type == WE_HDR_UNKNOWN) {
      len += strlen (elt->_u.str) + 2;
      continue;
    }

    if ((header_type != prev_field_name) || (header_type == WE_HDR_COOKIE)) {
      hname = we_cmmn_int2str (header_type, &we_hdr_names);
      len += strlen (hname) + 4;
    }
    else
      len += 2;

    switch (header_type) {
    case WE_HDR_ACCEPT:
    case WE_HDR_CONTENT_TYPE:
    case WE_HDR_X_WAP_PUSH_ACCEPT:
      if ((elt->utype == PUS_UTYPE_STRING) ||
          (elt->utype == PUS_UTYPE_STRING_LIT)) {
        len += strlen (elt->_u.str);
      }
      else {
        if (elt->utype == PUS_UTYPE_WE_UINT8)
          n = elt->_u.uint8;
        else if (elt->utype == PUS_UTYPE_WE_UINT32)
          n = elt->_u.uint32;
        else
          return FALSE;
        s = we_cmmn_int2str (n, &we_mime_types);
        if (s == NULL)
          return FALSE;
        len += strlen (s);
      }
      len += pus_get_parameter_length ((Pus_Hdr_params_t*) elt->extras);
      break;

    case WE_HDR_CACHE_CONTROL:
      len += pus_get_cache_control_length ((Pus_Hdr_cachecontrol_t*) elt->extras);
      break;

    case WE_HDR_AUTHORIZATION:
    case WE_HDR_PROXY_AUTHORIZATION:
    case WE_HDR_X_WAP_AUTHORIZATION:
      if (!we_cmmn_strncmp_nc (elt->_u.str, "digest", 6)) {
        
        len += strlen (elt->_u.str);
      }
      else {
        
        len += we_cmmn_base64_encode_len (strlen (elt->_u.str)) + 6;
      }
      break;

    case WE_HDR_PROXY_AUTHENTICATE:
    case WE_HDR_WWW_AUTHENTICATE:
      len += strlen (elt->_u.str) + 14;
      break;

    case WE_HDR_ACCEPT_CHARSET:
    case WE_HDR_X_WAP_PUSH_ACCEPT_CHARSET:
      if ((elt->utype == PUS_UTYPE_STRING) ||
        (elt->utype == PUS_UTYPE_STRING_LIT)) {
        len += strlen (elt->_u.str);
      }
      else if (elt->utype == PUS_UTYPE_WE_UINT32) {
        s = we_charset_int2str (elt->_u.uint32);
        len += strlen (s);
      }
      else
        return FALSE;
      break;

    case WE_HDR_ACCEPT_LANGUAGE:
    case WE_HDR_CONTENT_LANGUAGE:
    case WE_HDR_X_WAP_PUSH_ACCEPT_LANGUAGE:
      if ((elt->utype == PUS_UTYPE_STRING) ||
          (elt->utype == PUS_UTYPE_STRING_LIT)) {
        len += strlen (elt->_u.str);
      }
      else if (elt->utype == PUS_UTYPE_WE_UINT8) {
        s = we_cmmn_int2str (elt->_u.uint8, &we_languages);
        len += strlen (s);
      }
      else if (elt->utype == PUS_UTYPE_WE_UINT32) {
        s = we_cmmn_int2str (elt->_u.uint32, &we_languages);
        len += strlen (s);
      }
      else
        return FALSE;
      break;

    case WE_HDR_ACCEPT_APPLICATION:
      if ((elt->utype == PUS_UTYPE_STRING) ||
          (elt->utype == PUS_UTYPE_STRING_LIT)) {
        len += strlen (elt->_u.str);
      }
      else if (elt->utype == PUS_UTYPE_WE_UINT8) {
        s = we_cmmn_int2str (elt->_u.uint8, &pus_applications);
        len += strlen (s);
      }
      else if (elt->utype == PUS_UTYPE_WE_UINT32) {
        s = we_cmmn_int2str (elt->_u.uint32, &pus_applications);
        len += strlen (s);
      }
      else
        return FALSE;
      break;

    case WE_HDR_CONTENT_LENGTH:
      len += pus_get_int_length (elt->_u.uint32);
      break;

    case WE_HDR_X_WAP_PUSH_STATUS:
    case WE_HDR_X_WAP_TERMINAL_ID:
    case WE_HDR_X_WAP_CPITAG:
      if ((elt->utype == PUS_UTYPE_STRING) ||
          (elt->utype == PUS_UTYPE_STRING_LIT)) {
        len += strlen (elt->_u.str);
      }
      break;


    default:
      if (elt->utype == PUS_UTYPE_WE_UINT8)
        len += pus_get_int_length (elt->_u.uint8);
      else if (elt->utype == PUS_UTYPE_WE_UINT32)
        len += pus_get_int_length (elt->_u.uint32);
      else if (elt->utype == PUS_UTYPE_DATE)
        len += PUS_HDR_CVT_DATESTRING_LENGTH;
      else
        len += strlen (elt->_u.str);
      break;
    }
    prev_field_name = header_type;
  }

  return len;
}

static int
pus_get_int_length (WE_UINT32 n)
{
  if (n < 10)
    return 1;
  else if (n < 100)
    return 2;
  else if (n < 1000)
    return 3;
  else if (n < 10000)
    return 4;
  else if (n < 100000)
    return 5;
  else if (n < 1000000)
    return 6;
  else if (n < 10000000)
    return 7;
  else if (n < 100000000)
    return 8;
  else if (n < 1000000000)
    return 9;
  else
    return 10;
}




static int
pus_get_cache_control_length (Pus_Hdr_cachecontrol_t *cc)
{
  int len = 0;

  if (cc->flags & PUS_NO_CACHE_FLAG) {
    len += 10;
  }
  if (cc->flags & PUS_NO_STORE_FLAG) {
    len += 10;
  }
  if (cc->flags & PUS_MAX_AGE_FLAG) {
    len += 10 + pus_get_int_length (cc->secage);
  }
  if (cc->flags & PUS_MAX_STALE_FLAG) {
    len += 12 + pus_get_int_length (cc->secstale);
  }
  if (cc->flags & PUS_MIN_FRESH_FLAG) {
    len += 12 + pus_get_int_length (cc->secfresh);
  }
  if (cc->flags & PUS_NO_TRANSFORM_FLAG) {
    len += 14;
  }
  if (cc->flags & PUS_ONLY_IF_CACHED_FLAG) {
    len += 16;
  }
  if (cc->flags & PUS_PUBLIC_FLAG) {
    len += 8;
  }
  if (cc->flags & PUS_PRIVATE_FLAG) {
    len += 9;
  }
  if (cc->flags & PUS_MUST_REVALIDATE_FLAG) {
    len += 17;
  }
  if (cc->flags & PUS_PROXY_REVALIDATE_FLAG) {
    len += 18;
  }
  if (cc->flags & PUS_S_MAX_AGE_FLAG) {
    len += 11 + pus_get_int_length (cc->secS_maxage);
  }

  



  return (len > 0) ? (len - 2) : 0;
}




static int
pus_get_parameter_length (Pus_Hdr_params_t *param)
{
  int len = 0;
 
  while (param != NULL) {
    len += strlen (param->name) + strlen (param->value) + 3;
    param = param->next;
  }

  return len;
}




static void
Pus_Hdr_txt_create_string (Pus_Hdr_table_t* table, char *headerstring)
{
  int            str_index = 0;
  int            prev_field_name = -1;
  int            header_type, tmp;
  const char    *hname;
  Pus_Hdr_element_t *elt;
  const char    *s;
  char           tmpbuf[50];

  for (elt = table->hdr_fields; elt != NULL; elt = elt->next) {
    header_type = elt->field_name;

    if (header_type == WE_HDR_UNKNOWN) {
      str_index = pus_add_to_string (headerstring, str_index,
                                 elt->_u.str, "\r\n");
      prev_field_name = -1;
      continue;
    }

    if ((header_type != prev_field_name) || (header_type == WE_HDR_COOKIE)) {
      hname = we_cmmn_int2str (header_type, &we_hdr_names);
      Pus_Hdr_make_header_name (tmpbuf, hname);
      str_index = pus_add_to_string (headerstring, str_index, tmpbuf, NULL);
    }
    else if (header_type == WE_HDR_USER_AGENT) {
      str_index = pus_add_to_string (headerstring, str_index, " ", NULL);
    }
    else {
      str_index = pus_add_to_string (headerstring, str_index, ", ", NULL);
    }

    switch (header_type) {
    case WE_HDR_ACCEPT:
    case WE_HDR_CONTENT_TYPE:
    case WE_HDR_X_WAP_PUSH_ACCEPT:
      str_index = pus_create_content_type_string (elt, headerstring, str_index);
      break;

    case WE_HDR_CACHE_CONTROL:
      str_index = pus_create_cache_control_string ((Pus_Hdr_cachecontrol_t*) elt->extras, headerstring,
                                               str_index);
      break;

    case WE_HDR_AUTHORIZATION:
    case WE_HDR_PROXY_AUTHORIZATION:
    case WE_HDR_X_WAP_AUTHORIZATION:
      if (!we_cmmn_strncmp_nc (elt->_u.str, "digest", 6)) {
        
        str_index = pus_add_to_string (headerstring, str_index, elt->_u.str, NULL);
      }
      else {
        
        str_index = pus_add_to_string (headerstring, str_index, "Basic ", NULL);
        tmp = 1000;
        (void)we_cmmn_base64_encode (elt->_u.str, strlen (elt->_u.str),
                                  headerstring + str_index, &tmp);
        str_index += tmp;
      }
      break;

    case WE_HDR_PROXY_AUTHENTICATE:
    case WE_HDR_WWW_AUTHENTICATE:
      str_index = pus_add_to_string (headerstring, str_index,
                                 "Basic realm=\"", elt->_u.str);
      str_index = pus_add_to_string (headerstring, str_index, "\"", NULL);
      break;
      
    case WE_HDR_ACCEPT_CHARSET:
    case WE_HDR_X_WAP_PUSH_ACCEPT_CHARSET:
      if ((elt->utype == PUS_UTYPE_STRING) || (elt->utype == PUS_UTYPE_STRING_LIT)) {
        str_index = pus_add_to_string (headerstring, str_index, elt->_u.str, NULL);
      }
      else {
        s = we_charset_int2str (elt->_u.uint32);
        str_index = pus_add_to_string (headerstring, str_index, s, NULL);
      }
      break;

    case WE_HDR_ACCEPT_LANGUAGE:
    case WE_HDR_CONTENT_LANGUAGE:
    case WE_HDR_X_WAP_PUSH_ACCEPT_LANGUAGE:
      if ((elt->utype == PUS_UTYPE_STRING) || (elt->utype == PUS_UTYPE_STRING_LIT)) {
        str_index = pus_add_to_string (headerstring, str_index, elt->_u.str, NULL);
      }
      else {
        s = we_cmmn_int2str (elt->_u.uint32, &we_languages);
        str_index = pus_add_to_string (headerstring, str_index, s, NULL);
      }
      break;

    case WE_HDR_ACCEPT_APPLICATION:
      if ((elt->utype == PUS_UTYPE_STRING) || (elt->utype == PUS_UTYPE_STRING_LIT)) {
        str_index = pus_add_to_string (headerstring, str_index, elt->_u.str, NULL);
      }
      else {
        s = we_cmmn_int2str (elt->_u.uint32, &pus_applications);
        str_index = pus_add_to_string (headerstring, str_index, s, NULL);
      }
      break;

    case WE_HDR_X_WAP_PUSH_STATUS:
    case WE_HDR_X_WAP_TERMINAL_ID:
      if ((elt->utype == PUS_UTYPE_STRING) || (elt->utype == PUS_UTYPE_STRING_LIT)) {
        str_index = pus_add_to_string (headerstring, str_index, elt->_u.str, NULL);
      }
      break;

    default:
      if (elt->utype == PUS_UTYPE_WE_UINT8) {
        sprintf (tmpbuf, "%d", elt->_u.uint8);
        str_index = pus_add_to_string (headerstring, str_index, tmpbuf, NULL);
      }
      else if (elt->utype == PUS_UTYPE_WE_UINT32) {
        sprintf (tmpbuf, "%ld", elt->_u.uint32);
        str_index = pus_add_to_string (headerstring, str_index, tmpbuf, NULL);
      }
      else if (elt->utype == PUS_UTYPE_DATE) {
        we_cmmn_time2str (elt->_u.uint32, tmpbuf);
        str_index = pus_add_to_string (headerstring, str_index, tmpbuf, NULL);
      }
      else {
        str_index = pus_add_to_string (headerstring, str_index, elt->_u.str, NULL);
      }
      break;
    }

    if ((elt->next == NULL) || (elt->next->field_name != header_type) ||
        (header_type == WE_HDR_COOKIE))
      str_index = pus_add_crlf_to_string (headerstring, str_index);
    prev_field_name = header_type;
  }
  headerstring[str_index] = '\0';
}





static int
pus_create_content_type_string (Pus_Hdr_element_t *elt, char *headerstring,
                                int str_index)
{
  const char   *temp;
  Pus_Hdr_params_t *param;

  if (elt->utype == PUS_UTYPE_WE_UINT8) {
    temp = we_cmmn_int2str (elt->_u.uint8, &we_mime_types);
  }
  else if (elt->utype == PUS_UTYPE_WE_UINT32) {
    temp = we_cmmn_int2str (elt->_u.uint32, &we_mime_types);
  }
  else
    temp = elt->_u.str;

  str_index = pus_add_to_string (headerstring, str_index,
                             temp, NULL);

  for (param = (Pus_Hdr_params_t *)elt->extras; param != NULL;
       param = param->next) {
    str_index = pus_add_to_string (headerstring, str_index,
                                   "; ", param->name);
    str_index = pus_add_to_string (headerstring, str_index,
                                   "=", param->value);
  }

  return str_index;
}

static int
pus_create_cache_control_string (Pus_Hdr_cachecontrol_t *cc, char *headerstring,
                                int str_index)
{
  char buf[12];
  int  need_comma = 0;

  if (cc->flags & PUS_NO_CACHE_FLAG) {
    str_index = pus_add_to_string (headerstring, str_index,
                               need_comma ? ", " : NULL, "no-cache");
    need_comma = 1;
  }
  if (cc->flags & PUS_NO_STORE_FLAG) {
    str_index = pus_add_to_string (headerstring, str_index,
                               need_comma ? ", " : NULL, "no-store");
    need_comma = 1;
  }
  if (cc->flags & PUS_MAX_AGE_FLAG) {
    sprintf (buf, "%ld", cc->secage);
    str_index = pus_add_to_string (headerstring, str_index,
                               need_comma ? ", max-age=" : "max-age=", buf);
    need_comma = 1;
  }
  if (cc->flags & PUS_MAX_STALE_FLAG) {
    sprintf (buf, "%ld", cc->secstale);
    str_index = pus_add_to_string (headerstring, str_index,
                               need_comma ? ", max-stale=" : "max-stale=",
                               buf);
    need_comma = 1;
  }
  if (cc->flags & PUS_MIN_FRESH_FLAG) {
    sprintf (buf, "%ld", cc->secfresh);
    str_index = pus_add_to_string (headerstring, str_index,
                               need_comma ? ", min-fresh=" : "min-refresh=",
                               buf);
    need_comma = 1;
  }
  if (cc->flags & PUS_NO_TRANSFORM_FLAG) {
    str_index = pus_add_to_string (headerstring, str_index,
                               need_comma ? ", " : NULL, "no-transform");
    need_comma = 1;
  }
  if (cc->flags & PUS_ONLY_IF_CACHED_FLAG) {
    str_index = pus_add_to_string (headerstring, str_index,
                               need_comma ? ", " : NULL, "only-if-cached");
    need_comma = 1;
  }
  if (cc->flags & PUS_PUBLIC_FLAG) {
    str_index = pus_add_to_string (headerstring, str_index,
                               need_comma ? ", " : NULL, "public");
    need_comma = 1;
  }
  if (cc->flags & PUS_PRIVATE_FLAG) {
    str_index = pus_add_to_string (headerstring, str_index,
                               need_comma ? ", " : NULL, "private");
    need_comma = 1;
  }
  if (cc->flags & PUS_MUST_REVALIDATE_FLAG) {
    str_index = pus_add_to_string (headerstring, str_index,
                               need_comma ? ", " : NULL, "must-revalidate");
    need_comma = 1;
  }
  if (cc->flags & PUS_PROXY_REVALIDATE_FLAG) {
    str_index = pus_add_to_string (headerstring, str_index,
                               need_comma ? ", " : NULL, "proxy-revalidate");
    need_comma = 1;
  }
  if (cc->flags & PUS_S_MAX_AGE_FLAG) {
    sprintf (buf, "%lu", cc->secS_maxage);
    str_index = pus_add_to_string (headerstring, str_index,
                               need_comma ? ", s-maxage=" : "s-maxage=", buf);
  }

  return str_index;
}





static int
pus_add_crlf_to_string (char *headerstring, int str_index)
{
  headerstring[str_index] = 0x0D;      
  headerstring[str_index + 1] = 0x0A;  

  return str_index + 2;
}




static int
pus_add_to_string (char *headerstring, int str_index,
               const char *str_first, const char *str_after)
{
  if (str_first != NULL) {
    strcpy (headerstring + str_index, str_first);
    str_index += strlen (str_first);
  }
  if (str_after != NULL) {
    strcpy (headerstring + str_index, str_after);
    str_index += strlen (str_after);
  }

  return str_index;
}





static void
Pus_Hdr_make_header_name (char *dst, const char *src)
{
  const char *s;
  int         first = 1;

  for (s = src; *s != '\0'; s++) {
    if (first && ct_isalpha (*s)) {
      *dst++ = (char)(*s + 'A' - 'a');
      first = 0;
    }
    else {
      *dst++ = *s;
      if (!ct_isalphanum (*s))
        first = 1;
    }
  }
  *dst++ = ':';
  *dst++ = ' ';
  *dst = '\0';
}




void
Pus_Hdr_delete_table (Pus_Hdr_table_t* table)
{
  Pus_Hdr_element_t *temp;

  if (table != NULL) {
    while (table->hdr_fields != NULL) {
      temp = table->hdr_fields;
      table->hdr_fields = table->hdr_fields->next;
      Pus_Hdr_delete_element (temp);
    }
    PUS_MEM_FREE (table);
  }
}

#endif
