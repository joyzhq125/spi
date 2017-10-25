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

/* WE */
#include "We_Cfg.h"
#include "We_Mem.h"
#include "We_Def.h"
#include "We_Cmmn.h"
#include "We_Lib.h"
#include "We_Core.h"
#include "We_Log.h"

/* MMS */
#include "Mms_Def.h"

/* MSA */
#include "Msa_Rc.h"
#include "Msa_Cfg.h"
#include "Msa_Def.h"
#include "Msa_Types.h"
#include "Msa_Mem.h"
#include "Msa_Intsig.h"
#include "Msa_Addr.h"

/*! \file masch.c
 */

/******************************************************************************
 * Constants
 *****************************************************************************/

/* ! Scheme parsing constants */
#define MSA_SCHEME_MMS_TO           "mmsto:"    /*!< */
#define MSA_SCHEME_MAIL_TO          "mailto:"   /*!< The mailto: scheme 
                                                     identifier */
#define MSA_SCHEME_MMS              "mms:"      /*!< The mms: scheme identifier
                                                 */
#define MSA_SCHEME_TO_DELIMITER     '?'         /*!< "To" delimiter */
#define MSA_SCHEME_FIELD_DELIMITER  '&'         /*!< Field delimiter */
#define MSA_SCHEME_HEX              '%'         /*!< Hex identifier */
#define MSA_SCHEME_CC               "cc="       /*!< Cc identifier */
#define MSA_SCHEME_SUBJECT          "subject="  /*!< Subject identifier */

/******************************************************************************
 * Data-structures
 *****************************************************************************/

/******************************************************************************
 * Variables
 *****************************************************************************/

/******************************************************************************
 * Prototypes
 *****************************************************************************/

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief Extracts the subject and formats the string according to the scheme.
 *
 * \param dest This is where the NULL terminated subject string is written.
 * \param subject The string containing the subject
 * \param length The length of the subject
 *****************************************************************************/
static void setSubject(char **dest, const char *subject, unsigned int length)
{
    char *sub;
    char *tmpPtr;
    char *delim;
    char value;
    int i;

    /* Free previous subject */
    MSA_FREE(*dest);
    /* Copy the new subject */
    sub = MSA_ALLOC(length + sizeof(""));
    strncpy(sub, subject, length);
    sub[length] = '\0';
    /* Post processing: exchange '+' to a white space ' ' */
    i = 0;
    while('\0' != sub[i])
    {
        if (sub[i] == '+')
        {
            sub[i] = ' ';
        }
        i++;
    }
    /* Post process the subject and fix the %*/
    delim = sub;
    while(NULL != (delim = strchr(delim, MSA_SCHEME_HEX)))
    {
        /* Check that he format is %xx, where xx is a hex value */
        if (strlen(delim) >= 2 && we_cmmn_hex2byte(&(delim[1]), 
            (unsigned char *)&value))
        {
            /* Set the value at the delimiters place */
            delim[0] = value;
            /* Copy the rest of the string 2 characters to the "left" */
            tmpPtr = &(delim[3]);
            while('\0' != *tmpPtr)
            {
                tmpPtr[-2] = tmpPtr[0];
                tmpPtr += sizeof(char);
            }
            tmpPtr[-2] = '\0';
        }
        /* Skip the delimiter */
        delim += sizeof(char);
    }
    *dest = sub;
}

/*! Sets the addres
 * 
 *****************************************************************************/
static WE_BOOL setAddr(MsaAddrItem **item, const char *str)
{
    char *newValue;
    char *freeData;
    char *delim;
    char *tmpPtr;
    unsigned char value;
    int i;
    WE_BOOL status = FALSE;

    freeData = newValue = MSA_ALLOC(strlen(str) + 1);
    strcpy(newValue, str);
    /* Post process the string */
    /* Post processing: exchange '+' to a white space ' ' */
    i = 0;
    while('\0' != newValue[i])
    {
        if (newValue[i] == '+')
        {
            newValue[i] = ' ';
        }
        i++;
    }
    /* Post process the subject and fix the %*/
    delim = newValue;
    while(NULL != (delim = strchr(delim, MSA_SCHEME_HEX)))
    {
        /* Check that he format is %xx, where xx is a hex value */
        if (strlen(delim) >= 2 && we_cmmn_hex2byte(&(delim[1]), 
            (unsigned char *)&value))
        {
            /* Set the value at the delimiters place */
            delim[0] = (char)value;
            /* Copy the rest of the string 2 characters to the "left" */
            tmpPtr = &(delim[3]);
            while('\0' != *tmpPtr)
            {
                tmpPtr[-2] = tmpPtr[0];
                tmpPtr += sizeof(char);
            }
            tmpPtr[-2] = '\0';
        }
        /* Skip the delimiter */
        delim += sizeof(char);
    }
    /* Trim */
    /* Trim left */
    while(' ' == *newValue)
    {
        newValue = &newValue[1];
    }
    /* Trim right */
    i = 0;
    while((' ' != newValue[i]) && (WE_EOS != newValue[i]))
    {
        i++;
    }
    newValue[i] = WE_EOS;

    /* Verify the address */
    /*lint -e{788} */
    switch(Msa_GetAddrType(newValue))
    {
    case MSA_ADDR_TYPE_EMAIL:
        status = Msa_AddrItemInsert(item, NULL, MSA_ADDR_TYPE_EMAIL, newValue);
        break;
    case MSA_ADDR_TYPE_PLMN:
        status = Msa_AddrItemInsert(item, NULL, MSA_ADDR_TYPE_PLMN, newValue);
        break;
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
            "(%s) (%d) Address rejected: %s\n", __FILE__, __LINE__, newValue));
        status = FALSE;
        break;
    }
    MSA_FREE(freeData); 
    return status;
}


/*!
 * \brief Parse the scheme and present a C-structure with the scheme filled in.
 *
 * \param scheme The scheme, e.g., "mmsto:homer@simpson.com"
 * \return The scheme as a C-structure.
 *****************************************************************************/
MsaPropItem *msaSchemeToPropItem(const char *scheme)
{
    char *schemeStart;
    char *schemeStop;
    char *delimiter;
    char *nextDelimiter;
    char *name;
    char *value;
    char tmpChar;
    unsigned int length;
    MsaPropItem *propItem = NULL;
    int i;
    const char *schemes[4] = {MSA_SCHEME_MMS_TO, MSA_SCHEME_MAIL_TO, 
        MSA_SCHEME_MMS, NULL};

    /* Find scheme type */
    i = 0;
    schemeStart = NULL;
    while(schemes[i] != NULL)
    {
        /*lint -e{605} */
        if (NULL != (schemeStart = strstr(scheme, schemes[i])))
        {
            break;
        }
        ++i;
    }
    if (NULL == schemeStart)
    {
        return propItem;
    }
    schemeStop = schemeStart + strlen(schemes[i])*sizeof(char);
    /* Create the property item */
    propItem = MSA_ALLOC(sizeof(MsaPropItem));
    memset(propItem, 0, sizeof(MsaPropItem));
    /* Find the to field */
    delimiter = strchr(schemeStop, MSA_SCHEME_TO_DELIMITER);
    if (NULL == delimiter)
    {
        /* Fill in to field */
        (void)setAddr(&(propItem->to), schemeStop);
        return propItem;
    }
    else
    {
        tmpChar = schemeStop[delimiter - schemeStop];
        schemeStop[delimiter - schemeStop] = '\0';
        (void)setAddr(&(propItem->to), schemeStop);
        schemeStop[delimiter - schemeStop] = tmpChar;
    }

    /* Find Cc and Subject */
    while(NULL != delimiter)
    {
        /* Find the next delimiter */
        nextDelimiter = strchr(delimiter + sizeof(char), 
            MSA_SCHEME_FIELD_DELIMITER);
        if (NULL != nextDelimiter)
        {
            length = (unsigned)(nextDelimiter - delimiter);
        }
        else
        {
            length = strlen(delimiter);
        }
        /* Check for CC */
        value = NULL;
        if (NULL != (name = strstr(delimiter, MSA_SCHEME_CC)))
        {
            value = &(name[sizeof(MSA_SCHEME_CC) - 1]);
            tmpChar = value[length - sizeof(MSA_SCHEME_CC)];
            value[length - sizeof(MSA_SCHEME_CC)] = '\0';
            (void)setAddr(&(propItem->cc), value);
            value[length - sizeof(MSA_SCHEME_CC)] = tmpChar;
        }
        else if (NULL != (name = strstr(delimiter, MSA_SCHEME_SUBJECT)))
        {
            value = &(name[sizeof(MSA_SCHEME_SUBJECT) - 1]);
            setSubject(&(propItem->subject), value, length - 
                sizeof(MSA_SCHEME_SUBJECT));
        }
        /* Check next delimiter */
        delimiter = nextDelimiter;
    }
    return propItem;
}
