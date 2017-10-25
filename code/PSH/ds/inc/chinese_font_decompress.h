#ifndef CHINESE_FONT_DECOMPRESS /* To prevent inadvertently including a header twice */
#define CHINESE_FONT_DECOMPRESS


/*************************************************************************
                                                                          
   SEF CONFIDENTIAL AND PROPRIETARY                                       
                                                                          
   This source is the sole property of SEF Inc. Reproduction or           
   utilization of this source in whole or in part is forbidden without    
   the written consent of SEF Inc.                                        
                                                                          
   (c) Copyright SEF Inc.,         2003.  All Rights Reserved.    
*/
/*******************************************************************************
 Date       Author           Reference
   ========   ========         ==========================
   03-02-27   Chenyu           CR    ----    PC00011 
              - Initial creation 
               decompress 14x15 compressed GB0 bitmap

   03-03-31   Chenyu           PR    ----    P000023 
              - Add  symbol font library  of chinese 16 &12pixel 
   04/13/2004  lindawang       p002976    remove compressed font
   05/19/2004  chouwangyun     c005496    modify DS module on new odin base 
********************************************************************************/

/**************************Dependencies ***************************************/

#include    "OPUS_typedef.h"


/************************ Global Definitions and Declarations ****************/
#define COMP_SIZE_OF_TAG               12      /* size of tag in  the compressed font library         */
#define COMP_FONT12_SIZE_OF_INDEX1     (241*4+3846+2)
#define COMP_SIZE_OF_MODEL1            4096    /* size of model1 in  the compressed font library      */
#define COMP_SIZE_OF_MODEL2            512     /* size of model2 in  the compressed font library      */
#define GB0_FONT12_SEGMENT_LENGTH      964     /* 6763 GB0 is divided into 212 segment by 32 character             
                                                         each, each segment occupies 4 bytes                 */    
#define COMP_SYMBOL_SIZE_OF_INDEX      588
#define SMYBOL_SEGMENT_LENGTH          120 

#define COMP_FONT16_SIZE_OF_INDEX1     4230
#define GB0_FONT16_SEGMENT_LENGTH      848

#define BYTES_PER_CHINESE              32

#define FONT16                         16
#define FONT12                         12
#define SYMBOLFONT                       1
#define CHINESE_UNICODE_START          0x4e00
#define CHINESE_UNICODE_END            0x9fa0
//#define CHINESE_CHARACTER_NUM          846
/************************ Type  Declaration  **********************************/

typedef  unsigned long                 LWORD;


/************************ Extern Value ****************************************/

extern  const OP_UINT16  GB2312_unicode_table[];

/************************ Gloable  Function Declaration  ***************************/


/******************DEFINE END****************************************************/
#endif 
