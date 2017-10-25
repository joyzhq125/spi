/*==================================================================================================

    MODULE NAME : int_widget_common.c

    GENERAL DESCRIPTION

    SEF Telecom Confidential Proprietary
    (c) Copyright 2002 by SEF Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    04/03/2003   XXXXX            Cxxxxxx     Initial file creation.
    07/03/2003   Steven Lai       P000729     Fix the bug in setting a gadget in focus
    7/30/2003    Zhuxq            P001026     Delete the code of enter Editor App and add the code of Notification Imm to release
    07/26/2003   lindawang        P001074     Fix draw image problems.
    04/08/2003   Zhuxq            P001052     Modify the method of getting the start position of a gadget, and add the redraw code after a focus gadget lose its focus 
    08/05/2003   Steven Lai       P001140     Change the method of handling widget focus
    08/13/2003   Zhuxq            P001147     Change the selectgroup show and menu show
    08/14/2003   Steven Lai       P001256     Rearrange the widget show function calling code
    08/20/2003   Steven Lai       P001310     Improve the method of checking whether a widget is in focus
    08/20/2003   Zhuxq            P001279     modify the access the size of imagegadget
    08/22/2003   Zhuxq            P001290     Modify the MsfAction 
    08/25/2003   Zhuxq            P001355     Fixed some warning according to PC-Lint check result
    08/27/2003   Zhuxq            P001383     Fixed some bugs
    08/29/2003   Zhuxq            P001396     Add scroll-contents feature to string gadget
    09/02/2003   Steven Lai       P001457     Add a new function TPIa_widgetRemoveAllScreenFocus()
    09/25/2003   Zhuxq            P001696     Re-implement the dialog
    10/23/2003   Zhuxq            P001842     Optimize the paintbox and input method solution and fix some bug
    11/11/2003   Zhuxq            P001883     Optimize the redraw strategy to reduce the redraw times
    12/03/2003   Zhuxq            P002139     Enhance the sensitivity of scrollbar in WAP&MMS
    12/12/2003   Zhuxq            P002192     fix bug for wap memory leak
    01/05/2004   Zhuxq            CNXT000072  Add callback function to each widget in the widget system
    10/23/2003   Zhuxq            P002353     Using timer callback mechanism to support non-focus image gadget to play animation
    02/10/2004   chenxiao         p002405     add form scrollbar  in the dolphin project
    02/16/2004   chenxiao         p002474     add bitmap animation function in the dolphin project    
    02/17/2004   Zhuxq            P002483     Adjust the display system of widget to show up the annunciator and fix some bugs
    02/17/2004   Zhuxq            P002492     Add Viewer window to widget system
    02/19/2004   Zhuxq            P002504     Add box gadget to widget system
    03/01/2004   Zbzhang          P002564     Resolve the bug that when browse the given page    
    03/19/2004   Zhuxq            P002687     Fix some bugs in widget system
    03/27/2004   Zhuxq            P002760     Fix some bugs in widget system
    03/30/2004   zhuxq            P002700     Fix UMB bugs
    04/02/2004   zhuxq            P002789     Fix UMB bugs
    04/09/2004   zhuxq            P002858     Fix UMB bugs
    04/15/2004   zhuxq            P002977     Fix some bugs in UMB for Dolphin
    04/30/2004   zhuxq            P003022     Fix some bugs in UMB
    04/29/2004   zhuxq            P005196     Fix some bugs in UMB
    05/25/2004   chenxiao         p005637     change image interface with DS       
    06/04/2004   zhuxq            P005925     Correct issues found by PC-lint and fix some bugs
    06/09/2004   Jianghb          P006036     Add WHALE1,WHALE2 and NEMO compilerswitch to some module
    06/09/2004   zhuxq            P006048     forbid Menu to respond successive multiple actions
    06/09/2004   zhuxq            P006106     Fix bugs in UMB and WAP found in pretest for TF_DLP_VER_01.24    
    06/12/2004   zhuxq            P006085     Add lotus macro for lotus   
    06/18/2004   zhuxq            P006260     Make up WAP&UMB code to cnxt_int_wap branch
    07/23/2004   zhuxq            P007215     Prevent freeze in wap
    08/06/2004   zhuxq            P007577     adjust the layout of MMS/SMS for B1,  and fix some bugs
    08/30/2004   zhuxq            c007998     improve the implementation of predefined string to reduce the times of memory allocation
    
    Self-documenting Code
    Describe/explain low-level design of this module and/or group of funtions and/or specific
    funtion that are hard to understand by reading code and thus requires detail description.
    Free format !
        
====================================================================================================
    INCLUDE FILES
==================================================================================================*/

#include  "app_include.h"

#ifdef WIN32
#include "portab_new.h"
#else
#include "portab.h"
#endif

#include "SP_list.h"

#include "msf_int.h"
#include "msf_lib.h"

#include "msf_int_widget.h"
#include "int_widget_common.h"
#include "int_widget_custom.h"
#include "int_widget_focus.h"
#include "int_widget_imm.h"

#include "int_widget_show.h"
#include "Keymap.h"

#include "Msf_rc.h"
/*  widget registered timer struct   */
typedef struct  tagRegisteredTimer{
    OP_UINT32    timerId;                /* the timer id, used as event */
    TIMER_TYPE  timerType;            /*  timer type, the value as follows: ONE_SHOT or PERIODIC  */
    OP_UINT32    hWidget;               /*  the widget identifier  */
    widget_timer_callback_t fnOnTimerCallback;  /* the timeout callback function */

    struct  tagRegisteredTimer *pLink;   /* next RegisteredTimer item */
}RegisteredTimer;

/* current widget timer list, all current widget timer in it */
static RegisteredTimer *timerList = OP_NULL;
/* current used timer count */
static OP_INT16   nTimerCount = 0;

extern widget_release_func_t widgetReleaseFunc[MSF_MAX_WIDGETTYPE];
extern widget_remove_func_t widgetRemoveFunc[MSF_MAX_WIDGETTYPE];
extern widget_deallocate_func_t widgetDeallocateFunc[MSF_MAX_WIDGETTYPE];
extern const MSF_UINT16 action_map_array[8];

extern OP_BOOLEAN ds_get_file_image_size
(
    RM_IMAGE_FORMAT format,
    OP_UINT8         *image,
    OP_UINT32        DataSize,
    OP_UINT16        *pWidth,
    OP_UINT16        *pHeight
);

extern int widgetWindowDrawTitle(MsfWindow *pWin, MSF_UINT8* pDefaultTitle, OP_BOOLEAN bRefresh );

extern int widgetWindowAdjustClientArea(MsfWindow *pWin);

extern int widgetGadgetGetPosition(const MsfGadget* pGadget,  OP_INT16  *pX, OP_INT16  *pY);

extern int increase_ref_count(OP_UINT32 handle);

extern void ds_draw_partial_icon 
(
    OP_INT16           dsx,        /* destination start x coor */
    OP_INT16           dsy,        /* destination start y coor */
    OP_INT16           d_width,  /* destination width to be drawn */
    OP_INT16           d_height,  /* destination height to be drawn */
    OP_INT16           ssx,       /* source bitmap start x coor */
    OP_INT16           ssy,       /* source bitmap start y coor */
    RM_ICON_T         *icon
);

extern void TPIc_widgetNotify(MSF_UINT8 modId, MSF_UINT32 msfHandle, MsfNotificationType notificationType);


static int widgetRelatedEditorOnOk( MSF_UINT8 modId, MSF_UINT32 hEditor, MSF_UINT32 hAction)
{ 
    MsfEditor          *pEditor;
    MsfGadget       *pGadget;
    MsfTextInput      *pTextinput;
    MsfWidgetType  wtGadget, wtWin;
    MsfStringHandle hResultString;
    OP_BOOLEAN     bNeedNotify;
    MsfGadgetHandle hGadget;
    widget_state_change_callback_t fnCb;

    if( !ISVALIDHANDLE(hEditor))
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pEditor = (MsfEditor*)seekWidget(hEditor, &wtWin);
    if(!pEditor || wtWin != MSF_EDITOR )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    bNeedNotify = OP_FALSE;
    fnCb = OP_NULL;
    hGadget = INVALID_HANDLE;
    
    /*  the editor's related textinput exists. */
    pTextinput = (MsfTextInput*)seekWidget(pEditor->hRelatedTextInput, &wtGadget);
    if( pTextinput 
      && wtGadget== MSF_TEXTINPUT 
      && pTextinput->hEditor == hEditor )  /*  exactly be the textinput-edtor pair  */
    {
        pGadget = (MsfGadget*)pTextinput;
        
        /*  get edit result   */
        hResultString = TPIa_widgetStringCreateText( modId, hEditor, 0);
        if( ISVALIDHANDLE(hResultString))
        {
            /*  set to textinput  */
            TPIa_widgetTextSetText( pEditor->hRelatedTextInput,
                                                  INVALID_HANDLE,
                                                  pTextinput->input.textType,
                                                  pTextinput->input.txtInputBufMng.maxCharCount,
                                                  hResultString,
                                                  pTextinput->input.singleLine);

            TPIa_widgetRelease(hResultString);

            /* need to notify the module */
            if( pGadget->propertyMask & MSF_GADGET_PROPERTY_NOTIFY )
            {
                bNeedNotify = OP_TRUE;
                fnCb = pGadget->cbStateChange;
                hGadget = pGadget->gadgetHandle;
            }

#ifndef _KEYPAD
            /* notify the paintbox to save the editor data */
            if( pGadget->parent  && getWidgetType(pGadget->parent->windowHandle) == MSF_PAINTBOX )
            {
                TPIc_widgetNotify(pGadget->modId, pGadget->gadgetHandle, MsfNotifyLostFocus);
            }
#endif
        }
        pTextinput->hEditor = INVALID_HANDLE;
    }

    /*  close the editor window  */
    if( pEditor->windowData.parent != OP_NULL )
    {
        TPIa_widgetRemove( pEditor->windowData.parent->screenHandle, hEditor);
    }

    /*  need to notify module of  the content changing event */
    if( bNeedNotify )
    {
        if( fnCb )
        {
            (void)fnCb(modId, hGadget, MsfNotifyStateChange, OP_NULL);
        }
        else
        {
            TPIc_widgetNotify(modId, hGadget, MsfNotifyStateChange);
        }
    }

    return TPI_WIDGET_OK;
}

static int widgetRelatedEditorOnCancel(  MSF_UINT8 modId, MSF_UINT32 hEditor, MSF_UINT32 hAction)
{ 
    MsfEditor          *pEditor;
    MsfTextInput      *pTextinput;
    MsfWidgetType  wtGadget, wtWin;

    if( !ISVALIDHANDLE(hEditor))
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pEditor = (MsfEditor*)seekWidget(hEditor, &wtWin);
    if(!pEditor || wtWin != MSF_EDITOR )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    /*  the editor's related textinput exists. */
    pTextinput = (MsfTextInput*)seekWidget(pEditor->hRelatedTextInput, &wtGadget);
    if( pTextinput 
      && wtGadget== MSF_TEXTINPUT 
      && pTextinput->hEditor == hEditor )  /*  exactly be the textinput-edtor pair  */
    {
        pTextinput->hEditor = INVALID_HANDLE;
    }
    
    /*  close the editor window  */
    if( pEditor->windowData.parent != OP_NULL )
    {
        TPIa_widgetRemove( pEditor->windowData.parent->screenHandle, hEditor);
    }

    return TPI_WIDGET_OK;
}    

/* to disable the left action of screen, especially in browser */
static int widgetRelatedEditorOnLeftAction( MSF_UINT8 modId, MSF_UINT32 hEditor, MSF_UINT32 hAction)
{
    return TPI_WIDGET_OK;
}

static int widgetRelatedEditorOnTimer(MSF_UINT32 hEditor, MSF_UINT32 iTimerId )
{
    /* close the editor window */
    return widgetRelatedEditorOnCancel( getModID(hEditor), hEditor, INVALID_HANDLE);
}

static  int  widgetRelatedEditorOnStateChange(MSF_UINT8 modId, MSF_UINT32 hEditor, MsfNotificationType notiType, void *pData )
{
    if( notiType == MsfNotifyLostFocus )
    {
        /* start a timer to close the editor window */
        TPIa_widgetStartTimer( 10, MSF_FALSE, hEditor, widgetRelatedEditorOnTimer);
    }

    return TPI_WIDGET_OK;
}
/****************************************************************
  Widget General
 ***************************************************************/
/*=============================================================================
    FUNCTION: TPIa_widgetDeviceGetProperties

    DESCRIPTION:
        Gets the MsfDeviceProperties of the device.

    ARGUMENTS PASSED:
        theDeviceProperties : A pointer to an empty MsfDeviceProperties struct.        
                       
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise the appropriate 
        returncode.

    IMPORTANT NOTES:
        None
===============================================================================*/
int TPIa_widgetDeviceGetProperties(MsfDeviceProperties* theDeviceProperties)
{
    if (theDeviceProperties != OP_NULL)
    {
        theDeviceProperties->displaySize.height = WAP_SCRN_TEXT_HIGH;
        theDeviceProperties->displaySize.width= LCD_MAX_X;
        theDeviceProperties->isTouchScreen = OP_FALSE;
        theDeviceProperties->hasDragAbility = OP_FALSE;
        theDeviceProperties->isColorDisplay = OP_TRUE;
        theDeviceProperties->numberOfColors = 0xFFFF;
        /* 
         * all defined MsfEvents are supported except for MsfKey_Soft1, MsfKey_Soft2,
         * MsfKey_Soft3, MsfKey_Soft4, MsfKey_Function3, MsfKey_Function4, and MsfPointer
         * the value is 0000 0011 0000 1111 1111 1111 1111 1111
         */
        theDeviceProperties->supportedMsfEvents = 0x033FFFFF; 
        return TPI_WIDGET_OK;
    }
    else
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
}
/*=============================================================================
    FUNCTION: TPIa_widgetSetInFocus

    DESCRIPTION:
        Sets the specified widget to the currently active widget within the parent widget to
        which it belongs.
        
        If a call to this function is made with the focus parameter set to 0 and an
        MsfGadgetHandle to the MsfGadget currently in focus, the MsfGadget will lose the
        focus and no other MsfGadget attached to the same MsfWindow will get the focus.
        The MsfWindow will be in focus and no attached MsfGadgets will have focus.
        
        If a call to this function is made with the focus parameter set to 0 and an
        MsfWindowHandle to the MsfWindow currently in focus, the specified MsfWindow
        will lose the focus and the previous MsfWindow attached to the same MsfScreen will
        get the focus. If no other MsfWindow exists on that MsfScreen, the MsfScreen will be
        in focus and no attached MsfWindows will have focus.
        
        If a call to this function is made with the focus parameter set to 0 and an
        MsfScreenHandle to the MsfScreen currently in focus, the specified MsfScreen will
        lose the focus and the previous MsfScreen will get the focus. If no other MsfScreen
        exists, there will be no MsfScreen in focus.

        Note that when a call to this function results in a focus change, all widgets involved
        that have the notification functionality enabled must be notified of the focus change.
    
        (More information in Reference Manual)

    ARGUMENTS PASSED:
        handle  : The widget identifier.
        focus    : Tells if the widget wants to get in focus (1) or get out of focus (0).                
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise the
        appropriate return code is given.
 
    IMPORTANT NOTES:
        None
===============================================================================*/
int TPIa_widgetSetInFocus (MSF_UINT32 handle, int focus)
{
    /*
     * Sets the specified widget to the currently active widget within the parent widget to
     * which it belongs. But this function will not effect the state of the parent widget.
     */
    MsfWidgetType   widgetType;
    void           *pWidget;
    MsfScreen  *pScreen;
    MsfWindow *pWin;
    MsfGadget            *pGadget;
    int ret = TPI_WIDGET_OK;

    pWidget = seekWidget(handle, &widgetType);
    if (pWidget == OP_NULL)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else if (IS_LOWLEVEL(widgetType))
    {
        /*
         * the low-level widget cannot be set in focus 
         */
        return TPI_WIDGET_ERROR_BAD_HANDLE; 
    }

    if(focus == 0)
    {
        if (IS_SCREEN(widgetType))
        {
            pScreen = (MsfScreen *)pWidget;
            ret = removeScreenFocus(pScreen);
        }
        else if (IS_WINDOW(widgetType))
         {
            pWin = (MsfWindow *)pWidget;
            ret  = removeWindowFocus(pWin);
           
        }
        else if (IS_GADGET(widgetType))
        {
            pGadget = (MsfGadget*)pWidget;
            ret = removeGadgetFocus(pGadget);
        }
    }
    else    /* focus ==  1*/
    {
        if(IS_SCREEN(widgetType))  /* to set a screen in focus */
        {
            pScreen = (MsfScreen *)pWidget;
            ret = setScreenInFocus(pScreen);
        }
        else if(IS_WINDOW(widgetType))    /* to set a window in focus */
        {
            pWin = (MsfWindow *)pWidget;
            ret = setWindowInFocus(pWin);
        }
        else if(IS_GADGET(widgetType))
        {
            pGadget = (MsfGadget*)pWidget;
            ret = setGadgetInFocus(pGadget);
        }
            
    }

    if(IS_GADGET(widgetType) && ret != WIDGET_WINDOW_ALREADY_DRAWN )
    {
        widgetRedraw( (void *) pWidget, widgetType, handle, OP_TRUE);
    }
    
    return ret;
}

/*=============================================================================
    FUNCTION: TPIa_widgetRemoveAllScreenFocus

    DESCRIPTION:
        Set all screen in non-focus, so that the following widget draw/redraw operation will not be displayed.

    ARGUMENTS PASSED:
                        
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise the
        appropriate return code is given.
 
    IMPORTANT NOTES:
        None
===============================================================================*/
int TPIa_widgetRemoveAllScreenFocus (void)
{
    screenLoseFocus(pDisplay->focusedScreen);
    pDisplay->focusedScreen = OP_NULL;
    
    return TPI_WIDGET_OK;
}

/*=============================================================================
    FUNCTION: TPIa_widgetHasFocus

    DESCRIPTION:
        Checks if the specified widget is in focus. To be in focus it has to be 
        the currently active widget on the display. For example, an MsfGadget has 
        focus only if it is the currently active MsfGadget within the currently 
        active MsfWindow belonging to the currently active MsfScreen. This function 
        will also return true if the MsfHandle to the parent MsfWindow or MsfScreen,
        of the MsfGadget currently in focus, is passed as the parameter in the function 
        call.

    ARGUMENTS PASSED:
        handle  : The widget identifier.

    RETURN VALUE:
        1 if the specified widget is in focus, otherwise 0.
 
    IMPORTANT NOTES:
        None
===============================================================================*/
int TPIa_widgetHasFocus (MSF_UINT32 handle)
{
    /* 
     * we need to judge whether all the ancestors are in focus now.  This function should reture 
     * false if the parent widget is not the currently active widget.
     */
    MsfWidgetType   widgetType;
    int ret = 0;
    void *pWidget;
    
    pWidget = seekWidget(handle, &widgetType);
    if (pWidget == OP_NULL)
    {
        ret = 0;
    }
    else if (IS_SCREEN(widgetType))
    {
        MsfScreen *pScreen = pWidget;
        ret = pScreen->isFocused;
    }
    else if (IS_WINDOW(widgetType))
    {
        MsfWindow *ptr;
        ptr = (MsfWindow *)pWidget;
        ret = ptr->isFocused;
    }
    else if (IS_GADGET(widgetType))
    {
        MsfGadget *ptr;
        ptr = (MsfGadget *)pWidget;
        ret = ptr->isFocused;
    }

    return ret;
}

/*=============================================================================
    FUNCTION: TPIa_widgetRelease

    DESCRIPTION:
        Releases the specified widget and invalidates the MsfHandle. Other widgets 
        attached to the specified widget must be removed from the specified widget 
        by the integration and their reference counters must be decreased by one. 
        If for example an MsfForm is to be released and the MsfForm has an MsfGadget 
        attached to itself, the MsfGadget must be removed from the MsfForm and the 
        reference counter of the MsfGadget must be decreased by one before the MsfForm 
        is released. 

    ARGUMENTS PASSED:
        handle  : The widget identifier.

    RETURN VALUE:
        A positive integer if the operation was successful, otherwise the 
        appropriate return code.
 
    IMPORTANT NOTES:
        None
===============================================================================*/
int TPIa_widgetRelease (MSF_UINT32 handle)
{
    /*
     * Update the reference counter of all the widget attached to this widget.
     * Releases the specified widget and invalidates the MsfHandle.
     */
    MsfWidgetType widgetType;
    int ret;

    if (handle != INVALID_HANDLE)
    {
        widgetType = getWidgetType(handle);

        if(widgetType < MSF_MAX_WIDGETTYPE && widgetReleaseFunc[widgetType])
        {   
            ret = widgetReleaseFunc[widgetType](handle);
        }
        else
        {
            ret = TPI_WIDGET_ERROR_INVALID_PARAM;
        }
    }
    else
    {
        ret = TPI_WIDGET_ERROR_BAD_HANDLE;
    }
    return ret;
}
/*=============================================================================
    FUNCTION: TPIa_widgetReleaseAll

    DESCRIPTION:
        Releases all of the widgets created by the specified module and invalidates 
        their MsfHandles. Widgets with no users, that is, with the reference counter 
        equal to 0 are deallocated

    ARGUMENTS PASSED:
        modId  : The MSF Module identifier

    RETURN VALUE:
        A positive integer if the operation was successful, otherwise the 
        appropriate return code.
 
    IMPORTANT NOTES:
        None
===============================================================================*/
int TPIa_widgetReleaseAll (MSF_UINT8 modId)
{
    MSF_UINT8 index;
    GENERAL_LIST_STRUCT_T *curWidgetQueue;
    LIST_POSITION_STRUCT_T *pPos;
    void *ptr, *pTemp;
    MSF_UINT8 tempID = MSF_NUMBER_OF_MODULES;
    MSF_UINT32 handle, parentHandle;
    MsfScreen *pScreen;
    MsfWindow *pWindow;
    MsfGadget *pGadget;
    MsfImage *pImage;
    MsfString *pString;
    MsfIcon *pIcon;
    MsfSound *pSound;
    MsfStyle *pStyle;
    MsfColorAnim *pColorAnim;
    MsfMove *pMove;
    MsfRotation *pRotation;
    MsfAction    *pAction;
    
    int ret = TPI_WIDGET_OK;


    for (index = MSF_SCREEN; index < MSF_MAX_WIDGETTYPE; index++)
    {
        curWidgetQueue = &widgetQueue[index];
        ptr = SP_list_get_first_item(curWidgetQueue);
        pPos = curWidgetQueue->m_pHead;
        
        while(ptr != OP_NULL)
        {
            switch (index)
            {
                case MSF_SCREEN:
                    pScreen = (MsfScreen *)ptr;
                    tempID = pScreen->modId;
                    handle = pScreen->screenHandle;
                    break;
                    
                case MSF_PAINTBOX:
                case MSF_FORM:
                case MSF_DIALOG:
                case MSF_MENU:
                case MSF_MAINMENU:
                case MSF_VIEWER:
                case MSF_WAITINGWIN:
                case MSF_EDITOR:
                    pWindow = (MsfWindow *)ptr;
                    tempID = pWindow->modId;
                    handle = pWindow->windowHandle;
                    break;
                    
                case MSF_SELECTGROUP:
                case MSF_DATETIME:
                case MSF_TEXTINPUT:
                case MSF_STRINGGADGET:
                case MSF_IMAGEGADGET:
                case MSF_BUTTON:
                case MSF_BMPANIMATION:
                case MSF_BOX:
                case MSF_BAR:
                    pGadget = (MsfGadget *)ptr;
                    tempID = pGadget->modId;
                    handle = pGadget->gadgetHandle;
                    break;

                case MSF_IMAGE:
                    pImage = (MsfImage *)ptr;
                    tempID = pImage->modId;
                    handle = pImage->handle;
                    break;
                    
                case MSF_STRING:
                    pString = (MsfString *)ptr;
                    tempID = pString->modId;
                    handle = pString->handle;
                    break;
                    
                case MSF_ICON:
                    pIcon = (MsfIcon *)ptr;
                    tempID = pIcon->modId;
                    handle = pIcon->handle;
                    break;
                    
                case MSF_SOUND:
                    pSound = (MsfSound *)ptr;
                    tempID = pSound->modId;
                    handle = pSound->handle;
                    break;
                    
                case MSF_STYLE:
                    pStyle = (MsfStyle *)ptr;
                    tempID = pStyle->modId;
                    handle = pStyle->handle;
                    break;
                    
                case MSF_COLORANIM:
                    pColorAnim = (MsfColorAnim *)ptr;
                    tempID = pColorAnim->modId;
                    handle = pColorAnim->handle;
                    break;
                    
                case MSF_MOVE:
                    pMove = (MsfMove *)ptr;
                    tempID = pMove->modId;
                    handle = pMove->handle;
                    break;
                    
                case MSF_ROTATION:
                    pRotation = (MsfRotation *)ptr;
                    tempID = pRotation->modId;
                    handle = pRotation->handle;
                    break;

                case MSF_ACTION:
                    pAction = (MsfAction *)ptr;
                    tempID = pAction->modId;
                    handle = pAction->actionHandle;
                    break;

                default:
                    tempID = MSF_NUMBER_OF_MODULES;
                    break;
            }

            pPos = pPos->pNext;
            if( pPos )
            {
                ptr = pPos->pItem;
            }
            else
            {
                ptr = OP_NULL;
            }
            
            if (modId == tempID)
            {
                parentHandle = INVALID_HANDLE;
                if (IS_WINDOW(index) && pWindow->parent)
                {
                    parentHandle = pWindow->parent->screenHandle;
                }
                else if(IS_GADGET(index) && pGadget->parent)
                {
                    parentHandle = pGadget->parent->windowHandle;
                }

                if(widgetReleaseFunc[index])
                {
                    (void)widgetReleaseFunc[index](handle);
                }

                if ((IS_WINDOW(index) || IS_GADGET(index)) && 
                    (INVALID_HANDLE != parentHandle))
                {
                    TPIa_widgetRemove(parentHandle, handle);
                }
                
                pTemp = seekWidget(handle, OP_NULL);
                if (OP_NULL != pTemp && widgetDeallocateFunc[index])
                {
                    (void)widgetDeallocateFunc[index](pTemp);
                }
            }
        }
    }
    
    return ret;
}

/*=============================================================================
    FUNCTION: TPIa_widgetCopy

    DESCRIPTION:
        Creates a new widget that is a copy of the specified widget; that is, 
        with the same properties as the specified widget. Note that only the widget
        properties of the original widget will be copied to the new widget. No 
        attached MsfGadgets, MsfActions, MsfImages, MsfStrings or other widgets 
        will be copied or attached. Note also that the new widget (the copy) belongs
        to the calling module and must be released by that module when it is no 
        longer used.

    ARGUMENTS PASSED:
        modId  : The MSF Module identifier of the module that is creating the copy.
        handle  : The widget identifier.

    RETURN VALUE:
        The new MsfHandle if the operation was successful, otherwise 0.
 
    IMPORTANT NOTES:
        None
===============================================================================*/
MSF_UINT32 TPIa_widgetCopy (MSF_UINT8 modId, MSF_UINT32 handle)
{
    void *ptr;
    MsfWidgetType widgetType;
    MSF_UINT32 ret = 0;
    
    ptr = seekWidget(handle, &widgetType);

    if (ptr != OP_NULL)
    {
        switch (widgetType)
        {
            case MSF_SCREEN:
                {
                    MsfScreen *pScreen;
                    pScreen = (MsfScreen *)ptr;
                    ret = TPIa_widgetScreenCreate(modId, pScreen->notify, pScreen->style);
                    break;
                }
                
            case MSF_PAINTBOX:
                {
                    MsfWindow *pWindow;
                    //MsfPaintbox *pPaintbox;
                    pWindow = (MsfWindow *)ptr;
                    /*if (pWindow->data != OP_NULL)
                    {
                        pPaintbox = pWindow->data;
                    }*/
                    ret = TPIa_widgetPaintboxCreate(modId, 
                                                    &(pWindow->size), 
                                                    pWindow->propertyMask, 
                                                    pWindow->style);
                    

                    break;
                }
                
            case MSF_FORM:
                {
                    MsfWindow *pWindow;
                    //MsfForm *pForm;
                    pWindow = (MsfWindow *)ptr;
                    /*if (pWindow->data != OP_NULL)
                    {
                        pForm = pWindow->data;
                    }*/
                    ret = TPIa_widgetFormCreate(modId,
                                                &(pWindow->size), 
                                                pWindow->propertyMask, 
                                                pWindow->style);
                    break;
                }
            case MSF_DIALOG:
                {
                    //MsfWindow *pWindow;
                    MsfDialog *pDialog;
                    pDialog = (MsfDialog *)ptr;
                    //pDialog = pWindow->data;
                    ret = TPIa_widgetDialogCreate(modId, 
                                                  pDialog->dialogText, 
                                                  pDialog->type,
                                                  pDialog->timeoutTime, 
                                                  pDialog->windowData.propertyMask, 
                                                  pDialog->windowData.style);
                    break;
                }
            case MSF_MENU:
                {
                    MsfMenuWindow *pMenu;
                    MSF_UINT8 i;
                    MsfActionHandle implicitSelectAction; 

                    pMenu = (MsfMenuWindow *)ptr;
                    implicitSelectAction = INVALID_HANDLE;
                    for (i = 0; i < 8; i++)
                    {
                        if (pMenu->windowData.actionlist[i]->actionType == MsfSelect)
                        {
                            implicitSelectAction = pMenu->windowData.actionlist[i]->actionHandle;
                            break;
                        }
                    }
                    ret = TPIa_widgetMenuCreate(modId, 
                                                pMenu->pSelectgroup->choice.type,
                                                &(pMenu->windowData.size), 
                                                implicitSelectAction, 
                                                &(pMenu->pSelectgroup->choice.elementPos), 
                                                pMenu->pSelectgroup->choice.bitmask, 
                                                pMenu->windowData.propertyMask, 
                                                pMenu->windowData.style);

                    break;
                }
            case MSF_EDITOR:
                {
                    //MsfWindow *pWindow;
                    MsfEditor *pEditor;

                    pEditor = (MsfEditor *)ptr;
                    /*
                    if (pWindow->data != OP_NULL)
                    {
                        pEditor = pWindow->data;
                    }
                    */
                    ret = TPIa_widgetPaintboxCreate(modId, 
                                                    &(pEditor->windowData.size), 
                                                    pEditor->windowData.propertyMask, 
                                                    pEditor->windowData.style);

                    break;
                }
            case MSF_SELECTGROUP:
                {
                    //MsfGadget *pGadget;
                    MsfSelectgroup *pSelectgroup;

                    pSelectgroup = (MsfSelectgroup *)ptr;
                    /*
                    if (pGadget->data != OP_NULL)
                    {
                        pSelectgroup = pGadget->data;
                    }
                    */
                    ret = TPIa_widgetPaintboxCreate(modId, 
                                                    &(pSelectgroup->gadgetData.size), 
                                                    pSelectgroup->gadgetData.propertyMask, 
                                                    pSelectgroup->gadgetData.style);

                    break;
                }
            case MSF_DATETIME:
                {
                    //MsfGadget *pGadget;
                    MsfDateTimeStruct *pDatetime;
                    pDatetime = (MsfDateTimeStruct *)ptr;
                    /*
                    if (pGadget->data != OP_NULL)
                    {
                        pDatetime = pGadget->data;
                    }
                    */
                    ret = TPIa_widgetDateTimeCreate(modId, 
                                                    pDatetime->mode, 
                                                    &(pDatetime->time), 
                                                    &(pDatetime->date), 
                                                    &(pDatetime->gadgetData.size), 
                                                    pDatetime->gadgetData.propertyMask,
                                                    pDatetime->gadgetData.style);

                    break;
                }
            case MSF_TEXTINPUT:
                {
                    //MsfGadget *pGadget;
                    MsfTextInput *pTextInput;
                    //MsfInput  *pTextInput;
                    pTextInput = (MsfTextInput *)ptr;
                    /*
                    if (pGadget->data != OP_NULL)
                    {
                        pTextInput = pGadget->data;
                    }
                    */
                    ret = TPIa_widgetTextInputCreate(modId, 
                                                     (pTextInput->input).initString, 
                                                     (pTextInput->input).inputString, 
                                                     (pTextInput->input).textType, 
                                                     /*pTextInput->maxSize,*/ 
                                                     (pTextInput->input).txtInputBufMng.maxCharCount,
                                                     (pTextInput->input).singleLine, 
                                                     &(pTextInput->gadgetData.size), 
                                                     pTextInput->gadgetData.propertyMask, 
                                                     pTextInput->gadgetData.style);
                    break;
                }
            case MSF_STRINGGADGET:
                {
                    //MsfGadget *pGadget;
                    MsfStringGadget *pStringGadget;

                    pStringGadget = (MsfStringGadget *)ptr;
                    /*
                    if (pGadget->data != OP_NULL)
                    {
                        pStringGadget = pGadget->data;
                    }
                    */
                    ret = TPIa_widgetStringGadgetCreate(modId, 
                                                        pStringGadget->text, 
                                                        &(pStringGadget->gadgetData.size),  
                                                        pStringGadget->singleLine, 
                                                        pStringGadget->gadgetData.propertyMask,
                                                        pStringGadget->gadgetData.style);
                    break;
                }
            case MSF_IMAGEGADGET:
                {
                    //MsfGadget *pGadget;
                    MsfImageGadget *pImageGadget;

                    pImageGadget = (MsfImageGadget *)ptr;
                    /*
                    if (pGadget->data != OP_NULL)
                    {
                        pImageGadget = pGadget->data;
                    }
                    */
                    ret = TPIa_widgetImageGadgetCreate(modId, 
                                                       pImageGadget->image, 
                                                       &(pImageGadget->gadgetData.size),  
                                                       pImageGadget->imageZoom, 
                                                       pImageGadget->gadgetData.propertyMask,
                                                       pImageGadget->gadgetData.style);
                    break;
                }
            case MSF_BAR:
                {
                    //MsfGadget *pGadget;
                    MsfBar    *pBar;

                    pBar = (MsfBar*)ptr;
                    /*
                    if (pGadget->data != OP_NULL)
                    {
                        pBar = pGadget->data;
                    }
                    */
                    ret = TPIa_widgetBarCreate(modId, 
                                               pBar->barType, 
                                               pBar->maxValue, 
                                               pBar->value, 
                                               &(pBar->gadgetData.size), 
                                               pBar->gadgetData.propertyMask,
                                               pBar->gadgetData.style);
                    break;
                }
            case MSF_IMAGE:
                {
                    MsfImage *pImage;
                    pImage = (MsfImage *)ptr;
                    ret = TPIa_widgetImageCreate(modId, 
                                                 (char *)pImage->data, 
                                                 pImage->dataSize, 
                                                 pImage->format, 
                                                 0, 
                                                 pImage->style);
                    break;
                }
            case MSF_STRING:
                {
                    MsfString *pString;
                    pString = (MsfString *)ptr;
                    ret = TPIa_widgetStringCreate(modId, 
                                                  (char *)pString->data, 
                                                  pString->format, 
                                                  pString->length, 
                                                  pString->style);
                    break;
                }
            case MSF_ICON:
                {
                    MsfIcon *pIcon;
                    pIcon = (MsfIcon *)ptr;
                    ret = TPIa_widgetIconCreate(modId, 
                                                pIcon->type, 
                                                pIcon->style);
                    break;
                }
            case MSF_SOUND:
                {
                    MsfSound *pSound;
                    pSound = (MsfSound *)ptr;
                    ret = TPIa_widgetSoundCreate(modId, 
                                                 pSound->data, 
                                                 pSound->format, 
                                                 pSound->size, 
                                                 0);
                    break;
                }
            case MSF_STYLE:
                {
                    MsfStyle *pStyle;
                    pStyle = (MsfStyle *)ptr;
                    ret = TPIa_widgetStyleCreate(modId, 
                                                 &pStyle->color, 
                                                 &pStyle->backgroundColor, 
                                                 &pStyle->foreground, 
                                                 &pStyle->background, 
                                                 &pStyle->lineStyle, 
                                                 &pStyle->font, 
                                                 &pStyle->textProperty);
                    break;
                }
            case MSF_COLORANIM:
                {
                    MsfColorAnim *pColorAnim;
                    pColorAnim = (MsfColorAnim *)ptr;
                    ret = TPIa_widgetColorAnimCreate(modId, 
                                                     &pColorAnim->startColor, 
                                                     &pColorAnim->endColor, 
                                                     pColorAnim->duration, 
                                                     pColorAnim->nrOfSteps, 
                                                     pColorAnim->property, 
                                                     pColorAnim->repeat);
                    break;
                }
            case MSF_MOVE:
                {
                    MsfMove *pMove;
                    pMove = (MsfMove *)ptr;
                    ret = TPIa_widgetMoveCreate(modId, 
                                                &pMove->origin, 
                                                &pMove->destination, 
                                                pMove->duration, 
                                                pMove->nrOfSteps, 
                                                pMove->property, 
                                                pMove->repeat);
                    break;
                }
            case MSF_ROTATION:
                {
                    MsfRotation *pRotation;
                    pRotation = (MsfRotation *)ptr;
                    ret = TPIa_widgetRotationCreate(modId, 
                                                    pRotation->startAngle, 
                                                    pRotation->endAngle, 
                                                    pRotation->duration, 
                                                    pRotation->nrOfSteps, 
                                                    pRotation->property, 
                                                    pRotation->repeat);
                    break;
                }
            case MSF_ACTION:
                {
                    MsfAction *pAction;
                    pAction = (MsfAction *)ptr;
                    ret = TPIa_widgetActionCreate(modId, 
                                                  pAction->label, 
                                                  pAction->actionType, 
                                                  pAction->priority, 
                                                  pAction->propertyMask);
                    break;
                }
        }
    }
    return ret;
}

/*=============================================================================
    FUNCTION: TPIa_widgetRemove

    DESCRIPTION:
        Removes the widget specified by the handleToBeRemoved parameter from the
        widget specified by the handle parameter. This function is applicable to 
        MsfScreen,MsfWindow, MsfGadget, MsfAction, MsfMove, MsfRotation and 
        MsfColorAnim widgets. If the widget that is removed has a graphical 
        representation and is currently visible, the MsfDisplay should be updated 
        automatically by the integration. The removed widget is not released and 
        the MsfHandle is not invalidated.

        If an MsfWindow that is in focus is removed from its MsfScreen the MsfWindow 
        that previously was in focus will receive focus again. If that MsfWindow
        had any MsfGadget that was in focus when the MsfWindow lost its focus 
        before, the focus state will be restored, which means that the MsfGadget 
        will also receive focus again. Accordingly, if an MsfGadget that is in 
        focus is removed from its MsfWindow the MsfGadget that previously was 
        in focus will receive focus again.

    ARGUMENTS PASSED:
        handle : The widget identifier.
        handleToBeRemoved : The widget identifier of the widget that is to be removed.

    RETURN VALUE:
        A positive integer if the operation was successful, otherwise the appropriate 
        return code.
 
    IMPORTANT NOTES:

===============================================================================*/
int TPIa_widgetRemove(MSF_UINT32 handle, MSF_UINT32 handleToBeRemoved)
{
    /*
     * If handle is a screen and handleToBeRemoved is a window, handle the windowsInScreen list
     * If handle is a window and handleToBeRemoved is a gadget, handle gadgetsInWindow list
     * If handleToBeRemoved is a MsfAction, handle Actionlist
     * If handleToBeRemoved is MsfMove, MsfRotation or MsfColorAnim, reset the corresponding element
     */
    int ret = TPI_WIDGET_OK;


    MsfWidgetType iParWidgetType,iChldWidgetType;
    void   *pParWidget,*pChldWidget;

    pParWidget = seekWidget(handle,&iParWidgetType);
    if(!pParWidget)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM; 
    }
    
    pChldWidget = seekWidget(handleToBeRemoved,&iChldWidgetType);
    if(!pChldWidget)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM; 
    }

    if(widgetRemoveFunc[iChldWidgetType])
    {
        ret = widgetRemoveFunc[iChldWidgetType](pParWidget,iParWidgetType,pChldWidget,iChldWidgetType);
    }

    return ret;

}

/*=============================================================================
    FUNCTION: TPIa_widgetAddAction

    DESCRIPTION:
        Adds the specified MsfAction to the specified widget. This function is 
        applicable only to MsfWindow and MsfScreen widgets. The integration may, 
        for example, map the MsfAction to a softbutton or place it in a menu. 
        If the MsfAction is added to the MsfWindow or MsfScreen currently in focus, 
        and the MsfAction has a graphical representation, the MsfDisplay should 
        be updated automatically by the integration. MsfActions typically have a 
        graphical representation when they are attached to an MsfWindow but not 
        when they are attached to an MsfScreen.

    ARGUMENTS PASSED:
        handle : The widget identifier, that is, the MsfWindow or MsfScreen identifier.
        action : The MsfAction identifier

    RETURN VALUE:
        A positive integer if the operation was successful,otherwise the appropriate 
        return code is given.
 
    IMPORTANT NOTES:
        None
===============================================================================*/
int TPIa_widgetAddAction (MSF_UINT32 handle, MsfActionHandle action)
{
    MsfWidgetType widgetType, widgetType1;
    void *ptr = OP_NULL;
    MsfAction *pAction = OP_NULL;
    int ret = TPI_WIDGET_OK;
    MsfScreen *pScreen;
    MsfWindow *pWindow;
    MsfAction **pActionlist;
    
    ptr = seekWidget(handle, &widgetType);
    pAction = (MsfAction *)seekWidget(action, &widgetType1);

    if (ptr == OP_NULL ||
        widgetType > MSF_WINDOW_END|| /* not a screen or window */
        pAction == OP_NULL ||
        widgetType1 != MSF_ACTION)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    if (widgetType == MSF_SCREEN)
    {
        pScreen = (MsfScreen *)ptr;
        pActionlist = &pScreen->actionlist[pAction->actionType];
    }
    else /* handle is a window handle */
    {
        pWindow = (MsfWindow *)ptr;
        pActionlist = &pWindow->actionlist[pAction->actionType];
    }

    if (*pActionlist == OP_NULL) 
    { /* have not been added before */
        *pActionlist = pAction;
        increase_ref_count(action);

        {   /* Modify SoftKey  */
            OP_UINT8    *pChar;
            
            WIDGET_GET_STRING_DATA(pChar, pAction->label);
            
            if( !pChar )
            {
                return TPI_WIDGET_ERROR_UNEXPECTED;
            }
            
            switch( action_map_array[ pAction->actionType] )
            {
                case KEY_SOFT_LEFT:
                {
                    if (widgetType == MSF_SCREEN)
                    {
                        pScreen->softKeyList[0] = pChar;
                        pAction->ppSoftKey = &pScreen->softKeyList[0];
                    }
                    else
                    {
                        pWindow->softKeyList[0] = pChar;
                        pAction->ppSoftKey = &pWindow->softKeyList[0];
                    }
                    
                    break;
                }
                case KEY_OK:
                {
                    if (widgetType == MSF_SCREEN)
                    {
                        pScreen->softKeyList[1] = pChar;
                        pAction->ppSoftKey = &pScreen->softKeyList[1];
                    }
                    else
                    {
                        pWindow->softKeyList[1] = pChar;
                        pAction->ppSoftKey = &pWindow->softKeyList[1];
                    }
                    
                    break;
                }
                case KEY_SOFT_RIGHT:
                {
                    if (widgetType == MSF_SCREEN)
                    {
                        pScreen->softKeyList[2] = pChar;
                        pAction->ppSoftKey = &pScreen->softKeyList[2];
                    }
                    else
                    {
                        pWindow->softKeyList[2] = pChar;
                        pAction->ppSoftKey = &pWindow->softKeyList[2];
                    }
                    
                    break;
                }
                default:
                {
                    return TPI_WIDGET_ERROR_UNEXPECTED;
                    break;
                }        
            }    
            
            if (widgetType == MSF_SCREEN)
            {
                if( pScreen == seekFocusedScreen())
                {
                    if( ISVALIDHANDLE( pScreen->focusedChild ) )
                    {
                        pWindow = (MsfWindow*)pScreen->children->window;
                       return widgetWindowShowSoftKey( pWindow, OP_TRUE );
                    }
                    else
                    {
                        return widgetScreenShowSoftKey( pScreen, OP_TRUE );
                    }
                }
            }
            else
            {
                if (pWindow->isFocused)
                {
                    return widgetWindowShowSoftKey( pWindow, OP_TRUE );
                }
            }
        }
    }
    else
    {
        ret = TPI_WIDGET_ERROR_BAD_HANDLE;
    }

    return ret;
}

/*=============================================================================
    FUNCTION: TPIa_widgetRegisterMsfEvent

    DESCRIPTION:
        Tells the integration that if the specified MsfEventType occurs on the 
        specified widget (that is, when the specified widget is in focus), the 
        MsfEvent will be sent to the module using the TPIc_widgetUserEvent() 
        function or callback function (indicated with cbRegEventNotify). The integration will then ignore the MsfEvent and the module 
        will handle it. Note that the MsfEvent will be sent to the module only 
        if it has not been handled anywhere else before, for example by an 
        MsfGadget currently in focus (see Section 7.4.2). This function is 
        applicable only to MsfWindow and MsfScreen widgets. If the value -1 
        is passed in theMsfEventType parameter, the integration will pass on 
        all incoming MsfEvents (of any MsfEventType) on the specified widget 
        to the module.

        If the override flag is set in the function call, it tells the integration 
        that if the specified MsfEventType occurs when the specified widget is 
        in focus, the module will handle the MsfEvent and the integration will 
        ignore it. The integration simply sends the MsfEvent to the module using 
        the TPIc_widgetUserEvent() function and then ignores the MsfEvent 
        (see Section 7.4.2). If the value -1 is passed in theMsfEventType parameter, 
        the integration will ignore all incoming MsfEvents (of any MsfEventType) on 
        the specified widget, and they will be sent to the module. A module that 
        has specified that it wants to override or handle an MsfEvent can cancel 
        this instruction for a specified MsfEvent, by setting the unsubscribe  
        parameter to 1 in the function call.

    ARGUMENTS PASSED:
        handle : The widget identifier, that is, the MsfWindow or MsfScreen identifier.
        msfEventType : The MsfEventType that the module wants to handle.
        override     : Specifies if the MsfEvent is to be overridden (1) or not (0).
        unsubscribe  : Specifies if the MsfEvent is to be unsubscribed (1) or
                       not (0). If the specified MsfEvent is already subscribed
                       to, and the unsubscribe flag is set (1), the integration
                       will cancel the subscription as a result of this call.
    RETURN VALUE:
        A positive integer if the operation was successful,otherwise the appropriate 
        return code is given.
 
    IMPORTANT NOTES:
        None
===============================================================================*/
int TPIa_widgetRegisterMsfEvent
(
    MSF_UINT32 handle, 
    int msfEventType, 
    int override, 
    int unsubscribe,
    widget_register_event_callback_t  cbRegEventNotify
)
{
    MsfWidgetType widgetType;
    void *ptr = OP_NULL;
    MsfScreen *pScreen;
    MsfWindow *pWindow;
    MSF_UINT32 regEvents;
    int ret = TPI_WIDGET_OK;
    
    ptr = seekWidget(handle, &widgetType);

    if (ptr == OP_NULL ||
        widgetType > MSF_WINDOW_END || /* not a screen or window */
        (override != 0 && override != 1) ||
        (unsubscribe != 0 && unsubscribe != 1) ||
        msfEventType < -1 || 
        msfEventType > MsfKey_Undefined) 
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    if (widgetType == MSF_SCREEN)
    {
        pScreen = (MsfScreen *)ptr;
        regEvents = pScreen->registedEvents;
        pScreen->cbRegEventNotify = cbRegEventNotify;
    }
    else /* handle is a window handle */
    {
        pWindow = (MsfWindow *)ptr;
        regEvents = pWindow->registedEvents;
        pWindow->cbRegEventNotify = cbRegEventNotify;
    }

    /* 
     * now we donot support override, so the override parameter has not
     * been taken into consideration
     */
    if (unsubscribe == 1)
    {
        if (msfEventType == -1) 
        {
            regEvents = 0x0000;
        }
        else if (msfEventType > -1 && 
                 (regEvents & (0x0001 << msfEventType)))
        {
            regEvents &= ~(0x0001 << msfEventType);
        }
        else
        {
            ret = TPI_WIDGET_ERROR_NOT_FOUND;
        }
    }
    else
    {
        if (msfEventType == -1)
        {
            regEvents = 0x7FFF;
        }
        else
        {
            regEvents |= (0x0001 << msfEventType);
        }
    }

    if (ret == TPI_WIDGET_OK)
    {
        if (widgetType == MSF_SCREEN)
        {
            pScreen->registedEvents = regEvents;
        }
        else /* handle is a window handle */
        {
            pWindow->registedEvents = regEvents;
        }
    }
    
    return ret;
}

/*=============================================================================
    FUNCTION: TPIa_widgetSetTitle

    DESCRIPTION:
        Sets the title or label of the specified widget. This function is applicable 
        only to MsfWindow and MsfGadget and MsfAction widgets.

    ARGUMENTS PASSED:
        handle : The widget identifier, that is, the MsfWindow or MsfGadget or
                 MsfAction identifier.
        title : The MsfString identifier, containing the title text

    RETURN VALUE:
        A positive integer if the operation was successful, otherwise the 
        appropriate return code.
 
    IMPORTANT NOTES:
        None
===============================================================================*/
int TPIa_widgetSetTitle (MSF_UINT32 handle, MsfStringHandle title)
{
    MsfWidgetType   widgetType;
    MsfWindow          *pWindow;
    MsfGadget         *pGadget;
    MsfAction            *pAction;
    int                    ret = TPI_WIDGET_OK;
    void                  *pWidget;
    MsfScreen           *pScreen;

    pWidget = seekWidget(handle, &widgetType);
    if (pWidget == OP_NULL )
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else if( widgetType == MSF_DIALOG || widgetType == MSF_WAITINGWIN )
    {
        return TPI_WIDGET_OK;
    }
    else if(IS_WINDOW(widgetType))
    {
        pWindow = (MsfWindow *)pWidget;
        replaceHandle((OP_UINT32 *)&pWindow->title, title);
        pWindow->propertyMask |= MSF_WINDOW_PROPERTY_TITLE;
        
        /*  adjust client area   */
        widgetWindowAdjustClientArea(pWindow);

        pScreen = pWindow->parent;
        if((pWindow->isFocused)    /* the window is in focus  */
        ||(( pScreen && pScreen->isFocused)  /*or the parent screen is focus */
            &&(widgetType == MSF_FORM)
            &&(getWidgetType( pScreen->focusedChild) == MSF_PAINTBOX))) /* or the focus window is a paintbox*/
        {
            widgetWindowDrawTitle( pWindow,  OP_NULL, OP_TRUE );
        }
    }
    else if (IS_GADGET(widgetType))
    {
        pGadget = (MsfGadget *)pWidget;

        /*
          * need to adjust selectgroup elementsPerPage    -- Add by Zhuxq    2003/05/31
          */
        if( widgetType == MSF_SELECTGROUP || widgetType == MSF_BOX)
        {
            if( ISVALIDHANDLE(pGadget->title))
            {
                /*  not need to adjust selectgroup elementsPerPage   */
            }
            else if( ISVALIDHANDLE(title) )
            {
                ((MsfSelectgroup*)pGadget)->choice.elementsPerPage --;
            }
        }
        else if( widgetType == MSF_STRINGGADGET )
        {
            MsfStringGadget   *pStringGadget = (MsfStringGadget*)pWidget;

            pStringGadget->bNeedAdjust = OP_TRUE;
        }
        
        replaceHandle((OP_UINT32 *)&pGadget->title, title);
        if( ISVALIDHANDLE(title))
        {
            pGadget->propertyMask |= MSF_GADGET_PROPERTY_LABEL;
        }
        else
        {
            pGadget->propertyMask &= ~MSF_GADGET_PROPERTY_LABEL;
        }
        
        widgetRedraw(pWidget, widgetType, handle, OP_TRUE );
    }
    else if ( widgetType == MSF_ACTION)
    {
        pAction = (MsfAction *)pWidget;
        replaceHandle((OP_UINT32 *)&pAction->label, title);
        
        /*  already added to window or screen  */
        if( pAction->ppSoftKey )
        {
            OP_UINT8    *pChar ;
            WIDGET_GET_STRING_DATA(pChar, title);
            
            *(pAction->ppSoftKey) = pChar;
            
            pScreen = seekFocusedScreen();
            widgetScreenShowSoftKey( pScreen, OP_TRUE );
        }
    }
    else
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    return ret;
}
/*=============================================================================
    FUNCTION: TPIa_widgetSetBorder

    DESCRIPTION:
        Sets a border with the specified MsfLineStyle to the specified widget. This 
        function is applicable only to MsfWindow and MsfGadget widgets.

    ARGUMENTS PASSED:
        handle : The widget identifier, that is, the MsfWindow or MsfGadget identifier.
        borderStyle : An MsfLineStyle struct, containing the properties of the border.

    RETURN VALUE:
        A positive integer if the operation was successful, otherwise the 
        appropriate return code.
 
    IMPORTANT NOTES:
        None
===============================================================================*/
int TPIa_widgetSetBorder (MSF_UINT32 handle, MsfLineStyle borderStyle)
{
    MsfWidgetType   widgetType;
    MsfWindow *pWindow;
    MsfGadget *pGadget;
    int ret = TPI_WIDGET_OK;
    void *pWidget;

    pWidget = seekWidget(handle, &widgetType);
    if (pWidget == OP_NULL)
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }   
    else if(IS_WINDOW(widgetType))
    {
        pWindow = (MsfWindow *)pWidget;
        op_memcpy(&pWindow->borderStyle, &borderStyle, sizeof(MsfLineStyle));
        pWindow->propertyMask |= MSF_WINDOW_PROPERTY_BORDER;
        
        /*  adjust client area   */
        widgetWindowAdjustClientArea(pWindow);
        widgetRedraw(pWidget, widgetType, handle, OP_TRUE );
    }
    else if (IS_GADGET(widgetType))
    {
        pGadget = (MsfGadget *)pWidget;
        op_memcpy(&pGadget->borderStyle, &borderStyle, sizeof(MsfLineStyle));
        pGadget->propertyMask |= MSF_GADGET_PROPERTY_BORDER;
        widgetRedraw(pWidget, widgetType, handle, OP_TRUE );
    }
    else
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    return ret;
}

/*=============================================================================
    FUNCTION: TPIa_widgetSetPosition

    DESCRIPTION:
        Sets the position of the specified widget to the specified MsfPosition. 
        If the position parameter is set to 0 the integration will use the 
        parameter alignment to place the widget. If the position parameter has 
        a value (!= 0) the integration will use this value to place the widget 
        and will ignore the alignment parameter. This function is applicable 
        only to MsfWindow and MsfGadget widgets.

    ARGUMENTS PASSED:
        handle : The widget identifier, that is, the MsfWindow or MsfGadget 
                 identifier.
        position  : The MsfPosition of the widget.
        alignment : The alignment of the widget.

    RETURN VALUE:
        A positive integer if the operation was successful,otherwise the 
        appropriate return code is given.
 
    IMPORTANT NOTES:
        None
===============================================================================*/
int TPIa_widgetSetPosition(MSF_UINT32 handle, MsfPosition* position, MsfAlignment* alignment)
{
    MsfWidgetType   widgetType;
    MsfWindow *pWindow;
    MsfGadget *pGadget;
    int ret = TPI_WIDGET_OK;
    void *pWidget;

    pWidget = seekWidget(handle, &widgetType);
    if (pWidget == OP_NULL)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }   

    if(IS_WINDOW(widgetType))
    {
        pWindow = (MsfWindow *)pWidget;
        if (position != OP_NULL)
        {
            //pWindow->position = *position;
            op_memcpy(&pWindow->position, position, sizeof(MsfPosition));
        }
        else if (alignment != OP_NULL)
        {
            //pWindow->alignment = *alignment;
            op_memcpy(&pWindow->alignment, alignment, sizeof(MsfAlignment));
        }
        
        widgetRedraw(pWidget, widgetType, handle, OP_TRUE );
    }
    else if (IS_GADGET(widgetType))
    {
        MsfWidgetType   wt;
        MsfWindow *pWindow;
        pGadget = (MsfGadget *)pWidget;
        pGadget->alignment.horisontalPos = MsfHorizontalPosDefault;
        pGadget->alignment.verticalPos = MsfVerticalPosDefault;
        pWindow= (MsfWindow *)seekWidget(pGadget->parent->windowHandle,&wt);      
        if (position != OP_NULL)
        {
            //pGadget->position = *position;
            op_memcpy(&pGadget->position, position, sizeof(MsfPosition));

            if( pGadget->parent != OP_NULL )
            {
                if( wt == MSF_PAINTBOX )
                {
                    widgetRedraw(pWidget, widgetType, handle, OP_TRUE );
                }
            }
        }
        else if (alignment != OP_NULL)
        {
            //pGadget->alignment = *alignment;
            op_memcpy(&pGadget->alignment, alignment, sizeof(MsfAlignment));
        }
        if(MSF_FORM == wt)
        {
                
            MsfForm *pForm;
            pForm = (MsfForm *)pWindow;
            widgetFormAdjustScrollbar(pForm);
            widgetRedraw((void*)pWindow, wt, pWindow->windowHandle, OP_TRUE );
            
        }
    }
    else
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    return ret;
}
/*=============================================================================
    FUNCTION: TPIa_widgetSetSize 

    DESCRIPTION:
        Sets the full size of the specified widget to the specified MsfSize. 
        This function is applicable only to MsfWindow and MsfGadget widgets.

    ARGUMENTS PASSED:
        handle : The widget identifier, that is, the MsfWindow or MsfGadget
                 identifier.
        size   : The full MsfSize of the MsfWindow or MsfGadget.

    RETURN VALUE:
        A positive integer if the operation was successful,otherwise the 
        appropriate return code is given.
 
    IMPORTANT NOTES:
        None
===============================================================================*/
int TPIa_widgetSetSize (MSF_UINT32 handle, MsfSize* size)
{
    MsfWidgetType   widgetType;
    MsfWindow *pWindow;
    MsfGadget *pGadget;
    int ret = TPI_WIDGET_OK;
    void *pWidget;

    pWidget = seekWidget(handle, &widgetType);
    if (pWidget == OP_NULL || size == OP_NULL)
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }     
    else if(IS_WINDOW(widgetType))
    {
        pWindow = (MsfWindow *)pWidget;
        op_memcpy(&pWindow->size, size, sizeof(MsfSize));
        widgetRedraw(pWidget, widgetType, handle, OP_TRUE);
    }
    else if (IS_GADGET(widgetType))
    {
        pGadget = (MsfGadget *)pWidget;
        op_memcpy(&pGadget->size, size, sizeof(MsfSize));

        if( widgetType == MSF_SELECTGROUP || widgetType == MSF_BOX )
        {
            MsfSelectgroup    *pChoice = (MsfSelectgroup*)pWidget;
            pChoice->bItemPosCalculated = OP_FALSE;
        }
        else if( widgetType == MSF_STRINGGADGET )
        {
            MsfStringGadget   *pStringGadget = (MsfStringGadget*)pWidget;

            pStringGadget->bNeedAdjust = OP_TRUE;
        }
        else if( widgetType == MSF_BAR )
        {
            MsfBar *pBar = (MsfBar*)pWidget;
            /*  If the related widget of scrollbar is null, the scrollbar must be in the paintbox,
              *  so, adjust the page step
              */
            if( pBar->hRelatedWidget == INVALID_HANDLE )
            {    
                if( pBar->barType == MsfVerticalScrollBar )
                {
                    pBar->pageStep = pGadget->size.height - pBar->lineStep;
                }
                else if( pBar->barType == MsfHorizontalScrollBar )
                {
                    pBar->pageStep = pGadget->size.width- pBar->lineStep;
                }
            }
        }
        widgetRedraw(pWidget, widgetType, handle, OP_TRUE);
    }
    else
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    return ret;
}

/*=============================================================================
    FUNCTION: TPIa_widgetGetPosition 

    DESCRIPTION:
        Returns the MsfPosition of the specified widget. This function is applicable
        only to MsfWindow and MsfGadget widgets.

    ARGUMENTS PASSED:
        handle : The widget identifier, that is, the MsfWindow or MsfGadget
                     identifier.
        position : Pointer to an empty MsfPosition struct, where the MsfPosition of
                      the MsfWindow or MsfGadget is returned.

    RETURN VALUE:
        A positive integer if the operation was successful,otherwise the 
        appropriate return code is given.
 
    IMPORTANT NOTES:
        None
===============================================================================*/
int TPIa_widgetGetPosition (MSF_UINT32 handle, MsfPosition* position)
{
    MsfWidgetType   widgetType;
    MsfWindow *pWindow;
    MsfGadget *pGadget;
    int ret = TPI_WIDGET_OK;
    void *pWidget;

    if( position == OP_NULL )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    pWidget = seekWidget(handle, &widgetType);
    if (pWidget == OP_NULL)
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }     
    else  if(IS_WINDOW(widgetType))
    {
        pWindow = (MsfWindow *)pWidget;
        op_memcpy(position, &pWindow->position, sizeof(MsfPosition));
    }
    else if (IS_GADGET(widgetType))
    {
        pGadget = (MsfGadget *)pWidget;
        widgetGadgetGetPosition( pGadget, &position->x, &position->y);
     }
    else
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    return ret;
}

/*=============================================================================
    FUNCTION: TPIa_widgetGetSize 

    DESCRIPTION:
        Gets the full MsfSize of the specified widget. This function is applicable 
        to MsfWindow and MsfGadget widgets and also to MsfImage, MsfIcon and 
        MsfString widgets.

    ARGUMENTS PASSED:
        handle : The widget identifier, that is, the MsfWindow or MsfGadget identifier.
        size     : Pointer to an empty MsfSize struct, where the full MsfSize of the
                     MsfWindow or MsfGadget is returned

    RETURN VALUE:
        A positive integer if the operation was successful,otherwise the 
        appropriate return code is given.
 
    IMPORTANT NOTES:
        None
===============================================================================*/
int TPIa_widgetGetSize (MSF_UINT32 handle, MsfSize* size)
{
   MsfWidgetType   widgetType;
    MsfWindow *pWindow;
    MsfImage   *pImage;
    int ret = TPI_WIDGET_OK;
    void *pWidget;

    if (size == OP_NULL)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    pWidget = seekWidget(handle, &widgetType);
    if (pWidget == OP_NULL)
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }    
    else if(IS_WINDOW(widgetType))
    {
        pWindow = (MsfWindow *)pWidget;
        op_memcpy(size, &pWindow->size, sizeof(MsfSize));
    }
    else if (IS_GADGET(widgetType))
    {
        widgetGadgetGetSize((MsfGadget *)pWidget, size);
    }
    else if (widgetType == MSF_IMAGE)
    {
        /* get image structure */
        pImage = (MsfImage*)pWidget;
        
        if(pImage->bOriginal == OP_TRUE )
        {
            if( pImage->data == OP_NULL || pImage->moreData )
            {
                op_memset(size, 0, sizeof(MsfSize));
                return TPI_WIDGET_ERROR_INVALID_PARAM;
            }

            if( pImage->size.height <= 0 || pImage->size.width <= 0 )
            {
                if( OP_FALSE == ds_get_file_image_size(convertImageFormat(pImage->format),
                                       pImage->data,
                                       pImage->dataSize,
                                       (OP_UINT16 *)&(pImage->size.width),
                                       (OP_UINT16 *)&(pImage->size.height)))
                {                       
                    op_memset( &pImage->size, 0, sizeof(MsfSize));
                    *size = DefaultPropTab[MSF_IMAGE].size;
                    return TPI_WIDGET_ERROR_UNEXPECTED;
                }    
            }                       
        }
        
        if( IsValidImageRegion(pImage->size.width, pImage->size.height) )
        {
            op_memcpy(size, &pImage->size, sizeof(MsfSize));
        }
        else
        {
            *size =  DefaultPropTab[MSF_IMAGE].size;
        }
    }
    else if (widgetType == MSF_ICON || widgetType == MSF_STRING)
    {
        size->height = 0;
        size->width = 0;
    }
    else
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    return ret;
}

/*=============================================================================
    FUNCTION: TPIa_widgetAddAnimation 

    DESCRIPTION:
        Attaches the specified animation widget (MsfMove, MsfRotation, MsfColorAnim) 
        to the specified MsfGadget or MsfWindow. This function is applicable only
        to MsfWindow and MsfGadget widgets. The animation widget should start to 
        execute without any delay. As soon as the animation is finished the animation 
        widget is removed (not released) from the widget it was attached to.

    ARGUMENTS PASSED:
        handle : The MsfGadget or MsfWindow identifier.
        animation : The animation widget identifier, that is, the identifier of the
                    MsfMove, MsfRotation, or MsfColorAnim.

    RETURN VALUE:
        A positive integer if the operation was successful,otherwise the 
        appropriate return code is given.
 
    IMPORTANT NOTES:
        None
===============================================================================*/
int TPIa_widgetAddAnimation(MSF_UINT32 handle, MSF_UINT32 animation)
{
    // TBD
return 0;
}

/*=============================================================================
    FUNCTION: TPIa_widgetSetDecoration

    DESCRIPTION:
        Sets the specified MsfDecoration on the specified widget. This function 
        is applicable only to MsfWindow and MsfGadget widgets. The MsfDecoration 
        should start to execute without any delay.

    ARGUMENTS PASSED:
        handle : The MsfGadget or MsfWindow identifier.
        decoration : Specifies the MsfDecoration to be set.

    RETURN VALUE:
        A positive integer if the operation was successful,otherwise the 
        appropriate return code is given.
 
    IMPORTANT NOTES:
        None
===============================================================================*/
int TPIa_widgetSetDecoration(MSF_UINT32 handle, MsfDecoration decoration)
{
    // TBD
return 0;
}

/*=============================================================================
    FUNCTION: TPIa_widgetMsfEvent2Utf8

    DESCRIPTION:
        Retrieves the character that corresponds to a certain MsfEvent. The 
        returned value must be a character in UTF8-format that is not 
        OP_NULL-terminated. This function is typically used by a module that wants 
        to implement some sort of text input using an MsfPaintbox.

    ARGUMENTS PASSED:
        event : The MsfEvent.
        multitap : The number of consecutive multitap MsfEvents that has
                   occurred. For example, if the module receives the MsfEvent 
                   of type MsfKey_5 three times in a row and the last two has 
                   the MSF_EVENT_MODE_MULTITAP flag set, the module can call
                   this function with the multitap parameter set to 2. Such a 
                   call could for example return the character ¡°L¡±, depending 
                   on the integration. Note, that if the consecutive MsfEvents 
                   has differentMsfEventClasses, the multitap parameter should 
                   only be increased for MsfEvents of the same MsfEventClass.

        buffer : Pointer to a buffer in which the corresponding character in UTF8
                 format is returned. Note that the module must allocate a buffer 
                 at least 4 bytes. The integration will then use this buffer to 
                 returnof the character in UTF8 format. The returned character 
                 must not be OP_NULL-terminated.

    RETURN VALUE:
        A positive integer if the operation was successful,otherwise the 
        appropriate return code is given.
 
    IMPORTANT NOTES:
        None
===============================================================================*/
int TPIa_widgetMsfEvent2Utf8(MsfEvent *event, int multitap, char* buffer){return 0;}



/*=============================================================================
    FUNCTION: TPIa_widgetStartTimer

    DESCRIPTION:
        delay       : dleay time before start timer
        bPeriodic    : Periodic or One-time timer
        hWidget   : The timer event handler widget
        fnOnTimerCallback : The timer event handle callback function

    ARGUMENTS PASSED:

    RETURN VALUE:
        The timer id, if 0, there is no timer to assign.
 
    IMPORTANT NOTES:
        None
===============================================================================*/
MSF_UINT32 TPIa_widgetStartTimer( MSF_INT32 delay, MSF_BOOL bPeriodic, MSF_UINT32 hWidget, widget_timer_callback_t fnOnTimerCallback)
{
    OP_UINT32          iNewTimerId;
    RegisteredTimer    *pInsertPos = OP_NULL;
    RegisteredTimer    *pNext;
    RegisteredTimer    *pNewTimer;
    
    /* 
      *  1. find out the appropriate timer id, and inserted position
      */
    /* the timer list is empty, use the first timer id  */
    if( timerList == OP_NULL )
    {
        iNewTimerId = WIDGET_TIMER_START;
    }
    else if( timerList->timerId <WIDGET_TIMER_END )
    {
        iNewTimerId = timerList->timerId + 1;
    }
    else
    {
        pInsertPos = timerList;
        if( timerList->pLink == OP_NULL )  /* now, only one timer  */
        {
            iNewTimerId = WIDGET_TIMER_START;
        }
        else
        {
            iNewTimerId = WIDGET_TIMER_INVALID_ID;
            pNext = pInsertPos->pLink;
            while (pNext)
            {
                if( pNext->timerId + 1 < pInsertPos->timerId )
                {
                    iNewTimerId = pNext->timerId + 1;
                    break;
                }
                
                pInsertPos = pNext;
                pNext = pNext->pLink;
            }

            /* there is no available timer */
            if( iNewTimerId == WIDGET_TIMER_INVALID_ID )
            {
                return WIDGET_TIMER_INVALID_ID;
            }
        } 
    }

    /*
      * 2. add a new timer node into the timer list
      */
    pNewTimer = WIDGET_ALLOC(sizeof(RegisteredTimer));
    if( !pNewTimer )
    {
        return WIDGET_TIMER_INVALID_ID;
    }

    pNewTimer->timerId = iNewTimerId;
    if( bPeriodic == OP_FALSE )
    {
        pNewTimer->timerType = ONE_SHOT;;
    }
    else
    {
        pNewTimer->timerType = PERIODIC;
    }
    
    pNewTimer->hWidget = hWidget;
    pNewTimer->fnOnTimerCallback = fnOnTimerCallback;

    /* need to insert into the first position of the timer list  */
    if( !pInsertPos )
    {
        pNewTimer->pLink = timerList;
        timerList = pNewTimer;
    }
    else  /* insert into the middle of the timer list */
    {
        pNewTimer->pLink = pInsertPos->pLink;
        pInsertPos->pLink = pNewTimer;
    }

    /* timer count increase one  */
    nTimerCount ++;

    /*  start the opus timer  */
    OPUS_Start_Timer( (OPUS_EVENT_ENUM_TYPE)iNewTimerId, delay, 0, pNewTimer->timerType);
    
    return iNewTimerId;
}

/*=============================================================================
    FUNCTION: TPIa_widgetStopTimer

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:
        A positive integer if the operation was successful,otherwise the 
        appropriate return code is given.
 
    IMPORTANT NOTES:
        None
===============================================================================*/
int TPIa_widgetStopTimer( MSF_UINT32 hWidget, MSF_UINT32 iTimerId)
{
    RegisteredTimer    *pDeleteNode;
    RegisteredTimer    *pPreNode;

    if(! IS_WIDGET_TIMER_ID( iTimerId ) )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    if( nTimerCount == 0 || timerList == OP_NULL || timerList->timerId < iTimerId )
    {
        return TPI_WIDGET_OK;
    }

    pDeleteNode = OP_NULL;
    
    /*  find out the timer node in the tiemr list according to the timerId, and delete the node  */
    if( timerList->timerId == iTimerId )
    {
        if( timerList->hWidget == hWidget )
        {
            pDeleteNode = timerList;
            timerList = timerList->pLink;
            nTimerCount --;
        }
    }
    else
    {
        pPreNode = timerList;
        pDeleteNode = pPreNode->pLink;
        while(pDeleteNode)
        {
            if( pDeleteNode->timerId == iTimerId )
            {
                if( pDeleteNode->hWidget == hWidget )
                {
                    pPreNode->pLink = pDeleteNode->pLink;
                    nTimerCount --;
                }
                else
                {
                    pDeleteNode = OP_NULL;
                }
                break;
            }

            pPreNode = pDeleteNode;
            pDeleteNode = pDeleteNode->pLink;
        }
    }

    if( pDeleteNode )
    {
        /*  stop the opus timer  */
        OPUS_Stop_Timer( (OPUS_EVENT_ENUM_TYPE) iTimerId);
        WIDGET_FREE(pDeleteNode);
    }    
    
    return TPI_WIDGET_OK;
}

/*=============================================================================
    FUNCTION: widgetHandleTimer

    DESCRIPTION:

    ARGUMENTS PASSED:

    RETURN VALUE:
        A positive integer if the operation was successful,otherwise the 
        appropriate return code is given.
 
    IMPORTANT NOTES:
        None
===============================================================================*/
int widgetHandleTimer( OPUS_EVENT_ENUM_TYPE   event, OP_BOOLEAN   *handle)
{
    OP_UINT32          timerId;
    OP_UINT32          hWidget;
    RegisteredTimer    *pPreNode = OP_NULL;
    RegisteredTimer    *pTimerNode;
    widget_timer_callback_t  fnOnTimerCallback;
    
    /* not in dynamically alloc timer event  */
    if( !IS_WIDGET_TIMER_ID( event ) )
    {
        return TPI_WIDGET_OK;
    }

    *handle = OP_TRUE;
    timerId = (OP_UINT32) event;
    
    /* the timer not in the widget timer list */
    if( nTimerCount == 0 || timerList == OP_NULL || timerList->timerId < timerId )
    {
        /* there are some error in program, so stop the timer  */
        OPUS_Stop_Timer( event );
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    /* find out the timer node, and mark the pre timer node */
    if( timerList->timerId == timerId )
    {
        pTimerNode = timerList;
    }
    else
    {
        pPreNode = timerList;
        pTimerNode = timerList->pLink;

        while( pTimerNode )
        {
            if( pTimerNode->timerId == timerId)
            {
                break;
            }
            /* the timer is not in the widget timer list */
            else if( pTimerNode->timerId < timerId )
            {
                pTimerNode = OP_NULL;
                break;
            }
            
            pPreNode = pTimerNode;
            pTimerNode = pTimerNode->pLink;
        }
    }

    /* not found, i.e. the timer is not in the widget timer list */
    if( !pTimerNode )
    {
        OPUS_Stop_Timer(event);
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    fnOnTimerCallback = pTimerNode->fnOnTimerCallback;
    hWidget = pTimerNode->hWidget;

    /* if the timer is one shot, delete the timer node */
    if( pTimerNode->timerType == ONE_SHOT )
    {
        if( pTimerNode == timerList )
        {
            timerList = timerList->pLink;
        }
        else if( pPreNode )
        {
            pPreNode->pLink = pTimerNode->pLink;
        }

        WIDGET_FREE(pTimerNode);
        nTimerCount --;
    }

    /*  notify the widget of timer event  */
    (void)fnOnTimerCallback( hWidget, timerId );
    return TPI_WIDGET_OK;
}

/*=============================================================================
    FUNCTION: TPIa_widgetShowWaitingFlag

    DESCRIPTION:
    It will show a waiting flag in the center of the screen.

    ARGUMENTS PASSED:
    bClearAll   if 0, this function will only draw a waiting icon at the center of the screen;
                or it will fill all the screen with white color first and then draw a waiting icon 
                at the center of the screen
                
    RETURN VALUE:
 
    IMPORTANT NOTES:
        None
===============================================================================*/
void TPIa_widgetShowWaitingFlag( int bClearAll )
{
    RM_ICON_T *  pWaitingIcon;
    OP_INT16     x, y;
    OP_INT16     w,h;

    if( bClearAll )
    {
        ds_fill_rect_forward( 0, 0, (OP_INT16)(DS_SCRN_MAX_X - 1), (OP_INT16)(WAP_SCRN_TEXT_HIGH - 1), 0xFFFF);
        ds_set_softkeys_char(OP_NULL, OP_NULL, OP_NULL);
    }

    pWaitingIcon  = util_get_icon_from_res( ICON_WAITING );

    w = pWaitingIcon->biWidth;
    h = pWaitingIcon->biHeight;
    x = ( DS_SCRN_MAX_X - w ) >> 1;
    y = ( WAP_SCRN_TEXT_HIGH - h ) >> 1;

    ds_draw_partial_icon( x, y, w, h, 0, 0, pWaitingIcon);
        
    WAP_REFRESH
}


int TPIa_widgetTextInputDefaultStateChangeCb( MSF_UINT8 modId, MSF_UINT32 hTextInput, MsfNotificationType notType, void *pData )
{
    MsfTextInput          *pTextInput;
    MsfWidgetType    wt;
    MsfGadget          *pGadget;
    MsfEditor             *pEditor;
    MsfWindowHandle hEditor;
    MsfStringHandle   hInputString;
    MsfActionHandle  hActionOk;
    MsfActionHandle  hActionCancel;
    MsfActionHandle  hActionEmpty;
    MsfPosition         position={ 0, 0 };
    int                    iRet;

    pGadget = (MsfGadget*)seekWidget(hTextInput, &wt);
    if( !pGadget || wt != MSF_TEXTINPUT )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( pGadget->isFocused == OP_FALSE
        || getWidgetType(pGadget->parent->windowHandle) == MSF_EDITOR 
        || notType != MsfNotifyClick )
    {
        return TPI_WIDGET_OK;
    }

    pTextInput = (MsfTextInput*)pGadget;

    /* The related MsfEditor existed, there must be some error in program  */
    if( ISVALIDHANDLE(pTextInput->hEditor ) )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    hEditor = INVALID_HANDLE;
    hInputString = INVALID_HANDLE;
    hActionOk = INVALID_HANDLE;
    hActionCancel = INVALID_HANDLE;
    hActionEmpty = INVALID_HANDLE;
    iRet = TPI_WIDGET_OK;

    /* get the initial inputstring  */
    hInputString = TPIa_widgetStringCreateText(modId, pGadget->gadgetHandle, 0);
    if( !ISVALIDHANDLE(hInputString))
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    /* create the MsfEditor, and show it  */
    hEditor = TPIa_widgetEditorCreateEx( modId, 
                                                          INVALID_HANDLE,
                                                          hInputString,
                                                          pTextInput->input.textType,
                                                          pTextInput->input.txtInputBufMng.maxCharCount,
                                                          pTextInput->input.singleLine,
                                                          OP_NULL,
                                                          MSF_WINDOW_PROPERTY_SINGLEACTION |MSF_WINDOW_PROPERTY_TITLE, 
                                                          0,
                                                          widgetRelatedEditorOnStateChange);
    if( !ISVALIDHANDLE(hEditor))
    {
        iRet = TPI_WIDGET_ERROR_RESOURCE_LIMIT;
        goto CREATE_RELATED_EDITOR_END;
    }

    /*  create ok action  */
    hActionOk = TPIa_widgetActionCreateEx( modId,
                                                          TPIa_widgetStringGetPredefined(MSF_STR_ID_DLG_ACTION_OK),
                                                          MsfOk,
                                                          1,
                                                          0x8000,
                                                          widgetRelatedEditorOnOk);
    if( !ISVALIDHANDLE(hActionOk) )
    {
        iRet = TPI_WIDGET_ERROR_RESOURCE_LIMIT;
        goto CREATE_RELATED_EDITOR_END;
    }

    /* add to editor  */
    if( 0 > TPIa_widgetAddAction( hEditor, hActionOk) )
    {
        iRet = TPI_WIDGET_ERROR_UNEXPECTED;
        goto CREATE_RELATED_EDITOR_END;
    }
    
    /*  create cancel action  */
    hActionCancel= TPIa_widgetActionCreateEx( modId,
                                                          TPIa_widgetStringGetPredefined(MSF_STR_ID_DLG_ACTION_BACK),
                                                          MsfCancel,
                                                          1,
                                                          0x8000,
                                                          widgetRelatedEditorOnCancel);
    if( !ISVALIDHANDLE(hActionCancel) )
    {
        iRet = TPI_WIDGET_ERROR_RESOURCE_LIMIT;
        goto CREATE_RELATED_EDITOR_END;
    }

    /* add to editor  */
    if( 0 > TPIa_widgetAddAction( hEditor, hActionCancel) )
    {
        iRet = TPI_WIDGET_ERROR_UNEXPECTED;
        goto CREATE_RELATED_EDITOR_END;
    }
    
    /*  create empty action to disable left soft key  */
    hActionEmpty= TPIa_widgetActionCreateEx( modId,
                                                          TPIa_widgetStringGetPredefined(MSF_STR_ID_EMPTY_STRING),
                                                          MsfMenu,
                                                          1,
                                                          0x8000,
                                                          widgetRelatedEditorOnLeftAction);
    if( !ISVALIDHANDLE(hActionEmpty) )
    {
        iRet = TPI_WIDGET_ERROR_RESOURCE_LIMIT;
        goto CREATE_RELATED_EDITOR_END;
    }

    /* add to editor  */
    if( 0 > TPIa_widgetAddAction( hEditor, hActionEmpty) )
    {
        iRet = TPI_WIDGET_ERROR_UNEXPECTED;
        goto CREATE_RELATED_EDITOR_END;
    }
    
    /* add to screen */
    if( 0 > TPIa_widgetScreenAddWindow( pGadget->parent->parent->screenHandle,
                                                    hEditor,
                                                    &position,
                                                    INVALID_HANDLE,
                                                    INVALID_HANDLE))
    {
        iRet = TPI_WIDGET_ERROR_RESOURCE_LIMIT;
        goto CREATE_RELATED_EDITOR_END;
    }

    /* show the editor window  */
    if( 0 > TPIa_widgetSetInFocus( hEditor, 1))
    {
        TPIa_widgetRemove( pGadget->parent->parent->screenHandle, hEditor);
        iRet = TPI_WIDGET_ERROR_UNEXPECTED;
        goto CREATE_RELATED_EDITOR_END;
    }

    pTextInput->hEditor = hEditor;
    pEditor = (MsfEditor*)seekWidget( hEditor, OP_NULL );
    pEditor->hRelatedTextInput = pGadget->gadgetHandle;

CREATE_RELATED_EDITOR_END:
    if( ISVALIDHANDLE(hInputString) )
    {
        TPIa_widgetRelease( hInputString );
    }
    if( ISVALIDHANDLE(hEditor) )
    {
        TPIa_widgetRelease( hEditor );
    }
    if( ISVALIDHANDLE(hActionOk) )
    {
        TPIa_widgetRelease( hActionOk );
    }
    if( ISVALIDHANDLE(hActionCancel) )
    {
        TPIa_widgetRelease( hActionCancel );
    }
    if( ISVALIDHANDLE(hActionEmpty) )
    {
        TPIa_widgetRelease( hActionEmpty );
    }
    
    return iRet;
}

