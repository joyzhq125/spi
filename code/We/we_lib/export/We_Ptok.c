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
 * We_Ptok.c
 *
 * XML and HTML parser: tokenizer
 */

#include "We_Ptok.h"
#include "We_Ptxt.h"
#include "We_Chrt.h"
#include "We_Lib.h"

/**************************************************************
 * Constants                                                  *
 **************************************************************/

const unsigned char we_tok_charTab[] = {
/* 0x00 */ WE_TOK_CHAR_NONXML, WE_TOK_CHAR_NONXML, WE_TOK_CHAR_NONXML, WE_TOK_CHAR_NONXML,
/* 0x04 */ WE_TOK_CHAR_NONXML, WE_TOK_CHAR_NONXML, WE_TOK_CHAR_NONXML, WE_TOK_CHAR_NONXML,
/* 0x08 */ WE_TOK_CHAR_NONXML, WE_TOK_CHAR_S,      WE_TOK_CHAR_S,      WE_TOK_CHAR_S,
/* 0x0C */ WE_TOK_CHAR_NONXML, WE_TOK_CHAR_S,      WE_TOK_CHAR_NONXML, WE_TOK_CHAR_NONXML,
/* 0x10 */ WE_TOK_CHAR_NONXML, WE_TOK_CHAR_NONXML, WE_TOK_CHAR_NONXML, WE_TOK_CHAR_NONXML,
/* 0x14 */ WE_TOK_CHAR_NONXML, WE_TOK_CHAR_NONXML, WE_TOK_CHAR_NONXML, WE_TOK_CHAR_NONXML,
/* 0x18 */ WE_TOK_CHAR_NONXML, WE_TOK_CHAR_NONXML, WE_TOK_CHAR_NONXML, WE_TOK_CHAR_NONXML,
/* 0x1C */ WE_TOK_CHAR_NONXML, WE_TOK_CHAR_NONXML, WE_TOK_CHAR_NONXML, WE_TOK_CHAR_NONXML,
/* 0x20 */ WE_TOK_CHAR_S,      WE_TOK_CHAR_EXCL,   WE_TOK_CHAR_QUOT,   WE_TOK_CHAR_NUM,
/* 0x24 */ WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_AMP,    WE_TOK_CHAR_APOS,
/* 0x28 */ WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT,
/* 0x2C */ WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_MINUS,  WE_TOK_CHAR_DOT,    WE_TOK_CHAR_SOL,
/* 0x30 */ WE_TOK_CHAR_DIGIT,  WE_TOK_CHAR_DIGIT,  WE_TOK_CHAR_DIGIT,  WE_TOK_CHAR_DIGIT,
/* 0x34 */ WE_TOK_CHAR_DIGIT,  WE_TOK_CHAR_DIGIT,  WE_TOK_CHAR_DIGIT,  WE_TOK_CHAR_DIGIT,
/* 0x38 */ WE_TOK_CHAR_DIGIT,  WE_TOK_CHAR_DIGIT,  WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_SEMI,
/* 0x3C */ WE_TOK_CHAR_LT,     WE_TOK_CHAR_EQUALS, WE_TOK_CHAR_GT,     WE_TOK_CHAR_QUEST,

/* 0x40 */ WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_HEX,    WE_TOK_CHAR_HEX,    WE_TOK_CHAR_HEX,
/* 0x44 */ WE_TOK_CHAR_HEX,    WE_TOK_CHAR_HEX,    WE_TOK_CHAR_HEX,    WE_TOK_CHAR_NMSTRT,
/* 0x48 */ WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT,
/* 0x4C */ WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT,
/* 0x50 */ WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT,
/* 0x54 */ WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT,
/* 0x58 */ WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_LSQB,
/* 0x5C */ WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_RSQB,   WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT,
/* 0x60 */ WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_HEX,    WE_TOK_CHAR_HEX,    WE_TOK_CHAR_HEX,
/* 0x64 */ WE_TOK_CHAR_HEX,    WE_TOK_CHAR_HEX,    WE_TOK_CHAR_HEX,    WE_TOK_CHAR_NMSTRT,
/* 0x68 */ WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT,
/* 0x6C */ WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT,
/* 0x70 */ WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT,
/* 0x74 */ WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT,
/* 0x78 */ WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT,
/* 0x7C */ WE_TOK_CHAR_VERBAR, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT, WE_TOK_CHAR_NMSTRT
};


/**************************************************************
 * Functions                                                  *
 **************************************************************/

/*
 * Calculate the value of a well formed character entitiy. If
 * it has a non-Unicode value, then 0xFFFF is returned. This is
 * not a valid Unicode character. 's' points to the first
 * character of a string formated as "&#123" or "&#x123".
 */
WE_UINT32
we_tok_charRefNumber (const char* s)
{
  long ch;

  s += 2; /* Skip "&#" */
  if (*s == 'x')
    ch = we_cmmn_hex2long (s + 1);
  else 
    ch = atol (s);

  if (ch >= 0x110000 || (ch <= 127 && we_tok_charTab[ch] == WE_TOK_CHAR_NONXML) ||
      (ch >= 0xD800 && ch < 0xE000) || ch == 0xFFFF || ch == 0xFFFE)
    return 0xFFFF;

  return ch;
}



/*
 * Check if the PI target is valid. Target "xml" generates the
 * token WE_TOK_XML_DECL and all other valid targets generate
 * WE_TOK_PI. Invalid targets are "XML", "Xml" and so on and
 * generate WE_TOK_INVALID.
 */
static int
we_tok_checkXmlPi (const char* s, const char* end)
{
  int upper = 0;

  if (end - s != 3)
    return WE_TOK_PI;
  switch (*s) {
  case 'x':
    break;
  case 'X':
    upper = 1;
    break;
  default:
    return WE_TOK_PI;
  }
  s++;
  switch (*s) {
  case 'm':
    break;
  case 'M':
    upper = 1;
    break;
  default:
    return WE_TOK_PI;
  }
  s++;
  switch (*s) {
  case 'l':
    break;
  case 'L':
    upper = 1;
    break;
  default:
    return WE_TOK_PI;
  }

#ifndef WE_CFG_EXTENDED_WML_PARSING
  if (upper)
    return WE_TOK_INVALID;
#endif /* WE_CFG_EXTENDED_WML_PARSING */

  return WE_TOK_XML_DECL;
}



/*
 * Skip white space.
 */
int
we_tok_skipWhiteSpace (const char* *s, const char* end)
{
  register const char* p;

  p = *s;
  while (p < end) {
    if (! ct_isxmlwhitespace (*p)) {
      *s = p;
      return WE_TOK_OK;
    }
    p++;
  }
  *s = p;
  return WE_TOK_MORE;
}



/*
 * Scan through a name. At return, '*s' points to the first
 * non-valid name char.
 */
static int
we_tok_skipName (const char* *s, const char* end)
{
  int tok;
  
  if (*s == end)
    return WE_TOK_MORE;
  tok = CHAR_TOK (*s);
  if (tok != WE_TOK_CHAR_HEX && tok != WE_TOK_CHAR_NMSTRT)
    return WE_TOK_INVALID;

  while (++(*s) != end) {
    switch (CHAR_TOK (*s)) {
    case WE_TOK_CHAR_NMSTRT: 
    case WE_TOK_CHAR_HEX: 
    case WE_TOK_CHAR_DIGIT: 
    case WE_TOK_CHAR_DOT: 
    case WE_TOK_CHAR_MINUS: 
      break; 
    default:
      return WE_TOK_OK;
    }
  }
  return WE_TOK_MORE;
}



/*
 * Scan and skip a tag. Stop at first '>'. Quoted strings are
 * recognized.
 * 
 * <!data "string">
 *  <data "string">
 *   |             |
 */
static int
we_tok_skipTag (const char* s, const char* end, const char* *nextPtr, int *strict)
{
  int tok;
  int dummy;

  do {
    tok = we_tok_tagTok (s, end, nextPtr, &dummy, WE_TOK_STATE_ATTR_NAME, FALSE, strict);
    s = *nextPtr;
    if (tok == WE_TOK_MORE)
      return WE_TOK_MORE;
  } while (tok != WE_TOK_GT);

  return WE_TOK_IGNORE;
}



/*
 * Match 'matchStr' against '*str'. 'matchStr' is NUL terminated
 * and '*str' ends before 'end'.
 *
 * Returns
 * - WE_TOK_MORE if out of data.
 * - WE_TOK_OK if match.
 * - WE_TOK_INVALID if no match.
 */
static int
we_tok_matchElementString (const char* *str, const char* matchStr, const char* end)
{
  const char* s = *str + 1;

  while (s != end && we_ascii_lc[(unsigned char) *s] == *matchStr) {
    s++;
    matchStr++;
  }

  if (s == end)
    return WE_TOK_MORE;
  else if (*matchStr == '\0' && !ct_isalpha (*s) && *s != '_')
    return WE_TOK_OK;
  else
    return WE_TOK_INVALID;
}



/******************  Scan functions  *******************/

/*
 * 's' points to character following "&#x"
 *
 * &#xnum;
 * &#xhexX  X is not a valid hex character (HTML only)
 *    |
 *
 * Returns WE_TOK_NONE_MORE, WE_TOK_CHAR_DATA, WE_TOK_CHAR_REF.
 */
static int
we_tok_scanHexCharRef (const char*  s,
                        const char*  end,
                        const char* *nextPtr,
                        int         *strict)
{
  if (s != end) {
    switch (CHAR_TOK (s)) {
    case WE_TOK_CHAR_DIGIT:
    case WE_TOK_CHAR_HEX:
      break;
    
    default:
      *strict = FALSE;
      *nextPtr = s;
      return WE_TOK_CHAR_DATA;
    }
    
    for (s++; s != end; s++) {
      switch (CHAR_TOK (s)) {
      case WE_TOK_CHAR_DIGIT:
      case WE_TOK_CHAR_HEX:
        break;
    
      case WE_TOK_CHAR_SEMI:
        *nextPtr = s + 1;
        return WE_TOK_CHAR_REF;
      
      default:
        *strict = FALSE;
        *nextPtr = s;
        return WE_TOK_CHAR_REF;
      }
    }
  }
  return WE_TOK_MORE;
}



/*
 * 's' points to character following "&#"
 *
 * &#num;
 * &#xnum;
 * &#numX   X is not a valid numeric character (HTML only)
 * &#xhexX  X is not a valid hex character (HTML only)
 *   |
 *
 * Returns WE_TOK_NONE_MORE, WE_TOK_CHAR_DATA, WE_TOK_CHAR_REF.
 */
static int
we_tok_scanCharRef (const char*  s,
                     const char*  end,
                     const char* *nextPtr,
                     int         *strict)
{
  if (s != end) {
    if (*s == 'x')
      return we_tok_scanHexCharRef (s + 1, end, nextPtr, strict);
    
    switch (CHAR_TOK (s)) {
    case WE_TOK_CHAR_DIGIT:
      break;
    
    default:
      *strict = FALSE;
      *nextPtr = s;
      return WE_TOK_CHAR_DATA;
    }
    
    for (s++; s != end; s++) {
      switch (CHAR_TOK (s)) {
      case WE_TOK_CHAR_DIGIT:
        break;
    
      case WE_TOK_CHAR_SEMI:
        *nextPtr = s + 1;
        return WE_TOK_CHAR_REF;
      
      default:
        *strict = FALSE;
        *nextPtr = s;
        return WE_TOK_CHAR_REF;
      }
    }
  }
  return WE_TOK_MORE;
}



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
                 int         *strict)
{
  if (s == end)
    return WE_TOK_MORE;
  
  switch (CHAR_TOK (s)) {
  case WE_TOK_CHAR_NMSTRT:
  case WE_TOK_CHAR_HEX:
    s++;
    break;
  
  case WE_TOK_CHAR_NUM:
    return we_tok_scanCharRef (s + 1, end, nextPtr, strict);
  
  default:
    *strict = FALSE;
    *nextPtr = s;
    return WE_TOK_CHAR_DATA;
  }
  
  while (s != end) {
    switch (CHAR_TOK (s)) {
    case WE_TOK_CHAR_NMSTRT:
    case WE_TOK_CHAR_HEX:
    case WE_TOK_CHAR_DIGIT:
    case WE_TOK_CHAR_DOT:
    case WE_TOK_CHAR_MINUS:
      s++;
      break;
    
    case WE_TOK_CHAR_SEMI:
      *nextPtr = s + 1;
      return WE_TOK_ENTITY_REF;
    
    default:
      *strict = FALSE;
      *nextPtr = s;
      return WE_TOK_ENTITY_REF;
    }
  }
  return WE_TOK_MORE;
}



/*
 * Scan a PI. Sets data->str, marking the end of the taget.
 *
 * 's' points to character following "<?"
 *
 * <?target data?>
 *   |     | 
 *   s     data->str
 */

static int
we_tok_scanPi (const char* s, const char* end, const char* *nextPtr, we_tok_data_t* data)
{
  int tok;
  const char* target;

  target = s;
  if (s == end)
    return WE_TOK_MORE;

  switch (CHAR_TOK (s)) {
  case WE_TOK_CHAR_NMSTRT:
  case WE_TOK_CHAR_HEX:
    s++;
    break;

  default:
    return WE_TOK_INVALID;
  }

  while (s != end) {
    switch (CHAR_TOK (s)) {
    case WE_TOK_CHAR_NMSTRT:
    case WE_TOK_CHAR_HEX:
    case WE_TOK_CHAR_DIGIT:
    case WE_TOK_CHAR_DOT:
    case WE_TOK_CHAR_MINUS:
      s++;
      break;

    case WE_TOK_CHAR_S:      
      if ((tok = we_tok_checkXmlPi (target, s)) == WE_TOK_INVALID)
        return WE_TOK_INVALID;

      data->str = s; /* Used in prsr_prologueParser and prsr_contentParser */
      if (we_tok_skipWhiteSpace (&s, end) != WE_TOK_OK)
        return WE_TOK_MORE;

      while (s != end) {
        switch (CHAR_TOK (s)) {
        case WE_TOK_CHAR_NONXML:
          return WE_TOK_INVALID;

        case WE_TOK_CHAR_QUEST:
          s++;
          if (s == end)
            return WE_TOK_MORE;
          if (*s == '>') {
            *nextPtr = s + 1;
            return tok; /* TOK_PI or TOK_XML_DECL */
          }
          break;
          
        default:
          s++;
          break;
        }
      }

      return WE_TOK_MORE;

    case WE_TOK_CHAR_QUEST:
      data->str = s; /* Used in prsr_prologueParser and prsr_contentParser */

      if ((tok = we_tok_checkXmlPi (target, s)) == WE_TOK_INVALID)
        return WE_TOK_INVALID;

      s++;
      if (s == end)
        return WE_TOK_MORE;
      if (*s == '>') {
        *nextPtr = s + 1;
        return tok; /* TOK_PI or TOK_XML_DECL */
      }
      /* fall through */

    default:
      return WE_TOK_INVALID;
    }
  }
  return WE_TOK_MORE;
}



/*
 * Scan tag.
 *
 * 's' points to character following "<".
 *
 * <name
 * </name
 * <?target data?>
 * <!-- comment -->
 * <![CDATA[ text ]]>
 *  | 
 */
static int 
we_tok_scanLt (const char*      s,
                const char*      end,
                const char*     *nextPtr,
                we_tok_data_t*  data)
{
  int tok;

  if (s == end)
    return WE_TOK_MORE;
  
  switch (CHAR_TOK (s)) {
  case WE_TOK_CHAR_NMSTRT:
  case WE_TOK_CHAR_HEX:
    *nextPtr = s;
    return WE_TOK_START_TAG;
  
  case WE_TOK_CHAR_EXCL:
    if (++s == end)
      return WE_TOK_MORE;
    switch (CHAR_TOK (s)) {
    case WE_TOK_CHAR_MINUS:
      s++;
      if (s == end)
        return WE_TOK_MORE;
      if (*s == '-') {
        *nextPtr = s;
        return WE_TOK_COMMENT_OPEN;
      }
      break;

    case WE_TOK_CHAR_LSQB:
      if (end < s + 6)
        return WE_TOK_MORE;
      s++;
      if (! strncmp (s, "CDATA[", 6)) {
        *nextPtr = s + 6;
        return WE_TOK_CDATA_SECT_OPEN;
      }
      break;
    }

    data->strict = FALSE;
    return we_tok_skipTag (s, end, nextPtr, &data->strict);
  
  case WE_TOK_CHAR_QUEST:
    return we_tok_scanPi (s + 1, end, nextPtr, data);
  
  case WE_TOK_CHAR_SOL:
    if (++s == end)
      return WE_TOK_MORE;
    tok = CHAR_TOK (s);
    if (tok == WE_TOK_CHAR_NMSTRT || tok == WE_TOK_CHAR_HEX)
      return WE_TOK_END_TAG;
    /* fall through */
  
  default:
    *nextPtr = s;
    data->strict = FALSE;
    return WE_TOK_CHAR_DATA;
  }
}



/*
 * Scan a plain string.
 *
 * 's' points to opening character.
 *
 * "string"
 * 'string' 
 * |
 */
static int
we_tok_scanLiteral (const char* s, const char* end, const char* *nextPtr)
{
  char open;
  int tok;

  open = *s;
  while (++s != end) {
    tok = CHAR_TOK (s);
    switch (tok) {
    case WE_TOK_CHAR_NONXML:
      return WE_TOK_INVALID;
    
    case WE_TOK_CHAR_QUOT:
    case WE_TOK_CHAR_APOS:
      if (*s != open)
        break;
      s++;
      if (s == end)
        return WE_TOK_MORE;
      switch (CHAR_TOK (s)) {
      case WE_TOK_CHAR_S:
      case WE_TOK_CHAR_GT:
      case WE_TOK_CHAR_LSQB:
        *nextPtr = s;
        return WE_TOK_OK;
      default:
        return WE_TOK_INVALID;
      }
     
    default:
      break;
    }
  }
  return WE_TOK_MORE;
}



/*
 * Scan doctype declaration. Must be public and with no internal
 * subset. The URL is not used, nor is its syntax checked.
 *
 * data->str is set to the first character of the wid and data->strLen
 * to its length.
 *
 * <!DOCTYPE rootElement PUBLIC "wid" "url">
 *   |
 *   s
 */
static int
we_tok_scanDoctype (const char* s, const char* end, const char* *nextPtr, we_tok_data_t* data)
{
  enum {Verify_string, End_doctype, Root_part, Id_part, URL_part};
  static const char  v[] = {Verify_string /*DOCTYPE*/, Root_part, Verify_string /*PUBLIC*/,
                            Id_part, URL_part, End_doctype};
  static const char* const Doctype = "DOCTYPE";
  static const char* const Public  = "PUBLIC";
  const char* t;
  int         tok;
  int         len;
  int         i;

  t   = Doctype;
  len = 7;
  i   = 0;

  for (;;) {
    switch (v[i++]) {
    case Verify_string:
      if (s + len >= end)
        return WE_TOK_MORE;
      if (we_cmmn_strncmp_nc (s, t, len)) 
        return WE_TOK_INVALID;
      s += len;
      break;

    case Root_part:
      tok = we_tok_skipName (&s, end);
      if (tok != WE_TOK_OK)
        return tok;
      if (s == end)
        return WE_TOK_MORE;
      if (! ct_isxmlwhitespace (*s))
        return WE_TOK_INVALID;
      
      t = Public; /* Next string to verify */
      len = 6;
      break;

    case Id_part:
      tok = we_tok_scanLiteral (s, end, nextPtr); /* Does not check syntax */
      if (tok != WE_TOK_OK)
        return tok;
      data->str = s + 1; /* Used in prsr_prologueParser */
      data->strLen = *nextPtr - s - 2; /* Used in prsr_prologueParser */
      s = *nextPtr;
      break;

    case URL_part:
      if (*s == '>') 
        continue;
      tok = we_tok_scanLiteral (s, end, nextPtr); /* Does not check URL syntax */
      if (tok != WE_TOK_OK)
        return tok;
      s = *nextPtr;
      break;

    case End_doctype:
      if (*s == '>') {
        *nextPtr = s + 1;
        return WE_TOK_DOCTYPE;
      } else
        return WE_TOK_INVALID;
    }

    /* Skip white space */
    tok = we_tok_skipWhiteSpace (&s, end);
    if (tok != WE_TOK_OK)
      return tok;
  }
  /* return WE_TOK_OK;  Not reached */
}



/*
 * 's' points to character following "<!". Only comments and
 * DOCTYPE without internal subset are handled.
 */
static int
we_tok_scanDecl (const char* s, const char* end, const char* *nextPtr, we_tok_data_t* data)
{
  if (s == end)
    return WE_TOK_MORE;
  
  if (*s == '-') {
    if (++s == end)
      return WE_TOK_MORE;
    if (*s == '-')
      return WE_TOK_COMMENT_OPEN;

  } else {
    int tok;
    tok = we_tok_scanDoctype (s, end, nextPtr, data);
    if (tok != WE_TOK_INVALID)
      return tok;
  }

  /* This is neither a comment nor a valid XML DOCTYPE declaration */
  data->strict = FALSE;
  return we_tok_skipTag (s, end, nextPtr, &data->strict);
}



/*
 * Scan a plain string.
 *
 * 's' points to opening character.
 *
 * "string"
 * 'string' 
 * |
 */
static int
we_tok_scanQstring (const char*  s,
                     const char*  end,
                     const char* *nextPtr,
                     int         *nbrOfRefs,
                     int         *strict)
{
  int tok;
  int open;

  *nbrOfRefs = 0;
  open = *s++;
  while (s != end) {
    if (*s == open) {
      *nextPtr = s + 1;
      return WE_TOK_QSTRING;
    }
    
    tok = CHAR_TOK (s);
    switch (tok) {
    case WE_TOK_CHAR_AMP:
      tok = we_tok_scanRef (s + 1, end, &s, strict);
      (*nbrOfRefs)++;
      if (tok == WE_TOK_MORE)
        return WE_TOK_MORE;
      break;
    
    case WE_TOK_CHAR_LT:
    case WE_TOK_CHAR_NONXML:
      *strict = FALSE;
      /* fall through */

    default:
      s++;
      break;
    }
  }
  return WE_TOK_MORE;
}



static int
we_tok_scanString (const char*  s,
                    const char*  end,
                    const char* *nextPtr,
                    int         *nbrOfRefs,
                    int          state,
                    int         *strict)
{
  int returnToken;
  int tok;
  int type;

  if (s == end)
    return WE_TOK_MORE;

  type = CHAR_TOK (s);
  s++;
  if (type == WE_TOK_CHAR_HEX || type == WE_TOK_CHAR_NMSTRT)
    returnToken = WE_TOK_NAME;
  else
    returnToken = WE_TOK_STRING;

  while (s != end) {
    switch (CHAR_TOK (s)) {
    case WE_TOK_CHAR_NMSTRT: 
    case WE_TOK_CHAR_HEX: 
    case WE_TOK_CHAR_DIGIT: 
    case WE_TOK_CHAR_DOT: 
    case WE_TOK_CHAR_MINUS: 
      break;
      
    case WE_TOK_CHAR_S:
    case WE_TOK_CHAR_GT:
      *nextPtr = s;
      return returnToken;

    case WE_TOK_CHAR_EQUALS:
      if (state != WE_TOK_STATE_ATTR_VAL) {
        *nextPtr = s;
        return returnToken;
      }
      /* fall through */

    case WE_TOK_CHAR_LT:
    case WE_TOK_CHAR_QUOT:
    case WE_TOK_CHAR_APOS:
    case WE_TOK_CHAR_NONXML:
      *strict  = FALSE;
      returnToken = WE_TOK_STRING;
      break;

    case WE_TOK_CHAR_AMP:
      tok = we_tok_scanRef (s + 1, end, &s, strict);
      (*nbrOfRefs)++;
      if (tok == WE_TOK_MORE)
        return WE_TOK_MORE;
      break;

    case WE_TOK_CHAR_SOL:
      if (state != WE_TOK_STATE_ATTR_VAL) {
        *nextPtr = s;
        return returnToken;
      }
      /* fall through */

    default:
      returnToken = WE_TOK_STRING;
    }
    s++;
  }
  return WE_TOK_MORE;
}



/******************  Tokenizer main functions  *******************/

/*
 * Main scan function for the prologue.
 */
int
we_tok_prologueTok (const char* s, const char* end, const char* *nextPtr, we_tok_data_t* data)
{
  data->strict = TRUE;

  if (s == end)
    return WE_TOK_MORE;

  *nextPtr = s;
  
  switch (CHAR_TOK (s)) {
  case WE_TOK_CHAR_LT:
    s++;
    if (s == end)
      return WE_TOK_MORE;
    switch (CHAR_TOK (s)) {
    case WE_TOK_CHAR_EXCL:
      return we_tok_scanDecl (s + 1, end, nextPtr, data);
    case WE_TOK_CHAR_QUEST:
      return we_tok_scanPi (s + 1, end, nextPtr, data);
    case WE_TOK_CHAR_NMSTRT:
    case WE_TOK_CHAR_HEX:
      *nextPtr = s - 1;
      return WE_TOK_INSTANCE_START;
    }
    return WE_TOK_INVALID;

  case WE_TOK_CHAR_S:
    return we_tok_skipWhiteSpace (nextPtr, end);

  case WE_TOK_CHAR_NONXML:
    return WE_TOK_INVALID;

  default:
    return WE_TOK_CHAR_DATA;
  }
}



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
 */
int
we_tok_contentTok (const char* s, const char* end, const char* *nextPtr, we_tok_data_t* data)
{
  data->strict = TRUE;

  if (s == end)
    return WE_TOK_MORE;

  switch (CHAR_TOK (s)) {
  case WE_TOK_CHAR_LT:
    return we_tok_scanLt (s + 1, end, nextPtr, data);

  case WE_TOK_CHAR_AMP:
    return we_tok_scanRef (s + 1, end, nextPtr, &data->strict);
  
  case WE_TOK_CHAR_RSQB:
    s++;
    if (s == end)
      return WE_TOK_MORE;
    if (*s != ']')
      break;
    s++;
    if (s == end)
      return WE_TOK_MORE;
    if (*s != '>') {
      s--;
      break;
    }
    return WE_TOK_INVALID;
  
  case WE_TOK_CHAR_NONXML:
    *nextPtr = s + 1;
    return WE_TOK_INVALID_CHAR;
  
  default:
    s++;
    break;
  }
  
  /* Text content */

  while (s != end) {
    switch (CHAR_TOK (s)) {
    case WE_TOK_CHAR_RSQB:
      if (s + 1 != end) {
        if (*(s + 1) != ']') {
          s++;
          break;
        }
        if (s + 2 != end) {
          if (*(s + 2) != '>') {
            s++;
            break;
          }
          return WE_TOK_INVALID;
        }
      }
    /* fall through */
    
    case WE_TOK_CHAR_AMP:
    case WE_TOK_CHAR_LT:
    case WE_TOK_CHAR_NONXML:
      *nextPtr = s;
      return WE_TOK_CHAR_DATA;
    
    default:
      s++;
      break;
    }
  }
  *nextPtr = s;
  return WE_TOK_CHAR_DATA;
}



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
 *     <style> <!-- </style>
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
 * WE_TOK_SCRIPT_END_TAG, WE_TOK_TEXTAREA_END_TAG, TOK_COMMENT_OPEN.
 */
int
we_tok_opaqueDataTok (const char*  s,
                       const char*  end,
                       const char* *nextPtr,
                       int         *strict)
{
  static const char* const Plaintext = "/plaintext"; 
  static const char* const Style     = "/style";
  static const char* const Script    = "/script";
  static const char* const Textarea  = "/textarea";
  const char* matchStr;
  const char* start;
  int         tok;
  int         result;
  int         use_refs;
  
  use_refs = *strict == TRUE;
  *strict  = TRUE;
  start    = s;

  while (s != end) {
    
    if (*s == '-') {
      /* end of comment */
      if (end - s < 2)
        break;
      s++;
      if (*s != '-')
        continue;
      s++;
      if (*s != '>') {
        s--; /* Added to handle "--->" */ 
        *strict = FALSE;
        continue;
      }
      *nextPtr = s - 2;
      return WE_TOK_COMMENT_CLOSE;

    }
    else if (*s == ']') {
      /* CDATA end section */
      if (end - s < 2)
          break;
      s++;
      if (*s != ']')
        continue;
      s++;
      if (*s != '>') {
        s--; /* Added to handle "]]]>" */ 
        continue;
      }
      *nextPtr = s - 2;
      return WE_TOK_CDATA_SECT_CLOSE;

    }
    else if (*s == '<') {
      /* end tag */
      if (end - s < 4) /* '<' followed by shortest sequence "!--" */
        break;
      
      if (s[1] == '!' && s[2] == '-' && s[3] == '-') {
        *nextPtr = s + 4;
        return WE_TOK_COMMENT_OPEN;
      }
      else if (we_ascii_lc[(unsigned char) s[3]] == 't') {
        matchStr = Style;
        tok = WE_TOK_STYLE_END_TAG;
      }
      else if (we_ascii_lc[(unsigned char) s[2]] == 'p') {
        matchStr = Plaintext;
        tok = WE_TOK_PLAINTEXT_END_TAG;
      }
      else if (we_ascii_lc[(unsigned char) s[2]] == 's') {
        matchStr = Script;
        tok = WE_TOK_SCRIPT_END_TAG;
      }
      else {
        matchStr = Textarea;
        tok = WE_TOK_TEXTAREA_END_TAG;
      }
      
      result = we_tok_matchElementString (&s, matchStr, end);
      if (result == WE_TOK_INVALID) {
        s++;
        continue;
      }
      else if (result == WE_TOK_MORE)
        break;
      else {
        *nextPtr = s;
        return tok;
      }

    }
    else if (*s == '&' && use_refs) {
      if (s != start) {
        *nextPtr = s;
        return WE_TOK_CHAR_DATA;
      }
      else {
        return we_tok_scanRef (s + 1, end, nextPtr, strict);
      }
    }
    else {
      s++;
    }
  }

  *nextPtr = s;
  return WE_TOK_MORE;
}



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
                int         *strict)
{
  int tok;

  *strict = TRUE;

  if (s == end)
    return WE_TOK_MORE;
  
  *nbrOfRefs = 0;
  switch (CHAR_TOK (s)) {
  case WE_TOK_CHAR_APOS:
  case WE_TOK_CHAR_QUOT:
    if (state == WE_TOK_STATE_XML || state == WE_TOK_STATE_ATTR_VAL) {
      tok = we_tok_scanQstring (s, end, nextPtr, nbrOfRefs, strict);
      if (! endOfDoc || tok != WE_TOK_MORE)
        return tok;
      /* An unclosed qstring spans over the rest of the document */
    }
    goto Scan_string;

  case WE_TOK_CHAR_EQUALS:
    tok = WE_TOK_EQ;
    break;

  case WE_TOK_CHAR_LT:
    if (state == WE_TOK_STATE_ATTR_NAME) {
      tok = WE_TOK_LT;
      break;
    }
    goto Scan_string;

  case WE_TOK_CHAR_GT:
    tok = WE_TOK_GT;
    break;

  case WE_TOK_CHAR_S:
    for (;;) {
      if (++s == end)
        return WE_TOK_MORE;
      if (CHAR_TOK (s) != WE_TOK_CHAR_S)
        break;
    }
    s--;
    tok = WE_TOK_SPACE;
    break;

  case WE_TOK_CHAR_NONXML:
    tok = WE_TOK_INVALID;
    break;

  case WE_TOK_CHAR_SOL:
    if (state == WE_TOK_STATE_ELEM_NAME ||
        state == WE_TOK_STATE_ATTR_NAME) {
      tok = WE_TOK_SOL;
      break;
    } else if (state == WE_TOK_STATE_XML) {
      if (++s == end)
        return WE_TOK_MORE;
      if (*s == '>') {
        tok = WE_TOK_EMPTY_CLOSE;
        break;
      }
      s--;
      tok = WE_TOK_SOL;
      break;
    }
    goto Scan_string;

  default:
Scan_string:
    return we_tok_scanString (s, end, nextPtr, nbrOfRefs, state, strict);
  }

  *nextPtr = s + 1;
  return tok;
}

