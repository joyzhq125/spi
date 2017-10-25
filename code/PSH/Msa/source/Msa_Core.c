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

/* WE */
#include "We_Cfg.h"
#include "We_Mem.h"
#include "We_Def.h"
#include "We_Cmmn.h"
#include "We_Lib.h"
#include "We_Core.h"
#include "We_Wid.h"
#include "We_Log.h"
#include "We_Pipe.h"
#include "We_File.h"
#include "We_Act.h"

/* MMS */
#include "Mms_Def.h"
#include "Mms_Cfg.h"
#include "Mms_If.h"

/* SIS */
#include "Sis_Cfg.h"
#include "Sis_If.h"

/* SIA */
#include "Sia_If.h"

/* MTR */
#include "Msa_Ctrl.h"

/* MSA */
#include "Msa_Rc.h"
#include "Msa_Cfg.h"
#include "Msa_Def.h"
#include "Msa_Env.h"
#include "Msa_Types.h"
#include "Msa_Mem.h"
#include "Msa_Intsig.h"
#include "Msa_Core.h"
#include "Msa_Conf.h"
#include "Msa_Uicmn.h"
#include "Msa_Async.h"
#include "Msa_Mv.h"
#include "Msa_Mme.h"
#include "Msa_Mmv.h"
#include "Msa_Mob.h"
#include "Msa_Se.h"
#include "Msa_Mr.h"
#include "Msa_Me.h"
#include "Msa_Mcr.h"
#include "Msa_Crh.h"
#include "Msa_Comm.h"
#include "Msa_Uidia.h"
#include "Msa_Pbh.h"
#include "Msa_Cth.h"
#include "Msa_Opts.h"
#include "Msa_Del.h"
#include "Msa_Ph.h"
#include "Msa_Addr.h"
#include "Msa_Srh.h"

/*! \file macore.c
 *  \brief Handling of: start/stop of other modules, cross triggering, start-up
 *         data and handling of asynchronous error events from the MMS Service.
 */

/******************************************************************************
 * Data-types
 *****************************************************************************/
typedef struct 
{
    const char            *asString;
    const MsaStartupMode  asInteger;
}MsaModeType;

/******************************************************************************
 * Constants
 *****************************************************************************/

/* ! Command line parameters */ 
#define MSA_CL_MODE                 "mode"      /*!< The mode identifier */
#define MSA_CL_INSERT               "i"         /*!< Insert mode */
#define MSA_CL_MSG_ID               "m"         /*!< Message wid */
#define MSA_CL_TEMPLATE_ID          "t"         /*!< The message type */
#define MSA_CL_TO                   "to"        /*!< Message recipients */
#define MSA_CL_CC                   "cc"        /*!< Message recipients */
/*#define MSA_CL_SUBJECT              "subject"*/   /*!< The message subject */

/* Modes */
#define MSA_MODE_MENU         "menu"            /*!< The menu mode */
#define MSA_MODE_NEW          "new"             /*!< New message */
#define MSA_MODE_NEW_TEMPLATE "new_template"    /*!< New template */
#define MSA_MODE_VIEW         "view"            /*!< View a message */
#define MSA_MODE_DEL          "delete"          /*!< Delete a message */
#define MSA_MODE_FORWARD      "forward"         /*!< Forward a message */
#define MSA_MODE_REPLY        "reply"           /*!< Reply to a message */
#define MSA_MODE_REPLY_ALL    "reply_to_all"    /*!< Reply to a message */
#define MSA_MODE_SETTINGS     "settings"        /*!< Settings */
#define MSA_MODE_EDIT         "edit"            /*!< Edit a message */
#define MSA_MODE_SEND         "send"            /*!< Send a message */
#define MSA_MODE_DOWNLOAD     "download"        /*!< Download a message */
#define MSA_MODE_PROVISIONING "wait_for_settings"/*!<Receive provisioning */

static const MsaModeType modes[] = {
                            {MSA_MODE_NEW,          MSA_STARTUP_MODE_NEW},
                            {MSA_MODE_NEW_TEMPLATE, MSA_STARTUP_MODE_NEW_TEMPLATE},
                            {MSA_MODE_EDIT,         MSA_STARTUP_MODE_EDIT},
                            {MSA_MODE_FORWARD,      MSA_STARTUP_MODE_FORWARD},
                            {MSA_MODE_REPLY,        MSA_STARTUP_MODE_REPLY},
                            {MSA_MODE_REPLY_ALL,    MSA_STARTUP_MODE_REPLY_ALL},
                            {MSA_MODE_VIEW,         MSA_STARTUP_MODE_VIEW},
                            {MSA_MODE_DEL,          MSA_STARTUP_MODE_DELETE},
                            {MSA_MODE_SETTINGS,     MSA_STARTUP_MODE_CONFIG},
                            {MSA_MODE_SEND,         MSA_STARTUP_MODE_SEND},
                            {MSA_MODE_DOWNLOAD,     MSA_STARTUP_MODE_DOWNLOAD},
                            {MSA_MODE_PROVISIONING, MSA_STARTUP_MODE_PROVISIONING},
                            };

/*! \enum ModuleState
 *  \brief Possible states for the SIA module 
 */
typedef enum
{
    MOD_STARTED,                /* !< The module is started */
    MOD_STOPPED,                /* !< The module is stopped */
    MOD_INTERNAL_TERMINATION,   /* !< The module is terminating. The 
                                      termination is initiated through an 
                                      internal call to msaServiceStop. */
    MOD_APP_TERMINATION         /* !< The module is terminated as a part 
                                      of the termination of the application */
}ModuleState;

/******************************************************************************
 * Data-structures
 *****************************************************************************/

/*! \struct ModuleInfo
 *  \brief start-up information about other modules
 */
typedef struct
{
    ModuleState     state;      /* !< The current state of the module, see 
                                     #ModuleState */
    WE_BOOL        useCallback;/* !< TRUE if a signal is sent when the module 
                                     is terminated, otherwise FALSE */
    MsaStateMachine fsm;        /* !< The callback FSM */
    unsigned int    signal;     /* !< The signal sent to the callback FSM */
}ModuleInfo;

/*! \struct MsaSiaPlayInfo
 *  \brief structure for handling of playing SMIL messages
 */ 
typedef struct
{
    MmsMsgId        msgId;  /*!< The msgId of the message to play */
    MmsFileType     fileType; /*!< The file type of the message to start 
                                  playing */
    MsaStateMachine fsm;    /*!< The callback FSM */
    unsigned int    sig;    /*!< The callback signal */
    WE_INT32       res;    /*!< Result from playing */
}MsaSiaPlayInfo;

/******************************************************************************
 * Variables
 *****************************************************************************/

/*! The start start-up mode of the module, see #MsaStartupMode. 
 */
static MsaStartupMode       startupMode;

/*! The FSM that the MSA was started in
 */
static MsaStateMachine      startupFsm;

/*! Data-structures for handling of module start-up/termination. */
static ModuleInfo           mmsModInfo; /* !< MMS module info */
static ModuleInfo           siaModInfo; /* !< SIA module info */
static ModuleInfo           sisModInfo; /* !< SIS module info */

/*! Information about a play SMIL operation */
static MsaSiaPlayInfo siaPlay;

/*! Exception handling */
static WE_BOOL isInsideRun;    /*!< TRUE if the MSA is executing inside a 
                                     MSAc_run call, otherwise FALSE */
static WE_BOOL isExceptionAllowed; /*!< TRUE if a memory exception results in 
                                         a long jump, otherwise FALSE */
/* Used for start-up */
static MmsMsgId     msgId;

/*! Global variable for stack rollback to MSAc_run */
jmp_buf msaJmpBuf;
int msaPanicMode;

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static void parseCommandLine(const we_module_execute_cmd_t *command);
static void coreSignalHandler(MsaSignal *sig);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief Check whether memory exceptions are allowed or not.
 * 
 * \return TRUE if memory exceptions is allowed, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaIsExceptionAllowed(void)
{
    return isExceptionAllowed;
}

/*!
 * \brief Allows memory exceptions.
 * 
 * \param state TRUE if memory exceptions is allowed, otherwise FALSE.
 *****************************************************************************/
void msaAllowException(WE_BOOL state)
{
    isExceptionAllowed = state;
}

/*!
 * \brief Check whether the MSA is running through a call to MSAc_run.
 * 
 * \return TRUE if the MSA is executing through a call to MSAc_run, otherwise
 *         FALSE.
 *****************************************************************************/
WE_BOOL msaIsInsideRun(void)
{
    return isInsideRun;
}

/*!
 * \brief Changes the state that MSA is running in, i.e.,the MSA is running 
 *        through a call to MSAc_run or not.
 * 
 * \param state TRUE if the MSA is running through a call to MSAc_run, 
 *              otherwise FALSE.
 *****************************************************************************/
void msaSetInsideRun(WE_BOOL state)
{
    isInsideRun = state;
}

/*!
 * \brief This function is called in order to terminate the MSA when a 
 *        unrecoverable error has occured.
 *
 * \param showDialog TRUE if a dialog should be displayed before the shutdown,
 *                   otherwise FALSE.
 *****************************************************************************/
void msaPanic(int showDialog)
{
    if (showDialog)
    {
        if (msaShowPanicDialog())
        {
            /* Shutdown the application when the dialog disappears */
            msaPanicMode = MSA_PANIC_IMPLICIT;
            longjmp(msaJmpBuf, MSA_PANIC);
        }
    }
    /* If no dialog should be display or if the dialog could not be 
       displayed shutdown the application */
    msaPanicMode = MSA_PANIC_EXPLICIT;
    longjmp(msaJmpBuf, MSA_PANIC);
}

/*!
 * \brief Starts a service if not previously started.
 *
 * \param modId The identity of the module to start.
 * \param useCallback TRUE if a signal is sent when the module is started.
 * \param fsm The FSM to send a signal to when the module is started.
 * \param signal The signal to send when the module is started.
 *****************************************************************************/
void msaModuleStart(WE_UINT32 modId, WE_BOOL useCallback, 
    MsaStateMachine fsm, unsigned int signal)
{
    switch(modId)
    {
    case WE_MODID_MMS:
        if (MOD_STOPPED == mmsModInfo.state)
        {
            MMSif_startInterface();
            WE_MODULE_START(WE_MODID_MSA, WE_MODID_MMS, NULL, NULL, NULL);
        }
        break;

    case WE_MODID_SIA:
        if (MOD_STOPPED == siaModInfo.state)
        {
            WE_MODULE_START(WE_MODID_MSA, WE_MODID_SIA, NULL, NULL, NULL);
            siaModInfo.useCallback  = useCallback;
            siaModInfo.fsm          = fsm;
            siaModInfo.signal       = signal;
        }
        break;

    case WE_MODID_SIS:
        if (MOD_STOPPED == sisModInfo.state)
        {
            WE_MODULE_START(WE_MODID_MSA, WE_MODID_SIS, NULL, NULL, NULL);
            sisModInfo.useCallback  = useCallback;
            sisModInfo.fsm          = fsm;
            sisModInfo.signal       = signal;
        }
        break;

    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) Unsupported modId %d\n", __FILE__, __LINE__, modId));
        break;
    }
}

/*!
 * \brief Sets the MMS Service status state to started.
 *
 * \param modId The identity of the module that is started.
 *****************************************************************************/
void msaModuleStarted(WE_UINT32 modId)
{
    switch (modId)
    {
    case WE_MODID_MMS:
        mmsModInfo.state = MOD_STARTED;
        break;
    case WE_MODID_SIA:
        siaModInfo.state = MOD_STARTED;
        break;
    case WE_MODID_SIS:
        sisModInfo.state = MOD_STARTED;
        break;
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) Unsupported modId %d\n", __FILE__, __LINE__, modId));
        break;
    }
}

/*!
 * \brief Terminates the a module if it is running.
 *
 * \param modId The identity of the module that shall be stopped.
 * \param terminateApp TRUE if the entire application is terminating.
 *****************************************************************************/
void msaModuleStop(WE_UINT32 modId, WE_BOOL terminateApp)
{
    ModuleInfo *modInfo = NULL;

    switch (modId)
    {
    case WE_MODID_MMS:
        MMSif_applicationUnreg(WE_MODID_MSA);
        modInfo = &mmsModInfo;
        break;
    case WE_MODID_SIA:
        modInfo = &siaModInfo;
        break;
    case WE_MODID_SIS:
        modInfo = &sisModInfo;
        break;
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) Unsupported modId %d\n", __FILE__, __LINE__, modId));
        return;
    }
    /* Check if the application is terminating or not */
    if (terminateApp)
    {
        modInfo->state = MOD_APP_TERMINATION;
    }
    else
    {
        modInfo->state = MOD_INTERNAL_TERMINATION;
    }
    /* Send termination signal */
    WE_MODULE_TERMINATE(WE_MODID_MSA, (WE_UINT8)modId);
}

/*!
 * \brief Sets the MMS status state to stopped.
 *
 * \param modId The identity of the module that is stopped.
 *****************************************************************************/
void msaModuleStopped(WE_UINT32 modId)
{
    switch (modId)
    {
    case WE_MODID_MMS:
        mmsModInfo.state = MOD_STOPPED;
        break;
    case WE_MODID_SIA:
        siaModInfo.state = MOD_STOPPED;
        break;
    case WE_MODID_SIS:
        sisModInfo.state = MOD_STOPPED;
        break;
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) Unsupported modId %d\n", __FILE__, __LINE__, modId));
        break;
    }
}

/*!
 * \Checks whether the MMS Service has been started.
 *
 * \param modId The identity of the module that is started.
 * \return TRUE if the service is started, otherwise FALSE.
 *****************************************************************************/
WE_BOOL msaIsModuleStarted(WE_UINT32 modId)
{
    WE_BOOL started = FALSE;

    switch (modId)
    {
    case WE_MODID_MMS:
        started = (MOD_STARTED == mmsModInfo.state);
        break;
    case WE_MODID_SIA:
        started = (MOD_STARTED == siaModInfo.state);
        break;
    case WE_MODID_SIS:
        started = (MOD_STARTED == sisModInfo.state);
        break;
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) Unsupported modId %d\n", __FILE__, __LINE__, modId));
        break;
    }
    return started;
}

/*!
 *\brief Sets the startup FSM.
 *
 * \param stateM The state machine to start the application in.
 *****************************************************************************/
void msaSetStartupFsm(MsaStateMachine stateM)
{
    startupFsm = stateM;
}

/*!
 * \brief Gets the startup FSM.
 *
 * \return The startup FSM.
 *****************************************************************************/
MsaStateMachine msaGetStartupFsm(void)
{
    return startupFsm;
}

/*!
 * \brief Converts startup command line parameters into a static C 
 *        data-structure.
 *
 * \param cl A pointer to a string containing command line parameters.
 *****************************************************************************/
void msaSetStartupParameters(const we_module_execute_cmd_t *command)
{
    if (command->contentDataExists)
    {
        /* Check if the content originates from a WBA scheme */
        if (msaCthHandleSchemeCt(command))
        {
            startupMode = MSA_STARTUP_MODE_SCHEME;
        }
        /* If no start-up mode has been set, then check the command line */
        if (MSA_STARTUP_MODE_NONE == msaGetStartupMode())
        {
            parseCommandLine(command);
        }
    }
    else
    {
        parseCommandLine(command);
    }
    if ((MSA_STARTUP_MODE_DELETE != msaGetStartupMode()) && 
        (MSA_STARTUP_MODE_PROVISIONING != msaGetStartupMode()))
    {
        /*
         *	Only in DELETE and PROVISIONING startup mode do MSA not use a GUI. 
         *  In all other*  startup modes snatch focus at once. TR #14521.
         */
        (void)WE_WIDGET_SET_IN_FOCUS(msaGetScreenHandle(), TRUE);
    } /* if */
}

/*! \brief Parses the msgId.
 *
 * \param optsList The command line.
 * \param t The token to search for
 * \return The message Id, or 0 if the operation fails.
 *****************************************************************************/
static MmsMsgId getMsgId(const msa_opts_list *optsList, const char *t)
{
    msa_opts_token *token;
    MmsMsgId newMsgId = 0;
    char *data;
    if (NULL != (token = msa_opts_find_option(optsList, 
        t)))
    {
        if (((NULL != (token = msa_opts_get_next_arg(token))) &&
            msa_opts_get_str(WE_MODID_MSA, token, &data)))
        {
            newMsgId = (unsigned)we_cmmn_hex2long(data);
            MSA_FREE(data);
        }
    }
    else
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) No valid msgId for the operation\n", 
            __FILE__, __LINE__));
    }
    return newMsgId;
}

/*! \brief Handles the message properties provided through the command line, 
 *         i.e., To, Cc and Subject.
 *
 * \param optsList The current list of options.
 * \return A message property item, see #MsaPropItem.
 *****************************************************************************/
static MsaPropItem *getMsgProp(const msa_opts_list *optsList)
{
    MsaPropItem *item = NULL;
    msa_opts_token *token;
    char *data;
    MsaAddrItem *to = NULL;
    MsaAddrItem *cc = NULL;

    /* To */
    if (NULL != (token = msa_opts_find_option(optsList, MSA_CL_TO)))
    {
        if (NULL != (token = msa_opts_get_next_arg(token)))
        {
            if (msa_opts_get_str(WE_MODID_MSA, token, &data))
            {
                WE_LOG_MSG((WE_MODID_MSA, WE_LOG_DETAIL_HIGH,
                    "(%s) (%d) To option found! To = %s!\n", __FILE__, 
                    __LINE__, data));
                to = Msa_AddrStrToList(data);
            }        
        }
    }

    /* Cc */
    if (NULL != (token = msa_opts_find_option(optsList, MSA_CL_CC)))
    {
        if (NULL != (token = msa_opts_get_next_arg(token)))
        {
            if (msa_opts_get_str(WE_MODID_MSA, token, &data))
            {
                WE_LOG_MSG((WE_MODID_MSA, WE_LOG_DETAIL_HIGH,
                    "(%s) (%d) Cc option found! Cc = %s!\n", __FILE__, 
                    __LINE__, data));
                cc = Msa_AddrStrToList(data);
            }
        }
    }

    /* Subject */
/*
    if (NULL != (token = msa_opts_find_option(optsList, MSA_CL_SUBJECT)))
    {
        if (NULL != (token = msa_opts_get_next_arg(token)))
        {
            if (msa_opts_get_data(WE_MODID_MSA, token, &data))
            {
                WE_LOG_MSG((WE_MODID_MSA, WE_LOG_DETAIL_HIGH,
                    "(%s) (%d) Subject option found! subject = %s!\n", __FILE__, 
                    __LINE__, data));
                item->subject = data;
            }
        }
    }
*/
    /* Allocate structure */
    if ((NULL != to) || (NULL != cc))
    {
        item = MSA_ALLOC(sizeof(MsaPropItem));
        memset(item, 0, sizeof(MsaPropItem));
        item->to = to;
        item->cc = cc;
    }
    return item;
}

/*!
 * \brief Converts startup command line parameters into a static C 
 *        data-structure.
 *
 * \param cl A pointer to a string containing command line parameters.
 *****************************************************************************/
static void parseCommandLine(const we_module_execute_cmd_t *command)
{
    msa_opts_list *optsList;
    msa_opts_token *token;
    char *data;
    unsigned int i;
    char *cl;
    we_content_data_t *cData = NULL;
    /* Set default values */

    cl = command->commandOptions;
    startupMode = MSA_STARTUP_MODE_NEW;
    if (NULL == cl)
    {
        return;
    }

    if (NULL == (optsList = msa_opts_get_list(WE_MODID_MSA, cl)))
    {
        return;
    }
    /* Find the mode */
    if (NULL == (token = msa_opts_find_option(optsList, MSA_CL_MODE)))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) Erroneous start-up parameters: -mode missing\n", __FILE__, 
            __LINE__));
        (void)MSA_SIGNAL_SENDTO(MSA_CORE_FSM, MSA_SIG_CORE_TERMINATE_APP);
            msa_opts_free_list(WE_MODID_MSA, optsList);
            return;
    }
    /* Find the current mode */
    if (NULL != (token = msa_opts_get_next_arg(token)))
    {
        if (msa_opts_get_str(WE_MODID_MSA, token, &data))
        {
            for (i = 0; NULL != modes[i].asString; i++)
            {
                if (0 == we_cmmn_strcmp_nc(data, modes[i].asString))
                {
                    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                        "(%s) (%d) startup mode [%s] found\n", __FILE__, 
                        __LINE__, data));
                    startupMode = modes[i].asInteger;
                    break;
                }
            }
            MSA_FREE(data);
            /* Now the command line parameters are dependant on the mode  */
            switch(startupMode)
            {
            case MSA_STARTUP_MODE_SCHEME:
            case MSA_STARTUP_MODE_PROVISIONING:
                /* No more information is needed */
                break;
            case MSA_STARTUP_MODE_NEW:
                msgId = 0;
                cData = NULL;
                if (NULL != (token = msa_opts_find_option(optsList, 
                    MSA_CL_TEMPLATE_ID)))
                {
                    /* A new message needs to be created using a template. 
                       First to the wid of the template needs to be found. */
                    msgId = getMsgId(optsList, MSA_CL_TEMPLATE_ID);
                }
                /* check for the -i option */
                else if (NULL != (token = msa_opts_find_option(optsList, 
                    MSA_CL_INSERT)))
                {
                    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                        "(%s) (%d) insert mode detected\n", __FILE__, 
                        __LINE__));
                    cData = (command->contentDataExists) ?
                        command->contentData: NULL;
                }
                /* Set the insert data direct */
                if (!msaCthHandleInsertContentCt(cData, getMsgProp(optsList)))
                {
                    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                        "(%s) (%d) Failed to insert content.\n", __FILE__, 
                        __LINE__));
                }
                break;

            case MSA_STARTUP_MODE_CONFIG:
                /* No parameters are needed for settings */
                break;

            case MSA_STARTUP_MODE_NEW_TEMPLATE:
                break;

            case MSA_STARTUP_MODE_DELETE:
            case MSA_STARTUP_MODE_FORWARD:
            case MSA_STARTUP_MODE_REPLY:
            case MSA_STARTUP_MODE_REPLY_ALL:
            case MSA_STARTUP_MODE_VIEW:
            case MSA_STARTUP_MODE_SEND:
            case MSA_STARTUP_MODE_DOWNLOAD:
            case MSA_STARTUP_MODE_EDIT:
                msgId = getMsgId(optsList, MSA_CL_MSG_ID);
                break;

            case MSA_STARTUP_MODE_NONE:
            default:
                WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                    "(%s) (%d) Erroneous start-up parameters\n", __FILE__, 
                    __LINE__));
                (void)MSA_SIGNAL_SENDTO(MSA_CORE_FSM, MSA_SIG_CORE_TERMINATE_APP);
                break;
            }
        }
    }
    msa_opts_free_list(WE_MODID_MSA, optsList);
}

/*!
 * \brief Terminates MSA.
 *
 * \param internalOnly The module is only terminated internally, i.e., the 
 *                     MSA module is not terminated.
 *****************************************************************************/
void msaTerminate(WE_BOOL internalOnly)
{
    /* Terminate all FSMs */
    msaSignalTerminate();  /* Clear internal signal queue */
    msaCoreTerminate(internalOnly);
    Msa_AsyncTerminate();
    msaConfTerminate();    
    msaDelTerminate();
    msaMvTerminate();
    msaMeTerminate();
    msaSeTerminate();
    msaMmeTerminate();
    msaMmvTerminate();
    msaMobTerminate();
    msaMcrTerminate();
    msaCrhTerminate();
    Msa_CommTerminate();
    msaMrTerminate();
    msaPbhTerminate();
    msaCthTerminate();
    /* Terminate user interface */
    msaTerminateGui();
    msaPhTerminate();
    msaSrhTerminate();
    mtrCtrlTerminate(); 

    if (!internalOnly)
    {
        /* Deregister signal handler */
        WE_SIGNAL_DEREGISTER_QUEUE(WE_MODID_MSA);
        /* Free all resources just to be sure */
        WE_PIPE_CLOSE_ALL(WE_MODID_MSA);
        WE_FILE_CLOSE_ALL(WE_MODID_MSA);
        (void)WE_WIDGET_RELEASE_ALL(WE_MODID_MSA);
        WE_MEM_FREE_ALL(WE_MODID_MSA);
        /* Set module as terminated */
        WE_MODULE_IS_TERMINATED(WE_MODID_MSA);
        WE_KILL_TASK(WE_MODID_MSA);

        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
            "(%s) (%d) MSA is terminated\n", __FILE__, __LINE__));
    }
    else
    {
        /* Free resources */
        (void)WE_WIDGET_RELEASE_ALL(WE_MODID_MSA);
        WE_PIPE_CLOSE_ALL(WE_MODID_MSA);
        WE_FILE_CLOSE_ALL(WE_MODID_MSA);
        /* Do not free all memory since then the content routing info is lost*/
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) MSA is terminated internaly!\n", __FILE__, __LINE__));
    }
}

/*!
 * \brief Returns a flag describing how MSA was started
 *
 * \return The start-up mode.
 *****************************************************************************/
MsaStartupMode msaGetStartupMode()
{
    return startupMode;
}

/*!
 * \brief Handles error signals from the MMS Service.
 *
 * \param result The result code for the error encountered, see #MmsResult. 
 *****************************************************************************/
void msaHandleMmsErrors(int result)
{
    switch(result)
    {
    /*! An #MMSc_sendMessage or #MMSc_retrieveMessage operation is cancelled
     *  by an #MMSc_cancel. */
    case MMS_RESULT_CANCELLED_BY_USER:
    /*! Communication error: The address could not be resolved. */
    case MMS_RESULT_COMM_ADDRESS_UNRESOLVED:
    /*! Communication error: Bad gateway. */
    case MMS_RESULT_COMM_BAD_GATEWAY:
    /*! Communication error: Entity too large. */
    case MMS_RESULT_COMM_ENTITY_TOO_LARGE:
    /*! Communication error: Unknown communication error. */
    case MMS_RESULT_COMM_FAILED:
    /*! Communication error: File or message not found. */        
    case MMS_RESULT_COMM_FILE_NOT_FOUND:
    /*! Communication error: Communication not allowed, service or method 
     *  denied. */
    case MMS_RESULT_COMM_FORBIDDEN:
    /*! Communication error: PDU did not contain expected tag. */
    case MMS_RESULT_COMM_HEADER_TAG:
    /*! Communication error: An illegal or badly formatted PDU was received. */
    case MMS_RESULT_COMM_ILLEGAL_PDU:
    /*! Communication error: The operation (e.g. Reply Charging) had 
     *  limitations that were violated in the request. */
    case MMS_RESULT_COMM_LIMITATIONS_NOT_MET:
    /*! Communication error: The message size is specified in the notification.
     *  When the message is retrieved, it is larger than specified.
     */
    case MMS_RESULT_COMM_PROXY_ERROR:
    /*! Communication error: The MMS Proxy-Relay did not recognize one PDU
     *  that the MMS Service sent. This is probably due to a mis-configuration
     *  of the MMS Proxy-Relay version. */
    case MMS_RESULT_COMM_MMSC_VERSION:
    /*! Communication error: Error in TLS/WTLS setup. */
    case MMS_RESULT_COMM_SECURITY_ERROR:
    /*! Communication error: Unknown MMS Proxy-Relay error. */
    case MMS_RESULT_COMM_SERVER_ERROR:
    /*! Communication error: Wrong communication status. */
    case MMS_RESULT_COMM_STATUS:
    /*! Communication error: Operation timed out, e.g. no contact with the MMS 
     *  Proxy-Relay. */
    case MMS_RESULT_COMM_TIMEOUT:
    /*! Communication error: Unexpected transaction WID. */
    case MMS_RESULT_COMM_TRANSACTION_ID:
    /*! Communication error: Unauthorized. */
    case MMS_RESULT_COMM_UNAUTHORIZED:
    /*! Communication error: Service unavailable or not supported. */
    case MMS_RESULT_COMM_UNAVAILABLE:
    /*! Communication error: Unexpected message received. */
    case MMS_RESULT_COMM_UNEXPECTED_MESSAGE:
    /*! Communication error: Unsupported media type or content. */
    case MMS_RESULT_COMM_UNSUPPORTED_MEDIA_TYPE:
    /*! Communication error: Unsupported version. */
    case MMS_RESULT_COMM_UNSUPPORTED_VERSION:
    /*! Communication error: URI is too large. */
    case MMS_RESULT_COMM_URI_TOO_LARGE:
    /*! A message for a not supported MMS version is received */
    case MMS_RESULT_COMM_WRONG_MMS_VERSION:
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_ERROR), 
            MSA_DIALOG_ERROR);
        break;
    /*! The MMS Client has encountered an unrecoverable application error.
     *  The MMS Client must be stopped and restarted by the MMS Application.
     */
    case MMS_RESULT_RESTART_NEEDED:
        /* Close down application. Cannot make longjmp since we will have a 
         * memory leak */
        (void)MSA_SIGNAL_SENDTO(MSA_CORE_FSM, MSA_SIG_CORE_TERMINATE_APP);        
        break;
    case MMS_RESULT_OK:
        break;
    /*! One of the fields To, Cc, Bcc is invalid */
    case MMS_RESULT_INVALID_ADDRESS:
    /*! Invalid or not existing application header field. */
    case MMS_RESULT_INVALID_APPLICATION_HEADER:
    /*! Invalid or not existing Content-Id. */
    case MMS_RESULT_INVALID_CONTENT_ID:
    /*! Invalid or not existing Content-Location. */
    case MMS_RESULT_INVALID_CONTENT_LOCATION:

    /*! Content type is invalid. */
    case MMS_RESULT_INVALID_CONTENT_TYPE:
    
    /*! Expiry time is invalid. */
    case MMS_RESULT_INVALID_EXPIRY_TIME:

    /*! The total size of the MMS header is invalid. */
    case MMS_RESULT_INVALID_HEADER_SIZE:

    /*! Message class is invalid. */
    case MMS_RESULT_INVALID_MSG_CLASS:

    /*! Reply Charging Deadline is invalid.  */
    case MMS_RESULT_INVALID_REPLY_CHARGING_DEADLINE:

    /*! The storage type is invalid. */
    case MMS_RESULT_INVALID_STORAGE_TYPE:

    /*! Subject is invalid. */
    case MMS_RESULT_INVALID_SUBJECT:

    /*! The MMS version number is invalid for the requested operation. */
    case MMS_RESULT_INVALID_VERSION:
   
    /*! The message is corrupt. */
    case MMS_RESULT_MESSAGE_CORRUPT:

    /*! The message was empty. */
    case MMS_RESULT_MESSAGE_EMPTY:
    
    /*! The message with the specified message WID could not be found. */
    case MMS_RESULT_MESSAGE_ID_NOT_FOUND:

    /*! No report should be sent to this message. */
    case MMS_RESULT_NO_REPORT:

    /*! The queue holding immediate retrieval notifications is full. */
    case MMS_RESULT_NOTIFICATION_QUEUE_FULL:

    /*! The recipient address is not set in the message. */
    case MMS_RESULT_RECIPIENT_ADDRESS_NOT_SET:
    
    /*! The MMS Client has been stopped due to an internal error. */
    case MMS_RESULT_STOPPED:

    /*! The URI is not valid. */
    case MMS_RESULT_URI_NOT_VALID:
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_INVALID_MESSAGE), 
            MSA_DIALOG_INFO);
        break;
    /*! Not enough persistent storage for the requested operation. */
    case MMS_RESULT_INSUFFICIENT_PERSISTENT_STORAGE:
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_NO_PERSISTENT_STORAGE), 
            MSA_DIALOG_ALERT);
        break;
    /*! Not enough memory for the requested operation. */
    case MMS_RESULT_INSUFFICIENT_MEMORY:
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_NOT_ENOUGH_MEM), 
            MSA_DIALOG_ERROR);
        break;
    /*! The supplied buffer is full */
    case MMS_RESULT_BUFFER_FULL:
   /*! Too many consecutive calls to a connector function have been made without 
    *  waiting for the outcome of the previous calls.
    */        
    case MMS_RESULT_BUSY:
    /*! A requested body part was not found */
    case MMS_RESULT_BODY_PART_NOT_FOUND:
    /*! Configuration is not correct. */
    case MMS_RESULT_CONFIG_ERROR:
    /*! Index file is corrupt or does not exist. */
    case MMS_RESULT_INDEX_FILE_ERROR:
    /*! The current file operation cannot be performed at the moment. */
    case MMS_RESULT_DELAYED:
    /*! An unknown error has occurred. No further information can be given. */
    case MMS_RESULT_ERROR:
    /*! The End Of File has been reached. */
    case MMS_RESULT_EOF:
    /*! Error when creating forward header. */
    case MMS_RESULT_ERROR_CREATING_FORWARD:
    /*! Error when creating a Read Report. */
    case MMS_RESULT_ERROR_CREATING_READ_REPORT:
    /*! Error when creating send header. */
    case MMS_RESULT_ERROR_CREATING_SEND_HEADER:
    /*! Error creating backup file. */
    case MMS_RESULT_FILE_BACKUP_ERROR:
    /*! A file is not found.*/
    case MMS_RESULT_FILE_NOT_FOUND:
    /*! Error reading file. */
    case MMS_RESULT_FILE_READ_ERROR:
    /*! Error when trying write to file */
    case MMS_RESULT_FILE_WRITE_ERROR:
    /*! The folder is not found. */
    case MMS_RESULT_FOLDER_NOT_FOUND:
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "MSA MMS Sig error (%d)\n", result));
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_ERROR), 
            MSA_DIALOG_ERROR);
        break;
    case MMS_RESULT_MAX_RETRIEVAL_SIZE_EXCEEDED:
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_RETR_SIZE_EXCEEDED), 
            MSA_DIALOG_INFO);
        break;
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) Unhandled MMS Service result\n", __FILE__, __LINE__));
        break;
    }
}

/*!
 * \brief Initiates the CRR FSM.
 *
 * \param internalOnly TRUE if the modules is re-initiated, FALSE if the init
 *                     is called during a module start-up.
 *****************************************************************************/
void msaCoreInit(WE_BOOL internalOnly)
{
    startupMode         = MSA_STARTUP_MODE_NONE;
    startupFsm          = MSA_NOTUSED_FSM;
    
    if (!internalOnly)
    {
        mmsModInfo.state = MOD_STOPPED;
        siaModInfo.state = MOD_STOPPED;
        sisModInfo.state = MOD_STOPPED;
    }
    isInsideRun         = FALSE;
    isExceptionAllowed  = TRUE;
    msgId               = (MmsMsgId)0;
    memset(&siaPlay, 0, sizeof(MsaSiaPlayInfo));
    siaPlay.fsm = MSA_NOTUSED_FSM;
    msaSignalRegisterDst(MSA_CORE_FSM, coreSignalHandler);
}

/*!
 * \brief Terminates the CRR FSM.
 *
 * \param internalOnly TRUE if the modules is terminating internally, FALSE if 
 *                     the terminate is called during a module termination.
 *****************************************************************************/
void msaCoreTerminate(WE_BOOL internalOnly)
{
    msaSignalDeregister(MSA_CORE_FSM);
    if (!internalOnly)
    {
        mmsModInfo.state = MOD_STOPPED;
        siaModInfo.state = MOD_STOPPED;
        sisModInfo.state = MOD_STOPPED;
    }
    startupMode         = MSA_STARTUP_MODE_NONE;
    isInsideRun         = FALSE;
    isExceptionAllowed  = FALSE;
    msgId               = (MmsMsgId)0;
    siaPlay.fsm = MSA_NOTUSED_FSM;
}

/*!
 * \brief The signal handler for the CRR FSM.
 *
 * \param sig The current signal.
 *****************************************************************************/
static void coreSignalHandler(MsaSignal *sig)
{
    ModuleInfo *modInfo;

    /* Check which signal that was received */
    switch(sig->type)
    {
    case MSA_SIG_CORE_TERMINATE_APP:
        msaSignalDelete(sig);
        MSA_TERMINATE;
        break;
        
    case MSA_SIG_CORE_MODULE_ACTIVE:
        modInfo = NULL;
        if (WE_MODID_SIA == sig->u_param1)
        {
            modInfo = &siaModInfo;
            /* Start the SIA interface */
            SIAif_startInterface();

        }else if (WE_MODID_SIS == sig->u_param1)
        {
            modInfo = &sisModInfo;
            /* Start the SIS interface */
            (void)SISif_startInterface();
        }
        if (NULL != modInfo)
        {
            /* Set the state to started */
            modInfo->state = MOD_STARTED;
            /* Tell the starting FSM that the SIA is started */
            if (modInfo->useCallback)
            {
                (void)MSA_SIGNAL_SENDTO(modInfo->fsm, (int)modInfo->signal);
            }
        }
        break;

    case MSA_SIG_CORE_MODULE_TERMINATED:
        if (WE_MODID_MMS == sig->u_param1)
        {
            msaSignalDelete(sig);
            mmsModInfo.state = MOD_STOPPED;
            /* Long jump */
            MSA_TERMINATE;
            break;
        }
        else if (WE_MODID_SIA == sig->u_param1)
        {
            if (MOD_APP_TERMINATION == siaModInfo.state)
            {
                msaSignalDelete(sig);
                siaModInfo.state = MOD_STOPPED;
                /* Long jump */
                MSA_TERMINATE;
                break;
            }
            else
            {
                /* Send the response to the calling FSM if the SIA have 
                   terminated whithout response */
                if (MSA_NOTUSED_FSM != siaPlay.fsm)
                {
                    (void)MSA_SIGNAL_SENDTO_U(siaPlay.fsm, (int)siaPlay.sig,siaPlay.res);
                }
                /* Make sure only one response is valid */
                siaPlay.fsm = MSA_NOTUSED_FSM;
            }
            siaModInfo.state = MOD_STOPPED;
        }
        else if (WE_MODID_SIS == sig->u_param1)
        {
            if (MOD_APP_TERMINATION == sisModInfo.state)
            {
                msaSignalDelete(sig);
                sisModInfo.state = MOD_STOPPED;
                /* Long jump */
                MSA_TERMINATE;
                break;
            }
            sisModInfo.state = MOD_STOPPED;
        }
        break;

    case MSA_SIG_CORE_SIA_STARTED_PLY:
        /* Start playing the SMIL presentation if the SIA module is started */
        (void)SIAif_playSmil(WE_MODID_MSA, siaPlay.msgId, siaPlay.fileType);
        break;

    case MSA_SIG_CORE_SIA_STOP:
        /* Stop the SIA module when finnished plying the SMIL presentation */
        /* the callback is handled in module stop */
        siaPlay.res=sig->u_param1;
        msaModuleStop(WE_MODID_SIA, FALSE);
        /*if the SIA has failed  SIA has already displayed this to the user*/
       break;

    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) Unhandled signal in coreSignalHandler!\n", __FILE__, 
            __LINE__));
        break;
    }
    msaSignalDelete(sig);
}

/*!
 * \brief Initiates all FSM's of the MSA.
 * \param internalOnly TRUE if the module is restarting, else FALSE.
 *****************************************************************************/
void msaInit(WE_BOOL internalOnly)
{
    /* Init internal signal queue*/
    msaSignalInit();
    /* Init the UI */
    msaInitGui();

    /* Init all FSMs */
    if (!Msa_AsyncInit())
    {
        msaPanic(TRUE);
    }
    msaCoreInit(internalOnly);
    msaConfInit();
    msaMvInit();
    msaMeInit();
    msaSeInit();
    msaMmeInit();
    msaMmvInit();
    msaMobInit();
    msaCrhInit();
    Msa_CommInit();
    msaMcrInit();
    msaMrInit();
    msaPbhInit();
    msaCthInit();
    msaSrhInit();
    mtrCtrlInit(); 
    msaDelInit();

    /* Packages */
    if (!msaPhInit())
    {
        msaPanic(TRUE);
    }
}

/*!
 * \brief Starts the GUI for the application.
 *
 *****************************************************************************/
void msaActivateStartupFsm(void)
{
    /* Where to start is dependent on which mode the application is started */
    switch(msaGetStartupMode())
    {
    case MSA_STARTUP_MODE_NEW:
        if (0 == msgId)
        {
            /* This is a new message */
            (void)MSA_SIGNAL_SENDTO_U(MSA_CTH_FSM, MSA_SIG_CTH_START, 
                startupMode);
        }
        else
        {
            /* This is a new message using a template */
            (void)MSA_SIGNAL_SENDTO_IUU(MSA_ME_FSM, MSA_SIG_ME_ACTIVATE, 0,
                MSA_ME_STARTUP_FROM_TEMPLATE, msgId);
        }
        break;
    case MSA_STARTUP_MODE_NEW_TEMPLATE:
        (void)MSA_SIGNAL_SENDTO_IU(MSA_ME_FSM, MSA_SIG_ME_ACTIVATE, 0,
            MSA_ME_STARTUP_NEW_TEMPLATE);
        break;
    case MSA_STARTUP_MODE_EDIT:
        (void)MSA_SIGNAL_SENDTO_IUU(MSA_ME_FSM, MSA_SIG_ME_ACTIVATE, 0,
            MSA_ME_STARTUP_EDIT, msgId);
        break;
    case MSA_STARTUP_MODE_FORWARD:
        (void)MSA_SIGNAL_SENDTO_IUU(MSA_ME_FSM, MSA_SIG_ME_ACTIVATE, 0,
            MSA_ME_STARTUP_FORWARD, msgId);
        break;
    case MSA_STARTUP_MODE_REPLY:
        (void)MSA_SIGNAL_SENDTO_IUU(MSA_ME_FSM, MSA_SIG_ME_ACTIVATE, 0, 
            MSA_ME_STARTUP_REPLY, msgId);
        break;
    case MSA_STARTUP_MODE_REPLY_ALL:
        (void)MSA_SIGNAL_SENDTO_IUU(MSA_ME_FSM, MSA_SIG_ME_ACTIVATE, 0, 
            MSA_ME_STARTUP_REPLY_ALL, msgId);
        break;
    case MSA_STARTUP_MODE_DELETE:
        msaDeleteMsg(msgId, FALSE);
        break;
    case MSA_STARTUP_MODE_SCHEME:
        (void)MSA_SIGNAL_SENDTO_U(MSA_CTH_FSM, MSA_SIG_CTH_START, 
            startupMode);
        break;
    case MSA_STARTUP_MODE_VIEW:
        (void)MSA_SIGNAL_SENDTO_U(MSA_MV_FSM, MSA_SIG_MV_ACTIVATE, msgId);
        break;
    case MSA_STARTUP_MODE_CONFIG:
        /* Activate the configuration FSM */
        (void)MSA_SIGNAL_SENDTO(MSA_CONFIG_FSM, MSA_SIG_CONF_ACTIVATE);
        break;
    case MSA_STARTUP_MODE_SEND:
        /* Activate the sending FSM */
        Msa_SendMessage(MSA_NOTUSED_FSM, 0, msgId, TRUE);
        break;
    case MSA_STARTUP_MODE_DOWNLOAD:
        /* Activate the sending FSM */
        Msa_DownloadMessage(MSA_NOTUSED_FSM, 0, msgId);
        break;
    case MSA_STARTUP_MODE_PROVISIONING:
        /* Wait for provisioning data */
        break;
    case MSA_STARTUP_MODE_NONE:
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) Erroneous start-up mode!", __FILE__, __LINE__));
        MSA_TERMINATE;
        break;
    }
}

/*!
 * \brief Starts the SMIL player and plays the message.
 *
 * \param msgId The message identity of the message to start playing.
 * \param fileType The file type of the message. See #MmsFileType.
 * \param fsm The callback fsm (called when the player is stopped)
 * \param sig The callback signal.
 *****************************************************************************/
void msaPlaySmil(MmsMsgId pMsgId, MmsFileType fileType, MsaStateMachine fsm, 
    unsigned int sig)
{
    siaPlay.msgId       = pMsgId;
    siaPlay.fsm         = fsm;
    siaPlay.sig         = sig;
    siaPlay.fileType    = fileType;
    msaModuleStart(WE_MODID_SIA, TRUE, MSA_CORE_FSM, 
        MSA_SIG_CORE_SIA_STARTED_PLY);
}
