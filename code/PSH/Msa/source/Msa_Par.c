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

/*! \file mtr_par.c
 *  This file contains the control logic for rendering/viewing a text.
 *  PC-LINT OK 2004-02-17 ALYR
 */

/* WE */
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Cmmn.h"
#include "We_Chrt.h"

/* MTR */
#include "Msa.h"
#include "Msa_Cmn.h"
#include "Msa_Par.h"

/******************************************************************************
 * Constants
 *****************************************************************************/
#define IS_DIGIT(a)     ((a) >= '0' && (a) <= '9')

#define IS_PHONE_NO_CHARACTER(c)   ((c == '+') || (c == '-') || (c == '0') || \
    (c == '1') || (c == '2') || (c == '3') || \
    (c == '4') || (c == '5') || (c == '6') || \
    (c == '7') || (c == '8') || (c == '9'))


/*! Valid characters in a host name */
#define IS_HOST_LETTER(x) ( \
    (x) >= '0' && (x) <= '9' || \
    (x) >= 'a' && (x) <= 'z' || \
    (x) >= 'A' && (x) <= 'Z' || \
    (x) == '.' || (x) == '-' || (x) == '_')

/*! Valid characters in a email address */
#define IS_EMAIL_LETTER(x) (IS_HOST_LETTER(x) || (x) == '@' || (x) == '+')


/******************************************************************************
 * Data-structures
 *****************************************************************************/
typedef struct  
{
    MtrScheme scheme;
    const char *str;
    unsigned int size;
}MtrSchemesTable;

/******************************************************************************
 * Static variables
 *****************************************************************************/
static const MtrSchemesTable schemes[] = MTR_SUPPORTED_SCHEMES;

/******************************************************************************
 * Prototypes
 *****************************************************************************/

/******************************************************************************
 * Function implementations
 *****************************************************************************/

/*!
 * \brief Checks if the 'len' first characters in str are digits.
 *
 * \param sch The table of available schemes.
 * \param str The string to match against the table
 * \param scheme (out) The type of scheme if the string was a link, otherwise
 *  FALSE.
 * \return TRUE if the string was a link, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL isLink(const MtrSchemesTable *sch, char *str, MtrScheme *scheme)
{
    int i;
    WE_BOOL valid = TRUE;
    char *s = str;
    if (NULL == str) 
    {
        return FALSE;
    }
    for (i = 0; MTR_SCHEME_NONE != sch[i].scheme; i++)
    {
        *scheme = sch[i].scheme;
        /* Special handling for mail and phone number */
        if (MTR_SCHEME_MAIL == sch[i].scheme) 
        {
            /* Very simple, check for @ */
            if (NULL != we_cmmn_strnchr(str, '@', (int)strlen(str)))
            {
                valid = TRUE;
                /* Validate each character to be address safe */
                for (s = str; s && *s != '\0'; s++)
                {
                    if (!IS_EMAIL_LETTER(*s))
                    {
                        valid = FALSE;
                        break;
                    }
                }          
                if (valid) 
                {
                    return TRUE;
                }
            }
        }
        else if (MTR_SCHEME_PHONE == sch[i].scheme) 
        {
            if (strlen(str) > 2)
            {
                valid = TRUE;
                /* Validate each character to be phone number safe */
                for (s = str; s && *s != '\0'; s++)
                {
                    if (!IS_PHONE_NO_CHARACTER(*s))
                    {
                        valid = FALSE;
                        break;
                    }
                }                
                if (valid) 
                {
                    return TRUE;
                }
            }
        }
        /* Not special scheme, just string compare */
        else if (0 == we_cmmn_strncmp_nc(sch[i].str, str, 
            (int)sch[i].size))
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*!
 * \brief The actual parser.
 *
 * \param str The string to find links in.
 * \param result (out) The parse result.
 * \return TRUE on successful parse, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL doParse(char *str, MtrParseResult **result)
{
    char *oldPos;
    char *endPos;
    char tmpChar;
    int utf8Len, utf8Pos;
    MtrScheme scheme;

    if (NULL == str)
    {
        return FALSE;
    }
    utf8Pos = 0;
    /* Explode */
    while(str[0] != '\0')
    {
        oldPos = str;
        /* Skip white-space */
        while (('\0' != *str) && (ct_iswhitespace (*str) || ct_isblank(*str)))
        {
            str++;
            utf8Pos++;
        }
        endPos = str;
        /* Find the end of the block */
        while (('\0' != *endPos) && (!ct_iswhitespace(*endPos) && 
            !ct_isblank(*endPos)))
        {
            endPos++;
        }
        tmpChar = *endPos;
        *endPos = '\0';
        utf8Len = we_cmmn_utf8_strlen(str);
        /* Check for a valid link */
        if (isLink(schemes, str, &scheme)) 
        {
            /* One link found, insert the new item */
            Msa_InsertParseEntry(utf8Pos, str, utf8Len, scheme, result);
        }
        utf8Pos += utf8Len;
        /* Restore the white-space */
        *endPos = tmpChar;
        /* Increase pointer in order to check the next word */
        str = endPos;
        /* Check if the pointer has been increased in order to prevent an 
           infinite loop*/
        str = (oldPos == str) ? endPos + 1: endPos;
    }
    return TRUE;
}

/*!
 * \brief Main function for parsing text data
 *
 * \param inst Current instance.
 * \return TRUE on success, otherwise FALSE.
 *****************************************************************************/
WE_BOOL mtrParse(MtrInstance *inst)
{
    char *str;
    /* Get character buffer from string handle */
    str = Msa_GetStringBufferFromHandle(inst->stringHandle);
    inst->charBuffer = str;
    /* Parse the text */
    return doParse(str, &(inst->parse));
}

