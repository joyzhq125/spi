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

/*!\file mauimv.c
 * \brief The graphical user interface for the message property viewer.
 */

/* WE */
#include "We_Wid.h"
#include "We_Int.h"
#include "We_Mem.h"
#include "We_Cfg.h"
#include "We_Def.h"
#include "We_Lib.h"
#include "We_Cmmn.h"

/* MMS */
#include "Mms_Def.h"

/* MSA*/
#include "Msa_Cfg.h"
#include "Msa_Rc.h"
#include "Msa_Types.h"
#include "Msa_Intsig.h"
#include "Msa_Mr.h"
#include "Msa_Mv.h"
#include "Msa_Uimv.h"
#include "Msa_Mem.h"
#include "Msa_Uicmn.h"
#include "Msa_Uisig.h"
#include "Msa_Uimenu.h"
#include "Msa_Del.h"
#include "Msa_Addr.h"

/******************************************************************************
 * Constants
 *****************************************************************************/

/******************************************************************************
 * Data-structures
 *****************************************************************************/

/*!\struct MsaMvPropView
 *  \brief Message property viewer widget handles 
 */
typedef struct
{
    WeWindowHandle formHandle;         /*!< The handle of the form */

    WeGadgetHandle fromHandle;         /*!< The "From" gadget handle */
    WeGadgetHandle fromTitleHandle;    /*!< The "From" gadget handle */
    WeGadgetHandle toHandle;           /*!< The "To" gadget handle */
    WeGadgetHandle toTitleHandle;      /*!< The "To" gadget handle */
    WeGadgetHandle ccHandle;           /*!< The "Cc" gadget handle */
    WeGadgetHandle ccTitleHandle;      /*!< The "Cc" gadget handle */
    WeGadgetHandle subjectHandle;      /*!< The "Subject" gadget handle */
    WeGadgetHandle subjectTitleHandle; /*!< The "Subject" gadget handle */
}MsaMvPropView;

/*! \struct MsaMvRrDrView
 *  \brief Handles for read/delivery reports */
typedef struct
{
    WeWindowHandle formHandle;         /*!< The form handle */

    WeGadgetHandle fromHandle;         /*!< The from gadget handle */
    WeGadgetHandle fromTitleHandle;    /*!< The from gadget handle */
    WeGadgetHandle subjectHandle;      /*!< The subject gadget handle */
    WeGadgetHandle subjectTitleHandle; /*!< The subject gadget handle */
    WeGadgetHandle statusHandle;       /*!< The status gadget handle */
    WeGadgetHandle statusTitleHandle;  /*!< The status gadget handle */
    WeGadgetHandle dateHandle;         /*!< The date gadget handle */
    WeGadgetHandle dateTitleHandle;    /*!< The date gadget handle */
}MsaMvRrDrView;

/******************************************************************************
 * Static Data-structures
 *****************************************************************************/

/******************************************************************************
 * Data-structures for the user interface
 *****************************************************************************/

static MsaMvPropView    msaMvPropView;
static MsaMvRrDrView    msaMvRrDrView;

/******************************************************************************
 * Prototypes
 *****************************************************************************/

static WE_BOOL createMenuItems(MsaMessageType messageType);
static void navMenuSignalConverter(MsaNavAction actionType);
static WE_BOOL createStringGadget(WeWindowHandle win, 
    const WeStringHandle *altStrHandle, const char *str, 
    WE_UINT32 titleStrId, WeGadgetHandle *gadget, WePosition *pos, 
    WeSize *newSize, WeGadgetHandle *titleGadget);

/******************************************************************************
 * Function implementation
 *****************************************************************************/


/*!
 * \brief Creates a string gadget.
 *
 * \param str The string to be displayed in the string gadget.
 * \param titleStrId The sting wid for the title.
 * \param size The size of the gadget.
 * \return The gadget-handle if successful, otherwise 0.
 *****************************************************************************/
static WE_BOOL createStringGadget(WeWindowHandle win, 
    const WeStringHandle *altStrHandle,
    const char *str, WE_UINT32 titleStrId, WeGadgetHandle *gadget, 
    WePosition *pos, WeSize *size, WeGadgetHandle *titleGadget)
{
    WeSize valueSize;
    WeSize dSize;
    WeSize titleSize;
    WePosition dPos;
    WE_UINT16 width;
    WeStringHandle strHandle;

    if (0 == altStrHandle)
    {
        if (NULL == str)
        {
            strHandle = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, (char *)"", 
                WeUtf8, strlen("") + 1, 0);
        }    
        else
        {
            strHandle = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, str, WeUtf8, 
                (int)strlen(str)+ 1, 0);
        }
    }
    else
    {
        strHandle = MSA_GET_STR_ID(*altStrHandle);
        
    }
    if (0 == strHandle)
    {
        return FALSE;
    }
    (void)WE_WIDGET_GET_INSIDE_AREA(win, &dPos, &dSize);
    if (0 == (*titleGadget = WE_WIDGET_STRING_GADGET_CREATE(WE_MODID_MSA,
        MSA_GET_STR_ID(titleStrId), NULL, 0, FALSE, 0, 0)))
    {
        (void)WE_WIDGET_RELEASE(strHandle);
        return FALSE;
    }
    pos->x = (WE_UINT16)(MSA_GADGET_DEF_PADDING + dPos.x);
    if (0 > WE_WIDGET_WINDOW_ADD_GADGET(win, *titleGadget, pos)) /* Q04A */
    {
        return FALSE;
        
    }
    (void)WE_WIDGET_GET_SIZE(*titleGadget, &titleSize);

    width = (WE_UINT16)(dSize.width - titleSize.width - 3*MSA_GADGET_DEF_PADDING);
    if (0 == (*gadget = WE_WIDGET_STRING_GADGET_CREATE(WE_MODID_MSA,
        strHandle, NULL, width, FALSE, 0, 0)))
    {
        (void)WE_WIDGET_RELEASE(strHandle);
        return FALSE;
    }
    if (WE_WIDGET_STRING_GET_LENGTH(strHandle, 0, WeUtf8) > 0)
    {
        if (0 > WE_WIDGET_GADGET_SET_PROPERTIES(*gadget,
            WE_GADGET_PROPERTY_FOCUS, 0)) /* Q04A */
        {
            (void)WE_WIDGET_RELEASE(*gadget);
            return FALSE;
        }
    }
    (void)WE_WIDGET_GET_SIZE(*gadget, &valueSize);

    pos->x = (WE_INT16)(pos->x + titleSize.width + MSA_GADGET_DEF_PADDING);
    if (0 > WE_WIDGET_WINDOW_ADD_GADGET(win, *gadget, pos)) /* Q04A */
    {
        return FALSE;
        
    }
    size->height = (WE_UINT16)MAX(valueSize.height, titleSize.height);
    return TRUE;
}

/*!
 * \brief Creates the GUI for the message property dialog.
 *
 * \param propItem The current message properties.
 * \param fsmOk The fsm to send a signal to when a "ok" action is generated.
 * \param sigOk The signal to send when a "ok" action is generated.
 * \param fsmBack The fsm to send signal to when a "back" action is generated.
 * \param sigBack The signal to send when a "back" action is generated.
 * \return TRUE if the operation was successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaCreateMvPropView(MmsFileType fileType, const MsaPropItem *propItem, 
                             int fsmOk, int sigOk, int fsmBack, int sigBack)
{
    WeSize newSize = {0, 0};
    WePosition pos = {MSA_GADGET_DEF_PADDING, MSA_GADGET_DEF_PADDING};
    char *str;
    if (0 != msaMvPropView.formHandle)
    {
        return FALSE;
    }
    memset(&msaMvPropView, 0, sizeof(MsaMvPropView));
    /* Create the form */
    if (0 == (msaMvPropView.formHandle = WE_WIDGET_FORM_CREATE(WE_MODID_MSA, 
        NULL, 0, 0)))
    {
        return FALSE;
    }
    /* Actions */
    if (0 == msaCreateAction(msaMvPropView.formHandle, WeOk, NULL, 
        fsmOk, sigOk, MSA_STR_ID_NEXT))
    {
        return FALSE;
    }
    if (0 == msaCreateAction(msaMvPropView.formHandle, WeBack, NULL, 
        fsmBack, sigBack, MSA_STR_ID_BACK))
    {
        return FALSE;
    }
    if (0 == msaCreateAction(msaMvPropView.formHandle, WeMenu, NULL, 
        MSA_MV_FSM, MSA_SIG_MV_NAV_ACTIVATE, MSA_STR_ID_MENU))
    {
        return FALSE;
    }
    
    /* From name/address*/
    /* Do only display the from address if the message is sent from another
       terminal */
    if (MMS_SUFFIX_MSG == fileType)
    {
        str = Msa_AddrItemToStr(propItem->from);
        if (!createStringGadget(msaMvPropView.formHandle, NULL, str, 
            MSA_STR_ID_FROM, &msaMvPropView.fromHandle, &pos, &newSize,
            &msaMvPropView.fromTitleHandle))
        {
            MSA_FREE(str);
            return FALSE;
        }
        MSA_FREE(str);
        pos.y = (WE_INT16)((int)pos.y + (int)newSize.height + 
            MSA_GADGET_DEF_PADDING);
    }

    /*
     *	TO addresses 
     */
    str = Msa_AddrItemToStr(propItem->to);
    if (!createStringGadget(msaMvPropView.formHandle, NULL, str, MSA_STR_ID_TO, 
        &msaMvPropView.toHandle, &pos, &newSize,
        &msaMvPropView.toTitleHandle))
    {
        MSA_FREE(str);
        return FALSE;
    }
    MSA_FREE(str);
    pos.y = (WE_INT16)((int)pos.y + (int)newSize.height + 
        MSA_GADGET_DEF_PADDING);

    /* CC addresses */
    str = Msa_AddrItemToStr(propItem->cc);
    if (!createStringGadget(msaMvPropView.formHandle, NULL, str, MSA_STR_ID_CC, 
        &msaMvPropView.ccHandle, &pos, &newSize,
        &msaMvPropView.ccTitleHandle))
    {
        MSA_FREE(str);
        return FALSE;
    }
    MSA_FREE(str);
    pos.y = (WE_INT16)((int)pos.y + (int)newSize.height + 
        MSA_GADGET_DEF_PADDING);
    /* Subject */
    if (!createStringGadget(msaMvPropView.formHandle, NULL, propItem->subject, 
        MSA_STR_ID_SUBJECT, &msaMvPropView.subjectHandle, &pos, &newSize, 
        &msaMvPropView.subjectTitleHandle))
    {
        return FALSE;
    }

    msaSetTitle(msaMvPropView.formHandle, 
        MSA_GET_STR_ID(MSA_STR_ID_VIEW_TITLE), 0);
    
    return msaDisplayWindow(msaMvPropView.formHandle, msaGetPosLeftTop());
}

/*! \brief Set the message properties view.
 *
 * \param propItem The current message properties.
 *****************************************************************************/
WE_BOOL msaSetMvProperties(const MsaPropItem *propItem)
{
    char *str;
    if ((NULL != propItem) && (0 != msaMvPropView.formHandle))
    {
        if (0 != msaMvPropView.fromHandle)
        {
            str = Msa_AddrItemToStr(propItem->from);
            if (!msaSetStringGadgetValue(msaMvPropView.fromHandle, 0, str))
            {
                MSA_FREE(str);
                return FALSE;
            }
            MSA_FREE(str);
        }
        if (0 != msaMvPropView.toHandle)
        {
            str = Msa_AddrItemToStr(propItem->to);
            if (!msaSetStringGadgetValue(msaMvPropView.toHandle, 0, str))
            {
                MSA_FREE(str);
                return FALSE;
            }
            MSA_FREE(str);
        }
        str = Msa_AddrItemToStr(propItem->cc);
        if (!msaSetStringGadgetValue(msaMvPropView.ccHandle, 0, str))
        {
            MSA_FREE(str);
            return FALSE;
        }
        MSA_FREE(str);
        if (!msaSetStringGadgetValue(msaMvPropView.subjectHandle, 
            0, propItem->subject))
        {
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}


/*!
 * \brief Initiates the data-structures for the message viewer.
 *****************************************************************************/
void msaMvUiInit(void)
{
    /* Init widget data-structures */
    memset(&msaMvPropView, 0, sizeof(MsaMvPropView));
    memset(&msaMvRrDrView, 0, sizeof(MsaMvRrDrView));
}

/*!
 * \brief Deletes the property view.
 *
 *****************************************************************************/
void msaDeleteMvPropView(void)
{
    /* Delete form and actions */
    (void)msaDeleteWindow(msaMvPropView.formHandle);
    /* Delete gadgets */
    if (0 != msaMvPropView.fromHandle)
    {
        (void)WE_WIDGET_RELEASE(msaMvPropView.fromHandle);
    }
    if (0 != msaMvPropView.toHandle)
    {
        (void)WE_WIDGET_RELEASE(msaMvPropView.toHandle);
    }
    if (0 != msaMvPropView.ccHandle)
    {
        (void)WE_WIDGET_RELEASE(msaMvPropView.ccHandle);
    }
    if (0 != msaMvPropView.subjectHandle)
    {
        (void)WE_WIDGET_RELEASE(msaMvPropView.subjectHandle);
    }
    if (0 != msaMvPropView.fromTitleHandle)
    {
        (void)WE_WIDGET_RELEASE(msaMvPropView.fromTitleHandle);
    }  
    if (0 != msaMvPropView.toTitleHandle)
    {
        (void)WE_WIDGET_RELEASE(msaMvPropView.toTitleHandle);
    }
    if (0 != msaMvPropView.ccTitleHandle)
    {
        (void)WE_WIDGET_RELEASE(msaMvPropView.ccTitleHandle);
    }
    if (0 != msaMvPropView.subjectTitleHandle)
    {
        (void)WE_WIDGET_RELEASE(msaMvPropView.subjectTitleHandle);
    }
    /* Delete obsolete handles */
    memset(&msaMvPropView, 0, sizeof(MsaMvPropView));
}

/*!
* \brief Creates the GUI for read or delivery reports.
*
* \param titleStrId The form title.
* \param from The from address.
* \param subject The subject.
* \param statusStrId The status of the message.
* \param date The date of the message
* \return TRUE if successful, otherwise FALSE.
*****************************************************************************/
WE_BOOL msaCreateMvRrDrView(WE_UINT32 titleStrId, const char *from,
                             WE_UINT32 statusStrId, const char *date)
{
    WeSize newSize = {0, 0};
    WePosition pos = {0, 0};
    if (0 != msaMvRrDrView.formHandle)
    {
        return FALSE;
    }
    
    memset(&msaMvRrDrView, 0, sizeof(MsaMvRrDrView));
    
    if (0 == (msaMvRrDrView.formHandle = WE_WIDGET_FORM_CREATE(WE_MODID_MSA, 
        NULL, 0, 0)))
    {
        return FALSE;
    }
    /* Actions */
    if (0 == msaCreateAction(msaMvRrDrView.formHandle, WeOk, 
        NULL, MSA_MV_FSM, MSA_SIG_MV_DEACTIVATE, MSA_STR_ID_OK))
    {
        return FALSE;
    }
    if (0 == msaCreateAction(msaMvRrDrView.formHandle, WeCancel,  
        NULL, MSA_MV_FSM, MSA_SIG_MV_DEACTIVATE, MSA_STR_ID_CANCEL))
    {
        return FALSE;
    }
    /* From address */
    if (!createStringGadget(msaMvRrDrView.formHandle, NULL, from, 
        MSA_STR_ID_FROM, &msaMvRrDrView.fromHandle, &pos, &newSize, 
        &msaMvRrDrView.fromTitleHandle))
    {
        return FALSE;
    }
    pos.y = (WE_INT16)((int)pos.y + (int)newSize.height +
        MSA_GADGET_DEF_PADDING);
    newSize.height = newSize.width = 0;
    
    /* Date */
    if (!createStringGadget(msaMvRrDrView.formHandle, NULL, date, 
        MSA_STR_ID_DATE, &msaMvRrDrView.dateHandle, &pos, &newSize,
        &msaMvRrDrView.dateTitleHandle))
    {
        return FALSE;
    }
    pos.y = (WE_INT16)((int)pos.y + (int)newSize.height +
        MSA_GADGET_DEF_PADDING);
    newSize.height = newSize.width = 0;

    /* Status */
    if (!createStringGadget(msaMvRrDrView.formHandle, &statusStrId, NULL, 
        MSA_STR_ID_STATUS, &msaMvRrDrView.statusHandle, &pos, &newSize,
        &msaMvRrDrView.statusTitleHandle))
    {
        return FALSE;
    }
    (void)WE_WIDGET_WINDOW_SET_PROPERTIES(msaMvRrDrView.formHandle, 
        WE_WINDOW_PROPERTY_TITLE, 0); /* Q04A */
    (void)WE_WIDGET_SET_TITLE(msaMvRrDrView.formHandle, 
        MSA_GET_STR_ID(titleStrId), 0); /* Q04A */
    
    return msaDisplayWindow(msaMvRrDrView.formHandle, msaGetPosLeftTop());
}

/*!
 * \brief Deletes the read/delivery report view.
 *
 *****************************************************************************/
void msaDeleteMvRrDrView()
{
    /* Delete form and actions */
    (void)msaDeleteWindow(msaMvRrDrView.formHandle);
    /* Delete gadgets */
    if (0 != msaMvRrDrView.dateHandle)
    {
        (void)WE_WIDGET_RELEASE(msaMvRrDrView.dateHandle);
    }
    if (0 != msaMvRrDrView.subjectHandle)
    {
        (void)WE_WIDGET_RELEASE(msaMvRrDrView.subjectHandle);
    }
    if (0 != msaMvRrDrView.statusHandle)
    {
        (void)WE_WIDGET_RELEASE(msaMvRrDrView.statusHandle);
    }
    if (0 != msaMvRrDrView.fromTitleHandle)
    {
        (void)WE_WIDGET_RELEASE(msaMvRrDrView.fromTitleHandle);
    }
    if (0 != msaMvRrDrView.dateTitleHandle)
    {
        (void)WE_WIDGET_RELEASE(msaMvRrDrView.dateTitleHandle);
    }
    if (0 != msaMvRrDrView.subjectTitleHandle)
    {
        (void)WE_WIDGET_RELEASE(msaMvRrDrView.subjectTitleHandle);
    }
    if (0 != msaMvRrDrView.statusTitleHandle)
    {
        (void)WE_WIDGET_RELEASE(msaMvRrDrView.statusTitleHandle);
    }
    /* Delete obsolete handles */
    memset(&msaMvRrDrView, 0, sizeof(MsaMvRrDrView));
}


/*!
 * \brief Creates the navigation menu for the message viewer.
 *
 * \param messageType The type of message that is currently active.
 * \return TRUE on success, otherwise FALSE. 
 *****************************************************************************/
static WE_BOOL createMenuItems(MsaMessageType messageType)
{
    if (MSA_MR_SMIL == messageType)
    {
        if (!msaAddNavMenuItem(MSA_GET_STR_ID(MSA_STR_ID_VIEW_SMIL), 
            MSA_MV_VIEW_CONTENT))
        {
            return FALSE;
        }
    }
    else if (MSA_MR_MULTIPART_ALTERNATIVE == messageType ||
        MSA_MR_MULTIPART_MIXED == messageType || 
        MSA_MR_PLAIN_TEXT == messageType)
    {
        if (!msaAddNavMenuItem(MSA_GET_STR_ID(MSA_STR_ID_VIEW_ATTACHMENTS),
            MSA_MV_VIEW_CONTENT))
        {
            return FALSE;
        }
    }

    if (!msaAddNavMenuItem(MSA_GET_STR_ID(MSA_STR_ID_BACK), MSA_MV_BACK))
    {
        return FALSE;
    }
    return TRUE;
}

/*!
 * \brief Signal converter for the navigation menu.
 *
 * \param actionType The type of action. See #MsaNavAction
 *****************************************************************************/
static void navMenuSignalConverter(MsaNavAction actionType)
{
    if (actionType == MSA_NAV_ACTION_OK)
    {
        switch(msaGetNavMenuItem())            
        {
        case MSA_MV_VIEW_CONTENT:
            (void)MSA_SIGNAL_SENDTO(MSA_MV_FSM, MSA_SIG_MV_VIEW_CONTENT);
            break;
        case MSA_MV_BACK:
            (void)MSA_SIGNAL_SENDTO(MSA_MV_FSM, MSA_SIG_MV_DEACTIVATE);
            (void)MSA_SIGNAL_SENDTO(MSA_MV_FSM, MSA_SIG_MV_NAV_DEACTIVATE);
            break;
        }
        (void)MSA_SIGNAL_SENDTO(MSA_MV_FSM, MSA_SIG_MV_NAV_DEACTIVATE);
    }
    if (actionType == MSA_NAV_ACTION_BACK)
    {
        (void)MSA_SIGNAL_SENDTO(MSA_MV_FSM, MSA_SIG_MV_NAV_DEACTIVATE);

    }
}

/*!
 * \brief Creates a navigation menu for the message viewer.
 *
 * \param messageType The type of message that are currently being displayed.
 *                    messageType can be #MSA_MR_PLAIN_TEXT, #MSA_MR_SMIL,
 *                    #MSA_MR_MULTIPART_MIXED
 * \return TRUE if the operation was successful, otherwise FALSE.
 *****************************************************************************/ 
WE_BOOL msaCreateMvNavMenu(MsaMessageType messageType)
{
    /* Create navigation menu */
    if (!msaCreateNavMenu(0, navMenuSignalConverter))
    {
        msaRemoveNavMenu();
        return FALSE;
    }
    else if (!createMenuItems(messageType))
    {
        msaRemoveNavMenu();
        return FALSE;
    }
    else if (!msaShowNavMenu())
    {
        msaRemoveNavMenu();
        return FALSE;
    }
    return TRUE;
}

/*!
 * \brief Deletes the navigation menu for the message viewer.
 *
 *****************************************************************************/
void msaDeleteMvNavMenu()
{
    msaRemoveNavMenu();
}

