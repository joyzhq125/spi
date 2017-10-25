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

#ifndef SIA_DEF_H
#define SIA_DEF_H

/*--- Include files ---*/

/*--- Definitions/Declarations ---*/

/*--- Types ---*/
/*! \enum SiaResult Result codes from the SMA module */
typedef enum
{
    SIA_RESULT_OK,
	SIA_RESULT_BUSY,
	SIA_RESULT_ERROR,
	SIA_RESULT_INSUFFICIENT_PERSISTENT_STORAGE,
	SIA_RESULT_TO_MANY_SLIDES
} SiaResult;

/*--- Constants ---*/

/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

/*--- Global variables ---*/

/*--- Static variables ---*/

/*--- Prototypes ---*/
/******************************************************************************/

#endif /*SIA_DEF_H */
