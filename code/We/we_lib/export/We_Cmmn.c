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
 * We_Cmmn.c
 *
 * Created by Anders Edenwbandt, Fri Mar 16 13:05:56 2001.
 *
 * Common utilities.
 *
 * Revision history:
 *   010509, AED: New functions, cmmn_skip_blanks and cmmn_skip_field.
 *   010629, AED: General reorganization and cleanup.
 *                Functions for string lookup.
 *   010806, NKE: Added const to some parameters.
 *   010806, NKE: Updated cmmn_byte2hex to fit the header file description.
 *   010816, AED: New function, cmmn_hex2int.
 *   010829, AED: Change in cmmn_skip_field.
 *                New function, cmmn_skip_trailing_blanks
 *   010907, NKE: New function: cmmn_hex2long.
 *   010926, AED: Deleted WCHAR-routines, not needed anymore.
 *                Moved all memory-allocation routines to memalloc.c
 *   011003, AED: New function, cmmn_strcat.
 *   011025, AED: Cleanup
 *   011129, AED: New functions, cmmn_strcmp_nc, and cmmn_strncmp_nc
 *   011206, AED: New functions for UTF-8 string handling.
 *   020402, IPN: Changed to fit Mobile Suite Framework.
 *   020912, AED: New functions to convert date-time strings.
 *   050428, MAAR: ICR17556. we_cmmn_time2str() now return number of bytes.
 *
 */
#include "We_Def.h"
#include "We_Lib.h"
#include "We_Cmmn.h"
#include "We_Mem.h"
#include "We_Chrt.h"
#include "We_Brk.h"

/**********************************************************************
 * Basic string-handling functions
 **********************************************************************/

/*
 * Return a copy of the null-terminated string 's'.
 * If s == NULL, this function returns NULL.
 * NOTE: it is the caller's responsibility to deallocate the
 * returned string.
 */
char *
we_cmmn_strdup (WE_UINT8 modId, const char *s)
{
  char *s_new = NULL;

  if (s != NULL) {
    s_new = (char*)WE_MEM_ALLOC (modId, strlen (s) + 1);
    if (s_new != NULL)
      strcpy (s_new, s);
  }

  return s_new;
}

/*
 * Return a copy of the string 's' of length at most 'len' characters.
 * Copying stops if a null-byte is found.
 * NOTE: if s == NULL, this function returns a pointer
 * to a null-terminated string of length 0. THIS MAY CHANGE IN THE FUTURE!
 * NOTE: it is the caller's responsibility to deallocate the
 * returned string.
 */
char *
we_cmmn_strndup (WE_UINT8 modId, const char *s, int len)
{
  char *s_new;

  if (s != NULL) {
    s_new = (char*)WE_MEM_ALLOC (modId, len + 1);
    if (s_new != NULL) {
      strncpy (s_new, s, len);
      s_new[len] = '\0';
    }
  }
  else {
    s_new = (char*)WE_MEM_ALLOC (modId, 1);
    if (s_new != NULL) {
      s_new[0] = '\0';
    }
  }

  return s_new;
}

/*
 * Create and return a new string which holds the concatenation
 * of strings 's1' and 's2'.
 * NOTE: it is the caller's responsibility to deallocate the
 * returned string.
 */
char *
we_cmmn_strcat (WE_UINT8 modId, const char *s1, const char *s2)
{
  char *s_new = NULL;
  int   len1 = (s1 != NULL ? strlen (s1) : 0);
  int   len2 = (s2 != NULL ? strlen (s2) : 0);
  
  if ((s1 != NULL) || (s2 != NULL)) {
    s_new = (char*)WE_MEM_ALLOC (modId, len1 + len2 + 1);
    if (s1 != NULL)
      strcpy (s_new, s1);
    if (s2 != NULL)
      strcpy (s_new + len1, s2);
  }

  return s_new;
}

/*
 * Create and return a new string which holds the concatenation
 * of strings 's1' and len2 bytes of 's2'.
 * NOTE: it is the caller's responsibility to deallocate the
 * returned string.
 */
char *
we_cmmn_strncat (WE_UINT8 modId, const char *s1, const char *s2, int len2)
{
  char *s_new = NULL;
  int   len1 = (s1 != NULL ? strlen (s1) : 0);
  
  if ((s1 != NULL) || (s2 != NULL)) {
    s_new = (char*)WE_MEM_ALLOC (modId, len1 + len2 + 1);
    if (s1 != NULL)
      strcpy (s_new, s1);
    if (s2 != NULL)
      strncpy (s_new + len1, s2, len2);
    s_new[len1+len2] = '\0';
  }

  return s_new;
}
/*
 * Create and return a new string which holds the concatenation
 * of three strings 's1', 's2' and 's3'.
 * NOTE: it is the caller's responsibility to deallocate the
 * returned string.
 */
char *
we_cmmn_str3cat (WE_UINT8    modId,
                  const char  *s1,
                  const char  *s2,
                  const char  *s3)
{
  char *s_new = NULL;
  int   len1 = (s1 != NULL ? strlen (s1) : 0);
  int   len2 = (s2 != NULL ? strlen (s2) : 0);
  int   len3 = (s3 != NULL ? strlen (s3) : 0);
  
  if ((s1 != NULL) || (s2 != NULL) || (s3 != NULL)) {
    s_new = (char*)WE_MEM_ALLOC (modId, len1 + len2 +len3 + 1);
    if (s1 != NULL)
      strcpy (s_new, s1);
    if (s2 != NULL)
      strcpy (s_new + len1, s2);
    if (s3 != NULL)
      strcpy (s_new + len1 + len2, s3);
  }

  return s_new;
}

/*
 * Return a pointer to the first occurrence of the character 'c'
 * in the string 's'. The search stops at a null-byte or after
 * at most 'len' characters.
 */
char *
we_cmmn_strnchr (const char *s, char c, int len)
{
  int i;

  if (s == NULL)
    return NULL;

  for (i = 0; (i < len) && (s[i] != '\0'); i++) {
    if (s[i] == c)
      return (char *)(s + i);
  }

  return NULL;
}

/* Array of all ASCII characters, lower case only.
 * Used by the routines below that transform a string or character
 * into lower case. */
const unsigned char we_ascii_lc[256] = {
  0, 1, 2, 3, 4, 5, 6, 7,
  8, 9, 10, 11, 12, 13, 14, 15,
  16, 17, 18, 19, 20, 21, 22, 23,
  24, 25, 26, 27, 28, 29, 30, 31,
  ' ', '!', '"', '#', '$', '%', '&', '\'',
  '(', ')', '*', '+', ',', '-', '.', '/',
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', ':', ';', '<', '=', '>', '?',
  '@', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
  'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
  'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
  'x', 'y', 'z', '[', '\\', ']', '^', '_',
  '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
  'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
  'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
  'x', 'y', 'z', '{', '|', '}', '~', 127,
  128, 129, 130, 131, 132, 133, 134, 135,
  136, 137, 138, 139, 140, 141, 142, 143,
  144, 145, 146, 147, 148, 149, 150, 151,
  152, 153, 154, 155, 156, 157, 158, 159,
  160, 161, 162, 163, 164, 165, 166, 167,
  168, 169, 170, 171, 172, 173, 174, 175,
  176, 177, 178, 179, 180, 181, 182, 183,
  184, 185, 186, 187, 188, 189, 190, 191,
  192, 193, 194, 195, 196, 197, 198, 199,
  200, 201, 202, 203, 204, 205, 206, 207,
  208, 209, 210, 211, 212, 213, 214, 215,
  216, 217, 218, 219, 220, 221, 222, 223,
  224, 225, 226, 227, 228, 229, 230, 231,
  232, 233, 234, 235, 236, 237, 238, 239,
  240, 241, 242, 243, 244, 245, 246, 247,
  248, 249, 250, 251, 252, 253, 254, 255};

/* Array of all ASCII characters, upper case only.
 * Used for transforming a string or character
 * into upper case. */
const unsigned char we_ascii_uc[256] =
{
  0, 1, 2, 3, 4, 5, 6, 7,
  8, 9, 10, 11, 12, 13, 14, 15,
  16, 17, 18, 19, 20, 21, 22, 23,
  24, 25, 26, 27, 28, 29, 30, 31,
  ' ', '!', '"', '#', '$', '%', '&', '\'',
  '(', ')', '*', '+', ',', '-', '.', '/',
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', ':', ';', '<', '=', '>', '?',
  '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
  'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
  'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
  'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
  '`', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
  'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
  'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
  'X', 'Y', 'Z', '{', '|', '}', '~', 127,
  128, 129, 130, 131, 132, 133, 134, 135,
  136, 137, 138, 139, 140, 141, 142, 143,
  144, 145, 146, 147, 148, 149, 150, 151,
  152, 153, 154, 155, 156, 157, 158, 159,
  160, 161, 162, 163, 164, 165, 166, 167,
  168, 169, 170, 171, 172, 173, 174, 175,
  176, 177, 178, 179, 180, 181, 182, 183,
  184, 185, 186, 187, 188, 189, 190, 191,
  192, 193, 194, 195, 196, 197, 198, 199,
  200, 201, 202, 203, 204, 205, 206, 207,
  208, 209, 210, 211, 212, 213, 214, 215,
  216, 217, 218, 219, 220, 221, 222, 223,
  224, 225, 226, 227, 228, 229, 230, 231,
  232, 233, 234, 235, 236, 237, 238, 239,
  240, 241, 242, 243, 244, 245, 246, 247,
  248, 249, 250, 251, 252, 253, 254, 255};

/*
 * Copy the null-terminated string 'src' to 'dst',
 * while changing all uppercase letters to lower case.
 * Non-ASCII characters are not affected.
 */
void
we_cmmn_strcpy_lc (char *dst, const char *src)
{
  while (*src) {
    *dst++ = (char)we_ascii_lc[(unsigned char)(*src++)];
  }
  *dst = '\0';
}

/*
 * Copy at most 'len' characters from the string 'src' to 'dst',
 * while changing all uppercase letters to lower case.
 * Non-ASCII characters are not affected.
 */
void
we_cmmn_strncpy_lc (char *dst, const char *src, int len)
{
  int i;

  for (i = 0; i < len; i++) {
    dst[i] = (char)we_ascii_lc[(unsigned char)src[i]];
    if (src[i] == '\0')
      break;
  }
}

/*
 * Return the lower-case version of the character 'c'.
 * Non-ASCII characters are not affected.
 */
char
we_cmmn_chrlc (char c)
{
  return we_ascii_lc[(unsigned char)c];
}

/*
 * Perform a string comparison just like strcmp,
 * but without regard to case.
 * NOTE: only upper and lower case versions
 * of ASCII characters are treated as equal!
 */
int
we_cmmn_strcmp_nc (const char *s1, const char *s2)
{
  for (; we_ascii_lc[(unsigned char)*s1] == we_ascii_lc[(unsigned char)*s2];
       s1++, s2++) {
    if (*s1 == '\0')
      return 0;
  }
  return (we_ascii_lc[(unsigned char)*s1] - we_ascii_lc[(unsigned char)*s2]);
}

/*
 * Perform a string comparison just like strncmp,
 * but without regard to case.
 * NOTE: only upper and lower case versions
 * of ASCII characters are treated as equal!
 */
int
we_cmmn_strncmp_nc (const char *s1, const char *s2, int len)
{
  if (len <= 0)
    return 0;

  for (; we_ascii_lc[(unsigned char)*s1] == we_ascii_lc[(unsigned char)*s2];
       s1++, s2++) {
    if ((--len <= 0) || (*s1 == '\0'))
      return 0;
  }
  return (we_ascii_lc[(unsigned char)*s1] - we_ascii_lc[(unsigned char)*s2]);
}

/* Array used by cmmn_byte2hex below. */
static const char hex_digits[16] = {
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

/*
 * Write a textual representation of a byte as two letters "XY",
 * being the hexadecimal digits that represent the value of the byte.
 * The string is written to the location indicated by 's'.
 */
void
we_cmmn_byte2hex (unsigned char b, char *s)
{
  *s++ = hex_digits[(b >> 4) & 0xF];
  *s = hex_digits[b & 0xF];
}

/*
 * Read a string representation, consisting of two hexadecimal
 * digits, and store the byte value it represents in the location
 * indicated by 'b'.
 * Returns FALSE if the string 's' does not start with two
 * hexadecimal digits, and TRUE otherwise.
 */
int
we_cmmn_hex2byte (const char *s, unsigned char *b)
{
  char          c;
  unsigned char val;

  if (s == NULL)
    return FALSE;

  c = we_ascii_lc[(unsigned char)s[0]];
  if (ct_isdigit (c))
    val = (unsigned char)(c - '0');
  else if (ct_ishexletter (c))
    val = (unsigned char)(c - 'a' + 10);
  else
    return FALSE;

  c = we_ascii_lc[(unsigned char)s[1]];
  if (ct_isdigit (c))
    val = (unsigned char)((val << 4) + c - '0');
  else if (ct_ishexletter (c))
    val = (unsigned char)((val << 4) + c - 'a' + 10);
  else
    return FALSE;

  *b = val;

  return TRUE;
}

/*
 * Read a null-terminated string consisting of hexadecimal digits,
 * and return the integer value it represents.
 */
int
we_cmmn_hex2int (const char *s)
{
  int  n = 0;
  char c;

  while (ct_ishex (c = *s++)) {
    if (ct_isdigit (c))
      n = (n << 4) + (c - '0');
    else if (ct_islower (c))
      n = (n << 4) + (c - 'a' + 10);
    else
      n = (n << 4) + (c - 'A' + 10);
  }

  return n;
}


/*
 * Read a null-terminated string consisting of hexadecimal digits,
 * and return the integer value it represents.
 */
long
we_cmmn_hex2long (const char *s)
{
  long n = 0;
  char c;

  while (ct_ishex (c = *s++)) {
    if (ct_isdigit (c))
      n = (n << 4) + (c - '0');
    else if (ct_islower (c))
      n = (n << 4) + (c - 'a' + 10);
    else
      n = (n << 4) + (c - 'A' + 10);
  }

  return n;
}

static const unsigned char we_cmmn_utf8_bytes_per_character[16] = {
  1, 1, 1, 1, 1, 1, 1, 1,
  0, 0, 0, 0, 2, 2, 3, 4};

/*
 * Given a UTF-8 encoded null-terminated string,
 * return the number of Unicode characters in the string.
 */
int
we_cmmn_utf8_strlen (const char *s)
{
  int count = 0;
  int d;
  
  if (s == NULL)
    return 0;

  while (*s != '\0') {
    d = we_cmmn_utf8_bytes_per_character[(unsigned char)*s >> 4];
    /* d==0 msans that it is an errouneus character.*/
    if (d == 0)
      return count;
    s += d;
    count++;
  }

  return count;
}

/*
 * Given a UTF-8 encoded string of length 'len' bytes,
 * return the number of Unicode characters in the string.
 */
int
we_cmmn_utf8_strnlen (const char *s, int len)
{
  int count = 0;
  int d;

  if (s == NULL)
    return 0;

  while ((len > 0) && (*s != '\0')) {
    d = we_cmmn_utf8_bytes_per_character[(unsigned char)*s >> 4];
    /* d==0 msans that it is an errouneus character.*/
    if (d == 0)
      return count;
    s += d;
    len -= d;
    count++;
  }

  return count;
}

/*
 * Given a UTF-8 encoded string,
 * return the number of bytes in the first Unicode character.
 */
int
we_cmmn_utf8_charlen (const char *s)
{
  if (*s == '\0')
    return 0;
  else
    return we_cmmn_utf8_bytes_per_character[(unsigned char)*s >> 4];
}

/*
 * Given a UTF-8 encoded null-terminated string,
 * step past 'steps' Unicode characters and return
 * a pointer to the new position.
 */
char *
we_cmmn_utf8_str_incr (const char *s, int steps)
{
  while ((steps-- > 0) && (*s != '\0')) {
    s += we_cmmn_utf8_bytes_per_character[(unsigned char)*s >> 4];
  }

  return (char *)s;
}


/**********************************************************************
 * Other string functions
 **********************************************************************/

/*
 * Return a pointer to the first non-blank character
 * in the string 's'. A blank character is either SP or TAB.
 */
char *
we_cmmn_skip_blanks (const char *s)
{
  while ((*s == ' ') || (*s == '\t')) {
    s++;
  }

  return (char *)s;
}

/*
 * Return a pointer to the first white space character
 * in the string 's', or the NULL character terminating the string.
 * A white space character is either SP, TAB, CR, or LF.
 */
char *
we_cmmn_skip_field (const char *s)
{
  while (*s && !ct_iswhitespace (*s)) {
    s++;
  }

  return (char *)s;
}

/*
 * Given a string 's' and its length, return the length
 * of that string if trailing white space were omitted.
 * A white space character is either SP, TAB, CR, or LF.
 */
int
we_cmmn_skip_trailing_blanks (const char *s, int len)
{
  len--;
  while ((len > 0) && ct_iswhitespace (s[len])) {
    len--;
  }

  return len + 1;
}

/* The "alphabet" of base-64 encoding: */
static const unsigned char b64[65] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
 * Each input character in a base64 encoding represents 6 bits.
 * This function translates the character into its 6-bit value.
 * Returns -1 if the given input is not a legal base64 character.
 */
static int
we_cmmn_base64_decode_char (unsigned char b)
{
  if (b & 0x80)
    return -1;

  if (ct_isupper (b)) {
    return b - 'A';
  }
  else if (ct_islower (b)) {
    return b + 26 - 'a';
  }
  else if (ct_isdigit (b)) {
    return b + 52 - '0';
  }
  else if (b == '+') {
    return 62;
  }
  else if (b == '/') {
    return 63;
  }
  else {
    return -1;
  }
}

/*
 * Return the number of characters needed to decode
 * the string 'str' of length 'len'.
 */
int
we_cmmn_base64_decode_len (const char *str, int len)
{
  int new_len = (len / 4) * 3;

  if ((len > 0) && (str[len - 1] == '='))
    new_len--;
  if ((len > 1) && (str[len - 2] == '='))
    new_len--;

  return new_len;
}

/*
 * Given a string of base64-encoded data, decode it and
 * place it in the output buffer.
 * On input, 'outlen' should hold the maximum size of the output buffer,
 * on output, it will hold the number of bytes placed in the output buffer .
 *
 * Returns 0 if OK, and -1 on error.
 */
int
we_cmmn_base64_decode (const char *inbuf, int inlen, char *outbuf, int *outlen)
{
  char  b;
  int   i, j;
  int   cb;
  int   tmp = 0;
  int   saved_bits = 0;  /* The number of bits that have been
                          * saved in "tmp", to be used for the
                          * next output character. */

  j = 0;
  for (i = 0; i < inlen; i++) { /* For each character in the input */
    b = inbuf[i];
    if (b == '=') {
      /* Padding, we are done. */
      break;
    }
    if ((b == ' ') || (b == '\t') || (b == '\r') || (b == '\n')) {
      /* Ignore white space */
      continue;
    }

    /* Convert back from base64 encoding */
    cb = we_cmmn_base64_decode_char (b);
    if (cb < 0) { /* Illegal character */
      return -1;
    }
    if (saved_bits > 0) {
      if (j >= *outlen) {
        *outlen = j;
        return -1;
      }
      outbuf[j++] = (char)(tmp | (cb >> (saved_bits - 2)));
    }
    saved_bits = (saved_bits == 0) ? 6 : saved_bits - 2;
    if (saved_bits > 0) {
      tmp = (cb << (8 - saved_bits)) & 0xff;
    }
  }

  *outlen = j;
  return 0;
}

/*
 * Return the number of characters needed to encode
 * a string of length 'len'.
 */
int
we_cmmn_base64_encode_len (int len)
{
  return ((len + 2) / 3) * 4;
}

/*
 * Given a string of data, create a base-64 encoding
 * of said data, and place it in the output buffer.
 * On input, 'outlen' should hold the maximum size of the output buffer,
 * on output, it will hold the length of the string written there.
 * The encoded string will be null-terminated.
 *
 * Returns 0 if OK, and -1 on error.
 */
int
we_cmmn_base64_encode (const char *inbuf, int inlen, char *outbuf, int *outlen)
{
  int            len = ((inlen + 2) / 3) * 4;
  unsigned char *p;
  int            i, j;

  if (*outlen < len + 1) {
    *outlen = 0;
    return -1;
  }
  *outlen = len;

  for (i = 0, j = 0, p = (unsigned char *)outbuf;
       i + 2 < inlen; i += 3, p += 4, j += 4) {
    p[0] = b64[(inbuf[i] >> 2) & 0x3f];
    p[1] = b64[((inbuf[i] << 4) & 0x30) | ((inbuf[i + 1] >> 4) & 0x0f)];
    p[2] = b64[((inbuf[i + 1] << 2) & 0x3c) | ((inbuf[i + 2] >> 6) & 0x03)];
    p[3] = b64[inbuf[i + 2] & 0x3f];
  }

  if (i < inlen) {
    p[0] = b64[(inbuf[i] >> 2) & 0x3f];
    if (i + 1 < inlen) {
      p[1] = b64[((inbuf[i] << 4) & 0x30) | ((inbuf[i + 1] >> 4) & 0x0f)];
      p[2] = b64[((inbuf[i + 1] << 2) & 0x3c)];
    }
    else {
      p[1] = b64[((inbuf[i] << 4) & 0x30)];
      p[2] = '=';
    }
    p[3] = '=';
  }
  outbuf[len] = '\0';

  return 0;
}

/*
 * Finds the next appropriate line break for a string represented both as utf8
 * and as an WeString. The string is analysed based on the character data in the
 * string, and the result will normally fit into the given width. The result is
 * described by the index of the starting character as well as the index of
 * the ending character (not the byte/size of), along with some more detailed 
 * information as described in the definition of we_cmmn_line_break_t.
 *
 * Returns TRUE when successful, and FALSE for illegal parameters or character data.
 *
 * utf8_at_idx,         pointer to the utf8 data at the given start index. This is
 *                      typically the beginning of the string the first time, and
 *                      brk.continue_at the following calls.
 * string,              handle of the WeString that contains all the utf8 data.
 * brush,               handle of the brush that specifies the format of the string 
 *                      when rendered onscreen.
 * width,               width in pixels where the next break should be fitted into.
 * start_idx,           index of the character to start from (typically 0 first time and 
 *                      end_idx + 1 in the next call)
 * include_initial_ws,  whether the starting whitespace (U+0020) should be included in
 *                      the chunk of text that becomes the break and the width calculated.
 * brk                  the result.
 */

int
we_cmmn_next_line_break(const char* utf8_at_idx, 
                         WeStringHandle string,
                         WeBrushHandle brush,
                         WE_INT16 width,
                         int start_idx,
                         int include_initial_ws,
                         we_cmmn_line_break_t* brk)
{
  brk->fits = TRUE;
  brk->no_end_ws = 0;
  brk->width = 0;
  brk->start_idx;
  brk->end_idx = -1;
  brk->no_start_ws = 0;
  brk->first_is_glue = brk->last_is_glue = FALSE;
  brk->continue_at = NULL;

  if (width < 0 || !utf8_at_idx || !*utf8_at_idx || !string) {
    brk->fits = FALSE;
    return FALSE;
  }
  else {
    const char* p = utf8_at_idx;
    int i = 0;
    int bytes;
    const char* n;
    int ending_space = 0;
    int b = 0;
    int only_space = include_initial_ws ? TRUE : FALSE;

  /*?? is the below correct or should we drop WS before checking for glue?*/
    /* The glue check is performed before any space skipping.*/
    if (WE_BREAK_IS_GLUE(p)) {
      brk->first_is_glue = TRUE;      
    } 

    /* Bypass and count initial WS as needed.*/
      
    while (*p && *p == ' ') {
      if (!include_initial_ws)
        brk->no_start_ws++;
      else 
        i++;

      bytes = we_cmmn_utf8_bytes_per_character[(unsigned char)*p >> 4];
      /* Illegal character found.*/
      if (!bytes)
        return FALSE;
      p += bytes;
    }

    brk->start_idx = start_idx + brk->no_start_ws;

    if (!include_initial_ws) {
      i = brk->start_idx;
    }
    else {
      i += brk->start_idx;
    }

    /* Did we step til end of string when bypassing initial white space?*/
    if (!*p) {
      brk->end_idx = i;
      /* The below ensures that the insignificiant space that both start and end index
       * refers to is ingored in calculations by the caller.*/
      brk->no_end_ws = 1;
      return TRUE;
    }

    /* If the first char that should be calculated with is an NLF, the break should 
     * occur on it, by making the NLF character(s) ending white space.*/
    if (WE_BREAK_IS_NLF(p, b)) {
      /* b captures the additional character for *p in case of the CRLF NLF.*/
      brk->end_idx = i + b;
      brk->no_end_ws = 1 + b;
      brk->continue_at = *(p + 1 + b) ? p + 1 + b : NULL;
      return TRUE;
    }

    bytes = we_cmmn_utf8_bytes_per_character[(unsigned char)*p >> 4];
    if (!bytes)
      return FALSE;
    n = p + bytes;

    /* Find the next break opportunity until we bypass the allowable width or find
     * a hard break.*/
    while (*n) {
      /* b captures the additional character for *n in case of the CRLF NLF.*/
      int b = 0;
      int brk_type = WE_BREAK_TYPE(p, n, b);
      WE_INT16 w = 0;

      if (*p == ' ' && !only_space)
        ending_space++;
      else {
        ending_space = 0;
        only_space = FALSE;
      }

      if (brk_type == WE_BREAK_HARD) {
        int no_chars = i - brk->start_idx - ending_space + 1;

        if (no_chars)
          TPIa_widgetStringGetWidth(string, brush, brk->start_idx, no_chars, &w);

        if (w > width && brk->end_idx == -1) {
          /* Always grab at least the first break op, even if it does not fit into the width.*/
          brk->fits = FALSE;
        }

        if (w <= width || brk->end_idx == -1) {
          brk->width = w;
          brk->end_idx = i + b + 1;
          /* Add hard break character to ending space.*/
          brk->no_end_ws = ending_space + b + 1;
          brk->continue_at = *(n + b + 1) ? n + b + 1 : NULL;
        }

        /* Always stop at the first hard break.*/
        break;
      }
      else if (brk_type == WE_BREAK_OPPOR) {
        int no_chars = i - brk->start_idx - ending_space + 1;

        if (no_chars)
          TPIa_widgetStringGetWidth(string, brush, brk->start_idx, no_chars, &w);

        if (w > width && brk->end_idx == -1) {
          /* Always grab at least the first break op, even if it does not fit into the width.*/
          brk->fits = FALSE;
        }

        if (w <= width || brk->end_idx == -1) {
          brk->end_idx = i;
          brk->no_end_ws = ending_space;
          brk->width = w;
          brk->continue_at = *n ? n : NULL;
        }

        if (w > width)
          break;
      }

      /* Step to next character pair.*/
      p = n;
      bytes = we_cmmn_utf8_bytes_per_character[(unsigned char)*n >> 4];
      if (!bytes)
        return FALSE;
      n += bytes;
      i++;
    }

    /* If no break was chosen, create one which includes the full string if the full
     * string fits or no previous break opportunities were found.*/
    if (!*n) {

      /* Check for the last space*/
      if (*p == ' ' && !only_space)
        ending_space++;
      else
        ending_space = 0;

      if (brk->end_idx != -1) {
        /* There was a previous opportunity, check whether the full string fits.*/
        WE_INT16 w = 0;
        int no_chars = i - brk->start_idx - ending_space + 1;

        if (no_chars)
          TPIa_widgetStringGetWidth(string, brush, brk->start_idx, no_chars, &w);

        if (w <= width) {
          brk->end_idx = i;
          brk->no_end_ws = ending_space;
          brk->width = w;
          brk->continue_at = NULL;
        }
        else {
          /* Do not check for last is glue if not at end of string.*/
          return TRUE;
        }
      }
      else {
        int no_chars = i - brk->start_idx - ending_space + 1;

        if (no_chars)
          TPIa_widgetStringGetWidth(string, brush, brk->start_idx, no_chars, &brk->width);

        brk->end_idx = i;
        brk->no_end_ws = ending_space;
        brk->continue_at = NULL;
        brk->fits = brk->width <= width;
      }

      if (WE_BREAK_IS_GLUE(p)) {
        brk->last_is_glue = TRUE;      
      }
    }

    return TRUE;
  }
}


/*
 * Calculates the siallest possible as well as the widest possible width of a string.
 * Theese values are typically used in "shrink-wrapping" a text, for instance when
 * rendering tables in HTML.
 */
int
we_cmmn_min_max_width (const char* utf8_string, 
                        WeStringHandle string,
                        WeBrushHandle brush,
                        int* min_width,
                        int* max_width,
                        int* first_is_glue,
                        int* last_is_glue,
                        int* first_word_width,
                        int* last_word_width)
{
  const char* p = utf8_string;
  int i = 0;
  int bytes;
  const char* n;
  int min_s_idx = 0;
  int max_s_idx = 0;
  int first_word = TRUE;
  int b = 0;
  int ending_space = 0;
  int no_chars_min;
  int no_chars_max;
  WE_INT16 w = 0;
  int only_space = TRUE;

  *min_width = 0;
  *max_width = 0;
  *first_is_glue = FALSE;
  *last_is_glue = FALSE;

  /*?? is the below correct or should we drop WS before checking for glue?*/
  /* The glue check is performed before any space skipping.*/
  if (WE_BREAK_IS_GLUE(p)) {
    *first_is_glue = TRUE;      
  } 
  else if (WE_BREAK_IS_NLF(p, b)) {
    /* If the first character is an NLF, the first row has no min/max implications.*/
    i += 1 + b;
    p += 1 + b;
  }

  /*?? Bypass and count initial WS.*/
  while (*p && *p == ' ') {
    i++;
    first_word = FALSE;

    bytes = we_cmmn_utf8_bytes_per_character[(unsigned char)*p >> 4];
    /* Illegal character found.*/
    if (!bytes)
      return FALSE;
    p += bytes;
  }

  /* Did we step til end of string when bypassing initial white space/NLF?*/
  if (!*p) {
    return TRUE;
  }

  min_s_idx = max_s_idx = i;

  bytes = we_cmmn_utf8_bytes_per_character[(unsigned char)*p >> 4];
  if (!bytes)
    return FALSE;
  n = p + bytes;

  /* Find the next break opportunity until we bypass the allowable width or find
   * a hard break.*/
  while (*n) {
    /* b captures the additional character for *n in case of the CRLF NLF.*/
    int b = 0;
    int brk_type = WE_BREAK_TYPE(p, n, b);

    if (*p == ' ')
      ending_space++;
    else {
      ending_space = 0;
      only_space = FALSE;
    }

    if (brk_type == WE_BREAK_HARD) {
      no_chars_min = i - min_s_idx - ending_space + 1;
      no_chars_max = i - max_s_idx - ending_space + 1;

      if (no_chars_min) {
        TPIa_widgetStringGetWidth(string, brush, min_s_idx, no_chars_min, &w);

        *last_word_width = w;

        if (first_word) {
          *first_word_width = w;
          first_word = FALSE;
        }

        if (w > *min_width)
          *min_width = w;
      }

      if (no_chars_max) {
        TPIa_widgetStringGetWidth(string, brush, max_s_idx, no_chars_max, &w);

        if (first_word) {
          *first_word_width = w;
          first_word = FALSE;
        }

        if (w > *max_width)
          *max_width = w;
      }

      min_s_idx = max_s_idx = i + 1 + b;
    }
    else if (brk_type == WE_BREAK_OPPOR) {
      no_chars_min = i - min_s_idx - ending_space + 1;

      if (no_chars_min) {
        TPIa_widgetStringGetWidth(string, brush, min_s_idx, no_chars_min, &w);

        *last_word_width = w;

        if (first_word) {
          *first_word_width = w;
          first_word = FALSE;
        }

        if (w > *min_width)
          *min_width = w;
      }

      min_s_idx = i + 1;
    }

    /* Step to next character pair.*/
    p = n;
    bytes = we_cmmn_utf8_bytes_per_character[(unsigned char)*n >> 4];
    if (!bytes)
      return FALSE;
    n += bytes;
    i++;
  }

  if (WE_BREAK_IS_GLUE(p)) {
    *last_is_glue = TRUE;      
  }
  else if (*p == ' ' && !only_space)
    ending_space++;
  else
    ending_space = 0;

  /* Check the final parts of the string, if any.*/
  no_chars_min = i - min_s_idx - ending_space + 1;
  no_chars_max = i - max_s_idx - ending_space + 1;

  if (no_chars_min) {
    TPIa_widgetStringGetWidth(string, brush, min_s_idx, no_chars_min, &w);

    *last_word_width = w;

    if (first_word) {
      *first_word_width = w;
      first_word = FALSE;
    }

    if (w > *min_width)
      *min_width = w;
  }

  if (no_chars_max) {
    TPIa_widgetStringGetWidth(string, brush, max_s_idx, no_chars_max, &w);

    if (first_word)
      *first_word_width = w;

    if (w > *max_width)
      *max_width = w;
  }

  return TRUE;
}

/**********************************************************************
 * Date-time conversions
 **********************************************************************/

static const char we_cmmn_m[12] = {
  31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static const char we_cmmn_m4[12] = {
  31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static const int we_cmmn_monthdays[12] = {
  0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

static const char * const we_cmmn_day_names[] = {
  "Thu",
  "Fri",
  "Sat",
  "Sun",
  "Mon",
  "Tue",
  "Wed"
};

static const char * const we_cmmn_month_names[] = {
  "Jan",
  "Feb",
  "Mar",
  "Apr",
  "May",
  "Jun",
  "Jul",
  "Aug",
  "Sep",
  "Oct",
  "Nov",
  "Dec"
};

/*
 * Return the number of the month found in the string *s,
 * and update the pointer 's' to point beyond the month name.
 * Accepts any unambiguous prefix of a month name, in any
 * mixture of upper lower case.
 * Returns -1 if the month name was not recognized.
 */
static int
we_cmmn_month2int (char **s)
{
  char *start = *s;
  char *end = start;
  int   len, ret = -1;

  while (ct_isalpha (*end))
    end++;
  len = end - start;

  if ((len >= 2) && !we_cmmn_strncmp_nc (start, "january", len)) {
    ret = 1;
  }
  else if ((len >= 1) && !we_cmmn_strncmp_nc (start, "february", len)) {
    ret = 2;
  }
  else if ((len >= 3) && !we_cmmn_strncmp_nc (start, "march", len)) {
    ret = 3;
  }
  else if ((len >= 2) && !we_cmmn_strncmp_nc (start, "april", len)) {
    ret = 4;
  }
  else if ((len >= 3) && !we_cmmn_strncmp_nc (start, "may", len)) {
    ret = 5;
  }
  else if ((len >= 3) && !we_cmmn_strncmp_nc (start, "june", len)) {
    ret = 6;
  }
  else if ((len >= 3) && !we_cmmn_strncmp_nc (start, "july", len)) {
    ret = 7;
  }
  else if ((len >= 2) && !we_cmmn_strncmp_nc (start, "august", len)) {
    ret = 8;
  }
  else if ((len >= 1) && !we_cmmn_strncmp_nc (start, "september", len)) {
    ret = 9;
  }
  else if ((len >= 1) && !we_cmmn_strncmp_nc (start, "october", len)) {
    ret = 10;
  }
  else if ((len >= 1) && !we_cmmn_strncmp_nc (start, "november", len)) {
    ret = 11;
  }
  else if ((len >= 1) && !we_cmmn_strncmp_nc (start, "december", len)) {
    ret = 12;
  }
  if (ret > 0)
    *s += len;

  return ret;
}

/*
 * Converts a string (e.g. "Thu, 15 Nov 2001 08:12:31 GMT") into a
 * Unix 31-bit time value (seconds since 1/1 1970, GMT).
 * The date as a string can be in any of several different formats:
 *   "Sun, 06 Nov 1994 08:49:37 GMT"           RFC 822, updated by RFC 1123
 *   "Sunday, 06-Nov-94 08:49:37 GMT"          RFC 850, obsoleted by RFC 1036
 *   "Sun Nov  6 08:49:37 1994"                ANSI C's asctime() format
 *   "6 Nov 94 08:49:37 EST"                   One of many obsolete forms
 *   "[Sun,] 6 Nov 1994 08:49[:37] +0000"      RFC 2822
 *
 * The result is stored in 't'.
 * Returns TRUE on sucess, else FALSE.
 */
int
we_cmmn_str2time (const char *s, WE_UINT32 *t)
{
  int   month = -1, date, hour, min, sec = 0, zone = 0;
  int   leapyears, year = -1, x;
  char *p, *tmp;

  /* Do we have a weekday first? */
  p = we_cmmn_skip_blanks (s);
  if (ct_isalpha (p[0])) {
    /* Just skip over the weekday */
    p = we_cmmn_skip_field (p);
    p = we_cmmn_skip_blanks (p);
  }

  /* Do we have the name of the month before the date? */
  if (ct_isalpha (p[0])) {
    month = we_cmmn_month2int (&p);    
  }

  /* We should now be at the number of the day. */
  date = (int)strtol (p, &p, 10);
  if (p[0] == '-') {
    p++;
  }
  p = we_cmmn_skip_blanks (p);
  
  /* This should now be the name of the month (unless we saw it already) */
  if (ct_isalpha (p[0])) {
    if (month > 0)
      return FALSE;
    month = we_cmmn_month2int (&p);
  }
  if (month < 0)
    return FALSE;

  p = we_cmmn_skip_blanks (p);
  if (p[0] == '-') {
    p++;
  }

  /* Now, we either have the year, or, in case of ANSI C, the time.
   * We read a number and check whether it is followed by ':' to determine
   * which it is. */
  x = (int)strtol (p, &tmp, 10);
  if (tmp[0] == ':') {
    /* It was the hour */
    hour = x;
    p = tmp + 1;
  }
  else {
    /* It was the year. */
    year = x;
    p = we_cmmn_skip_blanks (tmp);
    hour = (int)strtol (p, &p, 10);
    if (p[0] != ':')
      return FALSE;
    p++;
  }

  /* Read minutes, and possibly seconds. */
  min = (int)strtol (p, &p, 10);
  if (p[0] == ':') {
    sec = (int)strtol (p + 1, &p, 10);
  }
  p = we_cmmn_skip_blanks (p);
  
  /* If we have asctime format, the year remains to be read,
   * otherwise it is the timezone. */
  if (year < 0) {
    if (!ct_isdigit (*p)) {
      return FALSE;
    }
    year = atoi (p);
  }
  else {
    if (ct_isalpha (p[0])) {
      if ((we_cmmn_strncmp_nc (p, "GMT", 3) == 0) ||
          (we_cmmn_strncmp_nc (p, "UT", 2) == 0)) {
        zone = 0;
      }
      else if (we_cmmn_strncmp_nc (p, "EDT", 3) == 0) {
        zone = -240;
      }
      else if (we_cmmn_strncmp_nc (p, "EST", 3) == 0) {
        zone = -300;
      }
      else if (we_cmmn_strncmp_nc (p, "CDT", 3) == 0) {
        zone = -300;
      }
      else if (we_cmmn_strncmp_nc (p, "CST", 3) == 0) {
        zone = -360;
      }
      else if (we_cmmn_strncmp_nc (p, "MDT", 3) == 0) {
        zone = -360;
      }
      else if (we_cmmn_strncmp_nc (p, "MST", 3) == 0) {
        zone = -420;
      }
      else if (we_cmmn_strncmp_nc (p, "PDT", 3) == 0) {
        zone = -420;
      }
      else if (we_cmmn_strncmp_nc (p, "PST", 3) == 0) {
        zone = -480;
      }
    }
    else {
      int sign = 1;

      if (p[0] == '+')
        p++;
      else if (p[0] == '-') {
        sign = -1;
        p++;
      }
      x = atoi (p);
      zone = (x / 100) * 60 + (x % 100);
      if (zone > 1200)
        return FALSE;
      if (sign < 0)
        zone = -zone;
    }
  }

  /* Check the year value we retrieved */
  if (year < 0)
    return FALSE;
  /* Compensate for 2-digit years */
  if (year < 40)
    year += 2000;
  else if (year < 100)
    year += 1900;
  if ((year < 1970) || (year > 2038)) {
    return FALSE;
  }
  year -= 1970;

  /* Compute number of leap years */
  leapyears = (year + 2) >> 2;
  if (((year & 0x03) == 0x02) && (month <= 2))
    leapyears--;

  *t =  year * 31536000 +
    (leapyears + we_cmmn_monthdays[month - 1] + date - 1) * 86400 +
    hour * 3600 + min * 60 + sec - zone * 60;

  return TRUE;
}

/*
 * Format a Unix 31-bit time value (seconds since 1/1 1970, GMT)
 * as a string in the format: "Thu, 15 Nov 2001 08:12:31 GMT"
 * (as specified in RFC 1123).
 * The parameter 't' is the given time, and the formatted string
 * is written to the string 's'.
 * Return the number of bytes written or -1 if something
 * went wrong.
 * NOTE: the string 's' must have room for at least 30 bytes.
 */
int
we_cmmn_time2str (WE_UINT32 t, char *s)
{
  int         secs, mins, hours, days, months, years, i, nly;
  const char *days_in_month;
  const char *day = NULL;

  /* First, divide into days, hours, minutes and seconds. */
  days = (int)(t / 86400);
  t = t % 86400;

  day = we_cmmn_day_names[days % 7];

  hours = (int)(t / 3600);
  t = t % 3600;

  mins = (int)(t / 60);
  secs = (int)(t % 60);

  /* Now the number of days has to be divided into years and months.
   * Start by approximating each year to be 365 days.
   * This approximation will be at most one off.
   * Compensate if necessary. */
  years = (days / 365);
  days = (days % 365);
  /* In this interval (1970 - 2038), every fourth year is a leap year,
   * without exceptions, starting with 1972. */
  nly = ((years + 1) / 4);
  if (days < nly) {
    years--;
    days = (days + 365 - nly);
    if (((years + 2) & 0x3) == 0) {
      days++;
    }
  }
  else {
    days -= nly;
  }

  /* To determine the month we simply do a linear search through
   * an array holding the number of days of each month.
   * First we have to select the correct array though,
   * there is one for leap years and one for non-leap years. */
  if (((years + 2) % 4) == 0)
    days_in_month = we_cmmn_m4;
  else
    days_in_month = we_cmmn_m;

  for (i = 0; i < 12; i++) {
    if (days < days_in_month[i])
      break;
    days -= days_in_month[i];
  }
  months = i;

  /* We are done. The variable "years" holds the number of complete
   * years that have passed since 1970; "months" holds the number
   * of complete months that have passed since the beginning of
   * the present year; and similarly for the other variables.
   *
   * The sprintf function returns the number of bytes (chars) stored
   * in the array s, not including the terminating null character. 
   */
  return sprintf (s, "%s, %02d %s %04d %02d:%02d:%02d GMT",
           day, days + 1, we_cmmn_month_names[months], (years + 1970),
           hours, mins, secs);
}


/* Parses a "duration" string (e.g. "PnYnMnDTnHnMnS") as defined in 
 * "XML Schema Part 2: Datatypes. W3C Recommendation 2 May 2001" 
 * and return the result in the duration struct
 */
void
we_cmmn_parse_duration(const char *s, we_cmmn_duration_t *duration)
{
  char  time_str[30] = "";
  char  period[30] = "";
  char  *w = NULL;
  char  *m = NULL;
  char  *p = NULL;
  int   year = 0;
  int   month = 0;
  int   day = 0;
  int   hour = 0;
  int   min = 0;
  int   sec = 0;

  /* Convert s to int */
  we_cmmn_strcpy_lc((char*)&period, s);

  /* Dates */
  w = strchr((char*)&period, 'p');
  if (w) {
    /* Years */
    p = strchr((char*)&period, 'y');
    if (p) {
      memset((void*)&time_str[0], 0, 30);
      strncpy(&time_str[0], w + 1, p - w -1);
      year = atoi(time_str);
      w = p;
    }
    
    /* Months */
    if (w) {
      p = strchr((char*)&period, 'm');
      if (p) {
        memset((void*)&time_str[0], 0, 30);
        strncpy(&time_str[0], w + 1, p - w -1);
        month = atoi(time_str);
        w = p;
      }
    }

    /* Days */
    if (w) {
      p = strchr((char*)&period, 'd');
      if (p) {
        memset((void*)&time_str[0], 0, 30);
        strncpy(&time_str[0], w + 1, p - w -1);
        day = atoi(time_str);
      }
    }
  }

  /* Times */
  w = strchr((char*)&period, 't');
  if (w) {
    m = w;

    /* Hours */
    p = strchr(m, 'h');
    if (p) {
      memset((void*)&time_str[0], 0, 30);
      strncpy(&time_str[0], w + 1, p - w -1);
      hour = atoi(time_str);
      w = p;
    }

    /* Minutes */
    p = strchr(m, 'm');
    if (p) {
      memset((void*)&time_str[0], 0, 30);
      strncpy(&time_str[0], w + 1, p - w -1);
      min = atoi(time_str);
      w = p;
    }

    /* Seconds */
    p = strchr(m, 's');
    if (p) {
      memset((void*)&time_str[0], 0, 30);
      strncpy(&time_str[0], w + 1, p - w -1);
      sec = atoi(time_str);
    }
  }

  duration->years = year;
  duration->months = month;
  duration->days = day;
  duration->hours = hour;
  duration->minutes = min;
  duration->seconds = sec;
}

/**********************************************************************
 * String lookup in hash tables
 **********************************************************************/

/*
 * Look up a string in the supplied hash table, and return
 * its associated integer value.
 * Returns -1 if the string was not found.
 */
int
we_cmmn_str2int (const char *name, int length, const we_strtable_info_t *info)
{
  const unsigned char        *hash_table = info->hash_table;
  int                         hash_table_size = info->hash_table_size;
  const we_strtable_entry_t *string_table = info->string_table;

  WE_UINT32 h = we_cmmn_strhash (name, length);
  int        idx = h % hash_table_size;
  int        step = 1 + (h % (hash_table_size - 2));
  int        tidx;

  for (;;) {
    tidx = hash_table[idx];
    if (tidx == 0xff)
      return -1;
    if (!strncmp (name, string_table[tidx].name, length) &&
        string_table[tidx].name[length] == '\0')
      return string_table[tidx].value;
    idx += step;
    if (idx >= hash_table_size)
      idx -= hash_table_size;
  }
}


/*
 * Look up a string in the supplied hash table, and return
 * its associated integer value.
 *
 * Lower-case version. When hashing and matching, all upper-
 * case ASCII characters are regarded as their lower-case
 * counterparts. Consequently the 'info' string table can
 * only contain lower-case strings.
 *
 * Returns -1 if the string was not found.
 */
int
we_cmmn_str2int_lc (const char *name, int length, const we_strtable_info_t *info)
{
  const unsigned char        *hash_table = info->hash_table;
  int                         hash_table_size = info->hash_table_size;
  const we_strtable_entry_t *string_table = info->string_table;

  WE_UINT32 h = we_cmmn_strhash_lc (name, length);
  int        idx = h % hash_table_size;
  int        step = 1 + (h % (hash_table_size - 2));
  int        tidx;

  for (;;) {
    tidx = hash_table[idx];
    if (tidx == 0xff)
      return -1;
    if (!we_cmmn_strncmp_nc (name, string_table[tidx].name, length) &&
        string_table[tidx].name[length] == '\0')
      return string_table[tidx].value;
    idx += step;
    if (idx >= hash_table_size)
      idx -= hash_table_size;
  }
}


/*
 * Look up an integer value in the supplied hash table,
 * and return its associated string.
 * Returns NULL if the integer value was not found.
 * NOTE: the caller MUST NOT deallocate the returned string!
 */
const char *
we_cmmn_int2str (int value, const we_strtable_info_t *info)
{
  if (info->search_method == WE_TABLE_SEARCH_INDEXING) {
    if ((value >= 0) && (value < info->string_table_size))
      return info->string_table[value].name;
  }
  else if (info->search_method == WE_TABLE_SEARCH_BINARY) {
    int                         i, j, k, v;
    const we_strtable_entry_t *string_table = info->string_table;

    i = 0;
    j = info->string_table_size - 1;
    while (i <= j) {
      k = ((i + j) >> 1);
      v = string_table[k].value;
      if (v == value)
        return string_table[k].name;
      else if (v < value)
        i = k + 1;
      else
        j = k - 1;
    }
  }
  else {
    /* Otherwise, use linear search. */
    int                         i;
    int                         string_table_size = info->string_table_size;
    const we_strtable_entry_t *string_table = info->string_table;

    for (i = 0; i < string_table_size; i++) {
      if (string_table[i].value == value)
        return string_table[i].name;
    }
  }

  return NULL;
}


/**********************************************************************
 * Misc routines
 **********************************************************************/

/*
 * Hash function. Given a string of indicated length,
 * this function returns a 32-bit unsigned integer
 * calculated over all positions in the string.
 */
WE_UINT32
we_cmmn_strhash (const char *s, int len)
{
  int i;
  int c0 = 0, c1 = 0, c2 = 0, c3 = 0;

  i = len;
  /* This merge of a switch and a while statement is known
   * as "Duff's device", and is used for efficiency reasons. */
  switch (len & 0x3) {
  case 0:
    while (i > 0) {
      c3 ^= s[--i];
  case 3:
      c2 ^= s[--i];
  case 2:
      c1 ^= s[--i];
  case 1:
      c0 ^= s[--i];
    }
  }

  return (((WE_UINT32)c0) << 24) | (((WE_UINT32)c1) << 16) |
    (((WE_UINT32)c2) <<  8) | ((WE_UINT32)c3);
}


/*
 * Hash function. Given a string of indicated length,
 * this function returns a 32-bit unsigned integer
 * calculated over all positions in the string.
 *
 * Lower-case version. All upper-case ASCII
 * characters are regarded as their lower-case
 * counterparts.
 */
WE_UINT32
we_cmmn_strhash_lc (const char *s, int len)
{
  int i;
  int c0 = 0, c1 = 0, c2 = 0, c3 = 0;

  i = len;
  /* This merge of a switch and a while statement is known
   * as "Duff's device", and is used for efficiency reasons. */
  switch (len & 0x3) {
  case 0:
    while (i > 0) {
      c3 ^= we_ascii_lc[(unsigned char)s[--i]];
  case 3:
      c2 ^= we_ascii_lc[(unsigned char)s[--i]];
  case 2:
      c1 ^= we_ascii_lc[(unsigned char)s[--i]];
  case 1:
      c0 ^= we_ascii_lc[(unsigned char)s[--i]];
    }
  }

  return (((WE_UINT32)c0) << 24) | (((WE_UINT32)c1) << 16) |
    (((WE_UINT32)c2) <<  8) | ((WE_UINT32)c3);
}
