/*=====================================================================================

  FILE NAME : we_csc.c
  MODULE NAME : WE Framework csc
  
    PRE-INCLUDE FILES DESCRIPTION
    
      GENERAL DESCRIPTION
      This file can be included by program when need to convert charset.In this file all csc interfaces and 
      related macro is defined.
      These Info is  aptotic and is not changed by switching Project.

      TECHFAITH Software Confidential Proprietary
      (c) Copyright 2006 by TechSoft. All Rights Reserved.
      =======================================================================================
      Revision History

      Modification                 Tracking
      Date           Author         Number        Description of changes
      ----------   ------------    ---------   --------------------------------------------
      2006-06-02    Song Yi		                   create this file
      2006-07-03    Song Yi                        change all function's return value ,when operate success ,return true
                                                   when there are not enough memery ,return NO_ENOUGH_MEMERY, which is defined
                                                   in WE_CSC.h
      2006-07-12    Song Yi                        Add the convert founction of WeCsc_CharSetUtf8ToUtf8
      Self-documenting Code
      Describe/explain low-level design, especially things in header files, of this module and/or group 
      of functions and/or specific function that are hard to understand by reading code and thus requires 
      detail description.
      Free format !
      2007-03-15    tang                            for linux

=====================================================================================*/

/*=====================================================================================
Include File Section
=====================================================================================*/
#include "we_def.h"
#include "we_mem.h"
#include "we_csc.h"
#include "we_scl.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>



/*******************************************************************************
* Macro Define Section
*******************************************************************************/

#ifndef MIN
#define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )
#endif

#define WE_CSC_SURROGATE_HIGH_START          0xd800UL
#define WE_CSC_SURROGATE_HIGH_END            0xdbffUL
#define WE_CSC_SURROGATE_LOW_START           0xdc00UL
#define WE_CSC_SURROGATE_LOW_END             0xdfffUL
#define WE_CSC_REPLACEMENT_CHAR              0x0000fffdUL



#define WE_CSC_BIG_ENDIAN                    1
#define WE_CSC_LITTLE_ENDIAN                 2

#define WE_CSC_TABLE_SIZE                    31
#define WE_CSC_STRINGS_TABLE_SIZE            38
#define WE_CSC_HASH_TABLE_SIZE               41
#define WE_NUMBER_OF_ADDITIONAL_CSC_SETS     2


#define WE_TABLE_SEARCH_LINEAR 3

#define UNICODE_TO_BIG5 3
#define UNICODE_TO_GB   2

#define GB_TO_UNICODE   5
#define BIG5_TO_UNICODE 6

/*when converting from Unicode to BIG5/GB, if there is no Chinese char matched for that Unicode,
a default char will be used to fill the position, usually it is '?' */
#define GB_ERR_CHAR     0xA1F5  /*'?'   in GB2312*/
#define BIG5_ERR_CHAR   0xA1BC  /*'?'   in BIG5*/

/*  File Path */
#define CSC_PATH_BIG5_TO_UCS                "/wecsctable/big5toucs.dat"
#define CSC_PATH_UCS_TO_BIG5                "/wecsctable/ucs2big5.dat"
#define CSC_PATH_GB_TO_UCS                  "/wecsctable/gb2ucs.dat"
#define CSC_PATH_UCS_TO_GB                  "/wecsctable/ucs2gb.dat"

/*******************************************************************************
* Type Define Section
*******************************************************************************/

/*the charset struct */
typedef struct _WeCharset
{
  WE_INT32           iFromCharset; /*charset type*/
  WE_INT32		     iToCharset;      /* charset type*/
  Fn_We_Charset_Convert* fnConvert; /* charset convert founction*/
} St_WeCharset;

const WE_UINT32 auiCharsetOffsetsFromUtf8[5] = {
    0UL, 0UL, 0x00003080UL, 0x000e2080UL, 0x03c82080UL};

const static St_WeCharset astWeCharSetTable[WE_CSC_TABLE_SIZE] = {
        {WE_CSC_ASCII,    WE_CSC_UTF_8,    WeCsc_CharsetAsciiToUtf8},
        {WE_CSC_LATIN_1,  WE_CSC_UTF_8,    WeCsc_CharsetLatin1ToUtf8},
        {WE_CSC_UTF_16BE, WE_CSC_UTF_8,    WeCsc_CharsetUtf16beToUtf8},
        {WE_CSC_UTF_16LE, WE_CSC_UTF_8,    WeCsc_CharsetUtf16leToUtf8},
        {WE_CSC_UTF_16,   WE_CSC_UTF_8,    WeCsc_CharsetUtf16ToUtf8},
        {WE_CSC_UCS_2BE,  WE_CSC_UTF_8,    WeCsc_CharsetUcs2beToUtf8},
        {WE_CSC_UCS_2LE,  WE_CSC_UTF_8,    WeCsc_CharsetUcs2leToUtf8},
        {WE_CSC_UCS_2,    WE_CSC_UTF_8,    WeCsc_CharsetUcs2ToUtf8},
        
        {WE_CSC_UTF_8,    WE_CSC_ASCII,    WeCsc_CharsetUtf8ToAscii},
        {WE_CSC_UTF_8,    WE_CSC_LATIN_1,  WeCsc_CharsetUtf8ToLatin1},
        {WE_CSC_UTF_8,    WE_CSC_UTF_16BE, WeCsc_CharsetUtf8ToUtf16be},
        {WE_CSC_UTF_8,    WE_CSC_UTF_16LE, WeCsc_CharsetUtf8ToUtf16le},
        {WE_CSC_UTF_8,    WE_CSC_UTF_16,   WeCsc_CharsetUtf8ToUtf16},
        {WE_CSC_UTF_8,    WE_CSC_UCS_2BE,  WeCsc_CharsetUtf8ToUcs2be},
        {WE_CSC_UTF_8,    WE_CSC_UCS_2LE,  WeCsc_CharsetUtf8ToUcs2le},
        {WE_CSC_UTF_8,    WE_CSC_UCS_2,    WeCsc_CharsetUtf8ToUcs2},
        
        {WE_CSC_ASCII,    WE_CSC_UTF_16BE, WeCsc_CharsetAsciiToUtf16be},
        {WE_CSC_ASCII,    WE_CSC_UTF_16LE, WeCsc_CharsetAsciiToUtf16le},
        {WE_CSC_LATIN_1,  WE_CSC_UTF_16BE, WeCsc_CharsetLatin1ToUtf16be},
        {WE_CSC_LATIN_1,  WE_CSC_UTF_16LE, WeCsc_CharsetLatin1ToUtf16le},
        {WE_CSC_UTF_16BE, WE_CSC_UTF_16LE, WeCsc_CharsetUtf16beToUtf16le},
        /*add*/
        {WE_CSC_BIG5,     WE_CSC_UTF_8,    WeCsc_CharsetBig5ToUtf8},
        {WE_CSC_UTF_8,    WE_CSC_BIG5,     WeCsc_CharsetUtf8ToBig5},
        {WE_CSC_GB2312,   WE_CSC_UTF_8,    WeCsc_CharsetGb2312ToUtf8},
        {WE_CSC_UTF_8,    WE_CSC_GB2312,   WeCsc_CharsetUtf8ToGb2312},
        {WE_CSC_LATIN_2,  WE_CSC_UTF_8,    WeCsc_CharsetLatin2ToUtf8},
        {WE_CSC_UTF_8,    WE_CSC_LATIN_2,  WeCsc_CharsetUtf8ToLatin2},
        {WE_CSC_HEBREW,   WE_CSC_UTF_8,    WeCsc_CharsetHebrewToUtf8},
        {WE_CSC_UTF_8,    WE_CSC_HEBREW,   WeCsc_CharsetUtf8ToHebrew},
        {WE_CSC_UTF_8,    WE_CSC_UTF_8,    WeCsc_CharSetUtf8ToUtf8},        
        {0, 0, NULL}
    };

static St_WeSCLStrTableEntry astWeCharSetStrings[WE_CSC_STRINGS_TABLE_SIZE] = {
    {"us-ascii",         WE_CSC_ASCII},
    {"ANSI_X3.4-1968",   WE_CSC_ASCII},
    {"iso-ir-6",         WE_CSC_ASCII},
    {"ansi_x3.4-1986",   WE_CSC_ASCII},
    {"iso_646.irv:1991", WE_CSC_ASCII},
    {"ascii",            WE_CSC_ASCII},
    {"iso646-us",        WE_CSC_ASCII},
    {"us",               WE_CSC_ASCII},
    {"ibm367",           WE_CSC_ASCII},
    {"cp367",            WE_CSC_ASCII},
    {"csascii",          WE_CSC_ASCII},
    {"iso-8859-1",       WE_CSC_LATIN_1},
    {"latin-1",          WE_CSC_LATIN_1},
    {"iso_8859-1:1987",  WE_CSC_LATIN_1},
    {"iso-ir-100",       WE_CSC_LATIN_1},
    {"iso_8859-1",       WE_CSC_LATIN_1},
    {"latin1",           WE_CSC_LATIN_1},
    {"l1",               WE_CSC_LATIN_1},
    {"ibm819",           WE_CSC_LATIN_1},
    {"cp819",            WE_CSC_LATIN_1},
    {"csisolatin1",      WE_CSC_LATIN_1},
    {"utf-8",            WE_CSC_UTF_8},
    {"iso-10646-ucs-2",  WE_CSC_UCS_2},
    {"csunicode",        WE_CSC_UCS_2},
    {"utf-16be",         WE_CSC_UTF_16BE},
    {"utf-16le",         WE_CSC_UTF_16LE},
    {"utf-16",           WE_CSC_UTF_16},
    /*add*/
    {"big5",             WE_CSC_BIG5},
    {"gb2312",           WE_CSC_GB2312 },
    {"iso-8859-2",       WE_CSC_LATIN_2},
    {"latin2",           WE_CSC_LATIN_2},
    {"iso-8859-2",       WE_CSC_LATIN_2},
    { "iso-ir-101",      WE_CSC_LATIN_2},    
    { "csISOLatin2",     WE_CSC_LATIN_2},
    { "iso-8859-8",      WE_CSC_HEBREW},    
    {"hebrew",           WE_CSC_HEBREW},   
    {"iso-ir-138",       WE_CSC_HEBREW},    
    {"csISOLatinHebrew", WE_CSC_HEBREW}
};

WE_UINT8 aucCharsetHashTable[WE_CSC_HASH_TABLE_SIZE] = { 
    15, 14, 24, 255, 20, 255, 0, 10, 2, 8,
    13, 18, 1, 5, 4, 12, 11, 255, 255, 255,
    26, 19, 7, 25, 255, 22, 255, 17, 255, 9,
    255, 16, 255, 255, 255, 3, 255, 255, 23, 21,
    6
};
WE_UINT16 ausLatin2CodeTable[128] = {
    0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,
    0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,
    0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,
    0x000,0x000,0x000,0x000,0x000,0x000,0x000,0x000,
    0x0a0,0x104,0x2d8,0x141,0x0a4,0x13d,0x15a,0x0a7,
    0x0a8,0x160,0x15e,0x164,0x179,0x0ad,0x17d,0x17b, 
    0x0b0,0x105,0x2db,0x142,0x0b4,0x13e,0x15b,0x2c7, 
    0x0b8,0x161,0x15f,0x165,0x17a,0x2dd,0x17e,0x17c, 
    0x154,0x0c1,0x0c2,0x102,0x0c4,0x139,0x106,0x0c7, 
    0x10c,0x0c9,0x118,0x0cb,0x11a,0x0cd,0x0ce,0x10e, 
    0x110,0x143,0x147,0x0d3,0x0d4,0x150,0x0d6,0x0d7, 
    0x158,0x16e,0x0da,0x170,0x0dc,0x0dd,0x162,0x0df,
    0x155,0x0e1,0x0e2,0x103,0x0e4,0x13a,0x107,0x0e7, 
    0x10d,0x0e9,0x119,0x0eb,0x11b,0x0ed,0x0ee,0x10f, 
    0x111,0x144,0x148,0x0f3,0x0f4,0x151,0x0f6,0x0f7,     
    0x159,0x16f,0x0fa,0x171,0x0fc,0x0fd,0x163,0x2d9   
};

WE_UINT8 aucCharsetUtf8BytesPerCharacter[16] = {
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 2, 2, 3, 4
};


const St_WeSCLStrtableInfo stWeCharsets = {
    astWeCharSetStrings,
    aucCharsetHashTable,
    WE_CSC_HASH_TABLE_SIZE,
    WE_CSC_STRINGS_TABLE_SIZE,
    WE_TABLE_SEARCH_LINEAR,
    0
};
/*query info*/
typedef struct _QueryInfo
{      
    WE_UINT16    usMinCode;
    WE_UINT16    usMaxCode;    
    WE_INT32     iCount;
    WE_UINT16    usOffset;    
    WE_INT16     sContinue;
}St_QueryInfo;

/***************************************************************************************************
*   Function Define Section
***************************************************************************************************/
/*=====================================================================================
FUNCTION: 
    We_FindFunctionOfCSC
CREATE DATE: 
    2006-05-30
AUTHOR: 
    Song Yi
DESCRIPTION:
    Get Charset Convert Function.
ARGUMENTS PASSED:
    iFromCharset: Source charset ID (Value must equal to upper macro)
    iToCharset: Destination charset ID (Value must equal to upper macro)
RETURN VALUE:
    This function will get pointer to function for converting charset.
USED GLOBAL VARIABLES:
    none
USED STATIC VARIABLES:
    none
CALL BY:
    All program need convert charset.
IMPORTANT NOTES:
    If you want to convert charset,firstly you must call this function for getting 
    function of converting charset,then you call the function of converting charset
    to convert charset.
=====================================================================================*/
Fn_We_Charset_Convert*  WeCsc_FindFunctionOfCSC
(
    WE_INT32 iFromCharset, 
    WE_INT32 iToCharset
)
{
    WE_INT32  iIndex = 0;

    for(iIndex = 0; astWeCharSetTable[iIndex].iFromCharset != 0; iIndex++)        
    {
        if((astWeCharSetTable[iIndex].iFromCharset == iFromCharset) \
        && (astWeCharSetTable[iIndex].iToCharset == iToCharset))
        {
            return  (astWeCharSetTable[iIndex].fnConvert);
        }
    }
  
    return  NULL;

}

/*=====================================================================================
FUNCTION: 
    We_FindFunctionOfCSCByBOM
CREATE DATE: 
    2006-05-28
AUTHOR: 
    Song Yi
DESCRIPTION:
    Get Charset Convert Function by endian of pcSrc.
ARGUMENTS PASSED:
    iFromCharset: Source charset ID (Value must equal to upper macro).
    iToCharset: Destination charset ID (Value must equal to upper macro).
    pcSrc:the source string used any Byte-Order Mark that might be present in it.
    piLen:output parameter.If *piLen==0,no BOM was found;
          If *piLen==2,a 2 byte BOM was found.
RETURN VALUE:
    This function will get pointer to function for converting charset.
USED GLOBAL VARIABLES:
    none
USED STATIC VARIABLES:
    none
CALL BY:
    All program need convert charset.
IMPORTANT NOTES:
    If you want to convert charset,firstly you must call this function for getting 
    function of converting charset,then you call the function of converting charset
    to convert charset.
    using any Byte-Order Mark that might be present in the source.
    On calling this function, the parameter '*iLen' should hold the number
    of bytes available in 'pcSrc', and on return '*iLen' is either 0
    (if no BOM was found) or 2 (if a 2 byte BOM was found).
    If there is no BOM, but 'iFromCharset' is an ambigous character encoding
    (that is, UCS-2 or UTFENOPERSISTMEMORY6) then this function will attempt to guess
    the byte order by checking if either of the two first bytes is zero.
    Returns NULL if no matching conversion function could be found.
=====================================================================================*/
Fn_We_Charset_Convert *WeCsc_FindFunctionOfCSCByBOM
(
    WE_INT32 iFromCharset,
    WE_INT32 iToCharset,
    const WE_CHAR *pcSrc,
    WE_INT32 *piLen
 )
{
    WE_INT8    ucOrderOfByte = 0;
    WE_INT32   uiLenOfUsed = 0;
    WE_INT32   iIndex = 0;
    
    if( NULL == piLen || NULL == pcSrc)
    {
        return NULL;    
    }

    /* Check for Byte-Order Mark (BOM) to determine which encoding to use. */
    if (*piLen >= 2) 
    {
        if (((WE_UCHAR)pcSrc[0] == 0xfe) && ((WE_UCHAR)pcSrc[1] == 0xff)) 
        {
            ucOrderOfByte = WE_CSC_BIG_ENDIAN;
            uiLenOfUsed = 2;
        }
        else if (((WE_UCHAR)pcSrc[0] == 0xff) && ((WE_UCHAR)pcSrc[1] == 0xfe))
        {
            ucOrderOfByte = WE_CSC_LITTLE_ENDIAN;
            uiLenOfUsed = 2;
        }
        else if ((iFromCharset == WE_CSC_UCS_2) ||
            (iFromCharset == WE_CSC_UTF_16)) 
        {
            /* We really need the byte order! */
            if (((WE_UCHAR)pcSrc[0] == 0) && ((WE_UCHAR)pcSrc[1] != 0))
            {
                ucOrderOfByte = WE_CSC_BIG_ENDIAN;
            }
            else if (((WE_UCHAR)pcSrc[0] != 0) && ((WE_UCHAR)pcSrc[1] == 0))
            {
                ucOrderOfByte = WE_CSC_LITTLE_ENDIAN;
            }
        }
    }
    *piLen = uiLenOfUsed;
    
    if (ucOrderOfByte == WE_CSC_BIG_ENDIAN)
    {
        if (iFromCharset == WE_CSC_UCS_2)
        {
            iFromCharset = WE_CSC_UCS_2BE;
        }
        else if (iFromCharset == WE_CSC_UTF_16)
        {
            iFromCharset = WE_CSC_UTF_16BE;
        }
    }
    else if (ucOrderOfByte == WE_CSC_LITTLE_ENDIAN) 
    {
        if (iFromCharset == WE_CSC_UCS_2)
        {
            iFromCharset = WE_CSC_UCS_2LE;
        }
        else if (iFromCharset == WE_CSC_UTF_16) 
        {
            iFromCharset = WE_CSC_UTF_16LE;
        }
    }
    
    if (iToCharset == WE_CSC_UCS_2)
    {
        iToCharset = WE_CSC_UCS_2BE;
    }
    else if (iToCharset == WE_CSC_UTF_16) 
    {
        iToCharset = WE_CSC_UTF_16BE;
    }
    
    for (iIndex = 0; astWeCharSetTable[iIndex].iFromCharset!= 0; iIndex++)
    {
        if ((astWeCharSetTable[iIndex].iFromCharset == iFromCharset) &&
            (astWeCharSetTable[iIndex].iToCharset == iToCharset))
        {
            return astWeCharSetTable[iIndex].fnConvert;
        }
    }
    
    
    return NULL;

}

/*=====================================================================================
FUNCTION: 
    WE_CHARSETINT2STR
CREATE DATE: 
    2006-06-06
AUTHOR: 
    Song Yi
DESCRIPTION:
    Given an integer representation of a charset, return a pointer to a string holding the name of that charset.
ARGUMENTS PASSED:
    iCharset:  integer representation of a charset.
RETURN VALUE:
    a pointer to a string holding the name of that charset.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need to know the charset's name.
IMPORTANT NOTES:
    the caller MUST NOT deallocate or modify the returned string
=====================================================================================*/
const WE_CHAR *WeCsc_CharsetInt2str
(
    WE_INT32 iCharset
)
{
    const WE_CHAR *pStr = NULL;     

    pStr =WE_SCL_INT2STR(iCharset, &stWeCharsets);
    
    if (pStr == NULL) 
    {
        WE_INT32  i;
        
        for (i = 0 ; i < WE_CSC_STRINGS_TABLE_SIZE; i++) 
        {
            if (astWeCharSetStrings[i].iValue == iCharset) 
            {
                pStr = astWeCharSetStrings[i].pcName;
                break;
            }
        }
    }
    
    return pStr;

}

/*=====================================================================================
FUNCTION: 
    WE_CHARSETSTR2INT
CREATE DATE: 
    2006-06-06
AUTHOR: 
    Song Yi
DESCRIPTION:
    iven a charset name as a string, return its integer value.
ARGUMENTS PASSED:
    pcName:  the charset's name.
    iLength: length of the charset
RETURN VALUE:
    return charset's integer value.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need to know the charset's integer value.
IMPORTANT NOTES:
    None
=====================================================================================*/
WE_INT32 WeCsc_CharsetStr2int 
(
    const WE_CHAR *pcName, 
    WE_INT32 iLength
)
{
   WE_INT32 iCharset = WE_CSC_UNKNOWN;

   if(NULL == pcName )
   {
      return E_WECSC_BAD_PRARAM;
   }
   
   iCharset= WE_SCL_STR2INTLC(pcName,iLength, &stWeCharsets);
   
   if ( iCharset < 0) 
   {
      WE_INT32  i = 0;
      
      iCharset = WE_CSC_UNKNOWN;

      for (i = 0; i < WE_CSC_STRINGS_TABLE_SIZE; i++) 
      {
         if (WE_SCL_STRNCMPNC (astWeCharSetStrings[i].pcName, pcName, iLength) == 0) 
         {
            iCharset =astWeCharSetStrings[i].iValue;
            return iCharset;
         }
         
      }
   }
   
   return iCharset;

}

/*=====================================================================================
FUNCTION: 
    WE_CHARSETUTF8TOUNICODE 
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert one Unicode character encoded as UTF-8 to its Unicode character code
ARGUMENTS PASSED:
     pChar: Source char.
     puiUnicode:point to  the character Unicode's location.
RETURN VALUE:
   Returns the number of bytes used from the UTF-8 string, or -1 on error.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    none
CALL BY:
    All program need Convert one Unicode character encoded as UTF-8 to its Unicode character code.
IMPORTANT NOTES:
    None
=====================================================================================*/
WE_INT32 WeCsc_CharsetUtf8ToUnicode
(
    WE_CHAR *pChar, 
    WE_UINT32 *puiUnicode
)
{
    WE_UINT32 uiResult = 0;
    WE_UINT8  ucNumOfByte = 0;
    if(NULL == pChar || NULL == puiUnicode )
    {
        return E_WECSC_BAD_PRARAM;
    }
    
    /* Read one Unicode character in UTF-8 format: */
    ucNumOfByte = aucCharsetUtf8BytesPerCharacter[(WE_UCHAR)*pChar >> 4];
    if(ucNumOfByte > WE_STRLEN(pChar))
    {
        return FALSE;
    }
    
    uiResult = 0;
    switch (ucNumOfByte)
    {
        case 0:
            return FALSE;
        
        case 4: uiResult += (WE_UCHAR)*pChar++; uiResult <<= 6;
        case 3: uiResult += (WE_UCHAR)*pChar++; uiResult <<= 6;
        case 2: uiResult += (WE_UCHAR)*pChar++; uiResult <<= 6;
        case 1: uiResult += (WE_UCHAR)*pChar++;
    }
    *puiUnicode = uiResult - auiCharsetOffsetsFromUtf8[ucNumOfByte];
    
    return ucNumOfByte;

}

/*=====================================================================================
FUNCTION: 
    WE_CHARSETUNICODETOUTF8
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert one Unicode character to UTF-8.
ARGUMENTS PASSED:
    uiUnicode: the Unicode location.
   pStrSource:point to  the The encoded string.
RETURN VALUE:
   Returns the number of bytes stored, or -1 on error.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    none
CALL BY:
    All program need Convert one Unicode character to UTF-8.
IMPORTANT NOTES:
    None
=====================================================================================*/
WE_INT32 WeCsc_CharsetUnicodeToUtf8
(
    WE_UINT32 uiUnicode, 
    WE_CHAR *pStrSource
)
{
     WE_UINT8   ucNumOfByte = 0;
     WE_UCHAR   ucMaskOfUttf8 = 0;

  /* Determine how many UTF8-bytes are needed for this character,
   * and which bits should be set in the first byte. */
//     if(/*pStrSource == NULL ||*/)
     //     {
     //         return E_WECSC_BAD_PRARAM;
     //     }
     
      if (uiUnicode < 0x80) 
      {
          ucNumOfByte = 1;
          ucMaskOfUttf8 = 0;
      }
      else if (uiUnicode < 0x800) 
      {
           ucNumOfByte = 2;
           ucMaskOfUttf8 = 0xc0;
      }
      else if (uiUnicode < 0x10000) 
      {
          ucNumOfByte = 3;
          ucMaskOfUttf8 = 0xe0;
      }
      else if (uiUnicode < 0x200000) 
      {
          ucNumOfByte = 4;
          ucMaskOfUttf8 = 0xf0;
      }
      else 
      {
          return FALSE;
      }


      if (pStrSource != NULL)
      {
         switch (ucNumOfByte)
         {
         case 4:
            pStrSource[3] = (WE_UCHAR)((uiUnicode & 0x3f) | 0x80);
            uiUnicode >>= 6;
         case 3:
            pStrSource[2] = (WE_UCHAR)((uiUnicode & 0x3f) | 0x80);
            uiUnicode >>= 6;
         case 2:
            pStrSource[1] = (WE_UCHAR)((uiUnicode & 0x3f) | 0x80);
            uiUnicode >>= 6;
         case 1:
            pStrSource[0] = (WE_UCHAR)(uiUnicode | ucMaskOfUttf8);
         }
      }

      return ucNumOfByte;
}

/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetAsciiToUtf8
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert Ascii string to UTF-8.
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input Ascii string 
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting,the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert  Ascii string to UTF-8.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   

=====================================================================================*/
static  WE_INT32 WeCsc_CharsetAsciiToUtf8 
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG  *plDstLen
)
{
  WE_CHAR    *pcTemp = NULL;
  WE_UINT32  uiLen = MIN (*plSrcLen, *plDstLen);

  if( NULL == pcSrc || NULL == plSrcLen || NULL==plDstLen)
  {
      return E_WECSC_BAD_PRARAM;
  }

  if (pcDst != NULL)
  {
    for (pcTemp = pcDst; pcTemp < pcDst + uiLen; pcTemp++, pcSrc++) 
    {
      if (((WE_UCHAR)*pcSrc) < 0x80)
      {
        *pcTemp = *pcSrc;
      }
      else
      {
        *pcTemp = 0x3f; /* Non-ASCII characters become '?' */
      }
    }
  }
  *plSrcLen = uiLen;
  *plDstLen = uiLen;

  return TRUE;
}

/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetLatin1ToUtf8
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert Latin1 string to UTF-8.
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input Ascii string 
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert  Latin1 string to UTF-8.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
static  WE_INT32 WeCsc_CharsetLatin1ToUtf8 
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG  *plDstLen
)
{
   WE_UINT32  uiTransToUcs = 0;
   WE_UINT32  uiSindex = 0;
   WE_UINT32  uiDindex = 0;
   WE_UINT8   ucNumOfByte = 0;
   WE_UINT8   ucMaskOfUtf8 = 0;
   
   if(NULL == pcSrc || NULL == plSrcLen ||  NULL == plDstLen )
   {
      return E_WECSC_BAD_PRARAM;
   }    
   for (;;) 
   {
      /* First, translate to UCS4 */
      if((WE_LONG) (uiSindex + 1) > *plSrcLen)
         break;
      
      uiTransToUcs = (WE_UINT32)((WE_UCHAR)pcSrc[uiSindex]);
      
      /* Determine how many UTF8-bytes are needed for this character,
      * and which bits should be set in the first byte. */
      if (uiTransToUcs < 0x80)
      {
         ucNumOfByte = 1;
         ucMaskOfUtf8 = 0;
      }
      else 
      {
         ucNumOfByte = 2;
         ucMaskOfUtf8 = 0xc0;
      }
      
      /* Only write to destination vector if pcDst != MSF_NULL */
      if (pcDst != NULL)
      {         
         /* Is there room for this in the destination vector? */
         if((WE_LONG) (uiDindex + ucNumOfByte) > *plDstLen)
            break;
         
         switch (ucNumOfByte)
         {
         case 2:
            pcDst[uiDindex + 1] = (WE_UCHAR) ((uiTransToUcs& 0x3f) | 0x80);
            uiTransToUcs >>= 6;
            
         case 1:
            pcDst[uiDindex] = (WE_UCHAR) (uiTransToUcs| ucMaskOfUtf8);
         }
      }
      uiSindex += 1;
      uiDindex += ucNumOfByte;
   }
   *plSrcLen = uiSindex;
   *plDstLen = uiDindex;
   
   return TRUE;
}

/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetUtf16beToUtf8
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert Utf16be string to UTF-8.
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input Utf16be string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert  Utf16be string to UTF-8.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
static WE_INT32 WeCsc_CharsetUtf16beToUtf8 
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG  *plDstLen
)
{
    if(NULL == pcSrc || NULL == plSrcLen || NULL == plDstLen )
    {
        return E_WECSC_BAD_PRARAM;
    }
    return WeCsc_CharsetUtf16AnyToUtf8 (pcSrc, plSrcLen, pcDst, plDstLen, TRUE);
}
/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetUtf16leToUtf8
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert Utf16be string to UTF-8.
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input Utf16le string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert  Utf16le string to UTF-8.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/

static WE_INT32 WeCsc_CharsetUtf16leToUtf8 
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG  *plDstLen
)
{
    if(NULL == pcSrc || NULL == plSrcLen ||  NULL == plDstLen )
    {
        return E_WECSC_BAD_PRARAM;
    }
    return WeCsc_CharsetUtf16AnyToUtf8 (pcSrc, plSrcLen, pcDst, plDstLen, FALSE);
}

/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetUtf16ToUtf8
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert Utf16 string to UTF-8,the defluat is big endian.
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input Utf16 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert  Utf16 string to UTF-8.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
static WE_INT32 WeCsc_CharsetUtf16ToUtf8 
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG  *plDstLen
)
{
    if(NULL == pcSrc || NULL == plSrcLen ||  NULL == plDstLen )
    {
        return E_WECSC_BAD_PRARAM;
    }
    /* For now, we just assume it is big endian. */
    return WeCsc_CharsetUtf16AnyToUtf8 (pcSrc, plSrcLen, pcDst, plDstLen, TRUE);
}

/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetUtf16AnyToUtf8
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert any Utf16 string to UTF-8,the defluat is big endian.
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input Utf16 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert  Utf16 string to UTF-8.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
static  WE_INT32 WeCsc_CharsetUtf16AnyToUtf8
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG  *plDstLen, 
    WE_INT32  iBigEndian
)                             
{
    WE_UINT32  uiTransToUcs = 0;
    WE_UINT32  uiTransToUcsEx = 0;
    WE_UINT32  uiSindex = 0;
    WE_UINT32  uiDindex = 0;
    WE_INT32   iSouNumOfByte = 0;
    WE_INT32   iDesNumOfByte = 0;
    WE_UINT8   ucMaskOfUtf8 = 0;
    WE_INT32   iShiftFirst = 0;
    WE_INT32   iShiftSecond = 0;
    WE_INT32   iShiftThird = 0;
    WE_INT32   iShiftForth = 0;
    
    if(NULL == pcSrc || NULL == plSrcLen ||  NULL == plDstLen)
    {
        return E_WECSC_BAD_PRARAM;
    }
    if (iBigEndian) 
    {
        iShiftFirst = 0; iShiftSecond = 1; iShiftThird = 2; iShiftForth = 3;
    }
    else 
    {
        iShiftFirst = 1; iShiftSecond = 0; iShiftThird = 3; iShiftForth = 2;
    }
    
    for (;;) 
    {
        /* First, translate to UCS4 */
        if ((WE_LONG)(uiSindex + 2 ) > *plSrcLen)
            break;
        
        uiTransToUcs = ((WE_UCHAR)pcSrc[uiSindex + iShiftFirst] << 8) |
            (WE_UCHAR)pcSrc[uiSindex + iShiftSecond];
        iSouNumOfByte = 2;
        
        if ((uiTransToUcs >= WE_CSC_SURROGATE_HIGH_START) &&
            (uiTransToUcs <= WE_CSC_SURROGATE_HIGH_END)) 
        {
            if ((WE_LONG)(uiSindex + 4 )> *plSrcLen)
                break;
            uiTransToUcsEx = ((WE_UCHAR)pcSrc[uiSindex + iShiftThird] << 8) |
                (WE_UCHAR)pcSrc[uiSindex + iShiftForth];
            iSouNumOfByte = 4;
            if ((uiTransToUcsEx >= WE_CSC_SURROGATE_LOW_START) &&
                (uiTransToUcsEx <= WE_CSC_SURROGATE_LOW_END)) 
            {
                uiTransToUcs = ((uiTransToUcs- WE_CSC_SURROGATE_HIGH_START) << 10)
                    + (uiTransToUcsEx - WE_CSC_SURROGATE_LOW_START) + 0x10000UL;
            }
        }
        
        /* Determine how many UTF8-bytes are needed for this character,
        * and which bits should be set in the first byte. */
        if (uiTransToUcs < 0x80) 
        {
            iDesNumOfByte = 1;
            ucMaskOfUtf8 = 0;
        }
        else if (uiTransToUcs < 0x800)
        {
           iDesNumOfByte= 2;
            ucMaskOfUtf8 = 0xc0;
        }
        else if (uiTransToUcs < 0x10000) 
        {
           iDesNumOfByte = 3;
            ucMaskOfUtf8 = 0xe0;
        }
        else if (uiTransToUcs < 0x200000) 
        {
           iDesNumOfByte = 4;
            ucMaskOfUtf8 = 0xf0;
        }
        else 
        {
            iDesNumOfByte = 2;
            uiTransToUcs = WE_CSC_REPLACEMENT_CHAR;
            ucMaskOfUtf8 = 0xc0;
        }
        
        
        /* Only write to destination vector if pcDst != MSF_NULL */
        if (pcDst != NULL)
        {
            /* Is there room for this in the destination vector? */
            if ((WE_LONG)(uiDindex + iDesNumOfByte )> *plDstLen)
               break;

            switch (iDesNumOfByte)
            {
                case 4:
                    pcDst[uiDindex + 3] = (WE_CHAR)((uiTransToUcs & 0x3f) | 0x80);
                    uiTransToUcs>>= 6;
                case 3:
                    pcDst[uiDindex + 2] = (WE_CHAR) ((uiTransToUcs & 0x3f) | 0x80);
                    uiTransToUcs >>= 6;
                case 2:
                    pcDst[uiDindex + 1] = (WE_CHAR) ((uiTransToUcs & 0x3f) | 0x80);
                    uiTransToUcs >>= 6;
                case 1:
                    pcDst[uiDindex] = (WE_CHAR) (uiTransToUcs | ucMaskOfUtf8);
            }
        }
        
        uiSindex += iSouNumOfByte;
        uiDindex += iDesNumOfByte;
    }
    *plSrcLen = uiSindex;
    *plDstLen = uiDindex;
    
    return TRUE;
}


/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetUcs2beToUtf8
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert Ucs2 big endian string to UTF-8
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input ucs2 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert big endian ucs2 string to UTF-8.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
static WE_INT32 WeCsc_CharsetUcs2beToUtf8 
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG  *plDstLen
)
{
    if(NULL == pcSrc || NULL == plSrcLen ||  NULL == plDstLen)
    {
        return E_WECSC_BAD_PRARAM;
    }
    return WeCsc_CharsetUcs2AnyToUtf8 (pcSrc, plSrcLen, pcDst, plDstLen, TRUE);
}

/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetUcs2leToUtf8
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert Ucs2 little endian string to UTF-8
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input ucs2 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert little endian ucs2 string to UTF-8.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/

static WE_INT32 WeCsc_CharsetUcs2leToUtf8 (const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG  *plDstLen)
{
    if(NULL == pcSrc || NULL == plSrcLen ||  NULL == plDstLen )
    {
        return E_WECSC_BAD_PRARAM;
    }
    return WeCsc_CharsetUcs2AnyToUtf8 (pcSrc, plSrcLen, pcDst, plDstLen, FALSE);
}


/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetUcs2ToUtf8
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert Ucs2 string to UTF-8,deflaut is big endian.
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input ucs2 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert  ucs2 string to UTF-8.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
static WE_INT32 WeCsc_CharsetUcs2ToUtf8 
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG  *plDstLen
)
{
    if(NULL == pcSrc || NULL == plSrcLen ||  NULL == plDstLen )
    {
        return E_WECSC_BAD_PRARAM;
    }
    return WeCsc_CharsetUcs2AnyToUtf8 (pcSrc, plSrcLen, pcDst, plDstLen, TRUE);
}

/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetUcs2ToUtf8
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert any Ucs2 string to UTF-8.
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input ucs2 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert  ucs2 string to UTF-8.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/

static WE_INT32 WeCsc_CharsetUcs2AnyToUtf8
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG  *plDstLen,
    WE_INT32  iBigEndian
)
{
   WE_UINT32   uiTransToUcs = 0;
   WE_UINT32   uiSindex = 0;
   WE_UINT32   uiDindex = 0;
   WE_INT32    iSouNumOfByte = 0; 
   WE_INT32    iDesNumOfByte = 0;
   WE_UINT8    ucMaskOfUtf8 = 0;
   WE_INT32    iShiftFirst = 0;
   WE_INT32    iShiftSecond = 0;
   
   
   if(NULL == pcSrc || NULL == plSrcLen || NULL == plDstLen )
   {
      return E_WECSC_BAD_PRARAM;
   }
   if (iBigEndian)
   {
      iShiftFirst = 0; iShiftSecond = 1; 
   }
   else
   {
      iShiftFirst = 1; iShiftSecond = 0; 
   }
   
   while(1)
   {
      /* First, translate to UCS4 */
      if ((WE_LONG)(uiSindex + 2 )> *plSrcLen)
         break;
      uiTransToUcs = ((WE_UCHAR)pcSrc[uiSindex + iShiftFirst] << 8) |
         (WE_UCHAR)pcSrc[uiSindex + iShiftSecond];
      iSouNumOfByte = 2;
      
      /* Determine how many UTF8-bytes are needed for this character,
      * and which bits should be set in the first byte. */
      if (uiTransToUcs < 0x80)
      {
         iDesNumOfByte = 1;
         ucMaskOfUtf8 = 0;
      }
      else if (uiTransToUcs < 0x800)
      {
         iDesNumOfByte = 2;
         ucMaskOfUtf8 = 0xc0;
      }
      else 
      {
         iDesNumOfByte = 3;
         ucMaskOfUtf8 = 0xe0;
      }
            
      /* Only write to destination vector if pcDst != MSF_NULL */
      if (pcDst != NULL) 
      {
         /* Is there room for this in the destination vector? */
         if ((WE_LONG)(uiDindex + iDesNumOfByte) > *plDstLen)
            break;

         switch (iDesNumOfByte)
         {
         case 3:
            pcDst[uiDindex + 2] = (WE_UCHAR) ((uiTransToUcs & 0x3f) | 0x80);
            uiTransToUcs >>= 6;
         case 2:
            pcDst[uiDindex + 1] = (WE_UCHAR) ((uiTransToUcs & 0x3f) | 0x80);
            uiTransToUcs >>= 6;
         case 1:
            pcDst[uiDindex] = (WE_UCHAR) (uiTransToUcs | ucMaskOfUtf8);
         }
      }
      
      uiSindex += iSouNumOfByte;
      uiDindex += iDesNumOfByte;
   }
   *plSrcLen = uiSindex;
   *plDstLen = uiDindex;
   
   return TRUE;
    
}
/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetUtf8ToAscii
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert any Utf8 string to Ascii.
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input utf8 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert Utf-8 string to Ascii.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/

static WE_INT32  WeCsc_CharsetUtf8ToAscii 
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG  *plDstLen
)
{
    WE_UINT32     uiSindex = 0;
    WE_UINT32     uiDindex = 0;
    WE_INT32      iSouNumOfByte = 0;
    WE_UINT32     uiResult = 0;
    
    if(NULL == pcSrc || NULL == plSrcLen || NULL == plDstLen)
    {
        return E_WECSC_BAD_PRARAM;
    }
    while ((WE_LONG)(uiSindex) < *plSrcLen) 
    {
        /* Read one Unicode character in UTF-8 format: */
        iSouNumOfByte =aucCharsetUtf8BytesPerCharacter[(WE_UCHAR)*pcSrc >> 4];
        if ((WE_LONG)(uiSindex + iSouNumOfByte) > *plSrcLen)
            break;
        
        uiResult = 0;
        switch (iSouNumOfByte)
        {
            case 0:
                return FALSE;
            
            case 4: uiResult += (WE_UCHAR)*pcSrc++; uiResult <<= 6;
            case 3: uiResult += (WE_UCHAR)*pcSrc++; uiResult <<= 6;
            case 2: uiResult += (WE_UCHAR)*pcSrc++; uiResult <<= 6;
            case 1: uiResult += (WE_UCHAR)*pcSrc++;
        }
        uiResult -= auiCharsetOffsetsFromUtf8[iSouNumOfByte];
        
        /* Write one Unicode character in Latin-1 format: */
        if (uiResult> 0x7f)
            uiResult = 0x3f; /* Non-ASCII characters become '?' */
        
        if (pcDst != NULL)
        {
           if((WE_LONG)(uiDindex +1) > *plDstLen)
              break;
 
            pcDst[uiDindex] = (WE_UCHAR)(uiResult & 0xff);
        }
        uiSindex += iSouNumOfByte;
        uiDindex += 1;
    }
    *plSrcLen = uiSindex;
    *plDstLen = uiDindex;
    
    return TRUE;
}

/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetUtf8ToLatin1
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert any Utf8 string to Latin1.
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input utf8 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert Utf-8 string to Latin1.
IMPORTANT NOTES:
    if the source string contains character codes
    outside the range of Latin-1, such characters will
    be converted to the character code 0x3f, that is '?'.
    if the output length is more larger and the output is NULL,it will output the length actually  need       
=====================================================================================*/
static WE_INT32 WeCsc_CharsetUtf8ToLatin1
(
     const WE_CHAR *pcSrc, 
     WE_LONG *plSrcLen, 
     WE_CHAR *pcDst, 
     WE_LONG *plDstLen
 )
{
    WE_INT32   iSindex = 0;
    WE_INT32   iDindex = 0;
    WE_INT32   iSouNumOfByte = 0;
    WE_UINT32  uiResult = 0;
    
    if(NULL == pcSrc || NULL == plSrcLen || NULL == plDstLen)
    {
        return E_WECSC_BAD_PRARAM;
    }
    while (iSindex < *plSrcLen)
    {
        /* Read one Unicode character in UTF-8 format: */
        iSouNumOfByte =aucCharsetUtf8BytesPerCharacter[(WE_UCHAR)*pcSrc >> 4];
        if (iSindex + iSouNumOfByte > *plSrcLen)
            break;
        
        uiResult = 0;
        switch (iSouNumOfByte) 
        {
        case 0:
            return FALSE;
            
        case 4: uiResult += (WE_CHAR)*pcSrc++; uiResult <<= 6;
        case 3: uiResult += (WE_CHAR)*pcSrc++; uiResult <<= 6;
        case 2: uiResult += (WE_CHAR)*pcSrc++; uiResult <<= 6;
        case 1: uiResult += (WE_CHAR)*pcSrc++;
        }
        uiResult -= auiCharsetOffsetsFromUtf8[iSouNumOfByte];
        
        /* Write one Unicode character in Latin-1 format: */
        if (uiResult > 0xff)
            uiResult = 0x3f; /* Non-Latin-1 characters become '?' */
        

        if (pcDst != NULL)
        {
           if(iDindex+1 > *plDstLen)
              break;

            pcDst[iDindex] = (WE_CHAR)(uiResult & 0xff);
        }
        iSindex += iSouNumOfByte;
        iDindex += 1;
    }
    *plSrcLen = iSindex;
    *plDstLen = iDindex;
    
    return TRUE;
}

/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetUtf8ToUtf16any 
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert any Utf8 string to any utf16 string.
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input utf8 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert Utf-8 string to utf16.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
static WE_INT32 WeCsc_CharsetUtf8ToUtf16any 
(
     const WE_CHAR *pcSrc, 
     WE_LONG *plSrcLen,
     WE_CHAR *pcDst, 
     WE_LONG *plDstLen,
     WE_INT32 iBigEndian
 )
{
    WE_INT32   iSindex = 0;
    WE_INT32   iDindex = 0;
    WE_INT32   iSouNumOfByte = 0;
    WE_INT32   iDesNumOfByte = 0;
    WE_UINT32  uiResult = 0;
    WE_UINT32  uiTmp = 0;
    WE_INT32   iMaskFirst = 0;
    WE_INT32   iMaskSecond = 0;
    WE_INT32   iMaskThird = 0;
    WE_INT32   iMaskForth = 0;
    
    if(NULL == pcSrc || NULL == plSrcLen || NULL == plDstLen )
    {
        return E_WECSC_BAD_PRARAM;
    }
    if (iBigEndian) 
    {
        iMaskFirst = 0; 
        iMaskSecond = 1; 
        iMaskThird = 2;
        iMaskForth = 3;
    }
    else
    {
        iMaskFirst = 1;
        iMaskSecond = 0; 
        iMaskThird = 3; 
        iMaskForth = 2;
    }
    
    while (iSindex < *plSrcLen) 
    {
        /* Read one Unicode character in UTF-8 format: */
        iSouNumOfByte = aucCharsetUtf8BytesPerCharacter[(WE_UCHAR)*pcSrc >> 4];
        if (iSindex + iSouNumOfByte > *plSrcLen)
            break;
        
        uiResult = 0;
        switch (iSouNumOfByte) 
        {
        case 0:
            return FALSE;
            
        case 4: uiResult += (WE_UCHAR)*pcSrc++; uiResult <<= 6;
        case 3: uiResult += (WE_UCHAR)*pcSrc++; uiResult <<= 6;
        case 2: uiResult += (WE_UCHAR)*pcSrc++; uiResult <<= 6;
        case 1: uiResult += (WE_UCHAR)*pcSrc++;
        }
        uiResult -= auiCharsetOffsetsFromUtf8[iSouNumOfByte];
        
        /* Write one Unicode character in UTF-16 format: */
        if (uiResult < 0x10000) 
        {           
            if (pcDst != NULL)
            {               
               if (iDindex + 2 > *plDstLen)
                  break;

                pcDst[iDindex + iMaskFirst] = (WE_CHAR)((uiResult >> 8) & 0xff);
                pcDst[iDindex + iMaskSecond] = (WE_CHAR)((uiResult & 0xff));
            }
            iDesNumOfByte = 2;
        }
        else
        {
            if (pcDst != NULL)
            {               
               if (iDindex + 4 > *plDstLen)
                  break;

                uiTmp = 0xd7c0 + (uiResult >> 10);
                pcDst[iDindex + iMaskFirst] = (WE_CHAR)((uiTmp >> 8) & 0xff);
                pcDst[iDindex + iMaskSecond] = (WE_CHAR)((uiTmp & 0xff));
                pcDst[iDindex + iMaskThird] = (WE_CHAR)(0xdc | ((uiResult & 0x3ff) >> 8));
                pcDst[iDindex + iMaskForth] = (WE_CHAR)(uiResult & 0xff);
            }
            iDesNumOfByte = 4;
        }
        iSindex += iSouNumOfByte;
        iDindex += iDesNumOfByte;
    }
    *plSrcLen = iSindex;
    *plDstLen = iDindex;
    
    return TRUE;
}
/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetUtf8ToUtf16le
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert any Utf8 string to big endian utf16 string.
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input utf8 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert Utf-8 string to big endian utf16.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
WE_INT32 WeCsc_CharsetUtf8ToUtf16be 
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG *plDstLen
)
{
    if(NULL == pcSrc || NULL == plSrcLen ||  NULL == plDstLen  )
    {
        return E_WECSC_BAD_PRARAM;
    }
    return WeCsc_CharsetUtf8ToUtf16any(pcSrc, plSrcLen, pcDst, plDstLen, TRUE);
}

/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetUtf8ToUtf16le
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert any Utf8 string to little edian utf16 string.
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input utf8 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert Utf-8 string to big endian utf16.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
WE_INT32 
WeCsc_CharsetUtf8ToUtf16le 
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG *plDstLen
 )
{
    if(NULL == pcSrc || NULL == plSrcLen || NULL == plDstLen  )
    {
        return E_WECSC_BAD_PRARAM;
    }
    return WeCsc_CharsetUtf8ToUtf16any(pcSrc, plSrcLen, pcDst, plDstLen, FALSE);
}

/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetUtf8ToUtf16any 
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert any Utf8 string to utf16 string, the deflaut is big endian 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input utf8 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert Utf-8 string to utf16.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
WE_INT32 
WeCsc_CharsetUtf8ToUtf16 
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG *plDstLen
 )
{
    if(NULL == pcSrc || NULL == plSrcLen ||NULL == plDstLen)
    {
        return E_WECSC_BAD_PRARAM;
    }
    /* For now, just assume that it is big endian. */
    return WeCsc_CharsetUtf8ToUtf16any( pcSrc, plSrcLen, pcDst, plDstLen, TRUE);
}

/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetUtf8ToUcs2any  
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert any Utf8 string to any Ucs2 string. 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input utf8 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert Utf-8 string to any utf16 string.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
static WE_INT32 WeCsc_CharsetUtf8ToUcs2any 
(
     const WE_CHAR *pcSrc, 
     WE_LONG *plSrcLen,
     WE_CHAR *pcDst, 
     WE_LONG *plDstLen,
     WE_INT32 iBigEndian
 )
{
    WE_INT32        iSindex = 0;
    WE_INT32        iDindex = 0;
    WE_INT32        iSouNumOfByte = 0; 
    WE_INT32        iDesNumOfByte = 0;
    WE_UINT32       uiResult = 0;
    WE_UINT32       uiNumFirst = 0; 
    WE_UINT32       uiNumSecond = 0;
    
    if(NULL == pcSrc || NULL == plSrcLen || NULL == plDstLen )
    {
        return E_WECSC_BAD_PRARAM;
    }
    if (iBigEndian)
    {
        uiNumFirst = 0; 
        uiNumSecond = 1;
    }
    else
    {
        uiNumFirst = 1; 
        uiNumSecond = 0;
    }
    
    while (iSindex < *plSrcLen)
    {
        /* Read one Unicode character in UTF-8 format: */
        iSouNumOfByte = aucCharsetUtf8BytesPerCharacter[(WE_UCHAR)*pcSrc >> 4];
        if (iSindex + iSouNumOfByte > *plSrcLen)
            break;
        
        uiResult = 0;
        switch (iSouNumOfByte) 
        {
        case 0:
            return FALSE;
            
        case 4: uiResult += (WE_UCHAR)*pcSrc++; uiResult <<= 6;
        case 3: uiResult += (WE_UCHAR)*pcSrc++; uiResult <<= 6;
        case 2: uiResult += (WE_UCHAR)*pcSrc++; uiResult <<= 6;
        case 1: uiResult += (WE_UCHAR)*pcSrc++;
        }
        
        uiResult -= auiCharsetOffsetsFromUtf8[iSouNumOfByte];
        
        /* Write one Unicode character in UCS-2 format: */
        uiResult &= 0xffff;
        
        
        
        if (pcDst != NULL) 
        {
           if (iDindex + 2 > *plDstLen)
              break;

            pcDst[iDindex + uiNumFirst] = (WE_CHAR)((uiResult >> 8) & 0xff);
            pcDst[iDindex + uiNumSecond] = (WE_CHAR)((uiResult & 0xff));
        }
        
        iDesNumOfByte = 2;
        iSindex += iSouNumOfByte;
        iDindex += iDesNumOfByte;
    }
    
    *plSrcLen = iSindex;
    *plDstLen = iDindex;
    
    return TRUE;
}

/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetUtf8ToUcs2be 
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert any Utf8 string to Ucs2 big endian string. 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input utf8 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert Utf-8 string to big endidan Ucs2 string.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
WE_INT32 WeCsc_CharsetUtf8ToUcs2be 
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG *plDstLen
 )
{
    if(NULL == pcSrc || NULL == plSrcLen || NULL == plDstLen )
    {
        return E_WECSC_BAD_PRARAM;
    }
    return WeCsc_CharsetUtf8ToUcs2any ( pcSrc, plSrcLen, pcDst, plDstLen, TRUE);
}

/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetUtf8ToUcs2le 
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert any Utf8 string to Ucs2 little endian string. 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input utf8 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert Utf-8 string to little endidan Ucs2 string.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
WE_INT32 WeCsc_CharsetUtf8ToUcs2le 
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG *plDstLen
 )
{
    if(NULL == pcSrc || NULL == plSrcLen || NULL == plDstLen )
    {
        return E_WECSC_BAD_PRARAM;
    }
    return WeCsc_CharsetUtf8ToUcs2any (pcSrc, plSrcLen, pcDst, plDstLen,FALSE);
}

/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetUtf8ToUcs2 
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert any Utf8 string to Ucs2 big endian string. 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input utf8 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert Utf-8 string to big endidan Ucs2 string.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
WE_INT32 WeCsc_CharsetUtf8ToUcs2
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG *plDstLen
 )
{
    if(NULL == pcSrc || NULL == plSrcLen  || NULL == plDstLen )
    {
        return E_WECSC_BAD_PRARAM;
    }
    return WeCsc_CharsetUtf8ToUcs2any ( pcSrc, plSrcLen, pcDst, plDstLen,TRUE);
}

/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetAsciiToUtf16any 
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert  ascii string to any utf16 string. 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input ascii string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert ascii string to utf-16 string.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
static WE_INT32 WeCsc_CharsetAsciiToUtf16any 
(  
     const WE_CHAR *pcSrc, 
     WE_LONG *plSrcLen,
     WE_CHAR *pcDst, 
     WE_LONG *plDstLen,
     WE_INT32 iBigEndian
 )
{
    WE_INT32         iSindex = 0;
    WE_INT32         iDindex = 0;
    WE_UINT32        uiResult = 0;
    WE_UINT32        uiNumFirst = 0;
    WE_UINT32        uiNumSecond = 0;
    
    if(NULL == pcSrc || NULL == plSrcLen || NULL == plDstLen)
    {
        return E_WECSC_BAD_PRARAM;
    }
    if (iBigEndian) 
    {
        uiNumFirst = 0; 
        uiNumSecond = 1;
    }
    else 
    {
        uiNumFirst = 1; 
        uiNumSecond = 0;
    }
    
    while (iSindex < *plSrcLen) 
    {
        /* Read one Unicode character in ASCII: */
        if (iSindex + 1 > *plSrcLen)
            break;
        
        uiResult = (WE_UCHAR)*pcSrc++;
        
        /* Write one Unicode character in UTF-16 format: */
        if (pcDst != NULL) 
        {
           if (iDindex + 2 > *plDstLen)
              break;
           
            pcDst[iDindex + uiNumFirst] = 0;
            pcDst[iDindex + uiNumSecond] = (WE_CHAR)((uiResult & 0xff));
        }
        
        iSindex += 1;
        iDindex += 2;
    }
    
    *plSrcLen = iSindex;
    *plDstLen = iDindex;
    
    return TRUE;
}


/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetAsciiToUtf16be 
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert  ascii string to big endian utf16 string. 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input ascii string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert ascii string to big endian utf-16 string.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
WE_INT32 WeCsc_CharsetAsciiToUtf16be
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG *plDstLen
 )
{
    if(NULL == pcSrc || NULL == plSrcLen  || NULL == plDstLen )
    {
        return E_WECSC_BAD_PRARAM;
    }
    return WeCsc_CharsetAsciiToUtf16any( pcSrc, plSrcLen, pcDst, plDstLen,TRUE);
}

/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetAsciiToUtf16le 
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert  ascii string to little endian utf16 string. 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input ascii string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert ascii string to little endian utf-16 string.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
WE_INT32 WeCsc_CharsetAsciiToUtf16le
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG *plDstLen
 )
{
    if(NULL == pcSrc || NULL == plSrcLen || NULL == plDstLen)
    {
        return E_WECSC_BAD_PRARAM;
    }
    return WeCsc_CharsetAsciiToUtf16any( pcSrc, plSrcLen, pcDst, plDstLen,FALSE);
}
/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetLatin1ToUtf16any 
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert  Latin1 string to any utf16 string. 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input Latin1 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert Latin1 string to any utf-16 string.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
static WE_INT32 WeCsc_CharsetLatin1ToUtf16any 
(    
 const WE_CHAR *pcSrc, 
 WE_LONG *plSrcLen,
 WE_CHAR *pcDst, 
 WE_LONG *plDstLen,
 WE_INT32 iBigEndian
 )
{
    WE_INT32        iSindex = 0;
    WE_INT32        iDindex = 0;
    WE_UINT32       uiResult = 0;
    WE_UINT32       uiNumFirst = 0;
    WE_UINT32       uiNumSecond = 0;
    
    if(NULL == pcSrc || NULL == plSrcLen || NULL == plDstLen  )
    {
        return E_WECSC_BAD_PRARAM;
    }
    if (iBigEndian) 
    {
        uiNumFirst = 0; 
        uiNumSecond = 1;
    }
    else 
    {
        uiNumFirst = 1;
        uiNumSecond = 0;
    }
    
    while (iSindex < *plSrcLen) 
    {
        /* Read one Unicode character in Latin-1: */
        if (iSindex + 1 > *plSrcLen)
            break;
        
        uiResult = (WE_UCHAR)*pcSrc++;
        
        /* Write one Unicode character in UTF-16 format: */        
        if (pcDst != NULL) 
        {
           if (iDindex + 2 > *plDstLen)
              break;

            pcDst[iDindex + uiNumFirst] = 0;
            pcDst[iDindex + uiNumSecond] = (WE_CHAR)((uiResult & 0xff));
        }
        
        iSindex += 1;
        iDindex += 2;
    }
    *plSrcLen = iSindex;
    *plDstLen = iDindex;
    
    return TRUE;
}
/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetLatin1ToUtf16be 
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert  Latin1 string to big endian utf16 string. 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input Latin1 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert Latin1 string to big endian utf-16 string.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
WE_INT32 WeCsc_CharsetLatin1ToUtf16be
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG *plDstLen
 )
{
    if(NULL == pcSrc || NULL == plSrcLen || NULL == plDstLen )
    {
        return E_WECSC_BAD_PRARAM;
    }
    return WeCsc_CharsetLatin1ToUtf16any(pcSrc, plSrcLen, pcDst, plDstLen,TRUE);
}

/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetLatin1ToUtf16le 
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert  Latin1 string to little endian utf16 string. 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input Latin1 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert Latin1 string to little endian utf-16 string.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
WE_INT32 WeCsc_CharsetLatin1ToUtf16le
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG *plDstLen
 )
{
    if(NULL == pcSrc || NULL == plSrcLen ||  NULL == plDstLen  )
    {
        return E_WECSC_BAD_PRARAM;
    }
    return WeCsc_CharsetLatin1ToUtf16any( pcSrc, plSrcLen, pcDst, plDstLen,FALSE);
}

/*=====================================================================================
FUNCTION: 
     WeCsc_CharsetUtf16beToUtf16le
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert  big endian utf16 string to little endian utf16 string. 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input utf16 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert big endian utf16 string to little endian utf16 string.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
static WE_INT32 WeCsc_CharsetUtf16beToUtf16le
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG *plDstLen
 )
{
   WE_INT32 iMin = MIN (*plSrcLen, *plDstLen);
   WE_INT32 i = 0;
   
   if(NULL == pcSrc || NULL == plSrcLen ||  NULL == plDstLen  )
   {
      return E_WECSC_BAD_PRARAM;
   }
   
   while (i + 1 < iMin) 
   {
      if (pcDst != NULL)
      {
         pcDst[i] = pcSrc[i + 1];
         pcDst[i + 1] = pcSrc[i];
      }
      i += 2;
   }
   *plSrcLen = i;
   *plDstLen = i;
   
   return TRUE;
}
/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetBig5ToUtf8
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert  big5 string to utf8 string. 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input big5 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert  big5 string to utf8 string.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
static WE_INT32 WeCsc_CharsetBig5ToUtf8
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG *plDstLen
)
{
  WE_INT32 iUniLen = 0;
  WE_UINT16 usTempLen = 0;
  WE_UCHAR *pcBig5Str = NULL;
  WE_CHAR *pcUniStr = NULL;

  if(NULL == pcSrc || NULL == plSrcLen || NULL == plDstLen || 0 == *plSrcLen || 0 == *plDstLen )
  {
      return E_WECSC_BAD_PRARAM;
  }
  pcBig5Str = (WE_UCHAR*)WE_MALLOC((*plSrcLen) + 1 );
  if(!pcBig5Str)
  {
    return E_WECSC_NO_ENOUGH_MEMERY;
  }
  WE_MEMSET(pcBig5Str, 0x00, (*plSrcLen) + 1 );

  pcUniStr = WE_MALLOC((*plSrcLen)* 2 + 2 );
  if(!pcUniStr)
  {
    WE_FREE(pcBig5Str);
    return E_WECSC_NO_ENOUGH_MEMERY;
  }

  /*copy  big5Str string */
  WE_MEMCPY((WE_CHAR *)pcBig5Str,(WE_CHAR *)pcSrc,*plSrcLen + 1);
  
  /* change big5 string to unicode string*/
  usTempLen = Big5CharToUnicode( pcBig5Str,(WE_UINT16 *)pcUniStr);
  if(0 == usTempLen)
  {
      WE_FREE(pcBig5Str);
      WE_FREE(pcUniStr);
      return FALSE;
  }
  iUniLen = (*plSrcLen)* 2 ;

  /* change unicode string to utf8 string*/
  if( E_WECSC_BAD_PRARAM == UnicodeToUtf8 (pcUniStr,&iUniLen,pcDst,plDstLen))
  {
      WE_FREE(pcBig5Str);
      WE_FREE(pcUniStr);
      return E_WECSC_BAD_PRARAM;
  }
  WE_FREE(pcBig5Str);
  WE_FREE(pcUniStr);
  return TRUE;
}
/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetUtf8ToBig5
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert utf8 string to big5 string. 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input utf8 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert  big5 string to utf8 string.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
static WE_INT32 WeCsc_CharsetUtf8ToBig5
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG *plDstLen
 )
{ 
  WE_INT32 iUniLen;
  WE_CHAR *pcUniStr = NULL;
//  iUniLen = *plDstLen;
  iUniLen = 0x7fffffff;

  if(NULL == pcSrc || NULL == plSrcLen  || NULL == plDstLen|| 0 == *plSrcLen || 0 == *plDstLen )
  {
      return E_WECSC_BAD_PRARAM;
  }
  
  pcUniStr = (WE_CHAR*) WE_MALLOC((*plSrcLen)* 2 + 2 );
  if(!pcUniStr)
  {
    return E_WECSC_NO_ENOUGH_MEMERY;
  }
  WE_MEMSET(pcUniStr, 0x00, (*plSrcLen)* 2 + 2);

  if(E_WECSC_BAD_PRARAM ==Utf8ToUnicode( pcSrc,plSrcLen,pcUniStr,&iUniLen) )    
  {
      WE_FREE(pcUniStr);      
      return E_WECSC_BAD_PRARAM;
  }
  
  if(FALSE == UnicodeToBig5Char((WE_UINT16 *)pcUniStr,(WE_UCHAR *)pcDst, plDstLen))
  {
      WE_FREE(pcUniStr);      
      return E_WECSC_BAD_PRARAM;
  }

  WE_FREE(pcUniStr);
  return TRUE;
}

/*=====================================================================================
FUNCTION: 
    GbCharToUnicode 
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert a GB encoded string into unicode string 
ARGUMENTS PASSED:
    pMe        : the csc instance 
    pucSrc     : the source string is coded in GB, it must be ended with 0x00.
    piSrcLen   : [In] the length of pucSrc.
                 [Out] the bytes of converted characters.
    pusDes     : The buffer to contain unicode string. the size must be large enough to contain 
    piDstLen   : The length of pucDes.(Here, bytes = length * 2)
RETURN VALUE:
    Returns length of the string and the output unicode string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert GB encoded string into unicode string 
IMPORTANT NOTES:
    None    
=====================================================================================*/
WE_INT32  GbCharToUnicode
( 
     WE_UINT8  *pucSrc,
     WE_INT32  *piSrcLen,
     WE_UINT16 *pusDes,
     WE_INT32  *piDstLen
)                          
{
    WE_INT32   iResult     = 0;
    WE_INT32   iSrcIndex   = 0;
    WE_INT32   iDstIndex   = 0;

    WE_UINT16  usSrcWord   = 0;
    WE_UINT16  usUnicode   = 0;

    WE_UINT16  usTemp      = 0;

    
    if( NULL == pucSrc ||  NULL == piSrcLen || NULL == pusDes || NULL == piDstLen)
    {
        return E_WECSC_BAD_PRARAM;
    }
    

    while (iSrcIndex < *piSrcLen)
    {           
         /*ANSI Char*/
        if(pucSrc[iSrcIndex] <= 0x7F)     
        {  
           /*just convert to 16bit.*/           
           pusDes[iDstIndex++] = (WE_UINT16)pucSrc[iSrcIndex];   
           
           iSrcIndex++;
        }
        else if(iSrcIndex + 1 < *piSrcLen)
        {
            usSrcWord = pucSrc[iSrcIndex + 1] | ( ((WE_UINT16)pucSrc[iSrcIndex]) << 8);
            iSrcIndex += 2;
            /*Two BYTE to form one WORD
            
            the GB code in table_gb[] and BIG5 code in table_big5[]
            is high-low byte reversed, that is, the 'qu' is high byte
            and 'we' is low byte.
            while Chinese encoded in computer are alwarys with 'qu' at 
            low byte and 'wei' at high byte.*/
            
           usTemp = usSrcWord - 0x8080;
           usUnicode = UnicodeFromGBTable(usTemp);
           pusDes[iDstIndex++] = usUnicode;
        }
        else
        {
           break;
        }
    }
    /*append NULL at the end of string*/

    pusDes[iDstIndex] = 0x0000;
   
    *piSrcLen = iSrcIndex;
    *piDstLen = iDstIndex;

    iResult = SUCCESS;

    return iResult;
}

/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetGb2312ToUtf8
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert Gb2312 string to utf8 string. 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input gb2312 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert Gb2312 string to utf8 string.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
static WE_INT32 WeCsc_CharsetGb2312ToUtf8
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG *plDstLen
 )
{
   WE_INT32    iUniLen  = 0;

   WE_UCHAR    *pucGb2312Str  = NULL;
   WE_INT32    iGb2312StrSize = 0;
   WE_CHAR     *pucUniStr     = NULL;
   
   if(NULL == pcSrc || NULL == plSrcLen || NULL == plDstLen  \
    || 0 == *plSrcLen || 0 == *plDstLen  )
   {
      return E_WECSC_BAD_PRARAM;
   }

   iGb2312StrSize = (*plSrcLen) + 1;

   pucGb2312Str = (WE_UCHAR*)WE_MALLOC(iGb2312StrSize);
   if(!pucGb2312Str)
   {
      return E_WECSC_NO_ENOUGH_MEMERY;
   }
   WE_MEMSET(pucGb2312Str, 0x00, iGb2312StrSize);
   
   /*copy  gb2312Str string */
   WE_MEMCPY((WE_CHAR *)pucGb2312Str, (WE_CHAR *)pcSrc, *plSrcLen);
   
   pucUniStr = (WE_CHAR*)WE_MALLOC((*plSrcLen)* 2 + 2 );
   if(!pucUniStr)
   {
      WE_FREE(pucGb2312Str);
      return E_WECSC_NO_ENOUGH_MEMERY;
   }
   
   
   /* change gb2312Str string to unicode string*/
   if(SUCCESS != GbCharToUnicode( pucGb2312Str, plSrcLen, (WE_UINT16 *)pucUniStr, &iUniLen))
   {
      WE_FREE(pucGb2312Str);
      WE_FREE(pucUniStr);
      return FALSE;
   }
   
   iUniLen <<= 1;    // Get bytes.
   
   /* change unicode string to utf8 string*/
   if(E_WECSC_BAD_PRARAM == UnicodeToUtf8 (pucUniStr,&iUniLen,pcDst,plDstLen))
   {
      WE_FREE(pucGb2312Str);
      WE_FREE(pucUniStr);
      return E_WECSC_BAD_PRARAM;
   }

   WE_FREE(pucGb2312Str);
   WE_FREE(pucUniStr);
   return TRUE;
}
/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetUtf8ToGb2312
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert utf8 string to Gb2312 string. 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input utf8 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert utf8 string to Gb2312 string.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
static WE_INT32  WeCsc_CharsetUtf8ToGb2312
(
 const WE_CHAR *pcSrc, 
 WE_LONG *plSrcLen, 
 WE_CHAR *pcDst, 
 WE_LONG *plDstLen
 )
{ 
  WE_INT32 iUniLen = 0x7fffffff;
  WE_CHAR *pucUniStr = NULL;
  
  if(NULL == pcSrc || NULL == plSrcLen || NULL == plDstLen|| 0 == *plSrcLen  )
  {
      return E_WECSC_BAD_PRARAM;
  }
  pucUniStr = (WE_CHAR*)WE_MALLOC((*plSrcLen)* 2 + 2 );
  

  if(!pucUniStr)
  {
    return E_WECSC_NO_ENOUGH_MEMERY;
  }
  WE_MEMSET(pucUniStr, 0x00, (*plSrcLen)* 2 + 2);

  if(E_WECSC_BAD_PRARAM == Utf8ToUnicode( pcSrc,plSrcLen,pucUniStr,&iUniLen))
  {
      WE_FREE(pucUniStr);
      return E_WECSC_BAD_PRARAM;      
  }

  if(FALSE ==UnicodeToGbChar( (WE_UINT16 *)pucUniStr,(WE_UINT8*)pcDst, (WE_INT32*)plDstLen))
  {
      WE_FREE(pucUniStr);
      return E_WECSC_BAD_PRARAM;        
  }

  WE_FREE(pucUniStr);
  return TRUE;

}

/*=====================================================================================
FUNCTION: 
    WeCsc_Iso8859ToUtf8
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert Iso8859 string to utf8 string. 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input Iso8859 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert Iso8859 string to utf8 string.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
static WE_INT32 WeCsc_Iso8859ToUtf8 
(
 const WE_CHAR *pcSrc, 
 WE_LONG *plSrcLen, 
 WE_CHAR *pcDst, 
 WE_LONG *plDstLen,
 const WE_UINT16 *pusCodeTable
 )
{
    WE_UINT32  uiResult = 0;
    WE_INT32   iSindex = 0;
    WE_INT32   iDindex = 0;
    WE_INT32   iDesNumOfByte = 0;
    WE_UINT8   ucMaskOfUtf8 = 0;

    if(NULL == pcSrc || NULL == plSrcLen ||  NULL == plDstLen|| 0 == *plSrcLen || 0 == *plDstLen )
    {
        return E_WECSC_BAD_PRARAM;
    }
    
    while(1)
    {
        /* First, translate to UCS4 */
        if (iSindex + 1 > *plSrcLen)
            break;
        
        if (((WE_UCHAR)pcSrc[iSindex]) <= 127) 
        {
            uiResult = (WE_UCHAR)pcSrc[iSindex];
        }
        else 
        {
            uiResult = (WE_UINT32)(pusCodeTable[(WE_UCHAR)pcSrc[iSindex] - 128]);
        }
        
        /* Determine how many UTF8-bytes are needed for this character,
        * and which bits should be set in the first byte. */
        if (uiResult < 0x80) 
        {
            iDesNumOfByte = 1;
            ucMaskOfUtf8 = 0;
        }
        else if (uiResult < 0x800) 
        {
            iDesNumOfByte = 2;
            ucMaskOfUtf8 = 0xc0;
        }
        else 
        {
            iDesNumOfByte = 3;
            ucMaskOfUtf8 = 0xe0;
        }
        
        /* Only write to destination vector if pcDst != MSF_NULL */
        if (pcDst != NULL) 
        {        
           /* Is there room for this in the destination vector */           
           if (iDindex + iDesNumOfByte > *plDstLen)
              break;

            switch (iDesNumOfByte) 
            {
            case 3:
                pcDst[iDindex + 2] = (WE_CHAR) ((uiResult & 0x3f) | 0x80);
                uiResult >>= 6;
                
            case 2:
                pcDst[iDindex + 1] = (WE_CHAR) ((uiResult & 0x3f) | 0x80);
                uiResult >>= 6;
                
            case 1:
                pcDst[iDindex] = (WE_CHAR) (uiResult | ucMaskOfUtf8);
            }
        }
        iSindex += 1;
        iDindex += iDesNumOfByte;
    }
    *plSrcLen = iSindex;
    *plDstLen = iDindex;
    
    return TRUE;
}

/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetLatin2ToUtf8
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert Latin2 string to utf8 string. 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input Latin2 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert Latin2 string to utf8 string.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
static WE_INT32 WeCsc_CharsetLatin2ToUtf8 
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG *plDstLen
 )
{
    if(NULL == pcSrc || NULL == plSrcLen ||  NULL == plDstLen || 0 == *plSrcLen || 0 == *plDstLen )
    {
        return E_WECSC_BAD_PRARAM;
    }
    return WeCsc_Iso8859ToUtf8( pcSrc, plSrcLen, pcDst, plDstLen, ausLatin2CodeTable);
}

/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetUtf8ToLatin2 
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert Utf8 string to Latin2 string. 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input Utf8 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert  Utf8 string to Latin2 string.
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
static WE_INT32 WeCsc_CharsetUtf8ToLatin2 
(
     const WE_CHAR *pcSrc, 
     WE_LONG *plSrcLen, 
     WE_CHAR *pcDst, 
     WE_LONG *plDstLen
 )
{
    WE_INT32   iSindex = 0;
    WE_INT32   iDindex = 0;
    WE_INT32   iSouNumOfByte = 0;
    WE_UINT32  uiResult = 0;
    WE_INT32   iHc = 0;
    WE_INT32   i = 0;
    
    if(NULL == pcSrc || NULL == plSrcLen  || NULL == plDstLen|| 0 == *plSrcLen )
    {
        return E_WECSC_BAD_PRARAM;
    }
    
    while (iSindex < *plSrcLen)
    {
        /* Read one Unicode character in UTF-8 format: */
        iSouNumOfByte = aucCharsetUtf8BytesPerCharacter[(WE_UCHAR)*pcSrc >> 4];
        if (iSindex + iSouNumOfByte > *plSrcLen)
            break;
        
        uiResult = 0;
        switch (iSouNumOfByte) 
        {
        case 0:
            return FALSE;
            
        case 4: uiResult += (WE_UCHAR)*pcSrc++; uiResult <<= 6;
        case 3: uiResult += (WE_UCHAR)*pcSrc++; uiResult <<= 6;
        case 2: uiResult += (WE_UCHAR)*pcSrc++; uiResult <<= 6;
        case 1: uiResult += (WE_UCHAR)*pcSrc++;
        }
        uiResult -= auiCharsetOffsetsFromUtf8[iSouNumOfByte];
        
        /* Encode one Unicode character in Latin-2 alphabet: */
        if (uiResult <= 0x7f) 
        {
            iHc = uiResult;
        }
        else 
        {
            iHc = '?';            
            for (i = 0x80; i <= 0xff; i++)
            {
                if (ausLatin2CodeTable[i] == uiResult)
                {
                    iHc = i;
                    break;
                }
            }
        }

        if (pcDst != NULL) 
        {          
           if (iDindex + 1 > *plDstLen)
              break;

            pcDst[iDindex] = (WE_CHAR)(iHc);
        }
        iSindex += iSouNumOfByte;
        iDindex += 1;
    }
    *plSrcLen = iSindex;
    *plDstLen = iDindex;
    
    return TRUE;
}

/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetHebrewToUtf8 
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert Hebrew string to Utf8 string. 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input hebrew string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert Hebrew string to Utf8 string. 
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
static WE_INT32 WeCsc_CharsetHebrewToUtf8 
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG *plDstLen
)
{
    WE_UINT16 ausHebrewCodeTable[128] = {0};
    WE_INT32 i;

    if(NULL == pcSrc || NULL == plSrcLen ||  NULL == plDstLen|| 0 == *plSrcLen  )
    {
        return E_WECSC_BAD_PRARAM;
    } 
    
    WE_MEMSET(ausHebrewCodeTable, 0x00, 64);
    
    WE_MEMSET(ausHebrewCodeTable+62, 0x00, 66);
    
    for(i = 32; i < 63; i++)
    {
       ausHebrewCodeTable[i] = 0x080 + (WE_UINT16)i ;
    }
    
    ausHebrewCodeTable[33] = 0x000;
    ausHebrewCodeTable[58] = 0x0f7;
    
    for(i = 96; i < 128; i++)
    {
        ausHebrewCodeTable[i] = 0x570 + (WE_UINT16)i;
    }
    
    ausHebrewCodeTable[123] = 0x000;
    ausHebrewCodeTable[124] = 0x000;
    ausHebrewCodeTable[125] = 0x200e;
    ausHebrewCodeTable[126] = 0x200f;
    ausHebrewCodeTable[127] = 0x000;
    
   
    return WeCsc_Iso8859ToUtf8 (pcSrc, plSrcLen, pcDst, plDstLen, ausHebrewCodeTable);

}


/*=====================================================================================
FUNCTION: 
    WeCsc_CharsetUtf8ToHebrew 
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert Utf8 string to hebrew string. 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input utf8 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert Hebrew string to Utf8 string. 
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
static WE_INT32 WeCsc_CharsetUtf8ToHebrew 
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG *plDstLen
 )
{
    WE_INT32   iSindex = 0;
    WE_INT32   iDindex = 0;
    WE_INT32   iSouNumOfByte = 0;
    WE_UINT32  uiResult = 0;
    WE_INT32   iHc = 0;
    
    if(NULL == pcSrc || NULL == plSrcLen || NULL == plDstLen|| 0 == *plSrcLen   )
    {
        return E_WECSC_BAD_PRARAM;
    }
    while (iSindex < *plSrcLen) 
    {
        /* Read one Unicode character in UTF-8 format: */
        iSouNumOfByte = aucCharsetUtf8BytesPerCharacter[(WE_UCHAR)*pcSrc >> 4];
        if (iSindex + iSouNumOfByte > *plSrcLen)
            break;
        
        uiResult = 0;
        switch (iSouNumOfByte) 
        {
        case 0:
            return FALSE;
            
        case 4: uiResult += (WE_UCHAR)*pcSrc++; uiResult <<= 6;
        case 3: uiResult += (WE_UCHAR)*pcSrc++; uiResult <<= 6;
        case 2: uiResult += (WE_UCHAR)*pcSrc++; uiResult <<= 6;
        case 1: uiResult += (WE_UCHAR)*pcSrc++;
        }
        uiResult -= auiCharsetOffsetsFromUtf8[iSouNumOfByte];
        
        /* Encode one Unicode character in Hebrew alphabet: */
        if (uiResult == 0xd7) 
        {
            iHc = 0xaa;
        }
        else if (uiResult == 0xf7) 
        {
            iHc = 0xba;
        }
        else if (uiResult == 0x2017) 
        {
            iHc = 0xdf;
        }
        else if (uiResult == 0x200e) 
        {
            iHc = 0xfd;
        }
        else if (uiResult == 0x200f) 
        {
            iHc = 0xfe;
        }
        else if ((uiResult <= 0x7f) || ((uiResult >= 0xa0) && (uiResult <= 0xbe))) 
        {
            iHc = (WE_INT32)uiResult;
        }
        else if ((uiResult >= 0x5d0) && (uiResult <= 0x5ea)) 
        {
            iHc = (uiResult - 0x5d0) + 0xe0;
        }
        else
        {
            iHc = '?';
        }

        if (pcDst != NULL)
        {
           if( iDindex +1 > *plDstLen)
              break;

            pcDst[iDindex] = (WE_CHAR)(iHc);
        }
        iSindex += iSouNumOfByte;
        iDindex += 1;
    }
    *plSrcLen = iSindex;
    *plDstLen = iDindex;
    
    return TRUE;
}

/*=====================================================================================
FUNCTION: 
    Big5CharToUnicode 
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert a Big5 encoded string into unicode string 
ARGUMENTS PASSED:
    pMe: the csc instance 
    pucSrc: the source string is coded in Big5, it must be ended with 0x00.
    pusDes: The buffer to contain unicode string. the size must be large enough to contain 
RETURN VALUE:
    Returns length of the string and the output unicode string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert Big5 encoded string into unicode string 
IMPORTANT NOTES:
    None    
=====================================================================================*/
static WE_UINT16  Big5CharToUnicode
( 
 WE_UINT8 *     pucSrc,
 WE_UINT16 *    pusDes
)                          
{
   WE_UINT16  usLen = 0;
   WE_UINT16  usSrcWord = 0;
   WE_UINT8   ucSrcByte = 0;
   WE_UINT16  usUnicode = 0;
   
   
   if(NULL == pucSrc  )
   {
      return FALSE;
   }
   while(0x00 != (ucSrcByte = *pucSrc++) )
   {    /*ANSI Char?*/
      if(ucSrcByte <= 0x7F)     
      {  /*just convert to 16bit.*/
         *pusDes++ = (WE_UINT16) ucSrcByte; 
         usLen ++;            
      }
      else
      {
         usSrcWord = (*pucSrc++) | (((WE_UINT16)ucSrcByte)<<8);
         /*Two BYTE to form one WORD
         
           the GB code in table_gb[] and BIG5 code in table_big5[]
           is high-low byte reversed, that is, the 'qu' is high byte
           and 'we' is low byte.
           while Chinese encoded in computer are alwarys with 'qu' at 
         low byte and 'wei' at high byte.*/
         
         /*look into BIG5 table*/
         
         usUnicode = UnicodeFromBIG5Table( usSrcWord);           
         
         *pusDes++ = usUnicode;                
         
         usLen  ++;
      }
   }
   
   *pusDes = 0x0000;           
   return usLen;
}

/*=====================================================================================
FUNCTION: 
    UnicodeFromGBTable
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert a unicode encoded char into Gb2312 char 
ARGUMENTS PASSED:
    pMe: the csc instance 
    pucSrc: the source char is coded in Unicode.
RETURN VALUE:
    Returns Gb2312 encode char.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert a unicode encoded char into Gb2312 char  
IMPORTANT NOTES:
    None    
=====================================================================================*/
static WE_UINT16  UnicodeFromGBTable
(
    WE_UINT16  usSrcWord
)
{
    WE_UINT16 usUnicode = 0;

    WE_UINT8  ucHiSrc = 0;
    WE_UINT8  ucLoSrc = 0;
    
    WE_UINT16  usSegOffset, usOffset;
    WE_UINT8   ucTemp1 ;
    WE_UINT8   ucTemp2 ;
    
    WE_UINT32  uiTemp = 0;
    WE_INT32   iFileHandle = 0;
    WE_INT32   iRes = 0;
    WE_INT32   iReadNum = 0;
 

    ucTemp1 = 41; /*0x29*/
    ucTemp2 = 48; /*0x30*/
   

    ucHiSrc = (WE_UINT8)(usSrcWord >> 8);
    ucLoSrc = (WE_UINT8)usSrcWord;


    if(ucHiSrc <= ucTemp1)
    {
        usSegOffset = (WE_UINT16)((ucHiSrc - 0x21) * 94);
        usOffset = usSegOffset + ((WE_UINT16)(ucLoSrc - (WE_UINT8)0x21)& 0xff);
    }
    else if(ucHiSrc >= ucTemp2)
    {
        usSegOffset = (WE_UINT16)((ucHiSrc - 0x30 + 0x08 + 1) * 0x5E);
        usOffset = usSegOffset + ((WE_UINT16)(ucLoSrc - (WE_UINT8)0x21)& 0xff);
    }
    
    else
    { 
        return 0xffff;
    }
    iFileHandle = open( CSC_PATH_GB_TO_UCS, O_RDONLY );
    if(-1 == iFileHandle)
    {
        return FALSE ;
    }
    iRes = lseek(iFileHandle,usOffset * 4,SEEK_SET);

    iReadNum = read(iFileHandle,&uiTemp,4);
    if(-1 == iReadNum)
    {
        return FALSE;
    }
    
    usUnicode = (WE_UINT16)(uiTemp >> 16);
    return usUnicode;

}

/*=====================================================================================
FUNCTION: 
    UnicodeFromBIG5Table
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert a unicode encoded char into Big5 char 
ARGUMENTS PASSED:
    pMe: the csc instance 
    pucSrc: the source char is coded in Unicode.
RETURN VALUE:
    Returns Gb2312 encode char.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert a unicode encoded char into Big5 char  
IMPORTANT NOTES:
    None    
=====================================================================================*/
static WE_UINT16  UnicodeFromBIG5Table
(
    WE_UINT16  usSrcWord
 )
{

    WE_UINT16 usUnicode = 0;
    WE_UINT8  ucHiSrc = 0;
    WE_UINT8  ucLoSrc = 0;

    WE_UINT16  usSegOffset , usOffset = 0;
    WE_UINT32 uiTemp;
    WE_INT32 iFileHandle = 0;
    WE_INT32 iRes = 0;
    WE_INT32 iReadNum = 0;


    ucHiSrc = (WE_UINT8)(usSrcWord >> 8);
    ucLoSrc = (WE_UINT8)usSrcWord;

    if(ucHiSrc < 0xA1)
    {
        return 0xffff;
    }

    usSegOffset = (WE_UINT16)((WE_UINT8)(ucHiSrc - 0xA1) * 0x9d);
    if(ucLoSrc >= (WE_UINT8)0x40 && ucLoSrc <= (WE_UINT8)0x7E)
    {
        usOffset = usSegOffset + ((WE_UINT16)(ucLoSrc - (WE_UINT8)0x40)& 0xff);
    }
    if(ucLoSrc >=(WE_UINT8)0xA1 && ucLoSrc <= (WE_UINT8)0XFE)
    {
        usOffset = usSegOffset + 0x3F + ((WE_UINT16)(ucLoSrc - (WE_UINT8)0xA1)& 0xff);
    }
    

    iFileHandle = open( CSC_PATH_BIG5_TO_UCS, O_RDONLY );
    if(-1 == iFileHandle)
    {
        return FALSE ;
    }
    iRes = lseek(iFileHandle,usOffset * 4,SEEK_SET);
    
    iReadNum = read(iFileHandle,&uiTemp,4);
    if(-1 == iReadNum)
    {
        return FALSE;
    }
    
    usUnicode = (WE_UINT16)(uiTemp >> 16);
    return usUnicode;

}

/*=====================================================================================
FUNCTION: 
    UnicodeToGbChar
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert a unicode encoded string into Gb2312 string 
ARGUMENTS PASSED:
    pMe: the csc instance 
    pucSrc: the source string is coded in Unicode.
    pucDes: the output string which is coded in Gb2312   
RETURN VALUE:
    Returns Gb2312 encode char.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert a unicode encoded string into Gb2312 string 
IMPORTANT NOTES:
    None    
=====================================================================================*/
static WE_UINT16 UnicodeToGbChar
(
 WE_UINT16 *    pusSrc,
 WE_UINT8 *     pucDes,
 WE_INT32*      plDstLen
 )
{
    WE_UINT16  usLength = 0;
    
    /*if parameters valid?*/
    if(pusSrc == NULL)
    {      
        return FALSE;
    }
    
    /*get the length of unicode string (char count).*/
    usLength = UnicodeStringLen(pusSrc);

    if(0 == usLength )
    {
        return FALSE;
    } 
    
    if(FALSE == UnicodeToGB( pusSrc, usLength, pucDes, plDstLen))
    {
        return FALSE;
    }
    
        
    /*get the length*/
 //   usLength = WE_STRLEN((WE_CHAR*)pucDes);

    return TRUE;
}

/*=====================================================================================
FUNCTION: 
    UnicodeToBig5Char
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert a unicode encoded string into Big5 string 
ARGUMENTS PASSED:
    pMe: the csc instance 
    pucSrc: the source string is coded in Unicode.
    pucDes: the output string which is coded in Big5   
RETURN VALUE:
    Returns Gb2312 encode char.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert a unicode encoded string into Big5 string 
IMPORTANT NOTES:
    None    
=====================================================================================*/
static WE_UINT16 UnicodeToBig5Char
(
 WE_UINT16 *    pusSrc,
 WE_UINT8 *     pucDes,
 WE_INT32*      plDstLen
 )
{
    WE_UINT16  usLength = 0;
    
    /*if parameters valid?*/
    if(pusSrc == NULL)
    {      
        return FALSE;
    }    
    
    /*get the length of unicode string (char count).*/
    usLength = UnicodeStringLen(pusSrc);
    
    if( FALSE == UnicodeToBIG5(pusSrc, usLength, pucDes, plDstLen))
    {
        return FALSE;
    }
    
    return TRUE;
}

/*=====================================================================================
FUNCTION: 
    UnicodeStringLen
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    this function to calculat the length of a string
ARGUMENTS PASSED:
    pusString: the input string
RETURN VALUE:
    Returns the length of string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need to get the length of string 
IMPORTANT NOTES:
    None    
=====================================================================================*/
static WE_UINT16  UnicodeStringLen(WE_UINT16 * pusString)
{
    WE_UINT16  usCount = 0;
    
    if(NULL == pusString)
    {
        return FALSE;
    }
    while(*pusString !=0x0000)
    {
        usCount ++;
        pusString ++;
    }
    return usCount;
}
/*=====================================================================================
FUNCTION: 
    UnicodeToGB
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert a unicode encoded string into GB2312 string 
ARGUMENTS PASSED:
    pMe: the csc instance 
    pucSrc: the source string is coded in Unicode.
    usSrcLen: the length of the source string.
    pucDes: the output string which is coded in GB2312   
RETURN VALUE:
    Returns the converting result ,and Gb2312 encode char.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert a unicode encoded string into Big5 string 
IMPORTANT NOTES:
    None    
=====================================================================================*/
static WE_UINT16  UnicodeToGB
(
    WE_UINT16 *pusSrc, 
    WE_UINT16 usSrcLen, 
    WE_UINT8  *pucDes,
    WE_INT32 *plDstLen
)
{
   /*if error occur, use 0xA1F5/0xa1bc for mismatched char*/

   WE_UINT16  usUnicode = 0;
   WE_UINT16  usMultiCode = 0;
   WE_INT32   iDindex = 0;
   
   if(NULL == pusSrc )
   {
      return FALSE;
   }
   
   while(usSrcLen--)
   {        
      usUnicode = *pusSrc ++;
      
      /*ANSI char, low byte = 0*/
      if((usUnicode & 0xff00) == 0)	
      {   
         /*discard the high byte*/ 
         if(NULL != pucDes )
         {
            if (iDindex + 1 > *plDstLen)
               break;
            *pucDes++ = (WE_UINT8)(usUnicode);                
         }
         iDindex += 1;
         continue;
      }
      
      /*get the BIG5/GB code according to unicode*/
      usMultiCode = LookGBTable(usUnicode);
      
      /*there is no corresponding GB/BIG5 chars found in table,*/
      if(usMultiCode == 0xffff)      
      {   
         /*use the default 'error' char*/
         usMultiCode = GB_ERR_CHAR;                  
      }      
      
      if( NULL != pucDes)
      {
         if (iDindex + 2 > *plDstLen)
            break;

         *(WE_UINT16*)pucDes = usMultiCode;
         pucDes += 2;
      }
      iDindex += 2;
   }
   if( NULL != pucDes)
   {
      *pucDes = 0x00;                          
   }
   
   *plDstLen = iDindex;     //to be change
   
   return TRUE;
}

/*=====================================================================================
FUNCTION: 
    UnicodeToBig5
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert a unicode encoded string into Big5 string 
ARGUMENTS PASSED:
    pMe: the csc instance 
    pucSrc: the source string is coded in Unicode.
    usSrcLen: the length of the source string.
    pucDes: the output string which is coded in Big5   
RETURN VALUE:
    Returns the converting result ,and Big5 encode char.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert a unicode encoded string into Big5 string 
IMPORTANT NOTES:
    None    
=====================================================================================*/
static WE_UINT16  UnicodeToBIG5
(
    WE_UINT16 *pusSrc, 
    WE_UINT16 usSrcLen, 
    WE_UINT8  *pucDes, 
    WE_INT32* plDstLen
)
/*if error occur, use 0xA1F5/0xa1bc for mismatched char*/
{
    WE_UINT16  usUnicode = 0;
    WE_UINT16  usMultiCode = 0;
    WE_INT32   iDindex = 0;

    if(NULL == pusSrc )
    {
        return FALSE;
    }
    
    while(usSrcLen--)
    {        
        usUnicode = *pusSrc ++;
       
        /*ANSI char, low byte = 0?*/
        if((usUnicode &0xff00) == 0)	
        {  /*discard the high byte */     
 
            if( NULL != pucDes)
            {      
               if (iDindex + 1 > *plDstLen)
                  break;   

                *pucDes++ = (WE_UINT8)(usUnicode);
            }
            iDindex += 1;
            continue;
        }
        else
        {     
           
        }
        /*get the BIG5/GB code according to unicode*/
        usMultiCode = LookBIG5Table( usUnicode);
        /*there is no corresponding GB/BIG5 chars found in table,*/  
        if(usMultiCode == 0xffff)     
        {  /*use the default 'error' char*/
            usMultiCode = BIG5_ERR_CHAR;
                        
        }
        if( NULL != pucDes)
        {
           if (iDindex + 2 > *plDstLen)
              break; 

            *(WE_UINT16*)pucDes = usMultiCode;
            pucDes += 2;
        }
        iDindex += 2;
    }
    if(NULL != pucDes)
    {
        *pucDes = 0x00;                           
    }
     
    *plDstLen = iDindex;
    return TRUE;
}

/*=====================================================================================
FUNCTION: 
    LookGBTable
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert a unicode encoded char into GB char 
ARGUMENTS PASSED:
    pMe: the csc instance 
    pucSrc: the source char is coded in Unicode.
RETURN VALUE:
    Returns Gb2312 encode char.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert a unicode encoded char into Gb char  
IMPORTANT NOTES:
    None    
=====================================================================================*/

static WE_UINT16 LookGBTable
(
 WE_UINT16 usSrcWord
)
{
    WE_UINT16 usGb = 0;
    WE_INT32  m = 0;
    WE_INT32  j = 0;
    WE_UINT16  usTemp2 = 0 ;
    WE_UINT8  ucHiSrc = 0;
    WE_UINT8  ucLoSrc =0;
    WE_UINT16 usSegOffset = 0, usOffset = 0;
    WE_UINT16 *pusTemp = NULL;
    WE_INT32 iTemp = 0;
    St_QueryInfo astUcsToGb[17] = {0};
    WE_INT32 iFileHandle = 0;
    WE_INT32 iRes = 0;
    WE_INT32 iReadNum = 0;
    
    iFileHandle = open( CSC_PATH_UCS_TO_GB, O_RDONLY );
    if(-1 == iFileHandle)
    {
        return FALSE ;
    }
    iReadNum = read(iFileHandle,astUcsToGb,17 * sizeof(St_QueryInfo));
    if(-1 == iReadNum)
    {
        return FALSE;
    }

    pusTemp = (WE_UINT16*)WE_MALLOC(sizeof(WE_UINT16) * 29);

    if(NULL == pusTemp)
    {
        close(iFileHandle);
        return FALSE;
    }

    ucHiSrc = (WE_INT8)(usSrcWord >> 8);
    ucLoSrc = (WE_INT8)usSrcWord;
    
    for(m = 0; m < 17; m++)
    {
            if(usSrcWord <= astUcsToGb[m].usMaxCode && usSrcWord >= astUcsToGb[m].usMinCode)
            break;
    }

    if( m==17)
    {
        return 0xffff;
    }
    
    if( m==15)
    {
        usSegOffset = 0x100 * (ucHiSrc - 0x4E);
        usOffset = (usSegOffset + ((WE_UINT16)ucLoSrc & 0xff)) * 2 + astUcsToGb[m].usOffset;
    }
    else if( m==3 || m== 4 || m==8 || m==9 || m==10 || m==12 || m==13 || m==14 || m==16)
    {
        usOffset = ((WE_UINT16)(ucLoSrc - (WE_UINT8)astUcsToGb[m].usMinCode)& 0xff) * 2 + astUcsToGb[m].usOffset;
            
    }
    else 
    {

        iRes = lseek(iFileHandle,astUcsToGb[m].usOffset * 2,SEEK_SET);
        iReadNum = read(iFileHandle,pusTemp,astUcsToGb[m].iCount * 4);
        if(-1 == iReadNum)
        {
            close(iFileHandle);
            return FALSE;
        }
       
        for(j = 0 ; j < astUcsToGb[m].iCount * 2 ; j += 2)
        {
            if(usSrcWord == pusTemp[j])
            {
                usGb = pusTemp[j+1];
                usTemp2 = usGb + 0x8080;
                usGb = (usTemp2 << 8) | (usTemp2 >>8);
                WE_FREE(pusTemp);
                return usGb;
            }
        }
        WE_FREE(pusTemp);        
        return 0xffff;
    }
    
    iRes = lseek(iFileHandle,usOffset * 2,SEEK_SET);
    iReadNum = read(iFileHandle,&iTemp, 4);
    if(-1 == iReadNum)
    {
        close(iFileHandle);
        return FALSE;
    }    
    
    if(iTemp == 0xffff)
    {        
        WE_FREE(pusTemp);    
        return 0xffff;  
    }
    
    usGb = (WE_UINT16)(iTemp >> 16);
    usTemp2 = usGb + 0x8080;
    usGb = (usTemp2 << 8) | (usTemp2 >>8);
    WE_FREE(pusTemp);
    return usGb;
      
}
/*=====================================================================================
FUNCTION: 
    LookBig5Table
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert a unicode encoded char into Big5 char 
ARGUMENTS PASSED:
    pMe: the csc instance 
    pucSrc: the source char is coded in Unicode.
RETURN VALUE:
    Returns Gb2312 encode char.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert a unicode encoded char into Big5 char  
IMPORTANT NOTES:
    None    
=====================================================================================*/
static WE_UINT16 LookBIG5Table
(
     WE_UINT16 usSrcWord
 )
{
    WE_UINT8  ucHiSrc;
    WE_UINT8  ucLoSrc;

    WE_UINT16 usBig5 = 0;
    WE_INT32  iTemp = 0;
    WE_INT32  i = 0;
    WE_INT32  j = 0;
    WE_UINT16 usSegOffset = 0, usOffset = 0;
    WE_UINT16 *pusTemp;
    St_QueryInfo astUcsToBig5[19] = {0};
    WE_INT32 iFileHandle = 0;

    WE_INT32 iRes = 0;
    WE_INT32 iReadNum = 0;
    
    iFileHandle = open( CSC_PATH_UCS_TO_BIG5, O_RDONLY );
    if(-1 == iFileHandle)
    {
        return FALSE ;
    }
    iReadNum = read(iFileHandle,astUcsToBig5,19 * sizeof(St_QueryInfo));
    if(-1 == iReadNum)
    {
        return FALSE;
    }
    

    pusTemp = (WE_UINT16*)WE_MALLOC( 2 * 56 * sizeof(WE_UINT16));


    ucHiSrc = (WE_INT8)(usSrcWord >> 8);
    ucLoSrc = (WE_INT8)usSrcWord;


    for(i = 0; i < 19; i++)
    {
        if(usSrcWord >= astUcsToBig5[i].usMinCode && usSrcWord <= astUcsToBig5[i].usMaxCode)
            break;
       
    }
    if(19 == i)
    {
        return 0xffff;
    }

    if( 14 == i)
    {        
        usSegOffset = 0x100 * (ucHiSrc - 0x4E);
        usOffset = (usSegOffset + ((WE_UINT16)ucLoSrc & 0xff)) * 2 + astUcsToBig5[i].usOffset;
    }

    /*can read the code indirectly*/
    else if( 10 == i || ( i >= 15 && i <= 18))
    {
        usOffset = ((WE_UINT16)(ucLoSrc - (WE_UINT8)astUcsToBig5[i].usMinCode)& 0xff) * 2 + astUcsToBig5[i].usOffset;
    }
    
    /*when need to query in order */
    else
    {
        iRes = lseek(iFileHandle,astUcsToBig5[i].usOffset* 2,SEEK_SET);
        iReadNum = read(iFileHandle,pusTemp,astUcsToBig5[i].iCount * 4);
        if(-1 == iReadNum)
        {
            close(iFileHandle);
            return FALSE;
        }

        for(j = 0 ; j < astUcsToBig5[i].iCount * 2 ; j += 2)
        {
            if(usSrcWord == pusTemp[j])
            {
                usBig5 = pusTemp[j+1];
                usBig5 = (usBig5 << 8) | (usBig5>>8);
                WE_FREE(pusTemp);
                return usBig5;
            }
        }
        WE_FREE(pusTemp);        
        return 0xffff;
    }
    iRes = lseek(iFileHandle,usOffset* 2,SEEK_SET);
    iReadNum = read(iFileHandle, &iTemp, 4);
    if(-1 == iReadNum)
    {
        close(iFileHandle);
        return FALSE;
    }
    
    if(iTemp == 0xffff)
    {        
        WE_FREE(pusTemp);    
        return 0xffff;  
    }
    
    usBig5 = (WE_UINT16)(iTemp >> 16);
    
    usBig5 =  (usBig5 << 8) | (usBig5>>8);     
    WE_FREE(pusTemp);
    return usBig5;
 
}

/*=====================================================================================
FUNCTION: 
    UnicodeToUtf8
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert unicode string to utf8 string. 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input unicode string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert unicode string to Utf8 string. 
IMPORTANT NOTES:
    None    
=====================================================================================*/
static WE_INT32 UnicodeToUtf8 
(
    const WE_CHAR *pcSrc, 
    WE_INT32 *piSrcLen,
    WE_CHAR *pcDst, 
    WE_INT32 *piDstLen
)
{
    WE_UINT32   uiResult = 0;
    WE_INT32    iSindex = 0;
    WE_INT32    iDindex = 0;
    WE_INT32    iSouNumOfByte = 0;
    WE_INT32    iDesNumOfByte = 0;
    WE_UINT8    uiMask = 0;
    WE_INT32    iShiftFirst = 0;
    WE_INT32    iShiftSecond = 0;

    if(NULL == pcSrc || NULL == piSrcLen  || NULL == piDstLen )
    {
        return E_WECSC_BAD_PRARAM;
    }
    iShiftFirst = 0; 
    iShiftSecond = 1; 
    
    while(1)
    {
        /* First, translate to UCS4 */
        if (iSindex + 2 > *piSrcLen)
            break;
        uiResult = ((WE_UCHAR)pcSrc[iSindex + iShiftSecond] << 8) |
            (WE_UCHAR)pcSrc[iSindex + iShiftFirst];
        iSouNumOfByte = 2;
        
        /* Determine how many UTF8-bytes are needed for this character,
        * and which bits should be set in the first byte. */
        if (uiResult < 0x80) 
        {
            iDesNumOfByte = 1;
            uiMask = 0;
        }
        else if (uiResult < 0x800) 
        {
            iDesNumOfByte = 2;
            uiMask = 0xc0;
        }
        else 
        {
            iDesNumOfByte = 3;
            uiMask = 0xe0;
        } 
        
        /* Only write to destination vector if pcDst != OP_NULL */
        if (pcDst != NULL) 
        {
           /* Is there room for this in the destination vector */ 
           if (iDindex + iDesNumOfByte > *piDstLen)
              break;

            switch (iDesNumOfByte) 
            {
            case 3:
                pcDst[iDindex + 2] = (WE_CHAR) ((uiResult & 0x3f) | 0x80);
                uiResult >>= 6;
                /* falltrough */
            case 2:
                pcDst[iDindex + 1] = (WE_CHAR) ((uiResult & 0x3f) | 0x80);
                uiResult >>= 6;
                /* falltrough */
            case 1:
                pcDst[iDindex] = (WE_CHAR) (uiResult | uiMask);
                break;
            default:
                break;
            }
        }
        
        iSindex += iSouNumOfByte;
        iDindex += iDesNumOfByte;
    }
    *piSrcLen = iSindex;
    *piDstLen = iDindex;
    
    return TRUE;
}


/*=====================================================================================
FUNCTION: 
    Utf8ToUnicode
CREATE DATE: 
    2006-06-02
AUTHOR: 
    Song Yi
DESCRIPTION:
    Convert Utf8 string to unicode string. 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input utf8 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Convert utf8 string to unicode string. 
IMPORTANT NOTES:
    None    
=====================================================================================*/
static WE_INT32 Utf8ToUnicode 
(
    const WE_CHAR *pcSrc, 
    WE_INT32 *piSrcLen,
    WE_CHAR *pcDst, 
    WE_INT32 *piDstLen
)
{
    WE_INT32     iSindex = 0;
    WE_INT32     iDindex = 0;
    WE_INT32     iSouNumOfByte = 0; 
    WE_INT32     iDesNumOfByte = 0;
    WE_UINT32    uiResult = 0;
    WE_INT32     iShiftFirst = 0;
    WE_INT32     iShiftSecond = 0;

    if(NULL == pcSrc || NULL == piSrcLen ||  NULL == piDstLen )
    {
        return E_WECSC_BAD_PRARAM;
    }
    
    iShiftFirst = 0;
    iShiftSecond = 1; 
    
    while (iSindex < *piSrcLen) 
    {
        /* Read one Unicode character in UTF-8 format: */
        iSouNumOfByte = aucCharsetUtf8BytesPerCharacter[(WE_UCHAR)*pcSrc >> 4];
        if (iSindex + iSouNumOfByte > *piSrcLen)
            break;
        
        uiResult = 0;
        switch (iSouNumOfByte) 
        {
        case 0:
            return -1;
            
        case 4: uiResult += (WE_UCHAR)*pcSrc++; uiResult <<= 6;
        case 3: uiResult += (WE_UCHAR)*pcSrc++; uiResult <<= 6;
        case 2: uiResult += (WE_UCHAR)*pcSrc++; uiResult <<= 6;
        case 1: uiResult += (WE_UCHAR)*pcSrc++;
        }
        uiResult -=auiCharsetOffsetsFromUtf8[iSouNumOfByte];
        
        /* Write one Unicode character: */
        uiResult &= 0xffff;

        if (pcDst != NULL) 
        {
           if (iDindex + 2> *piDstLen)
              break;

            pcDst[iDindex + iShiftFirst] = (WE_CHAR)((uiResult & 0xff));
            pcDst[iDindex + iShiftSecond] = (WE_CHAR)((uiResult >> 8) & 0xff);
        }
        iDesNumOfByte = 2;
        iSindex += iSouNumOfByte;
        iDindex += iDesNumOfByte;
    }
    *piSrcLen = iSindex;
    *piDstLen = iDindex;
    
    return TRUE;
}

/*=====================================================================================
FUNCTION: 
    WeCsc_CharSetUtf8ToUtf8 
CREATE DATE: 
    2006-07-11
AUTHOR: 
    Song Yi
DESCRIPTION:
    Filters out all incorrect utf-8 sequences of an utf-8 stream. 
ARGUMENTS PASSED:
    pMe: the csc instance 
    *pcSrc : input utf8 string location
    *plSrcLen: input string's length  
    *pcDst : the output string location 
    *plDstLen: the output string's length
RETURN VALUE:
    Returns the result of converting, the converted string and the length of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need Filters out all incorrect utf-8 sequences of an utf-8 stream.. 
IMPORTANT NOTES:
    if the output length is more larger and the output is NULL,it will output the length actually  need   
=====================================================================================*/
static WE_INT32 WeCsc_CharSetUtf8ToUtf8
(
    const WE_CHAR *pcSrc, 
    WE_LONG *plSrcLen, 
    WE_CHAR *pcDst, 
    WE_LONG *plDstLen
)
{
   WE_INT32    iSindex = 0, iDindex = 0;
    WE_INT32    iSnum;
    WE_INT32    iCharOk = FALSE;
    WE_UINT32   uiResult = 0;
    
    if(NULL == pcSrc || NULL == plSrcLen  || NULL == plDstLen )
    {
        return E_WECSC_BAD_PRARAM;
    }
    while (iSindex < *plSrcLen) 
    {
        /* Read one Unicode character in UTF-8 format: */
        iSnum = aucCharsetUtf8BytesPerCharacter[(WE_UCHAR)*pcSrc >> 4];
        
        if (iSindex + iSnum > *plSrcLen || iDindex + iSnum > *plDstLen || (iSnum == 0 && iDindex + 3 >= *plDstLen))
            break;
        
        /* Convert to UCS2 */
        if(iSnum > 1)
        {
            uiResult = 0;
            switch (iSnum) 
            {
                case 4: uiResult += (WE_UCHAR)*pcSrc++; uiResult <<= 6;
                case 3: uiResult += (WE_UCHAR)*pcSrc++; uiResult <<= 6;
                case 2: uiResult += (WE_UCHAR)*pcSrc++; uiResult <<= 6;
                case 1: uiResult += (WE_UCHAR)*pcSrc++;
            }
            
            uiResult = uiResult - auiCharsetOffsetsFromUtf8[iSnum];
            pcSrc -= iSnum;
        }
        switch(iSnum)
        {
        case 0:
            iCharOk = FALSE;
            break;
        case 1:
            iCharOk = TRUE;
            break;
        case 2:
            iCharOk = uiResult >= 0x80UL && (*(pcSrc+1) & 0xc0) == 0x80;
            break;
        case 3:
            iCharOk = uiResult >= 0x800UL && ((*(pcSrc+1) & 0xc0) == 0x80) && ((*(pcSrc+2) & 0xc0) == 0x80);
            break;
        case 4:
            iCharOk = uiResult >= 0x10000UL && !(*pcSrc & 0x08) && ((*(pcSrc+1) & 0xc0) == 0x80) && ((*(pcSrc+2) & 0xc0) == 0x80) && ((*(pcSrc+3) & 0xc0) == 0x80);
            break;
        }
        
        
        if(iCharOk)
        { /* Valid character. Copy the bytes */
            if (pcDst != NULL)
            {
                switch(iSnum)
                {
                    case 4: *pcDst++ = *pcSrc++; 
                    case 3: *pcDst++ = *pcSrc++;
                    case 2: *pcDst++ = *pcSrc++;
                    case 1: *pcDst++ = *pcSrc++;
                }
            }
            else
            {
                pcSrc += iSnum;
            }
            iSindex += iSnum;
            iDindex += iSnum;
        }
        else
        { /* Invalid character. Replace with WE_CHARSET_REPLACEMENT_CHAR */ 
            if(iDindex + 3 < *plDstLen)
            {
                WE_INT32  iLen = WeCsc_CharsetUnicodeToUtf8( WE_CSC_REPLACEMENT_CHAR ,pcDst);

                if (pcDst != NULL)
                {
                    if(iLen >0)
                    {
                        pcDst += iLen;
                        iDindex += iLen;
                    }
                    
                }
                else
                {
                     iDindex += iLen;
                }
            }
            pcSrc++;
            iSindex++;
            /* If this byte was a valid start byte but the character was invalid scan for the next start byte. */
            if(iSnum != 0)
            {
                while(aucCharsetUtf8BytesPerCharacter[(WE_UCHAR)*pcSrc >> 4] == 0 && iSindex < *plSrcLen)
                {
                    pcSrc++;
                    iSindex++;
                }
            }
        }
    }
    *plSrcLen = iSindex;
    *plDstLen = iDindex;
    
    return TRUE;
}

