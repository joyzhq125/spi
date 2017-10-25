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
 * We_Pdec.h
 *
 * XML and HTML parser: WBXML decoder
 */

#ifndef _we_pdec_h
#define _we_pdec_h

#ifndef _we_ptxt_h
#include "We_Ptxt.h"
#endif


/**************************************************************
 * Functions                                                  *
 **************************************************************/

int
we_dec_prologueDecoder (we_prsr_parser_t*  parser,
                         const char*         s,
                         const char*         end,
                         const char*        *nextPtr,
                         int                 limit);

/*
 * Common WBXML opaque data handler. Writes opaque data as normal
 * text. This msans opaque data can be be a part of any string
 * in character data callbacks.
 *
 * If WBXML opaque data is not to be ignored, then in the
 * initiation callback, set d->specific = we_dec_opaqueSpecific.
 */
/* Declared in We_Prsr.h.
int 
we_dec_opaqueSpecific (we_prsr_parser_t*  parser,
                        const char*        *s,
                        const char*         end,
                        int                 size,
                        int                 type);
*/


/*
 * Push opaque data handler. Write 7 bytes from attribute opaque
 * data and fill with zeros if indata is less than 7 bytes.
 */
int 
we_dec_pushSpecific (we_prsr_parser_t*  parser,
                      const char*        *s,
                      const char*         end,
                      int                 size,
                      int                 type);

/*
 * WML variable handler. Decodes WML extention tokens into variables.
 */ 
int 
we_dec_wmlSpecific (we_prsr_parser_t*  parser,
                     const char*        *s,
                     const char*         end,
                     int                 tok,
                     int                 isElement);



#endif
