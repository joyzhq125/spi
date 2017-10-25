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
    04/02/2004    zhuxq     P002789        Fix UMB bugs
    04/09/2004   zhuxq      P002858        Fix UMB bugs
    04/24/2004   zhuxq      P003022        Fix some bugs in UMB
    06/04/2004    zhuxq            P005925        Correct issues found by PC-lint and fix some bugs

    Self-documenting Code
    Describe/explain low-level design of this module and/or group of funtions and/or specific
    funtion that are hard to understand by reading code and thus requires detail description.
    Free format !
    
====================================================================================================
    INCLUDE FILES
==================================================================================================*/
#include  "app_include.h"
#include "window_manager.h"

/*==================================================================================================
    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

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
 static WindowManager  *winMgrList = OP_NULL;
/*==================================================================================================
    GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
    LOCAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================
    FUNCTION: WinMgr_getWinMgr

    DESCRIPTION:
    Find the window manager in the winMgrList with the modId, if found, 
    return the pointer of the WindowManager; or return NULL.

    ARGUMENTS PASSED:
    modId    the module/application identifier
    
    RETURN VALUE:
    If successful, return the pointer of the WindowManager structure; or return NULL

    IMPORTANT NOTES:
    None

==================================================================================================*/
WindowManager* WinMgr_getWinMgr( OP_UINT8  modId )
{
    WindowManager *pWinMgr = winMgrList;

    while( pWinMgr )
    {
        if( pWinMgr->modId == modId )
        {
            break;
        }

        pWinMgr = pWinMgr->pLink;
    }

    return pWinMgr;
}

/*==================================================================================================
    FUNCTION: WinMgr_create

    DESCRIPTION:
    Find the window manager in the winMgrList with the modId, if found, 
    return the pointer of the WindowManager; or create a WindowManager
    for the module/app specified by modId, and return the pointer of the WindowManager.

    ARGUMENTS PASSED:
    modId    the module/application identifier
    
    RETURN VALUE:
    If successful, return the pointer of the WindowManager structure; or return NULL

    IMPORTANT NOTES:
    None

==================================================================================================*/
WindowManager* WinMgr_create( OP_UINT8  modId )
{
    MsfScreenHandle hScreen;
    MsfWindowHandle hWaitingWin;
    MsfDeviceProperties deviceProperties;
    WindowManager *pWinMgr = winMgrList;

    while( pWinMgr )
    {
        if( pWinMgr->modId == modId )
        {
            return pWinMgr;
        }

        pWinMgr = pWinMgr->pLink;
    }

    /* the WinMgr not exists, create a window manage */
    pWinMgr = WIDGET_ALLOC(sizeof(WindowManager));
    if( !pWinMgr )
    {
        return OP_NULL;
    }

    op_memset(pWinMgr, 0, sizeof(WindowManager));
    
    /* create a screen  */
    hScreen = TPIa_widgetScreenCreateEx( modId, 0, 0, OP_NULL);

    if( !ISVALIDHANDLE(hScreen))
    {
        WIDGET_FREE(pWinMgr);
        return OP_NULL;
    }

    hWaitingWin = TPIa_widgetWaitingWinCreateEx(
                                                modId,
                                                MSF_FALSE,
                                                MSF_FALSE,
                                                MSF_FALSE,
                                                INVALID_HANDLE,
                                                0,
                                                OP_NULL,
                                                MSF_WINDOW_PROPERTY_NOTIFY, 
                                                0, 
                                                OP_NULL);
                                                
    if( !ISVALIDHANDLE(hWaitingWin))
    {
        TPIa_widgetRelease(hScreen);
        WIDGET_FREE(pWinMgr);
        return OP_NULL;
    }
    
    TPIa_widgetDeviceGetProperties(&deviceProperties);

    pWinMgr->modId = modId;
    pWinMgr->hScreen = hScreen;
    pWinMgr->iScreenWidth = deviceProperties.displaySize.width;
    pWinMgr->iScreenHeight = deviceProperties.displaySize.height;
    pWinMgr->pWinStack = OP_NULL;
    pWinMgr->hWaitingWin = hWaitingWin;

    /* add the window manager to the manager list */
    pWinMgr->pLink = winMgrList;
    winMgrList = pWinMgr;      
    
    TPIa_widgetSetInFocus( hScreen, MSF_TRUE );

    return pWinMgr;
}

/*==================================================================================================
    FUNCTION: WinMgr_destroy

    DESCRIPTION:
    The function will free all the window's resource by calling the fnWinDestroy function 
    of the WindowNode, and free all the WindowNode; and free the screen widget,
    last free the WindowManager structure

    ARGUMENTS PASSED:
    pWinMgr     the WindowManager structure pointer. If is NULL, the modId is used
    modId        the module/application identifier. If is NULL, the modId is used
    
    RETURN VALUE:
    none

    IMPORTANT NOTES:
    None

==================================================================================================*/
void WinMgr_destroy(WindowManager *pWinMgr, OP_UINT8  modId)
{
    WindowManager *pPreWinMgr;
    WindowManager *pWinMgrTemp;
    WindowNode     *pWinNode;
    WindowNode     *pWinNodeTemp;
    
    pPreWinMgr = OP_NULL;
    if( !pWinMgr )  /* the  pWinMgr parameter is invalid, use the modId parameter */
    {
        pWinMgr = winMgrList;
        while( pWinMgr )
        {
            if( pWinMgr->modId == modId )
            {
                break;
            }

            pPreWinMgr = pWinMgr;
            pWinMgr = pWinMgr->pLink;
        }
    }
    else
    {
        pWinMgrTemp = pWinMgr;
        pWinMgr = winMgrList;
        
        while( pWinMgr )
        {
            if( pWinMgrTemp == pWinMgr )
            {
                break;
            }

            pPreWinMgr = pWinMgr;
            pWinMgr = pWinMgr->pLink;
        }
    }

    /* not found, or the pWinMgr parameter is freed and re-free */
    if( !pWinMgr )
    {
        return;
    }
    
    TPIa_widgetSetInFocus(pWinMgr->hScreen, MSF_FALSE);
    
    pWinNode = pWinMgr->pWinStack;
    pWinMgr->pWinStack = OP_NULL;
    
    while( pWinNode )
    {
        pWinNodeTemp = pWinNode;
        pWinNode = pWinNode->pNext;
        
        /* similar to unregister window
          * remove window from screen
         */
        TPIa_widgetRemove(pWinMgr->hScreen, pWinNodeTemp->hWin);
        pWinNodeTemp->pWinMgr = OP_NULL;
        pWinNodeTemp->pNext = OP_NULL;
        
        (void)pWinNodeTemp->fnWinDestory( pWinNodeTemp );
    }

    /* release all widgets of the module */
    TPIa_widgetReleaseAll(pWinMgr->modId);

    /* Adjust the link  */
    if( pWinMgr == winMgrList )
    {   
        winMgrList = winMgrList->pLink;
    }
    else
    {
        pPreWinMgr->pLink = pWinMgr->pLink;
    }

    /* free the window manager node */
    WIDGET_FREE(pWinMgr);
    
}

/*==================================================================================================
    FUNCTION: WinMgr_getWindow

    DESCRIPTION:
    According to the window id specified by iWinId, find out the WindowNode.


    ARGUMENTS PASSED:
    pWinMgr     the WindowManager structure pointer
    modId    the module/application identifier
    
    RETURN VALUE:
    If successful, return the pointer of the WindowNode structure; or return NULL

    IMPORTANT NOTES:
    None

==================================================================================================*/
WindowNode*  WinMgr_getWindow(WindowManager * pWinMgr, OP_UINT32  iWinId)
{
    WindowNode     *pWinNode;

    if( !pWinMgr )
    {
        return OP_NULL;
    }
    
    pWinNode = pWinMgr->pWinStack;

    while( pWinNode )
    {
        if( pWinNode->iWinId == iWinId )
        {
            break;
        }

        pWinNode = pWinNode->pNext;
    }

    return pWinNode;
    
}

/*==================================================================================================
    FUNCTION: WinMgr_getWindowByHandle

    DESCRIPTION:
    According to the window handle specified by hWin, find out the WindowNode.


    ARGUMENTS PASSED:
    pWinMgr     the WindowManager structure pointer
    hWin          the window widget handle

    RETURN VALUE:
    If successful, return the pointer of the WindowNode structure; or return NULL

    IMPORTANT NOTES:
    None

==================================================================================================*/
WindowNode*  WinMgr_getWindowByHandle(WindowManager * pWinMgr, OP_UINT32 hWin )
{
    WindowNode     *pWinNode;

    if( !pWinMgr )
    {
        return OP_NULL;
    }
    
    pWinNode = pWinMgr->pWinStack;

    while( pWinNode )
    {
        if( pWinNode->hWin== hWin )
        {
            break;
        }

        pWinNode = pWinNode->pNext;
    }

    return pWinNode;
}


/*==================================================================================================
    FUNCTION: WinMgr_bringWinMgrToTop

    DESCRIPTION:
    Bring the window manager to top, to enable its window to handle user events.


    ARGUMENTS PASSED:
    pWinMgr     the WindowManager structure pointer. If is NULL, the modId is used
    modId        the module/application identifier. If is NULL, the modId is used

    RETURN VALUE:
    If successful, return 0; or return corresponding error code

    IMPORTANT NOTES:
    None

==================================================================================================*/
int WinMgr_bringWinMgrToTop(WindowManager * pWinMgr, OP_UINT8  modId )
{
    if( !pWinMgr )
    {
        pWinMgr = WinMgr_getWinMgr(modId);
    }

    if( !pWinMgr )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    TPIa_widgetSetInFocus(pWinMgr->hScreen, MSF_TRUE );

    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION: WinMgr_bringWinMgrToBack

    DESCRIPTION:
    Bring the window manager to back, to disable its window to handle user events.


    ARGUMENTS PASSED:
    pWinMgr     the WindowManager structure pointer. If is NULL, the modId is used
    modId        the module/application identifier. If is NULL, the modId is used

    RETURN VALUE:
    If successful, return 0; or return corresponding error code

    IMPORTANT NOTES:
    None

==================================================================================================*/
int WinMgr_bringWinMgrToBack(WindowManager * pWinMgr,OP_UINT8  modId )
{
    if( !pWinMgr )
    {
        pWinMgr = WinMgr_getWinMgr(modId);
    }

    if( !pWinMgr )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    TPIa_widgetSetInFocus( pWinMgr->hScreen, MSF_FALSE );

    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION: WinMgr_registerWindow

    DESCRIPTION:
    Add the WindowNode to the WindowManager; if there already exists a
    WindowNode in the WindowManager which iWinId equals the former's
    iWinId, return error code

    ARGUMENTS PASSED:
    pWinMgr     the WindowManager structure pointer
    pWinNode   the WindowNode structure pointer

    RETURN VALUE:
    If successful, return 0; or return corresponding error code

    IMPORTANT NOTES:
    None

==================================================================================================*/
int WinMgr_registerWindow(WindowManager* pWinMgr, WindowNode* pWinNode  )
{
    WindowNode     *pWinNodeTemp;
    
    if( !pWinMgr || !pWinNode )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    /* find out if the window already registered  */
    pWinNodeTemp = pWinMgr->pWinStack;
    while(pWinNodeTemp)
    {
        if( pWinNodeTemp == pWinNode || pWinNodeTemp->hWin == pWinNode->hWin )
        {
            return WIN_MGR_ERROR_WINDOW_ALREADY_REGISTER;
        }
        
        pWinNodeTemp = pWinNodeTemp->pNext;
    }

    if( !ISVALIDHANDLE(pWinNode->hWin ))
    {
        return WIN_MGR_ERROR_BAD_WINDOW_HANDLE;
    }

    /* add window to screen */
    if( 0 > TPIa_widgetScreenAddWindow(pWinMgr->hScreen, pWinNode->hWin, &pWinNode->pos, 0, 0))
    {
        return WIN_MGR_ERROR_WINDOW_REGISTER_FAILED;
    }

    pWinNode->pNext =  pWinMgr->pWinStack;
    pWinMgr->pWinStack = pWinNode;
    
    pWinNode->pWinMgr = pWinMgr;

    return WIN_MGR_WINDOW_REGISTER_SUCCESS;
    
}

/*==================================================================================================
    FUNCTION: WinMgr_unregisterWindow

    DESCRIPTION:
    Unregister a window. The function will remove the window node from the window manager.

    ARGUMENTS PASSED:
    pWinMgr     the WindowManager structure pointer
    pWinNode   the WindowNode structure pointer

    RETURN VALUE:
    If successful, return 0; or return corresponding error code

    IMPORTANT NOTES:
    need to update the design document

==================================================================================================*/
int WinMgr_unregisterWindow(WindowManager* pWinMgr, WindowNode* pWinNode)
{
    WindowNode     *pPreWinNode = OP_NULL;
    WindowNode     *pWinNodeTemp = OP_NULL;
    
    if( !pWinMgr || !pWinNode )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pWinNodeTemp = pWinNode;
    pWinNode = pWinMgr->pWinStack;
    while( pWinNode )
    {
        if( pWinNode == pWinNodeTemp )
        {
            break;
        }

        pPreWinNode = pWinNode;
        pWinNode = pWinNode->pNext;
    }

    /* window node not in the window list */
    if( !pWinNode )
    {
        return WIN_MGR_ERROR_WINDOW_NOT_EXIST;
    }

    TPIa_widgetRemove(pWinMgr->hScreen, pWinNode->hWin);

    /* Adjust the window list */
    if( pWinNode == pWinMgr->pWinStack )
    {
        pWinMgr->pWinStack = pWinNode->pNext;
    }
    else
    {
        pPreWinNode->pNext = pWinNode->pNext;
    }

    pWinNode->pNext = OP_NULL;
    pWinNode->pWinMgr = OP_NULL;

    return TPI_WIDGET_OK;
}


/*==================================================================================================
    FUNCTION: WinMgr_showWaiting

    DESCRIPTION:
    Show a waiting icon in the screen.

    ARGUMENTS PASSED:
    pWinMgr     the WindowManager structure pointer
    
    RETURN VALUE:
    If successful, return 0; or return corresponding error code

    IMPORTANT NOTES:

==================================================================================================*/
int  WinMgr_showWaiting( const WindowManager*pWinMgr )
{
    MsfPosition  pos = {0,0};
    
    if( !pWinMgr )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    if( ISVALIDHANDLE(pWinMgr->hWaitingWin))
    {
        TPIa_widgetScreenAddWindow(
                                            pWinMgr->hScreen,
                                            pWinMgr->hWaitingWin,
                                            &pos,
                                            INVALID_HANDLE,
                                            INVALID_HANDLE);
        TPIa_widgetSetInFocus(pWinMgr->hWaitingWin, MSF_TRUE);
    }
    
    return TPI_WIDGET_OK;
}


/*==================================================================================================
    FUNCTION: WinMgr_updateBegin

    DESCRIPTION:
    Indicates to begin updating the screen.

    ARGUMENTS PASSED:
    pWinMgr     the WindowManager structure pointer
    bShowWaiting   indicates if show the waiting flag
    
    RETURN VALUE:
    If successful, return 0; or return corresponding error code

    IMPORTANT NOTES:

==================================================================================================*/
int  WinMgr_updateBegin( WindowManager*pWinMgr, OP_BOOLEAN bShowWaiting )
{
    MsfPosition  pos = {0,0};
    
    if( !pWinMgr )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( bShowWaiting && ISVALIDHANDLE(pWinMgr->hWaitingWin))
    {
        TPIa_widgetScreenAddWindow(
                                            pWinMgr->hScreen,
                                            pWinMgr->hWaitingWin,
                                            &pos,
                                            INVALID_HANDLE,
                                            INVALID_HANDLE);
        TPIa_widgetSetInFocus(pWinMgr->hWaitingWin, MSF_TRUE);
    }
    
    TPIa_widgetUpdateBegin(pWinMgr->hScreen );

    return TPI_WIDGET_OK;
}          

/*==================================================================================================
    FUNCTION: WinMgr_updateEnd

    DESCRIPTION:
    Indicates to end updating the screen, and show the screen contents.

    ARGUMENTS PASSED:
    pWinMgr     the WindowManager structure pointer
    
    RETURN VALUE:
    If successful, return 0; or return corresponding error code

    IMPORTANT NOTES:

==================================================================================================*/
int WinMgr_updateEnd(  WindowManager*pWinMgr )
{
    if(!pWinMgr )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( ISVALIDHANDLE(pWinMgr->hWaitingWin))
    {
        TPIa_widgetRemove(pWinMgr->hScreen, pWinMgr->hWaitingWin);
    }

    TPIa_widgetUpdateEnd(pWinMgr->hScreen );

    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION: WinMgr_destroyWindow

    DESCRIPTION:
    Destroy a window. The function will remove the window node from the window manager,
    and call its destroy function.

    ARGUMENTS PASSED:
    pWinMgr     the WindowManager structure pointer
    pWinNode   the WindowNode structure pointer; if it is NULL, the iWinId must be valid
    iWinId         the window id; if pWinNode is not NULL, the iWinId will be ignored.

    RETURN VALUE:
    If successful, return 0; or return corresponding error code

    IMPORTANT NOTES:

    The destroy callback function of window node CAN NOT call the WinMgr_destroyWindow,
    or it will arise infinite recursion.

==================================================================================================*/
int WinMgr_destroyWindow(WindowManager* pWinMgr, WindowNode* pWinNode, OP_UINT32 iWinId)
{
    if( !pWinMgr )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( !pWinNode )
    {
        pWinNode = WinMgr_getWindow( pWinMgr, iWinId);
        if( !pWinNode )
        {
            return WIN_MGR_ERROR_WINDOW_NOT_EXIST;
        }
    }

    WinMgr_unregisterWindow( pWinMgr, pWinNode);
    if( pWinNode->fnWinDestory )
    {
        (void)pWinNode->fnWinDestory( pWinNode );
    }
    else
    {
        TPIa_widgetRelease( pWinNode->hWin);
        WIDGET_FREE(pWinNode );
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


