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

/*! \file mamob.h
 *  \brief Media object browser
 */

#ifndef _MAMOB_H_
#define _MAMOB_H_

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! \enum MsaMobSignalId
 *  Signals for the media object viewer FSM */
typedef enum 
{
    /*! Activate the media object viewer.
     * p_param: structure containing startup parameters (see #MsaMobActivation)
     */
    MSA_SIG_MOB_ACTIVATE,
    
    /*! Deactivate the media object viewer.
     */
    MSA_SIG_MOB_DEACTIVATE,

    /*! Signal used by mob to add one media object at a time (time slicing) 
     */
    MSA_SIG_MOB_ADD_NEXT_MO,

    /*! Signal used by mod to add a special operation 
     */
    MSA_SIG_MOB_ADD_NEXT_SOP,

    /*! Signal used by mob to start creating window with widgets
    */
    MSA_SIG_MOB_FINALIZE_WINDOW,

    /*! Signal sent by gui to MOB when Menu button is pressed
     */
    MSA_SIG_MOB_MENU_SELECTED,

    /*! Signal sent by gui to MOB when Ok button is pressed
     */
    MSA_SIG_MOB_OK_SELECTED,

    /*! Signal sent by gui to MOB when focus is altered
     */
    MSA_SIG_MOB_FOCUS_ACTIVITY,

    /*! Signal sent to MOB when End User presses left on joystick 
     */
    MSA_SIG_MOB_KEY_LEFT,

    /*! Signal sent to MOB when End User presses right on joystick 
     */
    MSA_SIG_MOB_KEY_RIGHT

}MsaMobSignalId;

/*! \enum MsaMobOperation
 *  Operation in MOB that requires external activity */
typedef enum
{
    MSA_MOB_DEACTIVATED,   /*!< MOB deactivated, i.e. back pressed */
    MSA_MOB_SELECT,        /*!< Entry selected in MOB */
    MSA_MOB_MENU,          /*!< Menu button pressed on entry */
    MSA_MOB_SPECIAL_OP,    /*!< Special operation selected */
    MSA_MOB_FOCUS_ACTIVITY /*!< Changed focus within attachment list */
}MsaMobOperation;

/*! \enum MsaMobSpecialOp
 *  Internal, special operation, selectable through attachment list */
typedef enum
{
    MSA_MOB_SOP_NO_OP     = 0x0000, /*!< No op, just end of list marker */
    MSA_MOB_SOP_NEXT_PAGE = 0x0001, /*!< Next page selector */
    MSA_MOB_SOP_PREV_PAGE = 0x0002, /*!< Previous page selector */
    MSA_MOB_SOP_ADD_MO    = 0x0004  /*!< Add media object */
}MsaMobSpecialOp;

/*! Number of different special operations */
#define MSA_MOB_SOP_COUNT   3

/*! Maximum number of media objects to display per page  */
#define MSA_MAX_NUMBER_OF_MO_ON_PAGE 5

/*! Maximum total height in pixels of all image and text widgets per page */
#define MSA_MAX_PAGE_HEIGHT          100

/******************************************************************************
 * Data-structures
 *****************************************************************************/

/*! \struct MsaMobActivation
 *  Initialization parameters for activation of MOB */
typedef struct
{
    MsaMoDataItem   *moList;               /*!< media objects */
    MsaMobSpecialOp sop;                   /*!< special operations */    
    WeStringHandle winTitle;              /*!< window title */
    MsaStateMachine fsm;                   /*!< the fsm that gets MOB op 
                                                reports */
    int             signal;                /*!< the signal to use when sending
                                                MOB op reports */
}MsaMobActivation;

/******************************************************************************
 * Prototypes
 *****************************************************************************/

void msaMobInit(void);
void msaMobTerminate(void);
void msaMobActivate(MsaMoDataItem *moList, MsaMobSpecialOp sop, 
    WeStringHandle winTitle, MsaStateMachine fsm, int signal); 

#endif /* _MAMOB_H_ */
