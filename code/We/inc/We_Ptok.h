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
 * We_Ptok.h
 *
 * XML and HTML parser: tokenizer
 */

#ifndef _we_ptok_h
#define _we_ptok_h

#ifndef _we_chrs_h
#include "We_Chrs.h"
#endif


/**************************************************************
 * Constants                                                  *
 **************************************************************/

extern const unsigned char tok_charTab[];

#define WE_TOK_CHAR_NMSTRT   0
#define WE_TOK_CHAR_NONXML   1
#define WE_TOK_CHAR_DIGIT    2  /* 0123456789 */
#define WE_TOK_CHAR_HEX      3  /* abcdefABCDEF */
#define WE_TOK_CHAR_DOT      4  /* . */
#define WE_TOK_CHAR_MINUS    5  /* - */
#define WE_TOK_CHAR_S        6  /* space, tab, cr, lf */
#define WE_TOK_CHAR_GT       7  /* > */
#define WE_TOK_CHAR_SOL      8  /* / */
#define WE_TOK_CHAR_SEMI     9  /* ; */
#define WE_TOK_CHAR_QUEST    10 /* ? */
#define WE_TOK_CHAR_AMP      11 /* & */
#define WE_TOK_CHAR_EQUALS   12 /* = */
#define WE_TOK_CHAR_LT       13 /* < */
#define WE_TOK_CHAR_RSQB     14 /* ] */
#define WE_TOK_CHAR_QUOT     15 /* " */
#define WE_TOK_CHAR_APOS     16 /* ' */
#define WE_TOK_CHAR_EXCL     17 /* ! */
#define WE_TOK_CHAR_NUM      18 /* # */
#define WE_TOK_CHAR_LSQB     19 /* [ */
#define WE_TOK_CHAR_VERBAR   20 /* | */



#define WE_TOK_OK                   0  /* All went well */
#define WE_TOK_MORE                 1  /* Not enough data to scan the next token */
#define WE_TOK_INVALID              2  /* Lexical error */
#define WE_TOK_INSTANCE_START       3  /* The first start tag in the document */
#define WE_TOK_IGNORE               32 /* Lexical error, ignore this token */
#define WE_TOK_INVALID_CHAR         33 /* Non-XML character */ 

#define WE_TOK_START_TAG            7  /* <e>                        */
#define WE_TOK_END_TAG              8  /* </e>                       */
#define WE_TOK_CHAR_DATA            9  /* qwerty                     */
#define WE_TOK_ENTITY_REF           10 /* &ref;                      */
#define WE_TOK_CHAR_REF             11 /* &#32; &#x20;               */
#define WE_TOK_PI                   12 /* <?processing instruction?> */
#define WE_TOK_XML_DECL             13 /* <?xml version="1.0"?>      */
#define WE_TOK_COMMENT_OPEN         14 /* <!--                       */
#define WE_TOK_COMMENT_CLOSE        15 /* -->                        */
#define WE_TOK_CDATA_SECT_OPEN      16 /* <![CDATA[                  */
#define WE_TOK_CDATA_SECT_CLOSE     17 /* ]]>                        */
#define WE_TOK_PLAINTEXT_END_TAG    18 /* </plaintext>               */
#define WE_TOK_STYLE_END_TAG        19 /* </style>                   */
#define WE_TOK_SCRIPT_END_TAG       20 /* </script>                  */
#define WE_TOK_TEXTAREA_END_TAG     21 /* </textarea>                */
#define WE_TOK_DOCTYPE              22 /* <!DOCTYPE ...>             */
/* The order of TOK_NAME up to and including TOK_SOL_NAME must not be changed */
#define WE_TOK_NAME                 23 /* String of name characters  */
#define WE_TOK_SPACE                24 /* Whitespace                 */
#define WE_TOK_EQ                   25 /* Charcter '='               */
#define WE_TOK_LT                   26 /* Charcter '<'               */
#define WE_TOK_GT                   27 /* Charcter '>'               */
#define WE_TOK_EMPTY_CLOSE          28 /* Charcters '/>'             */
#define WE_TOK_QSTRING              29 /* Quoted string              */
#define WE_TOK_STRING               30 /* Unuoted string             */
#define WE_TOK_SOL                  31 /* Charcter '/'               */


/* tok_tagTok states */
#define WE_TOK_STATE_XML            0
#define WE_TOK_STATE_ELEM_NAME      1
#define WE_TOK_STATE_ATTR_NAME      2
#define WE_TOK_STATE_ATTR_VAL       3


/**************************************************************
 * Macro definitions                                          *
 **************************************************************/

#define CHAR_TOK(ch) (*(unsigned char*)(ch) >= 128 ?\
                     WE_TOK_CHAR_NMSTRT : we_tok_charTab[(unsigned char)*(ch)])


/**************************************************************
 * Type definitions                                           *
 **************************************************************/

typedef struct we_tok_data_st {
  const char* str;
  int         strLen;
  int         strict;

} we_tok_data_t;


/**************************************************************
 * Functions                                                  *
 **************************************************************/

/*
 * Main scan function for content.
 *
 * If the function returns WE_TOK_MORE or WE_TOK_INVALID, then
 * *nextPtr is untouched.
 *
 * <name attr="value">
 * </name>
 * text
 * &entity;
 * ]]>
 * | 
 * s
 */
int
we_tok_contentTok (const char*      s, 
                    const char*      end,
                    const char*     *nextTokPtr,
                    we_tok_data_t*  data);



/*
 * Main scan function for the prologue.
 */
int
we_tok_prologueTok (const char*      s,
                     const char*      end,
                     const char*     *nextTokPtr,
                     we_tok_data_t*  data);



/*
 * Main scan function for CDATA sections and the HTML plaintext,
 * style and script elements.
 *
 * Non-XML characters do not cause a syntax error. This is a
 * violation of the XML specification.
 *
 * This function points '*nextPtr' to the first character after
 * the last handled. If the input is exhausted, then
 * '*nextPtr' == 'end', else '*nextPtr' points to the last
 * character known not to be a part of a potential new token.
 *
 * If '*strict' is TRUE when calling this function, references are
 * recognised as well. Is used only by textarea.
 *
 *   <![CDATA[ text ]]>
 *        <!-- text -->
 *     <style> text </style>
 *    <script> text </script>
 * <plaintext> text </plaintext>
 *  <textarea> text </textarea>
 *  <textarea> text &ref;
 *  <textarea>&aref;
 *            |     |
 *            s     at return
 *
 * Returns WE_TOK_NONE_MORE, WE_TOK_CHAR_DATA, WE_TOK_ENTITY_REF,
 * WE_TOK_CHAR_REF, WE_TOK_COMMENT_CLOSE, WE_TOK_CDATA_SECT_CLOSE,
 * WE_TOK_STYLE_END_TAG, WE_TOK_PLAINTEXT_END_TAG,
 * WE_TOK_SCRIPT_END_TAG, WE_TOK_TEXTAREA_END_TAG.
 */
int
we_tok_opaqueDataTok (const char*  s,
                       const char*  end,
                       const char* *nextPtr,
                       int         *strict); 



/*
 * Main scan function for tags.
 *
 * If the function returns TOK_NONE_MORE or TOK_INVALID, then
 * *nextPtr is untouched.
 *
 * <name>
 * <name/>
 * <name attr="value">
 * <name attr='value'>
 * <name attr= value >      (HTML only)
 * </name>
 * | 
 */
int
we_tok_tagTok (const char*  s,
                const char*  end,
                const char* *nextPtr,
                int         *nbrOfRefs,
                int          state,
                int          endOfDoc,
                int         *strict);



/*
 * Calculate the value of a well formed character entitiy. If
 * it has a non-Unicode value, then 0xFFFF is returned. This is
 * not a valid Unicode character. 's' points to the first
 * character of a string formated as "&#123" or "&#x123".
 */
WE_UINT32
we_tok_charRefNumber (const char* s);



/*
 * Parse an entity reference.
 * 's' points to character following "&"
 *
 * &name;
 * &#num;
 * &#xhex;
 * &nameX   X is not a valid name character (HTML only)
 * &#numX   X is not a valid numeric character (HTML only)
 * &#xhexX  X is not a valid hex character (HTML only)
 *  |
 *
 * Returns WE_TOK_NONE_MORE, WE_TOK_CHAR_DATA, WE_TOK_ENTITY_REF,
 * WE_TOK_CHAR_REF.
 */
int
we_tok_scanRef (const char*  s,
                 const char*  end,
                 const char* *nextPtr,
                 int         *strict);



/*
 * Skip white space.
 */
int
we_tok_skipWhiteSpace (const char* *s,
                        const char*  end);

#endif
