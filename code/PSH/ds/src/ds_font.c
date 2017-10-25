#ifdef __cplusplus
extern "C" {
#endif
/*==================================================================================================

    HEADER NAME : ds_font.c

    GENERAL DESCRIPTION
        This file contains font related functions.

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    07/24/2002   Ashley Lee       crxxxxx     Initial Creation
    04/04/2004   chouwangyun      p002825     change font for odin submenu
    07/01/2004   liren            p006614     add 16*32 font for dial      
    This file contains font related functions.


====================================================================================================
    INCLUDE FILES
==================================================================================================*/
#ifdef WIN32
#include "windows.h"
#include "portab_new.h"
#else
#include "portab.h"
#endif

#include "rm_include.h"
#include "SP_sysutils.h"

#include "ds_font.h"
/*==================================================================================================
    GLOBAL VARIABLES
==================================================================================================*/
DS_FONT_HDR_T      fonts[DS_FONT_COUNTS];    /* System fixed width fonts set array */

/*==================================================================================================
    GLOBAL FUNCTIONS
==================================================================================================*/

/*=================================================================================================
    FUNCTION: font_get_font_id 

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
void font_get_font_id 
(
    DS_FONTATTR fontattr,
    DS_FONT_ID *eng_fontid,
    DS_FONT_ID *chi_fontid    
)
{

   OP_BOOLEAN is_small_font   = (fontattr & FONT_SIZE_SMALL) ? OP_TRUE : OP_FALSE;
   OP_BOOLEAN is_dialing_font = (fontattr & FONT_DIAL) ? OP_TRUE : OP_FALSE;
   OP_BOOLEAN is_bold_font      = (fontattr & FONT_BOLD) ? OP_TRUE : OP_FALSE;
   OP_BOOLEAN is_dialing_l_font = (fontattr & FONT_DIAL_L) ? OP_TRUE : OP_FALSE;


   
   if (is_dialing_font)
   {
       if (is_small_font)
       { /* Same id for dialing font */
           *eng_fontid =  DS_DIAL_FONTID_START + DS_SMALL_FONTID;
           *chi_fontid =  DS_DIAL_FONTID_START + DS_SMALL_FONTID; 
       }
       else /* MEDIUM or LARGE - Currently use same font */
       { /* Same id for dialing font */
           *eng_fontid =  DS_DIAL_FONTID_START + DS_MEDIUM_FONTID;
           *chi_fontid =  DS_DIAL_FONTID_START + DS_MEDIUM_FONTID; 
       }
   }
   else if (is_dialing_l_font)
   {       /* 16*32 FONT SIZE IN THE QUICK DIAL EDIT */
           *eng_fontid =  DS_DIAL_L_FONTID_START + DS_SMALL_FONTID;
           *chi_fontid =  DS_DIAL_L_FONTID_START + DS_SMALL_FONTID; 
   }
   else 
   {
       if (is_small_font)
       { 
           *eng_fontid =  DS_ENG_FONTID_START + DS_SMALL_FONTID;
           *chi_fontid =  DS_CHI_FONTID_START + DS_SMALL_FONTID; 
       }
       else /* MEDIUM or LARGE - Currently use same font */
       { 
           if (is_bold_font) /* bold font is supported in normal size ascii only */
           {
               *eng_fontid =  DS_ENG_FONTID_START + DS_MEDIUM_BOLD_FONTID;
           }   
           else
           {
               *eng_fontid =  DS_ENG_FONTID_START + DS_MEDIUM_FONTID;
           }
           *chi_fontid =  DS_CHI_FONTID_START + DS_MEDIUM_FONTID; 
       }
   }
 
}

/*=================================================================================================
    FUNCTION: font_get_font_size 

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
OP_BOOLEAN font_get_font_size 
(
    DS_FONT_ID fontid,
    OP_UINT8 *font_width,
    OP_UINT8 *font_height

)
{
    OP_BOOLEAN is_valid_fontid;;

    /* Is it valid font id */
    if (fontid < DS_FONT_COUNTS)
    {
        is_valid_fontid = OP_TRUE;
        *font_width = fonts[fontid].cx;
        *font_height = fonts[fontid].cy;
    }
    else
    {
        is_valid_fontid = OP_FALSE;
        *font_width = 0;
        *font_height = 0;
    }

    return is_valid_fontid;
}


/*=================================================================================================
    FUNCTION: font_get_font_width 

    DESCRIPTION:
        Returns the width of the given font.

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
)
{
    OP_BOOLEAN is_valid_fontid;

    OP_UINT8 font_width;
    OP_UINT8 font_height;

    is_valid_fontid = font_get_font_size(fontid, &font_width, &font_height);     
    if (!is_valid_fontid)
    {
        font_width = 0;
    }

    return font_width;

}

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
)
{

    DS_FONT_ID eng_font_id;
    DS_FONT_ID chi_font_id;

    font_get_font_id(font_attr, &eng_font_id, &chi_font_id);

    return font_get_font_width(eng_font_id);  
}


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
)
{
    OP_BOOLEAN is_valid_fontid;

    OP_UINT8 font_width;
    OP_UINT8 font_height;

    is_valid_fontid = font_get_font_size(fontid, &font_width, &font_height);     
    if (!is_valid_fontid)
    {
        font_height = 0;
    }

    return font_height;

}

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
)
{

    DS_FONT_ID eng_font_id;
    DS_FONT_ID chi_font_id;

    font_get_font_id(font_attr, &eng_font_id, &chi_font_id);

    return font_get_font_height(eng_font_id);  
}


/*=================================================================================================
    FUNCTION: font_get_fontset_glyph

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
const OP_UINT8 *font_get_fontset_glyph
(
    DS_FONT_ID fontid
)
{
    const OP_UINT8   *pGlyphData; 
    switch (fontid)
    {
        case (DS_ENG_FONTID_START+DS_SMALL_FONTID):
            pGlyphData = fonts[DS_ENG_FONTID_START+DS_SMALL_FONTID].pGlyphData;
            break;
        case (DS_ENG_FONTID_START+DS_MEDIUM_FONTID):
            pGlyphData = fonts[DS_ENG_FONTID_START+DS_MEDIUM_FONTID].pGlyphData;
            break;
        case (DS_ENG_FONTID_START+DS_MEDIUM_BOLD_FONTID):
            pGlyphData = fonts[DS_ENG_FONTID_START+DS_MEDIUM_BOLD_FONTID].pGlyphData;
            break;

        case (DS_CHI_FONTID_START+DS_SMALL_FONTID):
            pGlyphData = fonts[DS_CHI_FONTID_START+DS_SMALL_FONTID].pGlyphData;
            break;
        case (DS_CHI_FONTID_START+DS_MEDIUM_FONTID):
            pGlyphData = fonts[DS_CHI_FONTID_START+DS_MEDIUM_FONTID].pGlyphData;
            break;

        case (DS_DIAL_FONTID_START+DS_SMALL_FONTID):
            pGlyphData = fonts[DS_DIAL_FONTID_START+DS_SMALL_FONTID].pGlyphData;
            break;
        case (DS_DIAL_FONTID_START+DS_MEDIUM_FONTID):
            pGlyphData = fonts[DS_DIAL_FONTID_START+DS_MEDIUM_FONTID].pGlyphData;
            break;
        case (DS_DIAL_L_FONTID_START+DS_SMALL_FONTID):
            pGlyphData = fonts[DS_DIAL_L_FONTID_START+DS_SMALL_FONTID].pGlyphData;
            break;

        default :
            pGlyphData = OP_NULL;
            break;
    }

    return pGlyphData;
}


/*=================================================================================================
    FUNCTION: font_init_fontsets

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
void font_init_fontsets
(
    void
)
{

    /* English Font sets */
    /* ASCII SMALL NORMAL FIXED 6 by 12*/
    {
        fonts[DS_ENG_FONTID_START].fontId = DS_ENG_FONTID_START+DS_SMALL_FONTID;      
        fonts[DS_ENG_FONTID_START].cx = 6;  
        fonts[DS_ENG_FONTID_START].cy = 12;     
        fonts[DS_ENG_FONTID_START].spacing = 0;                             /* Fixed width */  
        fonts[DS_ENG_FONTID_START].encode = DS_ASCII;                          /* ASCII - English */
        op_memcpy (fonts[DS_ENG_FONTID_START+DS_SMALL_FONTID].fontName, "ASCII NORMAL 6 by 12", 
                sizeof("ASCII NORMAL 6 by 12"));
        fonts[DS_ENG_FONTID_START+DS_SMALL_FONTID+DS_SMALL_FONTID].pGlyphData = ASCII_N_6_12;  
    }
 
    /* ASCII MEDIUM NORMAL FIXED 8 by 16 */
    {
        fonts[DS_ENG_FONTID_START+DS_MEDIUM_FONTID].fontId = DS_ENG_FONTID_START+DS_MEDIUM_FONTID;      
        fonts[DS_ENG_FONTID_START+DS_MEDIUM_FONTID].cx = 8;  
        fonts[DS_ENG_FONTID_START+DS_MEDIUM_FONTID].cy = 16;     
        fonts[DS_ENG_FONTID_START+DS_MEDIUM_FONTID].spacing = 0;                    /* Fixed width */  
        fonts[DS_ENG_FONTID_START+DS_MEDIUM_FONTID].encode = DS_ASCII;              /* ASCII - English */
        op_memcpy (fonts[DS_ENG_FONTID_START+DS_MEDIUM_FONTID].fontName, "ASCII NORMAL 8 by 16",
                 sizeof("ASCII NORMAL 8 by 16"));
        fonts[DS_ENG_FONTID_START+DS_MEDIUM_FONTID].pGlyphData = ASCII_N_8_16;  
    }
 
    /* ASCII MEDIUM BOLD FIXED 8 by 16 */
    {
        fonts[DS_ENG_FONTID_START+DS_MEDIUM_BOLD_FONTID].fontId = DS_ENG_FONTID_START+DS_MEDIUM_BOLD_FONTID;      
        fonts[DS_ENG_FONTID_START+DS_MEDIUM_BOLD_FONTID].cx = 8;  
        fonts[DS_ENG_FONTID_START+DS_MEDIUM_BOLD_FONTID].cy = 16;     
        fonts[DS_ENG_FONTID_START+DS_MEDIUM_BOLD_FONTID].spacing = 0;                /* Fixed width */  
        fonts[DS_ENG_FONTID_START+DS_MEDIUM_BOLD_FONTID].encode = DS_ASCII;          /* ASCII - English */
        op_memcpy (fonts[DS_ENG_FONTID_START+DS_MEDIUM_BOLD_FONTID].fontName, "ASCII BOLD 8 by 16",
                 sizeof("ASCII BOLD 8 by 16"));
        fonts[DS_ENG_FONTID_START+DS_MEDIUM_BOLD_FONTID].pGlyphData = ASCII_B_8_16;  
    }


    /* Chiness Font sets */
    /* GB2312 SMALL NORMAL FIXED 12 by 12*/
    {
        fonts[DS_CHI_FONTID_START+DS_SMALL_FONTID].fontId = DS_CHI_FONTID_START+DS_SMALL_FONTID;      
        fonts[DS_CHI_FONTID_START+DS_SMALL_FONTID].cx = 12;  
        fonts[DS_CHI_FONTID_START+DS_SMALL_FONTID].cy = 12;     
        fonts[DS_CHI_FONTID_START+DS_SMALL_FONTID].spacing = 0;                  /* Fixed width */  
        fonts[DS_CHI_FONTID_START+DS_SMALL_FONTID].encode = DS_GB2312;           /* CHINESS  GB2312 */
        op_memcpy (fonts[DS_CHI_FONTID_START+DS_SMALL_FONTID].fontName, "CHINESS GB2312 NORMAL 12 by 12", 
                sizeof("CHINESS GB2312 NORMAL 12 by 12"));
        fonts[DS_CHI_FONTID_START+DS_SMALL_FONTID].pGlyphData = GB2312_N_12_12;  
    }

    /* GB2312 MEDIUM NORMAL FIXED 16 by 16*/
    {
        fonts[DS_CHI_FONTID_START+DS_MEDIUM_FONTID].fontId = DS_CHI_FONTID_START+DS_MEDIUM_FONTID;      
        fonts[DS_CHI_FONTID_START+DS_MEDIUM_FONTID].cx = 16;  
        fonts[DS_CHI_FONTID_START+DS_MEDIUM_FONTID].cy = 16;     
        fonts[DS_CHI_FONTID_START+DS_MEDIUM_FONTID].spacing = 0;                     /* Fixed width */  
        fonts[DS_CHI_FONTID_START+DS_MEDIUM_FONTID].encode = DS_GB2312;              /* CHINESE  GB2312 */
        op_memcpy (fonts[DS_CHI_FONTID_START+DS_MEDIUM_FONTID].fontName, "CHINESE GB2312 NORMAL 16 by 16", 
                sizeof("CHINESE GB2312 NORMAL 16 by 16"));
        fonts[DS_CHI_FONTID_START+DS_MEDIUM_FONTID].pGlyphData = GB2312_N_16_16;  
    }

    /* Dialing digit Font sets */
    /* DIAL SMALL NORMAL FIXED 8 by 16*/
    {
        fonts[DS_DIAL_FONTID_START+DS_SMALL_FONTID].fontId = DS_DIAL_FONTID_START+DS_SMALL_FONTID;      
        fonts[DS_DIAL_FONTID_START+DS_SMALL_FONTID].cx = 8;  
        fonts[DS_DIAL_FONTID_START+DS_SMALL_FONTID].cy = 16;     
        fonts[DS_DIAL_FONTID_START+DS_SMALL_FONTID].spacing = 0;                   /* Fixed width */  
        fonts[DS_DIAL_FONTID_START+DS_SMALL_FONTID].encode = DS_ASCII;             /* ASCII - English */
        op_memcpy (fonts[DS_DIAL_FONTID_START+DS_SMALL_FONTID].fontName, "ASCII DIAL NORMAL 8 by 16", 
                sizeof("ASCII DIAL NORMAL 8 by 16"));
        fonts[DS_DIAL_FONTID_START+DS_SMALL_FONTID].pGlyphData = DIAL_N_8_16;  
    }
 
    /* DIAL SMALL NORMAL FIXED 16 by 32*/
    {
        fonts[DS_DIAL_FONTID_START+DS_MEDIUM_FONTID].fontId = DS_DIAL_FONTID_START+DS_MEDIUM_FONTID;      
        fonts[DS_DIAL_FONTID_START+DS_MEDIUM_FONTID].cx = 16;  
        fonts[DS_DIAL_FONTID_START+DS_MEDIUM_FONTID].cy = 32;     
        fonts[DS_DIAL_FONTID_START+DS_MEDIUM_FONTID].spacing = 0;                 /* Fixed width */  
        fonts[DS_DIAL_FONTID_START+DS_MEDIUM_FONTID].encode = DS_ASCII;           /* ASCII - English */
        op_memcpy (fonts[DS_DIAL_FONTID_START+1].fontName, "ASCII DIAL NORMAL 16 by 32", 
                sizeof("ASCII DIAL NORMAL 16 by 32"));
        fonts[DS_DIAL_FONTID_START+DS_MEDIUM_FONTID].pGlyphData = DIAL_N_16_32;  
    }
    /* DIAL LARGE NORMAL FIXED 16 by 32*/
    {
        fonts[DS_DIAL_L_FONTID_START+DS_SMALL_FONTID].fontId = DS_DIAL_L_FONTID_START+DS_SMALL_FONTID;      
        fonts[DS_DIAL_L_FONTID_START+DS_SMALL_FONTID].cx = 16;  
        fonts[DS_DIAL_L_FONTID_START+DS_SMALL_FONTID].cy = 32;     
        fonts[DS_DIAL_L_FONTID_START+DS_SMALL_FONTID].spacing = 0;                 /* Fixed width */  
        fonts[DS_DIAL_L_FONTID_START+DS_SMALL_FONTID].encode = DS_ASCII;           /* ASCII - English */
        op_memcpy (fonts[DS_DIAL_FONTID_START+1].fontName, "ASCII DIAL NORMAL 16 by 32", 
                sizeof("ASCII DIAL NORMAL 16 by 32"));
        fonts[DS_DIAL_L_FONTID_START+DS_SMALL_FONTID].pGlyphData = DIAL_N_L_16_32;  
    }


}

#ifdef __cplusplus
}
#endif
/*================================================================================================*/
