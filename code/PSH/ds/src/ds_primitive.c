#ifdef __cplusplus
extern "C" {
#endif
/*=================================================================================================

    MODULE NAME : ds_primitive.c

    GENERAL DESCRIPTION
         This file includes drawing and refreshing  primitives.

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
=================================================================================================
 
    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    06/22/2002   Ashley Lee       crxxxxx     Initial creation
    01/17/2003   Steven Lai       PP00049        Correct a bug in op_sin() and op_cos(). Now they can handle the degree less than 0 or larger than 360
    01/22/2003   Chenyu           PR  -   PP00025
    02/08/2003   Steven Lai       PP00073        Update the draw_icon(), draw_bitmap_image() function to handle the negative coordinations
    02/24/2002   wangwei          PP00117     Change draw bitmap to draw icon when play animation.
                                              - add draw_resized_icon function
    02/27/2003   Chenyu           PR    ----    PP00150 
                                  -Decompress font data 
    03/26/2003   lindawang        C000018     Set new pacific image for the first version.
    04/08/2003   lindawang        P000049     adjust the sublcd orientation.
                                              - remove warnings.
    04/30/2003   linda wang       P000202     Fix bug 1026
    05/15/2003   linda wang       P000289     support 256 color and extend the image size from UINT8 to UINT16.
    06/14/2003   lindawang        P000474     Support play gif animation and fix some bugs.
    06/25/2003   lindawang        P000687     fix the announciator bug and draw softkey seperatly.
    08/04/2003   Chenyu           P001129     Add MMS icon
    08/14/2003   lindawang        P001241     Fix some ds and gif decode problems.

    07/26/2003   lindawang        P001074     use a small buffer for save animation bg instead of animal_buffer.
    11/24/2003   Zhangxiaodong    P002103     fix bug for function copy_lcd_region
    12/04/2003   linda wang       p002147     Add qq and push image
    01/07/2004   steven           P002325   adjust font for dolphin
    02/17/2004   Zhuxq             P002492      Add Viewer window to widget system
    03/04/2004   lindawang        C002586     Add open window function and modify annun layout.
    03/12/2004   gongxiaofeng     P002645     Chang point-by-point writing to line-by-line and Drawing with parm which is negative.
    03/11/2004   steven li        P002617     Add color theme
    03/12/2004   linda wang       P002636     Fix bug for draw game image, which used vice_value.
                                              - but we still need consider optimize the draw image method.
    03/15/2004 chouwangyun    C002655       add idle and mainmenu to odin
    03/16/2004   lindawang        C002667     Mofify drawing dialing font.
    03/20/2004   quzongqin        P002652     Add user define color theme.
    03/30/2004   Steven Lai        P002795     Fix the bug that the softkey area will disappear if the idle image is animation
    04/04/2004   chouwangyun   p002825     change font for odin submenu
    05/05/2004   lindawang       c005261      Fix bug for power on/off animation and announciator display wrong..
    04/14/2004   chouwangyun   c003005     change submenu style
    04/19/2004   lindawang        P003037     Add softkey extend area function.    
    04/29/2004   lindawang        P005196     Fix bug for draw softkey.    
    05/19/2004  chouwangyun     c005496    modify DS module on new odin base 
    06/04/2004   lindawang        P005612     Speeden idle screen display.    
    06/08/2004  liren                 P005997    modify draw_openwindow_annun function
    06/11/2004  liren                 P006020    fix bugs for  announciator display disorder
    06/21/2004  liren             p006345   modify the title display position to left
    
    06/15/2004   liuqi           c006194       adjust the display of annunciator 
    06/23/2004   liren           c006348       change skey color for whale
    06/30/2004   liren           c006583     change openwindow display
    07/08/2004   liren           c006711     change submenu style for Odin
    07/12/2004   liren          p006927    add openwindow animation
    07/19/2004   liren          p007060    adjust openwindow animation
    07/30/2004   gongxiaofeng      p007403    clear annunciator on power on status
    09/07/2004   gongxiaofeng     c008167    resolve a problem that text can't be displayed when focus color is changed
  This file includes drawing and refreshing  primitives.
        
====================================================================================================
    INCLUDE FILES
==================================================================================================*/
#ifdef WIN32
#include    "windows.h"
#include    "portab_new.h"
#else
#include    "portab.h"
#endif

#include    "rm_include.h"
#include    "SP_sysutils.h"

#include    "ds_def.h"
#include    "ds_int_def.h"
#include    "ds_lcd.h"
#include    "ds_util.h"
#include    "ds_font.h"
#include    "ds_popup.h"
#include    "ds_primitive.h"

/*==================================================================================================
    LOCAL CONSTANTS
==================================================================================================*/
/* announciator width*/
static ann_item_width[8] = {22, 22, 22, 22, 22, 22, 22, 22};
#define ann_1_start_x             0

#define NUM_BITS_IN_BYTE           8
#define CLK_CENTER_OFFSET          4
#if ((PROJECT_ID == WHALE1_PROJECT)||(PROJECT_ID == WHALE2_PROJECT)||(PROJECT_ID == NEMO_PROJECT))
#define TITLE_START_X_POS          5
#endif

#define ann_2_start_x     (ann_1_start_x + ann_item_width[0])
#define ann_3_start_x     (ann_2_start_x + ann_item_width[1])
#define ann_4_start_x     (ann_3_start_x + ann_item_width[2])
#define ann_5_start_x     (ann_4_start_x + ann_item_width[3])
#define ann_6_start_x     (ann_5_start_x + ann_item_width[4])
#define ann_7_start_x     (ann_6_start_x + ann_item_width[5])
#define ann_8_start_x     (ann_7_start_x + ann_item_width[6])
#define ann_end_x         (ann_8_start_x + ann_item_width[7])

#define TITLE_ANI_ICON_WIDTH       41
#define TITLE_ANI_ICON_HEIGHT      32
static OP_UINT16    ani_bg[TITLE_ANI_ICON_WIDTH][TITLE_ANI_ICON_HEIGHT];  /* used for save bg of menu icon title animation */   

const double SIN_TABLE[91] = {
0.0000, 0.0175, 0.0349, 0.0523, 0.0698, 0.0872, 0.1045, 0.1219, 0.1392, 0.1564, 
0.1736, 0.1908, 0.2079, 0.2250, 0.2419, 0.2588, 0.2756, 0.2924, 0.3090, 0.3256, 
0.3420, 0.3584, 0.3746, 0.3907, 0.4067, 0.4226, 0.4384, 0.4540, 0.4695, 0.4848, 
0.5000, 0.5150, 0.5299, 0.5446, 0.5592, 0.5736, 0.5878, 0.6018, 0.6157, 0.6293, 
0.6428, 0.6561, 0.6691, 0.6820, 0.6947, 0.7071, 0.7193, 0.7314, 0.7431, 0.7547, 
0.7660, 0.7771, 0.7880, 0.7986, 0.8090, 0.8192, 0.8290, 0.8387, 0.8780, 0.8572, 
0.8660, 0.8746, 0.8829, 0.8910, 0.8988, 0.9063, 0.9135, 0.9205, 0.9272, 0.9336, 
0.9397, 0.9455, 0.9511, 0.9563, 0.9613, 0.9659, 0.9703, 0.9744, 0.9781, 0.9816, 
0.9848, 0.9877, 0.9903, 0.9925, 0.9945, 0.9962, 0.9976, 0.9986, 0.9994, 0.9998,1.0000
};

const double COS_TABLE[91] = {
1.0000, 0.9998, 0.9994, 0.9986, 0.9976, 0.9962, 0.9945, 0.9925, 0.9903, 0.9877, 
0.9848, 0.9816, 0.9781, 0.9744, 0.9703, 0.9659, 0.9613, 0.9563, 0.9511, 0.9455, 
0.9397, 0.9336, 0.9272, 0.9205, 0.9135, 0.9063, 0.8988, 0.8910, 0.8829, 0.8746, 
0.8660, 0.8572, 0.8480, 0.8387, 0.8290, 0.8192, 0.8090, 0.7986, 0.7880, 0.7771, 
0.7660, 0.7547, 0.7431, 0.7314, 0.7193, 0.7071, 0.6947, 0.6820, 0.6691, 0.6561, 
0.6428, 0.6293, 0.6157, 0.6018, 0.5878, 0.5736, 0.5592, 0.5446, 0.5299, 0.5150, 
0.5000, 0.4848, 0.4695, 0.4540, 0.4384, 0.4226, 0.4067, 0.3907, 0.3746, 0.3584, 
0.3420, 0.3256, 0.3090, 0.2924, 0.2756, 0.2588, 0.2419, 0.2250, 0.2079, 0.1908, 
0.1736, 0.1564, 0.1392, 0.1219, 0.1045, 0.0872, 0.0698, 0.0523, 0.0349, 0.0175,0.0000
};


/*==================================================================================================
    LOCAL TYPEDEFS - Structure, Union, Enumerations...etc
==================================================================================================*/
/* The value of enumeration is come from DS_SKEYALIGN_ENUM_T to use common values */
typedef enum
{
    ALIGN_LEFT   = 1,
    ALIGN_CENTER = 2,
    ALIGN_RIGHT  =3
} ALIGN_ENUM_T;

/* Analog Clcok Hand Enumeration - for idle screen  */
typedef enum
{
    ACLOCK_HAND_HOUR,
    ACLOCK_HAND_MINUTE
}   ACLOCK_HAND_TYPE_ENUM_T;

/*==================================================================================================
    LOCAL MACROS
==================================================================================================*/
/* if no update region, region is set to reversely */
#define VALID_REGION(rect)        ((rect.left != LCD_MAX_X-1) && (rect.top != LCD_MAX_Y-1) &&  \
                                  (rect.right != 0) && (rect.bottom != 0) )

#define VALID_POPUP_REGION(rect)  ((rect.left != LCD_POPUP_MAX_X-1) && (rect.top != LCD_POPUP_MAX_X-1) &&  \
                                   (rect.right != 0) && (rect.bottom != 0) )

#define digcmp(x,y)  (((x)>(y))?1:(((x)==(y))?0:-1))

/*==================================================================================================
    LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
    GLOBAL CONSTANTS
==================================================================================================*/
/* just for temp until we get real color per color theme */
const OP_UINT16 scramble_color[3] = {
    COLOR_LIGHT_BEIGE,
    COLOR_LIGHT_BEIGE,
    COLOR_LIGHT_BEIGE
};

/*==================================================================================================
    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static OP_UINT16  *get_scrn_buffer 
(
    SCRNAREA_ENUM_T area
);

static OP_INT16 get_aligned_x_pos 
(
    OP_INT16        start_x,
    OP_INT16        end_x,
    OP_UINT16        item_width,
    ALIGN_ENUM_T    align
);

static OP_BOOLEAN is_in_boundary
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           left,
    OP_INT16           top,
    OP_INT16           right,
    OP_INT16           bottom
);

static RM_ICON_T *get_registered_skeyicon 
(
    DS_SKEYPOS_ENUM_T    keypos, 
    OP_UINT8             *keyname
);

static OP_INT16 draw_ascii
(
    OP_INT16          x,
    OP_INT16          y, 
    OP_UINT8          data, 
    const OP_UINT8    *pGlyph_start, 
    DS_FONT_T         fd
);


static OP_INT16 draw_dial_digit
(
    OP_INT16          x,
    OP_INT16          y, 
    OP_UINT8          data, 
    const OP_UINT8    *pGlyph_start, 
    DS_FONT_T         fd
);


static OP_INT16 draw_symbol_n_chinese
(
    OP_INT16          x,
    OP_INT16          y, 
    OP_UINT16         data, 
    const OP_UINT8    *pGlyph_start,  /* it points symbolic glyph data with the given font id */
    DS_FONT_T         fd
);

static void draw_aclock_hand
(
    OP_INT16                      degree,
    OP_UINT8                      sx,
    OP_UINT8                      sy,
    OP_UINT8                      ex,
    OP_UINT8                      ey,
    ACLOCK_HAND_TYPE_ENUM_T       hand_type,
    DS_COLOR                      h_color,
    DS_COLOR                      s_color
);

static void draw_aclock_hands
( 
    OP_INT16              x_center,
    OP_INT16              y_center,
    OP_INT16              h_len,
    OP_INT16              m_len,
    DS_COLOR              h_color,
    DS_COLOR              s_color,
    OP_UINT8              hour,  
    OP_UINT8              min  
);


static void draw_overlaped_line
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           startx,
    OP_INT16           starty,
    OP_INT16           endx,
    OP_INT16           endy,
    DS_COLOR           color
);

static OP_BOOLEAN move_ustr_pointer
(
    OP_UINT16   num_to_skipped,
    OP_UINT8    **text_ptr
);

static OP_BOOLEAN make_positive_y_pos
(
    OP_INT16    *y, 
    OP_UINT8    **text_ptr
);

static OP_BOOLEAN make_positive_x_pos
(
    DS_FONTATTR    fontattr,
    OP_INT16       *x, 
    OP_UINT8       **text_ptr
);

static void set_softkey
(
    DS_SKEYPOS_ENUM_T    SOFTKEY_MASK,
    DS_SCREENATTR_T    sk_attr,
    OP_INT16                 xstart,
    OP_INT16                 xlimit,
    DS_SKEYDATA_T       cur_skey
);


extern OP_UINT16 *GetIdleImageBuffer(void);

/*==================================================================================================
    LOCAL FUNCTIONS
==================================================================================================*/


/*=================================================================================================
    FUNCTION: draw_aclock_hand 

    DESCRIPTION:
        Draw an analog clock hand.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        It is called by drawing primitives to add region that they newly draws.
        ds.rcUpdare region will be the refreshed region unless a user set his own reupdate region.        
==================================================================================================*/
static void draw_aclock_hand
(
    OP_INT16                      degree,
    OP_UINT8                      sx,
    OP_UINT8                      sy,
    OP_UINT8                      ex,
    OP_UINT8                      ey,
    ACLOCK_HAND_TYPE_ENUM_T       hand_type,
    DS_COLOR                      h_color,
    DS_COLOR                      s_color
)
{
    OP_BOOLEAN   is_hour_hand;

    is_hour_hand = (hand_type == ACLOCK_HAND_HOUR) ? OP_TRUE : OP_FALSE;

    /* draw center line */
    draw_overlaped_line(AREA_TEXT, sx, sy, ex, ey, h_color);
    
    if (degree == 90)
    {
        if (is_hour_hand)
        {        
            /* draw left line */
            draw_overlaped_line(AREA_TEXT, (OP_INT16)(sx-1), sy, (OP_INT16)(ex-1), ey, h_color);
        }
        /* draw right line */
        draw_overlaped_line(AREA_TEXT, (OP_INT16)(sx+1), sy, (OP_INT16)(ex+1), ey, s_color);        
    }
    else if (degree == 270)
    {
        /* draw upper line */
        draw_overlaped_line(AREA_TEXT, (OP_INT16)(sx-1), sy, (OP_INT16)(ex-1), ey, s_color);
        if (is_hour_hand)
        {        
            /* draw below line */
            draw_overlaped_line(AREA_TEXT, (OP_INT16)(sx+1), sy, (OP_INT16)(ex+1), ey, h_color);        
        }
    }
    else if (degree == 0)
    {
        if (is_hour_hand)
        {        
            /* draw upper line */
            draw_overlaped_line(AREA_TEXT, sx, (OP_INT16)(sy-1), ex, (OP_INT16)(ey-1), h_color);
        }  
        /* draw below line */
        draw_overlaped_line(AREA_TEXT, sx, (OP_INT16)(sy+1), ex, (OP_INT16)(ey+1), s_color);        
    }    
    else if (degree == 180)
    {
        if (is_hour_hand)
        {            
            /* draw upper line */
            draw_overlaped_line(AREA_TEXT, sx, (OP_INT16)(sy-1), ex, (OP_INT16)(ey-1), h_color);        
        }
        /* draw below line */
        draw_overlaped_line(AREA_TEXT, sx, (OP_INT16)(sy+1), ex, (OP_INT16)(ey+1), s_color);

    }
    else if ( degree < 90) /* Quadrant 1 */
    {
        if (is_hour_hand)
        {            
            /* draw upper left line */
            draw_overlaped_line(AREA_TEXT, (OP_INT16)(sx-1), (OP_INT16)(sy-1), (OP_INT16)(ex-1), (OP_INT16)(ey-1), h_color);
        }
        /* draw below right line */
        draw_overlaped_line(AREA_TEXT, (OP_INT16)(sx+1), (OP_INT16)(sy+1), (OP_INT16)(ex+1), (OP_INT16)(ey+1), s_color);
    }
    else if ( degree < 180 ) /* Quadrant 2 */
    {
        if (is_hour_hand)
        {            
            /* draw upper right line */
            draw_line(AREA_TEXT, (OP_INT16)(sx+1), (OP_INT16)(sy-1), (OP_INT16)(ex+1), (OP_INT16)(ey-1), h_color);
        }
        /* draw below left line */
        draw_overlaped_line(AREA_TEXT, (OP_INT16)(sx-1), (OP_INT16)(sy+1), (OP_INT16)(ex-1), (OP_INT16)(ey+1), s_color);
    }
    else if ( degree < 270 ) /* Quadrant 3 */
    {
        if (is_hour_hand)
        {            
            /* draw upper left line */
            draw_overlaped_line(AREA_TEXT, (OP_INT16)(sx-1), (OP_INT16)(sy-1), (OP_INT16)(ex-1), (OP_INT16)(ey-1), h_color);
        }
        /* draw below right line */
        draw_overlaped_line(AREA_TEXT, (OP_INT16)(sx+1), (OP_INT16)(sy+1), (OP_INT16)(ex+1), (OP_INT16)(ey+1), s_color);
    }
    else /* Quadrant 4 */
    {
        /* draw below left line */
        draw_overlaped_line(AREA_TEXT, (OP_INT16)(sx-1), (OP_INT16)(sy+1), (OP_INT16)(ex-1), (OP_INT16)(ey+1), s_color);
        if (is_hour_hand)
        {            
            /* draw upper right line */
            draw_overlaped_line(AREA_TEXT, (OP_INT16)(sx+1), (OP_INT16)(sy-1), (OP_INT16)(ex+1), (OP_INT16)(ey-1), h_color);
        }
    }   
}

/*=================================================================================================
    FUNCTION: draw_aclock_hands 

    DESCRIPTION:
        Draw analog clock hand.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        It is called by drawing primitives to add region that they newly draws.
        ds.rcUpdare region will be the refreshed region unless a user set his own reupdate region.        
==================================================================================================*/
static void draw_aclock_hands
( 
    OP_INT16              x_center,
    OP_INT16              y_center,
    OP_INT16              h_len,
    OP_INT16              m_len,
    DS_COLOR              h_color,
    DS_COLOR              s_color,
    OP_UINT8              hour,  
    OP_UINT8              min  
)
{
    OP_INT16     ex;
    OP_INT16     ey;

    double       degree;    

    /* draw hour hand */
    /* 12 hour analog clock*/
    hour = hour % 12;
    /* 12 O'clock is 90 degree, 1 O'clock is 60 degree, 2 O'clock is 30 degree, etc.
       450 degress is 360 degree + 90 degree. 30 degree per one hour */
    degree = (450 - hour*30) %360 - (min / 2);
    
    /*If the current degree<0,we should conversation it to 0 ~ 360 range */
    if(degree<0)
        degree+=360;

    ex = x_center + (OP_INT8)(h_len * op_cos((OP_INT16)degree));
    ey = y_center - (OP_INT8)(h_len * op_sin((OP_INT16)degree));
    draw_aclock_hand((OP_INT16)degree,(OP_UINT8) x_center,(OP_UINT8) y_center,(OP_UINT8) ex,(OP_UINT8) ey, ACLOCK_HAND_HOUR,h_color, s_color);

    /* draw minute hand */
    /* 450 degress is 360 degree + 90 degree. 6 degree per one minute */
    degree = (450 - min*6) %360;
    ex = x_center + (OP_INT8)(m_len * op_cos((OP_INT16)degree));
    ey = y_center - (OP_INT8)(m_len * op_sin((OP_INT16)degree));

    draw_aclock_hand((OP_INT16)degree,(OP_UINT8) x_center,(OP_UINT8) y_center,(OP_UINT8) ex,(OP_UINT8) ey, ACLOCK_HAND_MINUTE,h_color, s_color);
}


/*==================================================================================================
    FUNCTION: draw_overlaped_line 

    DESCRIPTION:
        Same with the draw_line except in case of diagonal line, line is overlapped.
        This is for analog clock hand.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
       It uses ºê·»½¼ÇÜ ¾Ë°í¸®Áò.
==================================================================================================*/
static void draw_overlaped_line
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           startx,
    OP_INT16           starty,
    OP_INT16           endx,
    OP_INT16           endy,
    DS_COLOR           color
)
{
    OP_INT16    idx; 
    OP_INT16    idy;
    OP_INT16    inc_x; 
    OP_INT16    inc_y;
    OP_INT16    deltaP; 
    OP_INT16    deltaN;
    OP_INT16    G;
    OP_INT16    i;
    OP_INT16    j;
    OP_INT16    tmp_x; 
    OP_INT16    tmp_y; 
    OP_UINT16   *buf;
    OP_INT16   step;

    buf = get_scrn_buffer(area);
  
    if (is_in_boundary(area, startx, starty, endx, endy))
    {   
        idx = ABS(endx - startx);
        idy = ABS(endy - starty);
    
        inc_x = digcmp(endx, startx);
        inc_y = digcmp(endy, starty);
    
        if (idx > idy) 
        { 
            G = 2*idy - idx;
            deltaP = idy * 2;
             deltaN = (idy - idx) * 2;
            step = starty;
            for (i = startx ; i != endx; i+=inc_x) 
            {
                if (G > 0) 
                {
                    step += inc_y;
                    G += deltaN;
                } 
                else 
                {
                    G += deltaP;
                }
               *(buf + step * LCD_MAX_X + i) = color;
               *(buf + step * LCD_MAX_X + (i+inc_x)) = color;
            }
        } 
        else if (idy > idx) 
        { 
            G = 2*idx - idy;
            deltaP = idx * 2;
            deltaN = (idx - idy) * 2;
            step = startx;
            for (i = starty ; i != endy; i+=inc_y) 
            {
                if (G > 0) 
                {
                    step += inc_x;
                    G += deltaN;
                }
                else 
                {
                    G += deltaP;
                }
               *(buf + i * LCD_MAX_X + step) = color;  
               *(buf + (i+inc_y) * LCD_MAX_X + step) = color;  
            }
        } 
        else 
        { /* idx == idy */
            for (i=startx, j=starty ; i != endx; i+=inc_x, j+=inc_y)
            {
               *(buf + j * LCD_MAX_X + i) = color;  
               *(buf + j * LCD_MAX_X + i+inc_x) = color;  
            }
        }
       *(buf + endy * LCD_MAX_X + endx) = color;  

        /* update updated area flag */
        UPDATESET(area);

        /* update updated region*/
        /* in case start coordinate is larger than end coordinate, switch them */
        if ( startx > endx)
        {
            tmp_x = endx;
            endx = startx;
            startx = tmp_x;
        }

        if ( starty > endy)
        {
            tmp_y = endy;
            endy = starty;
            starty = tmp_y;
        }

        add_update_region(area, startx, starty, endx, endy);  
    }
}

/*=================================================================================================
    FUNCTION: get_scrn_buffer 

    DESCRIPTION:
        Add newly update region.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        It is called by drawing primitives to add region that they newly draws.
        ds.rcUpdare region will be the refreshed region unless a user set his own reupdate region.        
==================================================================================================*/
static OP_UINT16 *get_scrn_buffer 
(
    SCRNAREA_ENUM_T    area
)
{
    OP_UINT16    *buf;

    switch (area) 
    {
        case AREA_ANNUN:
            buf = (OP_UINT16 *)annun_screen;
            break;
        case AREA_SOFTKEY:
            buf = (OP_UINT16 *)soft_screen;
            break;
        case AREA_POPUP:
            buf = (OP_UINT16 *)pop_screen;
            break;
        case AREA_TEXT:
            buf = (OP_UINT16 *)txt_screen;
            break;
        case AREA_BITMAP:
        default:
          buf = (OP_UINT16 *)bitmap_screen;
          break;
    }
    return buf;
}


/*=================================================================================================
    FUNCTION: get_aligned_x_pos 

    DESCRIPTION:
        Returns the calculated x position accrording to the given horizontal alignment.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None
==================================================================================================*/
static OP_INT16 get_aligned_x_pos 
(
    OP_INT16        start_x,
    OP_INT16        end_x,
    OP_UINT16        item_width,
    ALIGN_ENUM_T    align
)
{
    OP_INT16    aligned_x;

    if (align == ALIGN_LEFT)
    {
         aligned_x = start_x;        
    }
    else /* ALIGN_CENTER, ALIGN_RIGHT */
    {
         /*same with : start_x + (end_x - start_x - item_width)/2 */
         aligned_x = (start_x + end_x - (item_width-1)) /2 ; 
    }
    return aligned_x;
   
}
    

/*=================================================================================================
    FUNCTION: is_in_boundary 

    DESCRIPTION:
        Returns the calculated x position accrording to the given horizontal alignment.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None
==================================================================================================*/
static OP_BOOLEAN is_in_boundary
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           left,
    OP_INT16           top,
    OP_INT16           right,
    OP_INT16           bottom
)
{
    OP_BOOLEAN    is_in_boundary = OP_TRUE;
    OP_INT16      bound_y;

    /* Boundary check : if one coordinate exceed any screen boundary, do not draw */
    switch (area)
    {
        case AREA_ANNUN:
            bound_y = LCD_ANNUN_HIGH;           
            break; 
        case AREA_SOFTKEY:
            if (ds_get_extend_softkey() == OP_TRUE)
            {
                bound_y = LCD_EXT_SKEY_HIGH;           
            }
            else
            {
                bound_y = LCD_SKEY_HIGH;           
            }
            break; 
        case AREA_BITMAP:
            bound_y = LCD_MAX_Y;           
            break; 
        case AREA_POPUP:
        case AREA_TEXT:
            bound_y = (ds_get_disp_annun()) ? (LCD_TEXT_HIGH -1) : (LCD_EXT_TEXT_HIGH -1);           
            break; 
        case AREA_OPEN_WINDOW:
            bound_y = LCD_OPENWINDOW_HIGH;
            break;
        default:
            is_in_boundary = OP_TRUE;
            break;               
    }

    if     (is_in_boundary)
    {
        if (left > LCD_MAX_X || right > LCD_MAX_X ||
             top > bound_y || bottom > bound_y) 
        {
            is_in_boundary = OP_FALSE;
        } 
    }

    return is_in_boundary;

}


/*=================================================================================================
    FUNCTION: get_registered_skeyicon 

    DESCRIPTION:
        Returns the regirstered icon softkey.
        It is searched by softkey prompt.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None
==================================================================================================*/
static RM_ICON_T *get_registered_skeyicon 
(
    DS_SKEYPOS_ENUM_T    keypos, 
    OP_UINT8            *keyname
)
{
#if 0
    RM_ICON_T *sIcon_ptr = OP_NULL;
    if (keyname) 
    {
  
        if (keypos == SOFTKEY_CENTER) 
        {
            if (!op_strcmp(keyname, "MOVE")) 
            {
                sIcon_ptr = (RM_ICON_T *)&siconMove;
            }
            else if (!op_strcmp(keyname, "ToMail")) 
            {
                sIcon_ptr = (RM_ICON_T *)&siconToSms;
            }
            else if (!op_strcmp(keyname, "SCROLL")) 
            {
                sIcon_ptr = (RM_ICON_T *)&siconScroll;
            }
        }
    }
    return sIcon_ptr;
#else
    return OP_NULL;
#endif

}


/*=================================================================================================
    FUNCTION: draw_ascii 

    DESCRIPTION:
        Draw an ascii character.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None
==================================================================================================*/
static OP_INT16 draw_ascii
(
    OP_INT16          x,
    OP_INT16          y, 
    OP_UINT8          data, 
    const OP_UINT8    *pGlyph_start, 
    DS_FONT_T         fd
)
{
    OP_UINT8           *glyph;
    register OP_INT8   i;
    register OP_INT8   j;
    register OP_INT8   font_bits = fd.fontx;
    OP_INT16           new_xoffset = x;
    /* numb of OP_UINT8 in a row - either 1 or 2. (1 <=font width <=16) */ 
    OP_INT16           num_x_bytes = 1; 

    if (fd.fontx > NUM_BITS_IN_BYTE)
    {
        num_x_bytes = 2;
        font_bits = NUM_BITS_IN_BYTE;   
    }
    glyph = (OP_UINT8 *)pGlyph_start + (data)*fd.fonty*num_x_bytes;

      
    for (i = 0; i < fd.fonty; i++, glyph++)    
    {
        for (j = 0; j < font_bits; j++)     /* first x byte in the glyph */
        {
            if (fd.underline && i == (fd.fonty-1)) 
            {
                 *(fd.dest + fd.buf_width*(y+i) + (x+j)) = fd.fontcolor;
             } 
            else if ((*glyph) & (0x80>>j)) /* only of the pixel is on */
            {
                 *(fd.dest + fd.buf_width*(y+i) + (x+j)) = fd.fontcolor;
            } 
            else if (!fd.overlap) 
            {
                *(fd.dest + fd.buf_width*(y+i) + (x+j)) = fd.backcolor;
            }
          }
       if (num_x_bytes == 2) /* if the width is larger than 8 */
       {
            glyph++;
            x += NUM_BITS_IN_BYTE;
            font_bits = fd.fontx-NUM_BITS_IN_BYTE;
            for (j = 0; j < font_bits; j++)     /* sencond x byte in the glyph */
            {
                if (fd.underline && i == (fd.fonty-1)) 
                {
                     *(fd.dest + fd.buf_width*(y+i) + (x+j)) = fd.fontcolor;
                 } 
                else if ((*glyph) & (0x80>>j)) /* only of the pixel is on */
                {
                     *(fd.dest + fd.buf_width*(y+i) + (x+j)) = fd.fontcolor;
                } 
                else if (!fd.overlap) 
                {
                    *(fd.dest + fd.buf_width*(y+i) + (x+j)) = fd.backcolor;
                }
              }

       }  
    }
 
  return ( new_xoffset + fd.fontx );
 
}   


/*=================================================================================================
    FUNCTION:   

    DESCRIPTION:
        Draw a dialing digit.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Dialing digit is not supported underline, overlap.
        Width of a dialing digit is larger than a byte width (8bits) and smaller than or equal to
        two byte width (16bits).
        If this assumption is changed, calculating glyph data is also changed.
==================================================================================================*/
static OP_INT16 draw_dial_digit
(
    OP_INT16          x,
    OP_INT16          y, 
    OP_UINT8          data, 
    const OP_UINT8    *pGlyph_start, 
    DS_FONT_T         fd
)
{
    register OP_INT8     i;
    register OP_INT8     j;
    OP_UINT8             font_x_bytes;
    OP_INT16             new_xoffset = x;    
    const    OP_UINT8    *glyph;
    OP_UINT8             display_height, display_width;

    if (data > 0x2F && data< 0x3A) /* 0~9 */ 
    {
        data -= 0x2B;
    }
    else 
    {
        switch (data) 
        {
            case 0x23:  
                data = 0x01;  
                break;    /* "#" */
            case 0x2A:  
                data = 0x02;  
                break;    /* "*"  */
            case 0x2B:  
                data = 0x03;  
                break;    /* "+" */
            case 0x2D:  
                data = 0x04;  
                break;    /* "-" */
             case 0x70:  
                data = 0x0f;  
                break;    /* "p"  */                            
            default:   
                data = 0x00;  
                break;
        }
    }

    font_x_bytes = (fd.fontx+7)/NUM_BITS_IN_BYTE;
    glyph = pGlyph_start + (data)*fd.fonty * font_x_bytes; /* DS_DIAL_FONT_X_BYTES == 2 */
    
    if ((DS_DIAL_L_FONTID_START + DS_SMALL_FONTID) == fd.fontid)
    {
        display_height = FONT_DIAL_L_DISPLAY_HEIGHT;
        display_width = FONT_DIAL_L_DISPLAY_WIDTH;
    }
    else
    {
        display_height = FONT_DIAL_DISPLAY_HEIGHT;
        display_width = FONT_DIAL_DISPLAY_WIDTH;  
    }

    for (i = 0; i < display_height; i++, glyph++)    
    {
         for (j=0; j<NUM_BITS_IN_BYTE; j++)    /* first byte in x */
         {
             if ((*glyph) & (0x80>>j))  /* if the pixel is on */
             {
                 *(fd.dest + fd.buf_width*(y+i) + (x+j)) = fd.fontcolor;
             } 
             else 
             {
                 *(fd.dest + fd.buf_width*(y+i) + (x+j)) = fd.backcolor;
             }
         }
   
         if ( font_x_bytes >1 )
         {
             glyph++;
    
             for (j=0; j<(display_width-NUM_BITS_IN_BYTE); j++)    { /* second byte in x */
                if ((*glyph) & (0x80>>j))  {
                   *(fd.dest + fd.buf_width*(y+i) + (x+NUM_BITS_IN_BYTE+j)) = fd.fontcolor;
                } else  {
                   *(fd.dest + fd.buf_width*(y+i) + (x+NUM_BITS_IN_BYTE+j)) = fd.backcolor;
                }
             }
         }

       }
       return ( new_xoffset + display_width ); 
}   


/*=================================================================================================
    FUNCTION: draw_chinese

    DESCRIPTION:
        Draw a chiness or symbolic character.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Width of a chiness character (symbolic character also) is larger than a byte width 
        (8bits) and smaller than or equal to two byte width (16bits).
        If this assumption is changed, calculating glyph data is also changed.
==================================================================================================*/
static OP_INT16 draw_symbol_n_chinese
(
    OP_INT16            x,
    OP_INT16            y, 
    OP_UINT16           data, 
    const OP_UINT8      *pGlyph_start,  /* it points symbolic glyph data with the given font id */
    DS_FONT_T           fd
)
{
		OP_UINT8           *glyph;
		register OP_INT8    i;
		register OP_INT8    j;
		OP_INT16            new_xoffset = x;
		OP_UINT32           data_ptr;
		OP_UINT16           search_result = 0;

        /* find out the location in the graphic table for the given unicode  */
    search_result = GB2312_unicode_table[data];
        
        if (search_result > 0) /* get it */
        {
        data_ptr = (search_result - 1) * fd.fonty * FONT_CHI_X_BYTES;
        }
        else /*if the unicode can't be found, place the symbol with blank 0x12FB */
        {
        search_result = GB2312_unicode_table[0x12FB];
        data_ptr = search_result * fd.fonty * FONT_CHI_X_BYTES;
        }
        
    glyph = (OP_UINT8 *)pGlyph_start + data_ptr; 
        
    for (i = 0; i < fd.fonty; i++, glyph++)    
   {
        for (j = 0; j < NUM_BITS_IN_BYTE; j++)	 /* first x byte in the glyph */
        {
            if (fd.underline && i == (fd.fonty-1)) 
            {
                 *(fd.dest + fd.buf_width*(y+i) + (x+j)) = fd.fontcolor;
                } 
            else if ((*glyph) & (0x80>>j)) /* only of the pixel is on */
                {
                 *(fd.dest + fd.buf_width*(y+i) + (x+j)) = fd.fontcolor;
                } 
            else if (!fd.overlap) 
                {
                *(fd.dest + fd.buf_width*(y+i) + (x+j)) = fd.backcolor;
                }
            }
    
        glyph++;
            x += NUM_BITS_IN_BYTE;
        for (j = 0; j < fd.fontx-NUM_BITS_IN_BYTE; j++)     /* sencond x byte in the glyph */
            {
            if (fd.underline && i == (fd.fonty-1)) 
                {
                *(fd.dest + fd.buf_width*(y+i) + (x+j)) = fd.fontcolor;
                } 
            else if ((*glyph) & (0x80>>j)) /* only of the pixel is on */
                {
                *(fd.dest + fd.buf_width*(y+i) + (x+j)) = fd.fontcolor;
                } 
            else if (!fd.overlap) 
                {
                *(fd.dest + fd.buf_width*(y+i) + (x+j)) = fd.backcolor;
                }
            }
            x -= 8;
		
        }


 
  return ( new_xoffset + fd.fontx );
 
}   

/*==================================================================================================
    GLOBAL FUNCTIONS
==================================================================================================*/

/*=================================================================================================
    FUNCTION: op_sin 

    DESCRIPTION:
        Return sin value per given degree.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        It is called by drawing primitives to add region that they newly draws.
        ds.rcUpdare region will be the refreshed region unless a user set his own reupdate region.        
==================================================================================================*/
long double op_sin
(
    OP_INT16 degree
)
{
    OP_UINT16 temp_degree;
    temp_degree = degree % 360;
    
    if ( temp_degree <= 90)
        return SIN_TABLE[temp_degree];
    else if (temp_degree <= 180)
        return SIN_TABLE[180-temp_degree];
    else if (temp_degree <= 270)
        return (-1 * SIN_TABLE[temp_degree-180]);
    else if (temp_degree <=360)
        return (-1 * SIN_TABLE[360-temp_degree]);
    else
        return 0xff;       
}

/*=================================================================================================
    FUNCTION: op_cos 

    DESCRIPTION:
        Return cos value per given degree.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        It is called by drawing primitives to add region that they newly draws.
        ds.rcUpdare region will be the refreshed region unless a user set his own reupdate region.        
==================================================================================================*/
long double op_cos
(
    OP_INT16 degree
)
{
    OP_UINT16 temp_degree;
    temp_degree = degree % 360;
    
    if ( temp_degree <= 90)
        return COS_TABLE[temp_degree];
    else if (temp_degree <= 180)
        return (-1* COS_TABLE[180-temp_degree]);
    else if (temp_degree <= 270)
        return (-1 * COS_TABLE[temp_degree-180]);
    else if (temp_degree <=360)
        return COS_TABLE[360-temp_degree];
    else
        return 0xff;       
}


/*=================================================================================================
    FUNCTION: add_update_region 

    DESCRIPTION:
        Add newly update region.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        It is called by drawing primitives to add region that they newly draws.
        ds.rcUpdare region will be the refreshed region unless a user set his own reupdate region.        
==================================================================================================*/
void add_update_region 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           left,
    OP_INT16           top,
    OP_INT16           right,
    OP_INT16           bottom
)
{
    DS_RECT_T     new_rect;

    new_rect.left   = left;
    new_rect.top    = top;
    new_rect.right  = right;
    new_rect.bottom = bottom;

/* due to screen_idle that the bimtap image expand the whole screen, 
   limiting the bottom y coor to the lcd text high should be removed */    
    if ( area == AREA_POPUP)
    {
        ds.popwin->rcUpdate  = *(util_add_rect_in_boundary(&(ds.popwin->rcUpdate), new_rect, 
                                  LCD_POPUP_MAX_X-1, LCD_POPUP_MAX_Y-1));     
    }
    else if ( (area == AREA_TEXT) || (area == AREA_BITMAP) )
    {
         ds.rcUpdate  = *(util_add_rect_in_boundary(&ds.rcUpdate, new_rect, LCD_MAX_X-1, LCD_MAX_Y-1));     
    }
}

/*==================================================================================================
    FUNCTION: fill_rect 

    DESCRIPTION:
        Fill a rectangle to the specified screen.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void fill_rect 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           left,
    OP_INT16           top,
    OP_INT16           right,
    OP_INT16           bottom,
    DS_COLOR           color
)
{
    register OP_INT16    y;

    if (is_in_boundary(area, left, top, right, bottom))
    {      
  
        switch (area) 
        {
            case AREA_ANNUN:
                for (y = top; y <= bottom; y++)
                {
                    wmemset((OP_UINT16 *)&annun_screen[y][left], color, right-left+1);
                }
                break;
            case AREA_SOFTKEY:
               for (y = top; y <= bottom; y++)
               {
                   wmemset((OP_UINT16 *)&soft_screen[y][left], color, right-left+1);
               }
               break;
            case AREA_POPUP:
                for (y = top; y <= bottom; y++)
                {
                    wmemset((OP_UINT16 *)&pop_screen[y][left], color, right-left+1);
                }  
                break;
            case AREA_TEXT:
                for (y = top; y <= bottom; y++)
                {
                    wmemset((OP_UINT16 *)&txt_screen[y][left], color, right-left+1);
                }
                break;
            case AREA_OPEN_WINDOW:
                for (y = top; y <= bottom; y++)
                {
                    wmemset((OP_UINT16 *)&open_window_screen[y][left], color, right-left+1);
                }
                break;
            default: /* AREA_BITMAP */
                for (y = top; y <= bottom; y++)
                {
                    wmemset((OP_UINT16 *)&bitmap_screen[y][left], color, right-left+1);
                }
                break;
        }
        /* update updated area flag and updated region */
        UPDATESET(area);
        add_update_region(area, left, top, right, bottom);
    }
}


/*==================================================================================================
    FUNCTION: move_ustr_pointer 

    DESCRIPTION:
        This function checks x and y coordinates whether they are negative coordinates.
        If it is, set x and y to appropriate positive coordinates and move the text pointer to point 
        appropriate character that is sure to be written on the changed coordinates.
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
static OP_BOOLEAN move_ustr_pointer
(
    OP_UINT16   num_to_skipped,
    OP_UINT8    **text_ptr
)
{
    OP_BOOLEAN  ret_state = OP_TRUE; /* if it is false, the text is shorter than the string to be skipped */
    OP_UINT16   str_index = 0;
    OP_UINT16   char_count = 0; /* number of characters - ASCII : 1, CHINESE: 2 characters */
    OP_UINT16   *unicode_ptr;
    OP_UINT8    *text = *text_ptr;

    while ( (!( (text[str_index]==0) && (text[str_index+1]==0) )) && (char_count < num_to_skipped) ) 
    {
        unicode_ptr = (OP_UINT16 *)&text[str_index];
        if  (*unicode_ptr <= 0x007f)
        {
           char_count ++;
        }
        else
        {
           char_count +=2;
        }

        str_index += 2;
    }

    /* the last skipped character is Chinese */
    if (char_count > num_to_skipped)
    {
       str_index -=2;
    }

    *text_ptr = (*text_ptr + str_index);

    if (char_count < num_to_skipped)
    {
        ret_state = OP_FALSE;
    }

    return ret_state;
}


/*==================================================================================================
    FUNCTION: make_positive_y_pos 

    DESCRIPTION:
        This function checks y coordinate whether they are negative coordinates.
        If it is, set y to appropriate positive coordinates and move the text pointer to point 
        appropriate character that is sure to be written on the changed coordinates.
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
static OP_BOOLEAN make_positive_y_pos
(
    OP_INT16    *y, 
    OP_UINT8    **text_ptr
)
{
    OP_BOOLEAN  ret_state = OP_TRUE; /* if it is false, no text to be written */
    OP_INT16    difference;
    OP_UINT8    num_line_skipped = 0;
    
    /* if y coordinate is negative */
    if ( *y < 0 )
    {
        difference = ABS(*y);
        num_line_skipped = (difference+(LINE_HEIGHT-1)) / LINE_HEIGHT;
        num_line_skipped = num_line_skipped * DS_SCRN_CWIDE;
        ret_state = move_ustr_pointer(num_line_skipped , text_ptr);  
        *y = 0;           
    }     
    
    return ret_state;
}


/*==================================================================================================
    FUNCTION: make_positive_x_pos 

    DESCRIPTION:
        This function checks x coordinate whether they are negative coordinates.
        If it is, set x to appropriate positive coordinate and move the text pointer to point 
        appropriate character that is sure to be written on the changed coordinates.
       
    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
static OP_BOOLEAN make_positive_x_pos
(
    DS_FONTATTR  fontattr,
    OP_INT16     *x, 
    OP_UINT8    **text_ptr
)
{
    OP_BOOLEAN  ret_state = OP_TRUE; /* if it is false, no text to be written */
    OP_UINT8    font_width;
    OP_INT16    difference;
    OP_UINT8    num_char_skipped = 0;
    
    if ( *x < 0 )
    {
        difference = ABS(*x);
        font_width = (fontattr & FONT_SIZE_SMALL) ? FONTX_SMALL:FONTX_NORMAL;
        num_char_skipped = (difference+(font_width-1)) / font_width;        
        ret_state = move_ustr_pointer(num_char_skipped, text_ptr);             
        *x = ds.rcWin.left;
    }      
    
    return ret_state;

}


/**************************************************************************************************
    REFRESH Control Functions
**************************************************************************************************/

/*==================================================================================================
    FUNCTION: set_update_region

    DESCRIPTION:
        Set updare region.       

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
void set_update_region
(
    OP_INT16    left,
    OP_INT16    top,
    OP_INT16    right, 
    OP_INT16    bottom
)
{
    ds.rcUpdate.left   = MIN(left, (LCD_MAX_X-1)); 
    ds.rcUpdate.top    = MIN(top, LCD_MAX_Y-1);  
    ds.rcUpdate.right  = MIN(right, (LCD_MAX_X-1)); 
    ds.rcUpdate.bottom = MIN(bottom, LCD_MAX_Y-1); 
}


/*==================================================================================================
    FUNCTION: set_popup_update_region

    DESCRIPTION:
        Set new popup updated region.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        None.       
==================================================================================================*/
void set_popup_update_region
(
    OP_INT16    left,
    OP_INT16    top,
    OP_INT16    right, 
    OP_INT16    bottom
)
{

    OP_INT16    ylimit = LCD_POPUP_MAX_Y - 1 ;

    if (ds_get_disp_annun())
    {
        ylimit = LCD_POPUP_MAX_Y - LCD_TEXT_FRAME_START - 1 ;
    }

    ds.popwin->rcUpdate.left   = MIN(left, (LCD_POPUP_MAX_X-1)); 
    ds.popwin->rcUpdate.top    = MIN(top, ylimit);  
    ds.popwin->rcUpdate.right  = MIN(right, (LCD_POPUP_MAX_X-1)); 
    ds.popwin->rcUpdate.bottom = MIN(bottom, ylimit); 
}


/*==================================================================================================
    FUNCTION: clear_update_region

    DESCRIPTION:
        After refresh to the lcd, clear the update region and area flag. 

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
void clear_update_region
(
    void
)
{
    /* to invalidate the update region, set X and Y reverse way */
    set_update_region(LCD_MAX_X-1,LCD_MAX_Y-1,0,0);
    if (ds.popwin != OP_NULL)
    {
        set_popup_update_region(LCD_POPUP_MAX_X-1,LCD_POPUP_MAX_Y-1,0,0);
    }
    UPDATECLEAR(AREAMASK_ALL);
}


/*==================================================================================================
    FUNCTION: refresh_rectangle

    DESCRIPTION:
        Refresh the specified rectangle to the screen immediately.

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
void refresh_rectangle 
(
    DS_RECT_T    rect
)
{
    OP_BOOLEAN    is_full_size_screen_mode = OP_FALSE;
    LCD_RECT_T    u_rect;
    LCD_RECT_T    *u_ptr  = OP_NULL; /* bg updated region */
    LCD_RECT_T    *pu_ptr = OP_NULL; /* relative popup update region pointer*/
    
    if (VALID_REGION(rect))
    {
        u_rect.left = rect.left;
        u_rect.top = rect.top;
        u_rect.right = rect.right;
        u_rect.bottom = rect.bottom;
        u_ptr = &u_rect;

        /* if the newly updated region is overlapped with the currently active popup region,
           Popup should be updated together not to lose popup display */
        if (ds_is_popup())
        {
            util_set_rectangle(&ds.popwin->rcUpdate, 
                                 ds.popwin->pos.left,
                                 ds.popwin->pos.top,
                                 ds.popwin->pos.right,
                                 ds.popwin->pos.bottom);

            UPDATESET(AREAMASK_POPUP);
        }  
    }
        
    if (ds_is_popup() )
    {
        if  (VALID_POPUP_REGION(ds.popwin->rcUpdate)) 
        {     
            pu_ptr = (LCD_RECT_T *)&ds.popwin->rcUpdate;
        }       
    } 

    /* refresh either background or popup is updated */
    if ( (u_ptr != OP_NULL) || (pu_ptr != OP_NULL) || UPDATEAREA())
    {
        /* in case of background exist, even though only text buffer has been updated, 
           background should be update together */
        if ( (ds.update_area & AREAMASK_TEXT)&& 
              ( (ds.scrnMode == SCREEN_IDLE)       ||
                (ds.scrnMode == SCREEN_GENERAL_BG) ||
                (ds.scrnMode == SCREEN_OWNERDRAW) ||
                (ds.scrnMode == SCREEN_FUNC_BG)))           
        {
            UPDATESET(AREAMASK_BITMAP);
        }

        if ((ds.update_area & AREAMASK_BITMAP) || (ds.update_area & AREAMASK_ANNUN) ||
             (ds.update_area & AREAMASK_SKEY))
       
        {
            /* special case of full screen image */
            /* SCREEN_IDLE - idle screen with full screen size image and annuciator and skey.
               SCREEN_FULL_IMAGE - for power on/off image and animation 
                                 - popup (sim detect, ...)might be active on the screen */
            if ((ds.scrnMode == SCREEN_IDLE) || (ds.scrnMode == SCREEN_FULL_IMAGE) || (ds.scrnMode == SCREEN_MAINMENU))
            {
                is_full_size_screen_mode = OP_TRUE;
    
                 if (ds.scrnMode == SCREEN_IDLE)
                 {
                     /* due to full screen idle background image, annun and softkey area should be refreshed if
                        updated region is overlapped to their regions */
                     if (ds.rcUpdate.top < LCD_ANNUN_HIGH)
                     {
                         UPDATESET(AREAMASK_ANNUN);
                     } 
 
                     if (ds.rcUpdate.bottom >= LCD_MAX_Y - LCD_EXT_SKEY_HIGH)
                     {
                         UPDATESET(AREAMASK_SKEY);
                     }
                 } 
                 else /* SCREEN_FULL_IMAGE */
                 {
                     /* no popup?, that mean only full screen image exist - no need to update skey */
                     if (ds_is_popup() == OP_FALSE)
                     {
                         UPDATECLEAR(AREAMASK_SKEY);
                     }
                 }
            }
        }
  
        lcd_refresh(ds.update_area, is_full_size_screen_mode, ds.disp_annun, u_ptr, pu_ptr,scramble_color[0]);
#ifdef RECOVER_AFTER_POPUP
        /* Keep the last update area in case of background recovery when a popup window is closed. */ 
        if  ((ds.update_area & AREAMASK_TEXT) || (ds.update_area & AREAMASK_BITMAP))
        {
            ds.last_update_area = ds.update_area;
        }
#endif
        /* Clear the update region and area mask after refreshment */
        clear_update_region();    
    }
}


/*==================================================================================================
    FUNCTION: refresh_to_lcd

    DESCRIPTION:
       Refresh to lcd. Called by ds_refrsh().

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
void refresh_to_lcd
(
    void
)
{
    /* Main LCD refreshment */
    refresh_rectangle(ds.rcUpdate);   
}


/**************************************************************************************************
    DRAWING Primitives
**************************************************************************************************/

/*=================================================================================================
    FUNCTION: draw_text_line 

    DESCRIPTION:
        Draw text in the specified area and position accroding to the given atttibutes. 
        The text is drawn until it reaches the end of a line and the leftover text is truncated.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        To draw text longer than the a line from the staring position, use ds_draw_text().
        new line (\n) character makes it move to new line.
        tap (\t) is considered one space.
==================================================================================================*/
OP_INT16 draw_text_line 
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
)
{
    OP_UINT8                ascii;
    OP_UINT8                *char_ptr;           /* point to a charcter */
    OP_INT8                 i;  
    OP_INT8                 num_processed_byte;  /* number of the processed bytes from the given unicode string. */
    OP_INT16                xoffset;             /* x coordinate where a charcter to be drawn */
    OP_INT16                leng;                /* length of the given text string */    
    static OP_INT16         ylimit;              /* y limit of the assigned buffer (screen) */
    static DS_FONT_T        fd;                  /* font data */
    static DS_FONT_ID       ascii_font_id;       /* ASCII font (or dialing font) id with the given fontattr */
    static DS_FONT_ID       chi_font_id;         /* Chiness font id with the given fontattr */
    static OP_UINT8         ascii_fontx;         /* ASCII font width */
    static OP_UINT8         ascii_fonty;         /* ASCII font height */
    static OP_UINT8         chi_fontx;           /* Chiness font width */
    static OP_UINT8         chi_fonty;           /* Chiness font height */
    static const OP_UINT8   *pAsciiGlyph;        /* starting glyth data address of the ascii font */
    static const OP_UINT8   *pChiGlyph;          /* starting glyth data address of the chiness */
    OP_UINT16               unicode;
   
    num_processed_byte = 0;

    /* skip this if it is not the first line of the string */ 
    if ( is_one_line_text )
    {  

        /* either text string is null or y is negatibe and no character to be written after adjusting y coor,
           exit - no character to be written. */
        if ( (text == OP_NULL) && ( make_positive_y_pos(&y, &text) == OP_FALSE)) return OP_FALSE;

        /* Fill font data with the given font attribute */
    
        /* Get font ids with the given font attributes */
        /* ascii_font_id is either ascii character or dialing font */
        /* chi_font_id is for chiness and symbolic character */ 
        font_get_font_id(fontattr, &ascii_font_id, &chi_font_id);
    
        /* Get font sizes - font height is the same for both ascii and chiness */
    
        /* ascii_font_id is either english ascii or digit. */ 
        font_get_font_size(ascii_font_id,  &ascii_fontx, &ascii_fonty);  /* English and Dial digit case */  
       
        font_get_font_size(chi_font_id, &chi_fontx, &chi_fonty);  /*  Symbol and Chiness */  
    
        /* Get font glyph starting addresses */
        pAsciiGlyph = font_get_fontset_glyph(ascii_font_id); 
        pChiGlyph = font_get_fontset_glyph(chi_font_id);
    
        fd.underline = (fontattr & FONT_UNDERLINE);
        fd.overlap   = (fontattr & FONT_OVERLAP);
      
        switch (area) 
        {
            case AREA_ANNUN:
              fd.buf_width = LCD_MAX_X;
              fd.dest = (OP_UINT16 *)annun_screen;
              ylimit =  LCD_ANNUN_HIGH -1 ;  
              break;
            case AREA_SOFTKEY:
              fd.buf_width = LCD_MAX_X;
              fd.dest = (OP_UINT16 *)soft_screen;
              if (ds_get_extend_softkey() == OP_TRUE)
              {
                  ylimit =  LCD_EXT_SKEY_HIGH -1 ;         
              }
              else
              {
                  ylimit =  LCD_SKEY_HIGH -1 ;         
              }
             break;
            case AREA_POPUP:
              fd.buf_width = LCD_MAX_X;
              fd.dest = (OP_UINT16 *)pop_screen;
              ylimit = (ds_get_disp_annun()) ? (LCD_TEXT_HIGH -1) : (LCD_EXT_TEXT_HIGH -1);                  
              break;
            case AREA_BITMAP:
              fd.buf_width = LCD_MAX_X;
              fd.dest = (OP_UINT16 *)bitmap_screen;
              ylimit =  LCD_MAX_Y -1 ;         
             break;
            case AREA_OPEN_WINDOW:
              fd.buf_width = LCD_OPENWINDOW_MAX_X;
              fd.dest = (OP_UINT16 *)open_window_screen;
              ylimit =  LCD_OPENWINDOW_MAX_Y -1 ;
             break;
            default: /* AREA_TEXT */
              fd.buf_width = LCD_MAX_X;
              fd.dest = (OP_UINT16 *)txt_screen;
              ylimit = (ds_get_disp_annun()) ? (LCD_TEXT_HIGH -1) : (LCD_EXT_TEXT_HIGH -1);                  
             break;
        }
    }
    
    fd.fontcolor = fontcolor;
    fd.backcolor = backcolor;

    /* draw character after adjust x coor to positive if it is negative and write only if there is char to be written */
    if (make_positive_x_pos(fontattr, &x, &text))
    {
        leng = Ustrlen(text);
        
        char_ptr = (OP_UINT8 *)text;
           
        xoffset = x;
        
        /* If exceed the y boundary for this line, do not draw, then return FALSE.  */
        if ( ((y + chi_fonty -1) > ylimit) && ((y + ascii_fonty -1) > ylimit) )
        {
            return OP_FALSE;      
        }
    
    
        for (i = 0; ((xoffset <= xlimit) && (i < leng)) ; i += 2) 
        {
            unicode = (*(text+i+1)) << 8; 
            unicode |= *(text+i);
            if  (unicode <= 0x007f)
             {
                if ( (unicode != 0x0000) && ( i < leng ) && ((xoffset+ascii_fontx-1) <= xlimit))
                {   /* if it is not end of string and there is a space for the new character, 
                       draw a charcter and update x offset with the new value */
    
                    if (unicode == 0x000a) /* new line */
                    {
                        num_processed_byte +=2;
                        /* fill the background color to the end of line */
                        if (!fd.overlap)
                        {
                            fill_rect(area, xoffset, y, (OP_INT16)(fd.buf_width-1), (OP_INT16)(y+fd.fonty), fd.backcolor);
                        }
                        break; /* exit the loop */
                    }
#if 0
                    if (*unicode_ptr == '\t') /* tap */
                    {
                        *char_ptr = 0x20; /* consider a space  */
                    }
#endif
                    fd.fontx = ascii_fontx; 
                    fd.fonty = ascii_fonty;
    
                    if ((fontattr & FONT_DIAL)||(fontattr & FONT_DIAL_L))
                    {
                        fd.fontid = chi_font_id;
                        ascii = (OP_UINT8)(unicode & 0x00ff);
                        xoffset = draw_dial_digit(xoffset, y, ascii,pAsciiGlyph, fd);
                    }
                    else
                    {
                        ascii = (OP_UINT8)((unicode) & 0x00ff);
                        xoffset = draw_ascii(xoffset, y, ascii, pAsciiGlyph, fd);                 
                    }  
                    num_processed_byte +=2; 
                }
                else  /* reach the end of text string or the end of the screen */
                {
                    break; /* exit the loop */ 
                }  
            }        
            else if ( (i < leng)  && ((xoffset+chi_fontx-1) <= xlimit))
            { /* case of symbol or chiness, it is  consisted with two-byte code. */
                if (( unicode >= 0x0080) && (unicode <= 0xffe5))
                { 
                    fd.fontx = chi_fontx;
                    fd.fonty = chi_fonty;
                    fd.fontid = chi_font_id;
                    /* Compose LSB byte of the two-byte character code */
                    xoffset = (OP_INT16)draw_symbol_n_chinese(xoffset, y, unicode, pChiGlyph, fd);  
                    num_processed_byte +=2; 
                }
                else if (unicode == 0x0000) 
                { /* reach the end of the string */
                    break; /* exit the loop */
                }
                else /* invalid code. Ignore it. */ 
                {
                    ; /* do nothiing */
                }
            }
            else
            {
                 break; /* end of string */
            }
        }
    
        /* move the text pointer to the processed charcter pointer */
        text = (text + num_processed_byte);
    
        /* update updated area flag and updated region */
        UPDATESET(area);
        add_update_region(area, x, y, (OP_INT16)(xoffset-1), (OP_INT16)(y+fd.fonty-1));
    }

    return (num_processed_byte); /* return the number of processed character */
}

/*==================================================================================================
    FUNCTION: draw_text 

    DESCRIPTION:
        Draw text in the specified area and position accroding to the given atttibutes. 
        If the text exceeds the end of a line, it wil be auto-wrpped to the next line of the area.
        Text exceeds longer than given buffer (area) is truncated.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        To draw one line text, use ds_draw_text_line().
        new line (\n) character makes it move to new line.
        tap (\t) is considered one space.

==================================================================================================*/
void draw_text 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           x,
    OP_INT16           y,
    OP_INT16           xlimit,
    OP_UINT8           *text,
    DS_FONTATTR        fontattr,
    DS_COLOR           fontcolor,
    DS_COLOR           backcolor
)
{
    OP_INT16       coffset;          /* number of the processed character by draw_text_line */
    OP_UINT8       *next_text;       /* it is updated by draw_text_line to point to next character to be
                                        processed as a next line */

    OP_BOOLEAN     first_line = OP_TRUE;

    next_text = text;
    coffset   = 0;

    /* negative y coordinate is adjusted to positive one by make_positive_y_pos() and text pointer
       is moved to accordinly */
    /* x coordinate is adjusted to positive if it is negative inside of draw_text_line to adjust the
       x coordinate per line */
    if (make_positive_y_pos(&y, &text))
    {    
        do 
        {
            coffset = draw_text_line (area, x, y, xlimit, next_text, first_line, 
                                              fontattr, fontcolor, backcolor );
            next_text += coffset;
            if (ds_is_msg_popup_active())
            {
                y += POPUP_LINE_HEIGHT;
            }
            else
            {
                y += LINE_HEIGHT;    
            }
            first_line = OP_FALSE;
            
        } while ((IS_NOT_NULL_STRING(next_text)) && (coffset != 0));
    }
}


/*==================================================================================================
    FUNCTION: draw_char 

    DESCRIPTION:
        Draw a uniocode character.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
==================================================================================================*/
void draw_char 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16       x,
    OP_INT16       y,
    OP_UINT8       *text,
    DS_FONTATTR    fontattr,
    DS_COLOR       fontcolor,
    DS_COLOR       backcolor
)
{
    OP_UINT8 uni_char[4] = {0x00, 0x00, 0x00, 0x00};
    uni_char[0] = *text;
    uni_char[1] = *(text+1);
    draw_text_line(area, x, y, (OP_INT16)(LCD_MAX_X-1), uni_char, OP_TRUE, fontattr, fontcolor, backcolor);
}


/*==================================================================================================
    FUNCTION: draw_bitmap_image 

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
void draw_bitmap_image 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           x,
    OP_INT16           y,
    RM_BITMAP_T       *bitmap
)
{
    OP_INT16    i; 
    OP_INT16    j;
    OP_UINT8    text_screen_height;
    OP_INT16    left, right, top, bottom;
    DS_COLOR    (*dest)[LCD_MAX_X]; 

    text_screen_height = (ds_get_disp_annun()) ? LCD_TEXT_HIGH : LCD_EXT_TEXT_HIGH ;                  

    
    if (!bitmap|| !bitmap->data)  return;
    
    left = MAX(x, 0);
    top= MAX(y, 0);
    right=MIN(x+bitmap->biWidth, LCD_MAX_X);
    
    switch (area) 
    {
        case AREA_TEXT:
            dest = txt_screen;
            if (ds_get_disp_annun())
            {
                //top= MAX(top - LCD_ANNUN_HIGH, 0);
                bottom=MIN(y+bitmap->biHeight, LCD_TEXT_HIGH);
            }
            else
            {
                bottom=MIN(y+bitmap->biHeight, LCD_EXT_TEXT_HIGH);
            }
            break;
        case AREA_POPUP:
            dest = pop_screen;
            bottom=MIN(y+bitmap->biHeight, text_screen_height );
            break;
        case AREA_SOFTKEY:
            dest = soft_screen;
            if (ds_get_extend_softkey() == OP_TRUE) /* If it is screen_idle, the height of the softkey area is LCD_EXT_SKEY_HIGH*/
            {
                bottom=MIN(y+bitmap->biHeight, LCD_EXT_SKEY_HIGH);
            }
            else
            {
                bottom=MIN(y+bitmap->biHeight, LCD_SKEY_HIGH);
            }
            break;
        case AREA_ANNUN:
            dest = annun_screen;
            bottom=MIN(y+bitmap->biHeight, LCD_ANNUN_HIGH);
            break;
        case AREA_OPEN_WINDOW:
            dest = open_window_buffer;
            bottom=MIN(y+bitmap->biHeight, LCD_OPENWINDOW_HIGH);
            break;
        default: /* AREA_BITMAP */
            dest = bitmap_buffer;
            bottom=MIN(y+bitmap->biHeight, LCD_MAX_Y);
            
            /* if the image is longer than text buffer height, that means no auunciator is displayed.
               - case of full size image - power up image... */
#if 0
	     if (bottom - top>= LCD_EXT_TEXT_HIGH)
            {
                UPDATECLEAR(AREA_ANNUN); /* annuciator buffer is positioned at the end of text buffer */
            }
#endif

            break;
    }

    /* update updated area flag */
    UPDATESET(area);

    if(right<=0 || bottom <= 0)
        return; 

    for (i = top; i < bottom; i++) 
    {
        if(bitmap->biBitCount == 16)
        {  
            DS_COLOR   *image_ptr;

            image_ptr = (OP_UINT16 *)bitmap->data + (i-y)*bitmap->biWidth+(left-x);
            /*
            for (j = left; j < right; j++, image_ptr++) 
            {
                dest[i][j] = *image_ptr;
            }
            */
            op_memcpy(&dest[i][left], image_ptr, (right - left)*sizeof(DS_COLOR));
        }
        else  /* 8 bit color */
        {
            OP_UINT8  *image_ptr;
            image_ptr = (OP_UINT8 *)bitmap->data + (i-y)*bitmap->biWidth+(left-x);
            
            for (j = left; j < right; j++, image_ptr++) 
            {
                if(bitmap->ColorPallet != OP_NULL)
                dest[i][j] = * ((OP_UINT16 *)bitmap->ColorPallet + (*image_ptr));
            }
            
        }
    }
    add_update_region(area, left, top, (OP_INT16)(right-1), (OP_INT16)(bottom-1));  
}
 

/*==================================================================================================
    FUNCTION: draw_partial_bitmap_image 

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
void draw_partial_bitmap_image 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           dsx,        /* destination start x coor */
    OP_INT16           dsy,        /* destination start y coor */
    OP_INT16           d_width,  /* destination width to be drawn */
    OP_INT16           d_height,  /* destination height to be drawn */
    OP_INT16           ssx,       /* source bitmap start x coor */
    OP_INT16           ssy,       /* source bitmap start y coor */
    RM_BITMAP_T       *bitmap
)
{
    OP_UINT8    cx;
    OP_UINT8    cy;
    OP_UINT8    ylimit;
    OP_UINT16    s_buf_width ; /* source data buffer width */
    OP_UINT8    text_screen_height;
    OP_UINT8    skey_area_height;
    OP_INT16    dy;
    OP_INT16    sy;

    OP_UINT16   *dest;        /* destination buffer */
    
    OP_UINT16   *d_ptr;      /* destination buffer pointer.*/


    text_screen_height = (ds_get_disp_annun()) ? LCD_TEXT_HIGH : LCD_EXT_TEXT_HIGH ;            
    skey_area_height = (ds_get_extend_softkey() == OP_TRUE) ? LCD_EXT_SKEY_HIGH: LCD_SKEY_HIGH;
    
    if (!bitmap|| !bitmap->data)  return;

    /* 
    Judge whether the param of dsx and dsy are negatives.
    If they are,chang the coordinates of source bitmap's region.
    Judge whether dsx is large than it's width.
    If it is, return.
    The same to dsy.
    */
    if (dsx < 0)
    {
        ssx -= dsx;
        if(ssx > bitmap->biWidth)
        {
            return;
        }
        dsx = 0;
    }

    if (dsy < 0)
    {
        ssy -=dsy;
        if(ssy > bitmap->biHeight)
        {
            return;
        }
        dsy = 0;
    }
    
    if  ((dsx < LCD_MAX_X) && (dsy < LCD_MAX_Y))
    {    
        cx = MIN(d_width, (bitmap->biWidth-ssx));
        cy = MIN(d_height, (bitmap->biHeight-ssy));
        s_buf_width = bitmap->biWidth;
        
        ylimit = dsy + cy-1;
              
        switch (area) 
        {
            case AREA_TEXT:
                if (ylimit > text_screen_height) 
                {
                    ylimit = text_screen_height -1;
                }
                dest = (OP_UINT16 *)txt_screen;
                break;
            case AREA_POPUP:
                if (ylimit > text_screen_height) 
                {
                    ylimit = text_screen_height -1;
                }
                dest = (OP_UINT16 *)pop_screen;
                break;
            case AREA_SOFTKEY:
                if (ylimit > skey_area_height) 
                {
                    ylimit = skey_area_height -1;
                }
                dest = (OP_UINT16 *)soft_screen;
                break;
            case AREA_ANNUN:
                if (ylimit > LCD_ANNUN_HIGH)
                {
                    ylimit = LCD_ANNUN_HIGH -1;
                }
                dest = (OP_UINT16 *)annun_screen;
                break;
            case AREA_OPEN_WINDOW:
                if (ylimit > LCD_OPENWINDOW_HIGH)
                {
                    ylimit = LCD_OPENWINDOW_HIGH -1;
                }
                dest = (OP_UINT16 *)open_window_screen;
                break;
            default: /* AREA_BITMAP */
                if (ylimit > LCD_MAX_Y) 
                {
                    ylimit = LCD_MAX_Y -1;
                }
                dest = (OP_UINT16 *)bitmap_screen;
    
                /* if the image is longer than text buffer height, that means no auunciator is displayed.
                   - case of full size image - power up image... */
                if (ylimit >= LCD_EXT_TEXT_HIGH)
                {
                    UPDATECLEAR(AREA_ANNUN); /* annuciator buffer is positioned at the end of text buffer */
                }
    
                break;
        }
    
        /* adjust cy according to the y limit */
        if (dsy > ylimit)
        {
            cy = 0;
        }
        else if ( (dsy+cy-1) > ylimit)
        {
            cy = ylimit - dsy +1;
        }
    
        for ( dy = dsy, sy = ssy; dy < dsy+cy; dy++, sy++)
        {
            d_ptr = dest + dy*LCD_MAX_X+dsx;  
            if(bitmap->biBitCount == 16)
            {   /* copy line by line */
                OP_UINT16 *s_ptr;
                s_ptr =(OP_UINT16 *)bitmap->data + (sy*s_buf_width) + ssx;
                op_memcpy(d_ptr, s_ptr, cx*2);        
            }
            else
            {   
                OP_UINT8   offset;
                OP_UINT8   *s_ptr;
                s_ptr = (OP_UINT8 *)bitmap->data + (sy*s_buf_width) + ssx;
                for(offset = 0; offset < cx; offset++,s_ptr++)
                {
                    if(bitmap->ColorPallet != OP_NULL)
                        d_ptr[offset] = *((OP_UINT16 *)bitmap->ColorPallet + *s_ptr);
                }
            }
    
        }
        /* update updated area flag */
        UPDATESET(area);
        add_update_region(area, dsx, dsy, (OP_INT16)(dsx+cx-1), (OP_INT16)(dsy+cy-1));  
    }
}

/*==================================================================================================
    FUNCTION: draw_resized_bitmap_image 

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
void draw_resized_bitmap_image 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           dsx,         /* starting x coor in the destination */
    OP_INT16           dsy,         /* starting y coor in the destination */
    OP_UINT8           dw,          /* width of the destination */
    OP_UINT8           dh,          /* height of the destination */
    OP_INT16           ssx,         /* starting x coor in the source */      
    OP_INT16           ssy,         /* starting y coor in the source */
    RM_BITMAP_T       *bitmap      /* bitmap data */
)
{
  register OP_INT16 dx;            /* x coor of the destination */
  register OP_INT16 dy;            /* y coorof the destination */
  register OP_INT16 sx;            /* x coor of the source */
  register OP_INT16 sy;            /* y coor of the source */

  OP_UINT8          text_screen_height;
    OP_UINT8    skey_area_height;

  OP_UINT16         *dest;        /* destination buffer */
  OP_INT16          cxSrc;        /* count of x in source data */
  OP_INT16          cySrc;        /* count of y in source data */

  OP_INT16          rx;           /* resize rate of x */
  OP_INT16          ry;           /* resize rate of y */

  OP_UINT16         *d_ptr;      /* destination buffer pointer.*/


  text_screen_height = (ds_get_disp_annun()) ? LCD_TEXT_HIGH : LCD_EXT_TEXT_HIGH ;                  
  skey_area_height = (ds_get_extend_softkey() == OP_TRUE) ? LCD_EXT_SKEY_HIGH: LCD_SKEY_HIGH;

  if (dsx + dw > LCD_MAX_X)
  {
      dw = LCD_MAX_X - dsx -1;
  }

  /* get the destination buffer address */  
  switch (area) 
  {
      case AREA_TEXT:
        if (dsy + dh > text_screen_height) 
        {
            dh = text_screen_height - dsy - 1;
        }
        dest = (OP_UINT16 *)txt_screen;
        break;
      case AREA_ANNUN:
        if (dsy + dh > LCD_ANNUN_HIGH) 
        {
            dh = LCD_ANNUN_HIGH - dsy - 1;
        }
        dest = (OP_UINT16 *)annun_screen;
        break;
      case AREA_SOFTKEY:
        if (dsy + dh > skey_area_height) 
        {
            dh = skey_area_height - dsy - 1;
        }
        dest = (OP_UINT16 *)soft_screen;
        break;
      case AREA_POPUP:
        if (dsy + dh > text_screen_height) 
        {
            dh = text_screen_height - dsy - 1;
        }

        dest = (OP_UINT16 *)pop_screen;
        break;
      case AREA_BITMAP:
        if (dsy + dh > LCD_MAX_Y) 
        {
            dh = LCD_MAX_Y- dsy - 1;
        }
        dest = (OP_UINT16 *)bitmap_screen;
        break;
      default:
        break;
  }

  
  /* get the souce data info */
  cxSrc = (bitmap->biWidth - ssx +1);
  cySrc = (bitmap->biHeight - ssy +1);

  /* get the resize rate */ 
  rx = cxSrc/dw;
  ry = cySrc/dh;

  /* draw resized image */
  for (dy=dsy, sy=ssy; dy<dsy+ dh; dy++, sy+=ry) 
  {  
      d_ptr = dest + dy*LCD_MAX_X;  
      if(bitmap->biBitCount == 16)
      {
          OP_UINT16 *    s_ptr ;      /* souce data pointer.*/

          s_ptr = (OP_UINT16 *)bitmap->data + (sy*bitmap->biWidth);  
          for (dx=dsx, sx=ssx; dx<dsx+dw; dx++, sx+=rx) 
          {
              *(d_ptr+dx) = *(s_ptr+sx);
          }
      }
      else
      {
          OP_UINT8 *     s_ptr;      /* souce data pointer.*/

          s_ptr = (OP_UINT8 *)bitmap->data + (sy*bitmap->biWidth);  
          for (dx=dsx, sx=ssx; dx<dsx+dw; dx++, sx+=rx) 
          {
              if(bitmap->ColorPallet != OP_NULL)
              *(d_ptr+dx) = *((OP_UINT16 *)bitmap->ColorPallet + *(s_ptr+sx));
          }           
      }
  }

  /* update updated area flag */
  UPDATESET(area);
  add_update_region(area, dsx, dsy, (OP_INT16)(dsx+dw-1), (OP_INT16)(dsy+dh-1));  
}

/*==================================================================================================
    FUNCTION: draw_icon 

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
        Icon is drawn on the txt_screen if the given area neither AREA_POPUP nor AREA_SOFTKEY.
==================================================================================================*/
void draw_icon 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           x,
    OP_INT16           y,
    RM_ICON_T          *icon
)
{
    OP_INT16    i;
    OP_INT16    j;
    DS_COLOR    (*dest)[LCD_MAX_X]; 
    OP_INT16    left, right, top, bottom;
   
    if (!icon)  return;

    left = MAX(x, 0);
    top= MAX(y, 0);
    right=MIN(x+icon->biWidth, LCD_MAX_X);
    
    switch (area) 
    {
        case AREA_POPUP:
            dest = pop_screen;
            bottom=MIN(y+icon->biHeight, LCD_POPUP_MAX_Y);
            UPDATESET(AREAMASK_POPUP);
            break;

        case AREA_SOFTKEY:
            dest = soft_screen;
            if (ds_get_extend_softkey() == OP_TRUE)
            {
                bottom=MIN(y+icon->biHeight, LCD_EXT_SKEY_HIGH);
            }
            else
            {
                bottom=MIN(y+icon->biHeight, LCD_SKEY_HIGH);
            }
            UPDATESET(AREAMASK_SKEY);
            break;

        case AREA_BITMAP: /* only for SCREEN_FULL_IMAGE */
            dest = bitmap_buffer;
            bottom=MIN(y+icon->biHeight, LCD_MAX_Y);
            UPDATESET(AREAMASK_BITMAP);
            break;

        case AREA_OPEN_WINDOW: 
            dest = open_window_screen;
            bottom=MIN(y+icon->biHeight, LCD_OPENWINDOW_HIGH);
            UPDATESET(AREAMASK_OPENWINDOW);
            break;
            
        default:
            dest = txt_screen;
            if (ds_get_disp_annun())
            {
                bottom=MIN(y+icon->biHeight, LCD_TEXT_HIGH);
            }
            else
            {
                bottom=MIN(y+icon->biHeight, LCD_EXT_TEXT_HIGH);
            }

            UPDATESET(AREAMASK_TEXT);
            break;
    }

    if(right<=0 || bottom <= 0)
        return; 

    for (i = top; i < bottom; i++) 
    {
        if(icon->biBitCount == 16)
        {
            OP_UINT16 *image_ptr;
            image_ptr = (OP_UINT16 *)icon->data + (i-y)*icon->biWidth+(left-x);
            for (j = left; j < right; j++, image_ptr++) 
            {
                if (*image_ptr != icon->trans) 
                {
                    dest[i][j] = *image_ptr;
                } 
            }
        }
        else
        {
            OP_UINT8 *image_ptr;
            DS_COLOR data;
            image_ptr = (OP_UINT8 *)icon->data + (i-y)*icon->biWidth+(left-x);
            for (j = left; j < right; j++, image_ptr++) 
            {   
                data = *((OP_UINT16 *)icon->ColorPallet + *image_ptr);
                if( data != icon->trans) 
                {
                    dest[i][j] = data;
                } 
            }
        }
    }

    /* update updated region*/
    add_update_region(area, left, top, (OP_INT16)(right-1), (OP_INT16)(bottom-1));  
}



/*==================================================================================================
    FUNCTION: draw_partial_icon 

    DESCRIPTION:
        Draw icon partially to the specified area and region. The icon image is the internal 
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
void draw_partial_icon 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           dsx,        /* destination start x coor */
    OP_INT16           dsy,        /* destination start y coor */
    OP_INT16           d_width,  /* destination width to be drawn */
    OP_INT16           d_height,  /* destination height to be drawn */
    OP_INT16           ssx,       /* source icon start x coor */
    OP_INT16           ssy,       /* source icon start y coor */
    RM_ICON_T         *icon
)
{
    OP_UINT8    cx;
    OP_UINT8    cy;
    OP_UINT8    ylimit;
    OP_UINT16    s_buf_width ; /* source data buffer width */
    OP_UINT8    text_screen_height;
    OP_UINT8    skey_area_height;
    OP_UINT8    offset;
    OP_INT16    dy;
    OP_INT16    sy;

    OP_UINT16   *dest;        /* destination buffer */
    
    OP_UINT16   *d_ptr;      /* destination buffer pointer.*/


    text_screen_height = (ds_get_disp_annun()) ? LCD_TEXT_HIGH : LCD_EXT_TEXT_HIGH ;                  
    skey_area_height = (ds_get_extend_softkey() == OP_TRUE) ? LCD_EXT_SKEY_HIGH: LCD_SKEY_HIGH;
    
    if  ((icon != OP_NULL) && (icon->data != OP_NULL) && (dsx < LCD_MAX_X) && (dsy < LCD_MAX_Y))
    {    
        cx = MIN(d_width, (icon->biWidth-ssx));
        cy = MIN(d_height, (icon->biHeight-ssy));
        s_buf_width = icon->biWidth;
        
        ylimit = dsy + cy-1;
              
        switch (area) 
        {
            case AREA_TEXT:
                if (ylimit > text_screen_height) 
                {
                    ylimit = text_screen_height -1;
                }
                dest = (OP_UINT16 *)txt_screen;
                break;
            case AREA_POPUP:
                if (ylimit > text_screen_height) 
                {
                    ylimit = text_screen_height -1;
                }
                dest = (OP_UINT16 *)pop_screen;
                break;
            case AREA_SOFTKEY:
                if (ylimit > skey_area_height) 
                {
                    ylimit = skey_area_height -1;
                }
                dest = (OP_UINT16 *)soft_screen;
                break;
            case AREA_ANNUN:
                if (ylimit > LCD_ANNUN_HIGH)
                {
                    ylimit = LCD_ANNUN_HIGH -1;
                }
                dest = (OP_UINT16 *)annun_screen;
                break;
            default: /* AREA_BITMAP */
                if (ylimit > LCD_MAX_Y) 
                {
                    ylimit = LCD_MAX_Y -1;
                }
                dest = (OP_UINT16 *)bitmap_screen;
    
                /* if the image is longer than text buffer height, that means no auunciator is displayed.
                   - case of full size image - power up image... */
                if (ylimit >= LCD_EXT_TEXT_HIGH)
                {
                    UPDATECLEAR(AREA_ANNUN); /* annuciator buffer is positioned at the end of text buffer */
                }
    
                break;
        }
    
        /* adjust cy according to the y limit */
        if (dsy > ylimit)
        {
            cy = 0;
        }
        else if ( (dsy+cy-1) > ylimit)
        {
            cy = ylimit - dsy +1;
        }
    
        for ( dy = dsy, sy = ssy; dy < dsy+cy; dy++, sy++)
        {
            d_ptr = dest + dy*LCD_MAX_X+dsx;  
            if(icon->biBitCount == 16)
            { 
                OP_UINT16 *s_ptr;
                s_ptr = (OP_UINT16 *)icon->data + (sy*s_buf_width) + ssx;
            for (offset = 0; offset < cx; offset++, s_ptr++) 
            {
                if (*s_ptr != icon->trans) 
                {
                    d_ptr[offset] = *s_ptr;
                  }
            }
        }
            else
            {
                OP_UINT8 *s_ptr;
                s_ptr = (OP_UINT8 *)icon->data + (sy*s_buf_width) + ssx;
                for(offset = 0; offset < cx; offset++,s_ptr++)
                {
                    if(icon->ColorPallet != OP_NULL)
                        d_ptr[offset] = *((OP_UINT16 *)icon->ColorPallet + *s_ptr);
                }
            }
        }
        /* update updated area flag */
        UPDATESET(area);
        add_update_region(area, dsx, dsy, (OP_INT16)(dsx+cx-1), (OP_INT16)(dsy+cy-1));  
    }
}
/*==================================================================================================
    FUNCTION: draw_resized_icon 

    DESCRIPTION:
        Draw icon fit to the specified area and region. The icon image is the internal 
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
void draw_resized_icon 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           dsx,         /* starting x coor in the destination */
    OP_INT16           dsy,         /* starting y coor in the destination */
    OP_UINT8           dw,          /* width of the destination */
    OP_UINT8           dh,          /* height of the destination */
    OP_INT16           ssx,         /* starting x coor in the source */      
    OP_INT16           ssy,         /* starting y coor in the source */
    RM_ICON_T       *icon      /* icon data */
)
{
  register OP_INT16 dx;            /* x coor of the destination */
  register OP_INT16 dy;            /* y coorof the destination */
  register OP_INT16 sx;            /* x coor of the source */
  register OP_INT16 sy;            /* y coor of the source */

  OP_UINT8          text_screen_height;
  OP_UINT8    skey_area_height;

  OP_UINT16         *dest;        /* destination buffer */
  OP_INT16          cxSrc;        /* count of x in source data */
  OP_INT16          cySrc;        /* count of y in source data */

  OP_INT16          rx;           /* resize rate of x */
  OP_INT16          ry;           /* resize rate of y */

  OP_UINT16         *d_ptr;      /* destination buffer pointer.*/


  text_screen_height = (ds_get_disp_annun()) ? LCD_TEXT_HIGH : LCD_EXT_TEXT_HIGH ;                  
  skey_area_height = (ds_get_extend_softkey() == OP_TRUE) ? LCD_EXT_SKEY_HIGH: LCD_SKEY_HIGH;

  if (dsx + dw > LCD_MAX_X)
  {
      dw = LCD_MAX_X - dsx -1;
  }

  /* get the destination buffer address */  
  switch (area) 
  {
      case AREA_TEXT:
        if (dsy + dh > text_screen_height) 
        {
            dh = text_screen_height - dsy - 1;
        }
        dest = (OP_UINT16 *)txt_screen;
        break;
      case AREA_ANNUN:
        if (dsy + dh > LCD_ANNUN_HIGH) 
        {
            dh = LCD_ANNUN_HIGH - dsy - 1;
        }
        dest = (OP_UINT16 *)annun_screen;
        break;
      case AREA_SOFTKEY:
        if (dsy + dh > skey_area_height) 
        {
            dh = skey_area_height - dsy - 1;
        }
        dest = (OP_UINT16 *)soft_screen;
        break;
      case AREA_POPUP:
        if (dsy + dh > text_screen_height) 
        {
            dh = text_screen_height - dsy - 1;
        }

        dest = (OP_UINT16 *)pop_screen;
        break;
      case AREA_BITMAP:
        if (dsy + dh > LCD_MAX_X) 
        {
            dh = LCD_MAX_X - dsy - 1;
        }
        dest = (OP_UINT16 *)bitmap_screen;
        break;
      default:
        break;
  }

  /* get the souce data info */
  cxSrc = (icon->biWidth - ssx +1);
  cySrc = (icon->biHeight - ssy +1);
  /* get the resize rate */ 
  rx = cxSrc/dw;
  ry = cySrc/dh;

  /* draw resized image */
  for (dy=dsy, sy=ssy; dy<dsy+ dh; dy++, sy+=ry) 
  {  
      d_ptr = dest + dy*LCD_MAX_X;  
      if(icon->biBitCount == 16)
      {
          OP_UINT16 *     s_ptr;      /* souce data pointer.*/
          s_ptr = (OP_UINT16 *)icon->data + (sy*icon->biWidth);  
          for (dx=dsx, sx=ssx; dx<dsx+dw; dx++, sx+=rx) 
          {
              if((*s_ptr+sx) != icon->trans)
              {
                  *(d_ptr+dx) = *(s_ptr+sx);
              }
          }
      }
      else
      {
          OP_UINT8 *     s_ptr;      /* souce data pointer.*/
          DS_COLOR       data;
          s_ptr = (OP_UINT8 *)icon->data + (sy*icon->biWidth);  
          for (dx=dsx, sx=ssx; dx<dsx+dw; dx++, sx+=rx) 
          {
              data = *((OP_UINT16 *)icon->ColorPallet + *(s_ptr+sx));
              if(data != icon->trans)
              {
                  if(icon->ColorPallet != OP_NULL)
                  *(d_ptr+dx) = data;
              }
          }    
      }
  }

  /* update updated area flag */
  UPDATESET(area);
  add_update_region(area, dsx, dsy, (OP_INT16)(dsx+dw-1), (OP_INT16)(dsy+dh-1));  
}

/*==================================================================================================
    FUNCTION: get_pixel 

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
DS_COLOR get_pixel
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           pos_x,
    OP_INT16           pos_y
)
{
    OP_UINT16    *buf;
  
    buf = get_scrn_buffer(area);

    return *(buf + pos_y * LCD_MAX_X + pos_x);
}

/*==================================================================================================
    FUNCTION: put_pixel 

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
void put_pixel
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           pos_x,
    OP_INT16           pos_y,
    DS_COLOR           color
)
{
    OP_UINT16    *buf;

    buf = get_scrn_buffer(area);

    *(buf + pos_y * LCD_MAX_X + pos_x) = color;

    /* update updated area flag */
    UPDATESET(area);
    /* update updated region*/
    add_update_region(area, pos_x, pos_y, pos_x, pos_y);  

}


/*==================================================================================================
    FUNCTION: draw_line 

    DESCRIPTION:
        Draw a line to the specified screen.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
       It uses ºê·»½¼ÇÜ ¾Ë°í¸®Áò.
==================================================================================================*/
void draw_line
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           startx,
    OP_INT16           starty,
    OP_INT16           endx,
    OP_INT16           endy,
    DS_COLOR           color
)
{
    OP_INT16    idx; 
    OP_INT16    idy;
    OP_INT16    inc_x; 
    OP_INT16    inc_y;
    OP_INT16    deltaP; 
    OP_INT16    deltaN;
    OP_INT16    G;
    OP_INT16    i;
    OP_INT16    j;
    OP_INT16    tmp_x;
    OP_INT16    tmp_y;
    OP_UINT16   *buf;
    OP_INT16   step;

    buf = get_scrn_buffer(area);
  
    if (is_in_boundary(area, startx, starty, endx, endy))
    {   
        idx = ABS(endx - startx);
        idy = ABS(endy - starty);
    
        inc_x = digcmp(endx, startx);
        inc_y = digcmp(endy, starty);
    
        if (idx > idy) 
        { 
            G = 2*idy - idx;
            deltaP = idy * 2;
             deltaN = (idy - idx) * 2;
             step = starty;
            for (i = startx ; i != endx; i+=inc_x) 
            {
                if (G > 0) 
                {
                    step += inc_y;
                    G += deltaN;
                } 
                else 
                {
                    G += deltaP;
                }
               *(buf + step * LCD_MAX_X + i) = color;
            }
        } 
        else if (idy > idx) 
        { 
            G = 2*idx - idy;
            deltaP = idx * 2;
            deltaN = (idx - idy) * 2;
            step = startx;
            for (i = starty ; i != endy; i+=inc_y) 
            {
                if (G > 0) 
                {
                    step += inc_x;
                    G += deltaN;
                }
                else 
                {
                    G += deltaP;
                }
               *(buf + i * LCD_MAX_X + step) = color;  
            }
        } 
        else 
        { /* idx == idy */
            for (i=startx, j=starty ; i != endx; i+=inc_x, j+=inc_y)
            {
               *(buf + j * LCD_MAX_X + i) = color;  
            }
        }
       *(buf + endy * LCD_MAX_X + endx) = color;  

        /* update updated area flag */
        UPDATESET(area);

        /* update updated region*/
        /* in case start coordinate is larger than end coordinate, switch them */
        if ( startx > endx)
        {
            tmp_x = endx;
            endx = startx;
            startx = tmp_x;
        }

        if ( starty > endy)
        {
            tmp_y = endy;
            endy = starty;
            starty = tmp_y;
        }

        add_update_region(area, startx, starty, endx, endy);  
    }
}

/*==================================================================================================
    FUNCTION: draw_rect 

    DESCRIPTION:
        Draw a rectangle to the specified screen.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void draw_rect 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           left,
    OP_INT16           top,
    OP_INT16           right,
    OP_INT16           bottom,
    DS_COLOR           color
)
{
    register OP_INT16    i;

    if (is_in_boundary(area, left, top, right, bottom))
    {    
        switch (area) 
        {
            case AREA_ANNUN:
                for (i = top; i <= bottom; i++) 
                {
                    annun_screen[i][left]  = color;
                    annun_screen[i][right] = color;
                }
                for (i = left; i <= right; i++) 
                {
                    annun_screen[top][i]    = color;
                    annun_screen[bottom][i] = color;
                }
                break;
          
            case AREA_SOFTKEY:
                for (i = top; i <= bottom; i++) 
                {
                    soft_screen[i][left]  = color;
                    soft_screen[i][right] = color;
                }
                for (i = left; i <= right; i++) 
                {
                    soft_screen[top][i]    = color;
                    soft_screen[bottom][i] = color;
                }
                break;
        
            case AREA_POPUP:
                for (i = top; i <= bottom; i++) 
                {
                    pop_screen[i][left]  = color;
                    pop_screen[i][right] = color;
                }
                for (i = left; i <= right; i++) 
                {
                    pop_screen[top][i]    = color;
                    pop_screen[bottom][i] = color;
                }
                break;
              
            case AREA_TEXT:
                for (i = top; i <= bottom; i++) 
                {
                    txt_screen[i][left]  = color;
                    txt_screen[i][right] = color;
                }
                for (i = left; i <= right; i++) 
                {
                    txt_screen[top][i]    = color;
                    txt_screen[bottom][i] = color;
                }
                break;
            
            default: /* AREA_BITMAP*/
                for (i = top; i <= bottom; i++) 
                {
                    bitmap_screen[i][left]  = color;
                    bitmap_screen[i][right] = color;
                }
                for (i = left; i <= right; i++) 
                {
                    bitmap_screen[top][i]    = color;
                    bitmap_screen[bottom][i] = color;
                }
                break;
        }
        /* update updated area flag */
        UPDATESET(area);
        /* update updated region*/
        add_update_region(area, left, top, right, bottom);  
    }
}


/*==================================================================================================
    FUNCTION: draw_button_rect 

    DESCRIPTION:
        Draw a button style rectangle to the specified screen.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void draw_button_rect 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           left,
    OP_INT16           top,
    OP_INT16           right,
    OP_INT16           bottom,
    DS_COLOR           left_top,
    DS_COLOR           right_bottom
)
{
    register OP_INT16    i;
  
    if (is_in_boundary(area, left, top, right, bottom))
    {
        switch (area) 
        {
            case AREA_ANNUN:
                for (i = left; i <= right; i++) 
                {
                    annun_screen[top][i]    = left_top;
                    annun_screen[bottom][i] = right_bottom;
                }
                for (i = (top+1); i <= bottom; i++) 
                {
                    annun_screen[i][left]  = left_top;
                    annun_screen[i][right] = right_bottom;
                }
                break;
          
            case AREA_SOFTKEY:
                for (i = left; i <= right; i++) 
                {
                    soft_screen[top][i]    = left_top;
                    soft_screen[bottom][i] = right_bottom;
                }
                for (i = (top+1); i <= bottom; i++) 
                {
                    soft_screen[i][left]  = left_top;
                    soft_screen[i][right] = right_bottom;
                }
                break;
          
            case AREA_POPUP:
                for (i = left; i <= right; i++) 
                {
                    pop_screen[top][i]    = left_top;
                    pop_screen[bottom][i] = right_bottom;
                }
                for (i = (top+1); i <= bottom; i++) 
                {
                    pop_screen[i][left]  = left_top;
                    pop_screen[i][right] = right_bottom;
                }
                break;
              
            case AREA_TEXT:
                for (i = left; i <= right; i++) 
                {
                    txt_screen[top][i]    = left_top;
                    txt_screen[bottom][i] = right_bottom;
                }
                for (i = (top+1); i <= bottom; i++) 
                {
                    txt_screen[i][left]  = left_top;
                    txt_screen[i][right] = right_bottom;
                }
                UPDATESET(AREA_TEXT);
                break;
              
            default:
                for (i = left; i <= right; i++) 
                {
                    bitmap_screen[top][i]    = left_top;
                    bitmap_screen[bottom][i] = right_bottom;
                }
                for (i = (top+1); i <= bottom; i++) 
                {
                    bitmap_screen[i][left]  = left_top;
                    bitmap_screen[i][right] = right_bottom;
                }
                break;
        }
        UPDATESET(area);
        /* update updated region*/
        add_update_region(area, left, top, right, bottom);  
    }
}


/*==================================================================================================
    FUNCTION: draw_button 

    DESCRIPTION:
       Draw filled a button style rectangle to the specified screen.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void draw_button 
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           left,
    OP_INT16           top,
    OP_INT16           right,
    OP_INT16           bottom,
    DS_COLOR           top_left_color,
    DS_COLOR           right_bottom_color,
    DS_COLOR           fill_color
)
{
    draw_button_rect (area, left, top, right, bottom, top_left_color, right_bottom_color); 
    fill_rect (area, (OP_INT16)(left+1), (OP_INT16)(top+1), (OP_INT16)(right-1), (OP_INT16)(bottom-1), fill_color);
}

/*==================================================================================================
    FUNCTION: draw_title 

    DESCRIPTION:
       Draw title.
       
    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void draw_title 
(
    OP_UINT8                *title,
    RM_RESOURCE_ID_T      title_bg_res_id
)
{
    OP_INT16   x = 0;

    if (IS_NOT_NULL_STRING(title))
    {     
#if ((PROJECT_ID == WHALE1_PROJECT)||(PROJECT_ID == WHALE2_PROJECT)||(PROJECT_ID == NEMO_PROJECT))
        /* Left alignment */
        x = TITLE_START_X_POS;
#else
        /*center alignment*/
        x = (OP_INT16)(GET_CENTER(x, LCD_MAX_X_COOR, UstrCharCount(title)*FONTX_SMALL));
#endif
       /*
		if(ds.cComposer.color_theme == COLOR_THEME_USER_DEFINE)
        {   
            fill_rect(AREA_TEXT, 0, ds.title.y.top, LCD_MAX_X_COOR, 
                      ds.title.y.bottom,ds.cComposer.titleColor);
        }
        else
		*/
        draw_bitmap_image(AREA_TEXT, 0, ds.title.y.top, 
                            (RM_BITMAP_T *)util_get_bitmap_from_res(title_bg_res_id+ds_get_color_theme()));           
#if ((PROJECT_ID == WHALE1_PROJECT)||(PROJECT_ID == WHALE2_PROJECT)||(PROJECT_ID == NEMO_PROJECT))
        draw_text_line(AREA_TEXT, 
                       x, 
                       (OP_INT16)(ds.title.y.top + 1), 
                       LCD_MAX_X_COOR, 
                       title, 
                       OP_TRUE,
                       FA_WINTITLE,            
                       ds.cComposer.titleFontColor, 
                       COLOR_FONTBG);   

#else
        draw_text_line(AREA_TEXT, 
                       x, 
                       (OP_INT16)(ds.title.y.top + 3), 
                       LCD_MAX_X_COOR, 
                       title, 
                       OP_TRUE,
                       FA_WINTITLE,            
                       ds.cComposer.titleFontColor, 
                       COLOR_FONTBG);   
#endif
    }
}

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
)
{
        /* draw title upper shadow */
       draw_text_line(area, 
                       x, 
                       (OP_INT16)(y-1), 
                       xlimit, 
                       title, 
                       is_one_line_text,
                       fontattr,            
                       fontcolor, 
                       backcolor);
     
        /* draw title down shadow */
        draw_text_line(area, 
                       x, 
                       (OP_INT16)(y+1), 
                       xlimit, 
                       title, 
                       is_one_line_text,
                       fontattr,            
                       fontcolor, 
                       backcolor);

         /* draw title left shadow */
        draw_text_line(area, 
                        (OP_INT16)(x-1), 
                        (OP_INT16)y, 
                        xlimit, 
                        title, 
                        is_one_line_text,
                        fontattr,            
                        fontcolor, 
                        backcolor);
        /* draw title right shadow */
        draw_text_line(area, 
                        (OP_INT16)(x+1), 
                        (OP_INT16)y, 
                        xlimit, 
                        title, 
                        is_one_line_text,
                        fontattr,            
                        fontcolor, 
                        backcolor);

        /* right upper */
         draw_text_line(area, 
                        (OP_INT16)(x+1), 
                        (OP_INT16)(y-1), 
                        xlimit, 
                        title, 
                        is_one_line_text,
                        fontattr,            
                        fontcolor, 
                        backcolor);
        /* right down */
           draw_text_line(area, 
                        (OP_INT16)(x+1), 
                        (OP_INT16)(y+1), 
                        xlimit, 
                        title, 
                        is_one_line_text,
                        fontattr,            
                        fontcolor, 
                        backcolor);


        /* draw title left upper shadow */
        draw_text_line(area, 
                        (OP_INT16)(x-1), 
                        (OP_INT16)(y-1), 
                        xlimit, 
                        title, 
                        is_one_line_text,
                        fontattr,            
                        fontcolor, 
                        backcolor);

        /* draw title left down shadow */
        draw_text_line(area, 
                        (OP_INT16)(x-1), 
                        (OP_INT16)(y+1), 
                        xlimit, 
                        title, 
                        is_one_line_text,
                        fontattr,            
                        fontcolor, 
                        backcolor);


}
/*==================================================================================================
    FUNCTION: set_softkey

    DESCRIPTION:
       Draw softkeys - predefined string, icon or both together and user specified string in center
       alligned in softkey area.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Only horizontal alignment is supported. Middle alignment is default vertial alignment.
==================================================================================================*/
static void set_softkey
(
    DS_SKEYPOS_ENUM_T    SOFTKEY_MASK,
    DS_SCREENATTR_T    sk_attr,
    OP_INT16                 xstart,
    OP_INT16                 xlimit,
    DS_SKEYDATA_T       cur_skey
)
{
    RM_ICON_T      *iconSkey;
    OP_UINT8       font_cx;              /* font width used to draw softkey */
    OP_UINT8       font_cy;              /* font height used to draw softkey */
    OP_INT16       xoffset  = 0;         /* x offset to start draw in a softkey */
    OP_UINT8       skey_area_height;
    DS_COLOR      fontcolor;
    
    skey_area_height = (ds_get_extend_softkey() == OP_TRUE) ? LCD_EXT_SKEY_HIGH : LCD_SKEY_HIGH;
    
    /* Get skey font size */
    font_cx = font_get_font_width_w_fattr(sk_attr.fontAttr);
    font_cy = font_get_font_height_w_fattr(sk_attr.fontAttr);
    if ( (ds.scrnMode == SCREEN_IDLE) && (ds_is_popup() == OP_FALSE) )
    {
        OP_INT16   i;
        OP_UINT16  *pBuffer = OP_NULL;
        OP_UINT16  *pStart = OP_NULL;
        /* get idle image buffer */
        pBuffer = GetIdleImageBuffer();
        /* clear softkey region */
        for(i=0; i<LCD_EXT_TEXT_HIGH;i++)
        {
            pStart = pBuffer + (LCD_MAX_Y-LCD_EXT_SKEY_HIGH+i)*LCD_MAX_X + xstart;
            op_memcpy(&soft_screen[i][xstart], pStart,(xlimit-xstart+1)*sizeof(DS_COLOR));
            //op_memcpy(&soft_screen[i][xstart], &idle_image_buffer[LCD_MAX_Y-LCD_EXT_SKEY_HIGH+i][xstart],(xlimit-xstart+1)*sizeof(DS_COLOR));
        }
    } 

        /* here seems no use, it's alway return OP_NULL */
        iconSkey = get_registered_skeyicon(SOFTKEY_MASK, cur_skey.txt);
        if (iconSkey) 
        {
            /* In case of reginsered icon skey, it can't be drawn with skey text */ 
            if (cur_skey.attr & SKA_NOALIGN)            /* user defined position */
            {
                xoffset = cur_skey.x_pos;
            }
            else
            {
                /* Lower nibble of the skey atttibue represents alignment */
                xoffset = get_aligned_x_pos(xstart, xlimit, iconSkey->biWidth, (ALIGN_ENUM_T)(cur_skey.attr &0x0f));
            }
            draw_icon(AREA_SOFTKEY, xoffset, (OP_INT16)((skey_area_height-iconSkey->biHeight)/2), (RM_ICON_T *)iconSkey);  
        }
        else 
        {
            if (cur_skey.icon ) 
            {
                /* Lower nibble of the skey atttibue represents alignment */
                xoffset = get_aligned_x_pos(xstart, xlimit, cur_skey.icon->biWidth, (ALIGN_ENUM_T)(cur_skey.attr &0x0f));
                draw_icon(AREA_SOFTKEY, 
                            xoffset, 
                            (OP_INT16)((skey_area_height-cur_skey.icon->biHeight)/2), 
                            (RM_ICON_T *)cur_skey.icon);  
                xoffset = xstart + cur_skey.icon->biWidth;
            }
            else
            {
                RM_BITMAP_T*  pBitmap = OP_NULL;
                OP_INT8 len = UstrCharCount(cur_skey.txt);
                /* draw softkey bg */
                if(ds_get_extend_softkey() == OP_FALSE)
                {
                    if(ds.cComposer.color_theme == COLOR_THEME_USER_DEFINE)
                    {
                        fill_rect(AREA_SOFTKEY, xstart, 0, xlimit, skey_area_height, ds.cComposer.softkeyColor);    
                    }
                    else
                    {
                        pBitmap = (RM_BITMAP_T*)util_get_bitmap_from_res(BMP_SOFTKEY_BG_C1+ds_get_color_theme());
                        draw_partial_bitmap_image(AREA_SOFTKEY, xstart, 0, (OP_UINT16)(xlimit-xstart+1), skey_area_height, xstart, 0, pBitmap);
                    }
                }
                draw_partial_bitmap_image(AREA_SOFTKEY, xstart, 0, (OP_UINT16)(xlimit-xstart+1), skey_area_height, xstart, 0, pBitmap);
                xoffset = get_aligned_x_pos(xstart, xlimit, (OP_UINT8)(len * font_cx), (ALIGN_ENUM_T)(cur_skey.attr & 0x0f));

#if (PROJECT_ID == WHALE1_PROJECT)
                    sk_attr.fontColor = COLOR_GRASS_GREEN;
#else
                    sk_attr.fontColor = COLOR_NAVY_BLUE;   
#endif
                if(ds.cComposer.color_theme == COLOR_THEME_USER_DEFINE)
                {
                    fontcolor = ~ds.cComposer.softkeyColor;
                }
                else
                {
                    fontcolor = sk_attr.fontColor;
                }
                draw_text_line(AREA_SOFTKEY, xoffset, (OP_INT16)((skey_area_height-font_cy)/2), xlimit,
                    (OP_UINT8 *)cur_skey.txt,
                    OP_TRUE, 
                    (DS_FONTATTR)(sk_attr.fontAttr|FONT_OVERLAP), 
                    fontcolor, 
                    cur_skey.bgcolor);
            }
        }              

}

/*==================================================================================================
    FUNCTION: draw_softkeys 

    DESCRIPTION:
       Draw softkeys - predefined string, icon or both together and user specified string in center
       alligned in softkey area.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Only horizontal alignment is supported. Middle alignment is default vertial alignment.
==================================================================================================*/
void draw_softkeys 
( 
    DS_SKEYSET_T       *skey_ptr,
    DS_SKEYATTR_T      skey_attr
)
{ 
    OP_BOOLEAN     updated  = OP_FALSE;
    OP_INT16        xstart   = 0;         /* x start coordinate per softkey */
    OP_INT16        xlimit   = 0;         /* x end coordinate per softkey */
    DS_SKEYSET_T   skey     = *skey_ptr; /* Use local variable for speed access */

    /* Do not draw softkey in case of SKEYSTYLE_DXCAPTION. 
       Use ds_set_softkey_caption in this case */
    if ((skey.style &  SKEYSTYLE_DXCAPTION))
    {
        return;
    }

    /* draw left softkey */
    if (!(skey.left.attr & SKA_NOUSE) && skey.left.update) 
    {
        xstart = 0;
        if (ds_get_extend_softkey() == OP_FALSE)
        {
            xlimit = THREEKEY_S_SKEY_WIDTH -1;
        }
        else
        {
            xlimit = THREEKEY_S_IDLE_SKEY_WIDTH -1;
        }
        set_softkey(SOFTKEY_LEFT, skey_attr.skeyAttr, xstart, xlimit, skey.left);
        skey_ptr->left.update = OP_FALSE;
        updated = OP_TRUE;
    }
  

    /* draw center softkey - only exist in SKEYMODE_THREEKEY */
    if (!(skey.center.attr & SKA_NOUSE) && skey.center.update ) 
    {
        if (ds_get_extend_softkey() == OP_FALSE)
        {
            xstart = THREEKEY_S_SKEY_WIDTH;  /* after left key */
            xlimit = xstart + THREEKEY_C_SKEY_WIDTH -1;
        }
        else
        {
            xstart = THREEKEY_S_IDLE_SKEY_WIDTH; /* after left key */
            xlimit = xstart + THREEKEY_C_IDLE_SKEY_WIDTH -1;
        }
        set_softkey(SOFTKEY_CENTER, skey_attr.skeyAttr, xstart, xlimit, skey.center);
        skey_ptr->center.update = OP_FALSE;
        updated = OP_TRUE;      
    }
  
    /* draw right softkey - case of SKEYMODE_TWOKEY and SKEYMODE_THREEKEY*/
    if (!(skey.right.attr & SKA_NOUSE) && skey.right.update ) 
    {
        if (ds_get_extend_softkey() == OP_FALSE)
        {
            xstart = THREEKEY_C_SKEY_WIDTH + THREEKEY_S_SKEY_WIDTH;  /* after left and center key */
        }
        else
        {
            xstart = THREEKEY_C_IDLE_SKEY_WIDTH + THREEKEY_S_IDLE_SKEY_WIDTH; /* after left and center key */
        }
        xlimit = LCD_MAX_X_COOR;
        set_softkey(SOFTKEY_RIGHT, skey_attr.skeyAttr, xstart, xlimit, skey.right);
        skey_ptr->right.update = OP_FALSE;
        updated = OP_TRUE;      
    }
}


/*==================================================================================================
    FUNCTION: draw_softkey_caption 

    DESCRIPTION:
       Draw a softkey with the specified caption. One skey in the skey area.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void draw_softkey_caption 
( 
    OP_UINT8    *caption 
)
{
    OP_UINT8         leng         = 0;
    OP_UINT8         font_cx;
    OP_UINT8         font_cy;
    DS_SKEYATTR_T    skey_attr    = ds.scrnAttr.skey;
    OP_UINT8       skey_area_height;

    skey_area_height = (ds_get_extend_softkey() == OP_TRUE) ? LCD_EXT_SKEY_HIGH : LCD_SKEY_HIGH;
       
    draw_button(AREA_SOFTKEY, 0, 0, (OP_INT16)(LCD_MAX_X-1), (OP_INT16)(skey_area_height -1), 
                  skey_attr.skeyHLColor,skey_attr.skeyShadowColor,skey_attr.skeyAttr.bgColor);
    draw_line(AREA_SOFTKEY, 0, 0, (OP_INT16)(LCD_MAX_X-2),  0, skey_attr.skeyHLColor);
    draw_line(AREA_SOFTKEY, 0, 1, 0, 14, skey_attr.skeyHLColor);
    draw_line(AREA_SOFTKEY, (OP_INT16)(LCD_MAX_X-1), 0, (OP_INT16)(LCD_MAX_X-1), 14, skey_attr.skeyShadowColor);
  
    if (caption) 
    {
        //leng = MIN(SKEY_MAX_CHAR, op_strlen((char *)caption));
        if (Ustrlen(caption) > SKEY_MAX_CAP_CHAR)
        {
            Umemset(caption+SKEY_MAX_CAP_CHAR, 0x0000, 1);
        }
        leng = MIN(SKEY_MAX_CAP_CHAR, UstrCharCount(caption));
   
        /* Get skey font size */
        font_cx = font_get_font_width_w_fattr(skey_attr.skeyAttr.fontAttr);
        font_cy = font_get_font_height_w_fattr(skey_attr.skeyAttr.fontAttr);
    
        draw_text(AREA_SOFTKEY, (OP_INT16)((LCD_MAX_X-leng*font_cx)/2), (OP_INT16)((skey_area_height-font_cy)/2), LCD_MAX_X-1,
                   (OP_UINT8 *)caption, 
                   (DS_FONTATTR)(skey_attr.skeyAttr.fontAttr), skey_attr.skeyAttr.fontColor, 
                   skey_attr.skeyAttr.bgColor);
    }
    
    UPDATESET(AREAMASK_SKEY);
  
    /* Code */
}


/*==================================================================================================
    FUNCTION: clear_annuciator_buffer

    DESCRIPTION:
        Clear annuciator buffer with the annuciator background color.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        It is called when the non-displaying annuciaotr mode into displaying annuciator mode.

==================================================================================================*/
void clear_annuciator_buffer
(
    void
)
{
    if(ds.scrnMode == SCREEN_IDLE)
    {
        //fill_rect (AREA_ANNUN, 0,0, (OP_INT16)(LCD_MAX_X-1), (OP_INT16)(LCD_ANNUN_HIGH-1), ds.scrnAttr.annun.bgColor);
        op_memcpy(annun_screen, GetIdleImageBuffer(), LCD_MAX_X*LCD_ANNUN_HIGH*sizeof(DS_COLOR));
    }
    else
    {
        if(ds.cComposer.color_theme == COLOR_THEME_USER_DEFINE)
        {
            fill_rect(AREA_ANNUN, 0, 0, LCD_MAX_X_COOR, LCD_ANNUN_HIGH, ds.cComposer.annunColor);
        }
        else
        {
            draw_bitmap_image(AREA_ANNUN, 0, 0, (RM_BITMAP_T*)util_get_bitmap_from_res(BMP_ANNUCIATOR_BG1+ds_get_color_theme()));
        }
    }
}

/*==================================================================================================
    FUNCTION: draw_annunciator

    DESCRIPTION:
        Display or clear out icons in the annuciator area.

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
void draw_openwindow_annun
(
    DS_ANNUN_ENUM_T    annun,
    OP_BOOLEAN         onoff
)
{
#if (PROJECT_ID == WHALE1_PROJECT)
     OP_INT16           start_x=0;
     OP_INT16           start_y=0;
     RM_RESOURCE_ID_T   res_id=ICON_STATUS_OPENWIN_RSSI_0;
     RM_ICON_T          *icon_ptr   = OP_NULL;         
     
    /* draw_openwindow_annun() */
    switch(annun) 
    {
        case ANNUN_RSSI0:	
        case ANNUN_RSSI1:
        case ANNUN_RSSI2:
        case ANNUN_RSSI3:	
        case ANNUN_RSSI4:	
        case ANNUN_RSSI5:		
        case ANNUN_RSSI6:
            start_x = OPEN_WINDOW_ANNUN_START_X; //+OPENWINDOW_STATUS_ICON_WIDTH;
            start_y = LCD_TIMEDISPLAY_START_Y - LCD_OPENWINDOW_START_Y;
 /*           start_y = LCD_OPENWINDOW_START_Y + ds_get_image_height(ICON_OPENWIN_NUM_1) 
                         + 2*OPENWINDOW_STATUS_ICON_HEIGHT + OPENWINDOW_STATUS_DIANCHI_HEIGHT
                         + OPENWINDOW_STATUS_LINE_DOWN_Y+2;
   */         break;         

#if 0            
        case ANNUN_MISSED_CALL:
            start_x = OPEN_WINDOW_ANNUN_START_X;
            start_y = LCD_OPENWINDOW_START_Y + OPENWINDOW_STATUS_DIANCHI_HEIGHT;
            break;
#endif            
        case ANNUN_MSG:         
        case ANNUN_MMS:   
            start_x = OPEN_WINDOW_ANNUN_START_X + OPENWINDOW_STATUS_ICON_WIDTH;
            start_y = LCD_OPENWINDOW_START_Y;
            break;
#if 0            
		case ANNUN_QQ: 
            start_x = OPEN_WINDOW_ANNUN_START_X+OPENWINDOW_STATUS_ICON_WIDTH*2;
            start_y = LCD_OPENWINDOW_START_Y + ds_get_image_height(ICON_OPENWIN_NUM_1) 
                         + OPENWINDOW_STATUS_DIANCHI_HEIGHT + OPENWINDOW_STATUS_ICON_HEIGHT
                         + OPENWINDOW_STATUS_LINE_DOWN_Y;
            break;
            
        case ANNUN_ALARM:
            start_x = OPEN_WINDOW_ANNUN_START_X+OPENWINDOW_STATUS_ICON_WIDTH*2;
            start_y = LCD_OPENWINDOW_START_Y + OPENWINDOW_STATUS_DIANCHI_HEIGHT;
            break;
#endif

        
        case ANNUN_VIB:
		case ANNUN_NORMAL:
		case ANNUN_ALOUD:
		case ANNUN_INMOBILE:
		case ANNUN_EARPHONE:
        case ANNUN_VIB_BELL: 
		case ANNUN_MUTE:
	case ANNUN_SILENCE:
            start_x = OPEN_WINDOW_ANNUN_START_X + 2*OPENWINDOW_STATUS_ICON_WIDTH;
            start_y = LCD_OPENWINDOW_START_Y ;
            break;
            
      /*  case :   
            start_x = OPEN_WINDOW_ANNUN_START_X+OPENWINDOW_STATUS_ICON_WIDTH;
            start_y = LCD_OPENWINDOW_START_Y + ds_get_image_height(ICON_OPENWIN_NUM_1) 
                         + OPENWINDOW_STATUS_DIANCHI_HEIGHT + OPENWINDOW_STATUS_ICON_HEIGHT;
            res_id = ;
            ds_fill_rect(start_x, start_y, end_x,  
                               (OP_INT16)(start_y + OPENWINDOW_STATUS_ICON_HEIGHT),
                               COLOR_BLACK);
            draw_icon(AREA_OPEN_WINDOW, start_x, start_y, 
                         (RM_ICON_T *) util_get_icon_from_res(res_id)); 
            break;  */

	    case ANNUN_BATT0:
        case ANNUN_BATT1:
        case ANNUN_BATT2:
        case ANNUN_BATT3:
        case ANNUN_BATT4:
            start_x = OPEN_WINDOW_ANNUN_START_X + 3*OPENWINDOW_STATUS_ICON_WIDTH; //+OPENWINDOW_STATUS_ICON_WIDTH;
            start_y = LCD_OPENWINDOW_START_Y;
            break;

        default:
            return;
    }
       
    if ( onoff )
    {   
		res_id = (RM_RESOURCE_ID_T)(ICON_STATUS_OPENWIN_RSSI_0 + annun);
        icon_ptr = (RM_ICON_T *) util_get_icon_from_res(res_id);
	    fill_rect(AREA_OPEN_WINDOW,
                  start_x, 
                  start_y, 
                  (OP_INT16)(start_x + icon_ptr->biWidth),  
                  (OP_INT16)(start_y + icon_ptr->biHeight),
                  COLOR_BLACK);
        draw_icon(AREA_OPEN_WINDOW, 
                  start_x,
                  start_y, 
                  icon_ptr);      
    }
#else
     OP_INT16           start_x=0;
     OP_INT16           start_y=0;
     RM_RESOURCE_ID_T   res_id=ICON_STATUS_OPENWIN_RSSI_0;
     RM_ICON_T          *icon_ptr   = OP_NULL;   
    // RM_INT_ANIDATA_T    *ani_ptr = OP_NULL;
     
    /* draw_openwindow_annun() */
    switch(annun) 
    {
        case ANNUN_RSSI0:	
        case ANNUN_RSSI1:
        case ANNUN_RSSI2:
        case ANNUN_RSSI3:	
        case ANNUN_RSSI4:	
        case ANNUN_RSSI5:		
        case ANNUN_RSSI6:
            start_x = OPEN_WINDOW_ANNUN_START_X;
            start_y = LCD_TIMEDISPLAY_START_Y - OPENWINDOW_STATUS_ICON_HEIGHT - OPENWINDOW_STATUS_ICON_HEIGHT/2;
            break;         
           
        case ANNUN_MISSED_CALL:
            start_x = OPEN_WINDOW_ANNUN_START_X + OPENWINDOW_STATUS_ICON_WIDTH + OPENWINDOW_STATUS_ICON_WIDTH/2;
            start_y = LCD_TIMEDISPLAY_START_Y + ds_get_image_height(ICON_IDLE_TIMENUM_0) + OPENWINDOW_STATUS_ICON_HEIGHT/2;
            break;
            
        case ANNUN_MSG:         
        case ANNUN_MMS:
        case ANNUN_QQ:
            start_x = OPEN_WINDOW_ANNUN_START_X;
            start_y = LCD_TIMEDISPLAY_START_Y + ds_get_image_height(ICON_IDLE_TIMENUM_0) + OPENWINDOW_STATUS_ICON_HEIGHT/2;
            break;
            /*
            start_x = OPEN_WINDOW_ANNUN_START_X+OPENWINDOW_STATUS_ICON_WIDTH*2;
            start_y = LCD_OPENWINDOW_START_Y + ds_get_image_height(ICON_OPENWIN_NUM_1) 
                         + OPENWINDOW_STATUS_DIANCHI_HEIGHT + OPENWINDOW_STATUS_ICON_HEIGHT
                         + OPENWINDOW_STATUS_LINE_DOWN_Y;
                       */  
          //  break;
            
        case ANNUN_ALARM:
            start_x = OPEN_WINDOW_ANNUN_START_X + 3*OPENWINDOW_STATUS_ICON_WIDTH;
            start_y = LCD_TIMEDISPLAY_START_Y + ds_get_image_height(ICON_IDLE_TIMENUM_0) + OPENWINDOW_STATUS_ICON_HEIGHT/2;
            break;

        
        case ANNUN_VIB:
		case ANNUN_NORMAL:
		case ANNUN_ALOUD:
		case ANNUN_INMOBILE:
		case ANNUN_EARPHONE:
        case ANNUN_VIB_BELL: 
		case ANNUN_MUTE:
	case ANNUN_SILENCE:
            start_x = OPEN_WINDOW_ANNUN_START_X + OPENWINDOW_STATUS_ICON_WIDTH + OPENWINDOW_STATUS_ICON_WIDTH/2;
            start_y = LCD_TIMEDISPLAY_START_Y - OPENWINDOW_STATUS_ICON_HEIGHT - OPENWINDOW_STATUS_ICON_HEIGHT/2;
            break;
            
      /*  case :   
            start_x = OPEN_WINDOW_ANNUN_START_X+OPENWINDOW_STATUS_ICON_WIDTH;
            end_x = start_x + OPENWINDOW_STATUS_ICON_WIDTH;
            start_y = LCD_OPENWINDOW_START_Y + ds_get_image_height(ICON_OPENWIN_NUM_1) 
                         + OPENWINDOW_STATUS_DIANCHI_HEIGHT + OPENWINDOW_STATUS_ICON_HEIGHT;
            res_id = ;
            ds_fill_rect(start_x, start_y, end_x,  
                               (OP_INT16)(start_y + OPENWINDOW_STATUS_ICON_HEIGHT),
                               COLOR_BLACK);
            draw_icon(AREA_OPEN_WINDOW, start_x, start_y, 
                         (RM_ICON_T *) util_get_icon_from_res(res_id)); 
            break;  */

	    case ANNUN_BATT0:
        case ANNUN_BATT1:
        case ANNUN_BATT2:
        case ANNUN_BATT3:
        case ANNUN_BATT4:
            start_x = OPEN_WINDOW_ANNUN_START_X + 3*OPENWINDOW_STATUS_ICON_WIDTH;
            start_y = LCD_TIMEDISPLAY_START_Y - OPENWINDOW_STATUS_ICON_HEIGHT - OPENWINDOW_STATUS_ICON_HEIGHT/2;
            break;
        default:
            return;
    }
    res_id = (RM_RESOURCE_ID_T)(ICON_STATUS_OPENWIN_RSSI_0 + annun);
    icon_ptr = (RM_ICON_T *) util_get_icon_from_res(res_id);
	fill_rect(AREA_OPEN_WINDOW,
              start_x, 
              start_y, 
              (OP_INT16)(start_x + icon_ptr->biWidth),  
              (OP_INT16)(start_y + icon_ptr->biHeight),
              COLOR_BLACK);  
    if ( onoff )
    {       
    /*    ani_ptr = util_get_animation_from_res(ANI_OPENWINDOW);
        draw_partial_bitmap_image(AREA_BITMAP,
                                  start_x,
                                  start_y,
                                  icon_ptr->biWidth,
                                  icon_ptr->biHeight,                                  
                                  start_x,
                                  start_y,
                                  ani_ptr->scene);
     */  
        draw_icon(AREA_OPEN_WINDOW, 
                  start_x,
                  start_y, 
                  icon_ptr);      
   
   }
#endif
    /* update updated area flag */
    UPDATESET(AREAMASK_OPENWINDOW);  
}

/*==================================================================================================
    FUNCTION: draw_annunciator

    DESCRIPTION:
        Display or clear out icons in the annuciator area.

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
void draw_annunciator
(
    DS_ANNUN_ENUM_T    annun,
    OP_BOOLEAN         onoff
)
{
     OP_INT16           i; 
     OP_INT16           j;
     OP_INT16           start_x;
     OP_INT16           end_x;
     RM_RESOURCE_ID_T   res_id;
     RM_ICON_T          *icon;
     OP_UINT16          *icon_data;
    if(SP_get_power_on_status() == POWER_ON_STATUS_INIT)
    {
        return;
    }
    
    switch (annun)
    {
        case ANNUN_BATT0:  /* fall through */
        case ANNUN_BATT1:  /* fall through */
        case ANNUN_BATT2:  /* fall through */
        case ANNUN_BATT3:
        case ANNUN_BATT4:
            start_x =  ann_1_start_x ;
            end_x = ann_2_start_x - 1;
            break;
            
        case ANNUN_MSG:    // fall through 
        case ANNUN_MMS:    // fall through 
        case ANNUN_PUSH:
        case ANNUN_QQ:
        case ANNUN_VMAIL:
            start_x = ann_2_start_x;
            end_x = ann_3_start_x - 1;
            break;
            
        case ANNUN_SMS_PHONE_FULL:
        case ANNUN_SMS_SIM_FULL:
        case ANNUN_SMS_ALL_FULL:
            start_x = ann_3_start_x;
            end_x = ann_4_start_x - 1;
            break;
            
        case ANNUN_CONNECTED:         /* fall through */
        case ANNUN_MISSED_CALL:    /* fall through */
        case ANNUN_DIVERT:  
		case ANNUN_CSD:
            start_x = ann_4_start_x;
            end_x = ann_5_start_x - 1;
            break;

        case ANNUN_MUTE:         /* fall through */
        case ANNUN_VIB:
		case ANNUN_SILENCE:
		case ANNUN_NORMAL:
		case ANNUN_ALOUD:
		case ANNUN_INMOBILE:
		case ANNUN_EARPHONE:    
        case ANNUN_MELODY:
        case ANNUN_VIB_BELL:
            start_x = ann_5_start_x;
            end_x = ann_6_start_x - 1;
            break;
            
        case ANNUN_ALARM: //in calling, divert
        case ANNUN_ROAM: 
            start_x = ann_6_start_x;
            end_x = ann_7_start_x - 1;
            break;
            
        case ANNUN_GPRS:
        case ANNUN_GPRS_INACTIVE:
            start_x = ann_7_start_x;
            end_x = ann_8_start_x - 1;
            break;
            
        case ANNUN_RSSI0: /* fall through */
        case ANNUN_RSSI1: /* fall through */
        case ANNUN_RSSI2: /* fall through */
        case ANNUN_RSSI3: /* fall through */
        case ANNUN_RSSI4: /* fall through */
        case ANNUN_RSSI5: /* fall through */
        case ANNUN_RSSI6: 
            start_x = ann_8_start_x;
            end_x = ann_end_x - 1;
            break;                             
        default :
            start_x=0;
            end_x=0;
            break;
    }
    
    if(ds.scrnMode != SCREEN_IDLE)
    {
        if(ds.cComposer.color_theme == COLOR_THEME_USER_DEFINE)
        {
            fill_rect(AREA_ANNUN, start_x,0,end_x,
                      LCD_ANNUN_HIGH, ds.cComposer.annunColor);
        }
        else
        {
            draw_partial_bitmap_image(AREA_ANNUN, 
                              start_x, 
                              0,
                              (OP_INT16)(end_x - start_x + 1),
                              LCD_ANNUN_HIGH,
                              start_x,
                              0,
                              (RM_BITMAP_T*)util_get_bitmap_from_res(BMP_ANNUCIATOR_BG1+ds_get_color_theme()));
        }
    }
    else
    {
        OP_UINT16 *pBuffer = OP_NULL;
        OP_UINT16 *pStart = OP_NULL;
        
        pBuffer = GetIdleImageBuffer();
        for (i=0; i<LCD_ANNUN_HIGH;i++)
        {   
            pStart = pBuffer + i*LCD_MAX_X + start_x;
            op_memcpy(&annun_screen[i][start_x], pStart,(end_x-start_x+1)*sizeof(DS_COLOR));
        }
    }
    if (onoff)  
    {     
        res_id = (RM_RESOURCE_ID_T)(ICON_STATUS_RSSI_0 + annun);
        icon = util_get_icon_from_res(res_id);
        icon_data = (OP_UINT16 *)icon->data;
        for (i=0; i<LCD_ANNUN_HIGH;i++)
        {   
            for (j=start_x; j<=end_x; j++)
            {
                   if ( *(icon_data) != icon->trans ) 
                {
                    annun_screen[i][j] = *(icon_data);
                 }
                icon_data++;
            }
        }
    }

    /* update updated area flag */
    UPDATESET(AREAMASK_ANNUN);
}


/*==================================================================================================
    FUNCTION: draw_clock_hand

    DESCRIPTION:
        Draw Analog clock hand.

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
void draw_analog_clock
( 
    OP_INT16              x_center,
    OP_INT16              y_center,
    OP_INT16              h_len,
    OP_INT16              m_len,
    DS_COLOR              h_color,
    DS_COLOR              s_color,
    OP_UINT8              hour,  
    OP_UINT8              minute  
)
{  
    /* draw new time hands */
    draw_aclock_hands(x_center, y_center, h_len, m_len, h_color, s_color, hour, minute);   
}

/*==================================================================================================
    FUNCTION: copy_lcd_region

    DESCRIPTION:
        Copy the specified lcd buffer to the given buffer.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        It is important that the caller should pass the enough sized buffer to hold the specified 
        region.
==================================================================================================*/
void copy_lcd_region
(
   OP_UINT16    *dest_buf,
   OP_INT16     left,
   OP_INT16     top,
   OP_INT16     right,
   OP_INT16     bottom
)
{
    OP_INT16 y;
    OP_UINT16*  pDest = dest_buf;
    if (is_in_boundary(AREA_BITMAP, left, top, right, bottom))
    {
        for (y = top; y <= bottom; y++)
        {
            op_memcpy(pDest, &lcd_screen[y][left], (right - left + 1) * sizeof(DS_COLOR)); 
            pDest += right - left + 1;
        }        
    }
}

/*==================================================================================================
    FUNCTION: save_animation_icon_bg

    DESCRIPTION:
        backup the animation icon buffer into animal_screen.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        It is important that the caller should pass the enough sized buffer to hold the specified 
        region.
==================================================================================================*/
void save_animation_icon_bg
(
    SCRNAREA_ENUM_T    area,
    OP_INT16     left,
    OP_INT16     top,
    OP_INT16     right,
    OP_INT16     bottom
)
{
    OP_INT16    y, i;
    OP_INT16    height;
    
    height = bottom - top;
    wmemset((OP_UINT16 *)ani_bg, COLOR_WHITE, TITLE_ANI_ICON_WIDTH*TITLE_ANI_ICON_HEIGHT);
    if (is_in_boundary(area, left, top, right, bottom))
    {      
        switch(area)
        {
        case AREA_TEXT:
            for(y=top,i=0; (i<height && y<bottom); y++, i++)
            {
                op_memcpy((OP_UINT16*)&ani_bg[i][0], (OP_UINT16 *)&txt_screen[y][left], (right - left + 1) * sizeof(DS_COLOR)); 
            }
            break;
        case AREA_BITMAP:
            for(y=top,i=0; (i<height && y<bottom); y++, i++)
            {
                op_memcpy(&ani_bg[i][0], &bitmap_screen[y][left], (right - left + 1) * sizeof(DS_COLOR)); 
            }
            break;
        }
    }
}

/*==================================================================================================
    FUNCTION: set_animation_icon_bg

    DESCRIPTION:
        set the backup animation backgroud into the current screen.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        It is important that the caller should pass the enough sized buffer to hold the specified 
        region.
==================================================================================================*/
void set_animation_icon_bg
(
    SCRNAREA_ENUM_T    area,
    OP_INT16           left,
    OP_INT16           top,
    OP_INT16           right,
    OP_INT16           bottom
)
{
    OP_INT16    y, i;
    OP_INT16    height;

    height = bottom - top;
    if (is_in_boundary(area, left, top, right, bottom))
    {      
  
        switch (area) 
        {
            case AREA_TEXT:
                for(y=top,i=0; (i<height && y<bottom); y++, i++)
                {
                     op_memcpy( (OP_UINT16 *)&txt_screen[y][left], (OP_UINT16 *)&ani_bg[i][0],(right - left + 1) * sizeof(DS_COLOR)); 
                }
                break;
            case AREA_BITMAP:
                for(y=top,i=0; (i<height && y<bottom); y++, i++)
                {
                   op_memcpy((OP_UINT16 *)&bitmap_screen[y][left], &ani_bg[i],  (right - left + 1) * sizeof(DS_COLOR)); 
                }
                break;
        }
        /* update updated area flag and updated region */
        UPDATESET(area);
        add_update_region(area, left, top, right, bottom);
    }
}
/*================================================================================================*/

#ifdef __cplusplus
}
#endif
