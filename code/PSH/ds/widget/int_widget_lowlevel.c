/*==================================================================================================

    MODULE NAME : int_widget_lowlevel.c

    GENERAL DESCRIPTION

    SEF Telecom Confidential Proprietary
    (c) Copyright 2002 by SEF Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    04/03/2003   XXXXX            Cxxxxxx     Initial file creation.
    7/30/2003    Zhuxq            P001026     Modify the member visit because of the MsfEidtor struction's modification
    07/26/2003   lindawang        P001074     Fix draw image problems.
    08/06/2003   Steven Lai       P001140     Rearrange the .h files
    08/07/2003   YuanHui          P001169     add sound interface for widget.
    08/13/2003   Zhuxq            P001147     Change the selectgroup show and menu show
    8/11/2003    Chenjs           P001091     Update text input widget.
    08/14/2003   lindawang        P001254     Fix draw image problem for wap.
    08/20/2003   Zhuxq            P001279     modify the function of TPIa_widgetStringGetVisible and TPIa_widgetStringGetVisible
    08/25/2003   Zhuxq            P001355     Fixed some warning according to PC-Lint check result
    08/28/2003   Tree Zhan        P001404     Modify low level widget about  create-image and create-sound
    08/25/2003   Zhuxq            P001417     Fixed the bug:in some case the textinput in a wap page can not input
    08/29/2003   Zhuxq            P001429     Change the TPIa_widgetStringCreate, calc stringdata len using strlen
    09/02/2003   Zhuxq            P001477     Fix the bug: when opening a wap page( state 0/1), a call incomes, and hand up it, the MissedCall window will shows incorrect
    09/04/2003   Zhuxq            P001499     Fix the  bug: the view msg window shows error code
    09/25/2003   Zhuxq            P001696     Re-implement the dialog
    09/25/2003   Steven Lai       C001579     Add the feature to handle attachment in mms
    09/26/2003   linda wang       P001451     separate the gif play and show function.    
    12/06/2003   Zhuxq            P002156     Fix the bug that there are some mess text in some wap page
    12/12/2003   Zhuxq            P002192     fix bug for wap memory leak
    12/31/2003   Zhuxq            P002296     Avoid the MsfStyle's memory leak
    01/05/2004   Zhuxq            C000072     Applied the TPIa_widgetStringGetSubSize and TPIa_widgetStringGetVisible to form and other window
    01/16/2004   Zhuxq            P002369     Adjust the wap page layout
    02/17/2004   Zhuxq            P002492     Add Viewer window to widget system
    03/19/2004   Zhuxq            P002687     Fix some bugs in widget system
    03/19/2004   Chenxiao         p002688     change main menu  of wap and fix some bugs from NEC feedback   
    04/02/2004   zhuxq            P002789     Fix UMB bugs
    04/14/2004   Chenxiao         p002916     fix some bugs from NEC's response    
    04/15/2004   zhuxq            P002977     Fix some bugs in UMB for Dolphin
    04/14/2004   Chenxiao         p002916     fix some bugs from NEC's response    
    04/24/2004   zhuxq            P003022     Fix some bugs in UMB
    04/29/2004   zhuxq            P005196     Fix some bugs in UMB
    06/04/2004   zhuxq            P005925     Correct issues found by PC-lint and fix some bugs
    06/12/2004   majingtao        P006119     fix bugs about incomplete dialog information
    06/18/2004   zhuxq            P006260     Make up WAP&UMB code to cnxt_int_wap branch
    07/20/2004   zhuxq            P007108     Add playing GIF animation in some wap page with multi-frame GIF
    08/09/2004   Hugh zhang       p007607     Change font of widget dialog in lotus project.
    08/13/2004   zhuxq            P007751     Fix the bug that TextInput in WAP page can't be inputed after pressed OK
    08/18/2004   zhuxq            P007793     Trace widget memory issues, and reduce allocation times of MsfString
    08/30/2004   zhuxq            c007998     improve the implementation of predefined string to reduce the times of memory allocation
    09/17/2004   zhuxq            P008393     Fix the bug that audio plays only once in MMS
    
    Self-documenting Code
    Describe/explain low-level design of this module and/or group of funtions and/or specific
    funtion that are hard to understand by reading code and thus requires detail description.
    Free format !
        
====================================================================================================
    INCLUDE FILES
==================================================================================================*/

#ifdef WIN32
#include "windows.h"
#include "portab_new.h"
#else
#include <string.h>
#include "wcdtypes.h"
#include "portab.h"
#endif
#include "rm_include.h"
#include "SP_sysutils.h"
#include "ds.h"

#include "utility_string_func.h"
#include "SP_list.h"
#include "SP_audio.h"
#include "SP_volume.h"
#include "msf_int.h"
#include "msf_file.h"

#include "msf_int_widget.h"
#include "int_widget_common.h"
#include "int_widget_txt_input.h"

#include "int_widget_custom.h"
#include "int_widget_remove.h"

#include "int_pipe.h"
#include "rma_pipe.h"
#include "msf_pipe.h"

//#define  DEBUG_SHOW
#ifdef   DEBUG_SHOW
#define SHOW    ds_refresh();
#else
#define SHOW    
#endif




typedef struct
{
    /*
     * A positive integer if the operation was successful,
     *  otherwise the appropriate return code.
     */
    MsfWidgetType  widgetType;
    MSF_BOOL       bIsHoldDraw;
    /* If the widget is PaintBox */
    //MsfWindow      *pWindow;
    MsfPaintbox    *pPaintbox;
    /* If the widget is Image */
    MsfImage       *pImage;
}PAINTBOX_IMAGEGADGET_STRUCT_NAME_T;

/****************************************************************
 LOW LEVEL
 ***************************************************************/

extern  int widgetDrawStringByChar
(
    MsfWindow *pWin, 
    MsfWidgetDrawString     *pDrawString
);

extern int  widgetStringGetVisibleByChar
(
    const OP_UINT8 *ustr,
    MsfSize               *size,
    OP_INT16            iEngCharWidth,
    OP_BOOLEAN      bIncludeInitialWhiteSpaces, 
    int                       *nbrOfCharacters, 
    int                       *nbrOfInitialWhiteSpaces
);

int  widgetStringGetVisibleByWord
(
    const OP_UINT8 *ustr,
    MsfSize               *size,
    int                       iEngCharWidth,
    OP_BOOLEAN     bIncludeInitialWhiteSpaces, 
    int                      *nbrOfCharacters, 
    int                      *nbrOfEndingWhiteSpaces,
    int                      *nbrOfInitialWhiteSpaces
);

int  widgetStringGetSubSize
(
    const OP_UINT8 *ustr,
    OP_INT16            iEngCharWidth,
    int                        nbrOfChars, 
    int                        subwidth, 
    MsfSize                *pSize
);

int getRelatedStyle
(
    MSF_UINT32     msfHandle,
    MsfWidgetType  *pWidgetType,
    MsfStyle       **ppStyle,
    MsfStyleHandle  **ppHandle
);

RM_RESOURCE_ID_T widgetGetIconResID
(
    MsfIconType    MsfIconType
);

extern OP_UINT8*  widgetGetPredefinedStringEx(OP_UINT8 modId, OP_UINT8 lang, OP_UINT16 index);

/*==================================================================================================
    FUNCTION: get_Paintbox_Image_Info

    DESCRIPTION:
        Tells the integration that several consecutive calls to drawing functions for
        the specified MsfImage/MsfPaintbox will be made. This call tells the 
        integration to hold the actual execution of these calls until the 
        TPIa_widgetPerformDraw() is called. This function is only applicable to 
        MsfPaintbox and MsfImage widgets.

    ARGUMENTS PASSED:
        msfHandle: The identifier of the MsfPaintbox or MsfImage.
        
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise the appropriate
        return code.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int get_Paintbox_Image_Info
(
    MSF_UINT32                          msfHandle,
    PAINTBOX_IMAGEGADGET_STRUCT_NAME_T  *pResult
)
{
    void  *ptr = OP_NULL;
    int   retCode = TPI_WIDGET_ERROR_BAD_HANDLE;

    if(pResult == OP_NULL)
    {
        retCode = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        
        op_memset(pResult, 0x0, sizeof(PAINTBOX_IMAGEGADGET_STRUCT_NAME_T));

        
        /*
         * get the widget type, it should be MSF_PAINTBOX or MSF_IMAGEGADGET
         */
        ptr = seekWidget( msfHandle, &pResult->widgetType);

        if(OP_NULL == ptr)
        {
            retCode = TPI_WIDGET_ERROR_UNEXPECTED;
        }
        else if(pResult->widgetType == MSF_PAINTBOX) 
        {
            pResult->pPaintbox = (MsfPaintbox*)ptr;
            pResult->bIsHoldDraw = pResult->pPaintbox->isHoldDraw;
            retCode = TPI_WIDGET_OK;
        }
        else if(pResult->widgetType == MSF_IMAGE)
        {
            pResult->pImage = (MsfImage *)ptr;
            pResult->bIsHoldDraw = pResult->pImage->isHoldDraw;
            retCode = TPI_WIDGET_OK;
        }
    }
    
    return retCode;
    
}


int getRelatedStyle
(
    MSF_UINT32  msfHandle,
    MsfWidgetType  *pWidgetType,
    MsfStyle       **ppStyle,
    MsfStyleHandle  **ppHandle
)
{
    int              iRet;
    void           *ptr = OP_NULL;
    MsfWidgetType  styleType;
    MsfWidgetType  wt;
    MsfStyleHandle  hStyle;

    if( ppStyle == OP_NULL )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    hStyle = INVALID_HANDLE;
    
    *ppStyle = OP_NULL;
    if( ppHandle != OP_NULL )
    {
        *ppHandle = OP_NULL;
    }

    iRet = TPI_WIDGET_OK;
    ptr = seekWidget(msfHandle, &wt);
    if(ptr != OP_NULL)
    {
        *pWidgetType = wt;
        
        if( IS_SCREEN(wt ) )
        {
            MsfScreen  *pScreen = (MsfScreen *)ptr;
            hStyle = pScreen->style;
            if( ppHandle )
            {
                *ppHandle = &pScreen->style;
            }
        }
        else if( IS_WINDOW( wt ) )
        {
            MsfWindow   *pWindow = (MsfWindow *)ptr;
            hStyle = pWindow->style;
            if( ppHandle )
            {
                *ppHandle = &pWindow->style;
            }
        }
        else if( IS_GADGET(wt) )
        {
            MsfGadget  *pGadget = (MsfGadget *)ptr;
            hStyle = pGadget->style;
            if( ppHandle )
            {
                *ppHandle = &pGadget->style;
            }
        }
        else if( IS_LOWLEVEL( wt ) )
        {
            if( wt == MSF_STYLE )
            {
                *ppStyle = (MsfStyle*)ptr;
                return iRet;
            }
            
            switch( wt )
            {
                case MSF_IMAGE:
                {
                    hStyle = ((MsfImage*)ptr)->style;
                    if( ppHandle )
                    {
                        *ppHandle = &((MsfImage*)ptr)->style;
                    }
                    break;
                }
                case MSF_STRING:
                {
                    hStyle = ((MsfString*)ptr)->style;
                    if( ppHandle )
                    {
                        *ppHandle = &((MsfString*)ptr)->style;
                    }
                    break;
                }
                case MSF_ICON:
                {
                    hStyle = ((MsfIcon*)ptr)->style;
                    if( ppHandle )
                    {
                        *ppHandle = &((MsfIcon*)ptr)->style;
                    }
                    break;
                }
#if 0        
        /*        case MSF_STYLE:
                {
                    hStyle = (void*)pWidget;
                    break;
                }
        */        
                case MSF_SOUND:
                {
                    hStyle = ;
                    break;
                }
                case MSF_COLORANIM:
                {
                    hStyle = ;
                    break;
                }
                case MSF_MOVE:
                {
                    hStyle = ;
                    break;
                }
                case MSF_ROTATION:
                {
                    hStyle = ;
                    break;
                }
#endif
                default:
                {
                    iRet = TPI_WIDGET_ERROR_NOT_SUPPORTED;
                    break;
                }
            }
        }       
        else
        {
            iRet = TPI_WIDGET_ERROR_NOT_SUPPORTED;
        }
    }
    else
    {
        iRet = TPI_WIDGET_ERROR_UNEXPECTED;
    }

    if( hStyle != INVALID_HANDLE )
    {
        *ppStyle = (MsfStyle *)seekWidget( hStyle, &styleType);
        if( *ppStyle == OP_NULL ||  styleType != MSF_STYLE )
        {
            *ppStyle = OP_NULL;
            iRet =  TPI_WIDGET_ERROR_UNEXPECTED;
        }
    }

    return iRet;
}

/*==================================================================================================
    FUNCTION: widget_get_file_resource

    DESCRIPTION:
        get file resource.

    ARGUMENTS PASSED:
       modId : The MSF Module identifier.   
       hImage: The identifier of created empty MsfImage.
       resource : resource name.
        
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise the appropriate
        return code.

    IMPORTANT NOTES:
        donot forget to set pImage->format into MsfImage.
==================================================================================================*/
static int widget_get_file_resource
(
    MSF_UINT8       modId,
    MsfImage       *pImage,
    char*           resource
)
{
    int              hFile;
    
    /* open "resource" file */
    hFile = MSF_FILE_OPEN (modId,
                            resource,
                            MSF_FILE_SET_RDONLY | MSF_FILE_SET_CREATE,
                            0);
    if(hFile != TPI_FILE_OK)
    {
        /* GetLastError(); */
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }
    /* get file size */
    pImage->dataSize = MSF_FILE_GETSIZE(resource);
    /* malloc for keep pImage structure */
    pImage->data = WIDGET_ALLOC(pImage->dataSize);
    if(pImage->data == OP_NULL)
    {
        MSF_FILE_CLOSE(hFile);
        return  TPI_WIDGET_ERROR_BAD_HANDLE;
    }
    op_memset(pImage->data, 0, pImage->dataSize);
    /* read file */
    if(MSF_FILE_READ(hFile, pImage->data, pImage->dataSize) <= 0)
    {
        /* GetLastError(); */
        WIDGET_FREE(pImage->data);
        pImage->data = OP_NULL;
        MSF_FILE_CLOSE(hFile);
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }
    /* close file */
    MSF_FILE_CLOSE(hFile);
    /* //suggest COM_FILE_OK move into msf_file.h.
    if(MSF_FILE_CLOSE(hFile) != COM_FILE_OK)
    {
        WIDGET_FREE(pImage->data);
        pImage->data = OP_NULL;
    }*/
    
    /* fill pImage necessary info ?*/
    //pImage->format = ;
    //pImage->moreData = ;
    return 1;
}

/*==================================================================================================
    FUNCTION: widget_get_pipe_resource

    DESCRIPTION:
        get pipe resource.

    ARGUMENTS PASSED:
        modId : The MSF Module identifier.   
        hImage: The identifier of created empty MsfImage.
        resource : resource name.
        
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise the appropriate
        return code.

    IMPORTANT NOTES:
        donot forget to set pImage->format into MsfImage.
==================================================================================================*/
static int widget_get_pipe_resource
(
    MSF_UINT8       modId,
    MsfImage       *pImage,
    char*           resource
)
{   
    PIPE_DATA_EXCHAGE_BRS_DLE pipe_data;

    pImage->dataSize = 0x00;
    pImage->moreData = OP_TRUE;

    op_memset(&pipe_data, 0x00, sizeof(PIPE_DATA_EXCHAGE_BRS_DLE));
    pipe_data.MIME_type = RMA_MIME_TYPE_IMAGE;
    pipe_data.pipename = resource;
    pipe_data.u.pImage = pImage;
    pipe_data.originalModId = modId;
    rma_pipe_set_info(&pipe_data);
 
    return MSF_PIPE_ERROR_DELAYED;    

}

/*==================================================================================================
    FUNCTION: widget_get_file_resource_for_sound

    DESCRIPTION:
        get file resource.

    ARGUMENTS PASSED:
       modId : The MSF Module identifier.   
       pSound: The pointer of empty MsfSound.
       resource : resource name.
        
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise the appropriate
        return code.

    IMPORTANT NOTES:
        donot forget to set pImage->format into MsfImage.
==================================================================================================*/
static int widget_get_file_resource_for_sound
(
    MSF_UINT8       modId,
    MsfSound       *pSound,
    char*           resource
)
{
    int    size;
    int    hFile;
    char   *pBuf;
    
    /* open "resource" file */
    hFile = MSF_FILE_OPEN (modId,
                            resource,
                            MSF_FILE_SET_RDONLY | MSF_FILE_SET_CREATE,
                            0);
    if(hFile != TPI_FILE_OK)
    {
        /* GetLastError(); */
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }
    /* get file size */
    size = MSF_FILE_GETSIZE(resource);
    /* malloc for keep pImage structure */
    pBuf = WIDGET_ALLOC(size);
    if(pBuf == OP_NULL)
    {
        MSF_FILE_CLOSE(hFile);
        return  TPI_WIDGET_ERROR_BAD_HANDLE;
    }
    
    op_memset(pBuf, 0, size );
    /* read file */
    if(MSF_FILE_READ(hFile, pBuf, size ) <= 0)
    {
        /* GetLastError(); */
        WIDGET_FREE( pBuf );
        MSF_FILE_CLOSE(hFile);
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }

    pSound->data = pBuf;
    pSound->size = size;
    
    /* close file */
    MSF_FILE_CLOSE(hFile);
    
    return 1;
}

/*==================================================================================================
    FUNCTION: widget_get_pipe_resource_for_sound

    DESCRIPTION:
        get pipe resource.

    ARGUMENTS PASSED:
        modId : The MSF Module identifier.   
        pSound: The pointer of empty MsfSound.
        resource : resource name.
        
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise the appropriate
        return code.

    IMPORTANT NOTES:
        donot forget to set pImage->format into MsfImage.
==================================================================================================*/
static int widget_get_pipe_resource_for_sound
(
    MSF_UINT8       modId,
    MsfSound        *pSound,
    char*           resource
)
{   
    PIPE_DATA_EXCHAGE_BRS_DLE pipe_data;

    pSound->size = 0x00;
    pSound->moreData = OP_TRUE;

    op_memset(&pipe_data, 0x00, sizeof(PIPE_DATA_EXCHAGE_BRS_DLE));
    pipe_data.MIME_type = RMA_MIME_TYPE_SOUND;
    pipe_data.pipename = resource;
    pipe_data.u.pSound = pSound;
    pipe_data.originalModId = modId;
    rma_pipe_set_info(&pipe_data);
 
    return MSF_PIPE_ERROR_DELAYED;    
}

/*==================================================================================================
    FUNCTION: TPIa_widgetHoldDraw

    DESCRIPTION:
        Tells the integration that several consecutive calls to drawing functions for
        the specified MsfImage/MsfPaintbox will be made. This call tells the 
        integration to hold the actual execution of these calls until the 
        TPIa_widgetPerformDraw() is called. This function is only applicable to 
        MsfPaintbox and MsfImage widgets.

    ARGUMENTS PASSED:
        msfHandle: The identifier of the MsfPaintbox or MsfImage.
        
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise the appropriate
        return code.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetHoldDraw(MSF_UINT32 msfHandle)
{
    PAINTBOX_IMAGEGADGET_STRUCT_NAME_T  result;
    int                                 retCode;
        
    /*
     * get the widget type, it should be MSF_PAINTBOX or MSF_IMAGE
     */
    retCode = get_Paintbox_Image_Info(msfHandle, &result);
        
    /*
     *  we could not support holddraw if the widget is nonfocus
     */
    if(retCode ==TPI_WIDGET_OK)
    {
        if(result.widgetType == MSF_PAINTBOX)
        {
            result.pPaintbox->isHoldDraw = OP_TRUE;
        }
        else if(result.widgetType == MSF_IMAGE)
        {
            result.pImage->isHoldDraw = OP_TRUE;
        }
    }
                     
    return retCode;
}

/*==================================================================================================
    FUNCTION: TPIa_widgetPerformDraw

    DESCRIPTION:
        Tells the integration to execute the stored calls to drawing functions that 
        were made to the specified MsfImage/MsfPaintbox since the TPIa_widgetHoldDraw()
        call. If no previous call was made to TPIa_widgetHoldDraw() for the specified
        MsfImage/MsfPaintbox, this function call will be ignored by the integration 
        and a positive integer will be returned. This function is only applicable to
        MsfPaintbox and MsfImage widgets. Note that if the stored drawing functions 
        comprises the drawing of any widget, for example an MsfString, and the properties
        of that widget were changed since the TPIa_widgetHoldDraw() call, the drawing 
        of that widget will be made according to the current properties of the widget.
        
    ARGUMENTS PASSED:
        msfHandle: The identifier of the MsfPaintbox or MsfImage.
        
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise the appropriate
        return code.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetPerformDraw(MSF_UINT32 msfHandle)
{
    PAINTBOX_IMAGEGADGET_STRUCT_NAME_T  result;
    int                                 retCode;
        
    /*
     * get the widget type, it should be MSF_PAINTBOX or MSF_IMAGE
     */
    retCode = get_Paintbox_Image_Info(msfHandle, &result);

    /*
     *  we could not support holddraw if the widget is nonfocus
     */
        
    if(retCode ==TPI_WIDGET_OK)
    {
        if(result.widgetType == MSF_PAINTBOX)
        {
            if(result.pPaintbox->isHoldDraw == OP_TRUE && result.pPaintbox->windowData.isFocused == OP_TRUE )
            {
            /* 
             * draw the paintbox now
             * we could not support hold draw now,just refresh the screen if it is focus
             */
                WAP_REFRESH
                result.pPaintbox->isHoldDraw = OP_FALSE;
            }
        }
        else if(result.widgetType == MSF_IMAGE)
        {
            if(result.pImage->isHoldDraw == OP_TRUE)
            {
            /* 
             * draw the image now? should we need to cach the image data?
             * 
             * we could not support hold draw now,just refresh the screen if it is focus
             */
//                ds_draw_bitmap_image(OP_INT16 x, OP_INT16 y, RM_BITMAP_T * bitmap);
                WAP_REFRESH
                result.pImage->isHoldDraw = OP_FALSE;
            }
        }
    }
                     
    return retCode;

}

/*==================================================================================================
    FUNCTION: TPIa_widgetDrawLine 

    DESCRIPTION:
        Draws a line between the specified MsfPositions start and end.

    ARGUMENTS PASSED:
        msfHandle: The identifier of the MsfPaintbox or MsfImage.
        start    : Specifies the start MsfPosition of the line.
        end      : Specifies the end MsfPosition of the line.

    RETURN VALUE:
        A positive integer if the operation was successful, otherwise the appropriate
        return code.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetDrawLine 
(
    MSF_UINT32    msfHandle, 
    MsfPosition*  start,
    MsfPosition*  end
)
{
    PAINTBOX_IMAGEGADGET_STRUCT_NAME_T  result;
    DS_COLOR                   color;
    int                                 retCode;
        
    /*
     * get the widget type, it should be MSF_PAINTBOX or MSF_IMAGE
     */
    retCode = get_Paintbox_Image_Info(msfHandle, &result);

        
    if( (retCode == TPI_WIDGET_OK)
        && (result.widgetType == MSF_PAINTBOX) 
        && (result.pPaintbox->windowData.isFocused == OP_TRUE ))
    {
        retCode = widgetGetDrawingInfo( OP_NULL,
                                                                  MSF_MAX_WIDGETTYPE, 
                                                                  result.pPaintbox->brush, 
                                                                  OP_NULL, 
                                                                  &color, 
                                                                  OP_NULL);            
        if( retCode < 0 )
        {
            return retCode;
        }
        
        /*
         * maybe we should support linestyle
         */
        widgetScreenDrawLine((const MsfWindow *) result.pPaintbox, 
                                                start->x, 
                                                start->y, 
                                                end->x, 
                                                end->y, 
                                                color);
        SHOW
        
        /*
         * if the widget hasn't been set as holddraw, 
         * just keep the bitmap buffer but NOT refresh at once
         */
        if(result.bIsHoldDraw == OP_FALSE)
        {
            WAP_REFRESH
        }
    }

    return retCode;

}


/*==================================================================================================
    FUNCTION: TPIa_widgetDrawRect 

    DESCRIPTION:
        Draws a rectangle with the upper left co-ordinate specified in the MsfPosition
        position. The size of the rectangle is specified by the MsfSize size. If the 
        corners of the rectangle are to be rounded, the parameter arcSize specifies
        the horizontal and vertical diameter of the arc at the four corners of the 
        rectangle. If the parameter arcSize is set to 0, a normal rectangle will be 
        drawn, that is, with no rounded corners.

    ARGUMENTS PASSED:
        msfHandle  : The identifier of the MsfPaintbox or MsfImage.
        position   : Specifies the MsfPosition of the rectangle.
        size       : Specifies the MsfSize of the rectangle.
        arcSize    : Specifies the horizontal and vertical diameter of the arc at the
                     four corners of the rectangle.
        fill       : Specifies if the drawn rectangle is to be filled (1) or not (0).

    RETURN VALUE:
        A positive integer if the operation was successful, otherwise
        the appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetDrawRect 
(
    MSF_UINT32     msfHandle,
    MsfPosition*   position, 
    MsfSize*       size, 
    int            fill
)
{
    DS_COLOR                   color,backColor;
    int                                 retCode;
    PAINTBOX_IMAGEGADGET_STRUCT_NAME_T  result;
        
    /*
     * get the widget type, it should be MSF_PAINTBOX or MSF_IMAGE
     */
    retCode = get_Paintbox_Image_Info(msfHandle, &result);

    /*
     *  we could not support holddraw if the widget is nonfocus
     */
        
    if( (retCode == TPI_WIDGET_OK)
        && (result.widgetType == MSF_PAINTBOX) 
        && (result.pPaintbox->windowData.isFocused == OP_TRUE ))
    {
        retCode = widgetGetDrawingInfo( OP_NULL,
                                                                  MSF_MAX_WIDGETTYPE, 
                                                                  result.pPaintbox->brush, 
                                                                  OP_NULL, 
                                                                  &color, 
                                                                  &backColor);            

        if( retCode >= 0 )
        {
            if(fill == OP_TRUE)
            {
                /*
                 * fill in the rect with specific color
                 */
                 widgetScreenFillRect((const MsfWindow *) result.pPaintbox, 
                                                      (OP_INT16)position->x,
                                                      (OP_INT16)position->y,
                                                      (OP_INT16)(position->x + size->width-1),
                                                      (OP_INT16)(position->y + size->height-1), 
                                                      backColor);
            }
            else
            {
                widgetScreenDrawRect((const MsfWindow *) result.pPaintbox, 
                                                      (OP_INT16)position->x,
                                                      (OP_INT16)position->y,
                                                      (OP_INT16)(position->x + size->width-1),
                                                      (OP_INT16)(position->y + size->height-1), 
                                                        color);
            }
            
        SHOW
        
            /*
             * if the widget hasn't been set as holddraw, 
             * just keep the bitmap buffer but NOT refresh at once
             */
            if(result.bIsHoldDraw == OP_FALSE)
            {
                WAP_REFRESH
            }
        }
    }                

    return retCode;

}

/*==================================================================================================
    FUNCTION: TPIa_widgetDrawIcon 

    DESCRIPTION:
        Draws an Icon of the specified IconType with the upper left co-ordinate
        specified by the MsfPosition position.

    ARGUMENTS PASSED:
        msfHandle: The identifier of the MsfPaintbox or MsfImage.
        icon     : The identifier of the MsfIcon to be drawn.
        position : Specifies the MsfPosition of the bounding rectangle of the icon.

    RETURN VALUE:
        A positive integer if the operation was successful, otherwise 
        the appropriate return code is given.

    IMPORTANT NOTES:
        Note that the MsfIcon must be drawn according to the current MsfStyle settings of the MsfIcon,
        unless the MsfIcon was created with the default MsfStyle and no MsfStyle has been explicitly
        set for the MsfIcon. In that case the MsfIcon must be drawn according to the current MsfStyle
        settings of the current brush.
==================================================================================================*/
int TPIa_widgetDrawIcon 
( 
    MSF_UINT32     msfHandle, 
    MsfIconHandle  icon,
    MsfPosition*   position
)
{
    PAINTBOX_IMAGEGADGET_STRUCT_NAME_T  result;
    int                 retCode;
    RM_RESOURCE_ID_T icon_res_id;
    MsfWindow*         pWin;
    MsfWidgetType       widgetType;
    MsfIcon*            pIcon;
    
    retCode = get_Paintbox_Image_Info(msfHandle, &result);

    if( (retCode == TPI_WIDGET_OK)
        && (result.widgetType == MSF_PAINTBOX) 
        && (result.pPaintbox->windowData.isFocused == OP_TRUE ))
    {
        /* get parent window handle */
        pWin = (MsfWindow *)result.pPaintbox;
        pIcon = seekWidget(icon, &widgetType);

        if((pIcon != OP_NULL)&&(widgetType == MSF_IMAGE))
        {
            /* Get resid through different icon type, these icon resource existed in RM*/
            icon_res_id = widgetGetIconResID(pIcon->type);
            if(icon_res_id>RM_ICON_FIRST_ID && icon_res_id<RM_ICON_LAST_ID)
            {
                widgetScreenDrawIconRm(pWin, position->x, position->y, 0, 0, icon_res_id);
            }
        }
        /*
         * if the widget hasn't been set as holddraw, 
         * just keep the bitmap buffer but NOT refresh at once
         */
        if(result.bIsHoldDraw == OP_FALSE)
        {
            WAP_REFRESH
        }

    }

    return retCode;
}


/*==================================================================================================
    FUNCTION: TPIa_widgetDrawArc 

    DESCRIPTION:
        Draws an arc. The MsfPosition position specifies the position of 
        the upper left coordinate of the rectangle covering the arc. The
        MsfSize size specifies the width and height of the arc. The arc is
        drawn from the specified start angle and extends the specified 
        number of degrees. 0 degrees is interpreted as the positive x-axis,
        (3 o¡¯clock)and a positive angle indicates a counterclockwise move. 
        The angles are interpreted relative to the bounding rectangle, which
        means that 45 degrees is always on the line from the centre to the
        upper right corner. If the arc is to be filled, the filled region
        consists of the ¡°pie wedge¡± region bounded by the arc segment, the 
        radius extending from the centre to this arc at startAngle degrees, 
        and the radius extending from the centre to this arc at 
        startAngle + angleExtent degrees.

    ARGUMENTS PASSED:
        msfHandle  : The identifier of the MsfPaintbox or MsfImage.
        position   : Specifies the MsfPosition of the bounding rectangle.
        size       : Specifies the MsfSize of the bounding rectangle of the arc.
        startAngle : Specifies the starting angle of the arc.
        angleExtent: Specifies the angular extent of the arc.
        fill       : Specifies if the drawn arc is to be filled (1) or not (0).
        
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise 
        the appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetDrawArc 
(
    MSF_UINT32    msfHandle,
    MsfPosition*  position,
    MsfSize*      size, 
    int           startAngle,
    int           angleExtent, 
    int           fill
)
{return 0;}

/*==================================================================================================
    FUNCTION: TPIa_widgetDrawIcon 

    DESCRIPTION:
        Draws the specified part of the MsfString with the upper left 
        co-ordinate of the bounding rectangle placed at the specified
        MsfPosition position. The bounding rectangle of the MsfString
        has the specified maximum MsfSize. If the maxSize parameter 
        is 0 the full MsfString will be drawn with its actual size.

    ARGUMENTS PASSED:
        msfHandle    : The identifier of the MsfPaintbox or MsfImage.
        string       : Specifies the MsfString to be drawn.
        position     : Specifies the MsfPosition of the bounding rectangle 
                       of the string.
        maxSize      : Specifies the maximum MsfSize of the string to be
                       drawn.
        index        : Specifies the start index of the part of the string
                       that is to be drawn.
        nbrOfChars   : Specifies the number of characters of the string that
                       is to be drawn, starting from index. 0 specifies that
                       the rest of the string from index is drawn.
        baseline     : Specifies if the position parameter defines the
                       position of the start of the baseline of the MsfString
                       (1) or if the position parameter defines the normal 
                       position, that is, the position of the upper left 
                       corner of the bounding rectangle of the MsfString (0).
        useBrushStyle: Specifies if the MsfString is to be drawn with the style
                       specified by the brush (1) or if it is to be drawn with
                       the style specified by the MsfString itself (0).

    RETURN VALUE:
        A positive integer if the operation was successful, otherwise 
        the appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetDrawString
(
    MSF_UINT32        msfHandle, 
    MsfStringHandle   string, 
    MsfPosition*      position,
    MsfSize*          maxSize, 
    int               index,
    int               nbrOfChars, 
    int               baseline,
    int               useBrushStyle
)
{
    PAINTBOX_IMAGEGADGET_STRUCT_NAME_T  result;
    int                    retCode;
    MsfWidgetDrawString    drawString;
    OP_UINT8               buf[202];
    OP_UINT8               *pBuf;
    OP_INT16               iEngCharWidth, iEngCharHeight;
    OP_INT16               iLen;
    OP_UINT8               *pChar;

    if( position == OP_NULL || index < 0 )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    /*
      * get the widget type, it should be MSF_PAINTBOX or MSF_IMAGE
      */
    retCode = get_Paintbox_Image_Info(msfHandle, &result);

    /*
      *  we could not support holddraw if the widget is nonfocus
      */

    if( (retCode == TPI_WIDGET_OK)
        && (result.widgetType == MSF_PAINTBOX) 
        && (result.pPaintbox->windowData.isFocused == OP_TRUE ))
    {
        WIDGET_GET_STRING_DATA(pChar, string);
        
        if( !pChar )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }
        
        iLen = Ustrlen(pChar) / 2;
        if( iLen < index)
        {
            return TPI_WIDGET_ERROR_INVALID_PARAM;
        }

        if( nbrOfChars != 0 )
        {
            if( nbrOfChars + index > iLen )
            {
                iLen -= index;
            }
            else
            { 
                iLen = nbrOfChars;
            }
        }
        else
        {
            iLen -=  index;
        }

        iLen <<= 1;    /*   iLen = iLen*2   */
        if( iLen > 200 )
        {
            pBuf = WIDGET_ALLOC(iLen + 2);
            if( pBuf == OP_NULL )
            {
                return TPI_WIDGET_ERROR_RESOURCE_LIMIT;
            }
        }
        else
        {
            pBuf = buf;
        }

        op_memcpy( pBuf, (OP_UINT8*)(pChar + index*2) , iLen);
        pBuf[iLen] = pBuf[iLen +1] = 0x00;   /* add OP_NULL char  */
        drawString.pBuf = pBuf;

        drawString.bShowInitialWhiteSpaces = OP_FALSE;
        retCode = widgetGetDrawingInfo( OP_NULL,
                                        MSF_MAX_WIDGETTYPE, 
                                        result.pPaintbox->brush, 
                                        &drawString.fontAttr, 
                                        &drawString.fontcolor, 
                                        &drawString.backcolor);            
        if( retCode < 0 )
        {
            if( pBuf != buf )
            {
                WIDGET_FREE(pBuf);
            }
            
            return retCode;
        }
        
        if( 0 > widgetGetFontSizeInEngChar(drawString.fontAttr, &iEngCharWidth, &iEngCharHeight))
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }
        
        if( maxSize != OP_NULL )
        {
            drawString.size.width = maxSize->width;
            drawString.size.height= maxSize->height;
        }
        else
        {
            drawString.size.width = iEngCharWidth * UstrCharCount( pBuf );
            drawString.size.height= (iEngCharWidth << 1);
        }
        
        drawString.pos.x = position->x;
        /* show at the center   */
        drawString.pos.y = position->y +drawString.size.height /2 -iEngCharWidth ;
        
        widgetDrawStringByChar((MsfWindow *) result.pPaintbox, &drawString);            

        if( pBuf != buf )
        {
            WIDGET_FREE(pBuf);
        }
        
        SHOW
        
        /*
         * if the widget hasn't been set as holddraw, 
         * just keep the bitmap buffer but NOT refresh at once
         */
        if(result.bIsHoldDraw == OP_FALSE)
        {
            WAP_REFRESH
        }
    }                

    return retCode;
}

/*
*/
/*==================================================================================================
    FUNCTION: TPIa_widgetDrawPolygon 

    DESCRIPTION:
        Draws a polygon with the specified number of corners. The corner
        MsfPositions are specified by the *MSF_INT16 theCorners parameter.
        The horizontal value of the first MsfPosition is retrieved from
        the first position in the *int16 corners parameter, the vertical
        value of the first MsfPosition is retrieved from the second 
        position, the horizontal value of the second MsfPosition is
        retrieved from the third position and so on, until all of the
        polygon corners are specified.

    ARGUMENTS PASSED:
        msfHandle   : The identifier of the MsfPaintbox or MsfImage.
        nrOfCorners : Specifies the number of corners in the polygon
                      to be drawn.
        corners     : Specifies the MsfPositions of the corners in the polygon.
        fill        : Specifies if the drawn polygon is to be filled (1)
                      or not (0).

    RETURN VALUE:
        A positive integer if the operation was successful, otherwise 
        the appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetDrawPolygon 
(
    MSF_UINT32  msfHandle, 
    int         nrOfCorners, 
    MSF_INT16*  corners, 
    int         fill
)
{
 // TODO:  this function will be done in future.
    return 0;
}

/*==================================================================================================
    FUNCTION: TPIa_widgetDrawImage

    DESCRIPTION:
        Draws the specified MsfImage on the specified MsfPaintbox or
        MsfImage in the specified MsfPosition. The size of the drawn
        MsfImage is bounded by the specified MsfSize. Those parts of
        the MsfImage that are outside of the bounding rectangle will
        not be drawn.
        It is possible for a module to call the TPIa_widgetDrawImage()
        function even if the image is not fully created. It is the
        integration that decides what happens if such a call is made.
        The integration may choose to draw the part of the image that
        exist. The integration may also choose to ignore the function
        call if the image is not fully created.
        If a call to TPIa_widgetGetSize() function is made even if the
        image is not fully created, the integration should always
        return the MsfSize of the total image, i.e. the fully created
        image, if this is possible. If this is not possible the 
        integration should return the MsfSize (0,0).

    ARGUMENTS PASSED:
        msfHandle: The identifier of the MsfPaintbox or MsfImage.
        image    : The identifier of the MsfImage to be drawn.
        position : Specifies the MsfPosition of the bounding rectangle
                   of the MsfImage.
        maxSize  : Specifies the maximum MsfSize of the bounding
                   rectangle of the MsfImage.
        imageZoom: Specifies the MsfImageZoom the MsfImage should be
                   drawn with. The drawn MsfImage is still bounded by
                   the specified maxSize.
        
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise the appropriate
        return code.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetDrawImage
(
    MSF_UINT32       msfHandle,
    MsfImageHandle   image,
    MsfPosition*     position,
    MsfSize*         maxSize,
    MsfImageZoom     imageZoom
)
{
    PAINTBOX_IMAGEGADGET_STRUCT_NAME_T  result;
    int                                 retCode;
    MsfImage                            *pImage;
    MsfWidgetType                       widgetType;
    /*
     * get the widget type, it should be MSF_PAINTBOX or MSF_IMAGE
     */
    retCode = get_Paintbox_Image_Info(msfHandle, &result);
        
    if( (retCode == TPI_WIDGET_OK)
        && (result.widgetType == MSF_PAINTBOX) 
        && (result.pPaintbox->windowData.isFocused == OP_TRUE ))
    {
        pImage = seekWidget(image, &widgetType);

        if((pImage != OP_NULL)&&(widgetType == MSF_IMAGE))
        {
            widgetScreenDrawImage((MsfWindow*)result.pPaintbox, 
                                    position, 
                                    maxSize, 
                                    pImage, 
                                    imageZoom);
        }
        /*
         * if the widget hasn't been set as holddraw, 
         * just keep the bitmap buffer but NOT refresh at once
         */
        if(result.bIsHoldDraw == OP_FALSE)
        {
            WAP_REFRESH
        }
    }                

    return retCode;
}

/*==================================================================================================
    FUNCTION: TPIa_widgetDrawAccessKey

    DESCRIPTION:
        Draws an MsfAccessKey with the upper left co-ordinate specified
        by the MsfPosition position. The integration is responsible for
        drawing the device specific symbol or text that visualises the
        specified MsfAccessKey.

    ARGUMENTS PASSED:
        msfHandle : The identifier of the MsfPaintbox or MsfImage.
        accessKey : The identifier of the MsfAccessKey to be drawn.
        position  : Specifies the MsfPosition of the bounding rectangle
                    of the MsfAccessKey.
        
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise 
        the appropriate return code.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetDrawAccessKey
(
    MSF_UINT32          msfHandle, 
    MsfAccessKeyHandle  accessKey, 
    MsfPosition*        position
)
{
    PAINTBOX_IMAGEGADGET_STRUCT_NAME_T  result;
    int                                  retCode;
    MsfAccessKey                *pAccessKey;
    MsfWidgetType              widgetType;
    DS_FONTATTR                fontattr;
    RM_RESOURCE_ID_T     icon_res_id;
    char                               *str;
    char                               iKey;
    
    /*
     * get the widget type, it should be MSF_PAINTBOX or MSF_IMAGEGADGET
     */
    retCode = get_Paintbox_Image_Info(msfHandle, &result);
        
    if( (retCode == TPI_WIDGET_OK)
        && (result.widgetType == MSF_PAINTBOX) 
        && (result.pPaintbox->windowData.isFocused == OP_TRUE ))
    {
        pAccessKey = (MsfAccessKey *)seekWidget(accessKey, &widgetType);
        str = pAccessKey->accessKeyDefinition;
        if((pAccessKey != OP_NULL)&&(widgetType == MSF_ACCESSKEY) 
        && str != OP_NULL
        && Ustrlen((OP_UINT8*)str) == 2             /*  only one unicode char  */
        && str[1] == 0x00                  /*  '0'~ '9'  */
        && (str[0] >= 0x30  && str[0] <= 0x39))
        {
            retCode = widgetGetDrawingInfo( OP_NULL,
                                                                      MSF_MAX_WIDGETTYPE, 
                                                                      result.pPaintbox->brush, 
                                                                     &fontattr,
                                                                     OP_NULL,
                                                                     OP_NULL);
            if( retCode < 0 )
            {
                return retCode;
            }

            iKey = str[0] - 0x30;
            if( fontattr & FONT_SIZE_SMALL )
            {
                icon_res_id = ICON_POPUP_LINE_NUM_0 + iKey;
            }
            else
            {
                icon_res_id = ICON_LINE_NUM_C1_0 + iKey;
            }
            
            widgetScreenDrawIconRm((const MsfWindow*) result.pPaintbox, 
                                                           (OP_INT16)position->x, 
                                                           (OP_INT16)position->y, 
                                                           0,
                                                           0,
                                                           icon_res_id);            
            /*
             * if the widget hasn't been set as holddraw, 
             * just keep the bitmap buffer but NOT refresh at once
             */
            if(result.bIsHoldDraw == OP_FALSE)
            {
                WAP_REFRESH
            }
        }
        else
        {
            retCode = TPI_WIDGET_ERROR_UNEXPECTED;
        }

    }
    
    return retCode;
}

/****************************************************************
 STYLE
 ***************************************************************/
/*==================================================================================================
    FUNCTION: TPIa_widgetStyleCreate 

    DESCRIPTION:
        Creates an MsfStyle widget with the specified style attributes.

    ARGUMENTS PASSED:
        modId            : The MSF Module identifier
        color            : Specifies the MsfColor of the widget. If 0 the device
                           default is used.
        backgroundColor  : Specifies the background MsfColor of the widget. If
                           0 the device default is used.
        foreground       : Specifies the MsfPattern to be used as foreground for
                           the widget. If 0 the device default is used.
        background       : Specifies the MsfPattern to be used as background
                           for the widget. If 0 the device default is used.
        lineStyle        : Specifies the MsfLineStyle of the widget. If 0 the 
                           device default is used.
        font             : Specifies the MsfFont of the widget. If 0 the device
                           default is used.
        textProperty     : Specifies the MsfTextProperty of the widget. If 0 the
                           device default is used.

    RETURN VALUE:
        returns The new MsfStyleHandle if the operation was successful, otherwise 0

    IMPORTANT NOTES:
        None
==================================================================================================*/
MsfStyleHandle TPIa_widgetStyleCreate
(
    MSF_UINT8        modId,
    MsfColor*        color, 
    MsfColor*        backgroundColor,
    MsfPattern*      foreground,
    MsfPattern*      background,
    MsfLineStyle*    lineStyle, 
    MsfFont*         font,
    MsfTextProperty* textProperty
)
{
    MsfStyle         *pStyle;

    pStyle = WIDGET_NEW(MsfStyle);
    if(pStyle == OP_NULL)
    {
        return INVALID_HANDLE;
    }
    else
    {
        op_memset( pStyle, 0, sizeof(MsfStyle));
        
        pStyle->handle = getNewHandle(modId, MSF_STYLE );
        pStyle->modId = modId;
        pStyle->referenceCounter = 1;
        
        if( color != OP_NULL )
        {
            op_memcpy(&pStyle->color ,color, sizeof(MsfColor));
        }
        
        if( backgroundColor != OP_NULL )
        {
            op_memcpy(&pStyle->backgroundColor, backgroundColor, sizeof(MsfColor));
        }
        
        if( foreground != OP_NULL )
        {
            op_memcpy(&pStyle->foreground, foreground, sizeof(MsfPattern));
        }
        
        if( background != OP_NULL )
        {
            op_memcpy(&pStyle->background, background, sizeof(MsfPattern));
        }
        
        if( lineStyle != OP_NULL)
        {
            op_memcpy(&pStyle->lineStyle, lineStyle,sizeof(MsfLineStyle));
        }
        
        if(font != OP_NULL)
        {
            op_memcpy(&pStyle->font, font, sizeof(MsfFont));
        }
        
        if(textProperty != OP_NULL)
        {
            op_memcpy(&pStyle->textProperty, textProperty, sizeof(MsfTextProperty));
        }

        SP_list_insert_to_head(&widgetQueue[MSF_STYLE], pStyle);

        return pStyle->handle;
    }
    
}

/*==================================================================================================
    FUNCTION: TPIa_widgetGetBrush 

    DESCRIPTION:
        Retrieves an MsfHandle to the brush belonging to the specified widget. 
        This function is applicable only to MsfPaintbox and MsfImage widgets.
        The brush properties are used for the drawing functions in the low-level API.
        For example, a call to the function TPIa_widgetDrawRect() will draw a 
        rectangle according to the current style settings of the brush. The MsfStyle
        settings of the brush can be manipulated using the style-API and the
        MsfBrushHandle. The MsfBrushHandle does not have to be released.

    ARGUMENTS PASSED:
        msfHandle: The identifier of the MsfPaintbox or MsfImage.
        
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise the appropriate
        return code.

    IMPORTANT NOTES:
        None
==================================================================================================*/
MsfBrushHandle TPIa_widgetGetBrush
(
    MSF_UINT32 handle
)
{
    MsfBrushHandle                      brushHandle = 0;
    PAINTBOX_IMAGEGADGET_STRUCT_NAME_T  result;
    int                                 retCode;
        
    /*
     * get the widget type, it should be MSF_PAINTBOX or MSF_IMAGEGADGET
     */
    retCode = get_Paintbox_Image_Info(handle, &result);
        
    if(retCode == TPI_WIDGET_OK )
    {
        if(result.widgetType == MSF_PAINTBOX)
        {
            /*
             * we could not support hold draw now,just refresh the screen if it is focus
             */
            brushHandle = result.pPaintbox->brush;
//            brushHandle = result.pPaintbox->windowData.style;
        }
        else if(result.widgetType == MSF_IMAGE)
        {
            /*
             * we could not support hold draw now,just refresh the screen if it is focus
             */
            brushHandle = result.pImage->brush;
//            brushHandle = result.pImage->style;
        }
    }

    return brushHandle;

}

/*int TPIa_widgetSetBrushProperty(MSF_UINT32 handle, MsfLineStyle* lineStyle, MsfColor* lineColor, MsfColor* fillColor, MsfImageHandle linePattern, MsfImageHandle fillPattern, int padded, MsfFont* font, MsfTextProperty* textProperty){return 0;}*/

/*==================================================================================================
    FUNCTION: TPIa_widgetSetColor

    DESCRIPTION:
        Sets the current colour of the specified object
        to the specified MsfColor colour.

    ARGUMENTS PASSED:
        msfHandle : The identifier of the MSF widget.
        color     : Specifies the MsfColor to be set as the current
                    colour of the widget.
        background: Specifies if the MsfColor to be set is the
                    background (1) colour of the widget or the
                    foreground colour (0).
                    
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise
        the appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetSetColor
(
    MSF_UINT32  msfHandle,
    MsfColor*   color,
    int         background
)
{
    int            retValue;
    MsfWidgetType  widgetType;
    MsfStyle        curStyle;
    MsfStyle       *pStyle = OP_NULL;
    MsfStyleHandle      *pHandle;

    if( color == OP_NULL )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    retValue = getRelatedStyle(msfHandle, &widgetType, &pStyle, &pHandle);
    if( retValue < 0 )
    {
        return retValue;
    }
    
    if(pStyle != OP_NULL)
    {
        if(background != 0)
        {
            op_memcpy(&pStyle->backgroundColor, color, sizeof(MsfColor));        
        }
        else if(widgetType != MSF_SCREEN)
        {
            /* MsfScreen doesn't support foreground color */
            op_memcpy(&pStyle->color, color, sizeof(MsfColor));
        }
        else
        {
            retValue = TPI_WIDGET_ERROR_NOT_SUPPORTED;
        }
    }
    else if( pHandle )      /* the widget has style member, but not set  */
    {
        /*  get the default style settings  */
        retValue = widgetGetStyleInfo( OP_NULL, MSF_MAX_WIDGETTYPE, msfHandle, &curStyle );
        if( retValue >= 0 )
        {
            if(background != 0)
            {
                if( op_memcmp(&curStyle.backgroundColor, color, sizeof(MsfColor)) != 0 )
                {
                    *pHandle = TPIa_widgetStyleCreate( getModID(msfHandle), 
                                                                                    &curStyle.color, 
                                                                                    color, 
                                                                                    &curStyle.foreground, 
                                                                                    &curStyle.background, 
                                                                                    &curStyle.lineStyle,
                                                                                    &curStyle.font,
                                                                                    &curStyle.textProperty);
                    increase_ref_count(*pHandle);
                    TPIa_widgetRelease( *pHandle );
                }
            }
            else
            {
                if( op_memcmp(&curStyle.color, color, sizeof(MsfColor)) != 0 )
                {
                    *pHandle = TPIa_widgetStyleCreate( getModID(msfHandle), 
                                                                                    color, 
                                                                                    &curStyle.backgroundColor, 
                                                                                    &curStyle.foreground, 
                                                                                    &curStyle.background, 
                                                                                    &curStyle.lineStyle,
                                                                                    &curStyle.font,
                                                                                    &curStyle.textProperty);
                    increase_ref_count(*pHandle);
                    TPIa_widgetRelease( *pHandle );
                }
            }
        }
        else
        {
            MsfFont   *pFont;
            if( widgetType == MSF_MAINMENU || widgetType == MSF_MENU )
            {
                pFont = (MsfFont*)&DEFAULT_MENU_STYLE_FONT;
            }
            else if( MSF_DIALOG == widgetType )
            {
                pFont = (MsfFont*)&DEFAULT_DIALOG_STYLE_FONT;
            }
            else
            {
                pFont = (MsfFont*)&DEFAULT_STYLE_FONT;
            }
            if(background != 0)
            {
                
                *pHandle = TPIa_widgetStyleCreate( getModID(msfHandle), 
                                                                                (MsfColor*)(DefaultPropTab[widgetType].fgColors), 
                                                                                color, 
                                                                                (MsfPattern*)&DEFAULT_STYLE_FOREGROUND, 
                                                                                (MsfPattern*)&DEFAULT_STYLE_BACKGROUND, 
                                                                                (MsfLineStyle*)&DEFAULT_STYLE_LINESTYLE,
                                                                                (MsfFont*)pFont,
                                                                                (MsfTextProperty*)&DEFAULT_STYLE_TEXTPROPERTY);
                increase_ref_count(*pHandle);
                TPIa_widgetRelease( *pHandle );
            }
            else if(widgetType != MSF_SCREEN)
            {
                *pHandle = TPIa_widgetStyleCreate( getModID(msfHandle), 
                                                                                color, 
                                                                                (MsfColor*)(DefaultPropTab[widgetType].bgColors), 
                                                                                (MsfPattern*)&DEFAULT_STYLE_FOREGROUND, 
                                                                                (MsfPattern*)&DEFAULT_STYLE_BACKGROUND, 
                                                                                (MsfLineStyle*)&DEFAULT_STYLE_LINESTYLE,
                                                                                (MsfFont*)pFont,
                                                                                (MsfTextProperty*)&DEFAULT_STYLE_TEXTPROPERTY);
                increase_ref_count(*pHandle);
                TPIa_widgetRelease( *pHandle );
            }
            else
            {
                retValue = TPI_WIDGET_ERROR_NOT_SUPPORTED;
            }
        }
    }
    
    return retValue;
}


/*==================================================================================================
    FUNCTION: TPIa_widgetSetLineStyle

    DESCRIPTION:
        Sets the current line style of the specified object to 
        the specified MsfLineStyle style.

    ARGUMENTS PASSED:
        msfHandle : The identifier of the MSF widget.
        style     : Specifies the MsfLineStyle to be set as the
                    current line style of the widget.        
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise
        the appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetSetLineStyle
(
    MSF_UINT32    msfHandle, 
    MsfLineStyle* style
)
{
    int            retValue;
    MsfWidgetType  widgetType;
    MsfStyle       *pStyle = OP_NULL;
    MsfStyle        curStyle;
    MsfStyleHandle      *pHandle;

    if( style == OP_NULL )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
  
    retValue = getRelatedStyle(msfHandle, &widgetType, &pStyle, &pHandle);
    if( retValue < 0 )
    {
        return retValue;
    }

    if(pStyle != OP_NULL)
    {
        if(widgetType != MSF_SCREEN)
        {
            op_memcpy(&pStyle->lineStyle, style, sizeof(MsfLineStyle));        
        }
        else
        {
            /* MsfScreen doesn't support LineStyle */
            retValue = TPI_WIDGET_ERROR_NOT_SUPPORTED;
        }
    }
    else if( pHandle )      /* the widget has style member, but not set  */
    {
        /*  get the default style settings  */
        retValue = widgetGetStyleInfo( OP_NULL, MSF_MAX_WIDGETTYPE, msfHandle, &curStyle );
        if( retValue >= 0 )
        {
            if( op_memcmp(&curStyle.lineStyle, style, sizeof(MsfLineStyle)) != 0 )
            {
                *pHandle = TPIa_widgetStyleCreate( getModID(msfHandle), 
                                                                                &curStyle.color, 
                                                                                &curStyle.backgroundColor,  
                                                                                &curStyle.foreground, 
                                                                                &curStyle.background, 
                                                                                style, 
                                                                                &curStyle.font,
                                                                                &curStyle.textProperty);
                increase_ref_count(*pHandle);
                TPIa_widgetRelease( *pHandle );
            }
        }            
        else 
        {
            MsfFont   *pFont;
            if( widgetType == MSF_MAINMENU || widgetType == MSF_MENU )
            {
                pFont = (MsfFont*)&DEFAULT_MENU_STYLE_FONT;
            }
            else if( MSF_DIALOG == widgetType )
            {
                pFont = (MsfFont*)&DEFAULT_DIALOG_STYLE_FONT;
            }
            else
            {
                pFont = (MsfFont*)&DEFAULT_STYLE_FONT;
            }
            *pHandle = TPIa_widgetStyleCreate( getModID(msfHandle), 
                                            (MsfColor*)(DefaultPropTab[widgetType].fgColors), 
                                            (MsfColor*)(DefaultPropTab[widgetType].bgColors), 
                                            (MsfPattern*)&DEFAULT_STYLE_FOREGROUND, 
                                            (MsfPattern*)&DEFAULT_STYLE_BACKGROUND, 
                                            style, 
                                            (MsfFont*)pFont,
                                            (MsfTextProperty*)&DEFAULT_STYLE_TEXTPROPERTY);
            increase_ref_count(*pHandle);
            TPIa_widgetRelease( *pHandle );
        }
        
    }
    return retValue;
}


/*==================================================================================================
    FUNCTION: TPIa_widgetSetTextProperty

    DESCRIPTION:
        Sets the current text property of the specified object
        to the specified MsfTextProperty textProperty.

    ARGUMENTS PASSED:
        msfHandle    : The identifier of the MSF widget.
        textProperty : Specifies the MsfTextProperty to be set
                       as the current text property of the widget.
                       
    RETURN VALUE:
        A positive integer if the operation was successful,
        otherwise the appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetSetTextProperty
(
    MSF_UINT32        msfHandle,
    MsfTextProperty*  textProperty
)
{
    int            retValue;
    MsfWidgetType  widgetType;
    MsfStyle       *pStyle = OP_NULL;
    MsfStyle        curStyle;
    MsfStyleHandle      *pHandle;

    if( textProperty == OP_NULL )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
  
    retValue = getRelatedStyle(msfHandle, &widgetType, &pStyle, &pHandle);
    if( retValue < 0 )
    {
        return retValue;
    }

    if(pStyle != OP_NULL)
    {
        if(widgetType != MSF_SCREEN)
        {
            op_memcpy(&pStyle->textProperty, textProperty, sizeof(MsfTextProperty));        
        }
        else
        {
            /* MsfScreen doesn't support LineStyle */
            retValue = TPI_WIDGET_ERROR_NOT_SUPPORTED;
        }
    }
    else if( pHandle )      /* the widget has style member, but not set  */
    {
        /*  get the default style settings  */
        retValue = widgetGetStyleInfo( OP_NULL, MSF_MAX_WIDGETTYPE, msfHandle, &curStyle );
        if( retValue >= 0 )
        {
            if( op_memcmp(&curStyle.textProperty, textProperty, sizeof(MsfTextProperty)) != 0 )
            {
                *pHandle = TPIa_widgetStyleCreate( getModID(msfHandle), 
                                                                                &curStyle.color, 
                                                                                &curStyle.backgroundColor,  
                                                                                &curStyle.foreground, 
                                                                                &curStyle.background, 
                                                                                &curStyle.lineStyle,
                                                                                &curStyle.font,
                                                                                textProperty);
                increase_ref_count(*pHandle);
                TPIa_widgetRelease( *pHandle );
            }
        }
        else
        {
            MsfFont   *pFont;
            if( widgetType == MSF_MAINMENU || widgetType == MSF_MENU )
            {
                pFont = (MsfFont*)&DEFAULT_MENU_STYLE_FONT;
            }
            else if( MSF_DIALOG == widgetType )
            {
                pFont = (MsfFont*)&DEFAULT_DIALOG_STYLE_FONT;
            }
            else
            {
                pFont = (MsfFont*)&DEFAULT_STYLE_FONT;
            }
            *pHandle = TPIa_widgetStyleCreate( getModID(msfHandle), 
                                                                            (MsfColor*)(DefaultPropTab[widgetType].fgColors), 
                                                                            (MsfColor*)&(DefaultPropTab[widgetType].bgColors), 
                                                                            (MsfPattern*)&DEFAULT_STYLE_FOREGROUND, 
                                                                            (MsfPattern*)&DEFAULT_STYLE_BACKGROUND, 
                                                                            (MsfLineStyle*)&DEFAULT_STYLE_LINESTYLE,
                                                                            (MsfFont*)pFont,
                                                                            textProperty);
            increase_ref_count(*pHandle);
            TPIa_widgetRelease( *pHandle );
        }
    }
    
    return retValue;
}


/*==================================================================================================
    FUNCTION: TPIa_widgetSetPattern

    DESCRIPTION:
        Sets the current pattern of the specified object to
        the specified MsfPattern.

    ARGUMENTS PASSED:
        msfHandle  : The identifier of the MSF widget.
        pattern    : Specifies the MsfPattern to be used as
                     pattern for the widget.
        background : Specifies if the MsfPattern to be used
                     as pattern is the background (1) pattern
                     of the widget or the foreground pattern (0).  
                     
    RETURN VALUE:
        A positive integer if the operation was successful,
        otherwise the appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetSetPattern
(
    MSF_UINT32    msfHandle,
    MsfPattern*   pattern,
    int           background
)
{
    int            retValue;
    MsfWidgetType  widgetType;
    MsfStyle       *pStyle = OP_NULL;
    MsfStyle        curStyle;
    MsfStyleHandle      *pHandle;

    if( pattern == OP_NULL )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
  
    retValue = getRelatedStyle(msfHandle, &widgetType, &pStyle, &pHandle);
    if( retValue < 0 )
    {
        return retValue;
    }

    if(pStyle != OP_NULL)
    {
        if(background != 0)
        {
            op_memcpy(&pStyle->background, pattern, sizeof(MsfPattern));        
        }
        else if(widgetType != MSF_SCREEN)
        {
            /* MsfScreen doesn't support foreground pattern */
            op_memcpy(&pStyle->foreground, pattern, sizeof(MsfPattern));        
        }
        else
        {
            retValue = TPI_WIDGET_ERROR_NOT_SUPPORTED;
        }
    }
    else if( pHandle )      /* the widget has style member, but not set  */
    {
        /*  get the default style settings  */
        retValue = widgetGetStyleInfo( OP_NULL, MSF_MAX_WIDGETTYPE, msfHandle, &curStyle );
        if( retValue >= 0 )
        {
            if(background != 0)
            {
                if( op_memcmp(&curStyle.background, pattern, sizeof(MsfColor)) != 0 )
                {
                    *pHandle = TPIa_widgetStyleCreate( getModID(msfHandle), 
                                                                                    &curStyle.color, 
                                                                                    &curStyle.backgroundColor,  
                                                                                    &curStyle.foreground, 
                                                                                    pattern,             
                                                                                    &curStyle.lineStyle,
                                                                                    &curStyle.font,
                                                                                    &curStyle.textProperty);
                    increase_ref_count(*pHandle);
                    TPIa_widgetRelease( *pHandle );
                }
            }
            else
            {
                if( op_memcmp(&curStyle.foreground, pattern, sizeof(MsfColor)) != 0 )
                {
                    *pHandle = TPIa_widgetStyleCreate( getModID(msfHandle), 
                                                                                    &curStyle.color, 
                                                                                    &curStyle.backgroundColor,  
                                                                                    pattern,             
                                                                                    &curStyle.background, 
                                                                                    &curStyle.lineStyle,
                                                                                    &curStyle.font,
                                                                                    &curStyle.textProperty);
                    increase_ref_count(*pHandle);
                    TPIa_widgetRelease( *pHandle );
                }
            }
        }
        else
        {
            MsfFont   *pFont;
            if( widgetType == MSF_MAINMENU || widgetType == MSF_MENU )
            {
                pFont = (MsfFont*)&DEFAULT_MENU_STYLE_FONT;
            }
            else if( MSF_DIALOG == widgetType )
            {
                pFont = (MsfFont*)&DEFAULT_DIALOG_STYLE_FONT;
            }
            else
            {
                pFont = (MsfFont*)&DEFAULT_STYLE_FONT;
            }
            if(background != 0)
            {
                *pHandle = TPIa_widgetStyleCreate( getModID(msfHandle), 
                                                                                (MsfColor*)(DefaultPropTab[widgetType].fgColors), 
                                                                                (MsfColor*)(DefaultPropTab[widgetType].bgColors), 
                                                                                (MsfPattern*)&DEFAULT_STYLE_FOREGROUND, 
                                                                                pattern,             
                                                                                (MsfLineStyle*)&DEFAULT_STYLE_LINESTYLE,
                                                                                (MsfFont*)pFont,
                                                                                (MsfTextProperty*)&DEFAULT_STYLE_TEXTPROPERTY);
                increase_ref_count(*pHandle);
                TPIa_widgetRelease( *pHandle );
            }
            else if(widgetType != MSF_SCREEN)
            {
                *pHandle = TPIa_widgetStyleCreate( getModID(msfHandle), 
                                                                                (MsfColor*)(DefaultPropTab[widgetType].fgColors), 
                                                                                (MsfColor*)(DefaultPropTab[widgetType].bgColors), 
                                                                                pattern,             
                                                                                (MsfPattern*)&DEFAULT_STYLE_BACKGROUND, 
                                                                                (MsfLineStyle*)&DEFAULT_STYLE_LINESTYLE,
                                                                                (MsfFont*)pFont,
                                                                                (MsfTextProperty*)&DEFAULT_STYLE_TEXTPROPERTY);
                increase_ref_count(*pHandle);
                TPIa_widgetRelease( *pHandle );
            }
            else
            {
                retValue = TPI_WIDGET_ERROR_NOT_SUPPORTED;
            }
        }
    }
    return retValue;
}


/*==================================================================================================
    FUNCTION: TPIa_widgetSetFont

    DESCRIPTION:
        Sets the current font of the specified object to the
        specified MsfFont font.

    ARGUMENTS PASSED:
        msfHandle : The identifier of the MSF widget.
        font      : Specifies the MsfFont to be used as the
                    current font of the widget.
                    
    RETURN VALUE:
        A positive integer if the operation was successful,
        otherwise the appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetSetFont
(
    MSF_UINT32  msfHandle,
    MsfFont*    font
)
{
    int            retValue;
    MsfWidgetType  widgetType;
    MsfStyle       *pStyle = OP_NULL;
    MsfStyle        curStyle;
    MsfStyleHandle      *pHandle;

    if( font == OP_NULL )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
  
    retValue = getRelatedStyle(msfHandle, &widgetType, &pStyle, &pHandle);
    if( retValue < 0 )
    {
        return retValue;
    }

    if(pStyle != OP_NULL)
    {
        if(widgetType != MSF_SCREEN)
        {
            op_memcpy(&pStyle->font, font, sizeof(MsfFont));        
        }
        else
        {
            /* MsfScreen doesn't support LineStyle */
            retValue = TPI_WIDGET_ERROR_NOT_SUPPORTED;
        }
    }
    else if( pHandle )      /* the widget has style member, but not set  */
    {
        /*  get the default style settings  */
        if(widgetType != MSF_SCREEN)
        {
            retValue = widgetGetStyleInfo( OP_NULL, MSF_MAX_WIDGETTYPE, msfHandle, &curStyle );
            if( retValue >= 0 )
            {
                if( op_memcmp(&curStyle.font, font, sizeof(MsfFont)) != 0 )
                {
                    *pHandle = TPIa_widgetStyleCreate( getModID(msfHandle), 
                                                                                    &curStyle.color, 
                                                                                    &curStyle.backgroundColor,  
                                                                                    &curStyle.foreground, 
                                                                                    &curStyle.background, 
                                                                                    &curStyle.lineStyle,
                                                                                    font,                   
                                                                                    &curStyle.textProperty);
                    increase_ref_count(*pHandle);
                    TPIa_widgetRelease( *pHandle );
                }
            }
            else
            {
                *pHandle = TPIa_widgetStyleCreate( getModID(msfHandle), 
                                                                                (MsfColor*)(DefaultPropTab[widgetType].fgColors), 
                                                                                (MsfColor*)(DefaultPropTab[widgetType].bgColors), 
                                                                                (MsfPattern*)&DEFAULT_STYLE_FOREGROUND, 
                                                                                (MsfPattern*)&DEFAULT_STYLE_BACKGROUND, 
                                                                                (MsfLineStyle*)&DEFAULT_STYLE_LINESTYLE,
                                                                                font,                   
                                                                                (MsfTextProperty*)&DEFAULT_STYLE_TEXTPROPERTY);
                increase_ref_count(*pHandle);
                TPIa_widgetRelease( *pHandle );
            }
        }
        else
        {
            /* MsfScreen doesn't support LineStyle */
            retValue = TPI_WIDGET_ERROR_NOT_SUPPORTED;
        }
    }
//    return retValue;

    /*
     * We only support one kind of font 
     */
    return retValue;
}


/*==================================================================================================
    FUNCTION: TPIa_widgetFontGetFamily

    DESCRIPTION:
        Gets a font family supported by the integration.
        The integration must provide five generic font families.
        (If the integration has limited resources, five different
        fonts are not an absolute requirement, but it should be
        the goal). Generic font families are a fallback mechanism,
        which means that if none of the specified fonts can be
        selected, the generic font family is used.
        When getting a font family the module provides a list of
        desired font family names and also a fallback generic font
        family. The first font family name that matches a font
        supported by the device is chosen. If none of the specified
        font families are supported, the generic font family is used.

    ARGUMENTS PASSED:
        msfHandle   : The identifier of the MSF widget.
        nrOfFonts   : Specifies the number of font families that are
                      passed in the fontNames parameter.
        fontNames   : Specifies the list of font families that the
                      module supports, with the most preferred font
                      family first. Each font family name is
                      OP_NULL-terminated.
        genericFont : Specifies the MsfGeneric font to be used as a
                      fallback if none of the font families specified
                      in the fontNames parameter is supported by the
                      integration.
        fontFamilyNumber : The selected font family number is returned
                           in this parameter.

    RETURN VALUE:
        An integer specifying which of the provided font family names
        that was selected. 1 means that the first name in the list was
        chosen, 2 the second, and so on. 0 means that the generic font
        family was chosen. If an error occurred the appropriate return
        code is returned.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetFontGetFamily
(
    int                   nrOfFonts,
    char*                 fontNames,
    MsfGenericFont        genericFont, 
    MsfFontFamilyNumber*  fontFamilyNumber
)
{
   /*
     * We only support one kind of font 
     */
         
    return 0;
}



/*==================================================================================================
    FUNCTION: TPIa_widgetGetColor

    DESCRIPTION:
        Gets the current MsfColor of the specified object.

    ARGUMENTS PASSED:
        msfHandle : The identifier of the MSF widget.
        color     : Returns the MsfColor of the widget.
        background: Specifies if the retrieved MsfColor is the
                    background MsfColor (1) of the object or
                    the foreground MsfColor (0).
                    
    RETURN VALUE:
        A positive integer if the operation was successful,
        otherwise the appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetGetColor
(
    MSF_UINT32   msfHandle,
    MsfColor*    color,
    int          background
)
{
    int            retValue;
    MsfWidgetType  widgetType;
    MsfStyle       *pStyle = OP_NULL;
    MsfStyle        curStyle;
  
    if( color == OP_NULL )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    retValue = getRelatedStyle(msfHandle, &widgetType, &pStyle, OP_NULL);
    if( retValue < 0 )
    {
        return retValue;
    }

    if(pStyle != OP_NULL)
    {
        if( background != 0)
        {
            op_memcpy(color, &pStyle->backgroundColor, sizeof(MsfColor));        
        }
        else if(widgetType != MSF_SCREEN)
        {
            /* MsfScreen doesn't support foreground color */
            op_memcpy(color, &pStyle->color, sizeof(MsfColor));
        }
        else
        {
            retValue = TPI_WIDGET_ERROR_NOT_SUPPORTED;
        }
    }
    else        /*  the style not set , use the default style  */
    {
        retValue = widgetGetStyleInfo( OP_NULL, MSF_MAX_WIDGETTYPE, msfHandle, &curStyle );
        if( retValue >= 0 )
        {
            if( background != 0)
            {
                op_memcpy(color, (void *)&curStyle.backgroundColor, sizeof(MsfColor));        
            }
            else if(widgetType != MSF_SCREEN)
            {
                /* MsfScreen doesn't support foreground color */
                op_memcpy(color, (void *)&curStyle.color, sizeof(MsfColor));
            }
            else
            {
                retValue = TPI_WIDGET_ERROR_NOT_SUPPORTED;
            }
        }
        else
        {
            if( background != 0)
            {
                op_memcpy(color, (void *)(DefaultPropTab[widgetType].bgColors), sizeof(MsfColor));        
            }
            else if(widgetType != MSF_SCREEN)
            {
                /* MsfScreen doesn't support foreground color */
                op_memcpy(color, (void *)(DefaultPropTab[widgetType].fgColors), sizeof(MsfColor));
            }
            else
            {
                retValue = TPI_WIDGET_ERROR_NOT_SUPPORTED;
            }
        }
    }
    
    return retValue;
}

/*==================================================================================================
    FUNCTION: TPIa_widgetGetFont

    DESCRIPTION:
        Gets the current MsfFont of the specified object.

    ARGUMENTS PASSED:
        msfHandle : The identifier of the MSF widget.
        font      : Returns the MsfFont of the widget.
                    
    RETURN VALUE:
        A positive integer if the operation was successful,
        otherwise the appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetGetFont
(
    MSF_UINT32   msfHandle,
    MsfFont*     font
)
{
    int            retValue;
    MsfWidgetType  widgetType;
    MsfStyle       *pStyle = OP_NULL;
    MsfStyle        curStyle;
  
    if( font == OP_NULL )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    retValue = getRelatedStyle(msfHandle, &widgetType, &pStyle, OP_NULL);
    if( retValue < 0 )
    {
        return retValue;
    }

    if(pStyle != OP_NULL)
    {
        if(widgetType != MSF_SCREEN)
        {
            op_memcpy(font, &pStyle->font, sizeof(MsfFont));        
        }
        else
        {
            /* MsfScreen doesn't support LineStyle */
            retValue = TPI_WIDGET_ERROR_NOT_SUPPORTED;
        }
    }
    else        /*  the style not set , use the default style  */
    {
        retValue = widgetGetStyleInfo( OP_NULL, MSF_MAX_WIDGETTYPE, msfHandle, &curStyle );
        if( retValue >= 0 )
        {
            if(widgetType != MSF_SCREEN)
            {
                op_memcpy(font, (void *)&curStyle.font, sizeof(MsfFont));        
            }
            else
            {
                /* MsfScreen doesn't support LineStyle */
                retValue = TPI_WIDGET_ERROR_NOT_SUPPORTED;
            }
        }
        else
        {
            if(widgetType != MSF_SCREEN)
            {
                if( widgetType == MSF_MAINMENU || widgetType == MSF_MENU )
                {
                    *font = DEFAULT_MENU_STYLE_FONT;
                }
                else if( MSF_DIALOG == widgetType)
                {
                    *font = DEFAULT_DIALOG_STYLE_FONT;
                }
                else
                {
                    *font = DEFAULT_STYLE_FONT; 
                }
            }
            else
            {
                /* MsfScreen doesn't support LineStyle */
                retValue = TPI_WIDGET_ERROR_NOT_SUPPORTED;
            }
        }
    }
    
    return retValue;

    /*
     * We only support one kind of font 
     */
}


/*==================================================================================================
    FUNCTION: TPIa_widgetFontGetValues

    DESCRIPTION:
        Gets the values of ascent, height and X-height,
        for the specified MsfFont. 
        Figure 50 illustrates the four concepts for fonts.

    ARGUMENTS PASSED:
        font    : The MsfFont.
        ascent  : Returns the ascent of the MsfFont.
        height  : Returns the height of the MsfFont.
        xHeight : Returns the X-height of the MsfFont.
        
    RETURN VALUE:
        A positive integer if the operation was successful,
        otherwise the appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetFontGetValues
(
    MsfFont*  font, 
    int*      ascent,
    int*      height, 
    int*      xHeight
)
{
    if( font == OP_NULL || ascent == OP_NULL || height == OP_NULL || xHeight == OP_NULL )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    *height = MSF_WIDGET_FONT_HEIGHT;
    *ascent = MSF_WIDGET_FONT_HEIGHT - 2;
    *xHeight = MSF_WIDGET_FONT_HEIGHT - 2;
    
    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION: TPIa_widgetGetLineStyle

    DESCRIPTION:
        Gets the current MsfLineStyle of the specified object.

    ARGUMENTS PASSED:
        msfHandle    : The identifier of the MSF widget.
        msfLineStyle : Returns the MsfLineStyle of the widget.
        
    RETURN VALUE:
        A positive integer if the operation was successful,
        otherwise the appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetGetLineStyle
(
    MSF_UINT32     msfHandle,
    MsfLineStyle*  msfLineStyle
)
{
    int            retValue;
    MsfWidgetType  widgetType;
    MsfStyle       *pStyle = OP_NULL;
    MsfStyle        curStyle;
  
    if( msfLineStyle == OP_NULL )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    retValue = getRelatedStyle(msfHandle, &widgetType, &pStyle, OP_NULL);
    if( retValue < 0 )
    {
        return retValue;
    }

    if(pStyle != OP_NULL)
    {
        if(widgetType != MSF_SCREEN)
        {
            op_memcpy(msfLineStyle, &pStyle->lineStyle, sizeof(MsfLineStyle));        
        }
        else
        {
            /* MsfScreen doesn't support LineStyle */
            retValue = TPI_WIDGET_ERROR_NOT_SUPPORTED;
        }
    }
    else        /*  the style not set , use the default style  */
    {
        retValue = widgetGetStyleInfo( OP_NULL, MSF_MAX_WIDGETTYPE, msfHandle, &curStyle );
        if( retValue >= 0 )
        {
            if(widgetType != MSF_SCREEN)
            {
                op_memcpy(msfLineStyle, (void *)&curStyle.lineStyle, sizeof(MsfLineStyle));        
            }
            else
            {
                /* MsfScreen doesn't support LineStyle */
                retValue = TPI_WIDGET_ERROR_NOT_SUPPORTED;
            }
        }
        else
        {
            if(widgetType != MSF_SCREEN)
            {
                op_memcpy(msfLineStyle, (void *)(&DEFAULT_STYLE_LINESTYLE), sizeof(MsfLineStyle));        
            }
            else
            {
                /* MsfScreen doesn't support LineStyle */
                retValue = TPI_WIDGET_ERROR_NOT_SUPPORTED;
            }
        }
    }
    return retValue;
}


/*==================================================================================================
    FUNCTION: TPIa_widgetGetTextProperty

    DESCRIPTION:
        Gets the current MsfTextProperty of the specified object.

    ARGUMENTS PASSED:
        msfHandle    : The identifier of the MSF widget.
        textProperty : Returns the MsfTextProperty of the widget.
        
    RETURN VALUE:
        A positive integer if the operation was successful,
        otherwise the appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetGetTextProperty
(
    MSF_UINT32        msfHandle,
    MsfTextProperty*  textProperty
)
{
    int            retValue;
    MsfWidgetType  widgetType;
    MsfStyle       *pStyle = OP_NULL;
    MsfStyle        curStyle;
  
    if( textProperty == OP_NULL )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    retValue = getRelatedStyle(msfHandle, &widgetType, &pStyle, OP_NULL);
    if( retValue < 0 )
    {
        return retValue;
    }
    
    if(pStyle != OP_NULL)
    {
        if(widgetType != MSF_SCREEN)
        {
            op_memcpy(textProperty, &pStyle->textProperty, sizeof(MsfTextProperty));        
        }
        else
        {
            /* MsfScreen doesn't support LineStyle */
            retValue = TPI_WIDGET_ERROR_NOT_SUPPORTED;
        }
    }
    else        /*  the style not set , use the default style  */
    {
        retValue = widgetGetStyleInfo( OP_NULL, MSF_MAX_WIDGETTYPE, msfHandle, &curStyle );
        if( retValue >= 0 )
        {
            if(widgetType != MSF_SCREEN)
            {
                op_memcpy(textProperty, (void *)&curStyle.textProperty,  sizeof(MsfTextProperty));        
            }
            else
            {
                /* MsfScreen doesn't support LineStyle */
                retValue = TPI_WIDGET_ERROR_NOT_SUPPORTED;
            }
        }
        else
        {
            if(widgetType != MSF_SCREEN)
            {
                op_memcpy(textProperty, (void *)(&DEFAULT_STYLE_TEXTPROPERTY),  sizeof(MsfTextProperty));        
            }
            else
            {
                /* MsfScreen doesn't support LineStyle */
                retValue = TPI_WIDGET_ERROR_NOT_SUPPORTED;
            }
        }
    }
    return retValue;
}


/*==================================================================================================
    FUNCTION: TPIa_widgetGetPattern

    DESCRIPTION:
        Gets the current MsfPattern of the specified object.

    ARGUMENTS PASSED:
        msfHandle  : The identifier of the MSF widget.
        background : Specifies if the retrieved MsfPattern is the
                     background pattern (1) of the widget or the
                     foreground pattern (0).
        pattern    : Returns the MsfPattern of the specified widget.
        
    RETURN VALUE:
        A positive integer if the operation was successful,
        otherwise the appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetGetPattern
(
    MSF_UINT32    msfHandle,
    int           background,
    MsfPattern*   pattern
)
{
    int            retValue;
    MsfWidgetType  widgetType;
    MsfStyle       *pStyle = OP_NULL;
    MsfStyle        curStyle;
  
    if( pattern == OP_NULL )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    retValue = getRelatedStyle(msfHandle, &widgetType, &pStyle, OP_NULL );
    if( retValue < 0 )
    {
        return retValue;
    }

    if(pStyle != OP_NULL)
    {
        if(background != 0)
        {
            op_memcpy(pattern, &pStyle->background, sizeof(MsfPattern));        
        }
        else if(widgetType != MSF_SCREEN)
        {
            /* MsfScreen doesn't support foreground pattern */
            op_memcpy(pattern, &pStyle->foreground, sizeof(MsfPattern));        
        }
        else
        {
            retValue = TPI_WIDGET_ERROR_NOT_SUPPORTED;
        }
    }
    else        /*  the style not set , use the default style  */
    {
        retValue = widgetGetStyleInfo( OP_NULL, MSF_MAX_WIDGETTYPE, msfHandle, &curStyle );
        if( retValue >= 0 )
        {
            if(background != 0)
            {
                op_memcpy(pattern, (void *)&curStyle.background, sizeof(MsfPattern));        
            }
            else if(widgetType != MSF_SCREEN)
            {
                /* MsfScreen doesn't support foreground pattern */
                op_memcpy(pattern, (void *)&curStyle.foreground, sizeof(MsfPattern));        
            }
            else
            {
                retValue = TPI_WIDGET_ERROR_NOT_SUPPORTED;
            }
        }
        else
        {
            if(background != 0)
            {
                op_memcpy(pattern, (void *)(&DEFAULT_STYLE_BACKGROUND), sizeof(MsfPattern));        
            }
            else if(widgetType != MSF_SCREEN)
            {
                /* MsfScreen doesn't support foreground pattern */
                op_memcpy(pattern, (void *)(&DEFAULT_STYLE_FOREGROUND), sizeof(MsfPattern));        
            }
            else
            {
                retValue = TPI_WIDGET_ERROR_NOT_SUPPORTED;
            }
        }
    }
    return retValue;
}

int TPIa_widgetSetMarquee(MSF_UINT32 handle, MsfMarqueeType marqueeType, 
                          int direction, int repeat, int scrollAmount, 
                          int scrollDelay){return 0;}


/****************************************************************
 STRING
 ***************************************************************/
/*==================================================================================================
    FUNCTION: TPIa_widgetStringCreate

    DESCRIPTION:
        Creates an MsfString from the data provided in the char[]
        stringData parameter and returns the MsfStringHandle. The
        format of the stringData is specified by the stringFormat
        parameter.

    ARGUMENTS PASSED:
        modId        : The MSF Module identifier
        stringData   : Specifies the actual data of the MsfString
                       in the specified MsfStringFormat and of the
                       specified length.
        stringFormat : Specifies the MsfStringFormat of the string
                       data.
        length       : Specifies the length of the string data.
        defaultStyle : The MsfStyle identifier. If this parameter
                       is 0 the default style of the device will
                       be used.
        
    RETURN VALUE:
        returns The new MsfStringHandle if the operation was
        successful, otherwise 0.

    IMPORTANT NOTES:
        None
==================================================================================================*/
#ifndef WIDGET_DEBUG
MsfStringHandle TPIa_widgetStringCreate( MSF_UINT8 modId, const char* stringData, MsfStringFormat stringFormat, int length, MsfStyleHandle  defaultStyle )
{
#else
MsfStringHandle WIDGET_DEBUG_STRING_CREATE( MSF_UINT8 modId, const char* stringData, MsfStringFormat stringFormat, int length, MsfStyleHandle defaultStyle, const char *filename, int lineno )
{
#endif
    MsfString    *pStr;
    OP_UINT8     *pData;
    int          iLen, iSize;
    int          iCpyLen;
    int          iRet;

    /* 1. save the stringData */
    if( stringFormat != MsfUtf8 )    /* regard as unicode */
    {
        iLen = length  + length % 2;

        iSize =  iLen + 2 + sizeof(MsfString);
        pStr = (MsfString*)WIDGET_ALLOC( iSize );
        if( !pStr )
        {
            return INVALID_HANDLE;
        }
        
        op_memset( pStr, 0, iSize );  
        pData = ((OP_UINT8*)pStr) + sizeof(MsfString);
        
        iCpyLen = Ustrlen((OP_UINT8*)stringData);
        if( iCpyLen > iLen )
        {
        	iCpyLen = iLen;
        }
        op_memcpy( pData, stringData, iCpyLen);
    }
    else        /*need to convert to unicode */    
    {
        iLen = 0x007fffff;
        iRet = utf8_to_unicode(stringData, &length, OP_NULL, &iLen);
        if( iRet < 0 )
        {
            return INVALID_HANDLE;
        }
        
        iSize =  iLen + 2 + sizeof(MsfString);
        pStr = (MsfString*)WIDGET_ALLOC( iSize );
        if( !pStr )
        {
            return INVALID_HANDLE;
        }
        
        op_memset( pStr, 0, iSize );  
        pData = ((OP_UINT8*)pStr) + sizeof(MsfString);

        utf8_to_unicode(stringData, &length, (char *)pData, &iLen);

        pData[iLen] = 0;
        pData[iLen+1] = 0;
        
    }

    /* 2. form the MsfString */
    pStr->data = pData;
    pStr->modId = modId;
    pStr->handle = getNewHandle(modId, MSF_STRING);
    pStr->format = MsfUnicode;
    pStr->length = iLen;
    pStr->style = defaultStyle;
    pStr->referenceCounter = 1;

    SP_list_insert_to_head(&widgetQueue[MSF_STRING], pStr);
    
#ifdef   WIDGET_DEBUG
    {
        const char *pPtr;
        
        pPtr = strrchr(filename, '\\');
        if( pPtr )
        {
            pPtr++;
            op_printf( "\n%s line=%d [STRING] ALLOC:ptr=0x%08p\n", pPtr, lineno, (void*)(pStr->handle) );
        }
    }
#endif    

    return pStr->handle;
}

/*==================================================================================================
    FUNCTION: TPIa_widgetStringGetPredefined 

    DESCRIPTION:
    Retrieves a predefined static MsfString using the resId parameter and returns the
    MsfStringHandle. The resId parameter must be a globally unique identifier.
    Predefined static MsfStrings do not have to be released. If the module tries to release a
    predefined static MsfString, the integration just ignores it. Predefined static
    MsfStrings can typically be used when the MsfString is not likely to be changed or
    manipulated, for example titles, tickers, MsfDialog texts,and MsfChoice elements.

    ARGUMENTS PASSED:
    resId    : Specifies a globally unique string resource identifier.   
  
    RETURN VALUE:
    The MsfStringHandle if the operation was successful, otherwise 0.

    IMPORTANT NOTES:
        Need to discuss
==================================================================================================*/
MsfStringHandle TPIa_widgetStringGetPredefined (MSF_UINT32 resId)
{
    OP_UINT8    modId;
    OP_UINT8    lang;
    OP_UINT16   index;
    MSF_UINT32  stringHandle;

    /* The lower 8 bit of all the resId is its modId */
    modId = (OP_UINT8)(resId & 0xFF);
    index = (OP_UINT16)((resId >> 8) - 1);
    /*
     *   The predefined string handle is a UINT32 value, and it's content is:
     *   _____________________________________________________________________________
     *   | modId (8 bits) | widgetType (8 bits) | lang (4 bits) | index (12 bits)    |
     *   |----------------------------------------------------------------------------
     */
     
    if (RM_GetCurrentLanguage() == RM_LANG_CHN)
    {
        lang = 0;
        stringHandle = ((OP_UINT32)modId << 24) | ((OP_UINT32)MSF_STRING_RES << 16) | ((OP_UINT32)index);
    }
    else
    {   
        lang = 1;
        stringHandle = ((OP_UINT32)modId << 24) | ((OP_UINT32)MSF_STRING_RES << 16) | ((OP_UINT32)index) | 0x00001000;
    }
    
    if( widgetGetPredefinedStringEx(modId, lang, index) == OP_NULL ) /* not found the string */
    {
        stringHandle = INVALID_HANDLE;
    }

    return stringHandle;
}
/*==================================================================================================
    FUNCTION:  TPIa_widgetStringCreateText

    DESCRIPTION:
    Creates an MsfString from the text provided in the specified MsfEditor/MsfTextInput
    and returns the new MsfStringHandle. This function is also applicable to MsfDialog
    of type MsfPrompt, where the parameter index specifies the input field on the
    MsfDialog. Note that the module is responsible for deleting the created MsfString.

    ARGUMENTS PASSED:
    modId     : The MSF Module identifier
    handle     : The identifier of the MsfEditor or MsfTextInput or MsfDialog of type MsfPrompt
    index     : The index of the input field, in the case of an MsfDialog of type MsfPrompt. 
                   This parameter is not used in the case of an MsfEditor or MsfTextInput.      
                   
    RETURN VALUE:
    The new MsfStringHandle if the operation was successful,
    otherwise 0

    IMPORTANT NOTES:
        None
==================================================================================================*/
#ifndef WIDGET_DEBUG
MsfStringHandle TPIa_widgetStringCreateText (MSF_UINT8 modId, MSF_UINT32 handle, int index)
{
#else
MsfStringHandle WIDGET_DEBUG_STRING_CREATE_TEXT( MSF_UINT8 modId, MSF_UINT32 handle, int index, const char *filename, int lineno)
{
#endif                                             
    MsfDialog         *pDlg;
    MsfInput          *pInput;
    MsfTextInput      *pTextInput;
    MsfStringHandle   hRet;
    MsfWidgetType     widgetType;
    MsfString         *pStr;
    int               iStrLen;
    
    hRet = INVALID_HANDLE;
    pInput =OP_NULL;
    
    if( ISVALIDHANDLE(handle) )
    {
        pDlg = (MsfDialog*)seekWidget(handle, &widgetType);
        if(pDlg )
        {
            if(widgetType == MSF_DIALOG && pDlg->type == MsfPrompt )
            {
                pTextInput = widgetDialogGetInputByIndex( pDlg, index);
                if( pTextInput )
                {
                    pInput = &pTextInput->input;
                }
            }
            else if( widgetType== MSF_EDITOR) 
            {
                pInput = &(((MsfEditor*)pDlg)->input->input);
            }
            else if (widgetType == MSF_TEXTINPUT )
            {
                pInput = &(((MsfTextInput*)pDlg)->input);
            }
        }
        
        if( pInput )
        {
            
            iStrLen = TBM_GetContentLen(&(pInput->txtInputBufMng), OP_TRUE) + 2;

#ifndef  WIDGET_DEBUG
            hRet = TPIa_widgetStringCreate(modId,
                                           "",
                                           pInput->txtInputBufMng.format,
                                           iStrLen,
                                           pInput->txtInputBufMng.hStyle);  
#else
            hRet = WIDGET_DEBUG_STRING_CREATE(modId,
                                           "",
                                           pInput->txtInputBufMng.format,
                                           iStrLen,
                                           pInput->txtInputBufMng.hStyle,
                                           filename, lineno);  
#endif

            if( hRet != 0 )
            {
                pStr =(MsfString*)seekWidget( hRet, OP_NULL);
                TBM_GetContent(&(pInput->txtInputBufMng), OP_TRUE, pStr->data, 0);
            }
        }
    }
    
    return hRet;
}  
  /*==================================================================================================
      FUNCTION:  TPIa_widgetStringGetLength
  
      DESCRIPTION:
        Returns the number of characters or the length in bytes of the specified MsfString.
        The length in bytes always includes a null termination. The null termination is not
        included when number of characters is calculated.  
        
      ARGUMENTS PASSED:
        string         : The MsfString identifier.
        lengthInBytes : Specifies if this function returns the number of characters contained in the specified MsfString (0) or
                      if the return value is the length of the MsfString in bytes if the MsfStringFormat format is used.
        format         : Specifies the MsfStringFormat to be used if the length is to be calculated in bytes.          

      RETURN VALUE:
        The number of characters contained in the specified    MsfString, if the operation was successful, 
        otherwise the appropriate return code.

      IMPORTANT NOTES:
          None
  ==================================================================================================*/
int TPIa_widgetStringGetLength(MsfStringHandle string, int lengthInBytes, MsfStringFormat format)
{
    int            iLen;
    int            iSize;
    OP_UINT8       *pChar;

    WIDGET_GET_STRING_DATA(pChar, string);
    
    if( !pChar )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    iSize = Ustrlen(pChar);
    
    
    if( lengthInBytes == 0 )
    {
        return iSize / 2;
    }
    
    if( format != MsfUtf8 )    /* regard as Unicode */
    {
        return iSize + 2;
    }
    else            /* MsfUtf8 format, need to transform */
    {
        iLen = 10000;
        unicode_to_utf8((char *)pChar, &iSize, OP_NULL, &iLen);

        return iLen + 1;    /* include a null termination */
    }
}
  
  /*==================================================================================================
      FUNCTION:  TPIa_widgetStringGetData
  
      DESCRIPTION:
        Retrieves the data contained in the specified MsfString and puts it in buffer.
        The     returned string is always null-terminated.

      ARGUMENTS PASSED:
        string     : The MsfString identifier.
        buffer     : The buffer in which the actual MsfString data is returned.
        format     : Specifies the MsfStringFormat to be used in the returned buffer.

      RETURN VALUE:
        A positive integer if the operation was successful, 
        otherwise the appropriate return code is given.
        
      IMPORTANT NOTES:
          None
  ==================================================================================================*/
int TPIa_widgetStringGetData(MsfStringHandle string, char* buffer, MsfStringFormat format)
{
    OP_UINT8       *pChar;
    int            srcLen;
    int            dstLen;

    if(!buffer )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    WIDGET_GET_STRING_DATA(pChar, string);
    
    if( !pChar )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    if( format != MsfUtf8 )        /* regard as unicode */
    {
        Ustrcpy((OP_UINT8*)buffer, pChar );
    }
    else
    {
        dstLen = 10000;
        srcLen = Ustrlen(pChar);
        unicode_to_utf8((char*)pChar, &srcLen, buffer, &dstLen);
        buffer[dstLen]   = '\0';   
    }

    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  TPIa_widgetStringCompare

    DESCRIPTION:
    Compares two MsfStrings. The return value indicates the lexicographic relation of
    string1 to string2. Lexicographic means that if two MsfStrings are different, then
    either they have different characters at some index that is a valid index for both
    MsfStrings, or their lengths are different, or both. If they have different characters at
    one or more index positions, let k be the smallest such index; then the MsfString
    whose character at position k has the smaller value, as determined by using the <
    operator, lexicographically precedes the other MsfString. If there is no index position
    at which they differ, then the shorter MsfString lexicographically precedes the longer
    MsfString.

    ARGUMENTS PASSED:
    String1     : The first MsfString identifier.
    String2     :The second MsfString identifier.  
    
    RETURN VALUE:
    Zero if the MsfStrings match. A negative integer if string1 is less
    than string2 and a positive integer if string1 is greater than
    string2.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetStringCompare
(
    MsfStringHandle string1,
    MsfStringHandle string2
)
{
    OP_UINT8     *pChar1;
    OP_UINT8     *pChar2;

    WIDGET_GET_STRING_DATA(pChar1, string1);
    WIDGET_GET_STRING_DATA(pChar2, string2);
        
    if( !pChar1 )
    {
        if( pChar2 )
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
    else if( !pChar2 )
    {
        return 1;
    }

    return Ustrcmp( pChar1, pChar2 );
}

/*==================================================================================================
    FUNCTION:  TPIa_widgetStringGetVisible

    DESCRIPTION:
    It calculates the number of characters of the whole words, 
    counted from the index parameter of the MsfString, that 
    would be visible in a bounding rectangle of the specified 
    MsfSize when it is drawn according to its current style settings.

    ARGUMENTS PASSED:
    string     :Specifies the MsfString.
    handle     :MsfPaintbox or MsfImage identifier. If this parameter is a valid handle the size and 
             subwidth will be calculated using the style values of the     MsfPaintbox or MsfImage
             brush. If this parameter is 0 the style values of the MsfString will be used when
             calculating the size and subwidth.
    size         :Specifies the MsfSize of the rectangle.
    startIndex :Specifies the index in the MsfString from where the calculation starts.
    includeInitialWhiteSpaces : Specifies if the calculation will include any whitespace characters 
            in the beginning of the partial MsfString (the partial MsfString     means that the 
            calculation always starts from the index given in the startIndex parameter). If this 
            parameter is 1 the function will include any whitespaces in the beginning of the
            partial MsfString when the nbrOfCharacters parameter is calculated and the 
            nbrOfInitialWhiteSpaces parameter will be ignored, that is, the integration will not 
            calculate the number of whitespaces in the beginning of the MsfString, since they 
            are included in the nbrOfCharacters parameter. If this parameter is 0 the function 
            will disregard any whitespaces in the beginning of the partial MsfString when the 
            nbrOfCharacters parameter is calculated and the nbrOfInitialWhiteSpaces parameter
            will return the number of whitespaces that were disregarded in the beginning of 
            the MsfString.
    nbrOfCharacters : Returns the number of characters of the whole words, counted from the 
            specified index of the MsfString, that would be visible in a bounding rectangle of the 
            specified size when it is drawn according to the current style settings of the 
            specified object.
    nbrOfEndingWhiteSpaces :Returns the number of whitespace characters in the end of the 
            partial string specified by the nbrOfCharacters parameter.
    nbrOfInitialWhiteSpaces  :Returns the number of whitespace characters in the beginning of
            the partial string specified by the nbrOfCharacters parameter. This parameter is
            ignored if the includeInitialWhiteSpaces parameter is set to 1.            
            
    RETURN VALUE:
    A positive integer if the operation was successful, 
    otherwise the appropriate error code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetStringGetVisible
(
    MsfStringHandle string, 
    MSF_UINT32 handle, 
    MsfSize* size, 
    int startIndex, 
    int includeInitialWhiteSpaces,
    int* nbrOfCharacters,
    int* nbrOfEndingWhiteSpaces, 
    int* nbrOfInitialWhiteSpaces
)
{
    MSF_UINT32     hWidget;
    MsfWindow      *pWin;
    MsfWidgetType  widgetType;
    OP_UINT8       *pChar;
    DS_FONTATTR    fontAttr;
    OP_INT16       iEngCharWidth, iEngCharHeight;
    int            iRet;

    if( !ISVALIDHANDLE(handle) 
      ||!ISVALIDHANDLE(string)    
      || size == OP_NULL 
      || nbrOfCharacters == OP_NULL 
      || nbrOfEndingWhiteSpaces == OP_NULL
      || nbrOfInitialWhiteSpaces == OP_NULL 
      || startIndex < 0 )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    /*
      * NOTE: not support the image for the moment 
      */
    pWin = (MsfWindow*)seekWidget( handle, &widgetType );
    if( pWin && IS_WINDOW(widgetType))  
    {
        WIDGET_GET_STRING_DATA(pChar, string);
            
        if( !pChar )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }
        
        if( startIndex >= Ustrlen(pChar) / 2 )
        {
            *nbrOfCharacters = 0;
            *nbrOfEndingWhiteSpaces = 0;
            *nbrOfInitialWhiteSpaces = 0;
            
            return TPI_WIDGET_OK;
        }
        
        pChar += startIndex * 2;

        if( widgetType == MSF_PAINTBOX )
        {
            hWidget = ((MsfPaintbox*)pWin)->brush;
        }
        else
        {
            hWidget = handle;
        }
        
        iRet = widgetGetDrawingInfo( 
                                    OP_NULL, 
                                    MSF_MAX_WIDGETTYPE,  
                                    hWidget, 
                                    &fontAttr, 
                                    OP_NULL, 
                                    OP_NULL);       
        if( iRet >= 0 )
        {
            MsfSize   rectSize;
            
            if( 0 > widgetGetFontSizeInEngChar(fontAttr, &iEngCharWidth, &iEngCharHeight))
            {
                return TPI_WIDGET_ERROR_UNEXPECTED;
            }

            rectSize.width = size->width;
            rectSize.height = size->height;
            if( rectSize.height <= 0 )
            {
                rectSize.height = iEngCharWidth << 1;
            }
            
            iRet = widgetStringGetVisibleByWord( 
                                     pChar, 
                                     &rectSize, 
                                     iEngCharWidth, 
                                     (OP_BOOLEAN)includeInitialWhiteSpaces, 
                                     nbrOfCharacters, 
                                     nbrOfEndingWhiteSpaces, 
                                     nbrOfInitialWhiteSpaces);
        }                  

        return iRet;
    }
    
    return TPI_WIDGET_ERROR_UNEXPECTED;
}


/*==================================================================================================
    FUNCTION:  TPIa_widgetStringGetRectSize

    DESCRIPTION:
    It calculates the occupied rectange area of the string when it can be whole visible 
    in the window specified by the handle.

    ARGUMENTS PASSED:
    string     :Specifies the MsfString.
    handle     :window identifier. 
    size         :Specifies the width of the rectangle, and return the result size.
            
    RETURN VALUE:
    A positive integer if the operation was successful, 
    otherwise the appropriate error code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetStringGetRectSize
(
    MsfStringHandle string, 
    MSF_UINT32 handle, 
    MsfSize* size
)
{
    MSF_UINT32     hWidget;
    OP_INT16	   iVisibleCharWidth=0;
    MsfWindow      *pWin;
    MsfWidgetType  widgetType;
    OP_UINT8       *pChar;
    DS_FONTATTR    fontAttr;
    OP_INT16       iEngCharWidth, iEngCharHeight;
    MsfSize        rectSize;
    int            nVisibleLineChars, nbrOfInitialWhiteSpaces;
    int            iRet;

    if( !ISVALIDHANDLE(handle) 
      ||!ISVALIDHANDLE(string)    
      || size == OP_NULL )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    /*
      * NOTE: not support the image for the moment 
      */
    pWin = (MsfWindow*)seekWidget( handle, &widgetType );
    if( pWin && IS_WINDOW(widgetType))  
    {
        WIDGET_GET_STRING_DATA(pChar, string);
            
        if( !pChar )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }

        if( widgetType == MSF_PAINTBOX )
        {
            hWidget = ((MsfPaintbox*)pWin)->brush;
        }
        else
        {
            hWidget = handle;
        }
        
        iRet = widgetGetDrawingInfo( 
                                    OP_NULL, 
                                    MSF_MAX_WIDGETTYPE,  
                                    hWidget, 
                                    &fontAttr, 
                                    OP_NULL, 
                                    OP_NULL);       
        if( iRet >= 0 )
        {
            
            if( 0 > widgetGetFontSizeInEngChar(fontAttr, &iEngCharWidth, &iEngCharHeight))
            {
                return TPI_WIDGET_ERROR_UNEXPECTED;
            }

            rectSize.width = size->width;
            rectSize.height = GADGET_LINE_HEIGHT * 2;  /* for call widgetStringGetVisibleByChar smoothly */
            size->height = 0;
            
            while( *pChar != 0x00 || *(pChar+ 1) != 0x00 )
            {
                iVisibleCharWidth=widgetStringGetVisibleByChar( 
                                            pChar, 
                                            &rectSize, 
                                            iEngCharWidth, 
                                            OP_TRUE, 
                                            &nVisibleLineChars, 
                                            &nbrOfInitialWhiteSpaces);

                if( nVisibleLineChars <= 0 )  /* avoid dead loop  */
                {
                    break;
                }
                
                pChar += ( nVisibleLineChars << 1);
                size->height += GADGET_LINE_HEIGHT;
            }
            if(size->height==GADGET_LINE_HEIGHT)
                size->width=iVisibleCharWidth;
        }                  

        return iRet;
    }
    
    return TPI_WIDGET_ERROR_UNEXPECTED;
}

/*==================================================================================================
    FUNCTION:  TPIa_widgetStringGetSubSize

    DESCRIPTION:
    This function is mostly usable for text rendering. It calculates the MsfSize of an
    MsfString or parts of an MsfString.

    ARGUMENTS PASSED:
    string     :The MsfString identifier.
    handle     :MsfPaintbox or MsfImage identifier. If this parameter is a valid handle 
             the size and subwidth will be calculated using the style values of the
             MsfPaintbox or MsfImage brush. If this parameter is 0 the style values 
             of the MsfString will be used when calculating the size and subwidth.
    index     :Specifies the start index of the part of the string that the MsfSize is 
             to be calculated from.
    nbrOfChars:Specifies the number of characters in the MsfString, starting from 
             index, to calculate the MsfSize from. 0 specifies that the MsfSize is 
             calculated on the rest of the string from index counted.
    subwidth :Specifies if the returned MsfSize is the total MsfSize of the specified 
             substring (0) or if the returned MsfSize is the MsfSize of the widest 
             word in the specified substring (1).
    size         :Returns the MsfSize of the specified part of the MsfString, according to 
             the current style settings of the MsfString. If the subwidth parameter is 
             set to 1, this parameter returns the MsfSize of the word that has the 
             largest width in the specified substring.
        
    RETURN VALUE:
    A positive integer if the operation was successful, 
    otherwise the appropriate error code.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetStringGetSubSize
(
    MsfStringHandle string, 
    MSF_UINT32 handle,
    int index, 
    int nbrOfChars, 
    int subwidth, 
    MsfSize* size
)
{
    MSF_UINT32     hWidget;
    MsfWindow      *pWin;
    MsfWidgetType  widgetType;
    OP_UINT8       *pChar;
    DS_FONTATTR    fontAttr;
    int            iRet;
    OP_INT16       iEngCharWidth, iEngCharHeight;
    
    if( !ISVALIDHANDLE(handle) 
      ||!ISVALIDHANDLE(string)    
      || size == OP_NULL 
      || index < 0 )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    /*
      * NOTE: not support the image for the moment 
      */
    pWin = (MsfWindow*)seekWidget( handle, &widgetType );
    if( pWin && IS_WINDOW(widgetType))  
    {
        WIDGET_GET_STRING_DATA(pChar, string);

        if( !pChar )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }

        if( index >= Ustrlen(pChar) / 2 )
        {
            size->width= 0;
            size->height = 0;
            
            return TPI_WIDGET_OK;
        }

        pChar += index * 2;
        
        if( widgetType == MSF_PAINTBOX )
        {
            hWidget = ((MsfPaintbox*)pWin)->brush;
        }
        else
        {
            hWidget = handle;
        }
        iRet = widgetGetDrawingInfo( OP_NULL, 
                                     MSF_MAX_WIDGETTYPE,  
                                     hWidget, 
                                     &fontAttr, 
                                     OP_NULL, 
                                     OP_NULL);       
        if( iRet >= 0 )
        {
            /* get font size */
            if( 0> widgetGetFontSizeInEngChar(fontAttr, &iEngCharWidth, &iEngCharHeight ))
            {
                return TPI_WIDGET_ERROR_UNEXPECTED;
            }
            
            iRet = widgetStringGetSubSize( pChar, iEngCharWidth, nbrOfChars, subwidth, size );
        }                  

        return iRet;
    }
    
    return TPI_WIDGET_ERROR_UNEXPECTED;
}

/****************************************************************
 ICON
 ***************************************************************/
/*==================================================================================================
    FUNCTION:  TPIa_widgetIconCreate

    DESCRIPTION:
        Creates an MsfIcon of the specified MsfIconType and returns the MsfIconHandle.
        Note that when an MsfIcon is drawn using the TPIa_widgetDrawIcon() function, it
        must be drawn according to the current MsfStyle settings of the MsfIcon, unless the
        MsfIcon is created with the default MsfStyle, that is, the defaultStyle parameter is 0,
        and no MsfStyle has been explicitly set for the MsfIcon. In that case the MsfIcon must
        be drawn according to the current MsfStyle settings of the current brush.

    ARGUMENTS PASSED:
        modId         :The MSF Module identifier
        iconType         :Specifies the MsfIconType of the MsfIcon to be created.
        defaultStyle     :The MsfStyle identifier. If this parameter is 0 the default
                     style of the device will be used.
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
MsfIconHandle TPIa_widgetIconCreate 
(
    MSF_UINT8       modId, 
    MsfIconType     iconType, 
    MsfStyleHandle  defaultStyle
)
{
    MsfIcon    *pIcon;

    pIcon = WIDGET_NEW(MsfIcon);
    if(!pIcon)
    {
        return  INVALID_HANDLE;
    }
    else
    {
        op_memset(pIcon, 0, sizeof(MsfIcon));
        pIcon->modId = modId;
        pIcon->handle = getNewHandle( modId, MSF_ICON);
        replaceHandle((OP_UINT32 *)&pIcon->style, defaultStyle);
        pIcon->referenceCounter = 1;

        SP_list_insert_to_head(&widgetQueue[MSF_ICON], pIcon);

        return pIcon->handle;
    }
}


/****************************************************************
 IMAGE
 ***************************************************************/
/*==================================================================================================
    FUNCTION:  TPIa_widgetImageCreate

    DESCRIPTION:
        Creates an MsfImage from the data provided in the imageData parameter
        and returns the MsfImageHandle. The format of the MsfImage data is
        specified by the imageFormat parameter. The moreData parameter indicates 
        whether all of the MsfImage data was provided or if there is more to come.

    ARGUMENTS PASSED:
        modId        : The MSF Module identifier
        imageData    : Specifies the actual data of the MsfImage in the
                       specified MsfImageFormat and of the specified length.
        imageDataSize: Specifies the length of the image data.
        imageFormat  : Specifies the MsfImageFormat of the image data.
        moreData     : Specifies if all of the image data is contained in the
                       imageData parameter (0) or if there is more image data
                       to come (1).
        defaultStyle : The MsfStyle identifier used for the brush style
                       settings. If this parameter is 0 the default style of the
                       device will be used.
        
    RETURN VALUE:
        The new MsfImageHandle if the operation was successful, otherwise 0

    IMPORTANT NOTES:
        None
==================================================================================================*/
#ifndef WIDGET_DEBUG
MsfImageHandle TPIa_widgetImageCreate( MSF_UINT8 modId,  const char* imageData, int imageDataSize, MsfImageFormat  imageFormat, int moreData, MsfStyleHandle  defaultStyle)
#else
MsfImageHandle WIDGET_DEBUG_IMAGE_CREATE( MSF_UINT8 modId,  const char* imageData, int imageDataSize, MsfImageFormat imageFormat, int moreData, MsfStyleHandle defaultStyle, const char *filename, int lineno)
#endif
{
    MsfImage       *pImage;

    pImage = WIDGET_NEW(MsfImage);
    
    if(pImage == OP_NULL)
    {
        return  INVALID_HANDLE;
    }
    op_memset(pImage, 0x00, sizeof(MsfImage));
    
    pImage->data = (MSF_UINT8*)WIDGET_ALLOC(imageDataSize);
    if(pImage->data == OP_NULL)
    {
        WIDGET_FREE(pImage);
        return INVALID_HANDLE;
    }
    op_memset(pImage->data, 0, imageDataSize);
    op_memcpy(pImage->data, imageData, imageDataSize);
    pImage->bufferSize = imageDataSize;
    pImage->dataSize = imageDataSize;
    pImage->modId = modId;
    pImage->handle = getNewHandle( modId, MSF_IMAGE);
    pImage->referenceCounter = 1;
    replaceHandle((OP_UINT32 *)&pImage->style, defaultStyle);
    pImage->format = imageFormat;
    pImage->moreData = moreData;
    pImage->bOriginal = MSF_TRUE;
    
    SP_list_insert_to_head(&widgetQueue[MSF_IMAGE], pImage);
    
#ifdef   WIDGET_DEBUG
    {
        const char *pPtr;
        
        pPtr = strrchr(filename, '\\');
        if( pPtr )
        {
            pPtr++;
            op_printf( "\n%s line=%d [IMAGE] ALLOC:ptr=0x%08p\n", pPtr, lineno, (void*)(pImage->handle) );
        }
    }
#endif    

    WIDGET_TRACE("---- TPIa_widgetImageCreate --end---%d-- \n", imageFormat );
    return pImage->handle;

}


/*==================================================================================================
    FUNCTION:  TPIa_widgetImageCreateEx

    DESCRIPTION:
        Creates an MsfImage from the data provided in the imageData parameter
        and returns the MsfImageHandle. The format of the MsfImage data is
        specified by the imageFormat parameter. The moreData parameter indicates 
        whether all of the MsfImage data was provided or if there is more to come.

    ARGUMENTS PASSED:
        modId        : The MSF Module identifier
        imageData    : Specifies the actual data of the MsfImage in the
                       specified MsfImageFormat and of the specified length.
        imageDataSize: Specifies the length of the image data.
        totalSize    : total size of the image data
        imageFormat  : Specifies the MsfImageFormat of the image data.
        moreData     : Specifies if all of the image data is contained in the
                       imageData parameter (0) or if there is more image data
                       to come (1).
        defaultStyle : The MsfStyle identifier used for the brush style
                       settings. If this parameter is 0 the default style of the
                       device will be used.
        
    RETURN VALUE:
        The new MsfImageHandle if the operation was successful, otherwise 0

    IMPORTANT NOTES:
        None
==================================================================================================*/
#ifndef WIDGET_DEBUG
MsfImageHandle TPIa_widgetImageCreateEx( MSF_UINT8 modId, const char* imageData, int imageDataSize, int totalSize, MsfImageFormat  imageFormat, int moreData, MsfStyleHandle  defaultStyle)
#else
MsfImageHandle WIDGET_DEBUG_IMAGE_CREATE_EX( MSF_UINT8 modId, const char* imageData, int imageDataSize, int totalSize, MsfImageFormat imageFormat, int moreData, MsfStyleHandle  defaultStyle, const char *filename, int lineno)
#endif
{
    MsfImage       *pImage;

    if(totalSize <= 0)
    {
        return INVALID_HANDLE;
    }
    
    pImage = WIDGET_NEW(MsfImage);
    if(pImage == OP_NULL)
    {
        return  INVALID_HANDLE;
    }
    op_memset(pImage, 0x00, sizeof(MsfImage));
    
    pImage->data = (MSF_UINT8*)WIDGET_ALLOC(totalSize);
    if(pImage->data == OP_NULL)
    {
        WIDGET_FREE(pImage);
        return INVALID_HANDLE;
    }
    op_memset(pImage->data, 0, totalSize);
    op_memcpy(pImage->data, imageData, imageDataSize);
    pImage->bufferSize = totalSize;
    pImage->dataSize = imageDataSize;
    pImage->modId = modId;
    pImage->handle = getNewHandle( modId, MSF_IMAGE);
    pImage->referenceCounter = 1;
    replaceHandle((OP_UINT32 *)&pImage->style, defaultStyle);
    pImage->format = imageFormat;
    pImage->moreData = moreData;
    pImage->bOriginal = MSF_TRUE;
    
    SP_list_insert_to_head(&widgetQueue[MSF_IMAGE], pImage);

#ifdef   WIDGET_DEBUG
    {
        const char *pPtr;
        
        pPtr = strrchr(filename, '\\');
        if( pPtr )
        {
            pPtr++;
            op_printf( "\n%s line=%d [IMAGE] ALLOC:ptr=0x%08p\n", pPtr, lineno, (void*)(pImage->handle) );
        }
    }
#endif

    return pImage->handle;

}

/*==================================================================================================
    FUNCTION:  TPIa_widgetImageNextPart

    DESCRIPTION:
        Creates the next part of an MsfImage from the data provided in
        the imageData parameter. The format of the MsfImage data is the 
        same as when the first part of the MsfImage was created. The 
        moreData parameter indicates whether all of the MsfImage data 
        was provided or if there is more to come.

    ARGUMENTS PASSED:
        image         : The MsfImage identifier.
        imageData     : Specifies the actual data of the MsfImage in the
                        specified MsfImageFormat and of the specified length.
        imageDataSize : Specifies the length of the image data.
        moreData      : Specifies if all of the image data is contained 
                        in the imageData parameter (0) or if there is more 
                        image data to come (1).

    RETURN VALUE:
        A positive integer if the operation was successful, otherwise 
        the appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetImageNextPart 
(
    MsfImageHandle image,
    char*          imageData,
    int            imageDataSize,
    int            moreData
)
{
    MsfImage*       pImage;
    MsfWidgetType    widgetType;
    MSF_UINT16      buflen;
    MSF_UINT8       *pbuf;


    /* get pImage */
    pImage = seekWidget(image, &widgetType);
    if(pImage == OP_NULL)
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }
    
    buflen = pImage->dataSize + imageDataSize;
    if(buflen > pImage->bufferSize)
    {
        pbuf = WIDGET_ALLOC(buflen);
        if(pbuf == OP_NULL)
        { 
            if(pImage->data) WIDGET_FREE(pImage->data);
            pImage->dataSize = pImage->bufferSize = 0;
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }

        if(pImage->data)
        {
            op_memcpy(pbuf, pImage->data, pImage->dataSize);
            op_memcpy((pbuf+pImage->dataSize), imageData, imageDataSize);
            WIDGET_FREE(pImage->data);
            pImage->bufferSize = pImage->dataSize = buflen;
        }
        else
        {
            op_memcpy(pbuf, imageData, imageDataSize);
            pImage->bufferSize = pImage->dataSize = imageDataSize;
        }
        pImage->data = pbuf;
    }
    else
    {
        op_memcpy((pImage->data+pImage->dataSize), imageData, imageDataSize);
        pImage->dataSize = buflen;
    }
    
    pImage->moreData = moreData;
    
    WIDGET_TRACE( "---- TPIa_widgetImageNextPart --end \n");
    return TPI_WIDGET_OK;
}
/*==================================================================================================
    FUNCTION:  TPIa_widgetImageCreateIcon

    DESCRIPTION:
        Creates an MsgImage from the specified MsfIcon and returns MsfImageHandle. 
 
    ARGUMENTS PASSED:
        modId      : The MSF Module identifier
        icon        : Specifies the MsfIcon to be used when creating MsfImage.
        defaultStyle  : The MsfStyle identifier used for the brush style
                       settings. If this parameter is 0 the default style of the
                       device will be used.

    RETURN VALUE:
        A new MsdImageHandle if the operation was successful, otherwise 
        return 0.
 
    IMPORTANT NOTES:
        nobody use.
==================================================================================================*/
MsfImageHandle TPIa_widgetImageCreateIcon
(
    MSF_UINT8       modId,
    MsfIconHandle   icon,
    MsfStyleHandle  defaultStyle
)
{

    WIDGET_TRACE( "TPIa_widgetImageCreateIcon!\n" );
    return 0;
}

/*==================================================================================================
    FUNCTION:  TPIa_widgetImageCreateResource

    DESCRIPTION:
        Creates an MsfImage from the data obtained from the specified resource. 
        The format of the MsfImage data is specified by the imageFormat parameter.
        Note that the integration is responsible for performing any necessary opening,
        reading, closing and deleting operations on the specified resource,

    ARGUMENTS PASSED:
        modId        The MSF Module identifier
        resource      Specifies a resource, for example a file name of a file
                     containing MsfImage data. It could also be the name of a named pipe.
        resourceType  Specifies the MsfResourceType.
        imageFormat   Specifies the MsfImageFormat of the image data.

    RETURN VALUE:
        A new MsdImageHandle if the operation was successful, otherwise return 0.

    IMPORTANT NOTES:
        None
==================================================================================================*/
#ifndef   WIDGET_DEBUG
MsfImageHandle TPIa_widgetImageCreateResource(MSF_UINT8 modId, char* resource, MsfResourceType resourceType, MsfImageFormat imageFormat)
#else
MsfImageHandle WIDGET_DEBUG_IMAGE_CREATE_RESOURCE(MSF_UINT8 modId, char* resource, MsfResourceType resourceType, MsfImageFormat imageFormat, const char *filename, int lineno)
#endif
{
    MsfImageHandle  hImage;
    MsfSize          size = {0,0};   
    MsfImage*       pImage;
    MsfWidgetType    widgetType;

    if(resource == OP_NULL ||  resourceType == MsfResourceBuffer )
    {
        return INVALID_HANDLE;
    }

#ifndef   WIDGET_DEBUG
    hImage = TPIa_widgetImageCreateEmpty(modId, &size, 0);
#else
    hImage = WIDGET_DEBUG_IMAGE_CREATE_EMPTY(modId, &size, 0, filename, lineno);
#endif
    if(hImage == INVALID_HANDLE)
    {
        return INVALID_HANDLE;
    }
    /* get pImage */
    pImage = seekWidget(hImage, &widgetType);
    if(pImage == OP_NULL)
    {
        return INVALID_HANDLE;
    }
    /* set image format */
    pImage->format = imageFormat;
    if(resourceType == MsfResourceFile)
    {
        widget_get_file_resource(modId, pImage, resource);
    }
    if(resourceType == MsfResourcePipe)
    {
        widget_get_pipe_resource(modId, pImage, resource);  
    }
    
    WIDGET_TRACE("---- TPIa_widgetImageCreateResource --end \n");
    return hImage;
}
  
/*==================================================================================================
    FUNCTION:  TPIa_widgetImageCreateEmpty

    DESCRIPTION:
        Creates an empty MsfImage with the specified MsfSize and returns the MsfImageHandle. If an
        empty MsfImage is drawn on an MsfPaintbox it should be drawn like a filled rectangle with the
        current color of the MsfPaintbox.
        
    ARGUMENTS PASSED:
        modId      : The MSF Module identifier
        size        : Specifies the MsfSize of the empty MsfImage
        defaultStyle : The MsfStyle identifier used for the brush style
                       settings. If this parameter is 0 the default style of the
                       device will be used.

    RETURN VALUE:
        A new MsdImageHandle if the operation was successful, otherwise return 0.
  
    IMPORTANT NOTES:
        None
==================================================================================================*/
#ifndef   WIDGET_DEBUG
MsfImageHandle TPIa_widgetImageCreateEmpty( MSF_UINT8 modId, MsfSize* size, MsfStyleHandle defaultStyle)
#else
MsfImageHandle WIDGET_DEBUG_IMAGE_CREATE_EMPTY( MSF_UINT8 modId, MsfSize* size, MsfStyleHandle defaultStyle, const char *filename, int lineno)
#endif
{
    MsfImage       *pImage;

    pImage = WIDGET_NEW(MsfImage);
    
    if(pImage == OP_NULL)
    {
        return  INVALID_HANDLE;
    }
    op_memset(pImage, 0, sizeof(MsfImage));
    
    pImage->data = OP_NULL;
    pImage->colorPallet = OP_NULL;
    pImage->pGifBmpBuf = OP_NULL;
    pImage->size.height = size->height;
    pImage->size.width = size->width;
    pImage->dataSize = 0;
    pImage->modId = modId;
    pImage->handle = getNewHandle( modId, MSF_IMAGE);
    pImage->referenceCounter = 1;
    pImage->predefinedId = 0;
    replaceHandle((OP_UINT32 *)&pImage->style, defaultStyle);
    pImage->format = MsfBmp;   /*default format*/
    pImage->moreData = 0;
    pImage->bOriginal = OP_TRUE;

    SP_list_insert_to_head(&widgetQueue[MSF_IMAGE], pImage);

#ifdef   WIDGET_DEBUG
    {
        const char *pPtr;
        
        pPtr = strrchr(filename, '\\');
        if( pPtr )
        {
            pPtr++;
            op_printf( "\n%s line=%d [IMAGE] ALLOC:ptr=0x%08p\n", pPtr, lineno, (void*)(pImage->handle) );
        }
    }
#endif

    return pImage->handle;

}

/*==================================================================================================
    FUNCTION:  TPIa_widgetImageCreateFrame

    DESCRIPTION:
        Creates an MsfImage from what is currently shown on the display, 
        a so called screen dump, and returns the MsfImageHandle.

    ARGUMENTS PASSED:
        modId:       The MSF Module identifier
        size  :       Specifies the MsfSize of the MsfImage to be created.
                        0 specifies the maximum MsfSize, which is the display size.
        pos  :       Specifies the MsfPosition of the frame area on the current
                        MsfScreen from where the MsfImage is to be created.
        defaultStyle:  The MsfStyle identifier used for the brush style settings.
                        If this parameter is 0 the default style of the device will
                        be used.
    RETURN VALUE:
        A new MsdImageHandle if the operation was successful, otherwise return 0.

    IMPORTANT NOTES:
        None
==================================================================================================*/
MsfImageHandle TPIa_widgetImageCreateFrame
(
    MSF_UINT8      modId,
    MsfSize*       size,
    MsfPosition*   pos,
    MsfStyleHandle defaultStyle
)
{
    WIDGET_TRACE("---- TPIa_widgetImageCreateFrame ----\n ");
    return 0;
}

/*==================================================================================================
    FUNCTION:  TPIa_widgetImageCreateFormat

    DESCRIPTION:
        Creates an MsfImage from the data provided in the imageData parameter
        and returns the MsfImageHandle. The imageFormat parameter specifies the MimeType of the MsfImage data.
    ARGUMENTS PASSED:
        modId:       The MSF Module identifier
        imageData:    Specifies the MsfCreateData union containing the actual data or resource of the MsfImage.
        imageFormat:  Specifies the MimeType of the image data, If no MimiType is available the module may set it to 0.
        resourceType:  Specifies the MsfResourceType. If the MsfResourceType is MsfResourcePipe or MsfResourceFile, 
                      the resource para in the MsfCreateData union is used. If the MsfResourceType if MsfResourceBuffer,
                      the directData para in the MsfCreateData union is used.
        defaultStyle:   The MsfStyle identifier used for the brush style settings. 
                     If this parameter is 0 the default style of the device will be used.
    RETURN VALUE:
        A new MsdImageHandle if the operation was successful, otherwise return 0.

    IMPORTANT NOTES:
        the integration is responsible for performing any necessary opening, reading, closing and deleting operations on the specified resource.
==================================================================================================*/
MsfImageHandle TPIa_widgetImageCreateFormat
(
    MSF_UINT8 modId, 
    MsfCreateData* imageData, 
    const char* imageFormat, 
    MsfResourceType resourceType, 
    MsfStyleHandle defaultStyle
)
{
#if 0
    MsfImageHandle    hImage;

    if(imageData == MSF_NULL)
    {
        return INVALID_HANDLE;
    }
    if(imageFormat == MSF_NULL)
    { /* set default mime type */
    
    }
    else
    {
        /* check if imageFormat has been registered */
        /* load MimeType set */
    }
    
/*  how to get the imageFormat ??
    hImage = TPIa_widgetImageCreateResource( modId,
                                      imageData->resource, 
                                      resourceType, 
                                      imageFormat);
*/
#endif
    WIDGET_TRACE("---- TPIa_widgetImageCreateFormat ----\n ");


    return 0;
}
/*==================================================================================================
    FUNCTION:  TPIa_widgetImageCreateFrame

    DESCRIPTION:
        Retrieves a predefined static MsfImage using the resId parameter or the resString parameter and
        returns the MsfImageHandle. The resId parameter or the resString parameter must be a globally
        unique identifier. If the resString parameter is 0, the integration should use the resId parameter
        to find the predefined MsfImage. If the resString parameter is not 0, the integration should u
        the resString parameter to find the predefined MsfImage. The integration can typically use the
        resString parameter for pictogram support. Predefined static MsfImages do not have to be
        released. If the module tries to release a predefined static MsfImage, the integration just ignores
        it.
        se
    ARGUMENTS PASSED:
        resId:    specified a globally unique Image resource identifier as an integer.
        resString: specified a globally unique image resource identifier as a string.
    RETURN VALUE:
        returns The MsfImageHandle if the operation was successful,.

    IMPORTANT NOTES:
        the integration is responsible for performing any necessary opening, reading, closing and deleting operations on the specified resource.
==================================================================================================*/
MsfImageHandle TPIa_widgetImageGetPredefined
(
    MSF_UINT32 resId, 
    const char* resString
)
{
    MsfImageHandle  hImage;
    MsfSize          size;   
    MsfImage*       pImage;
    MSF_UINT8    modId;
    RM_ICON_T   *icon;
    
    if(  resId == 0 )
    {
        return INVALID_HANDLE;
    }

    /* find the image with the resId,  if found return the image handle */
    pImage = (MsfImage*)SP_list_get_first_item(&widgetQueue[MSF_IMAGE]);
    while( pImage != OP_NULL)
    {
        if (pImage->predefinedId == resId )
        {
            return pImage->handle;
        }
        
        pImage = (MsfImage*)SP_list_get_next_item(&widgetQueue[MSF_IMAGE]);
    }

    /* not found the image, create a image  */
    icon = (RM_ICON_T *)WidgetGetPredefineIcon(resId);
    if(icon == OP_NULL)
    {
        return INVALID_HANDLE;
    }

    size.width = icon->biWidth;
    size.height = icon->biHeight;
    
    /* The lower 8 bit of all the resId is its modId */
    modId = (MSF_UINT8)(resId & 0xFF);
    
    hImage = TPIa_widgetImageCreateEmpty(modId, &size, 0);
    if(hImage == INVALID_HANDLE)
    {
        return INVALID_HANDLE;
    }
    
    /* get pImage */
    pImage = seekWidget(hImage, OP_NULL);
    /* get predefined icon image, now we only support icon predefine */
    pImage->predefinedId = resId;
    pImage->data = (OP_UINT8 *)icon;
    pImage->colorPallet = OP_NULL;
    pImage->bOriginal = OP_FALSE;
    /*
     * For the predefined image, just keep them in memory. Mark it as "released" here, 
     * so when TPIa_widgetRelease() is called, the structure will not be deallocated.
     */
    pImage->isReleased = OP_TRUE;

    /* resString represents the fixed image name in Flash */
    if(resString != OP_NULL) 
    {
        //ds_getImagebyString(resString, pImage);
    }
    WIDGET_TRACE("---- TPIa_widgetImageGetPredefined ----\n ");

    return hImage;
}


/****************************************************************
 SOUND
 ***************************************************************/
/*==================================================================================================
    FUNCTION:  TPIa_widgetSoundCreate

    DESCRIPTION:
    Creates an MsfStyle widget with the specified style attributes.
    
    ARGUMENTS PASSED:
    modId         :The MSF Module identifier
    soundData     :Specifies the actual data of the MsfSound on the specified MsfSoundFormat 
                 and with the specified length
    soundFormat     :Specifies the MsfSoundFormat of the sound data.
    size             :Specifies the length of the sound data.
    moreData     :Specifies if all of the MsfSound data is contained in the soundData parameter (0) 
                 or if there is more MsfSound data to come (1).

    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
#ifndef WIDGET_DEBUG
MsfSoundHandle TPIa_widgetSoundCreate(MSF_UINT8 modId, char *soundData, MsfSoundFormat soundFormat, int size, int moreData)
#else
MsfSoundHandle WIDGET_DEBUG_SOUND_CREATE(MSF_UINT8 modId, char* soundData, MsfSoundFormat soundFormat, int size, int moreData, const char *filename, int lineno)
#endif
{
   MsfSound       *pSound;
   MsfSoundHandle soundHandler;
   
   WIDGET_TRACE("\n *** soundData = %d,size = %d,moreData =%d !!\n",soundData,size,moreData);
   
    pSound = WIDGET_NEW(MsfSound);
    if(!pSound)
    {
        WIDGET_TRACE("\n WIDGET_NEW(msfsound) failed. \n");
        soundHandler = INVALID_HANDLE;
    }
    else
    {
        op_memset(pSound, 0, sizeof(MsfSound));
        
        pSound->data = (char *)WIDGET_ALLOC(size);
        if( pSound->data == OP_NULL )
        {
            WIDGET_TRACE("\n WIDGET_ALLOC(size) failed. \n");
            
            WIDGET_FREE(pSound);
            soundHandler = INVALID_HANDLE;
        }
        else
        {
            op_memset(pSound->data, 0, size);
                
            op_memcpy(pSound->data, soundData, size);
                    
            pSound->modId = modId;
            pSound->handle = getNewHandle( modId, MSF_SOUND);
            pSound->format = soundFormat;
            pSound->moreData = moreData;
            pSound->bufferSize = pSound->size = size;
            pSound->referenceCounter = 1;
            
            SP_list_insert_to_head(&widgetQueue[MSF_SOUND], pSound);

            soundHandler = pSound->handle;
            
#ifdef   WIDGET_DEBUG
            {
                const char *pPtr;
                
                pPtr = strrchr(filename, '\\');
                if( pPtr )
                {
                    pPtr++;
                    op_printf( "\n%s line=%d [SOUND] ALLOC:ptr=0x%08p\n", pPtr, lineno, (void*)soundHandler );
                }
            }
#endif
        }
     }
    
    return soundHandler;
}


/*==================================================================================================
    FUNCTION:  TPIa_widgetSoundCreateEx

    DESCRIPTION:
    Creates an MsfStyle widget with the specified style attributes.
    
    ARGUMENTS PASSED:
    modId         : The MSF Module identifier
    soundData     : Specifies the actual data of the MsfSound on the specified MsfSoundFormat 
                    and with the specified length
    soundFormat   : Specifies the MsfSoundFormat of the sound data.
    size          : Specifies the length of the sound data.
    totalSize     : total size of the sound data
    moreData      : Specifies if all of the MsfSound data is contained in the soundData parameter (0) 
                    or if there is more MsfSound data to come (1).

    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
#ifndef WIDGET_DEBUG
MsfSoundHandle TPIa_widgetSoundCreateEx(MSF_UINT8 modId, char* soundData, MsfSoundFormat soundFormat, int size, int totalSize, int moreData)
#else
MsfSoundHandle WIDGET_DEBUG_SOUND_CREATE_EX(MSF_UINT8 modId, char* soundData, MsfSoundFormat soundFormat, int size, int totalSize, int moreData, const char *filename, int lineno)
#endif
{
    MsfSound       *pSound;
    MsfSoundHandle soundHandler;
   
    WIDGET_TRACE("\n *** soundData = %d,size = %d,moreData =%d !!\n",soundData,size,moreData);

    if(totalSize <= 0)
    {
        return INVALID_HANDLE;
    }
    
    pSound = WIDGET_NEW(MsfSound);
    if(!pSound)
    {
        WIDGET_TRACE("\n WIDGET_NEW(msfsound) failed. \n");
        soundHandler = INVALID_HANDLE;
    }
    else
    {
        op_memset(pSound, 0, sizeof(MsfSound));
        
        pSound->data = (char *)WIDGET_ALLOC(totalSize);
        if( pSound->data == OP_NULL )
        {
            WIDGET_TRACE("\n WIDGET_ALLOC(size) failed. \n");
            
            WIDGET_FREE(pSound);
            soundHandler = INVALID_HANDLE;
        }
        else
        {
            op_memset(pSound->data, 0, totalSize);
                
            op_memcpy(pSound->data, soundData, size);

            pSound->bufferSize = totalSize;
            pSound->modId = modId;
            pSound->handle = getNewHandle( modId, MSF_SOUND);
            pSound->format = soundFormat;
            pSound->moreData = moreData;
            pSound->size = size;
            pSound->referenceCounter = 1;
            
            SP_list_insert_to_head(&widgetQueue[MSF_SOUND], pSound);

            soundHandler = pSound->handle;
#ifdef   WIDGET_DEBUG
            {
                const char *pPtr;
                
                pPtr = strrchr(filename, '\\');
                if( pPtr )
                {
                    pPtr++;
                    op_printf( "\n%s line=%d [SOUND] ALLOC:ptr=0x%08p\n", pPtr, lineno, (void*)soundHandler );
                }
            }
#endif
        }
     }
    
    return soundHandler;
}

/*==================================================================================================
    FUNCTION:  TPIa_widgetSoundNextPart

    DESCRIPTION:
     Creates the next part of an MsfSound from the data provided in the soundData parameter. The
     format of the MsfSound data is the same as when the first part of the MsfSound was created.
     The moreData parameter indicates whether all of the MsfSound data was provided or if there is
     more to come.
    
    ARGUMENTS PASSED:
    sound       : The MSFSound identifier.
    soundData   : Specifies the actual data of the MsfSound in the specified
                  MsfSoundFormat and of the specified length.
    size        : Specifies the length of the MsfSound data.
    moreData    : Specifies if all of the MsfSound data is contained in the
                  soundData parameter (0) or if there is more MsfSound data to
                  come (1).
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise
        the appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetSoundNextPart (MsfSoundHandle sound, char* soundData, int size, int moreData)
{
    OP_UINT8*      pdata ;
    MsfSound       *pSound;
    MsfWidgetType  widgetType;
    OP_INT32       buflen;
    
    pSound = seekWidget(sound, &widgetType);
    if((pSound==OP_NULL)||(size <= 0))
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    buflen = pSound->size + size;
    if(buflen > pSound->bufferSize)
    {
        pdata= WIDGET_ALLOC(buflen);
        if(pdata == OP_NULL)
        {
            if(pSound->data) WIDGET_FREE(pSound->data);
            pSound->bufferSize = pSound->size = 0;
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }

        if(pSound->data)
        {
            op_memcpy(pdata, pSound->data, pSound->size);
            op_memcpy((pdata+pSound->size), soundData, size);
            WIDGET_FREE(pSound->data);
            pSound->size = pSound->bufferSize = buflen;
        }
        else
        {
            op_memcpy(pdata, soundData, size);
            pSound->size = pSound->bufferSize = size;
        }
        pSound->data =(char*) pdata;

    }
    else
    {
        op_memcpy(pSound->data+pSound->size , soundData, size);
        pSound->size = buflen;
    }

    pSound->moreData = moreData;
    WIDGET_TRACE( "\n-- TPIa_widgetImageNextPart --sound=%d,size=%d,moreData=%d\n",sound,size,moreData);
        
    return TPI_WIDGET_OK;
}
/*==================================================================================================
    FUNCTION:  TPIa_widgetSoundPlay

    DESCRIPTION:
    play the MsfSound's contents.
    
    ARGUMENTS PASSED:
    sound       : The MSFSound identifier.
   
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise
        the appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetSoundPlay (MsfSoundHandle sound)
{
    MsfSound *pSound;
    MsfWidgetType  widgetType;
    int ret = TPI_WIDGET_OK;
    
    pSound = seekWidget(sound, &widgetType);
    if(pSound==OP_NULL)
    {
        ret = TPI_WIDGET_ERROR_BAD_HANDLE;
    }
    else
    {
        if(pSound->moreData == 0)
        {
            WIDGET_TRACE( "\n** Hdia_soundplay, pSound->size = %d ,moredata=%d \n ",pSound->size,pSound->moreData);
            AUDIO_PLAY((OP_UINT8*)pSound->data,pSound->size,SP_GetVolume( BELL_LEVEL_ID ).LEVEL, 1);
            ret = TPI_WIDGET_OK;
        }
        else
        {
            ret = TPI_WIDGET_ERROR_NOT_SUPPORTED;
        }
    }
    return ret;
}
/*==================================================================================================
    FUNCTION:  TPIa_widgetSoundStop

    DESCRIPTION:
    stop the MsfSound's contents.
    
    ARGUMENTS PASSED:
    sound       : The MSFSound identifier.
   
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise
        the appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetSoundStop (MsfSoundHandle sound)
{
    SP_Audio_stop();
    return TPI_WIDGET_OK;
}
/*==================================================================================================
    FUNCTION:  TPIa_widgetSoundPlayPredefined

    DESCRIPTION:
    Plays the specified MsfPredefinedSound.If the device cannot play MsfSounds
    simultaneously, the first MsfSound must be stopped before the next MsfSound can be played.
    
    ARGUMENTS PASSED:
    sound       : The MSFSound identifier.
   
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise
        the appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetSoundPlayPredefined (MsfPredefinedSound sound){return 0;}
/*==================================================================================================
    FUNCTION:  TPIa_widgetSoundCreateResource

    DESCRIPTION:
    Plays the specified MsfPredefinedSound.If the device cannot play MsfSounds
    simultaneously, the first MsfSound must be stopped before the next MsfSound can be played.
    
    ARGUMENTS PASSED:

   modId : The MSF Module identifier
            resource Specifies a resource, for example a file name of a file
            containing MsfSound data. It could also be a name of for
            example a named pipe.
    resourceType : Specifies the MsfResourceType.
    soundFormat  : Specifies the MsfSoundFormat of the sound data.
   
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise
        the appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
#ifndef WIDGET_DEBUG
MsfSoundHandle TPIa_widgetSoundCreateResource(MSF_UINT8 modId, char* resource, MsfResourceType resourceType, MsfSoundFormat soundFormat)
#else
MsfSoundHandle WIDGET_DEBUG_SOUND_CREATE_RESOURCE(MSF_UINT8 modId, char* resource, MsfResourceType resourceType, MsfSoundFormat soundFormat, const char *filename, int lineno)
#endif
{
    MsfSound*       pSound;

    if(resource == OP_NULL ||  resourceType == MsfResourceBuffer )
    {
        return INVALID_HANDLE;
    }

    pSound = WIDGET_NEW(MsfSound);
    if(!pSound)
    {
        WIDGET_TRACE("\n WIDGET_NEW(msfsound) failed. \n");
        return INVALID_HANDLE;
    }
    op_memset(pSound, 0, sizeof(MsfSound));
    
    pSound->modId = modId;
    pSound->handle = getNewHandle( modId, MSF_SOUND);
    pSound->format = soundFormat;
    pSound->referenceCounter = 1;
    
    SP_list_insert_to_head(&widgetQueue[MSF_SOUND], pSound);
    
    if(resourceType == MsfResourceFile)
    {
        widget_get_file_resource_for_sound(modId, pSound, resource);
    }
    if(resourceType == MsfResourcePipe)
    {
        widget_get_pipe_resource_for_sound(modId, pSound, resource);  
    }
    
#ifdef   WIDGET_DEBUG
    {
        const char *pPtr;
        
        pPtr = strrchr(filename, '\\');
        if( pPtr )
        {
            pPtr++;
            op_printf( "\n%s line=%d [SOUND] ALLOC:ptr=0x%08p\n", pPtr, lineno, (void*)(pSound->handle) );
        }
    }
#endif
    
    return pSound->handle;
}

/*==================================================================================================
    FUNCTION:  TPIa_widgetSoundCreateFormat

    DESCRIPTION:
    Creates an MsfSound from the data provided in the soundData parameter and returns the
    MsfSoundHandle. The soundFormat parameter specifies the MimeType of the MsfSound data.
    parameter. Note that the integration is responsible for performing any necessary opening,
    reading, closing and deleting operations on the specified resource.    
    
    ARGUMENTS PASSED:

            modId : The MSF Module identifier
            resource Specifies a resource, for example a file name of a file
            containing MsfSound data. It could also be a name of for
            example a named pipe.
            resourceType : Specifies the MsfResourceType.
            soundFormat  : Specifies the MsfSoundFormat of the sound data.
   
    RETURN VALUE:
        A positive integer if the operation was successful, otherwise
        the appropriate return code is given.

    IMPORTANT NOTES:
        None
==================================================================================================*/
MsfSoundHandle TPIa_widgetSoundCreateFormat(MSF_UINT8 modId, MsfCreateData* soundData, 
                                            const char* soundFormat, MsfResourceType resourceType){return 0;}

MsfSoundHandle TPIa_widgetSoundGetPredefined(MSF_UINT32 resId){return 0;}


/****************************************************************
 ACCESS KEYS
 ***************************************************************/
/*==================================================================================================
    FUNCTION:  TPIa_widgetSetAccessKey

    DESCRIPTION:
        With this function the module gives the integration an access-key definition according
        to the CSS specification. The integration must map this access-key definition to some
        device specific buttons, soft keys or other available input mechanism. The integration
        must also allot the access-key definition a value, which is returned to the module as
        the MsfAccessKeyHandle.
        
    ARGUMENTS PASSED:
        handle                      :The widget identifier, i.e. the MsfWindow or MsfScreen or MsfGadget identifier.
        accessKeyDefinition  :Specifies the access key definition according to the CSS specification of access-key.
        override                    :Specifies if the access key event is to be overridden (1) or not (0).
        index                         :The index identifier, in the case the MsfAccessKey pertains to for example 
                                           an element in an MsfMenu or MsfSelectGroup.If the index parameter is not 
                                           used it should be set to -1.
        visualise                   :Specifies if the integration should visualise the MsfAccessKey or not.        
        
    RETURN VALUE:
        The new MsfAccessKeyHandle if the
        operation was successful, otherwise 0.
        
    IMPORTANT NOTES:
        None
==================================================================================================*/
MsfAccessKeyHandle TPIa_widgetSetAccessKey
(
    MSF_UINT32 handle,
    char* accessKeyDefinition,
    int override,
    int index,
    int visualise
)
{
    MSF_UINT8                 modId;
    MsfAccessKeyHandle  hNew;
    void                            *pWidget;
    MsfAccessKey             *pAccessKey;
    MsfAccessKey             *pTail = OP_NULL;
    MsfAccessKey             **ppAccessKeyList;
    MsfWidgetType           widgetType;
    int                               iNotFound;

    if( accessKeyDefinition == OP_NULL 
        ||(accessKeyDefinition[0] == 0x00 && accessKeyDefinition[1] == 0x00)  )
    {
        return INVALID_HANDLE;
    }

    if( ! ISVALIDHANDLE(handle) )
    {
        return INVALID_HANDLE;
    }

    ppAccessKeyList = OP_NULL;

    pWidget = seekWidget(handle, &widgetType);
    if( pWidget )
    {
        if(IS_SCREEN(widgetType) )
        {
            ppAccessKeyList = &((MsfScreen*)pWidget)->accessKey;
        }
        else if( IS_WINDOW(widgetType))
        {
            ppAccessKeyList = &((MsfWindow*)pWidget)->accessKey;
        }
        else if( IS_GADGET(widgetType))
        {       
            ppAccessKeyList = &((MsfGadget*)pWidget)->accessKey;
        }
    }

    if( ppAccessKeyList == OP_NULL )   /*the widget has no access-key list */
    {
        return INVALID_HANDLE;
    }

    if( *ppAccessKeyList != OP_NULL )  /* access-key list has elements */
    {
        pAccessKey = *ppAccessKeyList;
        
        /* find the access-key in the list */
        iNotFound = 1;
        while( pAccessKey && iNotFound != 0)    
        {
            iNotFound = Ustrcmp( (MSF_UINT8 *)pAccessKey->accessKeyDefinition, (MSF_UINT8 *)accessKeyDefinition);
            pTail = pAccessKey;
            pAccessKey = pAccessKey->next;
        }

        if( iNotFound == 0 )    /* found the access-key */
        {
            return  pAccessKey->handle;
        }
    }

    /* the access-key not in the list alloc a new access-key, 
      * and add it to the tail of the list
      */
      
    modId = getModID(handle);
    pAccessKey =WIDGET_NEW(MsfAccessKey);
    if( !pAccessKey )       /* mem not enough */
    {
        return INVALID_HANDLE;          
    }
    op_memset(pAccessKey, 0, sizeof(MsfAccessKey));
    
    pAccessKey->accessKeyDefinition = (char*)WIDGET_ALLOC(Ustrlen((OP_UINT8*)accessKeyDefinition)+2);
    if( pAccessKey->accessKeyDefinition == OP_NULL )   /* mem not enough */
    {
        WIDGET_FREE(pAccessKey );
        return INVALID_HANDLE;
    }
    op_memset(pAccessKey->accessKeyDefinition, 0, Ustrlen((OP_UINT8*)accessKeyDefinition)+2);
    hNew = getNewHandle(modId, MSF_ACCESSKEY);

    pAccessKey->handle = hNew;
    pAccessKey->index = index;
    pAccessKey->next = OP_NULL;
    pAccessKey->override = override;
    pAccessKey->visualise = visualise;
    Ustrcpy((MSF_UINT8 *)pAccessKey->accessKeyDefinition, (MSF_UINT8 *)accessKeyDefinition );

    if(*ppAccessKeyList == OP_NULL )
    {
        *ppAccessKeyList = pAccessKey;
    }
    else if( pTail )
    {
        pTail->next = pAccessKey;
    }
    
    return hNew;
}

/*==================================================================================================
    FUNCTION:  TPIa_widgetRemoveAccessKey

    DESCRIPTION:
        Removes the specified access key from the specified widget. This function is only
        applicable to MsfWindow and MsfScreen and MsfGadget widgets.

    ARGUMENTS PASSED:
        handle          :The widget identifier, i.e. the MsfWindow or MsfScreen or MsfGadget identifier.
        accessKey     :The identifier of the access key that is to be removed from the widget

    RETURN VALUE:
        A positive integer if the operation was successful, otherwise
        the appropriate return code.
        
    IMPORTANT NOTES:
        None
==================================================================================================*/
int TPIa_widgetRemoveAccessKey(MSF_UINT32 handle, MsfAccessKeyHandle accessKey)
{
    void                   *pWidget;
    MsfAccessKey    *pNode;
    MsfAccessKey    *pPreNode;
    MsfAccessKey    **ppAccessKeyList;
    MsfWidgetType  widgetType;

    if( ! ISVALIDHANDLE(handle) )
    {
        return TPI_WIDGET_ERROR_BAD_HANDLE;
    }
    
    ppAccessKeyList = OP_NULL;

    pWidget = seekWidget(handle, &widgetType);
    if( pWidget )
    {
        if(IS_SCREEN(widgetType) )
        {
            ppAccessKeyList = &((MsfScreen*)pWidget)->accessKey;
        }
        else if( IS_WINDOW(widgetType))
        {
            ppAccessKeyList = &((MsfWindow*)pWidget)->accessKey;
        }
        else if( IS_GADGET(widgetType))
        {       
            ppAccessKeyList = &((MsfGadget*)pWidget)->accessKey;
        }
    }

    if( ppAccessKeyList == OP_NULL     /*the widget has no access-key list */
        || *ppAccessKeyList == OP_NULL )   /* or the list is empty */
    {
        return TPI_WIDGET_OK;
    }

    pNode = *ppAccessKeyList;
    if( pNode->handle == accessKey )    /* the first node of the  list need to delete */
    {
        *ppAccessKeyList = pNode->next;
        WIDGET_FREE(pNode );
        return TPI_WIDGET_OK;
    }

    /* The node need to delete is not the first node of the list */
    while( pNode != OP_NULL &&  pNode->handle != accessKey )
    {
        pPreNode = pNode;
        pNode = pNode->next;
    }

    if( pNode )
    {
        pPreNode->next = pNode->next;
        WIDGET_FREE(pNode );
    }
    
    return TPI_WIDGET_OK;
}

/****************************************************************
 ANIMATION
 ***************************************************************/
MsfMoveHandle TPIa_widgetMoveCreate (MSF_UINT8 modId, MsfPosition* origin,
                     MsfPosition* destination, int duration, 
                     int nrOfSteps, MsfMoveProperty property, int repeat){return 0;}

MsfRotationHandle TPIa_widgetRotationCreate (MSF_UINT8 modId, int startAngle, 
                         int endAngle, int duration, int nrOfSteps, 
                         MsfMoveProperty property, int repeat){return 0;}

MsfColorAnimHandle TPIa_widgetColorAnimCreate (MSF_UINT8 modId, MsfColor* startColor, 
                          MsfColor* endColor, int duration, 
                          int nrOfSteps, MsfMoveProperty property, int repeat){return 0;}
