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
 * We_Ess.h
 * 
 * Description: To handle customized additional WE signals.
 *    
 */


#ifndef _we_ess_h
#define _we_ess_h

#ifndef _we_def_h 
#include "We_Def.h"
#endif

/**********************************************************************
 * Exported functions
 **********************************************************************/

/* Converts customized additional WE signals.
 *   module is the wid of the receiving module .
 *   signal is the signal wid of the signal received.
 *   buffer contains the signal buffer that shall be decoded to
 *   an internal signal structure.
 * WE will call this function when an WE signal has been received which
 * is not recognized by the generic WE core.
 * WE core will delete the signal buffer after the call to this function.
 * If there are no customized signals. just let this function return NULL.
 */
void*
we_ess_convert (WE_UINT8 module, WE_UINT16 signal, void* buffer);


/* Deletes customized additional WE signals.
 *   module is the wid of the calling module .
 *   signal is the wid of the signal to be deleted.
 *   p contains the signal structure that shall be deleted
 * WE core will call this function when an arbitrary WE signal is to be deleted
 * but the generic WE core does not recognized the signal.
 * If there are no customized signals. just let this function be empty.
 */
void
we_ess_destruct (WE_UINT8 module, WE_UINT16 signal, void* p);


#endif /* _we_ess_h */
