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

/*! \file mame.h
 *  \brief Message editor (properties) control logic. 
 */

#ifndef _MAME_H_
#define _MAME_H_

/******************************************************************************
 * Constants
 *****************************************************************************/

/*!\enum MsaMeSignalId
 * Signals for the ME FSM */
typedef enum 
{
    /*! Starts the message editor 
     *  u_param1 = startupMode 
     *  u_param2 = message wid
     *  p_param  = content routing info, see #MsaMeImg.
     */
    MSA_SIG_ME_ACTIVATE,

    /*! Response from the getMessageInfo request
     *  p_param = see #MmsListOfProperties.
     */
    MSA_SIG_ME_GET_MSG_INFO_RSP,

    /*! Start reading message from disk to internal representation
     *  params: none used
     */
    MSA_SIG_ME_MR_START,    

    /*! Reading of message from disk completed
     *  u_param1 = the result, see #MsaMrResult
     *  u_param2 = number of objects exceeding size limitations 
     *  p_param = the retrieved message, see #MsaMessage
     */
    MSA_SIG_ME_MR_DONE,    

    /*! Deactivate the message editor
     *  params: none used
     */
    MSA_SIG_ME_DEACTIVATE,

    /*! Edit content of current message
     *  params: none used
     */
    MSA_SIG_ME_EDIT_CONTENT,

    /*! Edit properties of current message
     *  params: none used
     */
     MSA_SIG_ME_EDIT_PROPERTIES,

    /*! Edit a property of the current message (TO/CC/BCC/Subject)
     *  u_param1: The property to edit, see #MsaMeProperty
     */
     MSA_SIG_ME_EDIT_PROPERTY,

    /*! Sending and saving of the current message, 
     *  u_param1 = requesting fsm 
     *  u_param2 = callback signal 
     */
    MSA_SIG_ME_SAVE,
        
    /*! Save the current message as a template,
     *  u_param1 = requesting fsm 
     *  u_param2 = callback signal 
     */
    MSA_SIG_ME_SAVE_AS_TEMPLATE,

    /*! Save the current template,
     *  u_param1 = requesting fsm 
     *  u_param2 = callback signal 
     */
    MSA_SIG_ME_SAVE_TEMPLATE,
     
    /*! Send the current message,
     *  u_param1 = requesting fsm 
     *  u_param2 = callback signal 
     */
    MSA_SIG_ME_SEND,

    /*! Saves the current message temporarily so that it can be previewed,
     *  u_param1 = requesting fsm 
     *  u_param2 = callback signal 
     */
    MSA_SIG_ME_SAVE_PREVIEW,

    /*! Callback signal used for message saving through MCR FSM. 
     *  i_param  = the message type, see #MmsFileType.
     *  u_param1 = result, see #MsaMcrResult, 
     *  u_param2 = msgId
     */
    MSA_SIG_ME_SAVE_DONE,

    /*! This signal is sent to the ME when the message has been sent
     *  u_param1 = result, see #MsaSendResult
     *  u_param2 = msgId  
     */
    MSA_SIG_ME_SEND_DONE,
    
    /*---- Navigation menu ----*/

    /*! Show navigation menu
     *  params: none used
     */
    MSA_SIG_ME_NAV_ACTIVATE,

    /*! Deleted the navigation menu
     *  params: none used
     */
    MSA_SIG_ME_NAV_DEACTIVATE,

    /*! Signal sent to the MSA_ME_FSM when the End User presses ok during
     *    creation of message properties.
     *  params: none used
     */
    MSA_SIG_ME_PROP_NEXT,
    
    /*! Signal sent to the MSA_ME_FSM when the End User presses back or chooses
     *    back in the navigation menu 
     *  params: none used
     */
    MSA_SIG_ME_PROP_BACK,
    
    /*! Deactivates the address input view
     *  params: none used
     */
    MSA_SIG_ME_ADDR_INPUT_DEACTIVATE,
    
    /*! Adds one or more recipient address to the current message
     *  p_param  = The addresses to append. See #MsaAddrItem
     *  u_param1 = The address to change (To or Cc), see #MsaMeProperty
     */
    MSA_SIG_ME_ADD_RECEPIENTS,

    /*! Changes a recipient address
     *  u_param1 = The address to change (To or Cc), see #MsaMeProperty
     *  u_param2 = The index of the item to change
     *  p_param  = The new entry
     */
    MSA_SIG_ME_CHANGE_RECEPIENT,

    /*! Address lookup 
     *  p_param = The lookup entry, see #MsaAddrItem
     */
    MSA_SIG_ME_LOOKUP_RSP,

    /*! Deactivates the address list view.
     *  params: none used
     */
    MSA_SIG_ME_ADDR_LIST_DEACTIVATE,

    /*! Activates the navigation menu for the address list view menu.
     *  params: none used
     */
    MSA_SIG_ME_ADDR_LIST_MENU_ACTIVATE,

    /*! Deactivates the navigation menu for the address list view menu.
     *  params: none used
     */
    MSA_SIG_ME_ADDR_LIST_MENU_DEACTIVATE,

    /*! Deletes a address from the list of addresses.
     *  u_param1 = The field type, MSA_ME_TO or MSA_ME_CC
     *  u_param2 = 
     */
    MSA_SIG_ME_DELETE_ADDR_ITEM,

    /*! A send or save requested from the property editor has finished
     *	u_param1 = the result as an WE_BOOL
     */
     MSA_SIG_ME_PROP_SAVE_DONE,
     MSA_SIG_ME_PROP_SAVE_AS_TEMPLATE_DONE,
     MSA_SIG_ME_PROP_SAVE_TEMPLATE_DONE,
     MSA_SIG_ME_PROP_SEND_DONE,

    /*! A send or save requested from the navigation menu has finished
     *	u_param1 = the result as an WE_BOOL
     */
     MSA_SIG_ME_MENU_SAVE_DONE,
     MSA_SIG_ME_MENU_SAVE_AS_TEMPLATE_DONE,
     MSA_SIG_ME_MENU_SAVE_TEMPLATE_DONE,
     MSA_SIG_ME_MENU_SEND_DONE
}MsaMeSignalId;

/*!\enum MsaMeStartupMode 
 * \brief Startup message types for the ME 
 */
typedef enum
{
	MSA_ME_STARTUP_NEW_MIXED = 0x01,    /*!< New multi-part mixed message */
    MSA_ME_STARTUP_NEW_SMIL,            /*!< New SMIL message             */
    MSA_ME_STARTUP_EDIT,				/*!< Edit message                 */
    MSA_ME_STARTUP_FORWARD,             /*!< Forward message              */
    MSA_ME_STARTUP_REPLY,               /*!< Reply to message             */
    MSA_ME_STARTUP_REPLY_ALL,           /*!< Reply to message             */
    MSA_ME_STARTUP_NEW_TEMPLATE,        /*!< Create a template message    */
    MSA_ME_STARTUP_FROM_TEMPLATE        /*!< Create message from template */
}MsaMeStartupMode;

/* !\enum MsaMeProperty
 *  \brief The different types of input fields in the message property editor
 */
typedef enum
{
    MSA_ME_NONE,    /*!< None of the properties selected */
    MSA_ME_TO,      /*!< The To field */
    MSA_ME_CC,      /*!< The Cc field */
    MSA_ME_BCC,     /*!< The Bcc field */
    MSA_ME_SUBJECT  /*!< The Subject field */
}MsaMeProperty;

/* Maximum size for the subject in characters */
#define MSA_ME_SUBJECT_MAX_SIZE         40

/* Checks if a new SMIL message should be created */
#define MSA_ME_IS_NEW_SMIL_MSG(m) ((MSA_ME_STARTUP_NEW_SMIL == m) ||\
    (MSA_ME_STARTUP_REPLY == m) || (MSA_ME_STARTUP_NEW_TEMPLATE == m))

/* Allow to send the message or not */
#define MSA_ME_ALLOW_SEND(m, t)    ((MSA_ME_STARTUP_NEW_TEMPLATE != m) && \
    (MSA_ME_STARTUP_EDIT != m) || (t == MMS_SUFFIX_SEND_REQ))

/* Allow the  "save to template" operation *//* TR 18015 */
#define MSA_ME_ALLOW_SAVE_AS_TEMPLATE(m)  ((MSA_ME_STARTUP_NEW_SMIL == m) ||\
    (MSA_ME_STARTUP_EDIT == m) || (MSA_ME_STARTUP_FROM_TEMPLATE == m) || (MSA_ME_STARTUP_NEW_TEMPLATE == m))

/* Allow to save template */
#define MSA_ME_ALLOW_SAVE_TEMPLATE(m)  ((MSA_ME_STARTUP_NEW_SMIL == m) ||\
    (MSA_ME_STARTUP_EDIT == m) || (MSA_ME_STARTUP_FROM_TEMPLATE == m) || (MSA_ME_STARTUP_NEW_TEMPLATE == m))

/* Allow to save a message
 *//* TR 18015 */
#define MSA_ME_ALLOW_SAVE(m, t) ((m != MSA_ME_STARTUP_FORWARD) &&\
            (t != MMS_SUFFIX_TEMPLATE) && (t != MMS_SUFFIX_MSG) && (MSA_ME_STARTUP_NEW_TEMPLATE != m) )

/******************************************************************************
 * Data-structures
 *****************************************************************************/

/* !\struct MsaMeInstance */
typedef struct
{
    MsaMeStartupMode    mode;           /*!< The start-up mode for the ME */
    MmsMsgId            origMsgId;      /*!< The msg wid of the originating msg*/
    MmsFileType         origFileType;   /*!< Originating file type */
    MmsTimeSec          lastSaved;      /*!< Last saved time */
    WE_BOOL            sendFlg;        /*!< Whether to send when created */
    MsaCtContentInsert  *ct;            /*!< Temporary storage when inserting*/

    MsaMessage          *theMessage;    /*!< The message being edited */
    MsaStateMachine     mcrFsm;         /*!< Callback FSM */
    int                 mcrSig;         /*!< Callback signal */
    
}MsaMeInstance;

/******************************************************************************
 * Prototype declarations 
 *****************************************************************************/

void msaMeInit(void); 
void msaMeTerminate(void);
void msaMeTerminateGui(void);

#endif /* _MAME_H_ */
