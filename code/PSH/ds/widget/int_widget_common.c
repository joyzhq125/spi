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
    7/30/2003    Zhuxq            P001026     Delete the functions widgetNotifyMsfEditor and widgetInputEnterEditor
    04/08/2003   Zhuxq            P001052     Add msf  default predefine strings
    08/05/2003   Steven Lai       P001140     Change the method of handling screen focus
    08/13/2003   Zhuxq            P001147     Change the selectgroup show and menu show
    8/11/2003    Chenjs           P001091     Update text input widget.
    08/20/2003   Zhuxq            P001279     modify the WidgetGetPredefineIcon and add a function of widgetImageGadgetGetSize
    08/25/2003   Steven Lai       P001355     Fixed some warning according to PC-Lint check result
    08/27/2003   Zhuxq            P001383     fixed some bugs
    09/03/2003   Zhuxq            P001479     Add two funcions: saveScreen and restoreScreen
    09/04/2003   linda wang       P001494     Add MMS Memory status
    09/25/2003   Zhuxq            P001696     Re-implement the dialog
    09/25/2003   Steven Lai       C001579     Add the feature to handle attachment in mms
    10/23/2003   Zhuxq            P001842     Optimize the paintbox and input method solution and fix some bug
    11/11/2003   Zhuxq            P001883     Optimize the redraw strategy to reduce the redraw times
    01/05/2004   Zhuxq            C000072     Add callback function to each widget in the widget system
    01/15/2004   Zhuxq            P002352     All editing actions of MsfTextInput turns to MsfEditor
    02/16/2004   Zhuxq            P002469     Add button gadget to widget system
    02/16/2004   chenxiao         p002474     add bitmap animation function in the dolphin project
    02/17/2004   Zhuxq            P002492     Add Viewer window to widget system
    02/19/2004   Zhuxq            P002504     Add box gadget to widget system
    02/24/2004   chenxiao         p002530     modify WAP & MSG main menu     
    03/26/2004   Zbzhang          C002699     Add UMB module
    04/15/2004   zhuxq            P002977     Fix some bugs in UMB for Dolphin
    04/14/2004   Dingjianxin      p002961     delete the conditional compiler about ALL_TEXT_EDIT_GOTO_EDITOR
    04/30/2004   zhuxq            P003022     Fix some bugs in UMB
    05/25/2004   chenxiao         p005637     change image interface with DS     
    05/24/2004   zhuxq            P005568     Adjust UI implementation in UMB and fix some bugs
    06/04/2004   zhuxq            P005925     Correct issues found by PC-lint and fix some bugs
    06/09/2004   zhuxq            P006048     forbid Menu to respond successive multiple actions
    06/09/2004   zhuxq            P006106     Fix bugs in UMB and WAP found in pretest for TF_DLP_VER_01.24    
    06/18/2004   zhuxq            P006260     Make up WAP&UMB code to cnxt_int_wap branch
    07/23/2004   zhuxq            P007215     Prevent freeze in wap
    08/06/2004   zhuxq            P007577     adjust the layout of MMS/SMS for B1,  and fix some bugs
    08/09/2004   Hugh zhang       p007607     Change font of widget dialog in lotus project.
    08/09/2004   Chenxiao         p007631     The cell phone will freeze in some web site
    08/18/2004   zhuxq            P007793     Trace widget memory issues, and reduce allocation times of MsfString
    08/30/2004   zhuxq            c007998     improve the implementation of predefined string to reduce the times of memory allocation
    09/13/2004   zhuxq            c008273     Add feature of inserting PB entry into SMS content for UMB
    
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
#include <string.h>
#include "portab.h"
#endif

#include "Utility_string_func.h"

#include "SP_list.h"

#include "msf_lib.h"
#include "Msf_env.h"

#include "msf_int_widget.h"
#include "int_widget_txt_input.h"

#include "EDITOR_type.h"
#include "Ds_def.h"
#include "int_widget_custom.h"
#include "int_widget_common.h"
#include "int_widget_remove.h"

#include "stk_rc.h"
#include "bra_rc.h"
#include "brs_rc.h"
#include "phs_rc.h"
//#include "prs_rc.h"

#include "mms_rc.h"
#include "msf_rc.h"
#include "rma_rc.h"
#include "sma_rc.h"
#include "stk_rc.h"
#include "uia_rc.h"
#include "umb_rc.h"

#ifdef _TOUCH_PANEL
#include "TP_Operator.h"
#endif
/*==================================================================================================
    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static OP_UINT16 getNewSerialNum(void);

static int widgetScreenGetStyle( MsfScreen* pScreen, MsfWidgetType widgetType, MsfStyle* pStyle );

static int widgetWindowGetStyle( MsfWindow* pWin, MsfWidgetType widgetType, MsfStyle* pStyle );

static int widgetGadgetGetStyle( MsfGadget* pGadget, MsfWidgetType widgetType, MsfStyle* pStyle );

static int widgetLowlevelGetStyle( void* pWidget, MsfWidgetType widgetType, MsfStyle* pStyle );
static int  widgetImageGadgetGetSize
( 
 MsfImageGadget  *pImageGadget, 
 OP_UINT32        hImageGadget,
 MsfSize            *pSize
 );

static int  widgetBmpAniGetSize
( 
 MsfBmpAnimation   *pBmpAnimation,
 MsfSize            *pSize
 );
/*==================================================================================================
    LOCAL CONSTANTS
==================================================================================================*/




/*==================================================================================================
    LOCAL TYPEDEFS - Structure, Union, Enumerations...etc
==================================================================================================*/


/*==================================================================================================
    LOCAL MACROS
==================================================================================================*/
/*
 * description of this macro, if needed. - Remember self documenting code
 */
 
/*==================================================================================================
    LOCAL CONSTANTS
==================================================================================================*/

/*  condition : start < end   */
#define   IS_IN_RANGE(start, end, pt)      ( (pt) >= (start) && (pt) <= (end) ) 
    

/*==================================================================================================
    LOCAL VARIABLES
==================================================================================================*/
static OP_UINT16 serial_num = 0;



/*==================================================================================================
    GLOBAL VARIABLES
==================================================================================================*/
GENERAL_LIST_STRUCT_T widgetQueue[MSF_MAX_WIDGETTYPE];
MsfDisplay display = {OP_NULL, OP_NULL, INVALID_HANDLE};
MsfDisplay *pDisplay = &display;


/*==================================================================================================
    LOCAL FUNCTIONS
==================================================================================================*/
static OP_UINT16 getNewSerialNum(void)
{
    if (serial_num < 0xFFFF)
    {
        serial_num++;
    }
    else
    {
        serial_num=0;
    }
    return serial_num;
}



/*==================================================================================================
    GLOBAL FUNCTIONS
==================================================================================================*/
extern DS_SCRNMODE_ENUM_T  widgetScreenSetMode( void );
extern OP_UINT8*  widgetGetPredefinedStringEx(OP_UINT8 modId, OP_UINT8 lang, OP_UINT16 index);

#ifdef WIDGET_DEBUG
void *  WIDGET_DEBUG_ALLOC(MSF_UINT32 size, const char *filename, int lineno)
{
    void *ptr;
    const char *pFile;
    
    if((pFile = strrchr(filename, '\\')))
    {
        pFile++;
    }
    else
    {
        pFile = filename;
    }

    ptr = op_alloc(size);
    op_printf( "\n%s line=%d size=%d ALLOC:ptr=0x%08p\n", pFile, lineno, size, ptr );
    return ptr;
}

void  WIDGET_DEBUG_FREE( void *p, const char *filename, int lineno )
{
    const char *pFile;
    
    if((pFile = strrchr(filename, '\\')))
    {
        pFile++;
    }
    else
    {
        pFile = filename;
    }
    op_printf( "\n%s line=%d FREE:ptr=0x%08p ", pFile, lineno, p );
    if( p )
    {
        op_free( p );
        op_printf( " FREE End\n" );
    }
    
}

#endif


/****************************************************************
  Widget related function
 ***************************************************************/
OP_UINT32 getNewHandle(OP_UINT8 modId, MsfWidgetType widgetType)
{
    OP_UINT16 newSerialNum;
    OP_UINT32 newWidgetHandle;

    /*
     *   The widget handle is a UINT32 value, and it's content is:
     *   _____________________________________________________________________
     *   | modId (8 bits) | widgetType (8 bits) |  newSerialNum (16 bits)    |
     *   |--------------------------------------------------------------------
     */
    newSerialNum = getNewSerialNum();
    newWidgetHandle = ((OP_UINT32)widgetType & 0xFF) << 16;
    newWidgetHandle |= ((OP_UINT32)modId << 24);
    newWidgetHandle |= (OP_UINT32)newSerialNum;

    return newWidgetHandle;
}

/*==================================================================================================
    FUNCTION: seekWidget

    DESCRIPTION:
        Provide the integration to get widget data and widget type.

    ARGUMENTS PASSED:
        handle      : The identifier of the widget.
        pWidgetType : a pointer to get the widget type. if this para is OP_NULL,
                      no type return
        
    RETURN VALUE:
        A pointer to the widget data, this data structure may be MsfWindow,
        MsfGadget or MsfScreen. If the input parameter is invalid, then OP_NULL
        pointer will return

    IMPORTANT NOTES:
        None
==================================================================================================*/
void* seekWidget
(
    OP_UINT32      handle,
    MsfWidgetType*  pWidgetType
)
{
    void                   *ptr = OP_NULL;
    MsfWidgetType          tmpWidgetType;

    if (handle == INVALID_HANDLE)
    {
        return OP_NULL;
    }
    
    tmpWidgetType = getWidgetType(handle);
    
    if(pWidgetType != OP_NULL)
    {
        *pWidgetType = tmpWidgetType;
    }

    if( tmpWidgetType == MSF_STRING_RES )
    {
        return (void*)widgetGetPredefinedStringEx(
                                        getModID(handle),
                                        (OP_UINT8)((handle & 0x0000F000) >> 12),
                                        (OP_UINT16)(handle & 0x00000FFF));
    }
    
    /*
     * if the handler has a widget type
     */
    if(tmpWidgetType < MSF_MAX_WIDGETTYPE)
    {
        ptr = SP_list_get_first_item(&widgetQueue[tmpWidgetType]);
    }

    if( ptr )
    {
        if( IS_WINDOW(tmpWidgetType ))
        {
            while(ptr != OP_NULL)
            {
                if (handle == ((MsfWindow *)ptr)->windowHandle )
                {
                     return ptr;
                }
                ptr = SP_list_get_next_item(&widgetQueue[tmpWidgetType]);
            }
        }
        else if( IS_GADGET( tmpWidgetType ))
        {
            while(ptr != OP_NULL)
            {
                if (handle == ((MsfGadget*)ptr)->gadgetHandle )
                {
                     return ptr;
                }
                ptr = SP_list_get_next_item(&widgetQueue[tmpWidgetType]);
            }
        }
        else
        {
            switch( tmpWidgetType )
            {
            case MSF_SCREEN:
                while(ptr != OP_NULL)
                {
                    if (handle == ((MsfScreen *)ptr)->screenHandle)
                    {
                        return ptr;
                    }
                    ptr = SP_list_get_next_item(&widgetQueue[tmpWidgetType]);
                }
                break;
            
            case MSF_IMAGE:
                while(ptr != OP_NULL)
                {
                    if (handle == ((MsfImage *)ptr)->handle)
                    {
                        return ptr;
                    }
                    ptr = SP_list_get_next_item(&widgetQueue[tmpWidgetType]);
                }
                break;
            
            case MSF_STRING:
                while(ptr != OP_NULL)
                {
                    if (handle == ((MsfString *)ptr)->handle)
                    {
                        return ptr;
                    }
                    ptr = SP_list_get_next_item(&widgetQueue[tmpWidgetType]);
                }
                break;
            
            case MSF_ICON:
                while(ptr != OP_NULL)
                {
                    if (handle == ((MsfIcon *)ptr)->handle)
                    {
                        return ptr;
                    }
                    ptr = SP_list_get_next_item(&widgetQueue[tmpWidgetType]);
                }
                break;
            
            case MSF_SOUND:
                while(ptr != OP_NULL)
                {
                    if (handle == ((MsfSound *)ptr)->handle)
                    {
                        return ptr;
                    }
                    ptr = SP_list_get_next_item(&widgetQueue[tmpWidgetType]);
                }
                break;
            
            case MSF_STYLE:
                while(ptr != OP_NULL)
                {
                    if (handle == ((MsfStyle *)ptr)->handle)
                    {
                        return ptr;
                    }
                    ptr = SP_list_get_next_item(&widgetQueue[tmpWidgetType]);
                }
                break;
            
            case MSF_COLORANIM:
                while(ptr != OP_NULL)
                {
                    if (handle == ((MsfColorAnim *)ptr)->handle)
                    {
                        return ptr;
                    }
                    ptr = SP_list_get_next_item(&widgetQueue[tmpWidgetType]);
                }
                break;
            
            case MSF_MOVE:
                while(ptr != OP_NULL)
                {
                    if (handle == ((MsfMove *)ptr)->handle)
                    {
                        return ptr;
                    }
                    ptr = SP_list_get_next_item(&widgetQueue[tmpWidgetType]);
                }
                break;
            
            case MSF_ROTATION:
                while(ptr != OP_NULL)
                {
                    if (handle == ((MsfRotation *)ptr)->handle)
                    {
                        return ptr;
                    }
                    ptr = SP_list_get_next_item(&widgetQueue[tmpWidgetType]);
                }
                break;

            case MSF_ACTION:
                while(ptr != OP_NULL)
                {
                    if (handle == ((MsfAction *)ptr)->actionHandle)
                    {
                        return ptr;
                    }
                    ptr = SP_list_get_next_item(&widgetQueue[tmpWidgetType]);
                }
                break;

            default:
                break;
            }    
        }
    }

    /* not found the widget, set widgettype value with MSF_MAX_WIDGETTYPE */
    if(pWidgetType != OP_NULL)
    {
        *pWidgetType = MSF_MAX_WIDGETTYPE;
    }
    
    return OP_NULL;
}

OP_BOOLEAN  isValidHandleOfType( OP_UINT32 handle, MsfWidgetType wgType)
{
    MsfWidgetType widgetType;
    void *ptr = OP_NULL;
    OP_BOOLEAN ret = OP_FALSE;

    if (handle != INVALID_HANDLE)
    {
        ptr = seekWidget(handle, &widgetType);
        if ((widgetType == wgType) && (ptr != OP_NULL))
        {
            ret = OP_TRUE;
        }
    }
    return ret;
}

/*==================================================================================================
    FUNCTION:  getInput

    DESCRIPTION:
        This function gets the MsfInput of the  MsfEditor/TextInput, and put the result to ppInput.
        
    ARGUMENTS PASSED:
        text        : The MsfEditor/TextInput identifier
        ppInput  : Return as found result. If not found , should return OP_NULL
        pSize : address for getting size. OP_NULL: caller discard size; Ohers: valid address for witting size;
        
    RETURN VALUE:
      A positive integer if the operation was successful,
      otherwise the appropriate return code is given

    IMPORTANT NOTES:
        None
==================================================================================================*/
int  getInput
(
    OP_UINT32 text, 
    MsfInput      **ppInput,
    MsfSize *pSize,
    OP_UINT16 *pLabelLines
)
{
    void           *ptr;
    MsfEditor      *pEditor;
    MsfTextInput   *pTextInput;
    MsfWidgetType  widgetType;
    int            iRet;

    if( !ppInput )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    *ppInput = OP_NULL;
    iRet = TPI_WIDGET_OK;

    ptr = seekWidget(text, &widgetType);
    if( ptr )
    {
        if( widgetType == MSF_EDITOR )
        {
            pEditor = (MsfEditor*)ptr;
            *ppInput = &(pEditor)->input->input;
            if(pSize)
            {
                op_memcpy( pSize,&(pEditor->windowData.size),sizeof(MsfSize));
            }

            if( pLabelLines )
            {
                *pLabelLines = 0; 
            }
        }
        else if( widgetType == MSF_TEXTINPUT )
        {
            pTextInput = (MsfTextInput *)ptr;
            *ppInput = &pTextInput->input;
            if(pSize)
            {
                op_memcpy( pSize,&(pTextInput->gadgetData.size),sizeof(MsfSize));
            }
            if( pLabelLines )
            {
                if( ISVALIDHANDLE(pTextInput->gadgetData.title)  )
                {
                    *pLabelLines = 1;
                }
                else
                {
                    *pLabelLines = 0; 
                }
            }
        }
        else
        {
            iRet = TPI_WIDGET_ERROR_INVALID_PARAM;
        }
    }
    else
    {
        iRet = TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    return iRet;
}

/*==================================================================================================
    FUNCTION:  getChoice

    DESCRIPTION:
        Get the MsfChoice structure of the specified MsfMenu or MsfSelectGroup  or MsfBox.
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
      A positive integer if the operation was successful,
      otherwise the appropriate return code is given
      
    IMPORTANT NOTES:
        None
==================================================================================================*/
int  getChoice(OP_UINT32 choice, MsfChoice  **ppChoice )
{
    void            *ptr;
    MsfWidgetType   widgetType;
    int             iRet;

    if( !ppChoice )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    *ppChoice = OP_NULL;
    iRet = TPI_WIDGET_OK;
    
    ptr = seekWidget(choice, &widgetType);
    if(ptr )
    {
        if( widgetType == MSF_MENU )
        {
            *ppChoice =&((MsfMenuWindow*)ptr)->pSelectgroup->choice;
        }
        else if( widgetType == MSF_SELECTGROUP || widgetType == MSF_BOX)
        {
            *ppChoice =&((MsfSelectgroup*)ptr)->choice;
        }
        else if(MSF_MAINMENU == widgetType)
        {
            *ppChoice = &((MsfMainMenuWindow *)ptr)->pSelectgroup->choice;
        }
        else
        {
            iRet =  TPI_WIDGET_ERROR_INVALID_PARAM;
        }
    }
    else
    {
        iRet =  TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    return iRet;
}

/*==================================================================================================
    FUNCTION:  getChoiceElementByIndex

    DESCRIPTION:
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
choiceElement* getChoiceElementByIndex( MsfChoice *choice, int index )
{
    int pos;
    choiceElement*  pItem;
    
    if(   !choice 
        || !choice->firstElement || choice->count == 0 /* element list is empty */
        || index < 0 || index >= choice->count)     /* or index not in the range */
    {
        return OP_NULL;
    }

    pos = 0;
    pItem = choice->firstElement;
    
    while( pItem && pos < index )
    {
        pos++;
        pItem = pItem->next;        
    }

    return pItem;
    
}

/*==================================================================================================
    FUNCTION:  getLastChoiceElement

    DESCRIPTION:
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
choiceElement* getLastChoiceElement( MsfChoice *choice)
{
    choiceElement*  pItem;
    
    if(   !choice  
        || !choice->firstElement || choice->count == 0 )    /* element list is empty */
    {
        return OP_NULL;
    }

    pItem = choice->firstElement;        /* the pItem must be not  OP_NULL */
    
    while( pItem->next )
    {
        pItem = pItem->next;        
    }

    return pItem;
    
}

int replaceHandle(OP_UINT32 *target, OP_UINT32 source)
{
    int ret = TPI_WIDGET_OK;
    if (target == OP_NULL)
    {
        ret = TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    else
    {
        if (*target != INVALID_HANDLE)
        {
            ret = decrease_ref_count(*target);
        }
        if (ret == TPI_WIDGET_OK)
        {
             /* 
              * sometime we need to set target as INVALID_HANDLE, so
              * if source == INVALID_HANDLE, we still do the following statement
              */
            *target = source;
            if (source != INVALID_HANDLE)
            {
                ret = increase_ref_count(source);
            }
        }
    }

    return ret;
}


/*==================================================================================================
    FUNCTION:  getInputTextStringToBuffer

    DESCRIPTION:
        According to the MsfInputType, copy the input text to the buffer pBuff, 
        or fill the buffer with some format, i.e.  fille with "*".
        The number of copied characters will be less than the maxSize of the MsfInput.
        
        Notes: Before enter  this funcion,  the bBuf must be allocated enough memory, if the iBufSize is 0.
             If iBufSize is not 0, the iBufSize will not contain the OP_NULL bytes, i.e. the pBuf must preserve the
             OP_NULL room.
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
int getInputTextStringToBuffer
( 
    MsfInput      *pInput, 
    OP_BOOLEAN    bIncludeInitString, 
    OP_BOOLEAN    bStartFromZero,    
    OP_UINT8     *pBuf, 
    int           iBufSize,
    OP_UINT16     *pInitStringLen
)
{
    OP_UINT8        *pChar;
    OP_UINT16       iSize;
    OP_UINT16       iCopyLen;
     
   if( !pInput || !pBuf || (bIncludeInitString && !pInitStringLen ))
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    /*  
      *  NOTE: In SingleLine Input, the InitString will Can be seen always ; 
      *             but  in MultiLine Input, the InitString can be scrolled to be invisible, 
      *             if so, we can not  put the InitString into the pBuf
      */
    if(  bIncludeInitString == OP_TRUE 
         && pInput->singleLine == 0      /*   MultiLine  Input   */                                   
         && pInput->txtInputBufMng.iLeftTopCharPos != 0 )  /*  the Initstring is invisible because of scroll down  */
    {
        bIncludeInitString = OP_FALSE;
    }
    
    /* calc the number of cope  bytes */
    iSize = iBufSize;
    if( iSize == 0 )
    {
        iSize = getInputTextStringLength( pInput,  bIncludeInitString, OP_FALSE, bStartFromZero);
    }

    if( pInitStringLen )
    {
        *pInitStringLen = 0;
    }    
    pBuf[0] = pBuf[1] = 0x00;
    
    /* get the input text */
    if( ISVALIDHANDLE(pInput->initString) && bIncludeInitString == OP_TRUE )
    {
        /* copy the initString data to pBuf*/
        WIDGET_GET_STRING_DATA(pChar, pInput->initString);
        
        if( !pChar )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }
        
        iCopyLen = (OP_UINT16)Ustrlen(pChar) ;

        if( iCopyLen >  iSize)          /* in case : iSize != 0 */
        {
            iCopyLen = iSize;
        }
        
        if( iCopyLen >= 2 )      /* The initString not empty */
        {
            op_memcpy(pBuf, pChar, iCopyLen);

            /*adjust the pBuf position and the iSize */
            pBuf +=iCopyLen;
            iSize -=iCopyLen;
            pBuf[0] = pBuf[1] = 0x00; /*add OP_NULL to the string end */

            if (pInitStringLen)
            {
                *pInitStringLen  = iCopyLen;
            }
        }
    }
    
    TBM_GetContent(  &(pInput->txtInputBufMng), bStartFromZero, pBuf,iSize);

    return TPI_WIDGET_OK;
}

/*==================================================================================================
    FUNCTION:  getInputTextStringLength

    DESCRIPTION:
        Get the text length  of the pInput gadget in bytes. If the bIncludeInitString is OP_TRUE, the returned length will 
        contain the length of initString of the pInput gadget. If the bMaxSize is OP_TRUE, the returned length
        is maxSize of the Input gadget , or plus the length of the initString  if the bIncludeInitString is OP_TRUE.
        
    ARGUMENTS PASSED:
        pInput                   : specifies the MsfInput 
        bIncludeInitString  : if is OP_TRUE, then the returned length includes the initString length, 
                                        or not includes the initString length
        bMaxSize               : If is OP_TRUE, then the returned length is the maxSize, or plus the length 
                                        of initString if the bIncludeInitString is OP_TRUE
        
    RETURN VALUE:

    IMPORTANT NOTES:
        None
==================================================================================================*/
int getInputTextStringLength
( 
    MsfInput *pInput, 
    OP_BOOLEAN bIncludeInitString, 
    OP_BOOLEAN bMaxSize,
    OP_BOOLEAN bStartFromZero
)
{
    OP_UINT8       *pChar;
    int            iSize;
    
    if( !pInput )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( bIncludeInitString == OP_TRUE && ISVALIDHANDLE(pInput->initString) )
    {
        WIDGET_GET_STRING_DATA(pChar, pInput->initString);
        
        if( !pChar )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }

        iSize = Ustrlen( pChar );
    }
    else
    {
        iSize = 0;
    }

    if( bMaxSize  == OP_TRUE  && pInput->txtInputBufMng.maxCharCount != 0 )
    {
        iSize += pInput->txtInputBufMng.maxCharCount * 2  ;
    }
    else
    {
        /*
        pStr = (MsfString*)seekWidget(pInput->inputString, &widgetType);
        if( !pStr || widgetType != MSF_STRING || pStr->data == OP_NULL )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }

        iSize += Ustrlen( (OP_UINT8*)pStr->data );
        */
        iSize += TBM_GetContentLen(&(pInput->txtInputBufMng), bStartFromZero);
    }

    return iSize;
}

MsfScreen * seekFocusedScreen(void)
{
    return pDisplay->focusedScreen;
}

MsfScreenHandle getFocusedScreenHandle(void)
{
    MsfScreen *pScreen;

    pScreen = seekFocusedScreen();
    if (pScreen != OP_NULL)
    {
        return pScreen->screenHandle;
    }
    else
    {
        return INVALID_HANDLE;
    }
}

void* seekFocusedWindow(void)
{
    MsfScreen *pScreen;
    void *pWindow = OP_NULL;

    pScreen = seekFocusedScreen();
    if (pScreen != OP_NULL)
    {
        if (pScreen->focusedChild != INVALID_HANDLE)
        {
            pWindow = pScreen->children->window;
        }
    }
    return pWindow;
}

#if 0
/*
 * "dst" has been alloced before this function is called
 */
int  UTF8toUnicode(OP_UINT8 * dst, const OP_UINT8 * src)
{
    long     src_len, dst_len;
    OP_UINT8 *pTmp;
    OP_UINT8 tmpChar;

    if (src == OP_NULL) 
    {
       *dst = 0;
       *(dst + 1) = 0;
       return 0;
    }
    src_len = op_strlen((char *)src);
    dst_len = 10000;
    
    if (msf_charset_utf8_to_ucs2 ((char *)src, &src_len, OP_NULL, &dst_len) < 0)
    {
        return -1;
    }
    msf_charset_utf8_to_ucs2 ((char *)src, &src_len, (char *)dst, &dst_len);
    dst[dst_len] = 0;
    dst[dst_len+1] = 0;
    pTmp = dst;
    while ( *pTmp || *(pTmp+1) )
    {
        tmpChar = *pTmp;
        *pTmp = *(pTmp+1);
        *(pTmp+1) = tmpChar;
        pTmp += 2;
    }   
    return dst_len;

}

/*
 * "dst" has been alloced before this function is called
 */
int  UnicodetoUTF8(OP_UINT8 * dst, OP_UINT8 * src)
{
    long  dst_len;
    long  src_len;
    char *srcCopy;
    char *pTmp;
    char tmpChar;
    
    src_len = Ustrlen(src);
    dst_len = 0x7fff;
    srcCopy = WIDGET_ALLOC(src_len+2);
    op_memcpy(srcCopy, (char *)src, src_len);
    
    pTmp = srcCopy;
    while ( *pTmp || *(pTmp+1) )
    {
        tmpChar = *pTmp;
        *pTmp = *(pTmp+1);
        *(pTmp+1) = tmpChar;
        pTmp += 2;
    }   

    msf_charset_ucs2_to_utf8(srcCopy, &src_len, (char *)dst, &dst_len);
    dst[dst_len] = '\0';
    WIDGET_FREE(srcCopy);
    return dst_len;
}
#endif

/*
 * after call this function, "dst" contains the UTF8 string, "dst_len" is the length
 * in bytes of this string
 */
int
unicode_to_utf8 (const char *src, int *src_len,
                     char *dst, int *dst_len)
{
    OP_UINT32  w;
    int        sidx = 0, didx = 0;
    int        sn, dn;
    OP_UINT8   mask;
    int        i0, i1;

    i0 = 0; i1 = 1; 

    for (;;) 
    {
        /* First, translate to UCS4 */
        if (sidx + 2 > *src_len)
            break;
        w = ((unsigned char)src[sidx + i1] << 8) |
            (unsigned char)src[sidx + i0];
        sn = 2;

        /* Determine how many UTF8-bytes are needed for this character,
        * and which bits should be set in the first byte. */
        if (w < 0x80) 
        {
            dn = 1;
            mask = 0;
        }
        else if (w < 0x800) 
        {
            dn = 2;
            mask = 0xc0;
        }
        else 
        {
            dn = 3;
            mask = 0xe0;
        }

        /* Is there room for this in the destination vector? */
        if (didx + dn > *dst_len)
            break;

        /* Only write to destination vector if dst != OP_NULL */
        if (dst != OP_NULL) 
        {
            switch (dn) 
            {
                case 3:
                    dst[didx + 2] = (char) ((w & 0x3f) | 0x80);
                    w >>= 6;
                    /* falltrough */
                case 2:
                    dst[didx + 1] = (char) ((w & 0x3f) | 0x80);
                    w >>= 6;
                    /* falltrough */
                case 1:
                    dst[didx] = (char) (w | mask);
                    break;
                default:
                    break;
            }
        }

        sidx += sn;
        didx += dn;
    }
    *src_len = sidx;
    *dst_len = didx;

    return 0;
}

static const OP_UINT32 charset_offsets_from_utf8[5] =  {
  0UL, 0UL, 0x00003080UL, 0x000e2080UL, 0x03c82080UL};

static const unsigned char charset_utf8_bytes_per_character[16] = {
  1, 1, 1, 1, 1, 1, 1, 1,
  0, 0, 0, 0, 2, 2, 3, 4};

int
utf8_to_unicode (const char *src, int *src_len,
                      char *dst, int *dst_len)
{
    int        sidx = 0, didx = 0;
    int        sn, dn;
    OP_UINT32 w;
    int        i0, i1;

    i0 = 0; i1 = 1; 

    while (sidx < *src_len) 
    {
        /* Read one Unicode character in UTF-8 format: */
        sn = charset_utf8_bytes_per_character[(unsigned char)*src >> 4];
        if (sidx + sn > *src_len)
            break;

        w = 0;
        switch (sn) 
        {
            case 0:
                return -1;

            case 4: w += (unsigned char)*src++; w <<= 6;
            case 3: w += (unsigned char)*src++; w <<= 6;
            case 2: w += (unsigned char)*src++; w <<= 6;
            case 1: w += (unsigned char)*src++;
        }
        w -= charset_offsets_from_utf8[sn];

        /* Write one Unicode character: */
        w &= 0xffff;
        if (didx + 2 > *dst_len)
            break;
        if (dst != OP_NULL) 
        {
            dst[didx + i0] = (char)((w & 0xff));
            dst[didx + i1] = (char)((w >> 8) & 0xff);
        }
        dn = 2;
        sidx += sn;
        didx += dn;
    }
    *src_len = sidx;
    *dst_len = didx;

    return 0;
}


/* calculate the cross points between a horizontal line and another line   */
OP_INT16  widgetGetLinesCrossPoints
( 
    OP_INT16 hline_y, 
    OP_INT16 hline_x_s, 
    OP_INT16 hline_x_e, 
    OP_INT16 line2_x_s,
    OP_INT16 line2_y_s,
    OP_INT16 line2_x_e,
    OP_INT16 line2_y_e,
    OP_INT16 *intersection1_x,
    OP_INT16 *intersection1_y,
    OP_INT16 *intersection2_x,
    OP_INT16 *intersection2_y
)
{
    OP_INT16  iTemp;
    
    if( hline_x_s > hline_x_e )
    {
        iTemp = hline_x_s;
        hline_x_s = hline_x_e;
        hline_x_e = iTemp;
    }
    
    /*  case uprightness:  
      *                   |
      *         -----|-------
      *                   |
      */ 
    if( line2_x_s == line2_x_e )
    {
        if( line2_y_s > line2_y_e )
        {
            iTemp = line2_y_s;
            line2_y_s = line2_y_e;
            line2_y_e = iTemp;
        }
        
        if( IS_IN_RANGE(hline_x_s, hline_x_e, line2_x_s) && IS_IN_RANGE( line2_y_s, line2_y_e, hline_y ) )
        {
            *intersection1_x = line2_x_s;
            *intersection1_y = hline_y;
            return 1;
        }
        else
        {
            return 0;
        }
    }
    
    /*  case parallel:  
      *                   
      *         -------------
      *         -----------          
      */ 
    else if ( line2_y_s == line2_y_e )
    {
        if( line2_y_s != hline_y )
        {
            return 0;
        }
        
        /*  case parallel:  
          *                   
          *         |---/---|-------/
          *                 
          */ 
        if( line2_x_s > line2_x_e )
        {
            iTemp = line2_x_s;
            line2_x_s = line2_x_e;
            line2_x_e = iTemp;
        }
        
          /*  |------|  /-----/     */
        if( line2_x_s > hline_x_e || hline_x_s > line2_x_e  ) 
        {
             return 0;
        }

        /*   |---/---|-------/   */
        *intersection1_y = hline_y;
          
        if(  line2_x_s < hline_x_s )
        {
            *intersection1_x = hline_x_s;
            
            if(  line2_x_e <  hline_x_e )
            {
                return 1;
            }
            else   /*  |----/------/--|    */
            {
                *intersection2_x = hline_x_e;
                *intersection2_y = hline_y;
                return 2;
            }
        }
        else 
        {
            *intersection1_x = hline_x_e;
            
            if( line2_x_e > hline_x_e )
            {
                return 1;
            }
            else  /*  /----|------|--/    */
            {
                *intersection1_x = line2_x_s;
                
                *intersection2_x = line2_x_e;
                *intersection2_y = hline_y;
                return 2;
            }
        }
    }
    else   
    {
        /*                             /                 
          *                            / 
          *                           /   
          *       -------------------
          *                        
          */
        if( (hline_y < line2_y_s && hline_y < line2_y_e) ||(hline_y > line2_y_s && hline_y > line2_y_e) )
        {
            return 0;
        }
        
        /*                             /                 
          *                            / 
          *       ----------/--------
          *                          /   
          *                         /
          */
        iTemp = line2_x_s +(( line2_x_e - line2_x_s ) *( hline_y -line2_y_s ) ) / ( line2_y_e - line2_y_s );

        if( IS_IN_RANGE(hline_x_s, hline_x_e, iTemp) )
        {
            if( line2_x_e > line2_x_s )
            {
                if( IS_IN_RANGE(line2_x_s, line2_x_e, iTemp) )
                {
                    *intersection1_x = iTemp;
                    *intersection1_y = hline_y;
                    return 1;
                }
            }
            else if( IS_IN_RANGE(line2_x_e, line2_x_s, iTemp) )
            {
                *intersection1_x = iTemp;
                *intersection1_y = hline_y;
                return 1;
                
            }
        }
    }

    return 0;
}

/*  get the visible part of  the line  in screen  */
OP_BOOLEAN widgetLineGetVisible
(
    OP_INT16           rectleft,
    OP_INT16           recttop,
    OP_INT16           rectright,
    OP_INT16           rectbottom,
    OP_INT16           *left,
    OP_INT16           *top,
    OP_INT16           *right,
    OP_INT16           *bottom
)
{
    OP_INT16           pt1_x = 0;
    OP_INT16           pt1_y = 0;
    OP_INT16           pt2_x = 0;
    OP_INT16           pt2_y = 0;
    OP_INT16           pt3_x = 0;
    OP_INT16           pt3_y = 0;
    
    OP_INT16           iPoints;
    OP_INT16           iTemp = 0;

    if( rectleft > rectright )
    {
        iTemp = rectleft;
        rectleft = rectright;
        rectright = iTemp;
    }

    if( recttop > rectbottom )
    {
        iTemp = recttop;
        recttop = rectbottom;
        rectbottom = iTemp;
    }

    iPoints = 0;
    if( IS_IN_RANGE( rectleft, rectright, *left )  && IS_IN_RANGE(recttop, rectbottom, *top ))
    {
        iPoints = 1;
        pt1_x = *left;
        pt1_y = *top;        
    }

    if( IS_IN_RANGE( rectleft, rectright, *right )  && IS_IN_RANGE(recttop, rectbottom, *bottom ))
    {
        iPoints ++;
        pt1_x = *right;
        pt1_y = *bottom;        
    }

    if( iPoints == 2 )
    {
        return OP_TRUE;
    }

    if( iPoints == 1 )
    {
        /* top line */
        iTemp = widgetGetLinesCrossPoints( recttop,
                                                            rectleft, 
                                                            rectright, 
                                                            *left, 
                                                            *top, 
                                                            *right , 
                                                            *bottom, 
                                                            &pt2_x, 
                                                            &pt2_y, 
                                                            &pt3_x,
                                                            &pt3_y);
        if( iTemp == 1 )
        {
            if( pt1_x != pt2_x ||pt1_y != pt2_y )
            {
                iPoints ++;
            }
        }
        else if( iTemp == 2 )
        {
            iPoints ++;
            if( pt1_x == pt2_x && pt1_y == pt2_y )
            {
                pt2_x = pt3_x;
                pt2_y = pt3_y;
            }
        }

        if( iPoints == 1 )
        {
            /* bottom line */
            iTemp = widgetGetLinesCrossPoints( rectbottom,
                                                                rectleft, 
                                                                rectright, 
                                                                *left, 
                                                                *top, 
                                                                *right , 
                                                                *bottom, 
                                                                &pt2_x, 
                                                                &pt2_y, 
                                                                &pt3_x,
                                                                &pt3_y);
            if( iTemp == 1 )
            {
                if( pt1_x != pt2_x ||pt1_y != pt2_y )
                {
                    iPoints ++;
                }
            }
            else if( iTemp == 2 )
            {
                iPoints ++;
                if( pt1_x == pt2_x && pt1_y == pt2_y )
                {
                    pt2_x = pt3_x;
                    pt2_y = pt3_y;
                }
            }
        }

        if( iPoints == 1 )
        {
            /* left line */
            iTemp = widgetGetLinesCrossPoints( rectleft,
                                                                recttop, 
                                                                rectbottom, 
                                                                *top, 
                                                                *left, 
                                                                *bottom, 
                                                                *right , 
                                                                &pt2_y, 
                                                                &pt2_x, 
                                                                &pt2_y,
                                                                &pt2_x);
            if( iTemp == 1 )
            {
                if( pt1_x != pt2_x ||pt1_y != pt2_y )
                {
                    iPoints ++;
                }
            }
            else if( iTemp == 2 )
            {
                iPoints ++;
                if( pt1_x == pt2_x && pt1_y == pt2_y )
                {
                    pt2_x = pt3_x;
                    pt2_y = pt3_y;
                }
            }
        }
        
        if( iPoints == 1 )
        {
            /* right line */
            iTemp = widgetGetLinesCrossPoints( rectright,
                                                                recttop, 
                                                                rectbottom, 
                                                                *top, 
                                                                *left, 
                                                                *bottom, 
                                                                *right , 
                                                                &pt2_y, 
                                                                &pt2_x, 
                                                                &pt2_y,
                                                                &pt2_x);
            if( iTemp == 1 )
            {
                if( pt1_x != pt2_x ||pt1_y != pt2_y )
                {
                    iPoints ++;
                }
            }
            else if( iTemp == 2 )
            {
                iPoints ++;
                if( pt1_x == pt2_x && pt1_y == pt2_y )
                {
                    pt2_x = pt3_x;
                    pt2_y = pt3_y;
                }
            }
        }

        if( iPoints == 2 )
        {
            if( pt1_x >= pt2_x && pt1_y >= pt2_y )
            {
                *left = pt2_x;
                *top = pt2_y;
                *right = pt1_x;
                *bottom = pt1_y;
            }
            else 
            {
                *left = pt1_x;
                *top = pt1_y;
                *right = pt2_x;
                *bottom = pt2_y;
            }

            return OP_TRUE;
        }
        else
        {
            return OP_FALSE;
        }
        
    }

    /* top line  */
    iTemp = widgetGetLinesCrossPoints( recttop,
                                                        rectleft, 
                                                        rectright, 
                                                        *left, 
                                                        *top, 
                                                        *right , 
                                                        *bottom, 
                                                        &pt1_x, 
                                                        &pt1_y, 
                                                        &pt2_x,
                                                        &pt2_y);
                                                        
    if( iTemp == 0 )    /* no intersection  */
    {
        /*  bottom line  */
        iTemp = widgetGetLinesCrossPoints( rectbottom,
                                                            rectleft, 
                                                            rectright, 
                                                            *left, 
                                                            *top, 
                                                            *right , 
                                                            *bottom, 
                                                            &pt1_x, 
                                                            &pt1_y, 
                                                            &pt2_x,
                                                            &pt2_y);
    }
    else if( iTemp == 1 )
    {
        iPoints ++;
        
        /*  bottom line  */
        iTemp = widgetGetLinesCrossPoints( rectbottom,
                                                            rectleft, 
                                                            rectright, 
                                                            *left, 
                                                            *top, 
                                                            *right , 
                                                            *bottom, 
                                                            &pt2_x, 
                                                            &pt2_y, 
                                                            OP_NULL,
                                                            OP_NULL);
    }
    else
    {
        iPoints = 2;
    }

    if( iPoints != 2 )
    {
        if( iTemp == 2 )
        {
            iPoints = 2;
        }
        else if( iTemp == 0 )
        {
            if( iPoints == 1 )
            {
                /* left line   */
                iTemp = widgetGetLinesCrossPoints( rectleft,
                                                                    recttop, 
                                                                    rectbottom, 
                                                                    *top, 
                                                                    *left, 
                                                                    *bottom, 
                                                                    *right , 
                                                                    &pt2_y, 
                                                                    &pt2_x, 
                                                                    OP_NULL,
                                                                    OP_NULL);
            }
            else
            {
                /* left line   */
                iTemp = widgetGetLinesCrossPoints( rectleft,
                                                                    recttop, 
                                                                    rectbottom, 
                                                                    *top, 
                                                                    *left, 
                                                                    *bottom, 
                                                                    *right , 
                                                                    &pt1_y, 
                                                                    &pt1_x, 
                                                                    &pt2_y,
                                                                    &pt2_x);
            }
        }
        else    /*iTemp == 1 */
        {
            if( iPoints == 1 )
            {
                if( pt1_x != pt2_x ||pt1_y != pt2_y )
                {
                    iPoints ++;
                }
                else
                {
                    /* left line   */
                    iTemp = widgetGetLinesCrossPoints( rectleft,
                                                                        recttop, 
                                                                        rectbottom, 
                                                                        *top, 
                                                                        *left, 
                                                                        *bottom, 
                                                                        *right , 
                                                                        &pt2_y, 
                                                                        &pt2_x, 
                                                                        OP_NULL,
                                                                        OP_NULL);
                }
            }
            else    /* iPoints == 0  */
            {
                iPoints ++;
                
                /* left line   */
                iTemp = widgetGetLinesCrossPoints( rectleft,
                                                                    recttop, 
                                                                    rectbottom, 
                                                                    *top, 
                                                                    *left, 
                                                                    *bottom, 
                                                                    *right , 
                                                                    &pt2_y, 
                                                                    &pt2_x, 
                                                                    OP_NULL,
                                                                    OP_NULL);
            }
        }
    }

    if( iPoints != 2 )
    {
        if( iTemp == 2 )
        {
            iPoints = 2;
        }
        else if( iTemp == 0 )
        {
            if( iPoints == 1 )
            {
                /* right line */
                iTemp = widgetGetLinesCrossPoints( rectright,
                                                                    recttop, 
                                                                    rectbottom, 
                                                                    *top, 
                                                                    *left, 
                                                                    *bottom, 
                                                                    *right , 
                                                                    &pt2_y, 
                                                                    &pt2_x, 
                                                                    OP_NULL,
                                                                    OP_NULL);
            }
            else      /* iPoints == 0  */
            {
                /* right line */
                iTemp = widgetGetLinesCrossPoints( rectright,
                                                                    recttop, 
                                                                    rectbottom, 
                                                                    *top, 
                                                                    *left, 
                                                                    *bottom, 
                                                                    *right , 
                                                                    &pt1_y, 
                                                                    &pt1_x, 
                                                                    &pt2_y,
                                                                    &pt2_x);
            }
        }
        else  /* iTemp == 1  */
        {
            if( iPoints == 1 )
            {
                if( pt1_x != pt2_x ||pt1_y != pt2_y )
                {
                    iPoints ++;
                }
                else
                {
                    /* right line */
                    iTemp = widgetGetLinesCrossPoints( rectright,
                                                                        recttop, 
                                                                        rectbottom, 
                                                                        *top, 
                                                                        *left, 
                                                                        *bottom, 
                                                                        *right , 
                                                                        &pt2_y, 
                                                                        &pt2_x, 
                                                                        OP_NULL,
                                                                        OP_NULL);
                }
            }
            else  /*  iPoints == 0  */
            {
                iPoints ++;
                
                /* right line */
                iTemp = widgetGetLinesCrossPoints( rectright,
                                                                    recttop, 
                                                                    rectbottom, 
                                                                    *top, 
                                                                    *left, 
                                                                    *bottom, 
                                                                    *right , 
                                                                    &pt2_y, 
                                                                    &pt2_x, 
                                                                    OP_NULL,
                                                                    OP_NULL);
            }
        }
    }

    /* right line */
    if( iPoints != 2 )
    {
        if( iTemp == 2 )
        {
            iPoints = 2;
        }
        else if( iTemp == 1 )
        {
            if( iPoints == 1 )
            {
                if( pt1_x != pt2_x ||pt1_y != pt2_y )
                {
                    iPoints ++;
                }
            }
            else
            {
                iPoints++;
            }
        }
    }

    if( iPoints == 2 )
    {
        if( pt1_x >= pt2_x && pt1_y >= pt2_y )
        {
            *left = pt2_x;
            *top = pt2_y;
            *right = pt1_x;
            *bottom = pt1_y;
        }
        else 
        {
            *left = pt1_x;
            *top = pt1_y;
            *right = pt2_x;
            *bottom = pt2_y;
        }

        return OP_TRUE;
    }
    else
    {
        return OP_FALSE;
    }
    
}

/*  get the visible part of  the line  in screen  */
OP_BOOLEAN widgetRectGetVisible
(
    OP_INT16           rectleft,
    OP_INT16           recttop,
    OP_INT16           rectright,
    OP_INT16           rectbottom,
    OP_INT16           *pLeft,
    OP_INT16           *pTop,
    OP_INT16           *pRight,
    OP_INT16           *pBottom
)
{
    OP_INT16           r1_left;
    OP_INT16           r1_top;
    OP_INT16           r1_right;
    OP_INT16           r1_bottom;
    
    OP_INT16           max_left;
    OP_INT16           max_top;
    OP_INT16           min_right;
    OP_INT16           min_bottom;

    OP_INT16           iTemp;

    if( !pLeft ||!pTop ||!pRight ||!pBottom )
    {
        return OP_FALSE;
    }
    
    if( rectleft > rectright )
    {
        iTemp = rectleft;
        rectleft = rectright;
        rectright = iTemp;
    }

    if( recttop > rectbottom )
    {
        iTemp = recttop;
        recttop = rectbottom;
        rectbottom = iTemp;
    }

    r1_left = *pLeft;
    r1_top = *pTop;
    r1_right = *pRight;
    r1_bottom = *pBottom;

    if( r1_left > r1_right )
    {
        iTemp = r1_right;
        r1_right = r1_left;
        r1_left = iTemp;
    }

    if( r1_top > r1_bottom )
    {
        iTemp = r1_top;
        r1_top = r1_bottom;
        r1_bottom = iTemp;
    }

    max_left = MAX(r1_left, rectleft);
    max_top = MAX(r1_top, recttop );
    min_right = MIN(r1_right, rectright );
    min_bottom = MIN(r1_bottom, rectbottom);

    if( max_left <= min_right && max_top <= min_bottom )
    {
        *pLeft = max_left;
        *pTop = max_top;
        *pRight = min_right;
        *pBottom = min_bottom;

        return OP_TRUE;
    }

    return OP_FALSE;
}   

int  widgetRGBToHSL( int  rgb)
{
    OP_UINT16  maxRGB, minRGB;
    OP_INT16    delta, delta2;
    MsfWidgetColor  colorRGB;
    MsfWidgetColor  color;

    colorRGB.color = rgb;
    
    if( colorRGB.colorRGB.r > colorRGB.colorRGB.g )
    {
        maxRGB = colorRGB.colorRGB.r;
        minRGB = colorRGB.colorRGB.g;
    }
    else
    {
        maxRGB = colorRGB.colorRGB.g;
        minRGB = colorRGB.colorRGB.r;
    }

    if( maxRGB < colorRGB.colorRGB.b )
    {
        maxRGB = colorRGB.colorRGB.b;
    }
    else if( minRGB > colorRGB.colorRGB.b )
    {
        minRGB= colorRGB.colorRGB.b;
    }

    if( minRGB == maxRGB )
    {
        color.colorHSL.l = (OP_UINT8) minRGB;
        color.colorHSL.s = 0;
        color.colorHSL.h = 0;
    }
    else
    {
        color.colorHSL.l = (OP_UINT8)((maxRGB + minRGB + 1 ) >> 1);
        
        delta = (maxRGB - minRGB) ;
        
        if( color.colorHSL.l < 128 )
        {
            color.colorHSL.s = (OP_UINT8)( delta * 255 /( maxRGB + minRGB));
        }
        else
        {
            color.colorHSL.s = (OP_UINT8)( delta * 255 /( 510 - maxRGB - minRGB));
        }

        delta2 = delta / 2;
        
        if( colorRGB.colorRGB.r == maxRGB)
        {
            if( colorRGB.colorRGB.b > colorRGB.colorRGB.g)
            {
                delta2 = -delta2;
            }
            
            color.colorHSL.h = ((colorRGB.colorRGB.g-colorRGB.colorRGB.b) * 60  + delta2)/ delta;
            if( color.colorHSL.h < 0 )
            {
                color.colorHSL.h += 360;
            }
        }
        else if( colorRGB.colorRGB.g == maxRGB)
        {
            if( colorRGB.colorRGB.r > colorRGB.colorRGB.b)
            {
                delta2 = -delta2;
            }
            
            color.colorHSL.h = 120 + ((colorRGB.colorRGB.b-colorRGB.colorRGB.r)* 60 + delta2) /delta;
        }
        else if( colorRGB.colorRGB.b == maxRGB)
        {
            if( colorRGB.colorRGB.g > colorRGB.colorRGB.r)
            {
                delta2 = -delta2;
            }
            
            color.colorHSL.h = 240 + ((colorRGB.colorRGB.r -colorRGB.colorRGB.g)* 60 + delta2) /delta;
        }
    }
    
    return color.color;
}


int  widgetHSLToRGB( int  hsl )
{
    MsfWidgetColor  color;
    MsfWidgetColor  colorHSL;
    OP_UINT16        t1, t2, t3, c;

    colorHSL.color = hsl;
    if (colorHSL.colorHSL.s == 0)
    {
        color.colorRGB.r = color.colorRGB.g = color.colorRGB.b = colorHSL.colorHSL.l;
    }
    else
    {
        if( colorHSL.colorHSL.l < 128 )
        {
            t2 = colorHSL.colorHSL.l * ( 255 + colorHSL.colorHSL.s );
        }
        else
        {
            t2 = (colorHSL.colorHSL.l * ( 255 - colorHSL.colorHSL.s )) + colorHSL.colorHSL.s * 255 ;
        }

        t1 = 510 * colorHSL.colorHSL.l - t2;

        /* R */
        t3 = colorHSL.colorHSL.h + 120 ; 
        if( t3 > 360 )
        {
            t3 -= 360;
        }
        
        if( t3 < 60 )
        {
            c = (t1 + ( t2 - t1 ) * t3 / 60 + 128 ) / 255;
        }
        else if( t3 < 180 )
        {
            c =  ( t2 + 128 ) / 255;
        }
        else if( t3 < 240 )
        {
            c = (t1 + ( t2 - t1 )*( 240 - t3 ) / 60  + 128) / 255 ;
        }
        else
        {
            c = (t1 + 128 ) / 255;
        }

        if( c > 255 )
        {
            color.colorRGB.r = 255;
        }
        else
        {
            color.colorRGB.r = (OP_UINT8)c;
        }
        
        /* G */
        t3 = colorHSL.colorHSL.h ;
        
        if( t3 < 60 )
        {
            c = (t1 + ( t2 - t1 ) * t3 / 60 + 128 ) / 255;
        }
        else if( t3 < 180 )
        {
            c =  ( t2 + 128 ) / 255;
        }
        else if( t3 < 240 )
        {
            c = (t1 + ( t2 - t1 )*( 240 - t3 ) / 60  + 128) / 255 ;
        }
        else
        {
            c = (t1 + 128 ) / 255;
        }

        if( c > 255 )
        {
            color.colorRGB.g = 255;
        }
        else
        {
            color.colorRGB.g = (OP_UINT8)c;
        }
        
        /* B */
        if( colorHSL.colorHSL.h > 120 )
        {
            t3 = colorHSL.colorHSL.h  - 120 ;
        }
        else
        {
            t3 = 240 + colorHSL.colorHSL.h;
        }
        
        if( t3 < 60 )
        {
            c = (t1 + ( t2 - t1 ) * t3 / 60 + 128 ) / 255;
        }
        else if( t3 < 180 )
        {
            c =  ( t2 + 128 ) / 255;
        }
        else if( t3 < 240 )
        {
            c = (t1 + ( t2 - t1 )*( 240 - t3 ) / 60  + 128) / 255 ;
        }
        else
        {
            c = (t1 + 128 ) / 255;
        }

        if( c > 255 )
        {
            color.colorRGB.b = 255;
        }
        else
        {
            color.colorRGB.b = (OP_UINT8)c;
        }
    }

    return color.color;
}

int widgetGetDrawingInfo
( 
    void                  *pWidget, 
    MsfWidgetType   widgetType, 
    OP_UINT32       msfHandle,
    DS_FONTATTR  *pFontattr, 
    DS_COLOR       *pFontcolor, 
    DS_COLOR       *pBackcolor
)
{
    void                   *pObj;
    MsfStyle               style;
    int                      iRet;
    
    if( !pWidget )
    {
        if( msfHandle == INVALID_HANDLE )
        {
            return TPI_WIDGET_ERROR_INVALID_PARAM;
        }

        pObj = seekWidget(msfHandle, &widgetType);
    }
    else
    {
        pObj = pWidget;
    }

    iRet = widgetGetStyleInfo( pObj, widgetType, 0, &style );
    if( iRet >= 0 )
    {
        if( pFontattr ) 
        {  
            /* only consider the bold style and  font size */
            *pFontattr = FONT_NORMAL;
            if( style.font.weight > 5)
            {
                *pFontattr |= FONT_BOLD;
            }

            if( style.font.size < FONT_SIZE_SEPERATOR )
            {
                *pFontattr |= FONT_SIZE_SMALL;
            }
            else
            {
                *pFontattr |= FONT_SIZE_MED;
            }
            
            if( style.textProperty.decoration & MSF_TEXT_DECORATION_UNDERLINE )
            {
                *pFontattr |= FONT_UNDERLINE;
            }

            if( style.textProperty.decoration & MSF_TEXT_DECORATION_SHADOW )
            {
                *pFontattr |= FONT_SHADOW;
            }
        }

        if( pFontcolor )
        {
            *pFontcolor =(DS_COLOR) _RGB((OP_UINT16)style.color.r, (OP_UINT16)style.color.g, (OP_UINT16)style.color.b);
        }

        if( pBackcolor )
        {
            *pBackcolor =(DS_COLOR) _RGB((OP_UINT16)style.backgroundColor.r, (OP_UINT16)style.backgroundColor.g, (OP_UINT16)style.backgroundColor.b);
        }
    }
    return iRet;
}

int widgetGetStyleInfo
(
    void                      *pWidget, 
    MsfWidgetType     widgetType, 
    OP_UINT32         msfHandle,
    MsfStyle                 *pStyle
)
{
    void                   *pObj;
    int                      iRet;
    
    if( !pStyle )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    if( !pWidget )
    {
        if( msfHandle == INVALID_HANDLE )
        {
            return TPI_WIDGET_ERROR_INVALID_PARAM;
        }

        pObj = seekWidget(msfHandle, &widgetType);
        if( !pObj )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }
    }
    else
    {
        pObj = pWidget;
    }

    if(IS_WINDOW(widgetType) ) 
    {
        iRet = widgetWindowGetStyle( (MsfWindow*)pObj, widgetType, pStyle );
    }
    else if( IS_GADGET(widgetType) )
    {
        iRet = widgetGadgetGetStyle( (MsfGadget*)pObj, widgetType, pStyle );
    }
    else if( IS_SCREEN(widgetType) )
    {
        iRet = widgetScreenGetStyle( (MsfScreen*)pObj, widgetType, pStyle );
    }
    else if( IS_LOWLEVEL(widgetType) )
    {
        iRet = widgetLowlevelGetStyle( pObj, widgetType, pStyle );
    }
    else
    {
        iRet = TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    return iRet;
}

static int widgetScreenGetStyle( MsfScreen* pScreen, MsfWidgetType widgetType, MsfStyle* pStyle )
{
    return TPI_WIDGET_ERROR_NOT_SUPPORTED;

# if 0
    MsfStyle               *pWidgetStyle;
    MsfWidgetType  widgetType;
    
    if( !pScreen || !pStyle )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( ISVALIDHANDLE(pScreen->style) )
    {
        pWidgetStyle = (MsfStyle*)seekWidget( pScreen->style ,  &widgetType);
        if( !pWidgetStyle || widgetType != MSF_STYLE )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }

        op_memcpy( pStyle, pWidgetStyle, sizeof(MsfStyle));
    }
    else
    {
        op_memset( pStyle, 0x00, sizeof(MsfStyle));
    }
    
    return TPI_WIDGET_OK;
#endif    
}

static int widgetWindowGetStyle( MsfWindow* pWin, MsfWidgetType widgetType, MsfStyle* pStyle )
{
    MsfStyle               *pWidgetStyle;
    MsfWidgetType  wt;
    
    if( !pWin || !pStyle )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( ISVALIDHANDLE(pWin->style) )
    {
        pWidgetStyle = (MsfStyle*)seekWidget( pWin->style ,  &wt);
        if( !pWidgetStyle || wt != MSF_STYLE )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }

        op_memcpy( pStyle, pWidgetStyle, sizeof(MsfStyle));
    }
    else
    {
        if( IS_WINDOW(widgetType) )
        {
            op_memcpy( &pStyle->color, DefaultPropTab[widgetType].fgColors, sizeof(MsfColor));
            op_memcpy( &pStyle->backgroundColor, DefaultPropTab[widgetType].bgColors, sizeof(MsfColor));
            op_memcpy( &pStyle->foreground, &DEFAULT_STYLE_FOREGROUND, sizeof(MsfPattern));
            op_memcpy( &pStyle->background, &DEFAULT_STYLE_BACKGROUND, sizeof(MsfPattern));
            op_memcpy( &pStyle->lineStyle, &DEFAULT_STYLE_LINESTYLE, sizeof(MsfLineStyle));
            if( MSF_MENU == widgetType|| MSF_MAINMENU ==  widgetType )
            {
                op_memcpy( &pStyle->font, &DEFAULT_MENU_STYLE_FONT, sizeof(MsfFont));
            }
            else if ( MSF_DIALOG == widgetType )
            {
                op_memcpy( &pStyle->font, &DEFAULT_DIALOG_STYLE_FONT, sizeof(MsfFont));
            }
            else
            {
                op_memcpy( &pStyle->font, &DEFAULT_STYLE_FONT, sizeof(MsfFont));
            }
            op_memcpy( &pStyle->textProperty, &DEFAULT_STYLE_TEXTPROPERTY, sizeof(MsfTextProperty));
        }
        else
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }
    }
    
    return TPI_WIDGET_OK;
}

static int widgetGadgetGetStyle( MsfGadget* pGadget, MsfWidgetType widgetType, MsfStyle* pStyle )
{
    MsfStyle               *pWidgetStyle;
    MsfWidgetType  wt;
    
    if( !pGadget || !pStyle )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( ISVALIDHANDLE(pGadget->style) )
    {
        pWidgetStyle = (MsfStyle*)seekWidget( pGadget->style ,  &wt);
        if( !pWidgetStyle || wt != MSF_STYLE )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }

        op_memcpy( pStyle, pWidgetStyle, sizeof(MsfStyle));
    }
    else if( pGadget->parent != OP_NULL )
    {
        return widgetWindowGetStyle( pGadget->parent, getWidgetType(pGadget->parent->windowHandle), pStyle);
    }
    else
    {
        if( IS_GADGET(widgetType) )
        {
            op_memcpy( &pStyle->color, DefaultPropTab[widgetType].fgColors, sizeof(MsfColor));
            op_memcpy( &pStyle->backgroundColor, DefaultPropTab[widgetType].bgColors, sizeof(MsfColor));
            op_memcpy( &pStyle->foreground, &DEFAULT_STYLE_FOREGROUND, sizeof(MsfPattern));
            op_memcpy( &pStyle->background, &DEFAULT_STYLE_BACKGROUND, sizeof(MsfPattern));
            op_memcpy( &pStyle->lineStyle, &DEFAULT_STYLE_LINESTYLE, sizeof(MsfLineStyle));
            op_memcpy( &pStyle->font, &DEFAULT_STYLE_FONT, sizeof(MsfFont));
            op_memcpy( &pStyle->textProperty, &DEFAULT_STYLE_TEXTPROPERTY, sizeof(MsfTextProperty));
        }
        else
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }
    }
    
    return TPI_WIDGET_OK;
}


static int widgetLowlevelGetStyle( void* pWidget, MsfWidgetType widgetType, MsfStyle* pStyle )
{
    MSF_UINT32      *pHStyle;
    MsfStyle               *pWidgetStyle;
    MsfWidgetType  wt;
    
    if( !pWidget || !pStyle )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( widgetType == MSF_STYLE )
    {
        op_memcpy( pStyle, pWidget, sizeof(MsfStyle));
        return TPI_WIDGET_OK;
    }
    
    switch( widgetType )
    {
        case MSF_IMAGE:
        {
            pHStyle = &((MsfImage*)pWidget)->style;
            break;
        }
        case MSF_STRING:
        {
            pHStyle = &((MsfString*)pWidget)->style;
            break;
        }
        case MSF_ICON:
        {
            pHStyle = &((MsfIcon*)pWidget)->style;
            break;
        }
#if 0        
/*        case MSF_STYLE:
        {
            pHStyle = (void*)pWidget;
            break;
        }
*/        
        case MSF_SOUND:
        {
            pHStyle = ;
            break;
        }
        case MSF_COLORANIM:
        {
            pHStyle = ;
            break;
        }
        case MSF_MOVE:
        {
            pHStyle = ;
            break;
        }
        case MSF_ROTATION:
        {
            pHStyle = ;
            break;
        }
#endif

        default:
        {
            pHStyle = OP_NULL;
            break;
        }
    }

    if( !pHStyle )
    {
        return TPI_WIDGET_ERROR_NOT_SUPPORTED;
    }
    
    if( ISVALIDHANDLE(*pHStyle) )
    {
        pWidgetStyle = (MsfStyle*)seekWidget( *pHStyle ,  &wt);
        if( !pWidgetStyle || wt != MSF_STYLE )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }

        op_memcpy( pStyle, pWidgetStyle, sizeof(MsfStyle));
    }
    else
    {
        op_memcpy( &pStyle->color, DefaultPropTab[widgetType].fgColors, sizeof(MsfColor));
        op_memcpy( &pStyle->backgroundColor, DefaultPropTab[widgetType].bgColors, sizeof(MsfColor));
        op_memcpy( &pStyle->foreground, &DEFAULT_STYLE_FOREGROUND, sizeof(MsfPattern));
        op_memcpy( &pStyle->background, &DEFAULT_STYLE_BACKGROUND, sizeof(MsfPattern));
        op_memcpy( &pStyle->lineStyle, &DEFAULT_STYLE_LINESTYLE, sizeof(MsfLineStyle));
        op_memcpy( &pStyle->font, &DEFAULT_STYLE_FONT, sizeof(MsfFont));
        op_memcpy( &pStyle->textProperty, &DEFAULT_STYLE_TEXTPROPERTY, sizeof(MsfTextProperty));
    }
    
    return TPI_WIDGET_OK;
}

OP_UINT8*  widgetGetPredefinedStringEx(OP_UINT8 modId, OP_UINT8 lang, OP_UINT16 index)
{
    char         **pEngTab;
    char         **pChnTab;
    OP_UINT8     *pStr;
    OP_UINT16    iMaxSize;

    switch (modId)
    {
        case MSF_MODID_MSM:
            pEngTab = (char**)msf_strings[0];
            pChnTab = (char**)msf_strings[1];
            iMaxSize = MSF_NUMBER_OF_STRINGS;
            break;
            
        case MSF_MODID_STK:
            pEngTab = (char**)stk_strings[0];
            pChnTab = (char**)stk_strings[1];
            iMaxSize = STK_NUMBER_OF_STRINGS;
            break;
            
        case MSF_MODID_BRA:
            pEngTab = (char**)bra_strings[0];
            pChnTab = (char**)bra_strings[1];
            iMaxSize = BRA_NUMBER_OF_STRINGS;
            break;
            
        case MSF_MODID_BRS:
            pEngTab = (char**)brs_strings[0];
            pChnTab = (char**)brs_strings[1];
            iMaxSize = BRS_NUMBER_OF_STRINGS;
            break;
            
        case MSF_MODID_PHS:
            pEngTab = (char**)phs_strings[0];
            pChnTab = (char**)phs_strings[1];
            iMaxSize = PHS_NUMBER_OF_STRINGS;
            break;
            
        /*case MSF_MODID_IBS:
            pEngTab = (char**)ibs_strings[0];
            pChnTab = (char**)ibs_strings[1];
            iMaxSize = IBS_NUMBER_OF_STRINGS;
            break;*/
            
        /*case MSF_MODID_PRS:
            pEngTab = (char**)prs_strings[0];
            pChnTab = (char**)prs_strings[1];
            iMaxSize = PRS_NUMBER_OF_STRINGS;
            break;*/
            
        /*case MSF_MODID_SEC:
            pEngTab = (char**)sec_strings[0];
            pChnTab = (char**)sec_strings[1];
            iMaxSize = SEC_NUMBER_OF_STRINGS;
            break;
            
        case MSF_MODID_MEA:
            pEngTab = (char**)mea_strings[0];
            pChnTab = (char**)mea_strings[1];
            iMaxSize = MEA_NUMBER_OF_STRINGS;
            break;*/
            
        case MSF_MODID_MMS:
            pEngTab = (char**)mms_strings[0];
            pChnTab = (char**)mms_strings[1];
            iMaxSize = MMS_NUMBER_OF_STRINGS;
            break;
            
        /*case MSF_MODID_SLS:
            pEngTab = (char**)sls_strings[0];
            pChnTab = (char**)sls_strings[1];
            iMaxSize = SLS_NUMBER_OF_STRINGS;
            break;
            
        case MSF_MODID_SMA:
            pEngTab = (char**)sma_strings[0];
            pChnTab = (char**)sma_strings[1];
            iMaxSize = SMA_NUMBER_OF_STRINGS;
            break;*/
            
        case MSF_MODID_UIA:
            pEngTab = (char**)uia_strings[0];
            pChnTab = (char**)uia_strings[1];
            iMaxSize = UIA_NUMBER_OF_STRINGS;
            break;
            
        case MSF_MODID_RMA:
            pEngTab = (char**)rma_strings[0];
            pChnTab = (char**)rma_strings[1];
            iMaxSize = RMA_NUMBER_OF_STRINGS;
            break; 
            
        case MSF_MODID_UMB:
            pEngTab = (char**)umb_strings[0];
            pChnTab = (char**)umb_strings[1];
            iMaxSize = UMB_NUMBER_OF_STRINGS;
            break;

        default:
            return OP_NULL;
            break;
            
    }

    if( index >= iMaxSize )
    {
        return OP_NULL;
    }
    
    if ( lang == 0 ) /* Chinese */
    {
        pStr = (OP_UINT8 *)(*(pChnTab + index));
        if (pStr[0] == 0x00 && pStr[1] == 0x00 )
        {
            pStr = (OP_UINT8 *)(*(pEngTab + index));
        }
    }
    else  /* treat as English */
    {
        pStr = (OP_UINT8 *)(*(pEngTab + index));
        if (pStr[0] == 0x00 && pStr[1] == 0x00 )
        {
            pStr = (OP_UINT8 *)(*(pChnTab + index));
        }
    }

    return pStr;
}

OP_UINT8*  widgetGetPredefinedString( OP_UINT32 resId )
{
    OP_UINT8    modId;
    OP_UINT8    lang;
    OP_UINT16   index;

    /* The lower 8 bit of all the resId is its modId */
    modId = (OP_UINT8)(resId & 0xFF);
    index = (OP_UINT16)((resId >> 8) - 1);
    
    if (RM_GetCurrentLanguage() == RM_LANG_CHN)
    {
        lang = 0;
    }
    else
    {   
        lang = 1;
    }
    
    return widgetGetPredefinedStringEx(modId, lang, index);
}


/*
  *  NOTE: the index of 0x0100 and 0x0200 are used for folder icon and file icon resource id
  */
RM_ICON_T *WidgetGetPredefineIcon
(
    OP_UINT32    resId
)
{
    MSF_UINT8    modId;
    OP_INT16     index;
    const MSF_UINT32   *iconList;

    /* The lower 8 bit of all the resId is its modId */
    modId = (OP_UINT8)(resId & 0xFF);

    switch (modId)
    {
        case MSF_MODID_MSM:
            iconList = msf_icons;
            break;
            
/*        case MSF_MODID_STK:
            iconList = stk_icons;
            break;*/
#ifdef MAIN_MENU_STYLE_IMAGE_TEXT            
        case MSF_MODID_BRA:
            iconList = bra_icons;
            break;
#endif            
/*        case MSF_MODID_BRS:
            iconList = brs_icons;
            break;*/
            
/*        case MSF_MODID_PHS:
            iconList = phs_icons;
            break;*/
            
        /*case MSF_MODID_IBS:
            iconList = ibs_icons;
            break;*/
            
        /*case MSF_MODID_PRS:
            iconList = prs_icons;
            break;*/
            
        /*case MSF_MODID_SEC:
            iconList = sec_icons;
            break;
            
        case MSF_MODID_MEA:
            iconList = mea_icons;
            break;*/
            
        case MSF_MODID_UMB:
            iconList = umb_icons;
            break;
            
/*        case MSF_MODID_MMS:
            iconList = mmsf_icons;
            break;*/
            
        /*case MSF_MODID_SLS:
            iconList = msf_icons;
            break;
            
        case MSF_MODID_SMA:
            iconList = sma_icons;
            break;*/
            
        case MSF_MODID_UIA:
            iconList = uia_icons;
            break;
            
 /*       case MSF_MODID_IT2:
            iconList = it2_icons;
            break; */

        default:
            return OP_NULL;
            break;
            
    }    
    
    index = (OP_INT16)(( resId >> 8) - 1);    

    return( (RM_ICON_T*)util_get_icon_from_res( iconList[index] ));

}

static int  widgetImageGadgetGetSize
( 
    MsfImageGadget*  pImageGadget, 
    OP_UINT32        hImageGadget,
    MsfSize            *pSize
)
{
    MsfGadget        *pGadget;
    MsfWidgetType  wt;
    MsfImage           *pImage;

    if( !pSize )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    op_memset( pSize, 0, sizeof(MsfSize));

    if( !pImageGadget )
    {
        if( ISVALIDHANDLE( hImageGadget ) )
        {
            pImageGadget = (MsfImageGadget*)seekWidget( hImageGadget, &wt );
            if( !pImageGadget || wt != MSF_IMAGEGADGET )
            {
                return TPI_WIDGET_ERROR_UNEXPECTED;
            }
        }
        else
        {
            return TPI_WIDGET_ERROR_INVALID_PARAM;
        }
    }

    pGadget = (MsfGadget*)pImageGadget;
    if( pGadget->size.width!= 0 || pGadget->size.height != 0 )
    {
        *pSize = pGadget->size;
    }
    else if( ISVALIDHANDLE( pImageGadget->image )) /*  use image size as the gadget size  */
    {
        pImage = (MsfImage*)seekWidget(pImageGadget->image, &wt );
        if( !pImage || wt != MSF_IMAGE )
        {
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }

        if(pImage->bOriginal == OP_TRUE ) 
        {
            if( pImage->moreData || pImage->data == OP_NULL)
            {
                return TPI_WIDGET_OK;
            }

            if( pImage->size.height <= 0 || pImage->size.width <= 0 )
            {
                if(OP_FALSE == ds_get_file_image_size(
                                        convertImageFormat( pImage->format ), 
                                        pImage->data,
                                        pImage->dataSize,
                                        (OP_UINT16*) &pImage->size.width,
                                        (OP_UINT16*) &pImage->size.height))
                {
                    op_memset( &pImage->size, 0, sizeof(MsfSize));
                    *pSize = DefaultPropTab[MSF_IMAGE].size;
                    return TPI_WIDGET_ERROR_UNEXPECTED;
                }
            }
        }
        
        *pSize = pImage->size;
        
        if( OP_FALSE == IsValidImageRegion(pSize->width, pSize->height) )
        {
            *pSize = DefaultPropTab[MSF_IMAGE].size;
        }
    }
    else
    {
        *pSize = DefaultPropTab[MSF_IMAGEGADGET].size;
    }

    return TPI_WIDGET_OK;
}

void saveScreen(void)
{
    if(pDisplay->focusedScreen != OP_NULL)
    {
        pDisplay->hScreenSave = pDisplay->focusedScreen->screenHandle;
        TPIa_widgetRemoveAllScreenFocus();
        clearBackupDisplay();     
    }
}

void restoreScreen(void)
{
    MsfScreenHandle  hScreen;

    /* force to refresh annunciator */
    widgetScreenSetMode();
    
#ifdef _TOUCH_PANEL
    TPO_SetCurrent_RPU(TPO_RUI_NONE);
#endif

    if( pDisplay->hScreenSave != INVALID_HANDLE 
        && widgetAppIsCurrentFocusApp(getModID(pDisplay->hScreenSave)))
    {
        hScreen = pDisplay->hScreenSave;
        pDisplay->hScreenSave = INVALID_HANDLE;
        
        TPIa_widgetSetInFocus( hScreen, 1);
    }
}

void widgetNumtoUstr(MSF_UINT8 *dest, const MSF_INT32 num)
{
    if(dest != OP_NULL)
    {
        NumtoU(dest, num);
    }
}

void widgetUstrCat(MSF_UINT8 *dest, MSF_UINT8 *src, MSF_UINT32 resId)
{
    if(dest != OP_NULL)
    {
        if(src != OP_NULL)
        {
            Ustrcat(dest, src);
        }else if(resId != 0)
        {
            Ustrcat(dest, widgetGetPredefinedString(resId));
        }
    }
}

void widgetUstrCpy(MSF_UINT8 *dest, MSF_UINT8 *src, MSF_UINT32 resId)
{
    if(dest != OP_NULL)
    {
        if(src != OP_NULL)
        {
            Ustrcpy(dest, src);
        }else if(resId != 0)
        {
            Ustrcpy(dest, widgetGetPredefinedString(resId));
        }
    }
}

MSF_INT32 widgetUstrlen(MSF_UINT8 *str)
{
    return Ustrlen(str);
}


MsfTextInput*  widgetDialogGetInputByIndex( MsfDialog *pDialog, int index )
{
    MsfWindow           *pWin;
    GadgetsInWindow   *pGadgetNode;
    int                     iNode;
    
    if( !pDialog || pDialog->type != MsfPrompt
        || index < 0 || index >= pDialog->inputCount )
    {
        return OP_NULL;
    }

    pWin = (MsfWindow*)pDialog;
    pGadgetNode = pWin->children;
    iNode = 0;

    while(pGadgetNode && iNode < index )
    {
        iNode++;
        pGadgetNode = pGadgetNode->next;
    }

    if( pGadgetNode )
    {
        return (MsfTextInput*)pGadgetNode->gadget;
    }

    return OP_NULL;
}



/*==================================================================================================
    FUNCTION:   widgetBmpAniGetSize

    DESCRIPTION:
         Get bitmap gadget size
        
    ARGUMENTS PASSED:
         bmpHandle:  bitmap gadget handle
         pSize:      point to gadget size
        
    RETURN VALUE:
          

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int  widgetBmpAniGetSize
( 
    MsfBmpAnimation   *pBmpAnimation,
    MsfSize            *pSize
)
{
    MsfGadget        *pGadget;

    if(( OP_NULL == pSize ) || (OP_NULL == pBmpAnimation))
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    } 
    pGadget = (MsfGadget*)pBmpAnimation;
    if( pGadget->size.width!= 0 || pGadget->size.height != 0 )
    {
        *pSize = pGadget->size;
    }
    else 
    {
       RM_INT_ANIDATA_T  *pTempAniData = pBmpAnimation->pAniData;
       
       if(OP_NULL == pTempAniData)
       {
           *pSize = DefaultPropTab[MSF_BMPANIMATION].size;
       }
       else
       {
          if(ANI_ICON == pTempAniData->image_type)
          {
              RM_ICON_T *pIcon;
              pIcon = (RM_ICON_T *)pTempAniData->scene;
              if((0 ==pIcon->biWidth) && (0 == pIcon->biHeight))
              {
                  *pSize = DefaultPropTab[MSF_BMPANIMATION].size;
              }
              else
              {
                 pSize->height = pIcon->biHeight;
                 pSize->width = pIcon->biWidth;
              }
          }
          else if(ANI_BITMAP == pTempAniData->image_type)
          {
              RM_BITMAP_T *pBmp;
              pBmp = (RM_BITMAP_T *)pTempAniData->scene;
              if((0 == pBmp->biWidth) && (0 == pBmp->biHeight))
              {
                  *pSize = DefaultPropTab[MSF_BMPANIMATION].size;
              }
              else
              {
                 pSize->height = pBmp->biHeight;
                 pSize->width = pBmp->biWidth;
              }
          }  
          else
          {
               *pSize = DefaultPropTab[MSF_IMAGEGADGET].size;
          }
       }
    }
    

    return TPI_WIDGET_OK;
}


/*==================================================================================================
    FUNCTION:   WidgetGadgetGetSize

    DESCRIPTION:
         Get  gadget size
        
    ARGUMENTS PASSED:
         pGaget:    point to gadget structure
         pSize:      point to gadget size
        
    RETURN VALUE:
          

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetGadgetGetSize(MsfGadget *pGadget, MsfSize *pSize)
{
    
    MsfWidgetType gadgetType;
    int iRet = TPI_WIDGET_OK;
    
    if((OP_NULL == pGadget) || (OP_NULL == pSize))
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    gadgetType = getWidgetType(pGadget->gadgetHandle);
    switch(gadgetType)
    {
        case MSF_IMAGEGADGET:
           {
               MsfImageGadget *pImage;
               pImage = (MsfImageGadget *)pGadget;    
               iRet = widgetImageGadgetGetSize(pImage, pImage->image, pSize); 
           }
           break;
        case MSF_BMPANIMATION:
           {
              MsfBmpAnimation *pBmpAni;
              pBmpAni = (MsfBmpAnimation *)pGadget;
              iRet = widgetBmpAniGetSize(pBmpAni, pSize);
           }
           break;
        default:
           *pSize = pGadget->size;
           break;
    }
    return iRet;
}


/*==================================================================================================
    FUNCTION:   widgetGetFontSizeInEngChar

    DESCRIPTION:
         Get the size of one ASCII char in terms of the fontAttr parameters.
        
    ARGUMENTS PASSED:
         fontAttr:          the font attributes
         pFontWidth:      returned the width of a ASCII char 
         pFontHeight      returned the height of a ASCII char 
        
    RETURN VALUE:
          

    IMPORTANT NOTES:
        None
==================================================================================================*/
int widgetGetFontSizeInEngChar
(
    DS_FONTATTR              fontAttr,
    OP_INT16                    *pFontWidth,
    OP_INT16                    *pFontHeight
)
{
    OP_UINT8            iWidth, iHeight;
    DS_FONT_ID         chn_fontid,eng_fontid;
    OP_BOOLEAN        valid_fontid = OP_FALSE;

    if( !pFontWidth || !pFontHeight )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    font_get_font_id(fontAttr,&eng_fontid,&chn_fontid);
    valid_fontid = font_get_font_size(eng_fontid, &iWidth, &iHeight);
    if( valid_fontid == OP_FALSE )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    *pFontWidth = iWidth;
    *pFontHeight = iHeight;

    return TPI_WIDGET_OK;
}    

/*==================================================================================================
    FUNCTION:   widgetAppIsCurrentFocusApp

    DESCRIPTION:
         Judge if the App which is involved with the module specified by the modId parameter is focused APP
        
    ARGUMENTS PASSED:
         modId:    the module identifier
        
    RETURN VALUE:
         if the APP involved with module identifier modId is focused application, return OP_TRUE;
         or return OP_FALSE

    IMPORTANT NOTES:
        None
==================================================================================================*/
OP_BOOLEAN  widgetAppIsCurrentFocusApp(MSF_UINT8 modId)
{
    OP_UINT8 iAppId = 255;

    if( modId ==MSF_MODID_UMB )
    {
        iAppId = APP_UMB;
    }
#ifdef MSF_MODID_XDICT    
    else if( modId == MSF_MODID_XDICT )
    {
        iAppId = APP_XDICT;
    }
#endif
    else if( modId == MSF_MODID_BRA 
          || modId == MSF_MODID_BRS
          || modId == MSF_MODID_UIA
#ifdef MSF_MODID_SMA
          || modId == MSF_MODID_SMA
#endif
#ifdef MSF_MODID_MEA
          || modId == MSF_MODID_MEA
#endif
          || modId == MSF_MODID_RMA)
    {
        iAppId = APP_WAP;
    }
    else
    {
        if( modId == MSF_MODID_MSM )
        {
            iAppId = AM_Get_Focus_App();
            
            if( iAppId == APP_WAP || iAppId == APP_UMB )
            {
                return OP_TRUE;
            }
            else
            {
                return OP_FALSE;
            }
        }
        
        iAppId = modId - 100;   /* for Xdict */
    }
    
    if( iAppId == AM_Get_Focus_App())
    {
        return OP_TRUE;
    }

    return OP_FALSE;
}

OP_BOOLEAN widgetIsFreeze(void)
{
    MsfScreen  *pScreen = seekFocusedScreen();
    if(  !pScreen )
    {
        if( pDisplay->hScreenSave == INVALID_HANDLE )
        {
            return OP_TRUE;
        }
    }
    else if(!widgetAppIsCurrentFocusApp(pScreen->modId) || pScreen->children == OP_NULL )
    {
        return OP_TRUE;
    }

    return OP_FALSE;
}
