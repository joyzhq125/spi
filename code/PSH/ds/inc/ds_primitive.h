#ifndef DS_PRIMITIVE_H
#define DS_PRIMITIVE_H
/*==================================================================================================

    HEADER NAME : ds_primitive.h

    GENERAL DESCRIPTION
        This file includes drawing and refreshing primitives.

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    06/15/2002   Ashley Lee       crxxxxx     Initial Creation
    02/24/2002   wangwei          PP00117     Change draw bitmap to draw icon when play animation.
    05/15/2003   linda wang       P000289     support 256 color.
    06/14/2003   lindawang        P000474     Support play gif animation and fix some bugs.
    This file includes drawing and refreshing  primitives.

====================================================================================================
    INCLUDE FILES
==================================================================================================*/

#include    "ds_def.h"
#include    "ds_int_def.h"

/*==================================================================================================
    CONSTANTS
==================================================================================================*/
extern const OP_UINT16 scramble_color[];

/*==================================================================================================
    MACROS
==================================================================================================*/
#define UPDATEAREA()           ( ds.update_area )
#define UPDATESET( area )      ( ds.update_area |= area )
#define UPDATECLEAR( area )    ( ds.update_area &= ~(area) )
#define UPDATESET_POPUP()      ( ds.update_area |= AREAMASK_POPUP )
#define UPDATECLEAR_POPUP()    ( ds.update_area &= ~(AREAMASK_POPUP) )

#define UPDATERECT()           ( ds.rcUpdate )

/*==================================================================================================
    DEFINITIONS
==================================================================================================*/

/*==================================================================================================
    FUNCTION PROTOTYPES
==================================================================================================*/
extern void set_update_region
(
    OP_INT16    left,
    OP_INT16    top,
    OP_INT16    right, 
    OP_INT16    bottom
);

extern void set_popup_update_region
(
    OP_INT16    left,
    OP_INT16    top,
    OP_INT16    right, 
    OP_INT16    bottom
);

extern  void clear_update_region
(
    void
);


extern void refresh_rectangle 
(
    DS_RECT_T
);


extern void refresh_to_lcd
(
    void
);

extern long double op_sin
(
    OP_INT16 degree
);

extern long double op_cos
(
    OP_INT16 degree
);

extern void add_update_region 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           left,
    OP_INT16           top,
    OP_INT16           right,
    OP_INT16           bottom
);

extern void fill_rect 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           left,
    OP_INT16           top,
    OP_INT16           right,
    OP_INT16           bottom,
    DS_COLOR           color
);

extern OP_INT16 draw_text_line 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           x,
    OP_INT16           y,
    OP_INT16           xlimit,
    OP_UINT8           *text,
    OP_BOOLEAN         is_one_line_text,   /* is this the first line of the string */
    DS_FONTATTR        fontattr,
    DS_COLOR           fontcolor,
    DS_COLOR           backcolor    
);

void draw_text_frame
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           x,
    OP_INT16           y,
    OP_INT16           xlimit,
    OP_UINT8           *title,
    OP_BOOLEAN         is_one_line_text,   /* is this the first line of the string */
    DS_FONTATTR        fontattr,
    DS_COLOR           fontcolor,
    DS_COLOR           backcolor    
);

extern void draw_text 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           x,
    OP_INT16           y,
    OP_INT16           xlimit,
    OP_UINT8           *text,
    DS_FONTATTR        fontattr,
    DS_COLOR           fontcolor,
    DS_COLOR           backcolor
);


extern void draw_char 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16       x,
    OP_INT16       y,
    OP_UINT8       *text,
    DS_FONTATTR    fontattr,
    DS_COLOR       fontcolor,
    DS_COLOR       backcolor
);

extern void draw_bitmap_image 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           x,
    OP_INT16           y,
    RM_BITMAP_T        *bitmap
);

extern void draw_partial_bitmap_image 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           dsx,        /* destination start x coor */
    OP_INT16           dsy,        /* destination start y coor */
    OP_INT16           d_width,  /* destination width to be drawn */
    OP_INT16           d_height,  /* destination height to be drawn */
    OP_INT16           ssx,       /* source bitmap start x coor */
    OP_INT16           ssy,       /* source bitmap start y coor */
    RM_BITMAP_T       *bitmap
);

extern void draw_resized_bitmap_image 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           dx,
    OP_INT16           dy,
    OP_UINT8           d_width,
    OP_UINT8           d_height,
    OP_INT16           sx,
    OP_INT16           sy,
    RM_BITMAP_T       *bitmap
);

extern void draw_icon 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           x,
    OP_INT16           y,
    RM_ICON_T          *icon
);

extern void draw_partial_icon 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           dsx,        /* destination start x coor */
    OP_INT16           dsy,        /* destination start y coor */
    OP_INT16           d_width,  /* destination width to be drawn */
    OP_INT16           d_height,  /* destination height to be drawn */
    OP_INT16           ssx,       /* source icon start x coor */
    OP_INT16           ssy,       /* source icon start y coor */
    RM_ICON_T         *icon
);

extern void draw_resized_icon 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           dsx,         /* starting x coor in the destination */
    OP_INT16           dsy,         /* starting y coor in the destination */
    OP_UINT8           dw,          /* width of the destination */
    OP_UINT8           dh,          /* height of the destination */
    OP_INT16           ssx,         /* starting x coor in the source */      
    OP_INT16           ssy,         /* starting y coor in the source */
    RM_ICON_T         *icon      /* icon data */
);

extern DS_COLOR get_pixel
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           pos_x,
    OP_INT16           pos_y
);

extern void put_pixel
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           pos_x,
    OP_INT16           pos_y,
    DS_COLOR           color
);

extern void draw_line
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           startx,
    OP_INT16           starty,
    OP_INT16           endx,
    OP_INT16           endy,
    DS_COLOR           color
);

extern void draw_rect 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           left,
    OP_INT16           top,
    OP_INT16           right,
    OP_INT16           bottom,
    DS_COLOR           color
);

extern void draw_button_rect 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           left,
    OP_INT16           top,
    OP_INT16           right,
    OP_INT16           bottom,
    DS_COLOR           left_top,
    DS_COLOR           right_bottom
);

extern void draw_button 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           left,
    OP_INT16           top,
    OP_INT16           right,
    OP_INT16           bottom,
    DS_COLOR           top_left_color,
    DS_COLOR           right_bottom_color,
    DS_COLOR           fill_color
);

extern void draw_title 
(
    OP_UINT8                *title,
    RM_RESOURCE_ID_T      title_bg_res_id
);

extern void draw_softkeys 
( 
    DS_SKEYSET_T       *skey_ptr,
    DS_SKEYATTR_T      skey_attr
);

extern void draw_softkey_caption 
( 
    OP_UINT8    *caption 
);

extern void clear_annuciator_buffer
(
    void
);

extern void draw_annunciator
(
    DS_ANNUN_ENUM_T    annun,
    OP_BOOLEAN         onoff
);

extern void draw_analog_clock
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

extern void copy_lcd_region
(
   OP_UINT16    *dest_buf,
   OP_INT16     left,
   OP_INT16     top,
   OP_INT16     right,
   OP_INT16     bottom
);

extern void save_animation_icon_bg
(
    SCRNAREA_ENUM_T    area,
    OP_INT16     left,
    OP_INT16     top,
    OP_INT16     right,
    OP_INT16     bottom
);

extern void set_animation_icon_bg
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           left,
    OP_INT16           top,
    OP_INT16           right,
    OP_INT16           bottom
);

/*================================================================================================*/
#endif  /* DS_PRIMITIVE_H */
