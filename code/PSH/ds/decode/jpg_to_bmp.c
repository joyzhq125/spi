#ifdef __cplusplus
extern "C" {
#endif
/*==================================================================================================

    MODULE NAME : jpg_to_bmp.c

    GENERAL DESCRIPTION
        functions for unicode encoding string.

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    03/24/2002   Andyli                         Initial Creation
    07/26/2003   lindawang        P001074       remove animal_buffer.
    08/07/2003   shifeng          P001170        1.fixed bug of mapping Huffman Table
                                                   2.optimized jpeg decoding algorithm to save 
                                                    maximun 60k bytes heap memory space.
                                                   3.removed some unreferenced global and local variables.

    08/12/2003   shifeng          P001171       optimized decoding algorithm to generate bitmap data with right direction.
    08/19/2003   shifeng          P001304       fixed bug of displaying some jpeg file error
    09/11/2003   lindawang        P001562       fix bug of jpeg jpeg_decode.
    09/12/2003   shifeng          P001809        refactored
    05/24/2004   penghaibo        p005629       rewrite jpg decode code for support more format of jpg file
    06/05/2004   penghaibo        p005951       fix write bitmap bug and add fatal error handle
    06/09/2004   penghaibo	  p006046       add error handle and remove warnning and trace info
    06/12/2004   penghaibo	  p006146       fix reset bug where decode error data and save memery
    06/16/2004   penghaibo        p006216       add more error handle use longjmp
    Self-documenting Code
    Describe/explain low-level design of this module and/or group of funtions and/or specific
    funtion that are hard to understand by reading code and thus requires detail description.
    Free format !
        
====================================================================================================
    INCLUDE FILES
==================================================================================================*/

#include "APP_include.h"
#include "jpg_to_bmp.h"
#include "cdjpeg.h"


/*==================================================================================================
    LOCAL CONSTANTS
==================================================================================================*/



/*==================================================================================================
    LOCAL TYPEDEFS - Structure, Union, Enumerations...etc
==================================================================================================*/

/*==================================================================================================
    LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
    GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
    LOCAL VARIABLES
==================================================================================================*/


/*==================================================================================================
    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

OP_BOOLEAN GetJpgImage_Width_Height
(
    OP_UINT8     *data,
    OP_UINT32    data_size,
    OP_UINT16    *pWidth,
    OP_UINT16    *pHeight
)
{
    struct jpeg_decompress_struct *cinfo;
    struct jpeg_error_mgr *jerr;
    int jmp_ret;
    op_debug(DEBUG_HIGH,"JPG--GetJpgImage_Width_Height start................\n");
    cinfo = (struct jpeg_decompress_struct*)op_alloc(sizeof(struct jpeg_decompress_struct));
    if(cinfo == OP_NULL)
        {
        return OP_FALSE;
        }
    jerr = (struct jpeg_error_mgr *)op_alloc(sizeof(struct jpeg_error_mgr));
    if(jerr == OP_NULL)
        {
        return OP_FALSE;
        }
    jmp_ret = setjmp(jerr->mark );
    
    if (jmp_ret !=0)  /*some error where decode*/
        {
        op_debug(DEBUG_HIGH,"JPG--return from setjmp,error code :%d\n",jmp_ret);
        op_free(cinfo);
        op_free(jerr);
        return OP_FALSE;
        }
    
    cinfo->err = jpeg_std_error(jerr);
    jpeg_create_decompress(cinfo);   
    
    cinfo->dct_method = JDCT_IFAST;
    jpeg_stdio_src(cinfo, (unsigned char*)data,data_size);
    
    (void) jpeg_read_header(cinfo, TRUE); 
    *pWidth = cinfo->image_width;
    *pHeight = cinfo->image_height;
    
    jpeg_destroy_decompress(cinfo);
    
    op_free(cinfo);
    op_free(jerr);
    op_debug(DEBUG_HIGH,"JPG--GetJpgImage_Width_Height end..............\n");
    
    return OP_TRUE;
}




OP_BOOLEAN LoadJPGData
(
 OP_UINT8 *dataBuf, 
 OP_UINT32 data_size,
 RM_BITMAP_T *pDestBmp
)
{
    struct jpeg_decompress_struct *cinfo;
    struct jpeg_error_mgr *jerr;
    djpeg_dest_ptr dest_mgr = NULL;
    JDIMENSION num_scanlines;
    int jmp_ret;
    op_debug(DEBUG_HIGH,"JPG--start decoding................\n");
    cinfo = (struct jpeg_decompress_struct*)op_alloc(sizeof(struct jpeg_decompress_struct));
    if(cinfo == OP_NULL)
        {
        return OP_FALSE;
        }
    jerr = (struct jpeg_error_mgr *)op_alloc(sizeof(struct jpeg_error_mgr));
    if(jerr == OP_NULL)
        {
        return OP_FALSE;
        }
    
    jmp_ret = setjmp(jerr->mark );
    
    if (jmp_ret !=0)  /*some error where decode*/
        {
        op_debug(DEBUG_HIGH,"JPG--return from setjmp,error code :%d\n",jmp_ret);
        op_free(cinfo);
        op_free(jerr);
        return OP_FALSE;
        }
    
    cinfo->err = jpeg_std_error(jerr);
    jpeg_create_decompress(cinfo);   
    
    jpeg_stdio_src(cinfo, (unsigned char*)dataBuf,data_size);
    
    jpeg_read_header(cinfo, TRUE); 
    
    
    
    dest_mgr = jinit_write_bmp(cinfo, FALSE);
    pDestBmp->biBitCount = 16;   /* Specifies the number of bits per pixel.
                                 1 -- The bitmap is monochrome; 
                                 16 -- The bitmap has a maximum of 2^16 colors. 
    */
    pDestBmp->biWidth = cinfo->image_width;      /* Specifies the width of the bitmap, in pixels */
    pDestBmp->biHeight=cinfo->image_height;
    
    dest_mgr->des_buff = (unsigned char *)pDestBmp->data;
    
    cinfo->dct_method = JDCT_IFAST;
    (void) jpeg_start_decompress(cinfo);
    
    
    while (cinfo->output_scanline < cinfo->output_height)
        {
        num_scanlines = jpeg_read_scanlines(cinfo, dest_mgr->buffer,
            dest_mgr->buffer_height);
        (*dest_mgr->put_pixel_rows) (cinfo, dest_mgr, num_scanlines);
        }
    
    jpeg_finish_decompress(cinfo);
    jpeg_destroy_decompress(cinfo);
    
    op_free(cinfo);
    op_free(jerr);
    op_debug(DEBUG_HIGH,"JPG--end decoding..............\n");
    return OP_TRUE;
}


#ifdef __cplusplus
}
#endif

