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

/*! \file mamme.h
 *  \brief Message editor (Multi-Part/Mixed) control logic. 
 */

#ifndef _MAMME_H_
#define _MAMME_H_

/******************************************************************************
 * Constants
 *****************************************************************************/
/*!\enum MsaMmeSignalId 
 * \brief Signals for the MME FSM 
 */
typedef enum 
{
    /*
     *	Activation. Send a MsaMessage as p_param.
     */
    MSA_SIG_MME_ACTIVATE,
    MSA_SIG_MME_DEACTIVATE,

    /*
     *	Action received from MOB
     */
    MSA_SIG_MME_MOB_OP,


    /*
     *  Signals related to manipulating the media object list
     */
    MSA_SIG_MME_GET_MEDIA_OBJECT,
    MSA_SIG_MME_GET_MEDIA_OBJECT_REPLACEMENT,
    MSA_SIG_MME_GET_MEDIA_OBJECT_RSP,
    MSA_SIG_MME_DELETE_MO,
    MSA_SIG_MME_ADD_TEXT,
    MSA_SIG_MME_EDIT_TEXT,
    MSA_SIG_MME_PLAY_SOUND,

    /*
     *	Text editor signals
     */
    MSA_SIG_MME_TEXT_OK,
    MSA_SIG_MME_ADD_TEXT_OK,
    MSA_SIG_MME_TEXT_BACK,

    /*
     *	Dialog respnose
     */
    MSA_SIG_MME_RESTART_MOB_IND,

    /*
     *	Message saving/sending
     */
     MSA_SIG_MME_SEND,
     MSA_SIG_MME_SEND_DONE,
     MSA_SIG_MME_SAVE,
     MSA_SIG_MME_SAVE_DONE,
     MSA_SIG_MME_SAVE_AS_TEMPLATE,
     MSA_SIG_MME_SAVE_TEMPLATE,
     MSA_SIG_MME_SAVE_AS_TEMPLATE_DONE,
     MSA_SIG_MME_SAVE_TEMPLATE_DONE,

    /*
     *	Navigation menu
     */
    MSA_SIG_MME_NAV_MENU_ACTIVATE,
    MSA_SIG_MME_NAV_MENU_DEACTIVATE
}MsaMmeSignalId;

/*!\enum MsaMmeMainMenuItems 
 * \brief Menu items for the MME
 */
typedef enum
{
    MSA_MME_ADD_TEXT,               /*!< Add text */
    MSA_MME_ADD_MEDIA_OBJECT,       /*!< Add MO to the message */
    MSA_MME_DELETE_MEDIA_OBJECT,    /*!< Delete MO from the message */
    MSA_MME_REPLACE_MEDIA_OBJECT,   /*!< Replace MO in the message */
    MSA_MME_EDIT_TEXT,              /*!< Edit text */
    MSA_MME_PLAY_SOUND,             /*!< Play sound */
    MSA_MME_SAVE,                   /*!< Save message */
    MSA_MME_SAVE_AS_TEMPLATE,       /*!< Save message as template */
    MSA_MME_SAVE_TEMPLATE,          /*!< Save template */
    MSA_MME_SEND,                   /*!< Send message */
    MSA_MME_PREVIEW,                /*!< Preview message */
    MSA_MME_BACK,                   /*!< Go back */
    MSA_MME_MENU_ITEM_COUNT         /*!< Not used */
}MsaMmeMainMenuItems;

/******************************************************************************
 * Prototypes
 *****************************************************************************/
void msaMmeInit(void);
void msaMmeTerminate(void);
void msaMmeTerminateGui(void);

#endif /* _MAMME_H_ */
