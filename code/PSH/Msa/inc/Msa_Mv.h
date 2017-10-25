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

/*! \file mamv.h
 *  \brief Message Viewer control logic.
 */

#ifndef _MAMV_H_
#define _MAMV_H_

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! \enum MsaMVSignalId
 *  Signals for the message viewer FSM */
typedef enum 
{
    /*! Activate the message viewer.
     * i_param: filetype
     * u_param1: message-wid to view
     */
    MSA_SIG_MV_ACTIVATE,
    
    /*! The response to the MMSif_getMsgInfo
     *
     *  p_param = the message info, see #MmsMessageFolderInfo
     */
    MSA_SIG_MV_GET_MSG_TYPE_RSP,

    /*
     *	The response gotten when MCR is done loading a message
     */
    MSA_SIG_MV_MR_DONE,

    /*! Deactivate the message viewer.
     */
    MSA_SIG_MV_DEACTIVATE,

    /*! View the SMIL message.
     */
    MSA_SIG_MV_VIEW_SMIL,
     
    /*! Activate the viewer for message properties
     *
     */
    MSA_SIG_MV_PROP_ACTIVATE,

    /*! The end user presses ok when the property viewer is active
     *
     */
    MSA_SIG_MV_PROP_OK,

    /*!
     * View content (SMIL/MM)
     */
    MSA_SIG_MV_VIEW_CONTENT,

    /* Navigation menu */
    /*! Activate navigation menu
     * i_param = the current part of the message that are displayed; 
     *           MSA_MESSAGE_TYPE_TEXT or MSA_MESSAGE_TYPE_ATTACHMENTS.
     */
    MSA_SIG_MV_NAV_ACTIVATE,

    /*! Deactivate navigation menu
     *
     */
    MSA_SIG_MV_NAV_DEACTIVATE,

    /*! The MMS service has returned requested Read Report data 
     *
     */
    MSA_SIG_MV_RR, 

    /*! The MMS service has returned requested Delivery Report data 
     *
     */
    MSA_SIG_MV_DR,

    /*! This signal is sent if a read-report shall be sent.
     * u_param = MsaConfirmDialogResult, ok or cancel.
     */
    MSA_SIG_MV_SEND_READ_REPORT,
    
    /*! The SIA has finished playing the SMIL presentation 
     *
     */
    MSA_SIG_MV_PLAY_DONE,

    /*! The download response
     *
     */
    MSA_SIG_MV_DOWNLOAD_RSP,

    /* Address lookup response
     *
     * p_param = The lookup entry, see #MsaAddrItem
     */
    MSA_SIG_MV_NAME_LOOKUP_RSP

}MsaMvSignalId;

/*! \enum MsaMvNavMenuItem
 *  Possible menu items for the message viewer */
typedef enum
{
    MSA_MV_VIEW_PROPERTIES, /* !< View message properties */
    MSA_MV_VIEW_CONTENT,    /* !< View the text part of a message */
    MSA_MV_BACK,            /* !< Navigate back */
    MSA_MV_NAV_MENU_COUNT   /* !< Not used */
}MsaMvNavMenuItem;

/******************************************************************************
 * Data-structures
 *****************************************************************************/

/******************************************************************************
 * Prototypes
 *****************************************************************************/

void msaMvInit(void);
void msaMvTerminate(void);

#endif /* _MAMV_H_ */










































