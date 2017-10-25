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

/*! \file macth.h
 *  Cross triggering handling.
 */

#ifndef _MACTH_H_
#define _MACTH_H_

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! \enum MsaCthSignalId
 *  Signals for the CORE FSM */
typedef enum 
{
    /*! Starts the content routing receive FSM
     * u_param1 = mode
     */
    MSA_SIG_CTH_START,

    /*! The dialog has been displayed.
     *	
     */
    MSA_SIG_CTH_DLG_RSP,

    /*!
     *
     */
    MSA_SIG_CTH_FILE_NOTIFY

}MsaCthSignalId;

/******************************************************************************
 * Data-types
 *****************************************************************************/

/******************************************************************************
 * Prototypes 
 *****************************************************************************/

void msaCthInit(void);
void msaCthTerminate(void);
void msaCtFreeContentInsert(MsaCtContentInsert *ci);
WE_BOOL msaCthHandleSchemeCt(const we_module_execute_cmd_t *command);
WE_BOOL msaCthHandleInsertContentCt(const we_content_data_t *contentData, 
    MsaPropItem *item);

#endif
