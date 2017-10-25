/*
 * Copyright (C) Techfaith, 2002-2005.
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

/* !\file smtr_cmn.h
 *  \brief Common data-types and constants used internally by the SMTR.
 */

#ifndef SMTRCMN_H
#define SMTRCMN_H

#include "Smtr.h"

/******************************************************************************
 * Constants
 *****************************************************************************/

#define WE_MODID_SMTR WE_MODID_SIA

#define SMTR_ALLOC SIA_ALLOC

#define SMTR_FREE SIA_FREE

/*!
 *  NOTE! Since regular expressions are not supported there is special
 *  handling of dynamic schemes.
 */
#define SMTR_SUPPORTED_SCHEMES  \
    {{SMTR_SCHEME_HTTP, "http://", sizeof("http://") - 1},\
     {SMTR_SCHEME_HTTPS, "https://", sizeof("https://") - 1},\
     {SMTR_SCHEME_WWW, "www.", sizeof("www.") - 1},\
     {SMTR_SCHEME_MAIL, NULL, 0},\
     {SMTR_SCHEME_PHONE, NULL, 0},\
     {SMTR_SCHEME_WAP, "wap.", sizeof("wap.") - 1},\
     {SMTR_SCHEME_NONE, NULL, 0}\
    }

#define IS_SET(flags, flag)   (((flags) & (flag)) == (flag))

/******************************************************************************
 * Data-types
 *****************************************************************************/


/*! \struct SmtrParseResult
 */
typedef struct _SmtrParseList  
{
    int index;
    SmtrScheme scheme;
    char *link;
    int utf8Len;
    WePosition pos;
    WE_BOOL isSelected;
    struct _SmtrParseList *next;
    struct _SmtrParseList *prev;
}SmtrParseResult;

/*! \struct SmtrIndexTableEntry
 */
typedef struct
{
    WE_UINT16 utf8Index;
    WE_UINT16 width;
}SmtrIndexTableEntry;

/*! \struct SmtrInstance
 *  \brief  A data structure for keeping instance data.
 */
typedef struct 
{
    int instance;                   /*!< The instance number for the text box */
    WeGadgetHandle gadgetHandle;   /*!< The handle to the gadget which holds the rendered image */
    WeStringHandle stringHandle;   /*!< The handle to the string to render */
    SmtrCreateTextBoxProperties prop;/*!< The create properties from user */

    WeSize boxSize;                /*!< Maximum size to draw in */
    /* Result data */
    SmtrParseResult *parse;          /*!< The result from the parsing */
    WeSize actualSize;             /*!< The size of the image after rendering */
    /* State data */
    SmtrParseResult *selectedLink;   /*!< Current selected link */
    WeEventType currentEvent;      /*!< The event which caused a redraw */
    WE_BOOL fullScreen;            /*!< Set to TRUE if entire text fits on screen */
    WE_BOOL lostFocus;             /*!< TRUE if the text box should lose focus */
    WE_BOOL redraw;                /*!< TRUE if the rendering is a redraw */
    WE_INT16 rowHeight;            /*!< The height of one row in pixels */
    int totalLength;                /*!< Total number of UTF8-characters in string */
    char *charBuffer;               /*!< A character buffer representation of the widget string */
    SmtrIndexTableEntry *indexTable; /*!< The index table */
    int currentIndex;               /*!< Current index in the index table */
    int rowCount;                   /*!< Number of used rows in the index table */
} SmtrInstance;

void smtrInitRowIndices(SmtrInstance *inst);
void smtrAddRowIndex(SmtrInstance *inst, const WE_UINT16 index, 
    const WE_UINT16 width);
void smtrFreeRowIndices(SmtrInstance *inst);
SmtrIndexTableEntry *smtrGetPrevIndex(const SmtrInstance *inst, int currArrayIndex);
SmtrIndexTableEntry *smtrGetNextIndex(const SmtrInstance *inst, int currArrayIndex);

void smtrFreeLinks(SmtrLinkEntry **data);
void smtrInsertParseEntry(int charPos, const char *str, int utf8Len, 
    SmtrScheme scheme, SmtrParseResult **parse);
char *smtrGetStringBufferFromHandle(WeStringHandle strHandle);
char *smtrStrDup(const char *str);
void smtrFreeParseResult(SmtrParseResult **data);
WE_BOOL smtrCopyLinkList(const SmtrParseResult *parse, SmtrLinkEntry **links);
#endif

