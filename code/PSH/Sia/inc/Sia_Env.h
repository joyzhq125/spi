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

/*! \file Sia_env.c
 * \brief  Exported interface functions from the SMA.
 */

#ifndef SIA_ENV_H
#define SIA_ENV_H

/*!
 * \brief Prepares the Smil application for running.
 *
 * The Smil application initializes internal memory structures, timers and data. It
 * is also reading configuration data and setting default values
 *
 * Type: Request Connector
 *
 * Adapter Name: None.
 *
 *****************************************************************************/
void SMAc_start (void);

/*!
 * \brief  Check if SMA wants to run. 
 *
 * This connector checks if the Smil application has any signals in its signal queue.
 * If so, the SIAc_run() function shall be called.
 *
 * Type: Request Connector
 *
 * Adapter Name: None.
 *
 * \return TRUE if SMA has work to do, FALSE otherwise
 *****************************************************************************/
int SMAc_wantsToRun (void);

/*!
 * \brief Performs an execution 'tick' for the Smil application.
 *
 * When this function is called, the Smil application executes one 'tick' (or 
 * actually one state change in the internal state machines). This function 
 * is only called if SMAc_wantsToRun() has returned TRUE.
 *
 * Type: Request Connector
 *
 * Adapter Name: None.
 *
 *****************************************************************************/
void SIAc_run (void);


/*!
 * \brief This function is called by the system in emergency situations.
 *
 *****************************************************************************/
void SMAc_kill (void);

#endif
