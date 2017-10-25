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
 * We_Cmmn.h
 *
 * Created by Anders Edenwbandt, Fri Mar 16 13:01:30 2001.
 *
 * Common utilities for the Mobile Internet Client.
 *
 * Revision history:
 *   010509, AED: New functions, cmmn_skip_blanks and cmmn_skip_field.
 *   010629, AED: General reorganization and cleanup.
 *                Functions for string lookup.
 *   010720, FKN: REQUEST_METHOD_OPTIONS, REQUEST_METHOD_CONNECT added
 *   010806, NKE: Added const to some parameters.
 *   010829, AED: Change in cmmn_skip_field.
 *                New function, cmmn_skip_trailing_blanks
 *   010907, NKE: New function: cmmn_hex2long.
 *   010926, AED: Deleted WCHAR-routines, not needed anymore.
 *                Moved all memory-allocation routines to memalloc.h
 *   011003, AED: New function, cmmn_strcat.
 *   011003, FKN: Connection types moved to capimic.h
 *   011129, AED: New functions, cmmn_strcmp_nc, and cmmn_strncmp_nc
 *   011206, AED: New functions for UTF-8 string handling.
 *   020402, IPN: Changed to fit Mobile Suite Framework.
 *   020912, AED: New functions to convert date-time strings.
 *   050428, MAAR: ICR17556. we_cmmn_time2str() now return number of bytes.
 *
 */
#ifndef _we_cmmn_h
#define _we_cmmn_h

#ifndef _we_int_h
#include "We_Int.h"
#endif


/**********************************************************************
 * General constants and macros
 **********************************************************************/


#ifdef ABS
#undef ABS
#endif
#define ABS(a)   (((a) < 0) ? (-(a)) : (a))

#ifdef MAX
#undef MAX
#endif
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#ifdef MIN
#undef MIN
#endif
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define WE_CMMN_TIME_STR_LEN    30

/**********************************************************************
 * Global variables
 **********************************************************************/

/* Array of all ASCII characters, lower case only. */
extern const unsigned char we_ascii_lc[256];
/* Array of all ASCII characters, upper case only. */
extern const unsigned char we_ascii_uc[256];

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
we_cmmn_strdup (WE_UINT8 modId, const char *s);

/*
 * Return a copy of the string 's' of length at most 'len' characters.
 * Copying stops if a null-byte is found.
 * NOTE: if s == NULL, this function returns a pointer
 * to a null-terminated string of length 0. THIS MAY CHANGE IN THE FUTURE!
 * NOTE: it is the caller's responsibility to deallocate the
 * returned string.
 */
char *
we_cmmn_strndup (WE_UINT8 modId, const char *s, int len);

/*
 * Create and return a new string which holds the concatenation
 * of strings 's1' and 's2'.
 * NOTE: it is the caller's responsibility to deallocate the
 * returned string.
 */
char *
we_cmmn_strcat (WE_UINT8 modId, const char *s1, const char *s2);

/*
 * Create and return a new string which holds the concatenation
 * of strings 's1' and len2 bytes of 's2'.
 * NOTE: it is the caller's responsibility to deallocate the
 * returned string.
 */
char *
we_cmmn_strncat (WE_UINT8 modId, const char *s1, const char *s2, int len2);
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
                  const char  *s3);

/*
 * Return a pointer to the first occurrence of the character 'c'
 * in the string 's'. The search stops at a null-byte or after
 * at most 'len' characters.
 */
char *
we_cmmn_strnchr (const char *s, char c, int len);

/*
 * Copy the null-terminated string 'src' to 'dst',
 * while changing all uppercase letters to lower case.
 */
void
we_cmmn_strcpy_lc (char *dst, const char *src);

/*
 * Copy at most 'len' characters from the string 'src' to 'dst',
 * while changing all uppercase letters to lower case.
 */
void
we_cmmn_strncpy_lc (char *dst, const char *src, int len);

/*
 * Return the lower-case version of the character 'c'.
 */
char
we_cmmn_chrlc (char c);

/*
 * Perform a string comparison just like strcmp,
 * but without regard to case.
 * NOTE: only upper and lower case versions
 * of ASCII characters are treated as equal!
 */
int
we_cmmn_strcmp_nc (const char *s1, const char *s2);

/*
 * Perform a string comparison just like strncmp,
 * but without regard to case.
 * NOTE: only upper and lower case versions
 * of ASCII characters are treated as equal!
 */
int
we_cmmn_strncmp_nc (const char *s1, const char *s2, int len);

/*
 * Write a textual representation of a byte as two letters "XY",
 * being the hexadecimal digits that represent the value of the byte.
 * The string is written to the location indicated by 's'.
 */
void
we_cmmn_byte2hex (unsigned char b, char *s);

/*
 * Read a string representation, consisting of two hexadecimal
 * digits, and store the byte value it represents in the location
 * indicated by 'b'.
 * Returns FALSE if the string 's' does not start with two
 * hexadecimal digits, and TRUE otherwise.
 */
int
we_cmmn_hex2byte (const char *s, unsigned char *b);

/*
 * Read a null-terminated string consisting of hexadecimal digits,
 * and return the integer value it represents.
 */
int
we_cmmn_hex2int (const char *s);

/*
 * Read a null-terminated string consisting of hexadecimal digits,
 * and return the integer value it represents.
 */
long
we_cmmn_hex2long (const char *s);

/*
 * Given a UTF-8 encoded null-terminated string,
 * return the number of Unicode characters in the string.
 */
int
we_cmmn_utf8_strlen (const char *s);

/*
 * Given a UTF-8 encoded string of length 'len' bytes,
 * return the number of Unicode characters in the string.
 */
int
we_cmmn_utf8_strnlen (const char *s, int len);

/*
 * Given a UTF-8 encoded string,
 * return the number of bytes in the first Unicode character.
 */
int
we_cmmn_utf8_charlen (const char *s);

/*
 * Given a UTF-8 encoded null-terminated string,
 * move 'steps' Unicode characters forward and return
 * a pointer to the new position.
 */
char *
we_cmmn_utf8_str_incr (const char *s, int steps);


/**********************************************************************
 * Other string functions
 **********************************************************************/

/*
 * Return a pointer to the first non-blank character
 * in the string 's'. A blank character is either SP or TAB.
 */
char *
we_cmmn_skip_blanks (const char *s);

/*
 * Return a pointer to the first white space character
 * in the string 's', or the NULL character terminating the string.
 * A white space character is either SP, TAB, CR, or LF.
 */
char *
we_cmmn_skip_field (const char *s);

/*
 * Given a string 's' and its length, return the length
 * of that string if trailing white space were omitted.
 * A white space character is either SP, TAB, CR, or LF.
 */
int
we_cmmn_skip_trailing_blanks (const char *s, int len);

/*
 * Return the number of characters needed to decode
 * the string 'str' of length 'len'.
 */
int
we_cmmn_base64_decode_len (const char *str, int len);

/*
 * Given a string of base64-encoded data, decode it and
 * place it in the output buffer.
 * On input, 'outlen' should hold the maximum size of the output buffer,
 * on output, it will hold the number of bytes placed in the output buffer .
 *
 * Returns 0 if OK, and -1 on error.
 */
int
we_cmmn_base64_decode (const char *inbuf, int inlen, char *outbuf, int *outlen);

/*
 * Return the number of characters needed to encode
 * a string of length 'len'.
 */
int
we_cmmn_base64_encode_len (int len);

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
we_cmmn_base64_encode (const char *inbuf, int inlen, char *outbuf, int *outlen);

/**********************************************************************
 * Line-break
 **********************************************************************/

typedef struct {
  /* The index of the first character of the break, no_start_ws is not included.*/
  int start_idx;
  /* The index of the last character of the break excluding whitespace.*/
  int end_idx;

  /* Number of whitespaces at the start of the string, if include_initial_ws was TRUE.*/
  int no_start_ws;
  /* Number of ending whitespace and NLF characters.*/
  int no_end_ws;

  /* Whether a result was found that did fit into the given width. When no breaks are
   * found the first break is still supplied, since it is likely to be needed anyway.
   */
  int fits;
  /* The width of the resulting string data, from start_idx to end_idx - no_end_ws.*/
  WE_INT16 width;

  /* If the first character of the string prevents linebreaks before this is TRUE (nbsp etc).*/
  int first_is_glue;
  /* If the last character of the string prevents linebreaks after this is TRUE.*/
  int last_is_glue;

  /* Pointer to the utf8 data at end_idx + no_end_ws +1, which should be passed into 
   * utf8_at_idx if this function is called again for the remains of the string. NULL 
   * when no more data exists.*/
  const char* continue_at;
} we_cmmn_line_break_t;

/* Examples of we_cmmn_line_break_t results when the width supplied to 
 * we_cmmn_next_line_break should create a break in the second word.
 *
 * The number of actual characters of the break is: end_idx - start_idx - no_end_ws + 1.
 *
 * width 
 * |--------|
 * |        |
 * "Techfaith rules"
 *  |    ||
 *  |start_idx = 0
 *       ||
 *       |end_idx = 5
 *        |
 *        |continue_at points here.
 * no_end_ws = 1
 * No characters of the break: 5 - 0 - 1 + 1 = 5
 *
 * width 
 * |-------------|
 * |             |
 * " Techfaith \r\nrules"
 *   |        ||
 *   |start_idx = 1
 *            ||
 *            |end_idx = 8
 *             |
 *             |continue_at points here.
 * no_end_ws = 3
 * No characters of the break: 8 - 1 - 3 + 1 = 5
 *
 * Corner cases:
 *
 * " "  : 
 *   when include_initial_ws == TRUE:
 *      start_idx = 0, end_idx = 0, no_end_ws = 1, width > 0      (no chars 1 - 0 - 1 + 1 => 1)
 *   when include_initial_ws == FALSE: 
 *      start_idx = 1, end_idx = 1, no_end_ws = 1, no_start_ws = 1, width = 0 (no chars 1 - 1 - 1 + 1 => 0)
 * "\n" : 
 *      start_idx = 0, end_idx = 0, no_end_ws = 1, width = 0      (no chars 0 - 0 - 1 + 1 => 0)
 * " \n": 
 *   when include_initial_ws == TRUE:
 *      start_idx = 0, end_idx = 1, no_end_ws = 1, width > 0      (no chars 1 - 1 - 0 + 1 => 1)
 *   when include_initial_ws == FALSE: 
 *      start_idx = 1, end_idx = 1, no_end_ws = 1, width = 0      (no chars 1 - 1 - 1 + 1 => 0)
 */

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
 *                      end_idx + 1 in the following calls)
 * include_initial_ws,  whether the starting whitespace (U+0020) should be included in
 *                      the chunk of text that becomes the break and the width calculated.
 * brk                  the result.
 */
int
we_cmmn_next_line_break (const char* utf8_at_idx, 
                          WeStringHandle string,
                          WeBrushHandle brush,
                          WE_INT16 width,
                          int start_idx,
                          int include_initial_ws,
                          we_cmmn_line_break_t* brk);

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
                        int* last_word_width);

/**********************************************************************
 * Date-time conversions
 **********************************************************************/

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
we_cmmn_str2time (const char *s, WE_UINT32 *t);

/*
 * Format a Unix 31-bit time value (seconds since 1/1 1970, GMT)
 * as a string in the format: "Thu, 15 Nov 2001 08:12:31 GMT"
 * (as specified in RFC 1123).
 * The parameter 't' is the given time, and the formatted string
 * is written to the string 's'.
 * Return the number of bytes written or -1 if something
 * went wrong.
 * NOTE: the string 's' must have room for at least 30 bytes
 * (see WE_CMMN_TIME_STR_LEN above).
 */
int
we_cmmn_time2str (WE_UINT32 t, char *s);


typedef struct  we_cmmn_duration_t{
  WE_UINT32          years;
  WE_UINT32          months;
  WE_UINT32          days;
  WE_UINT32          hours;
  WE_UINT32          minutes;
  WE_UINT32          seconds;
} we_cmmn_duration_t;

/* Parses a "duration" string (e.g. "PnYnMnDTnHnMnS") as defined in 
 * "XML Schema Part 2: Datatypes. W3C Recommendation 2 May 2001" 
 * and return the result in the duration struct
 */
void
we_cmmn_parse_duration(const char *s, we_cmmn_duration_t *duration);

/**********************************************************************
 * String lookup in hash tables
 **********************************************************************/

/* Different search methods for finding an integer in a string table. */
#define WE_TABLE_SEARCH_INDEXING                 1
#define WE_TABLE_SEARCH_BINARY                   2
#define WE_TABLE_SEARCH_LINEAR                   3


/* A string table is an array of elements of this type: */
typedef struct {
  const char *name;
  int         value;
} we_strtable_entry_t;

/* A string table should be complemented with a hash table
 * (automatically generated), and all the information about
 * the tables is collected in a structure of this kind: */
typedef struct {
  const unsigned char        *hash_table;
  int                         hash_table_size;
  short                       search_method;
  const we_strtable_entry_t *string_table;
  int                         string_table_size;
} we_strtable_info_t;


/*
 * Look up a string in the supplied hash table, and return
 * its associated integer value.
 * Returns -1 if the string was not found.
 */
int
we_cmmn_str2int (const char *name, int length, const we_strtable_info_t *info);

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
we_cmmn_str2int_lc (const char *name, int length, const we_strtable_info_t *info);

/*
 * Look up an integer value in the supplied hash table,
 * and return its associated string.
 * Returns NULL if the integer value was not found.
 * NOTE: the caller MUST NOT deallocate the returned string!
 */
const char *
we_cmmn_int2str (int value, const we_strtable_info_t *info);


/**********************************************************************
 * Misc routines
 **********************************************************************/

/*
 * Hash function. Given a string of indicated length,
 * this function returns a 32-bit unsigned integer
 * calculated over all positions in the string.
 */
WE_UINT32
we_cmmn_strhash (const char *str, int length);

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
we_cmmn_strhash_lc (const char *s, int len);

#endif
