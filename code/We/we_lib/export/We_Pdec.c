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
 * We_Pdec.c
 *
 * XML and HTML parser: WBXML decoder
 */

#include "We_Prsr.h"
#include "We_Pdec.h"
#include "We_Ptxt.h"
#include "We_Mem.h"
#include "We_Chrs.h"
#include "We_Lib.h"


/*
 * COMMENTS:
 *
 * Only one version of WBXML is supported, even though the public
 * wid can take several values. Version 1.3 is considered just a
 * clarification of 1.2 and 1.2 is (maybe erronously) taken to be
 * compatible with 1.1.
 *
 * Strings are defined to be terminated in a "character document
 * encoding" dependent way and cannot be presumed to include NULL
 * termination. This decoder assumes all supported encodings to 
 * be NULL terminated. parser->strchr_funct point out the strchr
 * function used for finding the terminating '\0'. I.e. UTF-16
 * strings uses two null-bytes as termination.
 *
 * The maximum string table size is WE_DEC_CFG_MAX_STRING_TABLE_SIZE.
 * Larger stringtables cause a syntax error.
 *
 * Each application must har three tables:
 * - a transform table from attrStart to attribute value
 * - an attrStart table
 * - an attrValue table
 *
 * The element code pages are 64 elements each and the page
 * number is multiplied by 64 and added to the token value:
 * page 0, token 0  -> element 0
 * page 0, token 63 -> element 63
 * page 1, token 0  -> element 64
 * page 1, token 63 -> element 127
 *
 * The attribute code pages are 128 elements each and the page
 * number is multiplied by 128 and added to the token value:
 * attribute start token
 * page 0, token 0   -> attribute 0
 * page 0, token 127 -> attribute 127
 * page 1, token 0   -> attribute 128
 * page 1, token 127 -> attribute 255
 *
 * attribute value token
 * page 0, token 128 -> attribute 128
 * page 0, token 255 -> attribute 255
 * page 1, token 128 -> attribute 256
 * page 1, token 255 -> attribute 383
 */

/*
 *  WBXML document public identifier codes:
 *
 *     1  Unknown or missing public identifier
 *     2  WML 1.0
 *     3  WTA event 1.0
 *     4  WML 1.1
 *     5  SI 1.0
 *     6  SL 1.0
 *     7  CO 1.0
 *     8  Channel 1.1
 *     9  WML 1,2
 *     A  WML 1.3
 *     B  Prov 1.0
 *     C  WTA-WML 1.2
 *     D  Channel 1.2
 *
 *  1100  PHONE.COM ALERT 1.0
 *  1101  PHONE.COM CACHE-OPERATION 1.0
 *  1102  PHONE.COM SIGNAL 1.0
 *  1103  PHONE.COM LIST 1.0
 *  1104  PHONE.COM LISTCMD 1.0
 *  1105  PHONE.COM CHANNEL 1.0
 *  1106  PHONE.COM MMC 1.0
 *  1107  PHONE.COM BEARER-CHOICE 1.0
 *  1108  PHONE.COM WML 1.1
 *  1109  PHONE.COM CHANNEL 1.1
 *  110A  PHONE.COM LIST 1.1
 *  110B  PHONE.COM LISTCMD 1.1
 *  110C  PHONE.COM MMC 1.1
 *  110D  PHONE.COM WML 1.3
 *  110E  PHONE.COM MMC 2.0
 */


/**************************************************************
 * Constants                                                  *
 **************************************************************/

#define WE_DEC_CFG_MAX_STRING_TABLE_SIZE 65536


#define WE_DEC_ATTRIBUTE_EXTENSION 0
#define WE_DEC_ELEMENT_EXTENSION   1 
#define WE_DEC_ATTRIBUTE_OPAQUE    2
#define WE_DEC_ELEMENT_OPAQUE      3


/**************************************************************
 * Type definitions                                           *
 **************************************************************/

typedef int (*we_dec_writeFnct_t) (we_prsr_parser_t*  parser,
                                    const char*        *s,
                                    const char*         end,
                                    int                 flush);


/**************************************************************
 * Local function declarations                                *
 **************************************************************/

static int
we_dec_piDecoder (we_prsr_parser_t*  parser,
                   const char*         s,
                   const char*         end,
                   const char*        *nextPtr,
                   int                 limit);

static int
we_dec_doPI (we_prsr_parser_t*  parser,
              const char*         s,
              const char*         end,
              const char*        *nextPtr);


/**************************************************************
 * Functions                                                  *
 **************************************************************/

/*
 * Read an mb_u_int32. '*s' points to an mb_u_in32 and is advanced
 * to the first following character. End points to the end of
 * the buffer. '*i' gives the return value.
 */
static int
we_dec_get_mb_uint32 (const char* *s, const char* end, WE_UINT32 *i)
{
  const char*  p;
  WE_UINT32   t;
  int          len;

  t = 0;
  p = *s;
  for (len = 0; len < 5; len++) {
    if (p >= end) {
      return WE_PRSR_STATUS_MORE;
    }
    t = (t << 7) + (*p & 0x7F);
    if (*(unsigned char*) p < 0x80) {
      *i = t;
      *s = p + 1;
      return WE_PRSR_STATUS_OK;     
    }
    p++;
  }
  return WE_PRSR_ERROR_SYNTAX;
}


/*
 * Dummy strchr used for finding null character in UTF-16
 * strings. 's' gives the string to search and 'c' is not used.
 */
static char* 
we_dec_wstrchr (const char* s, int c)
{
  c = c;

  while (*s != 0 || *(s + 1) != 0) {
    s += 2;
  }
  return (char*) s;
}



/*
 * Write a null-terminated string '*s' to the stack. 'end' points
 * to the null character and '*s' is advanced to the begining of
 * the following character.
 *
 * Flushes the stack when needed if 'flush' is TRUE.
 *
 * Returns error code.
 */
static int
we_dec_writeString (we_prsr_parser_t* parser, const char* *s, const char* end, int flush)
{
  we_prsr_stack_t* stack;
  long              src_len;
  long              src_rest;
  long              dst_len;
  int               error;

  src_len = end - *s;
  
  if (parser->codecvtFunct == NULL) {
    we_prsr_writeData (parser, *s, src_len, flush);
  }
  else {
    stack = &parser->stack;
    src_rest = src_len;

    for (;;) {
      dst_len = stack->end - stack->ptr - 1;
      if (parser->codecvtFunct (*s, &src_len, stack->ptr, &dst_len) == -1) {
        return WE_PRSR_ERROR_SYNTAX;
      }
      *s += src_len;
      src_rest -= src_len;
      stack->ptr += dst_len;

      if (src_rest <= 0) {
        break;
      }
      if (flush) {
        error = we_prsr_flushText (parser, FALSE);
      }
      else {
        error = we_prsr_expandStack (parser, 0);
      }
      if (error) {
        return error;
      }
      src_len = src_rest;
    }
  }
  /* Reset needed by both cases. Some bytes may be left
   * if they result in only a partial character. If so, discard
   * this data. */
  *s = end; 

  if (parser->stack.error) {
    return parser->stack.error;
  }

  (*s)++; /* Skip the null-character */
  if (parser->strchr_funct == we_dec_wstrchr) {
    (*s)++; /* wstr */
  }

  return WE_PRSR_STATUS_OK;
}



/*
 * Write a string from the string table to the stack, using
 * we_dec_writeString. '*s' points an mb_u_int32 and is advanced to
 * the first following character. 'end' points to the end of
 * the buffer.
 *
 * Flushes the stack when needed if 'flush' is TRUE.
 *
 * Returns error code.
 */
static int
we_dec_writeTableString (we_prsr_parser_t* parser, const char* *s, const char* end, int flush)
{
  const char* p;
  const char* q;
  WE_UINT32  index;
  int         error;

  error = we_dec_get_mb_uint32 (s, end, &index);
  if (error) {
    return error;
  }

  if (index >= (WE_UINT32) parser->stringTableLen) { /* Safe: stringTableLen >= 0 and siall */
    return WE_PRSR_ERROR_SYNTAX;
  }
  p = parser->stringTable + index;
  q = parser->strchr_funct (p, '\0'); /* Safe since the buffer is null terminated */
  if (q >= parser->stringTable + parser->stringTableLen - 
                                              (parser->strchr_funct == we_dec_wstrchr)) {
    return WE_PRSR_ERROR_SYNTAX;
  }
  return we_dec_writeString (parser, &p, q, flush);
}



/*
 * Common WBXML opaque data handler. Writes opaque data as normal
 * text. This msans opaque data can be be a part of any string
 * in character data callbacks.
 *
 * If WBXML opaque data is not to be ignored, then in the
 * initiation callback, set d->specific = we_dec_opaqueSpecific.
 */
int 
we_dec_opaqueSpecific (we_prsr_parser_t*  parser,
                        const char*        *s,
                        const char*         end,
                        int                 size,
                        int                 type)
{
  end = end;

  if (type != WE_DEC_ELEMENT_OPAQUE) {
    return WE_PRSR_STATUS_OK;
  }

  we_prsr_writeData (parser, *s, size, FALSE);

  return WE_PRSR_STATUS_OK;
  return parser->stack.error;
}



/*
 * Push opaque data handler. Write 7 bytes from attribute opaque
 * data and fill with zeros if indata is less than 7 bytes.
 *
 * ONLY USED BY PUS.
 */
int 
we_dec_pushSpecific (we_prsr_parser_t*  parser,
                      const char*        *s,
                      const char*         end,
                      int                 size,
                      int                 type)
{
  end = end;

  if (type != WE_DEC_ATTRIBUTE_OPAQUE) {
    return WE_PRSR_STATUS_OK;
  }
  
  we_prsr_writeData (parser, "$", 1, FALSE);
  if (size > 7) {
    size = 7;
  }
  we_prsr_writeData (parser, *s, size, FALSE);
  if (size < 7) {
    we_prsr_writeData (parser, "\0\0\0\0\0", 7 - size, FALSE);
  }
  return WE_PRSR_STATUS_OK;
  return parser->stack.error;
}



/*
 * Translate a WML variable string into a UTF-8 well-formatted
 * variable string and write it to the stack.
 *
 * ONLY USED BY THE WBS.
 */
static int
we_dec_writeVariableString (we_prsr_parser_t*   parser,
                             const char*         *s,
                             const char*          end,
                             we_dec_writeFnct_t  f,
                             int                  type)
{
  static const char* const CharStr = "$(:)";
  static const char* const TypeStr = "escapeunesc noesc";
  int                      error;

  we_prsr_writeData (parser, &CharStr[0], 2, FALSE);

  error = f (parser, s, end, FALSE);
  if (error) {
    parser->stack.ptr -= 2; /* Undo the "$(" if more data is needed. */
    return error;
  }

  we_prsr_writeData (parser, &CharStr[2], 1, FALSE);
  we_prsr_writeData (parser, &TypeStr[type * 6], type ? 5 : 6, FALSE);
  we_prsr_writeData (parser, &CharStr[3], 1, TRUE);
  return WE_PRSR_STATUS_OK;
  return parser->stack.error;
}



/*
 * Copy attribute data without normalisation.
 */
static we_prsr_attr_value_t
we_prsr_getOpaqueAttribute (we_prsr_parser_t*  parser, const char *data, int dataLen)
{
  we_prsr_attr_value_t u;
  
  u.s = WE_MEM_ALLOC (parser->modId, dataLen + 1);
  memcpy (u.s, data, dataLen);
  u.s[dataLen] = '\0';
  
  return u;
}



/*
 * WML variable handler. Decodes WML extension tokens into variables.
 *
 * ONLY USED BY THE WBS.
 */ 
int 
we_dec_wmlSpecific (we_prsr_parser_t*  parser,
                     const char*        *s,
                     const char*         end,
                     int                 tok,
                     int                 type)
{
  we_prsr_stack_t* stack;
  int               error;
  int               escapeType;

  if (type != WE_DEC_ELEMENT_EXTENSION &&
      type != WE_DEC_ATTRIBUTE_EXTENSION) { /* No opaque support */
    return WE_PRSR_STATUS_OK;
  }

  escapeType = tok & 0x3F;

  if (type == WE_DEC_ELEMENT_EXTENSION) { /* Is an element */
    int error = we_prsr_flushText (parser, TRUE);
    if (error) {
      return error;
    }
  }

  if (tok <= 0x82) {
    if (tok <= 0x42) { /* Inline */
      char* q = parser->strchr_funct (*s, 0);
      if (q >= end - (parser->strchr_funct == we_dec_wstrchr)) { /* Sentinel stopped the search */
        return WE_PRSR_STATUS_MORE;
      }
      if (type == WE_DEC_ELEMENT_EXTENSION) {
        error = we_dec_writeString (parser, s, q, FALSE);
      }
      else {
        error = we_dec_writeVariableString (parser, s, q, we_dec_writeString, escapeType);
      }
    } 
    else { /* Table */
      if (type == WE_DEC_ELEMENT_EXTENSION) {
        error = we_dec_writeTableString (parser, s, end, FALSE);
      }
      else {
        error = we_dec_writeVariableString (parser, s, end,
                                             we_dec_writeTableString, escapeType);
      }
    }

    if (! error && type == WE_DEC_ELEMENT_EXTENSION) {
      stack = &parser->stack;
      *stack->ptr = '\0'; /* Safe since we_dec_writeString leaves >= 1 char free */
      parser->p.charHandler (parser->p.userData, stack->sessionStart,
                             stack->ptr - stack->sessionStart,
                            (1 << (4 + escapeType)) | WE_PRSR_FLAG_WBXML_VARIABLE);
      stack->ptr = stack->sessionStart;
    }
  } 
  else { /* Single byte */
    error = WE_PRSR_ERROR_SYNTAX; /* Not used in WML */
  }

  return error;
}



/*
 * Decode the WBXML prolog.
 */
int
we_dec_prologueDecoder (we_prsr_parser_t*  parser,
                         const char*         s,
                         const char*         end,
                         const char*        *nextPtr,
                         int                 limit)
{
  const char* dpiPtr;
  const char* doctype_str;
  int         error;
  WE_UINT32  ui32;
  int         wbxml_code;
  int         charset;
  int         len;
  int         endOfData;
  int         doctype_len;

  limit = limit;

  endOfData   = *nextPtr == NULL;
  *nextPtr    = s;
  doctype_str = NULL;
  doctype_len = 0;
  wbxml_code  = 0;

  if (s >= end) {
    return WE_PRSR_STATUS_MORE;
  }

  /* Version number */
  if (*s < 1 || *s > 3) { /* WBXML 1.1, 1.2, 1.3 */
    return WE_PRSR_ERROR_UNSUPPORTED_WBXML;
  }


  /* Document public identifier */
  if (*++s == 0) {
    /* Literal form */
    s++;
    dpiPtr = s;
    error = we_dec_get_mb_uint32 (&s, end, &ui32); /* Dummy; just advance pointer */
  } 
  else {
    /* Numeric form */
    error = we_dec_get_mb_uint32 (&s, end, &ui32);
    wbxml_code  = (int) ui32;
    dpiPtr = NULL;
  }
  if (s >= end) {
    return WE_PRSR_STATUS_MORE;
  }
  if (error) {
    return error;
  }
  
  
  /* Charset */
  error = we_dec_get_mb_uint32 (&s, end, &ui32);
  if (error) {
    return error;
  }
  if (ui32 == 0) {
    charset = WE_CHARSET_UTF_8;
  }
  else {
    charset = (int) ui32;
  }
  parser->charset = charset;
  parser->strchr_funct = strchr;

  parser->codecvtFunct = we_charset_find_function (charset, WE_CHARSET_UTF_8);
  if (parser->codecvtFunct == NULL) {
    charset = WE_CHARSET_LATIN_1; /* Use Latin-1 instead of unsupported charset */
    parser->codecvtFunct = we_charset_find_function (WE_CHARSET_LATIN_1, WE_CHARSET_UTF_8);
  }
  if (charset == WE_CHARSET_UTF_16BE ||
      charset == WE_CHARSET_UTF_16LE ||
      charset == WE_CHARSET_UTF_16   ||
      charset == WE_CHARSET_UCS_2) {
    parser->strchr_funct = we_dec_wstrchr;
  }

  /* String table */
  error = we_dec_get_mb_uint32 (&s, end, &ui32);
  if (error) {
    return error;
  }
  len = MAX (0, (int) ui32);
  if (len > WE_DEC_CFG_MAX_STRING_TABLE_SIZE) {
    return WE_PRSR_ERROR_SYNTAX;
  }
  if (s + len >= end) {
    return WE_PRSR_STATUS_MORE;
  }

  parser->stringTableLen = (int) len;
  if (endOfData) {
    parser->stringTable = (char*) s; /* Entire document in memory; don't copy string table */
  }
  else {
    parser->stringTable = (char*) WE_MEM_ALLOC (parser->modId, len + 2);
    if (parser->stringTable == NULL)
      return WE_PRSR_ERROR_OUT_OF_MEMORY;
    memcpy (parser->stringTable, s, len);
    parser->stringTable[len]     = '\0'; /* Just in case the table is malformatted */
    parser->stringTable[len + 1] = '\0';
  }


  /* Document public identifier (revisited) */
  if (dpiPtr != NULL) { /* Literal dpi */
    we_prsr_stack_t* stack = &parser->stack;
    
    error = we_dec_writeTableString (parser, &dpiPtr, end, FALSE); 
    if (error)
      return error;
    
    doctype_str = stack->sessionStart;
    doctype_len = stack->ptr - stack->sessionStart;
    wbxml_code  = 0;

    stack->ptr = stack->sessionStart;
  }
  
  error = parser->initFunct (&parser->p, &parser->d, doctype_str,
                             doctype_len, wbxml_code, charset);
  switch (error) {
  case WE_PRSR_APPLICATION_ERROR:
    return WE_PRSR_ERROR_UNKNOWN_DOCTYPE;
  case WE_PRSR_APPLICATION_OUT_OF_MEMORY:
    return WE_PRSR_ERROR_OUT_OF_MEMORY;
  }

  /* 
   * we_prsr_newSession is not used. The same session is reused and
   * the stack mode is set to STACKMODE_WBXML_WML. This tells
   * Wbs_Hvld.c that the variables has already been extracted
   * from the character data.
   */
  parser->stack.mode = STACKMODE_WBXML_WML;

  /* Done */
  *nextPtr = s + len;
  parser->processor = we_dec_piDecoder;
  return WE_PRSR_STATUS_RETURN;
}



/*
 * Decode attributes and return them in '*attrs'. The function is
 * intended to be called twice: first to count the number of
 * attributes, then to extract them.If '*nextPtr' == NULL then
 * just count the number of attributes, else '*nbrOfAttrs' must
 * give the correct number of attributes. Unknown attributes are
 * ignored.
 *
 * It is the callers responsibility to dealloc '*attrs'.
 */
int
we_dec_decodeAttributes (we_prsr_parser_t*     parser,
                          const char*            s,
                          const char*            end,
                          const char*           *nextPtr,
                          int                   *nbrOfAttrs,
                          we_prsr_attribute_t* *attrs)
{
  char                  entityStr[4];
  we_prsr_stack_t*     stack;
  const char*           p;
  we_prsr_attribute_t* v = 0;
  WE_UINT32            i;
  int                   attribute = 0;
  int                   unknownAttribute;
  int                   error;
  int                   pos;
  int                   size;
  int                   result;
  int                   flag;
  int                   len;
  int                   mode;
  int                   opaque;
  unsigned char         tok;

  /* Verify that the attribute section starts with 
   * ([SWITCH_PAGE uint8] ATTRSTART) | LITERAL */
  if (*s == 0) { /* switch page */
    if (s + 2 >= end) {
      return WE_PRSR_STATUS_MORE;
    }
    if ((*(s + 2) & 0x3F) <= 4 || *((unsigned char*) s + 2) >= 128) {
      return WE_PRSR_ERROR_SYNTAX;
    }
  } 
  else if (((*s & 0x3F) <= 4 && *s != 0x44) || *(unsigned char *) s >= 128) {
    return WE_PRSR_ERROR_SYNTAX;
  }

  stack = &parser->stack;
  pos = -1;
  unknownAttribute = FALSE;
  opaque = FALSE;

  if (*nextPtr != NULL) {
    v = (we_prsr_attribute_t*) WE_MEM_ALLOC (parser->modId,
                                               sizeof (we_prsr_attribute_t) * *nbrOfAttrs);
  }

  while (s < end) {
    tok = *(unsigned char*) s++;

    if ((tok & 0x3F) > 4 || tok == 4 || tok == 1) {
      if (tok < 128) { /* terminating tokens == attrStart | end */
AttrStart:
        if (unknownAttribute) {
          /* Skip previous attribute if unknown */
          unknownAttribute = FALSE;
        }
        else {
          if (pos >= 0 && *nextPtr != NULL) {
            int failure;
            /* Save result from last attribute before starting on
             * the next one. This is also done to the last
             * attribute before ending. */
          
            len = stack->ptr - stack->sessionStart;
            p = we_cmmn_int2str (attribute, parser->p.attributes);
            flag = 0; /* No entity references in the input */

            mode = stack->mode;
            v[pos].type = (short) attribute;
            if (opaque || /* Some of the data was opaque; do not normalise */
                (parser->p.attributeType[attribute]  & ~WE_PRSR_TYPE_NO_CASE) ==
                              WE_PRSR_TYPE_CDATA) { /* Do normalisation for string attributes */
              v[pos].u = we_prsr_getOpaqueAttribute (parser, stack->sessionStart, len);
              failure = 0;
            } 
            else {
              v[pos].u = we_prsr_normalizeAttribute (parser, parser->p.attributeType[attribute],
                                              p, -1, stack->sessionStart, len, &flag, &failure);
            }
            v[pos].flag = (short) flag;
            stack->mode = mode;
            if (failure > 0) {
              pos--;
            }
            else if (failure < 0) {
              return WE_PRSR_ERROR_OUT_OF_MEMORY;
            }
          }
          pos++;
        }
        opaque = FALSE;
        stack->ptr = stack->sessionStart;
        
        if (tok == 1) { /* ------------------------------------------  end */
          if (*nextPtr != NULL && pos == 0) {
            /* All attribute value normalizations failed. The vector is empty. */
            WE_MEM_FREE (parser->modId, v);
          }
          *nextPtr = s;
          *nbrOfAttrs = pos;
          *attrs = v;
          return WE_PRSR_STATUS_OK;
        }

        /* ----------------------------------------------------------  attribute start */
        if ((tok & 0x3F) == 4) { /* literal */
          error = we_dec_writeTableString (parser, &s, end, FALSE);
          if (error) {
            goto Return_error;
          }
          result = we_cmmn_str2int (stack->sessionStart, stack->ptr - stack->sessionStart,
                              parser->p.attributes);
          stack->ptr = stack->sessionStart;

          if (result == -1) {
            unknownAttribute = TRUE;
          }
          else {
            attribute = result;
          }
        } 
        else { /* attribute start token */
          int attrStart = tok + parser->attrPage;

          /* Verify this is a known attribute */
          if (attrStart >= parser->d.transformSize ||
            parser->d.transform[attrStart] == WE_PRSR_NOT_USED) {
            unknownAttribute = TRUE;
          }
          else {
            attribute = parser->d.transform[attrStart];
            if (attribute & WE_PRSR_DEC_DATA) {
              p = we_cmmn_int2str (attrStart, parser->d.attrStart);
              if (p != NULL) {
                we_prsr_writeData (parser, p, 0, FALSE);
              }
              attribute &= ~WE_PRSR_DEC_DATA;
            }
          }
        }
      } 
      else { /* ---------------------------------------------------  attribute value */
AttrValue:        
        if (parser->d.attrValue != NULL) {
          p = we_cmmn_int2str (tok + parser->attrPage, parser->d.attrValue);
          if (p != NULL) {
            we_prsr_writeData (parser, p, 0, FALSE);
          }
        }
      }

      if (parser->stack.error) {
        return parser->stack.error;
      }

      /* ------------------------------------------------------------  string */
    } 
    else if (tok == 3) { /* Inline */
      char* q = parser->strchr_funct (s, 0);
      if (q >= end - (parser->strchr_funct == we_dec_wstrchr)) { /* Sentinel stopped search */
        goto Return_more;
      }
      error = we_dec_writeString (parser, &s, q, FALSE);
      if (error) {
        return error;
      }
    } 
    else if (tok == 0x83) { /* Table */
      error = we_dec_writeTableString (parser, &s, end, FALSE);
      if (error) {
        goto Return_error;
      }
    } 
    else if (tok > 2 && (tok & 0x3F) <= 2) { /* -------------------  extension */
Extension:
      if (parser->d.specific) {
        error = parser->d.specific (parser, &s, end, tok, WE_DEC_ATTRIBUTE_EXTENSION);
        if (error) {
          return error;
        }
      }
    } 
    else if (tok == 2) { /* ---------------------------------------  entity */
      error = we_dec_get_mb_uint32 (&s, end, &i);
      if (error) {
        goto Return_error;
      }
      size = we_charset_unicode_to_utf8 (i, entityStr);
      if (size == -1) {
        error = WE_PRSR_ERROR_SYNTAX;
        goto Return_error;
      }
      we_prsr_writeData (parser, entityStr, size, FALSE);
      if (parser->stack.error) {
        return parser->stack.error;
      }
    } 
    else if (tok == 0xC3) { /* ------------------------------------  opaque */
      error = we_dec_get_mb_uint32 (&s, end, &i);
      result = (int) i;
      if (error || result < 0) {
        goto Return_error;
      }
      if (s + result > end) {
        goto Return_more;
      }
      if (parser->d.specific) {
        parser->d.specific (parser, &s, NULL, result, WE_DEC_ATTRIBUTE_OPAQUE);
      }
      s += result;
      opaque = TRUE;

    } 
    else if (tok == 0) { /* ---------------------------------------  switch page */
      if (s + 2 >= end) {
        goto Return_more;
      }
      parser->attrPage = (unsigned short) ((*(unsigned char*) s++) << 7); /* 128 per page */

      tok = *(unsigned char*) s++;
      if (s >= end) {
        goto Return_more;
      }
      
      /* Attribute value except string, extension, entity and opaque */
      if (tok >= 128 && (tok & 0x3F) > 4) {
        goto AttrValue;
      }
      /* Attribute start except LITTERAL */
      else if ((tok & 0x3F) > 4 && tok < 128) {
        goto AttrStart;
      }
      /* Extension */
      else if (tok > 2 && (tok & 0x3F) <= 2) { 
        goto Extension;
      }
      else {
        return WE_PRSR_ERROR_SYNTAX;
      }

    } 
    else { /* ------------------------------------------  pi, LITERAL_A, LITERAL_C, LITERAL_AC */
      return WE_PRSR_ERROR_SYNTAX;
    }
  }
Return_more:
  stack->ptr = stack->sessionStart; /* Clear temporary data when interrupted */
  return WE_PRSR_STATUS_MORE;

Return_error:
  stack->ptr = stack->sessionStart; /* Clear temporary data when interrupted */
  return error;
}



/*
 * Decode the content part of the body. The optional processing
 * instructions leading and trailing the content are decoded by
 * we_dec_piDecoder.
 */
int
we_dec_contentDecoder (we_prsr_parser_t*  parser,
                        const char*         s,
                        const char*         end,
                        const char*        *nextPtr,
                        int                 limit)
{
  char                  entityStr[4];
  const char*           start;
  const char*           next;
  we_prsr_stack_t*     stack;
  we_prsr_attribute_t* attrs = 0;
  WE_UINT32            i;
  unsigned short        tok;   
  int                   element;   
  int                   error;
  int                   result;   
  int                   size;

  stack = &parser->stack;
  *nextPtr = s;

  /* If more opaque data left from last iteration, finish it first */
  if (parser->opaqueLen > 0) {
    size = MIN (end - s, parser->opaqueLen);
    if (parser->d.specific) {
      parser->d.specific (parser, &s, NULL, size, WE_DEC_ELEMENT_OPAQUE);
    }
    parser->opaqueLen -= size;
    s += size;
    if (parser->opaqueLen > 0) {
      *nextPtr = s;
      return WE_PRSR_STATUS_MORE;
    }
  }

  start = s;
  while (s - start < limit) {
    if (s >= end) {
      return WE_PRSR_STATUS_MORE;
    }

    tok = *(unsigned char*) s++;

    /* content */
    if ((tok & 0x3F) >= 4) { /* element == tag | literal */
Element:
      error = we_prsr_flushText (parser, TRUE);
      if (error) {
        return error;
      }
      
      if ((tok & 0x3F) == 4) { /* -----------------------------------  literal */
          error = we_dec_writeTableString (parser, &s, end, FALSE);
          if (error) {
            return error;
          }
          /* Unknown element returns -1, the same as PRSR_UNKNOWN_ELEMENT. */
          element = we_cmmn_str2int (stack->sessionStart, stack->ptr - stack->sessionStart,
                                      parser->p.elements);
          stack->ptr = stack->sessionStart;

      } 
      else { /* ---------------------------------------------------  tag */
        element = (tok & 0x3F) + parser->elemPage;

        





      }

      if (tok & 0x80) { /* Attribute */
        WE_UINT16 codePage = parser->attrPage;
        
        if (s >= end) {
          return WE_PRSR_STATUS_MORE;
        }

        next = NULL; /* Signals count-only mode */
        error = we_dec_decodeAttributes (parser, s, end, &next, &size, &attrs);
        if (error) {
          return error;
        }

        if (size > 0 && element >= 0) { /* Known element with known and correct attributes */
          next = s; /* Signals save-attribute mode */
          parser->attrPage = codePage; /* Restores state */
          we_dec_decodeAttributes (parser, s, end, &next, &size, &attrs);
        }
        s = next;
      
      } 
      else { /* No attributes */
        size = 0;
      }

      parser->tagLevel++;
      result = parser->p.startHandler (parser->p.userData, element, attrs, size);
      
      /* Element rejected */
      if (result == WE_PRSR_APPLICATION_OUT_OF_MEMORY) {
        return WE_PRSR_ERROR_OUT_OF_MEMORY;
      }
      else if (result != WE_PRSR_APPLICATION_OK) {
        return WE_PRSR_ERROR_APPLICATION;
      }

      if (! (tok & 0x40)) { /* No content; empty element */
        goto End;
      }

      /* ------------------------------------------------------------  string */
    } 
    else if (tok == 3) { /* Inline */
      char* q = parser->strchr_funct (s, 0);
      if (q >= end - (parser->strchr_funct == we_dec_wstrchr)) { /* Sentinel stopped the search */
        return WE_PRSR_STATUS_MORE;
      }
      error = we_dec_writeString (parser, &s, q, TRUE);
      if (error) {
        return error;
      }
    } 
    else if (tok == 0x83) { /* Table */
      error = we_dec_writeTableString (parser, &s, end, TRUE);
      if (error) {
        return error;
      }

    } 
    else if (tok == 1) { /* ---------------------------------------  end */
End:
      error = we_prsr_flushText (parser, TRUE);
      if (error) {
        return error;
      }
      error = parser->p.endHandler (parser->p.userData);
      if (error) {
        return error;
      }
      if (--parser->tagLevel == 0) {
        /* level -1 signals end of document */
        parser->tagLevel = -1;
        *nextPtr = s;
        /* 
         * A correct implementation should parse the epilogue at this stage,
         * but due to customer demands this phase is skipped. This prevents
         * the decoder from reporting errors when a stray 0x00 or 0x01 is found.
         *
         * parser->processor = we_dec_piDecoder;
         * return WE_PRSR_STATUS_RETURN;
         */
        return WE_PRSR_STATUS_OK;
      }

    } 
    else if (tok > 2 && (tok & 0x3F) <= 2) { /* -------------------  extension */
Extension:
      if (parser->d.specific) {
        error = parser->d.specific (parser, &s, end, tok, WE_DEC_ELEMENT_EXTENSION);
        if (error) {
          return error;
        }
      }
    } 
    else if (tok == 2) { /* ---------------------------------------  entity */
      error = we_dec_get_mb_uint32 (&s, end, &i);
      if (error) {
        return error;
      }

      size = we_charset_unicode_to_utf8 (i, entityStr);
      if (size == -1) {
        return WE_PRSR_ERROR_SYNTAX;
      }
      we_prsr_writeData (parser, entityStr, size, TRUE);
      if (parser->stack.error) {
        return parser->stack.error;
      }
    } 
    else if (tok == 0xC3) { /* ------------------------------------  opaque */
      error = we_dec_get_mb_uint32 (&s, end, &i);
      result = (int) i;
      if (error || result < 0) {
        return error;
      }

      size = MIN (result, end - s);
      if (parser->d.specific) {
        parser->d.specific (parser, &s, NULL, result, WE_DEC_ELEMENT_OPAQUE);
      }
      result -= size;
      s      += size;
      if (result > 0) {
        parser->opaqueLen = result;
        *nextPtr = s;
        return WE_PRSR_STATUS_MORE;
      }
    } 
    else if (tok == 0) { /* ---------------------------------------  switch page */
      if (s + 2 >= end) {
        return WE_PRSR_STATUS_MORE;
      }
      parser->elemPage = (unsigned short) ((*(unsigned char*) s++) << 6); /* 64 per page */

      tok = *(unsigned char*) s++;
      if (s >= end) {
        return WE_PRSR_STATUS_MORE;
      }

      if ((tok & 0x3F) >= 4) {
        goto Element;
      }
      else if (tok > 2 && (tok & 0x3F) <=2) {
        goto Extension;
      }
      else {
        return WE_PRSR_ERROR_SYNTAX;
      }
    } 
    else {/* ------------------------------------------------------  pi */
      error = we_dec_doPI (parser, s, end, nextPtr);
      if (error) {
        return error;
      }
      s = *nextPtr;
    }

    *nextPtr = s;
  }

  return WE_PRSR_STATUS_PAUSE;
}



/*
 * Decode a processing instruction. This function does not do
 * anything with the PI. It is just ignored. '*nextPtr' is still
 * updated.
 */
static int
we_dec_doPI (we_prsr_parser_t* parser, const char* s, const char* end, const char* *nextPtr)
{
  we_prsr_attribute_t* attrs; 
  int                   error;
  int                   size;

  *nextPtr = NULL; /* Signals count-only mode */
  error = we_dec_decodeAttributes (parser, s, end, nextPtr, &size, &attrs);
  if (error) {
    return error;
  }
  if (size > 1) { /* Must be 1, or 0 if the target is not a known attribute (most likely) */
    return WE_PRSR_ERROR_SYNTAX;
  }
  return WE_PRSR_STATUS_OK;
}



/*
 * Decode zero or more processing instructions.
 *
 * This process is used both for the epilogue and the start of
 * the document content.
 */
static int
we_dec_piDecoder (we_prsr_parser_t*  parser,
                   const char*         s,
                   const char*         end,
                   const char*        *nextPtr,
                   int                 limit)
{
  const char* start;
  int         error;

  *nextPtr = s;
  start = s;
  while (s - start < limit) {
    if (s >= end) {
      if (parser->tagLevel == -1) {/* This is the epilogue */
        return WE_PRSR_STATUS_OK;
      }
      else {
        return WE_PRSR_STATUS_MORE;
      }
    }

    if (*s != 0x43) { /* No more PIs */
      *nextPtr = s;
      if (parser->tagLevel == -1) { /* This is the epilogue */
        return WE_PRSR_ERROR_SYNTAX;
      }
      else {
        /* Verify that this is the beginning of valid content.
         * The body must starts with
         * [SWITCH_PAGE u_int8] (TAG | LITERAL_*) */

        if (*s == 0) { /* switch page */
          s += 2;
          if (s >= end) {
            return WE_PRSR_STATUS_MORE;
          }
        }
        if ((*(unsigned char*) s & 0x3F) >= 4) { /* tag (>4) or literal (4) */
          parser->processor = we_dec_contentDecoder;
          return WE_PRSR_STATUS_RETURN;
        } 
        else {
          return WE_PRSR_ERROR_SYNTAX;
        }
      }
    }

    /* Processing instruction */
    s++;
    
    error = we_dec_doPI (parser, s, end, nextPtr);
    if (error) {
      return error;
    }
    s = *nextPtr;
  }
  *nextPtr = s;
  return WE_PRSR_STATUS_PAUSE;
}

