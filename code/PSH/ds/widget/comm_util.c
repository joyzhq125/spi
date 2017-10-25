#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
    MODULE NAME : window_manager.c

    GENERAL DESCRIPTION
        Common file of UMB
        
    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2004 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    02/23/2004   Zhuxq      C002699     Initial file creation.
    04/02/2004    zhuxq            P002789        Fix UMB bugs
    05/11/2004   zhuxq        P005352       Optimize UMB and fix some bugs in UMB, includes CNXT00004775
    05/28/2004    zhuxq            P005745        Adjust layout in UMB for B1
    06/01/2004    zhuxq            P005819        Optimize UMB codes
    06/04/2004    zhuxq            P005925        Correct issues found by PC-lint and fix some bugs
    Self-documenting Code
    Describe/explain low-level design of this module and/or group of funtions and/or specific
    funtion that are hard to understand by reading code and thus requires detail description.
    Free format !
    
====================================================================================================
    INCLUDE FILES
==================================================================================================*/
#include  "app_include.h"
#include "comm_util.h"
#include "Utility_string_func.h"
#include "Msf_rc.h"

/*==================================================================================================
    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static int MenuOkActionCallback (MSF_UINT8 modId, MSF_UINT32 hWin, MSF_UINT32 hAction );
static int Comm_showDlgByTimerDefaultStateChangeCb(MSF_UINT8 modId, MSF_UINT32 hDlg, MsfNotificationType notiType, void *pData );

/*==================================================================================================
    LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
    LOCAL TYPEDEFS - Structure, Union, Enumerations...etc
==================================================================================================*/

/*==================================================================================================
    LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
    LOCAL VARIABLES
==================================================================================================*/
/*==================================================================================================
    GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
    LOCAL FUNCTIONS
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
    actionType the type of action
    fnCallback  the action's callback function
    
    RETURN VALUE:
    if successful, return the action widget handle; or return 0

    IMPORTANT NOTES:
    None

==================================================================================================*/
MsfActionHandle Comm_addActionToWindow
(
    OP_UINT8                      modId, 
    MsfWindowHandle            hWin, 
    MsfStringHandle              hLabel,
    int                               actionType,
    widget_action_callback_t fnCallback
)
{
    MsfActionHandle hAction;

    hAction = TPIa_widgetActionCreateEx( modId, hLabel, actionType, 1, 0x8000, fnCallback);
    if( !ISVALIDHANDLE(hAction))
    {
        return INVALID_HANDLE;
    }

    if( 0 > TPIa_widgetAddAction(hWin, hAction) )
    {
        TPIa_widgetRelease(hAction);
        return INVALID_HANDLE;
    }

    return hAction;
}

/*==================================================================================================
    FUNCTION: Comm_addStringGadgetToWindow

    DESCRIPTION:
    According to the modId, pStr, and fnCallback, create a stringGadget and 
    add it to window specified by hWin

    ARGUMENTS PASSED:
    modId       the application id
    hWin         the window widget identifier
    pStr           the string content, in Unicode format
    size          Specifies the MsfSize of the MsfStringGadget. 0 means
                   that the MsfStringGadget will be created with the default
                   MsfSize, i.e. with the MsfSize of the MsfString.
    singleLine    Specifies if the entered text is to be shown on one single
                    line (1) or on multiple lines (0), if the content size is
                    bigger than the MsfSize of the MsfTextInput. I.e. if the
                    content should be horisontally scrollable or vertically
                    scrollable.
    pos           the topleft position of the stringgadget                
    fnCallback  the stringGadget's state change callback function
    
    RETURN VALUE:
    if successful, return the StringGadget handle; or return 0

    IMPORTANT NOTES:
    None

==================================================================================================*/
MsfGadgetHandle Comm_addStringGadgetToWindow
(
    OP_UINT8                                modId, 
    MsfWindowHandle                     hWin,
    OP_UINT8                               *pStr,
    const MsfSize                          *pSize,
    const MsfPosition                     *pPos,
    int                                         iSingleLine,         
    widget_state_change_callback_t fnCallback
)
{
    MsfStringHandle   hStr;
    MsfGadgetHandle hGadget;
    int                    propertyMask;

    if( !pStr || !pSize)
    {
        return INVALID_HANDLE;
    }
    
    hStr = TPIa_widgetStringCreate( modId,
                                                     (const char*)pStr,
                                                     MsfUnicode,
                                                     Ustrlen(pStr),
                                                     0);
    if( !ISVALIDHANDLE(hStr))
    {
        return INVALID_HANDLE;
    }

    if( fnCallback != OP_NULL )
    {
        propertyMask = MSF_GADGET_PROPERTY_FOCUS | MSF_GADGET_PROPERTY_NOTIFY;
    }
    else
    {
        propertyMask = 0;
    }
    
    hGadget = TPIa_widgetStringGadgetCreateEx(
                                                    modId, 
                                                    hStr,
                                                    (MsfSize*)pSize,
                                                    iSingleLine,
                                                    propertyMask,
                                                    0,
                                                    fnCallback);
    if(!ISVALIDHANDLE(hGadget))
    {
        TPIa_widgetRelease(hStr);
        return INVALID_HANDLE;
    }
    
    if( 0 > TPIa_widgetWindowAddGadget(hWin, hGadget, (MsfPosition*)pPos, OP_NULL) )
    {
        TPIa_widgetRelease(hStr);
        TPIa_widgetRelease( hGadget );
        
        return INVALID_HANDLE;
    }
    
    TPIa_widgetRelease(hStr);

    return hGadget;
}

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
WindowNode* Comm_showMsgDlgForOnce
(
    OP_UINT8                     modId,
    MsfDialogType               dlgType,
    MsfStringHandle             hStr,
    OP_UINT32                    iWinId,
    widget_action_callback_t fnOkCallback,
    widget_action_callback_t fnCancelCallback
)
{
    MsfActionHandle  hOkAction;
    MsfActionHandle  hCancelAction;
    WindowManager *pWinMgr;
    WindowNode      *pWinNode;
    MsfWindowHandle hWin;

    if( dlgType < MsfAlert ||  dlgType >= MsfPrompt )
    {
        return OP_NULL;
    }
    
    pWinMgr = WinMgr_getWinMgr( modId );
    if( !pWinMgr )
    {
        return OP_NULL;
    }
    
    pWinNode = WinMgr_getWindow( pWinMgr, iWinId);
    if( pWinNode )
    {
        return OP_NULL;
    }

    hOkAction = INVALID_HANDLE;
    hCancelAction = INVALID_HANDLE;
    hWin = INVALID_HANDLE;
    
    pWinNode = WIDGET_ALLOC( sizeof(WindowNode));
    
    if(!pWinNode )
    {
        return OP_NULL;
    }

    op_memset(pWinNode, 0, sizeof(WindowNode));
    hWin = TPIa_widgetDialogCreate( modId, hStr, dlgType, 0, 0x8000, 0);
    if( !ISVALIDHANDLE(hWin) )
    {
        goto COMM_SHOW_DLG_FOR_ONCE_ERROR;
    }

    /*  add ok action  */
    if( fnOkCallback != OP_NULL )
    {
        hOkAction = Comm_addActionToWindow(
                                                modId,
                                                hWin,
                                                TPIa_widgetStringGetPredefined(MSF_STR_ID_DLG_ACTION_OK),
                                                MsfOk,
                                                fnOkCallback);
        if( !ISVALIDHANDLE(hOkAction))
        {
            goto COMM_SHOW_DLG_FOR_ONCE_ERROR;
        }
    }
    
    /*  add Cancel action  */
    if( dlgType == MsfConfirmation && fnCancelCallback != OP_NULL )
    {
        hCancelAction = Comm_addActionToWindow(
                                                modId,
                                                hWin,
                                                TPIa_widgetStringGetPredefined(MSF_STR_ID_DLG_ACTION_BACK),
                                                MsfBack,
                                                fnCancelCallback);
        if( !ISVALIDHANDLE(hCancelAction))
        {
            goto COMM_SHOW_DLG_FOR_ONCE_ERROR;
        }
    }

    /* fill the window node structure */
    pWinNode->iWinId = iWinId;
    pWinNode->hWin = hWin;
    pWinNode->pos.x = 0;
    pWinNode->pos.y = 0;
    pWinNode->fnWinDestory = Comm_defaultWindowDestroy;

    /* register the window node  */
    if( 0 > WinMgr_registerWindow( pWinMgr, pWinNode) )
    {
        goto COMM_SHOW_DLG_FOR_ONCE_ERROR;
    }

    /* show the dialog */
    WinMgr_showWindow(pWinNode, MSF_TRUE);
    
    TPIa_widgetRelease(hOkAction);
    TPIa_widgetRelease(hCancelAction);

    return pWinNode;

COMM_SHOW_DLG_FOR_ONCE_ERROR:

    if( pWinNode )
    {
        WIDGET_FREE( pWinNode );
    }
    if( ISVALIDHANDLE(hOkAction))
    {
        TPIa_widgetRelease(hOkAction);
    }
    if( ISVALIDHANDLE(hCancelAction))
    {
        TPIa_widgetRelease(hCancelAction);
    }
    if( ISVALIDHANDLE(hWin))
    {
        TPIa_widgetRelease(hWin);
    }
    
    return OP_NULL;
    
}

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
WindowNode* Comm_showMsgDlgByTimerForOnce
(
    OP_UINT8                              modId,
    MsfDialogType                        dlgType,
    MsfStringHandle                      hStr,
    OP_UINT32                            iWinId,
    OP_UINT32                            iTime,
    widget_state_change_callback_t fnCallback
)
{
    WindowManager *pWinMgr;
    WindowNode      *pWinNode;
    MsfWindowHandle hWin;

    if( dlgType < MsfAlert ||  dlgType >= MsfPrompt || dlgType == MsfConfirmation )
    {
        return OP_NULL;
    }
    
    pWinMgr = WinMgr_getWinMgr( modId );
    if( !pWinMgr )
    {
        return OP_NULL;
    }
    
    pWinNode = WinMgr_getWindow( pWinMgr, iWinId);
    if( pWinNode )
    {
        return OP_NULL;
    }

    hWin = INVALID_HANDLE;
    
    pWinNode = WIDGET_ALLOC( sizeof(WindowNode));
    
    if(!pWinNode )
    {
        return OP_NULL;
    }

    op_memset(pWinNode, 0, sizeof(WindowNode));
    hWin = TPIa_widgetDialogCreateEx(
                                            modId,
                                            hStr,
                                            dlgType,
                                            iTime,
                                            0x8000,
                                            0,
                                            fnCallback);
                                            
    if( !ISVALIDHANDLE(hWin) )
    {
        goto COMM_SHOW_DLG_FOR_ONCE_BY_TIMER_ERROR;
    }

    /* fill the window node structure */
    pWinNode->iWinId = iWinId;
    pWinNode->hWin = hWin;
    pWinNode->pos.x = 0;
    pWinNode->pos.y = 0;
    pWinNode->fnWinDestory = Comm_defaultWindowDestroy;

    /* register the window node  */
    if( 0 > WinMgr_registerWindow( pWinMgr, pWinNode) )
    {
        goto COMM_SHOW_DLG_FOR_ONCE_BY_TIMER_ERROR;
    }

    /* show the dialog */
    WinMgr_showWindow(pWinNode, MSF_TRUE);
    
    return pWinNode;

COMM_SHOW_DLG_FOR_ONCE_BY_TIMER_ERROR:

    if( pWinNode )
    {
        WIDGET_FREE( pWinNode );
    }
    
    if( ISVALIDHANDLE(hWin))
    {
        TPIa_widgetRelease(hWin);
    }
    
    return OP_NULL;
    
}

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
MsfWindowHandle Comm_showMsgDlg
(
    OP_UINT8                      modId,
    MsfDialogType                dlgType,
    MsfStringHandle              hStr,
    widget_action_callback_t fnOkCallback,
    widget_action_callback_t fnCancelCallback
)
{
    MsfActionHandle  hOkAction;
    MsfActionHandle  hCancelAction;
    WindowManager *pWinMgr;
    MsfWindowHandle hWin;
    MsfPosition          pos = {0,0};
    
    if( dlgType < MsfAlert ||  dlgType >= MsfPrompt )
    {
        return INVALID_HANDLE;
    }
    
    pWinMgr = WinMgr_getWinMgr( modId );
    if( !pWinMgr )
    {
        return INVALID_HANDLE;
    }

    hOkAction = INVALID_HANDLE;
    hCancelAction = INVALID_HANDLE;
    hWin = INVALID_HANDLE;

    /* create dialog */
    hWin = TPIa_widgetDialogCreate( modId, hStr, dlgType, 0, 0x8000, 0);
    if( !ISVALIDHANDLE(hWin) )
    {
        goto COMM_SHOW_MSG_DLG_ERROR;
    }

    /*  add ok action  */
    if( fnOkCallback != OP_NULL )
    {
        hOkAction = Comm_addActionToWindow(
                                                modId,
                                                hWin,
                                                TPIa_widgetStringGetPredefined(MSF_STR_ID_DLG_ACTION_OK),
                                                MsfOk,
                                                fnOkCallback);
        if( !ISVALIDHANDLE(hOkAction))
        {
            goto COMM_SHOW_MSG_DLG_ERROR;
        }
    }
    
    /*  add Cancel action  */
    if( dlgType == MsfConfirmation && fnCancelCallback != OP_NULL )
    {
        hCancelAction = Comm_addActionToWindow(
                                                modId,
                                                hWin,
                                                TPIa_widgetStringGetPredefined(MSF_STR_ID_DLG_ACTION_CANCEL),
                                                MsfCancel,
                                                fnCancelCallback);
        if( !ISVALIDHANDLE(hCancelAction))
        {
            goto COMM_SHOW_MSG_DLG_ERROR;
        }
    }

    /* add the dialog to the screen  */
    if( 0 > TPIa_widgetScreenAddWindow(pWinMgr->hScreen, hWin, &pos, 0, 0))
    {
        goto COMM_SHOW_MSG_DLG_ERROR;
    }
    
    /* show the dialog */
    TPIa_widgetSetInFocus(hWin, MSF_TRUE);
    
    TPIa_widgetRelease(hOkAction);
    TPIa_widgetRelease(hCancelAction);

    return hWin;

COMM_SHOW_MSG_DLG_ERROR:

    if( ISVALIDHANDLE(hOkAction))
    {
        TPIa_widgetRelease(hOkAction);
    }
    if( ISVALIDHANDLE(hCancelAction))
    {
        TPIa_widgetRelease(hCancelAction);
    }
    if( ISVALIDHANDLE(hWin))
    {
        TPIa_widgetRelease(hWin);
    }
    
    return INVALID_HANDLE;
}

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
    If  successful,  return the dialog's handle; or return 0

    IMPORTANT NOTES:
    None

==================================================================================================*/
MsfWindowHandle Comm_showMsgDlgByTimer
(
    OP_UINT8                              modId,
    MsfDialogType                        dlgType,
    MsfStringHandle                      hStr,
    OP_UINT32                             iTime,
    widget_state_change_callback_t fnCallback
)
{
    WindowManager *pWinMgr;
    MsfWindowHandle hWin;
    MsfPosition          pos = {0,0};

    if( dlgType < MsfAlert ||  dlgType >= MsfPrompt || dlgType == MsfConfirmation )
    {
        return INVALID_HANDLE;
    }
    
    pWinMgr = WinMgr_getWinMgr( modId );
    if( !pWinMgr )
    {
        return INVALID_HANDLE;
    }

    if( fnCallback == OP_NULL )
    {
        fnCallback = Comm_showDlgByTimerDefaultStateChangeCb;
    }
    
    hWin = TPIa_widgetDialogCreateEx(
                                            modId,
                                            hStr,
                                            dlgType,
                                            iTime,
                                            0x8000,
                                            0,
                                            fnCallback);
                                            
    if( !ISVALIDHANDLE(hWin) )
    {
        return INVALID_HANDLE;
    }

    /* add the dialog to the screen  */
    if( 0 > TPIa_widgetScreenAddWindow(pWinMgr->hScreen, hWin, &pos, 0, 0))
    {
        TPIa_widgetRelease(hWin);
        return INVALID_HANDLE;
    }
    
    /* show the dialog */
    TPIa_widgetSetInFocus(hWin, MSF_TRUE);

    return hWin;
}

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
MenuNode* Comm_createMenu
(
    OP_UINT8            modId,
    OP_UINT32          iWinId,
    OP_BOOLEAN       bMainMenu,
    OP_UINT32          hTitle,
    int                     bitmask,
    OP_UINT8          nItems,
    const MenuItemPair        *pItems,
    widget_action_callback_t fnCancelCallback,
    window_destroy_func_t   fnWinDestroy
)
{
    WindowManager *pWinMgr;
    MenuNode         *pMenuNode;
    MsfWindowHandle hWin;
    MsfActionHandle   hOkAction;
    MsfActionHandle   hCancelAction;
    MsfImageHandle   hIconImage;
    OP_BOOLEAN       bAddImage;
    MsfSize               size;
    int                      i;

    if( !pItems || nItems == 0 )
    {
        return OP_NULL;
    }
    
    pWinMgr = WinMgr_getWinMgr( modId );
    if( !pWinMgr )
    {
        return OP_NULL;
    }
    
    pMenuNode =(MenuNode*)WinMgr_getWindow( pWinMgr, iWinId);
    if( pMenuNode )
    {
        return OP_NULL;
    }

    hWin = INVALID_HANDLE;
    hOkAction = INVALID_HANDLE;
    hCancelAction = INVALID_HANDLE;
    
    pMenuNode = WIDGET_ALLOC( sizeof(MenuNode));
    
    if(!pMenuNode )
    {
        return OP_NULL;
    }

    op_memset(pMenuNode, 0 , sizeof(MenuNode));
    size.width = pWinMgr->iScreenWidth;
    size.height = pWinMgr->iScreenHeight;
    
    if( bMainMenu )
    {
        hWin = TPIa_widgetMainMenuCreateEx(
                                            modId,
                                            MsfImplicitChoice,
                                            &size,
                                            INVALID_HANDLE,
                                            OP_NULL,
                                            bitmask,
                                            0x8000,
                                            0,
                                            OP_NULL);
    }
    else
    {
        hWin = TPIa_widgetMenuCreateEx(
                                            modId,
                                            MsfImplicitChoice,
                                            &size,
                                            INVALID_HANDLE,
                                            OP_NULL,
                                            bitmask,
                                            0x8000,
                                            0,
                                            OP_NULL);
    }
    
    if( !ISVALIDHANDLE(hWin) )
    {
        goto COMM_CREATE_MENU_ERROR;
    }

    if(ISVALIDHANDLE(hTitle))
    {
        if( 0 > TPIa_widgetSetTitle(hWin, hTitle) )
        {
            goto COMM_CREATE_MENU_ERROR;
        }
    }
    
    /*  add ok action  */
    hOkAction = Comm_addActionToWindow(
                                            modId,
                                            hWin,
                                            TPIa_widgetStringGetPredefined(MSF_STR_ID_DLG_ACTION_OK),
                                            MsfOk,
                                            MenuOkActionCallback);
                                            
    if( !ISVALIDHANDLE(hOkAction))
    {
        goto COMM_CREATE_MENU_ERROR;
    }
    
    /*  add Cancel action  */
    hCancelAction = Comm_addActionToWindow(
                                            modId,
                                            hWin,
                                            TPIa_widgetStringGetPredefined(MSF_STR_ID_DLG_ACTION_BACK),
                                            MsfBack,
                                            fnCancelCallback);
                                            
    if( !ISVALIDHANDLE(hCancelAction))
    {
        goto COMM_CREATE_MENU_ERROR;
    }

    if( bitmask & (MSF_CHOICE_ELEMENT_IMAGE_1 | MSF_CHOICE_ELEMENT_IMAGE_2) )
    {
        bAddImage = OP_TRUE;
    }
    else
    {
        bAddImage = OP_FALSE;
    }

    hIconImage = INVALID_HANDLE;
    /*  add elements to menu  */
    for( i = 0; i < nItems; i++ )
    {
        if( bAddImage )
        {
            hIconImage = TPIa_widgetImageGetPredefined( pItems[i].iImageResId, OP_NULL );
        }
        
        if(0 > TPIa_widgetChoiceSetElement(
                                        hWin,
                                        i,
                                        TPIa_widgetStringGetPredefined(pItems[i].iLabelResId),
                                        INVALID_HANDLE,
                                        hIconImage,
                                        INVALID_HANDLE,
                                        MSF_TRUE))
        {
            goto COMM_CREATE_MENU_ERROR;
        }
    }

    /* fill the menu node structure */
    pMenuNode->winNode.iWinId = iWinId;
    pMenuNode->winNode.hWin = hWin;
    pMenuNode->winNode.pos.x = 0; /* window position */
    pMenuNode->winNode.pos.y = 0;
    pMenuNode->winNode.fnWinDestory = fnWinDestroy;
    
    pMenuNode->nItems = nItems;
    pMenuNode->pItems = (MenuItemPair*)pItems;

    /* register the window node  */
    if( 0 > WinMgr_registerWindow( pWinMgr, (WindowNode*)pMenuNode) )
    {
        goto COMM_CREATE_MENU_ERROR;
    }

    TPIa_widgetRelease(hOkAction);
    TPIa_widgetRelease(hCancelAction);

    return pMenuNode;

COMM_CREATE_MENU_ERROR:

    if( pMenuNode )
    {
        WIDGET_FREE( pMenuNode );
    }
    if( ISVALIDHANDLE(hOkAction))
    {
        TPIa_widgetRelease(hOkAction);
    }
    if( ISVALIDHANDLE(hCancelAction))
    {
        TPIa_widgetRelease(hCancelAction);
    }
    if( ISVALIDHANDLE(hWin))
    {
        TPIa_widgetRelease(hWin);
    }
    
    return OP_NULL;
}

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
WindowNode* Comm_callEditor
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
)
{
    MsfActionHandle  hOkAction;
    MsfActionHandle  hCancelAction;
    WindowManager *pWinMgr;
    WindowNode      *pWinNode;
    MsfSize              size;
    MsfWindowHandle hWin;

    pWinMgr = WinMgr_getWinMgr( modId );
    if( !pWinMgr )
    {
        return OP_NULL;
    }
    
    pWinNode = WinMgr_getWindow( pWinMgr, iWinId);
    if( pWinNode )
    {
        return OP_NULL;
    }

    hOkAction = INVALID_HANDLE;
    hCancelAction = INVALID_HANDLE;
    hWin = INVALID_HANDLE;
    
    pWinNode = WIDGET_ALLOC( sizeof(WindowNode));
    
    if(!pWinNode )
    {
        return OP_NULL;
    }

    op_memset(pWinNode, 0 , sizeof(WindowNode));
    /* get window size and create a editor  */
    size.width = pWinMgr->iScreenWidth;
    size.height = pWinMgr->iScreenHeight;

    hWin = TPIa_widgetEditorCreate(
                                        modId,
                                        INVALID_HANDLE,
                                        hInitStr,
                                        textType,
                                        nMaxChars,
                                        singleLine,
                                        &size,
                                        MSF_WINDOW_PROPERTY_SINGLEACTION |MSF_WINDOW_PROPERTY_TITLE,
                                        0);
                                        
    if( !ISVALIDHANDLE(hWin) )
    {
        goto COMM_CALL_EDITOR_ERROR;
    }

    /* set title of the editor */
    if( ISVALIDHANDLE(hTitle))
    {
        if( 0 > TPIa_widgetSetTitle(hWin, hTitle) )
        {
            goto COMM_CALL_EDITOR_ERROR;
        }
    }
    
    /*  add ok action  */
    if( fnOkCallback != OP_NULL )
    {
        hOkAction = Comm_addActionToWindow(
                                                modId,
                                                hWin,
                                                TPIa_widgetStringGetPredefined(MSF_STR_ID_DLG_ACTION_OK),
                                                MsfOk,
                                                fnOkCallback);
        if( !ISVALIDHANDLE(hOkAction))
        {
            goto COMM_CALL_EDITOR_ERROR;
        }
    }
    
    /*  add Cancel action  */
    if( fnCancelCallback != OP_NULL )
    {
        hCancelAction = Comm_addActionToWindow(
                                                modId,
                                                hWin,
                                                TPIa_widgetStringGetPredefined(MSF_STR_ID_DLG_ACTION_BACK),
                                                MsfBack,
                                                fnCancelCallback);
        if( !ISVALIDHANDLE(hCancelAction))
        {
            goto COMM_CALL_EDITOR_ERROR;
        }
    }

    /* fill the window node structure */
    pWinNode->iWinId = iWinId;
    pWinNode->hWin = hWin;
    pWinNode->pos.x = 0;
    pWinNode->pos.y = 0;
    pWinNode->fnWinDestory = Comm_defaultWindowDestroy;

    /* register the window node  */
    if( 0 > WinMgr_registerWindow( pWinMgr, pWinNode) )
    {
        goto COMM_CALL_EDITOR_ERROR;
    }

    /* show the editor */
    WinMgr_showWindow(pWinNode, MSF_TRUE);
    
    TPIa_widgetRelease(hOkAction);
    TPIa_widgetRelease(hCancelAction);

    return pWinNode;

COMM_CALL_EDITOR_ERROR:

    if( pWinNode )
    {
        WIDGET_FREE( pWinNode );
    }
    if( ISVALIDHANDLE(hOkAction))
    {
        TPIa_widgetRelease(hOkAction);
    }
    if( ISVALIDHANDLE(hCancelAction))
    {
        TPIa_widgetRelease(hCancelAction);
    }
    if( ISVALIDHANDLE(hWin))
    {
        TPIa_widgetRelease(hWin);
    }
    
    return OP_NULL;
}


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
WindowNode* Comm_callViewer
(
    OP_UINT8                      modId,
    OP_UINT32                    iWinId,
    OP_UINT32                    hTitle,
    OP_UINT32                    hMsg,
    widget_action_callback_t fnOkCallback,
    widget_action_callback_t fnCancelCallback
)
{
    MsfActionHandle  hOkAction;
    MsfActionHandle  hCancelAction;
    WindowManager *pWinMgr;
    WindowNode      *pWinNode;
    MsfSize              size;
    MsfWindowHandle hWin;

    pWinMgr = WinMgr_getWinMgr( modId );
    if( !pWinMgr )
    {
        return OP_NULL;
    }
    
    pWinNode = WinMgr_getWindow( pWinMgr, iWinId);
    if( pWinNode )
    {
        return OP_NULL;
    }

    hOkAction = INVALID_HANDLE;
    hCancelAction = INVALID_HANDLE;
    hWin = INVALID_HANDLE;
    
    pWinNode = WIDGET_ALLOC( sizeof(WindowNode));
    
    if(!pWinNode )
    {
        return OP_NULL;
    }

    op_memset(pWinNode, 0 , sizeof(WindowNode));
    /* Get the window size and create a Viewer  */
    size.width = pWinMgr->iScreenWidth;
    size.height = pWinMgr->iScreenHeight;

    hWin = TPIa_widgetViewerCreateEx(
                                        modId,
                                        hTitle,
                                        hMsg,
                                        &size,
                                        0x8000,
                                        0,
                                        OP_NULL);
                                        
    if( !ISVALIDHANDLE(hWin) )
    {
        goto COMM_CALL_VIEWER_ERROR;
    }

    /* set title of the editor */
    if( ISVALIDHANDLE(hTitle))
    {
        if( 0 > TPIa_widgetSetTitle(hWin, hTitle) )
        {
            goto COMM_CALL_VIEWER_ERROR;
        }
    }
    
    /*  add ok action  */
    if( fnOkCallback != OP_NULL )
    {
        hOkAction = Comm_addActionToWindow(
                                                modId,
                                                hWin,
                                                TPIa_widgetStringGetPredefined(MSF_STR_ID_DLG_ACTION_OK),
                                                MsfOk,
                                                fnOkCallback);
        if( !ISVALIDHANDLE(hOkAction))
        {
            goto COMM_CALL_VIEWER_ERROR;
        }
    }
    
    /*  add Cancel action  */
    if( fnCancelCallback != OP_NULL )
    {
        hCancelAction = Comm_addActionToWindow(
                                                modId,
                                                hWin,
                                                TPIa_widgetStringGetPredefined(MSF_STR_ID_DLG_ACTION_BACK),
                                                MsfBack,
                                                fnCancelCallback);
        if( !ISVALIDHANDLE(hCancelAction))
        {
            goto COMM_CALL_VIEWER_ERROR;
        }
    }

    /* fill the window node structure */
    pWinNode->iWinId = iWinId;
    pWinNode->hWin = hWin;
    pWinNode->pos.x = 0;
    pWinNode->pos.y = 0;
    pWinNode->fnWinDestory = Comm_defaultWindowDestroy;

    /* register the window node  */
    if( 0 > WinMgr_registerWindow( pWinMgr, pWinNode) )
    {
        goto COMM_CALL_VIEWER_ERROR;
    }

    /* show the dialog */
    WinMgr_showWindow(pWinNode, MSF_TRUE);
    
    TPIa_widgetRelease(hOkAction);
    TPIa_widgetRelease(hCancelAction);

    return pWinNode;

COMM_CALL_VIEWER_ERROR:

    if( pWinNode )
    {
        WIDGET_FREE( pWinNode );
    }
    if( ISVALIDHANDLE(hOkAction))
    {
        TPIa_widgetRelease(hOkAction);
    }
    if( ISVALIDHANDLE(hCancelAction))
    {
        TPIa_widgetRelease(hCancelAction);
    }
    if( ISVALIDHANDLE(hWin))
    {
        TPIa_widgetRelease(hWin);
    }
    
    return OP_NULL;
}

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
int  Comm_defaultWindowDestroy (WindowNode* pWinNode )
{
    if( !pWinNode )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( pWinNode->pWinMgr )
    {
        WinMgr_unregisterWindow( pWinNode->pWinMgr, pWinNode );
    }
    
    TPIa_widgetRelease( pWinNode->hWin);
    WIDGET_FREE(pWinNode );

    return TPI_WIDGET_OK;
}

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
int Comm_defaultWindowCancelCallback(MSF_UINT8 modId, MSF_UINT32 hWin, MSF_UINT32 hAction )
{
    WindowManager  *pWinMgr;
    WindowNode       *pWinNode;

    pWinMgr = WinMgr_getWinMgr( modId);

    if( !pWinMgr )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    pWinNode = WinMgr_getWindowByHandle( pWinMgr, hWin);
    if( !pWinNode )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    if( pWinNode->fnWinDestory != OP_NULL )
    {
        (void)pWinNode->fnWinDestory(pWinNode);
    }

    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION: MenuOkActionCallback

    DESCRIPTION:
    It is the state change callback function of dialog close by timer. When timeout, 
    the callback function will be called, it will release the dialog.
   

    ARGUMENTS PASSED:
    modId	      the module/application identifier
    hDlg	      the dialog widget identifier
    notiType	the notification type
    pData	      the extra data

    RETURN VALUE:
    If successful, return 0; or return corresponding error code

    IMPORTANT NOTES:
    None

==================================================================================================*/
static int MenuOkActionCallback (MSF_UINT8 modId, MSF_UINT32 hWin, MSF_UINT32 hAction )
{
    WindowManager  *pWinMgr;
    MenuNode         *pMenuNode;
    int                    iIndex;
    int                    iItemState;

    pWinMgr = WinMgr_getWinMgr( modId);

    if( !pWinMgr )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    pMenuNode =(MenuNode*)WinMgr_getWindowByHandle( pWinMgr, hWin);
    if( !pMenuNode )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    if( 0 > TPIa_widgetChoiceGetCurrentItemState( hWin, &iIndex, &iItemState) )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    if((iIndex >= 0 &&  iIndex < (int)pMenuNode->nItems)
       && ( iItemState & MSF_CHOICE_ELEMENT_SELECTED )
       && ( pMenuNode->pItems[iIndex].fnSelectCallback != OP_NULL ))
    {
         (void)pMenuNode->pItems[iIndex].fnSelectCallback(pMenuNode, (OP_INT16)iIndex );
    }

    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION: Comm_showDlgByTimerDefaultStateChangeCb

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
    
==================================================================================================*/
static int Comm_showDlgByTimerDefaultStateChangeCb(MSF_UINT8 modId, MSF_UINT32 hDlg, MsfNotificationType notiType, void *pData )
{
    TPIa_widgetRelease(hDlg);

    return TPI_WIDGET_OK;
}
/*==================================================================================================
    FUNCTION: Comm_showDlgDefaultCloseCb

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
    
==================================================================================================*/
int Comm_showDlgDefaultCloseCb(MSF_UINT8 modId, MSF_UINT32 hDlg, MSF_UINT32 hAction)
{
    WindowManager *pWinMgr;

    pWinMgr = WinMgr_getWinMgr(modId);
    if( pWinMgr )
    {
        TPIa_widgetRemove(pWinMgr->hScreen, hDlg);
        TPIa_widgetRelease(hDlg);
    }

    return TPI_WIDGET_OK;
}
/*==================================================================================================
    FUNCTION: 

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
    
==================================================================================================*/

/*================================================================================================*/

#ifdef __cplusplus
}
#endif



