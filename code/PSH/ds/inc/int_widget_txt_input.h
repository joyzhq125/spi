#ifndef INT_WIDGET_TXT_INPUT_H
#define INT_WIDGET_TXT_INPUT_H

/*=======================================================================

    HEADER NAME : header-filename.h

    GENERAL DESCRIPTION
        General description of the contents of this header file.

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
=========================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -------------------------
    MM/DD/YYYY   name of author   crxxxxx     Brief description of changes made
    7/30/2003     Chenjs               P001026    Add the declaration of TBM_AdjustCursorForScroll
    8/11/2003     Chenjs               P001091    Update text input widget.
    02/02/2004   Chenxiao         p002404       add textinput scrollbar  in the dolphin project
    02/17/2004   Zhuxq             P002492      Add Viewer window to widget system
    03/08/2004   Andrew           P002598       Update WAP Editor for Dolphin project
    04/09/2004   Lhjie            p002799      Delete a function declaration
    04/19/2004   chenxiao          p003028        fix umb setting bugs and widget bugs      
    04/22/2004   Andrew           P002659   Fix some bugs of projects that bases Dolphin
    05/24/2004    zhuxq            P005568        Adjust UI implementation in UMB and fix some bugs
    06/09/2004    zhuxq            P006048        forbid Menu to respond successive multiple actions
    06/18/2004    chenxiao            P006257        change return value of function
    Self-documenting Code
    Describe/explain low-level design, especially things in header files, of this module and/or
    group of funtions and/or specific funtion that are hard to understand by reading code
    and thus requires detail description.
    Free format !

=======================================================================
    INCLUDE FILES
======================================================================*/


/*=========================================================
    CONSTANTS
======================================================*/

/*========================================================================
    MACROS
============================================================================*/

/*==================================================================================================
    ENUMERATIONS
==================================================================================================*/


/*==================================================================================================
    DATA STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
/* 
 * Define Application's management data structure here as APP_appname_STRUCTURE_NAME_T
 * "APP_DATA_STRUCTURE_NAME_T app" field should be defined first for OPUS framework.
 */

/*==================================================================================================
    GLOBAL VARIABLES DECLARATIONS
==================================================================================================*/

/*==================================================================================================
    FUNCTION PROTOTYPES
==================================================================================================*/

extern OP_INT16  TBM_CreateFromStrHandle
(
    OP_INT16 charCount,
    MsfStringHandle hStr,
    OP_UINT32 hTxtinput
 );
extern OP_INT16 TBM_SetContent
(
    TBM_STRUCTURE_NAME_T  *pTBM, 
    OP_UINT8 * newText,
    OP_INT16 size
);
extern OP_INT16 TBM_GetContent
(
    TBM_STRUCTURE_NAME_T *pTBM, 
    OP_BOOLEAN                     bStartFromZero,
    OP_UINT8 *dataBuf,
    OP_INT16 size
);
extern OP_INT16 TBM_Destroy(TBM_STRUCTURE_NAME_T  *pTBM);
extern OP_INT16   TBM_ClearContent(TBM_STRUCTURE_NAME_T  *pTBM);

extern OP_UINT16  TBM_GetContentLen
(
    TBM_STRUCTURE_NAME_T  *pTBM,
    OP_BOOLEAN bStartFromZero
);

extern OP_INT16  TBM_AdjustInitCursorPos( TBM_STRUCTURE_NAME_T *pTBM);

extern OP_INT16  TBM_AdjustCursorForScroll
( 
    OP_UINT32    hTextInput,
    MsfSize  *pInputSize,
    OP_UINT16 iLabLineCnt,
    OP_INT16 iChangeSize
);

extern OP_BOOLEAN TBM_EnterString
(
    TBM_STRUCTURE_NAME_T  *pTBM,
    OP_UINT8*     pStr
);

extern OP_BOOLEAN   TBM_CursorHead
(
    TBM_STRUCTURE_NAME_T  *pTBM
);

extern OP_BOOLEAN TBM_CursorTail
(
    TBM_STRUCTURE_NAME_T  *pTBM
);
extern OP_BOOLEAN   TBM_DelCursorChar
(
    TBM_STRUCTURE_NAME_T  *pTBM
);
extern OP_BOOLEAN TBM_CursorBackward
(
    TBM_STRUCTURE_NAME_T  *pTBM
);
extern OP_UINT8   TBM_CursorForward
(
    TBM_STRUCTURE_NAME_T  *pTBM
);
extern OP_BOOLEAN TBM_CursorUp
(
    TBM_STRUCTURE_NAME_T  *pTBM
);
extern OP_BOOLEAN TBM_CursorDown
(
    TBM_STRUCTURE_NAME_T  *pTBM
);
extern OP_BOOLEAN   TBM_DelString
(
    TBM_STRUCTURE_NAME_T  *pTBM
);

extern void TBM_AdustCols(TBM_STRUCTURE_NAME_T  *pTBM, OP_BOOLEAN bAdustType);


extern void TBM_ScrollLineUp(TBM_STRUCTURE_NAME_T  *pTBM);
extern void TBM_ScrollLineDown(TBM_STRUCTURE_NAME_T  *pTBM);
extern void TBM_ScrollPageUp(TBM_STRUCTURE_NAME_T  *pTBM);
extern void TBM_ScrollPageDown(TBM_STRUCTURE_NAME_T  *pTBM);
OP_BOOLEAN TBM_Scroll(TBM_STRUCTURE_NAME_T *pTBM, SCROLLBAR_NOTIFICATION_T scrollFlag);

extern OP_BOOLEAN TBM_SetCursor
(
    TBM_STRUCTURE_NAME_T *pTBM,
    const LCD_POINT_T pt_start,
    const LCD_POINT_T pt_clicking,
    const DS_FONTATTR font_attr
);

extern OP_BOOLEAN  TVM_CanScrollCursor(MsfTextInput *pTextInput);

extern OP_BOOLEAN TBM_EnterStringEx(TBM_STRUCTURE_NAME_T  *pTBM, OP_UINT8 *pInsert, int insertLen, MsfTextType textType );
/*================================================================================================*/
#endif  /* FILENAME_H */
