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

/* \file mamem.h
 * Memory (de)allocation macros for MSA.
 */

#ifndef _MAMEM_H_
#define _MAMEM_H_

#ifndef _we_mem_h
#error we_mem.h must be included before mamem.h.
#endif

/******************************************************************************
 * Macros
 *****************************************************************************/

/*!\brief Allocation macro for MSA. If memory cannot be allocated then the
 *        application is going to terminate.
 *
 * \param s The number of bytes to allocate.
 * \return The pointer to the allocated memory.
 */
#define MSA_ALLOC(s)            msaAlloc(s)

/*!\brief Allocation macro for MSA. If memory cannot be allocated then the
 *        application is going to terminate. Memory is also cleared. 
 *
 * \param s The number of bytes to allocate.
 * \param p Pointer that will point to newly allocated memory
 */
#define MSA_CALLOC(p, s)            p = msaAlloc(s); memset((p), 0, (s))

/*!\brief Allocation macro for MSA. If memory cannot be allocated then the
 *        application is going to terminate.
 *
 * \param s The number of bytes to allocate.
 * \param type The type to allocate.
 * \return The pointer to the allocated memory.
 */
#define MSA_ALLOC_TYPE(type)    msaAlloc(sizeof(type))

/*!\brief Allocation macro for with termination used in MSA. 
 *
 * \param s The number of bytes to allocate.
 * \return The pointer to the allocated memory, or NULL if no memory could be 
 *         allocated.
 */
#define MSA_ALLOC_NE(s)         msaAllocNoException(s)

/*!\brief Deallocation macro for MSA 
 *
 * \param p The memory location to deallocate..
 * \return The pointer to the allocated memory, or NULL if no memory could be 
 *         allocated.
 */
#define MSA_FREE(p)       if (NULL != (p)) WE_MEM_FREE(WE_MODID_MSA, (p))

/******************************************************************************
 * Prototypes
 *****************************************************************************/

void *msaAllocNoException(WE_UINT32 size);
void *msaAlloc(WE_UINT32 size);

#endif
