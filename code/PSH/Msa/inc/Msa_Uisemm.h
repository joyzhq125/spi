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

/* !\file mauisemm.h
 *  \brief The main menu for the SMIL editor.
 */

#ifndef _MAUBSEMM_H_
#define _MAUBSEMM_H_

/******************************************************************************
 * Definitions
 *****************************************************************************/

/*!\enum MsaSeMainMenuItems 
 * Menu items for the SMIL editor 
 */
typedef enum
{
    MSA_SE_TEXT_FG_COLOR,       /*!< Change the text color */
    MSA_SE_TEXT_FG_COLOR_ALL,       /*!< Change the text color */
    MSA_SE_TEXT_BG_COLOR,       /*!< Change the text background color */
    MSA_SE_ITEM_TIMING,         /*!< Set the timing for a specific object */
    MSA_SE_DELETE_OBJECT,       /*!< Delete the currently active object */
    MSA_SE_INSERT_PAGE,         /*!< Insert a new page */
    MSA_SE_DELETE_SLIDE,        /*!< Delete the current slide */
    MSA_SE_DURATION,            /*!< Set the slide duration */
    MSA_SE_BG_COLOR,            /*!< Background color */
    MSA_SE_SAVE,                /*!< Save the message */
    MSA_SE_SAVE_AS_TEMPLATE,    /*!< Save the message as a template */
    MSA_SE_SAVE_TEMPLATE,       /*!< Save template */
    MSA_SE_SEND,                /*!< Send the message */
    MSA_SE_PREVIEW_SMIL,        /*!< Preview the message using the SIA */
    MSA_SE_BACK,                /*!< Go back */
    MSA_SE_MENU_ITEM_COUNT      /*!< Not used */
}MsaSeMainMenuItems;

/******************************************************************************
 * Prototype declarations
 *****************************************************************************/

/* Navigation menu for the SE */
WE_BOOL msaSeCreateNavMenu(int slideCount, int initiated, 
    MsaMeStartupMode mode, MmsFileType type);
void msaSeDeleteNavMenu(void);

#endif
