#ifndef  COMM_UTIL_H
#define  COMM_UTIL_H

/*==================================================================================================

    HEADER NAME : comm_util.h

    GENERAL DESCRIPTION
        Common file header of UMB.

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2004 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    02/24/2004   Zhuxq      C002699     Initial Creation
    05/11/2004   zhuxq        P005352       Optimize UMB and fix some bugs in UMB, includes CNXT00004775

    Self-documenting Code
    Describe/explain low-level design, especially things in header files, of this module and/or
    group of funtions and/or specific funtion that are hard to understand by reading code
    and thus requires detail description.
    Free format !

====================================================================================================
    INCLUDE FILES
==================================================================================================*/
#include "window_manager.h"

/*==================================================================================================
    CONSTANTS
==================================================================================================*/
/* Error codes */


/*==================================================================================================
    MACROS
==================================================================================================*/
/*==================================================================================================
    ENUMERATIONS
==================================================================================================*/

/*==================================================================================================
    DATA STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
struct tagMenuNode;

/* prototype of  menu item selected response callback*/
typedef  int (*menu_item_select_func_t)(  struct tagMenuNode*, OP_INT16  );

typedef struct tagMenuItemPair {
	OP_UINT32                     iImageResId;        /*  the resoure id of the menu item's image */
	OP_UINT32                     iLabelResId;          /*  the resoure id of the menu item's label */
	menu_item_select_func_t  fnSelectCallback;   /* the callback of menu item when selected*/
} MenuItemPair;

typedef struct tagMenuNode {
      WindowNode      winNode;         /*  the managed window node */
	OP_UINT8         nItems;           /*  the number of menu items */
	MenuItemPair    *pItems;          /* the label & selected-callback pair list of menu items */
} MenuNode;

/*==================================================================================================
    GLOBAL VARIABLES DECLARATIONS
==================================================================================================*/

/*==================================================================================================
    FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
    FUNCTION: Comm_addActionToWindow

    DESCRIPTION:
    According to the modId, hLabel and fnCallback, create an action widget, and add it to 
    the window specified by hWin.

    ARGUMENTS PASSED:
    modId       the application id
    hWin         the window widget identifier
    hLabel       the label of the action
    fnCallback  the action's callback function
    
    RETURN VALUE:
    if successful, return the action widget handle; or return 0

    IMPORTANT NOTES:
    None

==================================================================================================*/
extern MsfActionHandle Comm_addActionToWindow
(
    OP_UINT8                      modId, 
    MsfWindowHandle            hWin, 
    MsfStringHandle              hLabel,
    int                               actionType,
    widget_action_callback_t fnCallback
);

/*==================================================================================================
    FUNCTION: Comm_addStringGadgetToWindow

    DESCRIPTION:
    According to the modId, pStr, and fnCallback, create a stringGadget and 
    add it to window specified by hWin

    ARGUMENTS PASSED:
    modId       the application id
    hWin         the window widget identifier
    pStr           the string content
    fnCallback  the stringGadget's state change callback function
    
    RETURN VALUE:
    if successful, return the StringGadget handle; or return 0

    IMPORTANT NOTES:
    None

==================================================================================================*/
extern MsfGadgetHandle Comm_addStringGadgetToWindow
(
    OP_UINT8                                modId, 
    MsfWindowHandle                     hWin,
    OP_UINT8                               *pStr,
    const MsfSize                          *pSize,
    const MsfPosition                     *pPos,
    int                                         iSingleLine,         
    widget_state_change_callback_t fnCallback
);

/*==================================================================================================
    FUNCTION: Comm_showMsgDlgForOnce

    DESCRIPTION:
    Create and show a dialog with the dlgType, hStr, iWinId, fnOkCallback, fnCancelCallback parameters. 
    The dialog is closed  by actions.
    
    Note: The dialog is managed by the window manager, so it cannot be appeared two times on screen.

    ARGUMENTS PASSED:
    modId 	the module or application identifier
    dlgType       the type of  dialog. Can not be MsfPrompt type 
    hStr             the prompt string
    iWinId      the window id
    fnOkCallback          the Ok Action's  callback function
    fnCancelCallback    the Cancel Action's callback function. Only used to MsfConfirmation dialog
    
    RETURN VALUE:
    If  successful,  return the dialog's WindowNode pointer; or return NULL

    IMPORTANT NOTES:
    None

==================================================================================================*/
extern WindowNode* Comm_showMsgDlgForOnce
(
    OP_UINT8                     modId,
    MsfDialogType               dlgType,
    MsfStringHandle             hStr,
    OP_UINT32                    iWinId,
    widget_action_callback_t fnOkCallback,
    widget_action_callback_t fnCancelCallback
);

/*==================================================================================================
    FUNCTION: Comm_showMsgDlgByTimerForOnce

    DESCRIPTION:
    Create and show a dialog with the dlgType, hStr, iWinId, iTime, fnCallback parameters. 
    The dialog is closed when timeout 
    
    Note: The dialog is managed by the window manager, so it cannot be appeared two 
    times on screen.

    ARGUMENTS PASSED:
    modId 	the module or application identifier
    dlgType       the type of  dialog. Can not be MsfPrompt and MsfConfirmation type
    hStr             the prompt string
    iWinId        the window id
    iTime          the dialog will show in iTime, and when timeout, the dialog will be closed automatically
    fnCallback          the dialog state change  callback function
    
    RETURN VALUE:
    If  successful,  return the dialog's WindowNode pointer; or  return NULL

    IMPORTANT NOTES:
    None

==================================================================================================*/
extern WindowNode* Comm_showMsgDlgByTimerForOnce
(
    OP_UINT8                              modId,
    MsfDialogType                        dlgType,
    MsfStringHandle                      hStr,
    OP_UINT32                            iWinId,
    OP_UINT32                            iTime,
    widget_state_change_callback_t fnCallback
);

/*==================================================================================================
    FUNCTION: Comm_showMsgDlg

    DESCRIPTION:
    Create and show a dialog with the dlgType, hStr, fnOkCallback, fnCancelCallback parameters.
    The dialog is closed  by actions. 
    
    Note: The dialog has no window id, and it can not be managed by the window manager, so it
    may be appeared two times on screen. To avoid the case is the application's responsibility.


    ARGUMENTS PASSED:
    modId 	the module or application identifier
    dlgType       the type of  dialog. Can not be MsfPrompt type 
    hStr             the prompt string
    fnOkCallback          the Ok Action's  callback function
    fnCancelCallback    the Cancel Action's callback function. Only used to MsfConfirmation dialog
    
    RETURN VALUE:
    If  successful,  return the dialog's handle; or return 0

    IMPORTANT NOTES:
    None

==================================================================================================*/
extern MsfWindowHandle Comm_showMsgDlg
(
    OP_UINT8                      modId,
    MsfDialogType                dlgType,
    MsfStringHandle              hStr,
    widget_action_callback_t fnOkCallback,
    widget_action_callback_t fnCancelCallback
);

/*==================================================================================================
    FUNCTION: Comm_showMsgDlgByTimer

    DESCRIPTION:
    Create and show a dialog with the dlgType, hStr, iTime, fnCallback parameters. The dialog is
    closed when timeout.
    
    Note: The dialog has no window id, and it can not be managed by the window manager, so it
    may be appeared two times on screen. To avoid the case is the application's responsibility.


    ARGUMENTS PASSED:
    modId 	the module or application identifier
    dlgType       the type of  dialog. Can not be MsfPrompt and MsfConfirmation type
    hStr             the prompt string
    iTime          the dialog will show in iTime, and when timeout, the dialog will be closed automatically
    fnCallback          the dialog state change  callback function
    
    RETURN VALUE:
    If  successful,  return the dialog's WindowNode pointer; or  return NULL

    IMPORTANT NOTES:
    None

==================================================================================================*/
extern MsfWindowHandle Comm_showMsgDlgByTimer
(
    OP_UINT8                              modId,
    MsfDialogType                        dlgType,
    MsfStringHandle                      hStr,
    OP_UINT32                             iTime,
    widget_state_change_callback_t fnCallback
);

/*==================================================================================================
    FUNCTION: Comm_createMenu

    DESCRIPTION:
    Create a menu according to the pItems.


    ARGUMENTS PASSED:
    modId 	   the module or application identifier
    iWinId          the menu window id
    bMainMenu    indicates the menu is main menu
    hTitle           the title of the menu window
    nItems          the number of menu items
    pItems          the label & selected-callback pair list of menu items
    
    RETURN VALUE:
    If successful, return the pointer of the new menu's MenuNode ; or return NULL

    IMPORTANT NOTES:
    None

==================================================================================================*/
extern MenuNode* Comm_createMenu
(
    OP_UINT8          modId,
    OP_UINT32        iWinId,
    OP_BOOLEAN     bMainMenu,
    OP_UINT32        hTitle,
    int                   bitmask,
    OP_UINT8         nItems,
    const MenuItemPair        *pItems,
    widget_action_callback_t fnCancelCallback,
    window_destroy_func_t   fnWinDestroy
);

/*==================================================================================================
    FUNCTION: Comm_callEditor

    DESCRIPTION:
    Create and show a Editor according to the inputed parameters.

    ARGUMENTS PASSED:
    modId 	the module or application identifier
    iWinId	the editor window id
    hTitle	the title of the menu window
    hInitStr	the handle of init string
    textType   the text type
    singleLine  specify if only single line input
    pItems	the label & selected-callback pair list of menu items
    fnOkCallback	the Ok Action's callback function
    
    RETURN VALUE:
    If successful, return the pointer of the new editor's WindowNode; or return NULL

    IMPORTANT NOTES:
    None

==================================================================================================*/
extern WindowNode* Comm_callEditor
(
    OP_UINT8                      modId,
    OP_UINT32                    iWinId,
    OP_UINT32                    hTitle,
    OP_UINT32                    hInitStr,
    MsfTextType                 textType,
    int                               singleLine,
    int                               nMaxChars,
    widget_action_callback_t fnOkCallback,
    widget_action_callback_t fnCancelCallback
);

/*==================================================================================================
    FUNCTION: Comm_callViewer

    DESCRIPTION:
    Create and show a viewer according to the inputed parameters.

    ARGUMENTS PASSED:
    modId 	the module or application identifier
    iWinId	the editor window id
    hTitle	the title of the menu window
    hMsg	the handle of init string
    fnOkCallback          the Ok Action's  callback function
    fnCancelCallback    the Cancel Action's callback function.
    
    RETURN VALUE:
    If successful, return the pointer of the new editor's WindowNode; or return NULL

    IMPORTANT NOTES:
    None

==================================================================================================*/
extern WindowNode* Comm_callViewer
(
    OP_UINT8                      modId,
    OP_UINT32                    iWinId,
    OP_UINT32                    hTitle,
    OP_UINT32                    hMsg,
    widget_action_callback_t fnOkCallback,
    widget_action_callback_t fnCancelCallback
);

/*==================================================================================================
    FUNCTION: Comm_defaultWindowDestroy

    DESCRIPTION:
    This function is a default implementation of a window-destroy function. It will close 
    the window, and destroy the managed window node
    
    ARGUMENTS PASSED:
    pWinNode      the managed window node    
    
    RETURN VALUE:
    If successful, return 0; or return error code

    IMPORTANT NOTES:
    None

==================================================================================================*/
extern int  Comm_defaultWindowDestroy (WindowNode* pWinNode );

/*==================================================================================================
    FUNCTION: Comm_defaultWindowCancelCallback

    DESCRIPTION:
    This function is a default implementation of a Cancel action callback function. It will close 
    the window, and destroy the managed window node
    
    ARGUMENTS PASSED:
    modId    the module/application identifier
    hWin      the window widget identifier
    hAction  the action widget identifier
    
    RETURN VALUE:
    If successful, return 0; or return error code

    IMPORTANT NOTES:
    None

==================================================================================================*/
extern int Comm_defaultWindowCancelCallback(MSF_UINT8 modId, MSF_UINT32 hWin, MSF_UINT32 hAction );

/*==================================================================================================
    FUNCTION: Comm_showDlgDefaultCloseCb

    DESCRIPTION:
    This function is a default implementation of a cancel action call back function. It will close
    the dialog.

    ARGUMENTS PASSED:
    modId    the module/application identifier
    hWin      the window widget identifier
    hAction  the action widget identifier
    
    RETURN VALUE:
    If successful, return 0; or return error code

    IMPORTANT NOTES:
    
==================================================================================================*/
extern int Comm_showDlgDefaultCloseCb(MSF_UINT8 modId, MSF_UINT32 hDlg, MSF_UINT32 hAction);

/*================================================================================================*/
#endif



