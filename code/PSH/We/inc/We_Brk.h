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
 * We_Brk.h
 * 
 * Character analysis of line breaking classes per Unicode. The implementation delivered
 * does not include Unicode tables.
 *
 */
#ifndef _we_brk_h
#define _we_brk_h

/* NLF msans new line function and the below is a subset of the newline types
 * defined by unicode. If the below is updated to allow for mulitybyte NLFs (other than
 * the two character CRLF case handled below), the code using this define need to be
 * updated.*/
#define WE_BREAK_IS_NLF(n, b) ((*n == 0xD && *(n + 1) == 0xA) ? b = 1, TRUE : \
                                 *n == 0xA || *n == 0xD ? TRUE : FALSE)

/* The only glue character checked for below is non-breaking-space (&nbsp;)*/
#define WE_BREAK_IS_GLUE(c) ((unsigned char)*c == (unsigned char)0xC2 && \
                              (unsigned char)*(c + 1) == (unsigned char)0xA0)

#define WE_BREAK_PROHIB 0
#define WE_BREAK_HARD   1
#define WE_BREAK_OPPOR  2

#define WE_BREAK_TYPE(p, n, b) (WE_BREAK_IS_NLF(n, b) ? WE_BREAK_HARD : \
                                 (*p == ' ' && *n != ' ') ? WE_BREAK_OPPOR : \
                                                            WE_BREAK_PROHIB)


#endif
