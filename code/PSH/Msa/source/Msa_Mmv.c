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

/*! \file mammv.c
 *  \brief Viewer of Multipart Mixed message (implementation)
 */

/* WE */
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Wid.h"
#include "We_Mem.h"
#include "We_Core.h"
#include "We_Act.h"

/* MMS */
#include "Mms_Cfg.h"
#include "Mms_Def.h"
#include "Mms_If.h"

/* SIS */
#include "Sis_Cfg.h"
#include "Sis_If.h"

/* MSA */
#include "Msa_Rc.h"
#include "Msa_Cfg.h"
#include "Msa_Types.h"
#include "Msa_Intsig.h"
#include "Msa_Mr.h"
#include "Msa_Mem.h"
#include "Msa_Crh.h"
#include "Msa_Uidia.h"
#include "Msa_Utils.h"
#include "Msa_Uicmn.h"
#include "Msa_Ph.h"
#include "Msa_Mmv.h"
#include "Msa_Mob.h"
#include "Msa_Uimmv.h"
#include "Msa.h"
#include "Msa_Srh.h"
#include "Msa_Cmn.h"

/******************************************************************************
 * Data-structures
 *****************************************************************************/

/*! \struct MsaMmvInstance
 * Data-structure for storing multipart/mixed viewer instances
 */
typedef struct
{
    MsaMoDataItem           *currentMo;     /* !< current media object in list */
    MsaMoDataItem           *moList;        /* !< media object list */
    MmsMsgId                msgId;          /* !< message wid */
    MmsFileType             fileType;       /* !< file type */

    we_act_action_entry_t  *actions;       /* !< */
    WE_UINT32              actionCount;    /* !< */
    WE_UINT32              actionId;       /* !< */
}MsaMmvInstance;

/******************************************************************************
 * Variables
 *****************************************************************************/
static MsaMmvInstance *mmvInstance;       /* !< The mp/m viewer instance */

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static MsaMoDataItem *getMoAtIndex(const MsaMmvInstance *inst, int index);
static void viewUnknownMo(const MsaMmvInstance *inst);
static WE_BOOL viewAudioMo(const MsaMmvInstance *inst);
static void setCurrentMo(MsaMmvInstance *inst, int index);
static void startWba(char *url);
static WE_BOOL viewLink(void *linkIn, MsaStateMachine fsm, int sig);
static WE_BOOL viewMo(const MsaMmvInstance *inst);
static void handleObjAction(const MsaMmvInstance *instance, WE_UINT32 index);
static void msaMmvCreateInstance(MsaMmvInstance **instance,
                                 const MsaMessage *srcInstance);
static void msaMmvDeleteInstance(MsaMmvInstance **instance);
static void handleMobOp(MsaMmvInstance *inst, MsaMobOperation op, int i, 
    MtrLinkEntry *p);
static void msaMmvMain(MsaSignal *sig);


/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief Get the media object at a specified index
 * \param inst  The current instance
 * \param index The requested position in list (zero based)
 * \return The request media object or NULL if illegal bounds
 *****************************************************************************/
static MsaMoDataItem *getMoAtIndex(const MsaMmvInstance *inst, int index)
{
    MsaMoDataItem *mo = inst->moList;

    while (index && mo != NULL)
    {
        mo = mo->next;
        index--;
    }

    return mo;
}


/*!
 * \brief Send a media object to the content router
 * \param inst  The current instance
 * \return TRUE upon success
 *****************************************************************************/
static void viewUnknownMo(const MsaMmvInstance *inst)
{
    (void)inst;
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
        "(%s) (%d) The end user presses [ok] on unhandled content; ignoring "
        "action!\n", __FILE__, __LINE__));
}

/*!
 * \brief Preview an audio media object
 * \param inst  The current instance
 * \return TRUE upon success
 *****************************************************************************/
static WE_BOOL viewAudioMo(const MsaMmvInstance *inst)
{
    MsaMoDataItem *mo = inst->currentMo;

    if (mo == NULL)
    {
        return FALSE;
    }
    
    /* play audio */
    return msaUiMmvPlayAudio(mo);
}

/*!
 * \brief Set current media object by index
 * \param inst  The current instance
 * \param index Index 
 *****************************************************************************/
static void setCurrentMo(MsaMmvInstance *inst, int index)
{
    if (index >= 0)
    {
        inst->currentMo = getMoAtIndex(inst, index);    
    }
    else
    {
        inst->currentMo = NULL;
    }
}

/*!
 * \brief Starts the WBA with a specific URL.
 *
 * \param url The URL to open in the WBA.
 *****************************************************************************/
static void startWba(char *url)
{
#ifdef WE_MODID_WBA
    we_content_data_t  contentData;

    memset(&contentData, 0, sizeof(we_content_data_t));
    contentData.contentUrl = url;
    contentData.contentDataType = WE_CONTENT_DATA_NONE;
    WE_MODULE_START(WE_MODID_MSA, WE_MODID_WBA, NULL, &contentData, NULL);
#else
    (void)url;
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
        "(%s) (%d) Failed to start the WBA with the URL: %s, because the WBA"
        "is not defined!\n", __FILE__, __LINE__, url));
#endif
}

/*!
 * \brief Handles the viewing of a link detected inside a text object
 * 
 * \param linkIn  The particular link
 * \param fsm     The fsm to call when viewing is done
 * \param sig     The signal to send when viewing is done
 * \return TRUE upon success
 *****************************************************************************/
static WE_BOOL viewLink(void *linkIn, MsaStateMachine fsm, int sig)
{
    MtrLinkEntry *link = (MtrLinkEntry *)linkIn;

    if ((NULL == link) || (NULL == link->link))
    {
        return FALSE;
    } /* if */

    switch (link->scheme)
    {
    case MTR_SCHEME_HTTP:
    case MTR_SCHEME_HTTPS:
    case MTR_SCHEME_WWW:
    case MTR_SCHEME_WAP:
        startWba(link->link);
        return TRUE;

    case MTR_SCHEME_PHONE:
        msaSrhMakeCall(fsm, sig, link->link);
        return TRUE;

    case MTR_SCHEME_NONE:
    case MTR_SCHEME_MAIL:
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
            "(%s)(%d) Unhandled schemes!\n", __FILE__, __LINE__));
        return FALSE;
    } /* switch */

} /* viewLink */

/*!
 * \brief Preview a media object
 * \param inst  The current instance
 * \return TRUE upon success
 *****************************************************************************/
static WE_BOOL viewMo(const MsaMmvInstance *inst)
{
    MsaMoDataItem *mo = inst->currentMo;

    if (mo == NULL)
    {
        return FALSE;
    }

    /* Dispatch media object according to its media type */
    switch (msaMimeToObjGroup((char *)mo->type->strValue))
    {
    case MSA_MEDIA_GROUP_AUDIO:
        /* play audio */
        return viewAudioMo(inst);
    case MSA_MEDIA_GROUP_IMAGE:
        /* insert code here to view images */
        return TRUE;
    case MSA_MEDIA_GROUP_TEXT:
        /* insert code here to view text */
        return TRUE;
    case MSA_MEDIA_GROUP_NONE:
    default:
        /* let the content router handle the media object */
        viewUnknownMo(inst);
        return TRUE;
    }
}

/*!
 * \brief Handle object actions
 *
 * \param instance The current instance.
 * \param index The index in the object action list.
 *****************************************************************************/
static void handleObjAction(const MsaMmvInstance *inst, WE_UINT32 index)
{
    we_pck_attr_list_t *fileAttributes;

    if ((inst == NULL) || (inst->currentMo == NULL))
    {
        return;
    }
    /* Set file attributes */
    we_pck_attr_init(&fileAttributes);
    we_pck_add_attr_string_value (WE_MODID_MSA, &fileAttributes, 
		WE_PCK_ATTRIBUTE_MIME, (char *)inst->currentMo->type->strValue);
	we_pck_add_attr_int_value (WE_MODID_MSA, &fileAttributes,
		WE_PCK_ATTRIBUTE_SIZE, inst->currentMo->size);

    /* Send the content to the specified module */
    msaCrhSendContent(fileAttributes, inst->currentMo->name, 
        MSA_MMV_FSM, MSA_SIG_MMV_CRH_DONE, &(inst->actions[index]), 
        inst->msgId, (WE_UINT16)inst->currentMo->bpIndex);
    we_pck_attr_free(WE_MODID_MSA, fileAttributes);
}

/*!
 * \brief Creates a new instance used for viewing multipart/mixed msgs
 *
 * \param instance The current instance.
 * \param srcInstance The old getMsg instance.
 *****************************************************************************/
static void msaMmvCreateInstance(MsaMmvInstance **instance,
    const MsaMessage *srcInstance)
{   
    *instance = MSA_ALLOC(sizeof(MsaMmvInstance));
    /* Clear the structure */
    memset(*instance, 0, sizeof(MsaMmvInstance));

    (*instance)->moList = srcInstance->smilInfo->mediaObjects;
    (*instance)->msgId = srcInstance->msgId;
    (*instance)->fileType = srcInstance->fileType;
}

/*!
 * \brief Deletes multipart/mixed viewer instance 
 *
 * \param instance The current instance.
 *****************************************************************************/
static void msaMmvDeleteInstance(MsaMmvInstance **instance)
{
    msaUiMmvStopAudio();

    if (*instance != NULL)
    {
        if ((*instance)->actions != NULL)
        {
            msaFreeActionList((*instance)->actions, (*instance)->actionCount);
            (*instance)->actions = NULL;
        }
        MSA_FREE(*instance);
        *instance = NULL;
    }
}

/*! \brief
 *
 * \param contentType The content-type of the object.
 * \return The DRM content-type including parameters, or NULL.
 *****************************************************************************/
static char *makeDrmContentType(MmsContentType *contentType)
{
    if ((NULL != contentType) && (NULL != contentType->drmInfo) && 
        (NULL != contentType->drmInfo->drmContainerType.strValue))
    {
        return we_cmmn_strdup(WE_MODID_MSA, 
            (char *)contentType->drmInfo->drmContainerType.strValue);
    }
    return NULL;
}

/*!
 * \brief Handle signal received from MOB 
 * \param inst  the current instance
 * \param op    the MOB operation
 * \param i     integer parameter to MOB operation
 * \param p     pointer parameter to MOB operation
 *****************************************************************************/
static void handleMobOp(MsaMmvInstance *inst, MsaMobOperation op, int i, 
    MtrLinkEntry *p)

{
    MtrLinkEntry *links = p;
    char *drmContentType;

    switch (op)
    {
    case MSA_MOB_SELECT:
        /* User pressed Ok */
        if (i < 0)
        {
            break;
        }
        setCurrentMo(inst, i);
        if (NULL != p)
        {
            if (!viewLink(p, MSA_MMV_FSM, MSA_SIG_MMV_MAKE_CALL_RSP))
            {
                WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                    "%s(%d): mmv failed to view link\n", 
                    __FILE__, __LINE__));                      
            }
            /*lint -e{774} */
            Msa_FreeLinks(&links);
        }
        else
        {
            if (!viewMo(inst))
            {
                WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                    "%s(%d): mmv failed to view media object\n", 
                    __FILE__, __LINE__));                      
            }
        } /* if */
        break;

    case MSA_MOB_MENU:
        /* User pressed Menu */
        setCurrentMo(inst, i);
        msaUiMmvStopAudio();
        drmContentType = makeDrmContentType(inst->currentMo->type);
        msaCrhGetActions(inst->currentMo->type, drmContentType, MSA_MMV_FSM, 
            MSA_SIG_MMV_HANDLE_ACTION_RSP);
        MSA_FREE(drmContentType);
        break;

    case MSA_MOB_FOCUS_ACTIVITY:
        /* User changed widget focus */
        msaUiMmvStopAudio();
        break;
        
    case MSA_MOB_SPECIAL_OP:
    case MSA_MOB_DEACTIVATED:
        /* do nothing */
        break;

    default:
        break;
    }
}

/*!
 * \brief Signal handler for the multi-part mixed message viewer 
 * \param sig The received signal 
 *****************************************************************************/
static void msaMmvMain(MsaSignal *sig)
{
    switch ((MsaMmvSignalId) sig->type)
    {
    case MSA_SIG_MMV_ACTIVATE:     
        msaMmvCreateInstance(&mmvInstance,(MsaMessage*)sig->p_param);

        /* activate MOB and let it do the browsing of attachments */
        msaMobActivate(mmvInstance->moList, MSA_MOB_SOP_NO_OP,
            MSA_STR_ID_VIEW_ATTACHMENTS, MSA_MMV_FSM, 
            MSA_SIG_MMV_MOB_OP);
        break; 
        
    case MSA_SIG_MMV_DEACTIVATE:
        /* Clean-up the mmv instance */
        msaMmvDeleteInstance(&mmvInstance);
        break;

    case MSA_SIG_MMV_MOB_OP:
        /* Handle key press from MOB */
        msaUiMmvStopAudio();
        if (MSA_MOB_DEACTIVATED == (MsaMobOperation)sig->u_param1)
        {
            /* User selected Back, or MOB was deactivated */
            (void)MSA_SIGNAL_SENDTO(MSA_MMV_FSM, MSA_SIG_MMV_DEACTIVATE);
        }
        else
        {
            handleMobOp(mmvInstance, (MsaMobOperation)sig->u_param1, 
                sig->i_param, sig->p_param);
        }           
        break;
        
    case MSA_SIG_MMV_MAKE_CALL_RSP:
        /* sig->u_param1 has a TRUE/FALSE result */
        /* deliberately ignored since srh handles error dialogs for us */        
        break;

    case MSA_SIG_MMV_HANDLE_ACTION_RSP:
        if (sig->u_param1 == MSA_CRH_OK)
        {
            mmvInstance->actions        = sig->p_param;
            mmvInstance->actionCount    = sig->u_param2;
            if (!msaUiMmvCreateMenu(mmvInstance->actions, 
                mmvInstance->actionCount))
            {
                WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                    "%s(%d): mmv Menu failed to open\n", 
                    __FILE__, __LINE__));                      
            }
        }
        break;
            
    case MSA_SIG_MMV_NAV_DEACTIVATE:
        /* Displayed menu was deactivated */
        msaUiMmvDeleteMenu();
        if (mmvInstance->actions != NULL)
        {
            msaFreeActionList(mmvInstance->actions, mmvInstance->actionCount);
            mmvInstance->actions = NULL;
            mmvInstance->actionCount = 0;
        }
        break;

    case MSA_SIG_MMV_OBJ_ACTION:
        /* Object action selected from menu */
        handleObjAction(mmvInstance, sig->u_param1);
        break;

    case MSA_SIG_MMV_SAVE_DONE:
        /* Save operation finished */
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "%s(%d): Save object done\n", 
            __FILE__, __LINE__));
        msaUiMmvDeleteMenu();        
        break;

    case MSA_SIG_MMV_CRH_DONE:
        /* Content router handler finished */
        if (sig->u_param1 != MSA_CRH_OK)
        {
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_ATTACH_ERROR), 
                MSA_DIALOG_ALERT);
        }
        msaUiMmvDeleteMenu();
        break;

    default: 
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "%s(%d): mmvSignalHandler erroneous signal received(%d)\n", 
            __FILE__, __LINE__, sig->type));       
        break;
    }
    msaSignalDelete(sig);
}

/*!
 * \brief Registers signal handler
 *****************************************************************************/
void msaMmvInit(void)
{
    mmvInstance = NULL;
    msaSignalRegisterDst(MSA_MMV_FSM, msaMmvMain); /* set sig-handler */
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
        "MSA MMV: initialized\n"));
}

/*!
 * \brief Terminates, by de-registering signal handler
 *****************************************************************************/
void msaMmvTerminate(void)
{
    /* Clean-up the mmv instance */
    msaMmvDeleteInstance(&mmvInstance);

    /* Deregister signal handler */
    msaSignalDeregister(MSA_MMV_FSM);
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, "MSA MMV: terminated\n"));
}
