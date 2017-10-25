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

/*!\file mauidia.c
 * \brief Dialog handling.
 */

/* WE */
#include "We_Lib.h"
#include "We_Wid.h"
#include "We_Int.h"
#include "We_Def.h"
#include "We_Log.h"
#include "We_Core.h"
#include "We_Mem.h"
#include "We_Dlg.h"

/* MMS */
#include "Mms_Def.h"

/* MSA */
#include "Msa_Rc.h"
#include "Msa_Intsig.h"
#include "Msa_Def.h"
#include "Msa_Types.h"
#include "Msa_Uisig.h"
#include "Msa_Uidia.h"
#include "Msa_Uicmn.h"
#include "Msa_Core.h"
#include "Msa_Mem.h"
#include "Msa_Pckg.h"

/******************************************************************************
 * Defines
 *****************************************************************************/
#define MSA_MAX_DIALOGS     3 /* !< Maximum number of dialogs queued */
#define IS_DIALOG_DISPLAYED (msaDialog != NULL)

/******************************************************************************
 * Data-types
 *****************************************************************************/

/*! \struct MsaDialog
 *  Data-structures for global dialog data */
typedef struct
{
    MsaDialogType       dialogType;     /* !< The type of dialog that is displayed */

    WE_INT32           dlgOperId;      /* !< Current operation  */

    WeStringHandle     stringHandle;   /* !< */
    
    MsaStateMachine     fsm;            /* !< Callback FSM */
    int                 signal;         /* !< Callback signal */
    unsigned int        wid;             /* !< WID */

}MsaDialog;

/*! \struct MsaDialogItem
 * Dialog structure, used for queuing dialog  */
typedef struct
{
    WeStringHandle strHandle;  /* !< The string to display */

    MsaDialogType   dialogType; /* !< The type of dialog that is displayed */
    MsaStateMachine fsm;        /* !< Callback FSM */
    int             signal;     /* !< Callback signal */
    unsigned int    wid;         /* !< Callback data */
}MsaDialogItem;

/*
 * Dialog package standard functions to be registered in the
 * package encapsulation.
 */
static const msa_pckg_fnc_list_t funcList = {
        we_dlg_init, 
        we_dlg_handle_signal,
        we_dlg_get_private_data, 
        we_dlg_set_private_data,
        we_dlg_terminate, 
        we_dlg_delete,
        we_dlg_get_result_ext,
        we_dlg_result_free
};

/******************************************************************************
 * Static variables
 *****************************************************************************/

static MsaDialog            *msaDialog;     /*!< The current dialog  */
static MsaDialogItem        msaDialogQueue[MSA_MAX_DIALOGS];    /*!< The dialog queue  */
static MsaDialog            *msaPanicDialog;        /*!< The exception dialog */
static WE_BOOL             terminating;            /*!< Tells if the module is going down */

/* Package data */
static msa_pckg_handle_t    *pHandle;       /* Package encapsulation handle */
static msa_pkg_id_t         dlgPckId;       /* Dialog package WID */
static msa_pkg_handle_t     *dlgHandle;     /* !< Dialog instance WID */

/******************************************************************************
 * Prototypes
 *****************************************************************************/

static WE_BOOL addDialogItem(MsaDialogType dialogType, 
    WeStringHandle strHandle, MsaStateMachine fsm, int signal, unsigned int wid);
static WE_BOOL showDialog(void);
static WE_BOOL msaDeleteDialog(MsaDialog **dia);

/******************************************************************************
 * Function implementations
 *****************************************************************************/

/*! \brief MSA package operation completion entry point function.
 *
 * \param fsm      Fsm to forward signal to.
 * \param signal   Signal to send to fsm.
 * \param pkg_inst Package instance to completed operation.
 * \param wid       Package operation WID.
 * \param result   Package operation result (type-cast per operation)
 * \param data     Private data provided at registration of operation.
 *
 * \return 0.
 *
 * NOTE: The result is deallocated when this function returns.
 *       If result data is to be kept, either copy or snatch pointer
 *       and set it to NULL (so that the free operation not can free it). 
 *****************************************************************************/
/*lint -e{818} */
static int msaDiaPackSignalHand(WE_INT32 fsm, WE_INT32 signal,
    msa_pkg_id_t pckg_id, msa_pkg_handle_t *pkg_inst, WE_INT32 wid, 
    msa_pckg_result_t *result, void *data)
{
    we_dlg_result_message_t *res;
    we_dlg_result_confirm_t *confRes;

    (void)data;
    (void)pckg_id;
    (void)signal;
    (void)fsm;

    if ((NULL != msaDialog) && (dlgHandle == pkg_inst) && 
        (msaDialog->dlgOperId == wid))
    {
        if (MSA_DIALOG_CONFIRM == msaDialog->dialogType)
        {
            confRes = msa_pckg_get_result_data(result);
            if (WE_DLG_RETURN_NOT_AVAILABLE == confRes->result)
            {
                return FALSE;
            }
            else
            {
                /* Operation finnished, report to caller */
                (void)MSA_SIGNAL_SENDTO_UU(msaDialog->fsm, msaDialog->signal, 
                    (WE_DLG_RETURN_OK == confRes->result) ? MSA_CONFIRM_DIALOG_OK: 
                    MSA_CONFIRM_DIALOG_CANCEL, msaDialog->wid);
            }
        }
        else
        {
            /* Message dialog operation */
            res = msa_pckg_get_result_data(result);
            if (res->result != WE_DLG_RETURN_NOT_AVAILABLE)
            {
                /* Check for call-back */
                if (MSA_NOTUSED_FSM != msaDialog->fsm)
                {
                    (void)MSA_SIGNAL_SENDTO(msaDialog->fsm, msaDialog->signal);
                }
            }
        }
        /* Clean-up the instance */
        (void)msaDeleteDialog(&msaDialog);
        /* Next dialog */
        (void)showDialog();
        return TRUE;
    }else if ((dlgHandle == pkg_inst) && 
              (msaPanicDialog->dlgOperId == wid))
    {
        /* The exception dialog has been closed, terminate the application */
        (void)MSA_SIGNAL_SENDTO(MSA_CORE_FSM, MSA_SIG_CORE_TERMINATE_APP);
        return TRUE;
    }
    return FALSE;
}

/*!
 * \brief Add a dialog to the dialog queue.
 *
 * \param dialogType The type of dialog to display.
 * \param textStrId The string WID of the string to display in the dialog.
 * \param fsm       The callback FSM
 * \param signal    The callback signal
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL addDialogItem(MsaDialogType dialogType, 
    WeStringHandle strHandle, MsaStateMachine fsm, int signal, unsigned int wid)
{
    int i;
    for(i = 0; i < MSA_MAX_DIALOGS; i++)
    {
        /* Find an empty slot */
        if (0 == msaDialogQueue[i].strHandle)
        {
            msaDialogQueue[i].strHandle  = strHandle;
            msaDialogQueue[i].dialogType = dialogType;
            msaDialogQueue[i].fsm        = fsm;
            msaDialogQueue[i].signal     = signal;
            msaDialogQueue[i].wid         = wid;
            return TRUE;
        }
    }
    return FALSE;
}

/*!
 * \brief Deletes the dialog with dialogHandle .
 *
 * \param dialogHandle The handle of the window that is to be deleted.
 * \return TRUE if the window has been deleted, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL msaDeleteDialog(MsaDialog **dia)
{
    /* Message dialogs */
    if ((NULL != dia) && (*dia != NULL))
    {
        /* Check if the string-handle needs to be deleted */
        if (MSA_DIALOG_CONFIRM == (*dia)->dialogType)
        {
            /* Only confirm dialog has dynamic string resources */
            (void)WE_WIDGET_RELEASE((*dia)->stringHandle);
        }
        MSA_FREE(*dia);
        *dia = NULL;
        return TRUE;
    }
    return FALSE;
}

/*!
 * \brief Initiates the data-structures for the dialog handling.
 *****************************************************************************/
WE_BOOL msaInitDia(void)
{
    /* Initiate encapsulation */
    if (NULL == (pHandle = msa_pckg_create(WE_MODID_MSA)))
    {
        return FALSE;
    }
    /* Register packages */
    if (0 == (dlgPckId = msa_pckg_reg_pckg(pHandle, &funcList)))
    {
        return FALSE;
    }
    /* Sets callback function */
    if (!msa_pckg_set_response_handler(pHandle, msaDiaPackSignalHand))
    {
        return FALSE;
    }
    if (NULL == (dlgHandle = msa_pckg_create_inst(pHandle, dlgPckId)))
    {
        return FALSE;
    }

    /* Allocate without exceptionhandler during init */
    if (NULL == (msaPanicDialog = MSA_ALLOC_NE(sizeof(MsaDialog))))
    {
        return FALSE;
    }
    memset(msaPanicDialog, 0, sizeof(MsaDialog));
    /* Set-up the "panic" dialog...it must be allocated directly so that it can
       be displayed whithout allocation to much memory */
    msaPanicDialog->stringHandle = MSA_GET_STR_ID(MSA_STR_ID_PANIC);

    /* Set global variables to usable values */
    terminating = FALSE;
    memset(msaDialogQueue, 0, sizeof(msaDialogQueue));
    msaDialog = NULL;

    return TRUE;
}

/*!
 * \brief Creates and displays a global dialog.
 *
 * \param strHandle The text that is presented.
 * \param dialogType The type of dialog that is presented, see #MsaDialogType.
 * \return TRUE if the dialog was created, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaShowDialog(WeStringHandle strHandle, MsaDialogType dialogType)
{
    return msaShowDialogWithCallback(strHandle, dialogType, MSA_NOTUSED_FSM,
        0);
}

/*!
 * \brief Creates and displays a global dialog. 
 *
 * \param strHandle The text that is presented
 * \param dialogType The type of dialog that is presented, see #MsaDialogType.
 * \param fsm The fsm to call when dialog is closed.
 * \param signal The signal to send to the fsm when the dialog is closed.
 * \return TRUE if the dialog was created, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaShowDialogWithCallback(WeStringHandle strHandle, 
    MsaDialogType dialogType, MsaStateMachine fsm, int signal)
{    
    if (!WE_WIDGET_HAS_FOCUS(msaGetScreenHandle()))
    {
        /* Do not display the dialog if the screen is not visible */ 
        if (msaCheckWindowFocus())
        {
            /* Do not display the dialog if the module has screen is not 
               visible */
            return TRUE;
        }
        /* However, set the screen in focus if there are no windows 
           so that dialogs can be displayed even though no windows
           are present */
    }
    if (!addDialogItem(dialogType, strHandle, fsm, signal, 0))
    {
        return FALSE;
    }
    /* Check if a dialog is currently displayed */
    if (!IS_DIALOG_DISPLAYED)
    {
        /* Show next dialog in the queue */
        return showDialog();
    }
    return TRUE;
}

/*!
 * \brief Displays a dialog that requires End User actions to disappear.
 *
 * \param textStrId The text to be displayed or 0 if the str param is used 
 *                  instead.
 * \param str The text string to displayed if textStrId is 0
 * \param fsm The FSM to respond the result to.
 * \param signal The response signal.
 * \param wid A optional wid that is sent back in the response as u_param2.
 * \return  TRUE if successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaShowConfirmDialog(WE_UINT32 textStrId, const char *str, 
    MsaStateMachine fsm, int signal, unsigned int wid)
{
    WeStringHandle strHandle;
    char *tmpStr;

    if (0 == textStrId)
    {
        if (0 == (strHandle = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, str, 
           WeUtf8, (int)strlen(str) + 1, 0)))
        {
            return FALSE;
        }
    }
    else
    {
        strHandle = MSA_GET_STR_ID(textStrId);
        /* Create a new string handle just to avoid mixing static and dynamic 
           strings */
        if (NULL == (tmpStr = msaGetStringFromHandle(strHandle)))
        {
            return FALSE;
        }
        if (0 == (strHandle = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, tmpStr, WeUtf8, 
            (int)strlen(tmpStr) + 1, 0)))
        {
            /*lint -e{774} */
            MSA_FREE(tmpStr);
            return FALSE;
        }
        /*lint -e{774} */
        MSA_FREE(tmpStr);
    }

    if (!addDialogItem(MSA_DIALOG_CONFIRM, strHandle, fsm, signal, wid))
    {
        return FALSE;
    }
    /* Check if a dialog is currently displayed */
    if (!IS_DIALOG_DISPLAYED)
    {
        /* Show next dialog in the queue */
        return showDialog();
    }
    return TRUE;
}

/*! \brief Display the panic dialog.
 *
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaShowPanicDialog(void)
{
    we_dlg_message_t msg;

    /* Setup dlg package data */
    msg.dialog_type     = we_dlg_error;
    msg.dlg_data.handle = dlgHandle;
    msg.dlg_data.screen = msaGetScreenHandle();
    msg.dlg_data.title  = 0;
    msg.nbr_of_str_ids  = 1;
    msg.ok_string       = 0;
    msg.str_ids         = &(msaPanicDialog->stringHandle);
    /* Display the dialog */
    if ((msaPanicDialog->dlgOperId = we_dlg_create_message(&msg)) > 0)
    {
        return FALSE;
    }
    return TRUE;
}

/*!
 * \brief De-allocates resources allocated for the dialog handling.
 *****************************************************************************/
void msaTerminateDia(void)
{
    unsigned int i;
    /* Delete panic dialog */
    (void)msaDeleteDialog(&msaPanicDialog);
    (void)msaDeleteDialog(&msaDialog);
    (void)msa_pckg_unreg_pckg(pHandle, dlgPckId);
    msa_pckg_destroy(pHandle);
    pHandle = 0;
    
    dlgPckId  = 0;
    terminating = FALSE;
    for(i = 0; i < MSA_MAX_DIALOGS; i++)
    {
        if ((msaDialogQueue[i].dialogType != 0) && (msaDialogQueue[i].strHandle))
        {
            (void)WE_WIDGET_RELEASE(msaDialogQueue[i].strHandle);
        }
    }
    memset(msaDialogQueue, 0, sizeof(msaDialogQueue));
}

/*! \brief Creats a confirmation dialog.
 *
 * \type The dialog type, see #we_dialog_type_t.
 * \param dlg  The new dilog instance.
 * \param item The current queued item.
 *****************************************************************************/
static WE_BOOL createMsgDialog(we_dialog_type_t type, MsaDialog *dlg,
    const MsaDialogItem *item)
{
    we_dlg_message_t msgData;
    WeStringHandle strHandle;

    /* Initial handles needed */
    if (NULL == (dlgHandle = 
        msa_pckg_create_inst(pHandle, dlgPckId)))
    {
        return FALSE;
    }
    msaDialog->wid                   = item->wid;
    msaDialog->dialogType           = item->dialogType;
    msaDialog->fsm                  = item->fsm;
    msaDialog->signal               = item->signal;
    msaDialog->stringHandle         = item->strHandle;

    /* Create dialog */
    msgData.dialog_type             = type;
    msgData.dlg_data.handle         = dlgHandle;
    msgData.dlg_data.screen         = msaGetScreenHandle();
    msgData.dlg_data.title          = 0;
    msgData.nbr_of_str_ids          = 1;
    msgData.ok_string               = 0;
    strHandle                       = item->strHandle;
    msgData.str_ids                 = &strHandle;
    if ((dlg->dlgOperId = we_dlg_create_message(&msgData)) > 0)
    {
        if (!msa_pckg_reg_operation(pHandle, dlgHandle, 
            (unsigned long)dlg->dlgOperId, 0, 0, NULL))
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
    return TRUE;
}

/*! \brief Creats a confirmation dialog.
 *
 * \param dlg  The new dilog instance.
 * \param item The current queued item.
 *****************************************************************************/
static WE_BOOL createConfirmDialog(MsaDialog *dlg, const MsaDialogItem *item)
{
    we_dlg_confirm_t conf;
    WeStringHandle strHandle;

    /* Instance data */
    msaDialog->fsm          = item->fsm;
    msaDialog->signal       = item->signal;
    msaDialog->wid           = item->wid;
    msaDialog->dialogType   = item->dialogType;
    msaDialog->stringHandle = item->strHandle;

    /* Package data */
    conf.cancel_string      = 0;
    conf.dlg_data.handle    = dlgHandle;
    conf.dlg_data.screen    = msaGetScreenHandle();
    conf.dlg_data.title     = 0;
    conf.nbr_of_str_ids     = 1;
    strHandle               = item->strHandle;
    conf.str_ids            = &strHandle;
    conf.ok_string          = 0;
    if ((dlg->dlgOperId = we_dlg_create_confirm(&conf)) > 0)
    {
        if (!msa_pckg_reg_operation(pHandle, dlgHandle, 
            (unsigned long)dlg->dlgOperId, 0, 0, NULL))
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
    return TRUE;
}

/*!
 * \brief Creates and displays a dialog.
 * 
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL showDialog(void)
{
    int i;
    WE_BOOL ret = FALSE;
    
    if (NULL != msaDialog)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) instance busy!\n", __FILE__, __LINE__));
        return FALSE;
    }
    if (0 == msaDialogQueue[0].dialogType)
    {
        if (terminating)
        {
            /* No more dialogs to display and the application is in termination
               state, try to */
            (void)MSA_SIGNAL_SENDTO(MSA_CORE_FSM, MSA_SIG_CORE_TERMINATE_APP);
        }
        return FALSE;
    }
    MSA_CALLOC(msaDialog, sizeof(MsaDialog));
    switch (msaDialogQueue[0].dialogType)
    {
    case MSA_DIALOG_ALERT:
        ret = createMsgDialog(we_dlg_alert, msaDialog, &msaDialogQueue[0]);
        break;
    case MSA_DIALOG_WARNING:
        ret = createMsgDialog(we_dlg_warning, msaDialog, &msaDialogQueue[0]);
        break;
    case MSA_DIALOG_INFO:
        ret = createMsgDialog(we_dlg_info, msaDialog, &msaDialogQueue[0]);
        break;
    case MSA_DIALOG_ERROR:
        ret = createMsgDialog(we_dlg_error, msaDialog, &msaDialogQueue[0]);
        break;
    case MSA_DIALOG_CONFIRM:
        ret = createConfirmDialog(msaDialog, &msaDialogQueue[0]);
        break;
    }
    /* Remove the item from the queue */
    for(i = 0; i < MSA_MAX_DIALOGS - 1; i++)
    {
        memcpy(&msaDialogQueue[i], &msaDialogQueue[i + 1], 
            sizeof(MsaDialogItem));
    }

    /* Set last item to zero */
    msaDialogQueue[MSA_MAX_DIALOGS - 1].dialogType = (MsaDialogType)0;
    return ret;
}

/*! \brief Returns the number of dialogs queued, visible dialogs also counted
 *
 * \return The number of dialogs 
 *****************************************************************************/
unsigned int msaGetDialogCount(void)
{
    unsigned int count = 0;
    unsigned int i;
    for (i = 0; i < MSA_MAX_DIALOGS; i++)
    {
        if (0 != msaDialogQueue[i].dialogType)
        {
            ++count;
        }
    }
    if (NULL != msaDialog)
    {
        ++count;
    }
    return count;
}

/*! \brief Waits for the current dialog to close down, and then try to 
 *         terminate
 * 
 * \return TRUE if there are dialogs to display, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaCheckDialogs(void)
{
    terminating = TRUE;
    return (msaGetDialogCount() > 0);
}

/*! \brief Calls package encapsulation signal handler.
 *
 * \param signal   External signal to handle.
 * \param p        Signal data.
 *
 * \return Returns TRUE if signal is consumed.
 *****************************************************************************/
WE_BOOL msaDiaHandleSignal(WE_UINT16 signal, void *p)
{
    int res;

    res = msa_pckg_handle_signal(pHandle, signal, p);
    switch(res)
    {
    case WE_PACKAGE_OPERATION_COMPLETE:
    case WE_PACKAGE_SIGNAL_NOT_HANDLED:
        return FALSE;
    case WE_PACKAGE_SIGNAL_HANDLED:
        return TRUE;
    default:
        return FALSE;
    }
}
