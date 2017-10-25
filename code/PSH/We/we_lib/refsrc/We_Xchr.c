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
 * We_Xchr.c
 * 
 * Additional Character Set Support
 *
 *
 * Place in this file all additional character set conversion
 * routines that are needed. As an example, conversions to and
 * from ISO-8859-2 (Latin2) and ISO-8859-8 (Hebrew), are included.
 * These can be removed if not required.
 * If no additional character set support is required, this
 * whole file can be left empty.
 */
#include "We_Def.h"
#include "We_Chrs.h"
#include "We_Xchr.h"


/**********************************************************************
 * Function Declarations
 **********************************************************************/

static int
we_xchr_latin2_to_utf8 (const char *src, long *srcLen, char *dst, long *dstLen);

static int
we_xchr_utf8_to_latin2 (const char *src, long *srcLen, char *dst, long *dstLen);

static int
we_xchr_hebrew_to_utf8 (const char *src, long *srcLen, char *dst, long *dstLen);

static int
we_xchr_utf8_to_hebrew (const char *src, long *srcLen, char *dst, long *dstLen);


/**********************************************************************
 * Exported Global Variable
 **********************************************************************/

/*
 * This is the table that holds information about all additional
 * character sets that are supported.
 *
 * The first field, 'character_set', should hold the IANA registered
 * number for the character set in question.
 *
 * The second field, 'preferred_name', should hold a string with
 * the preferred name that identifies this character set.
 *
 * The third field, 'aliases', should hold a string with comma-separated
 * names that are recognized as alternative names for this character set.
 * If there are no such aliases, use the empty string (""). Do not include
 * any spaces among the names.
 *
 * The fourth field, 'cvt_to_utf8, should be a pointer to the function
 * that performs the conversion from the character set in question
 * to UTF-8. If no such function is provided, set this field to NULL.
 *
 * The fifth field, 'cvt_from_utf8, should be a pointer to the function
 * that performs the conversion from UTF-8 to the character set in question.
 * If no such function is provided, set this field to NULL. At least one
 * of the fields 'cvt_to_utf8' and 'cvt_from_utf8' must be different
 * from NULL, otherwise the entry serves no purpose.
 *
 */
const we_xchr_entry_t we_xchr_table[WE_NUMBER_OF_ADDITIONAL_CHARACTER_SETS] = {
  {5, "iso-8859-2", "latin2,iso-ir-101,iso_8859-2,l2,csISOLatin2",
    we_xchr_latin2_to_utf8, we_xchr_utf8_to_latin2},
  {11, "iso-8859-8", "hebrew,iso-ir-138,iso_8859-8,csISOLatinHebrew",
     we_xchr_hebrew_to_utf8, we_xchr_utf8_to_hebrew}
};


/**********************************************************************
 * Functions
 **********************************************************************/

static int
we_xchr_8859_to_utf8 (const char *src, long *srcLen, char *dst, long *dstLen,
                       const WE_UINT16 *code_table)
{
  WE_UINT32 w;
  int        sidx = 0, didx = 0;
  int        dn;
  WE_UINT8  mask;

  for (;;) {
    /* First, translate to UCS4 */
    if (sidx + 1 > *srcLen)
      break;

    if (((unsigned char)src[sidx]) <= 127) {
      w = (unsigned char)src[sidx];
    }
    else {
      w = (WE_UINT32)(code_table[(unsigned char)src[sidx] - 128]);
    }

    /* Determine how many UTF8-bytes are needed for this character,
     * and which bits should be set in the first byte. */
    if (w < 0x80) {
      dn = 1;
      mask = 0;
    }
    else if (w < 0x800) {
      dn = 2;
      mask = 0xc0;
    }
    else {
      dn = 3;
      mask = 0xe0;
    }

    /* Is there room for this in the destination vector? */
    if (didx + dn > *dstLen)
      break;

    /* Only write to destination vector if dst != NULL */
    if (dst != NULL) {
      switch (dn) {
      case 3:
        dst[didx + 2] = (char) ((w & 0x3f) | 0x80);
        w >>= 6;

      case 2:
        dst[didx + 1] = (char) ((w & 0x3f) | 0x80);
        w >>= 6;

      case 1:
        dst[didx] = (char) (w | mask);
      }
    }
    sidx += 1;
    didx += dn;
  }
  *srcLen = sidx;
  *dstLen = didx;

  return 0;
}


/**********************************************************************
 * Support for character set ISO-8859-2 (Latin-2)
 **********************************************************************/

static const WE_UINT16 we_xchr_latin2_code_table[256] = { 
  /* Not used */ 
  0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 
  0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 
  0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 
  0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 
  
  /* Latin-2 */ 
  0x0a0, 0x104, 0x2d8, 0x141, 0x0a4, 0x13d, 0x15a, 0x0a7,         
  0x0a8, 0x160, 0x15e, 0x164, 0x179, 0x0ad, 0x17d, 0x17b, 
  0x0b0, 0x105, 0x2db, 0x142, 0x0b4, 0x13e, 0x15b, 0x2c7, 
  0x0b8, 0x161, 0x15f, 0x165, 0x17a, 0x2dd, 0x17e, 0x17c, 
  0x154, 0x0c1, 0x0c2, 0x102, 0x0c4, 0x139, 0x106, 0x0c7, 
  0x10c, 0x0c9, 0x118, 0x0cb, 0x11a, 0x0cd, 0x0ce, 0x10e, 
  0x110, 0x143, 0x147, 0x0d3, 0x0d4, 0x150, 0x0d6, 0x0d7, 
  0x158, 0x16e, 0x0da, 0x170, 0x0dc, 0x0dd, 0x162, 0x0df, 
  0x155, 0x0e1, 0x0e2, 0x103, 0x0e4, 0x13a, 0x107, 0x0e7, 
  0x10d, 0x0e9, 0x119, 0x0eb, 0x11b, 0x0ed, 0x0ee, 0x10f, 
  0x111, 0x144, 0x148, 0x0f3, 0x0f4, 0x151, 0x0f6, 0x0f7,     
  0x159, 0x16f, 0x0fa, 0x171, 0x0fc, 0x0fd, 0x163, 0x2d9 
}; 


/*
 * Convert from Latin-2 (ISO-8859-2) to UTF-8.
 */
static int
we_xchr_latin2_to_utf8 (const char *src, long *srcLen, char *dst, long *dstLen)
{
  return we_xchr_8859_to_utf8 (src, srcLen, dst, dstLen, we_xchr_latin2_code_table);
}


/*
 * Convert from UTF-8 to Latin-2 (ISO-8859-2).
 */
static int
we_xchr_utf8_to_latin2 (const char *src, long *srcLen, char *dst, long *dstLen)
{
  int        sidx = 0, didx = 0;
  int        sn;
  WE_UINT32 w;
  int        hc;

  while (sidx < *srcLen) {
    /* Read one Unicode character in UTF-8 format: */
    sn = we_charset_utf8_bytes_per_character[(unsigned char)*src >> 4];
    if (sidx + sn > *srcLen)
      break;

    w = 0;
    switch (sn) {
    case 0:
      return -1;

    case 4: w += (unsigned char)*src++; w <<= 6;
    case 3: w += (unsigned char)*src++; w <<= 6;
    case 2: w += (unsigned char)*src++; w <<= 6;
    case 1: w += (unsigned char)*src++;
    }
    w -= we_charset_offsets_from_utf8[sn];

    /* Encode one Unicode character in Latin-2 alphabet: */
    if (w <= 0x7f) {
      hc = w;
    }
    else {
      int i;

      hc = '?';

      for (i = 0x80; i <= 0xff; i++) {
        if (we_xchr_latin2_code_table[i] == w) {
          hc = i;
          break;
        }
      }
    }

    if (dst != NULL) {
      dst[didx] = (char)(hc);
    }
    sidx += sn;
    didx += 1;
  }
  *srcLen = sidx;
  *dstLen = didx;

  return 0;
}


/**********************************************************************
 * Support for character set ISO-8859-5 (Hebrew)
 **********************************************************************/

static const WE_UINT16 we_xchr_hebrew_code_table[256] = {
  /* Not used */
  0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
  0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
  0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
  0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,

  /* Like Latin-1 (almost) */
  0x0a0, 0x000, 0x0a2, 0x0a3, 0x0a4, 0x0a5, 0x0a6, 0x0a7,
  0x0a8, 0x0a9, 0x0d7, 0x0ab, 0x0ac, 0x0ad, 0x0ae, 0x0af,
  0x0b0, 0x0b1, 0x0b2, 0x0b3, 0x0b4, 0x0b5, 0x0b6, 0x0b7,
  0x0b8, 0x0b9, 0x0f7, 0x0bb, 0x0bc, 0x0bd, 0x0be, 0x000,

  /* Not used (except position 0xdf) */
  0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
  0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
  0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
  0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x2017,

  /* Hebrew letters */
  0x5d0, 0x5d1, 0x5d2, 0x5d3, 0x5d4, 0x5d5, 0x5d6, 0x5d7,
  0x5d8, 0x5d9, 0x5da, 0x5db, 0x5dc, 0x5dd, 0x5de, 0x5df,
  0x5e0, 0x5e1, 0x5e2, 0x5e3, 0x5e4, 0x5e5, 0x5e6, 0x5e7,
  0x5e8, 0x5e9, 0x5ea, 0x000, 0x000, 0x200e, 0x200f, 0x000
};


/*
 * Convert from hebrew (ISO-8859-8) to UTF-8.
 */
static int
we_xchr_hebrew_to_utf8 (const char *src, long *srcLen, char *dst, long *dstLen)
{
  return we_xchr_8859_to_utf8 (src, srcLen, dst, dstLen, we_xchr_hebrew_code_table);
}


/*
 * Convert from UTF-8 to hebrew (ISO-8859-8).
 */
static int
we_xchr_utf8_to_hebrew (const char *src, long *srcLen, char *dst, long *dstLen)
{
  int        sidx = 0, didx = 0;
  int        sn;
  WE_UINT32 w;
  int        hc;

  while (sidx < *srcLen) {
    /* Read one Unicode character in UTF-8 format: */
    sn = we_charset_utf8_bytes_per_character[(unsigned char)*src >> 4];
    if (sidx + sn > *srcLen)
      break;

    w = 0;
    switch (sn) {
    case 0:
      return -1;

    case 4: w += (unsigned char)*src++; w <<= 6;
    case 3: w += (unsigned char)*src++; w <<= 6;
    case 2: w += (unsigned char)*src++; w <<= 6;
    case 1: w += (unsigned char)*src++;
    }
    w -= we_charset_offsets_from_utf8[sn];

    /* Encode one Unicode character in Hebrew alphabet: */
    if (w == 0xd7) {
      hc = 0xaa;
    }
    else if (w == 0xf7) {
      hc = 0xba;
    }
    else if (w == 0x2017) {
      hc = 0xdf;
    }
    else if (w == 0x200e) {
      hc = 0xfd;
    }
    else if (w == 0x200f) {
      hc = 0xfe;
    }
    else if ((w <= 0x7f) || ((w >= 0xa0) && (w <= 0xbe))) {
      hc = (int)w;
    }
    else if ((w >= 0x5d0) && (w <= 0x5ea)) {
      hc = (w - 0x5d0) + 0xe0;
    }
    else {
      hc = '?';
    }

    if (dst != NULL) {
      dst[didx] = (char)(hc);
    }
    sidx += sn;
    didx += 1;
  }
  *srcLen = sidx;
  *dstLen = didx;

  return 0;
}


