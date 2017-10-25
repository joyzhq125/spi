/*=====================================================================================

    HEADER NAME : we_csc.h
    MODULE NAME : WE Framework CSC(Character Set Convertor)

    PRE-INCLUDE FILES DESCRIPTION

    GENERAL DESCRIPTION
        This file can be included by all WE Programmer.In this file all csc interfaces and 
    related macro is defined.
        These Info is  aptotic and is not changed by switching Project.

    TECHFAITH Wireless Confidential Proprietary
    (c) Copyright 2006 by TechSoft. All Rights Reserved.
=======================================================================================
    Revision History
   
    Modification                 Tracking
    Date           Author         Number        Description of changes
    ----------   ------------    ---------   --------------------------------------------
    2007-03-09    Tang                      Add  a  macro NO_ENOUGH_MEMERY 

    Self-documenting Code
    Describe/explain low-level design, especially things in header files, of this module and/or group 
    of funtions and/or specific funtion that are hard to understand by reading code and thus requires 
    detail description.
    Free format !

=====================================================================================*/

/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/
#ifndef WE_CSC_H
#define WE_CSC_H

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/

#define WE_CSC_UNKNOWN       0
#define WE_CSC_ASCII         3
#define WE_CSC_ISO_8859_1    4
#define WE_CSC_LATIN_1       4
#define WE_CSC_ISO_8859_2    5
#define WE_CSC_LATIN_2       5
#define WE_CSC_ISO_8859_8    11
#define WE_CSC_HEBREW        11
#define WE_CSC_UCS_2         1000
#define WE_CSC_UCS_2BE       10001
#define WE_CSC_UCS_2LE       10002
#define WE_CSC_UTF_16BE      1013
#define WE_CSC_UTF_16LE      1014
#define WE_CSC_UTF_16        1015
#define WE_CSC_UTF_8         106
#define WE_CSC_BIG5          2026  
#define WE_CSC_GB2312        2025  

/*******************************************************************************
*   Type Define Section
*******************************************************************************/


/*Define function type start*/
typedef WE_INT32 Fn_We_Charset_Convert
(
    const WE_CHAR *pcSrc, /* The source buffer. */
    WE_LONG *plSrcLen,    /* On input, the number of bytes
                         * available in the source buffer;
                         * on output, the number of source
                         * bytes consumed. */
    WE_CHAR *pcDst,       /* The destination buffer. */
    WE_LONG *plDstLen     /* On input, the amount of free space
                         * in the destination buffer;
                         * on output, the number of bytes
                         * placed in the output buffer. */
);

/*Define function type end*/

/*Define struct &  union type start*/
typedef struct _WeCscXchrEntry
{
    WE_INT32               iCharacterSet;
    const WE_CHAR          *pcPreferredName;
    const WE_CHAR          *pcAliases;
    Fn_We_Charset_Convert* fnCvtToUtf8;  
    Fn_We_Charset_Convert* fnCvtFromUtf8;  
} St_WeCscXchrEntry;
/*Define struct &  union type end*/


/*Define Enum type start*/
typedef enum _WECSCERR
{
    E_WECSC_BAD_PRARAM =  -1,
    E_WECSC_NO_ENOUGH_MEMERY = -2,
    E_WECSC_NO_FILE = -3
}E_WECSCERR;
/*Define Enum type end*/



/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/



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
#define WE_FINDFUNCTIONOFCSC   We_FindFunctionOfCSC
#define We_FindFunctionOfCSC   ICSC_FINDFUNCTIONOFCSC
/*Fn_We_Charset_Convert * We_FindFunctionOfCSC
(
    ICSC * pICsc,
    WE_INT32 iFromCharset, 
    WE_INT32 iToCharset
);
*/
#define ICSC_FINDFUNCTIONOFCSC(pMe, iFromCharset, iToCharset)    WeCsc_FindFunctionOfCSC(iFromCharset,iToCharset)
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
    (that is, UCS-2 or UTF-16) then this function will attempt to guess
    the byte order by checking if either of the two first bytes is zero.
    Returns NULL if no matching conversion function could be found.
=====================================================================================*/
#define WE_FINDFUNCTIONOFCSCBYBOM    We_FindFunctionOfCSCByBOM

#define We_FindFunctionOfCSCByBOM     ICSC_FINDFUNCTIONOFCSCBYBOM

#define ICSC_FINDFUNCTIONOFCSCBYBOM(pMe, iFromCharset, iToCharset, pcStr, piLen)  WeCsc_FindFunctionOfCSCByBOM(iFromCharset, iToCharset, pcStr, piLen)
                                   


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
#define WE_CSC_UTF8TOUNICODE       WE_CscUtf8ToUnicode

#define WE_CscUtf8ToUnicode     ICSC_UTF8TOUNICODE

#define ICSC_UTF8TOUNICODE(pMe, pChar, puiUnicode)  WeCsc_CharsetUtf8ToUnicode(pChar, puiUnicode)

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
#define WE_CSC_UNICODETOUTF8       WE_CscUnicodeToUtf8

#define WE_CscUnicodeToUtf8     ICSC_UNICODETOUTF8

#define ICSC_UNICODETOUTF8(pMe, uiUnicode, pStrSource)  WeCsc_CharsetUnicodeToUtf8(uiUnicode,pStrSource)


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


/*Define function type start*/
   WE_UINT16  Big5CharToUnicode( WE_UINT8 *pusSrc,WE_UINT16 *pucDes);

   WE_UINT16 UnicodeToGbChar( WE_UINT16 *pusSrc,WE_UINT8 *pucDes,  WE_INT32 * plDstLen);
   WE_UINT16 UnicodeToBig5Char( WE_UINT16 *pusSrc,WE_UINT8 *pucDes, WE_INT32 * plDstLen);

   Fn_We_Charset_Convert*  WeCsc_FindFunctionOfCSC(WE_INT32 iFromCharset, WE_INT32 iToCharset);
   Fn_We_Charset_Convert *WeCsc_FindFunctionOfCSCByBOM(WE_INT32 iFromCharset,WE_INT32 iToCharset,const WE_CHAR *pcSrc,WE_INT32 *piLen);
   WE_INT32 WeCsc_CharsetUtf8ToUnicode(  WE_CHAR *pChar, WE_UINT32 *puiUnicode);
   WE_INT32 WeCsc_CharsetUnicodeToUtf8( WE_UINT32 uiUnicode, WE_CHAR *pStrSource);
   const WE_CHAR * WeCsc_CharsetInt2str(  WE_INT32 iCharset);
   WE_INT32 WeCsc_CharsetStr2int ( const WE_CHAR *pcName, WE_INT32 iLength);


   WE_INT32 WeCsc_CharsetAsciiToUtf8 ( const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG  *plDstLen);
   WE_INT32 WeCsc_CharsetLatin1ToUtf8 ( const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG  *plDstLen); 
   WE_INT32 WeCsc_CharsetUtf16AnyToUtf8(const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG  *plDstLen, WE_INT32  iBigEndian);
   WE_INT32 WeCsc_CharsetUtf16leToUtf8 ( const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG  *plDstLen);
   WE_INT32 WeCsc_CharsetUtf16beToUtf8 ( const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG  *plDstLen);
   WE_INT32 WeCsc_CharsetUtf16ToUtf8 ( const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG  *plDstLen);
   WE_INT32 WeCsc_CharsetUcs2beToUtf8 (  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG  *plDstLen);
   WE_INT32 WeCsc_CharsetUcs2leToUtf8 (  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG  *plDstLen);
   WE_INT32 WeCsc_CharsetUcs2ToUtf8 (  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG  *plDstLen);
   WE_INT32 WeCsc_CharsetUcs2AnyToUtf8(const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG  *plDstLen,WE_INT32  iBigEndian);
   WE_INT32 WeCsc_CharsetUtf8ToAscii (  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG  *plDstLen);
   WE_INT32 WeCsc_CharsetUtf8ToLatin1(  const WE_CHAR *pcSrcString, WE_LONG *piSrcLen, WE_CHAR *pcDst, WE_LONG *piDstLen);
   WE_INT32 WeCsc_CharsetUtf8ToUtf16any (const WE_CHAR *pcSrc, WE_LONG *plSrcLen,WE_CHAR *pcDst, WE_LONG *plDstLen,WE_INT32 iBigEndian);
   WE_INT32 WeCsc_CharsetUtf8ToUtf16be (  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG *plDstLen);
   WE_INT32 WeCsc_CharsetUtf8ToUtf16le (  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG *plDstLen);
   WE_INT32 WeCsc_CharsetUtf8ToUtf16 (  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG *plDstLen);
   WE_INT32 WeCsc_CharsetUtf8ToUcs2any (const WE_CHAR *pcSrc, WE_LONG *plSrcLen,WE_CHAR *pcDst, WE_LONG *plDstLen,WE_INT32 iBigEndian);
   WE_INT32 WeCsc_CharsetUtf8ToUcs2be (  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG *plDstLen);
   WE_INT32 WeCsc_CharsetUtf8ToUcs2le (  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG *plDstLen);
   WE_INT32 WeCsc_CharsetUtf8ToUcs2 (  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG *plDstLen);
   WE_INT32 WeCsc_CharsetAsciiToUtf16any ( const WE_CHAR *pcSrc, WE_LONG *plSrcLen,WE_CHAR *pcDst, WE_LONG *plDstLen,WE_INT32 iBigEndian);
   WE_INT32 WeCsc_CharsetAsciiToUtf16be(  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG *plDstLen);
   WE_INT32 WeCsc_CharsetAsciiToUtf16le(  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG *plDstLen);
   WE_INT32 WeCsc_CharsetLatin1ToUtf16any ( const WE_CHAR *pcSrc, WE_LONG *plSrcLen,WE_CHAR *pcDst, WE_LONG *plDstLen,WE_INT32 iBigEndian);
   WE_INT32 WeCsc_CharsetLatin1ToUtf16be(  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG *plDstLen);
   WE_INT32 WeCsc_CharsetLatin1ToUtf16le(  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG *plDstLen);
   WE_INT32 WeCsc_CharsetUtf16beToUtf16le(  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG *plDstLen);
   WE_INT32 WeCsc_CharsetBig5ToUtf8(  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG *plDstLen);
   WE_INT32 WeCsc_CharsetUtf8ToBig5(  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG *plDstLen);
   WE_INT32 WeCsc_CharsetGb2312ToUtf8(  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG *plDstLen);
   WE_INT32 WeCsc_CharsetUtf8ToGb2312(  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG *plDstLen);
   WE_INT32 WeCsc_Iso8859ToUtf8 (const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG *plDstLen,  const WE_UINT16 *pusCodeTable);
   WE_INT32 WeCsc_CharsetLatin2ToUtf8 (  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG *plDstLen);
   WE_INT32 WeCsc_CharsetUtf8ToLatin2 (  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG *plDstLen);
   WE_INT32 WeCsc_CharsetHebrewToUtf8 (  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG *plDstLen);
   WE_INT32 WeCsc_CharsetUtf8ToHebrew (  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG *plDstLen);
   WE_INT32 WeCsc_CharSetUtf8ToUtf8(  const WE_CHAR *pcSrc, WE_LONG *plSrcLen, WE_CHAR *pcDst, WE_LONG *plDstLen);
   const WE_CHAR * WeCsc_CharsetInt2str(  WE_INT32 iCharset);
   WE_INT32 WeCsc_CharsetStr2int ( const WE_CHAR *pcName, WE_INT32 iLength);
   WE_UINT16  UnicodeStringLen(WE_UINT16 * pusString);

   WE_UINT16  UnicodeFromGBTable( WE_UINT16  usSrcWord);
   WE_UINT16 UnicodeFromBIG5Table( WE_UINT16  usSrcWord);


   WE_UINT16  UnicodeToGB(  WE_UINT16 *pusSrc,  WE_UINT16 usSrcLen,WE_UINT8  *pucDes, WE_INT32 *plDstLen);
   WE_UINT16  UnicodeToBIG5(  WE_UINT16 *pusSrc,  WE_UINT16 usSrcLen,WE_UINT8  *pucDes, WE_INT32 * plDstLen);

   WE_UINT16 LookGBTable(  WE_UINT16 usSrcWord);
   WE_UINT16 LookBIG5Table(  WE_UINT16 usSrcWord);

   WE_INT32 UnicodeToUtf8 (const WE_CHAR *pcSrc, WE_INT32 *piSrcLen,WE_CHAR *pcDst, WE_INT32 *piDstLen);
   WE_INT32 Utf8ToUnicode ( const WE_CHAR *pcSrc, WE_INT32 *piSrcLen,WE_CHAR *pcDst, WE_INT32 *piDstLen);


#endif/*endif WE_CSC_H*/



































































