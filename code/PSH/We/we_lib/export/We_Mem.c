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
 * We_Mem.c
 *
 * Created by Anders Edenwbandt, Wed Sep 26 09:33:36 2001.
 *
 *
 * Memory allocation routines, tailored for platforms
 * with scarce RAM resources.
 *
 * Revision history:
 *   010926, AED: Merged all memory allocation routines into one file.
 *   020402, IPN: Changed to fit Mobile Suite Framework.
 *   021014, IPN: Fixed the internal memory allocator to fit WE.
 *   021014, IPN: Added functionality to take care of when a module
 *                run's out of memory.
 *   021030, IPN: Added functionality to access external modules.
 *   031217, RMZ: Updated functionality to access external modules.
 *
 */

#include "We_Mem.h"
#include "We_Errc.h"
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Cmmn.h"

typedef struct {
  WE_UINT32                allocated_bytes;
  we_mem_exception_func_t* exception_func;
  WE_BOOL                  use_extern_alloc;
} we_mem_module_t;

static we_mem_module_t mem_mod[WE_NUMBER_OF_MODULES];

#ifdef WE_CONFIG_INTERNAL_MALLOC

#define CHUNKHEADERSIZE        (2 * sizeof(WE_UINT32))
#define MINCHUNKSIZE           sizeof(chunk_t)
#define MALLOC_ALIGNMENT       4
#define MALLOC_ALIGN_MASK      3
#define MINBLOCKSIZE           (12 * MINCHUNKSIZE)

typedef struct chunk_st{
  WE_UINT32        prev_size; /* Size of previous chunk.            */
  WE_UINT32        size;      /* Size in bytes, including overhead, inuse bit is also stored here. */
  struct chunk_st*  fwd;       /* Double links -- used only if free. */
  struct chunk_st*  bck;
} chunk_t;
typedef chunk_t* chunkptr;

/*
 * We use segregated free lists, with separate lists for
 * chunks of different sizes. Currently, we use sizes that are
 * powers of two.
 * In list number n is kept all the free chunks whose size is
 * strictly less than maxsizes[n].
 */
#define NUM_FREE_LISTS 10
const static WE_UINT32   maxsizes[NUM_FREE_LISTS] = {
  16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 0x8FFFFFFF
};

typedef struct {
  unsigned char *baseptr;
  chunkptr      firstchunk;
  chunkptr      lastchunk;
  chunkptr      freelists[NUM_FREE_LISTS];
} mem_internal_t;

/*
 * Global variables.
 */
static mem_internal_t mem_internal[WE_NUMBER_OF_MODULES];


/*
 * Return the number of the list that a chunk of size "n"
 * belongs to.
 */
static int
list_idx (WE_UINT32 n)
{
  int l;

  for (l = 0; maxsizes[l] <= n; l++);

  return l;
}

/*
 * Macros, rather than function calls, are used for maximum performance:
 */

/* Conversion from chunk headers to user pointers, and back */
#define chunk2mem(p)    ((void*)((unsigned char *)(p) + CHUNKHEADERSIZE))
#define mem2chunk(mem)  ((chunkptr)((unsigned char *)(mem) - CHUNKHEADERSIZE))

/* Reading and writing of the size fields.
 * The rightmost bit in the size field also
 * holds the in use bit. */

#define chunksize(p)        (((p)->size & ~0x01) >> 1)

#define set_hd1(p, v)   ((p)->prev_size = (WE_UINT32)(v))
#define set_hd2(p, v)   ((p)->size = (WE_UINT32)(v))

/* The next and previous links */
#define prevchunk(p)    ((chunkptr)(((unsigned char *)(p)) - p->prev_size))
#define nextchunk(p)    ((chunkptr)(((unsigned char *)(p)) + chunksize (p)))

/* The IN USE bit */
#define chunk_isfree(p) ((((chunkptr)(p))->size & 0x01) == 0)
#define chunk_inuse(p) ((((chunkptr)(p))->size & 0x01) == 1)


/*
 * List operations.
 */
#define remove_from_list(p) \
  (p)->fwd->bck = (p)->bck; \
  (p)->bck->fwd = (p)->fwd;


#define add_to_list(l, p) \
  (p)->fwd = (l)->fwd; \
  (p)->bck = l; \
  (l)->fwd->bck = p; \
  (l)->fwd = p;


/*
 * Compute the chunk size we will need for an allocation request:
 */
#define request2size(req) \
 (((WE_UINT32)((req) + CHUNKHEADERSIZE + MALLOC_ALIGN_MASK) < \
  (WE_UINT32)(MINCHUNKSIZE + MALLOC_ALIGN_MASK)) ? MINCHUNKSIZE : \
  (WE_UINT32)(((req) + CHUNKHEADERSIZE + MALLOC_ALIGN_MASK) & ~(MALLOC_ALIGN_MASK)))



#ifdef WE_CFG_DEBUG_INTERNAL_MALLOC
#include <assert.h>

/*
 * This routine is called upon freeing and allocation,
 * and tries to verify that all fields in a chunk are valid.
 */
static void
check_allocated_chunk (WE_UINT8 modId, chunkptr p)
{
  chunkptr nextp, prevp;
  mem_internal_t* mod_mem;

  mod_mem = &(mem_internal[modId]);

  assert (chunk_inuse (p)); /* Should be marked as "in use". */
  assert ((unsigned char *)p >= mod_mem->baseptr);    /* Pointer must be in valid range. */
  assert (p <= mod_mem->lastchunk);
  assert (((WE_UINT32)p & MALLOC_ALIGN_MASK) == 0); /* Aligned correctly? */
  assert (chunksize (p) >= MINCHUNKSIZE); /* Not too siall. */

  /* Check adjacent chunks. */
  nextp = nextchunk (p);
  assert ((unsigned char *)nextp >= mod_mem->baseptr);
  assert (nextp <= mod_mem->lastchunk);
  assert (((WE_UINT32)nextp & MALLOC_ALIGN_MASK) == 0);
  assert (chunksize (nextp) >= MINCHUNKSIZE);
  assert (chunksize (p) == nextp->prev_size); /* Consistent headers? */
  if (chunk_isfree (nextp)) {
    chunkptr q;

    /* If this chunks is free, check the free list pointers. */
    q = nextp->fwd;
    assert ((unsigned char *)q >= mod_mem->baseptr);
    assert (q <= mod_mem->lastchunk);
    q = nextp->bck;
    assert ((unsigned char *)q >= mod_mem->baseptr);
    assert (q <= mod_mem->lastchunk);
  }

  if (p > mod_mem->firstchunk) {
    prevp = prevchunk (p);
    assert ((unsigned char *)prevp >= mod_mem->baseptr);
    assert (prevp <= mod_mem->lastchunk);
    assert (((WE_UINT32)prevp & MALLOC_ALIGN_MASK) == 0);
    assert (chunksize (prevp) >= MINCHUNKSIZE);
    assert (chunksize (prevp) == p->prev_size); /* Consistent headers? */
    if (chunk_isfree (prevp)) {
      chunkptr q;

      /* If this chunks is free, check the free list pointers. */
      q = prevp->fwd;
      assert ((unsigned char *)q >= mod_mem->baseptr);
      assert (q <= mod_mem->lastchunk);
      q = prevp->bck;
      assert ((unsigned char *)q >= mod_mem->baseptr);
      assert (q <= mod_mem->lastchunk);
    }
  }
}
#endif

/*
 * Initialize the internal memory allocation liwbary.
 * The first parameter indicates which module initialise
 * the memory liwbary, the second is a pointer to a block
 * of memory and the third is the size of the memory area.
 * Returns 0 on success, and -1 otherwise.
 */
static int
we_mem_internal_init (WE_UINT8 modId, void *mem, WE_UINT32 memsize)
{
  mem_internal_t* mod_mem;
  chunkptr        p;
  WE_UINT32      size;
  WE_INT16       i;

  mod_mem = &(mem_internal[modId]);

  /* Truncate to nearest multiple of the alignment. */
  memsize &= ~MALLOC_ALIGN_MASK;
  if ((mem == NULL) || (memsize < MINBLOCKSIZE) || (memsize > 0x7FFFFFFF)) {
    mod_mem->baseptr = NULL;
    return -1;
  }
  mod_mem->baseptr = mem;

  /* For each of the lists of free chunks, we need one chunk to
   * serve as list header. We mark each one as "in use" to avoid
   * having it merged with an adjacent chunk on freeing. */
  for (i = 0; i < NUM_FREE_LISTS; i++) {
    p = (chunkptr)(mod_mem->baseptr + i * MINCHUNKSIZE);
    set_hd1 (p, 0);
    set_hd2 (p, (MINCHUNKSIZE << 1) | 0x01);
    p->fwd = p;
    p->bck = p;
    mod_mem->freelists[i] = p;
  }

  /* This chunk starts out on the appropriate free list and
   * contains all of the available memory. */
  mod_mem->firstchunk = (chunkptr)(mod_mem->baseptr + NUM_FREE_LISTS * MINCHUNKSIZE);
  set_hd1 (mod_mem->firstchunk, MINCHUNKSIZE);
  size = memsize - ((NUM_FREE_LISTS + 1) * MINCHUNKSIZE);
  set_hd2 (mod_mem->firstchunk, size << 1);
  add_to_list (mod_mem->freelists[list_idx (size)], mod_mem->firstchunk);

  /* The last chunk is never placed on any list; it is marked "in use",
   * and simply acts as a sentry element, when scanning all the chunks. */
  mod_mem->lastchunk = (chunkptr)(((unsigned char *)(mod_mem->baseptr)) + memsize - MINCHUNKSIZE);
  set_hd1 (mod_mem->lastchunk, size);
  set_hd2 (mod_mem->lastchunk, (MINCHUNKSIZE << 1) | 0x01);

  return 0;
}

/*
 * Allocate and return a pointer to a memory area of at least
 * the indicated size. The allocated block of memory will be aligned
 * on a 4-byte boundary.
 * Returns 0 if allocation fails.
 */
static void *
we_mem_internal_malloc (WE_UINT8 modId, WE_UINT32 size)
{
  mem_internal_t* mod_mem;
  chunkptr p = 0, ptmp;
  WE_UINT32   nb;
  WE_UINT32   sz = 0xFFFFFFFF;
  WE_UINT32   remsize;
  int    i;

  mod_mem = &(mem_internal[modId]);

  if (mod_mem->baseptr == NULL)
    return 0;
  /* We add space for our overhead (4 bytes) plus round to nearest
   * larger multiple of 4, plus never allocate a chunk less than 8 bytes. */
  nb = request2size (size);

  /* Check all relevant free lists, until a non-empty one is found. */
  for (i = list_idx (nb); i < NUM_FREE_LISTS; i++) {
    chunkptr freelist = mod_mem->freelists[i];

    /* Search the entire list, select chunk with closest fit */
    for (ptmp = freelist->fwd; ptmp != freelist; ptmp = ptmp->fwd) {
      WE_UINT32 tmpsz = chunksize (ptmp);

      if (tmpsz == nb) { /* Exact fit: no need to search any further. */
        p = ptmp;
        sz = tmpsz;
        goto found;
      }
      else if (tmpsz > nb) { /* Chunk is large enough */
        if (tmpsz < sz) {
          /* This is the best so far. */
          p = ptmp;
          sz = tmpsz;
        }
      }
    }
    if (p != 0) {
      goto found;
    }
  }
  /* Searched all lists, but found no large enough chunk */
  return 0;

 found:
  /* We have found a large enough chunk, namely "p" of size "sz". */
  remove_from_list (p);
  remsize = sz - nb;

  if (remsize >= MINCHUNKSIZE) {
    /* The remainder is large enough to become a separate chunk */
    chunkptr q, next;
    chunkptr l;

    sz = nb;
    /* "q" will be the new chunk */
    q = (chunkptr)((unsigned char *)p + sz);
    set_hd2 (q, remsize << 1);
    set_hd1 (q, nb);
    next = nextchunk (q);
    next->prev_size = remsize;

    l = mod_mem->freelists[list_idx (remsize)];
    add_to_list (l, q);
  }
  set_hd2 (p, (sz << 1) | 0x01);

#ifdef WE_CFG_DEBUG_INTERNAL_MALLOC
  check_allocated_chunk (modId, p);
#endif
  return chunk2mem (p);
}

/*
 * Free a memory area previously allocated with we_mem_internal_malloc.
 * Calling this routine with 'mem' equal to 0 has no effect.
 */
static void
we_mem_internal_free (WE_UINT8 modId, void* mem)
{
  mem_internal_t* mod_mem;
  chunkptr p;         /* chunk corresponding to mem */
  WE_UINT32   sz;        /* its size */
  chunkptr next;      /* next adjacent chunk */
  chunkptr prev;      /* previous adjacent chunk */
  chunkptr l;

  mod_mem = &(mem_internal[modId]);

  if (mem == 0 || mod_mem->baseptr == NULL) {
    return;
  }

  p = mem2chunk (mem);
#ifdef WE_CFG_DEBUG_INTERNAL_MALLOC
  check_allocated_chunk (modId, p);
#endif
  if (chunk_isfree (p)) {
    return;
  }
  sz = chunksize (p);
  prev = prevchunk (p);
  next = nextchunk (p);
  
  if (chunk_isfree (prev)) {            /* Join with previous chunk */
    sz += chunksize (prev);
    p = prev;
    remove_from_list (prev);
  }
  if (chunk_isfree (next)) {            /* Join with next chunk */
    sz += chunksize (next);
    remove_from_list (next);
  }
  set_hd2 (p, sz << 1);
  next = nextchunk (p);
  next->prev_size = sz;

  l = mod_mem->freelists [list_idx (sz)];
  add_to_list (l, p);
}

/*
 * Free all memory areas allocated by a module, by doing a new Initialize.
 */
static void
we_mem_internal_free_all (WE_UINT8 modId)
{
  void*       mem;
  WE_UINT32  size = 0;

  mem = mem_internal[modId].baseptr;

  if (mem != NULL) {
    size = (WE_UINT32)(((unsigned char*)(mem_internal[modId].lastchunk)) + MINCHUNKSIZE - (unsigned char*)mem);
    we_mem_internal_init (modId, mem, size);
  }
}

#else /* NOT WE_CONFIG_INTERNAL_MALLOC */

typedef struct meminfo_st {
  void   *next;
  void   *prev;
  WE_UINT32  size;
} mem_info_t;

#define PREV(memory) (((mem_info_t *)memory)->prev)
#define NEXT(memory) (((mem_info_t *)memory)->next)

#define SIZE_OF_HEAD sizeof (mem_info_t)

static void we_mem_list_new (WE_UINT8 modId, void *memory, WE_UINT32 size);
static void we_mem_list_delete (WE_UINT8 modId, void *memory);
static void we_mem_list_delete_all (WE_UINT8 modId);

#endif


/**************************************************
 *  Memory block list used by memory allocator
 **************************************************/

#ifndef WE_CONFIG_INTERNAL_MALLOC

static void* mem_blocks[WE_NUMBER_OF_MODULES];

static void
we_mem_list_new (WE_UINT8 modId, void *memory, WE_UINT32 size)
{
  mem_info_t *info = (mem_info_t *)memory;

  PREV(memory) = NULL;
  if (mem_blocks[modId]){
    PREV (mem_blocks[modId]) = memory;
    NEXT (memory) = mem_blocks[modId];
  }
  else {
    NEXT (memory) = NULL;
  }
  mem_blocks[modId] = memory;
  info->size = size;
}

static void
we_mem_list_delete (WE_UINT8 modId, void *memory)
{
  if (PREV (memory) && NEXT (memory)) {
    NEXT(PREV(memory)) = NEXT(memory);
    PREV(NEXT(memory)) = PREV(memory);
  }
  else if (PREV(memory)) {
    NEXT(PREV(memory)) = NULL;
  }
  else if (NEXT(memory)) {
    PREV(NEXT(memory)) = NULL;
    mem_blocks[modId] = NEXT(memory);
  }
  else {
    mem_blocks[modId] = NULL;
  }
}

static void
we_mem_list_delete_all (WE_UINT8 modId)
{
  void *p = mem_blocks[modId];

  while (p != NULL) {
    void *t0 = p;

    p = NEXT(p);
    TPIa_memFree (modId, t0);
  }

  mem_blocks[modId] = NULL;
}

#endif /* NOT WE_CONFIG_INTERNAL_MALLOC */

/**********************************************************************
 * Memory allocation
 **********************************************************************/

static int lognum;

WE_UINT32
we_mem_allocated_bytes (WE_UINT8 modId)
{
  if (modId < WE_NUMBER_OF_MODULES) {
    return mem_mod[modId].allocated_bytes;
  }
  else {
    return 0;
  }
}

/*
 * Return the size of an allocated memory area.
 * The reported size includes the overhead used by the memory allocation
 * system.
 */
WE_UINT32
we_mem_get_size (void *mem)
{
#ifdef WE_CONFIG_INTERNAL_MALLOC
  chunkptr p;         /* chunk corresponding to mem */

  p = mem2chunk (mem);
  return chunksize (p);
#else
  mem_info_t *t = (mem_info_t *)((char *)mem - sizeof (mem_info_t));
  
  return t->size + SIZE_OF_HEAD;
#endif
}


/*
 * Allocate memory
 */
#ifdef WE_LOG_MODULE
void *
we_mem_alloc (WE_UINT8 modId, WE_UINT32 size, const char *filename, int lineno)
#else
void *
we_mem_alloc (WE_UINT8 modId, WE_UINT32 size)
#endif
{
  void *memory = NULL;

  if (mem_mod[modId].use_extern_alloc)
    return TPIa_memExternalAlloc (modId, size);

  if (
#ifdef WE_CONFIG_INTERNAL_MALLOC
      ((memory = we_mem_internal_malloc (modId, size)) == NULL))  {
#else
      ((memory = TPIa_memAlloc (modId, size + SIZE_OF_HEAD)) == NULL))  {
#endif
    /* Out-of-memory handling:
     *
     * If the module have an exception
     * function call it, otherwise return NULL.
     * 
     */
    if (mem_mod[modId].exception_func != NULL)
      mem_mod[modId].exception_func ();
    return NULL;
  }

#ifndef WE_CONFIG_INTERNAL_MALLOC
  we_mem_list_new (modId ,memory, size);
  memory = (void *)((char *)memory + SIZE_OF_HEAD);
#endif

  if (modId < WE_NUMBER_OF_MODULES) {
#ifdef WE_CONFIG_INTERNAL_MALLOC
    chunkptr ch;         /* chunk corresponding to mem */

    ch = mem2chunk (memory);
    mem_mod[modId].allocated_bytes += chunksize (ch);
#else
    mem_mod[modId].allocated_bytes += size + SIZE_OF_HEAD;
#endif
  }

#ifdef WE_LOG_MODULE
  {
    const char *s = strrchr (filename, '\\');

    if (s == NULL)
      s = filename;
    else
      s += 1;
    WE_LOG_MSG ((TPI_LOG_TYPE_MEMORY, modId, "MEM: + %#08x %6d %6d %6d %s (%d)\n",
                  memory, lognum++, size, modId, s, lineno));

  }
#endif

  return memory;
}

/*
 * Free memory
 */
#ifdef WE_LOG_MODULE
void
we_mem_free (WE_UINT8 modId, void *p, const char *filename, int lineno)
#else
void
we_mem_free (WE_UINT8 modId, void *p)
#endif
{
  if (p == NULL)
    return;

  if (mem_mod[modId].use_extern_alloc) {
    TPIa_memExternalFree (modId, p);
    return;
  }

  if (modId < WE_NUMBER_OF_MODULES) {
#ifdef WE_CONFIG_INTERNAL_MALLOC
    chunkptr ch;         /* chunk corresponding to mem */

    ch = mem2chunk (p);
    mem_mod[modId].allocated_bytes -= chunksize (ch);
#else
    mem_info_t *t = (mem_info_t *)((char *)p - sizeof (mem_info_t));
  
    mem_mod[modId].allocated_bytes -= (t->size + SIZE_OF_HEAD);
#endif
  }

#ifdef WE_LOG_MODULE
  if (p != NULL) {
    const char *s = strrchr (filename, '\\');

    if (s == NULL)
      s = filename;
    else
      s += 1;
    WE_LOG_MSG ((TPI_LOG_TYPE_MEMORY, modId, "MEM: - %#08x %6d        %6d %s (%d)\n",
                  p, lognum++, modId, s, lineno));
  }
#endif

#ifdef WE_CONFIG_INTERNAL_MALLOC
  we_mem_internal_free (modId, p);
#else
  {
    mem_info_t *t = (mem_info_t *)((char *)p - sizeof (mem_info_t));
    we_mem_list_delete (modId,t);
    TPIa_memFree (modId, t);
  }
#endif
}


void
we_mem_free_all (WE_UINT8 modId)
{
#ifdef WE_CONFIG_INTERNAL_MALLOC
  we_mem_internal_free_all (modId);
#else
  we_mem_list_delete_all (modId);
#endif
  if (modId < WE_NUMBER_OF_MODULES) {
    mem_mod[modId].allocated_bytes = 0;
  }
}


void
we_mem_init (WE_UINT8 modId, we_mem_exception_func_t* exceptionHandler,
              WE_BOOL use_extern_alloc)
{
#ifdef WE_CONFIG_INTERNAL_MALLOC
  void*       mem;
  WE_UINT32  size = 0;
#endif

  if (modId < WE_NUMBER_OF_MODULES) {
    if (modId == WE_MODID_FRW)
      lognum = 0;
    mem_mod[modId].exception_func = exceptionHandler;
    mem_mod[modId].allocated_bytes = 0;
    mem_mod[modId].use_extern_alloc = use_extern_alloc;

#ifdef WE_CONFIG_INTERNAL_MALLOC
    mem = TPIa_memInternalInit (modId, &size);
    we_mem_internal_init (modId, mem, size);
#else
    mem_blocks[modId] = NULL;
#endif

  }

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, modId, "WE: MEMORY - we_mem_init\n"));
}
