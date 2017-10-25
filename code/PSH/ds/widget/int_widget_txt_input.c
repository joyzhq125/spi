#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
                                                                          
   SEF CONFIDENTIAL AND PROPRIETARY                                       
                                                                          
   This source is the sole property of SEF Inc. Reproduction or           
   utilization of this source in whole or in part is forbidden without    
   the written consent of SEF Inc.                                        

                                                                          
   (c) Copyright SEF Inc.,         2002.  All Rights Reserved.            
                                                                          
**************************************************************************
                                                                          
   int_widget_txt_input.c: 
                                                                          
**************************************************************************
    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    04/03/2003   Chenjs           Cxxxxxx     initial file creation.
    7/30/2003    Chenjs           P001026     Modify the event handle manner
    8/11/2003    Chenjs           P001091     Upgrade text input widget.
    8/18/2003    Chenjs           p001265     Optimize entering string operation.
    08/25/2003   Zhuxq            P001355     Fixed some warning according to PC-Lint check result
    08/25/2003   Zhuxq            P001417     Fixed the bug:in some case the textinput in a wap page can not input
    10/17/2003   Zhuxq            C001603     Add touch panel feature to WAP & MMS
    11/6/2003    Zhuxq            P001981     Fix some bugs: in a menu with a scroll bar, the contents will not be scrolled to the end, etc
    11/11/2003   Zhuxq            P001883     Optimize the redraw strategy to reduce the redraw times
    01/05/2004   Zhuxq            CNXT000072  Add callback function to each widget in the widget system
    02/02/2004   Chenxiao         p002404     add textinput scrollbar  in the dolphin project
    02/16/2004   Zhuxq            P002469     Add button gadget to widget system and add modId parameter to event callback functions
    02/17/2004   Zhuxq            P002492     Add Viewer window to widget system
    02/28/2004   Zhuxq            P002556     Fix some widget bugs
    03/08/2004   Andrew           P002598     Update WAP Editor for Dolphin project
    04/15/2004   zhuxq            P002977     Fix some bugs in UMB for Dolphin
    04/19/2004   chenxiao         p003028     fix umb setting bugs and widget bugs    
    04/22/2004   Andrew           P002659     Fix some bugs of projects that bases Dolphin
    04/26/2004   chenxiao         p005117     fix some widget and umb bugs        
    04/29/2004   chenxiao         p005177     fix some umb and widget bugs           
    05/11/2004   Andrew           P005270     Handle return in Editor.
    05/18/2004   Zhang Youkun     p005280     set cursor position for fixing bug 5127
    05/22/2004   chenxiao         p005615     The return value is incorrect after homepage is modified    
    05/24/2004   Andrew           P005598     Don't allow user to input Chinese character in the password text input
    05/24/2004   zhuxq            P005568     Adjust UI implementation in UMB and fix some bugs
    06/04/2004   zhuxq            P005925     Correct issues found by PC-lint and fix some bugs
    06/04/2004   Andrew           P005882     Fix some Dolphin bugs
    06/07/2004   Andrew           P005999     Don't locate cursor when user clicks the line that has nothing.
    06/09/2004   zhuxq            P006048     forbid Menu to respond successive multiple actions
    06/18/2004   zhuxq            P006260     Make up WAP&UMB code to cnxt_int_wap branch
    07/02/2004   Andrew           P006453     Calculating the scroll bar position is wrong
    07/05/2004   Chenxiao         p006721     Some words can't be show correctly in the powerword
    07/30/2004   chenxiao         p007418     The cell phone will reset when line count in editor is over 255        
    07/29/2004   MengJianghong    P007310     Add no clear key mode,fix bug cnxt0012521.
    08/23/2004   Zhuxq            p007888     Fix the bug that the icon of 'abc' input method flashes when inputing in a wap page
    08/30/2004   zhuxq            c007998     improve the implementation of predefined string to reduce the times of memory allocation
    09/01/2004   Andrew           P008042     Fix some Editor bugs.
    09/02/2004   MengJianghong    P008033     Fix bug that keypad no response when pen down in scroll bar.

    Self-documenting Code
    Describe/explain low-level design, especially things in header files, of this module and/or
    group of funtions and/or specific funtion that are hard to understand by reading code
    and thus requires detail description.
    Free format !
---------- Dependencies --------------------------------------*/

#ifdef WIN32
#include "portab_new.h"
#else
#include "wcdtypes.h"
#include "portab.h"
#endif

#include "Rm_typedef.h"
#include "ds_def.h"
#include "Ds_drawing.h"
#include "Uhapi.h"
#include "SP_list.h"
#include "msf_env.h"
#include "KEY_table.h"
#include "gsm_timer.h"
#include "msf_int_widget.h"
#include "int_widget_common.h"
#include "int_widget_txt_input.h"
#include "msf_def.h"
#include "int_widget_imm.h"
#include "SP_volume.h"
#include "int_widget_custom.h"
#include "int_widget_show.h"

#ifdef WIN32
#include "assert.h"
#define OP_ASSERT(a)        assert(a)
#else
#define OP_ASSERT(a)
#endif

/*
 * In the case of uncompressed GSM 7 bit default alphabet data,
 * the maximum length of the short message within the TP-UD field
 * is 153 (160-7) characters.
 */
 #ifndef MAX_CONCATENATED_SMS_LENGTH
#define MAX_CONCATENATED_SMS_LENGTH  153
#endif

#define MAX_CONCATENATED_SMS_CH_LENGTH   (MAX_CONCATENATED_SMS_LENGTH/8*7/ 2)

/*=============================================================================
    LOCAL CONSTANTS
=============================================================================*/
#ifdef _NO_CLEAR_KEY
static  OP_UINT8   skeyStr[SKEY_MAX_CHAR*2+2];
#endif
/*=============================================================================
    LOCAL TYPEDEFS - Structure, Union, Enumerations...etc
=============================================================================*/

#define TBM_UNICODE_VALUE(pStr) ( ((pStr)[1] << 8) | (pStr)[0])

#define POINT_IN_RECT( pt, posLeftTop, rectSize )  \
        (((pt).x >= (posLeftTop).x && (pt).x < (posLeftTop).x + (rectSize).width) \
        && ((pt).y >= (posLeftTop).y && (pt).y < (posLeftTop).y + (rectSize).height))


/*=============================================================================
    LOCAL FUNCTION PROTOTYPES
=============================================================================*/
extern int  widgetInputGetEngCharSize
( 
    OP_UINT32       hText,
    const MsfSize   *pInputSize, 
    OP_INT16        *pEngCharsPerLine,
    OP_INT16        *pAdjustEngCharsPerLine,
    OP_INT16        *pLines 
);


extern int widgetRedraw( void *pWidget, MsfWidgetType widgetType, OP_UINT32  handle, OP_BOOLEAN   bRefresh);

extern int widgetImmEventHandler
(
    MsfWidgetImm*                   pImm,
    OPUS_EVENT_ENUM_TYPE   event, 
    void                                      *pMess,
    OP_BOOLEAN                       *handle
);

static OP_INT16 TBM_GetPlaceholderCnt
(
    OP_UINT8 *pStr, 
    OP_UINT16 iBegin,
    OP_UINT16 iEnd,
    OP_UINT16 iLinePlacehldCnt,
    OP_BOOLEAN bPadding
);
static OP_BOOLEAN TVM_Build(TBM_STRUCTURE_NAME_T * pTBM);


void TBM_CalculateTotalRows(TBM_STRUCTURE_NAME_T  *pTBM, OP_UINT16 iCharsPerLine);
/*==================================================================================================
    GLOBAL FUNCTIONS
==================================================================================================*/

/*****************************************New Code **************************/
/*=============================================================================
    FUNCTION:  TVM_SetCursor

    DESCRIPTION:
        Set cursor information.

    ARGUMENTS PASSED:
        pBuffer : address where source string buffer is at.
        iStartPos : start index of string buffer.
        iCursorPos : cursor index of string buffer
        pCursorOffset : placeholder count between iStartPos and iCursorPos.
        
    RETURN VALUE:
        OP_TRUE : operation successful 
        OP_FALSE : Errors occured.

    IMPORTANT NOTES:
        1>. This function's body is copied from EWM_setcursor(...).
=============================================================================*/
static OP_BOOLEAN TVM_SetCursor
(
    OP_UINT8    *pBuffer,
    OP_UINT16    iStartPos,
    OP_UINT16    iCursorPos,
    OP_INT16 *pCursorOffset
)
{
    OP_UINT8                *ptr;
   
    if(pBuffer == OP_NULL)
    {
        return OP_FALSE;
    }
    
    if(iCursorPos < iStartPos)
    {
        return OP_FALSE;
    }

    if(iCursorPos == iStartPos)
    {
        *pCursorOffset = 0;
        return OP_TRUE;
    }

    ptr = WIDGET_ALLOC((iCursorPos - iStartPos + 1)*2);
    if(ptr == OP_NULL)
    {
        return OP_FALSE;
    }
    op_memset(ptr, 0, (iCursorPos - iStartPos + 1)*2);

    Ustrncpy(ptr, &pBuffer[iStartPos*2], (iCursorPos - iStartPos)*2);
    *pCursorOffset = UstrCharCount(ptr);

    WIDGET_FREE(ptr);

    return OP_TRUE;
}
/*=============================================================================
    FUNCTION:  TVM_GetNextPos

    DESCRIPTION:
        Get char index of the following line .

    ARGUMENTS PASSED:
        pBuffer : address where source string buffer is at.
        from : Head char index in string buffer.
        width : Placeholder count of one line.
        nPos : position of next line's first char.
        
    RETURN VALUE:
        OP_TRUE : operation successful 
        OP_FALSE : Errors occured.

    IMPORTANT NOTES:
        1>. This function's body is copied from EWM_get_nextpos(...).
=============================================================================*/
static OP_BOOLEAN  TVM_GetNextPos
(
    OP_UINT8     *pBuffer,
    OP_UINT16    from,
    OP_UINT16     width,
    OP_UINT16    *nPos
)
{
    OP_UINT16   iPlacehldCnt = 0;
    OP_UINT8    index=0;
    OP_UINT16   value;
    OP_UINT8    *pSrcStr;
    
    if(pBuffer == OP_NULL)
    {
        return OP_FALSE;
    }

    pSrcStr = &pBuffer[from*2];

    value = TBM_UNICODE_VALUE(pSrcStr);
    while (value != 0x0000)
    {
        if  (value <= 0x007f)
        {
            iPlacehldCnt++;
        }
        else
        {
            iPlacehldCnt += 2;
        }

        if (value  == 0x000a)
        {
            index ++;
            break;
        }

        if(iPlacehldCnt>width)
        {
            break;
        }
        
        
        index ++;
        /*
        if( (value == 0x0020) || (value > 0x007f))
        {
            psp = index;
        }
        */
        
        pSrcStr += 2;
        value = TBM_UNICODE_VALUE(pSrcStr);
    }

    /*
    if((psp != 0) && (iPlacehldCnt > width))
    {
        *nPos = from + psp;
    }
    else
    */
    {
        *nPos = from + index;
    }

    if(*nPos == from)
    {
        return OP_FALSE;
    }
    
    return OP_TRUE;
}
/*=============================================================================
    FUNCTION:  TVM_SetSize

    DESCRIPTION:
        Set size information of textinput's view area .

    ARGUMENTS PASSED:
        pTBM : Textinput buffer manager.
        width : Placeholder count of one line.
        iAfterAdjustWidth: Placeholder count of one line after lines are adjusted
        height : rows count of textinput.
        bResize : OP_TRUE: resize operation.
                       OP_FALSE: Init operation.
RETURN VALUE:
        OP_TRUE :  operation OK.
        OP_FALSE : Errors occured.

    IMPORTANT NOTES:
        None
=============================================================================*/
OP_BOOLEAN TVM_SetSize
(
    TBM_STRUCTURE_NAME_T    *pTBM,
    OP_UINT16    width,
    OP_UINT16    iAfterAdjustWidth,
    OP_UINT16    height,
    OP_BOOLEAN bResize
)
{
    if( !pTBM ||width == 0 ||height == 0 )
    {
        return OP_FALSE;
    }

    pTBM->rows = height;
    pTBM->cols = width;
    pTBM->iColsHasScrollbar = iAfterAdjustWidth;

    if(pTBM->rowStartList != OP_NULL)
    {
        WIDGET_FREE(pTBM->rowStartList);
    }

    if(height == 1)
    {
        if(bResize)
        {   
            OP_INT16 iPlacCnt = 0,i;
            OP_UINT8 *pBuf;

            pBuf = pTBM->pTxtInputBuf;
            for(i = 2*pTBM->iCursorPos; iPlacCnt < pTBM->cursor_x; i -= 2)
            {
                if(TBM_UNICODE_VALUE(&pBuf[i ]) <= 0x007f)
                {
                    iPlacCnt += 1;
                }
                else
                {
                    iPlacCnt += 2;
                }          
            }

            pTBM->iLeftTopCharPos = i/2;
        }
        else
        {
            TBM_AdjustInitCursorPos(pTBM);
        }
    }
    else
    {
   
        pTBM->rowStartList = WIDGET_ALLOC(sizeof(OP_UINT16)*(height+1));
        if(pTBM->rowStartList == OP_NULL)
        {
            return OP_FALSE;
        }
        op_memset(pTBM->rowStartList, 0, (sizeof(OP_UINT16)*(height+1)));

        if(bResize)
        {
            TVM_Build(pTBM);
        }
        else
        {
            TBM_AdjustInitCursorPos(pTBM);
        }
      
    }

    return OP_TRUE;
}

/*=============================================================================
    FUNCTION:  TVM_SetSize

    DESCRIPTION:
        Build view information.

    ARGUMENTS PASSED:
        pTBM : Textinput buffer manager.
        
RETURN VALUE:
        OP_TRUE :  operation OK.
        OP_FALSE : Errors occured.

    IMPORTANT NOTES:
        1>. This function's body is copied from EWM_Rebuild(...).
=============================================================================*/
static OP_BOOLEAN TVM_Build
(
    TBM_STRUCTURE_NAME_T    *pTBM
)
{
    OP_UINT8    *pBuffer;
    OP_UINT16    iCursorPos;
    OP_UINT16   iStartPos, iNextPos;
    OP_UINT16    line_id;
    OP_UINT8     i;
    OP_BOOLEAN  bCursorHasSet = OP_FALSE;
//    OP_UINT16    iCurWidth = 0;
    OP_UINT16 total_lines;
    OP_UINT8 index;
    OP_UINT16   temp_cols = 0;


    if(!pTBM)  
    {
        return OP_FALSE;
    }

    pBuffer = pTBM->pTxtInputBuf;
    iCursorPos = pTBM->iCursorPos;

    if(pTBM->rowStartList == OP_NULL)
    {
        return OP_FALSE;
    }
    if(pTBM->bNeedScrollbar)
    {
        temp_cols = pTBM->iColsHasScrollbar;
    }
    else
    {
        temp_cols = pTBM->cols;
    }
    pTBM->index = 0;
    iStartPos = 0;
    iNextPos = 0;
    pTBM->rowStartList[pTBM->index] = iStartPos;
    pTBM->cnt = 1;
    line_id = 0;
    total_lines = 0;
    index = 0;
    
    while(TVM_GetNextPos(pBuffer, iStartPos, temp_cols, &iNextPos))
    {
        index++;
        total_lines ++;
        index %= pTBM->rows+1;
        pTBM->rowStartList[index] = iNextPos;

        if( (iCursorPos>=iStartPos) && (iCursorPos<iNextPos) )
        {
            if(line_id == pTBM->rows)
            {
                pTBM->cursor_y = line_id-1;
            }
            else
            {
                pTBM->cursor_y = line_id;
            }

            bCursorHasSet  = OP_TRUE;
            TVM_SetCursor(pBuffer, iStartPos, iCursorPos, &pTBM->cursor_x);
        }

        iStartPos = iNextPos;
        pTBM->cnt ++;
        line_id++;
        if(line_id == pTBM->rows+1)
        {
            line_id--;
        }

        if( (line_id == pTBM->rows) && (bCursorHasSet == OP_TRUE) )
        {
            break;
        }
    }

    if(pTBM->cnt > pTBM->rows+1)
    {
        pTBM->cnt = pTBM->rows+1;
    }
    pTBM->index = (index + 1)%pTBM->cnt;

    if(bCursorHasSet  == OP_FALSE)
    {
        i = (pTBM->cnt + pTBM->index - 2)%pTBM->cnt;
        pTBM->cursor_x = UstrCharCount(&pBuffer[pTBM->rowStartList[i]*2]);

        if (iCursorPos > 0)
        {
            OP_UINT16 len = Ustrlen(pBuffer);
            OP_ASSERT((OP_UINT16)(iCursorPos * 2 - 1) <= (len));
            if((pTBM->cursor_x > temp_cols) || ((pBuffer[iCursorPos*2-2] == 0x0A) && (pBuffer[iCursorPos*2-1] == 0x00)))
            {
                pTBM->cursor_x = 0;
                if(line_id == pTBM->rows)
                {
                    pTBM->cursor_y = line_id-1;
                    pTBM->index = (pTBM->index + 1)%pTBM->cnt;
                }
                else
                {
                    pTBM->cursor_y = line_id;
                }
            }
            else
            {
                pTBM->cursor_y = (line_id == 0) ? line_id : line_id - 1;
            }
        }
        else
        {
            pTBM->cursor_y = 0;
        }
    }

    if (pTBM->index != 0)
    {
        OP_UINT16 * p_arr = (OP_UINT16 *) op_alloc ((pTBM->rows + 1)* 2);
        
        if (p_arr == OP_NULL)
        {
            return OP_FALSE;
        }

        op_memset(p_arr, 0x00, (pTBM->rows + 1)* 2);
        index = pTBM->index;

        for ( i = 0; i < pTBM->cnt; i++)
        {
            index %= (pTBM->rows + 1);
            p_arr[i] = pTBM->rowStartList[index];
            index ++;
        }

        op_memcpy(pTBM->rowStartList, p_arr, pTBM->cnt * 2);
        pTBM->index = 0;
        
        op_free( p_arr );
    }    

    if (pTBM->cnt == pTBM->rows+1)
    {
        if ((0x00 == pBuffer[pTBM->rowStartList[pTBM->cnt - 2] * 2 ])
            && (0x00 == pBuffer[pTBM->rowStartList[pTBM->cnt - 2] * 2 + 1])
            )
        {
            pTBM->cnt -=1;
            total_lines ++;
        }
    }
    
    //
    // Set scrollbar information.
    //

    if (total_lines > pTBM->rows)
    {
        pTBM->top_line = total_lines - pTBM->rows;
    }
    else
    {
        pTBM->top_line = 0;
    }
    TBM_CalculateTotalRows(pTBM,temp_cols);


    return OP_TRUE;

}


OP_BOOLEAN TVM_InsertChar
(
    TBM_STRUCTURE_NAME_T *pTBM,
    OP_UINT16       uchar
)
{
    OP_INT16 cx, cy;
    OP_UINT8 index;
    OP_UINT16 start_pos, next_pos;
    OP_UINT8        *pBuffer;
    OP_UINT16       cursor_pos;
    OP_UINT16        cur_cols = 0;
    // OP_UINT8 line_id;
    
    if( pTBM == OP_NULL)
    {
        return OP_FALSE;
    }


    if(pTBM->rowStartList == OP_NULL)
    {
        return OP_FALSE;
    }
    if(pTBM->bNeedScrollbar)
    {
        cur_cols = pTBM->iColsHasScrollbar;
    }
    else
    {
        cur_cols = pTBM->cols;
    }
    pBuffer = pTBM->pTxtInputBuf;
    cursor_pos = pTBM->iCursorPos;

    //
    // The window has been scrolled.
    //
    if (pTBM->cursor_y < 0 || pTBM->cursor_y >= pTBM->rows)
    {
        TVM_Build(pTBM);
        return OP_TRUE;
    }
    //
    // x++(Eng char) or x +=2(Chn char), if x>cols, x-=cols.
    // or x = 0, y +=1 (Return)
    //
    {
        cx = pTBM->cursor_x;
        cy = pTBM->cursor_y;
        index = 0;

        if (uchar == 0x0A)
        {
            cx = 0;
            cy ++;
            //
            // If the return is inserted at the head of line
            // And the previous word is not return.
            //
            if (cursor_pos != 1
                && pTBM->cursor_x == 0
                && TBM_UNICODE_VALUE (pBuffer + (pTBM->rowStartList[pTBM->cursor_y] -1)*2 ) != 0x0A)
            {
                cy --;
            }
        }
        else
        {
            if (uchar > 0x7F)
            {
                cx += 2;
            }
            else
            {
                //
                // If the previous line has an English position, 
                // this char will be moved to previous line,
                // So the cx value is still zero.
                //
                cx ++;
                if (cx == 1)
                {
                    // Previous line has an English char position
                    if (pTBM->rowStartList[cy] >0)
                    {
                        if (TBM_UNICODE_VALUE (pBuffer + (pTBM->rowStartList[cy] -1)*2 ) != 0x0A)
                        {
                            start_pos = 0;
                            next_pos = 0;
                            index = 0;

                            while(TVM_GetNextPos(pBuffer, start_pos, cur_cols, &next_pos))
                            {        
                                start_pos = next_pos;
                                if (start_pos >= pTBM->rowStartList[cy])
                                {
                                    break;
                                }
                            }

                            if (start_pos == pTBM->rowStartList[cy] + 1)
                            {
                                cx = 0;
                            }
                        }
                    }
                }                
            }
            
            if (cx > cur_cols)
            {
                cx = (uchar > 0x7F) ? 2:1;
                cy ++;
            }
        }
        
        if (cy >=pTBM->rows)
        {
            //
            // Need scroll window.
            //
            index ++;
            pTBM->top_line ++;
            cy = pTBM->rows -1;
        }

    }

    //
    // Recalculate the rowStartList again.
    //
    pTBM->index = 0;
    pTBM->cursor_x = cx;
    pTBM->cursor_y = cy;

    pTBM->rowStartList[0] = pTBM->rowStartList[index];
    start_pos = pTBM->rowStartList[0];
    next_pos = 0;
    index = 0;

    while(TVM_GetNextPos(pBuffer, start_pos, cur_cols, &next_pos))
    {        
        index++;

        pTBM->rowStartList[index] = next_pos;
        start_pos = next_pos;
        if (index == pTBM->rows)
        {
            break;
        }
    }

    pTBM->cnt = index + 1;

    if (pTBM->cnt > pTBM->rows+1)
    {
        pTBM->cnt = pTBM->rows+1;
    }

    // TBM_AdustCols(pTBM, OP_TRUE);
    TBM_CalculateTotalRows(pTBM,cur_cols);



    return OP_TRUE;
}


OP_BOOLEAN TVM_DeleteChar
(
    TBM_STRUCTURE_NAME_T *pTBM,
    OP_UINT16       uchar
)
{
    OP_INT16 cx, cy;
    OP_UINT16 start_pos, next_pos;
    OP_UINT16 line_id;
    OP_UINT8 index;
    OP_UINT8 temp_index = 0;
    OP_UINT8 cur_len = 0;
    OP_BOOLEAN bScroll = OP_FALSE;
    OP_BOOLEAN bFind = OP_FALSE;    
    OP_UINT8        *pBuffer;
    OP_UINT16       cursor_pos;
    OP_UINT16   cur_cols = 0;
    
    if( pTBM == OP_NULL)
    {
        return OP_FALSE;
    }


    if(pTBM->rowStartList == OP_NULL)
    {
        return OP_FALSE;
    }
    pBuffer = pTBM->pTxtInputBuf;
    cursor_pos = pTBM->iCursorPos;
    if(pTBM->bNeedScrollbar)
    {
        cur_cols = pTBM->iColsHasScrollbar;
    }
    else
    {
        cur_cols = pTBM->cols;
    }

    //
    // The window has been scrolled.
    //
    if (pTBM->cursor_y < 0 || pTBM->cursor_y >= pTBM->rows)
    {
        TVM_Build(pTBM);
        return OP_TRUE;
    }

    //
    // x--(Eng char) or x +=2(Chn char), if x>cols, x-=cols.
    // or x = 0, y +=1 (Return)
    //
    {
        cx = pTBM->cursor_x;
        cy = pTBM->cursor_y;

        if (cx == 0)
        {
            //
            // If there is no any more text after this window, 
            // but there are some text before this window, it should be scrolled
            //

            //
            // Condition 1: the last line of text position is at the bottom of the window.
            // Condition 2: There is no text after this window.
            // Condition 3: There is some text before the window.
            //
            if (pTBM->cnt == pTBM->rows + 1)
            {
                OP_ASSERT(pTBM->rowStartList[pTBM->cnt -1] > 0);
                if (0 == TBM_UNICODE_VALUE(pBuffer + pTBM->rowStartList[pTBM->cnt -1] * 2 - 2))
                {
                    if (0x0A != TBM_UNICODE_VALUE(pBuffer + pTBM->rowStartList[pTBM->cnt -1] * 2 - 4))
                    {
                        if (pTBM->rowStartList[0] > 0)
                        {
                            bScroll = OP_TRUE;
                        }
                    }
                }
            }
            else if (pTBM->cnt == pTBM->rows)
            {
                // if (pBuffer [ (pTBM->rowStartList[pTBM->cnt - 1]  - 1)* 2 ]
                //    + pBuffer [ pTBM->rowStartList[pTBM->cnt - 1] * 2 - 1 ] == 0x0A)
                {
                    if (pTBM->rowStartList[0] > 0)
                    {
                        bScroll = OP_TRUE;
                    }
                }
            }

            if (cursor_pos == 0)
            {
                cx = 0;
            }
            else if (uchar == 0x0A 
                && TBM_UNICODE_VALUE(pBuffer +  (cursor_pos - 1) * 2) == 0x0A)
            {
                cx = 0;
            }
            else
            {
                cx = 0xFF;
            }
            

            if (cy != 0)
            {
                if (!bScroll)
                {
                    cy --;
                }
            }

            if (bScroll)
            {
                //? OP_ASSERT(pTBM->scrollbar_info.top_line > 0);
                pTBM->top_line --;
            }
        }
        else
        {
            if (uchar > 0x7F)
            {
                cx -= 2;
            }
            else
            {
                cx --;
            }
            //
            // Judge if it needs to delete one line.
            //
            if (cx == 0 
                && cursor_pos != 0 
                && (TBM_UNICODE_VALUE(pBuffer +  (cursor_pos - 1) * 2) != 0x0A)
                )
            {
                if (pTBM->cnt == pTBM->rows + 1)
                {
                    if (pBuffer [ pTBM->rowStartList[pTBM->cnt - 1] * 2 ] == 0
                        && pBuffer [ pTBM->rowStartList[pTBM->cnt - 1] * 2 + 1 ] == 0)
                    {
                        if (pTBM->rowStartList[0] > 0)
                        {
                            bScroll = OP_TRUE;
                        }
                    }
                }
                else if (pTBM->cnt == pTBM->rows)
                {
                    if (pTBM->rowStartList[0] > 0)
                    {
                        bScroll = OP_TRUE;
                    }
                }

                cy --;
                cx = 0xFF;
                if (bScroll)
                {
                    cy ++;
                    OP_ASSERT(pTBM->top_line > 0);
                    pTBM->top_line --;
                }
            }
        }
        
    }

    //
    // Recalculate the rowStartList again.
    //
    pTBM->index = 0;
    if (cx != 0xFF)
    {
        pTBM->cursor_x = cx;
    }
    pTBM->cursor_y = cy;

    if (bScroll)
    {
        start_pos = 0;
        next_pos = 0;
        while(TVM_GetNextPos(pBuffer, start_pos, cur_cols, &next_pos))
        {
            if (next_pos == pTBM->rowStartList[0])
            {
                bFind = OP_TRUE;
                break;
            }
            start_pos = next_pos;
        }
        if (!bFind)
        {
            OP_ASSERT(bFind);
            return OP_FALSE;
        }
    }
    else
    {
        start_pos = pTBM->rowStartList[0];
    }

    next_pos = 0;
    line_id = 0;
    index = 0;
    pTBM->rowStartList[0] = start_pos;

    while(TVM_GetNextPos(pBuffer, start_pos, cur_cols, &next_pos))
    {
        index++;
        line_id ++;

        pTBM->rowStartList[index] = next_pos;
        start_pos = next_pos;
        if (cx == 0xFF && line_id == pTBM->cursor_y)
        {
            temp_index = index;
        }
        if (line_id == pTBM->rows)
        {
            break;
        }
    }
    pTBM->cnt = line_id + 1;

    //
    // Get cursor_x
    //
    if (cx == 0xFF)
    {
        OP_UINT16 i, j, count;
        OP_UINT16 value;

        i = pTBM->rowStartList[temp_index];
        temp_index ++;
        if (temp_index >= pTBM->cnt)
        {
            cx = 0;
        }
        else
        {
            j = pTBM->rowStartList[temp_index];

            OP_ASSERT(j > i);
           
            count = j - i;

            start_pos = i;
            cur_len = 0;
            
            for(i=0; i<count; i++)
            {
                value   = pBuffer[(start_pos + i) * 2 + 1];
                value <<= 8;
                value  |= pBuffer[(start_pos + i) * 2 + 0];

                if (value == 0x0A || value == 0x00)
                {
                    pTBM->cursor_x = cx = cur_len;
                    break;
                }
                if  (value <= 0x007f)
                {
                    cur_len += 1;
                }
                else
                {
                    cur_len += 2;
                }
                if (start_pos + i + 1 == cursor_pos)
                {
                    pTBM->cursor_x = cur_len;
                    break;
                }
            }
        }
    }

    if (cx == 0xFF)
    {
        pTBM->cursor_x = cx = cur_len;        
    }

    TBM_CalculateTotalRows(pTBM,cur_cols);

    // TBM_AdustCols(pTBM, OP_FALSE);
    
    return OP_TRUE;
}

/*=============================================================================
    FUNCTION:  TBM_CreateFromStrHandle

    DESCRIPTION:
        Creates text input buffer manager. 

    ARGUMENTS PASSED:
        iCharCnt : max char count of text input to be created.      
        hStr : string handle.
        hTxtinput : handle of textinput gadget.
        
    RETURN VALUE:
        TPI_WIDGET_OK: successful.
        others : operation failed.

    IMPORTANT NOTES:
        TBM: abbr of string "TextinputBufferManager".
        TVM: abbr of string "TextinputViewManager"
        following cases are the same.
        None
=============================================================================*/
OP_INT16   TBM_CreateFromStrHandle
(
    OP_INT16 iCharCnt,
    MsfStringHandle hStr,
    OP_UINT32 hTxtinput
)
{
    OP_INT16      iStrLen;
    MsfWidgetType widgetType;
    MsfString     *pStr;
    OP_UINT8      *pChar = OP_NULL;
    MsfStyleHandle hStyle = INVALID_HANDLE;
    MsfInput      *pTextInput;
    OP_UINT16     iLabLineCnt;
    MsfSize       iSize;
    OP_INT16      iEngCharsPerLine,iLineCnt, iAdjustEngCharsPerLine;
    TBM_STRUCTURE_NAME_T *pTxtInputMng;

    /* Get view information*/
    getInput(hTxtinput, &pTextInput,&iSize,&iLabLineCnt);
    if(!pTextInput)
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }
    widgetInputGetEngCharSize(hTxtinput,&iSize,&iEngCharsPerLine, &iAdjustEngCharsPerLine,&iLineCnt);
    iLineCnt -= iLabLineCnt;
    pTxtInputMng = &pTextInput->txtInputBufMng;
    
    if(hStr)
    {
        pStr = (MsfString*)seekWidget(hStr, &widgetType);
        if( pStr ) 
        {
            if( widgetType == MSF_STRING_RES )
            {
                pChar = (OP_UINT8*)pStr;
            }
            else if( widgetType == MSF_STRING && pStr->data != OP_NULL )
            {
                pChar = pStr->data;
                hStyle = pStr->style;
            }
        }
            
        if( !pChar )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }
    }

    pTxtInputMng->pTxtInputBuf = WIDGET_ALLOC(iCharCnt*2 + 2);
    if(!pTxtInputMng->pTxtInputBuf)
    {
        return TPI_WIDGET_ERROR_RESOURCE_LIMIT;
    }
    op_memset(pTxtInputMng->pTxtInputBuf,0,iCharCnt*2 + 2);
    pTxtInputMng->maxCharCount = iCharCnt;
    pTxtInputMng->maxSize = iCharCnt;

    if(pChar)
    {
        replaceHandle((OP_UINT32 *)&pTxtInputMng->hStyle, hStyle);
        iStrLen = Ustrlen(pChar);
        if(iStrLen > pTxtInputMng->maxCharCount * 2)
        {
            iStrLen = pTxtInputMng->maxCharCount * 2;
        }
        if(iStrLen >= 0)
        {
            OP_UINT8  *pInitialData = OP_NULL;
            int j =0;
            pInitialData = pChar;
            /*Whether Chinese exist in the inserted string */
            while(pInitialData[j] != 0 || pInitialData[j+1] !=0)
            {
                if(!(pInitialData[j] <0x7f && pInitialData[j+1] ==0))
                {
                    if(iStrLen > pTxtInputMng->maxCharCount)
                    {
                        iStrLen = pTxtInputMng->maxCharCount -( pTxtInputMng->maxCharCount % 2) ;
                    }
                    break;
                }
                j += 2;
            }
            TBM_EnterStringEx(pTxtInputMng , pChar, iStrLen, pTextInput->textType );
        }
        /* op_memcpy(pTxtInputMng->pTxtInputBuf, pMsfString->data,iStrLen); */
        pTxtInputMng->format = MsfUnicode;
    }
    else
    {
        iStrLen = 0;
    }
    
    pTxtInputMng->pTxtInputBuf[iStrLen] = 0;
    pTxtInputMng->pTxtInputBuf[iStrLen +1] = 0;                    
    
    pTxtInputMng->cnt = 0;
    pTxtInputMng->rows = iLineCnt;
    pTxtInputMng->cols = iEngCharsPerLine;
    pTxtInputMng->iColsHasScrollbar = iAdjustEngCharsPerLine;
    pTxtInputMng->cursor_x = 0;
    pTxtInputMng->cursor_y = 0;
    pTxtInputMng->iCursorPos = iStrLen/2;
    pTxtInputMng->iLeftTopCharPos = 0;
    pTxtInputMng->iValidCharCount = iStrLen/2;
    pTxtInputMng->rowStartList = MSF_NULL;
    
    TVM_SetSize(pTxtInputMng, pTxtInputMng->cols, pTxtInputMng->iColsHasScrollbar, pTxtInputMng->rows,OP_FALSE);
    
    return TPI_WIDGET_OK;
}

/*=============================================================================
    FUNCTION:  TBM_Destroy

    DESCRIPTION:
        free dynamicly allocted buffer of text input buffer manager.

    ARGUMENTS PASSED:
        pTBM : text input buffer manager pointer.

    RETURN VALUE:
        0: successful.
        others : operation failed.

    IMPORTANT NOTES:
        None
=============================================================================*/
OP_INT16 TBM_Destroy(TBM_STRUCTURE_NAME_T  *pTBM)
{
    if(!pTBM || !pTBM->pTxtInputBuf)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if(pTBM->rows > 1 && pTBM->rowStartList)
    {
        WIDGET_FREE(pTBM->rowStartList);
    }
    
    WIDGET_FREE(pTBM->pTxtInputBuf);
    op_memset(pTBM, 0, sizeof(TBM_STRUCTURE_NAME_T));
    
    return TPI_WIDGET_OK;
}

/*=============================================================================
    FUNCTION:  TBM_SetContent

    DESCRIPTION:
        Copy string from specified string buffer into text input buffer manager  .

    ARGUMENTS PASSED:
        pTBM : address where source string buffer is at.
        newText : source string buffer.
        size : 0,copy call; othiers: only copy bytes of count size;

    RETURN VALUE:
        -1 : operation failed.
        others: Ther actual text byte count  that text manager accepted successfully.

    IMPORTANT NOTES:
        None
=============================================================================*/
OP_INT16 TBM_SetContent
(
    TBM_STRUCTURE_NAME_T  *pTBM, 
    OP_UINT8 * newText,
    OP_INT16 size
)
{
    OP_UINT16 iRealCopySize;
    int iValidBufSize;

    iRealCopySize = Ustrlen(newText) ; 
    iValidBufSize = pTBM->maxCharCount * 2;
    if(size > iRealCopySize)
    {
        size = iRealCopySize;
    }
    
    if(size == 0)   /* Copy all */
    {
        if(iRealCopySize > iValidBufSize)
        {
               iRealCopySize = iValidBufSize;    
        }
    }
    else        /* copy byte count of size */
    {
        if(size > iValidBufSize)
        {
            iRealCopySize = iValidBufSize;    
        } 
        else 
        {
            iRealCopySize  = size;
        }
    }

    op_memcpy(pTBM->pTxtInputBuf, newText, iRealCopySize);
    pTBM->pTxtInputBuf[iRealCopySize] = 0;
    pTBM->pTxtInputBuf[iRealCopySize+1] = 0;

    /* Modify text input buffer manager's attributes */
    pTBM->iValidCharCount = iRealCopySize/2;
    pTBM->iCursorPos = pTBM->iValidCharCount;
    
    return iRealCopySize;
    
}
/*=============================================================================
    FUNCTION:  TBM_GetContent

    DESCRIPTION:
        Copy string from text input buffer manager to specified string buffer.

    ARGUMENTS PASSED:
        pTBM : address where source string buffer is at.
        bStartFromZero : 
            OP_TRUE: the start index of content to be got is 0;
            OP_FALSE: the start index of content to be got is index of left top char in view.
        dataBuf : destination string buffer.
        size : 0,copy call; othiers: only copy bytes of count size;

    RETURN VALUE:
        <0 : operation failed.
        TPI_WIDGET_OK: successful.

    IMPORTANT NOTES:
        None
=============================================================================*/
OP_INT16 TBM_GetContent
(
    TBM_STRUCTURE_NAME_T *pTBM, 
    OP_BOOLEAN                     bStartFromZero,
    OP_UINT8                        *dataBuf,
    OP_INT16                         size
)
{
    OP_UINT16 iRealCopySize;
    OP_UINT8   *pChar;
    
    if(!pTBM)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;        
    }

    if( bStartFromZero == OP_TRUE )
    {
        iRealCopySize = pTBM->iValidCharCount*2;
        pChar = pTBM->pTxtInputBuf;
    }
    else
    {
        iRealCopySize = (pTBM->iValidCharCount -pTBM->iLeftTopCharPos )*2;
        pChar = pTBM->pTxtInputBuf + pTBM->iLeftTopCharPos * 2 ;
    }
    
    if(size != 0 && iRealCopySize > size)
    {
        iRealCopySize = size;
    }
    
    op_memcpy(dataBuf, pChar, iRealCopySize);
    dataBuf[iRealCopySize] = 0;
    dataBuf[iRealCopySize+1] = 0;        

    return TPI_WIDGET_OK;
}

/*=============================================================================
    FUNCTION:  TBM_GetContentLen

    DESCRIPTION:
        get string length from text input data buffer.

    ARGUMENTS PASSED:
        pTBM : point to text input manager buffer.
        bStartFromZero : 
                OP_TRUE: get the whole string length of text input data buffer.
                OP_FALSE: get the string started from left top char.

    RETURN VALUE:
        the byte count of the specified string.

    IMPORTANT NOTES:
        None.
=============================================================================*/
OP_UINT16  TBM_GetContentLen
(
    TBM_STRUCTURE_NAME_T  *pTBM,
    OP_BOOLEAN bStartFromZero
)
{
    if(bStartFromZero)
    {
        return pTBM->iValidCharCount * 2;
    }
    else 
    {
        return (pTBM->iValidCharCount  - pTBM->iLeftTopCharPos) * 2;
    }
 }

/*=============================================================================
    FUNCTION:  TBM_CursorTail

    DESCRIPTION:
        Shift cursor to the end of string.

    ARGUMENTS PASSED:
        pTBM : point to text input manager.

    RETURN VALUE:
        OP_FALSE: forwarding cursor failed. LCD doesn't need to be refreshed.
        OP_TRUE : forwarding cursor successful.LCD need to be refreshed.

    IMPORTANT NOTES:
        None
=============================================================================*/
OP_BOOLEAN TBM_CursorTail
(
    TBM_STRUCTURE_NAME_T  *pTBM
)
{
    OP_UINT16 iPlaceHolderCnt; 
    if(pTBM->iCursorPos == pTBM->iValidCharCount)
    {
        return OP_FALSE;
    }   
    pTBM->iCursorPos = pTBM->iValidCharCount;
    
    if(pTBM->rows == 1)
    {
        OP_INT16 iTmp;
        OP_UINT8   *pStr;

        pStr = pTBM->pTxtInputBuf;
        iPlaceHolderCnt = 0;
        for(iTmp = (pTBM->iValidCharCount * 2 - 2); iTmp >= 0; iTmp -= 2)
        {
            if  (TBM_UNICODE_VALUE((&pStr[iTmp])) <= 0x007f)
            {
                iPlaceHolderCnt++;
            }
            else
            {
                iPlaceHolderCnt +=2;
            } 

            if(iPlaceHolderCnt >= pTBM->cols)
            {
                if(iPlaceHolderCnt > pTBM->cols)
                {
                    iTmp += 2;
                }

                break;
            }
        }  

        if(iTmp < 0)
        {
            iTmp = 0;
        }
        pTBM->iLeftTopCharPos = iTmp/2;
        pTBM->cursor_x = TBM_GetPlaceholderCnt(pTBM->pTxtInputBuf,
                                                                    pTBM->iLeftTopCharPos, 
                                                                    pTBM->iCursorPos, 
                                                                    pTBM->cols, 
                                                                    OP_FALSE);
    }
    else
    {
        OP_UINT16  iCurWidth = 0;
        if(pTBM->bNeedScrollbar)
        {
            iCurWidth = pTBM->iColsHasScrollbar;
        }
        else
        {
            iCurWidth = pTBM->cols;
        }
        TBM_CalculateTotalRows(pTBM, iCurWidth);
           TVM_Build(pTBM);
        if(pTBM->cur_line >= pTBM->rows)
        {
            pTBM->top_line = pTBM->cur_line - pTBM->rows +1;
        }
        else
        {
            pTBM->top_line = 0;
        }
        
    }

    return OP_TRUE;
}

/*=============================================================================
    FUNCTION:  TBM_CursorHead

    DESCRIPTION:
        Shift cursor to the head of string.

    ARGUMENTS PASSED:
        pTBM : point to text input manager.

    RETURN VALUE:
        OP_FALSE: Needn't to shift cursor.
        OP_TRUE : forwarding cursorsuccessful.LCD need to be refreshed.

    IMPORTANT NOTES:
        None
=============================================================================*/
OP_BOOLEAN   TBM_CursorHead
(
    TBM_STRUCTURE_NAME_T  *pTBM
)
{
    pTBM->iLeftTopCharPos = 0;
    pTBM->iCursorPos = 0;
    if(pTBM->rows == 1)
    {
        pTBM->cursor_x = 0;
        return OP_TRUE;
    }
    else
    {
        OP_UINT16  iCurWidth = 0;
        if(pTBM->bNeedScrollbar)
        {
            iCurWidth = pTBM->iColsHasScrollbar;
        }
        else
        {
            iCurWidth = pTBM->cols;
        }
        TBM_CalculateTotalRows(pTBM, iCurWidth);
        TVM_Build(pTBM);
        if(pTBM->cur_line >= pTBM->rows)
        {
            pTBM->top_line = pTBM->cur_line - pTBM->rows +1;
        }
        else
        {
            pTBM->top_line = 0;
        }
        
    }
    
    return OP_TRUE;
}
/*=============================================================================
    FUNCTION:  TBM_CursorForward

    DESCRIPTION:
        Shift cursor forward(distance of one character placeholder)

    ARGUMENTS PASSED:
        pTBM : point to text input manager.

    RETURN VALUE:
        OP_FALSE: forwarding cursor failed. LCD doesn't need to be refreshed.
        OP_TRUE : 1:forwarding cursor one char successful.LCD need to be refreshed.
                        2:forwarding cursor two char successful.LCD need to be refreshed.

    IMPORTANT NOTES:
        None
=============================================================================*/
OP_UINT8   TBM_CursorForward
(
    TBM_STRUCTURE_NAME_T  *pTBM
)
{
    OP_UINT16 iPlacehldCnt;
    OP_UINT8   iFlag = 1; 
    
    if( pTBM == MSF_NULL 
        || pTBM->iCursorPos >= pTBM->maxCharCount )
    {
        return 0;
    }

    if(pTBM->iCursorPos >= pTBM->iValidCharCount)
    {
        /*
        TBM_EnterString(pTBM, (OP_UINT8*)"\x20\x0\x0\x0");
        */
        return 0;
    }
    else
    {
        pTBM->iCursorPos++;

        if(pTBM->rows == 1)
        {
            /*
             * Get placeholder count between left top visible char(including init str if exists) and corsor.
             */
            iPlacehldCnt = TBM_GetPlaceholderCnt(
                                    pTBM->pTxtInputBuf, 
                                    pTBM->iLeftTopCharPos, 
                                    pTBM->iCursorPos , 
                                    pTBM->cols,
                                    OP_TRUE);

            if(iPlacehldCnt >  pTBM->cols )
            {
                OP_UINT16 iCursorCharVal;
                OP_UINT16 iLeftTopCharVal;

                iCursorCharVal = TBM_UNICODE_VALUE(( pTBM->pTxtInputBuf +
                                                                                    2 * (pTBM->iCursorPos - 1 )));
                iLeftTopCharVal = TBM_UNICODE_VALUE((     pTBM->pTxtInputBuf + 
                                                                                  2 * pTBM->iLeftTopCharPos));
                if(iCursorCharVal <= 0x007f)  /* e: english char. cc: chinese char. *: any char */
                {  
                    
                    pTBM->iLeftTopCharPos ++;
                    if(iLeftTopCharVal > 0x007f)  /* cc|***|e */
                    {
                        OP_UINT16 iVal;
                        iVal = TBM_UNICODE_VALUE(( pTBM->pTxtInputBuf +
                                                                           2 * (pTBM->iCursorPos)));
                        /* If iVal is equal to 0,it indicates the end of unicode string*/
                        if(iVal > 0 && iVal <= 0x007f)
                        {
                            pTBM->iCursorPos ++;
                            iFlag = 2;
                        }                         
                    }
                }
                else        
                {
                    if(iLeftTopCharVal <= 0x007f)  /* e|***|cc */
                    {
                         pTBM->iLeftTopCharPos += 2;
                    }
                    else                                        /* cc|***|cc */
                    {
                        pTBM->iLeftTopCharPos++ ;
                    }
                }
            }

            pTBM->cursor_x = TBM_GetPlaceholderCnt(pTBM->pTxtInputBuf,
                                                                        pTBM->iLeftTopCharPos, 
                                                                        pTBM->iCursorPos, 
                                                                        pTBM->cols, 
                                                                        OP_FALSE);

        }
        else        /* Multi line*/
        {
            OP_UINT16  iCurWidth = 0;
            if(pTBM->bNeedScrollbar)
            {
                iCurWidth = pTBM->iColsHasScrollbar;
            }
            else
            {
                iCurWidth = pTBM->cols;
            }
            TBM_CalculateTotalRows(pTBM, iCurWidth);
            TVM_Build(pTBM);
            if(pTBM->cur_line >= pTBM->rows)
            {
                pTBM->top_line = pTBM->cur_line - pTBM->rows +1;
            }
            else
            {
                pTBM->top_line = 0;
            }
        }
    }
    return iFlag;
}
/*=============================================================================
    FUNCTION:  TBM_CursorBackward

    DESCRIPTION:
        Shift cursor backward (distance of one character placeholder)

    ARGUMENTS PASSED:
        pTBM : point to text input manager.

    RETURN VALUE:
        OP_FALSE: forwarding cursor failed. LCD doesn't need to be refreshed.
        OP_TRUE : forwarding cursor successful.LCD need to be refreshed.

    IMPORTANT NOTES:
        None
=============================================================================*/
OP_BOOLEAN TBM_CursorBackward
(
    TBM_STRUCTURE_NAME_T  *pTBM
)
{
    if(!pTBM  || pTBM->iCursorPos == 0)
    {
        return OP_FALSE;
    }

    pTBM->iCursorPos --; 

    if(pTBM->rows == 1)
    {
        if(pTBM->iCursorPos == pTBM->iLeftTopCharPos)  
        {
            if(pTBM->iLeftTopCharPos > 0 )
            {
                pTBM->iLeftTopCharPos --;
            }            
        }
        
        pTBM->cursor_x = TBM_GetPlaceholderCnt(pTBM->pTxtInputBuf,
                                                                            pTBM->iLeftTopCharPos, 
                                                                            pTBM->iCursorPos, 
                                                                            pTBM->cols, 
                                                                            OP_FALSE);            
    }
    else
    {
        OP_UINT16  iCurWidth = 0;
        if(pTBM->bNeedScrollbar)
        {
            iCurWidth = pTBM->iColsHasScrollbar;
        }
        else
        {
            iCurWidth = pTBM->cols;
        }
        TBM_CalculateTotalRows(pTBM, iCurWidth);
        TVM_Build(pTBM);
        if(pTBM->cur_line >= pTBM->rows)
        {
            pTBM->top_line = pTBM->cur_line - pTBM->rows +1;
        }
        else
        {
            pTBM->top_line = 0;
        }
    }

    return OP_TRUE;
}

/*=============================================================================
    FUNCTION:  TBM_EnterString

    DESCRIPTION:
        Enter string into text buffer manager at the position where cursor is.

    ARGUMENTS PASSED:
        pTBM : point to text input manager.
        pStr : source string to be entered.

    RETURN VALUE:
        OP_FALSE: entering string failed. LCD doesn't need to be refreshed.
        OP_TRUE : entering string successful.LCD need to be refreshed.

    IMPORTANT NOTES:
        None
=============================================================================*/
OP_BOOLEAN TBM_EnterString
(
    TBM_STRUCTURE_NAME_T  *pTBM,
    OP_UINT8*     pStr
)
{
    OP_UINT16  iAfterCousor, iSrcLen,iDst,iCpyLen,iLen;
    OP_UINT8        *pStrSrc, *pStrDst,*pStrTmp = OP_NULL,i,iRet;
    
    if( !pTBM  || !pStr )
    {
        return OP_FALSE;
    }

    iSrcLen = Ustrlen(pStr)/2;
    if(iSrcLen == 0 || iSrcLen > (pTBM->maxCharCount - pTBM->iValidCharCount))
    {
        if(iSrcLen > (pTBM->maxCharCount - pTBM->iValidCharCount))
        {
            OP_UINT8          volume;

            volume = SP_GetVolume( BELL_LEVEL_ID ).LEVEL;

            SP_Audio_play_request(RES_SOUND_0007, volume, OP_FALSE);
        }
        return OP_FALSE;
    }

    
    iAfterCousor = pTBM->iValidCharCount - pTBM->iCursorPos;
    if(iAfterCousor != 0)
    {
        pStrTmp = WIDGET_ALLOC(iAfterCousor * 2 + 2);
        if( !pStrTmp )
        {
            return OP_FALSE;
        }
        
        op_memcpy(  pStrTmp,
                                pTBM->pTxtInputBuf + pTBM->iCursorPos* 2, 
                                iAfterCousor * 2);
    }

    iCpyLen = iSrcLen + iAfterCousor;
    if((pTBM->maxCharCount - pTBM->iCursorPos) < iCpyLen)
    {
        iCpyLen = pTBM->maxCharCount - pTBM->iCursorPos;
    }

    /*Insert source string*/
    pStrDst = pTBM->pTxtInputBuf;
    if(pStrDst == MSF_NULL)
    {
        return OP_FALSE;
    }
    pStrDst += pTBM->iCursorPos * 2;
    pStrSrc = pStr;

    for(iDst = 0; iDst < iCpyLen; iDst++)
    {
        if(iDst == iSrcLen)
        {
            if( pStrTmp )
            {
                pStrSrc = pStrTmp;
            }
            else
            {
                break;
            }
        }
        *pStrDst++ = *pStrSrc++;
        *pStrDst++ = *pStrSrc++;
    }
    *pStrDst++ = '\0';
    *pStrDst++ = '\0';

    pTBM->iValidCharCount = pTBM->iCursorPos + iCpyLen;
    
    if(iDst >= iSrcLen)
    {
        iLen = iSrcLen;
    }
    else
    {
        iLen = iCpyLen;
    }

    if(pTBM->rows == 1)
    {
        for(i=0; i<iLen; i++)
        {
            iRet = TBM_CursorForward(pTBM);

            if(iRet == 2)
            {
                i++;
            }
        } 

        pTBM->cursor_x = TBM_GetPlaceholderCnt(pTBM->pTxtInputBuf,
                                                                            pTBM->iLeftTopCharPos, 
                                                                            pTBM->iCursorPos, 
                                                                            pTBM->cols, 
                                                                            OP_FALSE);
    }
    else
    {
        BOOL bNeedBefore = pTBM->bNeedScrollbar;
        pTBM->iCursorPos += iLen;
#ifdef _TOUCH_PANEL
        TBM_AdustCols(pTBM, OP_TRUE);
        if (bNeedBefore != pTBM->bNeedScrollbar)
        {
            TVM_Build( pTBM);
        }
        else
        {
            if (iSrcLen > 2)
            {
                TVM_Build( pTBM);
            }
            else
            {
                TVM_InsertChar(pTBM, (OP_UINT16)(TBM_UNICODE_VALUE(pStr)));
            }
        }
#else        
        TVM_Build( pTBM);
#endif
    }

    if(iAfterCousor != 0)
    {
        WIDGET_FREE(pStrTmp);
    }
    
  return OP_TRUE;
}

/*=============================================================================
    FUNCTION:  TBM_DelCursorChar

    DESCRIPTION:
        delete the char at the place where cursor is at.

    ARGUMENTS PASSED:
        pTBM : point to text input manager.

    RETURN VALUE:
        OP_FALSE: operation failed. LCD doesn't need to be refreshed.
        OP_TRUE : operationsuccessful.LCD need to be refreshed.

    IMPORTANT NOTES:
        None
=============================================================================*/
OP_BOOLEAN   TBM_DelCursorChar
(
    TBM_STRUCTURE_NAME_T  *pTBM
)
{
    OP_INT16  i;
    OP_UINT16 delete_char;
    
    if(pTBM == MSF_NULL)
    {
        return OP_FALSE;
    }

    if(pTBM->rows == 1)
    {
        if(pTBM->iLeftTopCharPos > 0) 
        {
            OP_UINT16 iTmp,iTmpPlaceholderCnt,iPlaceholderCnt,iLeftTopCharIdx;
            OP_UINT16 iLeftTopCharVal,iCurVal;
            OP_UINT8 *pBuf;

            pBuf = pTBM->pTxtInputBuf;
            iLeftTopCharIdx = pTBM->iLeftTopCharPos;
            if(pTBM->iCursorPos == pTBM->iValidCharCount)
            {
                iPlaceholderCnt = TBM_GetPlaceholderCnt( pBuf, 
                                                          pTBM->iLeftTopCharPos, 
                                                          (OP_UINT16)(pTBM->iCursorPos - 1), 
                                                          pTBM->cols,
                                                          OP_TRUE);
                /* count placeholder count left at the right of text input after deleting char*/
                iPlaceholderCnt = pTBM->cols - iPlaceholderCnt;
            }
            else
            {
                iCurVal = TBM_UNICODE_VALUE(&pBuf[2*(pTBM->iCursorPos - 1)]);
                iLeftTopCharVal = TBM_UNICODE_VALUE(&pBuf[2 * (iLeftTopCharIdx-1) ]);
                if(iCurVal <= 0x007f)
                {
                    if(iLeftTopCharVal > 0x007f)
                    {
                        iPlaceholderCnt = TBM_GetPlaceholderCnt( pBuf, 
                                                                  pTBM->iLeftTopCharPos, 
                                                                  pTBM->iValidCharCount, 
                                                                  pTBM->cols,
                                                                  OP_TRUE);   
                        if(iPlaceholderCnt < pTBM->cols)
                        {
                            iPlaceholderCnt = 2;
                        }
                        else
                        {
                            iPlaceholderCnt = 1;
                        }
                    }
                    else
                    {
                        iPlaceholderCnt = 1;
                    }
                }
                else
                {
                    iPlaceholderCnt = 2;            
                }
                
            }
            

            iTmpPlaceholderCnt = 0;
            for(iTmp = 0; iTmp < 3 && iLeftTopCharIdx >= 1; iTmp ++ )
            {
                iLeftTopCharIdx --;    
                iLeftTopCharVal = TBM_UNICODE_VALUE(&pBuf[2 * iLeftTopCharIdx ]);
                if(iLeftTopCharVal <= 0x007f)
                {
                    iTmpPlaceholderCnt += 1;
                }
                else
                {
                    iTmpPlaceholderCnt += 2;
                }

                if(iTmpPlaceholderCnt > iPlaceholderCnt)
                {
                    iLeftTopCharIdx ++;
                    break;
                }
            }

            pTBM->iLeftTopCharPos = iLeftTopCharIdx;
        }    
    }
    
    if(pTBM->iCursorPos == 0)
    {
        return OP_FALSE;
        /*
        if(pTBM->iValidCharCount == 0)
        {
            return OP_FALSE;
        }
        else
        {
            pTBM->iCursorPos  = 1;
        }
        */
    }

    /* del char*/
    delete_char = TBM_UNICODE_VALUE(&(pTBM->pTxtInputBuf[pTBM->iCursorPos * 2-2]));

    for(i=pTBM->iCursorPos * 2; i<(2*pTBM->iValidCharCount + 2);i+=2)
    {
        pTBM->pTxtInputBuf[i-2] = pTBM->pTxtInputBuf[i];
        pTBM->pTxtInputBuf[i-1] = pTBM->pTxtInputBuf[i+1];
    }
    
    pTBM->iCursorPos --;
    pTBM->iValidCharCount --;

    if(pTBM->rows == 1)
    {
        pTBM->cursor_x = TBM_GetPlaceholderCnt(pTBM->pTxtInputBuf,
                                                                    pTBM->iLeftTopCharPos, 
                                                                    pTBM->iCursorPos, 
                                                                    pTBM->cols, 
                                                                    OP_FALSE);
    
    }
    else
    {
        BOOL bNeedBefore = pTBM->bNeedScrollbar;
        TBM_AdustCols(pTBM, OP_FALSE);
#ifdef _TOUCH_PANEL
        if (bNeedBefore != pTBM->bNeedScrollbar)
        {
            TVM_Build(pTBM);
        }
        else
        {
            TVM_DeleteChar(pTBM, delete_char);
        }
#else
        TVM_Build(pTBM);
#endif
        TBM_AdustCols(pTBM, OP_FALSE);

    }
    return OP_TRUE;
}

/*=============================================================================
    FUNCTION:  TBM_DelString

    DESCRIPTION:
        Delete the string from 0 to cursor position. If cursor is at the head or tail of the string,
      clear the whole string.

    ARGUMENTS PASSED:
        pTBM : point to text input manager.

    RETURN VALUE:
        OP_FALSE: operation failed. LCD doesn't need to be refreshed.
        OP_TRUE : operationsuccessful.LCD need to be refreshed.

    IMPORTANT NOTES:
        None
=============================================================================*/
OP_BOOLEAN   TBM_DelString
(
    TBM_STRUCTURE_NAME_T  *pTBM
)
{
    if( pTBM->iCursorPos == 0 || 
         pTBM->iCursorPos == pTBM->iValidCharCount)
    {
           TBM_ClearContent(pTBM);
    }
    else
    {
        OP_INT16 i,j = 0;

        for(i = pTBM->iCursorPos * 2; i < pTBM->iValidCharCount * 2 + 2; i++)
        {
            pTBM->pTxtInputBuf[j++] = pTBM->pTxtInputBuf[i];
        }

        pTBM->iLeftTopCharPos = 0;
        pTBM->iCursorPos = 0;
        pTBM->iValidCharCount = j/2 -1;
        
        if(pTBM->rows == 1)
        {
            TBM_CursorHead(pTBM);
        }
        else
        {
            TBM_AdustCols(pTBM, OP_FALSE);
            TVM_Build(pTBM);
        }        
    }


    return OP_TRUE;
}

/*=============================================================================
    FUNCTION:  TBM_ClearContent

    DESCRIPTION:
        empty text input string buffer.

    ARGUMENTS PASSED:
        pTBM : point to text input manager..

    RETURN VALUE:
        0: successful.
        others : operation failed.

    IMPORTANT NOTES:
        None
=============================================================================*/
OP_INT16 TBM_ClearContent
(
    TBM_STRUCTURE_NAME_T  *pTBM
)
{
    if( ! pTBM )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if(pTBM->iValidCharCount == 0)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        pTBM->iLeftTopCharPos = 0;
        pTBM->iCursorPos = 0;
        pTBM->iValidCharCount = 0;
        pTBM->pTxtInputBuf[0] = 0;
        pTBM->pTxtInputBuf[1] = 0;

        if(pTBM->rows == 1)
        {
            TBM_CursorHead(pTBM);
        }
        else
        {
            TVM_Build(pTBM);
        }       
    }
    return TPI_WIDGET_OK;
}

/*=============================================================================
    FUNCTION:  TBM_CursorUp

    DESCRIPTION:
        Scroll cursor up one line .

    ARGUMENTS PASSED:
        pTBM : point to text input manager.

    RETURN VALUE:
        OP_FALSE: scrolling cursor up failed. LCD doesn't need to be refreshed.
        OP_TRUE : scrolling cursor up successful.LCD need to be refreshed.

    IMPORTANT NOTES:
        None
=============================================================================*/
OP_BOOLEAN TBM_CursorUp
(
    TBM_STRUCTURE_NAME_T  *pTBM
)
{
    OP_UINT8      i, j;
    OP_UINT16    count;
    OP_UINT16    value;
    OP_UINT16        iPlacehldCnt = 0;
    OP_UINT16   iCursorPos;
    OP_UINT8*   pBuffer;
    OP_UINT16  iCurWidth = 0;
    if(pTBM == OP_NULL || pTBM->rows == 1)
    {
        return OP_FALSE;
    }

    pBuffer = pTBM->pTxtInputBuf;
    iCursorPos = pTBM->iCursorPos;
    
    if(pTBM->cursor_y == 0)
    {
        /* the Cursor already at the first char of the buffer  */
        if( pTBM->iCursorPos == 0 )
        {
            return OP_FALSE;
        }
        pTBM->iCursorPos = 0;
    }
    else
    {
        i = (pTBM->cursor_y + pTBM->index - 1)%pTBM->cnt;
        j = (pTBM->cursor_y + pTBM->index )%pTBM->cnt;
        count = pTBM->rowStartList[j] - pTBM->rowStartList[i];

        iCursorPos = pTBM->rowStartList[i];
        value = TBM_UNICODE_VALUE((pBuffer + 2*iCursorPos));
        
        for(i=0; i<count; i++)
        {
            if  (value <= 0x007f)
            {
                iPlacehldCnt++;
            }
            else
            {
                iPlacehldCnt += 2;
            }

            if( iPlacehldCnt > pTBM->cursor_x)
            {
                break;
            }

            (iCursorPos) ++;
            
            value = TBM_UNICODE_VALUE((pBuffer + 2*iCursorPos));
        }
        
        if( iPlacehldCnt <= pTBM->cursor_x)
        {
            (iCursorPos) --;
        }

        pTBM->iCursorPos = iCursorPos;
    }
    
    if(pTBM->bNeedScrollbar)
    {
        iCurWidth = pTBM->iColsHasScrollbar;
    }
    else
    {
        iCurWidth = pTBM->cols;
    }
    TBM_CalculateTotalRows(pTBM, iCurWidth);
    TVM_Build(pTBM);
    if(pTBM->top_line >0)
    {
        pTBM->top_line--;
    }
    return OP_TRUE;
}

/*=============================================================================
    FUNCTION:  TBM_CursorDown

    DESCRIPTION:
        Scroll cursor down one line .

    ARGUMENTS PASSED:
        pTBM : point to text input manager.

    RETURN VALUE:
        OP_FALSE: scrolling cursor down failed. LCD doesn't need to be refreshed.
        OP_TRUE : scrolling cursor down successful.LCD need to be refreshed.

    IMPORTANT NOTES:
        None
=============================================================================*/
OP_BOOLEAN TBM_CursorDown
(
    TBM_STRUCTURE_NAME_T  *pTBM
)
{
    OP_UINT8    iSubIdx;       /* sub index of rowStartList which contains each
                                             * row's 1st char index in text input buffer 
                                             */           
    OP_UINT16        value;
    OP_UINT16        iPlacehldCnt = 0;
    OP_UINT16   iCursorPos;
    OP_UINT8*   pBuffer;
    OP_UINT16   iNextPos;             /* the  line after cursor position*/ 
    OP_UINT16   iCurWidth =0;
    pTBM = pTBM;
    if(pTBM == OP_NULL || pTBM->rows == 1)
    {
        return OP_FALSE;
    }
    
    pBuffer = pTBM->pTxtInputBuf;
    iCursorPos = pTBM->iCursorPos;
    
    value = TBM_UNICODE_VALUE((pBuffer + 2*iCursorPos));
    if(value == 0)
    {
        return OP_FALSE;
    }

    iSubIdx = (pTBM->cursor_y + pTBM->index + 1)%pTBM->cnt;
    iCursorPos = pTBM->rowStartList[iSubIdx];

    value = TBM_UNICODE_VALUE((pBuffer + 2*iCursorPos));

    /* Get next row's 1st char index*/
    if((pTBM->cursor_y + 1) < pTBM->rows)
    {
        iNextPos = pTBM->rowStartList[(iSubIdx+1)%pTBM->cnt];
    }
    else        /* cursor is displayed at the lowest row of textinput view area. */
    {
        if(pTBM->bNeedScrollbar)
        {
            iCurWidth = pTBM->iColsHasScrollbar;
        }
        else
        {
           iCurWidth = pTBM->cols;
        }
        TVM_GetNextPos(pTBM->pTxtInputBuf, 
                                            pTBM->rowStartList[iSubIdx], 
                                            iCurWidth,
                                            &iNextPos);
    }

    /* Get new cursor position */
    while(value != 0x0000)
    {
        if  (value <= 0x007f)
        {
            iPlacehldCnt++;
        }
        else
        {
            iPlacehldCnt += 2;
        }
        if(( iPlacehldCnt > pTBM->cursor_x) || (value == 0x000A))
        {
            break;
        }

        (iCursorPos) ++;
        if(iCursorPos > iNextPos )
        {
            iCursorPos --;
            break;
        }

        value = TBM_UNICODE_VALUE((pBuffer + 2*iCursorPos));
    }
    
    pTBM->iCursorPos = iCursorPos;
    
    if(pTBM->bNeedScrollbar)
    {
        iCurWidth = pTBM->iColsHasScrollbar;
    }
    else
    {
        iCurWidth = pTBM->cols;
    }
    TBM_CalculateTotalRows(pTBM, iCurWidth);
    TVM_Build(pTBM);
    if(pTBM->top_line + pTBM->rows < pTBM->TotalRows )
    {
        if(pTBM->cur_line >= pTBM->rows)
        {
            pTBM->top_line++;
        }

    }
    else
    {
        pTBM->top_line = pTBM->TotalRows - pTBM->rows;
    }
    return OP_TRUE;
}

/*=============================================================================
    FUNCTION:  TBM_GetPlaceholderCnt

    DESCRIPTION:
        Get placeholder count of string specified by char index  of begin char index and end char index.

    ARGUMENTS PASSED:
        pStr : point to string buffer..
        iBegin :   index of the first char of the string.
        iEnd    :   index of the last char of the string.
        iLinePlaceholderCnt : placeholder count of one line in view area.
        bPadding : 
            OP_TRUE: include placeholder which contain nothing,just for padding.
            OP_FALSE: discard padding placeholder.

    RETURN VALUE:
        -1 : unexpected error occured.
        others : placeholder count of specified string.

    IMPORTANT NOTES:
        the string placeholder count in text input view is greater than or equal to the num counted by this 
    method: one english char ocupies one placeholder and one chinese ocupy two.
        e.g:   |eeee_|   e: one english  char.
                 |cceee|   cc: one chinese char.
                                _: empty,the width is one placeholder.
        we can get the conclusion that string"eeeecceee" has  10 placeholder  in textinput view,not 9,for 
    one chinese char should not be tookapart.
=============================================================================*/
static OP_INT16 TBM_GetPlaceholderCnt
(
    OP_UINT8* pStr, 
    OP_UINT16 iBegin,
    OP_UINT16 iEnd,
    OP_UINT16 iLinePlacehldCnt,
    OP_BOOLEAN bPadding
)
{
    OP_UINT16 iTmp,iPlacCnt = 0;

    if(!pStr || iEnd < iBegin)
    {
        return -1;
    }
    
    for(iTmp = iBegin*2; iTmp < iEnd*2; iTmp +=2)
    {
            if  (TBM_UNICODE_VALUE((&pStr[iTmp])) <= 0x007f)
            {
                iPlacCnt ++;
            }
            else
            {
                if( (iPlacCnt + 1) % iLinePlacehldCnt == 0)
                {
                    iPlacCnt += 3;
                }
                else
                {
                    iPlacCnt +=2;
                }
            }        
    }

    if(bPadding)
    {
        if((iPlacCnt + 1) % iLinePlacehldCnt == 0)
        {
            if(TBM_UNICODE_VALUE((&pStr[iTmp])) > 0x007f)
            {
                iPlacCnt += 1;
            }
        }
    }
    
    return iPlacCnt;
}

/*=============================================================================
    FUNCTION:  TBM_AdjustInitCursorPos

    DESCRIPTION:
        Set cursor positionl.

    ARGUMENTS PASSED:
        pTextInput : point to text input .

    RETURN VALUE:
        TPI_WIDGET_OK : setting cursor position successful.
        TPI_WIDGET_ERROR_INVALID_PARAM : error has occured.    
        
    IMPORTANT NOTES:
        None
=============================================================================*/
OP_INT16  TBM_AdjustInitCursorPos
( 
    TBM_STRUCTURE_NAME_T   *pTBM
)
{   
    OP_UINT16 iStrPlacehldCnt;
    
    if(!pTBM || pTBM->rows == 0 )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if(pTBM->rows == 1)
    {
#ifdef _KEYPAD
        int i;
        pTBM->iLeftTopCharPos = 0;
        pTBM->iCursorPos = 0;
        for(i=0; i<pTBM->iValidCharCount; i++)
        {
            OP_UINT8 iRet = 0;
            iRet = TBM_CursorForward(pTBM);
            if(iRet == 2)
            {
                i++;
            }
        } 
#endif            
        iStrPlacehldCnt = TBM_GetPlaceholderCnt(pTBM->pTxtInputBuf,
                                                                pTBM->iLeftTopCharPos,
                                                                pTBM->iValidCharCount, 
                                                                pTBM->cols,
                                                                OP_TRUE);
        
        if(iStrPlacehldCnt > pTBM->cols)
        {
            pTBM->iCursorPos = pTBM->iLeftTopCharPos;
            pTBM->cursor_x = pTBM->iLeftTopCharPos;
        }
        else
        {
            pTBM->iCursorPos = pTBM->iValidCharCount;
            pTBM->cursor_x = iStrPlacehldCnt;
        }
    }
    else
    {
        pTBM->iCursorPos = pTBM->iValidCharCount;
        TBM_AdustCols(pTBM, OP_TRUE);
        TVM_Build(pTBM);
        /* If view area is full, set cursor at the head of string.*/
        /* remarked by zyk
        if(pTBM->rowStartList[0] != 0)
        {
            pTBM->iCursorPos =0;
            TVM_Build(pTBM);
        }
        */
    }

    return TPI_WIDGET_OK;
}
/*=============================================================================
    FUNCTION:  TBM_AdjustCursorForScroll

    DESCRIPTION:
        Adjust view information after textinput or editor's size has been modified.

    ARGUMENTS PASSED:
        pTextInput : point to text input .
        pInputSize  : size of text input.
        iLabLineCnt : line count of label.
        iChangeSize :  client area size of textinput is changed.

    RETURN VALUE:
        TPI_WIDGET_OK : operation is successful.
        TPI_WIDGET_ERROR_INVALID_PARAM : error has occured.    
        
    IMPORTANT NOTES:
        None
=============================================================================*/
OP_INT16  TBM_AdjustCursorForScroll
( 
    OP_UINT32    hTextInput,
    MsfSize  *pInputSize,
    OP_UINT16 iLabLineCnt,
    OP_INT16 iChangeSize
)
{
    MsfTextInput *pTextInput;
    MsfWidgetType   wt;
    MsfInput    *pInput;
    OP_INT16 iEngCharsPerLine,iLineCnt, iAdjustEngCharsPerLine;
//    OP_UINT16 iCurWidth = 0;

    if( !pInputSize )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pTextInput = (MsfTextInput*)seekWidget(hTextInput, &wt);
    if(!pTextInput || wt != MSF_TEXTINPUT)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pInput = &pTextInput->input;
    
    /* Get view information*/
    widgetInputGetEngCharSize(hTextInput,pInputSize,&iEngCharsPerLine, &iAdjustEngCharsPerLine,&iLineCnt);    
    iLineCnt -= iLabLineCnt;
    pInput->txtInputBufMng.rows = iLineCnt;
    pInput->txtInputBufMng.iColsHasScrollbar = iAdjustEngCharsPerLine;
    if(iChangeSize >0)
    {
        TBM_AdustCols(&pInput->txtInputBufMng, OP_FALSE);
    }
    else if(iChangeSize < 0 )
    {
       TBM_AdustCols(&pInput->txtInputBufMng, OP_TRUE);
    }
   
    TVM_SetSize(&pInput->txtInputBufMng, iEngCharsPerLine, iAdjustEngCharsPerLine, iLineCnt, OP_TRUE);
    return TPI_WIDGET_OK;
}

/*=============================================================================
    FUNCTION:  TBM_InsertString

    DESCRIPTION:
        external interface for IMM module to insert string into textinput buffer.

    ARGUMENTS PASSED:
        handle : point to text input .
        pStr  : source string

    RETURN VALUE:
        OP_TRUE : Inserting operation is  successful.
        OP_FALSE : Inserting operation is  failed.    
        
    IMPORTANT NOTES:
        None
=============================================================================*/
OP_BOOLEAN TBM_InsertString( OP_UINT32 handle, OP_UINT8 *pStr, OP_BOOLEAN bRefresh )
{
    MsfTextInput    *pTextInput;
    MsfWidgetType   wt;
    MsfInput        *pInput;
    OP_BOOLEAN      bInserted;
    OP_UINT8        *pChar;
    int             iStrLen;
    
    if( !pStr || ( *(pStr +1 ) == 0x00 && *pStr == 0x00))
    {
        return OP_FALSE;
    }

    pTextInput = (MsfTextInput*)seekWidget(handle, &wt);
    if( !pTextInput || wt != MSF_TEXTINPUT )
    {
        return OP_FALSE;
    }

    pInput = &pTextInput->input;
    
    /*  singleline textinput cannot insert enter char  */
    if( pInput->singleLine == 1 && (*(pStr +1 ) == 0x00 && *pStr == 0x0A ))
    {
        return OP_FALSE;
    }

    iStrLen = Ustrlen(pStr);
    if( iStrLen > 2 )  /* maybe QEng input method */
    {
        pChar = pInput->txtInputBufMng.pTxtInputBuf + pInput->txtInputBufMng.iCursorPos * 2 - 2;

        /*
         *  tranfer the first char to upcase in 3 case:
         *  1. current cursor is at the first position of the text buffer
         *  2. the first char in the text buffer is ' ', and current cursor is at the second position of the text buffer
         *  3. the two chars before current position is '. ' in the text buffer
         */
        if( pInput->txtInputBufMng.iCursorPos == 0 
          || (pInput->txtInputBufMng.iCursorPos == 1 && (*(pChar + 1 ) == 0 && *pChar == ' '))
          || (*(pChar-1) == 0 && *(pChar - 2) == '.' && (*(pChar + 1 ) == 0 && *pChar == ' ')))
        {
            if( *(pStr+1) == 0 && 'a' <= *pStr && 'z' >= *pStr )
            {
                *pStr -= ('a' - 'A');
            }
        }
    }
    bInserted =  TBM_EnterStringEx(&(pInput->txtInputBufMng), (OP_UINT8*)pStr, iStrLen, pInput->textType);
    if( bInserted== OP_TRUE  )
    {
#ifdef _NO_CLEAR_KEY
#if 0
        if (pInput->txtInputBufMng.iValidCharCount > 0)
        {
            util_get_text_from_res_w_max_len(PMPT_SKEY_DELETE, skeyStr, SKEY_MAX_CHAR*2 );
            pTextInput->gadgetData.softKeyList[2] = skeyStr;
        }
#endif        
#endif        
        if( bRefresh == OP_TRUE )
        {
            widgetRedraw( pTextInput, MSF_TEXTINPUT, handle, OP_TRUE);
        }

        if( pTextInput->gadgetData.propertyMask & MSF_GADGET_PROPERTY_NOTIFY)
        {
            if(pTextInput->gadgetData.cbStateChange)
            {
                (void)pTextInput->gadgetData.cbStateChange(pTextInput->gadgetData.modId, handle, MsfNotifyStateChange, OP_NULL);
            }
            else
            {
                TPIc_widgetNotify(pTextInput->gadgetData.modId, handle, MsfNotifyStateChange);
            }            
        }
    }
    
    return bInserted;
}

OP_BOOLEAN TBM_DeleteCurChar( OP_UINT32 handle, OP_BOOLEAN bRefresh )
{
    MsfTextInput        *pTextInput;
    MsfWidgetType   wt;
    MsfInput           *pInput;
    OP_BOOLEAN      bDeleted;
    
    pTextInput = (MsfTextInput*)seekWidget(handle, &wt);
    if( !pTextInput || wt != MSF_TEXTINPUT )
    {
        return OP_FALSE;
    }

    pInput = &pTextInput->input;
    
    bDeleted = TBM_DelCursorChar(&pInput->txtInputBufMng);
    if( bDeleted == OP_TRUE)
    {
        if( bRefresh == OP_TRUE )
        {
            widgetRedraw( OP_NULL, MSF_TEXTINPUT, handle, OP_TRUE);
        }

        if( pTextInput->gadgetData.propertyMask & MSF_GADGET_PROPERTY_NOTIFY )
        {
            if(pTextInput->gadgetData.cbStateChange)
            {
                (void)pTextInput->gadgetData.cbStateChange(pTextInput->gadgetData.modId, handle, MsfNotifyStateChange, OP_NULL);
            }
            else
            {
                TPIc_widgetNotify(pTextInput->gadgetData.modId, handle, MsfNotifyStateChange);
            }            
        }
    }
    return OP_TRUE;
}

/*=============================================================================
    FUNCTION:  TBM_AdustCols

    DESCRIPTION:
        Scroll bar and word per line are adjusted when string are inserted or deleted.

    ARGUMENTS PASSED:
        pTBM :  point to TBM_STRUCTURE_NAME_T .
        bAdustType  : TRUE---- Insert string
                      FALS---- Del string

    RETURN VALUE:      
        
    IMPORTANT NOTES:
        None
=============================================================================*/
void TBM_AdustCols(TBM_STRUCTURE_NAME_T  *pTBM, OP_BOOLEAN bAdustType)
{
    TBM_CalculateTotalRows(pTBM,pTBM->cols);/* Get total rows parameter*/
    if(pTBM->TotalRows > pTBM->rows)    /* wether scrollbar is needed */
    {   
        if(bAdustType)/*  insert string */
        {
            if(!pTBM->bNeedScrollbar)/* need scrollbar */
            {
                TBM_CalculateTotalRows(pTBM, pTBM->iColsHasScrollbar); /* get new total rows */
                pTBM->bNeedScrollbar = OP_TRUE;
            }
        } 
        else /* delete string */
        {
             if(pTBM->bNeedScrollbar)
             {
               
                 TBM_CalculateTotalRows(pTBM, pTBM->cols); /* get new total rows */
                 if(pTBM->TotalRows <= pTBM->rows)/* need clear scrollbar */
                 {
                     
                     pTBM->bNeedScrollbar = OP_FALSE;
                 }
                 else
                 {
                    TBM_CalculateTotalRows(pTBM, pTBM->iColsHasScrollbar); /* get new total rows */   
                 }
             }
        }
    
    }
    else
    {
        if(!bAdustType)
        {
            if(pTBM->bNeedScrollbar)
            {
               
                TBM_CalculateTotalRows(pTBM, pTBM->cols); /* get new total rows */
                if(pTBM->TotalRows <= pTBM->rows)
                {
                    pTBM->bNeedScrollbar = OP_FALSE;
                }
                else
                {
                    TBM_CalculateTotalRows(pTBM, pTBM->iColsHasScrollbar); /* get new total rows */ 
                }
            }
            
        }
        
        
    }

    pTBM->top_line = pTBM->cur_line - pTBM->cursor_y;

}

/*=============================================================================
    FUNCTION:  TBM_CalculateTotalRows

    DESCRIPTION:
        calculate total rows and current line of cursor

    ARGUMENTS PASSED:
        pTBM :  point to TBM_STRUCTURE_NAME_T .
        iCharsPerLine  : chars per line
                     

    RETURN VALUE:      
        
    IMPORTANT NOTES:
        None
=============================================================================*/
void TBM_CalculateTotalRows(TBM_STRUCTURE_NAME_T  *pTBM, OP_UINT16 iCharsPerLine)
{
    OP_UINT8    *pBuffer;
    OP_UINT16   iStartPos, iNextPos;
    OP_UINT16   iTotalRows = 0;
    
    iStartPos = 0;
    iNextPos = 0;
    pBuffer = pTBM->pTxtInputBuf;
    pTBM->cur_line = 0;
    
    while(TVM_GetNextPos(pBuffer, iStartPos, iCharsPerLine, &iNextPos))
    {
        iTotalRows++;
        if((pTBM->iCursorPos >= iStartPos) &&
            (pTBM->iCursorPos < iNextPos))
        {
            pTBM->cur_line = iTotalRows-1; /*Get the cursor rows*/
        }
        iStartPos = iNextPos;
    }
    if(pTBM->iCursorPos == iNextPos)
    {
        pTBM->cur_line = iTotalRows-1;
    }
    if (iStartPos > 0)
    {
        if (pBuffer[iStartPos * 2 -2] == 0x0A && pBuffer[iStartPos * 2 - 1] == 0x00)
        {
            iTotalRows ++;
            if(pTBM->iCursorPos == iNextPos)
            {
                pTBM->cur_line ++;
            }
        }
    }

    pTBM->TotalRows = iTotalRows;/* Get total rows */
}

OP_BOOLEAN TBM_SetCursor
(
    TBM_STRUCTURE_NAME_T *pTBM,
    const LCD_POINT_T pt_start,
    const LCD_POINT_T pt_clicking,
    const DS_FONTATTR font_attr
)
{
    OP_INT16                    font_width,font_height;
    OP_UINT16                   start_pos, y_pos, x_pos;
    OP_UINT8                    count, ch_len, multiple, i;
    OP_UINT16                   value;
    OP_UINT16                    cur_cols = 0;

    if (pTBM == OP_NULL)
    {
        return OP_FALSE;
    }

    if(pTBM->rowStartList== OP_NULL)
    {
        return OP_FALSE;
    }
    if(pTBM->bNeedScrollbar)
    {
        cur_cols = pTBM->iColsHasScrollbar;
    }
    else
    {
        cur_cols = pTBM->cols;
    }
    widgetGetFontSizeInEngChar(font_attr,&font_width,&font_height);
    
    //
    // Set y value on the screen.
    //
    y_pos = (pt_clicking.y - pt_start.y) / GADGET_LINE_HEIGHT;

    //
    // Check if y value is in the valid range.
    //
    if (y_pos >= pTBM->rows)
    {
        return OP_FALSE;
    }

    //
    // Check if the cursor can locate in the next line of current text.
    //
    // Must have got something here
    if (pTBM->cnt <= 1 )
    {
        return OP_TRUE;
    }

    if (y_pos > pTBM->cnt -1)
    {
        return OP_TRUE;
    }

    if (y_pos == (pTBM->cnt -1)
        && (TBM_UNICODE_VALUE(pTBM->pTxtInputBuf + (pTBM->rowStartList[pTBM->cnt -1] - 1) * 2) !=0x0A))
    {
        return OP_TRUE;
    }
    
    pTBM->cursor_y = (OP_UINT8)y_pos;
    
    start_pos= pTBM->rowStartList[pTBM->index + y_pos];

    if (pTBM->index + y_pos >= pTBM->cnt -1)
    {
        count = Ustrlen(pTBM->pTxtInputBuf)/2 - start_pos;
    }
    else
    {
        count = pTBM->rowStartList[pTBM->index + y_pos + 1] - pTBM->rowStartList[pTBM->index + y_pos];
    }

    ch_len = 0;
    if (pt_clicking.x <= pt_start.x)
    {
        x_pos = 0;
    }
    else
    {
        x_pos = pt_clicking.x - pt_start.x;
    }
    
    pTBM->cursor_x = 0xFF;
    
    for(i=0; i<count; i++)
    {
        value   = pTBM->pTxtInputBuf[(start_pos + i) * 2 + 1];
        value <<= 8;
        value  |= pTBM->pTxtInputBuf[(start_pos + i) * 2 + 0];

        if (value == 0x0A || value == 0x00)
        {
            pTBM->cursor_x = ch_len;
            pTBM->iCursorPos = start_pos + i;
            break;
        }
        if  (value <= 0x007f)
        {
            multiple = 1;
        }
        else
        {
            multiple = 2;
        }

        if ( x_pos >= ch_len * font_width
            && x_pos < ch_len * font_width + font_width * multiple/2)
        {
            pTBM->cursor_x = ch_len;
            pTBM->iCursorPos = start_pos + i;
            break;
        }
        else if (x_pos >= (ch_len * font_width + font_width * multiple/2)
            && x_pos <= (ch_len + multiple) * font_width)
        {
            pTBM->cursor_x = ch_len + multiple;
            pTBM->iCursorPos = start_pos + i + 1;
            break;
        }

        ch_len +=multiple;

        if( ch_len >= cur_cols)
        {
            break;
        }
    }    

    if (pTBM->cursor_x == 0xFF)
    {        
        pTBM->cursor_x = ch_len;
        pTBM->iCursorPos= start_pos + count;
    }

    return OP_TRUE;
}



OP_BOOLEAN TBM_Scroll(TBM_STRUCTURE_NAME_T *pTBM, SCROLLBAR_NOTIFICATION_T scrollFlag)
{
    OP_UINT16 iCount = 0;
    OP_UINT16 iNextIndex = 0;
    OP_UINT8    *pBuffer;
    OP_UINT16   iStartPos, iNextPos;
    OP_UINT16   iCurWidth = 0;
    OP_UINT16    updateCount = 0;
    OP_UINT16  old_top_line;
    
    if(OP_NULL == pTBM)
    {
        return OP_FALSE;
    }
    if(pTBM->TotalRows <= pTBM->rows )/* less than a page */
    {
        pTBM->top_line = 0;
        return OP_FALSE;
    }
    old_top_line = pTBM->top_line;
    
    switch(scrollFlag)
    {
    case SB_NOTIFY_LINE_UP:
        if(pTBM->top_line > 0)
        {
            pTBM->top_line--;
        }
        break;
    case SB_NOTIFY_LINE_DOWN:
        if(pTBM->top_line +pTBM->rows < pTBM->TotalRows)
        {
            pTBM->top_line++;
        }
        else /*  the last page */
        {
            pTBM->top_line = pTBM->TotalRows - pTBM->rows;
        }           
        break;
    case SB_NOTIFY_PAGE_UP:
        if(pTBM->top_line > pTBM->rows - 1)
        {
            pTBM->top_line -=  pTBM->rows - 1;
        }
        else
        {
            pTBM->top_line = 0;   
        }               
        break;
    case SB_NOTIFY_PAGE_DOWN:
        if(pTBM->top_line + pTBM->rows > pTBM->TotalRows - pTBM->rows)
        {
            pTBM->top_line = pTBM->TotalRows - pTBM->rows ;
        }
        else
        {
            pTBM->top_line +=  pTBM->rows - 1;    
        }
        break;
    default:
        break;
    }

    pTBM->cursor_y = pTBM->cursor_y - (pTBM->top_line - old_top_line);


    if(pTBM->bNeedScrollbar)
    {
        iCurWidth = pTBM->iColsHasScrollbar;
    }
    else
    {
        iCurWidth = pTBM->cols;
    }
    iStartPos = 0;
    iNextPos = 0;
    pBuffer = pTBM->pTxtInputBuf;
    pTBM->rowStartList[0] = 0;
    iNextIndex = 0;
    updateCount = pTBM->rows;
    if(0 == pTBM->top_line)
    {
        // updateCount = pTBM->rows -1;
        iNextIndex = 1;
    }
    while(TVM_GetNextPos(pBuffer, iStartPos, iCurWidth, &iNextPos))
    {
        iCount++;
        if((iCount >= (pTBM->top_line )) && 
            (iCount <= (pTBM->top_line + updateCount)))/* update area*/
        {                
            iNextIndex %= pTBM->rows+1;
            pTBM->rowStartList [iNextIndex] = iNextPos;
            iNextIndex++;
            
        }
        else if(iCount > (pTBM->top_line + updateCount)) 
        {
            break;
        }
        iStartPos = iNextPos;
        
    }

   pTBM->cnt = (iNextIndex);
        
    return OP_TRUE;
}


OP_BOOLEAN TBM_EnterStringEx(TBM_STRUCTURE_NAME_T  *pTBM, OP_UINT8 *pInsert, int insertLen, MsfTextType textType )
{
    OP_BOOLEAN srcHasChinese = OP_FALSE, insertHasChinese = OP_FALSE;
    OP_BOOLEAN bNeedInsert = OP_TRUE;
    OP_UINT8 *pSrc = OP_NULL, *pTemp = OP_NULL;
    int tempMax = 0;
    int cpyLen = 0;
    int iAfterCousor = 0;
    int i;
    pSrc = pTBM->pTxtInputBuf;
    if(OP_NULL == pSrc || 0 == insertLen || OP_NULL == pInsert)
    {
        return OP_FALSE;
    }
    if(insertLen/2 > (pTBM->maxCharCount - pTBM->iValidCharCount))
    {
        SP_Audio_play_request(RES_SOUND_0007, SP_GetVolume( BELL_LEVEL_ID ).LEVEL, OP_FALSE);
        return OP_FALSE;
    }
    if(pTBM->iValidCharCount - pTBM->iCursorPos > 0)
    {
        iAfterCousor = pTBM->iValidCharCount - pTBM->iCursorPos;
    }
    else
    {
        iAfterCousor = 0;    
    }
    
    i = 0;
    /*Whether Chinese exist in the inserted string */
    while(pInsert[i] != 0 || pInsert[i+1] !=0)
    {
        if(!(pInsert[i] <0x7f && pInsert[i+1] ==0))
        {
            insertHasChinese = OP_TRUE;
            break;
        }
        i += 2;
    }

    if( insertHasChinese == OP_FALSE )
    {
        i = 0;
        /*Whether Chinese exist in the inserted string */ 
        while(pSrc[i] != 0 || pSrc[i+1] !=0)
        {
            if(!(pSrc[i] <0x7f && pSrc[i+1] ==0))
            {
                srcHasChinese = OP_TRUE;
                break;
            }
            i +=2;
        }
    }

    tempMax = pTBM->maxCharCount;
    
    if(srcHasChinese ||insertHasChinese )/*chinese have exist in the buffer */
    {
        switch( textType )
        {
        case MsfPasswordText:
        {
            bNeedInsert = OP_FALSE;
        }
        case MsfPasswordNumber:
        {
            bNeedInsert = OP_FALSE;
        }
        case MsfSmsEms:
        {
            /* according to SMS/EMS coding, calc the length in chinese   */
            tempMax = (tempMax/MAX_CONCATENATED_SMS_LENGTH) * MAX_CONCATENATED_SMS_CH_LENGTH;
            break;
        }
        default:
        {
            tempMax /= 2;
            break;
        }
        }
    }

    if(bNeedInsert 
       &&(pTBM->iValidCharCount < tempMax)
       &&(pTBM->iValidCharCount + insertLen/2 <= tempMax))
    {
        pTemp = (OP_UINT8 *)WIDGET_ALLOC(iAfterCousor * 2 + insertLen + 2);
        if(OP_NULL == pTemp)
        {
          return OP_FALSE;
        }
        op_memset(pTemp,0, iAfterCousor * 2 + insertLen + 2);
        
        cpyLen = insertLen/2;
        op_memcpy(pTemp, pInsert, cpyLen*2);
        op_memcpy(pTemp+cpyLen*2, pTBM->pTxtInputBuf + pTBM->iCursorPos* 2, iAfterCousor * 2);
        op_memcpy(pSrc+pTBM->iCursorPos* 2, pTemp, (iAfterCousor + cpyLen) *2);
        pSrc[(pTBM->iValidCharCount +cpyLen) * 2] = '\0';
        pSrc[(pTBM->iValidCharCount +cpyLen) * 2 + 1] = '\0';
        
        WIDGET_FREE(pTemp);
    }
    else /* string can't be inserted */
    {
        SP_Audio_play_request(RES_SOUND_0007, SP_GetVolume( BELL_LEVEL_ID ).LEVEL, OP_FALSE);
        return OP_FALSE;
    }
    
    if(pTBM->iValidCharCount +cpyLen > pTBM->maxCharCount)
    {
        pTBM->iValidCharCount = pTBM->maxCharCount;
    }
    else
    {
        pTBM->iValidCharCount += cpyLen;
    }
    
    
    if(pTBM->rows == 1)
    {
#ifdef _KEYPAD
    for(i=0; i<cpyLen; i++)
    {
        OP_UINT8 iRet = 0;
        iRet = TBM_CursorForward(pTBM);
        if(iRet == 2)
        {
            i++;
        }
    } 
#else
    if(pTBM->iCursorPos +cpyLen > pTBM->maxCharCount)
    {
        pTBM->iCursorPos = pTBM->maxCharCount;
    }
    else
    {
        pTBM->iCursorPos += cpyLen;
    }
#endif
        pTBM->cursor_x = TBM_GetPlaceholderCnt(pTBM->pTxtInputBuf,
                                            pTBM->iLeftTopCharPos, 
                                            pTBM->iCursorPos, 
                                            pTBM->cols, 
                                            OP_FALSE);
    }
    else
    {
        BOOL bNeedBefore = pTBM->bNeedScrollbar;
        
        if(pTBM->iCursorPos +cpyLen > pTBM->maxCharCount)
        {
            pTBM->iCursorPos = pTBM->maxCharCount;
        }
        else
        {
            pTBM->iCursorPos += cpyLen;
        }    
        TBM_AdustCols(pTBM, OP_TRUE);
#ifdef _TOUCH_PANEL
        if (bNeedBefore != pTBM->bNeedScrollbar)
        {
            TVM_Build( pTBM);
        }
        else
        {
            if (insertLen > 2)
            {
                TVM_Build( pTBM);
            }
            else
            {
                TVM_InsertChar(pTBM, (OP_UINT16)(TBM_UNICODE_VALUE(pInsert)));
            }
        }
#else        
        TVM_Build( pTBM);
#endif
    }
    return OP_TRUE;
}

OP_BOOLEAN  TVM_CanScrollCursor(MsfTextInput *pTextInput)
{
    OP_INT16         iEngCharWidth, iEngCharHeight;

    DS_FONTATTR   fontAttr;
    MsfPosition        pos, clientStart;
    MsfSize             size, clientSize;
    MsfPosition        posCaretTop, posCaretBottom;
    
    
    if( !pTextInput || pTextInput->gadgetData.parent == OP_NULL )
    {
        return OP_FALSE;
    }

    if (pTextInput->input.txtInputBufMng.cursor_y >= pTextInput->input.txtInputBufMng.rows
        || pTextInput->input.txtInputBufMng.cursor_y < 0)
    {
        return OP_FALSE;
    }
    
    widgetGetDrawingInfo( 
                        (void*) pTextInput, 
                        getWidgetType( pTextInput->gadgetData.gadgetHandle), 
                        pTextInput->gadgetData.gadgetHandle, 
                        &fontAttr, 
                        OP_NULL, 
                        OP_NULL );

    if( 0 > widgetGetFontSizeInEngChar(fontAttr , &iEngCharWidth, &iEngCharHeight))
    {
        return OP_FALSE;
    }
    
    widgetTextInputGetEditArea( pTextInput, &pos, &size );

    if( pTextInput->input.txtInputBufMng.cursor_x  == 0 )
    {
        posCaretTop.x = posCaretBottom.x = pos.x;
    }
    else
    {
        posCaretTop.x = posCaretBottom.x = pos.x + pTextInput->input.txtInputBufMng.cursor_x * iEngCharWidth -1;
    }

    posCaretTop.y  = pos.y + pTextInput->input.txtInputBufMng.cursor_y * GADGET_LINE_HEIGHT;
    posCaretBottom.y = posCaretTop.y + GADGET_LINE_HEIGHT;

    widgetGetClientRect(pTextInput->gadgetData.parent, &clientStart, &clientSize);

    if( POINT_IN_RECT(posCaretTop, clientStart, clientSize) && POINT_IN_RECT(posCaretBottom, clientStart, clientSize))
    {
        return OP_TRUE;
    }

    return OP_FALSE;
}
#ifdef __cplusplus
}
#endif
