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






#include "We_Lib.h"    
#include "We_Mem.h"    

#include "Mms_Cfg.h"    
#include "Mms_Def.h"    
#include "Mms_Env.h"    
#include "Mmem.h"       






























void *mmsMemCalloc(WE_UINT32 size)
{
    void *tmp = WE_MEM_ALLOC( WE_MODID_MMS, size);

    if (tmp != NULL)
    {
        memset( tmp, 0, size);
    } 

    return tmp;
} 
