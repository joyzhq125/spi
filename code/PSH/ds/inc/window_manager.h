#ifndef  WINDOW_MANAGER_H
#define  WINDOW_MANAGER_H

/*==================================================================================================

    HEADER NAME : window_manager.h

    GENERAL DESCRIPTION
        Common file header of UMB.

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2004 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    02/23/2004   Zhuxq      C002699       Initial Creation
    04/02/2004    zhuxq     P002789        Fix UMB bugs
    04/09/2004   zhuxq      P002858        Fix UMB bugs
    04/15/2004    zhuxq     P002977        Fix some bugs in UMB for Dolphin

    Self-documenting Code
    Describe/explain low-level design, especially things in header files, of this module and/or
    group of funtions and/or specific funtion that are hard to understand by reading code
    and thus requires detail description.
    Free format !

====================================================================================================
    INCLUDE FILES
==================================================================================================*/

#include "OPUS_typedef.h"
#include "msf_def.h"
#include "Msf_int.h"

/*==================================================================================================
    CONSTANTS
==================================================================================================*/
#ifndef INVALID_HANDLE
#define INVALID_HANDLE      0
#endif

#ifndef ISVALIDHANDLE
#define ISVALIDHANDLE( handle )       ( (handle) != INVALID_HANDLE )
#endif

#define  WIN_MGR_WINDOW_REGISTER_SUCCESS                           0
#define  WIN_MGR_ERROR_WINDOW_ALREADY_REGISTER                -20
#define  WIN_MGR_ERROR_BAD_WINDOW_HANDLE                         -21
#define  WIN_MGR_ERROR_WINDOW_REGISTER_FAILED                  -22
#define  WIN_MGR_ERROR_WINDOW_NOT_EXIST                            -23


/*==================================================================================================
    MACROS
==================================================================================================*/

/*==================================================================================================
    ENUMERATIONS
==================================================================================================*/

/*==================================================================================================
    DATA STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
struct tagWindowNode;

typedef  int (*window_destroy_func_t)( struct tagWindowNode *);    /* prototype  of window destroy function */

typedef struct tagWindowManager {
    struct tagWindowManager   *pLink;    /* the next WindowManager */
    OP_UINT8            modId;              /* the application id */
    OP_UINT32           hScreen;            /* the application's screen */
    OP_INT16             iScreenWidth;     /* the editable width of the screen  */
    OP_INT16             iScreenHeight;    /* the editable height of the screen  */
    OP_UINT32           hWaitingWin;      /* the waiting window */
    struct tagWindowNode  *pWinStack;   /* the managed window stack */
} WindowManager;

typedef struct tagWindowNode{
    struct tagWindowNode    *pNext;             /* the next managed window node */
    OP_UINT32                  iWinId;              /* the window id */
    OP_UINT32                  hWin;                /* the window widget identifier */
    MsfPosition                  pos;                  /* the window position  */
    WindowManager           *pWinMgr;         /*  the window manager */
    window_destroy_func_t   fnWinDestory;  /* the destroy window function  */
}WindowNode;


/*==================================================================================================
    GLOBAL VARIABLES DECLARATIONS
==================================================================================================*/

/*==================================================================================================
    FUNCTION PROTOTYPES
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
extern WindowManager* WinMgr_getWinMgr( OP_UINT8  modId );

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
extern WindowManager* WinMgr_create( OP_UINT8  modId );

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
extern void WinMgr_destroy(WindowManager *pWinMgr, OP_UINT8  modId);

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
extern WindowNode*  WinMgr_getWindow(WindowManager * pWinMgr, OP_UINT32  iWinId);

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
extern WindowNode*  WinMgr_getWindowByHandle(WindowManager * pWinMgr, OP_UINT32 hWin );


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
extern int WinMgr_bringWinMgrToTop(WindowManager * pWinMgr, OP_UINT8  modId );

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
extern int WinMgr_bringWinMgrToBack(WindowManager * pWinMgr,OP_UINT8  modId );

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
extern int WinMgr_registerWindow(WindowManager* pWinMgr, WindowNode* pWinNode  );

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
extern int WinMgr_unregisterWindow(WindowManager* pWinMgr, WindowNode* pWinNode);


#define   WinMgr_showWindow( pWinNode, bShow )  \
          TPIa_widgetSetInFocus(((WindowNode*)pWinNode)->hWin, bShow )

          
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
int  WinMgr_showWaiting( const WindowManager *pWinMgr );

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
extern int   WinMgr_updateBegin( WindowManager*pWinMgr, OP_BOOLEAN bShowWaiting );
          
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
extern int   WinMgr_updateEnd(  WindowManager*pWinMgr );

/*==================================================================================================
    FUNCTION: WinMgr_destroyWindow

    DESCRIPTION:
    Destroy a window, and its resources

    ARGUMENTS PASSED:
    pWinMgr     the WindowManager structure pointer
    pWinNode   the WindowNode structure pointer; if it is NULL, the iWinId must be valid
    iWinId         the window id; if pWinNode is not NULL, the iWinId will be ignored.

    RETURN VALUE:
    If successful, return 0; or return corresponding error code

    IMPORTANT NOTES:
    None

==================================================================================================*/
extern int WinMgr_destroyWindow(WindowManager* pWinMgr, WindowNode* pWinNode, OP_UINT32 iWinId);


/*================================================================================================*/
#endif


