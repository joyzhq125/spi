#ifdef __cplusplus
extern "C" {
#endif
/*==================================================================================================

    MODULE NAME : ds_util.c

    GENERAL DESCRIPTION
        This file contains funcation required to refresh to the LCD.

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    06/22/2002   Ashley Lee       crxxxxx     Initial creation
    08/20/2003   linda wang       P001293     Remove the local malloc and free bugs.    
    04/24/2004   linda wang       c005088     Add jpg idle image and power on/off animation.
    This file contains utility funcations used in the Display System.
        
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
#include    "ds_util.h"

/*==================================================================================================
    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static void cnvrtToAscii
(
    OP_UINT8     *converted_str,
    OP_UINT16    *unicode_str
);

/*==================================================================================================
    LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
    FUNCTION: cnvtToAscii

    DESCRIPTION:
        Convert Uincoded alpha numeric characters into ASCII code.      
        Other than alpha numeric characters are not totuched.  
        This is temporay function until DS system is updated to handle unicode.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        None.
==================================================================================================*/
void cnvrtToAscii
(
    OP_UINT8    *cnv_str_ptr,
    OP_UINT16    *unicode_str
)
{
    OP_UINT32    i;
    OP_UINT32    j;

    i = j = 0;

    while ( unicode_str[i] != 0x0000)
    {
        if ( unicode_str[i] <= 0x007f )
        {
            cnv_str_ptr[j++] = (OP_UINT8) (unicode_str[i] & 0x00ff);
        }
        else
        {
            cnv_str_ptr[j++] = (OP_UINT8) ((unicode_str[i] & 0xff00) >> 8);
            cnv_str_ptr[j++] = (OP_UINT8) (unicode_str[i] & 0x00ff);
        }
        i ++;
    }
    cnv_str_ptr[j] = 0x00;
}


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
void wmemset
(
    OP_UINT16    *dest, 
    OP_UINT16    src, 
    OP_INT32     n
)
{
    OP_UINT16    i;
    for (i=0;i<n;i++)
    {
        *(dest+i) = src;
    }
}

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
void util_strcpy
(
    char          * s_des,
    const char    * s_org
)
{
    if ( s_org == OP_NULL)
    {
        s_des[0] = 0x00;
    }
    else
    {
        op_strcpy(s_des, s_org);
    }
}

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
int util_strcmp_diff
(
    const char    * s1,
    const char    * s2
)
{
    if (( s1 == OP_NULL) || (s2 == OP_NULL))
    {
        return 0;
    }
    else
    {
        return (op_strcmp(s1, s2));
    }
}

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
OP_UINT32 util_strlen
(
    const char    * str
)
{
    if (str == OP_NULL)
    {
        return 0;
    }
    else
    {
        return (op_strlen(str));
    }
}


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
DS_RECT_T *util_add_rect_in_boundary 
(
    DS_RECT_T    *rect_a,
    DS_RECT_T    rect_b,
    OP_UINT8     max_x,
    OP_UINT8     max_y 
)
{
    DS_RECT_T added_rect = *rect_a;
   
    added_rect.left      = MIN(added_rect.left,   rect_b.left); 
    added_rect.top       = MIN(added_rect.top,    rect_b.top);
    added_rect.right     = MAX(added_rect.right,  rect_b.right); 
    added_rect.bottom    = MAX(added_rect.bottom, rect_b.bottom);

    rect_a->left         = MIN(added_rect.left,   max_x); 
    rect_a->top          = MIN(added_rect.top,    max_y);  
    rect_a->right        = MIN(added_rect.right,  max_x); 
    rect_a->bottom       = MIN(added_rect.bottom, max_y); 

    return rect_a;    
}


/*=================================================================================================
    FUNCTION: util_get_text_from_res_w_max_len 

    DESCRIPTION:
        Retrieve a string with the given resource id and copy upto assigned length.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Returned pointer pointing converted tring shall be de-allocated by the caller.
 ==================================================================================================*/
OP_UINT8 * util_get_text_from_res_w_max_len
(
    RM_RESOURCE_ID_T    res_id, 
    OP_UINT8            *txt_str,
    OP_INT16            max_len
)
{
    OP_UINT16             res_len;
    RM_RETURN_STATUS_T    rm_status;
      
    rm_status = RM_GetResourceLength(res_id, &res_len);

    if (rm_status == RM_SUCCESS)
    {
       res_len = MIN (res_len, max_len);
       rm_status = RM_GetResource (res_id, (OP_UINT16 *)txt_str, res_len);
       if (rm_status == RM_SUCCESS)
       {
           txt_str[res_len] = 0x00;
           txt_str[res_len+1] = 0x00;
       }
       else
       {
           txt_str[0] = 0x00;
           txt_str[1] = 0x00;
       }
    } 
    return txt_str;    
}

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
OP_UINT8 *util_cnvrt_res_into_text
(
    RM_RESOURCE_ID_T    res_id,
    OP_UINT8           *res_str
)
{
    OP_UINT16    res_len;
    RM_RETURN_STATUS_T    rm_status;
      
    rm_status = RM_GetResourceLength(res_id, &res_len);

    if (rm_status == RM_SUCCESS)
    {
#if 1
       rm_status = RM_GetResource (res_id, (OP_UINT16 *)res_str, res_len);
#else
       rm_status = RM_GetResourceInGivenLanguage(res_id, RM_LANG_CHN, (OP_UINT16 *)res_str, res_len);
#endif
       if (rm_status == RM_SUCCESS)
       {
           res_str[res_len] = 0x00;
           res_str[res_len+1] = 0x00;
       }
       else
       {
           res_str[0] = 0x00;
           res_str[1] = 0x00;
       }
    } 
    return res_str;
}


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
RM_BITMAP_T *util_get_bitmap_from_res
(
    RM_RESOURCE_ID_T    res_id
)
{
    RM_RETURN_STATUS_T    rm_status;
    RM_BUFFER_LENGTH_T    res_len = 4;
    OP_UINT8              res_addr[4];
    OP_UINT32             *res_bitmap_addr_ptr = OP_NULL;

    if (res_id != NULL_BITMAP)
    {
        rm_status = RM_GetResource( res_id, &res_addr, res_len);
        if (rm_status == RM_SUCCESS)
        {
            res_bitmap_addr_ptr = (OP_UINT32 *)(*((OP_UINT32*)(&res_addr)));  
        }
    }
    return ((RM_BITMAP_T *)(res_bitmap_addr_ptr));
}

/*=================================================================================================
    FUNCTION: util_get_image_from_res 

    DESCRIPTION:
        Retrieve image data from the given image resouce id.

    ARGUMENTS PASSED:

    RETURN VALUE:
 
    IMPORTANT NOTES:
        Here for jpg idle image.
 ==================================================================================================*/
RM_IMAGE_T *util_get_image_from_res
(
    RM_RESOURCE_ID_T    res_id
)
{
    RM_RETURN_STATUS_T    rm_status;
    RM_BUFFER_LENGTH_T    res_len = 4;
    OP_UINT8              res_addr[4];
    OP_UINT32             *res_image_addr_ptr = OP_NULL;

    if (res_id != NULL_RESOURCE)
    {
        rm_status = RM_GetResource( res_id, &res_addr, res_len);
        if (rm_status == RM_SUCCESS)
        {
            res_image_addr_ptr = (OP_UINT32 *)(*((OP_UINT32*)(&res_addr)));  
        }
    }
    return ((RM_IMAGE_T *)(res_image_addr_ptr));
}


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
RM_ICON_T *util_get_icon_from_res
(
    RM_RESOURCE_ID_T    res_id
)
{
    RM_RETURN_STATUS_T    rm_status;
    RM_BUFFER_LENGTH_T    res_len = 4;
    OP_UINT8              res_addr[4];
    OP_UINT32             *res_icon_addr_ptr = OP_NULL;

    if (res_id != NULL_ICON)
    {
        rm_status = RM_GetResource( res_id, &res_addr, res_len);
        if (rm_status == RM_SUCCESS)
        {
            res_icon_addr_ptr = (OP_UINT32 *)(*((OP_UINT32*)(&res_addr)));  
        }
    }
 
    return ((RM_ICON_T *)(res_icon_addr_ptr));

}

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
RM_SUBLCD_IMAGE_T *util_get_sublcd_image_from_res
(
    RM_RESOURCE_ID_T    res_id
)
{
    RM_RETURN_STATUS_T    rm_status;
    RM_BUFFER_LENGTH_T    res_len = 4;
    OP_UINT8              res_addr[4];
    OP_UINT32             *res_icon_addr_ptr = OP_NULL;

    if (res_id != NULL_ICON)
    {
        rm_status = RM_GetResource( res_id, &res_addr, res_len);
        if (rm_status == RM_SUCCESS)
        {
            res_icon_addr_ptr = (OP_UINT32 *)(*((OP_UINT32*)(&res_addr)));  
        }
    }
 
    return ((RM_SUBLCD_IMAGE_T *)(res_icon_addr_ptr));

}



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
RM_INT_ANIDATA_T *util_get_animation_from_res
(
    RM_RESOURCE_ID_T    res_id
)
{
    RM_RETURN_STATUS_T    rm_status;
    RM_BUFFER_LENGTH_T    res_len = 4;
    OP_UINT8              res_addr[4];
    OP_UINT32             *res_animation_addr_ptr = OP_NULL;

    if (res_id != NULL_ICON)
    {
        rm_status = RM_GetResource( res_id, &res_addr, res_len);
        if (rm_status == RM_SUCCESS)
        {
            res_animation_addr_ptr = (OP_UINT32 *)(*((OP_UINT32*)(&res_addr)));  
        }
    }
 
    return ((RM_INT_ANIDATA_T *)(res_animation_addr_ptr));

}

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
void util_set_rectangle
(
    DS_RECT_T    *rect,
    OP_INT16     left,
    OP_INT16     top,
    OP_INT16     right,
    OP_INT16     bottom
)
{
    rect->left   = left;
    rect->top    = top;
    rect->right  = right;
    rect->bottom = bottom;

}


/*================================================================================================*/

#ifdef __cplusplus
}
#endif
