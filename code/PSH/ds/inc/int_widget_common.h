#ifndef INT_WIDGET_COMMON_H
#define INT_WIDGET_COMMON_H
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
    04/03/2003   Steven Lai       Cxxxxxx     Initial file creation. 
    07/02/2003   linda wang       P000763     fix draw bitmap file image problem 
    7/30/2003     Zhuxq               P001026    Rename the function widgetEnterEditor to widgetEditorShow
    07/26/2003   lindawang        P001074     Fix draw image problems.
    08/13/2003   Zhuxq             P001147      Change the selectgroup show and menu show
    08/20/2003   Zhuxq             P001279      modify the access the size of imagegadget
    09/03/2003   Zhuxq             P001479      Add two funcion declarations of saveScreen and restoreScreen
    09/04/2003   linda wang        P001494      Add MMS Memory status
                                                - add some Ustr fuctions.
    09/25/2003   Zhuxq             P001696     Re-implement the dialog
    
    09/26/2003   linda wang        P001451      separate the gif play and show function.
    02/16/2004   chenxiao           p002474     add bitmap animation function in the dolphin project
    02/17/2004   Zhuxq             P002492      Add Viewer window to widget system
    03/31/2004   dingjianxin        PR002754     Optimize the imagegadget when palying the annimation, the Timer start/stop by app not wap
    05/24/2004    zhuxq            P005568        Adjust UI implementation in UMB and fix some bugs
    08/30/2004   zhuxq             c007998     improve the implementation of predefined string to reduce the times of memory allocation
    Self-documenting Code
    Describe/explain low-level design, especially things in header files, of this module and/or
    group of funtions and/or specific funtion that are hard to understand by reading code
    and thus requires detail description.
    Free format !

====================================================================================================
    INCLUDE FILES
==================================================================================================*/
#include "SP_list.h"
#include "Ds_def.h"

/*==================================================================================================
    CONSTANTS
==================================================================================================*/
#define INVALID_HANDLE      0

#define BACKGROUND_COLOR ( 0XFFFF )
/*==================================================================================================
    MACROS
==================================================================================================*/
#define ISVALIDHANDLE( handle )       ( (handle) != INVALID_HANDLE )

#define IS_SCREEN(widgetType)    (widgetType == MSF_SCREEN)

#define IS_WINDOW(widgetType)   (widgetType > MSF_SCREEN && widgetType <= MSF_WINDOW_END)

#define IS_GADGET(widgetType) (widgetType > MSF_WINDOW_END && widgetType <= MSF_GADGET_END)

#define IS_LOWLEVEL(widgetType)    (widgetType > MSF_GADGET_END && widgetType <= MSF_LOWLEVEL_WIDGET_END)

#define getWidgetType(handle)  (MsfWidgetType)((((handle) & 0x00FF0000)>>16))

#define getModID(handle)       (OP_UINT8)(((handle) & 0xFF000000)>>24)

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#define WIDGET_GET_STRING_DATA(pChar, handle) \
    {   \
        MsfString   *pStr;  \
        MsfWidgetType wt;  \
        pChar = OP_NULL; \
        pStr = (MsfString*)seekWidget( handle,  &wt); \
        if( pStr )   \
        {   \
            if( wt == MSF_STRING_RES )  \
            {  \
                pChar = (OP_UINT8*)pStr;  \
            }      \
            else if( wt == MSF_STRING && pStr->data != OP_NULL )  \
            {    \
                pChar = pStr->data;   \
            }  \
        }  \
    }
    
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

typedef union{
    int   color;
    struct {
         OP_UINT8 r;
         OP_UINT8 g;
         OP_UINT8 b;
    }colorRGB;
    struct {
         OP_INT16 h;
         OP_UINT8 s;
         OP_UINT8 v;
    }colorHSV;  
    struct{
         OP_INT16 h;
         OP_UINT8 s;
         OP_UINT8 l;
    }colorHSL;      
} MsfWidgetColor;
/*==================================================================================================
    GLOBAL VARIABLES DECLARATIONS
==================================================================================================*/
extern GENERAL_LIST_STRUCT_T widgetQueue[];
extern MsfDisplay *pDisplay;


/*==================================================================================================
    FUNCTION PROTOTYPES
==================================================================================================*/

extern OP_UINT32 getNewHandle(OP_UINT8 modId, MsfWidgetType widgetType);


extern OP_BOOLEAN  isValidHandleOfType( OP_UINT32 handle, MsfWidgetType wgType);

extern  int  getInput(OP_UINT32 text, MsfInput  **ppInput,MsfSize *pSize, MSF_UINT16 *pLabelLines);

extern int  getChoice(OP_UINT32 choice, MsfChoice  **ppChoice );

extern void* seekWidget(OP_UINT32 handle, MsfWidgetType*  pWidgetType);

extern choiceElement* getChoiceElementByIndex(MsfChoice *choice, int index);

extern choiceElement* getLastChoiceElement( MsfChoice *choice);

extern int  fillInput(    MsfInput* pInput, OP_UINT8 modId,  MsfStringHandle initialString, MsfStringHandle inputString, MsfTextType type,  int maxSize,  int singleLine );

extern int strCatToBuffer( MsfStringHandle initString, MsfStringHandle inputString, OP_UINT8 *pBuf );

//extern int deleteHeadString( OP_UINT8 * pHead, OP_UINT8 * pStr , OP_UINT8 *pDst , int iDstSize );

extern int getInputTextStringLength( MsfInput *pInput, OP_BOOLEAN bIncludeInitString, OP_BOOLEAN bMaxSize, OP_BOOLEAN bStartFromZero );


extern int setInputTextInputString(MsfInput* pInput, OP_UINT8* pNewStr);

extern int replaceHandle(OP_UINT32 *target, OP_UINT32 source);

extern int getInputTextStringToBuffer( MsfInput *pInput, OP_BOOLEAN bIncludeInitString, OP_BOOLEAN bStartFromZero,OP_UINT8 *pBuf, int iBufSize, OP_UINT16 *pInitStringLen);


extern MsfScreen * seekFocusedScreen(void);

extern MsfScreenHandle getFocusedScreenHandle(void);

extern void* seekFocusedWindow(void);

/*
extern int UTF8toUnicode(OP_UINT8 * dst, const OP_UINT8 * src);

extern int UnicodetoUTF8(OP_UINT8 * dst, OP_UINT8 * src);
*/
extern int unicode_to_utf8 (const char *src, int *src_len,
                         char *dst, int *dst_len);

extern int utf8_to_unicode (const char *src, int *src_len,
                          char *dst, int *dst_len);

extern OP_INT16  widgetGetLinesCrossPoints
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
);

extern OP_BOOLEAN widgetLineGetVisible
(
    OP_INT16           rectleft,
    OP_INT16           recttop,
    OP_INT16           rectright,
    OP_INT16           rectbottom,
    OP_INT16           *left,
    OP_INT16           *top,
    OP_INT16           *right,
    OP_INT16           *bottom
);

extern OP_BOOLEAN widgetRectGetVisible
(
    OP_INT16           rectleft,
    OP_INT16           recttop,
    OP_INT16           rectright,
    OP_INT16           rectbottom,
    OP_INT16           *pLeft,
    OP_INT16           *pTop,
    OP_INT16           *pRight,
    OP_INT16           *pBottom
);


extern int  widgetRGBToHSL( int rgb);

extern int  widgetHSLToRGB( int  hsl );

extern int widgetGetDrawingInfo
( 
    void                  *pWidget, 
    MsfWidgetType   widgetType, 
    OP_UINT32       msfHandle,
    DS_FONTATTR  *pFontattr, 
    DS_COLOR       *pFontcolor, 
    DS_COLOR       *pBackcolor
);

extern int widgetGetStyleInfo
(
    void                      *pWidget, 
    MsfWidgetType     widgetType, 
    OP_UINT32         msfHandle,
    MsfStyle                 *pStyle
);

extern OP_UINT8*  widgetGetPredefinedString( OP_UINT32 resId );
extern OP_UINT8*  widgetGetPredefinedStringEx(OP_UINT8 modId, OP_UINT8 lang, OP_UINT16 index);
extern RM_ICON_T *WidgetGetPredefineIcon
(
    OP_UINT32    resId
);

extern int  widgetGadgetGetSize
( 
    MsfGadget         *pGadget,
    MsfSize            *pSize
);

extern void saveScreen(void);
extern void restoreScreen(void);
extern void widgetNumtoUstr(MSF_UINT8 *dest, const MSF_INT32 num);
extern void widgetUstrCat(MSF_UINT8 *dest, MSF_UINT8 *src, MSF_UINT32 resId);
extern void widgetUstrCpy(MSF_UINT8 *dest, MSF_UINT8 *src, MSF_UINT32 resId);
extern MSF_INT32 widgetUstrlen(MSF_UINT8 *str);


extern MsfTextInput*  widgetDialogGetInputByIndex( MsfDialog *pDialog, int index );
extern int widgetGetFontSizeInEngChar
(
    DS_FONTATTR              fontAttr,
    OP_INT16                    *pFontWidth,
    OP_INT16                    *pFontHeight
);


extern OP_BOOLEAN  widgetAppIsCurrentFocusApp(MSF_UINT8 modId);

/*================================================================================================*/
#endif  /* INT_WIDGET_COMMON_H */
