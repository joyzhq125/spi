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
#include "We_Core.h"
#include "We_Mem.h"
#include "We_Int.h"
#include "We_Wid.h"
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Pipe.h"
#include "We_File.h"
#include "We_Errc.h"
#include "We_Mem.h"

/* MMS */
#include "Mms_Def.h"

/* MSA */
#include "Msa_Vrsn.h"
#include "Msa_Def.h"
#include "Msa_Cfg.h"
#include "Msa_Env.h"
#include "Msa_Types.h"
#include "Msa_Sig.h"
#include "Msa_Intsig.h"
#include "Msa_Core.h"
#include "Msa_Mem.h"
#include "Msa_Addr.h"
#include "Msa_Pbh.h"
#include "Msa_Uidia.h"

/*!\file msa_env.c
 *	Integration functions.
 */

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static void handleException(void);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief This function is called in order to determine whether the MSA 
 *        application needs execution time.
 *
 * \return TRUE if the MSA wants execution time, otherwise FALSE.
 *****************************************************************************/
int MSAc_wantsToRun(void)
{
    if (WE_SIGNAL_QUEUE_LENGTH(WE_MODID_MSA) > 0 )
    {
        return TRUE;
    }
    /* Return the status of the internal signal queue */
    return !msaSignalQueueIsEmpty();
}

/*!
 * \brief In order for the MSA to get execution time this function is called.
 *          On each "tick", it handles one external signal and one internal
 *          signal
 *
 *****************************************************************************/
void MSAc_run(void)
{
    int res;
    /* Set roll back entry point*/
    res = setjmp(msaJmpBuf);
    msaSetInsideRun(TRUE);
    /* Process signal if a longjmp has not occured */
    if (res == 0)
    {
        /* check the external message queue */
        while(WE_SIGNAL_RETRIEVE(WE_MODID_MSA, msaSignalReceive))
            ;
        /* Process one signal in the signal queue */
        msaSignalProcess();
    }
    else
    {
        if (MSA_PANIC_EXPLICIT == msaPanicMode)
        {
            /* Shut down the MSA immediately */
            WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
                "MSAc_run explicit panic \n"));
            if (msaIsModuleStarted(WE_MODID_MMS))
            {
                /* Stop MMS service */
                msaModuleStop(WE_MODID_MMS, TRUE);
            }
            else if (msaIsModuleStarted(WE_MODID_SIA))
            {
                /* Stop SIA */
                msaModuleStop(WE_MODID_SIA, TRUE);
            }
            else if (msaIsModuleStarted(WE_MODID_SIS))
            {
                /* Stop SIS */
                msaModuleStop(WE_MODID_SIS, TRUE);
            }
            else if(msaCheckDialogs())
            {
                /* Nothing to do until the current dialog closes */
            }
            else
            {
                /* Terminate the MSA */
                msaTerminate(FALSE);
            }
        }
        else if (MSA_PANIC_IMPLICIT == msaPanicMode)
        {
            /* Shutdown the application when the panic dialog has been disabled */
            WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
                "MSAc_run implicit panic.\n"));
        }
        else
            WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "MSAc_run invalid setjmp value (%d)\n", res));
    }
    msaSetInsideRun(FALSE);
}

/*!
 * \brief Handles memory exceptions
 *
 *****************************************************************************/
static void handleException(void)
{
    if (msaIsInsideRun())
    {
        /* Terminate if inside a MSAc_run call */
        if (msaIsExceptionAllowed())
        {
            /* Report "Out of memory" to the integration */
            WE_ERROR(WE_MODID_MSA, WE_ERR_MODULE_OUT_OF_MEMORY);
            /* Perform a long jump */
            MSA_TERMINATE;
        }
        else
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) memory exception!\n", __FILE__, __LINE__));
        }
    }
    else
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) out of memory in an adaptor/connector call!\n", __FILE__,
            __LINE__));
    }
}

/*!
 * \brief Initiates MSA.
 *
 *****************************************************************************/
void MSAc_start(void)
{
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, "MSAc_start()\n"));
    /* Initiate memory manager */
    WE_MEM_INIT(WE_MODID_MSA, handleException, FALSE); /* Q04A */
    /* Initiate internal global data */
    msaInit(FALSE);

    /* Register module */
    WE_SIGNAL_REGISTER_QUEUE(WE_MODID_MSA);

    /* The module is started */
    /*lint -e{1776} */
    WE_MODULE_IS_CREATED(WE_MODID_MSA, MSA_VERSION);
}

/*!
 * \brief Kills the MSA module.
 *
 *****************************************************************************/
void MSAc_kill(void)
{
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
        "(%s) (%d) kill module is called.\n", __FILE__, __LINE__));
    WE_PIPE_CLOSE_ALL(WE_MODID_MSA);
    WE_FILE_CLOSE_ALL(WE_MODID_MSA);
    (void)WE_WIDGET_RELEASE_ALL(WE_MODID_MSA);
    WE_MEM_FREE_ALL(WE_MODID_MSA);
    WE_KILL_TASK(WE_MODID_MSA);
}

/* See the header file.
 * 
 *****************************************************************************/
WE_BOOL MSAc_pbLaunchRsp(WE_UINT16 result, WE_UINT16 wid, MsaPbItem *items)
{
    MsaAddrItem *cpy;
    MsaAddrItem *newItem;
    MsaPbItem   *tmp;

    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
        "(%s) (%d) MSAc_pbLaunchRsp called, result = %d, wid = %d!\n", __FILE__, 
        __LINE__, result, wid));
    for(tmp = items; tmp != NULL; tmp = tmp->next)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "Phone book item: type = %d, addr = %s, name = %s\n", 
            tmp->type, tmp->addr, tmp->name));
    }
    
    /* If the result is ok copy the data */
    cpy = NULL;
    if (MSA_PB_LAUNCH_RESULT_OK == result)
    {
        while(NULL != items)
        {
            if (NULL == (newItem = msaPbhCopyItem(items)))
            {
                Msa_AddrListFree(&cpy);
                return FALSE;
            }
            /* Insert item */
            if (NULL != cpy)
            {
                newItem->next = cpy;
            }
            cpy = newItem;
            /* Log item */
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                    "PB item: type = %d, name = \"%s\", addr = %s\n", 
                    newItem->addrType, newItem->name, newItem->address));
            /* Next item */
            items = items->next;
        }
    }
    /* Send the response */
    (void)MSA_SIGNAL_SENDTO_UUP(MSA_PBH_FSM, MSA_SIG_PBH_LAUNCH_RSP, 
        result, wid, cpy);
    return TRUE;
}

/* See the header file.
 * 
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL MSAc_pbLookupNameRsp(WE_UINT16 result, WE_UINT16 instanceId, 
    MsaPbItem *item)
{
    MsaAddrItem *cpyItem = NULL;
    MsaPbhLookupItem *tmp;
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
        "(%s) (%d) MSAc_pbLookupNameRsp result = %d, instanceId = %d, "
        "addrType = %d, name = %s, addr = %s\n", __FILE__, __LINE__, result, 
        instanceId, (NULL != item) ?item->type: 0, 
        (NULL != item) ? item->name: NULL, (NULL != item) ? item->addr: NULL));
    if ((MSA_PB_LOOKUP_RESULT_OK == result) && (NULL != item))
    {
        /* Copy the item */
        if (NULL == (cpyItem = msaPbhCopyItem(item)))
        {
            /* Prevent memory leak if the entry cannot be copied cannot be 
               sent */
            if (NULL != (tmp = msaPbhGetLookupEntry(instanceId)))
            {
                /*lint -e{774} */
                MSA_FREE(tmp);
            }
            return FALSE;
        }
    }
    /* Send the response */
    (void)MSA_SIGNAL_SENDTO_UUP(MSA_PBH_FSM, MSA_SIG_PBH_LOOKUP_RSP, 
        result, instanceId, cpyItem);
    return TRUE;
}
