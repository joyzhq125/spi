#ifdef __cplusplus
extern "C" {
#endif
/*==================================================================================================

    MODULE NAME : gif_to_bmp.c

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
    06/16/2003   Tree Zhan                      improve the speed of uncompressed gif file
    07/26/2003   lindawang        P000973       remove animal_buffer.
    08/16/2003   Tree Zhan        P001241       improve the speed of uncompressed gif file
    08/22/2003   shifeng          P001304       fixed the bug of displaying some gif file error.
    08/25/2003   shifeng          P001369       add code to tackle NULL pointer situation.
    08/27/2003   shifeng          P001388       solve the problem of resolving local colour pallet 
                                                error and reduce gif decoding memory spending
    08/28/2003   Tree Zhan        P001419       Fix gif not release problem
    10/14/2003   linda wang       C001806       Add function of setting download image to desktop.
    11/05/2003   shifeng          P001979       Fixed a bug that playing some gif file cause handset reboot.
    12/17/2003   linda wang       P002201     Add gif decode error handler and fix bug for download image display.
    04/02/2004   shifeng          P002831       fixed the bug of loop error when view cect.cmn.kongzhon.com/index.jsp
    06/03/2004   penghaibo        P005888       fixed the bug of unsupported gif file
    08/26/2004   penghaibo        p007954       fix bug of memery leak when a page comtainmore then five gif image
    Self-documenting Code
    Describe/explain low-level design of this module and/or group of funtions and/or specific
    funtion that are hard to understand by reading code and thus requires detail description.
    Free format !
        
====================================================================================================
    INCLUDE FILES
==================================================================================================*/

#include "APP_include.h"
#include "gif_to_bmp.h"


/*==================================================================================================
    LOCAL CONSTANTS
==================================================================================================*/
#define GIFHEADER_SIZE               13
#define IMAGEDATAINFO_SIZE           10
#define GRAPHCTRL_SIZE               8
#define APPCTRL_SIZE                 271
#define TEXTCTRL_SIZE                270
#define NOTEHCTRL_SIZE               259
#define MAX_GIF_NUMBER               50
#define PART_IMAGE_DATA_BUFFER_SIZE  sizeof(OP_UINT16)*5200
#define BITMAP_PALLET_SIZE           256
#define LZWTABLE_SIZE                5200
#define LZWTABLE_MAX_SEEK_TIMES      15000
#define MAX_CODES 4095

/*==================================================================================================
    LOCAL TYPEDEFS - Structure, Union, Enumerations...etc
==================================================================================================*/


typedef struct
{
    OP_UINT16 FileType; /* 89a or 87a */
    OP_UINT16 ColorType;/* 1,2,4,8,16,32,64,128,256 */
    OP_UINT16 ColorMode;/* 1,2,3,4,5,6,7,8 */
    OP_UINT16 Width;
    OP_UINT16 Height;
    OP_UINT16 FrameCount;
    OP_UINT8  InitPixelBits;
}GIFINFO;

typedef struct
{
    OP_UINT8 bRed;
    OP_UINT8 bGreen;
    OP_UINT8 bBlue;
}MYRGB;

/*
 * RGB structure, it is no use now, may be use future
 */
typedef struct 
{ 
    OP_UINT8    rgbBlue; 
    OP_UINT8    rgbGreen; 
    OP_UINT8    rgbRed; 
    OP_UINT8    rgbReserved; 
}RGB_QUAD;

typedef struct
{
    short        tColor;
    OP_UINT16    DelayTime;
    OP_UINT16    Left;
    OP_UINT16    Top;
    OP_UINT16    Width;
    OP_UINT16    Height;
    RGB_QUAD     Palette[256];    
    OP_UINT8*    pImageData;
    OP_BOOLEAN   SaveMode;/*1 = normal; 0 = e*/
}BMPIMAGE;


typedef struct
{
    OP_UINT8  Signature[6];
    OP_UINT16 ScreenWidth;
    OP_UINT16 ScreenDepth;
    OP_UINT8  GlobalFlagByte;
    OP_UINT8  BackGroundColor;
    OP_UINT8  AspectRadio;
}GIFHEADER;

typedef struct _graph_ctrl_struct
    {
    OP_UINT8    TransparentColorFlag:1;
    OP_UINT8    UseInputFlag:1;
    OP_UINT8    DisposalMethod:3;
    OP_UINT8    Reserved:3;
} graph_ctrl_struct;

typedef struct _image_ctrl_struct
    {
    OP_UINT8   l_color_table_size:3;
    OP_UINT8   reserved:2;
    OP_UINT8   sort_flag:1;
    OP_UINT8   interlace_mode:1;
    OP_UINT8   l_color_table:1;
    } image_ctrl_struct;

typedef struct
{
    OP_UINT8   ExtIntr;// 0x21    
    OP_UINT8   Label;//0xF9
    OP_UINT8   BlockSize;//0x04
    graph_ctrl_struct   graph_ctrl;
    OP_UINT16  DelayTime;
    OP_UINT8   TranColorIndex;
    OP_UINT8   blockTerminator;//0x00
}GRAPHCTRL;

typedef struct
{
    OP_UINT8   ImageLabel;/* default 0x2c */
    OP_UINT16  ImageLeft; 
    OP_UINT16  ImageTop;
    OP_UINT16  ImageWidth;
    OP_UINT16  ImageHeight;
    image_ctrl_struct   image_ctrl; 
    GRAPHCTRL  graphctrl;
    OP_UINT8 * image_data;
}IMAGEDATAINFO;

typedef struct
{
    OP_UINT16 Header;
    OP_UINT16 Tail;
    OP_UINT16 Code; 
}LZWTABLE;



typedef struct
{
    OP_UINT8 ExtIntr;// 0x21    
    OP_UINT8 Label;//0xFE
    OP_UINT8 Data[256];//256
    OP_UINT8 blockTerminator;//0x00
}NOTEHCTRL;

typedef struct
{
    OP_UINT8     ExtIntr;// 0x21    
    OP_UINT8     Label;//0x01
    OP_UINT8     BlockSize;//0x0c
    OP_UINT16    Left;
    OP_UINT16    Top;
    OP_UINT16    Width;
    OP_UINT16    Height; 
    OP_UINT8     ForeColorIndex;
    OP_UINT8     BkColorIndex;
    char         Data[256];
    OP_UINT8     blockTerminator;//0x00
}TEXTCTRL;
typedef struct
{
    OP_UINT8 ExtIntr;// 0x21    
    OP_UINT8 Label;//0xFF
    OP_UINT8 BlockSize;//0x0b
    OP_UINT8 Identifier[8];
    OP_UINT8 Authentication[3];
    OP_UINT8 Data[256];
    OP_UINT8 blockTerminator;//0x00
}APPCTRL;

typedef struct
{
    OP_UINT32   m_HeaderSize;
    GIFINFO     m_GifInfo;
    OP_UINT8    *m_pDataArea;
    OP_UINT8    *m_TailPointer;
    OP_UINT32   m_fileSize;
    OP_UINT16   m_GLBRgbQuad[BITMAP_PALLET_SIZE];/* Bitmap pattlen; */
    OP_UINT16   *p_CurRgbQuad;
    OP_UINT8    BackGroundColor;
    OP_UINT16   m_ScreenWidth;
    OP_UINT16   m_ScreenHeight;
    IMAGEDATAINFO prevImage;
    IMAGEDATAINFO currImage;
    OP_UINT32   offset_length;
    OP_UINT32   Offset;
    OP_BOOLEAN  first_enty;
    OP_UINT8    *pgifData;
}GIF_INFO_STRUCTRUE_T;

typedef struct
{
    OP_UINT32     handle;
    OP_BOOLEAN    bUse;
    GIF_INFO_STRUCTRUE_T *pGifInfo;
}GIF_INFO_ARRAY_STRUCTRUE_T;

typedef enum 
{
    GET,
    RELEASE
} BUFFER_PARAM_ENUM;

typedef struct _LZW_DECODE_CRTL {
    OP_INT16 curr_size;                     /* The current code size */
    OP_INT16 clear;                         /* Value for a clear code */
    OP_INT16 ending;                        /* Value for a ending code */
    OP_INT16 newcodes;                      /* First available code */
    OP_INT16 top_slot;                      /* Highest code for current size */
	OP_INT16 slot;                          /* Last read code */

    	/* The following static variables are used
	* for seperating out codes */
	OP_INT16 navail_bytes;              /* # bytes left in block */
	OP_INT16 nbits_left;                /* # bits left in current BYTE */
	OP_UINT8 b1;                           /* Current BYTE */
	OP_UINT8 *byte_buff;               /* Current block */
	OP_UINT8 *pbytes;                      /* Pointer to next BYTE in block */
    /*
    * source data info
    */
        OP_UINT8 * src_buf;
        OP_UINT16 offset;
        OP_UINT16 datacount;
    	/* The reason we have these seperated like this instead of using
	* a structure like the original Wilhite code did, is because this
	* stuff generally produces significantly faster code when compiled...
	* This code is full of similar speedups...  (For a good book on writing
	* C for speed or for space optomisation, see Efficient C by Tom Plum,
	* published by Plum-Hall Associates...)
	*/
	OP_UINT8 stack[MAX_CODES + 1];            /* Stack for storing pixels */
	OP_UINT8 suffix[MAX_CODES + 1];           /* Suffix table */
	OP_UINT32 prefix[MAX_CODES + 1];           /* Prefix linked list */

}lzw_decode_ctrl;


/*==================================================================================================
    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static OP_UINT32    GetGrphContent(OP_UINT8* pGrCtrl);
static OP_UINT32    ShowText(OP_UINT8* pText);
static OP_UINT32    GetAppContent(OP_UINT8* pApp);
static OP_UINT32    GetNoteContent(OP_UINT8* pNote);
static OP_BOOLEAN   ConvertToOpusBmp( OP_UINT16  xPos, OP_UINT16  yPos, OP_UINT8  *SrcData, OP_UINT8  *pDataBuffer );
static OP_BOOLEAN   GetCodeDataOnBits( OP_UINT8* CodeDataStr ,OP_UINT32 dataCount,OP_UINT8* pBuffer );
static OP_BOOLEAN   GetImage( OP_INT16 x, OP_INT16 y, OP_UINT8 *pData, RM_BITMAP_T *pLcdBuf, OP_UINT32 *pOffset );
static OP_UINT32    GetCodeCountOnChar( OP_UINT8* CodeDataStr );


static OP_UINT16*   PartImageDataBuffer( BUFFER_PARAM_ENUM action );
static OP_UINT8*    TrueDataBuffer( BUFFER_PARAM_ENUM action, OP_UINT32 new_size );
/*==================================================================================================
    LOCAL MACROS
==================================================================================================*/



/*==================================================================================================
    LOCAL VARIABLES
==================================================================================================*/
static GIF_INFO_STRUCTRUE_T        *pCurGifInfo = OP_NULL;

static GIF_INFO_ARRAY_STRUCTRUE_T  gifArray[MAX_GIF_NUMBER]={0};

////////////////////////////////////////////////////////////////////////////////

static const unsigned long code_mask[] = { 0x0000, 0x0001, 0x0003, 0x0007, 0x000F,
								  0x001F, 0x003F, 0x007F, 0x00FF,
								  0x01FF, 0x03FF, 0x07FF, 0x0FFF,
								  0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF };

////////////////////////////////////////////////////////////////////////////////
static  LZWTABLE    LZWTable[LZWTABLE_SIZE];

/*==================================================================================================
    GLOBAL VARIABLES
==================================================================================================*/




/*==================================================================================================
    LOCAL FUNCTIONS
==================================================================================================*/
static GIF_INFO_ARRAY_STRUCTRUE_T *gif_get_free(void)
{
    GIF_INFO_ARRAY_STRUCTRUE_T *ptr = OP_NULL;
    OP_UINT8   i;

    for (i = 0; i < MAX_GIF_NUMBER; i++)
    {
        if(gifArray[i].bUse == OP_FALSE) 
        {
            ptr = &gifArray[i];
            ptr->bUse = OP_TRUE;
            break;
        }
    }

    return ptr;
}

static GIF_INFO_ARRAY_STRUCTRUE_T *gif_find_info(OP_UINT32 handle)
{
    GIF_INFO_ARRAY_STRUCTRUE_T *ptr = OP_NULL;
    OP_UINT8   i;

    for (i = 0; i < MAX_GIF_NUMBER; i++)
    {
        if(gifArray[i].handle == handle) 
        {
            ptr = &gifArray[i];
            ptr->bUse = OP_TRUE;
            break;
        }
    }

    return ptr;
}



static OP_UINT32 GetGrphContent(OP_UINT8* pGrCtrl)
{
    GRAPHCTRL *GraphCtrl;
    OP_UINT8 *Filedata = pGrCtrl;
    GraphCtrl = &(pCurGifInfo->currImage.graphctrl);
    if( OP_NULL == pGrCtrl )
    {
        op_debug( DEBUG_HIGH, "gif_to_bmp.c---GetGrphContent---parameter is NULL pointer!\n" );
        return 0;
    }
    op_memcpy(&GraphCtrl->ExtIntr,Filedata,1);
    Filedata +=1;
    op_memcpy(&GraphCtrl->Label,Filedata,1);
    Filedata +=1;
    op_memcpy(&GraphCtrl->BlockSize,Filedata,1);
    Filedata +=1;
    op_memcpy(&GraphCtrl->graph_ctrl,Filedata,1);
    Filedata +=1;
    op_memcpy(&GraphCtrl->DelayTime,Filedata,2);
    Filedata +=2;
    op_memcpy(&GraphCtrl->TranColorIndex,Filedata,1);
    Filedata +=1;
    op_memcpy(&GraphCtrl->blockTerminator,Filedata,1);
       Filedata +=1;
    /*
     * 处置方法(Disposal Method)：指出处置图形的方法，当值为：
     *                  0 - 不使用处置方法
     *                   1 - 不处置图形，把图形从当前位置移去
     *                   2 - 回复到背景色
     *                   3 - 回复到先前状态
     *                 4-7 - 自定义
     */
 
    return GRAPHCTRL_SIZE;
}


static OP_UINT32 ShowText(OP_UINT8* pText)
{
    TEXTCTRL TextCtrl;
    if( OP_NULL == pText )
    {
        op_debug( DEBUG_HIGH, "gif_to_bmp.c---ShowText---parameter is NULL pointer!\n" );
        return 0;
    }
    op_memcpy(&TextCtrl,pText,TEXTCTRL_SIZE);
    return TEXTCTRL_SIZE+TextCtrl.Data [0] - 256 + 1;
}
static OP_UINT32 GetAppContent(OP_UINT8* pApp)
{
    APPCTRL AppCtrl;
    if( OP_NULL == pApp )
    {
        op_debug( DEBUG_HIGH, "gif_to_bmp.c---GetAppContent---parameter is NULL pointer!\n" );
        return 0;
    }
    op_memcpy(&AppCtrl,pApp,APPCTRL_SIZE);
    return APPCTRL_SIZE + AppCtrl.Data [0] - 256 + 1;
}

static OP_UINT32 GetNoteContent(OP_UINT8* pNote)
{
    /* For our OPUS, notectrl is no use now */
    NOTEHCTRL NoteCtrl;
   if( OP_NULL == pNote )
   {
        op_debug( DEBUG_HIGH, "gif_to_bmp.c---GetNoteContent---parameter is NULL pointer!\n" );
        return 0;
   }
    op_memcpy(&NoteCtrl,pNote,NOTEHCTRL_SIZE);   
    return NOTEHCTRL_SIZE +NoteCtrl.Data[0] - 256 + 1;
}

static OP_UINT32 GetCodeCountOnChar (OP_UINT8* CodeDataStr)
{
    OP_UINT32 dataCount = 0;
    OP_UINT8 dataLen;
    OP_UINT8* p1 = CodeDataStr;
    if( OP_NULL == CodeDataStr )
    {
        op_debug( DEBUG_HIGH, "gif_to_bmp.c---GetCodeCountOnChar---parameter is NULL pointer!\n" );
        return 0;
    }
    pCurGifInfo->offset_length = 0;
    while (1)
    {
        dataLen = *p1 ++;
        p1 += dataLen;
        if(p1 > pCurGifInfo->m_TailPointer)
        {
            return 0;
        }
        dataCount += dataLen+1;
        pCurGifInfo->offset_length += (dataLen + 1);
        if (dataLen == 0x00) break;
    }
    pCurGifInfo->offset_length ++;
    if((pCurGifInfo->offset_length + pCurGifInfo->Offset)> pCurGifInfo->m_fileSize)
    {
        dataCount = 0;
    }
    return dataCount;
}



/* get_next_code()
 * - gets the next code from the GIF file.  Returns the code, or else
 * a negative number in case of file errors...
 */
static OP_INT16 get_next_code(lzw_decode_ctrl * ctrl)
{
	OP_UINT32 ret;
    if (ctrl->offset >= ctrl->datacount) {
        return ctrl->ending;
    }
	if (ctrl->nbits_left == 0) {
		if (ctrl->navail_bytes <= 0) {
			/* Out of bytes in current block, so read next block */
			if ((ctrl->navail_bytes = (OP_INT16)ctrl->src_buf[ctrl->offset++]) < 0)
				return(ctrl->navail_bytes);
			else if (ctrl->navail_bytes) {
					ctrl->byte_buff = ctrl->src_buf + ctrl->offset;
                    ctrl->offset+= ctrl->navail_bytes;
                }
            ctrl->pbytes = ctrl->byte_buff;

		}
		ctrl->b1 = *ctrl->pbytes++;
		ctrl->nbits_left = 8;
		--ctrl->navail_bytes;
	}

	if (ctrl->navail_bytes<0) return ctrl->ending; // prevent deadlocks (thanks to Mike Melnikov)

	ret = ctrl->b1 >> (8 - ctrl->nbits_left);
	while (ctrl->curr_size > ctrl->nbits_left){
		if (ctrl->navail_bytes <= 0){
			/* Out of bytes in current block, so read next block*/
			if ((ctrl->navail_bytes =  (OP_INT16)ctrl->src_buf[ctrl->offset++]) < 0)
				return(ctrl->navail_bytes);
			else if (ctrl->navail_bytes){
					ctrl->byte_buff = ctrl->src_buf + ctrl->offset;
                    ctrl->offset+= ctrl->navail_bytes;
                }
            ctrl->pbytes = ctrl->byte_buff;

		}
		ctrl->b1 = *ctrl->pbytes++;
		ret |= ctrl->b1 << ctrl->nbits_left;
		ctrl->nbits_left += 8;
		--ctrl->navail_bytes;
	}
	ctrl->nbits_left = (OP_INT16)(ctrl->nbits_left-ctrl->curr_size);
	ret &= code_mask[ctrl->curr_size];
	return((OP_INT16)(ret));
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

static OP_INT16 init_exp(OP_INT16 size,lzw_decode_ctrl * ctrl)
    {
    ctrl->curr_size = (OP_INT16)(size + 1);
    ctrl->top_slot = (OP_INT16)(1 << ctrl->curr_size);
    ctrl->clear = (OP_INT16)(1 << size);
    ctrl->ending = (OP_INT16)(ctrl->clear + 1);
    ctrl->slot = ctrl->newcodes = (OP_INT16)(ctrl->ending + 1);
    ctrl->navail_bytes = ctrl->nbits_left = 0;
    
    op_memset(ctrl->stack,0,MAX_CODES + 1);
    op_memset(ctrl->prefix,0,MAX_CODES + 1);
    op_memset(ctrl->suffix,0,MAX_CODES + 1);
    return(0);
    }
////////////////////////////////////////////////////////////////////////////////

static OP_BOOLEAN  GetCodeDataOnBits( 
    OP_UINT8 *CodeDataStr,
    OP_UINT32 dataCount,
    OP_UINT8 *pBuffer
)
{
	register OP_UINT8 *sp, *bufptr;
	OP_UINT8 *buf;
	register OP_INT16 code, fc, oc, bufcnt;
	OP_INT16 c, size;
    lzw_decode_ctrl *ctrl;
    OP_UINT32 bad_code_count;
    OP_UINT16 linewidth;
    OP_UINT16 linecounter;
	/* Initialize for decoding a new image... */
	bad_code_count = 0;
	size = pCurGifInfo->m_GifInfo.ColorMode;
	if (size < 2 || 9 < size)	
        return(OP_FALSE);
	// out_line = outline;
	ctrl = (lzw_decode_ctrl *) op_alloc(sizeof(lzw_decode_ctrl));
    if(ctrl == OP_NULL)
        {
            return OP_FALSE;
        }
	init_exp(size,ctrl);

        ctrl->src_buf = CodeDataStr;
        ctrl->offset = 0;
        ctrl->datacount = dataCount;
	//printf("L %d %x\n",linewidth,size);

	/* Initialize in case they forgot to put in a clear code.
	 * (This shouldn't happen, but we'll try and decode it anyway...)
	 */
	oc = fc = 0;

   /* Allocate space for the decode buffer */
    //	if ((buf = new BYTE[linewidth + 1]) == NULL) return(OUT_OF_MEMORY);

   /* Set up the stack pointer and decode buffer pointer */
    linewidth =pCurGifInfo->currImage.ImageWidth;
	sp = ctrl->stack;
    
    //get last line of the des buf
      linecounter = 0;
        buf = pBuffer + linecounter * linewidth;
        linecounter++;
	bufptr = buf;
	bufcnt = linewidth;

   /* This is the main loop.  For each code we get we pass through the
	* linked list of prefix codes, pushing the corresponding "character" for
	* each code onto the stack.  When the list reaches a single "character"
	* we push that on the stack too, and then start unstacking each
    * character for output in the correct order.  Special handling is
	* included for the clear code, and the whole thing ends when we get
    * an ending code.
    */
	while ((c = get_next_code(ctrl)) != ctrl->ending) {
		/* If we had a file error, return without completing the decode*/
		if (c < 0){
			op_free(ctrl);
			return(OP_FALSE);
		}
		/* If the code is a clear code, reinitialize all necessary items.*/
		if (c == ctrl->clear){
			ctrl->curr_size = (short)(size + 1);
			ctrl->slot = ctrl->newcodes;
			ctrl->top_slot = (short)(1 << ctrl->curr_size);

			/* Continue reading codes until we get a non-clear code
			* (Another unlikely, but possible case...)
			*/
			while ((c = get_next_code(ctrl)) == ctrl->clear);

			/* If we get an ending code immediately after a clear code
			* (Yet another unlikely case), then break out of the loop.
			*/
			if (c == ctrl->ending) break;

			/* Finally, if the code is beyond the range of already set codes,
			* (This one had better NOT happen...  I have no idea what will
			* result from this, but I doubt it will look good...) then set it
			* to color zero.
			*/
			if (c >= ctrl->slot) c = 0;
			oc = fc = c;

			/* And let us not forget to put the char into the buffer... And
			* if, on the off chance, we were exactly one pixel from the end
			* of the line, we have to send the buffer to the out_line()
			* routine...
			*/
			*bufptr++ = (OP_UINT8)c;
			if (--bufcnt == 0) {
                           //move to next line

                           buf = pBuffer + linecounter * linewidth;
                            linecounter++;
				bufptr = buf;
				bufcnt = linewidth;
            }
		} else {
			/* In this case, it's not a clear code or an ending code, so
			* it must be a code code...  So we can now decode the code into
			* a stack of character codes. (Clear as mud, right?)
			*/
			code = c;

			/* Here we go again with one of those off chances...  If, on the
			* off chance, the code we got is beyond the range of those already
			* set up (Another thing which had better NOT happen...) we trick
			* the decoder into thinking it actually got the last code read.
			* (Hmmn... I'm not sure why this works...  But it does...)
			*/
			if (code >= ctrl->slot) {
				if (code > ctrl->slot) ++bad_code_count;
				code = oc;
				*sp++ = (OP_UINT8)fc;
            }

			/* Here we scan back along the linked list of prefixes, pushing
			* helpless characters (ie. suffixes) onto the stack as we do so.
			*/
			while (code >= ctrl->newcodes) {
				*sp++ = ctrl->suffix[code];
				code = ctrl->prefix[code];
            }

			/* Push the last character on the stack, and set up the new
			* prefix and suffix, and if the required slot number is greater
			* than that allowed by the current bit size, increase the bit
			* size.  (NOTE - If we are all full, we *don't* save the new
			* suffix and prefix...  I'm not certain if this is correct...
			* it might be more proper to overwrite the last code...
			*/
			*sp++ = (OP_UINT8)code;
			if (ctrl->slot < ctrl->top_slot){
				ctrl->suffix[ctrl->slot] = (OP_UINT8)(fc = (OP_UINT8)code);
				ctrl->prefix[(ctrl->slot)++] = oc;
				oc = c;
            }
			if (ctrl->slot >= ctrl->top_slot){
				if (ctrl->curr_size < 12) {
					ctrl->top_slot <<= 1;
					++(ctrl->curr_size);
				}
			}

			/* Now that we've pushed the decoded string (in reverse order)
			* onto the stack, lets pop it off and put it into our decode
			* buffer...  And when the decode buffer is full, write another
			* line...
			*/
            while (sp > ctrl->stack) {
                *bufptr++ = *(--sp);
                if (--bufcnt == 0) {
                    buf = pBuffer + linecounter * linewidth;
                    linecounter++;
                    bufptr = buf;
                    bufcnt = linewidth;
                    }
                }
		}
	}
//	if (bufcnt != linewidth)
//		ret = (short)out_line(iter, buf, (linewidth - bufcnt));
		op_free(ctrl);
        return OP_TRUE;
}


/*!
 *    \brief This funtion will convert a frame of gif file to OPUS lcd buffer
 */
static OP_BOOLEAN ConvertToOpusBmp
(
    OP_UINT16  xPos,            /*!< the X position of the LCD */
    OP_UINT16  yPos,            /*!< the Y position of the LCD */
    OP_UINT8  *SrcData,         /*!< source data from gif file */
    OP_UINT8  *pDataBuffer      /*!< LCD buffer */
)
{
    OP_UINT8   *p0           = OP_NULL;
    OP_UINT8   *pSrcData     = OP_NULL;
    OP_UINT8   *p2           = OP_NULL;
    OP_UINT8   *save  =  OP_NULL; 
    OP_INT32   j,k,Step,i;
    OP_BOOLEAN bFlag         = OP_FALSE;
    OP_UINT32  buffersize    = 0;


    if ( OP_NULL == SrcData || OP_NULL == pDataBuffer )
    {
        op_debug( DEBUG_HIGH, "gif_to_bmp.c---ConvertToOpusBmp---parameter is NULL pointer!\n" );
        return OP_FALSE;
    }
    pSrcData = SrcData;
    p0 = pDataBuffer;


    if (pCurGifInfo->currImage.image_ctrl.interlace_mode)/* if save mode, we should get data section */
    {
        OP_UINT8* pTmp;
        buffersize = pCurGifInfo->currImage.ImageWidth*pCurGifInfo->currImage.ImageHeight;
        p2 = op_alloc(buffersize);
        
        if( OP_NULL == p2 )
        {
            op_debug( DEBUG_HIGH, "gif_to_bmp---ConvertToOpusBmp---op_alloc failed!\n" );
            return OP_FALSE;
        }
        
        pTmp = p2;

        k = 0; Step = 8;
        for (j = 0; j < pCurGifInfo->currImage.ImageHeight  ; j ++)
        {
            op_memcpy(pTmp+k*pCurGifInfo->currImage.ImageWidth,
                      pSrcData+j*pCurGifInfo->currImage.ImageWidth,
                      pCurGifInfo->currImage.ImageWidth);
            k += Step;
            if (k >= pCurGifInfo->currImage.ImageHeight) break;
        }
        j++;
        k = 4; Step = 8;
        for (; j < pCurGifInfo->currImage.ImageHeight  ; j ++)
        {
            op_memcpy(pTmp+k*pCurGifInfo->currImage.ImageWidth,
                      pSrcData+j*pCurGifInfo->currImage.ImageWidth,
                      pCurGifInfo->currImage.ImageWidth);
            k += Step;
            if (k >= pCurGifInfo->currImage.ImageHeight) break;
        }
        j++;
        k = 2; Step = 4;
        for (; j < pCurGifInfo->currImage.ImageHeight  ; j ++)
        {
            op_memcpy(pTmp+k*pCurGifInfo->currImage.ImageWidth,
                      pSrcData+j*pCurGifInfo->currImage.ImageWidth,
                      pCurGifInfo->currImage.ImageWidth);
            k += Step;
            if (k >= pCurGifInfo->currImage.ImageHeight) break;
        }
        j++;
        k = 1; Step = 2;
        for (; j < pCurGifInfo->currImage.ImageHeight  ; j ++)
        {
            op_memcpy(pTmp+k*pCurGifInfo->currImage.ImageWidth,
                      pSrcData+j*pCurGifInfo->currImage.ImageWidth,
                      pCurGifInfo->currImage.ImageWidth);
            k += Step;
            if (k >= pCurGifInfo->currImage.ImageHeight) break;
        }

        pSrcData = p2;
    }
    p0 += (yPos * pCurGifInfo->m_ScreenWidth) + xPos;

    if(pCurGifInfo->currImage.graphctrl.graph_ctrl.TransparentColorFlag)
    {
        bFlag = OP_TRUE;
    }
    
    if((pCurGifInfo->currImage.ImageWidth == pCurGifInfo->m_ScreenWidth)&&
        (pCurGifInfo->currImage.ImageHeight == pCurGifInfo->m_ScreenHeight)&&
        (!bFlag)&&pCurGifInfo->first_enty==OP_FALSE)
        {
        //copy image to display buffer
        op_memcpy(p0,pSrcData,pCurGifInfo->currImage.ImageWidth*pCurGifInfo->currImage.ImageHeight);
        }
    else
        {
        if( pCurGifInfo->currImage.graphctrl.graph_ctrl.DisposalMethod == 3)
            {
  	         //save previous image for dispose
            save= (OP_UINT8 *)op_alloc(pCurGifInfo->currImage.ImageWidth*pCurGifInfo->currImage.ImageHeight);
            if (save == OP_NULL) {
                pCurGifInfo->currImage.graphctrl.graph_ctrl.DisposalMethod = 0;
            }
            pCurGifInfo->currImage.image_data = save ;
            }
        else
            {
            pCurGifInfo->currImage.image_data=OP_NULL;
            }
        
        for(i = 0; i < pCurGifInfo->currImage.ImageHeight; i++)
            {
            if(bFlag)
                {
                for(j = 0; j < pCurGifInfo->currImage.ImageWidth; j++)
                    {
                    if(pSrcData[j] != pCurGifInfo->currImage.graphctrl.TranColorIndex)
                        {
                        if( pCurGifInfo->currImage.graphctrl.graph_ctrl.DisposalMethod == 3)
                            {
                            //save pix data
                            save[j] = p0[j];
                            }
                        p0[j] = pSrcData[j];
                        }
                    }
                }
            else
                {
                if( pCurGifInfo->currImage.graphctrl.graph_ctrl.DisposalMethod == 3)
                    {
                    //save line data
                    op_memcpy(save, p0, pCurGifInfo->currImage.ImageWidth);
                    }
                op_memcpy(p0,pSrcData,pCurGifInfo->currImage.ImageWidth);
                }
            pSrcData += pCurGifInfo->currImage.ImageWidth;
            p0 += pCurGifInfo->m_ScreenWidth;

            if(pCurGifInfo->currImage.graphctrl.graph_ctrl.DisposalMethod == 3)
            {
            //save move save point
            save +=pCurGifInfo->currImage.ImageWidth;
            }
            
            }
        }   
    if(pCurGifInfo->currImage.image_ctrl.interlace_mode && p2!= OP_NULL)
    {
        op_free(p2);
    }
    
    return OP_TRUE;
}
    

static OP_BOOLEAN GetImage( OP_INT16 x, OP_INT16 y, OP_UINT8 *pData, RM_BITMAP_T *pLcdBuf, OP_UINT32 *pOffset )
{
    OP_UINT32   InitBits = 0;
    OP_UINT16   wRed, wGreen, wBlue;
    OP_UINT32   j = 0;
    OP_UINT8*   p             = OP_NULL;
    OP_UINT8*   Filedata      = OP_NULL;
    OP_UINT8*   pCodeData = OP_NULL;
    OP_UINT8*   pTrueCodeData = OP_NULL;
    OP_UINT32   dataCount     = 0;
    OP_UINT32   PalSize       = 0;
    IMAGEDATAINFO *ImageData;
    OP_BOOLEAN  bool_retval   = OP_FALSE;



    ImageData = &(pCurGifInfo->currImage);
	
    if ( OP_NULL == pData || OP_NULL == pLcdBuf || OP_NULL == pOffset )
    {
        return OP_FALSE;
    }

    Filedata = pData;
    /*  
        +---------------+
      1 |0 0 1 0 1 1 0 0|   ',' first byte of IMAGE DESCRIPTOR --> Image separator character
        +---------------+
     */
    op_memcpy(&ImageData->ImageLabel,Filedata,1);
    Filedata +=1;
    /*
        +---------------+
      2 |               |   Start of image in pixels from the
        +-  Image Left -+   left side of the screen (LSB first)
      3 |               |  
        +---------------+
     */
    op_memcpy(&ImageData->ImageLeft,Filedata,2);
    Filedata +=2;
    /*
        +---------------+
      4 |               |   
        +-  Image Top  -+   Start of image in pixels from the
      5 |               |   top of the screen (LSB first)
        +---------------+
     */
    op_memcpy(&ImageData->ImageTop,Filedata,2);
    Filedata +=2;
    /*
        +---------------+
      6 |               |   
        +-  Image Width-+   Width of the image in pixels (LSB first)
      7 |               |
        +---------------+
     */
    op_memcpy(&ImageData->ImageWidth,Filedata,2);
    Filedata +=2;
    /*
        +---------------+
      8 |               |   
        +- Image Height-+  Height of the image in pixels (LSB first)
      9 |               |
        +---------------+
     */
    op_memcpy(&ImageData->ImageHeight,Filedata,2);
    Filedata +=2;
    /*
        +-+-+-+-+-+-----+    M=0 - Use global color map, ignore 'pixel'
     10 |M|I|0|0|0|pixel|    M=1 - Local color map follows, use 'pixel'
        +-+-+-+-+-+-----+    I=0 - Image formatted in Sequential order
                             I=1 - Image formatted in Interlaced order
                             pixel+1 - # bits per pixel for this image
     */
    op_memcpy(&ImageData->image_ctrl,Filedata,1);
    Filedata +=1;
    
    p = Filedata;

    if(((ImageData->ImageLeft + ImageData->ImageWidth)>pCurGifInfo->m_ScreenWidth)
        ||((ImageData->ImageTop+ ImageData->ImageHeight)>pCurGifInfo->m_ScreenHeight))
    {
        return OP_FALSE;
    }

    pLcdBuf->biWidth = pCurGifInfo->m_ScreenWidth;
    pLcdBuf->biHeight = pCurGifInfo->m_ScreenHeight;
    
    if (ImageData->image_ctrl.l_color_table) 
    { 
        OP_UINT8 * pRGB = p;
        pCurGifInfo->m_GifInfo.ColorMode  = (ImageData->image_ctrl.l_color_table_size) + 1;
        pCurGifInfo->m_GifInfo.InitPixelBits = pCurGifInfo->m_GifInfo.ColorMode + 1;
        pCurGifInfo->m_GifInfo.ColorType = 1;
        pCurGifInfo->m_GifInfo.ColorType = pCurGifInfo->m_GifInfo.ColorType << pCurGifInfo->m_GifInfo.ColorMode;
       

        pCurGifInfo->p_CurRgbQuad = (OP_UINT16 *)pLcdBuf->ColorPallet;
        for (j = 0; j < pCurGifInfo->m_GifInfo.ColorType ; j ++)
        {
            /* get the firt byte -- Red byte */
            wRed = (*pRGB >> 3) & 0x1f;
            pCurGifInfo->p_CurRgbQuad[j] = (wRed<<11)&0xF800;
            pRGB++;

            /* get the second byte -- Green byte */
            wGreen = (*pRGB >> 2) & 0x3f;
            pCurGifInfo->p_CurRgbQuad[j] |= (wGreen<<5)&0x7E0;
            pRGB++;

            /* get the third byte -- Blue byte */
            wBlue = *pRGB;
            pCurGifInfo->p_CurRgbQuad[j] |= (wBlue >> 3) & 0x1f;
            pRGB++;
        }
        p += 3 * pCurGifInfo->m_GifInfo.ColorType;
        PalSize = 3 * pCurGifInfo->m_GifInfo.ColorType;
    } 
    else
        {
            //reset to globe color table
             pCurGifInfo->p_CurRgbQuad = (OP_UINT16 *)pLcdBuf->ColorPallet;
             op_memcpy(pCurGifInfo->p_CurRgbQuad,pCurGifInfo->m_GLBRgbQuad,BITMAP_PALLET_SIZE * sizeof(OP_UINT16));
        }
    
    pLcdBuf->biBitCount = 8; /*8-bit color bitmap*/

    InitBits = *p++;

    pCurGifInfo->m_GifInfo.ColorMode = InitBits;

    dataCount = GetCodeCountOnChar(p);
    if(dataCount == 0)
    {
        return OP_FALSE;
    }
    
    pCodeData = p;
    /*
     *	create buf to store decoded data
     */
    pTrueCodeData = TrueDataBuffer( GET, ImageData->ImageWidth * ImageData->ImageHeight*2 );
    if ( OP_NULL == pTrueCodeData )
    {
       return OP_FALSE;
    }

    /*
     *	decode data
     */
    bool_retval = GetCodeDataOnBits (pCodeData,dataCount, pTrueCodeData);
    if ( OP_FALSE == bool_retval )
    {
        return OP_FALSE;
    }

    //dispose old pic depend on disposalMethod
    if(pCurGifInfo->first_enty)  	
        {
        switch(pCurGifInfo->prevImage.graphctrl.graph_ctrl.DisposalMethod)
            {
            case 0:
                //do nothing
                break;
            case 1:
                //do nothing 
                break;
            case 2:  
                //restore area with background color 
                {
                OP_UINT8 * ps0;
                OP_UINT16 i;
                ps0 = (OP_UINT8*)pLcdBuf->data + pCurGifInfo->m_ScreenWidth * pCurGifInfo->prevImage.ImageTop + pCurGifInfo->prevImage.ImageLeft;
                for(i = 0 ;i< pCurGifInfo->prevImage.ImageHeight;i++){
                    op_memset(ps0,pCurGifInfo->BackGroundColor,pCurGifInfo->prevImage.ImageWidth);
                    ps0 += pCurGifInfo->m_ScreenWidth;
                    }
                }
                break;
            case 3:
                //restore area with previous graphic  
                {
                OP_UINT8 * ps0,*ps1;
                OP_UINT16 i;
                ps1 = pCurGifInfo->prevImage.image_data;
                ps0 = (OP_UINT8*)pLcdBuf->data + pCurGifInfo->m_ScreenWidth * pCurGifInfo->prevImage.ImageTop + pCurGifInfo->prevImage.ImageLeft;
                for(i = 0 ;i< pCurGifInfo->prevImage.ImageHeight;i++){
                    op_memcpy(ps0,ps1,pCurGifInfo->prevImage.ImageWidth);
                    ps0 += pCurGifInfo->m_ScreenWidth;
			        ps1 +=pCurGifInfo->prevImage.ImageWidth;
                    }
				op_free(pCurGifInfo->prevImage.image_data);
                pCurGifInfo->prevImage.image_data = OP_NULL;
                }
                break;
            default:
                break;
            }
        }
    else
        {
        //set background color
        op_memset(pLcdBuf->data,pCurGifInfo->BackGroundColor,pCurGifInfo->m_ScreenWidth * pCurGifInfo->m_ScreenHeight);
        }
    /*
     *	convert decoded data and merger to display buf
     */
    bool_retval = ConvertToOpusBmp(ImageData->ImageLeft, ImageData->ImageTop, pTrueCodeData, pLcdBuf->data);
    if ( OP_FALSE == bool_retval )
    {
        return OP_FALSE;
    }

    /*
     *	set src ptr to next next block
     */
    *pOffset = IMAGEDATAINFO_SIZE + pCurGifInfo->offset_length + PalSize;

     return OP_TRUE;
}

/*==================================================================================================
    GLOBAL FUNCTIONS
==================================================================================================*/


int GIF_LoadData
(
    OP_UINT32     handle,
    RM_BITMAP_T   *pLcdBuf,
    OP_UINT16     *delay
)
{
    OP_UINT32  Offset1                  = 0;
    OP_UINT8   *p                       = OP_NULL;
    GIF_INFO_ARRAY_STRUCTRUE_T  *pFound = OP_NULL; 
    OP_BOOLEAN                  bool_retval = OP_FALSE;


    if ( OP_NULL == pLcdBuf || OP_NULL == delay )
    {
        op_debug( DEBUG_HIGH, "gif_to_bmp.c---GIF_LoadData---parameter is NULL pointer!\n" );
        return GIF_ERROR;
    }


    pFound = gif_find_info(handle);
    if((OP_NULL == pFound)||(OP_NULL == pFound->pGifInfo))
    {
        return GIF_ERROR;
    }
        
    pCurGifInfo = pFound->pGifInfo;

    /*
     * need to check
     */
    p = pCurGifInfo->m_pDataArea;
    
    if(pCurGifInfo->first_enty)
    {
       p += pCurGifInfo->Offset;    
    }
    
    //save previous image imfo
    if(pCurGifInfo->first_enty)
        {
        op_memcpy(&(pCurGifInfo->prevImage), &(pCurGifInfo->currImage), sizeof(IMAGEDATAINFO));
   	}

    while(1)
    {
        if (p[0] == 0x21 && p[1] == 0xf9 && p[2] == 0x04)
        {
            Offset1 = GetGrphContent(p);
            p += Offset1;
            pCurGifInfo->Offset +=Offset1;

        }
        else if (p[0] == 0x21 && p[1] == 0x01 && p[2] == 0x0c)
        {
            Offset1 = ShowText(p);
            p += Offset1;
            pCurGifInfo->Offset +=Offset1;

        }
        else if (p[0] == 0x21 && p[1] == 0xff && p[2] == 0x0b)
        {
            Offset1 = GetAppContent(p);
            p += Offset1;
            pCurGifInfo->Offset +=Offset1;

        }
        else if (p[0] == 0x21 && p[1] == 0xfe)
        {
            Offset1 = GetNoteContent(p);
            p += Offset1;    
            pCurGifInfo->Offset +=Offset1;
        
        }
        else if (p[0] == 0x2c)
        {
            /* 0 0 1 0 1 1 0 0   - Image separator character*/
            bool_retval = GetImage( 0, 0, p, pLcdBuf, &Offset1 );
            if ( OP_FALSE == bool_retval )
            {
                return GIF_ERROR;
            }
            p += Offset1;    
            *delay = pCurGifInfo->currImage.graphctrl.DelayTime;
            pCurGifInfo->Offset +=Offset1;

            Offset1 = pCurGifInfo->m_fileSize - pCurGifInfo->m_HeaderSize - 1;
            if((OP_FALSE == pCurGifInfo->first_enty)&&
               (pCurGifInfo->Offset >= Offset1))
            {
                return GIF_ONLY_ONE_FRAME;
            }
            pCurGifInfo->first_enty = OP_TRUE;
            return GIF_SUCCESS;
        }
        else 
        {
            pCurGifInfo->Offset = 0;
            pCurGifInfo->first_enty = OP_FALSE;
            return GIF_TERMINATED;
        }
    }

    return GIF_ERROR;
}



OP_UINT32 GIF_AnalizeFileHeader
(
    OP_UINT32 handle,
    OP_UINT8  *pFileContent,
    OP_UINT32 fileSize,
    OP_UINT16 *pColorPallet
)
{
    OP_UINT32  j;
    OP_UINT8   *Filedata = OP_NULL;
    GIFHEADER  gifHeader;    /* GIF File Header */
    OP_UINT16  wRed, wGreen, wBlue;
    GIF_INFO_ARRAY_STRUCTRUE_T  *pFound = OP_NULL;


     if ( OP_NULL == pFileContent || OP_NULL == pColorPallet)
     {
        op_debug( DEBUG_HIGH, "gif_to_bmp.c---GIF_AnalizeFileHeader---parameter is NULL pointer!\n" );
        return 0;
     }
    

    pFound = gif_find_info(handle);
    if(OP_NULL == pFound )
    {
        pFound = gif_get_free();
        if(!pFound)
        {
            return 0;
        }
    }
    else if(pFound->pGifInfo)
    {
        return pFound->pGifInfo->m_HeaderSize;
    }
    
    pFound->handle = handle;
    if( OP_NULL == pFound->pGifInfo )
    {
        pFound->pGifInfo = op_new(GIF_INFO_STRUCTRUE_T);
        if ( OP_NULL == pFound->pGifInfo )
        {
            return 0;
        }
    }
    pCurGifInfo = pFound->pGifInfo;
    pCurGifInfo->first_enty = OP_FALSE;
    op_memset(pCurGifInfo,0,sizeof(GIF_INFO_STRUCTRUE_T));
    pCurGifInfo->m_HeaderSize =  GIFHEADER_SIZE;

    pCurGifInfo->m_fileSize = fileSize;
    pCurGifInfo->m_TailPointer = pFileContent + fileSize;
    
    Filedata = pFileContent;
    /*
     * The following GIF Signature identifies the data following  as  a
     * valid GIF image stream.  It consists of the following six characters:
     *    G I F 8 7 a
     */ 
    op_memcpy(&gifHeader.Signature,Filedata,6);
    Filedata +=6;

    /*
     * Raster width in pixels (LSB first)
     */
    op_memcpy(&gifHeader.ScreenWidth,Filedata,2);
    Filedata +=2;

    /*
     * Screen Height -- Raster height in pixels (LSB first)
     */
    op_memcpy(&gifHeader.ScreenDepth,Filedata,2);
    Filedata +=2;

    /*
     * +-+-----+-+-----+      M = 1, Global color map follows Descriptor
     * |M|  cr |0|pixel|  5   cr+1 = # bits of color resolution
     * +-+-----+-+-----+      pixel+1 = # bits/pixel in image     
     */
    op_memcpy(&gifHeader.GlobalFlagByte,Filedata,1);
    Filedata +=1;

    /*
     * background = Color index of screen background (color is defined from
     * the Global color map or default map if none specified)
     */
    op_memcpy(&gifHeader.BackGroundColor,Filedata,1);
    Filedata +=1;
    
    op_memcpy(&gifHeader.AspectRadio,Filedata,1);
    Filedata +=1;

    pCurGifInfo->m_ScreenWidth = gifHeader.ScreenWidth ;
    pCurGifInfo->m_GifInfo.Width = gifHeader.ScreenWidth ;
    pCurGifInfo->m_ScreenHeight = gifHeader.ScreenDepth ;
    pCurGifInfo->m_GifInfo.Height = gifHeader.ScreenDepth ;
    pCurGifInfo->BackGroundColor = gifHeader.BackGroundColor;
    
    /* pixel+1 = # bits/pixel in image 1,2,3,4,5,6,7,8 */     
    pCurGifInfo->m_GifInfo.ColorMode  = (gifHeader.GlobalFlagByte & 0x07) + 1;
    pCurGifInfo->m_GifInfo.InitPixelBits = pCurGifInfo->m_GifInfo.ColorMode + 1;
    pCurGifInfo->m_GifInfo.ColorType = 1;
    pCurGifInfo->m_GifInfo.ColorType = pCurGifInfo->m_GifInfo.ColorType << pCurGifInfo->m_GifInfo.ColorMode;
    pCurGifInfo->p_CurRgbQuad = pColorPallet;
    /* if M = 1, Global color map follows Descriptor */
    if (gifHeader.GlobalFlagByte & 0x80)
    { 
        /* read pal data and save it to BitMap pal */
        OP_UINT8* p = pFileContent + GIFHEADER_SIZE;    
        OP_UINT8 * pRGB = p;
        for (j = 0; j < pCurGifInfo->m_GifInfo.ColorType ; j ++)
        {
            /* get the firt byte -- Red byte */
            wRed = (*pRGB >> 3) & 0x1f;
            pCurGifInfo->m_GLBRgbQuad[j] = (wRed<<11)&0xF800;
            pRGB++;

            /* get the second byte -- Green byte */
            wGreen = (*pRGB >> 2) & 0x3f;
            pCurGifInfo->m_GLBRgbQuad[j] |= (wGreen<<5)&0x7E0;
            pRGB++;

            /* get the third byte -- Blue byte */
            wBlue = *pRGB;
            pCurGifInfo->m_GLBRgbQuad[j] |= (wBlue >> 3) & 0x1f;
            pRGB++;
        }
        p += 3 * pCurGifInfo->m_GifInfo.ColorType;
        pCurGifInfo->m_HeaderSize = GIFHEADER_SIZE+ 3 * pCurGifInfo->m_GifInfo.ColorType;
    }
    pCurGifInfo->m_pDataArea = pFileContent+pCurGifInfo->m_HeaderSize;
    return pCurGifInfo->m_HeaderSize;
}

void GIF_Terminate
(
    int       handle
)
{
    GIF_INFO_ARRAY_STRUCTRUE_T  *pFound;

    pFound = gif_find_info(handle);
    if( pFound )
    {
        pFound->bUse = OP_FALSE;
        pFound->handle = 0;
        if( pFound->pGifInfo )
        {
            op_free(pFound->pGifInfo);
        }
    }
    PartImageDataBuffer( RELEASE );
    TrueDataBuffer( RELEASE, 0 );
}



static OP_UINT16* PartImageDataBuffer( BUFFER_PARAM_ENUM action )
{
    static OP_UINT16* pBuffer = OP_NULL;
    switch( action )
    {
    case GET:
        if ( OP_NULL == pBuffer )
        {
            pBuffer = op_alloc( PART_IMAGE_DATA_BUFFER_SIZE );
            if ( OP_NULL == pBuffer )
            {
                op_debug( DEBUG_HIGH, "GIF----PartImageDataBuffer---op_alloc failed!\n" );
                return OP_NULL;
            }
        }
        break;
    case RELEASE:
        if ( pBuffer != OP_NULL )
        {
            op_free( pBuffer );
            pBuffer = OP_NULL;
        }
        break;
    default:
        break;
    }
    if ( pBuffer != OP_NULL )
    {
        op_memset( pBuffer, 0, PART_IMAGE_DATA_BUFFER_SIZE );
    }
    return pBuffer;
}


static OP_UINT8*    TrueDataBuffer( BUFFER_PARAM_ENUM action, OP_UINT32 new_size )
{
    static OP_UINT8* pBuffer = OP_NULL;
    static OP_UINT32 orignial_size = 0;
    switch( action )
    {
    case GET:
        if ( OP_NULL == pBuffer )
        {
            pBuffer = op_alloc( new_size );
            orignial_size = new_size;
            if ( OP_NULL == pBuffer )
            {
                op_debug( DEBUG_HIGH, "GIF----TrueDataBuffer---op_alloc failed!\n" );
                return OP_NULL;
            }
        }
        else if ( new_size > orignial_size )
        {
            op_free( pBuffer );
            pBuffer = op_alloc( new_size );
            orignial_size = new_size;
            if ( OP_NULL == pBuffer )
            {
                op_debug( DEBUG_HIGH, "GIF----TrueDataBuffer---op_alloc failed!\n" );
                return OP_NULL;
            }
        }
        break;
    case RELEASE:
        if ( pBuffer != OP_NULL )
        {
            op_free( pBuffer );
            pBuffer = OP_NULL;
            orignial_size = 0;
        }
        break;
    default:
        break;
    }
    if ( pBuffer != OP_NULL )
    {
        op_memset( pBuffer, 0, new_size );
    }
    return pBuffer;
}
/*==================================================================================================
    GLOBAL FUNCTIONS
==================================================================================================*/



/*================================================================================================*/

#ifdef __cplusplus
}
#endif
