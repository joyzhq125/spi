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
 * We_Ptxt.h
 *
 * XML and HTML parser: text parser and main functions
 */

#ifndef _we_ptxt_h
#define _we_ptxt_h

#ifndef _we_prsr_h
#include "We_Prsr.h"
#endif

#ifndef _we_ptok_h
#include "We_Ptok.h"
#endif

#ifndef _we_dcvt_h
#include "We_Dcvt.h"
#endif


/**************************************************************
 * Constants                                                  *
 **************************************************************/

/* Internal status codes */
#define WE_PRSR_STATUS_OK            0    /* All went well.                            */
#define WE_PRSR_STATUS_MORE          13   /* Need more input.                          */
#define WE_PRSR_STATUS_RETURN        14   /* Changed processor, call again.            */
#define WE_PRSR_STATUS_PAUSE         15   /* Time slot spent, but there is more to do. */
#define WE_PRSR_STATUS_NEW_CHARSET   16   /* Change character set to use.              */
#define WE_PRSR_STATUS_START_TAG     17   /* Start tag                                 */
#define WE_PRSR_STATUS_EMPTY_ELEMENT 18   /* Empty element                             */
#define WE_PRSR_STATUS_END_TAG       19   /* End tag                                   */
#define WE_PRSR_STATUS_META_SCAN     20   /* Start scanning for meta charset           */


/* Internal application return code */
#define WE_PRSR_APPLICATION_WARNING      3   /* Recoverable error if HTML, else failure */
#define WE_PRSR_APPLICATION_HTML         4   /* Returned by the HTML init funct for HTML docs */
#define WE_PRSR_APPLICATION_OK_BODY      5   /* This is the beginning of the document body */
#define WE_PRSR_APPLICATION_WARNING_BODY 7   /* Warning; beginning of the document body */
#define WE_PRSR_APPLICATION_WML          9   /* Returned by the HTML init funct on WML docs    */


/* Type of opaque data */
#define WE_PRSR_SECTION_CDATA           0    /* Data is CDATA */
#define WE_PRSR_SECTION_COMMENT         1    /* Data is a comment */
#define WE_PRSR_SECTION_PLAINTEXT       2    /* Data is plaintext content */
#define WE_PRSR_SECTION_STYLE           3    /* Data is style content */
#define WE_PRSR_SECTION_SCRIPT          4    /* Data is script content */
#define WE_PRSR_SECTION_TEXTAREA        5    /* Data is textarea content */


/* Character constants */
#define WE_PRSR_CHAR_CR          0x0D /* caridge return */
#define WE_PRSR_CHAR_LF          0x0A /* line feed */


/* Internal flags used for indicate text handling mode */
#define WE_PRSR_FLAG_FLUSH           0x01
#define WE_PRSR_FLAG_NORMALIZE       0x02
#define WE_PRSR_FLAG_ATTRIBUTE       0x04
#define WE_PRSR_FLAG_NO_TEXT         0x08

#define WE_PRSR_FLAG_CANCEL_SPACE    0x10
#define WE_PRSR_FLAG_OPAQUE          0x20
#define WE_PRSR_FLAG_WBXML_WML       0x40
#define WE_PRSR_FLAG_WBXML_VARIABLE  0x80


/* Internal constants for text handling modes */
#define STACKMODE_NONE            0
#define STACKMODE_NORMALIZE       (WE_PRSR_FLAG_NORMALIZE | WE_PRSR_FLAG_FLUSH)
#define STACKMODE_PRESERVE        (WE_PRSR_FLAG_FLUSH)
#define STACKMODE_ATTRIBUTE       (WE_PRSR_FLAG_NORMALIZE | WE_PRSR_FLAG_ATTRIBUTE)
#define STACKMODE_WBXML_WML       (WE_PRSR_FLAG_WBXML_WML)
#define STACKMODE_NO_TEXT         (WE_PRSR_FLAG_NO_TEXT)
#define STACKMODE_OPAQUE          (WE_PRSR_FLAG_FLUSH | WE_PRSR_FLAG_OPAQUE)
                         

/* Internal text handling flag */           
#define WE_PRSR_CDATA            0x40 /* Data is CDATA */

/* Text handling flags specific for HTML */
#define WE_PRSR_EMPTY            0x01 /* Empty element */
#define WE_PRSR_MISIATCH         0x02 /* Elements allowed to be misiatched */
#define WE_PRSR_CLOSE            0x04 /* Element may cause closure of another */
#define WE_PRSR_HEAD             0x08 /* Element may be in the head section */
#define WE_PRSR_BLOCK            0x10 /* This is a block element */
#define WE_PRSR_STOP             0x20 /* This element takes block as data */
#define WE_PRSR_HEADING          0x40 /* This element is a heading element */


/* Parser states */

/*
 *   ----------------------------------------
 *   |                         |            |
 *   |                         v            v
 * start ---> xml_decl ---> doctype ---> content <----> script_cmnt
 *                |            |            ^ ^
 *                |            |            | |
 *                |            -------------- |
 *                |                           |
 *                -----------------------------
 */

#define WE_PRSR_STATE_START        0    /* Passed start                           */
#define WE_PRSR_STATE_XML_DECL     1    /* Found and used XML PI                  */
#define WE_PRSR_STATE_DOCTYPE      2    /* Parsed the DOCTYPE statement           */
#define WE_PRSR_STATE_CONTENT      3    /* Instance start found; parsed prologue  */
#define WE_PRSR_STATE_SCRIPT_CMNT  4    /* Inside an XML comment in a script      */

#define WE_PRSR_STATE_DECODER      10   /* Using the WBXML decoder                */

/* Parser->html modes */
#define WE_PRSR_XML_MODE           0
#define WE_PRSR_HTML_MODE          1
#define WE_PRSR_EXTENDED_WML_MODE  2
/**************************************************************
 * Type definitions                                           *
 **************************************************************/

typedef char* (*strchr_t) (const char* s, int c);

typedef int (*we_prsr_processor_t) (we_prsr_parser_t*  parser,
                                     const char*         start,
                                     const char*         end,
                                     const char*        *nextPtr,
                                     int                 limit);

/* Text buffer handling */
typedef struct we_prsr_stack_st {
  char* sessionStart;     /* Start of current session */
  char* ptr;              /* Position of next character to write */
  char* start;            /* Start of buffer */
  char* end;              /* End of buffer */
  int   mode;             /* Text handling mode: STACKMODE_* */
  int   overflow;         /* True if there is an unwitten white-space */
  int   cancelSpace;      /* Ignore leading spaces */
  int   cancelSpace_bu;   /* Backup of cancelSpace. Used when STACKMODE_ATTRIBUTE. */
  int   downcase;         /* Downcase all character data written by prsr_writeData */
  int   error;            /* Error code set by stack functions */

} we_prsr_stack_t;


struct we_prsr_parser_lite_st {
  we_prsr_parserData_t     p;
  we_tok_data_t            data;           /* Tokenizer data */
  we_prsr_stack_t          stack;          /* Work space */
  we_prsr_processor_t      processor;      /* Parser or decoder used at the moment */
  we_prsr_processor_t      returnProc;     /* Parser to return to after a comment is parsed */
  we_prsr_piHandler_t*     piHandler;      /* Called for all processing instructions */

  we_prsr_init_t*          initFunct;      /* Initializer */

  char*                     buffer;         /* Input buffer */
  char*                     bufferStart;
  char*                     bufferEnd;
  long                      bufferSize;

  int                       handle;         /* Handle to the input pipe or file */
  int                       inputType;      /* Pipe, file or buffer */

  we_charset_convert_t*    cvtFunct;       /* Charset convertion function */
  char*                     cvtBuf;         /* Raw data buffer */
  char*                     cvtBufStart;
  long                      cvtBufLen;      /* Length of data */
  long                      cvtbufSize;     /* Size of buffer */

  int                       tagLevel;       /* Current element nesting depth */
  int                       state;          /* Prologue status */
  int                       charset;        /* Character set */
  int                       defaultCharset; /* Default character set */
  int                       numElems;       /* Current max nesting depth (HTML only) */
  int                       stackmode;      /* Stackmode telling how to handle PCDATA */
  int                       type;           /* Tell if data is a comment, CDATA or plaintext */
  int                       noMoreData;     /* TRUE if no more data is to be read */
  int                       mode;           /* Parse HTML instead of XML */
  int                       html_based;     /* XHTML or HTML */
  int                       meta_scan;      /* Scan the head section for meta charset */
  long                      meta_scan_offset;/*Where in buffer to restart during meta scan */
  int                       initalSize;     /* Initial buffer size */
  WE_UINT8                 modId;          /* Id of the calling module */

  /* WBS specific */
  const unsigned char*      htmlTable;      /* HTML specific element table */
  we_prsr_changeCharset_t* changeCharset;  /* Function to be called after meta charset */
};

/* Extended version of prsr_Parser_lite_st. Used together with the WBXML decoder. */
struct we_prsr_parser_st {
  we_prsr_parserData_t     p;
  we_tok_data_t            data;           /* Tokenizer data */
  we_prsr_stack_t          stack;          /* Work space */
  we_prsr_processor_t      processor;      /* Parser or decoder used at the moment */
  we_prsr_processor_t      returnProc;     /* Parser to return to after a comment is parsed */
  we_prsr_piHandler_t*     piHandler;      /* Called for all processing instructions */

  we_prsr_init_t*          initFunct;      /* Initializer */

  char*                     buffer;         /* Input buffer */
  char*                     bufferStart;
  char*                     bufferEnd;
  long                      bufferSize;

  int                       handle;         /* Handle to the input pipe or file */
  int                       inputType;      /* Pipe, file or buffer */

  we_charset_convert_t*    cvtFunct;       /* Charset convertion function */
  char*                     cvtBuf;         /* Raw data buffer */
  char*                     cvtBufStart;
  long                      cvtBufLen;      /* Length of data */
  long                      cvtbufSize;     /* Size of buffer */

  int                       tagLevel;       /* Current element nesting depth */
  int                       state;          /* Prologue status */
  int                       charset;        /* Character set */
  int                       defaultCharset; /* Default character set */
  int                       numElems;       /* Current max nesting depth (HTML only) */
  int                       stackmode;      /* Stackmode telling how to handle PCDATA */
  int                       type;           /* Tell if data is a comment, CDATA or plaintext */
  int                       noMoreData;     /* TRUE if no more data is to be read */
  int                       mode;           /* Parse HTML instead of XML */
  int                       html_based;     /* XHTML or HTML */
  int                       meta_scan;      /* Scan the head section for meta charset */
  long                      meta_scan_offset;/*Where in buffer to restart during meta scan */
  int                       initalSize;     /* Initial buffer size */
  WE_UINT8                 modId;          /* Id of the calling module */

  /* WBS specific */
  const unsigned char*      htmlTable;      /* HTML specific element table */
  we_prsr_changeCharset_t* changeCharset;  /* Function to be called after meta charset */

  /* WBXML decoder specific */
  we_prsr_decoderData_t    d;
  char*                     stringTable;    /* String table. Size limited to MAX_STRING_TABLE */
  int                       stringTableLen;
    
  int                       opaqueLen;      /* Nbr of bytes left in a half-read opaque block */ 
  we_charset_convert_t*    codecvtFunct;   /* Convert function, document encoding to UTF-8 */
  WE_UINT16                elemPage;       /* Element codepage */
  WE_UINT16                attrPage;       /* Attribute codepage */  
  strchr_t                  strchr_funct;   /* strchr for the current encoding */
};


/**************************************************************
 * Functions                                                  *
 **************************************************************/

int
we_prsr_expandStack (we_prsr_parser_t* parser, int len);

int
we_prsr_flushText (we_prsr_parser_t* parser, int final);

int
we_prsr_writeString (we_prsr_parser_t* parser, const char* s, int len);

void
we_prsr_writeData (we_prsr_parser_t* parser, const char* s, int len, int flush);

we_prsr_attr_value_t
we_prsr_normalizeAttribute (we_prsr_parser_t* parser,
                             int                type,
                             const char*        name, 
                             int                nameLen,
                             const char*        data, 
                             int                dataLen, 
                             int               *flag, 
                             int               *error);

int
we_prsr_guess_encoding (const char* s, int *length, int type);


#endif
