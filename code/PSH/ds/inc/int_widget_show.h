#ifndef INT_WIDGET_SHOW_H
#define INT_WIDGET_SHOW_H
/*==================================================================================================

    HEADER NAME : int_widget_common.h

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
    08/05/2003   Steven Lai       Cxxxxxx     Initial file creation. 
    8/18/2003     Zhuxq            P001263    Modify  the input method of textinput, allow to create a input method anytime  if a textinput require
    11/11/2003   Zhuxq             P001883     Optimize the redraw strategy to reduce the redraw times
    02/20/2004   Zhuxq             P002517     Adjust the handle order of pen events in widget system
    03/03/2004   MengJianghong     P002568     Selectgroup draw function adjust.
    03/19/2004   Chenxiao         p002688      change main menu  of wap and fix some bugs from NEC feedback     
    04/02/2004    zhuxq            P002789        Fix UMB bugs
    05/24/2004    zhuxq            P005568        Adjust UI implementation in UMB and fix some bugs
    Self-documenting Code
    Describe/explain low-level design, especially things in header files, of this module and/or
    group of funtions and/or specific funtion that are hard to understand by reading code
    and thus requires detail description.
    Free format !

====================================================================================================
    INCLUDE FILES
==================================================================================================*/

/*==================================================================================================
    CONSTANTS
==================================================================================================*/

/*==================================================================================================
    MACROS
==================================================================================================*/
#define   TEXTINPUT_IMM_INFO_NOT_IN_WINDOW   2

#define   CHOICE_REDRAW_ALL_ITEMS                    2

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

extern int widgetGetClientRect(const MsfWindow* pWin, MsfPosition  *pStart, MsfSize *pSize);

extern int widgetRedraw( void *pWidget, MsfWidgetType widgetType, OP_UINT32  handle, OP_BOOLEAN   bRefresh);
extern int widgetWindowAdjustClientArea(MsfWindow *pWin);
extern int  widgetFormAdjustScrollbar(MsfForm *pWidgetForm);

extern int  widgetPointClientToScreen
( 
    const MsfWindow   *pWin, 
    const OP_INT16  client_x,  
    const OP_INT16  client_y,  
    OP_INT16           *pScreen_x,
    OP_INT16           *pScreen_y
);
extern int widgetChoiceDrawItemByIndex
(
    MsfSelectgroup         *pSelectGroup,
    choiceElement          *pItem,
    OP_INT16              index,
    OP_BOOLEAN         bRefresh
);

extern OP_BOOLEAN widgetGetClientVisibleRect(const MsfWindow* pWin, MsfPosition  *pStart, MsfSize *pSize);
extern int widgetTextInputGetEditArea( const MsfTextInput *pInput, MsfPosition *pPos, MsfSize  *pSize );
extern int  widgetPointScreenToClient
( 
    const MsfWindow   *pWin, 
    const OP_INT16  screen_x,  
    const OP_INT16  screen_y,  
    OP_INT16           *pClient_x,
    OP_INT16           *pClient_y
);

extern void widgetBarAdjustValue( MsfBar *pBar );

/*================================================================================================*/
#endif  /* INT_WIDGET_SHOW_H */


