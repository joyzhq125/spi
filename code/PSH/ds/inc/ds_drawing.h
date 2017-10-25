#ifndef DS_DRAWING_H
#define DS_DRAWING_H
/*==================================================================================================

    HEADER NAME : ds_drawing.h

    GENERAL DESCRIPTION
        This file includes drawing modles using drawing primitives.

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    06/22/2002   Ashley Lee       crxxxxx     Initial Creation
    02/11/2003   Steven Lai        PP00069     Update the type of some parameter
    02/24/2002   wangwei          PP00117     Change draw bitmap to draw icon when play animation.
    05/15/2003   linda wang       P000289     Extend the width and height defination from UINT8 to UINT16.
    06/14/2003   lindawang        P000474     Support play gif animation and fix some bugs.
    07/02/2003   lindawang        P000763     Fix draw bitmap file image problems.
    07/18/2003   Yuan Hui         P000968     Remove warning.
    07/26/2003   lindawang        P001074     Fix some wap draw image problem.
    08/14/2003   lindawang        P001241     Fix some ds and gif decode problems.
    08/20/2003   linda wang       P001293     Remove the number icon define into ds_drawing.h.    
    08/23/2003   Yuan Hui         P001353     add interface for stop annuciator blink timer.
    09/26/2003   linda wang       P001714     Provide WAP_REFRESH function for speeden refresh MMS&WAP module. 
    02/09/2004   Liyingjun         p002449     adjust line base y for dolphin
    02/16/2004   Zhuxq             P002469     replace XXX_forward functions with macro definitions
    03/04/2004   lindawang        C002586     Add open window function and modify annun layout.
    03/19/2004   Chenxiao         p002688      change main menu  of wap and fix some bugs from NEC feedback     
    03/30/2004   Chenxiao         p002758      fix bugs for bookmark and color scheme in the wap module
    04/07/2004   lindawang        P002824      Add base y position define for other app.
    04/19/2004   lindawang        P003037      provide function for get and set extend softkey area.    
    04/23/2004   gongxiaofeng     p005058      add ds_set_scrollbar() function.
    04/24/2004   linda wang       c005088      Add jpg idle image and power on/off animation.
    05/24/2004   penghaibo        p005629      fix jpg decode bug (add parameter to decode ds_* function)
    05/31/2004   linda wang       c005786      fix bug for display annunciator when phone lock in power up.
    06/04/2004   linda wang       p005612      add draw all annunciator funcions for app.
    07/30/2004   gongxiaofeng     p007403       remove functions.
    This file includes drawing modles using drawing primitives.

====================================================================================================
    INCLUDE FILES
==================================================================================================*/

#include    "ds.h"
#include "cam_interface.h"
/*==================================================================================================
    CONSTANTS
==================================================================================================*/

/*==================================================================================================
    MACROS
==================================================================================================*/
/* line start y_position*/
#if ((PROJECT_ID == ODIN_PROJECT)||(PROJECT_ID == DOLPHIN_PROJECT))
#define    LINE_BASE_Y                    18
#else
#define    LINE_BASE_Y                    14
#endif

/* add for other app text or item display start */
#define    APP_USER_BASE_Y            (LINE_BASE_Y+LCD_ANNUN_HIGH)//((ds.disp_annun==OP_TRUE)?(LINE_BASE_Y+LCD_ANNUN_HIGH):LINE_BASE_Y)
/* Number of line number icons - 1~60, 0, # and * */
#define    NUM_LINENUM_ICON     63
#define    NUM_LINENUM_ICON_COUNT     60
 
#define POPUP_SCROLLBAR_WIDTH           7
#define POPUP_SCROLLBAR_LEFT            108
#define POPUP_SCROLLBAR_RIGHT           115
#define POPUP_SCROLLBAR_TOP             49
#define POPUP_SCROLLBAR_BOTTOM          101
#define POPUP_SCROLLBAR_SPILT_HIGHT     7
/*==================================================================================================
    GLOBAL VARIABLES DECLARATIONS
==================================================================================================*/

/*==================================================================================================
    FUNCTION PROTOTYPES
==================================================================================================*/
/* all following forward function is drawn in text area */
#define ds_draw_bitmap_image_forward(x, y, pBitmap)   \
    draw_bitmap_image(AREA_TEXT, x, y, pBitmap)
#define ds_draw_bitmap_image_rm_forward(x, y, bitmap_res_id)   \
    draw_bitmap_image(AREA_TEXT, x, y, (RM_BITMAP_T*)util_get_bitmap_from_res(bitmap_res_id))

#define ds_draw_partial_bitmap_image_forward(dsx,dsy,d_width,d_height, ssx, ssy, pBitmap)    \
    draw_partial_bitmap_image(AREA_TEXT, dsx, dsy, d_width, d_height, ssx, ssy, pBitmap)

#define ds_draw_partial_bitmap_image_rm_forward(dsx,dsy,d_width,d_height, ssx, ssy, bitmap_res_id)    \
    draw_partial_bitmap_image(AREA_TEXT, dsx, dsy, d_width, d_height, ssx, ssy, (RM_BITMAP_T*)util_get_bitmap_from_res(bitmap_res_id))

#define ds_draw_resized_bitmap_image_forward(dx, dy, d_width, d_height, sx, sy, pBitmap)   \
    draw_resized_bitmap_image(AREA_TEXT, dx, dy, d_width, d_height, sx, sy, pBitmap)

#define ds_draw_resized_bitmap_image_rm_forward(dx, dy, d_width, d_height, sx, sy, bitmap_res_id)   \
    draw_resized_bitmap_image(AREA_TEXT, dx, dy, d_width, d_height, sx, sy, (RM_BITMAP_T*)util_get_bitmap_from_res(bitmap_res_id))

#define ds_put_pixel_forward( pos_x, pos_y, color)    \
    put_pixel(AREA_TEXT, pos_x, pos_y, color)

#define ds_draw_line_forward( startx, starty, endx, endy, color)    \
    draw_line(AREA_TEXT, startx, starty, endx, endy, color)

#define ds_draw_rect_forward( left, top, right, bottom, color)   \
    draw_rect(AREA_TEXT, left, top, right, bottom, color)

#define ds_fill_rect_forward( left, top, right, bottom, color)    \
    fill_rect(AREA_TEXT, left, top, right, bottom, color)

#define ds_draw_button_rect_forward( left, top, right, bottom, left_top, right_bottom)    \
    draw_button_rect (AREA_TEXT, left, top, right, bottom, left_top, right_bottom)

#define ds_draw_button_forward( left, top, right, bottom, top_left_color, right_bottom_color, fill_color)  \
    draw_button (AREA_TEXT, left, top, right, bottom, top_left_color,right_bottom_color,fill_color)


/*==================================================================================================
    FUNCTION: init_ds

    DESCRIPTION:
        Sets up the screen handlers' internal variables, blanks the screen

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important aspect of using
        the function i.e side effect..etc
==================================================================================================*/
extern void ds_init_ds
( 
    void 
);


/*==================================================================================================
    FUNCTION: ds_init_scrn_attr 

    DESCRIPTION:
       Initialize screen attributes.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_init_scrn_attr 
( 
    void
);


/**************************************************************************************************
    Refresh Functions
**************************************************************************************************/

/*==================================================================================================
    FUNCTION: ds_set_update

    DESCRIPTION:
        Indicate that the somewhere of the screen is updated so that it will be refreshed next time. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important aspect of using
        the function i.e side effect..etc
==================================================================================================*/
extern void ds_set_update
(
    void
);

/*==================================================================================================
    FUNCTION: ds_set_update_region 

    DESCRIPTION:
        Update update region.  ds_refresh() refreshes this specified region only.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important aspect of using
        the function i.e side effect..etc
==================================================================================================*/
extern void ds_set_update_region 
(
    OP_INT16    left,
    OP_INT16    top,
    OP_INT16    right,
    OP_INT16    bottom
);

/*==================================================================================================
    FUNCTION: ds_refresh_region 

    DESCRIPTION:
        Refresh the specified region immediately. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important aspect of using
        the function i.e side effect..etc
==================================================================================================*/
extern void ds_refresh_region 
(
    OP_INT16            left,
    OP_INT16            top,
    OP_INT16            right,
    OP_INT16            bottom
);


/*==================================================================================================
    FUNCTION: ds_set_refresh_func

    DESCRIPTION:
        Register own drawing and refreshment function when it is SCREEN_OWNERDRAW mode

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important aspect of using
        the function i.e side effect..etc
==================================================================================================*/
extern void ds_set_refresh_func
(
    OP_BOOLEAN (*refresh_func)(OP_UINT32) 
);


/*==================================================================================================
    FUNCTION: ds_clear_refresh_func

    DESCRIPTION:
        Deregister own drawing and refreshment function.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important aspect of using
        the function i.e side effect..etc
==================================================================================================*/
extern void ds_clear_refresh_func
( 
    void 
);


/*==================================================================================================
    FUNCTION: ds_set_disp_annun 

    DESCRIPTION:
        Indicate the annuciator area should be displayed. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None
 ==================================================================================================*/
extern void ds_set_disp_annun
(
    void
);


/*==================================================================================================
    FUNCTION: ds_get_disp_annun 

    DESCRIPTION:
        Return whether the annuciator is displayed or not. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None
 ==================================================================================================*/
extern OP_BOOLEAN ds_get_disp_annun
(
    void
);
 

/*==================================================================================================
    FUNCTION: ds_clear_disp_annun 

    DESCRIPTION:
        Indicate the annuciator area is not displayed. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None
 ==================================================================================================*/
extern void ds_clear_disp_annun
(
    void
);

extern void ds_draw_all_maksed_annuciators
(
    void
);


/**************************************************************************************************
    Drawing Functions
**************************************************************************************************/

/*=================================================================================================
    FUNCTION: ds_get_string_width 

    DESCRIPTION:
        Measure the string widht in pixels with the given attribute.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        Text is unicoded.
==================================================================================================*/
extern OP_UINT32 ds_get_string_width 
(
    OP_UINT8       *text,
    DS_FONTATTR    fontattr
);


/*=================================================================================================
    FUNCTION: ds_draw_char 

    DESCRIPTION:
        Draw a character.It can be used to draw a cursor.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
==================================================================================================*/
extern void ds_draw_char 
(
    OP_INT16       x,
    OP_INT16       y,
    OP_UINT8       *text,
    DS_FONTATTR    fontattr,
    DS_COLOR       fontcolor,
    DS_COLOR       backcolor
);


/*=================================================================================================
    FUNCTION: ds_draw_text 

    DESCRIPTION:
        Draw text in the specified area and position accroding to the given atttibutes. 
        If the text exceeds the end of a line, it wil be auto-wrpped to the next line of the area.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        To draw one line text, use ds_draw_text_line().
==================================================================================================*/
extern void ds_draw_text 
(
    OP_INT16          x,
    OP_INT16          y,
    OP_UINT8          *text,
    DS_FONTATTR       fontattr,
    DS_COLOR          fontcolor,
    DS_COLOR          backcolor
);

/*=================================================================================================
    FUNCTION: ds_draw_text_rm 

    DESCRIPTION:
        Draw text with the given resource Id in the specified area and position accroding to the 
        given atttibutes. 
        If the text exceeds the end of a line, it wil be auto-wrpped to the next line of the area.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        To draw one line text, use ds_draw_text_line().
        Drawing text is done only on text screen.
==================================================================================================*/
extern void ds_draw_text_rm 
(
    OP_INT16            x,
    OP_INT16            y,
    RM_RESOURCE_ID_T    res_id,
    DS_FONTATTR         fontattr,
    DS_COLOR            fontcolor,
    DS_COLOR            backcolor
);


/*==================================================================================================
    FUNCTION: ds_draw_text_line 

    DESCRIPTION:
        Draw text in the specified area and position accroding to the given atttibutes. 
        The text is drawn until it reaches the end of a line and the leftover text is not be drawn.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        To draw text longer than the a line from the staring position, use ds_draw_text().
==================================================================================================*/
extern void ds_draw_text_line 
(
    OP_INT16       x,
    OP_INT16       y,
    OP_INT16       xlimit,
    OP_UINT8       *text,
    DS_FONTATTR    fontattr,
    DS_COLOR       fontcolor,
    DS_COLOR       backcolor
);


/*==================================================================================================
    FUNCTION: ds_draw_text_line_rm 

    DESCRIPTION:
        Draw text with the given resource Id in the specified area and position accroding to the given
        atttibutes. 
        The text is drawn until it reaches the end of a line and the leftover text is not be drawn.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        To draw text longer than the a line from the staring position, use ds_draw_text().
        Drawing text is done only on text screen.
==================================================================================================*/

extern void ds_draw_text_line_rm 
(
    OP_INT16               x,
    OP_INT16               y,
    OP_INT16               xlimit,
    RM_RESOURCE_ID_T    res_id,
    DS_FONTATTR         fontattr,
    DS_COLOR            fontcolor,
    DS_COLOR            backcolor
);

/*==================================================================================================
    FUNCTION: ds_clear_text 

    DESCRIPTION:
        Clear text buffer.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None
==================================================================================================*/
extern void ds_clear_text 
( 
    void 
);


/*==================================================================================================
    FUNCTION: ds_fill_text 

    DESCRIPTION:
        Fill the specified region of the text buffer with the given value.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None
==================================================================================================*/
extern void ds_fill_text 
(
    OP_INT16       startx,
    OP_INT16       starty,
    OP_INT16       endx,
    OP_INT16       endy,
    DS_COLOR       color
);

/*==================================================================================================
    FUNCTION: ds_draw_bitmap_image 

    DESCRIPTION:
        Draw bitmap image to the specified area and region. The bitmap image is the internal 
        formated image for 16bpp.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_draw_bitmap_image 
(
    OP_INT16          x,
    OP_INT16          y,
    RM_BITMAP_T      *bitmap
);

/*==================================================================================================
    FUNCTION: ds_draw_bitmap_image_rm 

    DESCRIPTION:
        Draw bitmap image to the specified area and region. The bitmap image is the internal 
        formated image for 16bpp.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_draw_bitmap_image_rm 
(
    OP_INT16            x,
    OP_INT16            y,
    RM_RESOURCE_ID_T    res_id
);
 

/*==================================================================================================
    FUNCTION: ds_draw_partial_bitmap_image 

    DESCRIPTION:
        Draw bitmap image partially to the specified area and region. The bitmap image is the internal 
        formated image for 16bpp.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_draw_partial_bitmap_image 
(
    OP_INT16           dsx,        /* destination start x coor */
    OP_INT16           dsy,        /* destination start y coor */
    OP_INT16           d_width,  /* destination width to be drawn */
    OP_INT16           d_height,  /* destination height to be drawn */
    OP_INT16           ssx,       /* source bitmap start x coor */
    OP_INT16           ssy,       /* source bitmap start y coor */
    RM_BITMAP_T       *bitmap
);

/*==================================================================================================
    FUNCTION: ds_draw_partial_bitmap_image 

    DESCRIPTION:
        Draw bitmap image partially to the specified area and region. The bitmap image is the internal 
        formated image for 16bpp.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_draw_partial_bitmap_image_rm 
(
    OP_INT16           dsx,        /* destination start x coor */
    OP_INT16           dsy,        /* destination start y coor */
    OP_INT16           d_width,  /* destination width to be drawn */
    OP_INT16           d_height,  /* destination height to be drawn */
    OP_INT16           ssx,       /* source bitmap start x coor */
    OP_INT16           ssy,       /* source bitmap start y coor */
    RM_RESOURCE_ID_T   bitmap_res_id
);

/*==================================================================================================
    FUNCTION: ds_draw_resized_bitmap_image 

    DESCRIPTION:
        Draw bitmap image to be fit into the destination size to the specified area and region. 
        The bitmap image is the internal formated image for 16bpp.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_draw_resized_bitmap_image 
(
    OP_INT16           dx,
    OP_INT16           dy,
    OP_UINT8           d_width,
    OP_UINT8           d_height,
    OP_INT16           sx,
    OP_INT16           sy,
    RM_BITMAP_T       *bitmap
);


/*==================================================================================================
    FUNCTION: ds_draw_resized_bitmap_image_rm 

    DESCRIPTION:
        Draw bitmap image to be fit into the destination size to the specified area and region. 
        The bitmap image is the internal formated image for 16bpp.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_draw_resized_bitmap_image_rm 
(
    OP_INT16           dx,
    OP_INT16           dy,
    OP_UINT8           d_width,
    OP_UINT8           d_height,
    OP_INT16           sx,
    OP_INT16           sy,
    RM_RESOURCE_ID_T  bitmap_res_id
);

/*==================================================================================================
    FUNCTION: ds_draw_icon 

    DESCRIPTION:
        Draw bitmap icon image to the specified area and region. An icon image specifies a 
        transparent color and this color is not drawn.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_draw_icon 
(
    OP_INT16      x,
    OP_INT16      y,
    RM_ICON_T     *icon
);

/*==================================================================================================
    FUNCTION: ds_draw_icon_rm 

    DESCRIPTION:
        Draw bitmap icon image to the specified area and region. An icon image specifies a 
        transparent color and this color is not drawn.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_draw_icon_rm 
(
    OP_INT16            x,
    OP_INT16            y,
    RM_RESOURCE_ID_T    res_id
);
/*==================================================================================================
    FUNCTION: ds_draw_resized_icon 

    DESCRIPTION:
        fit icon image to the specified area and region. An icon image specifies a 
        transparent color and this color is not drawn.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_draw_resized_icon 
(
    OP_INT16           dx,
    OP_INT16           dy,
    OP_UINT8           d_width,
    OP_UINT8           d_height,
    OP_INT16           sx,
    OP_INT16           sy,
    RM_ICON_T       *icon
);

/*==================================================================================================
    FUNCTION: ds_clear_image 

    DESCRIPTION:
        Clear the bitmap buffer.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_clear_image 
( 
    void 
);

/*==================================================================================================
    FUNCTION: ds_get_pixel 

    DESCRIPTION:
        Return the color of the specified point of the given area.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern DS_COLOR ds_get_pixel(
  OP_INT16          pos_x,
  OP_INT16          pos_y
);

/*==================================================================================================
    FUNCTION: ds_put_pixel 

    DESCRIPTION:
        Draw a pixel on the specified point of the given area.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_put_pixel
(
    OP_INT16          pos_x,
    OP_INT16          pos_y,
    DS_COLOR          color
);


/*==================================================================================================
    FUNCTION: ds_draw_line 

    DESCRIPTION:
        Draw a line.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_draw_line
(
    OP_INT16    startx,
    OP_INT16    starty,
    OP_INT16    endx,
    OP_INT16    endy,
    DS_COLOR    color
);


/*==================================================================================================
    FUNCTION: ds_draw_rect 

    DESCRIPTION:
        Draw a rectangle.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_draw_rect 
(
    OP_INT16    left,
    OP_INT16    top,
    OP_INT16    right,
    OP_INT16    bottom,
    DS_COLOR    color
);


/*==================================================================================================
    FUNCTION: ds_fill_rect 

    DESCRIPTION:
        Fill a rectangle.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_fill_rect 
(
    OP_INT16    left,
    OP_INT16    top,
    OP_INT16    right,
    OP_INT16    bottom,
    DS_COLOR    color
);


/*==================================================================================================
    FUNCTION: ds_draw_button_rect 

    DESCRIPTION:
        Draw a button style rectangle.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_draw_button_rect 
(
  OP_INT16    left,
  OP_INT16    top,
  OP_INT16    right,
  OP_INT16    bottom,
  DS_COLOR    left_top,
  DS_COLOR    right_bottom
);



/*==================================================================================================
    FUNCTION: ds_draw_button 

    DESCRIPTION:
       Draw filled a button style rectangle.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_draw_button 
(
    OP_INT16    left,
    OP_INT16    top,
    OP_INT16    right,
    OP_INT16    bottom,
    DS_COLOR    top_left_color,
    DS_COLOR    right_bottom_color,
    DS_COLOR    fill_color
);


/*==================================================================================================
    FUNCTION: ds_draw_title 

    DESCRIPTION:
       Draw title regardless screen mode.
       Do not use this in case of SCREEN_LIST, SCREEN_MENU, SCREEN_FUNC and SCREEN_FUNC_BG.
       In those case, use ds_set_title.

       The title background image style is normal one.
       
    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_draw_title 
(
    OP_UINT8                *title,
    DS_SCRN_TITLE_ENUM_T    title_type     
);

/*==================================================================================================
    FUNCTION: ds_draw_title_rm 

    DESCRIPTION:
       Draw title regardless screen mode.
       Do not use this in case of SCREEN_LIST, SCREEN_MENU, SCREEN_FUNC and SCREEN_FUNC_BG.
       In those case, use ds_set_title.

       The title background image style is normal one.
       
    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_draw_title_rm 
(
    RM_RESOURCE_ID_T        title_res_id,
    DS_SCRN_TITLE_ENUM_T    title_type     
);

/*==================================================================================================
    FUNCTION: ds_get_image_size 

    DESCRIPTION:
       Return given image (bitmap or icon) size (witdh and height).

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern OP_BOOLEAN ds_get_image_size 
(
    RM_RESOURCE_ID_T    res_id,
    OP_UINT16            *width,
    OP_UINT16            *height
);

/*==================================================================================================
    FUNCTION: ds_get_image_width 

    DESCRIPTION:
       Return given image (bitmap or icon) witdh.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern OP_UINT16 ds_get_image_width 
(
    RM_RESOURCE_ID_T    res_id
);

/*==================================================================================================
    FUNCTION: ds_get_image_height 

    DESCRIPTION:
       Return given image (bitmap or icon) height.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern OP_UINT16 ds_get_image_height 
(
    RM_RESOURCE_ID_T    res_id
);


/**************************************************************************************************
    Animation Control Functions
**************************************************************************************************/

/*==================================================================================================
    FUNCTION: ds_stop_animation 

    DESCRIPTION:
       Stop an active animaton.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_stop_animation 
(
    DS_ANI_T *ani
);
/*==================================================================================================
    FUNCTION: ds_resume_paused_animation

    DESCRIPTION:
        Resume the paused animation.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important aspect of using
        the function i.e side effect..etc
==================================================================================================*/

extern void ds_resume_paused_animation
( 
    void
);
/*==================================================================================================
    FUNCTION: ds_pause_current_animation

    DESCRIPTION:
        Pause the current animation.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important aspect of using
        the function i.e side effect..etc
==================================================================================================*/

extern void ds_pause_current_animation
( 
    void
);
/*==================================================================================================
    FUNCTION: ds_pause_animation 

    DESCRIPTION:
       Pause an active animiation.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_pause_animation 
(
    DS_ANI_T *ani
);


/*==================================================================================================
    FUNCTION: ds_resume_animation 

    DESCRIPTION:
       Resume a paused animiation.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_resume_animation 
(
    DS_ANI_T *ani
);

/*==================================================================================================
    FUNCTION: ds_start_animation 

    DESCRIPTION:
       start a new icon animaiton. 

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern DS_ANI_T * ds_start_animation
(
    OP_INT16            x,
    OP_INT16            y,
    RM_RESOURCE_ID_T    res_id,
    OP_UINT8            repeat,
    OP_UINT32           delay 
);

/*==================================================================================================
    FUNCTION: ds_start_resized_animaton 

    DESCRIPTION:
       Start an resized icon animaton fit to dest buffer.

    ARGUMENTS PASSED:
       repeat_num - number of repeat. To repeat it until it is stopped, set DS_ANI_CONTINUOUSLY. 
                    Maximum repeat numer is 254.
                    If 255 is given, the animation will not be started.
       delay      - time delay in milisecond between frames.

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern DS_ANI_T * ds_start_resized_animaton
(
    OP_INT16            dx,
    OP_INT16            dy,
    OP_UINT8            d_width,
    OP_UINT8            d_height,
    OP_INT16            sx,
    OP_INT16            sy,
    RM_RESOURCE_ID_T    res_id,
    OP_UINT8            repeat,
    OP_UINT32           delay 
);

/*==================================================================================================
    FUNCTION: ds_play_animation 

    DESCRIPTION:
       play an icon animaton .

    ARGUMENTS PASSED:
       repeat_num - number of repeat. To repeat it until it is stopped, set DS_ANI_CONTINUOUSLY. 
                    Maximum repeat numer is 254.
                    If 255 is given, the animation will not be started.
       delay      - time delay in milisecond between frames.

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern DS_ANI_STATUS_ENUM_T ds_play_animation 
(
    DS_ANI_T *ani
);

/**************************************************************************************************
    Softkey Control Functions
**************************************************************************************************/

/*==================================================================================================
    FUNCTION: ds_init_softkeys 

    DESCRIPTION:
       Initialize softkeys.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_init_softkeys 
( 
    void
);

/*==================================================================================================
    FUNCTION: ds_set_softkeys_rm 

    DESCRIPTION:
       Set predefined softkeys.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_set_softkeys_rm 
(
    RM_RESOURCE_ID_T    left_res_id,
    RM_RESOURCE_ID_T    center_res_id,
    RM_RESOURCE_ID_T    right_res_id
);

/*==================================================================================================
    FUNCTION: ds_set_softkeys_char 

    DESCRIPTION:
       Set the given characters as softkey string.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Upto 6 English and 3 Chiness characters are allowed per a softkey.
==================================================================================================*/
extern void ds_set_softkeys_char 
(
    OP_UINT8     *left,
    OP_UINT8     *center,
    OP_UINT8     *right
);

/*==================================================================================================
    FUNCTION: ds_set_softkeys_icon 

    DESCRIPTION:
       Set the given icons as a softkey icon. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        This icon is draw in center aligned in the specified softkey.
        This icon can be placed in front of the the softkey string if there exists specified sting. 
==================================================================================================*/
extern void ds_set_softkeys_icon 
(
    RM_ICON_T    *left,
    RM_ICON_T    *center,
    RM_ICON_T    *right
);

/*==================================================================================================
    FUNCTION: ds_set_softkeys_icon_rm 

    DESCRIPTION:
       Set the given icons as a softkey icon. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        This icon is draw in center aligned in the specified softkey.
        This icon can be placed in front of the the softkey string if there exists specified sting. 
==================================================================================================*/
extern void ds_set_softkeys_icon_rm 
(
    RM_RESOURCE_ID_T    left_res_id,
    RM_RESOURCE_ID_T    center_res_id,
    RM_RESOURCE_ID_T    right_res_id

);

void ds_set_softkeys_bg
(
    void
);
/*==================================================================================================
    FUNCTION: ds_set_softkey_caption 

    DESCRIPTION:
       Set the softkey canption. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_set_softkey_caption 
(
    OP_UINT8 *caption
);

/*==================================================================================================
    FUNCTION: ds_set_softkey_caption_rm 

    DESCRIPTION:
       Set the softkey canption. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_set_softkey_caption_rm 
(
    RM_RESOURCE_ID_T caption_res_id
);

/*==================================================================================================
    FUNCTION: ds_draw_any_softkey 

    DESCRIPTION:
        Draw a sofkkey according to the specified attribute. No pre-defined rule is assumed.

    ARGUMENTS PASSED:
        key - which softkey
        attr - softkey attribute
        x - x position to display text
        *text - text string
        length - text length: if -1, length is calculated automatically
        *icon - icon data pointer
        fgcolor - foreground(text) color
        bgcolor    - background color

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_draw_any_softkey 
(
    DS_SKEYPOS_ENUM_T    key,    
    DS_SOFTKEYATTR       attr,    
    OP_INT16              x,                
    OP_UINT8              *text,    
    OP_INT8              length,    
    RM_ICON_T            *icon,        
    DS_COLOR             fgcolor,        
    DS_COLOR             bgcolor        
);

/*==================================================================================================
    FUNCTION: ds_draw_any_softkey_rm 

    DESCRIPTION:
        Draw a sofkkey according to the specified attribute. No pre-defined rule is assumed.

    ARGUMENTS PASSED:
        key - which softkey
        attr - softkey attribute
        x - x position to display text
        RM_RESOURCE_ID_T - text resource
        length - text length: if -1, length is calculated automatically
        *icon - icon data pointer
        fgcolor - foreground(text) color
        bgcolor    - background color

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_draw_any_softkey_rm 
(
    DS_SKEYPOS_ENUM_T     key,    
    DS_SOFTKEYATTR        attr,    
    OP_INT16              x,                
    RM_RESOURCE_ID_T      text_res_id,    
    OP_INT8               length,    
    RM_ICON_T             *icon,        
    DS_COLOR              fgcolor,        
    DS_COLOR              bgcolor        
);


/**************************************************************************************************
    Small Drawing Widgets
**************************************************************************************************/


/*==================================================================================================
    FUNCTION: ds_draw_text_win_frame 

    DESCRIPTION:
        Draw window frame to the txt screen for text mode.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_draw_text_win_frame 
( 
    void
);


/*==================================================================================================
    FUNCTION: ds_draw_bg_win_frame 

    DESCRIPTION:
        Draw window frame to the bitmap screen for bitmap mode.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_draw_bg_win_frame 
( 
    void
);


/*==================================================================================================
    FUNCTION: ds_draw_popup_win_frame 

    DESCRIPTION:
        Draw popup window frame to the popup screen for popup.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_draw_popup_win_frame 
( 
    DS_RECT_T rect
);


/**************************************************************************************************
    Annuciator Control Functions
**************************************************************************************************/

/*==================================================================================================
    FUNCTION: ds_annuciator 

    DESCRIPTION:
       Set handset annunciators on or off

    ARGUMENTS PASSED:
       mask - mask of annunciators
       value - annunciator values to mask in

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_set_annuciator
(
    DS_ANNUN_ENUM_T    annuciator,                    
    OP_BOOLEAN         on_off,
    OP_BOOLEAN         blink                     
);

/*==================================================================================================
    FUNCTION: ds_set_rssi 

    DESCRIPTION:
       Set RSSI annunciator bar. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_set_rssi
(
    OP_UINT8       val,
    OP_BOOLEAN     blink
);

/*==================================================================================================
    FUNCTION: ds_set_rssi 

    DESCRIPTION:
       Set Battery annunciator bar. 

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
extern void ds_set_batt
(
    OP_UINT8       val,
    OP_BOOLEAN     blink
);


/**************************************************************************************************
   General Title Control Functions - Title is used in Menu, List, and Text Lines and Graphic Items
**************************************************************************************************/
extern void ds_set_title 
(
    OP_UINT8                *title,
    DS_SCRN_TITLE_ENUM_T    title_type     
);

extern void  ds_set_title_rm 
(
    RM_RESOURCE_ID_T        title_res_id,
    DS_SCRN_TITLE_ENUM_T    title_type     
);

extern void ds_set_title_bg 
(
    RM_BITMAP_T    *title_bg     
);

extern void ds_set_title_bg_rm 
(
    RM_RESOURCE_ID_T    title_bg_res_id     
);

extern void ds_set_title_draw_func 
(
    OP_UINT8    (*draw_func)(OP_UINT8, OP_UINT32) 
);

extern void ds_set_title_icon 
( 
    RM_ICON_T    *icon 
);

extern void ds_set_title_icon_rm
( 
    RM_RESOURCE_ID_T    icon_res_id
);

extern void ds_clear_title 
( 
    void
);



/**************************************************************************************************
   Text Line Control Functions
**************************************************************************************************/
extern OP_UINT8 *ds_get_line 
( 
    OP_UINT8     line 
);

extern void ds_set_line 
( 
    OP_UINT8    line, 
    OP_UINT8    *data 
);

extern void ds_set_line_rm 
( 
    OP_UINT8            line, 
    RM_RESOURCE_ID_T    txt_res_id 
);

extern void ds_add_line 
( 
    OP_UINT8    *data 
);

extern void ds_add_line_rm 
( 
    RM_RESOURCE_ID_T    txt_res_id 
);

extern void ds_del_line 
( 
    OP_UINT8    line 
);

extern void ds_clear_lines 
( 
    void 
);

extern void ds_clear_item 
( 
     OP_UINT8       item 
);

extern void ds_clear_items 
( 
    void 
);

extern void ds_set_line_icon 
(
    OP_UINT8     line,
    RM_ICON_T    *icon
);

extern void ds_set_line_icon_rm 
(
    OP_UINT8           line,
    RM_RESOURCE_ID_T   icon_res_id
);

extern void ds_set_line_bitmap 
(
    OP_UINT8       line,
    RM_BITMAP_T    *bitmap
);

extern void ds_set_line_bitmap_rm 
(
    OP_UINT8           line,
    RM_RESOURCE_ID_T   bitmap_res_id
);

extern void ds_set_line_draw_func 
( 
    OP_UINT8    line, 
    OP_UINT8    (*draw_func)(OP_UINT8, OP_UINT32) 
);

extern OP_UINT32 ds_get_line_data 
( 
    OP_UINT8     line 
);

extern void ds_set_line_data 
( 
     OP_UINT8      line, 
     OP_UINT32     data 
);

extern DS_LINEATTR ds_get_lineattr 
( 
     OP_UINT8     line 
);

extern void ds_set_lineattr 
( 
     OP_UINT8        line, 
     DS_LINEATTR     attr 
);

extern void ds_update_line 
( 
     OP_UINT8     line 
);

extern void ds_update_all_lines 
( 
     void
);

extern void ds_set_selection 
( 
     OP_UINT8     line 
);

extern void ds_set_line_number 
( 
     OP_UINT8    line,
     OP_UINT8    line_number 
);

extern void ds_set_check 
( 
     OP_UINT8       line, 
     OP_BOOLEAN     check 
);

extern OP_UINT8 ds_get_highlight // ui_get_cursor
( 
     void 
);

extern void ds_set_highlight //ui_set_cursor 
( 
     OP_UINT8       line, 
     OP_BOOLEAN     on 
);

extern void ds_set_char_cursor // ui_set_bar_cursor
(
     OP_UINT8    offset, 
     OP_UINT8    line, 
     DS_FONTATTR attr,
     OP_BOOLEAN  blink,
     DS_COLOR    fontcolor, 
     DS_COLOR    backcolor, 
     OP_UINT8    *cursor
);

extern void ds_deact_char_cursor 
(
     void
);


/**************************************************************************************************
   Graphic Item Control Functions
**************************************************************************************************/
extern DS_GITEMATTR ds_get_item_attr 
( 
     OP_UINT8     item 
);

extern void ds_set_item_attr 
( 
     OP_UINT8         item, 
     DS_GITEMATTR     attr 
);

extern void ds_set_bitmap_item 
( 
     OP_UINT8        item, 
     RM_BITMAP_T     *bitmap 
);

extern void ds_set_bitmap_item_rm 
( 
     OP_UINT8            item, 
     RM_RESOURCE_ID_T    bitmap_res_id
);

extern void ds_set_icon_item 
( 
     OP_UINT8      item, 
     RM_ICON_T     *icon 
);

extern void ds_set_icon_item_rm 
( 
     OP_UINT8            item, 
     RM_RESOURCE_ID_T    icon_res_id 
);

extern void ds_set_func_item 
( 
     OP_UINT8       item, 
     OP_BOOLEAN    (*draw_func)(OP_UINT8 index, OP_UINT32 data)
);

extern void ds_set_item_region 
( 
    OP_UINT8    item, 
    OP_INT16    left, 
    OP_INT16    top, 
    OP_INT16    right, 
    OP_INT16    bottom 
);

extern void ds_set_item_data 
( 
    OP_UINT8     item, 
    OP_UINT32    data 
);

extern void ds_update_item 
( 
    OP_UINT8    item 
);

extern void ds_refresh_item 
( 
    OP_UINT8    item 
);


/**************************************************************************************************
      SCREEN MODE CONTROL FUNCTIONS
**************************************************************************************************/
extern DS_SCRNMODE_ENUM_T ds_get_screen_mode 
( 
    void 
);

extern DS_SCRNMODE_ENUM_T ds_set_screen_mode 
( 
    DS_SCRNMODE_ENUM_T scrn 
);

/* returns the maximum number of lines according to the current screen mode */
extern OP_UINT8 ds_get_max_lines 
( 
    void
);


extern OP_UINT8 ds_get_line_height 
( 
    void
);

extern DS_LINE_Y_COOR_T ds_get_line_y 
( 
    OP_UINT8 line
);

extern OP_INT16 ds_get_line_text_x 
( 
    OP_UINT8 line
);

extern OP_UINT16 ds_get_font_bg 
( 
    void
);

extern void ds_save_region 
( 
    OP_UINT16    *dest_buf,
    OP_INT16     left,
    OP_INT16     top,
    OP_INT16     right,
    OP_INT16     bottom
);

extern void ds_set_screen_side_margines 
( 
    OP_INT16 left_margine,
    OP_INT16 right_margine
);

extern DS_COLORTHEME_ENUM_T ds_get_color_theme
(
    void
);

extern void ds_set_color_theme
(
    DS_COLORTHEME_ENUM_T    color_theme
);


extern void ds_draw_focus_bar
(
    OP_INT16                y_top,
    OP_INT16                y_bottom
);

extern DS_COLOR ds_get_focus_bar_color
(
    void
);

extern void ds_set_lcd_contrast
(
    OP_UINT8    contrast_value
);

extern void ds_draw_analog_clock
( 
    OP_INT16              x_center,
    OP_INT16              y_center,
    OP_INT16              h_len,
    OP_INT16              m_len,
    DS_COLOR              h_color,
    DS_COLOR              s_color,
    OP_UINT8              hour,  
    OP_UINT8              minute  
);


/*==================================================================================================
    FUNCTION: ds_refresh_lines_items 

    DESCRIPTION:
       It draws and refreshes as pre-defined attributes for line and item concept.
       It is called if the screen mode is one of SCREEN_LIST, SCREEN_MENU, or SCREEN_FUNC.
       
    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        If it refreshes partial screen, returns FALSE. 
        If whole screen refrshment is required, returns TRUE so that the screen is refreshed at 
        once.
==================================================================================================*/
OP_BOOLEAN ds_refresh_lines_items
(
    void
);


/*==================================================================================================
    FUNCTION: ds_refresh 

    DESCRIPTION:
       Refresh according to the screen mode.
       
    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void ds_refresh
( 
    void 
);

/* Conexant Simulator test phone */
DS_RECT_T ds_get_refresh_rect(void);

/*
 *	draw popup scrollbar
 */
extern void ds_set_popup_scrollbar(
                                 OP_UINT8    totalItems,    
                                 OP_UINT8    firstItem,    
                                 OP_UINT8    pageItems,
                                 OP_BOOLEAN  bCal
                                 );


extern OP_BOOLEAN ds_parse_image
(
    RM_BITMAP_T      *pBitmap, 
    OP_UINT8          *imageData,
    OP_UINT32         datasize,
    RM_IMAGE_FORMAT   format
);

extern OP_BOOLEAN ds_get_file_image_size
(
    RM_IMAGE_FORMAT format,
    OP_UINT8         *image,
    OP_UINT32        data_size,
    OP_UINT16        *pWidth,
    OP_UINT16        *pHeight
);
extern void ds_draw_wbmp_image 
(
    SCRNAREA_ENUM_T  area,
    OP_INT16           dsx,        /* destination start x coor */
    OP_INT16           dsy,        /* destination start y coor */
    OP_INT16           d_width,  /* destination width to be drawn */
    OP_INT16           d_height,  /* destination height to be drawn */
    OP_INT16           ssx,       /* source bitmap start x coor */
    OP_INT16           ssy,       /* source bitmap start y coor */
    OP_UINT8          *src_data
);
extern OP_BOOLEAN ds_need_stop_anun_blinktimer(void);

extern void ds_refresh_softkey_rectangle(void);
extern void ds_refresh_text_rectangle(LCD_RECT_T u_rect);
extern void ds_refresh_for_wap_mms(void);
extern void ds_refresh_text_area
(  OP_UINT16    *res_buf,
   OP_INT16     top,
   OP_INT16     bottom
 );
extern void ds_save_text_region
( 
 OP_UINT16    *dest_buf,
 OP_INT16     top,
 OP_INT16     bottom
 );
extern void ds_update_text_region
(
 OP_INT16     top,
 OP_INT16     bottom
 );

extern int ds_refresh_lcd_rect
(
    OP_UINT8 x_start, 
    OP_UINT8 y_start, 
    OP_UINT8 x_end, 
    OP_UINT8 y_end
);

 void refresh_annuciator
(
    void 
);

OP_BOOLEAN ds_get_composed_color
(
    DS_COLOREDCOMPOSER_T  *pCompose 
);

/*==================================================================================================
FUNCTION: ds_set_extend_softkey
   
DESCRIPTION:
			set extend softkey area.        
ARGUMENTS PASSED:
RETURN VALUE:
        
IMPORTANT NOTES:
==================================================================================================*/
void ds_set_extend_softkey(OP_BOOLEAN bExtend);
/*==================================================================================================
FUNCTION: ds_get_extend_softkey
   
DESCRIPTION:
			get if display extend softkey area.        
ARGUMENTS PASSED:
RETURN VALUE:
        OP_TRUE: use the extend softkey area.
        OP_FALSE: without use extend softkey area.
        
IMPORTANT NOTES:
==================================================================================================*/
OP_BOOLEAN ds_get_extend_softkey(void);

/*==================================================================================================
    FUNCTION: ds_draw_region 

    DESCRIPTION:
        Draw a cursor on the screen( in editor).
        Wrote by ZhangXiaoDong
       
         ARGUMENTS PASSED:
         OP_UINT8     Top_x:The x position of the top
         OP_UINT8     Top_y:The y position of the top
         OP_UINT8     Bottom_x:The y position of the bottom
         OP_UINT8     Bottom_y:The y position of the start bottom
         OP_UINT8     limit_y: The max Y value of cursor point.
        
    RETURN VALUE:
        none.
    IMPORTANT NOTES:
        
 =================================================================================================*/
extern void ds_set_reverse
(
 OP_UINT16     Left,
 OP_UINT16     Top,
 OP_UINT16     Right,
 OP_UINT16     Bottom
 );

extern OP_INT8 ds_decode_jpg_by_hardware
(
    OP_UINT8 *pData,
    OP_UINT32 filesize,
    RM_BITMAP_T *dest
);

extern OP_INT8 ds_put_jpeg_image_on_lcd_directly
(
    OP_INT16 x,
    OP_INT16 y,
    OP_UINT8 *pData,
    OP_UINT32 filesize,
    IMAGE_RESIZE zoom
);

/*================================================================================================*/
#endif  /* DS_DRAWING_H */
