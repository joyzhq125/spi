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

/*--- Include files ---*/
/* WE */
#include "We_Cfg.h"
#include "We_Core.h"
#include "We_Mem.h"
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Pipe.h"
#include "We_File.h"
#include "We_Wid.h"
#include "We_Pck.h"
#include "We_Act.h"

/* SIS */
#include "Sis_cfg.h"
#include "Sis_def.h"
#include "Sis_if.h"

/* MMS */
#include "Mms_Cfg.h"
#include "Mms_Def.h"
#include "Mms_If.h"

/* SMA */
#include "Sia_if.h"
#include "Satypes.h"
#include "Samain.h"
#include "Saintsig.h"
#include "Sauiform.h"
#include "Saui.h"
#include "Saslide.h"
#include "Saasync.h"
#include "Sia_def.h"
#include "Saph.h"
#include "Satimer.h"
#include "Saslide.h"
#include "Saui.h"
#include "Saload.h"
#include "Sadsp.h"
#include "Sacore.h"
#include "Sacrh.h"
#include "Saattach.h"
#include "Sasls.h"

/*--- Definitions/Declarations ---*/
/*! Defines the first slide */

/*--- Types ---*/

/* \struct SiaPlayerInfo Static information */
typedef struct
{
    WE_BOOL        isRunning;
    WE_BOOL        isInitialized;
} InstanceData;
/*--- Constants ---*/

/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

/*--- Global variables ---*/
 
/*--- Static variables ---*/

/* Information about the SMIL player and the current state. */
static InstanceData instData;

/* result code for exit operation */
static SiaResult    Sia_exit_result;
/* Buffer used for long jumps */
static jmp_buf *panicJmpbuf;
/* Variable for keeping panic code */
int smaPanicMode = SIA_PANIC_NO_DIALOG;

/* Buffer used for text convertion in function convertToUTF8()*/
/*--- Prototypes ---*/

static void signalHandler(SiaSignal *sig);

/*****************************************************************************
 * Exported functions                                                        *
 *****************************************************************************/

/*! \brief Initiates the SMA module. 
*****************************************************************************/
void smaCoreInit(void)
{
    /* register signal handler */
    smaSignalRegisterDst(SIA_FSM_CORE, signalHandler);
    memset(&instData, 0x00, sizeof(InstanceData));
} /* smaCoreInit */

/*! \brief Terminate the SMA Core.
 *****************************************************************************/    
void smaCoreTerminate(void)
{
} /* smaMainTerminate */
  
/*! \brief Exit the SMIL player, NOTE the SMA module is not terminated 
 *         when this function is called. This function should only be called
 *         from internal functions.
 * \param mode termination mode.
 * \param result the result code 
 *****************************************************************************/ 
void smaPanic(int mode, SiaResult result)
{
    Sia_exit_result = result;
    smaPanicMode = mode;
	longjmp(*panicJmpbuf, SIA_PANIC);	
} /* smaPanic */



/*! \brief Gets the Sia_exit_result
 *
 * \return Sia_exit_result
*****************************************************************************/
SiaResult smaGetSiaExitResult(void)
{
	return Sia_exit_result;
} /* smaGetSiaExitResult */


/*! \brief Must be called prior to smaPanic, to set the buffer for longjump
 *
*****************************************************************************/
void smaSetJumpBuffer(jmp_buf *jmpbuf)
{
	panicJmpbuf = jmpbuf;
}


/*! \brief Clear all current the SMIL player information, NOTE the SMA module 
 *   is not terminated by this function.
 *
 * \param result The result code to return
 * \sendRsp TRUE if the result signal should be sent to be sent, else FALSE
 *****************************************************************************/                   
void exitSmilPlayer(SiaResult result, WE_BOOL sendRsp)
{
	/* nothing to do if failed to stop playing */
    smaStopSmil();

    if (sendRsp)
	{
		smaSendPlaySmilRsp(result);
	} /* if */
    terminateSia();
} /* exitSmilPlayer */

/*!
 * \brief Signal handler
 * 
 * \param sig the signal
 *****************************************************************************/
static void signalHandler(SiaSignal *sig)
{
    switch (sig->type) 
    {
    case SIA_SIG_CORE_TERMINATE_APP:
        if (SIA_TERM_EXIT == sig->u_param1) 
        {
            smaSendPlaySmilRsp((SiaResult)sig->i_param);
        }
        else if (SIA_TERM_MODULES == sig->u_param1)
        {
            /* We always close SIS since it cannot exist in several instances */
            WE_MODULE_TERMINATE(WE_MODID_SIA, WE_MODID_SIS);            
        }
        else
        {
            smaSignalDelete(sig);
            smaPanic(SIA_PANIC_TERMINATE, (SiaResult)sig->i_param);
        }
        break;
    default:
        WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_SIA, 
            "MACORE: received unknown signal: %d\n", sig->type));
    }
    smaSignalDelete(sig);
} /* signalHandler */


/*!
 * \brief Initialize the SMA
 * 
 *****************************************************************************/
void initSia(void)
{
    /* initialize internal signal handling */
    smaSignalInit();

    /* initialize the Message handling FSM */
    if (smaMainInit() == FALSE || 
         smaAsyncInit() == FALSE)
    {
        EXIT_SMIL_PLAYER(SIA_RESULT_ERROR);
    } /* if */
    smaCoreInit();
    smaLoadInit();
    smaSisInit();
    smaTimerInit();
    smaDispatchInit();
    smaSlideInit();
    smaSlideFormInit();
    (void)smaCrhInit();
	smaAttachmentInit();
    smaUiInit();
    /* Packages */
    if (!smaPhInit())
    {
        EXIT_SMIL_PLAYER(SIA_RESULT_ERROR);
    }
    setSiaIsInitialized(TRUE);
} /* initSia */
  

void terminateSia(void)
{
    smaSignalTerminate();
    smaMainTerminate();

    smaLoadTerminate();
    smaSlsTerminate();
    smaSlideFormTerminate();
    smaCoreTerminate();
    smaDispatchTerminate();
    smaTimerTerminate();
    smaSlideTerminate();
	smaAsyncTerminate();
    smaCrhTerminate();
	smaAttachmentTerminate();
    smaUiTerminate();
    smaPhTerminate();

    setSiaIsRunning(FALSE);
    setSiaIsInitialized(FALSE);
    
    /* Deregister signal handler */
    WE_SIGNAL_DEREGISTER_QUEUE(WE_MODID_SIA);
    /* Free all resources just to be sure */
    WE_PIPE_CLOSE_ALL(WE_MODID_SIA);
    WE_FILE_CLOSE_ALL(WE_MODID_SIA);
    (void)WE_WIDGET_RELEASE_ALL(WE_MODID_SIA);
    WE_MEM_FREE_ALL(WE_MODID_SIA);

    /* Set module as terminated */
    WE_MODULE_IS_TERMINATED(WE_MODID_SIA);
    WE_KILL_TASK(WE_MODID_SIA);

    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIA, 
        "(%s) (%d) SMA is terminated\n", __FILE__, __LINE__));
} /* terminateSia */


/*!
 * \brief Returns the busy state of the SMA
 * 
 * \return TRUE if the SMA is busy, else FALSE
 *****************************************************************************/
WE_BOOL smaIsRunning(void)
{
    return instData.isRunning;
} /* smaIsRunning */

/*!
 * \brief Set the busy state of the SMA
 * 
 * \param state TRUE if the SMA is busy, else FALSE
 *****************************************************************************/
void setSiaIsRunning(WE_BOOL state)
{
    instData.isRunning = state;
} /* setSiaIsRunning */

/*!
 * \brief Returns the busy state of the SMA
 * 
 * \return TRUE if the SMA is busy, else FALSE
 *****************************************************************************/
WE_BOOL smaIsInitialized(void)
{
    return instData.isInitialized;
} /* smaIsInitialized */

/*!
 * \brief Set the busy state of the SMA
 * 
 * \param state TRUE if the SMA is busy, else FALSE
 *****************************************************************************/
void setSiaIsInitialized(WE_BOOL state)
{
    instData.isInitialized = state;
} /* setSiaIsInitialized */
