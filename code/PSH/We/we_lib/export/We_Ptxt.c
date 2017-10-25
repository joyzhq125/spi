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
 * We_Ptxt.c
 *
 * XML and HTML parser: text parser and main functions
 */

#include "We_Prsr.h"
#include "We_Lib.h"
#include "We_Chrt.h"
#include "We_Chrs.h"
#include "We_Mem.h"
#include "We_Pipe.h"
#include "We_Ptxt.h"
#include "We_Ptok.h"
#include "We_Pdec.h"
#include "We_Log.h"

 /*
 * Deciding what character set to use
 * ----------------------------------
 * Five sources of information is used in this order:
 * 1, HTTP header
 * 2. we_prsr_guess_encoding
 * 3. XML PI encoding attribute
 * 4. Meta data (HTML only)
 * 5. Default character set
 *
 * When the charset is decided, WE_PRSR_STATUS_NEW_CHARSET is
 * returned to prsr_parse and the the character set given by
 * parser->charset is used. If it is WE_CHARSET_UNKNOWN then the
 * parser->defaultCharset is used instead.
 *
 * After the charset has been decided, all subsequent information
 * is ignored. The decision is final.
 *
 * The charset decision is made
 * 1. As soon as any of the above listed sources gives a charset.
 * 2. When a the document is identified as a XML document. This
 *    is when whatever comes first of:
 *    A. A XML PI is found
 *    B. The doctype says this is a XML document
 *    C. The prologue has ended and the default
 *       document type is used and this is a XML
 *       document.
 * 3. When a HTML document body is found. This msans there is no
 *    meta element with charset information.
 * 
 * The default charset can be any charset, but since a part of
 * the document may have been parsed already, the default
 * charset should be an ASCII based one. If it wasn't ASCII
 * based then it wouldn't be possible to parse the prologue.
 */

/**************************************************************
 * Temporary constants                                        *
 **************************************************************/

#define WBS_HTML_ELEMENT_A             0x1C
#define WBS_HTML_ELEMENT_BODY          0x05
#define WBS_HTML_ELEMENT_DD            0x09
#define WBS_HTML_ELEMENT_DIR           0x4E
#define WBS_HTML_ELEMENT_DL            0x0C
#define WBS_HTML_ELEMENT_DT            0x0D
#define WBS_HTML_ELEMENT_FORM          0x0E
#define WBS_HTML_ELEMENT_FORM_END      0x59
#define WBS_HTML_ELEMENT_H1            0x0F
#define WBS_HTML_ELEMENT_H2            0x10
#define WBS_HTML_ELEMENT_H3            0x11
#define WBS_HTML_ELEMENT_H4            0x12
#define WBS_HTML_ELEMENT_H5            0x13
#define WBS_HTML_ELEMENT_H6            0x14
#define WBS_HTML_ELEMENT_HEAD          0x2C
#define WBS_HTML_ELEMENT_HTML          0x15
#define WBS_HTML_ELEMENT_LI            0x18
#define WBS_HTML_ELEMENT_MENU          0x50
#define WBS_HTML_ELEMENT_META          0x30
#define WBS_HTML_ELEMENT_OL            0x3A
#define WBS_HTML_ELEMENT_OPTION        0x35
#define WBS_HTML_ELEMENT_P             0x20
#define WBS_HTML_ELEMENT_PLAINTEXT     0x4B
#define WBS_HTML_ELEMENT_SCRIPT        0x54
#define WBS_HTML_ELEMENT_SELECT        0x37
#define WBS_HTML_ELEMENT_STYLE         0x1A
#define WBS_HTML_ELEMENT_STYLE         0x1A
#define WBS_HTML_ELEMENT_TABLE         0x1F
#define WBS_HTML_ELEMENT_TD            0x1D
#define WBS_HTML_ELEMENT_TEXTAREA      0x44
#define WBS_HTML_ELEMENT_TH            0x45
#define WBS_HTML_ELEMENT_TITLE         0x46
#define WBS_HTML_ELEMENT_TR            0x1E
#define WBS_HTML_ELEMENT_UL            0x47
#define WBS_HTML_ELEMENT_BR            0x26
#define WBS_HTML_ELEMENT_WML           0x3F
#define WBS_HTML_ELEMENT_IMG           0x2E
#define WBS_HTML_ELEMENT_INPUT         0x2F

#define WBS_HTML_ATTRIBUTE_CONTENT         0x0F
#define WBS_HTML_ATTRIBUTE_HTTP_EQUIV      0x18

/**************************************************************
 * Constants                                                  *
 **************************************************************/

/***************  Configuration constants  ***************/

/* Initial out text buffer size. */
#define WE_PRSR_CFG_INITIAL_STACK_SIZE     512

/*
 * Minimum free space in text out-buffer before expanding. A siall value may
 * cause internal text fragmentation. A large value may cause a slight
 * increase in memory usage during parsing.
 */
#define WE_PRSR_CFG_MIN_FREE_STACK         200 

/* 
 * WE_PRSR_CFG_TIME_SLOT     Bytes parsed in each iteration. Msasured by the
 *                            the size of the internal UTF-8 representation.
 *
 * WE_PRSR_CFG_BUF_MARGIN    In each iteration WE_PRSR_CFG_TIME_SLOT bytes plus some
 *                            excess data is parsed. The size of the excess varies.
 *                            A siall margin increases the risk of a buffer underrun
 *                            but a large margin leaves more unparsed data. Both
 *                            alternatives cost time: the first causes reparsing
 *                            of some data and the latter copying of unparsed data.
 *
 * WE_PRSR_CFG_MIN_FREE_BUF  If the free buffer size is less then this constant
 *                            when forced to load more data, then it will be expanded.
 *
 * Initial buffer size == WE_PRSR_CFG_TIME_SLOT + WE_PRSR_CFG_BUFFER_MARGIN * 2.
 * WE_PRSR_CFG_MAX_XML_DECL_LEN  <= buffer initial size.
 *
 * Large values will use up more memory during parsing, but siall value may cause
 * unnecessary adaptation of the buffer size, causing reparsing of some data. This
 * cost is comparable siall, at least for larger documents.
 */
#define WE_PRSR_CFG_TIME_SLOT         400
#define WE_PRSR_CFG_BUF_MARGIN        50
#define WE_PRSR_CFG_MIN_FREE_BUF      50


#define WE_PRSR_BUFFER_SIZE      (WE_PRSR_CFG_TIME_SLOT + WE_PRSR_CFG_BUF_MARGIN * 2) 
#define WE_PRSR_BUFFER_INC_SIZE  (WE_PRSR_CFG_TIME_SLOT + WE_PRSR_CFG_BUF_MARGIN) 
#define WE_PRSR_MAX_NBR_OF_ATTRS 20


/***************  XML declaration attributes  ***************/

static const we_strtable_entry_t we_prsr_XmlDeclAttrs_stringTable[] = {
  {"encoding", 0}
};

static const unsigned char we_prsr_XmlDeclAttrs_hashTable[] = {
  255, 255, 0
};

const we_strtable_info_t we_prsr_XmlDeclAttrs = {
  we_prsr_XmlDeclAttrs_hashTable,
  sizeof we_prsr_XmlDeclAttrs_hashTable,
  WE_TABLE_SEARCH_INDEXING,
  we_prsr_XmlDeclAttrs_stringTable,
  sizeof we_prsr_XmlDeclAttrs_stringTable / sizeof we_prsr_XmlDeclAttrs_stringTable[0]
};


static const WE_UINT16 we_prsr_PiAttributeTypes[] = {
  WE_PRSR_TYPE_CDATA
};


/***************  Windows character codes  ***************/

/*
 * Character 128 - 159 are unallocated in Unicode,
 * but reused in Windows.
 */
const WE_UINT16 we_prsr_Windows2Unicode[] = {
  0x20AC, 0x0081, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
  0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0x008D, 0x017D, 0x008F,
  0x0090, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
  0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0x009D, 0x017E, 0x0178
};


/***************  XML entities  ***************/

static const we_strtable_entry_t we_prsr_XmlEntities_stringTable[] = {
  {"amp",    0x0026}, /* ampersand */
  {"apos",   0x0027}, /* apostrophe */
  {"gt",     0x003E}, /* greater-than sign */
  {"lt",     0x003C}, /* less-than sign */
  {"quot",   0x0022}  /* quotation mark = APL quote */
};

static const unsigned char we_prsr_XmlEntities_hashTable[] = {
  0, 255, 255, 4, 3, 1, 2
};

const we_strtable_info_t we_prsr_XmlEntities = {
  we_prsr_XmlEntities_hashTable,
  sizeof we_prsr_XmlEntities_hashTable,
  WE_TABLE_SEARCH_LINEAR,
  we_prsr_XmlEntities_stringTable,
  sizeof we_prsr_XmlEntities_stringTable / sizeof we_prsr_XmlEntities_stringTable[0]
};



/**************************************************************
 * Macro definitions                                          *
 **************************************************************/

#define WE_PRSR_LOG_MSG(x) WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, parser->modId, x))

#ifdef WE_PRSR_LOG
#define WE_PRSR_LOG_ERROR(parser, s, error) we_prsr_logError (parser, s, error)
#else
#define WE_PRSR_LOG_ERROR(parser, s, error)
#endif


 
/**************************************************************
 * Type definitions                                           *
 **************************************************************/

typedef struct {
  WE_UINT32 pattern;          /* Pattern found in buffer */
  WE_UINT8  usedLen;          /* Number of chars in the buffer acutually decoded */
  int        charset;          /* The charset connected to this pattern */

} we_prsr_encoding_t;


typedef int we_prsr_str2int_t (const char *name, int length, const we_strtable_info_t *info);


/**************************************************************
 * Local function declarations                                *
 **************************************************************/

static int
we_prsr_initProcess (we_prsr_parser_t*  parser,
                      const char*         s,
                      const char*         end,
                      const char*        *nextPtr,
                      int                 limit);
static int
we_prsr_prologueParser (we_prsr_parser_t*  parser,
                         const char*         s,
                         const char*         end,
                         const char*        *nextPtr,
                         int                 limit);
static int 
we_prsr_contentParser (we_prsr_parser_t*  parser,
                        const char*         s,
                        const char*         end,
                        const char*        *nextPtr,
                        int                 limit);

static int
we_prsr_cdataSectionParser (we_prsr_parser_t*  parser,
                             const char*         s,
                             const char*         end,
                             const char*        *nextPtr,
                             int                 limit);

static int
we_prsr_meta_scan_parser (we_prsr_parser_t*  parser,
                           const char*         s,
                           const char*         end,
                           const char*        *nextPtr,
                           int                 limit);
static int
we_prsr_startTagHandler (we_prsr_parser_t*  parser,
                          const char*         s,
                          const char*         end,
                          const char*        *nextPtr,
                          int                 endOfDoc);

static int
we_prsr_elemTagParser (we_prsr_parser_t*     parser,
                        const char*            s,
                        const char*            end,
                        const char*           *nextPtr,
                        const char*           *name,
                        int                   *element,
                        we_prsr_attribute_t* *attrs,
                        int                   *size,
                        int                    pi,
                        int                    endOfDoc);

static int
we_prsr_newSession (we_prsr_parser_t* parser, int mode);

static int
we_prsr_endSession (we_prsr_parser_t* parser);

#ifdef WE_PRSR_LOG
void
we_prsr_logError (we_prsr_parser_t* parser, const char* s, const char* error);
#endif



/**************************************************************
 * Functions                                                  *
 **************************************************************/

/* ======== utility functions ======== */

/*
 * Examine the first four bytes of the document to guess
 * what encoding should be used from now on. More precise 
 * (and maybe conflicting) information may be found in the XML
 * declaration.
 *
 * 's' is a pointer the input buffer. '*lenght' returns the number
 * of character that must be desposed from the input buffer. This
 * is to compensate for a possible byte order mark.
 *
 * This function recognizes two things: BOM and initial "<?xm" or
 * "@cha".
 *
 * Using an ASCII based charset is enough to decode the XML
 * declaration. There more specific information may be found. Any
 * ASCII based charset can be used: ASCII, UTF-8, ISO-8859, S-JIS
 * and so on.
 *
 * If 'type' == 0 then this is a XML document, else this is a CSS
 * document. The differences are that in the latter case the search
 * string "@cha" is used instead and that ASCII based document
 * returns WE_CHARSET_ASCII.
 */
int
we_prsr_guess_encoding (const char* s, int *length, int type)
{
  static const we_prsr_encoding_t v_css[] = {
    {0x00000000, 0, WE_CHARSET_UNKNOWN}, /* 0 default                    */
    {0x00400063, 0, WE_CHARSET_UTF_16BE},/* 1 UTF-16BE    "@c"           */
    {0x40006300, 0, WE_CHARSET_UTF_16LE},/* 2 UTF-16LE    "@c"           */
    {0x40636861, 0, WE_CHARSET_UNKNOWN}, /* 3 ASCII based charset "@cha" */
    {0xEFBBBF00, 3, WE_CHARSET_UTF_8},   /* 4 UTF-8       BOM            */
    {0xFEFF0000, 2, WE_CHARSET_UTF_16BE},/* 5 UTF-16BE    BOM            */
    {0xFFFE0000, 2, WE_CHARSET_UTF_16LE} /* 6 UTF-16LE    BOM            */
  };

  static const we_prsr_encoding_t v_xml[] = {
    {0x00000000, 0, WE_CHARSET_UNKNOWN}, /* 0 default                    */
    {0x003C003F, 0, WE_CHARSET_UTF_16BE},/* 1 UTF-16BE    "<?"           */
    {0x3C003F00, 0, WE_CHARSET_UTF_16LE},/* 2 UTF-16LE    "<?"           */
    {0x3C3F786D, 0, WE_CHARSET_UNKNOWN}, /* 3 ASCII based charset "<?ml" */
    {0xEFBBBF00, 3, WE_CHARSET_UTF_8},   /* 4 UTF-8       BOM            */
    {0xFEFF0000, 2, WE_CHARSET_UTF_16BE},/* 5 UTF-16BE    BOM            */
    {0xFFFE0000, 2, WE_CHARSET_UTF_16LE} /* 6 UTF-16LE    BOM            */
  };

  /*{0xEFBBBF--, 3, WE_CHARSET_UTF_8}     * 4 EF BB BF                   */
  /*{0xFEFF####, 2, WE_CHARSET_UTF_16BE}  * 5 FE FF non-null non-null    */
  /*{0xFFFE####, 2, WE_CHARSET_UTF_16LE}  * 6 FF FE non-null non-null    */

  const we_prsr_encoding_t* v;
  WE_UINT32                 val;
  WE_UINT32                 t;
  int                        i;

  t = 0;
  for (i = 0; i < 4; i++) {
    t <<= 8;
    if (type)
      t += we_ascii_lc[(unsigned char) s[i]];
    else
      t += (unsigned char) s[i];
  }

  if (type)
    v = v_css;
  else
    v = v_xml;

  /* Find 't' in 'v' or if no match, index of the largest element
   * sialer than 't' */
  if (t > v[2].pattern)
    i = 3;
  else
    i = 0;

  if (t >= v[i + 2].pattern)
    i = i + 2;
  if (t >= v[i + 1].pattern)
    i = i + 1;
  
  /* Handle special cases (4, 5, 6) */  
  if (i == 4) {
    if ((t & 0xffffff00UL) != 0xefbbbf00UL)
      i = 0;
  } else if (i >= 5) { /* 5 or 6 */
    if (i == 5)
      val = 0xfeff0000UL;
     else
      val = 0xfffe0000UL;

    if ((t & 0xffff0000UL) != val || (t & 0xffffUL) == 0x0000) {
     i = 0;
    }
  } else
    if (v[i].pattern != t)
      i = 0;

  if (i == 0) {
    /* Default: see if UTF-16 fits. The XML specification says
     * UTF-16 is to be used only if the document starts with a
     * UTF-16 BOM, else use UTF-8. To increse the flexibility
     * these two cases are allowed. */
    if ((t & 0xff000000UL) == 0)
      i = 1;
    else if ((t & 0x00ff0000UL) == 0)
      i = 2;
  }

  *length = (int) v[i].usedLen;
  return v[i].charset;
}


/*
 * Write an entity reference to the stack. An invalid
 * referens is treated as text data.
 *
 * 's' points to "&" and 'len' is the total length of the
 * reference, including "&" and ";".
 *
 * Returns an error code.
 *
 * &reference;
 * |          |
 * s          s + len
 *
 * &reference
 * |         |
 * s         s + len
 */
static int
we_prsr_writeEntityRef (we_prsr_parser_t* parser, const char* s, int len)
{
  char         entityStr[4];
  int          ch;
  int          size;
  int          refLen;

  refLen = len - 1 - (s[len - 1] == ';');
  ch = we_cmmn_str2int (s + 1, refLen, parser->p.entities);
  if (ch == -1 && parser->mode != WE_PRSR_XML_MODE)
    ch = we_cmmn_str2int_lc (s + 1, refLen , parser->p.entities);

  if (ch == -1)
    return we_prsr_writeString (parser, s, len);

  size = we_charset_unicode_to_utf8 (ch, entityStr);
  return we_prsr_writeString (parser, entityStr, size);
}



/*
 * Write a character reference to the stack. An invalid
 * referens is treated as text data.
 *
 * 's' points to "&" and 'len' is the total length of the
 * reference, including "&#" and ";".
 *
 * &#char_ref;
 * |          |
 * s          s + len
 *
 * &#char_ref
 * |         |
 * s         s + len
 */
static int
we_prsr_writeCharacterRef (we_prsr_parser_t* parser, const char* s, int len)
{
  char         entityStr[4];
  WE_UINT32   ch;
  int          size;

  ch = we_tok_charRefNumber (s);

  if (ch == 0xFFFF)
    return we_prsr_writeString (parser, s, len);

  if (ch >= 128 && ch <= 159)
    ch = (WE_UINT32) we_prsr_Windows2Unicode[ch - 128];

  size = we_charset_unicode_to_utf8 (ch, entityStr);
  return we_prsr_writeString (parser, entityStr, size);
}



/*
 * Deallocates an attribute vector and its data.
 */
void
we_prsr_free_attributes (WE_UINT8 modId, we_prsr_attribute_t* attrs, int size)
{
  int i;

  if (size == 0)
    return;

  for (i = 0; i < size; i++)
    if (attrs[i].flag == WE_PRSR_ATTRVAL_STR)
      WE_MEM_FREE (modId, attrs[i].u.s);
  WE_MEM_FREE (modId, attrs);
}



/*
 * Extract the charset from a http-equiv content string.
 *
 * Format: "text/html; charset=US-ASCII"
 *
 * Returns the charset found or -1 if no charset was given.
*/
static int
we_prsr_getCharsetFromString (char* s)
{
  const char* p;
  const char* end;
  int         charset;

  we_cmmn_strcpy_lc (s, s); /* Downcase so strstr works */
  p = strstr (s, "charset");
  if (p == NULL)
    return -1; /* No charset was given */
  
  for (;;) { /* dummy loop */
    p += 7;
    while (! ct_isalphanum (*p) && *p != '-' && *p != '_' && *p != '.' && *p != '_')
      p++;
    if (*p == '\0')
      break;
    
    end = p;
    while (ct_isalphanum (*end) || *end == '-' || *end == '_' || *end == '.' || *end == '_')
      end++;

    charset = we_charset_str2int (p, end - p);
    if (charset >= 0)
      return charset;

    break;
  }

  return WE_CHARSET_UNKNOWN;
}



/* ======== parser functions ======== */

/*
 * Parse a section of the current document.
 *
 * Returns a status code.
 */
int
we_prsr_parse (we_prsr_parser_t* parser)
{
  char* next;
  char* temp;
  long  length;
  int   result;
  long  todo;
  int   bytesToProcess;
  int   forceRead;
  int   isOpen;
  long  available;

  if (parser == NULL)
    return WE_PRSR_ERROR_ABORTED;

  isOpen    = FALSE;
  available = 0;

  switch (parser->inputType) {
  case WE_PRSR_PIPE:
    WE_PIPE_STATUS (parser->handle, &isOpen, &available);
    break;
  case WE_PRSR_FILE:
    break;
  }

  bytesToProcess = WE_PRSR_CFG_TIME_SLOT;
  forceRead = parser->bufferStart == parser->buffer; /* Too wide condition, but ok */

  while (bytesToProcess > 0) {
    if (! parser->noMoreData) {
      /* See if a refill is needed */
      todo = (parser->bufferEnd - parser->bufferStart);
      if (todo < WE_PRSR_CFG_TIME_SLOT + WE_PRSR_CFG_BUF_MARGIN || forceRead) {
        /* Compact buffer and load more data */
        if (parser->bufferStart != parser->buffer) {
          /* Compact buffer, if not already done */
          memmove (parser->buffer, parser->bufferStart, todo);
          parser->bufferStart = parser->buffer;
          parser->bufferEnd = parser->buffer + todo;
        }

        if (parser->cvtFunct == NULL) { /* -----------  Unbuffered read */
          length = WE_PIPE_READ (parser->handle, parser->bufferStart + todo,
                                  parser->bufferSize - todo);
          if (length < 0) {
            switch (length) {
            case WE_PIPE_ERROR_CLOSED:
              isOpen = FALSE;
              available = 0;
              break;
            case WE_PIPE_ERROR_DELAYED:
              WE_PIPE_POLL (parser->handle);
              return WE_PRSR_STATUS_WAIT;
            default:
              return WE_PRSR_ERROR_ABORTED;
            }
          } else
            todo += length;

          parser->bufferEnd = parser->bufferStart + todo;

        } else { /* ----------------------------------  Buffered read */
          long srcLen;
          long dstLen;
          long srcUsed;
          long dstUsed;

          /* To be able to differ an erroneous character from a
           * half character at the end of the source buffer, a
           * arbitrary limit of 20 bytes is set. It is supposed
           * to larger than any input character.
           *
           * If no progess is shown and the source is almost
           * exhausted, this could be a legal half character or
           * an erroneous character. If no progress is shown with
           * enough input (> 20 bytes) than this can not be a half
           * character and must surely be an error. */
          dstLen = parser->bufferSize - todo;
          srcLen = parser->cvtBufLen;
          while (dstLen > 0) {
            switch (parser->inputType) {
            case WE_PRSR_PIPE:
              WE_PIPE_STATUS (parser->handle, &isOpen, &available);
              break;
            case WE_PRSR_FILE:
              break;
            }
            if (available <= 0 && srcLen <= 0)
              break;
            if (srcLen < 20 && available > 0) {
              /* Compact and fill cvtBuf */
              memmove (parser->cvtBuf, parser->cvtBufStart, srcLen);
              parser->cvtBufStart = parser->cvtBuf;
              length = WE_PIPE_READ (parser->handle, parser->cvtBufStart + srcLen,
                                      parser->cvtbufSize - srcLen);
              if (length < 0) {
                switch (length) {
                case WE_PIPE_ERROR_CLOSED:
                  isOpen = FALSE;
                  available = 0;
                  break;
                case WE_PIPE_ERROR_DELAYED:
                  WE_PIPE_POLL (parser->handle);
                  return WE_PRSR_STATUS_WAIT;
                default:
                  return WE_PRSR_ERROR_ABORTED;
                }
              } else
                  srcLen += length;
            }

            srcUsed = srcLen;
            dstUsed = dstLen;
            result = parser->cvtFunct (parser->cvtBufStart, &srcUsed,
                                       parser->bufferStart + todo, &dstUsed);
            if (result != 0)
              return WE_PRSR_ERROR_UNKNOWN_CHARSET; 
            if (dstUsed == 0) {
              if (dstLen <= 3)
                /* Less than maximal size of one UTF-8 character left.
                 * The destination did not fit a multibyte character. */
                break;
              if (srcLen >= 20)
                /* There is enough space to write a character and there is
                 * enough input to read a character, but no progress is done.
                 * This indicates an unwanted halt by a erroneous character.
                 * A correct convert function should return an error code or
                 * skip this character. */
                 return WE_PRSR_ERROR_UNKNOWN_CHARSET;

              switch (parser->inputType) {
              case WE_PRSR_PIPE:
                WE_PIPE_STATUS (parser->handle, &isOpen, &available);
                break;
              case WE_PRSR_FILE:
                break;
              }
              if (available == 0) {
                if (! isOpen) 
                  /* Cannot do anything with the data available and no more
                   * input will be available in the future = bad character in
                   * the last section. Skip the entire section and continue
                   * parsing. */
                   srcUsed = srcLen;
                else
                  /* Cannot do anything with the data available but more
                   * may be available in the future. */
                  break;
              }
            }
            parser->cvtBufStart += srcUsed;
            srcLen -= srcUsed;
            dstLen -= dstUsed;
            todo += dstUsed;
          }
          parser->bufferEnd = parser->bufferStart + todo;
          parser->cvtBufLen = srcLen;

        } /* ----------------------------------------- */

        if (todo < parser->bufferSize) {
          if (isOpen) {
            /* Pause if there is too little data in the buffer or
             * if the parser just asked for more input. This is to
             * avoid locking up the stream.
             *
             * If a muli-byte character is the last charcter to be
             * read to the buffer, and only a part of it fits, it
             * is not written. This causes a non-full buffer even
             * though there is more data available. The "> 3"
             * condition lets this case through.*/
            if (todo < WE_PRSR_CFG_TIME_SLOT + WE_PRSR_CFG_BUF_MARGIN ||
                (forceRead && parser->bufferSize - todo > 3)) {
              WE_PIPE_POLL (parser->handle);
              return WE_PRSR_STATUS_WAIT;
            }
          } else {
            if (parser->cvtBufLen == 0 && available == 0) {
              parser->noMoreData = TRUE;
              *parser->bufferEnd = '\0';
            }
          }
        }
        forceRead = FALSE;
      }
    }

Next_iteration:
    next = parser->noMoreData ? NULL : (parser->bufferStart + parser->meta_scan_offset);
    /* 
     * next == NULL is used to signal end of data, else
     * next is set start of the buffer. This gives next
     * a correct default return value.
     */
    
    result = parser->processor (parser,
                                parser->bufferStart + parser->meta_scan_offset,
                                parser->bufferEnd,
                                (const char**) &next,
                                bytesToProcess);
    bytesToProcess -= next - (parser->bufferStart + parser->meta_scan_offset);
    if (parser->meta_scan) {
      parser->meta_scan_offset = next - parser->bufferStart;
      next = parser->bufferStart;
    }
    parser->bufferStart = next;
    todo = parser->bufferEnd - next;

    switch (result) {
    case WE_PRSR_STATUS_OK: /* ----------------------  Finished */
      if (parser->meta_scan) {
        /* Terminate the meta charset scan and retry parsing */
        goto New_charset;
      }
      return WE_PRSR_STATUS_PARSING_COMPLETED;

    case WE_PRSR_STATUS_NEW_CHARSET: /* -------------  Change character set */
New_charset:
      /* Change character set */
      WE_PRSR_LOG_ERROR (NULL, NULL, "Change charset");
      for (;;) { /* Dummy loop */
        we_charset_convert_t* cvtFunct;
        int charset;

        if (parser->meta_scan) {
          /* Meta charset scan done. Restart parsing from parser->bufferStart. */
          parser->meta_scan        = FALSE;
          parser->meta_scan_offset = 0;
          parser->processor        = we_prsr_contentParser;
          parser->returnProc       = we_prsr_contentParser;
          parser->stackmode        = STACKMODE_NO_TEXT;
          parser->state            = WE_PRSR_STATE_CONTENT;
        }
        charset = parser->charset;
        if (charset == WE_CHARSET_UNKNOWN) {
          charset = parser->defaultCharset;
          parser->charset = charset;
        }

        cvtFunct = we_charset_find_function (charset, WE_CHARSET_UTF_8);
        if (cvtFunct == NULL) {
          /* The charset to use is not registered. Use a ASCII-
           * based charset defined for all values instead. */
          cvtFunct = we_charset_find_function (WE_CHARSET_LATIN_1, WE_CHARSET_UTF_8);
          
        }

        parser->cvtFunct    = cvtFunct;
        parser->cvtBuf      = parser->buffer;
        parser->cvtBufStart = parser->bufferStart;
        parser->cvtBufLen   = parser->bufferEnd - parser->bufferStart;
        parser->cvtbufSize  = parser->bufferSize;

        /* 3 sentinel elements for UTF-16 in WBXML */
        temp = (char*) WE_MEM_ALLOC (parser->modId, WE_PRSR_BUFFER_SIZE + 3); 
        if (temp == NULL)
          return WE_PRSR_ERROR_OUT_OF_MEMORY;
        temp[WE_PRSR_BUFFER_SIZE]     = '\0';
        temp[WE_PRSR_BUFFER_SIZE + 1] = '\0';
        temp[WE_PRSR_BUFFER_SIZE + 2] = '\0'; /* Extra sentinel for malformatted UTF-16 */

        parser->buffer      = temp;
        parser->bufferStart = temp;
        parser->bufferEnd   = temp;
        parser->bufferSize  = WE_PRSR_BUFFER_SIZE;

        /* The pipe/file may be exhausted, but now there is new data */
        parser->noMoreData  = FALSE;
        break;
      }

      if (parser->state >= WE_PRSR_STATE_DOCTYPE) {
        /* The init function has already been called, but with CHARSET_UNKNOWN.
         * Call again, but with the correct charset (negated to indicate update). */
        result = parser->initFunct (&parser->p, NULL, NULL, 0, 0, - parser->charset);
        if (result != WE_PRSR_APPLICATION_OK)
          return WE_PRSR_ERROR_APPLICATION;
      }
      break;

    case WE_PRSR_STATUS_MORE: /* --------------------  Need more data */
      if (parser->noMoreData) {
        if (parser->meta_scan) {
          /* Terminate the meta charset scan and retry parsing */
          goto New_charset;
        }
        WE_PRSR_LOG_ERROR (parser, NULL, "Unexpected end of document");
        return WE_PRSR_ERROR_UNEXPECTED_END_OF_DOC;
      }

      /* Only a part of the element text found in the buffer; load more data. */
      WE_PRSR_LOG_ERROR (NULL, NULL, "Buffer underrun");
      
      forceRead = TRUE;

      if (parser->bufferSize - todo < WE_PRSR_CFG_MIN_FREE_BUF) {
        /* Increase buffer size and compress */
        WE_PRSR_LOG_ERROR (NULL, NULL, "Buffer expanded");
        parser->bufferSize += WE_PRSR_BUFFER_INC_SIZE;

        /* 3 sentinel elements for UTF-16 in WBXML */
        temp = (char*) WE_MEM_ALLOC (parser->modId, parser->bufferSize + 3);
        if (temp == NULL)
          return WE_PRSR_ERROR_OUT_OF_MEMORY;
        temp[parser->bufferSize]     = '\0';
        temp[parser->bufferSize + 1] = '\0';
        temp[parser->bufferSize + 2] = '\0'; /* Extra sentinel for malformatted UTF-16 */

        memcpy (temp, next, todo);
        parser->bufferEnd = temp + todo;
        parser->bufferStart = temp;
        WE_MEM_FREE (parser->modId, parser->buffer);
        parser->buffer = temp;
      }
      break;
    
    case WE_PRSR_STATUS_PAUSE: /* -------------------  Scheduled pause */
      WE_PRSR_LOG_ERROR (NULL, NULL, "Scheduled pause");
      return WE_PRSR_STATUS_MORE_TO_DO;

    case WE_PRSR_STATUS_RETURN: /* ------------------  All went well, but there is more to do */
      goto Next_iteration;
      
    case WE_PRSR_STATUS_META_SCAN: /* ---------------  Start scanning for meta charset */
      /* The parsing is resumed at parser->bufferStart. During the
       * meta scan session, the buffer is using a look-ahead of
       * parser->meta_scan_offset bytes. */
      parser->meta_scan  = TRUE;
      parser->processor  = we_prsr_meta_scan_parser;
      parser->returnProc = we_prsr_meta_scan_parser;
      break;

    default: /* --------------------------------------  An error occured */
      if (parser->meta_scan && result != WE_PRSR_ERROR_OUT_OF_MEMORY) {
        /* Terminate the meta charset scan and retry parsing */
        goto New_charset;
      }
      return result;
    }      
  }
  WE_PRSR_LOG_ERROR (NULL, NULL, "Scheduled pause");
  return WE_PRSR_STATUS_MORE_TO_DO;
}



/*
 * Instantiate a new parser. 
 *
 * modId          - Module wid of the caller. Used for memory allocations.
 *
 * userData       - Pointer to opaque data. Not used by the parser, only passed on with
 *                  all call-backs. The caller can use this to save any information.
 *                  A typical use is to give a pointer to a struct containing data such
 *                  as nestling depth and the structure the parsed information is saved
 *                  in. 'userData' is also returned from a call to we_prsr_delete and
 *                  it is the users responsibility to deallocate all data.
 *
 * initFunct      - Pointer to the function called when the document prologue is parsed.
 *
 * piHandler      - Pointer to function called when a PI is found.
 *
 * type           - Type of input source: WE_PRSR_PIPE, WE_PRSR_FILE or WE_PRSR_BUFFER.
 *
 * document       - If 'type' is WE_PRSR_BUFFER, then this is a pointer to a complete
 *                  document. Must be terminated by a null character. This msans a UCS-2
 *                  or UTF-16 document must be terminated by double null bytes. This
 *                  concerns both text and WBXML documents.
 *
 * length         - If 'type' is WE_PRSR_BUFFER, then this is the document length
 *                  excluding the null-termination.
 *
 * handle         - If 'type' is WE_PRSR_PIPE or WE_PRSR_FILE, then this is a handle
 *                  to a pipe or file.
 *
 * bufferSize     - Buffer initial size. All other memory use parameters are scaled from
 *                  this parameter. This is only the initial size of the input buffer
 *                  and not an upper limit of memory usage. If needed any of the buffers
 *                  used can be expanded. Some less than 'bufferSize' data is parsed at
 *                  each call to we_prsr_parse, if available.
 *
 * charset        - Character set of the input, or WE_CHARSET_UNKNOWN if unknown.
 *
 * defaultCharset - The default charset to use if 'charset' is WE_CHARSET_UNKNOWN and
 *                  the parser could neither get the charset from the document nor guess
 *                  it.
 *
 * textMode       - TRUE if this is a text document or FALSE if this is a WBXML document.
 *
 * Returns a pointer to a new parser object. If out of memory, NULL is
 * returned.
 *
 * Input comes from either a pipe, a file or as a complete doucument in a
 * buffer. 'type' tells the type of input. Can be either WE_PRSR_PIPE,
 * WE_PRSR_FILE or WE_PRSR_BUFFER.
 *
 * If 'type' equals WE_PRSR_BUFFER, then 'buffer' is used for input and
 * 'length' tells the buffer length _excluding_ any termination. The
 * buffer is deallocated by the parser, no later than when we_prsr_delete
 * is called. Text document must be NUL terminated.
 *
 * If 'type' equals WE_PRSR_PIPE or WE_PRSR_FILE then the input is read
 * from a pipe or file given by 'handle'. 
 *
 * When calling we_prsr_delete the 'userData', 'handle' and 'type'
 * attributes are returned. If used, it is then up to the caller to
 * close and maybe delete the pipe or file. The pipe name can be stored
 * in for instance 'userData'.
 * 
 * The XML 1.0 specification says the default charset is UTF-8, but
 * HTTP 1.1 says latin-1. Any charset can be used as default.
 */
we_prsr_parser_t*
we_prsr_create (WE_UINT8             modId,
                 void*                 userData,
                 we_prsr_init_t*      initFunct,
                 we_prsr_piHandler_t* piHandler,
                 int                   type,
                 char*                 document,
                 int                   length,
                 int                   handle,
                 int                   bufferSize,
                 int                   charset,
                 int                   defaultCharset,
                 int                   textMode)
{
  we_prsr_parser_t* parser;
  we_prsr_stack_t*  stack;
  char*              stackBuf;

  if (type < 0 || type > 2)
    return NULL;

  if (textMode)
    parser = (we_prsr_parser_t*) WE_MEM_ALLOCTYPE (modId, struct we_prsr_parser_lite_st);
  else
    parser = WE_MEM_ALLOCTYPE (modId, struct we_prsr_parser_st);

  if (parser == NULL)
    return NULL;

  if (type == WE_PRSR_BUFFER) { /* --------------------  Read from given buffer */
    if (textMode)
      parser->buffer   = document;
    else {
      /* 
       * Make sure the WBXML document is NUL terminated by copying
       * the document. This is only used as a sentinel to protect
       * the decoder from faulty documents.
       */
      parser->buffer   = (char*) WE_MEM_ALLOC (modId, length + 3);
    
      if (parser->buffer == NULL) {
        WE_MEM_FREE (modId, parser);
        return NULL;
      }
      memcpy (parser->buffer, document, length);
      WE_MEM_FREE (modId, document);
      parser->buffer[length]     = '\0'; /* Sentinel element */
      parser->buffer[length + 1] = '\0'; /* Extra sentinel for WBXML UTF-16 */
      parser->buffer[length + 1] = '\0'; /* Extra sentinel for malformatted UTF-16 */
    }
    parser->bufferEnd  = parser->buffer + length;
    parser->bufferSize = length;
    parser->noMoreData = TRUE;
    parser->handle     = -1;

  } else { /* ------------------------------------------  Read from pipe or file */
    /* 3 sentinel elements */
    parser->buffer     = (char*) WE_MEM_ALLOC (modId, WE_PRSR_BUFFER_SIZE + 3);

    if (parser->buffer == NULL) {
      WE_MEM_FREE (modId, parser);
      return NULL;
    }
    parser->bufferSize = WE_PRSR_BUFFER_SIZE;
    parser->bufferEnd  = parser->buffer;
    parser->noMoreData = FALSE;
    parser->handle     = handle;
    /* Add a terminating null character to the buffer to
     * mark end of buffer and guarantee the strings in the
     * buffer will be null terminated. */
    parser->buffer[WE_PRSR_BUFFER_SIZE]     = '\0'; /* Sentinel element */
    parser->buffer[WE_PRSR_BUFFER_SIZE + 1] = '\0'; /* Extra sentinel for WBXML UTF-16 */
    parser->buffer[WE_PRSR_BUFFER_SIZE + 2] = '\0'; /* Extra sentinel for malformatted UTF-16 */
  } /* ------------------------------------------------- */

  stackBuf = (char*) WE_MEM_ALLOC (modId, WE_PRSR_CFG_INITIAL_STACK_SIZE);
  if (stackBuf == NULL) {
    WE_MEM_FREE (modId, parser->buffer);
    WE_MEM_FREE (modId, parser);
    return NULL;
  }

  parser->p.userData     = userData;
  parser->tagLevel       = 0;
  parser->initFunct      = initFunct;
  parser->piHandler      = piHandler;
  parser->p.entities     = &we_prsr_XmlEntities;

  parser->bufferStart    = parser->buffer;
  parser->charset        = charset >= 0 ? charset : WE_CHARSET_UNKNOWN;
  parser->defaultCharset = defaultCharset >= 0 ? defaultCharset : WE_CHARSET_UNKNOWN;
  parser->modId          = modId;

  parser->mode           = WE_PRSR_HTML_MODE; /* This may or may not be an XML document */
  parser->html_based     = FALSE;
  parser->meta_scan      = FALSE;
  parser->meta_scan_offset = 0;
  parser->cvtFunct       = NULL;
  parser->cvtBuf         = NULL;
  parser->cvtBufLen      = 0;
  parser->inputType      = type;
  parser->initalSize     = bufferSize;


  if (textMode) {
    parser->processor   = we_prsr_initProcess;
    parser->state       = WE_PRSR_STATE_START;

  } else { /* WBXML */
    parser->elemPage    = 0;
    parser->attrPage    = 0;
    parser->opaqueLen   = 0;
    parser->state       = WE_PRSR_STATE_DECODER;
    parser->processor   = we_dec_prologueDecoder;
    parser->stringTable = NULL;
  }

  /* Initiate the stack */
  stack = &parser->stack;
  
  stack->start          = stackBuf;
  stack->end            = stackBuf + WE_PRSR_CFG_INITIAL_STACK_SIZE;
  stack->ptr            = stackBuf;
  stack->sessionStart   = stackBuf;
  stack->mode           = STACKMODE_NONE;
  stack->overflow       = FALSE;
  stack->cancelSpace    = FALSE;
  stack->cancelSpace_bu = FALSE;
  stack->downcase       = FALSE;
  stack->error          = WE_PRSR_STATUS_OK;

  return parser;
}



/*
 * HTML specific configuration. Only used by the WBS.
 */
void
we_prsr_html_config (we_prsr_parser_t*        parser,
                      const unsigned char*      htmlTable,
                      we_prsr_changeCharset_t* changeCharset)
{
  if (parser == NULL)
    return;

  parser->htmlTable     = htmlTable;
  parser->changeCharset = changeCharset;
}



/*
 * Delete a parser object. All internal data is deallocated. If
 * reading from a pipe or file, '*handle' is set to its handle
 * value, else '*handle' is set to -1. '*type' is set to the input
 * type: WE_PRSR_PIPE, WE_PRSR_FILE or WE_PRSR_BUFFER.
 * '*userData' returns userData.
 *
 * If reading from a pipe or file, it is the user's responsibility
 * to close and maybe delete it. The same goes for '*userData'.
 *
 * Returns a status code.
 */
int
we_prsr_delete (we_prsr_parser_t* parser, int *type, int *handle, void* *userData)
{
  if (parser == NULL)
    return WE_PRSR_ERROR_ABORTED;

  *userData   = parser->p.userData;
  *handle     = parser->handle;
  *type       = parser->inputType;

  WE_MEM_FREE (parser->modId, parser->stack.start);
  WE_MEM_FREE (parser->modId, parser->buffer);
  WE_MEM_FREE (parser->modId, parser->cvtBuf);

  if (parser->state == WE_PRSR_STATE_DECODER) { /* WBXML */
    if (parser->stringTable != NULL && 
        (parser->stringTable <= parser->buffer ||
        parser->stringTable > parser->bufferEnd)) { /* External string table */
      WE_MEM_FREE (parser->modId, parser->stringTable);
    }
  }
  
  WE_MEM_FREE (parser->modId, parser);
  
  return WE_PRSR_STATUS_OK;
}



/* ======== HTML specific functions ======== */

/*
 * Do HTML specific initialization.
 */
static void 
we_prsr_initHtml (we_prsr_parser_t* parser)
{
  parser->stack.downcase = TRUE;
  parser->stack.ptr += 40;
  parser->numElems = 20;
}



/*
 * Add a missing html element to the document.
 */
static void
we_prsr_addHtmlElement (we_prsr_parser_t* parser)
{
  /* Wbs_Hvld_startElement cannot reject; does not need to check return value. */
  parser->p.startHandler (parser->p.userData, WBS_HTML_ELEMENT_HTML, NULL, 0);
  parser->stack.sessionStart[0] = (char) WBS_HTML_ELEMENT_HTML;
  parser->stack.sessionStart[1] = (char) STACKMODE_NORMALIZE;
  parser->tagLevel = 1;
}



/*
 * If 'element_index' positive, close the element 'element_index'
 * if found and all elements above it. If negative, 'element_index'
 * is a negated index do an element. Close all elements above this 
 * element, including it.
 *
 * Returns the new cancelSpace and stackmode states or -1 if
 * noting is match anyting on the stack.
 *
 * If 'block' is TRUE, then no misiatched elements are reopened.
 *
 * '*cancelSpace' is != 0 if any of the closed elements has the
 * cancel space property. If no elements are closed, then
 * '*cancelSpace' == 0.
 *
 * The normalize property is inherited from the elements parent.
 */
static int
we_prsr_closeElement (we_prsr_parser_t* parser, int element_index, int block, int *cancelSpace)
{
  const unsigned char* htmlTable;
  char* elementStack;
  int   result;
  int   top;
  int   bottom;
  int   cancel;
  int   e;
  int   i;
  int   j;

  elementStack = parser->stack.start + 1;
  cancel = 0;
  htmlTable = parser->htmlTable;

  top = (parser->tagLevel - 1) << 1;
  
  if (element_index < 0) {
    i = -element_index;
    if (i > top) {
      *cancelSpace = 0;
      return -1;
    }
  } else {
    for (i = top; i >= 0; i -= 2)
      if (elementStack[i] == element_index) 
        break;
    if (i < 0) { 
      *cancelSpace = 0;
      return -1;
    }
  }

  for (j = top + 1; j > i; j -= 2) {
    /* Wbs_Hvld_endElement never rejects; does not need to check return value. */
    parser->p.endHandler (parser->p.userData);
    cancel |= elementStack[j] & WE_PRSR_FLAG_CANCEL_SPACE;
  }

  /* Inherit element with the PRSR_MISIATCH flag */
  if (i > 0 && ! block) {
    for (bottom = i, j = i + 2; j <= top; j += 2) {
      e = elementStack[j];
      if (htmlTable[e] & WE_PRSR_MISIATCH) {
        result = parser->p.startHandler (parser->p.userData, e, NULL, 0);
        if (result == WE_PRSR_APPLICATION_OK) {
          elementStack[bottom + 1] = (char)
                                     (elementStack[j + 1] & ~WE_PRSR_FLAG_NORMALIZE |
                                      elementStack[bottom - 1] & WE_PRSR_FLAG_NORMALIZE);
          elementStack[bottom]     = (char) e;
          bottom += 2;
        }
      }
    }
    i = bottom;
  }

  parser->tagLevel = i >> 1;
  *cancelSpace = cancel;
  return elementStack[i - 1] & ~WE_PRSR_FLAG_CANCEL_SPACE;
}



/*
 * Search the HTML element stack for 'elem1' or 'elem2'. Stop
 * the search when 
 * 1. 'elem1' or 'elem2' is found,
 * 2. 'stop1', 'stop2', 'stop3' or 'stop4' is found,
 * 3. an element with the PRSR_STOP flag is set is found and
 *    'elem1' == 'elem2'.
 * 4. the entire stack is searched
 *
 * Returns on stop condition
 * 1. index of the first found element
 * 2. if close, index of the element above stopping element else -1
 * 3. if close, index of the element above stopping element else -1
 * 4. -1
 */
static int
we_prsr_findMatchingStart (we_prsr_parser_t* parser, int elem1, int elem2,
                        int stop1, int stop2, int stop3, int stop4, int close)
{
  const unsigned char* htmlTable;
  char* elementStack;
  int   elem;
  int   i;

  elementStack = parser->stack.start + 1;
  htmlTable = parser->htmlTable;

  for (i = (parser->tagLevel - 1) << 1; i > 0; i -= 2) {
    elem = elementStack[i];
    if (elem == elem1 || elem == elem2)
      return i;
    else if ((elem == stop1 || elem == stop2 || elem == stop3 || elem == stop4) ||
             (elem1 == elem2 && (htmlTable[elem] & WE_PRSR_STOP))) {
      if (close)
        return i + 2;
      else
        return -1;
    }
  }
  return -1;
}



/*
 * Find and close an element matching the START tag 'element'.
 * Returns TRUE if any of the elements closed had the cancelSpace
 * property, thus FALSE if no elements have been closed. 
 *
 * A stop element takes a block element as a child.
 *
 * element        closes          stop search at first     stop closes elements
 * ----------------------------------------------------------------------------
 * block element  p               stop element             yes
 * block element  all elements    stop element             yes
 * form           all elements    stop element tr table    yes
 * li             li              ol ul dir menu           no
 * dd dt          dd dt           dl                       no
 * option         option          select                   no
 * tr             tr th td        table                    yes
 * th td          th td           tr table                 yes
 * a              a               -                        no
 */
static int
we_prsr_closeMatchingStart (we_prsr_parser_t* parser, int element)
{
  int cancelSpace;
  int close;
  int elem1;
  int elem2;
  int stop1;
  int stop2;
  int stop3;
  int stop4;
  int index;

  cancelSpace = FALSE;
  close = FALSE;
  elem1 = element;
  elem2 = -1;
  stop1 = -1;
  stop2 = -1;
  stop3 = -1;
  stop4 = -1;

  if (parser->htmlTable[element] & WE_PRSR_BLOCK) {
    elem1 = -1;
    close = TRUE;
    if (element == WBS_HTML_ELEMENT_FORM) {
      /* The form element can be in a table without closing it.
       * Elements stopping the search:
       * table th td tr thead tbody tfoot.
       * th and td are caught by the stop element check.
       * thead, tbody and tfoot are not supported. */
      stop1 = WBS_HTML_ELEMENT_TR;
      stop2 = WBS_HTML_ELEMENT_TABLE;
    }
  } else {
    switch (element) {
    case WBS_HTML_ELEMENT_LI:
      stop1 = WBS_HTML_ELEMENT_OL;
      stop2 = WBS_HTML_ELEMENT_UL;
      stop3 = WBS_HTML_ELEMENT_DIR;
      stop4 = WBS_HTML_ELEMENT_MENU;
      break;

    case WBS_HTML_ELEMENT_DD:
    case WBS_HTML_ELEMENT_DT:
      elem1 = WBS_HTML_ELEMENT_DT;
      elem2 = WBS_HTML_ELEMENT_DD;
      stop1 = WBS_HTML_ELEMENT_DL;
      break;

    case WBS_HTML_ELEMENT_OPTION:
      stop1 = WBS_HTML_ELEMENT_SELECT;
      break;

    case WBS_HTML_ELEMENT_TR:
      stop1 = WBS_HTML_ELEMENT_TABLE;
      close = TRUE;
      break;

    case WBS_HTML_ELEMENT_A:
      break;

    default: 
      /* WBS_HTML_ELEMENT_TH */
      /* WBS_HTML_ELEMENT_TD */
      elem1 = WBS_HTML_ELEMENT_TH;
      elem2 = WBS_HTML_ELEMENT_TD;
      stop1 = WBS_HTML_ELEMENT_TR;
      stop2 = WBS_HTML_ELEMENT_TABLE;
      close = TRUE;
      break;
    }
  }

  index = we_prsr_findMatchingStart (parser, elem1, elem2, stop1, stop2, stop3, stop4, close);
  if (index != -1)
    we_prsr_closeElement (parser, -index, TRUE, &cancelSpace);
                   
  return cancelSpace;
}



/*
 * Search the HTML element stack for 'elem1' or 'elem2'. Stop
 * the search when 
 * 1. 'elemement' is found,
 * 2. 'stop1', 'stop2', 'stop3' or 'stop4' is found,
 * 3. an element with the PRSR_HEADING flag is set is found and
 *    'element' < 0.
 * 4. the entire stack is searched
 *
 * Returns on stop condition
 * 1. index of the first found element
 * 2. if close, index of stopping element else -1
 * 3. if close, index of stopping element else -1
 * 4. -1
 */
static int
we_prsr_findMatchingEnd (we_prsr_parser_t* parser, int element,
                          int stop1, int stop2, int stop3, int stop4, int close)
{
  const unsigned char* htmlTable;
  char* elementStack;
  int   elem;
  int   i;

  elementStack = parser->stack.start + 1;
  htmlTable = parser->htmlTable;
  
  for (i = (parser->tagLevel - 1) << 1; i > 0; i -= 2) {
    elem = elementStack[i];
    if (elem == element)
      return i;
    else if ((elem == stop1 || elem == stop2 || elem == stop3 || elem == stop4) ||
             (element < 0 && (htmlTable[elem] & WE_PRSR_HEADING))) {
      if (close)
        return i;
      else
        return -1;
    }
  }
  return -1;
}




/*
 * Find and close an element matching the END tag 'element'.
 * Returns the stack mode to use if any of the element has been
 * closed, else -1. If any of the elements closed had the
 * cancelSpace property then '*cancelSpace' is TRUE, else FALSE.
 *
 * If 'element' is a block element, then misiatch elements are
 * not reopened.
 *
 * element        closes          stop search at first     stop closes the stop element
 * ------------------------------------------------------------------------------------
 * /li            li              ol, ul, dir, menu        no
 * /dt            dt              dl                       no
 * /dd            dd              dl                       no
 * /tr            tr              table                    no
 * /td            td              table, tr                no
 * /th            th              table, tr                no
 * any /hx        any hx          body                     yes
 * any other      a match         -                        no
 * /head          -               -                        -
 * /body          -               -                        -
 * /html          -               -                        -
 */
static int
we_prsr_closeMatchingEnd (we_prsr_parser_t* parser, int element, int* cancelSpace)
{
  int close;
  int elem;
  int stop1;
  int stop2;
  int stop3;
  int stop4;
  int index;
  int result;

  result = -1;
  close  = FALSE;
  elem   = element;
  stop1  = -1;
  stop2  = -1;
  stop3  = -1;
  stop4  = -1;

  switch (element) {
  case WBS_HTML_ELEMENT_LI:
    stop1 = WBS_HTML_ELEMENT_OL;
    stop2 = WBS_HTML_ELEMENT_UL;
    stop3 = WBS_HTML_ELEMENT_DIR;
    stop4 = WBS_HTML_ELEMENT_MENU;
    break;

  case WBS_HTML_ELEMENT_DT:
  case WBS_HTML_ELEMENT_DD:
    stop1 = WBS_HTML_ELEMENT_DL;
    break;

  case WBS_HTML_ELEMENT_TR:
    stop1 = WBS_HTML_ELEMENT_TABLE;
    break;

  case WBS_HTML_ELEMENT_TD:
    stop1 = WBS_HTML_ELEMENT_TABLE;
    stop2 = WBS_HTML_ELEMENT_TR;
    break;
    
  case WBS_HTML_ELEMENT_TH:
    stop1 = WBS_HTML_ELEMENT_TABLE;
    stop2 = WBS_HTML_ELEMENT_TR;
    break;

  case WBS_HTML_ELEMENT_H1:
  case WBS_HTML_ELEMENT_H2:
  case WBS_HTML_ELEMENT_H3:
  case WBS_HTML_ELEMENT_H4:
  case WBS_HTML_ELEMENT_H5:
  case WBS_HTML_ELEMENT_H6:
    elem = -1;
    close = TRUE;
    break;

  case WBS_HTML_ELEMENT_HEAD:
  case WBS_HTML_ELEMENT_BODY:
  case WBS_HTML_ELEMENT_HTML:
    *cancelSpace = 0;
    return -1;
  }

  index = we_prsr_findMatchingEnd (parser, elem, stop1, stop2, stop3, stop4, close);
  if (index != -1)
    result = we_prsr_closeElement (parser, -index, parser->htmlTable[element] & WE_PRSR_BLOCK,
                                    cancelSpace);
                   
  return result;
}

/* ======== WML specific functions ======== */

#ifdef WE_CFG_EXTENDED_WML_PARSING
/*
 * Add a missing WML element to the document.
 */
static void
prsr_addWmlElement (we_prsr_parser_t* parser)
{
  /* vald_startElement cannot reject; does not need to check return value. */
  parser->p.startHandler (parser->p.userData, WBS_HTML_ELEMENT_WML, NULL, 0);
  parser->stack.sessionStart[0] = (char) WBS_HTML_ELEMENT_WML;
  parser->stack.sessionStart[1] = (char) WE_PRSR_FLAG_NORMALIZE | WE_PRSR_FLAG_NO_TEXT;
  parser->tagLevel = 1;
}
#endif

/* ======== processor functions ======== */

/*
 * Initiate the parser.
 */
static int
we_prsr_initProcess (we_prsr_parser_t*  parser,
                      const char*         s,
                      const char*         end,
                      const char*        *nextPtr,
                      int                 limit)
{
  int guess;
  int i;

  limit = limit;
  *nextPtr = s;

  if (end - s < 4 && ! parser->noMoreData) 
    return WE_PRSR_STATUS_MORE;
  if (end - s == 0)
    return WE_PRSR_STATUS_OK;

  /* At least four bytes are needed by prsr_guessEncoding. The
   * document does not necessarily contain these, by it is safe
   * to go on. */
  guess = we_prsr_guess_encoding (s, &i, 0);
  /* If a byte order mark is found, throw away the
   * corresponding chars. */
  *nextPtr += i;

  parser->processor = we_prsr_prologueParser;
  parser->returnProc = we_prsr_prologueParser;

  if (parser->charset != WE_CHARSET_UNKNOWN) {
    if (parser->charset == WE_CHARSET_UTF_16 && guess == WE_CHARSET_UTF_16LE)
      parser->charset = WE_CHARSET_UTF_16LE;
    /* 
     * If WE_CHARSET_UCS_2 and little endian is guessed then there are no
     * character code to use. Instead UTF-16LE is used since UCS-2 is a
     * subset for it.
     */
    if (parser->charset == WE_CHARSET_UCS_2 && guess == WE_CHARSET_UTF_16LE)
      parser->charset = WE_CHARSET_UTF_16LE;
    
    /* Charset given by a header.*/
    return WE_PRSR_STATUS_NEW_CHARSET;

  } else if (guess != WE_CHARSET_UNKNOWN) {
    /* Non-ASCII based encoding or UTF-8 with BOM */
    parser->charset = guess;
    return WE_PRSR_STATUS_NEW_CHARSET;
  } 

  /* The charset to use can not be decided. Search for more
   * information in the prologue. This can either be an XML
   * declaration or an meta element.
   *
   * If an XML declaration exists, it must be at the very start
   * of the document. Use raw data for the prologue, i.e. assume
   * UTF-8. If the prologue did not contain additional information,
   * switch to the default charset. 
   *
   * parser->charset == WE_CHARSET_UNKNOWN at this point. */
  return WE_PRSR_STATUS_RETURN;
}


/*
 * Parse the prologue.
 *
 * 's' is the current position, 'end' end of buffer, '*nextPtr'
 * the current pointer for the next iteration and 'limit' number
 * of bytes left to process before a pause.
 */
static int
we_prsr_prologueParser (we_prsr_parser_t*  parser,
                         const char*         s,
                         const char*         end,
                         const char*        *nextPtr,
                         int                 limit)
{
  const char*           start;
  const char*           next;
  we_prsr_attribute_t* attrs;
  we_prsr_stack_t*     stack;
  we_tok_data_t        tokData;
  int                   size;
  int                   tok;
  int                   charset;
  int                   addHtml;
  int                   result;

  start   = s;
  next    = s;
  addHtml = FALSE;
  while (s - start < limit) {
    tok = we_tok_prologueTok (s, end, &next, &tokData);
    if (parser->mode == WE_PRSR_XML_MODE && ! tokData.strict) { /* XML and using HTML extensions */
      WE_PRSR_LOG_ERROR (parser, s, "SGML syntax or HTML extensions used in XML document");
      return WE_PRSR_ERROR_SYNTAX;
    }
    
    switch (tok) {
    case WE_TOK_MORE: /* ------------------------------------  out of data or partial element */
      *nextPtr = s;
      return WE_PRSR_STATUS_MORE;

    case WE_TOK_IGNORE:  /* ---------------------------------  ignored invalid data */
      if (parser->mode != WE_PRSR_XML_MODE) {
        WE_PRSR_LOG_ERROR (parser, s, "Ignored invalid data");
        break;
      }
      /* fall through */

    case WE_TOK_INVALID: /* ---------------------------------  invalid data */
      WE_PRSR_LOG_ERROR (parser, s, "Invalid data");
      return WE_PRSR_ERROR_SYNTAX;

    case WE_TOK_CHAR_DATA: /* -------------------------------  character data (HTML only) */  
      if (parser->mode == WE_PRSR_XML_MODE) {
        WE_PRSR_LOG_ERROR (parser, s, "Character data in prologue");
        return WE_PRSR_ERROR_SYNTAX;
      }

#ifdef WE_CFG_EXTENDED_WML_PARSING
      else if (parser->mode == WE_PRSR_EXTENDED_WML_MODE) {
        next++;
        break;
      }
#endif /* WE_CFG_EXTENDED_WML_PARSING */

      addHtml = TRUE; /* Call prsr_addHtmlElement */
      /* Fall through */

    case WE_TOK_INSTANCE_START: /* --------------------------  end of prologue */
      *nextPtr = s;
      parser->processor = we_prsr_contentParser;
      parser->returnProc = we_prsr_contentParser;
      if (parser->state < WE_PRSR_STATE_DOCTYPE) { /* No DOCTYPE found */
        /* At this point, the charset may not be known */
        result = parser->initFunct (&parser->p, NULL, NULL, 0, 0, parser->charset);
        switch (result) {
        case WE_PRSR_APPLICATION_ERROR:
          WE_PRSR_LOG_ERROR (parser, s, "Unknown document type");
          return WE_PRSR_ERROR_UNKNOWN_DOCTYPE;
        case WE_PRSR_APPLICATION_OUT_OF_MEMORY:
          WE_PRSR_LOG_ERROR (parser, s, "Out of memory");
          return WE_PRSR_ERROR_OUT_OF_MEMORY;
        case WE_PRSR_APPLICATION_HTML:
          parser->mode = WE_PRSR_HTML_MODE;
          parser->html_based = TRUE;
          break;
#ifdef WE_CFG_EXTENDED_WML_PARSING
        case WE_PRSR_APPLICATION_WML:
          parser->mode = WE_PRSR_EXTENDED_WML_MODE;
          break;
#endif /* WE_CFG_EXTENDED_WML_PARSING */
        default:
          if (addHtml) {
            WE_PRSR_LOG_ERROR (parser, s, "Character data in prologue");
            return WE_PRSR_ERROR_SYNTAX;
          }
          parser->mode = WE_PRSR_XML_MODE;
          break;
        }
      }
      
      /* Dummy stackmode read when root end tag is found */
      stack = &parser->stack;
      stack->start[0] = STACKMODE_NORMALIZE;
      stack->ptr = ++stack->sessionStart;

      /* If the root element should be unknown, then
       * do not allow it to take text */
      parser->stackmode = STACKMODE_NO_TEXT;

      if (parser->mode != WE_PRSR_XML_MODE) {
        we_prsr_initHtml (parser);
      }

      if (addHtml) {
        we_prsr_addHtmlElement (parser);
        parser->stackmode = STACKMODE_NORMALIZE;
      }
      
      parser->state = WE_PRSR_STATE_CONTENT;

      if (parser->charset == WE_CHARSET_UNKNOWN) {
        if (parser->html_based && !addHtml) {
          /* This is an HTML document and the charset is unknown. Scan
           * for meta charset. Document begining with text and not with
           * <html> are not scanned to avoid loding pure text documents. */
          return WE_PRSR_STATUS_META_SCAN;
        }
        else {
          /* If no meta charset scan, then there are no additional sources
           * of information available. Set charset. */
          return WE_PRSR_STATUS_NEW_CHARSET;
        }
      }
      else {
        return WE_PRSR_STATUS_RETURN;
      }
      
    case WE_TOK_PI: /* --------------------------------------  non-xml processing instruction */
      if (parser->piHandler) {
        result = parser->piHandler (parser->p.userData, parser, s + 2, tokData.str, next - 2);
        
        if (result == WE_PRSR_APPLICATION_OUT_OF_MEMORY)
          return WE_PRSR_ERROR_OUT_OF_MEMORY;
        else if (result == WE_PRSR_APPLICATION_ERROR)
          return WE_PRSR_ERROR_APPLICATION;
      }
      break;

    case WE_TOK_XML_DECL: /* --------------------------------  xml processing instruction */
      /* If it exits, it must be at the very start of the document.
       * Only a Byte Order Mark may precede it, but a misplaced XML
       * declaration is still accepted if is only preceded white-space,
       * comments or other irrelevant data. */

      *nextPtr = next;
      /* Only an XML declarations first in the document is used */
      if (parser->state > WE_PRSR_STATE_START) {
        WE_PRSR_LOG_ERROR (parser, s, "XML declaration is not at document start");
        break;
      }

      parser->mode = WE_PRSR_XML_MODE;
      parser->state = WE_PRSR_STATE_XML_DECL;

      /* This msans this document uses some kind of US-ASCII
       * based encoding */

      
      
#ifndef WE_CFG_EXTENDED_WML_PARSING
      parser->mode = WE_PRSR_XML_MODE;
#endif

      if (parser->charset != WE_CHARSET_UNKNOWN) {
        break;
      }

      size = we_prsr_extract_attributes (parser, tokData.str, next - 2,
                                          &we_prsr_XmlDeclAttrs,
                                          we_prsr_PiAttributeTypes, &attrs);
      
      if (size == -2) {
        return WE_PRSR_ERROR_OUT_OF_MEMORY;
      }

      if (size >= 1) { /* encoding */
        char* temp = attrs[0].u.s;
        int len    = strlen (temp);

        charset = we_charset_str2int  (temp, len);
        if (charset > 0 &&
            charset != WE_CHARSET_UCS_2    && charset != WE_CHARSET_UTF_16 &&
            charset != WE_CHARSET_UTF_16LE && charset != WE_CHARSET_UTF_16BE) {
          /* An ASCII based charset was found. Use it if there is no better source. */
          parser->defaultCharset = charset;
        }
        we_prsr_free_attributes (parser->modId, attrs, size);
      }
      break;

    case WE_TOK_DOCTYPE: /* --------------------------------- document type */
      if (parser->state >= WE_PRSR_STATE_DOCTYPE)
        break;
      parser->state = WE_PRSR_STATE_DOCTYPE;

      /* At this point, the charset may not be known */
      result = parser->initFunct (&parser->p, NULL, tokData.str,
                                  tokData.strLen, 0, parser->charset);
      switch (result) {
      case WE_PRSR_APPLICATION_ERROR:
        WE_PRSR_LOG_ERROR (parser, s, "Unknown document type");
        return WE_PRSR_ERROR_UNKNOWN_DOCTYPE;
      case WE_PRSR_APPLICATION_OUT_OF_MEMORY:
        WE_PRSR_LOG_ERROR (parser, s, "Out of memory");
        return WE_PRSR_ERROR_OUT_OF_MEMORY;
      case WE_PRSR_APPLICATION_HTML:
        parser->mode = WE_PRSR_HTML_MODE;
        parser->html_based = TRUE;
        break;
#ifdef WE_CFG_EXTENDED_WML_PARSING
      case WE_PRSR_APPLICATION_WML:
        parser->mode = WE_PRSR_EXTENDED_WML_MODE;
        break;
#endif
      default:
        parser->mode = WE_PRSR_XML_MODE;
        break;
      }
      if (!parser->html_based && parser->charset == WE_CHARSET_UNKNOWN) {
        *nextPtr = next;
        return WE_PRSR_STATUS_NEW_CHARSET;
      }

      break;

    case WE_TOK_COMMENT_OPEN: /* ---------------------------- comment */
      parser->type = WE_PRSR_SECTION_COMMENT;
      parser->stack.mode |= STACKMODE_NO_TEXT;
      parser->processor = we_prsr_cdataSectionParser;
      *nextPtr = next;
#ifdef WE_PRSR_LOG
      if (! tokData.strict)
        WE_PRSR_LOG_ERROR (parser, s, "Warning: illegal comment");
#endif /* WE_PRSR_LOG */
      return WE_PRSR_STATUS_RETURN;
    }

    s = next;
  }
  *nextPtr = s;
  return WE_PRSR_STATUS_PAUSE;
}



/*
 * Parse the content part of the input document.
 *
 * Only an end element may end the contentProcessor by switching
 * to the epilogueProcessor. Runing out of input data will 
 * generate an error.
 *
 * 's' is the current position, 'end' end of buffer, '*nextPtr'
 * the current pointer for the next iteration and 'limit' number
 * of bytes left to process before a pause.
 */
static int 
we_prsr_contentParser (we_prsr_parser_t*  parser,
                        const char*         s,
                        const char*         end,
                        const char*        *nextPtr,
                        int                 limit)
{
  we_prsr_stack_t*       stack = &parser->stack;
  const char*       start;
  const char*       next;
  const char*       name;
  we_tok_data_t    tokData;
  int               tok;
  int               size;     
  int               element;
  int               textmode;
  int               cancelSpace;
  int               result;
  int               charTok;
  int               error;

  error = WE_PRSR_STATUS_OK;
  start = s;
  while (s - start < limit) {
    tok = we_tok_contentTok (s, end, &next, &tokData);
    if (parser->mode == WE_PRSR_XML_MODE && ! tokData.strict) { /* XML and using HTML extensions */
      WE_PRSR_LOG_ERROR (parser, s, "SGML syntax or HTML extensions used in XML document");
      return WE_PRSR_ERROR_SYNTAX;
    }
    charTok = tok == WE_TOK_CHAR_DATA ||
              tok == WE_TOK_ENTITY_REF || 
              tok == WE_TOK_CHAR_REF;
    
    /* Open a text session if 'tok' is character data, close it
     * when the first non-text token is found */
    if (stack->mode == STACKMODE_NONE) { /* Last token was an element */
      if (charTok) {
        if (we_prsr_newSession (parser, parser->stackmode))
          return WE_PRSR_ERROR_OUT_OF_MEMORY;
      }
    } else {
      if (! (charTok || tok == WE_TOK_COMMENT_OPEN)) /* Comments are ignored */ 
        we_prsr_endSession (parser);
    }

    switch (tok) {
    case WE_TOK_MORE: /* --------------------------------  out of data or partial element */
      if (parser->mode == WE_PRSR_HTML_MODE && *nextPtr == NULL) {
        /* Unexpected end of document is interpreted as a missing </html> */
        we_prsr_closeElement (parser, WBS_HTML_ELEMENT_HTML, TRUE, &cancelSpace);
        return WE_PRSR_STATUS_OK;
      }
      else 
#ifdef WE_CFG_EXTENDED_WML_PARSING
      if ((parser->mode == WE_PRSR_EXTENDED_WML_MODE) && *nextPtr == NULL) {
        /* Unexpected end of document is interpreted as a missing </wml> */
        we_prsr_closeElement (parser, WBS_HTML_ELEMENT_WML, TRUE, &cancelSpace);
       return WE_PRSR_STATUS_OK;
      } 
      else 
#endif  /* WE_CFG_EXTENDED_WML_PARSING */
      {
        *nextPtr = s;
        return WE_PRSR_STATUS_MORE;
      }

    case WE_TOK_IGNORE:  /* -----------------------------  ignored invalid data */
      if (parser->mode != WE_PRSR_XML_MODE) {
        WE_PRSR_LOG_ERROR (parser, s, "Ignored invalid data");
        break;
      }
      /* fall through */
    
    case WE_TOK_INVALID: /* -----------------------------  invalid data */
      WE_PRSR_LOG_ERROR (parser, s, "Invalid data");
      return WE_PRSR_ERROR_SYNTAX;
    
    case WE_TOK_INVALID_CHAR: /* ------------------------  non-XML character */
      WE_PRSR_LOG_ERROR (parser, s, "Invalid character skipped");
      break;
      
    case WE_TOK_START_TAG: /* ---------------------------  start tag or empty tag */    
      result = we_prsr_startTagHandler (parser, s, end, &next, *nextPtr == NULL);
      switch (result) {
      case WE_PRSR_CDATA:
        textmode = parser->stackmode & ~WE_PRSR_FLAG_NORMALIZE;
        goto Content_cdata;
      case WE_PRSR_STATUS_NEW_CHARSET:
        *nextPtr = next;
        return WE_PRSR_STATUS_NEW_CHARSET;
      case WE_PRSR_STATUS_OK:
        break;
      default:
        *nextPtr = s;
        return result;
      }
      break;

      case WE_TOK_END_TAG: /* -----------------------------  end tag */
      result = we_prsr_elemTagParser (parser, s, end, &next, &name, NULL,
                                       NULL, &size, FALSE, *nextPtr == NULL);

      /* HTML */
      if (parser->mode == WE_PRSR_HTML_MODE) {
        if (result == WE_PRSR_ERROR_SYNTAX) {
          WE_PRSR_LOG_ERROR (parser, s, "Syntax error");
          break;
        }
        if (parser->tagLevel == 0) /* Ignore misiatched end tag */
          break;
        element = we_cmmn_str2int_lc (name, size, parser->p.elements);

        if (element != -1) {
          if (element == WBS_HTML_ELEMENT_FORM) {
            result = parser->p.startHandler (parser->p.userData, WBS_HTML_ELEMENT_FORM_END,
                                             NULL, 0);
            if (result == WE_PRSR_APPLICATION_OK)
              /* Wbs_Hvld_endElement never rejects; does not need to check return value. */
              parser->p.endHandler (parser->p.userData);

          } else {
            result = we_prsr_closeMatchingEnd (parser, element, &cancelSpace);
            if (result >= 0) { /* Did actually close one or more elements */
              if (cancelSpace)
                stack->cancelSpace = TRUE;
              parser->stackmode = result;
            }
          }
        }
      } else
#ifdef WE_CFG_EXTENDED_WML_PARSING
        /* EXTENDED WML */
        if (parser->mode == WE_PRSR_EXTENDED_WML_MODE) {
          if (result == WE_PRSR_ERROR_SYNTAX) {
            WE_PRSR_LOG_ERROR (parser, s, "Syntax error");
            break;
          }
          if (parser->tagLevel == 0) /* Ignore misiatched end tag */
            break;
          element = we_cmmn_str2int_lc (name, size, parser->p.elements);
          
          if (element != -1) {
            result = we_prsr_closeElement (parser, element,
                                        parser->htmlTable[element] & WE_PRSR_BLOCK,
                                        &cancelSpace);
            if (result >= 0) { /* Did actually close one or more elements */
              if (cancelSpace)
                stack->cancelSpace = TRUE;
              parser->stackmode = result;
            }
          }
        } else
#endif /* WE_CFG_EXTENDED_WML_PARSING */
      /* XML */
      {
        if (result == WE_PRSR_ERROR_SYNTAX) {
          WE_PRSR_LOG_ERROR (parser, s, "Syntax error");
          return WE_PRSR_ERROR_SYNTAX;
        }
        if (parser->tagLevel == 0 || size != stack->ptr - stack->sessionStart - 1 ||
            strncmp (stack->sessionStart, name, size)) {
          WE_PRSR_LOG_ERROR (parser, s, "Misiatched end tag");
          return WE_PRSR_ERROR_SYNTAX; 
        }

        result = *(stack->ptr - 1);
        parser->tagLevel--;
        
        error = parser->p.endHandler (parser->p.userData);
        if (error == WE_PRSR_APPLICATION_OUT_OF_MEMORY)
          return WE_PRSR_ERROR_OUT_OF_MEMORY;
        else if (error == WE_PRSR_APPLICATION_ERROR)
          return WE_PRSR_ERROR_APPLICATION;

        if (result & WE_PRSR_FLAG_CANCEL_SPACE)
          stack->cancelSpace = TRUE;
        parser->stackmode = *(stack->sessionStart - 3) & ~WE_PRSR_FLAG_CANCEL_SPACE;

        we_prsr_endSession (parser);
      }

      if (parser->tagLevel == 0) {
        *nextPtr = next;
        return WE_PRSR_STATUS_OK;
      }      
      break;
    
    case WE_TOK_ENTITY_REF: /* --------------------------  entity ref (&amp;) */
      error = we_prsr_writeEntityRef (parser, s, next - s);
      break;
    
    case WE_TOK_CHAR_REF: /* ----------------------------  character ref: &#32; or &#x20; */
      error = we_prsr_writeCharacterRef (parser, s, next - s);
      break;

    case WE_TOK_CHAR_DATA: /* ---------------------------  text data */
      error = we_prsr_writeString (parser, s, next - s);
      break;
    
    case WE_TOK_CDATA_SECT_OPEN: /* ---------------------  CDATA */
      parser->type = WE_PRSR_SECTION_CDATA;
      if (parser->stackmode & WE_PRSR_FLAG_OPAQUE) { /* Inherit the opaque property */
        textmode = STACKMODE_OPAQUE;
      }
      else {
        textmode = STACKMODE_PRESERVE;
      }

Content_cdata:
      we_prsr_newSession (parser, textmode);
      parser->processor = we_prsr_cdataSectionParser;
      *nextPtr = next;
      return WE_PRSR_STATUS_RETURN;
    
    case WE_TOK_XML_DECL: /* ----------------------------  xml PI */
      WE_PRSR_LOG_ERROR (parser, s, "Misplaced PI inside content");
      return WE_PRSR_ERROR_SYNTAX;
    
    case WE_TOK_PI: /* ----------------------------------  non-xml PI */
      if (parser->piHandler) {
        error = parser->piHandler (parser->p.userData, parser, s + 2, tokData.str, next - 2);
        
        if (error == WE_PRSR_APPLICATION_OUT_OF_MEMORY)
          return WE_PRSR_ERROR_OUT_OF_MEMORY;
        else if (error == WE_PRSR_APPLICATION_ERROR)
          return WE_PRSR_ERROR_APPLICATION;
      }
      break;

    case WE_TOK_COMMENT_OPEN: /* ------------------------  comment */
      parser->type = WE_PRSR_SECTION_COMMENT;
      parser->stack.mode |= STACKMODE_NO_TEXT;
      parser->processor = we_prsr_cdataSectionParser;
      *nextPtr = next;
#ifdef WE_PRSR_LOG
      if (! tokData.strict)
        WE_PRSR_LOG_ERROR (parser, s, "Warning: illegal comment");
#endif /* WE_PRSR_LOG */
      return WE_PRSR_STATUS_RETURN;
    }

    if (error) {
      *nextPtr = s;
      return error;
    }

    if (parser->stack.error)
      return parser->stack.error;
    
    s = next;
  }
  *nextPtr = s;
  return WE_PRSR_STATUS_PAUSE;
}



/*
 * Parse a start tag, end tag or PI. Returns an array 'attrs'
 * of attributes.
 */
static int
we_prsr_elemTagParser (we_prsr_parser_t*     parser,
                        const char*            s,
                        const char*            end,
                        const char*           *nextPtr,
                        const char*           *name,
                        int                   *element,
                        we_prsr_attribute_t* *attrs,
                        int                   *size,
                        int                    pi,
                        int                    endOfDoc)
{
  /*
   * Array with state transitions xy.
   * x - new state
   * y - action taken
   * Negative numers are transitions not valid in XML.
   */
  static const signed char transitions[][9] = {
   /* name     sp     eq     lt     gt   eend   qstr    str    sol */
    { 0x21, -0x10, -0x10, -0x10, -0x78, -0x77, -0x10, -0x21,  0x60},/* 1 <|name name = value > */
    {-0x40,  0x30, -0x30, -0x30,  0x78,  0x77, -0x30, -0x30, -0x30},/* 2 <name| name = value > */
    { 0x44,  0x30, -0x30, -0x30,  0x78,  0x77, -0x30, -0x30, -0x30},/* 3 <name |name = value > */
    {-0x43,  0x40,  0x50, -0x40, -0x78, -0x77, -0x33, -0x33, -0x33},/* 4 <name name| = value > */
    {-0x36,  0x50, -0x50, -0x50, -0x78, -0x77,  0x35, -0x36, -0x36},/* 5 <name name =| value > */
    { 0x22, -0x60, -0x60, -0x60, -0x78, -0x77, -0x60, -0x22, -0x60},/* 6 </|name name = value >*/
                                                                    /* 3 <name name = value| > */
                                                                    /* 7 finished              */
  };
  static const char tokStates[] = {
    WE_TOK_STATE_XML,
    WE_TOK_STATE_ELEM_NAME, /* 1 <|name name = value > */
    WE_TOK_STATE_ATTR_NAME, /* 2 <name| name = value > */
    WE_TOK_STATE_ATTR_NAME, /* 3 <name |name = value > */
    WE_TOK_STATE_ATTR_NAME, /* 4 <name name| = value > */
    WE_TOK_STATE_ATTR_VAL,  /* 5 <name name =| value > */
    WE_TOK_STATE_ELEM_NAME, /* 6 </|name name = value >*/
    0                        /* 7 finished              */
  };

  we_prsr_attribute_t v[WE_PRSR_MAX_NBR_OF_ATTRS]; 
  we_prsr_stack_t*    stack     = &parser->stack;
  const char*          nameStart = NULL;
  we_prsr_str2int_t*  str2int;
  int                  nameLen   = 0;
  int                  pos;    /* 0 - MAX_NBR_OF_ATTRS; MAX when full, -1 when invalid element */
  int                  type;   /* -1; >0 during parsing an attribute value pair */ 
  int                  valueLen;
  int                  transition;
  int                  tok;
  int                  state;
  int                  tokState;
  int                  action;
  int                  xml;
  int                  attr;
  int                  flag;
  int                  len;
  int                  returnToken;
  int                  nbrOfRefs;
  int                  strict;

  state       = 1;
  returnToken = WE_PRSR_ERROR_SYNTAX;
  pos         = 0;
  xml         = parser->mode == WE_PRSR_XML_MODE;
  valueLen    = 0;
  nbrOfRefs   = 0;
  type        = -1; /* type >= 0 only when an attribute name has been found */

  if (xml) {
    str2int = we_cmmn_str2int;
    tokState = WE_TOK_STATE_XML;
  } else {
    str2int = we_cmmn_str2int_lc;
    tokState = WE_TOK_STATE_ELEM_NAME;
  }

  if (pi)
    state = 2; /* No "<name" */
  else
    s++; /* Skip "<" */

  for (;;) {
    tok = we_tok_tagTok (s, end, nextPtr, &nbrOfRefs, tokState, endOfDoc, &strict);
    
    
    if (tok == WE_TOK_MORE) {
      if (pi){
        if (s == end || ct_isxmlwhitespace (*s)) {
          returnToken = WE_PRSR_STATUS_OK;
          goto Finished;
        } else {
          returnToken = WE_PRSR_ERROR_SYNTAX;
          goto Exit;
        }
      } else {
        returnToken = WE_PRSR_STATUS_MORE;
        goto Exit;
      }
    } else if (tok == WE_TOK_INVALID) {
      if (xml) {
        returnToken = WE_PRSR_ERROR_SYNTAX;
        goto Exit;
      } else {
        s = *nextPtr; 
        continue;
      }
    }

    transition = transitions[state - 1][tok - WE_TOK_NAME];
    if (transition < 0) {
      if (xml) {
        returnToken = WE_PRSR_ERROR_SYNTAX;
        goto Exit;
      } else
        transition = -transition;
    }

    state  = transition >> 4;
    action = transition & 0x0F;

    if (tokState > 0)
      tokState = tokStates[state];


    switch (action) {    

    case 1: /* Element name; start tag */
      len = *nextPtr - s;
      we_prsr_writeData (parser, s, len, FALSE);
      *element = we_cmmn_str2int (stack->sessionStart, len, parser->p.elements);
      if (*element < 0)
        pos = -1; /* Do not save attribute values */
      returnToken = WE_PRSR_STATUS_START_TAG;
      break;

    case 2: /* Element name; end tag */
      *size = *nextPtr - s;
      *name = s;
      pos = -1; /* Do not save attribute values */
      returnToken = WE_PRSR_STATUS_END_TAG;
      break;

    case 3: /* Attribute value empty */
      if (type == WE_PRSR_TYPE_FLAG) {
        v[pos].flag = WE_PRSR_ATTRVAL_INT;
        pos++;
        type = -1;
      }
      if (tok != WE_TOK_NAME)
        break;
      /* fall through */

    case 4: /* Attribute name */
      if (xml && returnToken == WE_PRSR_STATUS_END_TAG)
        return WE_PRSR_ERROR_SYNTAX;
        
      if (pos >= 0 && pos < WE_PRSR_MAX_NBR_OF_ATTRS) {
        attr = str2int (s, *nextPtr - s, parser->p.attributes);
        if (attr < 0)
          break;
        v[pos].type = (short) attr;
        type = parser->p.attributeType[attr];
        nameStart = s;
        nameLen = *nextPtr - s;
      }
      break;

    case 5: /* Attribute value quoted string */
      s++;
      valueLen = -1;
      /* fall through */
             
    case 6: /* Attribute value */
      if (pos >= 0 && pos < WE_PRSR_MAX_NBR_OF_ATTRS && type >= 0) {
        int failure;

        valueLen += *nextPtr - s;
        flag = nbrOfRefs;
        v[pos].u = we_prsr_normalizeAttribute (parser, type, nameStart, nameLen,
                                                s, valueLen, &flag, &failure);
        v[pos].flag = (short) flag;
        type = -1;
        if (! failure)
          pos++;
      }
      valueLen = 0;
      break;

    case 7: /* Finished; empty element */
      if (returnToken == WE_PRSR_STATUS_END_TAG)
        return WE_PRSR_ERROR_SYNTAX;

      returnToken = WE_PRSR_STATUS_EMPTY_ELEMENT;
      /* fall through */
    
    case 8: /* Finished */
Finished:
      if (returnToken == WE_PRSR_STATUS_END_TAG)
        return WE_PRSR_STATUS_END_TAG;

      if (type == WE_PRSR_TYPE_FLAG) {
        v[pos].flag = WE_PRSR_ATTRVAL_INT;
        pos++;
      }
      if (xml) {
        int i;
        int j;

        for (i = 0; i < pos - 1; i++)
          for (j = i + 1; j < pos; j++)
            if (v[i].type == v[j].type) {
              WE_PRSR_LOG_ERROR (parser, s, "Duplicate attribute");
              returnToken = WE_PRSR_ERROR_SYNTAX;
              goto Exit;
            }
      }
      if (pos <= 0) {
        pos = 0;
        *attrs = NULL;
      } else {
        *attrs = WE_MEM_ALLOC (parser->modId, sizeof (we_prsr_attribute_t) * pos);
        memcpy (*attrs, v, sizeof (we_prsr_attribute_t) * pos);
      }
      *size = pos;

      return returnToken;
    }
    s = *nextPtr;
  }

Exit:
  {
    int i;

    for (i = 0; i < pos; i++)
      if (v[i].flag == WE_PRSR_ATTRVAL_STR)
        WE_MEM_FREE (parser->modId, v[i].u.s);

    return returnToken;
  }
}



/*
 * Start tag handler.
 */
static int
we_prsr_startTagHandler (we_prsr_parser_t*  parser,
                          const char*         s,
                          const char*         end,
                          const char*        *nextPtr,
                          int                 endOfDoc)
{
  we_prsr_stack_t*     stack = &parser->stack;
  we_prsr_attribute_t* attrs;
  int                   size;     
  int                   element;
  int                   stackmode;
  int                   cancelSpace;
  int                   result;
  int                   error;
  int                   returnToken;

  returnToken = WE_PRSR_STATUS_OK;
  we_prsr_newSession (parser, STACKMODE_NONE);

  result = we_prsr_elemTagParser (parser, s, end, nextPtr, NULL, &element,
                                   &attrs, &size, FALSE, endOfDoc);
  if (result == WE_PRSR_STATUS_MORE) {
    we_prsr_endSession (parser);
    return WE_PRSR_STATUS_MORE;
  } else if (result == WE_PRSR_ERROR_SYNTAX) {
    WE_PRSR_LOG_ERROR (parser, s, "Syntax error");
    if (parser->mode != WE_PRSR_XML_MODE) {
      we_prsr_endSession (parser);
      return WE_PRSR_STATUS_OK;
    } else
      return WE_PRSR_ERROR_SYNTAX;
  }

  if (element < 0) { /* Unknown element */
    WE_PRSR_LOG_ERROR (parser, s, "Warning: unknown element");
    element = WE_PRSR_UNKNOWN_ELEMENT;
  }                          

  /* Adjust the element stack */
  if (parser->mode == WE_PRSR_HTML_MODE) {
    we_prsr_endSession (parser);
    
    if (element == WE_PRSR_UNKNOWN_ELEMENT) {
      return WE_PRSR_STATUS_OK;
    }                          

    if (parser->tagLevel == 0 && element != WBS_HTML_ELEMENT_HTML) {
      /* Add a missing html element to a HTML document */
      we_prsr_addHtmlElement (parser);
    }

    if (parser->htmlTable[element] & WE_PRSR_CLOSE) {
      /* This is an element that may cause closure of another. */
      if (we_prsr_closeMatchingStart (parser, element)) {
        stack->cancelSpace = TRUE;
      }
      
    }

    
    if (element == WBS_HTML_ELEMENT_TITLE || element == WBS_HTML_ELEMENT_STYLE)
      parser->stack.cancelSpace_bu = parser->tagLevel == 1 ? -1 : 0; 
  }
#ifdef WE_CFG_EXTENDED_WML_PARSING
  if (parser->mode == WE_PRSR_EXTENDED_WML_MODE) {
    we_prsr_endSession (parser);
    
    if (parser->tagLevel == 0 && element != WBS_HTML_ELEMENT_WML) 
      /* Add a missing wml element to a Extended WML document */
      prsr_addWmlElement (parser);

    if (parser->htmlTable[element] & WE_PRSR_CLOSE) {
      /* This is an element that may cause closure of another. */
      if (we_prsr_closeMatchingStart (parser, element)) {
        stack->cancelSpace = TRUE;
      }
      
    }
  }
#endif /* WE_CFG_EXTENDED_WML_PARSING */

  error = parser->p.startHandler (parser->p.userData, element, attrs, size);

  switch (error) {
  case WE_PRSR_APPLICATION_ERROR:
    WE_PRSR_LOG_ERROR (parser, s, "Unrecoverable validation error");
    return WE_PRSR_ERROR_APPLICATION;
  case WE_PRSR_APPLICATION_OUT_OF_MEMORY:
    WE_PRSR_LOG_ERROR (parser, s, "Out of memory in startHandler");
    return WE_PRSR_ERROR_OUT_OF_MEMORY;
  case WE_PRSR_APPLICATION_WARNING:
  case WE_PRSR_APPLICATION_WARNING_BODY:
    if (parser->mode != WE_PRSR_XML_MODE) {
      WE_PRSR_LOG_ERROR (parser, s, "PARSER: Warning, validation rejected element.");
      if (error == WE_PRSR_APPLICATION_WARNING_BODY && parser->charset == WE_CHARSET_UNKNOWN)
        return WE_PRSR_STATUS_NEW_CHARSET;
      else
        return WE_PRSR_STATUS_OK;
    } else {
      WE_PRSR_LOG_ERROR (parser, s, "Error: validation rejected element");
      return WE_PRSR_ERROR_APPLICATION;
    }
    break;
  case WE_PRSR_APPLICATION_OK_BODY:
    if (parser->charset == WE_CHARSET_UNKNOWN)
      returnToken = WE_PRSR_STATUS_NEW_CHARSET;
    break;
  }

  /* Calculate the text handling parameters */
  cancelSpace = FALSE;

  if (element == WE_PRSR_UNKNOWN_ELEMENT) {
    stackmode = *(stack->sessionStart - 3) & ~WE_PRSR_FLAG_CANCEL_SPACE;
  }
  else {
    stackmode = parser->p.elementTable[element] & WE_PRSR_TEXT ?
                                          WE_PRSR_FLAG_FLUSH: WE_PRSR_FLAG_NO_TEXT;

    /* Cancel white-space around block elements */
    if (parser->p.elementTable[element] & WE_PRSR_CANCELSPACE) {
      stack->cancelSpace = TRUE;
      /* Matching end element should also cancel space */
      cancelSpace = WE_PRSR_FLAG_CANCEL_SPACE;
    }

    /* Stop space cancellation after visual elements. They have the
     * same influence on white-space handling as text data. */
    if (parser->p.elementTable[element] & WE_PRSR_VISUAL) {
      stack->cancelSpace = FALSE;
    }
  }

  /* HTML */
  if (parser->mode == WE_PRSR_HTML_MODE) {
    if (parser->htmlTable[element] & WE_PRSR_EMPTY) { /* Empty element */
      /* Wbs_Hvld_endElement never rejects; does not need to check return value. */
      parser->p.endHandler (parser->p.userData);
      parser->stackmode = parser->stack.sessionStart[(parser->tagLevel << 1) - 1] &
                                                        ~WE_PRSR_FLAG_CANCEL_SPACE;
      return WE_PRSR_STATUS_OK;
    }
    else { /* Non-empty element */
      int i;

      if (parser->tagLevel == parser->numElems) {
        /* Out of element stack space. Expand. */
        char dummy = '\0';

        /* prsr_writeData uses cmmn_strncpy_lc in this case
         * and so using &dummy is safe. */
        we_prsr_writeData (parser, &dummy, 40, FALSE);
        parser->numElems += 20;
      }
      
      if (parser->p.elementTable[element] & WE_PRSR_OPAQUE) {
        /* Preserve all white-space including CR LF */
        stack->cancelSpace = FALSE;
        stackmode |= WE_PRSR_FLAG_OPAQUE;
      } else if (parser->p.elementTable[element] & WE_PRSR_PRE)
        /* Preserve all white-space, normalise CR LF to a LF */
        stack->cancelSpace = FALSE;
      else
        stackmode |= WE_PRSR_FLAG_NORMALIZE;
     
      i = parser->tagLevel++ << 1;
      /* Inherit the preserve and opaque properties */
      stackmode = (stackmode & ~WE_PRSR_FLAG_NORMALIZE) |
                  (stackmode & stack->sessionStart[i - 1] & WE_PRSR_FLAG_NORMALIZE) |
                  (stack->sessionStart[i - 1] & WE_PRSR_FLAG_OPAQUE);
      stack->sessionStart[i]     = (char) element;
      stack->sessionStart[i + 1] = (char) (stackmode | cancelSpace);
    }
  } else
#ifdef WE_CFG_EXTENDED_WML_PARSING
  /* Extended WML */
  if (parser->mode == WE_PRSR_EXTENDED_WML_MODE) {

    if ((*(*nextPtr - 2) == '/') || 
        (element == WBS_HTML_ELEMENT_BR) || 
        (element == WBS_HTML_ELEMENT_IMG) ||
        (element == WBS_HTML_ELEMENT_INPUT)) { /* Empty element */
      /* vald_endElement never rejects. */
      error = parser->p.endHandler (parser->p.userData);
      parser->stackmode = parser->stack.sessionStart[(parser->tagLevel << 1) - 1] &
                                                        ~WE_PRSR_FLAG_CANCEL_SPACE;
      return WE_PRSR_STATUS_OK;

    } else { /* Non-empty element */ 
      int i;

      if (parser->tagLevel == parser->numElems) {
        /* Out of element stack space. Expand. */
        char dummy = '\0';

        /* prsr_writeData uses cmmn_strncpy_lc in this case
         * and so using &dummy is safe. */
        we_prsr_writeData (parser, &dummy, 40, FALSE);
        parser->numElems += 20;
      }
    
      if (parser->p.elementTable[element] & WE_PRSR_PRE)
        /* Preserve all white-space */
        stack->cancelSpace = FALSE;
      else
        stackmode |= WE_PRSR_FLAG_NORMALIZE;
      
   
      i = parser->tagLevel++ << 1;
      stackmode = stackmode & ~WE_PRSR_FLAG_NORMALIZE |
                  stackmode & stack->sessionStart[i - 1] & WE_PRSR_FLAG_NORMALIZE;
      stack->sessionStart[i]     = (char) element;
      stack->sessionStart[i + 1] = (char) (stackmode | cancelSpace);
    }
  } else
#endif /* WE_CFG_EXTENDED_WML_PARSING */
  /* XML */
  {
    if (result == WE_PRSR_STATUS_EMPTY_ELEMENT) {
      error = parser->p.endHandler (parser->p.userData);
      if (error == WE_PRSR_APPLICATION_OUT_OF_MEMORY)
        return WE_PRSR_ERROR_OUT_OF_MEMORY;
      else if (error == WE_PRSR_APPLICATION_ERROR)
        return WE_PRSR_ERROR_APPLICATION;

      we_prsr_endSession (parser);
      parser->stackmode = *(stack->ptr - 1) & ~WE_PRSR_FLAG_CANCEL_SPACE;
      return WE_PRSR_STATUS_OK;

    } else { /* Non-empty element */ 
      parser->tagLevel++;
  
      if (element != WE_PRSR_UNKNOWN_ELEMENT) {
        if (parser->p.elementTable[element] & WE_PRSR_OPAQUE) {
          /* Preserve all white-space including CR LF */
          stack->cancelSpace = FALSE;
          stackmode |= WE_PRSR_FLAG_OPAQUE;
        } else if (parser->p.elementTable[element] & WE_PRSR_PRE)
          /* Preserve all white-space, normalise CR LF to a LF */
          stack->cancelSpace = FALSE;
        else
          stackmode |= WE_PRSR_FLAG_NORMALIZE;
      }

      /* Inherit the preserve and opaque properties */
      stackmode = (stackmode & ~WE_PRSR_FLAG_NORMALIZE) |
                  (stackmode & *(stack->sessionStart - 3) & WE_PRSR_FLAG_NORMALIZE) |
                  (*(stack->sessionStart - 3) & WE_PRSR_FLAG_OPAQUE);
      /* Safe since prsr_writeData leaves >= 1 char free */
      *stack->ptr++ = (char) (stackmode | cancelSpace);
    }
  }
  parser->stackmode = stackmode;
  
  /* This is a non-empty element */

  /* Some HTML elements can take CDATA as content */
  if (element != WE_PRSR_UNKNOWN_ELEMENT &&
      (parser->p.elementTable[element] & WE_PRSR_CDATA) &&
       parser->html_based) {
    switch (element) {
    case WBS_HTML_ELEMENT_STYLE:
      parser->type = WE_PRSR_SECTION_STYLE;
      break;
    case WBS_HTML_ELEMENT_SCRIPT:
      parser->type = WE_PRSR_SECTION_SCRIPT;
      break;
    case WBS_HTML_ELEMENT_PLAINTEXT:
      parser->type = WE_PRSR_SECTION_PLAINTEXT;
      break;
    case WBS_HTML_ELEMENT_TEXTAREA:
      parser->type = WE_PRSR_SECTION_TEXTAREA;
      break;
    }
    return WE_PRSR_CDATA;
  }

  return returnToken;
}



/*
 * Parse a CDATA section, HTML plaintext or style content. All
 * character data is written to the stack.
 *
 * 's' is the current position, 'end' end of buffer, '*nextPtr'
 * the current pointer for the next iteration and 'limit' number
 * of bytes left to process before a pause.
 *
 * 'limit' is not used.
 */
static int
we_prsr_cdataSectionParser (we_prsr_parser_t*  parser,
                             const char*         s,
                             const char*         end,
                             const char*        *nextPtr,
                             int                 limit)
{
  int tok;
  int n = 0;
  int noMoreData;
  int strict;
  int error;

  limit = limit;
  
  noMoreData = *nextPtr == NULL;
  error = WE_PRSR_STATUS_OK;

  for (;;) {
    strict = parser->type == WE_PRSR_SECTION_TEXTAREA; /* refs are used if strict == TRUE */
    tok = we_tok_opaqueDataTok (s, end, nextPtr, &strict);
    

    if (tok != WE_TOK_CHAR_REF && tok != WE_TOK_ENTITY_REF && *nextPtr - s > 0) {
      error = we_prsr_writeString (parser, s, *nextPtr - s);
      if (error) {
        *nextPtr = s;
        return error;
      }
    }
  
    if (tok == WE_TOK_MORE) {
      if (noMoreData && !parser->meta_scan) {
        int dummy;
        /* Unexpected end of document is interpreted as a missing </html> */
        if (parser->type == WE_PRSR_SECTION_COMMENT) {
          parser->stack.mode = parser->stack.mode & ~STACKMODE_NO_TEXT;
          if (parser->stack.mode != STACKMODE_NONE)
            we_prsr_endSession (parser);
        }
        if (parser->mode == WE_PRSR_HTML_MODE)
          we_prsr_closeElement (parser, WBS_HTML_ELEMENT_HTML, TRUE, &dummy);
        else
          we_prsr_closeElement (parser, WBS_HTML_ELEMENT_WML, TRUE, &dummy);
        
        return WE_PRSR_STATUS_OK;
      } 
      else {
        return WE_PRSR_STATUS_MORE;
      }
    }

    switch (tok) {
    case WE_TOK_CDATA_SECT_CLOSE:
      if (parser->type == WE_PRSR_SECTION_CDATA) {
        *nextPtr += 3; /* Compensate for the left out "]]>" */
        parser->stack.cancelSpace = FALSE;
        goto End;
      } else
        n = 3;
      break;
    
    case WE_TOK_PLAINTEXT_END_TAG:
      if (parser->type == WE_PRSR_SECTION_PLAINTEXT)
        goto End;
      else
        n = 12;
      break;

    case WE_TOK_STYLE_END_TAG:
      if (parser->type == WE_PRSR_SECTION_STYLE)
        goto End;
      else
        n = 8;
      break;

    case WE_TOK_SCRIPT_END_TAG:
      if (parser->type == WE_PRSR_SECTION_SCRIPT &&
          parser->state == WE_PRSR_STATE_CONTENT) {
        goto End;
      }
      n = 9;
      break;

    case WE_TOK_TEXTAREA_END_TAG:
      if (parser->type == WE_PRSR_SECTION_TEXTAREA)
        goto End;
      else
        n = 11;
      break;

    case WE_TOK_COMMENT_OPEN:
      if (parser->type == WE_PRSR_SECTION_SCRIPT) {
        parser->state = WE_PRSR_STATE_SCRIPT_CMNT;
      }
      n = 0;
      break;

    case WE_TOK_COMMENT_CLOSE:
      if (parser->type == WE_PRSR_SECTION_COMMENT) {
        *nextPtr += 3; /* Compensate for the left out "-->" */
        parser->stack.mode ^= STACKMODE_NO_TEXT;
        parser->processor = parser->returnProc;
        return WE_PRSR_STATUS_RETURN;
      }
      else if (parser->type == WE_PRSR_SECTION_SCRIPT) {
        parser->state = WE_PRSR_STATE_CONTENT;
      }
      n = 3;
      break;
      
    case WE_TOK_ENTITY_REF: /* entity ref (&amp;) */
      error = we_prsr_writeEntityRef (parser, s, *nextPtr - s);
      n = 0;
      break;
      
    case WE_TOK_CHAR_REF: /* character ref: &#32; or &#x20; */
      error = we_prsr_writeCharacterRef (parser, s, *nextPtr - s);
      n = 0;
      break;
      
    case WE_TOK_CHAR_DATA:
      /* Text already written */
      n = 0;
      break;
    }

    if (!error && n > 0) {
      error = we_prsr_writeString (parser, *nextPtr, n);
    }
    
    if (error) {
      *nextPtr = s;
      return error;
    }

    if (parser->stack.error)
      return parser->stack.error;

    s = *nextPtr + n;
  }

End:
  we_prsr_endSession (parser);
  parser->processor = parser->returnProc;
  return WE_PRSR_STATUS_RETURN;
}



/*
 * Parse the head section of a HTML or XHTML document searching for
 * a meta element containing charset information.
 *
 * 's' is the current position, 'end' end of buffer, '*nextPtr'
 * the current pointer for the next iteration and 'limit' number
 * of bytes left to process before a pause.
 */
static int 
we_prsr_meta_scan_parser (we_prsr_parser_t*  parser,
                           const char*         s,
                           const char*         end,
                           const char*        *nextPtr,
                           int                 limit)
{
  const char       *start;
  const char       *next;
  const char       *name;
  we_tok_data_t    tokData;
  int               tok;
  int               size;     
  int               result;
  int               error;

  error = WE_PRSR_STATUS_OK;
  start = s;
  while (s - start < limit) {
    tok = we_tok_contentTok (s, end, &next, &tokData);
    if (parser->mode == WE_PRSR_XML_MODE && ! tokData.strict) { /* XML and using HTML extensions */
      return WE_PRSR_ERROR_SYNTAX;
    }

    switch (tok) {
    case WE_TOK_MORE: /* --------------------------------  out of data or partial element */
      return WE_PRSR_STATUS_MORE;

    case WE_TOK_IGNORE:  /* -----------------------------  ignored invalid data */
      if (parser->mode != WE_PRSR_XML_MODE) {
        break;
      }
      /* fall through */
    
    case WE_TOK_INVALID: /* -----------------------------  invalid data */
      return WE_PRSR_ERROR_SYNTAX;
    
    case WE_TOK_START_TAG: /* ---------------------------  start tag or empty tag */    
      {
        we_prsr_attribute_t* attrs;
        int                   size;     
        int                   element;
        int                   result;
        int                   returnToken;
        int                   endOfDoc = *nextPtr == NULL;

        returnToken = WE_PRSR_STATUS_OK;

        we_prsr_newSession (parser, STACKMODE_NONE);
        result = we_prsr_elemTagParser (parser, s, end, &next, NULL, &element,
                                         &attrs, &size, FALSE, endOfDoc);
        we_prsr_endSession (parser);

        if (result == WE_PRSR_STATUS_MORE) {
          return WE_PRSR_STATUS_MORE;
        }
        else if (result == WE_PRSR_ERROR_SYNTAX) {
          if (parser->mode != WE_PRSR_XML_MODE) {
            break;
          } 
          return WE_PRSR_ERROR_SYNTAX;
        }

        if (element == WBS_HTML_ELEMENT_META) {
          /* Catch meta charset */
          int i;
          int index_http_equiv = -1;
          int index_content    = -1;
          
          for (i = 0; i < size; i++) {
            switch (attrs[i].type) {
            case WBS_HTML_ATTRIBUTE_HTTP_EQUIV:
              if (! we_cmmn_strcmp_nc (attrs[i].u.s, "content-type")) 
                index_http_equiv = i;
              break;
            case WBS_HTML_ATTRIBUTE_CONTENT:
              index_content = i;
              break;
            }
          }
          if (index_http_equiv >= 0 && index_content >= 0) {
            /* Meta charset found */
            int charset = we_prsr_getCharsetFromString (attrs[index_content].u.s);

            if (charset >= 0 &&
              charset != WE_CHARSET_UCS_2    && charset != WE_CHARSET_UTF_16 &&
              charset != WE_CHARSET_UTF_16LE && charset != WE_CHARSET_UTF_16BE) {
              /* An ASCII based charset was found */
              parser->charset = charset;
            }
            we_prsr_free_attributes (parser->modId, attrs, size);
            return WE_PRSR_STATUS_NEW_CHARSET;
          }
        }

        we_prsr_free_attributes (parser->modId, attrs, size);

        if (element != WE_PRSR_UNKNOWN_ELEMENT &&
            !(parser->htmlTable[element] & WE_PRSR_HEAD)) {
          /* Done searching, body element found */
          return WE_PRSR_STATUS_NEW_CHARSET;
        }
        else if (element == WBS_HTML_ELEMENT_STYLE) {
          parser->type = WE_PRSR_SECTION_STYLE;
          goto Content_cdata;
        }
        else if (element == WBS_HTML_ELEMENT_SCRIPT) {
          parser->type = WE_PRSR_SECTION_SCRIPT;
          goto Content_cdata;
        }
      }
      break;

    case WE_TOK_END_TAG: /* -----------------------------  end tag */
      result = we_prsr_elemTagParser (parser, s, end, &next, &name, NULL,
                                       NULL, &size, FALSE, *nextPtr == NULL);

      if (parser->mode == WE_PRSR_XML_MODE && result == WE_PRSR_ERROR_SYNTAX) {
        return WE_PRSR_ERROR_SYNTAX;
      }
      break;
    
    case WE_TOK_CDATA_SECT_OPEN: /* ---------------------  CDATA */
      parser->type = WE_PRSR_SECTION_CDATA;
Content_cdata:
      we_prsr_newSession (parser, STACKMODE_NO_TEXT);
      parser->processor = we_prsr_cdataSectionParser;
      *nextPtr = next;
      return WE_PRSR_STATUS_RETURN;
    

    case WE_TOK_COMMENT_OPEN: /* ------------------------  comment */
      parser->type = WE_PRSR_SECTION_COMMENT;
      parser->processor = we_prsr_cdataSectionParser;
      *nextPtr = next;
      return WE_PRSR_STATUS_RETURN;

    case WE_TOK_INVALID_CHAR: /* ------------------------  non-XML character */
    case WE_TOK_ENTITY_REF: /* --------------------------  entity ref (&amp;) */
    case WE_TOK_CHAR_REF: /* ----------------------------  character ref: &#32; or &#x20; */
    case WE_TOK_CHAR_DATA: /* ---------------------------  text data */
    case WE_TOK_XML_DECL: /* ----------------------------  xml PI */
    case WE_TOK_PI: /* ----------------------------------  non-xml PI */
      break;
    }

    if (error) {
      *nextPtr = s;
      return error;
    }

    if (parser->stack.error)
      return parser->stack.error;
    
    s = next;
  }

  *nextPtr = s;
  return WE_PRSR_STATUS_PAUSE;
}
/* ======== attribute functions ======== */

/*
 * Convert a char string 's' containing a positive number to an
 * integer. If the number is followed by a "%" then it is negated.
 *
 * Accepted forms are only "123" and "123%". Surrounding white
 * space is ignored as well as any trailing data, that is "123px"
 * is interpreted as "123".
 *
 * 'end' is not used.
 *
 * '*error' is set to FALSE on success else TRUE.
 */

int
we_prsr_read_num (const char* s, const char* end, int *error)
{
  int num;
  
  if (! ct_isdigit (*s)) {
    *error = TRUE;
    return 0;
  }

  num = atoi (s);
  while (ct_isdigit (*s))
    s++;
  if (*s == '%') {
    num = - num;
  }
  
  *error = FALSE;
  return num;
}



/*
 * Normalize an attribute value 'data' to to 'type'. 'name' is
 * the element name as string. 'nameLen' and 'dataLen' are the
 * corresponding lengths. Used as an inparameter, '*flag' is set
 * to the number of "&" characters 's' contains.
 *
 * Used as an outparameter '*flag' signals if the returned union
 * contains a string or an integer. '*error' is 0 if 'data'
 * was successfully normalized, else the returned union is
 * undefined. '*error' is > 0 if a syntax error, < 0 if out of
 * memory.
 *
 * 'nameLen' is only used if 'type' is TYPE_FLAG. If its value is
 * -1, then strlen is used on 'name' to get the length of the name.
 */
we_prsr_attr_value_t
we_prsr_normalizeAttribute (we_prsr_parser_t*  parser,
                             int                 type,
                             const char*         name,
                             int                 nameLen,
                             const char*         data,
                             int                 dataLen,
                             int                *flag,
                             int                *error)
{
  we_prsr_attr_value_t u;
  const char*           p;    
  const char*           start;    
  const char*           end;    
  int                   i;
  int                   size;
  int                   tok;
  int                   strict;

  *error = 0;
  u.i = 0;
  
  if (we_prsr_newSession (parser, STACKMODE_ATTRIBUTE)) { /* Out of memory */
    *error = -1;
    return u;
  }

  /* Replace references and remove whitespace.
   * The number of '&' characters is known from the parsing. */
  start = data;
  for (i = 0; i < *flag; i++) {
    p = strchr (start, '&');
    tok = we_tok_scanRef (p + 1, data + dataLen + 1, &end, &strict);
    
    if (tok == WE_TOK_CHAR_DATA)
      p = end;
    we_prsr_writeString (parser, start, p - start);
    
    if (tok == WE_TOK_CHAR_DATA)
      start = p;
    else {
      if (tok == WE_TOK_CHAR_REF)
        we_prsr_writeCharacterRef (parser, p, end - p);
      else
        we_prsr_writeEntityRef (parser, p, end - p);
      start = end;  
    }
  }
  size = (data + dataLen) - start;
  if (size > 0)
    we_prsr_writeString (parser, start, size);
  if (parser->stack.error) {
    *error = -1;
    return u;
  }

  p = data; /* Signals new session */
  data = parser->stack.sessionStart;
  dataLen = parser->stack.ptr - parser->stack.sessionStart;

  switch (type & ~WE_PRSR_TYPE_NO_CASE) {
/*
  case WE_PRSR_TYPE_ID:
  case WE_PRSR_TYPE_IDREF:
  case WE_PRSR_TYPE_IDREFS:
  case WE_PRSR_TYPE_NMTOKEN:
  case WE_PRSR_TYPE_NMTOKENS:
*/
  case WE_PRSR_TYPE_CDATA:
    *flag = WE_PRSR_ATTRVAL_STR;

    u.s = WE_MEM_ALLOC (parser->modId, dataLen + 1);
    if (u.s == NULL) {
      *error = -1;
      return u;
    }

    if (type & WE_PRSR_TYPE_NO_CASE)
      we_cmmn_strncpy_lc (u.s, data, dataLen);
    else
      /* 
       * we_cmmn_strndup is not used since this function is used by
       * the WBXML decoder and the data can contain null characters.
       */
      memcpy (u.s, data, dataLen);
    u.s[dataLen] = '\0';
    break;
    
  case WE_PRSR_TYPE_NUM:
  case WE_PRSR_TYPE_PRCNT:
    *flag = WE_PRSR_ATTRVAL_INT;
    if (dataLen == 0) {
      *error = 1;
      break;
    }
    *parser->stack.ptr = '\0';
    u.i = we_prsr_read_num (data, data + dataLen, error);
    if (u.i < 0 && (type & ~WE_PRSR_TYPE_NO_CASE) == WE_PRSR_TYPE_NUM)
      *error = 1;
    break;

  case WE_PRSR_TYPE_FLAG:
    *flag = WE_PRSR_ATTRVAL_INT;
    if (parser->mode != WE_PRSR_XML_MODE)
      break; /* Anything goes for HTML */
    if (nameLen < 0)
      nameLen = strlen (name);
    if (dataLen == nameLen) {
      if (type & WE_PRSR_TYPE_NO_CASE) {
        if (!we_cmmn_strncmp_nc (data, name, dataLen))
          break;
      } else {
        if (!strncmp (data, name, dataLen))
          break;
      }
    }
    *error = 1;
    break;

  default: /* enumerated type */
    *flag = WE_PRSR_ATTRVAL_INT;
    if (dataLen == 0) {
      *error = 1;
      break;
    }
    if (type & WE_PRSR_TYPE_NO_CASE)
      u.i = we_cmmn_str2int_lc (data, dataLen, parser->p.attrvals);
    else
      u.i = we_cmmn_str2int (data, dataLen, parser->p.attrvals);
    i = u.i >> 8; /* u.i < 0x8000 or -1 */
    if (u.i == -1 ||
        (i >= 10 ? i != (type & 63): ! ((1 << i) & ((type & ~WE_PRSR_TYPE_NO_CASE) >> 6))))
      *error = 1;
    break;
  }

  we_prsr_endSession (parser);

  return u;
}



/*
 * Parse and extract attribute value pairs. Returns the size of
 * the '*attrs' vector. -1 if the the input was malformatted or
 * -2 if out of memory.
 *
 *   attr="val" att2="val" ?>
 * |                       |
 * s                       end
 */
int
we_prsr_extract_attributes (we_prsr_parser_t*          parser,
                             const char*                 s,
                             const char*                 end,
                             const we_strtable_info_t*  attributes,
                             const WE_UINT16*           attributeType,
                             we_prsr_attribute_t*      *attrs)
{
  const we_strtable_info_t* tempAttributes;
  const WE_UINT16*          tempAttributeType;
  const char*                next;
  int                        size;

  tempAttributes    = parser->p.attributes;
  tempAttributeType = parser->p.attributeType;

  parser->p.attributes    = attributes;
  parser->p.attributeType = attributeType;

  if (we_prsr_elemTagParser (parser, s, end, &next, NULL, NULL, attrs, &size, TRUE, TRUE) != 
                                                                          WE_PRSR_STATUS_OK)
    size = -1;

  




  parser->p.attributes    = tempAttributes;
  parser->p.attributeType = tempAttributeType;

  return size;
}


/* ======== stack functions ======== */

/*
 * Expand the stack space. At least another 'len' characters
 * are needed. If 'len' == 0 the a default size of
 * WE_PRSR_CFG_INITIAL_STACK_SIZE / 2 is used.
 *
 * Returns an error code.
 */
int
we_prsr_expandStack (we_prsr_parser_t* parser, int len)
{
  we_prsr_stack_t* stack = &parser->stack;
  char*             temp;
  int               new_size;
  int               old_size;

  WE_PRSR_LOG_ERROR (NULL, NULL, "Stack expanded");

  if (len == 0)
    len = WE_PRSR_CFG_INITIAL_STACK_SIZE / 2;
  else
    len += 100;

  old_size = stack->ptr - stack->start;
  new_size = old_size + len;
  temp     = (char*) WE_MEM_ALLOC (parser->modId, new_size);
  if (temp == NULL) {
    stack->mode = STACKMODE_NO_TEXT;
    stack->error = WE_PRSR_ERROR_OUT_OF_MEMORY;
    return WE_PRSR_ERROR_OUT_OF_MEMORY;
  }

  memcpy (temp, stack->start, old_size);
  WE_MEM_FREE (parser->modId, stack->start);

  stack->end          = temp + new_size;
  stack->ptr          = temp + old_size;
  stack->sessionStart = temp + (stack->sessionStart - stack->start);
  stack->start        = temp;

  return WE_PRSR_STATUS_OK;
}



/*
 * Empty all data belonging to the current session by sending it
 * the character data handler. Multibyte characters are not split.
 * Instead these excess bytes are moved to sessionStart.
 *
 * Return an error code.
 */
int
we_prsr_flushText (we_prsr_parser_t* parser, int final)
{
  we_prsr_stack_t*  stack = &parser->stack;
  char*              s;
  int                len;
  char               ch;
  int                i;
  int                size;
  int                error;

  len = stack->ptr - stack->sessionStart;
  if (len == 0)
    return WE_PRSR_STATUS_OK;
  else
    if (parser->charset == WE_CHARSET_UNKNOWN && (parser->mode == WE_PRSR_HTML_MODE) &&
        (parser->tagLevel == 1 ||
          (parser->tagLevel == 2 && parser->stack.cancelSpace_bu != -1))) {
          
      /* Missing a body element in a HTML document without charset information. */
      stack->overflow = FALSE;
      return WE_PRSR_STATUS_NEW_CHARSET;
    }

  s = stack->ptr;
  
  /* Don't split multi-byte characters */
  
  /* Find first byte of a multi-byte character */
  for (i = 1; (((unsigned char)s[-i] & 0xC0) == 0x80) && (i < 4); i++) {
    /* Empty */
  }

  size = we_cmmn_utf8_charlen (s - i);
  if (i == size || size == 0) {
    /*
     *'s' points to the last byte, ok to break here.
     *
     * If size == 0 then this is not a Unicode 3 UTF-8 encoded character
     * or a null-byte. Cannot recover, just split character.
     */
    i = 0;
  }

  s -= i;
  ch = *s;
  *s = '\0';
  error = parser->p.charHandler (parser->p.userData, stack->sessionStart, len, stack->mode);

  switch (error) {
  case WE_PRSR_APPLICATION_ERROR:
    stack->mode = STACKMODE_NO_TEXT;
    stack->error = WE_PRSR_ERROR_APPLICATION;
    return WE_PRSR_ERROR_APPLICATION;
  case WE_PRSR_APPLICATION_OUT_OF_MEMORY:
    stack->mode = STACKMODE_NO_TEXT;
    stack->error = WE_PRSR_ERROR_OUT_OF_MEMORY;
    return WE_PRSR_ERROR_OUT_OF_MEMORY;
  }

  if (final)
    i = 0;

  if (i > 0) {
    /* A half character at the end. Move it to sessionStart. */
    s = stack->sessionStart;
    *s++ = ch;
    i--;
    memcpy (s, stack->ptr - i, i);
    stack->ptr = s + i;

  } else
    stack->ptr = stack->sessionStart;

  return WE_PRSR_STATUS_OK;
}



/*
 * Copy 'len' characters from 's' to the stack. If 'len' is zero,
 * then 's' i assumed to be null-terminated and the whole string
 * is copied.
 *
 * When 'flush' is TRUE, flush the stack from text if needed else
 * expand the stack.
 */
void
we_prsr_writeData (we_prsr_parser_t* parser, const char* s, int len, int flush)
{
  we_prsr_stack_t* stack = &parser->stack;
  int i;
  int todo;

  if (stack->mode & WE_PRSR_FLAG_NO_TEXT) /* Only used when out of memory */
    return;

  if (len == 0)
    todo = strlen (s);
  else
    todo = len;

  if (! flush && todo > stack->end - stack->ptr - 1) /* Need room for an extra '\0' */
    if (we_prsr_expandStack (parser, todo + 1))
      return;

  for (;;) {
    i = MIN (todo, stack->end - stack->ptr - 1);
    
    if (stack->downcase)
      we_cmmn_strncpy_lc (stack->ptr, s, i); /* Used for HTML element and attribute names */
    else
      memcpy (stack->ptr, s, i);

    stack->ptr += i;
    todo -= i;
    s += i;
    if (todo > 0) {
      if (we_prsr_flushText (parser, FALSE))
        return;
    } else
      break;
  }
}



/*
 * Copy 'len' source characters from 's' to the stack. CR or CRLF
 * are replaced by LF. Whitespace are removed if the WE_PRSR_FLAG_NORMALIZE
 * is set in stack->mode.
 *
 * Flush the stack from text if WE_PRSR_FLAG_FLUSH is set.
 *
 * Returns an error code.
 */
int
we_prsr_writeString (we_prsr_parser_t* parser, const char* s, int len)
{
  we_prsr_stack_t* stack = &parser->stack;
  const char*       p;
  char*             q;
  int               cr;
  int               sp;
  int               freeSpace;
  int               error;

  if (stack->mode & WE_PRSR_FLAG_NO_TEXT)
    return WE_PRSR_STATUS_OK;

  freeSpace = stack->end - stack->ptr - 2; /* Need extra room for '\0' and maybe '\n' or ' ' */

  if (stack->mode & WE_PRSR_FLAG_NORMALIZE) { /* remove whitespace */
    sp = stack->overflow;
    for (p = s, q = stack->ptr; p < s + len; p++) {
      if (freeSpace <= 0) {
        stack->ptr = q;

        if (stack->mode & WE_PRSR_FLAG_FLUSH) {
          error = we_prsr_flushText (parser, FALSE);
          if (error)
            return error;
        } else {
          if (we_prsr_expandStack (parser, s + len - p)) /* Upper limit for the needed space */
            return WE_PRSR_ERROR_OUT_OF_MEMORY;
        }

        freeSpace = stack->end - stack->ptr - 2; 
        q = stack->ptr;
      }

      if (ct_isxmlwhitespace (*p)) {
        sp = TRUE;
      } else {
        if (sp) {
          sp = FALSE;
          if (! stack->cancelSpace) {
            freeSpace--;
            *q++ = ' ';
          }
        }
        stack->cancelSpace = FALSE;
        freeSpace--;
        *q++ = *p;
      }
    }
    stack->overflow = sp;
    if (parser->charset == WE_CHARSET_UNKNOWN && (parser->mode == WE_PRSR_HTML_MODE) &&
        parser->stack.mode != STACKMODE_ATTRIBUTE &&
        (parser->tagLevel == 1 || 
          (parser->tagLevel == 2 && parser->stack.cancelSpace_bu != -1)) &&
          
        q - stack->sessionStart > 0 ) {
        /* Missing a body element in a HTML document without charset information. */
        stack->overflow = FALSE;
        return WE_PRSR_STATUS_NEW_CHARSET;
    }
  }
  else { /* preserve whitespace */
    cr = stack->overflow;
    for (p = s, q = stack->ptr; p < s + len; p++) {
      if (freeSpace <= 0) {
        stack->ptr = q;
        
        if (stack->mode & WE_PRSR_FLAG_FLUSH) {
          error = we_prsr_flushText (parser, FALSE);
          if (error)
            return error;
        } else {
          if (we_prsr_expandStack (parser, s + len - p)) /* Upper limit for the needed space */
            return WE_PRSR_ERROR_OUT_OF_MEMORY;
        }

        freeSpace = stack->end - stack->ptr - 2; 
        q = stack->ptr;
      }

      if (*p == WE_PRSR_CHAR_CR && !(stack->mode & WE_PRSR_FLAG_OPAQUE)) {
        if (cr) {
          freeSpace--;
          *q++ = WE_PRSR_CHAR_LF;
        } else
          cr = TRUE;

      } else {
        if (cr) {
          cr = FALSE;
          if (*p != WE_PRSR_CHAR_LF) {
            freeSpace--;
            *q++ = WE_PRSR_CHAR_LF;
          }
        }
        freeSpace--;
        *q++ = *p;
      }
    }
    stack->overflow = cr;
  } 

  stack->ptr = q;
  return WE_PRSR_STATUS_OK;
}



/*
 * Start a new session. Returns error code: WE_PRSR_STATUS_OK or
 * WE_PRSR_ERROR_OUT_OF_MEMORY.
 */
static int
we_prsr_newSession (we_prsr_parser_t* parser, int mode)
{
  we_prsr_stack_t*  stack = &parser->stack;
  unsigned int       len;

  if (mode != STACKMODE_NONE && stack->end - stack->sessionStart < WE_PRSR_CFG_MIN_FREE_STACK ||
      stack->ptr == stack->end) {
    if (we_prsr_expandStack (parser, 0))
      return WE_PRSR_ERROR_OUT_OF_MEMORY;
  }

  stack->overflow = FALSE;
  stack->mode = mode;

  len = stack->ptr - stack->sessionStart + 2;
  
  *stack->ptr++ = (unsigned char) len;
  *stack->ptr++ = (unsigned char) (len >> 8);
  stack->sessionStart = stack->ptr;

  if (mode == STACKMODE_ATTRIBUTE) {
    stack->cancelSpace_bu = stack->cancelSpace;
    stack->cancelSpace = TRUE;
  }

  return WE_PRSR_STATUS_OK;
}



/*
 * Return to previous session. Only elements are stored on the
 * stack, and therefore the previous mode is not needed to be
 * restored.
 */
static int
we_prsr_endSession (we_prsr_parser_t* parser)
{
  we_prsr_stack_t* stack = &parser->stack;
  unsigned int      len;
  int               error;

  if (stack->mode == STACKMODE_NORMALIZE) {
    if (stack->overflow && ! stack->cancelSpace) {
      *stack->ptr++ = ' ';
      stack->cancelSpace = TRUE;
    }
    error = we_prsr_flushText (parser, TRUE);
    if (error)
      return error;

  } else if (stack->mode == STACKMODE_PRESERVE ||
             stack->mode == STACKMODE_OPAQUE) {
    if (stack->overflow)
      *stack->ptr++ = WE_PRSR_CHAR_LF; 
    error = we_prsr_flushText (parser, TRUE);
    if (error)
      return error;
  
  } else if (stack->mode == STACKMODE_ATTRIBUTE) {
    stack->cancelSpace = stack->cancelSpace_bu;
  }

  stack->ptr = stack->sessionStart - 1;
  len = *(unsigned char*) stack->ptr--;
  len = (len << 8) + *(unsigned char*) stack->ptr;
  stack->sessionStart -= len;
  stack->mode = STACKMODE_NONE;

  return WE_PRSR_STATUS_OK;
}


/**************************************************************
 * Log functionality                                          *
 **************************************************************/

#ifdef WE_PRSR_LOG

/*
 * LOG_PRSR_PRINTF (fmt, arg) <=> printf (fmt, arg)
 */

#if defined WE_PRSR_LOG_API && defined WE_PRSR_LOG_WBS_FILE
void parser_log_func (char *s, int logtype);
WE_BOOL parserLogActive = TRUE;

#define LOG_PRSR_PRINTF(f, e)   { char tmpStr[128];tmpStr[sizeof(tmpStr) - 1] = 0; \
                                  _snprintf(tmpStr, sizeof(tmpStr) - 1, f, e); \
                                  parser_log_func (tmpStr, 1);\
                                  Wbs_Hvld_log_msg (f, e); }

#elif defined WE_PRSR_LOG_API
void parser_log_func (char *s, int logtype);
WE_BOOL parserLogActive = TRUE;

#define LOG_PRSR_PRINTF(f, e)   { char tmpStr[128];tmpStr[sizeof(tmpStr) - 1] = 0; \
                                  _snprintf(tmpStr, sizeof(tmpStr) - 1, f, e); \
                                  parser_log_func (tmpStr, 1); }

#elif defined WE_PRSR_LOG_WBS_FILE
void Wbs_Hvld_log_msg (const char *format, ...);

#define LOG_PRSR_PRINTF(f, e)   Wbs_Hvld_log_msg (f, e)       


#elif defined WE_PRSR_LOG_CONSOLE
#define LOG_PRSR_PRINTF(f, e)   printf(f, e)


#elif defined WE_PRSR_LOG_STD
#define LOG_PRSR_PRINTF(f, e)   WE_LOG_MSG ((WE_LOG_DETAIL_MEDIUM, parser->modId, f, e))

#endif


/*
 * Print the line of the document causing the error.
 */
void
we_prsr_logError (we_prsr_parser_t* parser, const char* s, const char* error)
{
  char  buf[71];
  char  dash[] = "----------";
  char* start;
  char* end;
  int   pre;
  int   post;
  int   i;

  if (s == NULL) {
#if WE_PRSR_STATISTICS
    LOG_PRSR_PRINTF ("\nPARSER: %s. *******************\n", error);
#endif
    return;
  }

  start = parser->bufferStart;
  end = parser->bufferEnd;
  
  if (s < start || s > end || s == NULL) {
    LOG_PRSR_PRINTF ("\nPARSER: %s. Error location unknown.\n", error);
    return;
  }

  pre = MIN (s - start, 10);
  for (i = -1; i >= -pre; i--)
    if (ct_isxmlwhitespace (s[i]))
      buf[i + 10] = ' ';
    else
      buf[i + 10] = s[i];
  
  post = MIN (end - s, 60);
  buf [post + 10] = '\0';  
  for (i = 0; i < post; i++)
    if (ct_isxmlwhitespace (s[i]))
      buf[i + 10] = ' ';
    else if (ct_isctrl (s[i]))
      buf[i + 10] = '.';
    else
      buf[i + 10] = s[i];

  if (post == 0) {
    LOG_PRSR_PRINTF ("\nPARSER: %s. No data available.\n", error);
    return;
  }

  LOG_PRSR_PRINTF ("\nPARSER: %s.\n", error);
  LOG_PRSR_PRINTF ("PARSER: %s\n", buf - pre + 10);
  
  dash [pre] = '\0';
  LOG_PRSR_PRINTF ("PARSER: %s^\n", dash);
}

#endif /* WE_PRSR_LOG */
