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
 * We_Mem.h
 *
 * Created by Anders Edenwbandt, Wed Sep 26 09:59:07 2001.
 *
 *
 * Revision history:
 *   020402, IPN: Changed to fit Mobile Suite Framework.
 *   021014, IPN: Fixed the internal memory allocator to fit WE.
 *   021014, IPN: Added an exception function parameter in we_mem_init.
 *
 */
#ifndef _we_mem_h
#define _we_mem_h

#ifndef _we_def_h
#include "We_Def.h"
#endif
#ifndef _we_cfg_h
#include "We_Cfg.h"
#endif

/************************************************************
 * Macros
 ************************************************************/

#define WE_MEM_INIT                  we_mem_init
#define WE_MEM_FREE_ALL              we_mem_free_all

#ifdef WE_LOG_MODULE

#define WE_MEM_ALLOC(wid, s)          we_mem_alloc (wid, s, __FILE__, __LINE__)
#define WE_MEM_ALLOCTYPE(wid, t) (t *)we_mem_alloc (wid, sizeof (t), __FILE__, __LINE__)
#define WE_MEM_FREE(wid, p)           we_mem_free (wid, p, __FILE__, __LINE__)

#else

#define WE_MEM_ALLOC(wid, s)          we_mem_alloc (wid, s)
#define WE_MEM_ALLOCTYPE(wid, t) (t *)we_mem_alloc (wid, sizeof (t))
#define WE_MEM_FREE(wid, p)           we_mem_free (wid, p)

#endif

#define WE_MEM_GET_SIZE              we_mem_get_size

/************************************************************
 * Typedef
 ************************************************************/

typedef void we_mem_exception_func_t (void);


/************************************************************
 * Functions
 ************************************************************/

#ifdef WE_LOG_MODULE
void *
we_mem_alloc (WE_UINT8 modId, WE_UINT32 size, const char *filename, int lineno);
#else
void *
we_mem_alloc (WE_UINT8 modId, WE_UINT32 size);
#endif

#ifdef WE_LOG_MODULE
void
we_mem_free (WE_UINT8 modId, void *p, const char *filename, int lineno);
#else
void
we_mem_free (WE_UINT8 modId, void *p);
#endif

WE_UINT32
we_mem_get_size (void *mem);

WE_UINT32
we_mem_allocated_bytes (WE_UINT8 modId);

void
we_mem_free_all (WE_UINT8 modId);

/*
 * Initialise the memory handler. The parameter exceptionHandler is
 * a pointer to an exception function. This function is called when
 * a memory allocation is failed. If the module doesn't have any
 * exception function, set exceptionHandler to NULL, which results
 * in when a memory allocation is failed NULL is returned.
 * When use_extern_alloc is FALSE, either the internal allocator or
 * or the allocator, TPIa_memAlloc, is used.
 * When use_extern_alloc is TRUE, an external allocator,
 * TPIa_memExternalAlloc, is used.
 */
void
we_mem_init (WE_UINT8 modId, we_mem_exception_func_t* exceptionHandler,
              WE_BOOL use_extern_alloc);

#endif
