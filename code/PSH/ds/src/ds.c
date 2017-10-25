#ifdef __cplusplus
extern "C" {
#endif
/*==================================================================================================

    MODULE NAME : ds.c

    GENERAL DESCRIPTION
        This file includes DS initialization founction.

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    07/218/2002   Ashley Lee       crxxxxx     Initial creation
    03/26/2003   lindawang        C000018     Set new pacific image for the first version.

    This file includes DS initialization founction.
        
====================================================================================================
    INCLUDE FILES
==================================================================================================*/

#ifdef WIN32
#include    "windows.h"
#include    "portab_new.h"
#else
#include    "portab.h"
#endif

#include    "rm_include.h"
#include    "SP_sysutils.h"
#include    "ds.h"

/*==================================================================================================
    LOCAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================
    GLOBAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
    FUNCTION: ds_init

    DESCRIPTION:
        Initialize display system.

    ARGUMENTS PASSED:
        You may omit description of parameters whose name is descriptive enough to understand
        by reading the code without reading the description of arguments

    RETURN VALUE:
        You may omit description of return value if the retunr value is generally understandable
        by software engineers

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important aspect of using
        the function i.e side effect..etc
==================================================================================================*/
void ds_init
( 
    void 
)
{
    ds_init_ds();
    popup_init_popup_windows();
}


/*================================================================================================*/

#ifdef __cplusplus
}
#endif

