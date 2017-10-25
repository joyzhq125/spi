#ifndef DS_FONT_H
#define DS_FONT_H

/*==================================================================================================

    HEADER NAME : ds_font.c

    GENERAL DESCRIPTION
        This file contains font glyth data and related functions.

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    06/24/2002   Ashley Lee       crxxxxx     Initial Creation
    01/07/2004   steven           P002325     delete some macros 
    04/04/2004   chouwangyun   p002825     change font for odin submenu
    05/19/2004  chouwangyun     c005496    modify DS module on new odin base 
    07/01/2004   liren            p006614  add 16*32 font library for dial font
This file contains font glyth data and related functions.


====================================================================================================
    INCLUDE FILES
==================================================================================================*/

#include	"ds_def.h"


/*==================================================================================================
    GLOBAL CONSTANTS
==================================================================================================*/
#define DS_ENG_FONTID_START          0
#define DS_ENG_FONT_COUNTS           3    /* SMALL, MEDIUM/LARGE Normal, MEDIUM/LARGE Bold */

#define DS_CHI_FONTID_START          3 /* DS_ENG_FONTID_START +  DS_ENG_FONT_COUNTS */
#define DS_CHI_FONT_COUNTS           2    /* SMALL, MEDIUM/LARGE */

#define DS_DIAL_FONTID_START         5 /* DS_CHI_FONTID_START + DS_CHI_FONT_COUNTS */
#define DS_DIAL_FONT_COUNTS          2    /* SMALL, MEDIUM/LARGE */

#define DS_DIAL_L_FONTID_START       7 /* DS_CHI_FONTID_START + DS_CHI_FONT_COUNTS */
#define DS_DIAL_L_FONT_COUNTS        1    /* SMALL, MEDIUM/LARGE */

#define DS_SMALL_FONTID              0
#define DS_MEDIUM_FONTID             1    /* MEDIUM and LARGE are same font */
#define DS_MEDIUM_BOLD_FONTID        2    /* MEDIUM and LARGE are same font */


#define DS_FONT_COUNTS               8    /* DS_ENG_FONT_COUNTS(3) + DS_CHI_FONT_COUNTS(2) + DS_DIAL_FONT_COUNTS(3) */
                                                     
#define DS_FONT_NAME_LEN            40


#define FONT16                         16
#define FONT12                         12

extern const OP_UINT8 ASCII_N_6_12[];
extern const OP_UINT8 ASCII_N_8_16[];
extern const OP_UINT8 ASCII_B_8_16[];
extern const OP_UINT8 DIAL_N_8_16[];
extern const OP_UINT8 DIAL_N_16_32[];
extern const OP_UINT8 DIAL_N_L_16_32[];

extern const OP_UINT8 GB2312_N_16_16[];
extern const OP_UINT8 GB2312_N_12_12[];

extern const OP_UINT16 GB2312_unicode_table[];


/*==================================================================================================
    GLOBAL DEFINITION 
==================================================================================================*/

typedef enum
{
    DS_ASCII,            /* 8bit character set */
    DS_GB2312,           /* 16bit GB2312 character set for simplefied Chiness */
    DS_BIG5,             /* 16 bit BIG5 character set for traditional Chinese */
    DS_UNICODE_CJK       /* 16bit Unicode CJK character set for Chinese */
} DS_ENCODE_ENUM_T;

/* Fixed Width Font Data Structure */
typedef struct 
{
    DS_FONT_ID          fontId;         /* Numerical Id for the font set */
    OP_UINT8            cx;             /* count of x of the font set */         
    OP_UINT8            cy;
    OP_UINT8            spacing;        /* 0 = fixed; 1 = proportional */
    OP_UINT8            style;          /* 0 = normal, 1 = bold, 2 = italic */
    DS_ENCODE_ENUM_T    encode;         /* Encoding type */
    const OP_UINT8      *pGlyphData;    /* starting address of the font set */
    OP_UINT8            fontName[DS_FONT_NAME_LEN+1];  /* name of the Font set */
} DS_FONT_HDR_T;

extern DS_FONT_HDR_T      fonts[DS_FONT_COUNTS];    /* System fixed width fonts set array */

/*==================================================================================================
    GLOBAL FUNCTIONS
==================================================================================================*/

/*=================================================================================================
    FUNCTION: get_font_id 

    DESCRIPTION:
        Returns the system font ids (both English and Chiness) with the given font attribute.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None
==================================================================================================*/
extern void font_get_font_id 
(
    DS_FONTATTR fontattr,
    DS_FONT_ID *eng_fontid,
    DS_FONT_ID *chi_fontid    
);


/*=================================================================================================
    FUNCTION: get_font_size 

    DESCRIPTION:
        Returns the width and height of the given font.

    ARGUMENTS PASSED:
        font_width and font_height are returned parameters.

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        If invalid font id is provided, returns FALSE. Otherwise returns TRUE.
==================================================================================================*/
extern OP_BOOLEAN font_get_font_size 
(
    DS_FONT_ID fontid,
    OP_UINT8 *font_width,
    OP_UINT8 *font_height

);


/*=================================================================================================
    FUNCTION: font_get_font_height 

    DESCRIPTION:
        Returns the height of the given font.

    ARGUMENTS PASSED:

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        If invalid font id is provided, returns FALSE. Otherwise returns TRUE.
==================================================================================================*/
OP_UINT8 font_get_font_height 
(
    DS_FONT_ID fontid
);


/*=================================================================================================
    FUNCTION: font_get_font_width 

    DESCRIPTION:
        Returns the height of the given font.

    ARGUMENTS PASSED:

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        If invalid font id is provided, returns FALSE. Otherwise returns TRUE.
==================================================================================================*/
OP_UINT8 font_get_font_width
(
    DS_FONT_ID fontid
);


/*=================================================================================================
    FUNCTION: font_get_font_width_w_fattr 

    DESCRIPTION:
        Returns the width with the given font attribute.

    ARGUMENTS PASSED:

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        If invalid font id is provided, returns FALSE. Otherwise returns TRUE.
==================================================================================================*/
OP_UINT8 font_get_font_width_w_fattr 
(
    DS_FONTATTR font_attr
);

/*=================================================================================================
    FUNCTION: font_get_font_height_w_fattr 

    DESCRIPTION:
        Returns the height with the given font attribute.

    ARGUMENTS PASSED:

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        If invalid font id is provided, returns FALSE. Otherwise returns TRUE.
==================================================================================================*/
OP_UINT8 font_get_font_height_w_fattr 
(
    DS_FONTATTR font_attr
);


/*=================================================================================================
    FUNCTION: get_fontset_glyph

    DESCRIPTION:
        Returns the staring address of the glyph data of the given font id.
        The exact address of a charcter in the font set can be calculated with this returned 
        address since it is fixed width character set.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None
==================================================================================================*/
extern const OP_UINT8 *font_get_fontset_glyph
(
    DS_FONT_ID fontid
);

/*=================================================================================================
    FUNCTION: init_fontsets

    DESCRIPTION:
        Initialize system support font sets according to the system defined font structure.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None
==================================================================================================*/
extern void font_init_fontsets
(
    void
);



#endif /* DS_FONT_H */
/*================================================================================================*/
