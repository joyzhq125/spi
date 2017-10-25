/*
 * Copyright (C) Techfaith 2002-2005.
 * All rights reserved.
 *
 * This software is covered by the license agreement between
 * the end user and Techfaith, and may be 
 * used and copied only in accordance with the terms of the 
 * said agreement.
 *
 * Techfaith assumes no responsibility or 
 * liability for any errors or inaccuracies in this software, 
 * or any consequential, incidental or indirect damage arising
 * out of the use of the software.
 *
 */
/*
 * Definitions needed as part of the adaptation for a
 * specific platform.
 *
 */
#ifndef WE_DEF_H
#define WE_DEF_H

/************************************************************
 * Generic type definitions
 ************************************************************/

typedef signed char      WE_INT8;   /*  INT8    :   8 bit signed integer  */
typedef unsigned char   WE_UINT8;  /*  UINT8   :   8 bit unsigned integer  */
typedef signed short      WE_INT16;  /*  INT16   :   16 bit signed integer   */
typedef unsigned short  WE_UINT16; /*  UINT16  :   16 bit unsigned integer */
typedef signed int          WE_INT32;  /*  INT32   :   32 bit signed integer   */
typedef unsigned int      WE_UINT32; /*  UINT32  :   32 bit unsigned integer */
typedef signed long         WE_LONG;  /*  INT32   :   32 bit signed integer   */
typedef unsigned long   WE_ULONG; /*  UINT32  :   32 bit unsigned integer */
typedef int                     WE_BOOL;   /*  BOOLEAN :   int */
typedef signed short      WE_WCHAR;  /*  INT16   :   16 bit signed integer   */
typedef signed char      WE_CHAR;   /*  INT8    :   8 bit signed integer  */
typedef unsigned char   WE_UCHAR;  /*  UINT8   :   8 bit unsigned integer  */


/* Define this if the platform has support for floating-point numbers. */
#define CFG_WE_HAS_FLOAT

/* If no real type exists for the host device, define as INT32. */
typedef float           WE_FLOAT32;  /* FLOAT32 :   32 bit real */


/************************************************************
 * Constants
 ************************************************************/
#ifndef NULL
#define NULL  (void *)0           /*  NULL    :   Null pointer */
#endif

#ifndef TRUE
#define TRUE    1                 /*  TRUE    :   Integer value 1 */
#endif

#ifndef FALSE
#define FALSE   0                 /*  FALSE   :   Integer value 0 */
#endif

/************************************************************
 * Defines
 ************************************************************/

#define WE_UNUSED_PARAMETER(x)                   (x) = (x)

#endif
