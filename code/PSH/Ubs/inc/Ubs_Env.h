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
 * ubs_env.h
 *
 * Created by Klas Hermodsson, Tue 03 June 2003.
 *
 * Revision  history:
 * 
 *
 */

#ifndef UBS_ENV_H
#define UBS_ENV_H


void
UBSc_start (void);

int
UBSc_wantsToRun (void);

void
UBSc_run (void);

void
UBSc_kill (void);

#endif      /*UBS_ENV_H*/
