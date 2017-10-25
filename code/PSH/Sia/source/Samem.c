/*
 * Copyright (C) Techfaith, 2002-2005.
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

/*! \file mmem.c.c
 * \brief  <brief description>
 */

/*--- Include files ---*/
#include "We_Lib.h"    /* WE: ANSI Standard libs allowed to use */
#include "We_Mem.h"    /* WE: Memory handling */

/*--- Definitions/Declarations ---*/

/*--- Types ---*/

/*--- Constants ---*/

/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

/*--- Global variables ---*/

/*--- Static variables ---*/

/*--- Prototypes ---*/

/******************************************************************************/

/*!
 * \brief "Safe" allocation of zero-initialized memory.
 * 
 * Allocates a memory block with all zeroes. Makes a longjump if no more memory 
 * is available so in that case will this function never return.
 *
 * \param size Size of memory area to allocate
 * \return ptr to allocated memory.
 *****************************************************************************/
void *smaMemCalloc(WE_UINT32 size)
{
    void *tmp = WE_MEM_ALLOC(WE_MODID_SIA, size);

    if (tmp != NULL)
    {
        memset(tmp, 0, size);
    } /* if */

    return tmp;
} /* smaMemCalloc */
