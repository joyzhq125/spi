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

/* !\file mamem.c
 * Memory handling.
 */

/* WE */
#include "We_Mem.h"
#include "We_Lib.h"
#include "We_Core.h"

/* MMS */
#include "Mms_Def.h"

/* MSA */
#include "Msa_Mem.h"
#include "Msa_Intsig.h"
#include "Msa_Core.h"


/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief Allocates memory and performs a clean exit if allocation fails.
 *        This function will not return if memory allocation fails, instead
 *        the application will terminate.
 *
 * \param size The size in bytes to allocate.
 * \return A pointer to the allocated memory, or NULL if no exceptions is 
 *         allowed.
 *****************************************************************************/
void *msaAlloc(WE_UINT32 size)
{
    msaAllowException(TRUE);
    return WE_MEM_ALLOC(WE_MODID_MSA, size);
}

/*!
 * \brief Allocates memory for the MSA.
 *
 * \param size The size in bytes to allocate.
 * \return A pointer to the allocated memory or NULL if no memory is available.
 *****************************************************************************/
void *msaAllocNoException(WE_UINT32 size)
{
    void *mem;
    msaAllowException(FALSE);
    mem = WE_MEM_ALLOC(WE_MODID_MSA, size);
    msaAllowException(TRUE);
    return mem;
}
