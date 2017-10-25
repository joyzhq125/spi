/*=====================================================================================

    FILE NAME : we_scl.c
    MODULE NAME : WE Framework SCL(Similar C Library)

    PRE-INCLUDE FILES DESCRIPTION

    GENERAL DESCRIPTION
        This file can be included by all WE Programmer.In this file all scl interfaces and 
    related macro is defined.
        These Info is  aptotic and is not changed by switching Project.

    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TechSoft. All Rights Reserved.
=======================================================================================
    Revision History
   
    Modification                 Tracking
    Date           Author         Number        Description of changes
    ----------   ------------    ---------   --------------------------------------------
    2006-06-02   Yang Min				       create this file

    Self-documenting Code
    Describe/explain low-level design, especially things in header files, of this module and/or group 
    of functions and/or specific function that are hard to understand by reading code and thus requires 
    detail description.
    Free format !

=====================================================================================*/
#include "we_scl.h"

#ifdef BREW
#include "AEEFile.h"			/*File interface definitions*/
#include "AEEstdlib.h"			/*The AEE library*/
#include "AEEShell.h"           /*Shell interface definitions*/
#endif

#include "we_mem.h"

/*Define function type start*/
static WE_CHAR *WeSCL_CreateAlphabetTable(void);
static WE_CHAR WeSCL_GetByteOfUTF8(WE_CHAR cTempValue);

static WE_INT32 WeSCL_MonthDays(WE_INT32 iNum);
static WE_CHAR *WeSCL_DayNames(WE_INT32 iNum);
static WE_CHAR *WeSCL_MonthNames(WE_INT32 iNum);
static WE_INT32 WeSCL_LeapOneMonthDays(WE_INT32 iNum);
static WE_CHAR WeSCL_GetHexDigits(WE_UCHAR ucNum);
static WE_INT32 WeSCL_OneMonthDays(WE_INT32 iNum);

/*Define function type end*/


/*Define Constant Macro start*/

/*The max and min of the address length*/
#define WE_IP_ADDRESS_MAX_LEN                    15
#define WE_IP_ADDRESS_MIN_LEN                    7

#define WE_PROXY_PORT_MAX_LEN                    5
#define WE_PROXY_PORT_MAX_NUMBER                 65535

/*The error return value*/
#define USER_ERORR                               0xFF
/*Define Constant Macro end*/




/*====================================================================================
FUNCTION: WeSCL_CreateAlphabetTable
              
CREATE DATE: 2006-06-02
AUTHOR: Yang Min
        
DESCRIPTION:
    Create the alphabet table.
              
ARGUMENTS PASSED:
    None.         
RETURN VALUE:
    Return the pointer to the table if success, else return FALSE.   

USED GLOBAL VARIABLES:
    None.
USED STATIC VARIABLES:
    None.
CALL BY:
    WeSCL_Base64Encode function.
IMPORTANT NOTES:
    When calls to WeSCL_Base64Encode function, it will call this function to create the 
    alphabet table. Because the memory allocated in this function is not freed in the 
    function itself, so remember to free the pointer to the table after using the table.       
=======================================================================================*/
static WE_CHAR *WeSCL_CreateAlphabetTable(void)
{
    WE_CHAR     *pcAlphabet     = NULL;
    WE_CHAR     *pcAlphabetTemp = NULL;
    WE_CHAR     cValueUppercase = 'A';
    WE_CHAR     cValueLowercase = 'a';
    WE_CHAR     cValueDigital   = '0';

    /*Allocate memory to the alphabet table pointer*/
    if (NULL == (pcAlphabet = (WE_CHAR *)WE_MALLOC(65*sizeof(WE_CHAR))))
    {
        return FALSE;
    }
    WE_MEMSET(pcAlphabet, 0, 65*sizeof(WE_CHAR));
    pcAlphabetTemp = pcAlphabet;
    /*'A'~'Z' character*/
    while(cValueUppercase <= 'Z')
    {
        *pcAlphabetTemp = cValueUppercase;
        pcAlphabetTemp++;
        cValueUppercase++;
    }
    /*'a'~'z' character*/
    while (cValueLowercase <= 'z')
    {
        *pcAlphabetTemp = cValueLowercase;
        pcAlphabetTemp++;
        cValueLowercase++;
    }
    /*'0'~'9' character*/
    while (cValueDigital <= '9')
    {
        *pcAlphabetTemp = cValueDigital;
        pcAlphabetTemp++;
        cValueDigital++;
    }
    /*'+','/' character*/
    *pcAlphabetTemp = '+';
    pcAlphabetTemp++;
    
    *pcAlphabetTemp = '/';
    
    return pcAlphabet;
}


/*====================================================================================
FUNCTION: WeSCL_GetCharType
              
CREATE DATE: 2006-06-02
AUTHOR: Yang Min
        
DESCRIPTION:
    Get the character type value by the character input.
              
ARGUMENTS PASSED:
    None.          
RETURN VALUE:
    Return the character type value if success, else return USER_ERORR.      

USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need to get the correspond value in the character type table.
IMPORTANT NOTES:
    When function call the macro function to get the character type from the character type
    table, it will call this function indirectly.       
=======================================================================================*/
WE_UINT16 WeSCL_GetCharType(WE_UINT16 usCharacter)
{   
    /*Judge the character value of input*/
    if ((usCharacter <= 8) || (usCharacter >= 14 && usCharacter <= 31) || usCharacter == 127)
    {
        return _C;
    }
    else if (usCharacter >= 128 && usCharacter <= 255)
    {
        return 0;
    }
    else if (usCharacter >= 103 && usCharacter <= 122)
    {
        return _LC;
    }
    else if (usCharacter >= 71 && usCharacter <= 90)
    {
        return _UC;
    }
    else if (usCharacter >= 48 && usCharacter <= 57)
    {
        return _D;
    }
    else if (usCharacter >= 65 && usCharacter <= 70)
    {
        return _UC | _X;
    }
    else if (usCharacter >= 97 && usCharacter <= 102)
    {
        return _LC | _X;
    }
    else
    {
        switch(usCharacter)
        {
        case 9:
            return _C | _WS | _B | _T | _XWS;
        case 10:
            return _C | _WS | _XWS;
        case 11:
        case 12:
            return _C | _WS;
        case 13:
            return _C | _WS | _XWS;
        case 32:
            return _SP | _WS | _B | _T | _XWS;
        case 33:
        case 39:
        case 42:
        case 45:
        case 46:
        case 95:
        case 126:
            return _M;
        case 34:
        case 62:
            return _DL | _T;
        case 35:
            return _DL;
        case 36:
        case 38:
        case 43:
            return _SRES;
        case 37:
           return _ESC | _DL;
        case 40:
        case 41:
            return _M | _T;
        case 44:
        case 58:
        case 61:
        case 64:
            return _SRES | _T;
        case 47:
        case 59:
        case 63:
            return _ARES | _T;
        case 60:
            return _DL | _T;
        case 91:
        case 92:
        case 93:
        case 123:
        case 125:
            return _UW | _T;
        case 94:
        case 96:
        case 124:
            return _UW;  
        default:
            return USER_ERORR;
        }
    }
    return USER_ERORR;
}

/*====================================================================================
FUNCTION: WeSCL_GetByteOfUTF8
              
CREATE DATE: 2006-06-02
AUTHOR: Yang Min
        
DESCRIPTION:
    Create the UTF-8 number of character table, and get the byte of input character.
              
ARGUMENTS PASSED:
    cTempValue: The character whose bytes to be found in the table.          
RETURN VALUE:
    Return the pointer to the table if success, else return FALSE.     

USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need to get the character type table.
IMPORTANT NOTES:
    When WeSCL_Utf8StrIncr, WeSCL_Utf8CharLen, WeSCL_Utf8Strlen, WeSCL_Utf8StrnLen want to 
    get the byte of UTF8 character, will use this function.       
=======================================================================================*/
static WE_CHAR WeSCL_GetByteOfUTF8(WE_CHAR cTempValue)
{
    
    /*WE_CHAR *pucTemp           = NULL;*/
    WE_CHAR  cUTF8Byte         = 0;
    /*The UTF8 table*/
    WE_UCHAR acTable[17] = {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 3, 4};

    cTempValue = ((WE_UCHAR)((WE_UCHAR)(cTempValue) >> 4));
    
    /*Check if the value is legality*/
    if (cTempValue >= 16 || cTempValue & 0x80) /* 0x80 means "< 0" */
    {
        return FALSE;
    }

    cUTF8Byte = acTable[cTempValue];
    return cUTF8Byte;
}

/*====================================================================================
FUNCTION: WeSCL_StrToLower

CREATE DATE: 2006-06-02
AUTHOR: Yang Min

DESCRIPTION:
    Converts source string to a lower string.

ARGUMENTS PASSED:
    pcStrDest : the destination string
	pcStrSource : the source string to be changed

RETURN VALUE:
    TRUE if the operation was successful, otherwise the appropriate return FALSE.   

USED GLOBAL VARIABLES:
	None
USED STATIC VARIABLES:
	None
CALL BY:
	All program need to convert string to lower string.
IMPORTANT NOTES:
	The length of the string to be converted can not be longer than 4.       
=======================================================================================*/
WE_BOOL WeSCL_StrToLower
(
    WE_CHAR *pcStrDest,
    WE_CHAR *pcStrSource
)
{
    WE_CHAR     acTempExt[5];
    WE_CHAR     *pcTempSource   = NULL;
    WE_INT32    iPos            = 0;
    WE_INT    iLen;

    if(!pcStrSource || !pcStrDest)
    {
        return FALSE;
    }
    /*Get the length of byte string*/
    iLen = WE_STRLEN(pcStrSource);

    if (NULL == (pcTempSource = (WE_CHAR *)WE_MALLOC((iLen + 1)*sizeof(WE_CHAR))))
    {
        return FALSE;
    }
    WE_MEMSET(pcTempSource, 0, (iLen + 1)*sizeof(WE_CHAR));
    WE_MEMCPY(pcTempSource, pcStrSource, (iLen + 1)*sizeof(WE_CHAR));

    if(iLen >= 5)
    {
        WE_FREE(pcTempSource);
        pcTempSource = NULL;
        return FALSE;
    }

    /*Set the "acTempExt" array to 0*/
    WE_MEMSET(acTempExt,0x00,sizeof(acTempExt));
    while(iPos < iLen)
    {
        /* Change the character to lowercase */
        acTempExt[iPos++] = (WE_CHAR)WE_STRLOWER((int)*pcTempSource);
        pcTempSource++;
    }/* End while */

    /* Copy the "acTempExt" string to the destination string */
    WE_STRCPY(pcStrDest, acTempExt);

    WE_FREE(pcTempSource);
    pcTempSource = NULL;
    return TRUE;
}/*End WeSCL_StrToLower*/


/*====================================================================================
FUNCTION: WeSCL_Base64DecodeChar

CREATE DATE: 2006-06-02
AUTHOR: Yang Min

DESCRIPTION:
    Translates the character into its 6-bit decoded base64 value.

ARGUMENTS PASSED:
    ucCHAR : the character to be changed.

RETURN VALUE:
	Returns -1 if the given input is not a legal base64 character.
	Otherwise return the Translated character.

USED GLOBAL VARIABLES:
	None
USED STATIC VARIABLES:
	None
CALL BY:
	All program need decode character.
IMPORTANT NOTES:
	The WeSCL_Base64DecodeChar function translates the character into its 6-bit value 
    for each input character in a base64.
    Because WeSCL_Base64DecodeChar function translates the character into its 6-bit value 
    for each input character in a base64,it is good practice always to decode the character.      
=======================================================================================*/
WE_INT32 WeSCL_Base64DecodeChar (WE_UCHAR ucCHAR)
{
    /*The given input is not a legal base64 character*/
    if (ucCHAR & 0x80)
    {
        return -1;
    }
    /*The encoded character is upper character*/
    if (ct_isupper (ucCHAR))
    {
        return ucCHAR - 'A';
    }
    /*The encoded character is lower character*/
    else if (ct_islower (ucCHAR))
    {
        return ucCHAR + 26 - 'a';
    }
    /*The encoded character is digital*/
    else if (ct_isdigit (ucCHAR))
    {
        return ucCHAR + 52 - '0';
    }
    /*The encoded character is "+"*/
    else if (ucCHAR == '+')
    {
        return 62;
    }
    /*The encoded character is "/"*/
    else if (ucCHAR == '/')
    {
        return 63;
    }
    /*Out of the range*/
    else 
    {
        return -1;
    }
}/*End WeSCL_Base64DecodeChar*/


/*====================================================================================
FUNCTION: WeSCL_Base64Decode

CREATE DATE: 2006-06-05
AUTHOR: Yang Min

DESCRIPTION:
	Given a string of base64-encoded data, decode it and place it 
	in the output buffer.

ARGUMENTS PASSED:
	*pcInBuf: The pointer to the string to be decoded
	iInLen:	  The number of the characters of the string to be decoded
	*pcOutBuf:	The pointer to the decoded string
	*piOutLen:The decoded string character number

RETURN VALUE:
	Each of these functions returns 0, or -1 if translated false.

USED GLOBAL VARIABLES:
	None
USED STATIC VARIABLES:
	None
CALL BY:
	All program need decode character.
IMPORTANT NOTES:
	The WeSCL_Base64Decode function decode a string of base64-encoded data and place
    it in the output buffer .
    Because WeSCL_Base64Decode calls function decode a string of base64-encoded data and place
    it in the output buffer,it is good practice always to handle the input character . 
=======================================================================================*/
WE_INT32 WeSCL_Base64Decode 
(
    const WE_CHAR   *pcInBuf,
    WE_INT32        iInLen,
    WE_CHAR         *pcOutBuf,
    WE_INT32        *piOutLen
)
{
    WE_CHAR     cInBufValue;
    WE_INT32    iTotalCount;
    WE_INT32    iTempCount = 0;
    WE_INT32    iEncodeValue;
    WE_INT32    iTmpValue = 0;

    /* The number of bits that have been saved in "iTmpValue", 
    to be used for the next output character. */
    WE_INT32   iSavedBits = 0;  
    if (!pcInBuf || !pcOutBuf || !piOutLen || 0 == iInLen)
    {
        return -1;
    }

    /* Convert back all character of encoded string */
    for (iTotalCount = 0; iTotalCount < iInLen; iTotalCount++) 
    {	
        /* For each character in the input */
        cInBufValue = pcInBuf[iTotalCount];
        if (cInBufValue == '=')
        {
            /* Padding, we have done. */
            break;
        }
        if ((cInBufValue == ' ') || (cInBufValue == '\t') || (cInBufValue == '\r') || (cInBufValue == '\n')) 
        {
            /* Ignore white space, table, return and new line character*/
            continue;
        }

        /* Convert back from base64 encoding */
        iEncodeValue = WeSCL_Base64DecodeChar (cInBufValue);
        if (iEncodeValue < 0) 
        { 
            /* Illegal character */
            return -1;
        }
        if (iSavedBits > 0) 
        {
            if (iTempCount >= *piOutLen)
            {
                *piOutLen = iTempCount;
                return -1;
            }
            pcOutBuf[iTempCount++] = (WE_CHAR)(iTmpValue | (iEncodeValue >> (iSavedBits - 2)));
        }
        /* Set the number of saved bits */
        iSavedBits = (iSavedBits == 0) ? 6 : iSavedBits - 2;
        if (iSavedBits > 0)
        {
            iTmpValue = (iEncodeValue << (8 - iSavedBits)) & 0xff;
        }
    }/*End for*/

    *piOutLen = iTempCount;
    return 0;
}/*End WeSCL_Base64Decode*/

/*=====================================================================================
FUNCTION: 
    WeSCL_Base64DecodeLen
CREATE DATE: 
    2006-06-05
AUTHOR: 
    Yang Min
DESCRIPTION:
    calculate the number of characters needed to decode.
ARGUMENTS PASSED:
    pcStr: the string needed to decode
    iLen: the length of the string
RETURN VALUE:
    If it is successful, each of these functions returns the result of calculation, the number of 
    character needed to be decoded. Else return -1;
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need decode character.
IMPORTANT NOTES:
    Before use he WeSCL_Base64DecodeLen function calculate the number of character needed to 
    decode the string 'pcStr' of length 'iLen'. 
    Because WeSCL_Base64DecodeLen function calculate the number of character needed to 
    decode the string 'pcStr' of length 'iLen',it is good practice always to complete
    decode the character. 
  
=====================================================================================*/

WE_INT32 WeSCL_Base64DecodeLen
(
    const WE_CHAR   *pcStr,
    WE_INT32        iLen
)
{
    WE_INT32 iNewLen;	/* The length of decoded base64 string */
    
    if (iLen == 0)
    {
        return 0;
    }
    if (!pcStr) 
    {
        return -1;
    }
    /* The Encoded string length is about 4/3 of the original string */
    iNewLen = (iLen / 4) * 3;
    /* There is one pad in the decoded string */
    if ((iLen > 0) && (pcStr[iLen - 1] == '='))
    {
        iNewLen--;
    }
    /* There is an other pad in the decoded string */
    if ((iLen > 1) && (pcStr[iLen - 2] == '='))
    {	
        iNewLen--;
    }

    return iNewLen;
}/* End WeSCL_Base64DecodeLen */


/*=====================================================================================
FUNCTION: 
    WeSCL_Base64Encode
CREATE DATE: 
    2006-06-05
AUTHOR: 
    Yang Min
DESCRIPTION:
    Create a base-64 encoding data, and place it in the output buffer.
ARGUMENTS PASSED:
    pcInBuf: the buffer that storage input character 
    iInLen:  the length of the input character
    pcOutBuf:the buffer that storage output character
    piOutLen:the length of output character had encoded.On input, 'piOutLen' should
             hold the maximum size of the output buffer,on output, it will hold
             the length of the string written there.
RETURN VALUE:
    Each of these functions returns 0 if the decode succeed, and -1 on false.
USED GLOBAL VARIABLES:
    None.
USED STATIC VARIABLES:
    None.
CALL BY:
    All program need encode character.
IMPORTANT NOTES:
    The WeSCL_Base64Encode function create a base-64 encoding of said data, and 
    place it in the output buffer.
    Because WeSCL_Base64Encode function create a base-64 encoding of said data, and 
    place it in the output buffer,it is good practice always to handle character. 
  
=====================================================================================*/
WE_INT32 WeSCL_Base64Encode
(
    const WE_CHAR   *pcInBuf, 
    WE_INT32        iInLen, 
    WE_CHAR         *pcOutBuf, 
    WE_INT32        *piOutLen
)
{
    WE_INT32        iLen = ((iInLen + 2) / 3) * 4;
    WE_UCHAR        *pucTemp;
    WE_INT32        iOrigCount;
    WE_INT32        iEncodedCount;

    WE_UCHAR        *pcBsae64 = NULL;
    
    if (!pcInBuf || !pcOutBuf || !piOutLen || iInLen == 0)
    {
        return -1;
    }
    /* The base64 decode table */
    if (NULL == (pcBsae64 = (WE_UCHAR*)WeSCL_CreateAlphabetTable()))
    {
        return -1;
    }
    if (*piOutLen < iLen + 1) 
    {
        /* There is something wrong with the length */
        *piOutLen = 0;
        WE_FREE(pcBsae64);
        pcBsae64 = NULL;
        return -1;
    }
    *piOutLen = iLen;

    /* Encode 3*n characters */
    for (iOrigCount = 0, iEncodedCount = 0, pucTemp = (WE_UCHAR *)pcOutBuf;
       iOrigCount + 2 < iInLen; iOrigCount += 3, pucTemp += 4, iEncodedCount += 4) 
    {
        pucTemp[0] = pcBsae64[(pcInBuf[iOrigCount] >> 2) & 0x3f];
        pucTemp[1] = pcBsae64[((pcInBuf[iOrigCount] << 4) & 0x30) | ((pcInBuf[iOrigCount + 1] >> 4) & 0x0f)];
        pucTemp[2] = pcBsae64[((pcInBuf[iOrigCount + 1] << 2) & 0x3c) | ((pcInBuf[iOrigCount + 2] >> 6) & 0x03)];
        pucTemp[3] = pcBsae64[pcInBuf[iOrigCount + 2] & 0x3f];
    }/* End for */
   
    /* Check whether the number of original string is 3*n */
    if (iOrigCount < iInLen)
    {
        /* The number of original string is not 3*n */
        pucTemp[0] = pcBsae64[(pcInBuf[iOrigCount] >> 2) & 0x3f];
        /* Supply the encode string with pad */
        if (iOrigCount + 1 < iInLen)
        {
            /* The number of character is (3*n + 2), supply one pad to the end of the encoded string */
            pucTemp[1] = pcBsae64[((pcInBuf[iOrigCount] << 4) & 0x30) | ((pcInBuf[iOrigCount + 1] >> 4) & 0x0f)];
            pucTemp[2] = pcBsae64[((pcInBuf[iOrigCount + 1] << 2) & 0x3c)];
        }
        else
        {
            /* The number of character is (3*n + 1), supply two pad to the end of the encoded string */
            pucTemp[1] = pcBsae64[((pcInBuf[iOrigCount] << 4) & 0x30)];
            pucTemp[2] = '=';
        }
        pucTemp[3] = '=';
    }/* End if */
    pcOutBuf[iLen] = '\0';
    
    WE_FREE(pcBsae64);
    pcBsae64 = NULL;
    return 0;
}

/*=====================================================================================
FUNCTION: 
    WeSCL_StrnDup
CREATE DATE: 
    2006-06-05
AUTHOR: 
    Chenhw
DESCRIPTION:
    Duplicate string.
ARGUMENTS PASSED:
    pcStrSource: a string is duplicated by this function.
    iLen:Length of string needing be copied.
RETURN VALUE:
    Each of these functions returns a pointer to the storage location for the copied 
    string or NULL if storage cannot be allocated.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need duplicate a string.
IMPORTANT NOTES:
    The WeSCL_Strndup function calls MALLOC to allocate storage space for a copy of 
    pcStrSource and then copies specify length pcStrSource to the allocated space.
    Because WeSCL_Strndup calls MALLOC to allocate storage space for the copy of 
    pcStrSource,it is good practice always to release this memory by calling the 
    FREE routine on the pointer returned by the call to WeSCL_Strndup.      
  
=====================================================================================*/
WE_CHAR* WeSCL_StrnDup
(
    const WE_CHAR *pcStrSource,
    WE_INT32 iLen
)
{
    if (iLen < 0)
    {
        return NULL;
    }
    else
    {   
        WE_CHAR *pcDstStr = NULL;	/*The temporary pointer*/
	    /*The value input is not NULL*/
        if (pcStrSource != NULL) 
        {
		    /*Allocate memory to the temporary pointer*/
            pcDstStr = (WE_CHAR *)WE_MALLOC((iLen + 1)*sizeof(WE_CHAR));	
            if (pcDstStr != NULL) 
            {
			    /*Successed to allocate memory to the temporary pointer, copy the source string*/
                WE_STRNCPYLC(pcDstStr, pcStrSource, iLen);
                pcDstStr[iLen] = '\0';
            }
        }
	    /*The value inpit is NULL*/
        else
        {
		    /*The value is NULL, just to allocate 1 byte of memory to the pointer*/
            pcDstStr = (WE_CHAR*) WE_MALLOC (sizeof(WE_CHAR));
            if (pcDstStr != NULL)
            {
			    /*Set the end of the string to '\0'*/
                pcDstStr[0] = '\0';
            }
        }
        return pcDstStr;
    }
}/*End WeSCL_StrnDup*/


/*=====================================================================================
FUNCTION: 
    WeSCL_StrCat
CREATE DATE: 
    2006-06-05
AUTHOR: 
    Chenhw
DESCRIPTION:
    Connect two string and Create a new string.
ARGUMENTS PASSED:
    pcStrSource1: First string is connected by this function.
    pcStrSource2: Second string is connected by this function.
RETURN VALUE:
    Each of these functions returns a pointer to the storage location for the connected 
    two string or NULL if storage cannot be allocated.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need connect two string.
IMPORTANT NOTES:
    The WeSCL_Strcat function calls MALLOC to allocate storage space for a connective 
    of pcStrSource1&pcStrSource2 .
    Because WeSCL_Strcat calls MALLOC to allocate storage space for the connective 
    of pcStrSource1&pcStrSource2,it is good practice always to release this memory 
    by calling the FREE routine on the pointer returned by the call to WeSCL_Strcat. 
  
=====================================================================================*/
WE_CHAR* WeSCL_StrCat
(
    const WE_CHAR *pcStrSource1,
    const WE_CHAR *pcStrSource2
)
{
    WE_CHAR* pcDstStr = NULL;
	/*Get the length of the two put in string*/
    WE_INT32 iLen1 = (pcStrSource1 != NULL ? WE_STRLEN(pcStrSource1) : 0);
    WE_INT32 iLen2 = (pcStrSource2 != NULL ? WE_STRLEN(pcStrSource2) : 0);

	/*All of the two string is not NULL*/
    if((pcStrSource1 != NULL)||(pcStrSource2 != NULL))
    {
		/*Allocate memory to the destination pointer, "+ 1" because of '\0'*/
        pcDstStr = (WE_CHAR*)WE_MALLOC((iLen1 + iLen2 + 1)*sizeof(WE_CHAR));
        if (pcDstStr != NULL) 
        {
            if (pcStrSource1 != NULL)
            {
                /*Copy pcStrSource1 first*/
                WE_STRCPY(pcDstStr,pcStrSource1);
            }
            if (pcStrSource2 != NULL)
            {
                /*Then copy pcStrSource2*/
                WE_STRCPY(pcDstStr + iLen1,pcStrSource2);
            }            
        }
    }/*End if*/
    return pcDstStr;
}/*End WeSCL_StrCat*/


/*=====================================================================================
FUNCTION: 
    WeSCL_StrnCat
CREATE DATE: 
    2006-06-05
AUTHOR: 
    Chenhw
DESCRIPTION:
    Connect two string and Create a new string.
ARGUMENTS PASSED:
    pcStrSource1: First string is connected by this function.
    pcStrSource2: Second string is connected by this function.
    iLen2:Length of chars needed connect in pcStrSource2.
RETURN VALUE:
    Each of these functions returns a pointer to the storage location for the connected 
    two string or NULL if storage cannot be allocated.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need connect two string.
IMPORTANT NOTES:
    The WeSCL_Strncat function calls MALLOC to allocate storage space for a connective 
    of pcStrSource1&pcStrSource2 .
    Because WeSCL_Strncat calls MALLOC to allocate storage space for the connective 
    of pcStrSource1&pcStrSource2,it is good practice always to release this memory 
    by calling the FREE routine on the pointer returned by the call to WeSCL_Strncat. 
  
=====================================================================================*/
WE_CHAR* WeSCL_StrnCat
(
    const WE_CHAR *pcStrSource1, 
    const WE_CHAR *pcStrSource2, 
    WE_INT32 iLen2
)
{
    if (iLen2 < 0)
    {
        return NULL;
    }
    else
    {
        WE_CHAR *pcDstStr = NULL;
	    /*Get the string lengths of pcStrSource1*/
        WE_INT32 iLen1 = (pcStrSource1 != NULL ? WE_STRLEN(pcStrSource1):0);
	    /*All of the two strings is not NULL*/
        if ((pcStrSource1 != NULL)||(pcStrSource2 != NULL)) 
        {
		    /*Allocate memory to the destination pointer, "+ 1" because of '\0'*/
            pcDstStr = (WE_CHAR*)WE_MALLOC(iLen1 + iLen2 + 1);
            if (pcDstStr != NULL)
            { 
                if (pcStrSource1 != NULL)
                {
                    /*Copy pcStrSource1 first*/
                    WE_STRCPY(pcDstStr,pcStrSource1);
                }
                if ((pcStrSource2 != NULL)&&(iLen2 != 0))
                {
                    /*Copy iLen2 bytes of string to the end of pcStrSource1*/
                    WE_STRNCPYLC(pcDstStr + iLen1,pcStrSource2,iLen2);
                }	
                /*Copying does not contain the '\0',so append it to the end of the destination string*/
                pcDstStr[iLen1 + iLen2] = '\0';
            }		                 
        }/*End if*/
        return pcDstStr;
    }
}/*End WeSCL_StrnCat*/


/*=====================================================================================
FUNCTION: 
    WeSCL_Str3Cat
CREATE DATE: 
    2006-06-05
AUTHOR: 
    Chenhw
DESCRIPTION:
    Connect three strings and Create a new string.
ARGUMENTS PASSED:
    pcStrSource1: First string is connected by this function.
    pcStrSource2: Second string is connected by this function.
    pcStrSource2: Third string is connected by this function.
RETURN VALUE:
    Each of these functions returns a pointer to the storage location for the connected 
    three string or NULL if storage cannot be allocated.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need connect three string.
IMPORTANT NOTES:
    The WeSCL_Str3cat function calls MALLOC to allocate storage space for a connective 
    of pcStrSource1&pcStrSource2&pcStrSource3.
    Because WeSCL_Str3cat calls MALLOC to allocate storage space for the connective 
    of pcStrSource1&pcStrSource2&pcStrSource3,it is good practice always to release 
    this memory by calling the FREE routine on the pointer returned by the call to 
    WeSCL_Str3Cat.   

=====================================================================================*/
WE_CHAR* WeSCL_Str3Cat
(
    const WE_CHAR *pcStrSource1, 
    const WE_CHAR *pcStrSource2, 
    const WE_CHAR *pcStrSource3
)
{
    WE_CHAR* pcDstStr = NULL;
	/*Get the string length of three strings*/
    WE_INT32 iLen1 = (pcStrSource1 != NULL ? WE_STRLEN(pcStrSource1):0);
    WE_INT32 iLen2 = (pcStrSource2 != NULL ? WE_STRLEN(pcStrSource2):0);
    WE_INT32 iLen3 = (pcStrSource3 != NULL ? WE_STRLEN(pcStrSource3):0);
	
	/*All of the three strings is not NULL*/
    if ((pcStrSource1 != NULL)||(pcStrSource2 != NULL)||(pcStrSource3 != NULL))
    {
		/*Allocate memory to the destination pointer, "+ 1" because of '\0'*/
        pcDstStr = (WE_CHAR*)WE_MALLOC(iLen1+iLen2+iLen3+1);
        if (NULL == pcDstStr) 
        {
            return NULL;
        }
        if (pcStrSource1 != NULL)
        {
			/*Copy the first string to destination pointer*/
            WE_STRCPY(pcDstStr,pcStrSource1);
        }
        if (pcStrSource2 != NULL)
        {
			/*Copy the second string to destination pointer*/
            WE_STRCPY(pcDstStr + iLen1,pcStrSource2);
        }
        if (pcStrSource3 != NULL)
        {
			/*Copy the last string to destination pointer*/
            WE_STRCPY(pcDstStr+iLen1+iLen2,pcStrSource3);
        }
    }/*End if*/
    return pcDstStr;
}/*End WeSCL_Str3Cat*/


/*=====================================================================================
FUNCTION: 
    WeSCL_StrnChr
CREATE DATE: 
    2006-06-05
AUTHOR: 
    Chenhw
DESCRIPTION:
    Find a character in a string at first occurrence position.
ARGUMENTS PASSED:
    pcStrSource: Specify string found char.
    cCharFound: Specify char found in a string.
    iLen: Find char in a string after at most 'iLen' characters.
RETURN VALUE:
    Return a pointer to the first occurrence of the character 'cCharFound'
    in the string 'pcStrSource'. The search stops at a null-byte or after
    at most 'iLen' characters.If not found return NULL.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need find char in a string.
IMPORTANT NOTES:
    None.   

=====================================================================================*/
WE_CHAR* WeSCL_StrnChr
(
    const WE_CHAR   *pcStrSource,
    WE_CHAR         cCharFound,
    WE_INT32        iLen
)
{
    if ((NULL == pcStrSource)||(iLen <= 0)) 
    {
        return NULL;
    }
    else
    {
        WE_INT32 iCount;
        /*Find the character one by one*/
        for(iCount = 0; (iCount < iLen)&&(pcStrSource[iCount] != '\0'); iCount++)
        {
            /*Return a pointer to the first occurrence of the character 'cCharFound'*/
            if (pcStrSource[iCount] == cCharFound)
            {
                return (WE_CHAR *)(pcStrSource + iCount);
            }
        }/*End for*/
        /*Can't find the character, return NULL*/
        return NULL;
    }
}/*End WeSCL_StrnChr*/


/*=====================================================================================
FUNCTION: 
	WeSCL_StrCpyLc
CREATE DATE: 
	2006-06-05
AUTHOR: 
	Chenhw
DESCRIPTION:
	Copy the null-terminated string from source buffer to destination buffer, while changing all 
	uppercase letters to lowercase letters.
ARGUMENTS PASSED:
	pcStrSource : Source string where the data is read from.
	pcStrDest   : Destination string where the data is passed into.
RETURN VALUE:
	void
USED GLOBAL VARIABLES:
	None
USED STATIC VARIABLES:
	None
CALL BY:
	All program need copy null-terminated string from one buffer to a specific buffer
IMPORTANT NOTES:
Non-ASCII characters are not affected with this function.
	None

=====================================================================================*/
void WeSCL_StrCpyLc
(
    WE_CHAR         *pcStrDest,
    const WE_CHAR   *pcStrSource
)
{
    if ((pcStrSource != NULL) && (pcStrDest != NULL))
    {
        /*Copy and convert characters to lowercase one by one*/
        while (*pcStrSource) 
        {
            *pcStrDest++= WeSCL_ChrLc(*pcStrSource++);
        }
        /*Set the end of the string to '\0'*/
        *pcStrDest = '\0';                
    }
}/*End WeSCL_StrCpyLc*/


/*=====================================================================================
FUNCTION: 
    WeSCL_StrnCpyLc
CREATE DATE: 
	2006-06-05
AUTHOR: 
	Chenhw
DESCRIPTION:
    Copy a null-terminated string from source buffer to destination buffer with specific string
	length, while changing all uppercase letters to lowercase.
ARGUMENTS PASSED:
	pcStrSource : Source string where the data is read from.
	pcStrDest   : Destination string where the data is passed into.
	iLen        : Length of string needing be copied.
RETURN VALUE:
	void
USED GLOBAL VARIABLES:
	None
USED STATIC VARIABLES:
	None
CALL BY:
	All program need copy null-terminated string with specific length from one buffer to a specific buffer
IMPORTANT NOTES:
	Non-ASCII characters are not affected with this function.
 
=====================================================================================*/
void WeSCL_StrnCpyLc
(
    WE_CHAR         *pcStrDest,
    const WE_CHAR   *pcStrSource,
    WE_INT32        iLen
)
{
    WE_INT32 iCount;
	if ((pcStrSource != NULL) &&(pcStrDest != NULL)&&(iLen > 0))
    {

        /*Copy and convert characters to lowercase one by one*/
        for (iCount = 0 ; iCount < iLen; iCount++) 
        {
            pcStrDest[iCount] = WeSCL_ChrLc(pcStrSource[iCount]);
            if ('\0' == pcStrSource[iCount])
            {
                /*The end of the string, jump the loop*/
                break;
            }
        }/*End for*/
        /*Set the end of the string to '\0'*/
        pcStrDest[iCount] = '\0';        
	}
}/*End WeSCL_StrnCpyLc*/ 

/*=====================================================================================
FUNCTION: 
	WeSCL_ChrLc
CREATE DATE: 
    2006-06-05
AUTHOR: 
    Chenhw
DESCRIPTION:
    Change a character to lowercase.
ARGUMENTS PASSED:
    cChr : the character which will be changed to lowercase.  
RETURN VALUE:
    he lowercase version of a character
USED GLOBAL VARIABLES:
    none
USED STATIC VARIABLES:
    none
CALL BY:
    All program need to change a uppercase character to a lowercase character.
IMPORTANT NOTES:
    Non-ASCII characters are not affected with this function.
    
=====================================================================================*/
WE_CHAR WeSCL_ChrLc(WE_CHAR cChr)
{
    return ((cChr>='A')&&(cChr<='Z')) ? (cChr +('a'-'A')) : cChr;
}
/*=====================================================================================
FUNCTION: 
	WeSCL_StrCmpNc
CREATE DATE: 
	2006-06-05
AUTHOR: 
	Chenhw
DESCRIPTION:
	Perform a string comparison between two strings, but without changing the uppercase to lowercase.
ARGUMENTS PASSED:
	pcStrSource1 :  First string which is compared.
	pcStrSource2 :  Second String which is compared.
RETURN VALUE:
	The first different character's ASCII value's difference in two strings.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
	None
CALL BY:
	All program need to compare two strings
IMPORTANT NOTES:
	Only upper and lower case versions of ASCII characters are treated as equal!

=====================================================================================*/
WE_INT32 WeSCL_StrCmpNc
(
    const WE_CHAR *pcStrSource1,
    const WE_CHAR *pcStrSource2
)
{
    /*Both of the strings are NULL, return 0*/
    if ((NULL == pcStrSource1) && (NULL == pcStrSource2)) 
    {
        return 0;
    }
    /*pcStrSource2 is NULL, return 1*/
    if ((NULL == pcStrSource2))
    {
        return 1;
    }
    /*pcStrSource1 is NULL, return -1*/
    if ((NULL == pcStrSource1))
    {
        return -1;
    }
	/*All of the two string is not NULL, start to compare the two strings*/
    for (; WeSCL_ChrLc(*pcStrSource1) == WeSCL_ChrLc(*pcStrSource2);
        pcStrSource1++, pcStrSource2++)
    {
        if ('\0' == *pcStrSource1)
        {
            /*The end of pcStrSource1, stop to compare the two strings, return 0*/
            return 0;
        }
    }/*End for*/
    /*Return the first different character's ASCII value's difference*/
    return (WeSCL_ChrLc(*pcStrSource1)- WeSCL_ChrLc(*pcStrSource2));	
}/*End WeSCL_StrCmpNc*/


/*=====================================================================================
FUNCTION: 
WeSCL_StrnCmpNc
CREATE DATE: 
	2006-06-05
AUTHOR: 
	Chenhw
DESCRIPTION:
	Perform a string comparison between two strings with specific length, but without 
	changing the uppercase to lowercase.
ARGUMENTS PASSED:
	pcStrSource1 :  First string which is compared.
	pcStrSource2 :  Second String which is compared.
	iLen         :  Length of string needs to compare.
RETURN VALUE:
	The first different character's ASCII value's difference in two strings.
USED GLOBAL VARIABLES:
	None
USED STATIC VARIABLES:
	None
CALL BY:
	All program need to compare two strings with specific string length.
IMPORTANT NOTES:
	only upper and lower case versions of ASCII characters are treated as equal!

=====================================================================================*/
WE_INT32 WeSCL_StrnCmpNc
(
    const WE_CHAR   *pcStrSource1,
    const WE_CHAR   *pcStrSource2,
    WE_INT32        iLen
)
{    
    if (iLen <= 0)
    {
        /*The length is lower than 0, return 0*/
        return 0;
    }
    /*Both of the strings are NULL, return 0*/
    if ((NULL == pcStrSource1)&&(NULL == pcStrSource2)) 
    {
        return 0;
    }
    /*pcStrSource2 is NULL, return 1*/
    if ((NULL == pcStrSource2))
    {
        return 1;
    }
    /*pcStrSource1 is NULL, return -1*/
    if ((NULL == pcStrSource1))
    {
        return -1;
    }
    /*All of the two string is not NULL, start to compare the two strings*/
    for (; WeSCL_ChrLc(*pcStrSource1) == WeSCL_ChrLc(*pcStrSource2);
        pcStrSource1++, pcStrSource2++) 
    {
        /*The end of pcStrSource1 or length of string has compared, 
          stop to compare the two strings, return 0*/
        if ((--iLen <= 0) || ('\0' == *pcStrSource1) || ('\0' == *pcStrSource2))
        {
            return 0;
        }
    }
    
    /*Return the first different character's ASCII value's difference*/
    return (WeSCL_ChrLc(*pcStrSource1) - WeSCL_ChrLc(*pcStrSource2));
}/*End WeSCL_StrnCmpNc*/

/*=====================================================================================
FUNCTION: 
    WeSCL_Int2Str 
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Zhou jun jiang
DESCRIPTION:
	Look up  integer value in the supplied hash table, 
ARGUMENTS PASSED:
    iValue: integer value need to be found.
	pstStrInfo:  struct containing the string table and hash table information.
RETURN VALUE:
	Returns -1 if the integer value was not found,otherwise return its associated string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need to find  a integer value's associated string.
IMPORTANT NOTES:
	The caller must not deallocate the returned string. 
  
=====================================================================================*/
const WE_CHAR* WeSCL_Int2Str(WE_INT32 iValue, const St_WeSCLStrtableInfo *pstStrInfo)
{

    /*Check the input parameters*/
    if( NULL== pstStrInfo)
    {
        return NULL;
    }
    
    /*Search method is indexing*/
    if ( WE_TABLE_SEARCH_INDEXING == pstStrInfo->sSearchMethod ) 
    {
        if ((iValue >= 0) && (iValue < pstStrInfo->iStringTableSize))
        {
            return pstStrInfo->pstStringTable[iValue].pcName;
        }
    }
    /*Search method is binary*/
    else if (WE_TABLE_SEARCH_BINARY == pstStrInfo->sSearchMethod) 
    {
        WE_INT32                     iFirst, iLast, iMiddle; /*counters*/
        WE_INT32                     iSearchValue;
        const St_WeSCLStrTableEntry *pstStringTable = pstStrInfo->pstStringTable;
        
        iFirst = 0;
        iLast = pstStrInfo->iStringTableSize - 1;
        /*use binary search to search iValue in StringTable*/
        while (iFirst <= iLast)
        {
            iMiddle = ((iFirst + iLast) >> 1);
            iSearchValue = pstStringTable[iMiddle].iValue;
            if (iSearchValue == iValue)
            {
                return pstStringTable[iMiddle].pcName;
            }
            else if (iSearchValue < iValue)
            {
                iFirst = iMiddle + 1;
            }
            else
            {
                iLast = iMiddle - 1;
            }
        }/*End while*/
    }

    /* Otherwise, use linear search. */
    else 
    {
        WE_INT32                         iCount;
        WE_INT32                         iStringTableSize = pstStrInfo->iStringTableSize;
        const St_WeSCLStrTableEntry      *pstStringTable = pstStrInfo->pstStringTable;
        
        for (iCount = 0; iCount < iStringTableSize; iCount++) 
        {
            if (pstStringTable[iCount].iValue == iValue)
            {
                return pstStringTable[iCount].pcName;
            }
        }/*End for*/
    }
    return NULL;
}

/*=====================================================================================
FUNCTION: 
    WeSCL_StrHash 
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Zhou jun jiang
DESCRIPTION:
	This hash function returns a 32-bit unsigned integer calculated over
	all positions in the given string.
ARGUMENTS PASSED:
    pStrSource: the given string .
	iLength:  the length of string need to be transformed, this number 
    must not larger than length of string and smaller than 0
RETURN VALUE:
	 Returns -1 if the parameters is invalidation,otherwise return the hash value of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need to calculate a hash value of a string.
IMPORTANT NOTES:
	None. 
  
=====================================================================================*/
WE_UINT32 WeSCL_StrHash(const WE_CHAR* pcStrSource,WE_INT32 iLength)
{
    WE_INT32    iCount;/*counter*/
    WE_INT32    c0 = 0;
    WE_INT32    c1 = 0;
    WE_INT32    c2 = 0;
    WE_INT32    c3 = 0;
    
    /*Check the input parameters*/
    if( NULL == pcStrSource || iLength <= 0 || (WE_INT32)WE_STRLEN(pcStrSource) < iLength)
    {
        return STR_HASH_ERROR;
    }
    iCount = iLength;
    /* This merge of a switch and a while statement is known
    * as "Duff's device", and is used for efficiency reasons. */
    switch (iLength & 0x3) 
    {
    case 0:
        while (iCount > 0)
        {
                c3 ^= pcStrSource[--iCount];
            case 3:
                c2 ^= pcStrSource[--iCount];
            case 2:
                c1 ^= pcStrSource[--iCount];
            case 1:
                c0 ^= pcStrSource[--iCount];
        }
    default:
        break;
    }
    
    return (((WE_UINT32)c0) << 24) | (((WE_UINT32)c1) << 16) |
        (((WE_UINT32)c2) <<  8) | ((WE_UINT32)c3);
}

/*=====================================================================================
FUNCTION: 
    WeSCL_StrHashLc 
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Zhou jun jiang
DESCRIPTION:
	This hash function returns a 32-bit unsigned integer calculated over
	all positions in the given string ,all upper string is regards as lower.
ARGUMENTS PASSED:
    pStrSource: the given string .
	iLength:  the length of string need to be transformed, 
    this number must not larger than length of string and smaller than 0
RETURN VALUE:
    Returns STR_HASH_ERROR(0xFFFFFFFF) if the parameters is invalidation,otherwise return the hash value of the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need to calculate a hash value of a string.
IMPORTANT NOTES:
	None. 
  
=====================================================================================*/
WE_UINT32 WeSCL_StrHashLc(const WE_CHAR* pcStrSource, WE_INT32 iLength)
{
    WE_INT32 iCount;
    WE_INT32    c0 = 0;
    WE_INT32    c1 = 0;
    WE_INT32    c2 = 0;
    WE_INT32    c3 = 0;
    
    /*Check the input parameters*/
    if(iLength <= 0 || NULL == pcStrSource || (WE_INT32)WE_STRLEN(pcStrSource) < iLength)
    {
        return STR_HASH_ERROR;
    }
    iCount = iLength;
    /* This merge of a switch and a while statement is known
    * as "Duff's device", and is used for efficiency reasons. */
    switch (iLength & 0x3) 
    {
    case 0:
        while (iCount > 0) 
        {
            /*change uper-case to lower-case  */
                c3 ^= WeSCL_ChrLc(pcStrSource[--iCount]);
            case 3:
                c2 ^= WeSCL_ChrLc(pcStrSource[--iCount]);
            case 2:
                c1 ^= WeSCL_ChrLc(pcStrSource[--iCount]);
            case 1:
                c0 ^= WeSCL_ChrLc(pcStrSource[--iCount]);
        }
    default:
        break;
    }
    
    return (((WE_UINT32)c0) << 24) | (((WE_UINT32)c1) << 16) |
        (((WE_UINT32)c2) <<  8) | ((WE_UINT32)c3);
}
/*=====================================================================================
FUNCTION: 
    WeSCL_Str2Int 
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Zhou jun jiang
DESCRIPTION:
	Look up a string's integer value in the supplied hash table, 
ARGUMENTS PASSED:
    pcStrSource: the string which need to be find integer value.
	iLength: the length of the string
	pstStrInfo: the struct containing the string table and hash table information.
RETURN VALUE:
	Returns -1 if the string was not found,otherwise return the string's integer value
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need to find a string's integer value.
IMPORTANT NOTES:
    Hash table size must large than 2. 
  
=====================================================================================*/
WE_INT32 WeSCL_Str2Int
(
    const WE_CHAR           *pcStrSource,
    WE_INT32                iLength,
    const St_WeSCLStrtableInfo    *pstStrInfo
)
{
    const WE_UCHAR              *pucHashTable = NULL;
    WE_INT32                    iHashTableSize = 0;
    const St_WeSCLStrTableEntry *pstStringTable = NULL;

    /*Get the hash value of the string*/
    WE_UINT32  uiStrHas = 0;
    WE_INT32   iDx = 0;
    WE_INT32   iStep = 0;
    WE_INT32   iTidx = 0;
    WE_INT32	 iCount = 0;

    /*Check the input parameters*/
    if(NULL == pstStrInfo || NULL == pcStrSource || iLength <= 0)
    {
        return -1;
    }

    pucHashTable = pstStrInfo->pucHashTable;
    iHashTableSize = pstStrInfo->iHashTableSize;
    pstStringTable = pstStrInfo->pstStringTable;
    
    if(iHashTableSize <= 2)
    {
        return -1;
    }

    uiStrHas = WeSCL_StrHash(pcStrSource, iLength);

    iDx = uiStrHas % iHashTableSize;
    iStep = 1 + (uiStrHas % (iHashTableSize - 2));

    /*Start to look up the string's integer value in the hash table*/
    for (;iCount < ((iHashTableSize + 1) / 2); iCount++)
    {
        iTidx = pucHashTable[iDx];
        if (iTidx == 0xff || iTidx >= pstStrInfo->iStringTableSize)
        {
            return -1;
        }
        /*Compare the string value*/
        if (!WE_STRNCMPNC(pcStrSource, pstStringTable[iTidx].pcName, iLength) &&
            pstStringTable[iTidx].pcName[iLength] == '\0')
        {
            return pstStringTable[iTidx].iValue;
        }
        
        iDx += iStep;
        if (iDx >= iHashTableSize)
        {
            iDx -= iHashTableSize;
        }
    }
    
    return -1;
}


/*=====================================================================================
FUNCTION: 
    WeSCL_Str2IntLc 
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Zhou jun jiang
DESCRIPTION:
	Look up a string's integer value in the supplied hash table  
ARGUMENTS PASSED:
    pcStrSource: the string need to be find integer value.
	iLength: the length of the string
	pstStrInfo: the struct containing the string table and hash table information ,
	            the ASCII of the string is lower case.
RETURN VALUE:
	Returns -1 if the string was not found,otherwise return the string's integer value
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need to find a string's lower case integer value.
IMPORTANT NOTES:
	This is lower-case version which means when hashing and matching, all upper-
	case ASCII characters are regarded as their lower-case
    counterparts. Consequently the 'info' string table can
    only contain lower-case strings.
   Hash table size must large than 2
  
=====================================================================================*/
WE_INT32 WeSCL_Str2IntLc
(
    const WE_CHAR *pcStrSource,
    WE_INT32 iLength,
    const St_WeSCLStrtableInfo *pstStrInfo
)
{
    const WE_UCHAR                  *pucHashTable = NULL;
    WE_INT32                         iHashTableSize = 0;
    const St_WeSCLStrTableEntry     *pstStringTable = NULL;
    
    /*calculated over all positions in the string.*/
    WE_UINT32       iStrHashLc = 0;
    WE_INT32        iDx = 0;
    WE_INT32        iStep = 0;
    WE_INT32        iTidx = 0;
    WE_INT32	 			iCount = 0;

    /*Check the input parameters*/
    if(NULL == pcStrSource || NULL == pstStrInfo || iLength <= 0)
    {
        return -1;
    }

    pucHashTable = pstStrInfo->pucHashTable;        /*Get Hash Table*/
    iHashTableSize = pstStrInfo->iHashTableSize;    /*Get Hash Table Size*/
    pstStringTable = pstStrInfo->pstStringTable;    /*Get String Table*/
    
    if(iHashTableSize <= 2)
    {
        return -1;
    }
    
    iStrHashLc = WeSCL_StrHashLc (pcStrSource, iLength); /*calculated string hash*/

    iDx = iStrHashLc % iHashTableSize;
    iStep = 1 + (iStrHashLc % (iHashTableSize - 2));

    /*Start to look up the string's integer value*/
    for (;iCount < ((iHashTableSize + 1) / 2); iCount++)
    {
        iTidx = pucHashTable[iDx];
        if (iTidx == 0xff || iTidx >= pstStrInfo->iStringTableSize)
        {
            return -1;
        }
        /*compare tow string return 0 is equal and other is different*/
        if (!WeSCL_StrnCmpNc(pcStrSource, pstStringTable[iTidx].pcName, iLength) &&
            pstStringTable[iTidx].pcName[iLength] == '\0')
        {
            return pstStringTable[iTidx].iValue;
        }
        iDx += iStep;
        if (iDx >= iHashTableSize)
        {
            iDx -= iHashTableSize;
        }
    }
    
    return -1;
}


/*=====================================================================================
FUNCTION: 
    WeSCL_CheckIPAddr
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Zhou jun jiang
DESCRIPTION:
    check whether IP address is valid.
ARGUMENTS PASSED:
    pcIPAddress: IP Address.
RETURN VALUE:
    Each of these functions returns TRUE if the IP Address is valid, otherwise,
    returns FALSE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need check the IP Address.
IMPORTANT NOTES:
    The WeSCL_CheckIPAddr function check whether IP address is valid.
    Because WeSCL_CheckIPAddr function check whether IP address is valid, it is good
    practice always to the program run non-terminate. 
  
=====================================================================================*/
WE_BOOL  WeSCL_CheckIPAddr(WE_CHAR *pcIPAddress)
{   
    WE_BOOL   bRet = TRUE;
    WE_UINT16 iCount;
    WE_UINT16 usPreDotPos = 0;
    WE_UINT8 *pucTemp = NULL;
    WE_UINT8  aucTempData[4];
    WE_UINT8  ucDotCount = 0;
    
    /*Check the input parameters*/
    if(( NULL == pcIPAddress) ||  
        (WE_STRLEN(pcIPAddress) > WE_IP_ADDRESS_MAX_LEN) ||
        (WE_STRLEN(pcIPAddress) < WE_IP_ADDRESS_MIN_LEN))
    {
        bRet = FALSE;
    }
    else
    {
        pucTemp = (WE_UINT8 *)pcIPAddress;
        for(iCount = 0; iCount < WE_STRLEN(pcIPAddress); iCount++)
        {
            if((pcIPAddress[iCount] < 0x30 || pcIPAddress[iCount] > 0x39) &&
                (pcIPAddress[iCount] != '.'))
            {
                bRet = FALSE;
                break;
            }
            else if('.' == pcIPAddress[iCount])
            {
                ucDotCount++;
                WE_MEMSET(aucTempData, 0, sizeof(aucTempData));
                if((iCount-usPreDotPos < 1) || (iCount - usPreDotPos) > 3)
                {
                    bRet = FALSE;
                    break;
                }
                WE_MEMCPY((WE_CHAR *)aucTempData,(WE_CHAR *)(&pcIPAddress[usPreDotPos]), iCount-usPreDotPos);
                if(WE_ATOI((WE_CHAR *)aucTempData) > 255)/* whether the number after the dot is more than 255 */
                {
                    bRet = FALSE;
                    break;
                }
                usPreDotPos = iCount+1;
            }
        }/* End for */
        if( (3 != ucDotCount)   || /* the dot count isn't equal to three */
            ('.' == pcIPAddress[iCount - 1]))/* the last char is '.' */
        {
            bRet = FALSE;
        }
        else 
        {
            /* whether the number after the last dot is more than 255 */
            WE_MEMSET(aucTempData, 0, sizeof(aucTempData));
            if((iCount-usPreDotPos < 1) || (iCount - usPreDotPos) > 3)
            {
                bRet = FALSE;
                
            }
            if(FALSE != bRet)
            {
                WE_MEMCPY((WE_CHAR *)aucTempData,(WE_CHAR *)(&pcIPAddress[usPreDotPos]), iCount-usPreDotPos);
                if(WE_ATOI((WE_CHAR *)aucTempData) > 255)
                {
                    bRet = FALSE;
                }
            }
        }/*End else*/
    }

    return bRet;
}


/*=====================================================================================
FUNCTION: 
    WeSCL_CheckProxyPort
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Zhou jun jiang
DESCRIPTION:
    check whether proxy port value is valid.
ARGUMENTS PASSED:
    pcPort: the proxy port
RETURN VALUE:
    Each of these functions returns TRUE if the proxy port is valid, otherwise,
    returns FALSE
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need connect two string.
IMPORTANT NOTES:
    The WeSCL_CheckProxyPort function check whether proxy port value is valid.
    Because WeSCL_CheckProxyPort function check whether proxy port value is valid,
    it is good practice always to the program run non-terminate. 
  
=====================================================================================*/
WE_BOOL  WeSCL_CheckProxyPort(WE_CHAR *pcPort)
{
    WE_BOOL bRet = TRUE;
    WE_INT32  iTempPort;
    WE_INT32  iCount;/* counter*/
    
    /*Check the input parameters*/
    if((NULL == pcPort) || (WE_STRLEN(pcPort) < 1) ||
        (WE_STRLEN(pcPort) > WE_PROXY_PORT_MAX_LEN))
    {
        bRet = FALSE;
    }
    else
    {
        for(iCount = 0; iCount < (WE_INT32)WE_STRLEN(pcPort); iCount++)
        {
            /* Whether data is number */
            if((pcPort[iCount] < 0x30) || (pcPort[iCount] > 0x39))
            {
                bRet = FALSE;
                break;
            }
        }
        if(bRet)
        {
            iTempPort = WE_ATOI(pcPort);
            if(iTempPort > WE_PROXY_PORT_MAX_NUMBER)
            {
                bRet = FALSE;
            }
        }
    }
    return bRet;
}
/*=====================================================================================
FUNCTION: 
    WeSCL_Month2Int
CREATE DATE: 
 2006-06-07
AUTHOR: 
 Venly Feng
DESCRIPTION:
    Find  the number of the month in the string .
ARGUMENTS PASSED:
    ppcStrSource: the string which want to find the month.
RETURN VALUE:
    Return the number of the month found in the string *ppcStrSource,
    and update the pointer 'ppcStrSource' to point beyond the month name if success, 
    return -1 otherwise.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need find the month in string.
IMPORTANT NOTES:
    The function which shoud change the string of months into number should call this 
    function.
  
=====================================================================================*/
WE_INT8 WeSCL_Month2Int(WE_CHAR **ppcStrSource)
{
    WE_CHAR *pcStart = NULL;
    WE_CHAR *pcEnd = NULL;
    WE_INT32 iLen=-1;
    WE_INT8 cRet = 0;
 
    if (NULL == ppcStrSource || NULL == *ppcStrSource)
    {
        return 0;
    }
    pcStart = *ppcStrSource;
    pcEnd = pcStart;
    while (ct_isalpha (*pcEnd))
    {
        pcEnd++;
    }
    iLen = pcEnd - pcStart;
    
    if ((iLen >= 2) && !WeSCL_StrnCmpNc (pcStart, "january", iLen)) 
    {
        cRet = 1;
    }
    else if ((iLen >= 1) && !WeSCL_StrnCmpNc (pcStart, "february", iLen)) 
    {
        cRet = 2;
    }
    else if ((iLen >= 3) && !WeSCL_StrnCmpNc (pcStart, "march", iLen))
    {
        cRet = 3;
    }
    else if ((iLen >= 2) && !WeSCL_StrnCmpNc (pcStart, "april", iLen)) 
    {
        cRet = 4;
    }
    else if ((iLen >= 3) && !WeSCL_StrnCmpNc (pcStart, "may", iLen)) 
    {
        cRet = 5;
    }
    else if ((iLen >= 3) && !WeSCL_StrnCmpNc (pcStart, "june", iLen)) 
    {
        cRet = 6;
    }
    else if ((iLen >= 3) && !WeSCL_StrnCmpNc (pcStart, "july", iLen))
    {
        cRet = 7;
    }
    else if ((iLen >= 2) && !WeSCL_StrnCmpNc (pcStart, "august", iLen)) 
    {
        cRet = 8;
    }
    else if ((iLen >= 1) && !WeSCL_StrnCmpNc (pcStart, "september", iLen))
    {
        cRet = 9;
    }
    else if ((iLen >= 1) && !WeSCL_StrnCmpNc (pcStart, "october", iLen))
    {
        cRet = 10;
    }
    else if ((iLen >= 1) && !WeSCL_StrnCmpNc (pcStart, "november", iLen))
    {
        cRet = 11;
    }
    else if ((iLen >= 1) && !WeSCL_StrnCmpNc (pcStart, "december", iLen)) 
    {
        cRet = 12;
    }
    if (cRet > 0)
    {
        *ppcStrSource += iLen;
    }
    
    return cRet;
}
 

/*=====================================================================================
FUNCTION: 
    WeSCL_ParseDuration
CREATE DATE: 
 2006-06-07
AUTHOR: 
 Venly Feng
DESCRIPTION:
    Parses a "duration" string (e.g. "PnYnMnDTnHnMnS") as defined in 
    "XML Schema Part 2: Datatypes. W3C Recommendation 2 May 2001" 
ARGUMENTS PASSED:
    pcStrSource: the string need to be parsed.
    pstDuration: the output struct containing the parsed string  .
RETURN VALUE:
    Return the result of the operation (FALSE or TRUE)
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need to parses a "duration" string.
IMPORTANT NOTES:
    The function which should parse a "duration" string (e.g. "PnYnMnDTnHnMnS") would 
    call this function. 
  
=====================================================================================*/
WE_BOOL WeSCL_ParseDuration
(
    const WE_CHAR* pcStrSource,
    St_WeSCLDuration *pstDuration
)
{
    WE_CHAR  acTimeStr[30] = "";
    WE_CHAR  acPeriod[30] = "";
    WE_CHAR  *pcTemp1 = NULL;
    WE_CHAR  *pcTemp2 = NULL;
    WE_CHAR  *pcTemp3 = NULL;
    WE_UINT32   uiYear = 0;
    WE_UINT32   uiMonth = 0;
    WE_UINT32   uiDay = 0;
    WE_UINT32   uiHour = 0;
    WE_UINT32   uiMinutes = 0;
    WE_UINT32   uiSeconds = 0;
    WE_INT8    cCount = 0;
    WE_INT8    cCycle = 0;
 
    if (NULL == pcStrSource || NULL == pstDuration)
    {
        return FALSE;
    }
 
    /* Convert s to int */
    WeSCL_StrCpyLc((WE_CHAR*)acPeriod, pcStrSource);
    
    /* Dates */
    pcTemp1 = WE_STRCHR((WE_CHAR*)acPeriod, 'p');

    if (pcTemp1 != NULL && 't' == *(pcTemp1 + 1))
    {
        return FALSE;
    }
    if (pcTemp1) 
    {
 
        /* Years */
        pcTemp3 = WE_STRCHR((WE_CHAR*)acPeriod, 'y');
        if (pcTemp3) 
        {
            WE_MEMSET((WE_HANDLE)&acTimeStr[0], 0, 30);
            cCount = pcTemp3 - pcTemp1 -1;

            for (cCycle = 0; cCycle < cCount; cCycle++)
            {
                if (0 == ct_isdigit(*(pcTemp1 + cCycle +1)))
                {
                    return FALSE;
                }
            }
            WE_STRNCPYLC(&acTimeStr[0], pcTemp1 + 1, cCount);
            uiYear = WE_ATOI(acTimeStr);
            pcTemp1 = pcTemp3;
 
        }
        
        /* Months */
        if (pcTemp1)
        {
            pcTemp3 = WE_STRCHR((WE_CHAR*)acPeriod, 'm');
            if (pcTemp3) 
            {
                WE_MEMSET((WE_HANDLE)&acTimeStr[0], 0, 30);
                cCount = pcTemp3 - pcTemp1 -1;

                for (cCycle = 0; cCycle < cCount; cCycle++)
                {
                    if (0 == ct_isdigit(*(pcTemp1 + cCycle +1)))
                    {
                        return FALSE;
                    }
                }
                WE_STRNCPYLC(&acTimeStr[0], pcTemp1 + 1, cCount);
                uiMonth = WE_ATOI(acTimeStr);
                pcTemp1 = pcTemp3;
 
            }
        }
        
        /* Days */
        if (pcTemp1) 
        {
            pcTemp3 = WE_STRCHR((WE_CHAR*)acPeriod, 'd');
            if (pcTemp3) 
            {
                WE_MEMSET((WE_HANDLE)&acTimeStr[0], 0, 30);
                cCount = pcTemp3 - pcTemp1 -1;
                if (cCount < 0)
                {
                    return FALSE;
                }
                for (cCycle = 0; cCycle < cCount; cCycle++)
                {
                    if (0 == ct_isdigit(*(pcTemp1 + cCycle +1)))
                    {
                        return FALSE;
                    }
                }
                WE_STRNCPYLC(&acTimeStr[0], pcTemp1 + 1, cCount);
               uiDay = WE_ATOI(acTimeStr);
            }
        }
    }
    
    /* Times */
    pcTemp1 = WE_STRCHR((WE_CHAR*)acPeriod, 't');
    if (pcTemp1) 
    {
        pcTemp2 = pcTemp1;
        
        /* Hours */
        pcTemp3 = WE_STRCHR(pcTemp2, 'h');
        if (pcTemp3) 
        {
            WE_MEMSET((WE_HANDLE)&acTimeStr[0], 0, 30);
            cCount = pcTemp3 - pcTemp1 -1;
            if (cCount < 0)
            {
                return FALSE;
            }
            for (cCycle = 0; cCycle < cCount; cCycle++)
            {
                if (!ct_isdigit(*(pcTemp1 + cCycle +1)))
                {
                    return FALSE;
                }
            }
            WE_STRNCPYLC(&acTimeStr[0], pcTemp1 + 1, cCount);
            uiHour = WE_ATOI(acTimeStr);
            pcTemp1 = pcTemp3;
        }
        
        /* Minutes */
        pcTemp3 = WE_STRCHR(pcTemp2, 'm');
        if (pcTemp3) 
        {
            WE_MEMSET((WE_HANDLE)&acTimeStr[0], 0, 30);
            cCount = pcTemp3 - pcTemp1 -1;
            if (cCount < 0)
            {
                return FALSE;
            }
            for (cCycle = 0; cCycle < cCount; cCycle++)
            {
                if (!ct_isdigit(*(pcTemp1 + cCycle +1)))
                {
                    return FALSE;
                }
            }
            WE_STRNCPYLC(&acTimeStr[0], pcTemp1 + 1, cCount);
            uiMinutes = WE_ATOI(acTimeStr);
            pcTemp1 = pcTemp3;
        }
        
        /* Seconds */
        pcTemp3 = WE_STRCHR(pcTemp2, 's');
        if (pcTemp3) 
        {
            WE_MEMSET((WE_HANDLE)&acTimeStr[0], 0, 30);
            cCount = pcTemp3 - pcTemp1 -1;
            if (cCount < 0)
            {
                return FALSE;
            }
            for (cCycle = 0; cCycle < cCount; cCycle++)
            {
                if (!ct_isdigit(*(pcTemp1 + cCycle +1)))
                {
                    return FALSE;
                }
            }
            WE_STRNCPYLC(&acTimeStr[0], pcTemp1 + 1, cCount);
            uiSeconds = WE_ATOI(acTimeStr);
        }
    }
    
    /*Set the duration value*/
    pstDuration->uiYears = uiYear;
    pstDuration->uiMonths = uiMonth;
    pstDuration->uiDays = uiDay;
    pstDuration->uiHours = uiHour;
    pstDuration->uiMinutes = uiMinutes;
    pstDuration->uiSeconds = uiSeconds;
    return TRUE;
}
 
/*=====================================================================================
FUNCTION: 
    WeSCL_Str2Time
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Venly Feng
DESCRIPTION:
    Converts a string (e.g. "Thu, 15 Nov 2001 08:12:31 GMT") into a
    Unix 31-bit time value (seconds since 1/1 1970, GMT).
ARGUMENTS PASSED:
    pcStrSource: the string which want to convert.
    puiSecond:  the second which the string convert to.
RETURN VALUE:
    Return the result of the operation (FALSE or TRUE) 
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need convert string to time.
IMPORTANT NOTES:
    The function need convert string like "Thu, 15 Nov 2001 08:12:31 GMT" should call 
    this function. The caller must check the valid of the passed string. 
  
=====================================================================================*/
WE_BOOL WeSCL_Str2Time
(
    const WE_CHAR *pcStrSource,
    WE_UINT32 *puiSecond
)
{
    WE_INT32 iMonth = -1;
    WE_INT32 iDate;
    WE_INT32 iHour;
    WE_INT32 iMinutes;
    WE_INT32 iSeconds = 0;
    WE_INT32 iZone = 0;
    WE_INT32 iLeapyYears;
    WE_INT32 iYear = -1;
    WE_INT32 iTemp;
    WE_INT32 iSign = 1;
    WE_CHAR *pcStr;
    WE_CHAR *pcTmp;
 
    if (NULL == pcStrSource || NULL == puiSecond)
    {
        if (puiSecond != NULL)
        {
            *puiSecond = 0;
        }
        return FALSE;
    }
    
    /* Do we have a weekday first? */
    pcStr = WeSCL_SkipBlanks (pcStrSource);
    if (NULL == pcStr) 
    {
        return FALSE;
    }
    if (ct_isalpha (pcStr[0]))
    {
        /* Just skip over the weekday */
        pcStr = WeSCL_SkipField (pcStr);
        pcStr = WeSCL_SkipBlanks (pcStr);
        if (NULL == pcStr) 
        {
            return FALSE;
        }
    }  
 
    /* Do we have the name of the month before the date? */
    if (ct_isalpha (pcStr[0])) 
    {
        iMonth = WeSCL_Month2Int (&pcStr);    
    }
    
    /* We should now be at the number of the day. */
    iDate = (WE_INT32)WE_STRTOUL (pcStr, &pcStr, 10);
    if (pcStr[0] == '-') 
    {
        pcStr++;
    }
    pcStr = WeSCL_SkipBlanks (pcStr);
    if (NULL == pcStr) 
    {
        return FALSE;
    }
    
    /* This should now be the name of the month (unless we saw it already) */
    if (ct_isalpha (pcStr[0])) 
    {
        if (iMonth > 0)
        {
            *puiSecond = 0;
            return FALSE;
        }
        iMonth = WeSCL_Month2Int (&pcStr);
    }
    if (iMonth < 0)
    {
        *puiSecond = 0;
        return FALSE;
    }
    pcStr = WeSCL_SkipBlanks (pcStr);
    if (NULL == pcStr) 
    {
        return FALSE;
    }
    if (pcStr[0] == '-')
    {
        pcStr++;
    }
    
    /* Now, we either have the year, or, in case of ANSI C, the time.
    * We read a number and check whether it is followed by ':' to determine
    * which it is. */
    
    iTemp = (WE_INT32)WE_STRTOUL (pcStr, &pcTmp, 10);
    if (pcTmp[0] == ':') 
    {
        /* It was the hour */
        iHour = iTemp;
        pcStr = pcTmp + 1;
    }
    else
    {
        /* It was the year. */
        iYear = iTemp;
        pcStr = WeSCL_SkipBlanks (pcTmp);
        if (NULL == pcStr) 
        {
            return FALSE;
        }
        iHour = (WE_INT32)WE_STRTOUL (pcStr, &pcStr, 10);
        if (pcStr[0] != ':')
        {
            *puiSecond = 0;
            return FALSE;
        }
        pcStr++;
    }
    
    /* Read minutes, and possibly seconds. */
    iMinutes = (WE_INT32)WE_STRTOUL (pcStr, &pcStr, 10);
    if (pcStr[0] == ':')
    {
        iSeconds = (WE_INT32)WE_STRTOUL (pcStr + 1, &pcStr, 10);
    }
    pcStr = WeSCL_SkipBlanks (pcStr);
    if (NULL == pcStr) 
    {
        return FALSE;
    }
    
    /* If we have asctime format, the year remains to be read,
    * otherwise it is the timezone. */
    if (iYear < 0) 
    {
        if (!ct_isdigit (*pcStr)) 
        {
            *puiSecond = 0;
            return FALSE;
        }
        iYear = WE_ATOI (pcStr);
    }
    else 
    {
        if (ct_isalpha (pcStr[0]))
        {
            if ((WeSCL_StrnCmpNc (pcStr, "GMT", 3) == 0) ||
                (WeSCL_StrnCmpNc (pcStr, "UT", 2) == 0)) 
            {
                iZone = 0;
            }
            else if (WeSCL_StrnCmpNc (pcStr, "EDT", 3) == 0) 
            {
                iZone = -240;
            }
            else if (WeSCL_StrnCmpNc (pcStr, "EST", 3) == 0) 
            {
                iZone = -300;
            }
            else if (WeSCL_StrnCmpNc (pcStr, "CDT", 3) == 0) 
            {
                iZone = -300;
            }
            else if (WeSCL_StrnCmpNc (pcStr, "CST", 3) == 0) 
            {
                iZone = -360;
            }
            else if (WeSCL_StrnCmpNc (pcStr, "MDT", 3) == 0)
            {
                iZone = -360;
            }
            else if (WeSCL_StrnCmpNc (pcStr, "MST", 3) == 0) 
            {
                iZone = -420;
            }
            else if (WeSCL_StrnCmpNc (pcStr, "PDT", 3) == 0)
            {
                iZone = -420;
            }
            else if (WeSCL_StrnCmpNc (pcStr, "PST", 3) == 0) 
            {
                iZone = -480;
            }
           
        }
        
        else 
        {
            if (pcStr[0] == '+')
            {
                pcStr++;
            }
            else if (pcStr[0] == '-') 
            {
                iSign = -1;
                pcStr++;
            }
            iTemp = WE_ATOI (pcStr);
            iZone = (iTemp / 100) * 60 + (iTemp % 100);
            if (iZone > 1200)
            {
                *puiSecond = 0;
                return FALSE;
            }
            if (iSign < 0)
            {
              iZone = -iZone;
            }
        }
    }
    
    /* Check the year value we retrieved */
    if (iYear < 0)
    {
        *puiSecond = 0;
        return FALSE;
    }
    /* Compensate for 2-digit years */
    if (iYear < 40)
    {
        iYear += 2000;
    }
    else if (iYear < 100)
    {
        iYear += 1900;
    }
    if ((iYear < 1970) || (iYear > 2038))
    {
        *puiSecond = 0;
        return FALSE;
    }
    iYear -= 1970;
    
    /* Compute number of leap years */
    iLeapyYears = (iYear + 2) >> 2;
    if (((iYear & 0x03) == 0x02) && (iMonth <= 2))
    {
        iLeapyYears--;
    }
 
    *puiSecond =  iYear * 31536000 +
        (iLeapyYears + WeSCL_MonthDays(iMonth - 1) + iDate - 1) * 86400 +
        iHour * 3600 + iMinutes * 60 + iSeconds - iZone * 60;
    
    return TRUE;
}
/*=====================================================================================
FUNCTION: 
    WeSCL_MonthDays
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Venly Feng
DESCRIPTION:
    Get the character type table.
ARGUMENTS PASSED:
    iNum: the number of sequence in the table.
RETURN VALUE:
    Return the correct value if success, 0 otherwise. 
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need find the MonthDays in string.
IMPORTANT NOTES:
    This function call by WeSCL_Str2Time function in this file.
  
=====================================================================================*/
WE_INT32 WeSCL_MonthDays(WE_INT32 iNum)
{
    WE_INT32 aiTemp[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
 
    if (iNum > 11 || iNum < 0)
    {
        return 0;
    }
 
    return aiTemp[iNum];
 
}
/*=====================================================================================
FUNCTION: 
    WeSCL_Time2Str
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Venly Feng
DESCRIPTION:
    Format a Unix 31-bit time value (seconds since 1/1 1970, GMT)
    as a string in the format: "Thu, 15 Nov 2001 08:12:31 GMT"
    (as specified in RFC 1123).
ARGUMENTS PASSED:
    uiSecond: the given seconds which is start from 1/1 1970, GMT.
    pcStrSource: the formatted string.
RETURN VALUE:
    void
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need to convert time to string.
IMPORTANT NOTES:
    The string pcStrSource must have room for at least 30 bytes. 
  
=====================================================================================*/
void WeSCL_Time2Str
(
    WE_UINT32 uiSecond,
    WE_CHAR *pcStrSource
)
{
    WE_INT32 iSecs;
    WE_INT32 iMins;
    WE_INT32 iHours;
    WE_INT32 iDays;
    WE_INT32 iMonths;
    WE_INT32 iYears;
    WE_INT32 iCount;
    WE_INT32 iTemp;
    WE_CHAR *pcDay = NULL;
    WE_CHAR *pcMonthNames =NULL;
 
    if (NULL == pcStrSource)
    {
        return;
    }
    
    /* First, divide into days, hours, minutes and seconds. */
    iDays = (WE_INT32)(uiSecond / 86400);
    uiSecond = uiSecond % 86400;
    
    pcDay = WeSCL_DayNames(iDays % 7);
    
    iHours = (WE_INT32)(uiSecond / 3600);
    uiSecond = uiSecond % 3600;
    
    iMins = (WE_INT32)(uiSecond / 60);
    iSecs = (WE_INT32)(uiSecond % 60);
    
    /* Now the number of days has to be divided into years and months.
    * Start by approximating each year to be 365 days.
    * This approximation will be at most one off.
    * Compensate if necessary. */
    iYears = (iDays / 365);
    iDays = (iDays % 365);
    /* In this interval (1970 - 2038), every fourth year is a leap year,
    * without exceptions, starting with 1972. */
    iTemp = ((iYears + 1) / 4);
    if (iDays < iTemp)
    {
        iYears--;
        iDays = (iDays + 365 - iTemp);
        if (((iYears + 2) & 0x3) == 0)
        {
            iDays++;
        }
    }
    else
    {
        iDays -= iTemp;
    }
    
    /* To determine the month we simply do a linear search through
    * an array holding the number of days of each month.
    * First we have to select the correct array though,
    * there is one for leap years and one for non-leap years. */
    if (((iYears + 2) % 4) == 0)
    {
        for (iCount = 0; iCount < 12; iCount++) 
        {
            if (iDays < WeSCL_OneMonthDays(iCount))
            {
                break;
            }
            iDays -= WeSCL_OneMonthDays(iCount);
        }
    }
    else
    {
        for (iCount = 0; iCount < 12; iCount++) 
        {
            if (iDays < WeSCL_LeapOneMonthDays(iCount))
            {
                break;
            }
            iDays -= WeSCL_LeapOneMonthDays(iCount);
        }
 
    }
    
 
    iMonths = iCount;
    pcMonthNames = WeSCL_MonthNames(iMonths);
    /* We are done. The variable "years" holds the number of complete
    * years that have passed since 1970; "months" holds the number
    * of complete months that have passed since the beginning of
    * the present year; and similarly for the other variables. */
    WE_SPRINTF (pcStrSource, "%s, %02d %s %04d %02d:%02d:%02d GMT",
        pcDay, iDays + 1, pcMonthNames, (iYears + 1970),
        iHours, iMins, iSecs);
 
    WE_FREE(pcDay);
    WE_FREE(pcMonthNames);
    pcDay = NULL;
    pcMonthNames = NULL;
}
 
/*=====================================================================================
FUNCTION: 
    WeSCL_Hex2Byte
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Venly Feng
DESCRIPTION:
    Change hex value representation string to binary value representation string. 
ARGUMENTS PASSED:
    pcHexSource :  The location that store the hex value representation string.  
    pucByteDest :  The location that store the binary value representation string.
RETURN VALUE:
    Returns FALSE if the string 'pcHexSource' does not start with two 
    hexadecimal digits, and TRUE otherwise.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES: 
    None
CALL BY:
    All program need to change hex value representation string to binary value 
    representation string. 
IMPORTANT NOTES: 
    This function only can change 2bit hex value.
=====================================================================================*/
WE_BOOL WeSCL_Hex2Byte
(
    const WE_CHAR *pcHexSource,
    WE_UINT8 *pucByteDest
)
{
    WE_CHAR  cStr;
    WE_UINT8 ucVal;
 
    if (NULL == pcHexSource || NULL == pucByteDest )
    {
        return FALSE;
    }
    
    cStr = WeSCL_ChrLc(pcHexSource[0]);
    if (ct_isdigit (cStr))
    {
        ucVal = (WE_UCHAR)(cStr - '0');
    }
    else if (ct_ishexletter (cStr))
    {
        ucVal = (WE_UCHAR)(cStr - 'a' + 10);
    }
    else
    {
        return FALSE;
    }
    
    cStr = WeSCL_ChrLc(pcHexSource[1]);
    if (ct_isdigit (cStr))
    {
        ucVal = (WE_UCHAR)((ucVal << 4) + cStr - '0');
        
    }
    else if (ct_ishexletter (cStr))
    {
        ucVal = (WE_UCHAR)((ucVal << 4) + cStr - 'a' + 10);
    }
    else
    {
        return FALSE;
    }
    
    *pucByteDest = ucVal;
    
    return TRUE;
}
 
/*=====================================================================================
FUNCTION: 
    WeSCL_Hex2Int
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Venly Feng
DESCRIPTION:
    Change hex value representation string to integer value representation string. 
ARGUMENTS PASSED:
    pcHexLocation  :  The location that store the hex value representation string.  
RETURN VALUE:
    Return the integer value representation string if success, return 0 if false.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES: 
    None
CALL BY:
    All program need to change hex value representation string to integer value 
    representation string. 
IMPORTANT NOTES:  
    None
=====================================================================================*/
WE_INT32 WeSCL_Hex2Int ( const WE_CHAR *pcHexLocation )
{
    WE_INT32  iResult = 0;
    WE_INT32  iCycle = 0;
    WE_CHAR   acHex[9];
    WE_CHAR   *pcTemp = NULL;

    
    if (NULL == pcHexLocation)
    {
        return 0;
    }
    WE_MEMSET(acHex, 0, 9);
    for (iCycle = 0; iCycle < 8; iCycle++) 
    {
        if (ct_ishex(*pcHexLocation)) 
        {
            acHex[iCycle] = *pcHexLocation++;
        }
        else
        {
            break;
        }
    }
    pcTemp = acHex;
    while (0 != *pcTemp)
    {
        if (ct_isdigit (*pcTemp))
        {
            iResult = (iResult << 4) + (*pcTemp - '0');
        }
        else if (ct_islower (*pcTemp))
        {
            iResult = (iResult << 4) + (*pcTemp - 'a' + 10);
        }
        else
        {
            iResult = (iResult << 4) + (*pcTemp - 'A' + 10);
        }
        pcTemp++;
    }
    return iResult;
}
/*=====================================================================================
FUNCTION: 
    WeSCL_Hex2Long
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Venly Feng
DESCRIPTION:
    Change hex value representation string to long integer value representation string. 
ARGUMENTS PASSED:
    pcHexLocation  :  The location that store the hex value representation string.  
RETURN VALUE:
    Return the long integer value representation string if success, return 0 if false.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES: 
    None
CALL BY:
    All program need to change hex value representation string to long integer value 
    representation string. 
IMPORTANT NOTES:
    None
=====================================================================================*/
WE_LONG WeSCL_Hex2Long(const WE_CHAR* pcHexLocation)
{
    WE_LONG  iResult = 0;
    WE_CHAR cTemp;
    
    if (NULL == pcHexLocation)
    {
        return 0;
    }
    while (ct_ishex (cTemp = *pcHexLocation++))
    {
        if (ct_isdigit (cTemp))
        {
            iResult = (iResult << 4) + (cTemp - '0');
        }
        else if (ct_islower (cTemp))
        {
            iResult = (iResult << 4) + (cTemp - 'a' + 10);
        }
        else
        {
            iResult = (iResult << 4) + (cTemp - 'A' + 10);
        }
        if (iResult < 0)
        {
            return 0;
        }
    }
    if (cTemp != 0)
    {
        return 0;
    }
    
    return iResult;
}
/*=====================================================================================
FUNCTION: 
    WeSCL_Byte2Hex
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Venly Feng
DESCRIPTION:
    Change a binary value representation character to a hex value representation character. 
ARGUMENTS PASSED:
    ucByte        :  The character which will be changed.
    pucHexLocation:  The location that the changed character written into.
RETURN VALUE:
    void
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES: 
    None
CALL BY:
    All program need to change a binary value representation character to a hex value 
    representation character.
IMPORTANT NOTES:  
    None
=====================================================================================*/
void WeSCL_Byte2Hex
(
    WE_UINT8 ucByte,
    WE_CHAR *pcStrSource
)
{
    WE_UINT8 ucTemp;
    if (NULL == pcStrSource)
    {
        return;
    }
    ucTemp = (ucByte >> 4) & 0xF;
    *pcStrSource++ = WeSCL_GetHexDigits(ucTemp);
    ucTemp = ucByte & 0xF;
    *pcStrSource = WeSCL_GetHexDigits(ucTemp);
}
 
/*=====================================================================================
FUNCTION: 
    WeSCL_DayNames
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Venly Feng
DESCRIPTION:
    Get the pointer to the table which contain the names of a day in a week.
              
ARGUMENTS PASSED:
    iNum: the order of one day in a week .      
RETURN VALUE:
    Return the pointer to the string if success, else return NULL.   
 
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need to get the acDayNames table.
IMPORTANT NOTES:
    Free the pointer to the table after using the table. This function call 
    by WeSCL_Time2Str function in this file.
=====================================================================================*/
static WE_CHAR *WeSCL_DayNames(WE_INT32 iNum)
{
    WE_CHAR *pcTemp = NULL;
    WE_CHAR acDayNames[7][4] =          /*-ropi*/
    {
        
        "Thu",
        "Fri",
        "Sat",
        "Sun",
        "Mon",
        "Tue",
        "Wed"
    };
 
    if ( iNum > 6 || iNum <0)
    {
        return NULL;
    }
     /*Allocate memory to the acDayNames table pointer*/
    pcTemp = (WE_CHAR*) WE_MALLOC(4*sizeof(WE_CHAR));
    if (NULL == pcTemp)
    {
        return NULL;
    }
    WE_MEMSET(pcTemp , 0, 4);
    WE_MEMCPY(pcTemp, acDayNames[iNum], 3);
    return pcTemp;
}
/*=====================================================================================
FUNCTION: 
    WeSCL_MonthNames
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Venly Feng
DESCRIPTION:
    Get the pointer to the table which contain the names of the month.
              
ARGUMENTS PASSED:
    iNum: the order of one month in a year       
RETURN VALUE:
    Return the pointer to the table if success, else return NULL.   
 
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need to get the acMonthNames table.
IMPORTANT NOTES:
    Free the pointer to the table after using the table. This function call 
    by WeSCL_Time2Str function in this file.
  
=====================================================================================*/
static WE_CHAR *WeSCL_MonthNames(WE_INT32 iNum)
{
    WE_CHAR *pcTemp = NULL;
    WE_CHAR acMonthNames[12][4] =
    {
        "Jan",
        "Feb",
        "Mar",
        "Apr",
        "May",
        "Jun",
        "Jul",
        "Aug",
        "Sep",
        "Oct",
        "Nov",
        "Dec"
    };
 
    if ( iNum < 0 || iNum > 11 )
    {
        return NULL;
    }
     /*Allocate memory to the acMonthNames table pointer*/
    pcTemp = (WE_CHAR*) WE_MALLOC(4*sizeof(WE_CHAR));
    if (NULL == pcTemp)
    {
        return NULL;
    }
    WE_MEMSET(pcTemp , 0, 4);
    WE_MEMCPY(pcTemp, acMonthNames[iNum], 3);
 
    return pcTemp;
}
/*=====================================================================================
FUNCTION: 
    WeSCL_OneMonthDays
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Venly Feng
DESCRIPTION:
    Get the value in the array which contain the days of a month of a year.
              
ARGUMENTS PASSED:
    iNum:        
RETURN VALUE:
    Return the value if success, else return 0.   
 
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need to get the oneMonthDays .
IMPORTANT NOTES:
    This function call by WeSCL_Time2Str function in this file.
  
=====================================================================================*/
static WE_INT32 WeSCL_OneMonthDays(WE_INT32 iNum)
{
    WE_INT32 aiTemp[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
 
    if (iNum > 11 || iNum < 0)
    {
        return 0;
    }
 
    return aiTemp[iNum];
 
}
/*=====================================================================================
FUNCTION: 
    WeSCL_LeapOneMonthDays
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Venly Feng
DESCRIPTION:
    Get the value in array of the days of one month of leap year.
              
ARGUMENTS PASSED:
    None         
RETURN VALUE:
    Return the value if success, else return 0.   
 
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need to get the leapOneMonthDays.
IMPORTANT NOTES:
    This function call by WeSCL_Time2Str function in this file.
  
=====================================================================================*/
static WE_INT32 WeSCL_LeapOneMonthDays(WE_INT32 iNum)
{
    WE_INT32 aiTemp[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
 
    if (iNum > 11 || iNum < 0)
    {
        return 0;
    }
 
    return aiTemp[iNum];
}
/*=====================================================================================
FUNCTION: 
    WeSCL_GetHexDigits
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Venly Feng
DESCRIPTION:
    Get the value in hex digits table.
              
ARGUMENTS PASSED:
    ucNum :  The order of the table will get         
RETURN VALUE:
    Return the value in the array if success, else return 0.   
 
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need to get the value in hex digits array.
IMPORTANT NOTES:
    This function call by WeSCL_Byte2Hex function in this file.
  
=====================================================================================*/
static WE_CHAR WeSCL_GetHexDigits(WE_UINT8 ucNum)
{
 
    WE_CHAR acHexDigits[16] = 
    {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
    };
    if ( ucNum > 15 )
    {
        return 0;
    }
    return acHexDigits[ucNum];
}
/*=====================================================================================
FUNCTION: 
    WeSCL_SKipTailingBlanks
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Bill Huang
DESCRIPTION:
    Given a string 'pcStrSource' and its length, return the length of that string if trailing 
    white space were skipped. A white space character is either SP, TAB, CR, or LF.
ARGUMENTS PASSED:
    pcStrSource: a pointer to a const character string.
    iLen: the length of the string.
RETURN VALUE:
    Return the length of the string if trailing white space were skipped.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need the length of the string if trailing white space were omitted.
IMPORTANT NOTES:
    None
=====================================================================================*/
WE_INT32 WeSCL_SKipTailingBlanks
(
    const WE_CHAR* pcStrSource, 
    WE_INT32 iLen
)
{
	if (NULL == pcStrSource)
	{
		return -1;
	}
	
    iLen--;
    while ((iLen > 0) && ct_iswhitespace(pcStrSource[iLen]))
    {
        iLen --;
    }
    return iLen + 1;
}

/*=====================================================================================
FUNCTION: 
    WeSCL_SkipField
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Bill Huang
DESCRIPTION:
    Return a pointer to the first white space character in the string 'pcStrSource', or the 
    NULL character terminating the string.A white space character is either SP, TAB, CR, 
    or LF.
ARGUMENTS PASSED:
    pcStrSource: a pointer to a const character string.
RETURN VALUE:
    Return a pointer to the first white space character in the string 'pcStrSource', or the 
    NULL character terminating the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need return a pointer to the first white space character in the 
    string 'pcString', or the NULL character terminating the string.
IMPORTANT NOTES:
    None
=====================================================================================*/
WE_CHAR* WeSCL_SkipField(const WE_CHAR *pcStrSource)
{
	if (NULL == pcStrSource)
	{
		return NULL;
	}

    while (*pcStrSource && !ct_iswhitespace(*pcStrSource))
    {
        pcStrSource ++;
    }
    return (WE_CHAR*)pcStrSource;
}

/*=====================================================================================
FUNCTION: 
    WeSCL_SkipBlanks
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Bill Huang
DESCRIPTION:
    Return a pointer to the first non-blank character in the string 'pcStrSource'. A blank 
    character is either SP or TAB.
ARGUMENTS PASSED:
    pcStrSource: a pointer to a const character string.
RETURN VALUE:
    Return a pointer to the first non-blank character in the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need return a pointer to the first non-blank character in the string.
IMPORTANT NOTES:
    None
=====================================================================================*/
WE_CHAR* WeSCL_SkipBlanks(const WE_CHAR *pcStrSource)
{
	if (NULL == pcStrSource)
	{
		return NULL;
	}

    while ((' ' == *pcStrSource) || ('\t' == *pcStrSource)) 
    {
        pcStrSource ++;
    }
    
    return (WE_CHAR *)pcStrSource;
}

/*=====================================================================================
FUNCTION: 
    WeSCL_Utf8StrIncr
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Bill Huang
DESCRIPTION:
    Given a UTF-8 encoded null-terminated string, step past 'iSteps' Unicode characters 
    and return a pointer to the new position.
ARGUMENTS PASSED:
    pcStrSource: a pointer to a const character string.
    iSteps: the number of bytes that you want to step past Unicode characters.
RETURN VALUE:
    Return a pointer to the new position in the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need return a pointer to the new position in the string.
IMPORTANT NOTES:
    None
=====================================================================================*/
WE_CHAR *WeSCL_Utf8StrIncr
(
    const WE_CHAR *pcStrSource, 
    WE_INT32 iSteps
)
{
	if (NULL == pcStrSource)
	{
		return NULL;
	}
	
    while ((iSteps -- > 0) && (*pcStrSource != '\0')) 
    {
        pcStrSource += (WE_UINT32)WeSCL_GetByteOfUTF8(*pcStrSource);
    }
    return (WE_CHAR *)pcStrSource;
}


/*=====================================================================================
FUNCTION: 
    WeSCL_Utf8CharLen 
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Bill Huang
DESCRIPTION:
    Given a UTF-8 encoded string, return the number of bytes in the first Unicode character.
ARGUMENTS PASSED:
    pcStrSource: a pointer to a const character string.
RETURN VALUE:
    Return the number of bytes in the first Unicode character.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need return the number of bytes in the first Unicode character.
IMPORTANT NOTES: 
    None
=====================================================================================*/
WE_INT32  WeSCL_Utf8CharLen(const WE_CHAR *pcStrSource)
{
    WE_CHAR cGetUTF8Byte = 0;

	if (NULL == pcStrSource)
	{
		return -1;
	}

    if (*pcStrSource == '\0')
    {
        return 0;
    }
    else
    {
        cGetUTF8Byte = WeSCL_GetByteOfUTF8(*pcStrSource);
        return(cGetUTF8Byte);
    }
}

/*=====================================================================================
FUNCTION: 
    WeSCL_Utf8StrLen
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Bill Huang
DESCRIPTION:
    Given a UTF-8 encoded null-terminated string,return the number of Unicode characters
    in the string.
ARGUMENTS PASSED:
    pcString: a pointer to a const character string.
RETURN VALUE:
    The number of Unicode  characters in the string.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need return the number of Unicode characters in the string.
IMPORTANT NOTES:
    None 
=====================================================================================*/
WE_INT32 WeSCL_Utf8StrLen (const WE_CHAR *pcString)
{ 
    WE_UCHAR ucGetUTF8Byte = 0;
    WE_INT32 iCount = 0;
    
    if (NULL == pcString)
    {
        return -1;
    }
    
    while (*pcString != '\0') 
    {
        ucGetUTF8Byte = WeSCL_GetByteOfUTF8(*pcString);
        if (0 == ucGetUTF8Byte)
        {
            return iCount;
        }
        else
        {
            pcString += ucGetUTF8Byte;
            iCount ++;
        }
    }
    return iCount;
}

/*=====================================================================================
FUNCTION: 
    WeSCL_Utf8StrnLen
CREATE DATE: 
    2006-06-07
AUTHOR: 
    Bill Huang
DESCRIPTION:
    Given a UTF-8 encoded string of length 'iLen' bytes,return he number of Unicode 
    characters in the string.
ARGUMENTS PASSED:
    pcStrSource: a pointer to a const character string.
    iLen: the length of the string.
RETURN VALUE:
    The number of Unicode characters in the string
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    All program need the number of Unicode characters in the string that its length is
    'iLen' bytes.
IMPORTANT NOTES:
    None
=====================================================================================*/
WE_INT32 WeSCL_Utf8StrnLen  
(
    const WE_CHAR *pcStrSource,
    WE_INT32      iLen
)
{
    WE_UCHAR ucGetUTF8Byte = 0;
    WE_INT32 iCount = 0; 
    
    if (NULL == pcStrSource)
    {
        return -1;
    }
  
    while ((iLen > 0) && (*pcStrSource != '\0')) 
    {
        ucGetUTF8Byte = WeSCL_GetByteOfUTF8(*pcStrSource);
        if (ucGetUTF8Byte == 0)
        {
            return iCount;
        }
        else
        {
            pcStrSource += ucGetUTF8Byte;
            iLen -= ucGetUTF8Byte;
            iCount++;
        }
        
    }
    return iCount;
}
