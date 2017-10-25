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
 * Wml_Inpf.c
 *
 * Created by Henrik Olsson. 030603
 *
 * Revision history:
 *   030603, Created
 *   040225, Clarified preconditions for wml_input_make_valid concerning 
 *   format of 'inStr'.
 */
/************************************************************
 * This file is used for:
 * + validating wml input using a format string
 ************************************************************/

#include <string.h>
#include <stdio.h>

#include "Wml_Inpf.h"

#include "We_Cmmn.h"
#include "We_Chrt.h"

/*
 * Returns TRUE if the character pointed to by 'ch' 
 * conforms to the 'format_code'.
 * Returns FALSE otherwise and replaces ch with
 * a character that confirms to the format
 */

static int
is_valid_ch_for_current_lang (char *ch, int charlen)
{
  return (charlen > 1) || ct_isascii (*ch);
}

static int
wml_make_char_valid (char *ch,
                     int   charlen,
                     char  format_code,
                     int   isConstant,
                     int  *out_charlen)
{
  int isOk = TRUE;

  *out_charlen = charlen;

  if (isConstant)
  {
    if (*ch != format_code)
    {
      *ch = format_code;
      *out_charlen = 1;
      isOk = FALSE;
    }
  }
  else
  {

    switch (format_code)
    {
      case 'A':
      {
        if (!*ch || !is_valid_ch_for_current_lang (ch, charlen) || ct_isdigit (*ch) || ct_islower (*ch))
        {
          char newChar[] = "A";
          if (ct_islower (*ch))
          {
            newChar[0] = (char)(*ch - ('a' - 'A'));
          }
          *ch = newChar[0];
          *out_charlen = 1;
          isOk = FALSE;
        }
        break;
      }

      case 'a':
      {
        if (!*ch || !is_valid_ch_for_current_lang (ch, charlen) || ct_isdigit (*ch) || ct_isupper (*ch))
        {
          char newChar[] = "a";

          if (ct_isupper (*ch))
          {
            newChar[0] = (char)(*ch + ('a' - 'A'));
          }
          *ch = newChar[0];
          *out_charlen = 1;
          isOk = FALSE;
        }
        break;
      }

      case 'N':
      {
        if (!*ch ||  !is_valid_ch_for_current_lang (ch, charlen) || !ct_isdigit (*ch))
        {
          *ch = '0';
          *out_charlen = 1;
          isOk = FALSE;
        }
        break;
      }

      case 'n':
      {
        if (!*ch || !is_valid_ch_for_current_lang (ch, charlen) || ct_isupper (*ch) || ct_islower (*ch))
        {
          *ch = '0';
          *out_charlen = 1;
          isOk = FALSE;
        }
        break;
      }

      case 'X':
      {
        if (!*ch || !is_valid_ch_for_current_lang (ch, charlen) || ct_islower (*ch))
        {
          char newChar[] = "A";

          if (ct_islower (*ch))
          {
            newChar[0] = (char)(*ch - ('a' - 'A'));
          }
          *ch = newChar[0];
          *out_charlen = 1;
          isOk = FALSE;
        }
        break;
      }

      case 'x':
      {
        if (!*ch || !is_valid_ch_for_current_lang (ch, charlen) || ct_isupper (*ch))
        {
          char newChar[] = "a";

          if (ct_isupper (*ch))
          {
            newChar[0] = (char)(*ch + ('a' - 'A'));
          }
          *ch = newChar[0];
          *out_charlen = 1;
          isOk = FALSE;
        }
        break;
      }

      case 'M':
      case 'm':
      default:
      {
        if (!*ch || !is_valid_ch_for_current_lang (ch, charlen))
        {
          *ch = 'a';
          *out_charlen = 1;
          isOk = FALSE;
        }
        break;
      }
    } 
  }

  return isOk;
}

/* 
 * formats 'inStr' according to a valid 'format' string and places the
 * result in 'outStr'.
 *
 * Returns: TRUE if 'outStr' was properly generated according to format.
 * The only reason for failure is if the formatted string doesn't fit into
 * 'outStr'
 *
 * 'inStr' must be valid according to UTF8.
 *
 * 'startSel','endSel': defines a selection in inStr that this function should 
 * delete if possible. If 'startSel' equals 'endSel' it only says where 'newChars'
 * should be inserted. 'startSel' must be <= 'endSel'
 *
 * 'newChars' are characters to be inserted in 'outStr' at startSel position.
 * if 'newChars is NULL no chars are inserted.
 *
 * 'format' is a format string formatted according to WAP-WML-INPUT-FORMAT.
 *
 * 'inputRequired' should be set according to if -wap-input-required has been set.
 *  (1=true 0=false -1=<not set>
 *
 * 'outStr' is the resulting string. 'outStr' must not point to the same data as 'inStr'. 
 * If outStr is NULL
 * no data is written to 'outStr' but 'outStrSize' is still calculated. If the formatted
 * string doesn't fit into 'outStr' the str is generated 'outStrSize' bytes (ie 
 * not nullterminated) and the required size is returned in 'outStrSize'.
 *
 * 'outStrSize' is an in/out parameter. At entry of the function it states 
 * the size of the 'outStr'.
 * At exit of the function it states the required size of the  'outStr'. This
 * might be used to calculate how much to allocate for the formatted string. 

*/
int 
wml_input_make_valid (const char  *inStr,
                     int           startSel,
                     int           endSel,
                     const char   *newChars,
                     const char   *formatStr,
                     int           inputRequired,
                     char         *outStr,
                     int          *outStrSize)
{
  int formatStrPos = 0;
  int isMultiple = FALSE;
  int inStrPos = 0;
  int outStrPos = 0;
  int newCharsPos = 0;
  int isLanguageSpecific = FALSE;
  enum States {
    FORMAT_PARSE, 
    FORMAT_APPLY,
    FINISHED 
  };
  int multipleLimit = -1;
  char currFormat = 0;

  int inStrLen = inStr ? strlen (inStr) : 0;
  int newCharsLen = newChars ? strlen (newChars) : 0;

  int state = FORMAT_PARSE;
  int isConstant = FALSE;
  int isOk = TRUE;
  const char* pFormatStr = "*M";
  if (formatStr && strlen (formatStr))
  {
    pFormatStr = formatStr;
  }


  if (!inputRequired && (!newChars || !newChars[0]) && (endSel - startSel) == inStrLen)
  {
    state = FINISHED;
  }

  while (state != FINISHED)
  {
    switch (state)
    {
      case FORMAT_PARSE:
      {
        if (pFormatStr[formatStrPos] && multipleLimit)
        {
          if (isLanguageSpecific)
          {
            if (pFormatStr[formatStrPos] == '>')
            {
              isLanguageSpecific = FALSE;
              currFormat = 'M';
              state = FORMAT_APPLY;
              if (!isMultiple)
              {
                formatStrPos++;
              }
            }
            else
            {
              formatStrPos++;
            }
          }
          else if (isConstant)
          {
            currFormat = pFormatStr[formatStrPos];
            if (!isMultiple)
            {
              formatStrPos++;
            }
            state = FORMAT_APPLY;
          }
          else
          {
            switch (pFormatStr[formatStrPos])
            {
              case '0':
              case '1':
              case '2':
              case '3':
              case '4':
              case '5':
              case '6':
              case '7':
              case '8':
              case '9':
              {
                if (multipleLimit == -1)
                {
                  isMultiple = TRUE;
                  sscanf (&pFormatStr[formatStrPos],"%d", &multipleLimit);
                }
                formatStrPos++;
                break;
              }
              case '\\':
              {
                /*a constant*/
                isConstant = TRUE;
                formatStrPos++;
                break;
              }
              case '*':
              {
                isMultiple = TRUE;
                formatStrPos++;
                break;
              }
              case '<':
              {
                isLanguageSpecific = TRUE;
                formatStrPos++;
                break;
              }
              default:
              {
                currFormat = pFormatStr[formatStrPos];
                state = FORMAT_APPLY;
                if (!isMultiple)
                {
                  formatStrPos++;
                }
              }
            }
          }
        }
        else
        {
          state = FINISHED;
        }
        break;
      }
      case FORMAT_APPLY:
      {
        state = FORMAT_PARSE;
        if (inStr && inStrPos < startSel)
        {
          int charlen = we_cmmn_utf8_charlen (&inStr[inStrPos]);
          int out_charlen = charlen;
          if (outStr && ((outStrPos + charlen) <= *outStrSize))
          {
            memcpy (&outStr[outStrPos], &inStr[inStrPos], charlen);
            wml_make_char_valid (&outStr[outStrPos], charlen, currFormat, isConstant, &out_charlen);
          }
          if (multipleLimit != -1)
          {
            multipleLimit--;
          }
          inStrPos += charlen;
          outStrPos += out_charlen;
        }
        else if (inStr && inStrPos < inStrLen || newCharsPos < newCharsLen)
        {
          if (newChars && newCharsPos < newCharsLen)
          {
            int charlen = 1;
            int out_charlen = 1;
            /*add new chars*/
            out_charlen = charlen = we_cmmn_utf8_charlen (&newChars[newCharsPos]);
            if (outStr && ((outStrPos + charlen)  <= *outStrSize))
            {
              memcpy (&outStr[outStrPos], &newChars[newCharsPos], charlen);
              wml_make_char_valid (&outStr[outStrPos], charlen, currFormat, isConstant, &out_charlen);
            }
            if (multipleLimit != -1)
            {
              multipleLimit--;
            }
            outStrPos += out_charlen;
            newCharsPos += charlen;
            if (!isMultiple)
            {
              /*overwrite*/
              inStrPos++;
            }
          }
          else
          {
            while ((inStrPos < endSel) && 
              (isMultiple && (!inputRequired || (((endSel - startSel) - inStrLen) + newCharsLen))))
            {
              /*delete selection*/
              inStrPos++;
            }
            if (inStrPos < inStrLen)
            {
              int charlen = we_cmmn_utf8_charlen (&inStr[inStrPos]);
              int out_charlen = charlen;
              if (outStr && ((outStrPos + out_charlen) <= *outStrSize))
              {
                memcpy (&outStr[outStrPos], &inStr[inStrPos], charlen);
                wml_make_char_valid (&outStr[outStrPos], charlen, currFormat, isConstant, &out_charlen);
              }
              if (multipleLimit != -1)
              {
                multipleLimit--;
              }
              inStrPos += charlen;
              outStrPos += out_charlen;
            }
          }
        }
        else
        {
          if (isMultiple && !inputRequired)
          {
            state = FINISHED;
          }
          else
          {
            int out_charlen = 1;
            if (outStr && (outStrPos < *outStrSize))
            {
              outStr[outStrPos] = 0;

              wml_make_char_valid (&outStr[outStrPos], 1, currFormat, isConstant, &out_charlen);
            }
            if (multipleLimit != -1)
            {
              multipleLimit--;
            }
            outStrPos += out_charlen;
          }
        }
        if (!isMultiple)
        {
          isConstant = FALSE;
        }
        inputRequired = FALSE;
      }
    }
  }
  /*add nulltermination*/
  if (outStr && (outStrPos < *outStrSize))
  {
    outStr[outStrPos] = 0;
  }
  outStrPos++;

  if (outStrPos > *outStrSize)
  {
    isOk = FALSE;
  }

  *outStrSize = outStrPos;

  return isOk;
}


