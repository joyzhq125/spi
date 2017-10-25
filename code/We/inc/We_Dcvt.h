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
 * We_Dcvt.h
 *
 * Created by Anders Edenwbandt.
 *
 * Revision history:
 *   010612, AED: New function, data_cvt_change_pos.
 *   011025, AED: Cleanup.
 *   020402, IPN: Changed to fit Mobile Suite Framework.
 *
 */
/************************************************************
 * Functions to convert between an external and an internal
 * representation.
 *
 * All decoding and encoding functions take a pointer to a
 * "conversion object" (see type definition below) as
 * the first argument. Usually, the second argument is a pointer
 * to an element of the type to be converted.
 *
 * There are four different conversion operations:
 *   WE_DCVT_DECODE         convert from external to internal representation
 *   WE_DCVT_ENCODE         convert from internal to external representation
 *   WE_DCVT_ENCODE_SIZE    calculate the buffer size required to do an
 *                             encoding operation
 *   WE_DCVT_FREE           deallocate any memory that was allocated as
 *                             part of a previous decoding operation
 *
 * Each function returns TRUE on success and FALSE on error.
 *
 ************************************************************/
#ifndef _we_dcvt_h
#define _we_dcvt_h

#ifndef _we_def_h
#include "We_Def.h"
#endif

/************************************************************
 * Macro definitions
 ************************************************************/

#define WE_DCVT_GET_BYTE(obj)    ((obj)->data[(obj)->pos++])
#define WE_DCVT_PUT_BYTE(obj, b) ((obj)->data[(obj)->pos++] = (unsigned char)(b))

/*
 * Return the number of bytes remaining in the buffer.
 */
#define WE_DCVT_REM_LENGTH(str) ((str)->length - (str)->pos)


#define WE_DCVT_DECODE             1
#define WE_DCVT_ENCODE             2
#define WE_DCVT_ENCODE_SIZE        3
#define WE_DCVT_FREE               4

/*
 * The object used for converting between internal
 * and external data representation.
 */
typedef struct {
  long           pos;
  long           length;
  short          operation;
  unsigned char  *data;
  WE_UINT8       module;
} we_dcvt_t;

typedef int we_dcvt_element_t (we_dcvt_t *obj, void *p);

void
we_dcvt_init (we_dcvt_t *obj, short operation,
               void *buf, long length, WE_UINT8 modId);

/*
 * Change the current buffer position of the conversion object.
 * Returns TRUE if the change can be accomodated within
 * the bounds of the buffer, FALSE otherwise.
 */
int
we_dcvt_change_pos (we_dcvt_t* obj, long steps);

int
we_dcvt_uint8 (we_dcvt_t *obj, WE_UINT8 *p);

int
we_dcvt_int8 (we_dcvt_t *obj, WE_INT8 *p);

int
we_dcvt_uint16 (we_dcvt_t *obj, WE_UINT16 *p);

int
we_dcvt_int16 (we_dcvt_t *obj, WE_INT16 *p);

int
we_dcvt_uint32 (we_dcvt_t *obj, WE_UINT32 *p);

int
we_dcvt_int32 (we_dcvt_t *obj, WE_INT32 *p);

int
we_dcvt_bool (we_dcvt_t *obj, WE_BOOL *p);

int
we_dcvt_uintvar_len (WE_UINT32 n);

int
we_dcvt_uintvar (we_dcvt_t *obj, WE_UINT32 *p);

int
we_dcvt_static_uchar_vector (we_dcvt_t *obj, long length, unsigned char *p);

int
we_dcvt_uchar_vector (we_dcvt_t *obj, long length, unsigned char **p);

int
we_dcvt_string (we_dcvt_t *obj, char **p);

#ifdef CFG_WE_HAS_FLOAT
int
we_dcvt_float32 (we_dcvt_t *obj, WE_FLOAT32 *p);
#endif

int
we_dcvt_array (we_dcvt_t *obj, int element_size,
                int num_elements, void **array,
                we_dcvt_element_t *cvt_element);

#endif
