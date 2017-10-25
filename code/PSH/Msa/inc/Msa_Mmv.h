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

/*! \file mammv.h
 *  \brief Viewer of Multipart Mixed messages (interface)
 */

#ifndef _MAMMV_H_
#define _MAMMV_H_

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! \enum MsaMmvSignalId
 *  Signals for the multipart mixed message viewer FSM */
typedef enum 
{
    /*! Activate the MM message viewer.
     * p_param MsaMessage structure indicating message to view.
     */
    MSA_SIG_MMV_ACTIVATE,
    
    /*! Deactivate the MM message viewer.
     */
    MSA_SIG_MMV_DEACTIVATE,

    /*! Return signal from media object browser
     * u_param1 Type of MOB operation, see #MsaMobOperation
     * u_param2 The selected attachment, or -1 if none.
     * p_param Link information, if user selected link, see #MtrLinkEntry
     */
    MSA_SIG_MMV_MOB_OP,

    /*! The list of possible actions are returned through this signal
     *
     *  u_param1 = result
     *  u_param2 = count
     *  p_param  = the list of actions
     */
    MSA_SIG_MMV_HANDLE_ACTION_RSP,

    /*! Deactivate the attachment menu */
    MSA_SIG_MMV_NAV_DEACTIVATE,

    /*! Handle object action
     *
     *  u_param1 = The object action index
     */
    MSA_SIG_MMV_OBJ_ACTION,

    /*! Save done response signal */
    MSA_SIG_MMV_SAVE_DONE,

    /*! Content router done with playing
     * u_param1 Result of CR operation, see #MsaCrhResult.
     */
    MSA_SIG_MMV_CRH_DONE,

    /*! Make call response 
     * u_param1 TRUE/FALSE result 
     */
    MSA_SIG_MMV_MAKE_CALL_RSP
}MsaMmvSignalId;

/******************************************************************************
 * Prototypes
 *****************************************************************************/

void msaMmvInit(void);
void msaMmvTerminate(void);


#endif /* _MAMMV_H_ */
