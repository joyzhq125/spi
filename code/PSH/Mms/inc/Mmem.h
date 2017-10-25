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





#ifndef MMEM_H
#define MMEM_H












#define M_ALLOC(s)          WE_MEM_ALLOC( WE_MODID_MMS, s)
#define M_CALLOC(s)         mmsMemCalloc(s)
#define M_ALLOCTYPE(t)      mmsMemCalloc(sizeof(t))
#define M_FREE(p)           {WE_MEM_FREE( WE_MODID_MMS, p); (p) = NULL;}


void *mmsMemCalloc(WE_UINT32 size);

#endif 
