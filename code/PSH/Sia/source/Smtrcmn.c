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

/*! \file smtr_cmn.c
 *  This file contains the control logic for rendering/viewing a text.
 *  PC-LINT OK 2004-02-17 ALYR
 */

/* WE */
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Wid.h"
#include "We_Mem.h"
#include "We_Cmmn.h"

/* SMA */
#include "Samem.h"

/* SMTR */
#include "Smtr.h"
#include "Smtrcmn.h"

/******************************************************************************
 * Constants
 *****************************************************************************/

/******************************************************************************
 * Data-structures
 *****************************************************************************/

/******************************************************************************
 * Static variables
 *****************************************************************************/

/******************************************************************************
 * Prototypes
 *****************************************************************************/

/******************************************************************************
 * Function implementations
 *****************************************************************************/

/*!
 * \brief Duplicate a string
 *
 * \param str The string to duplicate
 * \return The newly allocated string if successful, otherwise NULL.
 *****************************************************************************/
char *smtrStrDup(const char *str)
{
    return we_cmmn_strdup(WE_MODID_SMTR, str);
}

/*!
 * \brief Gets a character buffer from a string handle
 *
 * \param strHandle The string handle to get buffer from
 * \return The newly allocated string if successful, otherwise NULL.
 *****************************************************************************/
char *smtrGetStringBufferFromHandle(WeStringHandle strHandle)
{
    char *tmpStr;
    int length;
    
    if (0 >= (length = WE_WIDGET_STRING_GET_LENGTH(strHandle, TRUE, 
        WeUtf8)))
    {
        return NULL;
    }
    /* Add space for NULL termination*/
    tmpStr = (char *)SMTR_ALLOC(sizeof(char) * 
        (unsigned)length);
    memset(tmpStr, 0, (unsigned int)length);
    /* Get string data */
    if (0 > WE_WIDGET_STRING_GET_DATA(strHandle, tmpStr, WeUtf8))
    {
        /*lint -e{774} */
        SMTR_FREE(tmpStr);
        tmpStr = NULL;
        return NULL;    
    }
    return tmpStr;
    
}

/*!
 * \brief Insert entry in double linked parse list
 *
 * \param charPos Current position in string
 *****************************************************************************/
void smtrInsertParseEntry(int charPos, const char *str, int utf8Len, 
    SmtrScheme scheme, SmtrParseResult **parse)
{
    SmtrParseResult *entry;
    entry = WE_MEM_ALLOCTYPE(WE_MODID_SMTR, SmtrParseResult);
    memset(entry, 0, sizeof(SmtrParseResult));
    entry->scheme = scheme;
    entry->link = smtrStrDup(str);
    entry->utf8Len = utf8Len;
    entry->index = charPos;
    entry->pos.x = entry->pos.y = 0;
    entry->next = *parse;
    entry->prev = NULL;
    if (NULL != *parse) 
    {
        (*parse)->prev = entry;        
    }
    *parse = entry;
}

/*!
 * \brief 
 *
 * \param data Data to free
 *****************************************************************************/
void smtrFreeLinks(SmtrLinkEntry **data)
{
    SmtrLinkEntry *p1;
    while (*data) 
    {
        if ((*data)->link) 
        {
            WE_MEM_FREE(WE_MODID_SMTR, (*data)->link);
            (*data)->link = NULL;
        }
        p1 = (*data);
        (*data) = (*data)->next;
        p1->next = NULL;
        WE_MEM_FREE(WE_MODID_SMTR, p1);
    }
    
}

/*!
 * \brief 
 *
 * \param inst Current instance
 * \return TRUE if succesful, otherwise FALSE.
 *****************************************************************************/
void smtrFreeParseResult(SmtrParseResult **data)
{
    SmtrParseResult *p1;
    while (*data) 
    {
        if ((*data)->link) 
        {
            SMTR_FREE((*data)->link);
            (*data)->link = NULL;
        }
        p1 = (*data);
        (*data) = (*data)->next;
        p1->next = NULL;
        SMTR_FREE(p1);
    }
    
}

/*!
 * \brief Allocates a new link.
 *
 * \param p The parse result
 * \param l (out) The new list
 * \return TRUE if succesful, otherwise FALSE.
 *****************************************************************************/
/*lint -e{621}*/
static void insertLink(const SmtrParseResult *p, SmtrLinkEntry **l)
{
    SmtrLinkEntry *link;
    link = WE_MEM_ALLOCTYPE(WE_MODID_SMTR, SmtrLinkEntry);
    memset(link, 0, sizeof(SmtrLinkEntry));
    link->link = smtrStrDup(p->link);
    link->scheme = p->scheme;
    link->next = *l;
    *l = link;
}

/*!
 * \brief Copies links from the parse result to a new list.
 *
 * \param parse The parse result
 * \param links (out) The new list
 * \return TRUE if succesful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL smtrCopyLinkList(const SmtrParseResult *parse, SmtrLinkEntry **links)
{
    SmtrParseResult *p;
    if (!parse || !links) 
    {
        return FALSE;
    }
    /* Since the list is backwards and we build a stack the new list
     * will be the right order */
    /*lint -e{605}*/
    p = (SmtrParseResult*)parse;
    /* Start copy */
    while (p) 
    {
        insertLink((const SmtrParseResult*)p, links);
        p = p->next;
    }
    return TRUE;
}


void smtrInitRowIndices(SmtrInstance *inst)
{
#define SMTR_MAX_ROWS 100
    inst->rowCount = 0;
    inst->indexTable = (SmtrIndexTableEntry*)
        SMTR_ALLOC(sizeof(SmtrIndexTableEntry) * SMTR_MAX_ROWS);
    memset(inst->indexTable, 0, sizeof(SmtrIndexTableEntry) * SMTR_MAX_ROWS);
}

void smtrAddRowIndex(SmtrInstance *inst, const WE_UINT16 index, 
    const WE_UINT16 width)
{
    SmtrIndexTableEntry *tmp;
    if (inst->rowCount && (0 == (inst->rowCount % SMTR_MAX_ROWS))) 
    {
        /* Not enough rows. Gimme more... */
        tmp = (SmtrIndexTableEntry *)SMTR_ALLOC(sizeof(SmtrIndexTableEntry) * 
            (WE_UINT32)(SMTR_MAX_ROWS + inst->rowCount));
        memcpy(tmp, inst->indexTable, 
            sizeof(SmtrIndexTableEntry) * (WE_UINT32)inst->rowCount);
        memset(tmp + inst->rowCount, 0, 
            sizeof(SmtrIndexTableEntry) * SMTR_MAX_ROWS);
        SMTR_FREE(inst->indexTable);
        inst->indexTable = tmp;
    }
    inst->indexTable[inst->rowCount].utf8Index = index;
    inst->indexTable[inst->rowCount++].width = width;
}

void smtrFreeRowIndices(SmtrInstance *inst)
{
    SMTR_FREE(inst->indexTable);
    inst->indexTable = NULL;
    inst->rowCount = 0;
}

SmtrIndexTableEntry *smtrGetPrevIndex(const SmtrInstance *inst, int currArrayIndex)
{
    if (currArrayIndex > 0 && currArrayIndex < (inst->rowCount - 1)) 
    {
        return &inst->indexTable[currArrayIndex - 1];
    }
    return &inst->indexTable[currArrayIndex];
}

SmtrIndexTableEntry *smtrGetNextIndex(const SmtrInstance *inst, int currArrayIndex)
{
    if (currArrayIndex > 0 && currArrayIndex < (inst->rowCount - 1)) 
    {
        return &inst->indexTable[currArrayIndex + 1];
    }
    return &inst->indexTable[currArrayIndex];
}
