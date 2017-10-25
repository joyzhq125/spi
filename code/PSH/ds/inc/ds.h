#ifndef DS_H
#define DS_H
/*==================================================================================================

    HEADER NAME : ds.h

    GENERAL DESCRIPTION
        This file includes display system files for Display system user.

    TECHFAITH Telecom Confidential Proprietary
    (c) Copyright 2002 by TECHFAITH Telecom Corp. All Rights Reserved.
====================================================================================================

    Revision History
   
    Modification                  Tracking
    Date         Author           Number      Description of changes
    ----------   --------------   ---------   -----------------------------------------------------------------------------------
    06/22/2002   Ashley Lee       crxxxxx     Initial Creation
    03/26/2003   lindawang        C000018     Set new pacific image for the first version.
    06/14/2003   lindawang        P000474     Support play gif animation and fix some bugs.
    This file includes display system files for Display system user.


====================================================================================================
    INCLUDE FILES
==================================================================================================*/

#include    "OPUS_typedef.h"
#include    "ds_def.h"
#include    "ds_drawing.h"
#include    "ds_primitive.h"
#include    "ds_popup.h"
#include    "ds_sublcd.h"
#include    "product.h"
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
extern void ds_init
( 
    void 
);

/*================================================================================================*/
#endif  /* DS_DEF_H */
