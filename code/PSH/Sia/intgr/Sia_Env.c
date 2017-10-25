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
    
#include "We_Lib.h"    /* WE: ANSI Standard libs allowed to use */
#include "We_Def.h"    /* WE: Global definitions */ 
#include "We_Core.h"   /* WE: System core call */ 
#include "We_Dcvt.h"   /* WE: Type conversion routines */ 
#include "We_Mem.h"    /* WE: Memory handling */
#include "We_Log.h"    /* WE: Signal logging */ 
#include "We_Wid.h"    /* WE: Widget handling */ 

/* SIS */
#include "Sis_def.h"
#include "Sis_cfg.h"
#include "Sis_if.h"

/* SMA */
#include "Sia_def.h"
#include "Sia_if.h"
#include "Sia_vrsn.h"   /* Version information */
#include "Satypes.h"
#include "Sasig.h"
#include "Saintsig.h"
#include "Sia_env.h"
#include "Sacore.h"
/*--- Definitions/Declarations ---*/

/*--- Types ---*/

/*--- Constants ---*/

/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

/*--- Global variables ---*/

/*--- Static variables ---*/
/* The following two variables are used to handle out-of-memory situations. The
 * naming of variables and functions for these are somewhat different due to 
 * WE. The variable mms_inside_run is used to indicate if the call comes 
 * via the SIAc_run function so that it is safe to longjmp back to SIAc_run in
 * case of an out-of-memory situation. It is actually a BOOL but in order to
 * be exactly the same as other modules, it has type int and will use 0 and 1
 * to indicate TRUE and FALSE. */
static jmp_buf      Sia_jmpbuf;     /* Buffer for longjmp */
static int          Sia_inside_run; /* If call originates from SIAc_run or not */


/*--- Prototypes ---*/
static void Sia_exception_handler(void);

/*****************************************************************************/

/*
 * Prepares SMA for running.
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void SMAc_start(void)
{
    Sia_inside_run = 0;
    memset( Sia_jmpbuf, 0, sizeof(Sia_jmpbuf));

    /* Initialize the memory allocator. Needed in most other functions. */
    WE_MEM_INIT(WE_MODID_SIA, Sia_exception_handler, FALSE);

    /* Register the signal queue */ 
    WE_SIGNAL_REGISTER_QUEUE(WE_MODID_SIA);

    /* Register conversion and destruction functions */
    WE_SIGNAL_REG_FUNCTIONS(WE_MODID_SIA, Sia_convert, Sia_destruct);
    
    /* Registrate the module into WE */
    WE_MODULE_IS_CREATED(WE_MODID_SIA, (char *)SIA_VERSION);
} /* SMAc_start */


/*
 * Perform an execution 'tick' for SMA.
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void SIAc_run(void)
{
    int jmpResult = 0;
	SiaResult Sia_exit_result;

    Sia_inside_run = 1;
	jmpResult = setjmp( Sia_jmpbuf);

	/* Copy the jump buffer */
	/*lint -e{545} */
	smaSetJumpBuffer( &Sia_jmpbuf );

    if ( jmpResult == 0)
    {
        if (WE_SIGNAL_QUEUE_LENGTH( WE_MODID_SIA) > 0)
        {
            /* Retrieve ONE external signal. Can't take'em all since it can be MANY */
            (void) WE_SIGNAL_RETRIEVE(WE_MODID_SIA, smaReceiveExtSignal);
        } /* if */

        smaSignalProcess();   /* Process one signal and return. */
    }
    else
    {
        if (SIA_PANIC_OUT_OF_MEMORY == smaPanicMode)
        {
           /*
            * A longjmp from alloc. If we are here there is no more
            * memory, and the situation could not be worse.
            * Terminate the SMA and let the TDA re-initialise it again.
            */
            SMAc_kill();
        } 
        else if (SIA_PANIC_NO_DIALOG == smaPanicMode)
        {
		    Sia_exit_result = smaGetSiaExitResult();
            exitSmilPlayer(Sia_exit_result, TRUE);
        } 
        else if (SIA_PANIC_TERMINATE == smaPanicMode)
        {
		    Sia_exit_result = smaGetSiaExitResult();
            exitSmilPlayer(Sia_exit_result, FALSE);
            SMAc_kill();
        }/* if */
    }
    Sia_inside_run = 0;
} /* SIAc_run */



/*
 * Check if SMA wants to run. 
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
int SMAc_wantsToRun(void)
{
    return !smaSignalQueueIsEmpty() ||
        WE_SIGNAL_QUEUE_LENGTH(WE_MODID_SIA) > 0;
} /* SMAc_wantsToRun */

/*
 * Emergency terminate function. Used by WE to kill the module.
 *
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void SMAc_kill (void)
{
    (void)WE_WIDGET_RELEASE_ALL(WE_MODID_SIA);
    
	WE_SIGNAL_DEREGISTER_QUEUE(WE_MODID_SIA);
	WE_MEM_FREE_ALL(WE_MODID_SIA);
    WE_KILL_TASK(WE_MODID_SIA);
	WE_MODULE_IS_TERMINATED(WE_MODID_SIA);
} /* SMAc_kill */

/*!
 * \brief Used to handle memory exceptions and to emergency stop the SMA Application
 *
 * Odd function name to be consistent with external requirements.
 *
 *****************************************************************************/
static void Sia_exception_handler(void)
{
    if (Sia_inside_run)
    {
        smaPanicMode = SIA_PANIC_OUT_OF_MEMORY;
        longjmp( Sia_jmpbuf, SIA_PANIC);
    }
    else
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_SIA, 
            "%s(%d): Insufficient memory!\n", __FILE__, __LINE__));

        SMAc_kill();
    } /* if */
} /* Sia_exception_handler */

