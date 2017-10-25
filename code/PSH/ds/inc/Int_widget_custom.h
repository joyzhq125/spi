#ifndef INT_WIDGET_CUSTOM_H
#define INT_WIDGET_CUSTOM_H
/*==================================================================================================

    HEADER NAME : int_widget_custom.h

    GENERAL DESCRIPTION
        General description of the contents of this header file.

    SEF Telecom Confidential Proprietary
    (c) Copyright 2003 by SEF Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    23/05/2003   zhuxq            Cxxxxxx     Initial file creation. 
    07/26/2003   lindawang        P001074     Fix draw image problems.
    04/08/2003   Zhuxq            P001052     Modify the image default size and add a propertyMask member to structure MsfDefaultProperties
    05/08/2003   Steven Lai       P001140     Rearrange the .h files.
    08/13/2003   Zhuxq            P001147     Change the selectgroup show and menu show
    08/14/2003   lindawang        P001254     Fix draw image problem for wap.
    08/20/2003   lindawang        P001308     Add stop_Gif_timer() define  
    8/18/2003    Zhuxq            P001263     Modify  the input method of textinput, allow to create a input method anytime  if a textinput require
    08/20/2003   Zhuxq            P001279     modify the default size of imagegadget
    08/23/2003   lindawang        P001350     solove the MMS Gif stop problem.
    09/26/2003   linda wang       P001451     separate the gif play and show function.    
    10/17/2003   Zhuxq            C001603     Add touch panel feature to WAP & MMS
    10/23/2003   Zhuxq            P001842     Optimize the paintbox and input method solution and fix some bug
    12/03/2003   Zhuxq            P002139     Enhance the sensitivity of scrollbar in WAP&MMS
    12/30/2003   linda wang       P002286     Modify the macro define for wap module menu title and item margine.
    01/05/2004   Zhuxq            C000072     Add callback function to each widget in the widget system
    01/15/2004   Zhuxq            P002352     All editing actions of MsfTextInput turns to MsfEditor
    02/10/2004   chenxiao         p002405     add form scrollbar  in the dolphin project
    02/16/2004   Zhuxq            P002469     Add button gadget to widget system
    02/16/2004   chenxiao         p002474     add bitmap animation function in the dolphin project    
    02/17/2004   Zhuxq            P002483     Adjust the display system of widget to show up the annunciator and fix some bugs
    02/24/2004   chenxiao         p002530     modify WAP & MSG main menu 
    02/28/2004   Zhuxq            P002556     Rename the arrow button callback functions
    03/19/2004   Chenxiao         p002688     change main menu  of wap and fix some bugs from NEC feedback     
    03/30/2004   zhuxq            P002700     Fix UMB bugs
    03/31/2004   Dingjianxin      P002754     Delete the startGifTimer and stopGifTimer function decleration
    04/03/2004   chenxiao         p002806     The display information is incorrect.        
    04/02/2004   zhuxq            P002789     Fix UMB bugs
    04/14/2004   Dingjianxin      p002961     Delete the conditional compiler
    05/11/2004   zhuxq            P005514     Adjust layout for B1 and synchronize B1 and Dolpin
    05/28/2004   zhuxq            P005745     Adjust layout in UMB for B1
    06/04/2004   zhuxq            P005925     Correct issues found by PC-lint and fix some bugs
    06/08/2004   zhuxq            P005978     Synchronize B1 with Dolphin( TF_DLP_VER_01.23 )
    06/12/2004   majingtao        P006119     fix bugs about incomplete dialog information
    06/12/2004   zhuxq            P006085     Add lotus macro for lotus   
    06/18/2004   zhuxq            P006260     Make up WAP&UMB code to cnxt_int_wap branch
    08/09/2004   Hugh zhang       p007607     Change font of widget dialog in lotus project.
    
    Self-documenting Code
    Describe/explain low-level design, especially things in header files, of this module and/or
    group of funtions and/or specific funtion that are hard to understand by reading code
    and thus requires detail description.
    Free format !

====================================================================================================
    INCLUDE FILES
==================================================================================================*/
#include "widget_defs.h"
#include "msf_int.h"
/*==================================================================================================
    CONSTANTS
==================================================================================================*/


/*==================================================================================================
    MACROS
==================================================================================================*/

#define   DEFAULT_GADGET_HEIGHT   ( GADGET_LINE_HEIGHT + 4 )
#define   DEFAULT_GADGET_WIDTH    (DS_SCRN_MAX_X -4)
#define   DEFAULT_CHOICE_HEIGHT   (CHOICE_LINE_HEIGHT * 4  + 4 )

#define   IMAGE_DRAW_DEFAULT_IMAGE   2


#define WIDGET_PEN_EVENT_TO_OK   MSF_GADGET_PROPERTY_PEN_CLICK

#ifdef _SHOW_ANNUNCIATOR_IN_ALL_WINDOW
#define  WAP_SCRN_TEXT_HIGH    LCD_TEXT_HIGH
#else
#define  WAP_SCRN_TEXT_HIGH    LCD_EXT_TEXT_HIGH
#endif


#define   WAP_REFRESH      WidgetRefresh();


/*==================================================================================================
    ENUMERATIONS
==================================================================================================*/

/*==================================================================================================
    DATA STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct MsfDefaultProperties{
    MsfPosition                              pos;
    MsfSize                                    size;
    const MsfColor                        *fgColors;
    const MsfColor                        *bgColors;
    int                                           propertyMask;
}MsfDefaultProperties;

typedef struct{
    const OP_UINT8 *pBuf;
    MsfPosition         pos;
    MsfSize               size;
    DS_COLOR          fontcolor;
    DS_COLOR          backcolor;
    DS_FONTATTR      fontAttr;
    OP_BOOLEAN      bShowInitialWhiteSpaces;
}MsfWidgetDrawString;
typedef enum 
{
    IMG_UP = 0,
    IMG_DOWN,
    IMG_LEFT,
    IMG_RIGHT,
    IMG_INVALID
}IMAGEGADGET_HANDLE_AREA;


extern const MsfDefaultProperties  DefaultPropTab[];

/*
  * style default value
  */
extern const MsfPattern       DEFAULT_STYLE_FOREGROUND;
extern const MsfPattern       DEFAULT_STYLE_BACKGROUND ;
extern const MsfLineStyle     DEFAULT_STYLE_LINESTYLE;
extern const MsfFont          DEFAULT_STYLE_FONT;
extern const MsfFont          DEFAULT_MENU_STYLE_FONT;
extern const MsfFont          DEFAULT_TEXTINPUT_STYLE_FONT;
extern const MsfFont          DEFAULT_DIALOG_STYLE_FONT;
extern const MsfTextProperty  DEFAULT_STYLE_TEXTPROPERTY;
extern const DS_COLOR         DEFAULT_INPUT_INITSTRING_BG_COLORS;
extern const DS_COLOR         DEFAULT_INPUT_FOCUS_BG_COLORS;
extern const MsfColor         DEFAULT_BAR_FG_COLORS;
extern const DS_COLOR         DEFAULT_FOCUS_BORDER_COLORS;
extern const DS_COLOR         DEFAULT_FOCUS_BG_COLORS;
extern const MsfColor         DEFAULT_INPUT_BG_COLORS;
extern const DS_COLOR         DEFAULT_SCROLLBAR_FG_COLORS;
extern const DS_COLOR         DEFAULT_SCROLLBAR_BG_COLORS;
extern const RM_RESOURCE_ID_T ButtonNormalBackgroundRes;
extern const RM_RESOURCE_ID_T ButtonFocusBackgroundRes;

extern int widgetScreenDrawLine
(
    const MsfWindow   *pWin,
    OP_INT16 start_x,
    OP_INT16 start_y,
    OP_INT16 end_x,
    OP_INT16 end_y,
    DS_COLOR color
);

extern int widgetScreenNCDrawLine
(
    const MsfWindow   *pWin,
    OP_INT16 start_x,
    OP_INT16 start_y,
    OP_INT16 end_x,
    OP_INT16 end_y,
    DS_COLOR color
);

extern int widgetScreenDrawRect
(
    const MsfWindow   *pWin,
    OP_INT16 left,
    OP_INT16 top,
    OP_INT16 right,
    OP_INT16 bottom,
    DS_COLOR color
);

extern int widgetScreenFillWindowClientRect(const MsfWindow   *pWin, DS_COLOR color );

extern int widgetScreenFillRect
(
    const MsfWindow   *pWin,
    OP_INT16 left,
    OP_INT16 top,
    OP_INT16 right,
    OP_INT16 bottom,
    DS_COLOR color
);

extern int widgetScreenDrawTextLine
(
    const MsfWindow   *pWin,
    OP_INT16       x,
    OP_INT16       y,
    OP_INT16       xlimit,
    OP_UINT8       *text,
    DS_FONTATTR    fontattr,
    DS_COLOR       fontcolor,
    DS_COLOR       backcolor
);

extern int  widgetScreenDrawIconRm
(
    const MsfWindow            *pWin,
    OP_INT16                      x,
    OP_INT16                      y,
    OP_INT16                     width,
    OP_INT16                     height,
    RM_RESOURCE_ID_T     icon_res_id
);

extern int  widgetScreenDrawIcon
(
    const MsfWindow            *pWin,
    OP_INT16                      x,
    OP_INT16                      y,
    OP_INT16                     width,
    OP_INT16                     height,
    RM_ICON_T                  *pIcon
);

extern int  widgetScreenDrawBitmapRm
(
    const MsfWindow            *pWin,
    OP_INT16                      x,
    OP_INT16                      y,
    OP_INT16                     width,
    OP_INT16                     height,
    RM_RESOURCE_ID_T     icon_res_id
);

extern int  widgetScreenDrawBitmap
(
    const MsfWindow            *pWin,
    OP_INT16                      x,
    OP_INT16                      y,
    OP_INT16                     width,
    OP_INT16                     height,
    RM_BITMAP_T               *pBitmap
);

extern int  widgetScreenDrawImage
(
    MsfWindow         *pWin,
    MsfPosition         *pStart, 
    MsfSize            *pSize,
    MsfImage          *pImage,
    MsfImageZoom      imageZoom
);

extern int widgetScreenDrawCaret
(  
    const MsfWindow  *pWin, 
    OP_INT16            x, 
    OP_INT16            y,  
    OP_INT16            iCaretHeight
);

extern DS_SCRN_TITLE_ENUM_T  widgetWindowGetTitleType( const MsfWindow  *pWin );

extern int  widgetWindowShowSoftKey( const MsfWindow  *pWin, OP_BOOLEAN bRefresh );
extern int  widgetScreenShowSoftKey( const MsfScreen* pScreen, OP_BOOLEAN bRefresh );
extern RM_IMAGE_FORMAT convertImageFormat
(
    MsfImageFormat imageformat
);
extern int widget_draw_default_image
(
    MsfWindow       *pWin,
    MsfPosition       *pStart, 
    MsfSize          *pSize
);

extern void  widget_draw_focus_rect
(
    int             iValue,
    MsfWindow       *pWin,
    MsfPosition     *pStart, 
    MsfSize         *pSize
);
extern int widgetImageGadgetPlay
(
    MsfImageGadget *pImageGadget,
    OP_BOOLEAN bRefresh
);

#if 0
extern void get_image_src_postion
(
    MsfPosition    *src_start,  /* dest resource start of image */
    MsfPosition    *cur_srcstart, /* start position opositive the image(0,0)*/
    MsfPosition    *gadgetPos,
    MsfSize        *gadgetSize,
    MsfSize        *imageSize,
    OP_INT16       tp_x,
    OP_INT16       tp_y
);

extern void image_scroll
(
    MsfGadget    *pGadget,
    MsfImage     *pImage,
    OP_INT16     tp_x,
    OP_INT16     tp_y
);
#endif

extern int fnBmpAniTimerCallBack(MsfGadgetHandle handle, OP_UINT32 TimerId);
extern int widget_button_up_arrow_callback(MSF_UINT8 modID, /* modId */
                                      MSF_UINT32 handle ,           /*  widget handle */
                                      MsfNotificationType notifyType,  /*  state notification  */
                                      void *pExtraData );   /*  extra data. it is used  ONLY the MsfNotificationType is MsfNotifyStateChange. The meanings as following: */
extern int widget_button_down_arrow_callback(MSF_UINT8 modID, /* modId */
                                      MSF_UINT32 handle ,           /*  widget handle */
                                      MsfNotificationType notifyType,  /*  state notification  */
                                      void *pExtraData );   /*  extra data. it is used  ONLY the MsfNotificationType is MsfNotifyStateChange. The meanings as following: */

extern OP_BOOLEAN  widgetGetImageSize(RM_RESOURCE_ID_T res_id, MsfSize *pSize);
extern int widgetDrawAnnunciator();
extern void backupDisplay( void );
extern OP_BOOLEAN restoreDisplay( OP_BOOLEAN   bOnlyPaintBox , MsfWindow  *pPaintBox);
extern void  clearBackupDisplay(void);
extern int WidgetRefresh();

extern OP_BOOLEAN IsValidImageRegion
(
    OP_INT16 width, 
    OP_INT16 height
);

/*================================================================================================*/
#endif  /* INT_WIDGET_CUSTOM_H */

