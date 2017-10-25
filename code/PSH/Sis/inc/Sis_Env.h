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
/*! \file Sis_env.h
 * \brief  <brief description>
 */

#ifndef SIS_ENV_H
#define SIS_ENV_H

/*--- Definitions/Declarations ---*/

/*--- Types ---*/

/*--- Constants ---*/

/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

void
SISc_start(void);

int
SISc_wantsToRun(void);

void
SISc_run(void);

/*!
 * \brief This function is called by the system in emergency situations.
 *
 *****************************************************************************/
void SISc_kill (void);

/*!
 * \brief Used to emergency stop the SMIL Service in case of no available memory
 *
 * ONLY FOR SIS SERVICE INTERNAL USAGE.
 *****************************************************************************/
void Sis_exception_handler(void);


#endif /* SIS_ENV_H */



