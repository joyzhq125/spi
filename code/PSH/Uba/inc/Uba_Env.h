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
 * uba_env.h
 *
 * Created by Anders Evertsson, Thu Jun 5 2003.
 *
 * Revision  history:
 *
 */
#ifndef UBA_ENV_H
#define UBA_ENV_H


/**********************************************************************
 * Function declarations
 **********************************************************************/

void
UBAc_start (void);

void
UBAc_kill (void);

int
UBAc_wantsToRun (void);

void
UBAc_run (void);

#endif      /*UBA_ENV_H*/
