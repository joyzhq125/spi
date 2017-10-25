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
 * We_Url.c
 *
 * Liwbary of routines for handling URLs.
 *
 * Created by Anders Edenwbandt.
 *
 * Revision history:
 *   010926, AED: New function, url_find_path.
 *   011003, AED: New functions, url_drop_fragment, url_add_fragment.
 *   011004, AED: New function, url_wmls_escape_string.
 *   011015, AED: New function, url_wmls_unescape_string.
 *   020510, AED: Modified for WE
 *   030304, AED: Extended to handle URLs with IPv6 addresses, according
 *                to RFC 2732 and RFC 2373.
 *
 */
#include "We_Cfg.h"
#include "We_Def.h"
#include "We_Mem.h"
#include "We_Core.h"
#include "We_Chrt.h"
#include "We_Url.h"
#include "We_Cmmn.h"
#include "We_Lib.h"


/**********************************************************************
 * Private utility routines:
 **********************************************************************/

/*
 * Return the value of the unsigned decimal integer stored
 * in the first "len" bytes of the string "s".
 * Used to read port numbers.
 */
static int
we_url_getnum (const char *s, int len)
{
  int n = 0;
  int i;

  for (i = 0; i < len; i++)
    n = n * 10 + (s[i] - '0');

  return n;
}

/*
 * String comparison routine that returns TRUE if the strings are equal.
 * If 'noCase' is TRUE, the case of letters is insignificant.
 * Hex escape sequences of the form '%XY' are handled correctly,
 * that is, such a sequence is compared as if the character had first
 * been unescaped, unless it is a character in the "reserved" set.
 * The routine accepts NULL pointers as input, in which case the
 * result is TRUE if both string pointers are NULL.
*/
static int
we_url_compare_strings (const char *bs1, int len1,
                         const char *bs2, int len2, int no_case)
{
  char b1, b2, tmp;
  int  reserved_escape1, reserved_escape2;

  if (bs1 == NULL)
    return bs2 == NULL;
  else if (bs2 == NULL)
    return FALSE;

  for (; (len1 > 0) && (len2 > 0); len1--, len2--) {
    reserved_escape1 = reserved_escape2 = FALSE;

    b1 = *bs1++;
    if ((b1 == '%') && (len1 >= 2) &&
        we_cmmn_hex2byte (bs1, (unsigned char *)&tmp)) {
      bs1 += 2;
      len1 -= 2;
      b1 = tmp;
      if (ct_isreserved (tmp) || (tmp == '#')) {
        reserved_escape1 = TRUE;
      }
    }

    b2 = *bs2++;
    if ((b2 == '%') && (len2 >= 2) &&
        we_cmmn_hex2byte (bs2, (unsigned char *)&tmp)) {
      bs2 += 2;
      len2 -= 2;
      b2 = tmp;
      if (ct_isreserved (tmp) || (tmp == '#')) {
        reserved_escape2 = TRUE;
      }
    }

    if (no_case) {
      b1 = we_ascii_lc[(unsigned char)b1];
      b2 = we_ascii_lc[(unsigned char)b2];
    }

    if ((b1 != b2) || (reserved_escape1 ^ reserved_escape2))
      return FALSE;
  }

  return (len1 == len2);
}

/************************************************************
 * Public routines:
 ************************************************************/

const static struct {
  const we_scheme_t  sch;
  const char         *str;
  const WE_UINT8     slen;
} we_schemes[] = {
  {we_scheme_empty, "", 0},
  {we_scheme_http, "http", 4},
  {we_scheme_https, "https", 5},
  {we_scheme_file, "file", 4},
  {we_scheme_wtai, "wtai", 4},
  {we_scheme_about, "about", 5},
  {we_scheme_function, "function", 8},
  {we_scheme_unknown, "unknown", 7}
};
#define WE_LAST_SCHEME 7

/*
 * Return the scheme type named by the given string.
 * Returns scheme_unknown if it is not one of the predefined types.
 */
static we_scheme_t
we_url_scheme2type (const char *bs, int len)
{
  int i;

  if (bs == NULL)
    return we_scheme_empty;

  for (i = 0; i < WE_LAST_SCHEME; i++) {
    if (we_url_compare_strings (bs, len,
                                 we_schemes[i].str, we_schemes[i].slen, TRUE)) {
      return we_schemes[i].sch;
    }
  }

  return we_scheme_unknown;
}

/*
 * Return the scheme type named by the given string.
 * Returns scheme_unknown if its not one of the predefined types.
 */
we_scheme_t
we_url_str2scheme (char *sch)
{
  return we_url_scheme2type (sch, strlen (sch));
}

/*
 * Return a string representation of the scheme value.
 * NOTE: the caller must NOT modify or deallocate the returned string!
 */
const char *
we_url_scheme2str (we_scheme_t scheme)
{
  return we_schemes[((((int)scheme) >= 0) && (scheme < WE_LAST_SCHEME)) ? scheme :
                    WE_LAST_SCHEME].str;
}

/*
 * Sets all fields in the URL struct to NULL and 0, respectively.
 */
void
we_url_clear (we_url_t *urlp)
{
  int i;

  if (urlp == NULL)
    return;

  urlp->scheme_type = we_scheme_empty;
  for (i = 0; i < WE_URL_NUM_PARTS; i++) {
    urlp->s[i] = NULL;
    urlp->len[i] = 0;
  }
}

/*
 * Check that the string of length 'len' is a non-empty decimal number.
 */
static int
we_url_is_number (const char *bs, int len)
{
  int i;

  if (len <= 0)
    return FALSE;

  for (i = 0; i < len; i++, bs++)
    if (!ct_isdigit (*bs))
      return FALSE;

  return TRUE;
}

static int
we_url_value_limit(const char *bs, int len,int limit)
{
  int i,weight=1,sum=0;

  if (len <= 0)
    return FALSE;

  for (i=len, bs+=len-1 ; i>0 && sum<=limit; i--, bs--, weight*=10)
    if (ct_isdigit (*bs))
      sum+=((*bs)-'0')*weight;
    else
      return FALSE;

  return sum<=limit;
}


static int
we_url_check_top_label (const char *bs, int len)
{
  int i;

  if (len == 0)
    return FALSE;

  if (!ct_isalpha (bs[0]))
    return FALSE;
  for (i = 1; i < len - 1; i++) {
    if (!ct_isalphanum (bs[i]) && (bs[i] != '-'))
      return FALSE;
  }

  return ct_isalphanum (bs[len - 1]);
}

static int
we_url_check_domain_label (const char *bs, int len)
{
  int i;

  if (len == 0)
    return FALSE;

  if (!ct_isalphanum (bs[0]))
    return FALSE;
  for (i = 1; i < len - 1; i++) {
    if (!ct_isalphanum (bs[i]) && (bs[i] != '-') && (bs[i] != '_'))
      return FALSE;
  }

  return ct_isalphanum (bs[len - 1]);
}

static int
we_url_check_host_name (const char *bs, int len)
{
  int i, k;

  for (k = 0; k < len;) {
    for (i = k; (i < len) && (bs[i] != '.'); i++);
    if (i >= len)
      return we_url_check_top_label (&bs[k], (WE_UINT16)(i - k));
    if (!we_url_check_domain_label (&bs[k], (WE_UINT16)(i - k)))
      return FALSE;
    k = i + 1;
  }

  return TRUE;
}

/*
 * The grammar for IPv4 addresses is:
 *   IPv4address = 1*3DIGIT "." 1*3DIGIT "." 1*3DIGIT "." 1*3DIGIT
 */
static int
we_url_check_ipv4_address (const char *bs, int len)
{
  int i, k;
  int n = 0;

  for (k = 0; k < len;) {
    for (i = k; (i < len) && (bs[i] != '.'); i++);
    if (i + 1 == len)
      return FALSE;
    if ((i == k) || (i > k + 3) ||
          !we_url_value_limit(&bs[k], (WE_INT16)(i - k),255))
      return FALSE;
    n++;
    k = i + 1;
  }

  return (n == 4);
}


/*
 * The grammar for IPv6 addresses is:
 *   IPv6address = hexpart [ ":" IPv4address ]
 *   IPv4address = 1*3DIGIT "." 1*3DIGIT "." 1*3DIGIT "." 1*3DIGIT
 *
 *   hexpart     = hexseq | hexseq "::" [ hexseq ] | "::" [ hexseq ]
 *   hexseq      = hex4 *( ":" hex4)
 *   hex4        = 1*4HEXDIG
 */
static int
we_url_check_ipv6_address (const char *s, int len)
{
  int i, j, k;
  int len1 = 0, len2 = 0;
  int is_double = FALSE;
  int has_seen_double = 0;

  if (len <= 0)
    return FALSE;

  for (k = 0; k < len;) {
    for (i = k; (i < len) && (s[i] != ':'); i++);
    if (i >= len) {
      len2 = i - k;
      break;
    }
    if ((i < len - 1) && (s[i + 1] == ':')) {
      if (has_seen_double)
        return FALSE;
      is_double = TRUE;
      has_seen_double = TRUE;
    }
    else {
      is_double = FALSE;
    }

    len1 = i - k;
    if (len1 == 0) {
      /* Empty first seqment only allowed if it is terminated by "::" */
      if ((k > 0) || !is_double) {
        return FALSE;
      }
    }
    else if (len1 > 4) {
      return FALSE;
    }
    else {
      for (j = k; j < k + len1; j++) {
        if (!ct_ishex (s[j])) {
          return FALSE;
        }
      }
    }
    k = i + (is_double ? 2 : 1);
  }

  /* Last component can be either a IPv4 address, or a hex4 */
  if (len2 == 0) {
    /* The last component may be empty iff it was preceded by "::" */
    return is_double;
  }
  else if (we_cmmn_strnchr (s + k, '.', len2) != NULL) {
    return we_url_check_ipv4_address (s + k, len2);
  }
  else if (len2 > 4) {
    return FALSE;
  }
  else {
    for (j = k; j < k + len2; j++) {
      if (!ct_ishex (s[j])) {
        return FALSE;
      }
    }
  }

  return TRUE;
}

/*
 * Check the syntax of the scheme part.
 * As a side effect, store the scheme type in the we_url_t structure.
 */
static int
we_url_check_scheme (we_url_t *urlp)
{
  const char *p = urlp->s[WE_URL_SCHEME_PART];
  int         len = urlp->len[WE_URL_SCHEME_PART];
  int         i;

  if ((p == NULL) || (len == 0)) {
    urlp->scheme_type = we_scheme_empty;
    return TRUE;
  }

  /* Must start with a letter. */
  if (!ct_isalpha (*p))
    return FALSE;

  /* The rest of the scheme characters should be drawn
   * from the set {a-z,A-Z,0-9,+,-,.} */
  for (i = 1; i < len; i++) {
    char b = p[i];
    if (!ct_isalphanum (b) && (b != '+') && (b != '-') && (b != '.'))
      return FALSE;
  }
  urlp->scheme_type = we_url_scheme2type (p, len);

  return TRUE;
}

/*
 * Check the syntax of the userinfo part of a URL.
 */
static int
we_url_check_userinfo (we_url_t *urlp)
{
  const char *p = urlp->s[WE_URL_USERINFO_PART];
  int         len = urlp->len[WE_URL_USERINFO_PART];
  int         i;

  /* The userinfo part is *( pchar | ";" ) */
  for (i = 0; i < len; i++) {
    char b = p[i];
    if (!ct_ispchar (b) && (b != ';'))
      return FALSE;
  }

  return TRUE;
}

/*
 * Check the syntax of the host part of a URL.
 * The host part is defined as:
 *   host  = hostname | IPv4address | IPv6reference
 */
static int
we_url_check_host (we_url_t *urlp)
{
  const char *p = urlp->s[WE_URL_HOST_PART];
  int         len = urlp->len[WE_URL_HOST_PART];
  int         i;

  if ((p == NULL) || (len == 0))
    return TRUE;

  if (p[0] == '[')
    return we_url_check_ipv6_address (p + 1, len - 2);

  if (p[0] == '.')
    return FALSE;

  /* Find right-most '.' */
  for (i = len - 1; (i > 0) && (p[i] != '.'); i--);

  if (i == 0)
    /* Only one component, must be a top-level domain. */
    return we_url_check_top_label (p, len);

  else if (i == len - 1)
    /* A final '.' can be ignored in a hostname. */
    return we_url_check_host_name (p, len - 1);

  else if (ct_isdigit (p[i + 1]))
    /* If the final component starts with a digit, it must be an IP-address */
    return we_url_check_ipv4_address (p, len);

  else
    return we_url_check_host_name (p, len);
}

/*
 * Check the syntax of the port number field of a URL.
 * The definition of the port number field is:  port = *digit
 */
static int
we_url_check_port (we_url_t *urlp)
{
  const char *p = urlp->s[WE_URL_PORT_PART];
  int         len = urlp->len[WE_URL_PORT_PART];
  int         i;

  if (p == NULL)
    return TRUE;

  if (len == 0)
    return FALSE;

  for (i = 0; i < len; i++) {
    if (!ct_isdigit (p[i]))
      return FALSE;
  }

  return TRUE;
}

/*
 * Check the syntax of the "authority" part of a URL.
 */
static int
we_url_check_authority (we_url_t *urlp)
{
  if ((urlp->s[WE_URL_AUTHORITY_PART] == NULL) ||
      (urlp->len[WE_URL_AUTHORITY_PART] == 0))
    return TRUE;

  return we_url_check_userinfo (urlp)
    && we_url_check_host (urlp)
    && we_url_check_port (urlp);
}

/*
 * Check the syntax of the query part of a URL.
 * The definition of the query part is:  query = *uric
 */
static int
we_url_check_query (we_url_t *urlp)
{
  const char *p = urlp->s[WE_URL_QUERY_PART];
  int         len = urlp->len[WE_URL_QUERY_PART];
  int         i;

  for (i = 0; i < len; i++) {
    char b = p[i];
    /* Allow \ even thou it should not be allowed according to the specification. 
        Most other browsers allow it. */
    if (!ct_isuric (b) && b != '\\') { 
      if ((b == '%') && we_cmmn_hex2byte (p + i + 1, (unsigned char *)&b)) {
        i += 2;
      }
      else
        return FALSE;
    }
  }

  return TRUE;
}

/*
 * Check the syntax of the fragment part of a URL.
 * The definition of the fragment part is:  fragment = *uric
 * NOTE: our checking of fragments differ from RFC2396 in that we
 * allow '#' and '"' in fragment parts.
 */
static int
we_url_check_fragment (we_url_t *urlp)
{
  const char *p = urlp->s[WE_URL_FRAGMENT_PART];
  int         len = urlp->len[WE_URL_FRAGMENT_PART];
  int         i;

  for (i = 0; i < len; i++) {
    char b = p[i];
    if (!ct_isuric (b) && (b != '#') && (b != '"')) {
      if ((b == '%') && we_cmmn_hex2byte (p + i + 1, (unsigned char *)&b)) {
        i += 2;
      }
      else
        return FALSE;
    }
  }

  return TRUE;
}

/*
 * Check the syntax of the path component of a URL.
 */
static int
we_url_check_path (we_url_t *urlp)
{
  const char *p = urlp->s[WE_URL_PATH_PART];
  int         len = urlp->len[WE_URL_PATH_PART];
  int         i;

  for (i = 0; i < len; i++) {
    char b = p[i];
    if (!ct_ispchar (b) && (b != '/') && (b != ';')) {
      if ((b == '%') && we_cmmn_hex2byte (p + i + 1, (unsigned char *)&b))
        i += 2;
      else
        return FALSE;
    }
  }

  return TRUE;
}

/*
 * Take a string representation of a URL and parse it into its
 * components, and store these as fields in the given URL struct.
 * All components are stored in their original (possibly escaped) form.
 * Returns TRUE if the URL could be broken down into its
 * components, and FALSE otherwise.
 * The input parameter "bs" is a null-terminated character string.
 */
int
we_url_parse (const char *bs, we_url_t *urlp)
{
  char *start = (char *)bs;
  char *p;
  char  b = 0;
  int         i, len;

  if ((bs == NULL) || (urlp == NULL))
    return FALSE;

  we_url_clear (urlp);
  len = strlen (bs);

  /* Search for first part */
  for (p = start; *p; p++) {
    b = *p;
    if ((b == ':') || (b == '/') || (b == '?') || (b == '#'))
      break;
  }
  if (*p == 0)
    /* The whole URL is just one relative path segment */
    goto POS51;

  /* Determine which part it is that we found: */
  switch (b) {
  case ':':
    /* We have found the scheme */
    urlp->s[WE_URL_SCHEME_PART] = start;
    urlp->len[WE_URL_SCHEME_PART] = p - start;
    urlp->scheme_type = we_url_scheme2type (start, p - start);
    start = p + 1;
    break;

  case '?':
  case '#':
    /* We have found the path, followed by either query or fragment */
    urlp->s[WE_URL_PATH_PART] = start;
    urlp->len[WE_URL_PATH_PART] = p - start;
    start = p;
    if (b == '?')
      goto BEGIN_QUERY;
    else
      goto BEGIN_FRAGMENT;
  }

  if ((start[0] != '/') || (start[1] != '/'))
    goto POS5;
  /* We have a '//' that starts here */
  start += 2;
  /* Search for the end of the authority part */
  for (p = start; *p; p++) { 
    char b = *p;
    if ((b == '/') || (b == '?') || (b == '#')) {
      break;
    }
  }
  urlp->s[WE_URL_AUTHORITY_PART] = start;
  len = p - start;
  urlp->len[WE_URL_AUTHORITY_PART] = len;

  /* Divide the authority part into userinfo, host, and port number,
   * according to the rules:
   *   authority     = [ userinfo "@" ] hostport
   *   hostport      = host [ ":" port ]
   *   host          = hostname | IPv4address | IPv6reference
   *   IPv6reference = "[" IPv6address "]"
   */
  if (start[0] == '[')
    goto BEGIN_HOSTPORT;

  for (i = 0; i < len; i++) {
    if (start[i] == '@') {
      urlp->s[WE_URL_USERINFO_PART] = start;
      urlp->len[WE_URL_USERINFO_PART] = i;
      start += i + 1;
      len -= (i + 1);
      goto BEGIN_HOSTPORT;
    }
    else if (start[i] == ':') {
      urlp->s[WE_URL_HOST_PART] = start;
      urlp->len[WE_URL_HOST_PART] = i;
      urlp->s[WE_URL_PORT_PART] = &start[i + 1];
      urlp->len[WE_URL_PORT_PART] = len - i - 1;
      goto BEGIN_PATH;
    }
  }
  urlp->s[WE_URL_HOST_PART] = start;
  urlp->len[WE_URL_HOST_PART] = len;
  goto BEGIN_PATH;

 BEGIN_HOSTPORT:
  urlp->s[WE_URL_HOST_PART] = start;
  urlp->len[WE_URL_HOST_PART] = len;

  /* Divide into host and port */
  if (start[0] == '[') {
    /* IPv6reference, must find matching ']' */
    for (i = 1; i < len; i++) {
      if (start[i] == ']') {
        urlp->len[WE_URL_HOST_PART] = i + 1;
        i++;
        goto BEGIN_PORT;
      }
    }
    /* No matching ']', error. */
    return FALSE;
  }
  else {
    for (i = 0; i < len; i++) {
      if (start[i] == ':') {
        urlp->len[WE_URL_HOST_PART] = i;
        goto BEGIN_PORT;
      }
    }
  }
 BEGIN_PORT:
  if ((i < len) && (start[i] == ':')) {
    urlp->s[WE_URL_PORT_PART] = &start[i + 1];
    urlp->len[WE_URL_PORT_PART] = len - i - 1;
  }

 BEGIN_PATH:
  /* Check if there is anything left */
  if (*p == 0)
    goto POS_END;
  start = p;

 POS5:
  /* We are now at the beginning of the path part.
   * Search for the end of the path. */
  for (p = start; *p; p++) { 
    char b = *p;
    if ((b == '?') || (b == '#'))
      break;
  }
 POS51:
  urlp->s[WE_URL_PATH_PART] = start;
  urlp->len[WE_URL_PATH_PART] = p - start;
  /* Check if there is anything left */
  if (*p == 0)
    goto POS_END;
  start = p;

 BEGIN_QUERY:
  if (start[0] != '?')
    goto BEGIN_FRAGMENT;
  start += 1;
  /* We are now at the beginning of the query part.
   * Search for the end of the query part. */
  for (p = start; *p; p++) { 
    if (*p == '#')
      break;
  }
  urlp->s[WE_URL_QUERY_PART] = start;
  urlp->len[WE_URL_QUERY_PART] = p - start;
  /* Check if there is anything left */
  if (*p == 0)
    goto POS_END;
  start = p;

 BEGIN_FRAGMENT:
  if (start[0] != '#')
    /* This should not be possible? */
    return FALSE;

  start += 1;
  /* We are now at the beginning of the fragment part.
   * Search for the end of the fragment part. */
  for (p = start; *p; p++);
  urlp->s[WE_URL_FRAGMENT_PART] = start;
  urlp->len[WE_URL_FRAGMENT_PART] = p - start;

 POS_END:
  return TRUE;
}

/*
 * The inverse of the parsing routine: create a URL string
 * from its different parts.
 * Returns NULL in case of error.
 * NOTE: it is the caller's responsibility to deallocate the returned string.
 */
char *
we_url_assemble (WE_UINT8 module, we_url_t *urlp)
{
  int len =
    (urlp->s[WE_URL_SCHEME_PART] != NULL ? urlp->len[WE_URL_SCHEME_PART] + 1 : 0) +
    (urlp->s[WE_URL_AUTHORITY_PART] != NULL ? 2 : 0) +
    (urlp->s[WE_URL_USERINFO_PART] != NULL ? urlp->len[WE_URL_USERINFO_PART] + 1 : 0) +
    (urlp->s[WE_URL_HOST_PART] != NULL ? urlp->len[WE_URL_HOST_PART] : 0) +
    (urlp->s[WE_URL_PORT_PART] != NULL ? urlp->len[WE_URL_PORT_PART] + 1 : 0) +
    (urlp->s[WE_URL_PATH_PART] != NULL ? urlp->len[WE_URL_PATH_PART] : 0) +
    (urlp->s[WE_URL_QUERY_PART] != NULL ? urlp->len[WE_URL_QUERY_PART] + 1 : 0) +
    (urlp->s[WE_URL_FRAGMENT_PART] != NULL ? urlp->len[WE_URL_FRAGMENT_PART] + 1 : 0);
  char *buf, *p;

  buf = WE_MEM_ALLOC (module, len + 1);

  p = buf;
  if (urlp->s[WE_URL_SCHEME_PART] != NULL) {
    memcpy (p, urlp->s[WE_URL_SCHEME_PART], urlp->len[WE_URL_SCHEME_PART]);
    p += urlp->len[WE_URL_SCHEME_PART];
    *p++ = ':';
  }

  if (urlp->s[WE_URL_AUTHORITY_PART] != NULL) {
    *p++ = '/';
    *p++ = '/';
  }    

  if (urlp->s[WE_URL_USERINFO_PART] != NULL) {
    memcpy (p, urlp->s[WE_URL_USERINFO_PART], urlp->len[WE_URL_USERINFO_PART]);
    p += urlp->len[WE_URL_USERINFO_PART];
    *p++ = '@';
  }

  if (urlp->s[WE_URL_HOST_PART] != NULL) {
    memcpy (p, urlp->s[WE_URL_HOST_PART], urlp->len[WE_URL_HOST_PART]);
    p += urlp->len[WE_URL_HOST_PART];
  }

  if (urlp->s[WE_URL_PORT_PART] != NULL) {
    *p++ = ':';
    memcpy (p, urlp->s[WE_URL_PORT_PART], urlp->len[WE_URL_PORT_PART]);
    p += urlp->len[WE_URL_PORT_PART];
  }

  if (urlp->s[WE_URL_PATH_PART] != NULL) {
    memcpy (p, urlp->s[WE_URL_PATH_PART], urlp->len[WE_URL_PATH_PART]);
    p += urlp->len[WE_URL_PATH_PART];
  }

  if (urlp->s[WE_URL_QUERY_PART] != NULL) {
    *p++ = '?';
    memcpy (p, urlp->s[WE_URL_QUERY_PART], urlp->len[WE_URL_QUERY_PART]);
    p += urlp->len[WE_URL_QUERY_PART];
  }

  if (urlp->s[WE_URL_FRAGMENT_PART] != NULL) {
    *p++ = '#';
    memcpy (p, urlp->s[WE_URL_FRAGMENT_PART], urlp->len[WE_URL_FRAGMENT_PART]);
    p += urlp->len[WE_URL_FRAGMENT_PART];
  }
  *p = '\0';

  return buf;
}


#define HASH_CHAR(n, c, d)  {n ^= (c << d); d = (d + 8) & 0x1f; }

/*
 * Hash part of a URL, while replacing escape sequences with
 * the characters they represent, unless they are "reserved".
 */
static void
we_url_hash_part (we_url_t *urlp, int which_part, WE_UINT32 *n, int *d)
{
  const char *p;
  int         len;
  int         i;
  char        c, tmp;

  p = urlp->s[which_part];
  len = urlp->len[which_part];
  if ((p != NULL) && (len > 0)) {
    for (i = 0; i < len; i++) {
      c = p[i];
      if ((c == '%') && (i + 2 < len) &&
          we_cmmn_hex2byte (&p[i + 1], (unsigned char *)&tmp)) {
        if (ct_isreserved (tmp) || (tmp == '#')) {
          HASH_CHAR (*n, c, *d);
          HASH_CHAR (*n, p[i + 1], *d);
          HASH_CHAR (*n, p[i + 2], *d);
        }
        else {
          HASH_CHAR (*n, tmp, *d);
        }
        i += 2;
      }
      else {
        HASH_CHAR (*n, c, *d);
      }
    }
  }
}

/*
 * Compute a hash value from a URL and store it in the location pointed
 * to by "hv". All equivalent URLs will hash to the same value,
 * but two non-equal URLs may also have the same hash value. However,
 * the probability of a collision is siall.
 *
 * The hashing is done on a "canonical" form of the given URL.
 * The following conversions are applied:
 *   1) The scheme and authority parts are changed into lower case.
 *   2) Default port numbers are removed, other port numbers have
 *      leading zeros dropped.
 *   3) Escape sequences (%xy) are replaced by the character they
 *      represent, unless it is a reserved character. In the latter
 *      case, the hex digits in the escape sequence are changed to
 *      lower case.
 * For any two URLs X and Y that compare equal (i.e., the routine
 * WE_URL_Equal returns TRUE), this routine will produce identical
 * hash values.
 * The hash function is simply an XOR of the URL string divided
 * into four-byte blocks.
 *
 * Returns FALSE on error, TRUE otherwise.
 */
int
we_url_hash (const char *bs, WE_UINT32 *hv)
{
  int         i, len, port;
  const char *p;
  char        c;
  we_url_t   url;
  WE_UINT32  n = 0;
  int         d = 0;

  if ((bs == NULL) || !we_url_parse (bs, &url))
    return FALSE;

  /* Hash the scheme */
  if (url.s[WE_URL_SCHEME_PART] != NULL) {
    p = url.s[WE_URL_SCHEME_PART];
    for (i = 0; i < url.len[WE_URL_SCHEME_PART]; i++) {
      c = we_ascii_lc[(unsigned char)(p[i])];
      HASH_CHAR (n, c, d);
    }
    HASH_CHAR (n, ':', d);
  }

  if (url.s[WE_URL_AUTHORITY_PART] != NULL) {
    HASH_CHAR (n, '/', d);
    HASH_CHAR (n, '/', d);

    if (url.s[WE_URL_USERINFO_PART] != NULL) {
      p = url.s[WE_URL_USERINFO_PART];
      for (i = 0; i < url.len[WE_URL_USERINFO_PART]; i++) {
        c = we_ascii_lc[(unsigned char)(p[i])];
        HASH_CHAR (n, c, d);
      }
      HASH_CHAR (n, '@', d);
    }
    if (url.s[WE_URL_HOST_PART] != NULL) {
      p = url.s[WE_URL_HOST_PART];
      len = url.len[WE_URL_HOST_PART];
      if ((len > 0) && (p[len - 1] == '.'))
        len--;
      for (i = 0; i < len; i++) {
        c = we_ascii_lc[(unsigned char)(p[i])];
        HASH_CHAR (n, c, d);
      }
    }

    /* Get port number: skip default port numbers and empty port numbers,
     * and drop leading zeros in the port number. */
    p = url.s[WE_URL_PORT_PART];
    len = url.len[WE_URL_PORT_PART];
    if ((p != NULL) && (len > 0)) {
      port = we_url_getnum (p, len);
      if (!(((url.scheme_type == we_scheme_http) && (port == 80)) ||
            ((url.scheme_type == we_scheme_https) && (port == 443)))) {
        HASH_CHAR (n, ':', d);
        if (port == 0) {
          HASH_CHAR (n, '0', d);
        }
        else {
          while (*p == '0') {
            p++;
            len--;
          }
          for (i = 0; i < len; i++) {
            HASH_CHAR (n, p[i], d);
          }
        }
      }
    }
  }

  we_url_hash_part (&url, WE_URL_PATH_PART, &n, &d);

  if (url.s[WE_URL_QUERY_PART] != NULL) {
    HASH_CHAR (n, '?', d);
    we_url_hash_part (&url, WE_URL_QUERY_PART, &n, &d);
  }

  if (url.s[WE_URL_FRAGMENT_PART] != NULL) {
    HASH_CHAR (n, '#', d);
    we_url_hash_part (&url, WE_URL_FRAGMENT_PART, &n, &d);
  }

  *hv = n;
  return TRUE;
}

/*
 * Given a URL struct, construct and return a string representation
 * of the URL.
 * Returns NULL in case of error.
 * NOTE: It is the callers responsibility to deallocate the returned string.
 */
char *
we_url_to_string (WE_UINT8 module, we_url_t *url)
{
  if (url == NULL)
    return NULL;

  return we_url_assemble (module, url);
}

#define IS_DOT(x) ((x >= buf + 2) && (*(x - 1) == '.') && \
    (*(x - 2) == '\0'))

#define IS_DOTDOT(x) ((x >= buf + 3) && (*(x - 1) == '.') && \
    (*(x - 2) == '.') && (*(x - 3) == '\0'))

/*
 * Given two path names, an absolute path (starting with '/') in 'base',
 * and a relative path in 'rel', combine them into a new absolute path
 * removing all unnecessary './' and '../' segments.
 * Returns a newly allocated string with the new path.
 */
static char *
we_url_remove_dots (WE_UINT8 module, const char *base, int base_len,
                     const char *rel, int rel_len)
{
  int    len = 0;
  char  *buf, *p, *q;
  int    i, j, n, num_segs;
  char **segment;
  int    removed_final_slash = 0;

  /* Find the right-most '/' character in the base path. */
  for (i = base_len - 1; i >= 0; i--) {
    if (base[i] == '/')
      break;
  }
  if (i >= 0)
    len = i + 1;
  buf = WE_MEM_ALLOC (module, len + rel_len + 1);

  /* Append the relative path to the base path. */
  memcpy (buf, base, len);
  memcpy (buf + len, rel, rel_len);
  n = len + rel_len;
  buf[n] = '\0';

  /* Count the number of '/'-characters */
  j = 0;
  for (i = 0; i < n - 1; i++) {
    if (buf[i] == '/')
      j++;
  }
  num_segs = j + 1;
  segment = WE_MEM_ALLOC (module, num_segs * sizeof (char *));

  /* Find all '/'-characters, and replace with null bytes. */
  j = 0;
  for (i = 0; i < n - 1; i++) {
    if (buf[i] == '/') {
      buf[i] = '\0';
      segment[j++] = buf + i;
    }
  }
  if (buf[n - 1] == '/') {
    buf[n - 1] = '\0';
    segment[j++] = buf + n - 1;
    removed_final_slash = 1;
  }
  else {
    segment[j++] = buf + n;
  }

  /* Remove all occurrences of './' */
  j = 0;
  for (i = 0; i < num_segs - 1; i++) {
    p = segment[i];
    if (!IS_DOT(p)) {
      segment[j++] = segment[i];
    }
  }
  p = segment[num_segs - 1];
  if (!IS_DOT(p)) {
    segment[j++] = segment[i];
  }
  else
    removed_final_slash = 1;
  num_segs = j;

  /* Remove all occurrences of '<segment>/../'. */
  for (i = 0, j = 0; i < num_segs - 1; i++){
    if ((j > 1) && !IS_DOTDOT (segment[j - 1]) && IS_DOTDOT (segment[i])) {
      j--;
    }
    else
      segment[j++] = segment[i];
  }
  if ((j > 1) && !IS_DOTDOT (segment[j - 1]) &&
      IS_DOTDOT (segment[num_segs - 1])) {
    j--;
    removed_final_slash = 1;
  }
  else
    segment[j++] = segment[num_segs - 1];
  num_segs = j;

  /* Assemble final string */
  p = buf;
  for (i = 1; i < num_segs; i++) {
    *p = '/';
    q = segment[i] - 1;
    while ((*q != '\0') && (*q != '/'))
      q--;
    while ((*++p = *++q) != '\0');
  }
  if (removed_final_slash)
    *p++ = '/';
  *p = '\0';

  WE_MEM_FREE (module, segment);
  return buf;
}

/*
 * Given a base URL and a relative URL, resolve the relative reference
 * and store as an absolute URL in the string "*abs".
 * Returns TRUE on success, FALSE otherwise, in which case nothing
 * is stored in "abs".
 * NOTE: It is the callers responsibility to deallocate the returned string.
 */
static int
we_url_resolve_internal (WE_UINT8 module, we_url_t *base, we_url_t *rel, char **abs)
{
  we_url_t new_url;

  if ((base == NULL) || (rel == NULL) || (abs == NULL))
    return FALSE;

  if (base->s[WE_URL_SCHEME_PART] == NULL)
    /* Not a correct absolute URL. */
    return FALSE;

  if (base->len[WE_URL_PATH_PART] == 0) {
    base->s[WE_URL_PATH_PART] = "/";
    base->len[WE_URL_PATH_PART] = 1;
  }

  /* If rel has a scheme, then it is an absolute reference.
   * Just copy it. */
  if (rel->s[WE_URL_SCHEME_PART] != NULL) {
    if ((*abs = we_url_assemble (module, rel)) == NULL)
      return FALSE;
  }

  /* If rel has an authority part, just use the scheme from the base part. */
  else if (rel->s[WE_URL_AUTHORITY_PART] != NULL) {
    new_url = *rel;
    new_url.s[WE_URL_SCHEME_PART] = base->s[WE_URL_SCHEME_PART];
    new_url.len[WE_URL_SCHEME_PART] = base->len[WE_URL_SCHEME_PART];
    new_url.scheme_type = base->scheme_type;

    *abs = we_url_assemble (module, &new_url);
  }
  else if ((rel->len[WE_URL_PATH_PART] == 0) ) {
      /* 'rel' is empty or just a fragment */
      new_url = *base;
      
      new_url.s[WE_URL_FRAGMENT_PART] = rel->s[WE_URL_FRAGMENT_PART];
      new_url.len[WE_URL_FRAGMENT_PART] = rel->len[WE_URL_FRAGMENT_PART];
      new_url.s[WE_URL_QUERY_PART] = rel->s[WE_URL_QUERY_PART];
      new_url.len[WE_URL_QUERY_PART] = rel->len[WE_URL_QUERY_PART];
      *abs = we_url_assemble (module, &new_url);
  }
  else {
    /* 'rel' does not have a scheme nor an authority part,
     * but is more than just a fragment */
    char *tmp;

    if (rel->len[WE_URL_PATH_PART] == 0)
      tmp = we_url_remove_dots (module,
                                 base->s[WE_URL_PATH_PART],
                                 base->len[WE_URL_PATH_PART],
                                 "", 0);
    else if (*(rel->s[WE_URL_PATH_PART]) != '/')
      tmp = we_url_remove_dots (module,
                                 base->s[WE_URL_PATH_PART],
                                 base->len[WE_URL_PATH_PART],
                                 rel->s[WE_URL_PATH_PART],
                                 rel->len[WE_URL_PATH_PART]);
    else
      tmp = we_cmmn_strndup (module, rel->s[WE_URL_PATH_PART],
                              rel->len[WE_URL_PATH_PART]);

    new_url = *base;
    new_url.s[WE_URL_PATH_PART] = tmp;
    new_url.len[WE_URL_PATH_PART] = strlen (tmp);
    new_url.s[WE_URL_QUERY_PART] = rel->s[WE_URL_QUERY_PART];
    new_url.len[WE_URL_QUERY_PART] = rel->len[WE_URL_QUERY_PART];
    new_url.s[WE_URL_FRAGMENT_PART] = rel->s[WE_URL_FRAGMENT_PART];
    new_url.len[WE_URL_FRAGMENT_PART] = rel->len[WE_URL_FRAGMENT_PART];

    *abs = we_url_assemble (module, &new_url);
    WE_MEM_FREE (module, tmp);
  }

  return TRUE;
}

int
we_url_resolve (WE_UINT8 module, const char *base, const char *rel, char **abs)
{
  we_url_t base_url, relative_url;
  int       ok = TRUE;

  if ((base == NULL) || (rel == NULL) || (abs == NULL))
    return FALSE;

  we_url_clear (&relative_url);

  if (!we_url_parse (base, &base_url) ||
      !we_url_parse (rel, &relative_url) ||
      !we_url_resolve_internal (module, &base_url, &relative_url, abs)) {
    *abs = NULL;
    ok = FALSE;
  }

  return ok;
}


/*
 * Return TRUE if the two URLs are equal, FALSE otherwise.
 * "which_components" is a bitmap indicating which parts of the URLs
 * should be included in the comparison.
 * Returns FALSE in case of error.
 */
static int
we_url_equal_internal (we_url_t *url1, we_url_t *url2, int which_components)
{
  const char *p1, *p2, *path1, *path2;
  int         p1_len, p2_len, path1_len, path2_len;
  int         port1 = 0, port2 = 0;

  if (url1 == NULL)
    return (url2 == NULL);
  if (url2 == NULL)
    return FALSE;

  if (which_components & WE_URL_PORT_COMP) {
    if ((url1->s[WE_URL_PORT_PART] == NULL) &&
        (url1->scheme_type == we_scheme_http)) {
      p1 = "80";
      p1_len = 2;
    }
    else if ((url1->s[WE_URL_PORT_PART] == NULL) &&
             (url1->scheme_type == we_scheme_https)) {
      p1 = "443";
      p1_len = 3;
    }
    else {
      p1 = url1->s[WE_URL_PORT_PART];
      p1_len = url1->len[WE_URL_PORT_PART];
    }
    port1 = we_url_getnum (p1, p1_len);

    if ((url2->s[WE_URL_PORT_PART] == NULL) &&
        (url2->scheme_type == we_scheme_http)) {
      p2 = "80";
      p2_len = 2;
    }
    else if ((url2->s[WE_URL_PORT_PART] == NULL) &&
             (url2->scheme_type == we_scheme_https)) {
      p2 = "443";
      p2_len = 3;
    }
    else {
      p2 = url2->s[WE_URL_PORT_PART];
      p2_len = url2->len[WE_URL_PORT_PART];
    }
    port2 = we_url_getnum (p2, p2_len);
  }

  if (url1->len[WE_URL_PATH_PART] == 0) {
    path1 = "/";
    path1_len = 1;
  }
  else {
    path1 = url1->s[WE_URL_PATH_PART];
    path1_len = url1->len[WE_URL_PATH_PART];
  }

  if (url2->len[WE_URL_PATH_PART] == 0) {
    path2 = "/";
    path2_len = 1;
  }
  else {
    path2 = url2->s[WE_URL_PATH_PART];
    path2_len = url2->len[WE_URL_PATH_PART];
  }

  return
    (!(which_components & WE_URL_SCHEME_COMP) ||
     we_url_compare_strings (url1->s[WE_URL_SCHEME_PART],
                              url1->len[WE_URL_SCHEME_PART],
                              url2->s[WE_URL_SCHEME_PART],
                              url2->len[WE_URL_SCHEME_PART],
                              TRUE)) &&

    (!(which_components & WE_URL_USERINFO_COMP) ||
     we_url_compare_strings (url1->s[WE_URL_USERINFO_PART],
                              url1->len[WE_URL_USERINFO_PART],
                              url2->s[WE_URL_USERINFO_PART],
                              url2->len[WE_URL_USERINFO_PART],
                              TRUE)) &&

    (!(which_components & WE_URL_HOST_COMP) ||
     we_url_compare_strings (url1->s[WE_URL_HOST_PART],
                              url1->len[WE_URL_HOST_PART],
                              url2->s[WE_URL_HOST_PART],
                              url2->len[WE_URL_HOST_PART],
                              TRUE)) &&

    (!(which_components & WE_URL_PORT_COMP) || (port1 == port2)) &&

    (!(which_components & WE_URL_PATH_COMP) ||
     we_url_compare_strings (path1, path1_len, path2, path2_len, FALSE)) &&

    (!(which_components & WE_URL_QUERY_COMP) ||
     we_url_compare_strings (url1->s[WE_URL_QUERY_PART],
                              url1->len[WE_URL_QUERY_PART],
                              url2->s[WE_URL_QUERY_PART],
                              url2->len[WE_URL_QUERY_PART],
                              FALSE)) &&

    (!(which_components & WE_URL_FRAG_COMP) ||
     we_url_compare_strings (url1->s[WE_URL_FRAGMENT_PART],
                              url1->len[WE_URL_FRAGMENT_PART],
                              url2->s[WE_URL_FRAGMENT_PART],
                              url2->len[WE_URL_FRAGMENT_PART],
                              FALSE));
}

/*
 * Return TRUE if the two URLs are equal, FALSE otherwise.
 * "which_components" is a bitmap indicating which parts of the URLs
 * should be included in the comparison.
 * Returns FALSE in case of error.
 */
int
we_url_equal (const char *bs1, const char *bs2, int which_components)
{
  we_url_t url1, url2;

  if (bs1 == NULL)
    return (bs2 == NULL);
  if (bs2 == NULL)
    return FALSE;

  return
    we_url_parse (bs1, &url1) &&
    we_url_parse (bs2, &url2) &&
    we_url_equal_internal (&url1, &url2, which_components);
}

/*
 * Return TRUE if the given string URL has a valid format, FALSE otherwise.
 */
int
we_url_is_valid (const char* url)
{
  we_url_t url_st;

  return
    we_url_parse (url, &url_st) &&
    we_url_check_scheme (&url_st) &&
    we_url_check_authority (&url_st) &&
    we_url_check_path (&url_st) &&
    we_url_check_query (&url_st) &&
    we_url_check_fragment (&url_st);
}


/*
 * Retrieval of the different parts of a URL.
 */
static char *
we_url_get_part (WE_UINT8 module, const char* url, int which_part)
{
  we_url_t  urlst;
  char      *part = NULL;

  if ((url != NULL) &&
      we_url_parse (url, &urlst) &&
      (urlst.s[which_part] != NULL)) {
    part = we_cmmn_strndup (module, urlst.s[which_part], urlst.len[which_part]);
  }

  return part;
}

/*
 * Extract the scheme of a URL.
 * Returns NULL in case of error, including that the URL is not valid,
 * or if the URL does not have a scheme component.
 * NOTE: it is the responsibility of the caller to deallocate
 * the returned string.
 */
char *
we_url_get_scheme (WE_UINT8 module, const char* url)
{
  return we_url_get_part (module, url, WE_URL_SCHEME_PART);
}

/*
 * Extract the scheme type of a URL.
 * Returns scheme_empty in case of error, including that the URL is not valid,
 * or if the URL does not have a scheme component.
 */
we_scheme_t
we_url_get_scheme_type (const char* url)
{
  we_url_t urlst;

  if (url == NULL)
    return we_scheme_empty;

  if (!we_url_parse (url, &urlst) ||
      !we_url_check_scheme (&urlst))
    return we_scheme_empty;

  return urlst.scheme_type;
}


/*
 * Extract the host part of a URL.
 * Returns NULL in case of error, including that the URL is not valid,
 * or if the ULR does not have a host part.
 * NOTE: it is the responsibility of the caller to deallocate
 * the returned string.
 */
char *
we_url_get_host (WE_UINT8 module, const char* url)
{
  return we_url_get_part (module, url, WE_URL_HOST_PART);
}

/*
 * Extract the port number of a URL.
 * Returns NULL in case of error, including that the URL is not valid,
 * or if the URL does not have a port number.
 * NOTE: it is the responsibility of the caller to deallocate
 * the returned string.
 */
char *
we_url_get_port (WE_UINT8 module, const char* url)
{
  return we_url_get_part (module, url, WE_URL_PORT_PART);
}

/*
 * Extract the host part and port number of a URL.
 * Returns NULL in case of error, including that the URL is not valid,
 * or if the URL does not have a host part.
 * NOTE: it is the responsibility of the caller to deallocate
 * the returned string.
 */
char *
we_url_get_hostport (WE_UINT8 module, const char* url)
{
  we_url_t  urlst;
  char      *part = NULL;

  if ((url != NULL) &&
      we_url_parse (url, &urlst) &&
      (urlst.s[WE_URL_HOST_PART] != NULL)) {
    int len = urlst.len[WE_URL_HOST_PART];

    if (urlst.s[WE_URL_PORT_PART] != NULL)
      len += 1 + urlst.len[WE_URL_PORT_PART];

    part = WE_MEM_ALLOC (module, len + 1);
    strncpy (part, urlst.s[WE_URL_HOST_PART], len);
    part[len] = '\0';

    if (urlst.s[WE_URL_PORT_PART] != NULL) {
      char *p = part + urlst.len[WE_URL_HOST_PART];
      *p++ = ':';
      strncpy (p, urlst.s[WE_URL_PORT_PART], urlst.len[WE_URL_PORT_PART]);
      p[urlst.len[WE_URL_PORT_PART]] = '\0';
    }
  }

  return part;
}

/*
 * Return the path component of a URL.
 * Returns NULL in case of error, or if the URL does not have a path component.
 * NOTE: it is the responsibility of the caller to deallocate the string.
 */
static char*
we_url_get_path_internal (WE_UINT8 module, we_url_t *urlp)
{
  const char *q;
  char       *bs, *p;
  int         i, len;
  int         inparam;

  if ((urlp == NULL) || (urlp->len[WE_URL_PATH_PART] == 0)) {
    return NULL;
  }

  q = urlp->s[WE_URL_PATH_PART];
  len = 0;
  inparam = 0;
  for (i = 0; i < urlp->len[WE_URL_PATH_PART]; i++) {
    if (inparam && (q[i] == '/')) {
      inparam = 0;
    }
    else if (!inparam && (q[i] == ';')) {
      inparam = 1;
    }
    if (!inparam) {
      len++;
    }
  }

  bs = WE_MEM_ALLOC (module, len + 1);
  if (bs == NULL)
    return NULL;

  p = bs;
  q = urlp->s[WE_URL_PATH_PART];
  inparam = 0;
  for (i = 0; i < urlp->len[WE_URL_PATH_PART]; i++) {
    if (inparam && (q[i] == '/')) {
      inparam = 0;
    }
    else if (!inparam && (q[i] == ';')) {
      inparam = 1;
    }
    if (!inparam) {
      *p++ = q[i];
    }
  }
  *p = '\0';

  return bs;
}

/*
 * Extract the path component of a URL.
 * Returns NULL in case of error, including that the URL is not valid,
 * or if the URL does not have a path component.
 * NOTE: it is the caller's responsibility to deallocate the returned string.
 */
char *
we_url_get_path (WE_UINT8 module, const char* url)
{
  we_url_t urlst;

  if (url == NULL)
    return NULL;

  if (!we_url_parse (url, &urlst)) {
    return NULL;
  }

  return we_url_get_path_internal (module, &urlst);
}

/*
 * Return the parameter component of a URL.
 * Returns NULL in case of error, or if the URL does not have any parameters.
 * NOTE: it is the responsibility of the caller to deallocate the string.
 */
static char *
we_url_get_parameters_internal (WE_UINT8 module, we_url_t *urlp)
{
  int         i;
  int         len, plen;
  const char *p;
  char       *bs;

  if ((urlp == NULL) || (urlp->len[WE_URL_PATH_PART] == 0)) {
    return NULL;
  }

  len = urlp->len[WE_URL_PATH_PART];
  p = urlp->s[WE_URL_PATH_PART];

  /* Find the right-most '/', if any. */
  for (i = len - 1; (i >= 0) && (p[i] != '/'); i--);
  i++;

  /* Find the first ';' after the last '/', if any. */
  for (; (i < len) && (p[i] != ';'); i++);
  if (i == len) {
    /* The last path segment has no parameter part */
    return NULL;
  }
  p += i + 1;
  plen = len - i - 1;
  if ((bs = WE_MEM_ALLOC (module, plen + 1)) == NULL) {
    return NULL;
  }
  memcpy (bs, p, plen);
  bs[plen] = '\0';

  return bs;
}

/*
 * Extract the parameter component of a URL.
 * Returns NULL in case of error, including that the URL is not valid,
 * or if the URL does not have any parameters.
 * NOTE: it is the responsibility of the caller to deallocate
 * the returned string.
 */
char *
we_url_get_parameters (WE_UINT8 module, const char* url)
{
  we_url_t  urlst;
  char      *param = NULL;

  if ((url != NULL) &&
      we_url_parse (url, &urlst)) {
    param = we_url_get_parameters_internal (module, &urlst);
  }

  return param;
}

/*
 * Extract the query part of a URL.
 * Returns NULL in case of error, including that the URL is not valid,
 * or if the URL does not have a query part.
 * NOTE: it is the responsibility of the caller to deallocate
 * the returned string.
 */
char *
we_url_get_query (WE_UINT8 module, const char* url)
{
  return we_url_get_part (module, url, WE_URL_QUERY_PART);
}

/*
 * Extract the fragment part of a URL.
 * Returns NULL in case of error, including that the URL is not valid,
 * or if the URL does not have a fragment part.
 * NOTE: it is the responsibility of the caller to deallocate
 * the returned string.
 */
char *
we_url_get_fragment (WE_UINT8 module, const char* url)
{
  return we_url_get_part (module, url, WE_URL_FRAGMENT_PART);
}


/*
 * Return a pointer to where the path part of the URL begins.
 * NOTE: returns a pointer into the original string, i.e.,
 * does not allocate any new string.
 */
const char *
we_url_find_path (const char* url)
{
  we_url_t   urlst;
  const char *path = NULL;

  if ((url != NULL) &&
      we_url_parse (url, &urlst)) {
    path = urlst.s[WE_URL_PATH_PART];
  }

  return path;
}

/*
 * Return a copy of 'url' with the fragment part (if any)
 * removed. Also sets '*fragment' to point to a copy of
 * the fragment part of 'url', or NULL if there is no fragment part.
 * NOTE: it is the responsibility of the caller to deallocate
 * the returned string, as well as the fragment string.
 */
char *
we_url_drop_fragment (WE_UINT8 module, const char *url, char **fragment)
{
  we_url_t  urlst;
  char      *new_url = NULL;

  if ((url != NULL) &&
      we_url_parse (url, &urlst)) {
    if (fragment != NULL) {
      if (urlst.s[WE_URL_FRAGMENT_PART] != NULL)
        *fragment = we_cmmn_strdup (module, urlst.s[WE_URL_FRAGMENT_PART]);
      else
        *fragment = NULL;
    }

    urlst.s[WE_URL_FRAGMENT_PART] = NULL;
    urlst.len[WE_URL_FRAGMENT_PART] = 0;
    new_url = we_url_assemble (module, &urlst);
  }

  return new_url;
}

/*
 * Return a copy of 'url' with the query part (if any) removed. 
 * NOTE: it is the responsibility of the caller to deallocate
 * the returned string.
 */
char *
we_url_drop_query (WE_UINT8 module, const char *url)
{
  we_url_t  urlst;
  char      *new_url = NULL;

  if ((url != NULL) &&
      we_url_parse (url, &urlst)) {

    urlst.s[WE_URL_QUERY_PART] = NULL;
    urlst.len[WE_URL_QUERY_PART] = 0;
    new_url = we_url_assemble (module, &urlst);
  }

  return new_url;
}

/*
 * Return a copy of 'url' with 'fragment' added as the fragment part.
 * If 'url' already has a fragment part, it is replaced with
 * 'fragment'.
 * NOTE: it is the responsibility of the caller to deallocate
 * the returned string.
 */
char *
we_url_add_fragment (WE_UINT8 module, const char *url, const char *fragment)
{
  we_url_t  urlst;
  char      *new_url = NULL;

  if ((url != NULL) &&
      we_url_parse (url, &urlst)) {
    urlst.s[WE_URL_FRAGMENT_PART] = fragment;
    urlst.len[WE_URL_FRAGMENT_PART] = (fragment != NULL) ? strlen (fragment) : 0;

    new_url = we_url_assemble (module, &urlst);
  }

  return new_url;
}


/*
 * Other utility routines
 */

/*
 * Return a copy of 'pbString' where each character belonging to the set of
 * so called "special characters" or being in the range 0x80-0xff,
 * has been replaced by a hexadecimal esacape sequence of the form "%xy".
 * Returns NULL in case of error.
 * NOTE: it is the callers responsibility to deallocate the returned string.
 */
char*
we_url_escape_string (WE_UINT8 module, const char* ps)
{
  const char *p;
  char       *q, *s;
  int         l = 0;
  int         r = 0;

  if (ps == NULL)
    return NULL;

  for (p = ps; *p; p++) {
    if (ct_isspecial (*p) || (*p & 0x80))
      r++;
    else
      l++;
  }

  s = WE_MEM_ALLOC (module, l + 3 * r + 1);

  for (p = ps, q = s; *p; p++) {
    if (ct_isspecial (*p) || (*p & 0x80)) {
      *q++ = '%';
      we_cmmn_byte2hex (*p, q);
      q += 2;
    }
    else
      *q++ = *p;
  }
  *q = '\0';

  return s;
}

/*
 * Return a copy of 's' where each non-ASCII character
 * has been replaced by a hexadecimal esacape sequence of the form "%xy".
 * NOTE: it is the callers responsibility to deallocate the returned string.
 */
char *
we_url_escape_non_ascii (WE_UINT8 module, const char* s)
{
  const char *p;
  char       *q, *new_s;
  int         l = 0;
  int         r = 0;

  if (s == NULL)
    return NULL;

  for (p = s; *p; p++) {
    if (*p & 0x80)
      r++;
    else
      l++;
  }

  new_s = WE_MEM_ALLOC (module, l + 3 * r + 1);

  for (p = s, q = new_s; *p; p++) {
    if (*p & 0x80) {
      *q++ = '%';
      we_cmmn_byte2hex (*p, q);
      q += 2;
    }
    else
      *q++ = *p;
  }
  *q = '\0';

  return new_s;
}

/*
 * Return a copy of 's' where each character belonging to the set of
 * so called "special characters" or being in the range 0x80-0xff,
 * has been replaced by a hexadecimal esacape sequence of the form "%xy".
 * Returns NULL if 's' contains characters whose Unicode representation
 * is greater than 0xff.
 * NOTE: it is the callers responsibility to deallocate the returned string.
 */
char *
we_url_wmls_escape_string (WE_UINT8 module, const char* ps)
{
  const char    *p;
  char          *q, *s;
  int            n = 0;
  unsigned char  b;

  if (ps == NULL)
    return NULL;

  /* We should not allow characters > 0xff. Now, in UTF-8, the characters
   * in the range 0x80-0xff are represented by two bytes each:
   * from <0xc2,0x80> to <0xc3,0xbf>. Hence, the condition becomes
   * to test that no byte has a value > 0xc3. */
  for (p = ps; *p; p++) {
    if (((unsigned char)*p) > 0xc3)
      return NULL;
    if (ct_isspecial (*p)) {
      n += 3;
    }
    else if (*p & 0x80) {
      n += 3;
      p++;
    }
    else {
      n++;
    }
  }

  s = WE_MEM_ALLOC (module, n + 1);

  for (p = ps, q = s; *p; p++) {
    if (ct_isspecial (*p)) {
      *q++ = '%';
      we_cmmn_byte2hex (*p, q);
      q += 2;
    }
    else if (*p & 0x80) {
      b = (unsigned char)((*p++ << 6) & 0xff);
      b |= (unsigned char)(*p & 0x3f);
      *q++ = '%';
      we_cmmn_byte2hex (b, q);
      q += 2;
    }
    else
      *q++ = *p;
  }
  *q = '\0';

  return s;
}

/*
 * Return a copy of 'pbString' where each hexadecimal escape sequence
 * of the form "%xy" has been replaced with the character it represents.
 * Returns NULL in case of error.
 * NOTE: it is the callers responsibility to deallocate the returned string.
 */
char *
we_url_unescape_string (WE_UINT8 module, const char* ps)
{
  char *s = NULL;

  if ((ps != NULL) &&
      ((s = WE_MEM_ALLOC (module, strlen (ps) + 1)) != NULL)) {
    we_url_unescape_string_in_place (s, ps);
  }

  return s;
}

/*
 * Return a copy of 'pbString' where each hexadecimal escape sequence
 * of the form "%xy" has been replaced with the character it represents.
 * Returns NULL if 'ps' contains characters that are not part of
 * the US-ASCII character set.
 * NOTE: it is the callers responsibility to deallocate the returned string.
 */
char *
we_url_wmls_unescape_string (WE_UINT8 module, const char* s)
{
  char          *tmp = NULL, *dst;
  const char    *src;
  unsigned char  b;

  if (s != NULL) {
    tmp = WE_MEM_ALLOC (module, strlen (s) + 1);

    src = s;
    dst = tmp;
    while (*src) {
      if (!ct_isascii (*src)) {
        WE_MEM_FREE (module, tmp);
        return NULL;
      }
      if ((*src == '%') && we_cmmn_hex2byte (src + 1, &b)) {
        if (b & 0x80) {
          *dst++ = (char)(0xc0 | (b >> 6));
          *dst++ = (char)(0x80 | (b & 0x3f));
        }
        else {
          *dst++ = (char)b;
        }
        src += 3;
      }
      else
        *dst++ = *src++;
    }
    *dst = '\0';
  }

  return tmp;
}

/*
 * Copy the string "src" to the string "dst", while replacing
 * all escape sequences with the characters they represent.
 * Works correctly even if called with the same argument for src and dst.
 */
void
we_url_unescape_string_in_place (char *dst, const char *src)
{
  char b;

  if ((src == NULL) || (dst == NULL))
    return;

  while (*src) {
    if ((*src == '%') && we_cmmn_hex2byte (src + 1, (unsigned char *)&b)) {
      *dst++ = b;
      src += 3;
    }
    else
      *dst++ = *src++;
  }
  *dst = '\0';
}

/*
 * Return a copy of 'pbString' where each blank character
 * has been replaced by a hexadecimal esacape sequence of the form "%xy".
 * Returns NULL in case of error.
 * NOTE: it is the callers responsibility to deallocate the returned string.
 */
char *
we_url_escape_blanks (WE_UINT8 module, const char* ps)
{
  const char *p;
  char       *q, *s;
  int         l = 0;
  int         r = 0;

  if (ps == NULL)
    return NULL;

  for (p = ps; *p; p++) {
    if (ct_isblank (*p))
      r++;
    else
      l++;
  }

  s = WE_MEM_ALLOC (module, l + 3 * r + 1);

  for (p = ps, q = s; *p; p++) {
    if (ct_isblank (*p)) {
      *q++ = '%';
      we_cmmn_byte2hex (*p, q);
      q += 2;
    }
    else
      *q++ = *p;
  }
  *q = '\0';

  return s;
}

/**********************************************************************
 * Routines for access control.
 **********************************************************************/

/*
 * Check that the string 's1' is a componentwise suffix of string 's2'.
 * That is, if s1 = "def.com" and s2 = "abc.def.com", then return TRUE,
 * but if s1 = "c.def.com" return FALSE.
 */
static int
we_url_domain_is_suffix (const char *s1, int len1, const char *s2, int len2)
{
  int i1, i2, k1, k2;
  int l1, l2;

  if (len1 == 0)
    return TRUE;

  if (len2 == 0)
    return FALSE;

  for (k1 = len1, k2 = len2; (k1 > 0) && (k2 > 0);) {
    /* Search backwards for next '.' */
    for (i1 = k1 - 1; (i1 >= 0) && (s1[i1] != '.'); i1--);
    for (i2 = k2 - 1; (i2 >= 0) && (s2[i2] != '.'); i2--);
    l1 = k1 - i1 - 1;
    l2 = k2 - i2 - 1;
    if (!we_url_compare_strings (&s1[i1 + 1], l1, &s2[i2 + 1], l2, TRUE))
      return FALSE;
    k1 = i1;
    k2 = i2;
  }

  return (k1 <= 0);
}

/*
 * Check that the string 'a' is a componentwise prefix of string 'b'.
 * That is, if a = "/abc/def" and b = "/abc/def/jkl", then return TRUE,
 * but if a = "/abc/def/j" return FALSE.
 */
static int
we_url_path_is_prefix (const char *s1, int len1, const char *s2, int len2)
{
  int i1, i2, k1, k2;
  int l1, l2;

  /* Handle special cases first: */
  if (len1 == 0)
    return TRUE;

  if (len2 == 0)
    return FALSE;

  if ((s1[0] != '/') || (s2[0] != '/'))
    return FALSE;

  /* General case: */
  for (k1 = 0, k2 = 0; (k1 < len1) && (k2 < len2);) {
    /* Search forward for next '/' */
    for (i1 = k1 + 1; (i1 < len1) && (s1[i1] != '/'); i1++);
    for (i2 = k2 + 1; (i2 < len2) && (s2[i2] != '/'); i2++);
    l1 = i1 - k1 - 1;
    l2 = i2 - k2 - 1;
    if ((l1 > 0) &&
        !we_url_compare_strings (&s1[k1 + 1], l1, &s2[k2 + 1], l2, FALSE))
      return FALSE;
    k1 = i1;
    k2 = i2;
  }

  return (k1 >= len1);
}

/*
 * Check if the given absolute URL has access rights,
 * given 'domain' and 'path' as access control attributes.
 *
 * Assumes that all three strings are zero-terminated char strings,
 * and that 'abs_url' is a valid absolute URL.
 * Returns FALSE if either 'abs_url' or 'domain' is NULL.
 * If 'path' is NULL, the routine works as if it were the path "/".
 *
 * The check is performed by verifying that the domain attribute
 * is a component-wise suffix of the domain part of the absolute URL,
 * and that the path attribute is a component-wise prefix of
 * the path part of the absolute URL. Comparisons of the paths
 * are case-sensitive, but comparisons of the domain components are not.
 * Handles escape sequences ("%xy") correctly.
 */
int
we_url_check_access (char *abs_url, char *domain, char *path)
{
  we_url_t url;

  if ((abs_url == NULL) || (domain == NULL))
    return FALSE;

  if (path == NULL)
    path = "/";

  if (!we_url_parse (abs_url, &url))
    return FALSE;

  return
    (url.s[WE_URL_HOST_PART] != NULL) &&
    (url.len[WE_URL_PATH_PART] > 0) &&
    we_url_domain_is_suffix (domain, strlen (domain),
                              url.s[WE_URL_HOST_PART], url.len[WE_URL_HOST_PART]) &&
    we_url_path_is_prefix (path, strlen (path),
                            url.s[WE_URL_PATH_PART], url.len[WE_URL_PATH_PART]);
}

/*
 * Take an incomplete URL, like "www.abc.com", and turn it into
 * a correct absolute URL using heuristic methods. This is not a
 * well-defined process, rather it makes a guess as to what the user msans.
 * In the example above, the result would be "http://www.abc.com/".
 * In case of failure, NULL is returned.
 * NOTE: it is the caller's responsibility to deallocate the returned string.
 */
char *
we_url_make_complete (WE_UINT8 module, char *bs)
{
  we_url_t  url, new_url;
  char      *p, *ns;

  if ((bs == NULL) || !we_url_parse (bs, &url)) {
    return NULL;
  }
  new_url = url;

  /* The original URL has a scheme different from http(s). */
  if ((url.s[WE_URL_SCHEME_PART] != NULL) &&
      (url.scheme_type != we_scheme_http) &&
      (url.scheme_type != we_scheme_https)) {
    /* It might be that what we think is the scheme is really
     * the host followed by a port number. Consider for example
     * this case:  www.abc.com:8080/some/path
     * We use the following heuristic: if the scheme part
     * contains one or more '.'-characters, and if the part
     * immediately following the scheme is an integer,
     * then we assume that what we really have is a host and port.
     */
    char *s = we_cmmn_strnchr (url.s[WE_URL_PATH_PART], '/',
                                url.len[WE_URL_PATH_PART]);
    int plen;

    if (s != NULL)
      plen = s - url.s[WE_URL_PATH_PART];
    else
      plen = url.len[WE_URL_PATH_PART];

    if ((we_cmmn_strnchr (url.s[WE_URL_SCHEME_PART], '.',
                           url.len[WE_URL_SCHEME_PART]) != NULL) &&
        (url.s[WE_URL_PATH_PART] != NULL) && (plen > 0) &&
        we_url_is_number (url.s[WE_URL_PATH_PART], plen)) {
      new_url.s[WE_URL_SCHEME_PART] = "http";
      new_url.len[WE_URL_SCHEME_PART] = 4;
      new_url.scheme_type = we_scheme_http;

      new_url.s[WE_URL_AUTHORITY_PART] = new_url.s[WE_URL_HOST_PART] = bs;
      new_url.len[WE_URL_AUTHORITY_PART] = new_url.len[WE_URL_HOST_PART]  =
        url.len[WE_URL_SCHEME_PART];

      new_url.s[WE_URL_PORT_PART] = url.s[WE_URL_PATH_PART];
      new_url.len[WE_URL_PORT_PART] = plen;
          
      new_url.s[WE_URL_PATH_PART] = s;
      new_url.len[WE_URL_PATH_PART] = url.len[WE_URL_PATH_PART] - plen;
    }
    else
      /* We have to assume that it is an absolute URL with an unknown scheme */
      return we_cmmn_strdup (module, bs);
  }

  /* The original URL does not have a scheme; we attach 'http:' */
  if (url.s[WE_URL_SCHEME_PART] == NULL) {
    new_url.s[WE_URL_SCHEME_PART] = "http";
    new_url.len[WE_URL_SCHEME_PART] = 4;
    new_url.scheme_type = we_scheme_http;
  }

  /* The original URL does not have an authority part (starting with '//');
   * we promote the initial segment of the path, up to (but not including)
   * the first '/'-character, or the whole path in case it has no such
   * delimiter. If the path is an absolute path (starting with '/'),
   * we cannot guess what the host should have been. */
  if (new_url.s[WE_URL_AUTHORITY_PART] == NULL) {
    if (url.len[WE_URL_PATH_PART] == 0) {
      return NULL;
    }

    p = we_cmmn_strnchr (url.s[WE_URL_PATH_PART], '/', url.len[WE_URL_PATH_PART]);
    if (p == NULL) {
      /* There is no '/' character: make the whole string be the host part. */
      new_url.s[WE_URL_AUTHORITY_PART] = new_url.s[WE_URL_HOST_PART] =
        url.s[WE_URL_PATH_PART];
      new_url.len[WE_URL_AUTHORITY_PART] = new_url.len[WE_URL_HOST_PART]
        = url.len[WE_URL_PATH_PART];
      new_url.len[WE_URL_PATH_PART] = 0;
    }
    else if (p != url.s[WE_URL_PATH_PART]) {
      /* String does not start with '/': let prefix be host part. */
      new_url.s[WE_URL_AUTHORITY_PART] = new_url.s[WE_URL_HOST_PART] =
        url.s[WE_URL_PATH_PART];
      new_url.len[WE_URL_AUTHORITY_PART] = new_url.len[WE_URL_HOST_PART] =
        (p - url.s[WE_URL_PATH_PART]);
      new_url.s[WE_URL_PATH_PART] = p;
      new_url.len[WE_URL_PATH_PART] = url.len[WE_URL_PATH_PART] -
        new_url.len[WE_URL_HOST_PART];
    }
    else {
      /* The URL is simply an absolute path, we cannot deduce the host! */
      return NULL;
    }
  }
  
  if (new_url.len[WE_URL_PATH_PART] == 0) {
    /* The URL (original or modified as above), which is now a http(s) URL,
     * has no path part; we attach '/', the root path. */
    new_url.s[WE_URL_PATH_PART] = "/";
    new_url.len[WE_URL_PATH_PART] = 1;
  }

  /* Allocate a new buffer and copy all the parts to it. */
  ns = we_url_assemble (module, &new_url);

  return ns;
}

/*
 * Given two URLs, compute the minimum relative path, i.e., the shortest
 * relative path that together with 'bs1' can be resolved to yield
 * 'bs2'. If 'include_fragment' is TRUE any fragment part in 'bs2'
 * is included in the result, otherwise it is dropped.
 * Returns NULL in case of error.
 * NOTE: it is the caller's responsibility to deallocate the returned string.
 */
char *
we_url_min_relative_path (WE_UINT8 module, char *bs1, char *bs2, int include_fragment)
{
  we_url_t   url1, url2;
  const char *s1, *s2;
  char       *buf, *p, *res;
  int         len, len1, len2, l1, l2, n;
  int         i, i1, i2, k1, k2;

  if (!we_url_parse (bs1, &url1) ||
      !we_url_parse (bs2, &url2))
    return NULL;
  
  if (!we_url_equal_internal (&url1, &url2,
                               WE_URL_SCHEME_COMP | WE_URL_HOST_COMP |
                               WE_URL_PORT_COMP)) {
    return we_url_to_string (module, &url2);
  }
  s1 = url1.s[WE_URL_PATH_PART];
  s2 = url2.s[WE_URL_PATH_PART];
  if ((s1 == NULL) || (s2 == NULL))
    return NULL;

  len1 = url1.len[WE_URL_PATH_PART];
  len2 = url2.len[WE_URL_PATH_PART];
  if ((len1 == 0) || (len2 == 0))
    return NULL;
  if ((s1[0] != '/') || (s2[0] != '/'))
    return NULL;

  /* Drop everything after the last '/' in path 1. */
  for (i1 = len1 - 1; i1 > 0; i1--) {
    if (s1[i1] == '/')
      break;
  }
  len1 = i1 + 1;

  /* Now, path 1 ends with a '/'. Search past the initial, equal,
   * path segments in the two paths. A path segment starts right
   * after a '/' and ends on the next '/'. */
  for (k1 = 1, k2 = 1; (k1 < len1) && (k2 < len2);) {
    /* Here, k1 is the index following the last seen '/',
     * and i1 is moved  up to the next '/', or the end of the string. */
    for (i1 = k1; (i1 < len1) && (s1[i1] != '/'); i1++);
    for (i2 = k2; (i2 < len2) && (s2[i2] != '/'); i2++);
    l1 = i1 - k1;
    l2 = i2 - k2;
    if (!((l1 == l2) &&
          we_url_compare_strings (&s1[k1], l1, &s2[k2], l2, FALSE))) {
      break;
    }
    k1 = i1 + 1;
    k2 = i2 + 1;
  }

  /* Count how many more '/' there are in path 1. */
  n = 0;
  for (i1 = k1; i1 < len1; i1++) {
    if (s1[i1] == '/')
      n++;
  }

  /* The path we create will consist of what ever part was left
   * in path 2 (i.e., that did not match a corresponding part in
   * path 1), plus n "../" segments. Note, that we might end up
   * with a string of length 0 here.
   * Special case: if the paths differ already in the first
   * segment, then just use the absolute path from url2. */
  if (k1 == 1 || k2 >= len2) {
    len = len2;
    p = buf = WE_MEM_ALLOC (module, len + 1);
    memcpy (p, s2, len2);
    p += len2;
  }
  else {
    len = n * 3 + len2 - k2;
    p = buf = WE_MEM_ALLOC (module, len + 1);
    for (i = 0; i < n; i++) {
      memcpy (p, "../", 3);
      p += 3;
    }
    if (len2 > k2) {
      memcpy (p, &s2[k2], len2 - k2);
      p += len2 - k2;
    }
  }
  *p = '\0';

  url2.s[WE_URL_PATH_PART] = buf;
  url2.len[WE_URL_PATH_PART] = len;
  url2.s[WE_URL_AUTHORITY_PART] = url2.s[WE_URL_SCHEME_PART]
    = url2.s[WE_URL_HOST_PART] = url2.s[WE_URL_PORT_PART] = NULL;
  url2.len[WE_URL_AUTHORITY_PART] = url2.len[WE_URL_SCHEME_PART]
    = url2.len[WE_URL_HOST_PART] = url2.len[WE_URL_PORT_PART] = 0;
  url2.scheme_type = we_scheme_empty;

  if (!include_fragment) {
    url2.s[WE_URL_FRAGMENT_PART] = NULL;
    url2.len[WE_URL_FRAGMENT_PART] = 0;
  }

  res = we_url_assemble (module, &url2);
  WE_MEM_FREE (module, buf);

  return res;
}

/*
 * Check that a URL can be parsed and has at least a scheme, host, and path
 * component.
 */
int
we_url_is_complete (const char* url)
{
  we_url_t urlst;

  return
    we_url_parse (url, &urlst) &&
    (urlst.s[WE_URL_SCHEME_PART] != NULL) &&
    (urlst.s[WE_URL_HOST_PART] != NULL) &&
    (urlst.len[WE_URL_PATH_PART] > 0);
}

/*
 * Return a URL that is a copy of 'old_url', but with 'new_query'
 * appended to the query part. If 'old_url' already has a query part,
 * that old part will be separated from the new by a '&'-character.
 * Returns NULL in case of error.
 * NOTE: it is the responsibility of the caller to deallocate
 * the returned string.
 */
char *
we_url_append_to_query (WE_UINT8 module, char *old_url, char *new_query)
{
  we_url_t  url;
  int    nqlen;
  char  *new_url, *tmp = NULL;

  if (old_url == NULL)
    return NULL;

  if ((new_query == NULL) || ((nqlen = strlen (new_query)) == 0))
    return we_cmmn_strdup (module, old_url);

  if (!we_url_parse (old_url, &url))
    return NULL;

  if (url.s[WE_URL_QUERY_PART] == NULL) {
    url.s[WE_URL_QUERY_PART] = new_query;
    url.len[WE_URL_QUERY_PART] = nqlen;
  }
  else {
    tmp = WE_MEM_ALLOC (module, url.len[WE_URL_QUERY_PART] + nqlen + 2);
    memcpy (tmp, url.s[WE_URL_QUERY_PART], url.len[WE_URL_QUERY_PART]);
    tmp[url.len[WE_URL_QUERY_PART]] = '&';
    memcpy (tmp + url.len[WE_URL_QUERY_PART] + 1, new_query, nqlen);
    url.s[WE_URL_QUERY_PART] = tmp;
    url.len[WE_URL_QUERY_PART] += nqlen + 1;
  }

  new_url = we_url_assemble (module, &url);
  if (tmp != NULL)
    WE_MEM_FREE (module, tmp);

  return new_url;
}

/*
 * Return the longest componentwise common prefix of two URL paths.
 * Returns NULL in case of error.
 * NOTE: it is the responsibility of the caller to deallocate
 * the returned string.
 */
char *
we_url_longest_common_prefix (WE_UINT8 module, char *s1, char *s2)
{
  int   len1, len2, l1, l2;
  int   i1, i2, k1, k2;
  char *p;

  if ((s1 == NULL) || (s2 == NULL))
    return NULL;

  len1 = strlen (s1);
  len2 = strlen (s2);
  if ((len1 == 0) || (len2 == 0))
    return NULL;
  if ((s1[0] != '/') || (s2[0] != '/'))
    return NULL;

  for (k1 = 0, k2 = 0; (k1 < len1) && (k2 < len2);) {
    /* Search forward for next '/' */
    for (i1 = k1 + 1; (i1 < len1) && (s1[i1] != '/'); i1++);
    for (i2 = k2 + 1; (i2 < len2) && (s2[i2] != '/'); i2++);
    l1 = i1 - k1 - 1;
    l2 = i2 - k2 - 1;
    if (!we_url_compare_strings (&s1[k1 + 1], l1, &s2[k2 + 1], l2, FALSE))
      break;
    k1 = i1;
    k2 = i2;
  }
  if (k1 == 0)
    k1 = 1;

  p = WE_MEM_ALLOC (module, k1 + 1);
  memcpy (p, s1, k1);
  p[k1] = '\0';

  return p;
}

/*
 * Check if the URL "path1" is a prefix of "path2".
 * The prefix match is done according to the rules in section 7.4
 * in "WAP Cache Operation Specification".
 */
int
we_url_is_prefix (char *path1, char *path2)
{
  we_url_t url1, url2;

  if (!we_url_parse (path1, &url1) ||
      !we_url_parse (path2, &url2)) {
    return FALSE;
  }
  
  if (!we_url_equal_internal (&url1, &url2, WE_URL_SCHEME_COMP))
    return FALSE;

  if ((url1.s[WE_URL_AUTHORITY_PART] == NULL) &&
      (url2.s[WE_URL_AUTHORITY_PART] == NULL) &&
      (url1.len[WE_URL_PATH_PART] > 0) &&
      (url2.len[WE_URL_PATH_PART] > 0) &&
      ((url1.s[WE_URL_PATH_PART])[0] != '/') &&
      ((url2.s[WE_URL_PATH_PART])[0] != '/')) {
    /* Both URLs are of the "opaque" type. */
    return we_url_equal_internal (&url1, &url2, WE_URL_PATH_COMP);
  }
      
  /* For non-opaque URLs, the authority parts must be equal. */
  if (!we_url_equal_internal (&url1, &url2,
                               WE_URL_USERINFO_COMP | WE_URL_HOST_COMP | WE_URL_PORT_COMP))
    return FALSE;

  /* An empty path is the prefix on any other path. */
  if (url1.len[WE_URL_PATH_PART] == 0)
    return TRUE;

  /* Check that path1 is a segment-wise prefix of path2. */
  return we_url_path_is_prefix (url1.s[WE_URL_PATH_PART], url1.len[WE_URL_PATH_PART],
                                 url2.s[WE_URL_PATH_PART], url2.len[WE_URL_PATH_PART]);
}

char *
we_url_escape_all_string (WE_UINT8 module, const char* ps, int len)
{
  const char *p;
  char       *q, *s;
  int         l = 0;
  int         r = 0;
  int         i = 0;

  if (ps == NULL || len == 0)
    return NULL;

  for (p = ps; i < len; i++, p++) {
    if (!ct_isalphanum (*p))
      r++;
    else
      l++;
  }

  s = WE_MEM_ALLOC (module, l + 3 * r + 1);
  i = 0;
  for (p = ps, q = s; i < len; p++, i++) {
    if (ct_isspecial (*p) || (*p & 0x80)) {
      *q++ = '%';
      we_cmmn_byte2hex (*p, q);
      q += 2;
    }
    else
      *q++ = *p;
  }
  *q = '\0';

  return s;
}
