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

/* !\file mauicomm.c
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

/* SIS */
#include "Sis_Cfg.h"
#include "Sis_If.h"

/* MSA */
#include "Msa_Rc.h"
#include "Msa_Intsig.h"
#include "Msa_Def.h"
#include "Msa_Types.h"
#include "Msa_Uicmn.h"
#include "Msa_Utils.h"
#include "Msa_Mem.h"
#include "Msa_Uidia.h"


/*!
 * \brief Creates a transaction request dialog
 *
 * \param fsm The FSM to report the answer to.
 * \param signal The signal to send.
 * \param strId1 The first predefined string to use
 * \param strId2 The second predefined string to use
 * \param size The size of the message to download.
 * \return TRUE if the dialog is displayed successfully, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL msaShowTransferDialog(MsaStateMachine fsm, unsigned int signal, 
    WE_UINT32 strId1, WE_UINT32 strId2, unsigned int size)
{
    char *str1;
    char *str3;
    char *newStr;
    char buffer[10*sizeof(char) + sizeof('\0')];

    str1 = msaGetStringFromHandle(MSA_GET_STR_ID(strId1));
    str3 = msaGetStringFromHandle(MSA_GET_STR_ID(strId2));
    sprintf(buffer, "%d", size);
    if (NULL == (newStr = we_cmmn_str3cat(WE_MODID_MSA, str1, buffer, str3)))
    {
        return FALSE;
    }
    MSA_FREE(str1);
    MSA_FREE(str3);
    if (!msaShowConfirmDialog(0, newStr, fsm, (int)signal, 0))
    {
        /*lint -e{774} */
        MSA_FREE(newStr);
        return FALSE;
    }
    /*lint -e{774} */
    MSA_FREE(newStr);
    return TRUE;
}


/*!
 * \brief Creates a download request dialog
 *
 * \param fsm The FSM to report the answer to.
 * \param signal The signal to send.
 * \param size The size of the message to download.
 * \return TRUE if the dialog is displayed successfully, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaShowHomeRetrievalDialog(MsaStateMachine fsm, unsigned int signal, 
    unsigned int size)
{
    return msaShowTransferDialog(fsm, signal, MSA_STR_ID_DOWNLOAD_REQ1,
        MSA_STR_ID_DOWNLOAD_REQ2, size);
}


/*!
 * \brief Creates a download request dialog for use when roaming
 *
 * \param fsm The FSM to report the answer to.
 * \param signal The signal to send.
 * \param size The size of the message to download.
 * \return TRUE if the dialog is displayed successfully, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaShowRoamingRetrievalDialog(MsaStateMachine fsm, 
    unsigned int signal, unsigned int size)
{
    return msaShowTransferDialog(fsm, signal, MSA_STR_ID_ROAM_DOWNLOAD_REQ1,
        MSA_STR_ID_ROAM_DOWNLOAD_REQ2, size);
}

/*!
 * \brief Creates a send request dialog for use when roaming
 *
 * \param fsm The FSM to report the answer to.
 * \param signal The signal to send.
 * \param size The size of the message to download.
 * \return TRUE if the dialog is displayed successfully, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaShowRoamingSendDialog(MsaStateMachine fsm, unsigned int signal,
                                  unsigned int size)
{
    return msaShowTransferDialog(fsm, signal, MSA_STR_ID_ROAM_SEND_REQ1,
        MSA_STR_ID_ROAM_SEND_REQ2, size);

}

/*!
 * \brief Creates a network settings error dialog
 *
 * \param fsm The FSM to report the answer to.
 * \param signal The signal to send.
 * \return TRUE if the dialog is displayed successfully, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaShowSettingsErrorDialog(MsaStateMachine fsm, int signal)
{
    return msaShowDialogWithCallback(MSA_GET_STR_ID(
        MSA_STR_ID_INVALID_NETWORK_SETTINGS), MSA_DIALOG_ERROR, fsm, signal);
}
