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

/* !\file mautils.c
 * Utility functions for MSA.
 */

/* WE */
#include "We_Mem.h"
#include "We_Def.h"
#include "We_Cmmn.h"
#include "We_Lib.h"
#include "We_Core.h"
#include "We_Log.h"
#include "We_Chrs.h"
#include "We_Hdr.h"

/* MMS */
#include "Mms_Def.h"

/* SIS */
#include "Sis_Cfg.h"
#include "Sis_If.h"

/* MSA */
#include "Msa_Cfg.h"
#include "Msa_Def.h"
#include "Msa_Intsig.h"
#include "Msa_Types.h"
#include "Msa_Mem.h"
#include "Msa_Utils.h"
#include "Msa_Intsig.h"
#include "Msa_Addr.h"
#include "Msa_Intsig.h"
#include "Msa_Rc.h"

/******************************************************************************
 * Data-types
 *****************************************************************************/

/*! \struct MsaCharsetConv
 *	Structure for handling character set type conversin.
 */
typedef struct
{
    WE_UINT32 weCharset;
    MmsCharset mmsCharset;
}MsaCharsetConv;

/******************************************************************************
 * Constants
 *****************************************************************************/
/*! File filter for media objects */
/*lint -e{708} */
static const MsaMediaObjects supportedMediaObjects[] = MSA_CFG_MEDIA_OBJECTS;

/*! Supported character sets for the MSA */
static const MsaCharsetConv supportedCharsets[] = MSA_CFG_SUPPORTED_CHARSETS;

/*! Defines the optional UTF-8 identifier */
#define MSA_UTF8_BOM                    "\xEF\xBB\xBF"

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static void handleUtf8Bom(char *str);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief free all memory of a MmsContentType
 * 
 * \param params The content type parameters to free
 *****************************************************************************/
void msaFreeMmsContentTypeParams(MmsAllParams *params)
{
    MmsAllParams *paramPointer = NULL;  
    WE_BOOL doFree = FALSE;
    
    while (params != NULL)
    {
        paramPointer = params->next;
        MSA_FREE(params->key);
        params->key = NULL;
        if (params->type == MMS_PARAM_STRING)
        {
            MSA_FREE(params->value.string);
            params->value.string = NULL;
        } /* if */
        
        if (!doFree)
        {
            /* do not delete first */
            doFree = TRUE;
        }
        else
        {
            MSA_FREE(params);
        } /* if */
        
        params = paramPointer;
    } /* while */
} /* freeMmsContentTypeParams */

/*! \brief removes a content type parameter
 *	
 * \param params The list of parameters
 * \param paramType The type of param to remove
 *****************************************************************************/
void msaRemoveMmsParam(MmsAllParams **params, MmsParam paramToRemove)
{
    MmsAllParams *tmpParam;
    MmsAllParams *prevParam = NULL;
    tmpParam = *params;
    while (NULL != tmpParam)
    {
        if (tmpParam->param == paramToRemove)
        {
            /* Remove the item from the list */
            if (NULL != prevParam)
            {
                prevParam->next = tmpParam->next;
            }
            else
            {
                /* Change the first item */
                *params = tmpParam->next;
            }
            /* Deallocate the item to remove */
            if (MMS_PARAM_STRING == tmpParam->type)
            {
                MSA_FREE(tmpParam->value.string);
            }
            MSA_FREE(tmpParam);
            /* The operation is done */
            return;
        }
        /* Next item */
        prevParam = tmpParam;
        tmpParam  = tmpParam->next;
    }
}

/*!
 * \brief Generates a name for a file
 * \param extension What extension to set on the file. (max 3 char)
 * \return The name or NULL on error
 *****************************************************************************/
char *msaCreateFileName(const char *extension)
{
    char *name = NULL;
    unsigned int size = 0;
    static WE_UINT32 counter; /* Initialized to zero at start-up */
    WE_UINT32 currentTime = 0;
    unsigned int extensionSize = 0;

    if (NULL != extension)
    {
        extensionSize = strlen(extension);
    }

    /* create name for text */
    currentTime = WE_TIME_GET_CURRENT();

    /* current-time + "." + extension + NULL */
    size = (sizeof(WE_UINT32)*2 + 1 + extensionSize + 1);
    
    name = MSA_ALLOC((unsigned long)size);
    memset(name, 0, size);
    if (NULL != extension)
    {
        /* Use the extension */
        /*lint -e{727} */
        sprintf(name, "%lx.%s", (long)(currentTime<<8)|(counter&0xff), 
            extension);
    }
    else
    {
        /* Skip the extension */
        /*lint -e{727} */
        sprintf(name, "%lx", (long)(currentTime<<8)|(counter&0xff));
    }

    /* Use a counter to prevent problems if the CPU is to fast, i.e., 
       the timer has not been increased between two calls, 256 calls
       to msaCreatefileName during the same time is not likely */
    ++counter;
    return name;
} /* createFileName */

/*!
 * \brief Creates a unique filename from the 
 *
 * \param mimeType The media type to create a unique filename from.
 * \return The name filename, or NULL. The caller is responsible for 
 *         de-allocating the string.
 *****************************************************************************/
char *msaCreateFileNameFromType(const char * mimeType)
{
    unsigned int i;
    const MsaMediaObjects *items;
    /*lint -e{1776}*/
    /*lint -e{788}*/
    items = supportedMediaObjects;
    for (i = 0; 
        i < sizeof(supportedMediaObjects)/sizeof(supportedMediaObjects[0]); i++)
    {
        if (0 == we_cmmn_strcmp_nc(mimeType, items[i].mimeType))
        {
            return msaCreateFileName(items[i].fileExtension);
        }
    }
    /* The type could not be determined, do not create the extension */
    return msaCreateFileName(NULL);
}

/*! \brief converts between #MsaMediaType and #MsaObjectType.
 *
 * \param mediaType The media type to convert.
 * \return The corresponding #MsaObjectType.
 *****************************************************************************/
MsaMediaGroupType msaMimeToObjGroup(const char *mimeType)
{
    unsigned int i;
    const MsaMediaObjects *items;

    items = supportedMediaObjects;
    for (i = 0; i < sizeof(supportedMediaObjects)/sizeof(supportedMediaObjects[0]); i++)
    {
        if (0 == we_cmmn_strcmp_nc(mimeType, items[i].mimeType))
        {
            return items[i].objectType;
        }
    }
    return MSA_MEDIA_GROUP_NONE;
}

/*! \brief Appends two address lists.
 *
 *  After this call the only valid MsaAddrItem is the returned one.
 *
 * \param first The first part of the new list 
 * \param second The second part of the new list 
 * \return The new list.
 *****************************************************************************/
MsaAddrItem *msaAppendAddrItems(MsaAddrItem *first, MsaAddrItem *second)
{
    MsaAddrItem *tmp;

    if (NULL == first)
    {
        return second;
    }
    for(tmp = first; NULL != tmp->next; tmp = tmp->next)
        ;
    tmp->next = second;
    return first;
}

/*!
 * \brief       Parses a property item (MSA data structure) and frees all data.
 *              Beware of static strings!
 * \param prop  The property item to parse and free
 * \param dynamicOnly TRUE if memory allocated in the structure is 
 *              de-allocated, i.e., the structure is not released.
 *****************************************************************************/
void msaFreePropItem(MsaPropItem *prop, WE_BOOL dynamicOnly)
{
    if (NULL == prop)
    {
        return;
    }
    /* Delete addresses */
    Msa_AddrListFree(&(prop->to));
    Msa_AddrListFree(&(prop->cc));
    Msa_AddrListFree(&(prop->bcc));
    Msa_AddrListFree(&(prop->from));
    /* Subject */
    MSA_FREE(prop->subject);
    prop->subject = NULL;
    MSA_FREE(prop->replyChargingId);
    prop->replyChargingId = NULL;

    /* Check if the structure needs to be de-allocated */
    if (!dynamicOnly)
    {
        /*lint -e{774} */
        MSA_FREE(prop);
    }
}

/*!
 * \brief Checks if the character type can be displayed.
 *
 * \param charset The #MmsCharset character type.
 * \return a charset if the character can be used, otherwise WE_CHARSET_UNKNOWN
 *****************************************************************************/
WE_UINT32 msaIsValidCharset(MmsCharset charset)
{
    const MsaCharsetConv *current = supportedCharsets;
    unsigned int i;
    /* Check if it is possible to do display the charset type*/
    for (i = 0; i < sizeof(supportedCharsets)/sizeof(supportedCharsets[0]); i++)
    {
        if (current[i].mmsCharset == charset)
        {
            return current[i].weCharset;
        }
    }
    return WE_CHARSET_UNKNOWN;
}

/*! \brief Removes the optional UTF-8 mark if it is present
 *
 * \param str The string to remove the "possible" UTF-8 mark from.
 *****************************************************************************/
static void handleUtf8Bom(char *str)
{
    if (NULL != str)
    {
        /* Check if the optional UTF-8 identifier is present */
        if ((strlen(str) > strlen(MSA_UTF8_BOM)) && 
            (memcmp(str, MSA_UTF8_BOM, strlen(MSA_UTF8_BOM)) == 0))
        {
            /* Move the content back in the buffer so that the identifier is
               removed */
            memmove(str, &(str[strlen(MSA_UTF8_BOM)]), 
                strlen(str) - strlen(MSA_UTF8_BOM) + 1);
        }
    }
}

/*!
 * \brief Converts text to UTF-8.
 *
 * \return TRUE if the conversion was successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaConvertToUTF8(int charset, WE_UINT32 *size, unsigned char **text)
{
    /* MMS to WE type conversion */
    we_charset_convert_t *convFunc;
    long destSize;
    long srcSize;
    char *destBuffer;
    int length;

    /* Handle types that do not need any conversion */
    if ( (charset == WE_CHARSET_UTF_8) || 
         (charset == WE_CHARSET_ASCII))
    {
        /* Allocate the old text size + room for the '\0' termination */
        MSA_CALLOC(destBuffer, *size + 1);
        memcpy(destBuffer, *text, *size);
        /* Free the old data */
        MSA_FREE(*text);
        /* Set the new buffer */
        *size = strlen(destBuffer) + 1;
        *text = (unsigned char *)destBuffer;
        /* Removes the option UTF8 identifier if present */
        handleUtf8Bom((char *)*text);
        return TRUE;
    }
    /* Handle character set conversion */
    length = (int)*size;
    convFunc = we_charset_find_function_bom(charset, WE_CHARSET_UTF_8, 
        (char *)*text, &length);
    if (NULL != convFunc)
    {
        srcSize = *size;

        /* Find buffer size, tell WE we have lots of memory available */
        destSize = (long)((unsigned long)-1 >> 1);
        if (0 != convFunc((char *)*text, &srcSize, NULL, &destSize))
        {
            return FALSE;
        }
        /* Allocate buffer, allocate one extra byte so that there is always
           space for the '\0' termination */
        MSA_CALLOC(destBuffer, (unsigned long)destSize + sizeof(char));

        /* Do the conversion */
        if (0 != convFunc((char *)*text, (long *)size, destBuffer, 
            &destSize))
        {
            /*lint -e{774}*/
            MSA_FREE(destBuffer);
            return FALSE;
        }
        MSA_FREE(*text);
        *text = (unsigned char*)destBuffer;
        *size = strlen(destBuffer) + 1;
        /* Removes the option UTF8 identifier if present */
        handleUtf8Bom((char *)*text);
        return TRUE;
    }
    return FALSE;
}

/*! \brief Removes the slash '/' character from the file path.
 *
 * \param The filename including the path. This string is changed after 
 *        completion of this function.
 *****************************************************************************/
void msaRemPathFromFileName(char *fileName)
{
    char *tmp;
    unsigned int i;
    if (NULL == fileName)
    {
        return;
    }
    tmp = fileName;
    /* Find the last '/' */
    while(NULL != strchr(tmp, '/'))
    {
        /* get next "/"*/
        /*lint -e{613} */
        tmp = strchr(tmp, '/');
        /*lint -e{613} */
        tmp = &(tmp[1]);
    }
    /* Remove the "path" part  */
    for (i = 0; i < strlen(tmp) + 1; i++)
    {
        fileName[i] = tmp[i];
    }
}

/*!
 * \brief Converts a MsaMediaType to a SisMediaType
 *
 * \param mimeType The MIME-type of the object.
 * \return #SisMediaType Converted media type
 *****************************************************************************/
SisMediaType msaMimeToSis(const char *mimeType)
{
    unsigned int i;
    const MsaMediaObjects *items;
    /*lint -e{788} */
    items = supportedMediaObjects;
    for (i = 0; i < sizeof(supportedMediaObjects)/sizeof(supportedMediaObjects[0]); i++)
    {
        if (0 == we_cmmn_strcmp_nc(mimeType, items[i].mimeType))
        {
            return items[i].sisMediaType;
        }
    }
    return SIS_NOT_USED;
} /* convertMediaTypeToSis */

/*! \brief Gets the text-type used by the MSA, i.e., the first text occurrence 
 *         in the table of valid content-types.
 *
 * \return The mime-type used for text in the MSA-application.
 *****************************************************************************/
const char *msaGetTextMimeType()
{
    unsigned int i;
    const MsaMediaObjects *items;
    /*lint -e{788} */
    items = supportedMediaObjects;
    for (i = 0; i < sizeof(supportedMediaObjects)/sizeof(supportedMediaObjects[0]); i++)
    {
        if (MSA_MEDIA_GROUP_TEXT == items[i].objectType)
        {
            return items[i].mimeType;
        }
    }
    return NULL;
}
