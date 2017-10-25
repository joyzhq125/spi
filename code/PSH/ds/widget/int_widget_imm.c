/*==================================================================================================

    MODULE NAME : int_widget_imm.c

    GENERAL DESCRIPTION

    SEF Telecom Confidential Proprietary
    (c) Copyright 2002 by SEF Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    04/03/2003   Zhuxq            Cxxxxxx     Initial file creation.
    7/30/2003    Zhuxq            P001026     Shorten the time of timeout WIDGET_IMM_INPUT_TIMEOUT
    8/11/2003    Chenjs           P001091     Update text input widget.
    8/18/2003    Zhuxq            P001263     Modify  the input method of textinput, allow to create a input method anytime  if a textinput require
    08/23/2003   Zhuxq            P001347     Fix some bugs of the Input Method, and delete the invalid references
    09/01/2003   Zhuxq            P001444     Fix the bug of the Input Method, which shows incorrect 
    09/25/2003   Steven Lai       C001579     Add the feature to handle attachment in mms
    10/17/2003   Zhuxq            C001603     Add touch panel feature to WAP & MMS
    10/23/2003   Zhuxq            P001842     Optimize the paintbox and input method solution and fix some bug
    11/11/2003   Zhuxq            P001883     Optimize the redraw strategy to reduce the redraw times
    12/04/2003   Andrew           P002113     Add switching IME by # and * (Long Press)
    01/05/2004   Zhuxq            CNXT000072  Add callback function to each widget in the widget system
    01/15/2004   Zhuxq            P002352     All editing actions of MsfTextInput turns to MsfEditor
    02/02/2004   Chenxiao         p002404     add textinput scrollbar  in the dolphin project     
    02/17/2004   Zhuxq            P002492     Add Viewer window to widget system
    02/20/2004   Zhuxq            P002517     Adjust the handle order of pen events in widget system
    03/08/2004   Andrew           P002598     Update WAP Editor for Dolphin project
    03/12/2004   Andrew           P002635     Update the IME UI according to NEC request.
    03/27/2004   Zhuxq            P002760     Fix some bugs in widget system
    04/22/2004   Andrew           P002659     Fix some bugs of projects that bases Dolphin
    04/14/2004   Dingjianxin      P002961     Delete the conditional compiler 
    04/30/2004   Andrew           P005156     Update symbol IME.
    05/13/2004   Andrew           P005368     Save IME before focus change.
    05/24/2004   zhuxq            P005568     Adjust UI implementation in UMB and fix some bugs
    06/04/2004   zhuxq            P005925     Correct issues found by PC-lint and fix some bugs
    06/04/2004   Andrew           P005882     Fix some Dolphin bugs
    06/09/2004   Jianghb          P006036     Add WHALE1,WHALE2 and NEMO compilerswitch to some module
    06/09/2004   zhuxq            P006048     forbid Menu to respond successive multiple actions
    06/23/2004   Andrew           P006351     Fix some Lotus bugs
    06/28/2004   MengJianghong    P006526     Add smart eng .
    06/30/2004   chenxiao         p006470     add Pinyin input method for downloading the sound of wap.        
    07/05/2004   Andrew           P006419     Merge B1 and Lotus to same source code
    07/05/2004   zhuxq            P006708     Fix  bugs: CNXT00007777, CNXT00004950, CNXT00009472, CNXT00008770, CNXT00008391, CNXT00009745
    07/10/2004   MengJianghong    P006618     Fix bug can't input symbol in letter ime
    07/20/2004   zhuxq            P007108     Add playing GIF animation in some wap page with multi-frame GIF
    07/27/2004   Andrew           P007283     Merge for Wap Branch
    07/29/2004   MengJianghong    P007310     Add no clear key mode for B1 and fix bug not del candidate when repeat input.
    08/03/2004   Andrew           P007476     Fix some bugs related Dolphin series of projects
    08/05/2004   MengJianghong    P007492     Fix bug cnxt00012855 ,define the default ime number in email .
    08/10/2004   Andrew           P007534     Fix some bugs for Editor branch
    08/12/2004   Andrew           P007707     Fix some bugs for Editor branch
    08/13/2004   zhuxq            P007751     Fix the bug that TextInput in WAP page can't be inputed after pressed OK
    08/12/2004   MengJianghong    P007680     Del symbol ime 
    08/18/2004   zhuxq            P007793     Trace widget memory issues, and reduce allocation times of MsfString
    08/23/2004   Zhuxq            p007888     Fix the bug that the icon of 'abc' input method flashes when inputing in a wap page
    08/24/2004   Gaoyan           p007915     remove imm icon while input box lost focus
    09/01/2004   Andrew           P008042     Fix some Editor bugs.

    08/23/2004   MengJianghong    P007900     Fix bug that can't response if location cursor in hw ime 
    09/02/2004   MengJianghong    P008033     Fix bug that keypad no response when pen down in scroll bar.    
    Self-documenting Code
    Describe/explain low-level design of this module and/or group of funtions and/or specific
    funtion that are hard to understand by reading code and thus requires detail description.
    Free format !
        
====================================================================================================
    INCLUDE FILES
==================================================================================================*/

#include "APP_include.h"
#include "Input_Method_manager.h" 
#include "Msf_int_widget.h"
#include "int_widget_imm.h"

#include "int_widget_common.h"
#include "SP_volume.h"
#include "TP_Operator.h"
#ifdef _TOUCH_PANEL
#include "IME_HANDWRITING.h"
#endif
#include "int_widget_custom.h"
#include "int_widget_show.h"
#include "Int_widget_txt_input.h"
#include "scrollbar.h"


#ifndef _KEYPAD
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

#define  WIDGET_IMM_TRANLATED                              1
#define  WIDGET_IMM_ORIGINAL_PEN_EVENT              2
#define  WIDGET_IMM_DISCARD                                  3
#define  WIDGET_IMM_NO_PEN_EVENT                        4


#define MAX_EDITOR_IME_CNT                                             5      

#define MAX_IME_LOOP_NUM        5     // For # loop (Main)
#define MAX_IME_SUBLOOP_NUM      3    // For * loop (Sub)
#define  WIDGET_IMM_INTERVAL_BETWEEN_LINES      0

/*==================================================================================================
    LOCAL CONSTANTS
==================================================================================================*/
static const RM_SOUND_ID_T  WIDGET_IMM_WARNING_SOUND = RES_SOUND_0007;

static const INPUT_METHOD_ENUM_T widgetImeList[MsfNoTextType][MAX_EDITOR_IME_CNT]=
{  /* MsfText */
    {
        INPUT_METHOD_HANDWRITING,
        INPUT_METHOD_PINYIN,
        INPUT_METHOD_ENGLISH,
        INPUT_METHOD_NUMBER,
        INPUT_METHOD_SYMBOL
    },
    /* MsfName */
    {
        INPUT_METHOD_HANDWRITING,
        INPUT_METHOD_PINYIN,
        INPUT_METHOD_ENGLISH,
        INPUT_METHOD_NUMBER,
        INPUT_METHOD_SYMBOL
    },
   /* MsfIpAddress */
    { 
        INPUT_METHOD_NUMBER , 
        INPUT_METHOD_ENGLISH,
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE
    }, 
   /*  msfUrl */
    {
        INPUT_METHOD_ENGLISH,
        INPUT_METHOD_NUMBER, 
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE
    }, 
   /*MsfEmailAddress*/                            
    {
        INPUT_METHOD_NUMBER , 
        INPUT_METHOD_ENGLISH, 
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE
    },  
    /*MsfPhoneNumber*/
    {
        INPUT_METHOD_PNUMBER_GROUP, 
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE
    },  
    /*  MsfNumeric */ 
    {
        INPUT_METHOD_NUMERIC , 
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE
    },  
    /* MsfNumber */
    {
        INPUT_METHOD_DIGITAL , 
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE
    },  
    /* MsfInteger */
    {
        INPUT_METHOD_DIGITAL , 
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE
    },  
    /*MsfPasswordText*/
    {
        INPUT_METHOD_NUMBER , 
        INPUT_METHOD_ENGLISH, 
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE
    },  
    /*MsfPasswordNumber*/
    {
        INPUT_METHOD_DIGITAL , 
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE
    },  
   /* MsfPredictive */
    {
        INPUT_METHOD_SYMBOL, 
        INPUT_METHOD_ENGLISH,
        INPUT_METHOD_NUMBER, 
        INPUT_METHOD_NONE , 
        INPUT_METHOD_NONE
    }, 
   /* MsfFile */
    {
        INPUT_METHOD_ENGLISH, 
        INPUT_METHOD_NUMBER,
        INPUT_METHOD_SYMBOL, 
        INPUT_METHOD_PINYIN, 
        INPUT_METHOD_NONE
    }, 
    /* MsfFilePath */
    {
        INPUT_METHOD_SYMBOL, 
        INPUT_METHOD_ENGLISH,
        INPUT_METHOD_NUMBER, 
        INPUT_METHOD_NONE, 
        INPUT_METHOD_NONE
    }, 
    /* MsfSmsEms */
    {
        INPUT_METHOD_HANDWRITING,
        INPUT_METHOD_PINYIN,
        INPUT_METHOD_ENGLISH,
        INPUT_METHOD_NUMBER,
        INPUT_METHOD_SYMBOL
    },
};
/*
* Two array to save the sequence of changing IME.
* Andrew P002133
*/
static const INPUT_METHOD_ENUM_T ime_inlist_pound [MAX_IME_LOOP_NUM] =
{
    INPUT_METHOD_HANDWRITING,
    INPUT_METHOD_PINYIN,
    INPUT_METHOD_NUMBER,
}; 

static const INPUT_METHOD_ENUM_T ime_inlist_star [MAX_IME_SUBLOOP_NUM] =
{
0,0,0
};

/*==================================================================================================
    LOCAL VARIABLES
==================================================================================================*/
static OP_BOOLEAN   bWidgetImmCreated = OP_FALSE;

static MsfWidgetImm widgetImm;

static  OP_UINT8   skeyStr[SKEY_MAX_CHAR*2+2];

/*==================================================================================================
    GLOBAL VARIABLES
==================================================================================================*/

/*
 *  OP_TRUE: cursor is showing.
 *  OP_FALSE: cursor is hiding.
 */
static OP_BOOLEAN gb_is_cursor_showing;

/*==================================================================================================
    GLOBAL EXTERNAL   FUNCTIONS
==================================================================================================*/
extern OP_BOOLEAN TBM_InsertString( OP_UINT32 handle, OP_UINT8 *pStr, OP_BOOLEAN bRefresh);
extern OP_BOOLEAN TBM_DeleteCurChar( OP_UINT32 handle, OP_BOOLEAN bRefresh );
extern int  widgetWindowShowSoftKey( const MsfWindow  *pWin, OP_BOOLEAN bRefresh );
extern int widgetInputGetImmInfo
(
    OP_UINT32          hInput, 
    MsfTextType         *pTextType,  
    OP_INT16            **ppCurImeIndex,
    DS_FONTATTR      *pFontAttr,
    OP_UINT32          *phWin,
    OP_INT16            *pCurWinBottom,
    struct tagMsfWidgetImm ***pppImmObj,
    struct tagMsfWidgetImm ***pppImmObjInScreen
);

extern int widgetPaintBoxRedrawForImm( MsfWindow    *pPaintBox, OP_BOOLEAN bRefresh);

extern OP_BOOLEAN widgetFocusGadgetAdjustWindow
(
    MsfGadget  *pGadget,
    MsfWindow   *pWin
);

extern int  widgetDrawTextInputCaret
( 
    const MsfTextInput      *pInput,
    OP_BOOLEAN            bRefresh
);
/*==================================================================================================
    LOCAL FUNCTIONS
==================================================================================================*/
int widgetImmSetWidgetLeftSoftKey
( 
    MsfWidgetImm  *pImm, 
    OP_UINT8* pLeftSoftKey
);

int  widgetImmNextImm(MsfWidgetImm  *pImm);

int widgetImmEventHandler
(
    MsfWidgetImm*                   pImm,
    OPUS_EVENT_ENUM_TYPE   event, 
    void                                      *pMess,
    OP_BOOLEAN                       *handle
);

int widgetImmOpen(MsfWidgetImm  *pImm );
int widgetImmClose(MsfWidgetImm  *pImm );

static OP_BOOLEAN widgetImmInsertString
(
    MsfWidgetImm  *pImm, 
    OP_BOOLEAN    bRefreshOnGadget
);

static int widgetImmAdjustWindow( MsfWidgetImm *pImm, OP_BOOLEAN  bAfterCreated );

int widgetImmRedrawScreen( MsfWidgetImm  *pImm );

static int  widgetImmPreHandle
(
    MsfWidgetImm                  *pImm,
    OPUS_EVENT_ENUM_TYPE   event, 
    void                                *pMess,
    OP_BOOLEAN                    *handle,
    OP_BOOLEAN                    *bTranslated,
    TPO_OPERATION_PARAM_STRUCTURE_T *pOperation
);

static void  widgetImmResultHandle
(
    MsfWidgetImm                  *pImm,
    OPUS_EVENT_ENUM_TYPE   event, 
    void                                *pMess,
    OP_BOOLEAN                    *handle,
    MsfImmHandleResult           imm_ret
);

static int widgetImmSwitch
(
    MsfWidgetImm * pImm, 
    INPUT_METHOD_ENUM_T new_ime
);

static  int widgetImmSetInputMode(MsfWidgetImm *pImm, INPUT_DISP_MODE_ENUM_T disp_mode )
{
    switch(disp_mode)
    {
    case INPUT_DISP_MODE_HANWRITING:
        pImm->start_y = EDITOR_IME_HW_START_Y;
        break;
    case INPUT_DISP_MODE_PINYIN:
        pImm->start_y = EDITOR_IME_PINYIN_START_Y;
        break;
    case INPUT_DISP_MODE_ENGLISH:
        pImm->start_y = EDITOR_IME_ENGLISH_START_Y;
        break;
    case INPUT_DISP_MODE_SYMBOL:
        pImm->start_y = EDITOR_IME_SYMBOL_START_Y;
        break;
    case INPUT_DISP_MODE_NUMBER:
        pImm->start_y = EDITOR_IME_NUMBER_START_Y;
        break;
    case INPUT_DISP_MODE_PBNUMBER:
    case INPUT_DISP_MODE_PHONENUM_GROUP:
        pImm->start_y = EDITOR_IME_PBNUMBER_START_Y;
        break;
    case INPUT_DISP_MODE_NUMERIC:
        pImm->start_y = EDITOR_IME_NUMERIC_START_Y;
        break;
    case INPUT_DISP_MODE_DIGITAL:
        pImm->start_y = EDITOR_IME_DIGITAL_START_Y;
        break;
    case INPUT_DISP_MODE_NONE:
    default:
        pImm->start_y = LCD_EXT_TEXT_HIGH;
        
        break;
    }
    
    pImm->input_disp_mode = disp_mode;
    pImm->start_x = 0;

#if ( (PROJECT_ID == DOLPHIN_PROJECT) || (PROJECT_ID == ODIN_PROJECT) || (PROJECT_ID == WHALE1_PROJECT)\
    ||(PROJECT_ID == WHALE2_PROJECT) || (PROJECT_ID == NEMO_PROJECT) )
#else
    {   /*  set left softkey  */
        RM_RESOURCE_ID_T                skey_left;
        
        if(IMM_has_menu(pImm->hImm) == OP_TRUE)
        {
            IMM_get_ime_skey( pImm->hImm, &skey_left );
        }
        else
        {
            skey_left = PMPT_SKEY_EMPTY;
        }
            
        util_get_text_from_res_w_max_len(skey_left, skeyStr, SKEY_MAX_CHAR*2 );
        widgetImmSetWidgetLeftSoftKey( pImm, skeyStr );
    }
#endif

    return TPI_WIDGET_OK;
}

/*==================================================================================================
    GLOBAL FUNCTIONS
==================================================================================================*/
MsfWidgetImm*  widgetImmCreate
( 
    OP_UINT32                       hInput, 
    OP_INT16                         iImeList,
    OP_INT16                         iImeIndex,
    DS_FONTATTR                   font_attr,
    OP_UINT32                      hWin,
    OP_INT16                        iWinBottom
)
{
    OP_INT16       font_width,font_height;
  
    if( hInput == INVALID_HANDLE || iImeList < 0 || iImeList >= (OP_INT16)MsfNoTextType )
    {
        return OP_NULL;
    }

    /*  already created  */
    if(OP_TRUE == bWidgetImmCreated) 
    {
        /*  exactly the old textinput  */
        if( hInput == widgetImm.hInput )
        {
            return &widgetImm;
        }
        else  /*  destroy the old imm  */
        {
            IMM_destroy( widgetImm.hImm );
        }
    }
    
    op_memset(&widgetImm, 0x0, sizeof(MsfWidgetImm));

    font_attr &= ~ FONT_SIZE_SMALL;
    font_attr |= FONT_SIZE_LARGE;

    if(0 > widgetGetFontSizeInEngChar(font_attr, &font_width, &font_height) )
    {
        return OP_NULL;
    }

    widgetImm.hImm = IMM_create();
    IMM_set_call_ap_type(widgetImm.hImm, IMM_CALL_FROM_WAP);
    
    if( widgetImm.hImm == OP_NULL )
    {
        return OP_NULL;
    }

    widgetImm.hWindow = hWin;
    widgetImm.hInput = hInput;
    
    widgetImm.iWinBottom = iWinBottom;
    widgetImm.iCurImeList = iImeList;
    widgetImm.bOpen = OP_FALSE;
    
    if( iImeIndex < 0 || iImeIndex >= MAX_EDITOR_IME_CNT 
    || widgetImeList[widgetImm.iCurImeList][iImeIndex] == INPUT_METHOD_NONE  )
    {
        iImeIndex = 0;
    }
    
    widgetImm.iCurImeIndex = iImeIndex;
    if( OP_FALSE ==IMM_select_IME( widgetImm.hImm, widgetImeList[widgetImm.iCurImeList][iImeIndex] ))
    {
        IMM_destroy(widgetImm.hImm);
        widgetImm.hImm = 0;
        return OP_NULL;
    }
    
    /*
      * Notify IMM the display cols
      */
    IMM_set_disp_width(widgetImm.hImm, (OP_UINT8)(LCD_MAX_X/font_width/2*2));

    bWidgetImmCreated = OP_TRUE;
    return &widgetImm;
}

int widgetImmDestroy(MsfWidgetImm *pImm, OP_UINT32 hInput )
{
    int iRet = TPI_WIDGET_ERROR_INVALID_PARAM;

   
    if( (pImm== &widgetImm) 
         && ( OP_TRUE == bWidgetImmCreated)
         && ( hInput == widgetImm.hInput ))
    {
        iRet = TPI_WIDGET_OK;
        
        bWidgetImmCreated = OP_FALSE;
        
        skeyStr[0] = skeyStr[1] = 0x00;
        widgetImmSetWidgetLeftSoftKey( pImm, OP_NULL );
        
        IMM_destroy(pImm->hImm);
        pImm->hImm = 0;

        if( pImm->bOpen == OP_TRUE )
        {
            pImm->bOpen = OP_FALSE;
            iRet = widgetImmAdjustWindow( pImm, OP_FALSE);
        }
        op_memset(pImm, 0, sizeof(widgetImm));
    }

    return iRet;
}


/*==================================================================================================
    FUNCTION:  widgetImmEventHandler

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         < 0 : some error
         0   :  event handled
         2   :  event handled, and need to redraw the content

    IMPORTANT NOTES:
        The words in imm_state name have special meanings:
            READY          need insert a char(s)
            PREREADY     the char(s) is ready, but need user to select
            PENDING       the char(s) is ready, but user can not select
            REPEAT        need the IMM_send_event to re-handle one time
            WARNING     user operation error
            
==================================================================================================*/
int widgetImmEventHandler
(
    MsfWidgetImm*                   pImm,
    OPUS_EVENT_ENUM_TYPE   event, 
    void                                      *pMess,
    OP_BOOLEAN                       *handle
)
{
    MsfImmHandleResult             imm_ret;
    IME_STATE_ENUM_T            imm_state = IME_STATE_NONE;
    OP_BOOLEAN                      bRedraw = OP_FALSE;
    int                             iRet;
    OP_BOOLEAN bTextChanged = OP_FALSE;
    MsfTextInput *pTextInput;
    MsfInput* pInput ;
    MsfWidgetType   wt;

/*
    iRet = widgetImmPreHandle( pImm, event, pMess, handle, &bTranslated,  &op);
    if( iRet != WIDGET_EVENT_NOT_HANDLE )
    {
        return iRet;
    }
    */
    
    *handle = OP_TRUE;
    imm_ret = WIDGET_RET_NONE;
    
    pTextInput = (MsfTextInput*)seekWidget(pImm->hInput, &wt);
    if( !pTextInput || wt != MSF_TEXTINPUT )
    {
        return OP_FALSE;
    }

    pInput = &pTextInput->input;
    
    if(!pInput)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if (pImm->input_disp_mode == INPUT_DISP_MODE_HANWRITING
        && event == OPUS_FOCUS_PEN)
    {
        TP_PEN_EVENT_STRUCTURE_T   *pPen;
        pPen = (TP_PEN_EVENT_STRUCTURE_T *)pMess;
        //
        // If the pen down event occurs in the scroll bar area.
        //
        if (pPen->state == TP_PEN_STATE_PENDN
            && pPen->x >= EDITOR_WIDTH
            && pPen->x < LCD_MAX_X
            && pPen->y >= EDITOR_WINDOW_REGION_TOP_FIX
            && pPen->y < EDITOR_IME_HW_START_Y + LCD_ANNUN_HIGH
            )
        {
            return TPI_WIDGET_HW_SCROLLING;
        }
        
    }


    /*
     * Use IMM to process.
     */
    while(1)
    {
        /*
         * Send event to IMM
         */
        imm_state = IMM_send_event( pImm->hImm , event, pMess);

        if( pImm->bOpen == OP_FALSE
            && imm_state != IME_STATE_WARNING 
            && imm_state != IME_STATE_NONE 
            && imm_state < IME_STATE_SKEY_CHANGED)
        {
            iRet = widgetImmOpen( pImm );
        }
        
        /*
         * Process return code
         */
        switch(imm_state)
        {
        case IME_STATE_WARNING:
            imm_ret = WIDGET_RET_WARNING;
            break;
        case IME_STATE_NONE:
            // imm_ret = WIDGET_RET_PROCESSED;
            break;
        case IME_STATE_SKEY_CHANGED:
            {   /*  set left softkey  */
                RM_RESOURCE_ID_T                skey_left;
                
                IMM_get_ime_skey( pImm->hImm, &skey_left );
                util_get_text_from_res_w_max_len(skey_left, skeyStr, SKEY_MAX_CHAR*2 );
                widgetImmSetWidgetLeftSoftKey( pImm, skeyStr );
            }
        /* fallthough to handle the previous inputed char*/
        case IME_STATE_READY_REPEAT:
        case IME_STATE_READY_PASS:
        case IME_STATE_READY:
        case IME_STATE_READY_PENDING:
        case IME_STATE_HW_READY_PASS:
            {
                if( (pImm->input_disp_mode == INPUT_DISP_MODE_HANWRITING )
                    && ( pImm->bPrereadyInserted == OP_TRUE )
                    && ( imm_state != IME_STATE_READY_PASS )
                    && ( imm_state != IME_STATE_READY)
                    )
                {
                    TBM_DeleteCurChar(pImm->hInput, OP_FALSE);
                    pImm->bPrereadyInserted = OP_FALSE;
                }
                widgetImmInsertString(pImm ,OP_TRUE );
            }                

            /* not redraw the symbol area */
            /*
            if( iRet != WIDGET_WINDOW_ALREADY_DRAWN
                && widgetImeList[pImm->iCurImeList][pImm->iCurImeIndex] != INPUT_METHOD_SYMBOL )
            {
                widgetImmShow( pImm);
            }
            */
            
            if( (imm_state == IME_STATE_READY_PASS) || (imm_state == IME_STATE_HW_READY_PASS))
            {
                imm_ret = WIDGET_RET_NONE;
                break;
            }
            else if(imm_state == IME_STATE_READY_REPEAT)
            {
                continue;
            }
            else
            {
                imm_ret = WIDGET_RET_PROCESSED;
                break;
            }
            
        case IME_STATE_PENDING:
            imm_ret =  WIDGET_RET_PROCESSED;
            /*
            if(  iRet != WIDGET_WINDOW_ALREADY_DRAWN )
            {
                widgetImmShow( pImm );
            }
            */
            break;

        case IME_STATE_PREREADY:
        case IME_STATE_SYMBOL_PREREADY:
            if( pImm->input_disp_mode == INPUT_DISP_MODE_HANWRITING 
                && event == OPUS_TIMER_TEXT_INPUT )
            {
                pImm->bPrereadyInserted = widgetImmInsertString(pImm ,OP_FALSE);
            }
            imm_ret = WIDGET_RET_PROCESSED;
            break;
        case IME_STATE_READY_PREREADY:
            imm_ret = WIDGET_RET_PROCESSED;
            
            if( pImm->input_disp_mode == INPUT_DISP_MODE_HANWRITING )
            {
                /*  write another word  */
                if( event == OPUS_TIMER_TEXT_INPUT )
                {
                    pImm->bPrereadyInserted = widgetImmInsertString(pImm, OP_FALSE);
                }
                else
                {
                    if( event == OPUS_FOCUS_KEYPAD )
                    {
                        // TBM_InsertString( pImm->hInput, pImm->candidate_buf, OP_TRUE);
                        if(  iRet != WIDGET_WINDOW_ALREADY_DRAWN )
                        {
                           widgetImmShow( pImm, OP_TRUE);
                        }
                    }
                    else
                    {
                        widgetImmInsertString(pImm, OP_TRUE);
                    }
                    pImm->bPrereadyInserted = OP_FALSE;
                }
            }
            else
            {
                bRedraw = widgetImmInsertString(pImm, OP_TRUE);
                if( (bRedraw == OP_TRUE) && ( iRet != WIDGET_WINDOW_ALREADY_DRAWN ))
                {
                    widgetImmShow( pImm, OP_TRUE );
                }
            }  
            break;
        case IME_STATE_HW_PASS:    
            pImm->bOnHandWritting = OP_TRUE;
            return TPI_WIDGET_OK;
            
        case IME_STATE_DELETE:
            bTextChanged = TBM_DelCursorChar(&(pInput->txtInputBufMng));
            imm_ret = WIDGET_RET_PROCESSED;
            break;
        case IME_STATE_DELETE_ALL:
            bTextChanged = TBM_DelString(&(pInput->txtInputBufMng));
            imm_ret = WIDGET_RET_PROCESSED;
            break;
        case IME_STATE_CHANGE_TO_NUMBER:
            event = INPUT_METHOD_NUMBER;
            imm_ret = WIDGET_RET_IME_SWITCH;
            break;
        case IME_STATE_CHANGE_TO_SYMBOL:
            event = INPUT_METHOD_SYMBOL;
            imm_ret = WIDGET_RET_IME_SWITCH;
            break;
        case IME_STATE_CHANGE_TO_PREVIOUS:
            event = INPUT_METHOD_NONE;
            imm_ret = WIDGET_RET_IME_SWITCH;
            break;
        case IME_STATE_SWITCH_IME:
            {
                INPUT_METHOD_ENUM_T ime;
                IMM_get_imeid_switch(pImm->hImm, &ime);
                if (ime > INPUT_METHOD_NONE
                    && ime <INPUT_METHOD_MAX)
                {
                    event = ime;
                    imm_ret = WIDGET_RET_IME_SWITCH;
                }
                else
                {
                    imm_ret = WIDGET_RET_PROCESSED;
                }
                break;
            }
        case IME_STATE_HW_LOCATING_CUSROR:
            if (gb_is_cursor_showing)
            {
                widgetDrawTextInputCaret(pTextInput, OP_FALSE);
            }
            
            gb_is_cursor_showing = OP_FALSE;
            pImm->bPrereadyInserted = OP_FALSE;
            return TPI_WIDGET_HW_CURSOR_LOCATING;
            break;
        default:
            return TPI_WIDGET_ERROR_UNEXPECTED;
            break;
        }
        break;
    }

    if( bTextChanged)
    {
        widgetRedraw( (void*)pTextInput, MSF_TEXTINPUT, pImm->hInput, OP_TRUE);
    }
    /*
     *  Handle editor actions set by IME.
     */
    {
        OP_UINT32 editor_action;
        INPUT_METHOD_ENUM_T   ime;
        IMM_get_editor_action(pImm->hImm, &editor_action);
        IMM_reset_editor_action(pImm->hImm);
        
        if ((editor_action & EDITOR_ACTION_SWITCH_IME) > 0)
        {
            IMM_get_imeid(pImm->hImm, &ime);
            if (ime == INPUT_METHOD_SYMBOL)
            {
                INPUT_METHOD_ENUM_T   pre_ime;
                if(IMM_get_prev_imeid (pImm->hImm,&pre_ime) == OP_TRUE)
                {
                    if (pre_ime > INPUT_METHOD_NONE && pre_ime < INPUT_METHOD_MAX)
                    {
                        event = pre_ime;
                        imm_ret = WIDGET_RET_IME_SWITCH;
                    }
                }
            }
        }
        if ((editor_action & EDITOR_ACTION_STOP_DELETING_TIMER) > 0)
        {
            pImm->bTimerStop = OP_TRUE;
        }
    }

    if (IME_STATE_NONE == imm_state)
    {
        if( event == OPUS_TIMER_TP_KEYPRESS_REPEAT)
        {
            pImm->bTimerStop = OP_FALSE;
            OPUS_Start_Timer(OPUS_TIMER_DELETE_CONTINUOUSLY, 
                EDITOR_TIMER_DEL_CONTINUOUS_TIME, 
                0, 
                ONE_SHOT);
                
            return TPI_WIDGET_OK;
        }
        
        if( event == OPUS_TIMER_DELETE_CONTINUOUSLY && (!pImm->bTimerStop))
        {
            bTextChanged = TBM_DelCursorChar(&(pInput->txtInputBufMng));
            if (bTextChanged)
            {
                widgetRedraw( (void*)pTextInput, MSF_TEXTINPUT, pImm->hInput, OP_TRUE);
                OPUS_Start_Timer(OPUS_TIMER_DELETE_CONTINUOUSLY, 
                    EDITOR_TIMER_DEL_CONTINUOUS_TIME, 
                    0, 
                    ONE_SHOT);
            }
            return TPI_WIDGET_OK;
        }
    }

    widgetImmResultHandle( pImm, event, pMess, handle, imm_ret);
    
    return TPI_WIDGET_OK;
}

int widgetImmShow( MsfWidgetImm* pImm, OP_BOOLEAN  bRefresh )
{
    if (!pImm)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    if( pImm  != &widgetImm || pImm->hImm == 0 )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    IMM_draw_win_frame(pImm->hImm);

    if (bRefresh)
    {
        MsfWidgetType wt;
        MsfWindow  *pWin;
        
        pWin = (MsfWindow*)seekWidget(pImm->hWindow, &wt);
        if( pWin && IS_WINDOW(wt) && pWin->bDrawing == OP_FALSE)
        {
            WAP_REFRESH
        }
    }

    return TPI_WIDGET_OK;
}


int  widgetImmDrawElements
(
    MsfWidgetImm* pImm,
    OP_UINT8*        pElement,
    OP_UINT8*        pCandidate,
    OP_BOOLEAN        bCandidateSpecial
)
{
    int          iRet = TPI_WIDGET_ERROR_UNEXPECTED;    
    
    if(!pImm )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }


    return iRet;
}

int widgetImmSwitch(MsfWidgetImm * pImm, INPUT_METHOD_ENUM_T new_ime)
{
    INPUT_DISP_MODE_ENUM_T    disp_mode;
    // INPUT_METHOD_ENUM_T         current_ime;
/*
    if (new_ime == INPUT_METHOD_SYMBOL || new_ime == INPUT_METHOD_NUMBER)
    {
        IMM_get_imeid(pImm->hImm, &current_ime);
        if (current_ime != INPUT_METHOD_SYMBOL && current_ime != INPUT_METHOD_NUMBER)
        {
            pImm->pre_ime = current_ime;
        }
    }

    if (new_ime == INPUT_METHOD_NONE)
    {
        new_ime = pImm->pre_ime;
    }
    */
    widgetImmClose( pImm );

    IMM_select_IME(pImm->hImm, new_ime);
    IMM_get_disp_mode(pImm->hImm, &disp_mode);
    widgetImmSetInputMode(pImm, disp_mode);
    
    widgetImmOpen( pImm );
    widgetImmShow( pImm, OP_FALSE );

    return OP_TRUE;
}

int  widgetImmNextImm(MsfWidgetImm  *pImm)
{
    OP_INT16    iNewImm;
    INPUT_DISP_MODE_ENUM_T    disp_mode;
    INPUT_METHOD_ENUM_T       eIme = INPUT_METHOD_NONE;         // for switch ime.
    OP_UINT8                index = 0xFF;   // the index of ime in the switch array
    int i;      // for loop

    if( !pImm )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    // Change by *
    if (pImm->eChangeFromStarPound == MSFIMM_CHANGE_IME_SUB)
    {
        // Get current IME.
            IMM_get_imeid(pImm->hImm, &eIme);
        
        // get the index in the sub switching array ( * Long Press ).
        for (i = 0; i < MAX_IME_SUBLOOP_NUM; i++)
        {
            if (eIme == ime_inlist_star[i])
            {
                index = i;
                break;
            }
        }
        if (0xFF == index)
        {
            return OP_FALSE;
        }

        // Get next IME.
        index = (index + 1) % MAX_IME_SUBLOOP_NUM;
        eIme = ime_inlist_star[index];
    }
    
    // Change by # and left soft key
    else if (pImm->eChangeFromStarPound == MSFIMM_CHANGE_IME_MAIN)
    {
        // Switch IME by left soft key.
        /* calc the next ime index */
        iNewImm = (pImm->iCurImeIndex + 1) % MAX_EDITOR_IME_CNT;
        if( widgetImeList[pImm->iCurImeList][iNewImm] == INPUT_METHOD_NONE )
        {
            iNewImm = 0;
        }

        if(pImm->iCurImeIndex == iNewImm)  /* only one ime */
        {
            return (TPI_WIDGET_OK);
        }
        else 
        {
            pImm->iCurImeIndex = iNewImm;
            eIme = widgetImeList[pImm->iCurImeList][pImm->iCurImeIndex];
        }
    }
    
    widgetImmClose( pImm );

    IMM_select_IME(pImm->hImm, eIme);
    IMM_get_disp_mode(pImm->hImm, &disp_mode);
    widgetImmSetInputMode(pImm, disp_mode);
    
//    if( (widgetImeList[pImm->iCurImeList][pImm->iCurImeIndex] == INPUT_METHOD_SYMBOL )
//        ||(widgetImeList[pImm->iCurImeList][pImm->iCurImeIndex] == INPUT_METHOD_HANDWRITING ))
    {
        widgetImmOpen( pImm );
        widgetImmShow( pImm, OP_TRUE );
    }
    
    return (TPI_WIDGET_OK);    
}

int widgetImmSetWidgetLeftSoftKey( MsfWidgetImm  *pImm, OP_UINT8* pLeftSoftKey)
{
    MsfGadget        *pGadget;
    MsfWidgetType  wt;
    
    if( !pImm )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pGadget = (MsfGadget*)seekWidget( pImm->hInput, &wt);
    if( !pGadget || wt != MSF_TEXTINPUT )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    pGadget->softKeyList[0] = pLeftSoftKey;

    if( pGadget->parent != OP_NULL )
    {
        widgetWindowShowSoftKey( pGadget->parent, OP_TRUE );
    }

    return TPI_WIDGET_OK;
}


int  widgetInputFocusChange
( 
    OP_UINT32       hInput, 
    OP_BOOLEAN    bInfocus
)
{
    static OP_BOOLEAN   bReEnter = OP_FALSE;
    MsfTextType         txtType;
    OP_INT16            *pCurImeIndex;
    DS_FONTATTR      fontattr;
    OP_UINT32          hWin;
    OP_INT16            iCurWinBottom;
    MsfWidgetImm      **ppImmObj;
    MsfWidgetImm      **ppScreenImmObj;
    int                      iRet;
    MsfWidgetType     wt;
    
    wt = getWidgetType(hInput);
    if( wt != MSF_TEXTINPUT )
    {
        return TPI_WIDGET_OK;
    }
    
    /*  avoid reenter the function  */
    if( bReEnter == OP_TRUE )
    {
        return TPI_WIDGET_OK;
    }

    bReEnter = OP_TRUE;
    
    iRet = widgetInputGetImmInfo( 
                            hInput, 
                            &txtType, 
                            &pCurImeIndex, 
                            &fontattr, 
                            &hWin,
                            &iCurWinBottom, 
                            &ppImmObj,
                            &ppScreenImmObj);

    if( iRet < 0 )
    {
        bReEnter = OP_FALSE;
        return iRet;
    }
    
    if( bInfocus == OP_TRUE )
    {
        MsfWidgetImm *pImm;
        INPUT_DISP_MODE_ENUM_T  disp_mode;
        OP_INT16            iImeList;

        iImeList =  ((txtType >= MsfNoTextType )? 0 : (OP_INT16) txtType);
        
        pImm = widgetImmCreate( hInput, iImeList, *pCurImeIndex, fontattr, hWin, iCurWinBottom);
        if(  pImm == OP_NULL )
        {
            bReEnter = OP_FALSE;
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }

        *ppImmObj  = pImm;
        *ppScreenImmObj = pImm;
        
        /*
          * Notify EDM current how many line the IME will be occupied
          */
        IMM_get_disp_mode(pImm->hImm, &disp_mode);
        iRet = widgetImmSetInputMode( pImm, disp_mode);
        if( iRet < 0 )
        {
            bReEnter = OP_FALSE;
            return iRet;
        }

        // IMM_draw_win_frame (pImm->hImm);
        
        //if( widgetImeList[pImm->iCurImeList][*pCurImeIndex] == INPUT_METHOD_SYMBOL 
        //    || pImm->input_disp_mode == INPUT_DISP_MODE_HANWRITING )   
        {
            pImm->bOpen = OP_TRUE;
            iRet = widgetImmAdjustWindow( pImm, OP_TRUE );
        }
        
    }
    else
    {
        MsfWidgetImm   *pImm;
        MsfWindow         *pWin;
        INPUT_METHOD_ENUM_T ime_id, ime_in_list;
        int i;

        if( *ppImmObj == OP_NULL )
        {
            bReEnter = OP_FALSE;
            return TPI_WIDGET_OK;
        }        
        
        pImm = *ppImmObj;        
        *ppImmObj = OP_NULL;
        if( !ppScreenImmObj )
        {
            pWin = seekWidget(pImm->hWindow, &wt);
            if( pWin && pWin->isFocused )
            {
                pWin->parent->pImm = OP_NULL;
            }
        }
        else
        {
            *ppScreenImmObj = OP_NULL;
        }
        
        /* save the imm index  */
        // *pCurImeIndex = pImm->iCurImeIndex;
        IMM_get_imeid(pImm->hImm, &ime_id);
        *pCurImeIndex = 0;
        if (ime_id >= INPUT_METHOD_HANDWRITING
            && ime_id <= INPUT_METHOD_SYMBOL)
        {
            for (i = 0; i < MAX_EDITOR_IME_CNT; i++)
            {
                ime_in_list = widgetImeList[pImm->iCurImeList][i];
                if (ime_in_list == INPUT_METHOD_NONE)
                {
                    break;
                }
                if (ime_in_list == ime_id)
                {
                    *pCurImeIndex = i;
                    break;
                }
            }
        }
        
        iRet = widgetImmDestroy( pImm, hInput );
    }

    bReEnter = OP_FALSE;
    return iRet;
}

int widgetImmOpen(MsfWidgetImm  *pImm )
{
    int   iRet;
    
    if( !pImm )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    iRet = TPI_WIDGET_OK;
    
    if((pImm->input_disp_mode != INPUT_DISP_MODE_NONE )
        && ( pImm->input_disp_mode  < INPUT_DISP_MODE_MAX ))
    {
        if (pImm->bOpen == OP_FALSE )
        {
            pImm->bOpen = OP_TRUE;
            iRet = widgetImmAdjustWindow( pImm, OP_FALSE );
            if( iRet < 0 )
            {
                return iRet;
            }
        }
    }
    
    return iRet;
}

int widgetImmClose(MsfWidgetImm  *pImm )
{
    int   iRet;
    
    if( !pImm )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    iRet = TPI_WIDGET_OK;
    
    if( pImm->bOpen == OP_TRUE )
    {
        pImm->bOpen = OP_FALSE;
        iRet = widgetImmAdjustWindow( pImm, OP_FALSE );
    }

    return iRet;
}    


/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
    
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
                   -1       :  some error
                     0       :  not redraw whole window ( normal )
                     2       :  already draw whole window

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int widgetImmAdjustWindow( MsfWidgetImm  *pImm, OP_BOOLEAN  bAfterCreated)
{
    MsfWindow          *pWin;
    MsfGadget          *pGadget;
    MsfWidgetType    wtWin, wtGadget;
    OP_INT16           iImmStart, iWinBottom;    
    OP_INT16           iDelta;
    int                     iRet;

    if( !pImm )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pWin = (MsfWindow*)seekWidget(pImm->hWindow, &wtWin);
    if( !pWin || !IS_WINDOW(wtWin))
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    iImmStart = pImm->start_y -1;
    iWinBottom = pImm->iWinBottom;
    iRet = TPI_WIDGET_OK;
    
    if( wtWin == MSF_PAINTBOX )
    {
        /* If the imm is closed,  redraw the paintbox  */
        if( pImm->bOpen == OP_FALSE  && iImmStart < iWinBottom  )
        {
            widgetPaintBoxRedrawForImm(pWin, OP_TRUE );
            iRet = WIDGET_WINDOW_ALREADY_DRAWN;
        }
    }
    else if( wtWin == MSF_EDITOR )
    {
        OP_INT16 iLabLines;

        /* not consider that the editor is not full screen */
        iDelta = 0;
        if(  iImmStart < iWinBottom )
        {
            if( pImm->bOpen == OP_TRUE)
            {
                iDelta = iImmStart - iWinBottom;
            }
            else 
            {
                iDelta = iWinBottom - iImmStart;
            }
        }
        
        if( iDelta != 0 )  /*  change window size  */
        {
            pGadget = (MsfGadget*)((MsfEditor*)pWin)->input;

            pGadget->size.height += iDelta;

            if( ISVALIDHANDLE(pGadget->title)  )
            {
                iLabLines = 1;
            }
            else
            {
                iLabLines = 0; 
            }
            
            TBM_AdjustCursorForScroll( pGadget->gadgetHandle, &pGadget->size, iLabLines, iDelta);
            if( bAfterCreated )
            {
                widgetRedraw((void*)pWin, wtWin, INVALID_HANDLE, OP_TRUE);
                iRet = WIDGET_WINDOW_ALREADY_DRAWN;
            }
            else
            {
                iRet = widgetRedraw( (void*)pGadget, MSF_TEXTINPUT, INVALID_HANDLE, OP_TRUE);
            }
        }
    }
    else
    {
        if( pImm->bOpen == OP_TRUE)
        {
            pGadget = (MsfGadget*)seekWidget( pImm->hInput, &wtGadget);
            if( !pGadget || ( wtGadget != MSF_TEXTINPUT )) 
            {
                return TPI_WIDGET_ERROR_UNEXPECTED;
            }
            
            if( widgetFocusGadgetAdjustWindow(pGadget, pWin) == OP_TRUE)
            {
                widgetRedraw((void*)pWin, wtWin, INVALID_HANDLE, OP_TRUE);
                iRet = WIDGET_WINDOW_ALREADY_DRAWN;
            }
        }
        else   /*  enlarge window */
        {
            if( iImmStart < iWinBottom )
            {
                iDelta = iImmStart - iWinBottom;
            }
            
            pWin->scrollPos.y += iDelta;
            if( pWin->scrollPos.y < 0 )
            {
                pWin->scrollPos.y = 0;
            }
            
            /* need whole window redrawing  */
            iRet = widgetRedraw( (void*)pWin, wtWin, pWin->windowHandle, OP_TRUE);
            if( iRet >= 0 )
            {
                iRet = WIDGET_WINDOW_ALREADY_DRAWN;
            }
        }        
    }
    
    return iRet;
}

static int  widgetImmPreHandle
(
    MsfWidgetImm                  *pImm,
    OPUS_EVENT_ENUM_TYPE   event, 
    void                                *pMess,
    OP_BOOLEAN                    *handle,
    OP_BOOLEAN                    *bTranslated,
    TPO_OPERATION_PARAM_STRUCTURE_T *pOperation
)
{
    KEYEVENT_STRUCT  *pKey;
    
    if( (pImm!= &widgetImm) || handle == OP_NULL || bTranslated == OP_NULL || !pOperation )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    *bTranslated = OP_FALSE;

    if( event == OPUS_FOCUS_KEYPAD ) 
    {
        if( pImm->bOnHandWritting == OP_TRUE )
        {
            *handle = OP_TRUE;
            return WIDGET_EVENT_HANDLED;
        }
        
        pKey = (KEYEVENT_STRUCT*)pMess;
        
        /*
         *  Add KEY_STAR event is handled by IMM itself.
         *  Do not pass by this function.
         *  Andrew P002113
         */
        if( (pKey->state == UHKEY_RELEASE) &&(pKey->code != KEY_POUND ) && (pKey->code != KEY_STAR) )
        {
            if( pKey->code != pImm->pressKeyCode )
            {
                *handle = OP_FALSE;
            }
            else
            {
                *handle = OP_TRUE;
            }
            
            /*  don't  left  the key-release event to window handle  */
            return WIDGET_EVENT_HANDLED;
        } 
    }

    return WIDGET_EVENT_NOT_HANDLE;
}

static void  widgetImmResultHandle
(
    MsfWidgetImm                  *pImm,
    OPUS_EVENT_ENUM_TYPE   event, 
    void                                *pMess,
    OP_BOOLEAN                    *handle,
    MsfImmHandleResult           imm_ret
)
{
    KEYEVENT_STRUCT  *pKey;
    OP_UINT8               volume;
    static OP_BOOLEAN           bRecievedLongPress = OP_FALSE;
    static OP_BOOLEAN           bStarHandled = OP_FALSE;

    if( event == OPUS_TIMER_TEXT_INPUT
       && pImm->input_disp_mode == INPUT_DISP_MODE_HANWRITING )
    {
        pImm->bOnHandWritting = OP_FALSE;
    }

    if( event == OPUS_FOCUS_KEYPAD)
    {
        pKey = (KEYEVENT_STRUCT*)pMess;
        if (pKey->code == KEY_STAR && UHKEY_PRESS == pKey->state && WIDGET_RET_NONE != imm_ret)
        {
            bStarHandled = OP_TRUE;
        }
        else if (pKey->code != KEY_STAR)
        {
            bStarHandled = OP_FALSE;
        }
    }
    
    if( WIDGET_RET_NONE == imm_ret )
    {
        if( event == OPUS_FOCUS_KEYPAD) 
        {
            pKey = (KEYEVENT_STRUCT*)pMess;
            
            if(  // pKey->code == KEY_SOFT_LEFT || 
                ( pKey->code == KEY_POUND               // Add press # to switch IME.
                && UHKEY_RELEASE == pKey->state         // Handle when key release
                && OP_FALSE == bRecievedLongPress) )    // Handle when it is not long press
                {
                    pImm->eChangeFromStarPound = MSFIMM_CHANGE_IME_MAIN;
                    imm_ret = WIDGET_RET_IME_SELECT;
                }
            else if (( pKey->code == KEY_STAR )
                && (UHKEY_RELEASE == pKey->state)       // Handle when key release
                && (OP_FALSE == bRecievedLongPress))     // Handle when it is not long press
            {
                if (OP_FALSE == bStarHandled )      // Handle it if it is not handled by smart English.
                {
                    TBM_InsertString(pImm->hInput, (OP_UINT8*)"\x20\x0\x0\x0", OP_TRUE );
                    imm_ret = WIDGET_RET_PROCESSED;
                }
                else
                {
                    bStarHandled = OP_FALSE;
                }
            }
            
            // Reset Long Press event
            if ( UHKEY_RELEASE == pKey->state )
            {
                bRecievedLongPress = OP_FALSE;
            }
        }

        /*
         *  Add press * to switch IME.
         */
        pKey = (KEYEVENT_STRUCT*)pMess;
        if (event == OPUS_TIMER_KEYPRESS_REPEAT)
        {
            if ( ( pKey->code == KEY_STAR )
                && ( (pImm->iCurImeList == MsfText) || (pImm->iCurImeList == MsfName) ) )
            {
                INPUT_METHOD_ENUM_T eIme = widgetImeList[pImm->iCurImeList][pImm->iCurImeIndex];
               
                if (INPUT_METHOD_HANDWRITING == eIme)
                {
                    if (OP_FALSE == bRecievedLongPress)     // Only handling once
                    {
                        // Switch letter between UPPER and lower.
                        pImm->eChangeFromStarPound = MSFIMM_CHANGE_IME_LETTER;
                        imm_ret = WIDGET_RET_IME_SELECT;
                        bRecievedLongPress = OP_TRUE;       // Set long press flag
                    }
                }
            }

            else if ( pKey->code == KEY_POUND )
            {
                /*be handled in multi line text input only. */
                if (OP_FALSE == bRecievedLongPress)     // Only handling once
                {
                    int iRet;
                    MsfInput *pTextInput;
                    
                    iRet = getInput(pImm->hInput, &pTextInput,OP_NULL,OP_NULL);
                    if( iRet >= 0 )
                    {
                        if( pTextInput->txtInputBufMng.rows > 1)
                        {
                            TBM_InsertString(pImm->hInput, (OP_UINT8*)"\xa\x0\x0\x0", OP_TRUE );
                        }
                    }
                    bRecievedLongPress = OP_TRUE;       // Set long press flag
                }
            }
        }
    }

    switch(imm_ret)
    {
    case WIDGET_RET_IME_SELECT:
        TPIa_widgetUpdateBegin(pImm->hWindow);
        widgetImmNextImm(pImm);
        TPIa_widgetUpdateEnd(pImm->hWindow);
        break;
    case WIDGET_RET_IME_SWITCH:
        TPIa_widgetUpdateBegin(pImm->hWindow);
        widgetImmSwitch(pImm, event);
        TPIa_widgetUpdateEnd(pImm->hWindow);
        break;
    case WIDGET_RET_WARNING:
    case WIDGET_RET_TOO_LONG:
    case WIDGET_RET_TOO_FEW:
    case WIDGET_RET_OUTOF_RANGE:
        {
            volume = SP_GetVolume( BELL_LEVEL_ID ).LEVEL;
            SP_Audio_play_request(WIDGET_IMM_WARNING_SOUND, volume, OP_FALSE);
            break;
        }
    case WIDGET_RET_PROCESSED:
    case WIDGET_RET_FINISHED:
        break;
    case WIDGET_RET_BACK:
    case WIDGET_RET_QUIT:
    case WIDGET_RET_MAX:
    case WIDGET_RET_NONE:
    default:
        *handle = OP_FALSE;
        break;
    }

    if( event == OPUS_FOCUS_KEYPAD )
    {
        pKey = (KEYEVENT_STRUCT*)pMess;
        if( *handle == OP_TRUE )
        {
            /*  save the pressed key code into pressKeyCode,  
            * and not let the key release event  pass on  
            */
            pImm->pressKeyCode = pKey->code;
        }
        else
        {
            pImm->pressKeyCode = 0;
        }
    }

}

int widgetImmRedrawScreen( MsfWidgetImm  *pImm )
{
    OP_BOOLEAN  bDrawScreenDirect;
    MsfWindow       *pWin;
    MsfWidgetType wt;
    MsfGadget *pTextInput=OP_NULL; 
    if( !pImm )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pWin = (MsfWindow*)seekWidget( pImm->hWindow, &wt);
    if( !pWin || !IS_WINDOW(wt))
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    bDrawScreenDirect = OP_TRUE;
    if( wt  == MSF_PAINTBOX)
    {
        if( restoreDisplay( OP_FALSE, OP_NULL ) )
        {
            bDrawScreenDirect = OP_FALSE;
            
            pTextInput = (MsfGadget*)seekWidget(pImm->hInput, &wt);
            widgetRedraw( (void*)pTextInput, MSF_TEXTINPUT, INVALID_HANDLE, OP_FALSE);

            if( pTextInput->isFocused == OP_TRUE && pImm->bOpen == OP_TRUE)
            {
                widgetImmShow( pImm, OP_TRUE );
            }
            
            WAP_REFRESH
        }
    }

    if( bDrawScreenDirect == OP_TRUE )
    {
        widgetRedraw( pWin->parent, MSF_SCREEN, INVALID_HANDLE, OP_TRUE);
    }

    return TPI_WIDGET_OK;
}


static OP_BOOLEAN widgetImmInsertString
(
 MsfWidgetImm  *pImm,
 OP_BOOLEAN    bRefreshOnGadget
 )
{
    OP_UINT8         *pResult;
    OP_BOOLEAN     bInserted;
    
    bInserted = OP_FALSE;
    
    IMM_get_result(pImm->hImm , &pResult);
    if( *pResult != 0x00 || *(pResult+1) != 0x00 )
    {
        bInserted = TBM_InsertString(pImm->hInput, pResult, bRefreshOnGadget );
    } 
    
    if( bRefreshOnGadget == OP_FALSE )
    {
        widgetImmRedrawScreen( pImm);
    }
    
    return bInserted;
}

void widgetImmSetCursorState(OP_BOOLEAN b_show)
{
    gb_is_cursor_showing = b_show;
}

OP_BOOLEAN widgetImmGetCursorState(void)
{
    return gb_is_cursor_showing;
}

void widgetImmChangeCursorState(void)
{
    gb_is_cursor_showing = gb_is_cursor_showing ? OP_FALSE : OP_TRUE;
}


#else

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
#define  WIDGET_IMM_CANDIDATE_COUNT                  EDITOR_CANDIDATE_COUNT


#define  WIDGET_IMM_TRANLATED                              1
#define  WIDGET_IMM_ORIGINAL_PEN_EVENT              2
#define  WIDGET_IMM_DISCARD                                  3
#define  WIDGET_IMM_NO_PEN_EVENT                        4

#ifndef _KEYPAD
        #define MAX_EDITOR_IME_CNT      5
#else

#ifndef _EDITOR_NO_SYMBOL_IME

    #ifdef _TOUCH_PANEL
            #define MAX_EDITOR_IME_CNT      8
    #else
            #define MAX_EDITOR_IME_CNT      7
    #endif

#else

    #ifdef _TOUCH_PANEL
            #define MAX_EDITOR_IME_CNT      7
    #else
            #define MAX_EDITOR_IME_CNT      6
    #endif

#endif

#endif
#define  WIDGET_IMM_INTERVAL_BETWEEN_LINES      0

/*==================================================================================================
    LOCAL CONSTANTS
==================================================================================================*/
static const RM_SOUND_ID_T  WIDGET_IMM_WARNING_SOUND = RES_SOUND_0007;

static const INPUT_METHOD_ENUM_T widgetImeList[MsfNoTextType][MAX_EDITOR_IME_CNT]=
{
    /* MsfText */
    {
#ifdef _TOUCH_PANEL
    INPUT_METHOD_HANDWRITING,
#endif
     INPUT_METHOD_PINYIN , 
     INPUT_METHOD_STROKE , 
     INPUT_METHOD_SMART_ENG , 
#ifndef _EDITOR_NO_SYMBOL_IME     
     INPUT_METHOD_SYMBOL ,
#endif     
     INPUT_METHOD_LETTER_LOWER,
     INPUT_METHOD_LETTER_UPPER , 
        INPUT_METHOD_NUMBER
    },
   /* MsfName */
    {
#ifdef _TOUCH_PANEL
    INPUT_METHOD_HANDWRITING,
#endif
     INPUT_METHOD_PINYIN , 
     INPUT_METHOD_STROKE , 
     INPUT_METHOD_SMART_ENG , 
#ifndef _EDITOR_NO_SYMBOL_IME     
     INPUT_METHOD_SYMBOL ,
#endif     
     INPUT_METHOD_LETTER_LOWER,
     INPUT_METHOD_LETTER_UPPER , 
        INPUT_METHOD_NUMBER
    },
   /* MsfIpAddress */
   { 
     INPUT_METHOD_NUMBER , 
     INPUT_METHOD_LETTER_LOWER,
     INPUT_METHOD_LETTER_UPPER , 
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE, 
#ifndef _EDITOR_NO_SYMBOL_IME     
     INPUT_METHOD_NONE,
#endif     
#ifdef _TOUCH_PANEL
    INPUT_METHOD_NONE,
#endif
    },
   /*  msfUrl */
   {
     INPUT_METHOD_LETTER_LOWER,
     INPUT_METHOD_LETTER_UPPER , 
     INPUT_METHOD_NUMBER ,     
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE,
#ifndef _EDITOR_NO_SYMBOL_IME     
     INPUT_METHOD_NONE,
#endif     
#ifdef _TOUCH_PANEL
    INPUT_METHOD_NONE,
#endif
    },
   /*MsfEmailAddress*/                            
   {      
     INPUT_METHOD_NUMBER ,     
     INPUT_METHOD_LETTER_LOWER,
     INPUT_METHOD_LETTER_UPPER , 
     INPUT_METHOD_NONE,
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE,
#ifndef _EDITOR_NO_SYMBOL_IME     
     INPUT_METHOD_NONE,
#endif     
#ifdef _TOUCH_PANEL
    INPUT_METHOD_NONE,
#endif
    },
   /*MsfPhoneNumber*/                            
   {
     INPUT_METHOD_PNUMBER_GROUP, 
     INPUT_METHOD_NONE,
     INPUT_METHOD_NONE , 
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE, 
#ifndef _EDITOR_NO_SYMBOL_IME     
     INPUT_METHOD_NONE,
#endif     
#ifdef _TOUCH_PANEL
    INPUT_METHOD_NONE,
#endif
    },
   /*  MsfNumeric */ 
   {
     INPUT_METHOD_NUMBER , 
#ifndef _EDITOR_NO_SYMBOL_IME        
     INPUT_METHOD_SYMBOL,
#endif     
     INPUT_METHOD_NONE , 
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE,
#ifdef _TOUCH_PANEL
    INPUT_METHOD_NONE,
#endif
    },
   /* MsfNumber */
   {
     INPUT_METHOD_NUMBER , 
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE, 
#ifndef _EDITOR_NO_SYMBOL_IME     
     INPUT_METHOD_NONE,
#endif     
#ifdef _TOUCH_PANEL
    INPUT_METHOD_NONE,
#endif
    },
   /* MsfInteger */
   {
     INPUT_METHOD_NUMBER , 
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE, 
#ifndef _EDITOR_NO_SYMBOL_IME     
     INPUT_METHOD_NONE,
#endif     
#ifdef _TOUCH_PANEL
    INPUT_METHOD_NONE,
#endif
    },
   /*MsfPasswordText*/
   {
     INPUT_METHOD_NUMBER,
     INPUT_METHOD_LETTER_LOWER,
     INPUT_METHOD_LETTER_UPPER , 
#ifndef _EDITOR_NO_SYMBOL_IME     
     INPUT_METHOD_SYMBOL, 
#endif     
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE,
#ifdef _TOUCH_PANEL
    INPUT_METHOD_NONE,
#endif
    },
   /*MsfPasswordNumber*/
   {
     INPUT_METHOD_NUMBER,
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE,
     INPUT_METHOD_NONE, 
#ifndef _EDITOR_NO_SYMBOL_IME     
     INPUT_METHOD_NONE,
#endif     
#ifdef _TOUCH_PANEL
    INPUT_METHOD_NONE,
#endif
    },
   /* MsfPredictive */
   {
#ifndef _EDITOR_NO_SYMBOL_IME   
     INPUT_METHOD_SYMBOL, 
#endif     
     INPUT_METHOD_SMART_ENG,
     INPUT_METHOD_LETTER_LOWER,
     INPUT_METHOD_LETTER_UPPER , 
     INPUT_METHOD_NUMBER, 
     INPUT_METHOD_NONE,
     INPUT_METHOD_NONE,
#ifdef _TOUCH_PANEL
    INPUT_METHOD_NONE,
#endif
    },
   /* MsfFile */
    {
#ifdef _TOUCH_PANEL
    INPUT_METHOD_HANDWRITING,
#endif
     INPUT_METHOD_PINYIN,
     INPUT_METHOD_SMART_ENG ,
     INPUT_METHOD_LETTER_LOWER,
     INPUT_METHOD_LETTER_UPPER , 
     INPUT_METHOD_NUMBER,
#ifndef _EDITOR_NO_SYMBOL_IME     
     INPUT_METHOD_SYMBOL,       
#endif     
     INPUT_METHOD_STROKE, 
    }, 
   /* MsfFilePath */
   {
     INPUT_METHOD_SMART_ENG,
     INPUT_METHOD_LETTER_LOWER,
     INPUT_METHOD_LETTER_UPPER , 
#ifndef _EDITOR_NO_SYMBOL_IME     
     INPUT_METHOD_SYMBOL,
#endif     
     INPUT_METHOD_NUMBER,
     INPUT_METHOD_NONE, 
     INPUT_METHOD_NONE,
#ifdef _TOUCH_PANEL
    INPUT_METHOD_NONE,
#endif
    },
    /* MsfSmsEms */
    {
#ifdef _TOUCH_PANEL
    INPUT_METHOD_HANDWRITING,
#endif
     INPUT_METHOD_PINYIN , 
     INPUT_METHOD_STROKE , 
     INPUT_METHOD_SMART_ENG , 
#ifndef _EDITOR_NO_SYMBOL_IME     
     INPUT_METHOD_SYMBOL ,
#endif     
     INPUT_METHOD_LETTER_LOWER,
     INPUT_METHOD_LETTER_UPPER , 
     INPUT_METHOD_NUMBER
    },
};


/*==================================================================================================
    LOCAL VARIABLES
==================================================================================================*/
static OP_BOOLEAN   bWidgetImmCreated = OP_FALSE;

static MsfWidgetImm widgetImm;

static  OP_UINT8   skeyStr[SKEY_MAX_CHAR*2+2];
#ifdef _NO_CLEAR_KEY
static  OP_UINT8   SoftRightKeyStr[SKEY_MAX_CHAR*2+2];
#endif
/*==================================================================================================
    GLOBAL VARIABLES
==================================================================================================*/

/*
 *  OP_TRUE: cursor is showing.
 *  OP_FALSE: cursor is hiding.
 */
static OP_BOOLEAN gb_is_cursor_showing;

/*==================================================================================================
    GLOBAL EXTERNAL   FUNCTIONS
==================================================================================================*/
extern OP_BOOLEAN TBM_InsertString( OP_UINT32 handle, OP_UINT8 *pStr, OP_BOOLEAN bRefresh);
extern OP_BOOLEAN TBM_DeleteCurChar( OP_UINT32 handle, OP_BOOLEAN bRefresh );
extern int  widgetWindowShowSoftKey( const MsfWindow  *pWin, OP_BOOLEAN bRefresh );
extern int widgetInputGetImmInfo
(
    OP_UINT32          hInput, 
    MsfTextType         *pTextType,  
    OP_INT16            **ppCurImeIndex,
    DS_FONTATTR      *pFontAttr,
    OP_UINT32          *phWin,
    OP_INT16            *pCurWinBottom,
    struct tagMsfWidgetImm ***pppImmObj,
    struct tagMsfWidgetImm ***pppImmObjInScreen
);

extern int widgetPaintBoxRedrawForImm( MsfWindow    *pPaintBox, OP_BOOLEAN bRefresh);

extern OP_BOOLEAN widgetFocusGadgetAdjustWindow
(
    MsfGadget  *pGadget,
    MsfWindow   *pWin
);

extern int  widgetDrawTextInputCaret
( 
    const MsfTextInput      *pInput,
    OP_BOOLEAN            bRefresh
);

extern int  widgetWindowDrawAllGadgets(MsfWindow *pWin);

/*==================================================================================================
    LOCAL FUNCTIONS
==================================================================================================*/
int widgetImmSetWidgetLeftSoftKey
( 
    MsfWidgetImm  *pImm, 
    OP_UINT8* pLeftSoftKey
);
#ifdef _NO_CLEAR_KEY
int widgetImmSetWidgetRightSoftKey
( 
    MsfWidgetImm  *pImm, 
    OP_UINT8* pLeftSoftKey
);
#endif
static int  widgetImmSetImm(MsfWidgetImm  *pImm,OP_BOOLEAN bNext);

int widgetImmEventHandler
(
    MsfWidgetImm*                   pImm,
    OPUS_EVENT_ENUM_TYPE   event, 
    void                                      *pMess,
    OP_BOOLEAN                       *handle
);

int widgetImmOpen(MsfWidgetImm  *pImm );
int widgetImmClose(MsfWidgetImm  *pImm );

static OP_BOOLEAN widgetImmInsertString
(
    MsfWidgetImm  *pImm, 
    OP_BOOLEAN    bRefreshOnGadget
);

static int widgetImmAdjustWindow( MsfWidgetImm *pImm, OP_BOOLEAN  bAfterCreated );

int widgetImmRedrawScreen( MsfWidgetImm  *pImm );

static int  widgetImmPreHandle
(
    MsfWidgetImm                  *pImm,
    OPUS_EVENT_ENUM_TYPE   event, 
    void                                *pMess,
    OP_BOOLEAN                    *handle
);

static void  widgetImmResultHandle
(
    MsfWidgetImm                  *pImm,
    OPUS_EVENT_ENUM_TYPE   event, 
    void                                *pMess,
    OP_BOOLEAN                    *handle,
    MsfImmHandleResult           imm_ret
);

static int widgetImmSwitch
(
    MsfWidgetImm * pImm, 
    INPUT_METHOD_ENUM_T new_ime
);

static  int widgetImmSetInputMode(MsfWidgetImm *pImm, INPUT_DISP_MODE_ENUM_T disp_mode )
{
    INPUT_STATE_ENUM_T          input_state = INPUT_STATE_ANY;

    /*
     * Get current input state
     */
     IMM_get_current_state(pImm->hImm, &input_state);
    
    switch(disp_mode)
    {
    case INPUT_DISP_MODE_HANWRITING:
        pImm->start_y = EDITOR_IME_HW_START_Y;
        break;
    case INPUT_DISP_MODE_PINYIN:
        pImm->start_y = IME_PINYIN_DEVIDE_Y - 19; //EDITOR_IME_PINYIN_START_Y;
        if ( INPUT_STATE_SYMBOL == input_state)
        {
            pImm->start_y = EDITOR_IME_SYMBOL_START_Y;
        }    
        break;
    case INPUT_DISP_MODE_STROKE:
        pImm->start_y = IME_PINYIN_DEVIDE_Y - 19; //EDITOR_IME_PINYIN_START_Y;
        if ( INPUT_STATE_SYMBOL == input_state)
        {
            pImm->start_y = EDITOR_IME_SYMBOL_START_Y;
        }        
        break;
    case INPUT_DISP_MODE_SMART_ENG:
        pImm->start_y = EDITOR_ENG_START_Y;
        break;
    case INPUT_DISP_MODE_LETTER_LOWER:
        pImm->start_y = EDITOR_ENG_START_Y;
        break;
    case INPUT_DISP_MODE_LETTER_UPPER:
        pImm->start_y = EDITOR_ENG_START_Y;
        break;
    case INPUT_DISP_MODE_SYMBOL:
        pImm->start_y = EDITOR_IME_SYMBOL_START_Y;
        break;
    case INPUT_DISP_MODE_NUMBER:
        pImm->start_y = EDITOR_IME_NUMBER_START_Y;
        break;
    case INPUT_DISP_MODE_PBNUMBER:
    case INPUT_DISP_MODE_PHONENUM_GROUP:
        pImm->start_y = EDITOR_IME_PBNUMBER_START_Y;
        break;
    case INPUT_DISP_MODE_NUMERIC:
        pImm->start_y = EDITOR_IME_NUMERIC_START_Y;
        break;
    case INPUT_DISP_MODE_DIGITAL:
        pImm->start_y = EDITOR_IME_DIGITAL_START_Y;
        break;
    case INPUT_DISP_MODE_NONE:
    default:
        pImm->start_y = WAP_SCRN_TEXT_HIGH;
        
        break;
    }
    
    pImm->input_disp_mode = disp_mode;
    pImm->start_x = 0;

#if ( (PROJECT_ID == DOLPHIN_PROJECT) || (PROJECT_ID == ODIN_PROJECT) || (PROJECT_ID == WHALE1_PROJECT)\
    ||(PROJECT_ID == WHALE2_PROJECT) || (PROJECT_ID == NEMO_PROJECT) )
#else
#ifdef IMM_SHOW_IME_NAME_ON_TITLE
    IMM_draw_ime_icon(pImm->hImm);
    if (OP_TRUE == pImm->bRefreshImeIcon)
    {
        LCD_RECT_T rect;  
        RM_RESOURCE_ID_T        ime_icon_id;
        OP_UINT16 icon_width;
        
        IMM_get_ime_icon_id(pImm->hImm, &ime_icon_id);
        icon_width = ds_get_image_width(ime_icon_id);
        
        rect.left = TITLE_ICON_X_BEGIN;
        rect.right = TITLE_ICON_X_BEGIN + icon_width - 1;
        rect.top = TITLE_Y_BEGIN;
        rect.bottom = EDITOR_TITLE_BAR_HEIGHT -1;
        ds_refresh_text_rectangle(rect);
    }
#else
    {   /*  set left softkey  */
        RM_RESOURCE_ID_T                skey_left;
        
        if(IMM_has_menu(pImm->hImm) == OP_TRUE)
        {
            IMM_get_ime_skey( pImm->hImm, &skey_left );
        }
        else
        {
            skey_left = PMPT_SKEY_EMPTY;
        }
            
        util_get_text_from_res_w_max_len(skey_left, skeyStr, SKEY_MAX_CHAR*2 );
        widgetImmSetWidgetLeftSoftKey( pImm, skeyStr );
    }
#endif
#endif

    return TPI_WIDGET_OK;
}

/*==================================================================================================
    GLOBAL FUNCTIONS
==================================================================================================*/
MsfWidgetImm*  widgetImmCreate
( 
    OP_UINT32                       hInput, 
    OP_INT16                         iImeList,
    OP_INT16                         iImeIndex,
    DS_FONTATTR                   font_attr,
    OP_UINT32                      hWin,
    OP_INT16                        iWinBottom
)
{
    OP_INT16       font_width,font_height;
  
    if( hInput == INVALID_HANDLE || iImeList < 0 || iImeList >= (OP_INT16)MsfNoTextType )
    {
        return OP_NULL;
    }

    /*  already created  */
    if(OP_TRUE == bWidgetImmCreated) 
    {
        /*  exactly the old textinput  */
        if( hInput == widgetImm.hInput )
        {
            return &widgetImm;
        }
        else  /*  destroy the old imm  */
        {
            IMM_destroy( widgetImm.hImm );
        }
    }
    
    op_memset(&widgetImm, 0x0, sizeof(MsfWidgetImm));

    if(0 > widgetGetFontSizeInEngChar(font_attr, &font_width, &font_height) )
    {
        return OP_NULL;
    }

    widgetImm.hImm = IMM_create();
    IMM_set_call_ap_type(widgetImm.hImm, IMM_CALL_FROM_WAP);
    
    if( widgetImm.hImm == OP_NULL )
    {
        return OP_NULL;
    }

    widgetImm.hWindow = hWin;
    widgetImm.hInput = hInput;
    
    widgetImm.iWinBottom = iWinBottom;
    widgetImm.iCurImeList = iImeList;
    widgetImm.bOpen = OP_FALSE;
    widgetImm.bRefreshImeIcon = OP_TRUE;
    
    IMM_set_candidate_count( widgetImm.hImm , WIDGET_IMM_CANDIDATE_COUNT );
    if( iImeIndex < 0 || iImeIndex >= MAX_EDITOR_IME_CNT 
    || widgetImeList[widgetImm.iCurImeList][iImeIndex] == INPUT_METHOD_NONE  )
    {
        iImeIndex = 0;
    }
    
    widgetImm.iCurImeIndex = iImeIndex;
    if( OP_FALSE ==IMM_select_IME( widgetImm.hImm, widgetImeList[widgetImm.iCurImeList][iImeIndex] ))
    {
        IMM_destroy(widgetImm.hImm);
        widgetImm.hImm = 0;
        return OP_NULL;
    }
    
    /*
      * Notify IMM the display cols
      */
    IMM_set_disp_width(widgetImm.hImm, (OP_UINT8)((LCD_MAX_X )/font_width/2*2));

    bWidgetImmCreated = OP_TRUE;
    return &widgetImm;
}

int widgetImmDestroy(MsfWidgetImm *pImm, OP_UINT32 hInput )
{
    int iRet = TPI_WIDGET_ERROR_INVALID_PARAM;

    if( (pImm== &widgetImm) 
         && ( OP_TRUE == bWidgetImmCreated)
         && ( hInput == widgetImm.hInput ))
    {
        iRet = TPI_WIDGET_OK;
        
        bWidgetImmCreated = OP_FALSE;
        
        skeyStr[0] = skeyStr[1] = 0x00;
        widgetImmSetWidgetLeftSoftKey( pImm, OP_NULL );
        
        IMM_destroy(pImm->hImm);
        pImm->hImm = 0;

        if( pImm->bOpen == OP_TRUE )
        {
            pImm->bOpen = OP_FALSE;
            iRet = widgetImmAdjustWindow( pImm, OP_FALSE);
        }
        op_memset(pImm, 0, sizeof(widgetImm));
    }

    return iRet;
}


/*==================================================================================================
    FUNCTION:  widgetImmEventHandler

    DESCRIPTION:
        
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
         < 0 : some error
         0   :  event handled
         2   :  event handled, and need to redraw the content

    IMPORTANT NOTES:
        The words in imm_state name have special meanings:
            READY          need insert a char(s)
            PREREADY     the char(s) is ready, but need user to select
            PENDING       the char(s) is ready, but user can not select
            REPEAT        need the IMM_send_event to re-handle one time
            WARNING     user operation error
            
==================================================================================================*/
int widgetImmEventHandler
(
    MsfWidgetImm*                   pImm,
    OPUS_EVENT_ENUM_TYPE   event, 
    void                                      *pMess,
    OP_BOOLEAN                       *handle
)
{
    MsfImmHandleResult             imm_ret = WIDGET_RET_NONE;
    IME_STATE_ENUM_T            imm_state = IME_STATE_NONE;
    INPUT_STATE_ENUM_T          input_state = INPUT_STATE_ANY;
    INPUT_DISP_MODE_ENUM_T    disp_mode = INPUT_DISP_MODE_NONE;
    OP_BOOLEAN                      bRedraw = OP_FALSE;
    int                             iRet;
    OP_BOOLEAN bTextChanged = OP_FALSE;
    MsfTextInput *pTextInput;
    MsfInput* pInput ;
    MsfWidgetType   wt;
#ifndef EDITOR_LETTER_SHOW_ELEMENT    
    OP_UINT8                           *pResult;
#endif
#ifdef _EDITOR_NO_CLEAR_KEY
    IME_HAS_CANDIDATE_ENUM_T eImehasCandidate = IME_HAS_CANDIDATE_NONE;
    OP_BOOLEAN                         bRtn = OP_FALSE;
#endif

    iRet = widgetImmPreHandle( pImm, event, pMess, handle);
    if( iRet != WIDGET_EVENT_NOT_HANDLE )
    {
        return iRet;
    }

    
    *handle = OP_TRUE;
    imm_ret = WIDGET_RET_NONE;
    
    pTextInput = (MsfTextInput*)seekWidget(pImm->hInput, &wt);
    if( !pTextInput || wt != MSF_TEXTINPUT )
    {
        return OP_FALSE;
    }

    pInput = &pTextInput->input;
    
    if(!pInput)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if (pImm->input_disp_mode == INPUT_DISP_MODE_HANWRITING
        && event == OPUS_FOCUS_PEN)
    {
        TP_PEN_EVENT_STRUCTURE_T   *pPen;
        pPen = (TP_PEN_EVENT_STRUCTURE_T *)pMess;
        //
        // If the pen down event occurs in the scroll bar area.
        //
        if (pPen->state == TP_PEN_STATE_PENDN
            && pPen->x >= EDITOR_WIDTH
            && pPen->x < LCD_MAX_X
            && pPen->y >= EDITOR_WINDOW_REGION_TOP_FIX
            && pPen->y < EDITOR_IME_HW_START_Y + LCD_ANNUN_HIGH
            )
        {
            return TPI_WIDGET_HW_SCROLLING;
        }
        
    }


    /*
     * Use IMM to process.
     */
    while(1)
    {
        /*
         * Get current input state
         */
         IMM_get_current_state(pImm->hImm, &input_state);
        /*
         * Send event to IMM
         */
        imm_state = IMM_send_event( pImm->hImm , event, pMess);

        if( pImm->bOpen == OP_FALSE
            && imm_state != IME_STATE_WARNING 
            && imm_state != IME_STATE_NONE 
            && imm_state < IME_STATE_SKEY_CHANGED)
        {
            IMM_get_disp_mode(pImm->hImm, &disp_mode);
            widgetImmSetInputMode(pImm, disp_mode);

            iRet = widgetImmOpen( pImm );
            // IMM_draw_win_frame(pImm->hImm);            
        }
        
        /*
         * Process return code
         */
        switch(imm_state)
        {
        case IME_STATE_WARNING:
            imm_ret = WIDGET_RET_WARNING;
            break;
        case IME_STATE_NONE:
            // imm_ret = WIDGET_RET_PROCESSED;
            imm_ret = WIDGET_RET_NONE;
            break;
        case IME_STATE_SKEY_CHANGED:
#ifdef IMM_SHOW_IME_NAME_ON_TITLE
            IMM_draw_ime_icon(pImm->hImm);
            {
                LCD_RECT_T rect;
                RM_RESOURCE_ID_T        ime_icon_id;
                OP_UINT16 icon_width;
                
                IMM_get_ime_icon_id(pImm->hImm, &ime_icon_id);
                icon_width = ds_get_image_width(ime_icon_id);
        
                rect.left = TITLE_ICON_X_BEGIN;
                rect.right = TITLE_ICON_X_BEGIN + icon_width - 1;
                rect.top = TITLE_Y_BEGIN;
                rect.bottom = EDITOR_TITLE_BAR_HEIGHT -1;
                ds_refresh_text_rectangle(rect);
            }
#else
            {   /*  set left softkey  */
                RM_RESOURCE_ID_T                skey_left;
                
                IMM_get_ime_skey( pImm->hImm, &skey_left );
                util_get_text_from_res_w_max_len(skey_left, skeyStr, SKEY_MAX_CHAR*2 );
                widgetImmSetWidgetLeftSoftKey( pImm, skeyStr );
            }
#endif
        /* fallthough to handle the previous inputed char*/
        case IME_STATE_READY_REPEAT:
        case IME_STATE_READY_PASS:
        case IME_STATE_READY:
        case IME_STATE_READY_PENDING:
        case IME_STATE_HW_READY_PASS:
            {
                if( (pImm->input_disp_mode == INPUT_DISP_MODE_HANWRITING )
                    && ( pImm->bPrereadyInserted == OP_TRUE )
                    && ( imm_state != IME_STATE_READY_PASS )
                    && ( imm_state != IME_STATE_READY)
                    )
                {
                    TBM_DeleteCurChar(pImm->hInput, OP_FALSE);
                    pImm->bPrereadyInserted = OP_FALSE;
                }
                else if( (imm_state == IME_STATE_READY)
                    && (pImm->bPrereadyInserted == OP_TRUE)
                    && INPUT_METHOD_PNUMBER_GROUP == widgetImeList[pImm->iCurImeList][pImm->iCurImeIndex]
                    )
                {
                    TBM_DeleteCurChar(pImm->hInput, OP_FALSE);
                    pImm->bPrereadyInserted = OP_FALSE;
                }
                
                if ( (IME_STATE_READY == imm_state) &&
                      (INPUT_STATE_SYMBOL != input_state) &&
                          ((pImm->input_disp_mode == INPUT_DISP_MODE_LETTER_LOWER)||
                           (pImm->input_disp_mode == INPUT_DISP_MODE_LETTER_UPPER)
                          )
                        ) /* Make bPrereadyInsert FALSE after letter timer out */
                {
#ifdef EDITOR_LETTER_SHOW_ELEMENT                
                    pImm->bPrereadyInserted = OP_FALSE;
#else
                    IMM_get_result(pImm->hImm , &pResult);
                    if( *pResult != 0x00 || *(pResult+1) != 0x00 )
                    {
                        TBM_InsertString(pImm->hInput, pResult, OP_TRUE );
                    }
#endif                    
                }
                else
                {
                    widgetImmInsertString(pImm ,OP_TRUE );
                }                
            }                

            /* not redraw the symbol area */

            if( iRet != WIDGET_WINDOW_ALREADY_DRAWN
#ifndef _EDITOR_NO_SYMBOL_IME                
                && widgetImeList[pImm->iCurImeList][pImm->iCurImeIndex] != INPUT_METHOD_SYMBOL 
#endif                
                )
            {
                widgetImmShow( pImm, OP_FALSE);
            }
            
            if( (imm_state == IME_STATE_READY_PASS) || (imm_state == IME_STATE_HW_READY_PASS))
            {
                imm_ret = WIDGET_RET_NONE;
                break;
            }
            else if(imm_state == IME_STATE_READY_REPEAT)
            {
                continue;
            }
            else
            {
                imm_ret = WIDGET_RET_PROCESSED;
                break;
            }
            
        case IME_STATE_PENDING:
            imm_ret =  WIDGET_RET_PROCESSED;
            if(  iRet != WIDGET_WINDOW_ALREADY_DRAWN )
            {
                widgetImmShow( pImm, OP_FALSE );
            }
            break;

        case IME_STATE_PREREADY:
        case IME_STATE_SYMBOL_PREREADY:

            /* Process Phone book Preready Insert */
            if ( INPUT_METHOD_PNUMBER_GROUP == widgetImeList[pImm->iCurImeList][pImm->iCurImeIndex] )
            {
                pImm->bPrereadyInserted = widgetImmInsertString(pImm ,OP_FALSE);
            }
            else if ((pImm->input_disp_mode == INPUT_DISP_MODE_HANWRITING) && 
                     (event == OPUS_TIMER_TEXT_INPUT)
                     ) /* Process timer out in hw */
            {
                pImm->bPrereadyInserted = widgetImmInsertString(pImm ,OP_FALSE);
            }
            else if (((pImm->input_disp_mode == INPUT_DISP_MODE_LETTER_LOWER) || 
                      (pImm->input_disp_mode == INPUT_DISP_MODE_LETTER_UPPER)
                      )&&
                      (IME_STATE_PREREADY == imm_state)
                     ) /* Process letter first input */
            {
#ifdef EDITOR_LETTER_SHOW_ELEMENT            
                if (pImm->bPrereadyInserted == OP_TRUE) /* Process letter same key except first time */
                {
                    TBM_DeleteCurChar(pImm->hInput, OP_FALSE);
                }

                pImm->bPrereadyInserted = widgetImmInsertString(pImm ,OP_TRUE);
                if (pImm->bPrereadyInserted == OP_FALSE)
            {
                    imm_ret = WIDGET_RET_WARNING;
                    break;
                }
#endif                
            }
            imm_ret = WIDGET_RET_PROCESSED;
            break;
        case IME_STATE_READY_PREREADY:
            imm_ret = WIDGET_RET_PROCESSED;
            
            if( pImm->input_disp_mode == INPUT_DISP_MODE_HANWRITING )
            {
                /*  write another word  */
                if( event == OPUS_TIMER_TEXT_INPUT )
                {
                    pImm->bPrereadyInserted = widgetImmInsertString(pImm, OP_FALSE);
                }
                else
                {
                    if( event == OPUS_FOCUS_KEYPAD )
                    {
                        // TBM_InsertString( pImm->hInput, pImm->candidate_buf, OP_TRUE);
                        if(  iRet != WIDGET_WINDOW_ALREADY_DRAWN )
                        {
                           widgetImmShow( pImm, OP_TRUE);
                        }
                    }
                    else
                    {
                        widgetImmInsertString(pImm, OP_TRUE);
                    }
                    pImm->bPrereadyInserted = OP_FALSE;
                }
            }
            else
            {
                bRedraw = widgetImmInsertString(pImm, OP_TRUE);
                if( (bRedraw == OP_TRUE) && ( iRet != WIDGET_WINDOW_ALREADY_DRAWN ))
                {
                    widgetImmShow( pImm, OP_TRUE );
                }
            }  
            break;
#ifdef _EDITOR_HAS_IME_HANDWRITING            
        case IME_STATE_HW_PASS:    
            {
                TPO_EDITOR_PEN_STATUS ebOnWrittingOrRecognize;

                ebOnWrittingOrRecognize = TPO_Get_Editor_Pen_Status();
                if(ebOnWrittingOrRecognize == TP_EDITOR_PEN_ON_WRITING ||
                   ebOnWrittingOrRecognize == TP_EDITOR_PEN_ON_RECOGNISING)
                {
                    pImm->bOnHandWritting = OP_TRUE;
                }
                else
                {
                    pImm->bOnHandWritting = OP_FALSE;
                }
            }
            return TPI_WIDGET_OK;
#endif            
        case IME_STATE_DELETE:
            bTextChanged = TBM_DelCursorChar(&(pInput->txtInputBufMng));
            imm_ret = WIDGET_RET_PROCESSED;
            break;
        case IME_STATE_DELETE_ALL:
            bTextChanged = TBM_DelString(&(pInput->txtInputBufMng));
            imm_ret = WIDGET_RET_PROCESSED;
            break;
        case IME_STATE_CHANGE_TO_NUMBER:
            event = INPUT_METHOD_NUMBER;
            imm_ret = WIDGET_RET_IME_SWITCH;
            break;
#ifndef _EDITOR_NO_SYMBOL_IME            
        case IME_STATE_CHANGE_TO_SYMBOL:
            event = INPUT_METHOD_SYMBOL;
            imm_ret = WIDGET_RET_IME_SWITCH;
            break;
#endif            
        case IME_STATE_CHANGE_TO_PREVIOUS:
            event = INPUT_METHOD_NONE;
            imm_ret = WIDGET_RET_IME_SWITCH;
            break;
        case IME_STATE_SWITCH_IME:
            {
                INPUT_METHOD_ENUM_T ime;
                IMM_get_imeid_switch(pImm->hImm, &ime);
                if (ime > INPUT_METHOD_NONE
                    && ime <INPUT_METHOD_MAX)
                {
                    event = ime;
                    imm_ret = WIDGET_RET_IME_SWITCH;
                }
                else
                {
                    imm_ret = WIDGET_RET_PROCESSED;
                }
                break;
            }
        case IME_STATE_HW_LOCATING_CUSROR:
            if (gb_is_cursor_showing)
            {
                widgetDrawTextInputCaret(pTextInput, OP_FALSE);
            }
            
            gb_is_cursor_showing = OP_FALSE;
            pImm->bOnHandWritting = OP_FALSE;
            pImm->bPrereadyInserted = OP_FALSE;
            return TPI_WIDGET_HW_CURSOR_LOCATING;
            break;
        default:
            return TPI_WIDGET_ERROR_UNEXPECTED;
            break;
        }
        break;
    }
#ifdef _NO_CLEAR_KEY    
        /* Set right soft key */
        bRtn = ImmGetImeHasCandidateState(pImm->hImm, &eImehasCandidate);
        if (!bRtn)
        {
            eImehasCandidate = IME_HAS_CANDIDATE_NONE;
        }

        if (eImehasCandidate == IME_HAS_CANDIDATE_NORMAL)               /* Show "delete"  */
        {    
            util_get_text_from_res_w_max_len(PMPT_SKEY_DELETE, SoftRightKeyStr, SKEY_MAX_CHAR*2 );
            widgetImmSetWidgetRightSoftKey( pImm, SoftRightKeyStr );
        }
        else if (eImehasCandidate == IME_HAS_CANDIDATE_LEGEND)         /* Show "back " */
        {
            util_get_text_from_res_w_max_len(PMPT_SKEY_BACK, SoftRightKeyStr, SKEY_MAX_CHAR*2 );
            widgetImmSetWidgetRightSoftKey( pImm, SoftRightKeyStr );            
        }
        else if (eImehasCandidate == IME_HAS_CANDIDATE_NONE)            /* Show widget */
        {
            if (pInput->txtInputBufMng.iValidCharCount > 0)
            {
                util_get_text_from_res_w_max_len(PMPT_SKEY_DELETE, SoftRightKeyStr, SKEY_MAX_CHAR*2 );
                widgetImmSetWidgetRightSoftKey( pImm, SoftRightKeyStr );
            }
            else
            {
               widgetImmSetWidgetRightSoftKey( pImm, OP_NULL );
            }
        }
#endif
    if( bTextChanged)
    {
        widgetRedraw( (void*)pTextInput, MSF_TEXTINPUT, pImm->hInput, OP_TRUE);
    }
    /*
     *  Handle editor actions set by IME.
     */
    {
        OP_UINT32 editor_action;
        INPUT_METHOD_ENUM_T   ime;
        IMM_get_editor_action(pImm->hImm, &editor_action);
        IMM_reset_editor_action(pImm->hImm);
        
        if ((editor_action & EDITOR_ACTION_SWITCH_IME) > 0)
        {
#ifndef _EDITOR_NO_SYMBOL_IME        
            IMM_get_imeid(pImm->hImm, &ime);
            if (ime == INPUT_METHOD_SYMBOL)
            {
                INPUT_METHOD_ENUM_T   pre_ime;
                if(IMM_get_prev_imeid (pImm->hImm,&pre_ime) == OP_TRUE)
                {
                    if (pre_ime > INPUT_METHOD_NONE && pre_ime < INPUT_METHOD_MAX)
                    {
                        event = pre_ime;
                        imm_ret = WIDGET_RET_IME_SWITCH;
                    }
                }
            }
#endif            
        }
        if ((editor_action & EDITOR_ACTION_STOP_DELETING_TIMER) > 0)
        {
            pImm->bTimerStop = OP_TRUE;
        }
    }

#ifdef _EDITOR_HANDLE_PEN_EVENT
    if (IME_STATE_NONE == imm_state)
    {
        if( event == OPUS_TIMER_TP_KEYPRESS_REPEAT)
        {
            pImm->bTimerStop = OP_FALSE;
            OPUS_Start_Timer(OPUS_TIMER_DELETE_CONTINUOUSLY, 
                EDITOR_TIMER_DEL_CONTINUOUS_TIME, 
                0, 
                ONE_SHOT);
                
            return TPI_WIDGET_OK;
        }
        
        if( event == OPUS_TIMER_DELETE_CONTINUOUSLY && (!pImm->bTimerStop))
        {
            bTextChanged = TBM_DelCursorChar(&(pInput->txtInputBufMng));
            if (bTextChanged)
            {
                widgetRedraw( (void*)pTextInput, MSF_TEXTINPUT, pImm->hInput, OP_TRUE);
                OPUS_Start_Timer(OPUS_TIMER_DELETE_CONTINUOUSLY, 
                    EDITOR_TIMER_DEL_CONTINUOUS_TIME, 
                    0, 
                    ONE_SHOT);
            }
            return TPI_WIDGET_OK;
        }
    }
#endif

    widgetImmResultHandle( pImm, event, pMess, handle, imm_ret);
    
    return TPI_WIDGET_OK;
}

int widgetImmShow( MsfWidgetImm* pImm, OP_BOOLEAN  bRefresh )
{
    OP_UINT8                            *pElement;
    OP_UINT8                            *pCandidate;
    OP_UINT8                             iStartPos;
    OP_UINT8                             iLen;
    OP_BOOLEAN                        bSpecial;

    if (!pImm)
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    if( pImm  != &widgetImm || pImm->hImm == 0 )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    if( bRefresh == OP_TRUE )
    {
        IMM_draw_win_frame(pImm->hImm);
    }

    IMM_get_pending(pImm->hImm, &pElement, &iStartPos, &iLen, &pCandidate, &bSpecial);

    if( 
        ((!pElement || (pElement[0] == 0x00 && pElement[1] == 0x00))
        &&( !pCandidate || (pCandidate[0] == 0x00 && pCandidate[1] == 0x00))) 
#ifdef _EDITOR_HAS_IME_HANDWRITING
        && (widgetImeList[pImm->iCurImeList][pImm->iCurImeIndex] != INPUT_METHOD_HANDWRITING)
#endif
        )
    {
        return widgetImmClose( pImm );
    }

    {
        MsfWidgetType wt;
        MsfWindow  *pWin;
        
        pWin = (MsfWindow*)seekWidget(pImm->hWindow, &wt);
        if( pWin && IS_WINDOW(wt) && pWin->bDrawing == OP_FALSE)
        {
            WAP_REFRESH
        }
    }
    
    return TPI_WIDGET_OK;
}


int  widgetImmDrawElements
(
    MsfWidgetImm* pImm,
    OP_UINT8*        pElement,
    OP_UINT8*        pCandidate,
    OP_BOOLEAN        bCandidateSpecial
)
{
    int          iRet = TPI_WIDGET_ERROR_UNEXPECTED;    
    
    if(!pImm )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }


    return iRet;
}

int widgetImmSwitch(MsfWidgetImm * pImm, INPUT_METHOD_ENUM_T new_ime)
{
    INPUT_DISP_MODE_ENUM_T    disp_mode;
    // INPUT_METHOD_ENUM_T         current_ime;
/*
    if (new_ime == INPUT_METHOD_SYMBOL || new_ime == INPUT_METHOD_NUMBER)
    {
        IMM_get_imeid(pImm->hImm, &current_ime);
        if (current_ime != INPUT_METHOD_SYMBOL && current_ime != INPUT_METHOD_NUMBER)
        {
            pImm->pre_ime = current_ime;
        }
    }

    if (new_ime == INPUT_METHOD_NONE)
    {
        new_ime = pImm->pre_ime;
    }
    */
    widgetImmClose( pImm );

    IMM_select_IME(pImm->hImm, new_ime);
    IMM_get_disp_mode(pImm->hImm, &disp_mode);
    widgetImmSetInputMode(pImm, disp_mode);
    
    widgetImmOpen( pImm );
    widgetImmShow( pImm, OP_TRUE );

    return OP_TRUE;
}
static int  widgetImmSetImm(MsfWidgetImm  *pImm,OP_BOOLEAN bNext)
{
    OP_INT16    iNewImm;
    INPUT_DISP_MODE_ENUM_T    disp_mode;
    INPUT_METHOD_ENUM_T       eIme = INPUT_METHOD_NONE;         // for switch ime.
    OP_UINT8                index = 0xFF;   // the index of ime in the switch array
    int i;      // for loop
    int iCurrentMaxCount = MAX_EDITOR_IME_CNT;
    if( !pImm )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    {
        /* Get Current Max Count According To MsfNoTextType */
        for (i = 0; i < MAX_EDITOR_IME_CNT; i++)
        {
            if (widgetImeList[pImm->iCurImeList][i] == INPUT_METHOD_NONE )
            {
                iCurrentMaxCount = i;
                break;
            }
        }

        /* calc the new ime index */
        if (bNext)
        {
            iNewImm = (pImm->iCurImeIndex + 1) % iCurrentMaxCount;
        }
        else
        {
            iNewImm = (iCurrentMaxCount + pImm->iCurImeIndex - 1)%iCurrentMaxCount;
        }
        
        if( widgetImeList[pImm->iCurImeList][iNewImm] == INPUT_METHOD_NONE )
        {
            iNewImm = 0;
        }

        if(pImm->iCurImeIndex == iNewImm)  /* only one ime */
        {
            return (TPI_WIDGET_OK);
        }
        else 
        {
            pImm->iCurImeIndex = iNewImm;
            eIme = widgetImeList[pImm->iCurImeList][pImm->iCurImeIndex];
        }
    }
    
    widgetImmClose( pImm );

    IMM_select_IME(pImm->hImm, eIme);
    IMM_get_disp_mode(pImm->hImm, &disp_mode);
    widgetImmSetInputMode(pImm, disp_mode);
    
    if( 
#ifndef _EDITOR_NO_SYMBOL_IME        
        (widgetImeList[pImm->iCurImeList][pImm->iCurImeIndex] == INPUT_METHOD_SYMBOL )
#else
     OP_FALSE
#endif     
#ifdef _TOUCH_PANEL
        ||(widgetImeList[pImm->iCurImeList][pImm->iCurImeIndex] == INPUT_METHOD_HANDWRITING )
#endif
        )
    {
        widgetImmOpen( pImm );
        widgetImmShow( pImm, OP_TRUE );
    }
    
    return (TPI_WIDGET_OK);    
}

int widgetImmSetWidgetLeftSoftKey( MsfWidgetImm  *pImm, OP_UINT8* pLeftSoftKey)
{
    MsfGadget        *pGadget;
    MsfWidgetType  wt;
    
    if( !pImm )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pGadget = (MsfGadget*)seekWidget( pImm->hInput, &wt);
    if( !pGadget || wt != MSF_TEXTINPUT )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    pGadget->softKeyList[0] = pLeftSoftKey;

    if( pGadget->parent != OP_NULL )
    {
        widgetWindowShowSoftKey( pGadget->parent, OP_TRUE );
    }

    return TPI_WIDGET_OK;
}


int  widgetInputFocusChange
( 
    OP_UINT32       hInput, 
    OP_BOOLEAN    bInfocus
)
{
    static OP_BOOLEAN   bReEnter = OP_FALSE;
    MsfTextType         txtType;
    OP_INT16            *pCurImeIndex;
    DS_FONTATTR      fontattr;
    OP_UINT32          hWin;
    OP_INT16            iCurWinBottom;
    MsfWidgetImm      **ppImmObj;
    MsfWidgetImm      **ppScreenImmObj;
    int                      iRet;
    MsfWidgetType     wt;
    
    wt = getWidgetType(hInput);
    if( wt != MSF_TEXTINPUT )
    {
        return TPI_WIDGET_OK;
    }
    
    /*  avoid reenter the function  */
    if( bReEnter == OP_TRUE )
    {
        return TPI_WIDGET_OK;
    }

    bReEnter = OP_TRUE;
    
    iRet = widgetInputGetImmInfo( 
                            hInput, 
                            &txtType, 
                            &pCurImeIndex, 
                            &fontattr, 
                            &hWin,
                            &iCurWinBottom, 
                            &ppImmObj,
                            &ppScreenImmObj);

    if( iRet < 0 )
    {
        bReEnter = OP_FALSE;
        return iRet;
    }
    
    if( bInfocus == OP_TRUE )
    {
        MsfWidgetImm *pImm;
        INPUT_DISP_MODE_ENUM_T  disp_mode;
        OP_INT16            iImeList;

        iImeList =  ((txtType >= MsfNoTextType )? 0 : (OP_INT16) txtType);
        
        pImm = widgetImmCreate( hInput, iImeList, *pCurImeIndex, fontattr, hWin, iCurWinBottom);
        if(  pImm == OP_NULL )
        {
            bReEnter = OP_FALSE;
            return TPI_WIDGET_ERROR_UNEXPECTED;
        }

        *ppImmObj  = pImm;
        *ppScreenImmObj = pImm;
        
        /*
          * Notify EDM current how many line the IME will be occupied
          */
        IMM_get_disp_mode(pImm->hImm, &disp_mode);
        pImm->bRefreshImeIcon = OP_FALSE;
        iRet = widgetImmSetInputMode( pImm, disp_mode);
        pImm->bRefreshImeIcon = OP_TRUE;
        if( iRet < 0 )
        {
            bReEnter = OP_FALSE;
            return iRet;
        }

        // IMM_draw_win_frame (pImm->hImm);
#ifndef _EDITOR_NO_SYMBOL_IME        
        if( widgetImeList[pImm->iCurImeList][*pCurImeIndex] == INPUT_METHOD_SYMBOL 
            || pImm->input_disp_mode == INPUT_DISP_MODE_HANWRITING )   
#else
        if( pImm->input_disp_mode == INPUT_DISP_MODE_HANWRITING )   
#endif    
        {
            pImm->bOpen = OP_TRUE;
            iRet = widgetImmAdjustWindow( pImm, OP_TRUE );
        }
        
    }
    else
    {
        MsfWidgetImm   *pImm;
        MsfWindow         *pWin;
        INPUT_METHOD_ENUM_T ime_id;

        if( *ppImmObj == OP_NULL )
        {
            bReEnter = OP_FALSE;
            return TPI_WIDGET_OK;
        }        
        
        pImm = *ppImmObj;        
        *ppImmObj = OP_NULL;
        if( !ppScreenImmObj )
        {
            pWin = seekWidget(pImm->hWindow, &wt);
            if( pWin && pWin->isFocused )
            {
                pWin->parent->pImm = OP_NULL;
            }
        }
        else
        {
            *ppScreenImmObj = OP_NULL;
        }
        
        /* save the imm index  */
         *pCurImeIndex = pImm->iCurImeIndex;
        IMM_get_imeid(pImm->hImm, &ime_id);
        
#ifndef _KEYPAD
        if (ime_id >= INPUT_METHOD_HANDWRITING
            && ime_id <= INPUT_METHOD_SYMBOL)
        {
            for (i = 0; i < MAX_EDITOR_IME_CNT; i++)
            {
                ime_in_list = widgetImeList[pImm->iCurImeList][i];
                if (ime_in_list == INPUT_METHOD_NONE)
                {
                    break;
                }
                if (ime_in_list == ime_id)
                {
                    *pCurImeIndex = i;
                    break;
                }
            }
        }
#endif

/*added by gaoyan,to redraw title while the input box lost focus*/
#ifdef IMM_SHOW_IME_NAME_ON_TITLE
        pWin = seekWidget(pImm->hWindow, &wt);
        if(pWin && pWin->isFocused && IS_WINDOW(wt ))
        {
            if(wt == MSF_PAINTBOX)
            {
                widgetPaintBoxRedrawForImm(pWin, OP_TRUE );
            }
            else
            {
                widgetWindowDrawTitle(pWin, OP_NULL, OP_TRUE);        
            }
        }
#endif
        
        iRet = widgetImmDestroy( pImm, hInput );
    }

    bReEnter = OP_FALSE;
    return iRet;
}

int widgetImmOpen(MsfWidgetImm  *pImm )
{
    int   iRet;
    
    if( !pImm )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    iRet = TPI_WIDGET_OK;
    
    if((pImm->input_disp_mode != INPUT_DISP_MODE_NONE )
        && ( pImm->input_disp_mode  < INPUT_DISP_MODE_MAX ))
    {
        if (pImm->bOpen == OP_FALSE )
        {
            pImm->bOpen = OP_TRUE;
            iRet = widgetImmAdjustWindow( pImm, OP_FALSE );
            if( iRet < 0 )
            {
                return iRet;
            }
        }
    }
    
    return iRet;
}

int widgetImmClose(MsfWidgetImm  *pImm )
{
    int   iRet;
    
    if( !pImm )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    iRet = TPI_WIDGET_OK;
    
    if( pImm->bOpen == OP_TRUE )
    {
        pImm->bOpen = OP_FALSE;
        iRet = widgetImmAdjustWindow( pImm, OP_FALSE );
    }

    return iRet;
}    


/*==================================================================================================
    FUNCTION:  

    DESCRIPTION:
    
        
    ARGUMENTS PASSED:
        
    RETURN VALUE:
                   -1       :  some error
                     0       :  not redraw whole window ( normal )
                     2       :  already draw whole window

    IMPORTANT NOTES:
        None
==================================================================================================*/
static int widgetImmAdjustWindow( MsfWidgetImm  *pImm, OP_BOOLEAN  bAfterCreated)
{
    MsfWindow          *pWin;
    MsfGadget          *pGadget;
    MsfWidgetType    wtWin, wtGadget;
    OP_INT16           iImmStart, iWinBottom;    
    OP_INT16           iDelta;
    int                     iRet;

    if( !pImm )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pWin = (MsfWindow*)seekWidget(pImm->hWindow, &wtWin);
    if( !pWin || !IS_WINDOW(wtWin))
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    iImmStart = pImm->start_y -1;
    iWinBottom = pImm->iWinBottom;
    iRet = TPI_WIDGET_OK;
    
    if( wtWin == MSF_PAINTBOX )
    {
        /* If the imm is closed,  redraw the paintbox  */
        if( pImm->bOpen == OP_FALSE  && iImmStart < iWinBottom  )
        {
            widgetPaintBoxRedrawForImm(pWin, OP_TRUE );
            iRet = WIDGET_WINDOW_ALREADY_DRAWN;
        }
    }
    else if( wtWin == MSF_EDITOR )
    {
        OP_INT16 iLabLines;

        /* not consider that the editor is not full screen */
        iDelta = 0;
        if(  iImmStart < iWinBottom )
        {
            if( pImm->bOpen == OP_TRUE)
            {
                iDelta = iImmStart - iWinBottom;
            }
            else 
            {
                iDelta = iWinBottom - iImmStart;
            }
        }
        
        if( iDelta != 0 )  /*  change window size  */
        {
            pGadget = (MsfGadget*)((MsfEditor*)pWin)->input;

            pGadget->size.height += iDelta;

            if( ISVALIDHANDLE(pGadget->title)  )
            {
                iLabLines = 1;
            }
            else
            {
                iLabLines = 0; 
            }
            
            TBM_AdjustCursorForScroll( pGadget->gadgetHandle, &pGadget->size, iLabLines, iDelta);
            if( bAfterCreated )
            {
                widgetRedraw((void*)pWin, wtWin, INVALID_HANDLE, OP_TRUE);
                iRet = WIDGET_WINDOW_ALREADY_DRAWN;
            }
            else
            {
                iRet = widgetRedraw( (void*)pGadget, MSF_TEXTINPUT, INVALID_HANDLE, OP_TRUE);
            }
        }
    }
    else
    {
        if( pImm->bOpen == OP_TRUE)
        {
            pGadget = (MsfGadget*)seekWidget( pImm->hInput, &wtGadget);
            if( !pGadget || ( wtGadget != MSF_TEXTINPUT )) 
            {
                return TPI_WIDGET_ERROR_UNEXPECTED;
            }
            
            if( widgetFocusGadgetAdjustWindow(pGadget, pWin) == OP_TRUE)
            {
                widgetRedraw((void*)pWin, wtWin, INVALID_HANDLE, OP_TRUE);
                iRet = WIDGET_WINDOW_ALREADY_DRAWN;
            }
        }
        else   /*  enlarge window */
        {
            iDelta = 0;
            if( iImmStart < iWinBottom )
            {
                iDelta = iImmStart - iWinBottom;
            }
            
            pWin->scrollPos.y += iDelta;
            if( pWin->scrollPos.y < 0 )
            {
                pWin->scrollPos.y = 0;
            }
            
            /* need whole window redrawing  */
            iRet = widgetRedraw( (void*)pWin, wtWin, pWin->windowHandle, OP_TRUE);
            if( iRet >= 0 )
            {
                iRet = WIDGET_WINDOW_ALREADY_DRAWN;
            }
        }        
    }
    
    return iRet;
}

static int  widgetImmPreHandle
(
    MsfWidgetImm                  *pImm,
    OPUS_EVENT_ENUM_TYPE   event, 
    void                                *pMess,
    OP_BOOLEAN                    *handle
)
{
    KEYEVENT_STRUCT  *pKey;
    
    if( (pImm!= &widgetImm) || (handle == OP_NULL) )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }
    
    if( event == OPUS_FOCUS_KEYPAD || event == OPUS_TIMER_KEYPRESS_REPEAT) 
    {
        if( pImm->bOnHandWritting == OP_TRUE )
        {
            *handle = OP_TRUE;
            return WIDGET_EVENT_HANDLED;
        }
        
        pKey = (KEYEVENT_STRUCT*)pMess;
        
        /*
         *  Add KEY_STAR event is handled by IMM itself.
         *  Do not pass by this function.
         *  Andrew P002113
         */
        if(((pKey->state == UHKEY_RELEASE)||(event == OPUS_TIMER_KEYPRESS_REPEAT)) 
            &&(pKey->code != KEY_POUND ) 
            &&(pKey->code != KEY_STAR)
            &&(pKey->code != KEY_0)
          )
        {
            if( pKey->code != pImm->pressKeyCode )
            {
                *handle = OP_FALSE;
            }
            else
            {
                *handle = OP_TRUE;
            }
            
            /*  don't  left  the key-release event to window handle  */
            return WIDGET_EVENT_HANDLED;
        }
    }

    return WIDGET_EVENT_NOT_HANDLE;
}

static void  widgetImmResultHandle
(
    MsfWidgetImm                  *pImm,
    OPUS_EVENT_ENUM_TYPE   event, 
    void                                *pMess,
    OP_BOOLEAN                    *handle,
    MsfImmHandleResult           imm_ret
)
{
    KEYEVENT_STRUCT  *pKey;
    OP_UINT8               volume;
    static OP_BOOLEAN           bRecievedLongPress = OP_FALSE;
    OP_BOOLEAN bNextTurn = OP_TRUE;

    if( event == OPUS_TIMER_TEXT_INPUT
       && pImm->input_disp_mode == INPUT_DISP_MODE_HANWRITING )
    {
        pImm->bOnHandWritting = OP_FALSE;
    }

    
    if( WIDGET_RET_NONE == imm_ret )
    {
        if( event == OPUS_FOCUS_KEYPAD) 
        {
            pKey = (KEYEVENT_STRUCT*)pMess;
            
            if( 
#ifndef IMM_SHOW_IME_NAME_ON_TITLE
            pKey->code == KEY_SOFT_LEFT || 
#endif 
                ( pKey->code == EDITOR_KEY_SWITCH_IME               // Add press # to switch IME.
                && UHKEY_RELEASE == pKey->state         // Handle when key release
                && OP_FALSE == bRecievedLongPress) )    // Handle when it is not long press
            {
                imm_ret = WIDGET_RET_IME_SELECT;
            }
            else if( (pKey->code == KEY_0)
                   &&(pKey->state == UHKEY_PRESS)
                   )
            {
                TBM_InsertString(pImm->hInput, (OP_UINT8*)"\x20\x0\x0\x0", OP_TRUE );
                imm_ret = WIDGET_RET_PROCESSED;
            }
#ifdef EDITOR_ENABLE_SIDE_KEY_SWITCH_IME
            else if (( pKey->code == KEY_SIDE_DOWN)
                && (OP_FALSE == bRecievedLongPress))
            {
                imm_ret = WIDGET_RET_IME_SELECT;
                bNextTurn = OP_TRUE;
            }
            else if (( pKey->code == KEY_SIDE_UP)
                && (OP_FALSE == bRecievedLongPress))
            {
                imm_ret = WIDGET_RET_IME_SELECT;
                bNextTurn = OP_FALSE;
            }
#endif
            
            // Reset Long Press event
            if ( UHKEY_RELEASE == pKey->state )
            {
                bRecievedLongPress = OP_FALSE;
            }
        }

        pKey = (KEYEVENT_STRUCT*)pMess;
        if (event == OPUS_TIMER_KEYPRESS_REPEAT)
        {
            if ( pKey->code == KEY_POUND )
            {
                /*be handled in multi line text input only. */
                if (OP_FALSE == bRecievedLongPress)     // Only handling once
                {
                    int iRet;
                    MsfInput *pTextInput;
                    
                    iRet = getInput(pImm->hInput, &pTextInput,OP_NULL,OP_NULL);
                    if( iRet >= 0 )
                    {
                        if( pTextInput->txtInputBufMng.rows > 1)
                        {
                            TBM_InsertString(pImm->hInput, (OP_UINT8*)"\xa\x0\x0\x0", OP_TRUE );
                        }
                    }
                    bRecievedLongPress = OP_TRUE;       // Set long press flag
                }
            }
        }
    }

    switch(imm_ret)
    {
    case WIDGET_RET_IME_SELECT:
        TPIa_widgetUpdateBegin(pImm->hWindow);
        widgetImmSetImm(pImm,bNextTurn);
        TPIa_widgetUpdateEnd(pImm->hWindow);
        break;
#ifdef _EDITOR_HANDLE_PEN_EVENT
    case WIDGET_RET_IME_SWITCH:
        TPIa_widgetUpdateBegin(pImm->hWindow);
        widgetImmSwitch(pImm, event);
        TPIa_widgetUpdateEnd(pImm->hWindow);
        break;
#endif
    case WIDGET_RET_WARNING:
    case WIDGET_RET_TOO_LONG:
    case WIDGET_RET_TOO_FEW:
    case WIDGET_RET_OUTOF_RANGE:
        {
            volume = SP_GetVolume( BELL_LEVEL_ID ).LEVEL;
            SP_Audio_play_request(WIDGET_IMM_WARNING_SOUND, volume, OP_FALSE);
            break;
        }
    case WIDGET_RET_PROCESSED:
    case WIDGET_RET_FINISHED:
        break;
    case WIDGET_RET_BACK:
    case WIDGET_RET_QUIT:
    case WIDGET_RET_MAX:
    case WIDGET_RET_NONE:
    default:
        *handle = OP_FALSE;
        break;
    }

    if( event == OPUS_FOCUS_KEYPAD )
    {
        pKey = (KEYEVENT_STRUCT*)pMess;
        if( *handle == OP_TRUE )
        {
            /*  save the pressed key code into pressKeyCode,  
            * and not let the key release event  pass on  
            */
            pImm->pressKeyCode = pKey->code;
        }
        else
        {
            pImm->pressKeyCode = 0;
        }
    }

}

int widgetImmRedrawScreen( MsfWidgetImm  *pImm )
{
    OP_BOOLEAN  bDrawScreenDirect;
    MsfWindow       *pWin;
    MsfWidgetType wt;
    MsfGadget *pTextInput=OP_NULL; 
    if( !pImm )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pWin = (MsfWindow*)seekWidget( pImm->hWindow, &wt);
    if( !pWin || !IS_WINDOW(wt))
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }
    
    bDrawScreenDirect = OP_TRUE;
    if( wt  == MSF_PAINTBOX)
    {
        if( restoreDisplay( OP_FALSE, OP_NULL ) )
        {
            GadgetsInWindow   *pGadgetNode;
            MsfGadget          *pGadget;

            bDrawScreenDirect = OP_FALSE;
            
            widgetWindowDrawAllGadgets(pWin);
            
            /* play image (gif animation)  */
            pGadgetNode = pWin->children;
            while( pGadgetNode )
            {
                pGadget = pGadgetNode->gadget;
                if(pGadget && getWidgetType(pGadget->gadgetHandle) == MSF_IMAGEGADGET )
                {
                    TPIa_widgetImagePlay(pGadget->gadgetHandle);
                }

                pGadgetNode = pGadgetNode->next;
            }
            
            pTextInput = (MsfGadget*)seekWidget(pImm->hInput, &wt);
//            widgetRedraw( (void*)pTextInput, MSF_TEXTINPUT, INVALID_HANDLE, OP_FALSE);

            if( pTextInput->isFocused == OP_TRUE && pImm->bOpen == OP_TRUE)
            {
                widgetImmShow( pImm, OP_TRUE );
            }
            
            WAP_REFRESH
        }
    }

    if( bDrawScreenDirect == OP_TRUE )
    {
        widgetRedraw( pWin->parent, MSF_SCREEN, INVALID_HANDLE, OP_TRUE);
    }

    return TPI_WIDGET_OK;
}

int widgetImmSetWidgetRightSoftKey( MsfWidgetImm  *pImm, OP_UINT8* pRightSoftKey)
{
    MsfGadget        *pGadget;
    MsfWidgetType  wt;
    
    if( !pImm )
    {
        return TPI_WIDGET_ERROR_INVALID_PARAM;
    }

    pGadget = (MsfGadget*)seekWidget( pImm->hInput, &wt);
    if( !pGadget || wt != MSF_TEXTINPUT )
    {
        return TPI_WIDGET_ERROR_UNEXPECTED;
    }

    pGadget->softKeyList[2] = pRightSoftKey;

    if( pGadget->parent != OP_NULL )
    {
        widgetWindowShowSoftKey( pGadget->parent, OP_TRUE );
    }

    return TPI_WIDGET_OK;
}
static OP_BOOLEAN widgetImmInsertString
(
 MsfWidgetImm  *pImm,
 OP_BOOLEAN    bRefreshOnGadget
 )
{
    OP_UINT8         *pResult;
    OP_BOOLEAN     bInserted;
    
    bInserted = OP_FALSE;
    
    IMM_get_result(pImm->hImm , &pResult);
    if( *pResult != 0x00 || *(pResult+1) != 0x00 )
    {
        bInserted = TBM_InsertString(pImm->hInput, pResult, bRefreshOnGadget );
    } 
    
    if( bRefreshOnGadget == OP_FALSE )
    {
        widgetImmRedrawScreen( pImm);
    }
    
    return bInserted;
}

void widgetImmSetCursorState(OP_BOOLEAN b_show)
{
    gb_is_cursor_showing = b_show;
}

OP_BOOLEAN widgetImmGetCursorState(void)
{
    return gb_is_cursor_showing;
}

void widgetImmChangeCursorState(void)
{
    gb_is_cursor_showing = gb_is_cursor_showing ? OP_FALSE : OP_TRUE;
}
#endif
