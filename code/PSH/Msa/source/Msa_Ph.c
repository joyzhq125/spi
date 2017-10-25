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

/*!\file maph.c
 * \brief Packet handler for the MSA.
 */

/* WE */
#include "We_Cfg.h"
#include "We_Mem.h"
#include "We_Def.h"
#include "We_Cmmn.h"
#include "We_Lib.h"
#include "We_Core.h"
#include "We_Log.h"
#include "We_Dlg.h"
#include "We_Wid.h"
#include "We_Afi.h"

/* MMS */
#include "Mms_Def.h"

/* MSA */
#include "Msa_Mem.h"
#include "Msa_Def.h"
#include "Msa_Types.h"
#include "Msa_Pckg.h"
#include "Msa_Intsig.h"
#include "Msa_Uicmn.h"
#include "Msa_Ph.h"

/******************************************************************************
 * Constants
 *****************************************************************************/
#define MSA_AFI_READ_CHUNK_SIZE         1024

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
        we_dlg_get_result,
        we_dlg_result_free
};

static const msa_pckg_fnc_list_t afiFuncList = {
        we_afi_init, 
        we_afi_handle_signal,
        we_afi_get_private_data, 
        we_afi_set_private_data,
        we_afi_terminate, 
        NULL,
        we_afi_get_result,
        we_afi_result_free
};

/******************************************************************************
 * Data-types
 *****************************************************************************/

/*! \enum MsaAfiReadState 
 * States used when reading a file from persistent storage
 */
typedef enum
{
    MSA_AFI_READ_INIT,          /*!< */
    MSA_AFI_READ_OPEN,          /*!< */
    MSA_AFI_READ_SEEK,          /*!< */
    MSA_AFI_READ_GET_SIZE,      /*!< */
    MSA_AFI_READ_GET_MIME_TYPE, /*!< */
    MSA_AFI_READ_READ_CHUNK,    /*!< */
    MSA_AFI_READ_CLOSE          /*!< */
}MsaAfiReadState;

/*! \struct MsaAfiReadOp
 *	
 */
typedef struct
{
    MsaAfiReadState     state;
    WE_UINT32          afiOpId;

    WE_INT32           fileHandle;

    msa_pkg_handle_t    *afiHandle;

    WE_UINT32          pos;

    MsaAfiReadResultData *res;

    MsaStateMachine     fsm;
    WE_UINT32          maxSize;
    int                 signal;
}MsaAfiReadOp;

/******************************************************************************
 * Static data
 *****************************************************************************/
/* Package */
static msa_pckg_handle_t *pHandle;      /* Package encapsulation handle */
static msa_pkg_id_t       dlgPckId;     /* Dialog package WID */
static msa_pkg_handle_t  *dlgHandle;    /* Dialog instance WID */
static WE_UINT32         dlgOperId;    /* */
/* AFI */
static MsaAfiReadOp     *afiReadOp;
static msa_pkg_id_t     afiPckId;


/******************************************************************************
 * Prototypes
 *****************************************************************************/
static WE_BOOL createReadInst(MsaAfiReadOp **rOp, char *fileName, 
    MsaStateMachine fsm, int signal, void *callbackData, WE_UINT32 maxSize);
static WE_BOOL handleReadOp(msa_pkg_handle_t *rHandle, MsaAfiReadOp **rOp, 
    msa_pckg_result_t *result);
static void deleteReadInst(msa_pckg_handle_t *rHandle, MsaAfiReadOp **rOp, 
    MsaAfiReadResult result);
static void phSignalHandler(MsaSignal *sig);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*! \brief Creates an instance for loading content from persistent storage
 *         through the use of AFI.
 *
 * \param rOp The new instance is written to this pointer.
 * \param fileName The file to open and read from.
 * \param fsm The callback FSM.
 * \param signal The callback signal.
 * \param callbackData The data that should be provided in the callback signal.
 * \maxSize The maximum size of the file to read.
 * \return 
 *****************************************************************************/
static WE_BOOL createReadInst(MsaAfiReadOp **rOp, 
    char *fileName, MsaStateMachine fsm, int signal, void *callbackData, 
    WE_UINT32 maxSize)
{
    MsaAfiReadOp *curr;
    MSA_CALLOC(curr, sizeof(MsaAfiReadOp));
    MSA_CALLOC(curr->res, sizeof(MsaAfiReadResultData));
    if (NULL == (curr->afiHandle = msa_pckg_create_inst(pHandle, 
        afiPckId)))
    {
        return FALSE;
    }

    curr->state         = MSA_AFI_READ_INIT;
    curr->fsm           = fsm;
    curr->signal        = signal;
    curr->pos           = 0;
    curr->maxSize       = maxSize;

    curr->res->fileName     = fileName;
    curr->res->callbackData = callbackData;
    curr->res->size         = 0;
    *rOp = curr;
    return TRUE;
}

/*! \brief This is the state-machine for the read operation.
 *
 * \param rHandle The handle of the package.
 * \param rOp The current instance.
 * \param result The result of the previous operation.
 *****************************************************************************/
static WE_BOOL handleReadOp(msa_pkg_handle_t *rHandle, MsaAfiReadOp **rOp, 
    msa_pckg_result_t *result)
{
    we_afi_result_read_t           *readRes;
    we_afi_result_open_t           *openRes;
    we_afi_result_get_attributes_t *getRes;
    WE_UINT32                      bytesToRead;
    WE_INT32                       iRes;
    we_pck_attr_list_t             *attrList;
    MsaAfiReadOp                    *curr;

    if (NULL == (curr = *rOp))
    {
        return FALSE;
    }
    switch(curr->state)
    {
    case MSA_AFI_READ_INIT:
    case MSA_AFI_READ_OPEN:
        if (NULL == result)
        {
            if (WE_PACKAGE_ERROR == (curr->afiOpId = we_afi_open(
                curr->afiHandle, curr->res->fileName, WE_AFI_RDONLY))) /* Q04A */
            {
                deleteReadInst(rHandle, rOp, MSA_AFI_READ_RESULT_FAILED);
                return FALSE;
            }
            if (!msa_pckg_reg_operation(pHandle, curr->afiHandle, 
                curr->afiOpId, 0, 0, NULL))
            {
                deleteReadInst(rHandle, rOp, MSA_AFI_READ_RESULT_FAILED);
                return FALSE;
            }
            break;
        }
        else
        {
            openRes = msa_pckg_get_result_data(result);
            if (WE_AFI_OK == openRes->result)
            {
                curr->state = MSA_AFI_READ_GET_MIME_TYPE;
                curr->fileHandle = openRes->file_handle;
            }
            else
            {
                deleteReadInst(rHandle, rOp, MSA_AFI_READ_RESULT_FAILED);
                return FALSE;
            }
        }
        (void)MSA_SIGNAL_SENDTO_P(MSA_PH_FSM, MSA_SIG_PH_READ_RESULT, NULL);
        break;

    case MSA_AFI_READ_GET_MIME_TYPE:
        /* Get the mime-type in its own state since it may be missing */
        if (NULL == result)
        {
            if (WE_PACKAGE_ERROR == (curr->afiOpId = 
                we_afi_get_attributes(curr->afiHandle, curr->res->fileName, 
                WE_PCK_ATTRIBUTE_MIME)))
            {
                deleteReadInst(rHandle, rOp, MSA_AFI_READ_RESULT_FAILED);
                return FALSE;
            }
            if (!msa_pckg_reg_operation(pHandle, curr->afiHandle, 
                curr->afiOpId, 0, 0, NULL))
            {
                deleteReadInst(rHandle, rOp, MSA_AFI_READ_RESULT_FAILED);
                return FALSE;
            }
            break;
        }
        else
        {
            getRes = msa_pckg_get_result_data(result);
            if (WE_AFI_OK == getRes->result)
            {
                attrList = getRes->attr_list;
                while (NULL != attrList)
                {
                    if (WE_PCK_ATTRIBUTE_MIME == attrList->attribute)
                    {
                        /* Copy the content type */
                        curr->res->mimeType = 
                            we_cmmn_strdup(WE_MODID_MSA, attrList->_u.s);
                    }
                    attrList = attrList->next;
                }
            }
            /* Perform the next state even though the mime-type is missing */
            result = NULL;
            curr->state = MSA_AFI_READ_GET_SIZE;
        }
        /* Next state */
        (void)MSA_SIGNAL_SENDTO_P(MSA_PH_FSM, MSA_SIG_PH_READ_RESULT, NULL);
        break;

    case MSA_AFI_READ_GET_SIZE:
        if (NULL == result)
        {
            if (WE_PACKAGE_ERROR == (curr->afiOpId = 
                we_afi_get_attributes(curr->afiHandle, curr->res->fileName, 
                WE_PCK_ATTRIBUTE_SIZE)))
            {
                deleteReadInst(rHandle, rOp, MSA_AFI_READ_RESULT_FAILED);
                return FALSE;
            }
            if (!msa_pckg_reg_operation(pHandle, curr->afiHandle, 
                curr->afiOpId, 0, 0, NULL))
            {
                deleteReadInst(rHandle, rOp, MSA_AFI_READ_RESULT_FAILED);
                return FALSE;
            }
            break;
        }
        else
        {
            getRes = msa_pckg_get_result_data(result);
            if (WE_AFI_OK == getRes->result)
            {
                attrList = getRes->attr_list;
                while (NULL != attrList)
                {
                    if (WE_PCK_ATTRIBUTE_SIZE == attrList->attribute)
                    {
                        curr->res->size = attrList->_u.i;
                    }
                    attrList = attrList->next;
                }
                if (curr->res->size > curr->maxSize)
                {
                    deleteReadInst(rHandle, rOp, MSA_AFI_READ_RESULT_TO_LARGE);
                    return FALSE;
                }
                /* Allocate the data buffer */
                if (NULL == (curr->res->data = MSA_ALLOC(curr->res->size)))
                {
                    deleteReadInst(rHandle, rOp, MSA_AFI_READ_RESULT_FAILED);
                    return FALSE;
                }
                result = NULL;
                curr->state = MSA_AFI_READ_SEEK;
            }
            else
            {
                return FALSE;
            }
        }
        (void)MSA_SIGNAL_SENDTO_P(MSA_PH_FSM, MSA_SIG_PH_READ_RESULT, NULL);
        break;

    case MSA_AFI_READ_SEEK:
        if (we_afi_seek(curr->afiHandle, curr->fileHandle, curr->pos, 
            TPI_FILE_SEEK_SET) < 0)
        {
            deleteReadInst(rHandle, rOp, MSA_AFI_READ_RESULT_FAILED);
            return FALSE;
        }
        result = NULL;
        curr->state = MSA_AFI_READ_READ_CHUNK;
        (void)MSA_SIGNAL_SENDTO_P(MSA_PH_FSM, MSA_SIG_PH_READ_RESULT, NULL);
        break;

    case MSA_AFI_READ_READ_CHUNK:
        readRes = NULL;
        if (NULL != result)
        {
            readRes = msa_pckg_get_result_data(result);
            if (WE_AFI_OK != readRes->result)
            {
                deleteReadInst(rHandle, rOp, MSA_AFI_READ_RESULT_FAILED);
                return FALSE;
            }
            curr->pos +=readRes->data_len;
            (void)MSA_SIGNAL_SENDTO_P(MSA_PH_FSM, MSA_SIG_PH_READ_RESULT,
                NULL);
            break;
        }
        /* Calculate the size to read */
        if (curr->pos < curr->res->size)
        {
            bytesToRead = (curr->res->size - curr->pos > MSA_AFI_READ_CHUNK_SIZE) ? 
                    MSA_AFI_READ_CHUNK_SIZE: curr->res->size - curr->pos;
            /* Read the data */
            if (bytesToRead > 0)
            {
                if (WE_PACKAGE_ERROR == (curr->afiOpId = we_afi_read(
                    curr->afiHandle, curr->fileHandle, 
                    &((unsigned char *)curr->res->data)[curr->pos], 
                    bytesToRead)))
                {
                    deleteReadInst(rHandle, rOp, MSA_AFI_READ_RESULT_FAILED);
                    return FALSE;
                }
                if (!msa_pckg_reg_operation(pHandle, curr->afiHandle, 
                    curr->afiOpId, 0, 0, NULL))
                {
                    deleteReadInst(rHandle, rOp, MSA_AFI_READ_RESULT_FAILED);
                    return FALSE;
                }
            }
            break;
        }
        else
        {
            result = NULL;
            curr->state = MSA_AFI_READ_CLOSE;
        }
        (void)MSA_SIGNAL_SENDTO_P(MSA_PH_FSM, MSA_SIG_PH_READ_RESULT, NULL);
        break;

    case MSA_AFI_READ_CLOSE:
        if (NULL != result)
        {
            iRes = msa_pckg_get_result_int(result);
            /* Result struct is missing in AFI */
            if (WE_AFI_OK == iRes)
            {
                deleteReadInst(rHandle, rOp, MSA_AFI_READ_RESULT_OK);
                return TRUE;
            }
            
        }
        if (WE_PACKAGE_ERROR == (curr->afiOpId = 
            we_afi_close(curr->afiHandle, curr->fileHandle)))
        {
            deleteReadInst(rHandle, rOp, MSA_AFI_READ_RESULT_FAILED);
            return FALSE;
        }
        if (!msa_pckg_reg_operation(pHandle, curr->afiHandle, 
            curr->afiOpId, MSA_PH_FSM, MSA_SIG_PH_READ_RESULT, NULL))
        {
            deleteReadInst(rHandle, rOp, MSA_AFI_READ_RESULT_FAILED);
            return FALSE;
        }        
        break;
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
            "(%s) (%d) unhandled signal to the AFI load file FSM\n", __FILE__, 
            __LINE__));
        break;
    }
    return TRUE;
}

/*! \brief De-allocates the read instance.
 *
 * \param rHandle The packet handle.
 * \param rOp The read operation instance.
 * \param result The result of  the operation.
 *****************************************************************************/
static void deleteReadInst(msa_pckg_handle_t *rHandle, MsaAfiReadOp **rOp, 
    MsaAfiReadResult result)
{
    MsaAfiReadOp *curr;
    curr = *rOp;
    if (NULL == curr)
    {
        return;
    }
    /* Send The response */
    if (result != MSA_AFI_READ_RESULT_INT_TERM)
    {
        /* Send the response back */
        (void)MSA_SIGNAL_SENDTO_UP(curr->fsm, curr->signal, result, curr->res);
        curr->res = NULL;
    }
    /* Delete open AFI handles */
    msa_pckg_terminate(rHandle, curr->afiHandle, FALSE);
    /* De-allocate memory */
    /* File buffer */
    if ((NULL != curr->res) && (NULL != curr->res->data))
    {
        MSA_FREE(curr->res->data);
        curr->res->data = NULL;
    }
    /* File-name*/
    if ((NULL != curr->res) && (NULL != curr->res->fileName))
    {
        MSA_FREE(curr->res->fileName);
        curr->res->fileName = NULL;
    }
    /* Mime type */
    if ((NULL != curr->res) && (NULL != curr->res->mimeType))
    {
        MSA_FREE(curr->res->mimeType);
        curr->res->mimeType = NULL;
    }
    /* The result data  */
    if (NULL != curr->res)
    {
        MSA_FREE(curr->res);
        curr->res = NULL;
    }
    MSA_FREE(curr);
    *rOp = NULL;
}


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
static int msaPacketSignalHand(WE_INT32 fsm, WE_INT32 signal,
    msa_pkg_id_t pckg_id, msa_pkg_handle_t *pkg_inst, WE_INT32 wid, 
    msa_pckg_result_t *result, void *data)
{
    pkg_inst = NULL;
    wid = 0;
    data = NULL;

    if (dlgPckId == pckg_id)
    {
        (void)MSA_SIGNAL_SENDTO_UP((MsaStateMachine)fsm, signal, 
            ((we_dlg_result_open_t *)result->oper_result._u.data)->result,
            ((we_dlg_result_open_t *)result->oper_result._u.data)->file_name);
        ((we_dlg_result_open_t *)result->oper_result._u.data)->file_name = NULL;
        MSA_FREE(result->oper_result._u.data);
        result->oper_result._u.data = NULL;
    }
    else if ((NULL != afiReadOp) && (afiPckId == pckg_id))
    {
        if (!handleReadOp(pHandle, &afiReadOp, result))
        {
            deleteReadInst(pHandle, &afiReadOp, MSA_AFI_READ_RESULT_FAILED);
        }
    }
    return 0;
}

/*! \brief Initiates the package handling for MSA.
 *
 * \return TRUE on successful initialization.
 *
 * Initializes the:
 * - Dialog package
 * - Afi package
 *
 *****************************************************************************/
WE_BOOL msaPhInit(void)
{
    /* Initiate encapsulation */
    if (NULL == (pHandle = msa_pckg_create(WE_MODID_MSA)))
    {
        return FALSE;
    }

     /* Dlg */
    if (0 == (dlgPckId = msa_pckg_reg_pckg(pHandle, &funcList)))
    {
        return FALSE;
    }
    if (NULL == (dlgHandle = msa_pckg_create_inst(pHandle, dlgPckId)))
    {
        return FALSE;
    }
    /* AFI */
    if (0 == (afiPckId = msa_pckg_reg_pckg(pHandle, &afiFuncList)))
    {
        return FALSE;
    }

    /* Sets callback function */
    if (!msa_pckg_set_response_handler(pHandle, msaPacketSignalHand))
    {
        return FALSE;
    }
    msaSignalRegisterDst(MSA_PH_FSM, phSignalHandler);
    return TRUE;
}

/*! \brief Calls package encapsulation signal handler.
 *
 * \param signal   External signal to handle.
 * \param p        Signal data.
 *
 * \return Returns TRUE if signal is consumed.
 *****************************************************************************/
WE_BOOL msaPhHandleSignal(WE_UINT16 signal, void *p)
{
    int res;

    res = msa_pckg_handle_signal(pHandle, signal, p);
    switch(res)
    {
    case WE_PACKAGE_OPERATION_COMPLETE:
    case WE_PACKAGE_SIGNAL_NOT_HANDLED:
        return FALSE;
        break;
    case WE_PACKAGE_SIGNAL_HANDLED:
        return TRUE;
    default:
        return FALSE;
    }
}

/*! \brief Terminates MSA package handling
 *
 * Terminates packages:
 * - Dialog package
 * - Afi package
 *
 *****************************************************************************/
void msaPhTerminate(void)
{
    (void)msa_pckg_unreg_pckg(pHandle, afiPckId);
    (void)msa_pckg_unreg_pckg(pHandle, dlgPckId);
    msa_pckg_destroy(pHandle);
    pHandle = 0;
    msaSignalDeregister(MSA_PH_FSM);
}

/*! \brief Creates a dialog for input 
 *
 * \param fsm       Fsm that receives internal signal at completion.
 * \param signal    Signal to send to fsm at completion of the operation.
 * \param mimeTypes Comma separated list of mime-types wildcard '*' are allowed.
 * \return A handle to the operation or 0 if the operation fails.
 *****************************************************************************/
WE_BOOL msaOpenDlgCreateMediaInput(MsaStateMachine fsm, int signal, 
    const char *mimeTypes)
{
    we_dlg_open_t openData;
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
        "(%s) (%d) msaOpenDlgCreateMediaInput fsm = %d, signal = %d, "
        "mimeTypes = %s\n", __FILE__, __LINE__, fsm, signal, mimeTypes));
    memset(&openData, 0, sizeof(we_dlg_open_t));
    openData.mime_types = mimeTypes;
    openData.permission = WE_DLG_PERMISSION_FORWARD;
    openData.dlg_data.screen = msaGetScreenHandle();
    openData.dlg_data.handle = dlgHandle;
    if ((dlgOperId = we_dlg_create_open(&openData)) > 0)
    {
        if (!msa_pckg_reg_operation(pHandle, dlgHandle, dlgOperId, fsm, 
            signal, NULL))
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
    return TRUE;
} /* msaCreateMediaInputDlg */

/*! \brief Aborts the open dialog.
 *
 * \param The handle of the operation to abort.
 *****************************************************************************/
void *msaOpenDlgMediaInputAbort(void)
{
    void *ptr = NULL;
    (void)msa_pckg_cancel(pHandle, dlgHandle, dlgPckId, &ptr);
    return ptr;
}

/*! \brief Load a file from persistent storage.
 *
 * \param fsm The callback FSM
 * \param signal The callback signal
 * \param fileName The AFI file path of the file to read.
 * \param callbackData The callback data.
 * \param maxSize The maximum file size of the file to open.
 * \return TRUE if the operation is successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaAfiLoadFromFile(MsaStateMachine fsm, int signal, 
    char *fileName, void *callbackData, WE_UINT32 maxSize)
{
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
        "(%s) (%d) msaAfiLoadFromFile fsm = %d, signal = %d, "
        "fileName = %s, callbackData = %x, maxSize\n", __FILE__, __LINE__, 
        fsm, signal, fileName, callbackData, maxSize));
    if (NULL == afiReadOp)
    {
        if (!createReadInst(&afiReadOp, fileName, fsm, signal, 
            callbackData, maxSize) || !handleReadOp(pHandle, &afiReadOp, NULL))
        {
            deleteReadInst(pHandle, &afiReadOp, MSA_AFI_READ_RESULT_FAILED);
            return FALSE;
        }
    }
    else
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
            "(%s) (%d) msaAfiLoadFromFile instance is busy\n", __FILE__, 
            __LINE__));
        return FALSE;
    }
    return TRUE;
}

/*! \brief Abort the current file operation.
 *
 * \return Returns the data provided in the call to #msaAfiLoadFromFile.
 *****************************************************************************/
void *msaAfiLoadFromFileAbort(void)
{
    void *ptr = NULL;
    if (NULL != afiReadOp)
    {
        (void)msa_pckg_cancel(pHandle, afiReadOp->afiHandle, afiReadOp->afiOpId, 
            &ptr);
        deleteReadInst(pHandle, &afiReadOp, MSA_AFI_READ_RESULT_INT_TERM);
    }
    return ptr;
}

/*! \brief Signal handling for the packet handler.
 *
 * \param sig The incoming signal
 *****************************************************************************/
static void phSignalHandler(MsaSignal *sig)
{
    switch(sig->type)
    {
    case MSA_SIG_PH_READ_RESULT:
        (void)handleReadOp(pHandle, &afiReadOp, sig->p_param);
        break;
    }
    msaSignalDelete(sig);
}

/*! brief De-allocates a #MsaAfiReadResultData data-structure.
 *
 * \param The data to de-allocate.
 *****************************************************************************/
void msaAfiLoadFreeData(MsaAfiReadResultData *data)
{
    if (NULL != data)
    {
        if (NULL != data->data)
        {
            MSA_FREE(data->data);
        }
        if (NULL != data->mimeType)
        {
            MSA_FREE(data->mimeType);
        }
        if (NULL != data->fileName)
        {
            MSA_FREE(data->fileName);
        }
        /*lint -e{774} */
        MSA_FREE(data);
    }
}
