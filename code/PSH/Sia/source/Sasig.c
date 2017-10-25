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
/* MMS */
#include "We_Lib.h"    /* WE: ANSI Standard libs allowed to use */
#include "We_Cfg.h"
#include "We_Core.h"
#include "We_Log.h"
#include "We_Act.h"

/* MMS */
#include "Mms_Cfg.h"
#include "Mms_Def.h"
#include "Mms_If.h"

/* SIS */
#include "Sis_cfg.h"
#include "Sis_if.h"
#include "Sis_def.h"

/* SMA */
#include "Sia_if.h"
#include "Satypes.h"        /* SMA: */
#include "Satimer.h"
#include "Samain.h"
#include "Saintsig.h"
#include "Saasync.h"
#include "Sasls.h"
#include "Sauiform.h"
#include "Sasig.h"
#include "Saui.h"
#include "Sasls.h"
#include "Saph.h"
#include "Sauisig.h"
#include "Saload.h"
#include "Sauidia.h"
#include "Sacore.h"
#include "Sacrh.h"
#include "Saload.h"

/*--- Definitions/Declarations ---*/

/*--- Types ---*/

/*--- Constants ---*/

/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

/*--- Global variables ---*/

/*--- Static variables ---*/

/*--- Prototypes ---*/
static void msfSignalHandler(WE_UINT16 signal, void *p);
static void mmsSignalHandler(WE_UINT16 signal, void *p);
static void slsSignalHandler(WE_UINT16 signal, void *p);
static WE_BOOL smaSignalHandler(WE_UINT8 srcModule, 
    WE_UINT16 signal, void *p);

static void handleModuleStatus (WE_UINT8 status, WE_UINT8 module);


/*!
 * \brief Router for external signals received. Routes them to the appropriate 
 *          signal handler/converter. 
 * \param src_module The internal destination the signal is intended for
 * \param signal The external signal received that we should route to its 
 *                  destination "within" SMA
 * \param p The serialized signal data
 *****************************************************************************/
void smaReceiveExtSignal(WE_UINT8 src_module, WE_UINT16 signal, void* p)
{
    /* Check if the package handler consumes this signal */
    if (smaPhHandleSignal(signal, p) || smaCrhHandleSignal(signal, p) ||
        smaDiaHandleSignal(signal, p))
    {
        WE_SIGNAL_DESTRUCT(WE_MODID_SIA, signal, p);
        return;
    } /* if */
    
    /* Dispatch signals to the correct module */
    switch(src_module)
    {
    case WE_MODID_FRW:
        msfSignalHandler(signal, p);
        break;
   
    case WE_MODID_MMS:
        mmsSignalHandler(signal, p);
        break;

    case WE_MODID_SIS:
        slsSignalHandler(signal, p);
        break;
    default:
        /* check if its a module who wants to play a SMIL */
        if (smaSignalHandler(src_module, signal, p))
        {

        } 
        else
        {
            WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA,
            "(%s) (%d) Erroneous signal recieved in meaSignalReceive\n", 
            __FILE__, __LINE__));
            WE_SIGNAL_DESTRUCT(WE_MODID_SIA, signal, p);
        }/* if */
        break;
    }
} /* smaReceiveExtSignal */

static void msfSignalHandler(WE_UINT16 signal, void *p)
{
    switch (signal) 
    {
    case WE_SIG_MODULE_TERMINATE:
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIA, "SMA: received  "
                "signal WE_SIG_MODULE_TERMINATE\n"));

            /* terminate the smil service */
            TERMINATE_SMIL_PLAYER(SIA_RESULT_OK, SIA_TERM_MODULES);
            break;
        }
    case WE_SIG_MODULE_START:
    case WE_SIG_MODULE_EXECUTE_COMMAND:
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIA, "SMA: received "
                "signal WE_SIG_MODULE_START\n"));
    
            /* Starting SMIL Service - on response start MMS */
            WE_MODULE_START(WE_MODID_SIA, WE_MODID_SIS, NULL, NULL, NULL);
            break;
        }
    case WE_SIG_MODULE_STATUS:
        {
            /* received module status */
            we_module_status_t *str = (we_module_status_t*)p;

            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIA, "SMA: received "
                "signal WE_SIG_MODULE_STATUS\n"));            
            
            /* handle the received module status */
            handleModuleStatus(str->status, str->modId);
            break;
        }
    case WE_SIG_FILE_NOTIFY:
        {
    	    if (!smaIsRunning())
		    {
			    /* sma is not currently initialized and running, skip this signal */
			    break;
		    } /* if */
        
            (void)SIA_SIGNAL_SENDTO_IU(SIA_FSM_ASYNC, SIA_SIG_ASYNC_NOTIFY,
                ((we_file_notify_t *)p)->eventType, 
                (WE_UINT32)((we_file_notify_t *)p)->fileHandle);
            break;
        }
	case WE_SIG_PIPE_NOTIFY:
        {
            (void)SIA_SIGNAL_SENDTO_IU(SIA_FSM_ASYNC, MMS_SIG_ASYNC_PIPE_NOTIFY, 
                ((we_pipe_notify_t *)p)->eventType, 
                (WE_UINT32)((we_pipe_notify_t *)p)->handle);
            break;
        }
    case WE_SIG_WIDGET_ACTION:
    case WE_SIG_WIDGET_NOTIFY:
    case WE_SIG_WIDGET_USEREVT:
        {
            if (!smaIsRunning())
		    {
			    /* sma is not currently initialized and running, skip this signal */
			    break;
		    } /* if */

		    smaWidgetSignalHandler(signal, p);
        }
        break;
    case WE_SIG_TIMER_EXPIRED:
        {
       	    we_timer_expired_t *timer = (we_timer_expired_t *)p;

            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIA, "SMA: received "
                "signal WE_SIG_TIMER_EXPIRED\n"));

            if (!smaIsRunning())
		    {
			    /* sma is not currently initialized and running, skip this signal */
			    break;
		    } /* if */
        
       	    if (timer->timerID == SIA_DIALOG_TIMER_ID && smaIsPlaying())
            {
                /* Dialog timer expired, kill dialog screen */
                widgetDeleteScreen();
            } 
            else
            {
                /* Handle objects until slide is finished */
                smaHandleTimerExpiry((SiaTimer)timer->timerID);
            }
            break;
        }
    case WE_SIG_TEL_MAKE_CALL_RESPONSE:
        {
            if (!smaIsRunning())
		    {
			    /* sma is not currently initialized and running, skip this signal */
			    break;
		    } /* if */  

            smaMakeCallRsp((we_tel_make_call_resp_t *)p);
            break;
        }
    default:
        {
            /* This was not a signal that we are interested in */
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIA, "SMA: received "
                "unknown signal: %d\n", signal));
            break;
        }
    } /* switch */
    
    if (p != NULL)
    {
        WE_SIGNAL_DESTRUCT (WE_MODID_SIA, signal, p);
    } /* if */

}

static WE_BOOL smaSignalHandler(WE_UINT8 srcModule, 
    WE_UINT16 signal, void *p)
{
    SiaPlaySmilReq *playReq;

    switch (signal) 
    {
    case SIA_SIG_PLAY_SMIL_REQ:
        playReq = (SiaPlaySmilReq *)p;

        if (smaIsRunning())
        {
            smaSendPlaySmilRsp(SIA_RESULT_BUSY);
        }
        else
        {
            smaHandlePlaySmilReq(srcModule, playReq->msgId, playReq->userData);
        } /* if */
        break;
    default:
        /* This was not a signal that we are interested in */
        return FALSE;
    } /* switch */
    
    if (p != NULL)
    {
        WE_SIGNAL_DESTRUCT (WE_MODID_SIA, signal, p);
    } /* if */
    return TRUE;
} /* smaSignalHandler */


static void mmsSignalHandler(WE_UINT16 signal, void *p)
{
    switch(signal)
    {
    case MMS_SIG_GET_BODY_PART_REPLY:
        /* Send the response to the correct FSM */
        if (SIA_GET_BP_RSP_DEST_LOAD == ((MmsGetBodyPartReply*)p)->userData)
        {
            smaLoadHandleGetBpRsp((MmsGetBodyPartReply*)p);
        }
        else if (SIA_GET_BP_RSP_DEST_CRH == ((MmsGetBodyPartReply*)p)->userData)
        {
            smaCrhHandleGetBpRsp((MmsGetBodyPartReply*)p);
        }
        break;
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA,
            "(%s) (%d) invalid signal mmsServiceSignalHandler\n", __FILE__, 
            __LINE__));
        break;
    }
    /* Delete signal */
    WE_SIGNAL_DESTRUCT(WE_MODID_SIA, signal, p);    
} /* mmsSignalHandler */


static void slsSignalHandler(WE_UINT16 signal, void *p)
{
    switch (signal) 
    {
    case SIS_OPEN_SMIL_RSP:
        {
            /* Got some smile information */
            SisOpenSmilRsp *openSmilRsp = (SisOpenSmilRsp *)p;
            
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIA, "SMA: received "
                " signal SIS_OPEN_SMIL_RSP\n"));
            
            smaHandleOpenSmilRsp(openSmilRsp);
            break;
        }
    case SIS_GET_SLIDE_INFO_RSP:
        {
			/* Got some Slide information */
            SisGetSlideInfoRsp *rsp = (SisGetSlideInfoRsp *)p;
            
			if (!smaIsRunning())
			{
				/* SMA is not running - just skip */
				break;
			} /* if */
			
            /* set slide data */
            smaHandleGetSlideInfoRsp(rsp);
            break;
        }
    case SIS_CLOSE_SMIL_RSP:
        {
	        SisCloseSmilRsp *closeSmilRsp = (SisCloseSmilRsp *)p;

            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIA, 
                "SMA: received signal SIS_CLOSE_SMIL_RSP\n"));

			if (!smaIsRunning())
			{
				/* SMA is not currently initialized and running, skip this signal */
				break;
			} /* if */
			
            if (closeSmilRsp == NULL || closeSmilRsp->result != SIS_RESULT_OK)
            {
                /* nothing to do if close failes */
                WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIA, 
                    "SMA: SIS failed to close SMIL in SIS_CLOSE_SMIL_RSP\n"));
            } 
            else if (closeSmilRsp->result == SIS_RESULT_OK)
            {
                /* start playing start playing the SMIL */
                
            }/* if */
            break;
        }
    default:
        {
            /* This was not a signal that we are interested in */
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIA, "SMA: received "
                "unknown signal: %d\n", signal));
            break;
        }
    } /* switch */
    
    if (p != NULL)
    {
        WE_SIGNAL_DESTRUCT (WE_MODID_SIA, signal, p);
    } /* if */

}


/*! \brief Handles module status information. Only information about
 *         the SIS module is used. On all other modules TRUE is returned. 
 *  \param status Status of the module.
 *  \paramn module The module to check status on.
 *  \return TRUE on success or if the module is of no importance. FALSE if SMA
 *          should terminate.
 *****************************************************************************/
static void handleModuleStatus(WE_UINT8 status, WE_UINT8 module)
{
    if (status == MODULE_STATUS_ACTIVE)
    {
        /* First SIS is started then MMS */
        if (module == WE_MODID_SIS)
        {
            /* SIS is started  */
            SISif_startInterface();
            /* start MMS */
            WE_MODULE_START(WE_MODID_SIA, WE_MODID_MMS, NULL, NULL, NULL);
        }
        else if (module == WE_MODID_MMS)
        {
            /* MMS is started  */
            MMSif_startInterface();

            if (!smaIsInitialized()) 
            {
                initSia();                
                setSiaIsInitialized(TRUE);
            }
            WE_MODULE_IS_ACTIVE(WE_MODID_SIA);
        }
    }
    else if (status == MODULE_STATUS_TERMINATED)
    {
        /* First SIS is terminated then MMS */
        if (module == WE_MODID_SIS)
        {
            WE_MODULE_TERMINATE(WE_MODID_SIA, WE_MODID_MMS);
        }
        else if (module == WE_MODID_MMS)
        {
            /* MMS is terminated - terminate SMA */
            TERMINATE_SMIL_PLAYER(SIA_RESULT_OK, SIA_TERM_DIE);
        }
    }/* if */
} /* handleModuleStatus */
