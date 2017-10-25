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
/*! \file Sis_env.c
 * \brief  
 */

/*--- Include files ---*/
#include "We_Core.h"
#include "We_Mem.h"
#include "We_Cfg.h"
#include "We_Lib.h"
#include "We_Log.h"
#include "We_File.h"   /* WE: File handling */ 
#include "We_Errc.h"

#include "Mms_Def.h"

#include "Sis_def.h"
#include "Sis_cfg.h"
#include "Sis_if.h"
#include "Sis_main.h"
#include "Sis_vrsn.h"
#include "Sis_Isig.h"
   
/*--- Definitions/Declarations ---*/

/*--- Types ---*/

/*--- Constants ---*/

/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

/*--- Global variables ---*/

/*--- Static variables ---*/

static jmp_buf      Sis_jmpbuf;           /* Buffer for longjmp */
static int          Sis_inside_run = 0;   /* Whether or not we're in SIS_run */

/*--- Prototypes ---*/
void Sis_exception_handler(void);
/******************************************************************************/

/*!
 * \brief <Short description>
 * 
 * \param <x> <Description of x>
 * \return <whatever is returned>
 *****************************************************************************/
void SISc_start(void)
{

    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIS, "SISc_start()\n"));
    /* Initiate memory manager */
    WE_MEM_INIT (WE_MODID_SIS, Sis_exception_handler, FALSE);
    
    WE_SIGNAL_REG_FUNCTIONS (WE_MODID_SIS, slsConvert, slsDestruct);
    
    /* Initiate internal global data */
    slsInit();

    /* Initialise the external signal handler. */
    WE_SIGNAL_REGISTER_QUEUE (WE_MODID_SIS);
	/* Tell WE that the module is created. */ 
    /*lint -e{1776} */
    WE_MODULE_IS_CREATED (WE_MODID_SIS, SIS_VERSION);
   
} /* SISc_start */


/*!
 * \brief <Short description>
 * 
 * \param <x> <Description of x>
 * \return <whatever is returned>
 *****************************************************************************/
int SISc_wantsToRun(void)
{
  if (WE_SIGNAL_QUEUE_LENGTH (WE_MODID_SIS) > 0 )
  {
      return TRUE;
  }
  return !slsSignalQueueIsEmpty();
}


/*!
 * \brief <Short description>
 * 
 * \param <x> <Description of x>
 * \return <whatever is returned>
 *****************************************************************************/
void SISc_run(void)
{
    Sis_inside_run = TRUE;

    if (setjmp( Sis_jmpbuf ) == 0)
    {
        while(WE_SIGNAL_RETRIEVE(WE_MODID_SIS, slsSignalDispatcher))
            ;
        /* Process one signal in the signal queue */
        slsSignalProcess();
    }
    else
    {
       /*
        * A longjmp from memAlloc. If we are here there is no more
        * memory, and the situation could not be worse.
        * Terminate SIS and let the TDA re-initialise it again.
        */
        WE_ERROR(WE_MODID_SIS, WE_ERR_MODULE_OUT_OF_MEMORY);
        WE_FILE_CLOSE_ALL (WE_MODID_SIS);     
        WE_MEM_FREE_ALL(WE_MODID_SIS);
        WE_MODULE_IS_TERMINATED(WE_MODID_SIS);
        WE_SIGNAL_DEREGISTER_QUEUE(WE_MODID_SIS);
        WE_KILL_TASK (WE_MODID_SIS);
    } /* if */

    Sis_inside_run = FALSE;
} /* SISc_run */


void SISc_kill(void)
{
  WE_FILE_CLOSE_ALL (WE_MODID_SIS);    /* Only if the module use files. */
  WE_MEM_FREE_ALL (WE_MODID_SIS);
  WE_KILL_TASK (WE_MODID_SIS);
} /* SISc_kill */


/*!
 * \brief Used to emergency stop the SMIL Service in case of no available memory
 *
 * ONLY FOR SIS SERVICE INTERNAL USAGE.
 *****************************************************************************/
void Sis_exception_handler(void)
{
    if (Sis_inside_run)
    {
        longjmp( Sis_jmpbuf, 1);
    }
    else
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIS, 
            "%s(%d): Insufficient memory!\n", __FILE__, __LINE__));
    } /* if */
} /* Sis_exception_handler */
