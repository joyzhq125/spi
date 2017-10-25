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

/*! \file mtr_rend.c
 *  This file contains the control logic for rendering/viewing a text.
 *  PC-LINT OK 2004-02-17 ALYR
 */
/* the prototype of methods calculateImageSize and  createIndexTable have
 * been changed to accept a current WeFont as parameter. The calculation
 * will use the current WeFont than default. TR 17667
 */
/* WE */
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Wid.h"
#include "We_Cmmn.h"
#include "We_Mem.h"
#include "We_Core.h"

/* MMS */
#include "Mms_Def.h"

/* MSA */
#include "Msa_Mem.h"
#include "Msa_Def.h"
#include "Msa_Cfg.h"
#include "Msa_Types.h"
#include "Msa_Intsig.h"
#include "Msa_Conf.h"

/* MTR */
#include "Msa.h"
#include "Msa_Cmn.h"
#include "Msa_Rend.h"

/******************************************************************************
 * Constants
 *****************************************************************************/

#define IS_NEWLINE(c) ((c)==0x0A||(c)==0x0D)

#define MTR_BASELINE_SIZE        (1)

#define MTR_LINK_COLOR {0, 0, 255}

/******************************************************************************
 * Data-structures
 *****************************************************************************/

/******************************************************************************
 * Static variables
 *****************************************************************************/
static MtrInstance *currInst;

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static WE_BOOL linkExists(int startPos, int endPos);

/******************************************************************************
 * Function implementations
 *****************************************************************************/

/*!
 * \brief Creates an empty image to draw in.
 *
 * \param prop
 * \param size
 * \return The image handle on success, otherwise 0.
 *****************************************************************************/
static WeImageHandle createEmptyImage(const WeStyleHandle style, 
    const WeSize *size)
{
    WeImageHandle imgHandle;

    /* Create an empty image */
    if (0 == (imgHandle = WE_WIDGET_IMAGE_CREATE_EMPTY(WE_MODID_MTR, 
        (WeSize *)size, style)))
    {
        return 0;
    }
    return imgHandle;
}

/*!
 * \brief Checks if any links are present within current character range.
 *
 * \param startPos Start index.
 * \param endPos End index.
 * \return TRUE if a link exists within specified range, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL linkExists(int startPos, int endPos)
{
    MtrParseResult *p;

    p = currInst->parse;
    while (p) 
    {
        if (p->index >= startPos && p->index < endPos) 
        {
            return TRUE;
        }
        /* Since the list is backwards we can stop here */
        if (p->index < startPos) 
        {
            /* Since the list is backwards we can stop here */
            return FALSE;
        }
        p = p->next;
    }
    return FALSE;
}

/*!
 * \brief Finds the first link in the text.
 *
 * \return The link if it exists, otherwise NULL.
 *****************************************************************************/
static MtrParseResult *getFirstLink()
{
    MtrParseResult *p, *head = NULL;
    
    p = currInst->parse;
    while (p)
    {
        head = p;
        p = p->next;
    }
    return head;    
}

/*!
 * \brief Finds the next link in logical order.
 *
 * \return The link if it exists, otherwise NULL.
 *****************************************************************************/
static MtrParseResult *getNextLink(const MtrParseResult *link)
{
    if (link) 
    {
        /* prev is returned since we have a stack, not a queue */
        return link->prev;    
    }
    return NULL;
}

/*!
 * \brief Finds the previous link in logical order.
 *
 * \return The link if it exists, otherwise NULL.
 *****************************************************************************/
static MtrParseResult *getPrevLink(const MtrParseResult *link)
{
    if (link) 
    {
        /* next is returned since we have a stack, not a queue */
        return link->next;    
    }
    return NULL;
}

/*!
 * \brief Finds next visible link within current character range.
 *
 * \param startPos Start index.
 * \param endPos End index.
 * \return The link if it exists within specified range, otherwise NULL.
 *****************************************************************************/
static MtrParseResult *getNextVisibleLink(int startPos, int endPos)
{
    MtrParseResult *p;
    
    p = getFirstLink();
    while (p) 
    {
        if (p->index >= startPos && p->index < endPos) 
        {
            return p;
        }
        /* We are outside visible area now so we can stop */
        if (p->index >= endPos) 
        {
            /* Since the list is backwards we can stop here */
            return NULL;
        }
        p = getNextLink(p);
    }
    return NULL;
}

/*!
 * \brief Draws a link and the space before it if it exists.
 * \param imgHandle
 * \param strHandle
 * \param link
 * \param pos
 * \param index
 * \param chars
 * \param size
 * \return The new index on success, otherwise -1.
 *****************************************************************************/
static int drawLink(WeImageHandle imgHandle, WeStringHandle strHandle, 
    const MtrParseResult *link, WePosition *pos, int index, int chars, 
    const WeSize *size, WE_BOOL hideCursor)
{
    WeTextProperty tmpProp, textProp;
    WeColor tmpColor, tmpBgColor;
    WeColor color = MTR_LINK_COLOR;
    WeColor bgColor = {0, 0, 0};
    WeSize tmpSize;

    /* Draw the space before the link, if any */
    if (link->index > index)
    {
        if (WE_WIDGET_STRING_GET_WIDTH(strHandle, 0, index, 
            link->index - index, &tmpSize.width) < 0)
        {
            return -1;
        }
        if (WE_WIDGET_DRAW_STRING(imgHandle, strHandle, pos, size, index, 
            link->index - index, 1) < 0) /* Q04A */
        {
            return -1;
        }
        pos->x = (WE_INT16)(pos->x + tmpSize.width);
        index = link->index;
    }
    if (WE_WIDGET_STYLE_GET_DEFAULT(WeStringStyle, &tmpColor, &tmpBgColor,
        0, 0, 0, 0, &textProp) < 0)
    {
        memcpy(&tmpColor, &color, sizeof(WeColor));
    }
    if (link->isSelected && !hideCursor) 
    {
        /* Swap foreground and background color */
        (void)WE_WIDGET_SET_COLOR(strHandle, &tmpColor, 1);
        (void)WE_WIDGET_SET_COLOR(strHandle, &tmpBgColor, 0);                    
    }
    else
    {
        if (!tmpBgColor.r && !tmpBgColor.g && 255 == tmpBgColor.b) 
        {
            (void)WE_WIDGET_SET_COLOR(strHandle, &bgColor, 0);
        }
        else
        {
            (void)WE_WIDGET_SET_COLOR(strHandle, &color, 0);
        }
    }
    tmpProp.decoration = textProp.decoration;
    textProp.decoration = WE_TEXT_DECORATION_UNDERLINE;
    if (WE_WIDGET_SET_TEXT_PROPERTY(strHandle, &textProp) < 0)
    {
        return -1;
    }
    if (WE_WIDGET_DRAW_STRING(imgHandle, strHandle, pos, size, index, 
        chars, 0) < 0) /* Q04A */
    {
        return -1;
    }
    /* Restore styles */
    (void)WE_WIDGET_SET_COLOR(strHandle, &tmpColor, 0);
    if (link->isSelected) 
    {
        (void)WE_WIDGET_SET_COLOR(strHandle, &tmpBgColor, 1);
    }
    textProp.decoration = tmpProp.decoration;
    if (WE_WIDGET_SET_TEXT_PROPERTY(strHandle, &textProp) < 0)
    {
        return -1;        
    }
    if (WE_WIDGET_STRING_GET_WIDTH(strHandle, 0, index, 
        chars, &tmpSize.width) < 0)
    {
        return -1;
    }
    /* Increase index/position counters */
    index += chars;
    pos->x = (WE_INT16)(pos->x + tmpSize.width);
    return index;
}

/*!
 * \brief .
 * \param 
 * \return success, otherwise -1.
 *****************************************************************************/
static WE_BOOL createIndexTable(MtrInstance *inst, WeFont *currFont)
{
    WeStringHandle strHandle;
    we_cmmn_line_break_t trav;
    char *utf8Str;
    WeSize tmpSize = {0,0};
    int lines, i, visibleChars, index;
	WeStyleHandle style = 0;
    WE_BOOL vertResize, isFirst = TRUE;
    
	strHandle = inst->stringHandle;
	/*TR 17667*/
	if(currFont){
		style = WE_WIDGET_STYLE_CREATE(WE_MODID_MTR, &inst->prop.textColor, 
        &inst->prop.bgColor, NULL, NULL, NULL, currFont, NULL); 
	}
	/**************/
    memset(&trav, 0, sizeof(we_cmmn_line_break_t));
    
    /* At least one line must be visible */
    vertResize = IS_SET(inst->prop.flags, MTR_PROP_VERTICAL_RESIZE);
    if (vertResize) 
    {
        tmpSize.height = 0; 
    }
    else 
    {
        tmpSize.height = inst->boxSize.height; 
    }
    tmpSize.width = inst->boxSize.width;    
    utf8Str = Msa_GetStringBufferFromHandle(strHandle);    
    trav.continue_at = utf8Str;

    while (trav.continue_at && *trav.continue_at) 
    {
        trav.start_idx = (isFirst ? isFirst=FALSE, 0 : trav.end_idx + 1); /* + trav.no_end_ws;*/
        /* Check how many characters that can be displayed, TR 17667 */
        if (!we_cmmn_next_line_break(trav.continue_at, strHandle, (WeBrushHandle)style, 
            tmpSize.width, trav.start_idx, FALSE, &trav)) 
        {
            MTR_FREE(utf8Str);
            return FALSE;
        }
        /*  Add current row to the rows array. This call is after 
         *  we_cmmn_next_line_break since we need the width also
         *  start_idx is the same as before the call.
         */
        if (trav.fits) 
        {
            Msa_AddRowIndex(inst, (WE_UINT16)trav.start_idx, (WE_UINT16)trav.width);
        }
        else
        {
            /* We need to split the row in parts */
            /* Calculate nuber of rows to span */
            lines = (trav.width % tmpSize.width ? 
                (trav.width / tmpSize.width) + 1 : (trav.width / tmpSize.width));
            index = trav.start_idx;
            for (i = 0; i < lines; i++)
            {
                /* Since we_cmmn_next_line_break does not support breaking
                 * inside words we have to use HDI-calls instead for calculating
                 * the width of each row the word will span 
                 */
                (void)WE_WIDGET_STRING_GET_NBR_OF_CHARS(strHandle, 0, 
                    index, tmpSize.width, &visibleChars);
                if (WE_WIDGET_STRING_GET_WIDTH(strHandle, 0, index, 
                    visibleChars, &trav.width) < 0)
                {
                    MTR_FREE(utf8Str);
                    return FALSE;
                }
                Msa_AddRowIndex(inst, (WE_UINT16)index, (WE_UINT16)trav.width);
                index += visibleChars;
            }
        }
    }
    MTR_FREE(utf8Str);
    return TRUE;
}

/*!
 * \brief .
 * \param 
 * \return success, otherwise -1.
 *****************************************************************************/
static WE_BOOL calculateImageSize(const MtrInstance *inst, const WeSize *maxSize,
    WeSize *size, WE_INT16 *rowHeight, WeFont *currFont)
{
    WE_BOOL vertResize, horizResize;
    WeSize tmpSize = {0, 0};
    int maxRows;
    int gadgetHeight;
    WeStringHandle strHandle;
    int fontHeight;
    WeFont font;
    int tmp;
	WeStyleHandle style = 0;
    
    if (0 == maxSize->width) 
    {
        return FALSE;
    }

    /* Check flags */
    vertResize = IS_SET(inst->prop.flags, MTR_PROP_VERTICAL_RESIZE);
    horizResize = IS_SET(inst->prop.flags, MTR_PROP_HORIZONTAL_RESIZE);
    strHandle = inst->stringHandle;


    /* Get the font to determine the height */
	/* TR 17667 */
	if(currFont){
	   font = *currFont;
	   style = WE_WIDGET_STYLE_CREATE(WE_MODID_MTR, &inst->prop.textColor, 
        &inst->prop.bgColor, NULL, NULL, NULL, currFont, NULL);
	}
	else{
       if (WE_WIDGET_STYLE_GET_DEFAULT(WeStringStyle, NULL, NULL, NULL, NULL, 
        NULL, &font, NULL) < 0) 
	   {
           return FALSE;
		}
	}
    
    /* Get the heigh of the text in order to calculate the height of the 
    gadget later on */
    if (WE_WIDGET_FONT_GET_VALUES(&font, &tmp, &fontHeight, &tmp) < 0)
    {
        return FALSE;
    }
    *rowHeight = (WE_INT16)(fontHeight + MTR_BASELINE_SIZE);
    /* Check if the text can be displayed on one line */
    /* At least one line must be visible */
    if (vertResize) 
    {
        tmpSize.height = 0; 
    }
    else 
    {
        tmpSize.height = maxSize->height; 
    }
    tmpSize.width = maxSize->width;
    gadgetHeight = 0;
    if (1 == inst->rowCount)
    {
        /* The text can be displayed on one line, shrink the width */
        if (WE_WIDGET_STRING_GET_WIDTH(strHandle, (WeBrushHandle)style, 0, inst->totalLength, 
            &tmpSize.width) < 0)
        {
            return FALSE;
        }
        if (horizResize) 
        {
            size->width  = tmpSize.width;
        }
        else
        {
            size->width = maxSize->width;
        }
        if (vertResize) 
        {
            size->height = *rowHeight;
        }
        else
        {
            size->height = maxSize->height;
        }
    }
    else
    {
        maxRows = maxSize->height / *rowHeight;
        gadgetHeight = *rowHeight * maxRows;
        if (inst->rowCount < maxRows) 
        {
            gadgetHeight = *rowHeight * inst->rowCount;
        }
        if (vertResize && gadgetHeight < maxSize->height)
        {
            size->height = (WE_INT16)gadgetHeight;
        }
        else
        {
            size->height = maxSize->height;
        }

        size->width  = maxSize->width;
    }
    return TRUE;
}

/*!
 * \brief Draws a link and the space before it if it exists.
 * \param imgHandle
 * \param strHandle
 * \param link
 * \param pos
 * \param index
 * \param chars
 * \param size
 * \return The new index on success, otherwise -1.
 *****************************************************************************/
static WeImageHandle doRender(WeStringHandle strHandle,
    const WeSize *size, WE_INT16 rowHeight, WeStyleHandle style)
{
    int visibleChars;
    int toRend = 0;
    int index, rowIndex;
    int rowEnd, endIndex;
    MtrParseResult *link = NULL;
    WeImageHandle imgHandle;
    WePosition pos = {0, 0};
    WE_BOOL linkSpan = FALSE;
    WE_BOOL renderLinks, hideCursor;
    MtrIndexTableEntry *p;
    char *charPos = NULL;
    int align; 
	MsaConfig *mc = NULL;
	
	mc = msaGetConfig();
    align = (int) mc->alignment;
	
    renderLinks = IS_SET(currInst->prop.flags, MTR_PROP_RENDER_LINKS);
    hideCursor = IS_SET(currInst->prop.flags, MTR_PROP_HIDE_CURSOR);

    /* Create an empty image with the calculated size */
    if (0 == (imgHandle = createEmptyImage(style, size)))
    {
        return FALSE;
    }
    
    /* Draw the text on the image */
    rowIndex = currInst->currentIndex;

    /*  Get end row. If text to render takes less size than the box itself,
     *  we adjust the end row accordingly. For instance, if we can fit 5 lines
     *  on the screen but have only 3 left to show, the end will be index + 3
     *  otherwise index + 5
     */
    if ((currInst->rowCount - currInst->currentIndex) < 
        (size->height / rowHeight)) 
    {
        rowEnd = currInst->rowCount;
    }
    else
    {
        rowEnd = rowIndex + (size->height / rowHeight);
    }

    /* This is somewhat expensive so it might be better to keep track
     * of character buffer position in instance data
     */
    charPos = we_cmmn_utf8_str_incr(currInst->charBuffer, 
        currInst->indexTable[rowIndex].utf8Index);

    while (rowIndex < rowEnd)
    {
        int i=0;
        p = &currInst->indexTable[rowIndex];
        /* Check if newline */
        
        if (IS_NEWLINE(*charPos))
        {
            /* Next line */
            pos.y = (WE_INT16)(pos.y + rowHeight);
            rowIndex++;
            charPos = we_cmmn_utf8_str_incr(charPos, 
                (p + 1)->utf8Index - p->utf8Index);
            continue;
        }
      
        index = p->utf8Index;
        visibleChars = (rowIndex + 1 == rowEnd ? currInst->totalLength - index : 
            (p + 1)->utf8Index - index);

         for(i;i<visibleChars;i++)
        {
            if(IS_NEWLINE(*(charPos+i)))
            {
                visibleChars=i;
                break;
            }

        } 

        if ((renderLinks && linkExists(index, index + visibleChars))
            || linkSpan) 
        {
			if(!align){
                /* left alignment the text CR 17749*/
				pos.x = 0;
			}else{
                /* Center the text */
                pos.x = (WE_INT16)((WE_UINT16)(size->width - p->width) >> 1);
            }
            endIndex = index + visibleChars;
            if (linkSpan && link) 
            {
                /* Check if link span multiple rows */
                if (toRend > visibleChars) 
                {
                    /* Draw the link */
                    if ((index = drawLink(imgHandle, strHandle, link, &pos, index, 
                        visibleChars, size, hideCursor)) < 0)
                    {
                        (void)WE_WIDGET_RELEASE(imgHandle);
                        return FALSE;
                    }
                    linkSpan = TRUE;
                    toRend -= visibleChars; 
                }
                else
                {
                    /* Draw the link */
                    if ((index = drawLink(imgHandle, strHandle, link, &pos, index, 
                        toRend, size, hideCursor)) < 0)
                    {
                        (void)WE_WIDGET_RELEASE(imgHandle);
                        return FALSE;
                    }
                    toRend = link->utf8Len - (index - link->index);
                    linkSpan = FALSE;
                }
            }
            /* For each link... */
            while (!linkSpan &&
                (NULL != (link = getNextVisibleLink(index, endIndex))))
            {
                /* Check if link span multiple rows */
                if (link->index + link->utf8Len > endIndex) 
                {
                    linkSpan = TRUE;
                    toRend = link->utf8Len - (endIndex - link->index);
                }
                else
                {
                    linkSpan = FALSE;
                    toRend = 0;
                }
                /* Draw the link */
                if ((index = drawLink(imgHandle, strHandle, link, &pos, index, 
                    link->utf8Len - toRend, size, hideCursor)) < 0)
                {
                    (void)WE_WIDGET_RELEASE(imgHandle);
                    return FALSE;
                }
            }
            /* Check if anything left to draw on the row */
            if (!linkSpan && (index < endIndex)) 
            {
                if (WE_WIDGET_DRAW_STRING(imgHandle, strHandle, &pos, size, index, 
                    endIndex - index, 1) < 0) /* Q04A */
                {
                    (void)WE_WIDGET_RELEASE(imgHandle);
                    return FALSE;
                }
            }
        }
        else {
			if(!align){
                /* left alignment the text CR 17749*/
				pos.x = 0;
			}else{
                /* Center the text */
                pos.x = (WE_INT16)((WE_UINT16)(size->width - p->width) >> 1);
            }
            /* Draw */
            if (WE_WIDGET_DRAW_STRING(imgHandle, strHandle, &pos, size, index, 
                visibleChars, 1) < 0) /* Q04A */
            {
                (void)WE_WIDGET_RELEASE(imgHandle);
                return FALSE;
            }
        }
        /* Next line */
        pos.y = (WE_INT16)(pos.y + rowHeight);
        rowIndex++;
        charPos = we_cmmn_utf8_str_incr(charPos, 
            (rowIndex == rowEnd ? currInst->totalLength - p->utf8Index : 
            (p + 1)->utf8Index - p->utf8Index));
    }
        
    if (rowIndex == currInst->rowCount)
    {
        if (0 == currInst->currentIndex) 
        {
            currInst->fullScreen = TRUE;
        }
        else
        {
            currInst->fullScreen = FALSE;
        }
    }
    return imgHandle;
}

/*!
 * \brief Calculates new start and end indices for current screen.
 *
 * \return TRUE if indices has changed, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL setNewIndices(void)
{
    int visible;
    if (WeKey_Up == currInst->currentEvent || 
        WeKey_Left == currInst->currentEvent) 
    {
        if (0 == currInst->currentIndex) 
        {
            /* Nothing has changed */
            return FALSE;
        }
        currInst->currentIndex--;
    }
    else 
    {
        visible = (currInst->actualSize.height / currInst->rowHeight);        
        if (currInst->currentIndex + visible >= currInst->rowCount) 
        {
            /* Nothing has changed */
            return FALSE;
        }
        currInst->currentIndex++;
    }
    return TRUE;
}

/*!
 * \brief Finds the nearest link from current.
 *
 * \param dirDown TRUE if search direction is down.
 * \return TRUE if a new link is selected, otherwise FALSE and current link
 *  is unchanged.
 *****************************************************************************/
static WE_BOOL findNearestLink(WE_BOOL dirDown)
{
    MtrParseResult *tmp;
    if (dirDown) 
    {
        if (NULL != (tmp = getNextLink(currInst->selectedLink)))
        {
            currInst->selectedLink->isSelected = FALSE;
            currInst->selectedLink = tmp;
            currInst->selectedLink->isSelected = TRUE;
            return TRUE;
        }
        return FALSE;        
    }
    else
    {
        if (NULL != (tmp = getPrevLink(currInst->selectedLink)))
        {
            currInst->selectedLink->isSelected = FALSE;
            currInst->selectedLink = tmp;
            currInst->selectedLink->isSelected = TRUE;
            return TRUE;
        }
    }
    return FALSE;
}

/*!
 * \brief Check if a link is visible in current range 
 *
 * \param link The link to check.
 * \param start Where to start the search from.
 * \param inst Current instance.
 * \return The new instance or NULL if no new instance could be created.
 *****************************************************************************/
static WE_BOOL isVisible(const MtrParseResult *link, int start, 
    const MtrInstance *inst)
{
    int endRow = 0, endIndex = 0;
    if (!inst || !link || !inst->rowHeight) 
    {
        return FALSE;
    }
    start = (start < 0 ? 0 : start);
    endRow = start + (inst->actualSize.height / inst->rowHeight);
    if (endRow > (inst->rowCount - 1))
    {
        endRow = inst->rowCount - 1;
    }
    if (endRow == inst->rowCount - 1) 
    {
    /* If we are at the last row we do not have a next row to get index from
        * so we use total count instead. */
        endIndex = inst->totalLength;
    }
    else
    {
        endIndex = inst->indexTable[endRow + 1].utf8Index;
    }
    if (link->index >= inst->indexTable[start].utf8Index &&
        link->index < endIndex) 
    {
        return TRUE;
    }
    return FALSE;
}

/*!
 * \brief 
 *
 * \param item Data for the new instance.
 * \return The new instance or NULL if no new instance could be created.
 *****************************************************************************/
WE_BOOL mtrRender(MtrInstance *inst)
{
    WeSize *size;
    MtrParseResult *tmp;
    WeImageHandle imgHandle;
    WE_BOOL linkChanged = FALSE;
    WE_BOOL indexChanged = FALSE;
    WE_BOOL hideCursor;
    WE_INT16 rowHeight;
    WeStyleHandle style;
	/*TR 17667*/
	WeFont fonttmp = {0};
    
    /* Set current instance */
    currInst = inst;
    /* Check flags */
    hideCursor = IS_SET(inst->prop.flags, MTR_PROP_HIDE_CURSOR);

    size = &inst->actualSize;
    /* Create the style for the text box */
    if (0 == (style = WE_WIDGET_STYLE_CREATE(WE_MODID_MTR, &inst->prop.textColor, 
        &inst->prop.bgColor, NULL, NULL, NULL, &fonttmp, NULL)))
    {
        return FALSE;
    }
    
    /* Check if this is a redraw */
    if (currInst->redraw) 
    {
        if (!hideCursor) 
        {
            /* Take actions according to current event */
            if (WeKey_Up == inst->currentEvent || 
                WeKey_Left == inst->currentEvent) 
            {
                /*  Key up...Find prev selected and check if the link will be
                 *  visible. */
                tmp = getPrevLink(currInst->selectedLink);
                if (isVisible(tmp, currInst->currentIndex - 1, currInst)) 
                {
                    linkChanged = findNearestLink(FALSE);
                }
            }
            else
            {
                /* Key down...Find next selected and check if the link will be
                 *  visible. */
                tmp = getNextLink(currInst->selectedLink);
                if (isVisible(tmp, currInst->currentIndex + 1, currInst)) 
                {
                    linkChanged = findNearestLink(TRUE);
                }
            }
        }
        /* We only scroll text if we have not already scrolled cursor */
        if (!linkChanged && !inst->fullScreen)
        {
            /* Find next index */
            indexChanged = setNewIndices();            
        }
        /* We only have to redraw if something has changed */
        if (indexChanged || linkChanged) 
        {
            /* create image */
            if (0 == (imgHandle = doRender(inst->stringHandle,
                size, inst->rowHeight, style)))
            {
                return FALSE;
            }
            /* Set size of new image in gadget to avoid stretching */
            if (WE_WIDGET_SET_SIZE(inst->gadgetHandle, size) < 0)
            {
                (void)WE_WIDGET_RELEASE(imgHandle);                
                (void)WE_WIDGET_RELEASE(style);
                return FALSE;
            }
            /* Replace image in gadget */
            if (WE_WIDGET_IMAGE_GADGET_SET(inst->gadgetHandle, imgHandle, 
                WeImageZoomAutofit) < 0)
            {
                (void)WE_WIDGET_RELEASE(style);
                (void)WE_WIDGET_RELEASE(imgHandle);                
                return FALSE;
            }
            (void)WE_WIDGET_RELEASE(imgHandle);
        }
        if (!indexChanged && !linkChanged) 
        {
            /* Nothing has happened, we can loose focus */
            currInst->lostFocus = TRUE;
        }
        else
        {
            currInst->lostFocus = FALSE;
        }
    }
    else
    {
        size->width = inst->boxSize.width;
        size->height = 0;
        /* Get the total number of characters. */
        currInst->totalLength = 
            WE_WIDGET_STRING_GET_LENGTH(inst->stringHandle, 0, WeUtf8);
        /* Create the index table the first time */
        if (!createIndexTable(currInst, &fonttmp))
        {
            (void)WE_WIDGET_RELEASE(style);
            return FALSE;
        }
        /* Calculate the drawing space needed to render the text on */
        if (!calculateImageSize(inst, &inst->boxSize, size, &rowHeight, &fonttmp))
        {
            (void)WE_WIDGET_RELEASE(style);
            return FALSE;
        }
        /* Save row height for later... */
        inst->rowHeight = rowHeight;
        /* Mark first link */
        currInst->selectedLink = getFirstLink();
        if (currInst->selectedLink) 
        {
            currInst->selectedLink->isSelected = TRUE;
        }
        inst->currentIndex = 0;
        /* Render text to an image */
        if (0 == (imgHandle = doRender(inst->stringHandle,
            size, rowHeight, style)))
        {
            (void)WE_WIDGET_RELEASE(style);
            return FALSE;
        }
        /* Create the image gadget to hold the rendered text */
        if (0 == (inst->gadgetHandle = 
            WE_WIDGET_IMAGE_GADGET_CREATE(WE_MODID_MTR, imgHandle, size, 
            WeImageZoomAutofit, 
            WE_GADGET_PROPERTY_FOCUS, style)))
        {
            (void)WE_WIDGET_RELEASE(style);
            (void)WE_WIDGET_RELEASE(imgHandle);
            return FALSE;
        }
        (void)WE_WIDGET_RELEASE(imgHandle);
    }
    (void)WE_WIDGET_RELEASE(style);
    return TRUE;
}

