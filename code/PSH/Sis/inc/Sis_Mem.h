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





#ifndef SIS_MEM_H
#define SIS_MEM_H












#define SIS_ALLOC(s)          WE_MEM_ALLOC(WE_MODID_SIS, s)
#define SIS_CALLOC(s)         slsMemCalloc(s)
#define SIS_ALLOCTYPE(t)      slsMemCalloc(sizeof(t))
#define SIS_FREE(p)           {if (NULL != (p)) WE_MEM_FREE(WE_MODID_SIS, (p)); (p) = NULL;}




void *slsMemCalloc(WE_UINT32 size);

#endif 
