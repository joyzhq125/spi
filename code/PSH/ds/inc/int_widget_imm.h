#ifndef INT_WIDGET_IMM_H
#define INT_WIDGET_IMM_H
/*==================================================================================================

    HEADER NAME : int_widget_imm.h

    GENERAL DESCRIPTION
        General description of the contents of this header file.

    SEF Telecom Confidential Proprietary
    (c) Copyright 2003 by SEF Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    MM/DD/YYYY   name of author   crxxxxx     Brief description of changes made
    04/03/2003   Steven Lai       Cxxxxxx     Initial file creation. 
    07/02/2003   linda wang       P000763     fix draw bitmap file image problem 
    8/18/2003     Zhuxq             P001263    Modify  the input method of textinput, allow to create a input method anytime  if a textinput require
    10/17/2003   Zhuxq             C001603     Add touch panel feature to WAP & MMS
    10/23/2003   Zhuxq             P001842     Optimize the paintbox and input method solution and fix some bug
    11/11/2003   Zhuxq             P001883     Optimize the redraw strategy to reduce the redraw times
    12/04/2003   Andrew           P002113     Add switching IME by # and * (Long Press)
    03/08/2004   Andrew           P002598       Update WAP Editor for Dolphin project
    04/22/2004   Andrew           P002659   Fix some bugs of projects that bases Dolphin
    06/04/2004   Andrew           P005882     Fix some Dolphin bugs
    06/23/2004   Andrew           P006351     Fix some Lotus bugs
    09/01/2004   Andrew           P008042     Fix some Editor bugs.
    
    Self-documenting Code
    Describe/explain low-level design, especially things in header files, of this module and/or
    group of funtions and/or specific funtion that are hard to understand by reading code
    and thus requires detail description.
    Free format !

====================================================================================================
    INCLUDE FILES
==================================================================================================*/
#include "Input_Method_manager.h"
/*==================================================================================================
    CONSTANTS
==================================================================================================*/
/*==================================================================================================
    MACROS
==================================================================================================*/

#define   WIDGET_WINDOW_ALREADY_DRAWN    2

#define TEXTINPUT_IMM_CHAR_INSERTED  2


#define   WIDGET_IMM_MAX_LINE_LENGTH   20

/*==================================================================================================
    TYPE DEFINITION
==================================================================================================*/

/*
 * MsfWidgetImm  return code
 */
typedef enum
{
    WIDGET_RET_NONE = 0,
    WIDGET_RET_PROCESSED,
    WIDGET_RET_OUTOF_RANGE,
    WIDGET_RET_WRONG_PNUMBER,
    WIDGET_RET_TOO_FEW,
    WIDGET_RET_TOO_LONG,
    WIDGET_RET_WARNING,
    WIDGET_RET_PBOOK_SELECT,
    WIDGET_RET_IME_SELECT,
    WIDGET_RET_BACK,
    WIDGET_RET_FINISHED,
    WIDGET_RET_QUIT,
    WIDGET_RET_IME_SWITCH,
    WIDGET_RET_MAX
}MsfImmHandleResult;

/*
* The enum indicates which sequence will be used in changing the IME to next one. 
* Andrew for P002133
*/
typedef enum
{
    MSFIMM_CHANGE_IME_MAIN,       // Switch IME by left soft key and key pound.
    MSFIMM_CHANGE_IME_SUB,         // Use ime_inlist_star[] when long pressing * to switch IME
    MSFIMM_CHANGE_IME_LETTER,       // Switch LETTER between UPPER and lower for Handwriting.
    MSFIMM_CHANGE_IME_MAX
} MsfImm_CHANGE_IME_ENUM_T;
typedef  struct tagMsfWidgetImm{
    OP_UINT32                   hInput;
    OP_UINT32                   hWindow;
    HIMM                           hImm;
    OP_INT16                     iCurImeIndex;
    OP_INT16                     iCurImeList;
    INPUT_DISP_MODE_ENUM_T    input_disp_mode;

    OP_INT16                    start_x;
    OP_INT16                    start_y;
    INPUT_METHOD_ENUM_T         pre_ime;

    OP_BOOLEAN               bOpen;
    OP_BOOLEAN               bPrereadyInserted;
    OP_BOOLEAN               bOnHandWritting;
    OP_INT16                   iWinBottom;

    
    OP_UINT16                  pressKeyCode;
    /*
    * Add for implementing to change IME using # and *
    * Default is EEM_CHANGE_IME_OTHER (0)
    */
    MsfImm_CHANGE_IME_ENUM_T     eChangeFromStarPound;
    OP_BOOLEAN          bTimerStop;
    OP_BOOLEAN          bRefreshImeIcon;
}MsfWidgetImm;

extern MsfWidgetImm* widgetImmCreate
( 
    OP_UINT32                       hInput, 
    OP_INT16                         iImeList,
    OP_INT16                         iImeIndex,
    DS_FONTATTR                   font_attr,

    OP_UINT32                       hWin,
    OP_INT16                         iWinBottom
);

extern int widgetImmDestroy(MsfWidgetImm *pImm, OP_UINT32 hInput );

extern int widgetImmEventHandler
(
    MsfWidgetImm*                   pImm,
    OPUS_EVENT_ENUM_TYPE   event, 
    void                                      *pMess,
    OP_BOOLEAN                       *handle
);



extern int widgetImmShow( MsfWidgetImm* pImm , OP_BOOLEAN  bRefresh);

extern void widgetImmSetCursorState(OP_BOOLEAN b_show);

extern OP_BOOLEAN widgetImmGetCursorState(void);

extern void widgetImmChangeCursorState(void);

/*================================================================================================*/
#endif  /* INT_WIDGET_COMMON_H */
