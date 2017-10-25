/*
 * Copyright (C) Techfaith, 2002-2005.
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
#include "We_Mem.h"
#include "We_Dlg.h"

/* SIS */
#include "Sis_cfg.h"
#include "Sis_if.h"

/* SMA */
#include "Sia_rc.h"
#include "Saintsig.h"
#include "Sauisig.h"
#include "Sauidia.h"
#include "Sia_def.h"
#include "Satypes.h"
#include "Sauiform.h"
#include "Saui.h"
#include "Samem.h"
#include "Sapckg.h"
#include "Sacore.h"

/******************************************************************************
 * Defines
 *****************************************************************************/
#define SIA_MAX_DIALOGS     3 /* !< Maximum number of dialogs queued */
#define IS_DIALOG_DISPLAYED (smaDialog != NULL)

/******************************************************************************
 * Data-types
 *****************************************************************************/

/*! \struct SiaDialog
 *  Data-structures for global dialog data */
typedef struct
{
    SiaDialogType       dialogType;     /* !< The type of dialog that is displayed */

    WE_INT32           dlgOperId;      /* !< Current operation  */

    WeStringHandle     stringHandle;   /* !< */
    
    SiaStateMachine     fsm;            /* !< Callback FSM */
    int                 signal;         /* !< Callback signal */
    unsigned int        id;             /* !< ID */

}SiaDialog;

/*! \struct SiaDialogItem
 * Dialog structure, used for queuing dialog  */
typedef struct
{
    WeStringHandle strHandle;  /* !< The string to display */

    SiaDialogType   dialogType; /* !< The type of dialog that is displayed */
    SiaStateMachine fsm;        /* !< Callback FSM */
    int             signal;     /* !< Callback signal */
    unsigned int    id;         /* !< Callback data */
}SiaDialogItem;

/*
 * Dialog package standard functions to be registered in the
 * package encapsulation.
 */
static const Sia_pckg_fnc_list_t funcList = {
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

static SiaDialog            *smaDialog;     /*!< The current dialog  */
static SiaDialogItem        smaDialogQueue[SIA_MAX_DIALOGS];    /*!< The dialog queue  */
static SiaDialog            *smaPanicDialog;        /*!< The exception dialog */
static WE_BOOL             terminating;            /*!< Tells if the module is going down */

/* Package data */
static Sia_pckg_handle_t    *pHandle;       /* Package encapsulation handle */
static Sia_pkg_id_t         dlgPckId;       /* Dialog package ID */
static Sia_pkg_handle_t     *dlgHandle;     /* !< Dialog instance ID */

/******************************************************************************
 * Prototypes
 *****************************************************************************/

static WE_BOOL addDialogItem(SiaDialogType dialogType, 
    WeStringHandle strHandle, SiaStateMachine fsm, int signal, unsigned int id);
static WE_BOOL showDialog(void);
static WE_BOOL smaDeleteDialog(SiaDialog **dia);

/******************************************************************************
 * Function implementations
 *****************************************************************************/

/*! \brief SMA package operation completion entry point function.
 *
 * \param fsm      Fsm to forward signal to.
 * \param signal   Signal to send to fsm.
 * \param pkg_inst Package instance to completed operation.
 * \param id       Package operation ID.
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
static int smaDiaPackSignalHand(WE_INT32 fsm, WE_INT32 signal,
    Sia_pkg_id_t pckg_id, Sia_pkg_handle_t *pkg_inst, WE_INT32 id, 
    Sia_pckg_result_t *result, void *data)
{
    we_dlg_result_message_t *res;
    we_dlg_result_confirm_t *confRes;

    (void)data;
    (void)pckg_id;
    (void)signal;
    (void)fsm;

    if ((NULL != smaDialog) && (dlgHandle == pkg_inst) && 
        (smaDialog->dlgOperId == id))
    {
        if (SIA_DIALOG_CONFIRM == smaDialog->dialogType)
        {
            confRes = Sia_pckg_get_result_data(result);
            if (WE_DLG_RETURN_NOT_AVAILABLE == confRes->result)
            {
                return FALSE;
            }
            else
            {
                /* Operation finnished, report to caller */
                (void)SIA_SIGNAL_SENDTO_UU(smaDialog->fsm, smaDialog->signal, 
                    (WE_DLG_RETURN_OK == confRes->result) ? SIA_CONFIRM_DIALOG_OK: 
                    SIA_CONFIRM_DIALOG_CANCEL, smaDialog->id);
            }
        }
        else
        {
            /* Message dialog operation */
            res = Sia_pckg_get_result_data(result);
            if (res->result != WE_DLG_RETURN_NOT_AVAILABLE)
            {
                /* Check for call-back */
                if (SIA_NOTUSED_FSM != smaDialog->fsm)
                {
                    (void)SIA_SIGNAL_SENDTO(smaDialog->fsm, smaDialog->signal);
                }
            }
        }
        /* Clean-up the instance */
        (void)smaDeleteDialog(&smaDialog);
        /* Next dialog */
        (void)showDialog();
        return TRUE;
    }else if ((dlgHandle == pkg_inst) && 
              (smaPanicDialog->dlgOperId == id))
    {
        /* The exception dialog has been closed, terminate the application */
        (void)SIA_SIGNAL_SENDTO(SIA_FSM_CORE, SIA_SIG_CORE_TERMINATE_APP);
        return TRUE;
    }
    return FALSE;
}

/*!
 * \brief Add a dialog to the dialog queue.
 *
 * \param dialogType The type of dialog to display.
 * \param textStrId The string ID of the string to display in the dialog.
 * \param fsm       The callback FSM
 * \param signal    The callback signal
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL addDialogItem(SiaDialogType dialogType, 
    WeStringHandle strHandle, SiaStateMachine fsm, int signal, unsigned int id)
{
    int i;
    for(i = 0; i < SIA_MAX_DIALOGS; i++)
    {
        /* Find an empty slot */
        if (0 == smaDialogQueue[i].strHandle)
        {
            smaDialogQueue[i].strHandle  = strHandle;
            smaDialogQueue[i].dialogType = dialogType;
            smaDialogQueue[i].fsm        = fsm;
            smaDialogQueue[i].signal     = signal;
            smaDialogQueue[i].id         = id;
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
static WE_BOOL smaDeleteDialog(SiaDialog **dia)
{
    /* Message dialogs */
    if ((NULL != dia) && (*dia != NULL))
    {
        /* Check if the string-handle needs to be deleted */
        if (SIA_DIALOG_CONFIRM == (*dia)->dialogType)
        {
            /* Only confirm dialog has dynamic string resources */
            (void)WE_WIDGET_RELEASE((*dia)->stringHandle);
        }
        SIA_FREE(*dia);
        *dia = NULL;
        return TRUE;
    }
    return FALSE;
}

/*!
 * \brief Initiates the data-structures for the dialog handling.
 *****************************************************************************/
WE_BOOL smaInitDia(void)
{
    /* Initiate encapsulation */
    if (NULL == (pHandle = Sia_pckg_create(WE_MODID_SIA)))
    {
        return FALSE;
    }
    /* Register packages */
    if (0 == (dlgPckId = Sia_pckg_reg_pckg(pHandle, &funcList)))
    {
        return FALSE;
    }
    /* Sets callback function */
    if (!Sia_pckg_set_response_handler(pHandle, smaDiaPackSignalHand))
    {
        return FALSE;
    }
    if (NULL == (dlgHandle = Sia_pckg_create_inst(pHandle, dlgPckId)))
    {
        return FALSE;
    }

	/*panic dialog */
    /* Allocate without exceptionhandler during init */
    if (NULL == (smaPanicDialog = SIA_ALLOC(sizeof(SiaDialog))))
    {
        return FALSE;
    }
    memset(smaPanicDialog, 0, sizeof(SiaDialog));
    /* Set-up the "panic" dialog...it must be allocated directly so that it can
       be displayed whithout allocation to much memory */
    smaPanicDialog->stringHandle = SIA_GET_STR_ID(SIA_STR_ID_PANIC);

    /* Set global variables to usable values */
    terminating = FALSE;
    memset(smaDialogQueue, 0, sizeof(smaDialogQueue));
    smaDialog = NULL;

    return TRUE;
}

/*!
 * \brief Creates and displays a global dialog.
 *
 * \param strHandle The text that is presented.
 * \param dialogType The type of dialog that is presented, see #SiaDialogType.
 * \return TRUE if the dialog was created, otherwise FALSE.
 *****************************************************************************/
WE_BOOL smaShowDialog(WeStringHandle strHandle, SiaDialogType dialogType)
{
    return smaShowDialogWithCallback(strHandle, dialogType, SIA_NOTUSED_FSM,
        0);
}

/*!
 * \brief Creates and displays a global dialog. 
 *
 * \param strHandle The text that is presented
 * \param dialogType The type of dialog that is presented, see #SiaDialogType.
 * \param fsm The fsm to call when dialog is closed.
 * \param signal The signal to send to the fsm when the dialog is closed.
 * \return TRUE if the dialog was created, otherwise FALSE.
 *****************************************************************************/
WE_BOOL smaShowDialogWithCallback(WeStringHandle strHandle, 
    SiaDialogType dialogType, SiaStateMachine fsm, int signal)
{    
    if (!WE_WIDGET_HAS_FOCUS(smaGetScreenHandle()))
    {
        /* Do not display the dialog if the screen is not visible */ 
        if (smaCheckWindowFocus())
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
 * \param id A optional id that is sent back in the response as u_param2.
 * \return  TRUE if successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL smaShowConfirmDialog(WE_UINT32 textStrId, const char *str, 
    SiaStateMachine fsm, int signal, unsigned int id)
{
    WeStringHandle strHandle;
    char *tmpStr;

    if (0 == textStrId)
    {
        if (0 == (strHandle = WE_WIDGET_STRING_CREATE(WE_MODID_SIA, str, 
           WeUtf8, (int)strlen(str) + 1, 0)))
        {
            return FALSE;
        }
    }
    else
    {
        strHandle = SIA_GET_STR_ID(textStrId);
        /* Create a new string handle just to avoid mixing static and dynamic 
           strings */
        if (NULL == (tmpStr = smaGetStringFromHandle(strHandle)))
        {
            return FALSE;
        }
        if (0 == (strHandle = WE_WIDGET_STRING_CREATE(WE_MODID_SIA, tmpStr, WeUtf8, 
            (int)strlen(tmpStr) + 1, 0)))
        {
            /*lint -e{774} */
            SIA_FREE(tmpStr);
            return FALSE;
        }
        /*lint -e{774} */
        SIA_FREE(tmpStr);
    }

    if (!addDialogItem(SIA_DIALOG_CONFIRM, strHandle, fsm, signal, id))
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
WE_BOOL smaShowPanicDialog(void)
{
    we_dlg_message_t msg;

    /* Setup dlg package data */
    msg.dialog_type     = we_dlg_error;
    msg.dlg_data.handle = dlgHandle;
    msg.dlg_data.screen = smaGetScreenHandle();
    msg.dlg_data.title  = 0;
    msg.nbr_of_str_ids  = 1;
    msg.ok_string       = 0;
    msg.str_ids         = &(smaPanicDialog->stringHandle);
    /* Display the dialog */
    if ((smaPanicDialog->dlgOperId = we_dlg_create_message(&msg)) > 0)
    {
        return FALSE;
    }
    return TRUE;
}

/*!
 * \brief De-allocates resources allocated for the dialog handling.
 *****************************************************************************/
void smaTerminateDia(void)
{
    unsigned int i;
    /* Delete panic dialog */
    (void)smaDeleteDialog(&smaPanicDialog);
    (void)smaDeleteDialog(&smaDialog);
    (void)Sia_pckg_unreg_pckg(pHandle, dlgPckId);
    Sia_pckg_destroy(pHandle);
    pHandle = 0;
    
    dlgPckId  = 0;
    terminating = FALSE;
    for(i = 0; i < SIA_MAX_DIALOGS; i++)
    {
        if ((smaDialogQueue[i].dialogType != 0) && (smaDialogQueue[i].strHandle))
        {
            (void)WE_WIDGET_RELEASE(smaDialogQueue[i].strHandle);
        }
    }
    memset(smaDialogQueue, 0, sizeof(smaDialogQueue));
}

/*! \brief Creats a confirmation dialog.
 *
 * \type The dialog type, see #we_dialog_type_t.
 * \param dlg  The new dilog instance.
 * \param item The current queued item.
 *****************************************************************************/
static WE_BOOL createMsgDialog(we_dialog_type_t type, SiaDialog *dlg,
    const SiaDialogItem *item)
{
    we_dlg_message_t msgData;
    WeStringHandle strHandle;

    /* Initial handles needed */
    if (NULL == (dlgHandle = 
        Sia_pckg_create_inst(pHandle, dlgPckId)))
    {
        return FALSE;
    }
    smaDialog->id                   = item->id;
    smaDialog->dialogType           = item->dialogType;
    smaDialog->fsm                  = item->fsm;
    smaDialog->signal               = item->signal;
    smaDialog->stringHandle         = item->strHandle;

    /* Create dialog */
    msgData.dialog_type             = type;
    msgData.dlg_data.handle         = dlgHandle;
    msgData.dlg_data.screen         = smaGetScreenHandle();
    msgData.dlg_data.title          = 0;
    msgData.nbr_of_str_ids          = 1;
    msgData.ok_string               = 0;
    strHandle                       = item->strHandle;
    msgData.str_ids                 = &strHandle;
    if ((dlg->dlgOperId = we_dlg_create_message(&msgData)) > 0)
    {
        if (!Sia_pckg_reg_operation(pHandle, dlgHandle, 
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
static WE_BOOL createConfirmDialog(SiaDialog *dlg, const SiaDialogItem *item)
{
    we_dlg_confirm_t conf;

    /* Instance data */
    smaDialog->fsm          = item->fsm;
    smaDialog->signal       = item->signal;
    smaDialog->id           = item->id;
    smaDialog->dialogType   = item->dialogType;

    /* Package data */
    conf.cancel_string      = 0;
    conf.dlg_data.handle    = dlgHandle;
    conf.dlg_data.screen    = smaGetScreenHandle();
    conf.dlg_data.title     = 0;
    conf.nbr_of_str_ids     = 1;
    conf.str_ids            = &(dlg->stringHandle);
    conf.ok_string          = 0;
    if ((dlg->dlgOperId = we_dlg_create_confirm(&conf)) > 0)
    {
        if (!Sia_pckg_reg_operation(pHandle, dlgHandle, 
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
    
    if (NULL != smaDialog)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIA,
            "(%s) (%d) instance busy!\n", __FILE__, __LINE__));
        return FALSE;
    }
    if (0 == smaDialogQueue[0].dialogType)
    {
        if (terminating)
        {
            /* No more dialogs to display and the application is in termination
               state, try to */
             (void)SIA_SIGNAL_SENDTO(SIA_FSM_CORE, SIA_SIG_CORE_TERMINATE_APP);
        }
        return FALSE;
    }
    smaDialog = SIA_CALLOC(sizeof(SiaDialog));
    switch (smaDialogQueue[0].dialogType)
    {
    case SIA_DIALOG_ALERT:
        ret = createMsgDialog(we_dlg_alert, smaDialog, &smaDialogQueue[0]);
        break;
    case SIA_DIALOG_WARNING:
        ret = createMsgDialog(we_dlg_warning, smaDialog, &smaDialogQueue[0]);
        break;
    case SIA_DIALOG_INFO:
        ret = createMsgDialog(we_dlg_info, smaDialog, &smaDialogQueue[0]);
        break;
    case SIA_DIALOG_ERROR:
        ret = createMsgDialog(we_dlg_error, smaDialog, &smaDialogQueue[0]);
        break;
    case SIA_DIALOG_CONFIRM:
        ret = createConfirmDialog(smaDialog, &smaDialogQueue[0]);
        break;
    }
    /* Remove the item from the queue */
    for(i = 0; i < SIA_MAX_DIALOGS - 1; i++)
    {
        memcpy(&smaDialogQueue[i], &smaDialogQueue[i + 1], 
            sizeof(SiaDialogItem));
    }

    /* Set last item to zero */
    smaDialogQueue[SIA_MAX_DIALOGS - 1].dialogType = (SiaDialogType)0;
    return ret;
}

/*! \brief Returns the number of dialogs queued, visible dialogs also counted
 *
 * \return The number of dialogs 
 *****************************************************************************/
unsigned int smaGetDialogCount(void)
{
    unsigned int count = 0;
    unsigned int i;
    for (i = 0; i < SIA_MAX_DIALOGS; i++)
    {
        if (0 != smaDialogQueue[i].dialogType)
        {
            ++count;
        }
    }
    if (NULL != smaDialog)
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
WE_BOOL smaCheckDialogs(void)
{
    terminating = TRUE;
    return (smaGetDialogCount() > 0);
}

/*! \brief Calls package encapsulation signal handler.
 *
 * \param signal   External signal to handle.
 * \param p        Signal data.
 *
 * \return Returns TRUE if signal is consumed.
 *****************************************************************************/
WE_BOOL smaDiaHandleSignal(WE_UINT16 signal, void *p)
{
    int res;

    res = Sia_pckg_handle_signal(pHandle, signal, p);
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
