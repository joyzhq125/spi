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

/*! \file Mms_Env.h
 * \brief  Exported functions to be used by the WE
 *
 * Exported connector functions, i.e. from Target Device Application to the MMS 
 * client. For every connector function, the following data are listed:
 * - A brief one line description of the function
 * - A longer description of the function
 * - The parameters for the function
 */

#ifndef MMSENV_H
#define MMSENV_H

/*--- Definitions/Declarations ---*/

/*--- Types ---*/

/*--- Constants ---*/
        
/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

/*--- Prototypes ---*/

/*!
 * \brief This function is called by the system in emergency situations.
 *
 *****************************************************************************/
void MMSc_kill (void);

/*!
 * \brief Performs an execution 'tick' for the MMS client.
 *
 * When this function is called, the MMS client executes one 'tick' (or 
 * actually one state change in the internal state machines). This function 
 * is only called if MMSc_wantsToRun() has returned TRUE.
 *
 * Type: Request Connector
 *
 * Adapter Name: None.
 *
 *****************************************************************************/
void MMSc_run(void);

/*!
 * \brief Prepares the MMS client for running.
 *
 * The MMS client initializes internal memory structures, timers and data. It
 * is also reading configuration data and setting default values
 *
 * Type: Request Connector
 *
 * Adapter Name: None.
 *
 *****************************************************************************/
void MMSc_start(void);

/*!
 * \brief Cleans up all resources and ends the MMS client.
 *
 * All allocated memory and other resources are deallocated. Tasks 
 * currently running will be aborted.
 *
 * Type: Request Connector
 *
 * Adapter Name: None.
 *
 *****************************************************************************/
void MMSc_terminate(void);

/*!
 * \brief  Check if MMS wants to run. 
 *
 * This connector checks if the MMS client has any signals in its signal queue.
 * If so, the MMSc_run() function shall be called.
 *
 * Type: Request Connector
 *
 * Adapter Name: None.
 *
 * \return 1 if MMS Client has work to do, 0 otherwise
 *****************************************************************************/
int MMSc_wantsToRun(void);

/*
 * \brief Returns the number of registered applications.
 *
 * This is for MMS Service internal usage.
 *
 * \return g_numRegisteredApps.
 *****************************************************************************/
WE_UINT8 mmsNoOfRegisteredApps(void);

#endif  /* MMSENV_H */
