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

/*!\file macth.c
 *  Cross triggering handling.
 */

/* WE */
#include "We_Cfg.h"
#include "We_Mem.h"
#include "We_Def.h"
#include "We_Cmmn.h"
#include "We_Lib.h"
#include "We_Core.h"
#include "We_Wid.h"
#include "We_Log.h"
#include "We_File.h"

/* MMS */
#include "Mms_Def.h"

/* SIS */
#include "Sis_Cfg.h"
#include "Sis_If.h"

/* MSA */
#include "Msa_Rc.h"
#include "Msa_Cfg.h"
#include "Msa_Def.h"
#include "Msa_Types.h"
#include "Msa_Mem.h"
#include "Msa_Intsig.h"
#include "Msa_Core.h"
#include "Msa_Uicmn.h"
#include "Msa_Async.h"
#include "Msa_Uidia.h"
#include "Msa_Cth.h"
#include "Msa_Mr.h"
#include "Msa_Me.h"
#include "Msa_Utils.h"
#include "Msa_Sch.h"
#include "Msa_Ph.h"

/*! \file macth.c
 */

/******************************************************************************
 * Constants
 *****************************************************************************/
typedef enum
{
    MSA_CTH_RESULT_OK,
    MSA_CTH_RESULT_ERROR
}MsaCthResult;

/******************************************************************************
 * Data-structures
 *****************************************************************************/

/*! \struct MsaCthInsert
 * Data-structure for storing information about content routing (Image)
 */
typedef struct
{
    char                    *buffer;        /*!< The buffer storing the image 
                                                  data */
    WE_INT32               bufferSize;     /*!< The size of the buffer */
    char                    *url;           /*!< The name of the media object*/
    char                    *fileName;      /*!< The filename if the data is 
                                                 retrieved through a file */
    char                    *mimeType;      /*!< The content type of the data */
    MsaPropItem             *propItem;      /*!< Message properties */
}MsaCthInsert;

/******************************************************************************
 * Variables
 *****************************************************************************/

/*! Data-structure used when starting in content routing mode */
static MsaCthInsert *insertInst;

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static void cthCreateInstance(MsaCthInsert **instance);
static void cthDeleteInstance(MsaCthInsert **instance, MsaCthResult result);
static void cthSignalHandler(MsaSignal *sig);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief Initiates the CTH FSM.
 *
 *****************************************************************************/
void msaCthInit()
{
    insertInst = NULL;
    msaSignalRegisterDst(MSA_CTH_FSM, cthSignalHandler);
}

/*!
 * \brief Terminates the CTH FSM.
 *
 *****************************************************************************/
void msaCthTerminate()
{
    if (NULL != insertInst)
    {
        cthDeleteInstance(&insertInst, MSA_CTH_RESULT_OK);
    }
    msaSignalDeregister(MSA_CTH_FSM);
}

/*!
 * \brief Creates a new instance used for cross triggering (image)
 *
 * \param instance The current instance.
 *****************************************************************************/
static void cthCreateInstance(MsaCthInsert **instance)
{
    *instance = MSA_ALLOC(sizeof(MsaCthInsert));
    /* Clear the structure */
    memset(*instance, 0, sizeof(MsaCthInsert));
}

/*!
 * \brief Deletes cross triggering (image) a instance 
 *
 * \param instance The current instance.
 * \param result The result that caused the deletion.
 *****************************************************************************/
static void cthDeleteInstance(MsaCthInsert **instance, 
    MsaCthResult result)
{
    MsaCthInsert *inst;

    if (*instance != NULL)
    {
        inst = *instance;
        MSA_FREE(inst->buffer);
        inst->buffer = NULL;

        MSA_FREE(inst->fileName);
        MSA_FREE(inst->url);
        MSA_FREE(inst->mimeType);
        msaFreePropItem(inst->propItem, FALSE);
        MSA_FREE(inst);
        *instance = NULL;
    }
    if (result != MSA_CTH_RESULT_OK)
    {
        /* If the start-up fails then terminate */
        MSA_TERMINATE;
    }
}

/*!
 * \brief Read content from a file.
 *
 * \param instance Current instance.
 *****************************************************************************/
static WE_BOOL readContent(MsaCthInsert *instance)
{
    char *tmpFileName;
    tmpFileName = we_cmmn_strdup(WE_MODID_MSA, instance->fileName);
    if (!msaAfiLoadFromFile(MSA_CTH_FSM, MSA_SIG_CTH_FILE_NOTIFY, 
        tmpFileName, NULL, ~((WE_UINT32)0)))
    {
        return FALSE;
    }
    return TRUE;
}

/*!
 * \brief The signal handler for the CTH FSM.
 *
 * \param sig The current signal.
 *****************************************************************************/
static void cthSignalHandler(MsaSignal *sig)
{
    MsaPropItem *propItem;
    MsaCtContentInsert *ct;
    MsaStartupMode mode;
    MsaAfiReadResultData *afiData;

    /* Check which signal that was received */
    switch(sig->type)
    {
    case MSA_SIG_CTH_START:
        mode = (MsaStartupMode)sig->u_param1;
        /* Check if the application should be started with a default image */
        if (MSA_STARTUP_MODE_NEW == mode)
        {
            /* The instance should have been created already */
            if (NULL == insertInst)
            {
                /* No instance has been created continue with the start-up 
                   anyway*/
                (void)MSA_SIGNAL_SENDTO_U(MSA_ME_FSM, MSA_SIG_ME_ACTIVATE, 
                    MSA_ME_STARTUP_NEW_SMIL);
                break;

            }

            if (NULL == insertInst->fileName)
            {
                /* No content to insert. Start with the message properties */
                ct = MSA_ALLOC(sizeof(MsaCtContentInsert));
                memset(ct, 0, sizeof(MsaCtContentInsert));
                /* Send the message properties */
                ct->propItem = insertInst->propItem;
                insertInst->propItem = NULL;

                (void)MSA_SIGNAL_SENDTO_UP(MSA_ME_FSM, MSA_SIG_ME_ACTIVATE, 
                    MSA_ME_STARTUP_NEW_SMIL, ct);
                cthDeleteInstance(&insertInst, MSA_CTH_RESULT_OK);
            }
            /* Read content */
            else if (!readContent(insertInst))
            {
                /* Content exists. Load the content before starting */
                /* Delete the signal and terminate */
                msaSignalDelete(sig);
                /* Long jump */
                cthDeleteInstance(&insertInst, MSA_CTH_RESULT_ERROR);
            }
        }
        /* Check if the application should be started with to, cc and/or 
           subject filled in */
        else if (MSA_STARTUP_MODE_SCHEME == mode)
        {
            propItem = msaSchemeToPropItem(insertInst->url);
            ct = MSA_ALLOC(sizeof(MsaCtContentInsert));
            memset(ct, 0, sizeof(MsaCtContentInsert));
            ct->buffer       = NULL;
            ct->bufferSize   = 0;
            ct->mimeType     = NULL;
            ct->propItem     = propItem;
            /* Time to start edit the message */
            (void)MSA_SIGNAL_SENDTO_UP(MSA_ME_FSM, MSA_SIG_ME_ACTIVATE, 
                MSA_ME_STARTUP_NEW_SMIL, ct);
            cthDeleteInstance(&insertInst, MSA_CTH_RESULT_OK);
        }
        break;
    case MSA_SIG_CTH_FILE_NOTIFY:
        if (sig->u_param1 == MSA_AFI_READ_RESULT_OK)
        {
            afiData = sig->p_param;
            ct = MSA_ALLOC(sizeof(MsaCtContentInsert));
            memset(ct, 0, sizeof(MsaCtContentInsert));
            ct->name            = insertInst->url;
            insertInst->url     = NULL;
            ct->buffer          = afiData->data;
            afiData->data       = NULL;
            ct->bufferSize      = afiData->size;
            afiData->size       = 0;
            msaAfiLoadFreeData(afiData);
            ct->mimeType        = we_cmmn_strdup(WE_MODID_MSA, insertInst->mimeType);
            insertInst->buffer = NULL;
            insertInst->bufferSize = 0;
            ct->propItem     = insertInst->propItem;
            insertInst->propItem = NULL;
            /* Time to start edit the message */
            (void)MSA_SIGNAL_SENDTO_UP(MSA_ME_FSM, MSA_SIG_ME_ACTIVATE, 
                MSA_ME_STARTUP_NEW_SMIL, ct);
            /* Clean-up */
            cthDeleteInstance(&insertInst, MSA_CTH_RESULT_OK);
        }
        else
        {
            /* Error reading the content */
            (void)msaShowDialogWithCallback(MSA_GET_STR_ID(MSA_STR_ID_CRR_FAILED_TO_READ), 
                MSA_DIALOG_ERROR, MSA_CTH_FSM, MSA_SIG_CTH_DLG_RSP);
            /* The respone is handled in the next state */
        }
        break;

    case MSA_SIG_CTH_DLG_RSP:
            /* Edit the message anyway without the content */
            (void)MSA_SIGNAL_SENDTO_U(MSA_ME_FSM, MSA_SIG_ME_ACTIVATE, 
                MSA_ME_STARTUP_NEW_SMIL);
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, "(%s) (%d) Failed "
                "to load content to insert in the new SMIL message\n", __FILE__, 
                __LINE__));
            cthDeleteInstance(&insertInst, MSA_CTH_RESULT_OK);
        break;

    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) Unhandled signal in cthSignalHandler!\n", __FILE__, 
            __LINE__));
        break;
    }
    msaSignalDelete(sig);
}

/*! \brief Handles
 *
 * \param command The current start-up data.
 * \param startupMode The current start-up mode.
 * \return TRUE if the content was handled, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaCthHandleSchemeCt(const we_module_execute_cmd_t *command)
{
    /* Check if the content originates from a MMSTO: */
#ifdef MSA_CFG_CT_BROWSER_SCHEMES
#ifdef WE_MODID_WBS
    if ( (WE_CONTENT_DATA_NONE == command->contentData->contentDataType)
         && (NULL != command->contentData->contentUrl) &&
         (command->srcModId == WE_MODID_WBS))
    {
        cthCreateInstance(&insertInst);
        insertInst->url = MSA_ALLOC(
            strlen(command->contentData->contentUrl) + 1);
        we_cmmn_strcpy_lc(insertInst->url, 
            command->contentData->contentUrl);
        return TRUE;
    }
#else
#error The WBS module must be used in order to allow cross triggering!
    (void)command
#endif
#else
    (void)command;
#endif
    return FALSE;
}

/*! \brief De-allocates a #MsaCtContentInsert structure.
 *
 * \param ci The Structure to de-allocate.
 * \param command The current start-up data.
 * \param startupMode The current start-up mode.
 * \return TRUE if the content was handled, otherwise FALSE.
 *****************************************************************************/
void msaCtFreeContentInsert(MsaCtContentInsert *ci)
{
    if (NULL != ci)
    {
        MSA_FREE(ci->buffer);
        ci->buffer = NULL;
        ci->bufferSize = 0;
        MSA_FREE(ci->name);
        ci->name = NULL;
        MSA_FREE(ci->mimeType);
        ci->mimeType = NULL;
        msaFreePropItem(ci->propItem, FALSE);
        /*lint -e{774} */
        MSA_FREE(ci);
    }
}

/*! \brief Handles cross triggering "-mode new -i"
 *
 * \param contentData Information about the content to insert, or NULL if no
 *                    content needs to be inserted.
 * \param item        The recipient(s) and the subject of the new message.
 * \param command The current command.
 *****************************************************************************/
WE_BOOL msaCthHandleInsertContentCt(const we_content_data_t *contentData, 
    MsaPropItem *item)
{
    cthCreateInstance(&insertInst);
    /* Check if there are any content data to insert, i.e., pictures, sounds or
       text */
    if ((NULL != contentData) && (NULL != contentData->contentSource) && 
        (NULL != contentData->contentUrl) && 
        (NULL != contentData->routingIdentifier))
    {
        WE_LOG_MSG((WE_MODID_MSA, WE_LOG_DETAIL_HIGH, 
            "CTH: inserting content - source:%s name:%s mimeType:%s\n", 
            contentData->contentSource, contentData->contentUrl, 
            contentData->routingIdentifier));
        /* This is a valid request */
        if (MSA_MEDIA_GROUP_NONE == 
            msaMimeToObjGroup(contentData->routingIdentifier))
        {
            WE_LOG_MSG((WE_MODID_MSA, WE_LOG_DETAIL_HIGH, 
                "(%s) (%d) unsupported media types=%s!\n", __FILE__, __LINE__, 
                contentData->routingIdentifier));
            return FALSE;
        }
        insertInst->mimeType = we_cmmn_strdup(WE_MODID_MSA, 
            contentData->routingIdentifier);
        /* Copy the file path/name */
        insertInst->fileName = we_cmmn_strdup(WE_MODID_MSA, 
            contentData->contentSource);
        /* Copy the name of the media object */
        insertInst->url = we_cmmn_strdup(WE_MODID_MSA, 
            contentData->contentUrl);
        /* Make sure the path is not included in the name */
        msaRemPathFromFileName(insertInst->url);

        return TRUE;
    }
    /* Handle the to field */

    insertInst->propItem = item;
    return TRUE;
}
