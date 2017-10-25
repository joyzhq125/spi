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
 * codecvt.c
 *
 * Created by Anders Edenwbandt, Mon May 14 15:05:31 2001.
 *
 *
 * Revision history:
 *   010629, AED: New functions to convert to UTF8.
 *                Hash table for charset names.
 *   010806, NKE: Added const to some functions.
 *   010806, NKE: Made we_charset_utf8_bytes_per_character public
 *   010926, AED: Fixed errors in several of the conversion functions.
 *                Added functions to convert from UTF8 to ASCII and
 *                Latin-1.
 *   011010  KEOL Added support for registering functions
 *                to convert from external charsets
 *   020507, AED: Modified for WE
 *   030924, AED: Handle additional (external) character sets.
 *
 */
#include "We_Def.h"
#include "We_Mem.h"
#include "We_Core.h"
#include "We_Chrs.h"
#include "We_Chrt.h"
#include "We_Cmmn.h"
#include "We_Lib.h"
#include "We_Xchr.h"


/**********************************************************************
 * Constans:
 **********************************************************************/

#define WE_CHARSET_SURROGATE_HIGH_START          0xd800UL
#define WE_CHARSET_SURROGATE_HIGH_END            0xdbffUL
#define WE_CHARSET_SURROGATE_LOW_START           0xdc00UL
#define WE_CHARSET_SURROGATE_LOW_END             0xdfffUL
#define WE_CHARSET_REPLACEMENT_CHAR              0x0000fffdUL

#define WE_CHARSET_UCS_2BE                       10001
#define WE_CHARSET_UCS_2LE                       10002

#define WE_CHARSET_BIG_ENDIAN                    1
#define WE_CHARSET_LITTLE_ENDIAN                 2


/**********************************************************************
 * Type definitions:
 **********************************************************************/

typedef struct {
  int                    from_charset;
  int                    to_charset;
  we_charset_convert_t *convert;
} we_charset_t;


/**********************************************************************
 * Global variables
 **********************************************************************/

const WE_UINT32 we_charset_offsets_from_utf8[5] =  {
  0UL, 0UL, 0x00003080UL, 0x000e2080UL, 0x03c82080UL};

const unsigned char we_charset_utf8_bytes_per_character[16] = {
  1, 1, 1, 1, 1, 1, 1, 1,
  0, 0, 0, 0, 2, 2, 3, 4};

const static we_charset_t we_charset_table[] = {
  {WE_CHARSET_ASCII,    WE_CHARSET_UTF_8,    we_charset_ascii_to_utf8},
  {WE_CHARSET_LATIN_1,  WE_CHARSET_UTF_8,    we_charset_latin1_to_utf8},
  {WE_CHARSET_UTF_16BE, WE_CHARSET_UTF_8,    we_charset_utf16be_to_utf8},
  {WE_CHARSET_UTF_16LE, WE_CHARSET_UTF_8,    we_charset_utf16le_to_utf8},
  {WE_CHARSET_UTF_16,   WE_CHARSET_UTF_8,    we_charset_utf16_to_utf8},
  {WE_CHARSET_UCS_2BE,  WE_CHARSET_UTF_8,    we_charset_ucs2be_to_utf8},
  {WE_CHARSET_UCS_2LE,  WE_CHARSET_UTF_8,    we_charset_ucs2le_to_utf8},
  {WE_CHARSET_UCS_2,    WE_CHARSET_UTF_8,    we_charset_ucs2_to_utf8},

  {WE_CHARSET_UTF_8,    WE_CHARSET_ASCII,    we_charset_utf8_to_ascii},
  {WE_CHARSET_UTF_8,    WE_CHARSET_LATIN_1,  we_charset_utf8_to_latin1},
  {WE_CHARSET_UTF_8,    WE_CHARSET_UTF_16BE, we_charset_utf8_to_utf16be},
  {WE_CHARSET_UTF_8,    WE_CHARSET_UTF_16LE, we_charset_utf8_to_utf16le},
  {WE_CHARSET_UTF_8,    WE_CHARSET_UTF_16,   we_charset_utf8_to_utf16},
  {WE_CHARSET_UTF_8,    WE_CHARSET_UCS_2BE,  we_charset_utf8_to_ucs2be},
  {WE_CHARSET_UTF_8,    WE_CHARSET_UCS_2LE,  we_charset_utf8_to_ucs2le},
  {WE_CHARSET_UTF_8,    WE_CHARSET_UCS_2,    we_charset_utf8_to_ucs2},

  {WE_CHARSET_ASCII,    WE_CHARSET_UTF_16BE, we_charset_ascii_to_utf16be},
  {WE_CHARSET_ASCII,    WE_CHARSET_UTF_16LE, we_charset_ascii_to_utf16le},
  {WE_CHARSET_LATIN_1,  WE_CHARSET_UTF_16BE, we_charset_latin1_to_utf16be},
  {WE_CHARSET_LATIN_1,  WE_CHARSET_UTF_16LE, we_charset_latin1_to_utf16le},
  {WE_CHARSET_UTF_16BE, WE_CHARSET_UTF_16LE, we_charset_utf16be_to_utf16le},
  {WE_CHARSET_UTF_8,    WE_CHARSET_UTF_8,    we_charset_utf8_to_utf8},
  {0, 0, NULL}
};


const static we_strtable_entry_t we_charset_strings[] = {
  {"us-ascii",         WE_CHARSET_ASCII},
  {"ANSI_X3.4-1968",   WE_CHARSET_ASCII},
  {"iso-ir-6",         WE_CHARSET_ASCII},
  {"ansi_x3.4-1986",   WE_CHARSET_ASCII},
  {"iso_646.irv:1991", WE_CHARSET_ASCII},
  {"ascii",            WE_CHARSET_ASCII},
  {"iso646-us",        WE_CHARSET_ASCII},
  {"us",               WE_CHARSET_ASCII},
  {"ibm367",           WE_CHARSET_ASCII},
  {"cp367",            WE_CHARSET_ASCII},
  {"csascii",          WE_CHARSET_ASCII},
  {"iso-8859-1",       WE_CHARSET_LATIN_1},
  {"latin-1",          WE_CHARSET_LATIN_1},
  {"iso_8859-1:1987",  WE_CHARSET_LATIN_1},
  {"iso-ir-100",       WE_CHARSET_LATIN_1},
  {"iso_8859-1",       WE_CHARSET_LATIN_1},
  {"latin1",           WE_CHARSET_LATIN_1},
  {"l1",               WE_CHARSET_LATIN_1},
  {"ibm819",           WE_CHARSET_LATIN_1},
  {"cp819",            WE_CHARSET_LATIN_1},
  {"csisolatin1",      WE_CHARSET_LATIN_1},
  {"utf-8",            WE_CHARSET_UTF_8},
  {"iso-10646-ucs-2",  WE_CHARSET_UCS_2},
  {"csunicode",        WE_CHARSET_UCS_2},
  {"utf-16be",         WE_CHARSET_UTF_16BE},
  {"utf-16le",         WE_CHARSET_UTF_16LE},
  {"utf-16",           WE_CHARSET_UTF_16}
};


/************************************************************
 * Hash table generated by mkhash.pl
 ************************************************************/

static const unsigned char we_charset_hash_table[] = {
  15, 14, 24, 255, 20, 255, 0, 10, 2, 8,
  13, 18, 1, 5, 4, 12, 11, 255, 255, 255,
  26, 19, 7, 25, 255, 22, 255, 17, 255, 9,
  255, 16, 255, 255, 255, 3, 255, 255, 23, 21,
  6
};

#define WE_CHARSET_HASH_TABLE_SIZE               41
#define WE_CHARSET_STRING_TABLE_SIZE             27

const we_strtable_info_t we_charsets = {
  we_charset_hash_table,
  WE_CHARSET_HASH_TABLE_SIZE,
  WE_TABLE_SEARCH_LINEAR,
  we_charset_strings,
  WE_CHARSET_STRING_TABLE_SIZE
};

/************************************************************
 * End of hash table generated by mkhash.pl
 ************************************************************/

/**********************************************************************
 * Misc Functions:
 **********************************************************************/

/*
 * Find a suitable conversion function that can convert
 * from charset 'fromCharset' to charset 'toCharset'.
 * Returns NULL if no matching conversion function could be found.
 */
we_charset_convert_t *
we_charset_find_function (int fromCharset, int toCharset)
{
  int i;

  for (i = 0; we_charset_table[i].from_charset != 0; i++) {
    if ((we_charset_table[i].from_charset == fromCharset) &&
        (we_charset_table[i].to_charset == toCharset)) {
      return we_charset_table[i].convert;
    }
  }
#if WE_NUMBER_OF_ADDITIONAL_CHARACTER_SETS > 0
  for (i = 0; i < WE_NUMBER_OF_ADDITIONAL_CHARACTER_SETS; i++) {
    if ((we_xchr_table[i].character_set == fromCharset) &&
        (toCharset == WE_CHARSET_UTF_8) &&
        (we_xchr_table[i].cvt_to_utf8 != NULL)) {
      return we_xchr_table[i].cvt_to_utf8;
    }
    else if ((we_xchr_table[i].character_set == toCharset) &&
             (fromCharset == WE_CHARSET_UTF_8) &&
             (we_xchr_table[i].cvt_from_utf8 != NULL)) {
      return we_xchr_table[i].cvt_from_utf8;
    }
  }
#endif

  return NULL;
}

/*
 * Find a suitable conversion function that can convert
 * from charset 'fromCharset' to charset 'toCharset',
 * using any Byte-Order Mark that might be present in the source.
 * On calling this function, the parameter '*len' should hold the number
 * of bytes available in 'src', and on return '*len' is either 0
 * (if no BOM was found) or 2 (if a 2 byte BOM was found).
 * If there is no BOM, but 'fromCharset' is an ambigous character encoding
 * (that is, UCS-2 or UTF-16) then this function will attempt to guess
 * the byte order by checking if either of the two first bytes is zero.
 * Returns NULL if no matching conversion function could be found.
 */
we_charset_convert_t *
we_charset_find_function_bom (int fromCharset, int toCharset, const char *src, int *len)
{
  int byte_order = 0;
  int used_len = 0;
  int i;

  /* Check for Byte-Order Mark (BOM) to determine which encoding to use. */
  if (*len >= 2) {
    if (((unsigned char)src[0] == 0xfe) && ((unsigned char)src[1] == 0xff)) {
      byte_order = WE_CHARSET_BIG_ENDIAN;
      used_len = 2;
    }
    else if (((unsigned char)src[0] == 0xff) && ((unsigned char)src[1] == 0xfe)) {
      byte_order = WE_CHARSET_LITTLE_ENDIAN;
      used_len = 2;
    }
    else if ((fromCharset == WE_CHARSET_UCS_2) ||
             (fromCharset == WE_CHARSET_UTF_16)) {
      /* We really need the byte order! */
      if (((unsigned char)src[0] == 0) && ((unsigned char)src[1] != 0)) {
        byte_order = WE_CHARSET_BIG_ENDIAN;
      }
      else if (((unsigned char)src[0] != 0) && ((unsigned char)src[1] == 0)) {
        byte_order = WE_CHARSET_LITTLE_ENDIAN;
      }
    }
  }
  *len = used_len;

  if (byte_order == WE_CHARSET_BIG_ENDIAN) {
    if (fromCharset == WE_CHARSET_UCS_2) {
      fromCharset = WE_CHARSET_UCS_2BE;
    }
    else if (fromCharset == WE_CHARSET_UTF_16) {
      fromCharset = WE_CHARSET_UTF_16BE;
    }
  }
  else if (byte_order == WE_CHARSET_LITTLE_ENDIAN) {
    if (fromCharset == WE_CHARSET_UCS_2) {
      fromCharset = WE_CHARSET_UCS_2LE;
    }
    else if (fromCharset == WE_CHARSET_UTF_16) {
      fromCharset = WE_CHARSET_UTF_16LE;
    }
  }

  if (toCharset == WE_CHARSET_UCS_2) {
    toCharset = WE_CHARSET_UCS_2BE;
  }
  else if (toCharset == WE_CHARSET_UTF_16) {
    toCharset = WE_CHARSET_UTF_16BE;
  }

  for (i = 0; we_charset_table[i].from_charset != 0; i++) {
    if ((we_charset_table[i].from_charset == fromCharset) &&
        (we_charset_table[i].to_charset == toCharset)) {
      return we_charset_table[i].convert;
    }
  }

#if WE_NUMBER_OF_ADDITIONAL_CHARACTER_SETS > 0
  for (i = 0; i < WE_NUMBER_OF_ADDITIONAL_CHARACTER_SETS; i++) {
    if ((we_xchr_table[i].character_set == fromCharset) &&
        (toCharset == WE_CHARSET_UTF_8) &&
        (we_xchr_table[i].cvt_to_utf8 != NULL)) {
      return we_xchr_table[i].cvt_to_utf8;
    }
    else if ((we_xchr_table[i].character_set == toCharset) &&
             (fromCharset == WE_CHARSET_UTF_8) &&
             (we_xchr_table[i].cvt_from_utf8 != NULL)) {
      return we_xchr_table[i].cvt_from_utf8;
    }
  }
#endif

  return NULL;
}

/*
 * Given an integer representation of a charset,
 * return a pointer to a string holding the name of that charset.
 * NOTE: the caller MUST NOT deallocate or modify the returned string!
 */
const char *
we_charset_int2str (int charset)
{
  const char *s = we_cmmn_int2str (charset, &we_charsets);

#if WE_NUMBER_OF_ADDITIONAL_CHARACTER_SETS > 0
  if (s == NULL) {
    int i;

    for (i = 0; i < WE_NUMBER_OF_ADDITIONAL_CHARACTER_SETS; i++) {
      if (we_xchr_table[i].character_set == charset) {
        s = we_xchr_table[i].preferred_name;
        break;
      }
    }
  }
#endif

  return s;
}

/*
 * Given a charset name as a string, return its integer value.
 * Returns -1 if the string does not have an integer representation.
 */
int
we_charset_str2int (const char *name, int length)
{
  int charset = we_cmmn_str2int_lc (name, length, &we_charsets);

#if WE_NUMBER_OF_ADDITIONAL_CHARACTER_SETS > 0
  if (charset < 0) {
    int i;

    for (i = 0; i < WE_NUMBER_OF_ADDITIONAL_CHARACTER_SETS; i++) {
      if (we_cmmn_strncmp_nc (we_xchr_table[i].preferred_name, name, length) == 0) {
        charset = we_xchr_table[i].character_set;
        goto done;
      }
      else if (we_xchr_table[i].aliases != NULL) {
        const char *s = we_xchr_table[i].aliases;
        const char *t;
        int   len;

        for (;;) {
          t = strchr (s, ',');
          if (t == NULL) {
            len = strlen (s);
          }
          else {
            len = t - s;
          }
          if ((len == length) && (we_cmmn_strncmp_nc (s, name, length) == 0)) {
            charset = we_xchr_table[i].character_set;
            goto done;
          }

          if (t == NULL) {
            break;
          }
          s = t + 1;
        }
      }
    }
  }
 done:
#endif

  return charset;
}

/*
 * Convert one Unicode character encoded as UTF-8 to its Unicode character code.
 * The character code is stored in the location pointed to by 'uc'.
 * Returns the number of bytes used from the UTF-8 string, or -1 on error.
 */
int
we_charset_utf8_to_unicode (const char *s, WE_UINT32 *uc)
{
  WE_UINT32 w;
  int        n;

  /* Read one Unicode character in UTF-8 format: */
  n = we_charset_utf8_bytes_per_character[(unsigned char)*s >> 4];

  w = 0;
  switch (n) {
  case 0:
    return -1;

  case 4: w += (unsigned char)*s++; w <<= 6;
  case 3: w += (unsigned char)*s++; w <<= 6;
  case 2: w += (unsigned char)*s++; w <<= 6;
  case 1: w += (unsigned char)*s++;
  }
  *uc = w - we_charset_offsets_from_utf8[n];

  return n;
}


/*
 * Convert one Unicode character to UTF-8. The encoded string is stored in 's'.
 * Returns the number of bytes stored, or -1 on error.
 */
int
we_charset_unicode_to_utf8 (WE_UINT32 uc, char *s)
{
  int           dn;
  unsigned char mask;

  /* Determine how many UTF8-bytes are needed for this character,
   * and which bits should be set in the first byte. */
  if (uc < 0x80) {
    dn = 1;
    mask = 0;
  }
  else if (uc < 0x800) {
    dn = 2;
    mask = 0xc0;
  }
  else if (uc < 0x10000) {
    dn = 3;
    mask = 0xe0;
  }
  else if (uc < 0x200000) {
    dn = 4;
    mask = 0xf0;
  }
  else {
    return -1;
  }

  switch (dn) {
  case 4:
    s[3] = (char)((uc & 0x3f) | 0x80);
    uc >>= 6;
  case 3:
    s[2] = (char)((uc & 0x3f) | 0x80);
    uc >>= 6;
  case 2:
    s[1] = (char)((uc & 0x3f) | 0x80);
    uc >>= 6;
  case 1:
    s[0] = (char)(uc | mask);
  }

  return dn;
}


/**********************************************************************
 * Character Encoding Conversion Functions:
 **********************************************************************/

int
we_charset_ascii_to_utf8 (const char *src, long *srcLen, char *dst, long *dstLen)
{
  char *p;
  int n = MIN (*srcLen, *dstLen);

  if (dst != NULL) {
    for (p = dst; p < dst + n; p++, src++) {
      if (((unsigned char)*src) < 0x80) {
        *p = *src;
      }
      else {
        *p = 0x3f; /* Non-ASCII characters become '?' */
      }
    }
  }
  *srcLen = n;
  *dstLen = n;

  return 0;
}

int
we_charset_latin1_to_utf8 (const char *src, long *srcLen, char *dst, long *dstLen)
{
  WE_UINT32 w;
  int        sidx = 0, didx = 0;
  int        dn;
  WE_UINT8  mask;

  for (;;) {
    /* First, translate to UCS4 */
    if (sidx + 1 > *srcLen)
      break;

    w = (WE_UINT32)((unsigned char)src[sidx]);

    /* Determine how many UTF8-bytes are needed for this character,
     * and which bits should be set in the first byte. */
    if (w < 0x80) {
      dn = 1;
      mask = 0;
    }
    else {
      dn = 2;
      mask = 0xc0;
    }

    /* Is there room for this in the destination vector? */
    if (didx + dn > *dstLen)
      break;

    /* Only write to destination vector if dst != NULL */
    if (dst != NULL) {
      switch (dn) {
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

static int
we_charset_utf16any_to_utf8 (const char *src, long *src_len,
                              char *dst, long *dst_len,
                              int big_endian)
{
  WE_UINT32 w, w2;
  int        sidx = 0, didx = 0;
  int        sn, dn;
  WE_UINT8  mask;
  int        i0, i1, i2, i3;

  if (big_endian) {
    i0 = 0; i1 = 1; i2 = 2; i3 = 3;
  }
  else {
    i0 = 1; i1 = 0; i2 = 3; i3 = 2;
  }

  for (;;) {
    /* First, translate to UCS4 */
    if (sidx + 2 > *src_len)
      break;
    w = ((unsigned char)src[sidx + i0] << 8) |
      (unsigned char)src[sidx + i1];
    sn = 2;

    if ((w >= WE_CHARSET_SURROGATE_HIGH_START) &&
        (w <= WE_CHARSET_SURROGATE_HIGH_END)) {
      if (sidx + 4 > *src_len)
        break;
      w2 = ((unsigned char)src[sidx + i2] << 8) |
        (unsigned char)src[sidx + i3];
      sn = 4;
      if ((w2 >= WE_CHARSET_SURROGATE_LOW_START) &&
          (w2 <= WE_CHARSET_SURROGATE_LOW_END)) {
        w = ((w - WE_CHARSET_SURROGATE_HIGH_START) << 10)
          + (w2 - WE_CHARSET_SURROGATE_LOW_START) + 0x10000UL;
      }
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
    else if (w < 0x10000) {
      dn = 3;
      mask = 0xe0;
    }
    else if (w < 0x200000) {
      dn = 4;
      mask = 0xf0;
    }
    else {
      dn = 2;
      w = WE_CHARSET_REPLACEMENT_CHAR;
      mask = 0xc0;
    }

    /* Is there room for this in the destination vector? */
    if (didx + dn > *dst_len)
      break;

    /* Only write to destination vector if dst != NULL */
    if (dst != NULL) {
      switch (dn) {
      case 4:
        dst[didx + 3] = (char)((w & 0x3f) | 0x80);
        w >>= 6;
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

    sidx += sn;
    didx += dn;
  }
  *src_len = sidx;
  *dst_len = didx;

  return 0;
}

int
we_charset_utf16be_to_utf8 (const char *src, long *srcLen, char *dst, long *dstLen)
{
  return we_charset_utf16any_to_utf8 (src, srcLen, dst, dstLen, TRUE);
}

int
we_charset_utf16le_to_utf8 (const char *src, long *srcLen, char *dst, long *dstLen)
{
  return we_charset_utf16any_to_utf8 (src, srcLen, dst, dstLen, FALSE);
}

int
we_charset_utf16_to_utf8 (const char *src, long *srcLen, char *dst, long *dstLen)
{
  /* For now, we just assume it is big endian. */
  return we_charset_utf16any_to_utf8 (src, srcLen, dst, dstLen, TRUE);
}

static int
we_charset_ucs2any_to_utf8 (const char *src, long *src_len,
                             char *dst, long *dst_len,
                             int big_endian)
{
  WE_UINT32 w;
  int        sidx = 0, didx = 0;
  int        sn, dn;
  WE_UINT8  mask;
  int        i0, i1;

  if (big_endian) {
    i0 = 0; i1 = 1;
  }
  else {
    i0 = 1; i1 = 0;
  }

  for (;;) {
    /* First, translate to UCS4 */
    if (sidx + 2 > *src_len)
      break;
    w = ((unsigned char)src[sidx + i0] << 8) |
      (unsigned char)src[sidx + i1];
    sn = 2;

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
    if (didx + dn > *dst_len)
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

    sidx += sn;
    didx += dn;
  }
  *src_len = sidx;
  *dst_len = didx;

  return 0;
}

int
we_charset_ucs2be_to_utf8 (const char *src, long *srcLen, char *dst, long *dstLen)
{
  return we_charset_ucs2any_to_utf8 (src, srcLen, dst, dstLen, TRUE);
}

int
we_charset_ucs2le_to_utf8 (const char *src, long *srcLen, char *dst, long *dstLen)
{
  return we_charset_ucs2any_to_utf8 (src, srcLen, dst, dstLen, FALSE);
}

int
we_charset_ucs2_to_utf8 (const char *src, long *srcLen, char *dst, long *dstLen)
{
  return we_charset_ucs2any_to_utf8 (src, srcLen, dst, dstLen, TRUE);
}


/*
 * NOTE: if the source string contains character codes
 * outside the range of ASCII, such characters will
 * be converted to the character code 0x3f, that is '?'.
 */
int
we_charset_utf8_to_ascii (const char *src, long *srcLen, char *dst, long *dstLen)
{
  int        sidx = 0, didx = 0;
  int        sn;
  WE_UINT32 w;

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

    /* Write one Unicode character in Latin-1 format: */
    if (w > 0x7f)
      w = 0x3f; /* Non-ASCII characters become '?' */

    if (dst != NULL) {
      dst[didx] = (char)(w & 0xff);
    }
    sidx += sn;
    didx += 1;
  }
  *srcLen = sidx;
  *dstLen = didx;

  return 0;
}

/*
 * NOTE: if the source string contains character codes
 * outside the range of Latin-1, such characters will
 * be converted to the character code 0x3f, that is '?'.
 */
int
we_charset_utf8_to_latin1 (const char *src, long *srcLen, char *dst, long *dstLen)
{
  int        sidx = 0, didx = 0;
  int        sn;
  WE_UINT32 w;

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

    /* Write one Unicode character in Latin-1 format: */
    if (w > 0xff)
      w = 0x3f; /* Non-Latin-1 characters become '?' */

    if (dst != NULL) {
      dst[didx] = (char)(w & 0xff);
    }
    sidx += sn;
    didx += 1;
  }
  *srcLen = sidx;
  *dstLen = didx;

  return 0;
}

static int
we_charset_utf8_to_utf16any (const char *src, long *src_len,
                              char *dst, long *dst_len,
                              int big_endian)
{
  int        sidx = 0, didx = 0;
  int        sn, dn;
  WE_UINT32 w, tmp;
  int        i0, i1, i2, i3;

  if (big_endian) {
    i0 = 0; i1 = 1; i2 = 2; i3 = 3;
  }
  else {
    i0 = 1; i1 = 0; i2 = 3; i3 = 2;
  }

  while (sidx < *src_len) {
    /* Read one Unicode character in UTF-8 format: */
    sn = we_charset_utf8_bytes_per_character[(unsigned char)*src >> 4];
    if (sidx + sn > *src_len)
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

    /* Write one Unicode character in UTF-16 format: */
    if (w < 0x10000) {
      if (didx + 2 > *dst_len)
        break;
      if (dst != NULL) {
        dst[didx + i0] = (char)((w >> 8) & 0xff);
        dst[didx + i1] = (char)((w & 0xff));
      }
      dn = 2;
    }
    else {
      if (didx + 4 > *dst_len)
        break;
      if (dst != NULL) {
        tmp = 0xd7c0 + (w >> 10);
        dst[didx + i0] = (char)((tmp >> 8) & 0xff);
        dst[didx + i1] = (char)((tmp & 0xff));
        dst[didx + i2] = (char)(0xdc | ((w & 0x3ff) >> 8));
        dst[didx + i3] = (char)(w & 0xff);
      }
      dn = 4;
    }
    sidx += sn;
    didx += dn;
  }
  *src_len = sidx;
  *dst_len = didx;

  return 0;
}

int
we_charset_utf8_to_utf16be (const char *src, long *srcLen, char *dst, long *dstLen)
{
  return we_charset_utf8_to_utf16any (src, srcLen, dst, dstLen, TRUE);
}

int
we_charset_utf8_to_utf16le (const char *src, long *srcLen, char *dst, long *dstLen)
{
  return we_charset_utf8_to_utf16any (src, srcLen, dst, dstLen, FALSE);
}

int
we_charset_utf8_to_utf16 (const char *src, long *srcLen, char *dst, long *dstLen)
{
  /* For now, just assume that it is big endian. */
  return we_charset_utf8_to_utf16any (src, srcLen, dst, dstLen, TRUE);
}

static int
we_charset_utf8_to_ucs2any (const char *src, long *src_len,
                              char *dst, long *dst_len,
                              int big_endian)
{
  int        sidx = 0, didx = 0;
  int        sn, dn;
  WE_UINT32 w;
  int        i0, i1;

  if (big_endian) {
    i0 = 0; i1 = 1;
  }
  else {
    i0 = 1; i1 = 0;
  }

  while (sidx < *src_len) {
    /* Read one Unicode character in UTF-8 format: */
    sn = we_charset_utf8_bytes_per_character[(unsigned char)*src >> 4];
    if (sidx + sn > *src_len)
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

    /* Write one Unicode character in UCS-2 format: */
    w &= 0xffff;
    if (didx + 2 > *dst_len)
      break;
    if (dst != NULL) {
      dst[didx + i0] = (char)((w >> 8) & 0xff);
      dst[didx + i1] = (char)((w & 0xff));
    }
    dn = 2;
    sidx += sn;
    didx += dn;
  }
  *src_len = sidx;
  *dst_len = didx;

  return 0;
}

int
we_charset_utf8_to_ucs2be (const char *src, long *srcLen, char *dst, long *dstLen)
{
  return we_charset_utf8_to_ucs2any (src, srcLen, dst, dstLen, TRUE);
}

int
we_charset_utf8_to_ucs2le (const char *src, long *srcLen, char *dst, long *dstLen)
{
  return we_charset_utf8_to_ucs2any (src, srcLen, dst, dstLen, FALSE);
}

int
we_charset_utf8_to_ucs2 (const char *src, long *srcLen, char *dst, long *dstLen)
{
  return we_charset_utf8_to_ucs2any (src, srcLen, dst, dstLen, TRUE);
}


static int
we_charset_ascii_to_utf16any (const char *src, long *src_len,
                               char *dst, long *dst_len, int big_endian)
{
  int        sidx = 0, didx = 0;
  WE_UINT32 w;
  int        i0, i1;

  if (big_endian) {
    i0 = 0; i1 = 1;
  }
  else {
    i0 = 1; i1 = 0;
  }

  while (sidx < *src_len) {
    /* Read one Unicode character in ASCII: */
    if (sidx + 1 > *src_len)
      break;

    w = (unsigned char)*src++;

    /* Write one Unicode character in UTF-16 format: */
    if (didx + 2 > *dst_len)
      break;

    if (dst != NULL) {
      dst[didx + i0] = 0;
      dst[didx + i1] = (char)((w & 0xff));
    }

    sidx += 1;
    didx += 2;
  }
  *src_len = sidx;
  *dst_len = didx;

  return 0;
}

int
we_charset_ascii_to_utf16be (const char *src, long *srcLen, char *dst, long *dstLen)
{
  return we_charset_ascii_to_utf16any (src, srcLen, dst, dstLen, TRUE);
}

int
we_charset_ascii_to_utf16le (const char *src, long *srcLen, char *dst, long *dstLen)
{
  return we_charset_ascii_to_utf16any (src, srcLen, dst, dstLen, FALSE);
}


static int
we_charset_latin1_to_utf16any (const char *src, long *src_len,
                                char *dst, long *dst_len, int big_endian)
{
  int        sidx = 0, didx = 0;
  WE_UINT32 w;
  int        i0, i1;

  if (big_endian) {
    i0 = 0; i1 = 1;
  }
  else {
    i0 = 1; i1 = 0;
  }

  while (sidx < *src_len) {
    /* Read one Unicode character in Latin-1: */
    if (sidx + 1 > *src_len)
      break;

    w = (unsigned char)*src++;

    /* Write one Unicode character in UTF-16 format: */
    if (didx + 2 > *dst_len)
      break;

    if (dst != NULL) {
      dst[didx + i0] = 0;
      dst[didx + i1] = (char)((w & 0xff));
    }

    sidx += 1;
    didx += 2;
  }
  *src_len = sidx;
  *dst_len = didx;

  return 0;
}

int
we_charset_latin1_to_utf16be (const char *src, long *srcLen, char *dst, long *dstLen)
{
  return we_charset_latin1_to_utf16any (src, srcLen, dst, dstLen, TRUE);
}

int
we_charset_latin1_to_utf16le (const char *src, long *srcLen, char *dst, long *dstLen)
{
  return we_charset_latin1_to_utf16any (src, srcLen, dst, dstLen, FALSE);
}


int
we_charset_utf16be_to_utf16le (const char *src, long *srcLen, char *dst, long *dstLen)
{
  int n = MIN (*srcLen, *dstLen);
  int i;

  i = 0;
  while (i + 1 < n) {
    if (dst != NULL) {
      dst[i] = src[i + 1];
      dst[i + 1] = src[i];
    }
    i += 2;
  }
  *srcLen = i;
  *dstLen = i;

  return 0;
}

/*
 * Filters out all incorrect utf-8 sequences of an utf-8 stream.
 */
int 
we_charset_utf8_to_utf8 (const char *src, long *src_len,
                                 char *dst, long *dst_len)
{
  long    sidx = 0, didx = 0;
  int    sn;
  int    char_ok = FALSE;
  WE_UINT32 w = 0;
 
  while (sidx < *src_len) {
    /* Read one Unicode character in UTF-8 format: */
    sn = we_charset_utf8_bytes_per_character[(unsigned char)*src >> 4];
  
    if (sidx + sn > *src_len || didx + sn > *dst_len || (sn == 0 && didx + 3 >= *dst_len))
      break;

    /* Convert to UCS2 */
    if(sn > 1){
      w = 0;
      switch (sn) {
      case 4: w += (unsigned char)*src++; w <<= 6;
      case 3: w += (unsigned char)*src++; w <<= 6;
      case 2: w += (unsigned char)*src++; w <<= 6;
      case 1: w += (unsigned char)*src++;
      }
      w = w - we_charset_offsets_from_utf8[sn];
      src -= sn;
    }
    
    /* Validate all bytes and check for overlong characters 
    
    To make the code below understandable. UTF-8 is build up like this:

    1 byte:  0xxxxxxx  
    2 bytes: 110xxxxx 10xxxxxx  
    3 bytes: 1110xxxx 10xxxxxx 10xxxxxx  
    4 bytes: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  
        
    */
    switch(sn){
    case 0:
      char_ok = FALSE;
      break;
    case 1:
      char_ok = TRUE;
      break;
    case 2:
      char_ok = w >= 0x80UL && (*(src+1) & 0xc0) == 0x80;
      break;
    case 3:
      char_ok = w >= 0x800UL && ((*(src+1) & 0xc0) == 0x80) && ((*(src+2) & 0xc0) == 0x80);
      break;
    case 4:
      char_ok = w >= 0x10000UL && !(*src & 0x08) && ((*(src+1) & 0xc0) == 0x80) && ((*(src+2) & 0xc0) == 0x80) && ((*(src+3) & 0xc0) == 0x80);
      break;
    }


    if(char_ok){ /* Valid character. Copy the bytes */
      if (dst != NULL){
        switch(sn){
        case 4: *dst++ = *src++; 
        case 3: *dst++ = *src++;
        case 2: *dst++ = *src++;
        case 1: *dst++ = *src++;
        }
      }
      else{
        src += sn;
      }
      sidx += sn;
      didx += sn;
    }
    else{ /* Invalid character. Replace with WE_CHARSET_REPLACEMENT_CHAR */ 
      if(didx + 3 < *dst_len){
        if (dst != NULL){
          int len = we_charset_unicode_to_utf8(WE_CHARSET_REPLACEMENT_CHAR,dst);
          if(len>0){
            dst += len;
            didx += len;
          }

        }
      }
      src++;
      sidx++;
      /* If this byte was a valid start byte but the character was invalid scan for the next start byte. */
      if(sn != 0){
        while(we_charset_utf8_bytes_per_character[(unsigned char)*src >> 4] == 0 && sidx < *src_len){
          src++;
          sidx++;
        }
      }
    }
  }
  *src_len = sidx;
  *dst_len = didx;

  return 0;
}
