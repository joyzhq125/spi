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
 * We_Dcvt.c
 *
 * Created by Anders Edenwbandt.
 *
 * Revision history:
 *   010612, AED: New function, data_cvt_change_pos.
 *   011015, AED: Fixed error in decoding negative numbers.
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

#include "We_Cfg.h"
#include "We_Dcvt.h"
#include "We_Mem.h"
#include "We_Lib.h"

/************************************************************
 * Conversion routines
 ************************************************************/

/*
 * Prepare a conversion object for a specific operation,
 * using the indicated buffer of given length.
 */
void
we_dcvt_init (we_dcvt_t *obj, short operation,
               void *buf, long length, WE_UINT8 modId)
{
  obj->operation = operation;
  obj->data = (unsigned char *)buf;
  obj->length = length;
  obj->pos = 0;
  obj->module = modId;
}

/*
 * Change the current buffer position of the conversion object.
 * Returns TRUE if the change can be accomodated within
 * the bounds of the buffer, FALSE otherwise.
 */
int
we_dcvt_change_pos (we_dcvt_t* obj, long steps)
{
  long tmp = obj->pos + steps;

  switch (obj->operation) {
  case WE_DCVT_ENCODE_SIZE:
    obj->pos = tmp;
    return TRUE;
  default:
    if ((tmp >= 0) && (tmp <= obj->length)) {
      obj->pos = tmp;
      return TRUE;
    }
  }
  return FALSE;
}

/*
 * Convert a 1-byte unsigned integer.
 */
int
we_dcvt_uint8 (we_dcvt_t *obj, WE_UINT8 *p)
{
  switch (obj->operation) {
  case WE_DCVT_DECODE:
    if (WE_DCVT_REM_LENGTH (obj) < 1)
      return FALSE;
    *p = WE_DCVT_GET_BYTE (obj);
    break;

  case WE_DCVT_ENCODE:
    if (WE_DCVT_REM_LENGTH (obj) < 1)
      return FALSE;
    WE_DCVT_PUT_BYTE (obj, *p);
    break;

  case WE_DCVT_ENCODE_SIZE:
    obj->pos += 1;
    break;

  case WE_DCVT_FREE:
    break;

  default:
    return FALSE;
  }

  return TRUE;
}

/*
 * Convert a 1-byte signed integer in two's complement format.
 */
int
we_dcvt_int8 (we_dcvt_t *obj, WE_INT8 *p)
{
  WE_UINT8 a;

  switch (obj->operation) {
  case WE_DCVT_DECODE:
    if (WE_DCVT_REM_LENGTH (obj) < 1)
      return FALSE;
    a = WE_DCVT_GET_BYTE (obj);
    if (a & 0x80) {
      /* We have a negative number */
      a = (WE_UINT8)(a & 0x7f);
      if (a == 0) {
        *p = -0x7f - 1;
      }
      else {
        a = (WE_UINT8)((0x7f - a) + 1);
        *p = (WE_INT8)(-(WE_INT8)a);
      }
    }
    else {
      *p = (WE_INT8)a;
    }
    break;

  case WE_DCVT_ENCODE:
    if (WE_DCVT_REM_LENGTH (obj) < 1)
      return FALSE;
    if (*p < 0) {
      a = (WE_UINT8)((0x7f + *p) + 1);
      a |= 0x80;
    }
    else {
      a = (WE_UINT8)*p;
    }
    WE_DCVT_PUT_BYTE (obj, a);
    break;

  case WE_DCVT_ENCODE_SIZE:
    obj->pos += 1;
    break;

  case WE_DCVT_FREE:
    break;

  default:
    return FALSE;
  }

  return TRUE;
}

/*
 * Convert a 2-byte unsigned integer in big-endian format.
 */
int
we_dcvt_uint16 (we_dcvt_t *obj, WE_UINT16 *p)
{
  WE_UINT8 a, b;

  switch (obj->operation) {
  case WE_DCVT_DECODE:
    if (WE_DCVT_REM_LENGTH (obj) < 2)
      return FALSE;
    a = WE_DCVT_GET_BYTE (obj);
    b = WE_DCVT_GET_BYTE (obj);
    *p = (WE_UINT16)(((WE_UINT16)a << 8) | (WE_UINT16)b);
    break;

  case WE_DCVT_ENCODE:
    if (WE_DCVT_REM_LENGTH (obj) < 2)
      return FALSE;
    WE_DCVT_PUT_BYTE (obj, ((*p >> 8) & 0xff));
    WE_DCVT_PUT_BYTE (obj, (*p & 0xff));
    break;

  case WE_DCVT_ENCODE_SIZE:
    obj->pos += 2;
    break;

  case WE_DCVT_FREE:
    break;

  default:
    return FALSE;
  }

  return TRUE;
}

/*
 * Convert a 2-byte signed integer in big-endian, two's complement format.
 */
int
we_dcvt_int16 (we_dcvt_t *obj, WE_INT16 *p)
{
  unsigned char a, b;
  unsigned int  n;

  switch (obj->operation) {
  case WE_DCVT_DECODE:
    if (WE_DCVT_REM_LENGTH (obj) < 2)
      return FALSE;
    a = WE_DCVT_GET_BYTE (obj);
    b = WE_DCVT_GET_BYTE (obj);
    n = (WE_UINT16)(((WE_UINT16)a << 8) | (WE_UINT16)b);
    if (n & 0x8000) {
      /* We have a negative number */
      n = (WE_UINT16)(n & 0x7fff);
      if (n == 0) {
        *p = -0x7fff - 1;
      }
      else {
        n = (WE_UINT16)((0x7fff - n) + 1);
        *p = (WE_INT16)(-(WE_INT16)n);
      }
    }
    else {
      *p = (WE_INT16)n;
    }
    break;

  case WE_DCVT_ENCODE:
    if (WE_DCVT_REM_LENGTH (obj) < 2)
      return FALSE;
    if (*p < 0) {
      n = (WE_UINT16)((0x7fff + *p) + 1);
      n |= 0x8000;
    }
    else {
      n = *p;
    }
    WE_DCVT_PUT_BYTE (obj, ((n >> 8) & 0xff));
    WE_DCVT_PUT_BYTE (obj, (n & 0xff));
    break;

  case WE_DCVT_ENCODE_SIZE:
    obj->pos += 2;
    break;

  case WE_DCVT_FREE:
    break;

  default:
    return FALSE;
  }

  return TRUE;
}

/*
 * Convert a 4-byte unsigned integer in big-endian format.
 */
int
we_dcvt_uint32 (we_dcvt_t *obj, WE_UINT32 *p)
{
  unsigned char a, b, c, d;

  switch (obj->operation) {
  case WE_DCVT_DECODE:
    if (WE_DCVT_REM_LENGTH (obj) < 4)
      return FALSE;
    a = WE_DCVT_GET_BYTE (obj);
    b = WE_DCVT_GET_BYTE (obj);
    c = WE_DCVT_GET_BYTE (obj);
    d = WE_DCVT_GET_BYTE (obj);
    *p = ((WE_UINT32)a << 24) | ((WE_UINT32)b << 16) | ((WE_UINT32)c << 8) | (WE_UINT32)d;
    break;

  case WE_DCVT_ENCODE:
    if (WE_DCVT_REM_LENGTH (obj) < 4)
      return FALSE;
    WE_DCVT_PUT_BYTE (obj, ((*p >> 24) & 0xff));
    WE_DCVT_PUT_BYTE (obj, ((*p >> 16) & 0xff));
    WE_DCVT_PUT_BYTE (obj, ((*p >> 8) & 0xff));
    WE_DCVT_PUT_BYTE (obj, (*p & 0xff));
    break;

  case WE_DCVT_ENCODE_SIZE:
    obj->pos += 4;
    break;

  case WE_DCVT_FREE:
    break;

  default:
    return FALSE;
  }

  return TRUE;
}

#ifdef CFG_WE_HAS_FLOAT
int
we_dcvt_float32 (we_dcvt_t *obj, WE_FLOAT32 *p)
{
  unsigned char a, b, c, d;
  WE_UINT32        u;

  switch (obj->operation) {
  case WE_DCVT_DECODE:
    if (WE_DCVT_REM_LENGTH (obj) < 4)
      return FALSE;
    a = WE_DCVT_GET_BYTE (obj);
    b = WE_DCVT_GET_BYTE (obj);
    c = WE_DCVT_GET_BYTE (obj);
    d = WE_DCVT_GET_BYTE (obj);
    u = ((WE_UINT32)a << 24) | ((WE_UINT32)b << 16) | ((WE_UINT32)c << 8) | (WE_UINT32)d;
    *p = *(WE_FLOAT32 *)&u;
    break;

  case WE_DCVT_ENCODE:
    if (WE_DCVT_REM_LENGTH (obj) < 4)
      return FALSE;
    u = *(WE_UINT32 *)p;
    WE_DCVT_PUT_BYTE (obj, ((u >> 24) & 0xff));
    WE_DCVT_PUT_BYTE (obj, ((u >> 16) & 0xff));
    WE_DCVT_PUT_BYTE (obj, ((u >> 8) & 0xff));
    WE_DCVT_PUT_BYTE (obj, (u & 0xff));
    break;

  case WE_DCVT_ENCODE_SIZE:
    obj->pos += 4;
    break;

  case WE_DCVT_FREE:
    break;

  default:
    return FALSE;
  }

  return TRUE;
}
#endif

/*
 * Convert a 4-byte signed integer in big-endian, two's complement format.
 */
int
we_dcvt_int32 (we_dcvt_t *obj, WE_INT32 *p)
{
  unsigned char     a, b, c, d;
  WE_UINT32 n;

  switch (obj->operation) {
  case WE_DCVT_DECODE:
    if (WE_DCVT_REM_LENGTH (obj) < 4)
      return FALSE;
    a = WE_DCVT_GET_BYTE (obj);
    b = WE_DCVT_GET_BYTE (obj);
    c = WE_DCVT_GET_BYTE (obj);
    d = WE_DCVT_GET_BYTE (obj);
    n = ((WE_UINT32)a << 24) | ((WE_UINT32)b << 16) | ((WE_UINT32)c << 8) | (WE_UINT32)d;
    if (n & 0x80000000) {
      /* We have a negative number */
      n = (WE_UINT32)(n & 0x7fffffff);
      if (n == 0) {
        *p = -0x7fffffff - 1;
      }
      else {
        n = (0x7fffffff - n) + 1;
        *p = -(WE_INT32)n;
      }
    }
    else {
      *p = (WE_INT32)n;
    }
    break;

  case WE_DCVT_ENCODE:
    if (WE_DCVT_REM_LENGTH (obj) < 4)
      return FALSE;
    if (*p < 0) {
      n = (0x7fffffff + *p) + 1;
      n |= 0x80000000;
    }
    else {
      n = *p;
    }
    WE_DCVT_PUT_BYTE (obj, ((n >> 24) & 0xff));
    WE_DCVT_PUT_BYTE (obj, ((n >> 16) & 0xff));
    WE_DCVT_PUT_BYTE (obj, ((n >> 8) & 0xff));
    WE_DCVT_PUT_BYTE (obj, (n & 0xff));
    break;

  case WE_DCVT_ENCODE_SIZE:
    obj->pos += 4;
    break;

  case WE_DCVT_FREE:
    break;

  default:
    return FALSE;
  }

  return TRUE;
}

/*
 * Convert a Boolean.
 * NOTE: the name WE_BOOL is an alias for 'int'. The size
 * of an 'int' is platform-dependent, but since a WE_BOOL
 * can only hold the value 0 or 1, a WE_BOOL will always
 * be encoded as one unsigned byte.
 */
int
we_dcvt_bool (we_dcvt_t *obj, WE_BOOL *p)
{
  WE_UINT8 tmp;

  switch (obj->operation) {
  case WE_DCVT_DECODE:
    if (WE_DCVT_REM_LENGTH (obj) < 1)
      return FALSE;
    tmp = WE_DCVT_GET_BYTE (obj);
    *p = (WE_BOOL)tmp;
    break;

  case WE_DCVT_ENCODE:
    if (WE_DCVT_REM_LENGTH (obj) < 1)
      return FALSE;
    tmp = (WE_UINT8)*p;
    WE_DCVT_PUT_BYTE (obj, tmp);
    break;

  case WE_DCVT_ENCODE_SIZE:
    obj->pos += 1;
    break;

  case WE_DCVT_FREE:
    break;

  default:
    return FALSE;
  }

  return TRUE;
}

/*
 * Return the number of bytes required to store n as a Uintvar.
 */
int
we_dcvt_uintvar_len (WE_UINT32 n)
{
  if (n < 0x80)
    return 1;
  else if (n < 0x4000)
    return 2;
  else if (n < 0x200000)
    return 3;
  else if (n < 0x10000000)
    return 4;
  else
    return 5;
}

/*
 * Convert a Uintvar.
 */
int
we_dcvt_uintvar (we_dcvt_t *obj, WE_UINT32 *p)
{
  switch (obj->operation) {
  case WE_DCVT_DECODE:
    {
      WE_UINT32        n = 0;
      WE_UINT8         k = 0;
      unsigned char b;

      do {
        if (WE_DCVT_REM_LENGTH (obj) <= 0)
          return FALSE;
        b = WE_DCVT_GET_BYTE (obj);
        n <<= 7;
        n |= (b & 0x7f);
        k++;
        if (k > 5)
          return FALSE;
      } while (b > 0x7f);
      *p = n;
    }
    break;

  case WE_DCVT_ENCODE:
    {
      WE_UINT32 n = *p;
      int    k = we_dcvt_uintvar_len (n);
      WE_UINT8  b;

      if (WE_DCVT_REM_LENGTH (obj) < k)
        return FALSE;
      for (; k > 0; k--) {
        b = (WE_UINT8)((n >> ((k - 1) * 7)) & 0x7f);
        if (k > 1)
          b |= 0x80;
        WE_DCVT_PUT_BYTE (obj, b);
      }
    }
    break;

  case WE_DCVT_ENCODE_SIZE:
    obj->pos += we_dcvt_uintvar_len (*p);
    break;

  case WE_DCVT_FREE:
    break;

  default:
    return FALSE;
  }

  return TRUE;
}

/*
 * Convert a static bytevector, i.e., a vector whose internal
 * representation is a static vector (e.g., unsigned char v[12];).
 */
int
we_dcvt_static_uchar_vector (we_dcvt_t *obj, long length, unsigned char *p)
{
  switch (obj->operation) {
  case WE_DCVT_DECODE:
    if (WE_DCVT_REM_LENGTH (obj) < length)
      return FALSE;
    memcpy (p, obj->data + obj->pos, length);
    obj->pos += length;
    break;

  case WE_DCVT_ENCODE:
    if (WE_DCVT_REM_LENGTH (obj) < length)
      return FALSE;
    memcpy (obj->data + obj->pos, p, length);
    /* Fall through */

  case WE_DCVT_ENCODE_SIZE:
    obj->pos += length;
    break;

  case WE_DCVT_FREE:
    break;

  default:
    return FALSE;
  }

  return TRUE;
}

/*
 * Convert a byte vector, i.e., a vector whose internal representation
 * is a pointer to an unsigned char.
 */
int
we_dcvt_uchar_vector (we_dcvt_t *obj, long length, unsigned char **p)
{
  switch (obj->operation) {
  case WE_DCVT_DECODE:
    if (WE_DCVT_REM_LENGTH (obj) < length) {
      *p = NULL;
      return FALSE;
    }
    if (length == 0) {
      *p = NULL;
      break;
    }
    *p = WE_MEM_ALLOC (obj->module, length);
    memcpy (*p, obj->data + obj->pos, length);
    obj->pos += length;
    break;

  case WE_DCVT_ENCODE:
    if (WE_DCVT_REM_LENGTH (obj) < length)
      return FALSE;
    memcpy (obj->data + obj->pos, *p, length);
    /* Fall through */

  case WE_DCVT_ENCODE_SIZE:
    obj->pos += length;
    break;

  case WE_DCVT_FREE:
    WE_MEM_FREE (obj->module, *p);
    break;

  default:
    return FALSE;
  }

  return TRUE;
}

int
we_dcvt_string (we_dcvt_t *obj, char **p)
{
  const char *s;
  long        length;
  int         i;

  switch (obj->operation) {
  case WE_DCVT_DECODE:
    length = WE_DCVT_REM_LENGTH (obj);
    s = (const char*)(obj->data + obj->pos);
    for (i = 0; i < length; i++) {
      if (s[i] == '\0')
        break;
    }
    if (i == length) {
      *p = NULL;
      return FALSE;
    }
    length = i;
    if (length == 0) {
      *p = NULL;
    }
    else {
      *p = WE_MEM_ALLOC (obj->module, length + 1);
      memcpy ((void *)*p, obj->data + obj->pos, length);
      ((char *)*p)[length] = '\0';
    }
    obj->pos += length + 1;
    break;

  case WE_DCVT_ENCODE:
    length = ((*p != NULL) ? strlen (*p) : 0);
    if (WE_DCVT_REM_LENGTH (obj) < length + 1)
      return FALSE;
    if (*p != NULL)
      memcpy (obj->data + obj->pos, *p, length);
    (obj->data + obj->pos)[length] = '\0';
    obj->pos += length + 1;
    break;

  case WE_DCVT_ENCODE_SIZE:
    length = ((*p != NULL) ? strlen (*p) : 0);
    obj->pos += length + 1;
    break;

  case WE_DCVT_FREE:
    WE_MEM_FREE (obj->module, (void *)*p);
    break;

  default:
    return FALSE;
  }

  return TRUE;
}

int
we_dcvt_array (we_dcvt_t *obj, int element_size,
                int num_elements, void **array,
                we_dcvt_element_t *cvt_element)
{
  int   i;
  long  length;
  char *p;

  switch (obj->operation) {
  case WE_DCVT_DECODE:
    length = element_size * num_elements;
    if (length == 0) {
      *array = NULL;
      break;
    }
    *array = p = WE_MEM_ALLOC (obj->module, length);
    for (i = 0; i < num_elements; i++) {
      if (!cvt_element (obj, p + i * element_size)) {
        WE_MEM_FREE (obj->module, p);
        return FALSE;
      }
    }
    break;

  case WE_DCVT_ENCODE:
  case WE_DCVT_ENCODE_SIZE:
    p = *array;
    for (i = 0; i < num_elements; i++) {
      if (!cvt_element (obj, p + i * element_size)) {
        return FALSE;
      }
    }
    break;

  case WE_DCVT_FREE:
    p = *array;
    for (i = 0; i < num_elements; i++) {
      if (!cvt_element (obj, p + i * element_size)) {
        return FALSE;
      }
    }
    WE_MEM_FREE (obj->module, p);
    break;

  default:
    return FALSE;
  }

  return TRUE;
}

