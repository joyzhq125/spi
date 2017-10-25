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

#ifndef SAMEM_H
#define SAMEM_H

/*--- Definitions/Declarations ---*/


/*--- Types ---*/

/*********************************************
 * Memory Macro
 *********************************************/
#define SIA_ALLOC(s)      WE_MEM_ALLOC(WE_MODID_SIA,s)
#define SIA_CALLOC(s)     smaMemCalloc(s)
#define SIA_ALLOCTYPE(t)  WE_MEM_ALLOCTYPE(WE_MODID_SIA,t)
#define SIA_FREE(p)       WE_MEM_FREE(WE_MODID_SIA,p)


/*********************************************
 * Exported function
 *********************************************/
void *smaMemCalloc(WE_UINT32 size);
#endif /* SAMEM_H */
