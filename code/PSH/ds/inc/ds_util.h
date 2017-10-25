#ifndef DS_UTIL_H
#define DS_UTIL_H
/*==================================================================================================

    MODULE NAME : ds_util.h

    GENERAL DESCRIPTION
        This file contains display system utility funcation.

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    06/22/2002   Ashley Lee       crxxxxx     Initial creation
    04/24/2004   linda wang       c005088     Add jpg idle image and power on/off animation.
    This file contains display system utility funcation.
        
====================================================================================================
    INCLUDE FILES
==================================================================================================*/

#include	"ds_def.h"


/*==================================================================================================
    GLOBAL MACROS
==================================================================================================*/
#define MIN(a,b)       ( (a)<(b) ? (a) : (b) )
#define MAX(a,b)       ( (a)>(b) ? (a) : (b) )
#define ABS(a)         ( (a)>=0 ? (a) : (-1) *(a))
#define WIDTH(rect)    ( ((rect).right) - ((rect).left) )
#define HEIGHT(rect)   ( ((rect).bottom) - ((rect).top) )

//#define IS_NOT_NULL_STRING(txt)    ( ((txt != OP_NULL) && txt[0] != 0x00) ? OP_TRUE : OP_FALSE)
#define IS_NOT_NULL_STRING(txt)    ( ((txt[0] == 0x00) && (txt[1] == 0x00)) ? OP_FALSE : OP_TRUE)

/* center coordinate */
#define GET_CENTER(start, end, width) ( ((end-start+1) > width) ? ((start+end-width+2)/2) : start)
#define GET_FONT_WIDTH(fontattr)      ((fontattr & FONT_SIZE_SMALL) ? FONTX_SMALL : FONTX_NORMAL)


/*==================================================================================================
    GLOBAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
    FUNCTION: memset2

    DESCRIPTION:
        memory set util function to set word data.

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
extern void wmemset
(
    OP_UINT16    *dest, 
    OP_UINT16    src, 
    OP_INT32     n
);


/*==================================================================================================
    FUNCTION: util_strcpy

    DESCRIPTION:
        string copy checking whether the souce buffer is null or not.

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
extern void util_strcpy
(
    char          * s_des,
    const char    * s_org
);

/*==================================================================================================
    FUNCTION: util_strcmp_diff

    DESCRIPTION:
        Compare two string.
        If one of a string is null,returns 0 (regard two staring is same).

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments.
        Returns 0 if two strings are same or one of the string is a null string.

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important aspect of using
        the function i.e side effect..etc
==================================================================================================*/
extern int util_strcmp_diff
(
    const char    * s1,
    const char    * s2
);


/*==================================================================================================
    FUNCTION: util_strlen

    DESCRIPTION:
        Compare two string.
        If one of a string is null,returns 0 (regard two staring is same).

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments.
        Returns 0 if two strings are same or one of the string is a null string.

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important aspect of using
        the function i.e side effect..etc
==================================================================================================*/
extern OP_UINT32 util_strlen
(
    const char    * str
);


/*=================================================================================================
    FUNCTION: util_add_rect_in_boundary 

    DESCRIPTION:
        Add two given rectangle and apply the given boundaries.

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
extern DS_RECT_T *util_add_rect_in_boundary 
(
    DS_RECT_T    *rect_a,
    DS_RECT_T    rect_b,
    OP_UINT8     max_x,
    OP_UINT8     max_y 
);


/*=================================================================================================
    FUNCTION: util_get_text_from_res_w_max_len 

    DESCRIPTION:
        Retrieve a string with the given resource id and copy within the assigned length.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Returned pointer pointing converted tring shall be de-allocated by the caller.
 ==================================================================================================*/
extern OP_UINT8 * util_get_text_from_res_w_max_len
(
    RM_RESOURCE_ID_T    res_id, 
    OP_UINT8            *txt_str,
    OP_INT16            max_len
);

/*=================================================================================================
    FUNCTION: util_cnvrt_res_into_text 

    DESCRIPTION:
        Retrieve a string with the given resource id.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Returned pointer pointing converted tring shall be de-allocated by the caller.
 ==================================================================================================*/
extern OP_UINT8 *util_cnvrt_res_into_text
(
    RM_RESOURCE_ID_T    res_id,
    OP_UINT8            *cnv_str
);

/*=================================================================================================
    FUNCTION: util_get_bitmap_from_res 

    DESCRIPTION:
        Retrieve bitmap data from the given bitmap resouce id.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
 ==================================================================================================*/
extern RM_BITMAP_T *util_get_bitmap_from_res
(
    RM_RESOURCE_ID_T    res_id
);

/*=================================================================================================
    FUNCTION: util_get_image_from_res 

    DESCRIPTION:
        Retrieve image data from the given image resouce id.

    ARGUMENTS PASSED:

    RETURN VALUE:

    IMPORTANT NOTES:
        Here for jpg idle image.
 ==================================================================================================*/
extern RM_IMAGE_T *util_get_image_from_res
(
    RM_RESOURCE_ID_T    res_id
);
/*=================================================================================================
    FUNCTION: util_get_icon_from_res 

    DESCRIPTION:
        Retrieve icon bitmap data from the given bitmap resouce id.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
 ==================================================================================================*/
extern RM_ICON_T *util_get_icon_from_res
(
    RM_RESOURCE_ID_T    res_id
);

/*=================================================================================================
    FUNCTION: util_get_sublcd_image_from_res 

    DESCRIPTION:
        Retrieve icon bitmap data from the given bitmap resouce id.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
 ==================================================================================================*/
extern RM_SUBLCD_IMAGE_T *util_get_sublcd_image_from_res
(
    RM_RESOURCE_ID_T    res_id
);


/*=================================================================================================
    FUNCTION: util_get_animation_from_res 

    DESCRIPTION:
        Retrieve animation data from the given bitmap resouce id.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
 ==================================================================================================*/
extern RM_INT_ANIDATA_T *util_get_animation_from_res
(
    RM_RESOURCE_ID_T    res_id
);

/*=================================================================================================
    FUNCTION: util_set_rectangle 

    DESCRIPTION:
        Retrieve icon bitmap data from the given bitmap resouce id.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
 ==================================================================================================*/
extern void util_set_rectangle
(
    DS_RECT_T    *rect,
    OP_INT16     left,
    OP_INT16     top,
    OP_INT16     right,
    OP_INT16     bottom
);



/*================================================================================================*/
#endif /* DS_UTIL_H */

