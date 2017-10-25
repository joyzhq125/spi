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

/*! \file mase.h
 *  \brief Control logic for the SMIL editor.
 */

#ifndef _MASE_H_
#define _MASE_H_

/******************************************************************************
 * Constants
 *****************************************************************************/

/*!\enum MsaSeSignalId 
 * \brief Signals for the SE FSM 
 */
typedef enum 
{
    /* SMIL edit core */
    /*! Activates the SMIL editor
     * p_param  = The message, see #MsaMeInstance 
     * u_param1 = -
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_ACTIVATE,

    /*! Deactivates the SMIL editor
     * p_param  = -
     * u_param1 = -
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_DEACTIVATE,

    /*! Inserts a new slide into the presentation, the new slide is displayed
     *  to the End User.
     * p_param  = -
     * u_param1 = -
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_INSERT_SLIDE,

    /*! Deletes the slide that is currently active
     * p_param  = -
     * u_param1 = -
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_DELETE_SLIDE,

    /* Retrieves a media object using the AFI file system.
     * u_param1 = The result, see #MsaOpenDlgResult
     * p_param  = The file-name or NULL.
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_GET_MEDIA_OBJECT,

    /*! Sets the next slide as the active one
     *
     * p_param  = -
     * u_param1 = -
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_NEXT_SLIDE,

    /*! Sets the previous slide as the active one
     *
     * p_param  = -
     * u_param1 = -
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_PREVIOUS_SLIDE,

    /*! Save the message
     *
     * p_param  = -
     * u_param1 = -
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_SAVE,

    /*! Save the message as a template
     *
     * p_param  = -
     * u_param1 = TRUE if save was ok, otherwise FALSE.
     * u_param2 = Message Id av the new message
     * i_param  = -
     */
    MSA_SIG_SE_SAVE_AS_TEMPLATE,

    /*! Save the message as a template
     *
     * p_param  = -
     * u_param1 = TRUE if save was ok, otherwise FALSE.
     * u_param2 = Message Id av the new message
     * i_param  = -
     */
    MSA_SIG_SE_SAVE_TEMPLATE,

    /*! SMIL creation signals
     *
     * p_param  = -
     * u_param1 = TRUE if save was ok, otherwise FALSE.
     * u_param2 = Message Id av the new message
     * i_param  = -
     */
    MSA_SIG_SE_SMIL_SAVE_RSP,

    /*! Sends the message
     *
     * p_param  = -
     * u_param1 = -
     * u_param2 = -
     * i_param  = -	
     */
    MSA_SIG_SE_SEND,

    /*! Activates the main menu
     *
     * p_param  = -
     * u_param1 = -
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_NAV_MENU_ACTIVATE,

    /*! Deactivates the main menu
     *
     * p_param  = -
     * u_param1 = -
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_NAV_MENU_DEACTIVATE,

    /*! Handles insert/edit of media objects 
     *
     * p_param  = -
     * u_param1 = The type of object the handle, see #MsaMediaGroupType
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_HANDLE_OBJECT,

    /*! The response when an object has been opened
     *
     * p_param  = -
     * u_param1 = -
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_GET_MEDIA_OBJECT_RSP,

    /*! Preview the current message
     *
     * p_param  = -
     * u_param1 = -
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_PREVIEW_SMIL,

    /*! The message is saved for the forthcoming preview
     *
     * p_param  = -
     * u_param1 = TRUE if save was ok, otherwise FALSE.
     * u_param2 = The msgId of the new message.
     * i_param  = -
     */
    MSA_SIG_SE_PREVIEW_SAVE_DONE,

    /*! The message has been previewed
     *
     * p_param  = -
     * u_param1 = -
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_PREVIEW_DONE,

    /* Removes a media object from the current presentation
     *
     * p_param  = -
     * u_param1 = the object type to remove, see #MsaMediaGroup
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_REM_OBJECT,

    /*! Launch the color selector form for the background color
     *
     * p_param  = -
     * u_param1 = -
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_SET_BG_COLOR,

    /*! The response from the color selector form
     *
     * p_param  = -
     * u_param1 = the new color
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_SET_BG_COLOR_RSP,

    /*! Launch the color selector form
     *
     * p_param  = -
     * u_param1 = -
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_SET_TEXT_COLOR,

    /*! The response from the color selector form
     *
     * u_param1 = the new color
     */
    MSA_SIG_SE_SET_TEXT_COLOR_RSP,

    /*! 
     *
     * p_param  = -
     * u_param1 = -
     * u_param2 = -
     * i_param  = -
     */
        MSA_SIG_SE_SET_TEXT_COLOR_ALL,

    /*! The response from the color selector form
     *
     * u_param1 = the new color
     */
    MSA_SIG_SE_SET_TEXT_COLOR_ALL_RSP,

    /*! 
     *
     * p_param  = -
     * u_param1 = -
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_GADGET_GOT_FOCUS,

    /*! 
     *
     * p_param  = -
     * u_param1 = -
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_GADGET_LOST_FOCUS,

    /*! 
     *
     * p_param  = -
     * u_param1 = -
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_EVENT_KEY_DOWN,

    /*! 
     *
     * p_param  = -
     * u_param1 = -
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_EVENT_KEY_UP,

    /*! 
     * p_param  = -
     * u_param1 = The type of object to set the timing. See 
     *            #MsaMediaGroupType
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_SET_ITEM_TIMING,

    /*! 
     *
     * p_param  = -
     * u_param1 = -
     * u_param2 = -
     * i_param  = -
     */
    MSA_SIG_SE_SET_ITEM_TIMING_RSP

}MsaSeSignalId;

/******************************************************************************
* Data-structures
*****************************************************************************/

/******************************************************************************
 * Prototypes
 *****************************************************************************/

void msaSeInit(void);
void msaSeTerminate(void);
unsigned int msaSeGetDuration(void);
WE_BOOL msaSeHandleDuration(char *duration, unsigned int notUsed, 
    void *dataPtr, unsigned int uintValue);
void msaSeTerminateGui(void);

WE_BOOL msaSeHandleObjStart(char *str, unsigned int notUsed, 
    void *dataPtr, unsigned int uintValue);
WE_BOOL msaSeHandleObjDur(char *str, unsigned int notUsed, 
    void *dataPtr, unsigned int uintValue);

#endif /* _MASE_H_ */
