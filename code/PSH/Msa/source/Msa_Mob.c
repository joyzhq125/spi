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

/*! \file mamob.c
 *  \brief Media object browser
 */

/* WE */
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Mem.h"

/* MMS */
#include "Mms_Cfg.h"
#include "Mms_Def.h"

/* MSA */
#include "Msa_Rc.h"
#include "Msa_Cfg.h"
#include "Msa_Types.h"
#include "Msa_Intsig.h"
#include "Msa_Mem.h"
#include "Msa_Mob.h"
#include "Msa.h"
#include "Msa_Uimob.h"
   
/******************************************************************************
 * Data-structures
 *****************************************************************************/

/*! \struct MsaMobPageInfo
 * Data-structure for information about each rendered page of attachments
 */
typedef struct MsaMobPageInfoSt
{
    unsigned moOffset;             /*!< offset in # of objects for this page */
    struct MsaMobPageInfoSt *prev; /*!< previous page in list */
}MsaMobPageInfo;

/*! \struct MsaMobInstance
 * Data-structure for storing multipart/mixed viewer instances
 */
typedef struct 
{
    MsaStateMachine   fsm;             /*!< the fsm that wants info about MOB
                                            activitty */
    int               returnSignal;    /*!< signal to use for sending info
                                            about MOB activity */
    MsaMoDataItem     *currentMo;      /*!< current media object in list */
    unsigned          moOffset;        /*!< offset in objects for current page
                                            in list, 0 = first page */
    unsigned          noOfMoOnPage;    /*!< number of media objects on page */
    unsigned          totalPageHeight; /*!< current page height in pixels */
    MsaMoDataItem     *moList;         /*!< media object list */
    MsaMobSpecialOp   sop;             /*!< extra special operations */
    MsaMobSpecialOp   sopToAdd;        /*!< special operations to add */
    MsaMobPageInfo    *pages;          /*!< list of page information */
    WeStringHandle   winTitle;        /*!< window title */
}MsaMobInstance;

/******************************************************************************
 * Variables
 *****************************************************************************/

static MsaMobInstance *mobInstance;   /*!< The current MOB instance */

/******************************************************************************
 * Prototypes
 *****************************************************************************/

static void pushPage(MsaMobInstance *inst);
static void popPage(MsaMobInstance *inst);
static MsaMobSpecialOp findSpecialOp(MsaMobSpecialOp opbits);
static void clearSpecialOp(MsaMobSpecialOp *opbits, MsaMobSpecialOp op);
static void resetSpecialOps(MsaMobInstance *inst);
static MsaMoDataItem *findMoAtOffset(const MsaMobInstance *inst);
static void gotoFirstPage(MsaMobInstance *inst);
static void gotoNextPage(MsaMobInstance *inst);
static void gotoPrevPage(MsaMobInstance *inst);
static void msaMobCreateInstance(MsaMobInstance **instance,
    MsaMobActivation *mob);
static void msaMobDeleteInstance(MsaMobInstance **instance);
static WE_BOOL moLimitExceeded(const MsaMobInstance *inst);
static MsaMobSignalId addNextSpecialOp(MsaMobInstance *inst);
static MsaMobSignalId getNextMediaObject(MsaMobInstance *inst);
static int checkFocus(const MsaMobInstance *inst);
static void handleMobMenu(const MsaMobInstance *inst);
static void handleMobOk(MsaMobInstance *inst);
static void msaMobMain(MsaSignal *sig);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief Push information about current page on page stack
 * \param inst  the current instance
 *****************************************************************************/
static void pushPage(MsaMobInstance *inst)
{
    MsaMobPageInfo *page;

    if (NULL == inst->pages)
    {
        MSA_CALLOC(inst->pages, sizeof(MsaMobPageInfo));
    }
    else
    {
        page = inst->pages;
        MSA_CALLOC(inst->pages, sizeof(MsaMobPageInfo));
        inst->pages->prev = page;
    }
    inst->pages->moOffset = inst->moOffset;
}

/*!
 * \brief Pop information about previous page from stack
 * \param inst  the current instance
 *****************************************************************************/
static void popPage(MsaMobInstance *inst)
{
    MsaMobPageInfo *page;
    
    if (NULL == inst->pages)
    {
        return;
    }

    page = inst->pages->prev;   
    inst->moOffset = inst->pages->moOffset;
    MSA_FREE(inst->pages);
    inst->pages = page;
}

/*!
 * \brief Extract next special operation from bitmask
 * \param opbits The bitmask of special operations. 
 * \return the next special operation, or MSA_MOB_SOP_NO_OP if none found.
 *****************************************************************************/
static MsaMobSpecialOp findSpecialOp(MsaMobSpecialOp opbits)
{
    if ((opbits & MSA_MOB_SOP_ADD_MO) > 0)
    {
        return MSA_MOB_SOP_ADD_MO;
    }
    else if ((opbits & MSA_MOB_SOP_NEXT_PAGE) > 0)
    {
        return MSA_MOB_SOP_NEXT_PAGE;
    }
    else if ((opbits & MSA_MOB_SOP_PREV_PAGE) > 0) 
    {
        return MSA_MOB_SOP_PREV_PAGE;
    }

    return MSA_MOB_SOP_NO_OP;
}

/*!
 * \brief Clear a specific special operation from a special op bitmask
 * \param opbits The bitmask of special operations. 
 * \param op The operation to clear
 *****************************************************************************/
static void clearSpecialOp(MsaMobSpecialOp *opbits, MsaMobSpecialOp op)
{
     WE_UINT32 bitmask;

     bitmask = (WE_UINT32)*opbits;
     *opbits = (MsaMobSpecialOp)(~((WE_UINT32)op) & bitmask);
}

/* \brief Reset special operations to the initial special ops 
 * \param inst The current instance
 *****************************************************************************/
static void resetSpecialOps(MsaMobInstance *inst)
{    
    inst->sopToAdd = inst->sop;
    clearSpecialOp(&inst->sopToAdd, MSA_MOB_SOP_NEXT_PAGE);
    clearSpecialOp(&inst->sopToAdd, MSA_MOB_SOP_PREV_PAGE);
}

/*!
 * \brief Return the media object at the current offset
 * \param inst   the current instance
 * \return The data item at the current offset
 *****************************************************************************/
static MsaMoDataItem *findMoAtOffset(const MsaMobInstance *inst)
{
    MsaMoDataItem *item;
    unsigned i;

    item = inst->moList;
    i = inst->moOffset;
    while (item != NULL && i > 0)
    {
        i--;
        item = item->next;
    }
    return item;
}

/*!
 * \brief Switch to the first page of attachments
 * \param inst  the current instance
 *****************************************************************************/
static void gotoFirstPage(MsaMobInstance *inst)
{
    inst->noOfMoOnPage = 0;
    inst->currentMo = inst->moList;
    resetSpecialOps(inst);
}

/*!
 * \brief Switch to the next page of attachments
 * \param inst  the current instance
 *****************************************************************************/
static void gotoNextPage(MsaMobInstance *inst)
{
    /* save current page on stack */
    pushPage(inst);

    /* adjust offset so we pass all objects previously shown */
    inst->moOffset += inst->noOfMoOnPage;   
    inst->noOfMoOnPage = 0;
    inst->currentMo = findMoAtOffset(inst);
    resetSpecialOps(inst);
}

/*!
 * \brief Switch to the previous page of attachments
 * \param inst  the current instance
 *****************************************************************************/
static void gotoPrevPage(MsaMobInstance *inst)
{
    /* pop previous page off stack */
    popPage(inst);
    inst->noOfMoOnPage = 0;
    inst->currentMo = findMoAtOffset(inst);
    resetSpecialOps(inst);
}

/*!
 * \brief Creates a new instance used for the media object browser
 * \param instance    The current instance.
 * \param mob         Mob activation information
 *****************************************************************************/
static void msaMobCreateInstance(MsaMobInstance **instance,
                                 MsaMobActivation *mob)
{    
    MSA_CALLOC(*instance, sizeof(MsaMobInstance));

    /* Copy data into structure */
    (*instance)->fsm            = mob->fsm;
    (*instance)->returnSignal   = mob->signal;
    (*instance)->moList         = mob->moList;
    (*instance)->sop            = mob->sop;
    (*instance)->winTitle       = mob->winTitle;

    MSA_FREE(mob);
}

/*!
 * \brief Deletes media object browser instance 
 *
 * \param instance The current instance.
 *****************************************************************************/
static void msaMobDeleteInstance(MsaMobInstance **instance)
{
    MsaMobInstance *inst;
    
    if (*instance != NULL)
    {
        inst = *instance;

        MSA_FREE(inst);
        *instance = NULL;
    }
}

/*!
 * \brief Checks if adding a new media object would exceed page limits
 * \param instance The current instance.
 * \return TRUE if limit would be exceeded 
 *****************************************************************************/
static WE_BOOL moLimitExceeded(const MsaMobInstance *inst)
{
    return (MSA_MAX_NUMBER_OF_MO_ON_PAGE <= inst->noOfMoOnPage ||
            MSA_MAX_PAGE_HEIGHT <= inst->totalPageHeight);
}

/*!
 * \brief Add next special operation to widget
 * \param inst The current instance.
 * \return Signal telling what to do next
 *****************************************************************************/
static MsaMobSignalId addNextSpecialOp(MsaMobInstance *inst)
{
    MsaMobSpecialOp op = findSpecialOp(inst->sopToAdd);
    clearSpecialOp(&inst->sopToAdd, op);

    if (!msaUiMobAddSpecialOp(op))
    {
        return MSA_SIG_MOB_DEACTIVATE;
    }

    if (findSpecialOp(inst->sopToAdd))
    {
        return MSA_SIG_MOB_ADD_NEXT_SOP;        
    }

    return MSA_SIG_MOB_FINALIZE_WINDOW;
}

/*!
 * \brief Retrieve next media object from message
 * \param inst  the current instance
 * \return Signal telling what to do next
 *****************************************************************************/
static MsaMobSignalId getNextMediaObject(MsaMobInstance *inst)
{       
    if (!msaUiMobAddWidget(inst->currentMo))
    { 
        return MSA_SIG_MOB_DEACTIVATE;
    }

    inst->currentMo = inst->currentMo->next;
    inst->noOfMoOnPage++;
    /* inst->totalPageHeight = ? ; */

    if (NULL == inst->currentMo || moLimitExceeded(inst))
    {
        /* Done with media objects, add special operations, or create window 
         if there are none */

        if (inst->moOffset > 0)                
        {
            inst->sopToAdd |= MSA_MOB_SOP_PREV_PAGE;
        }
        if (inst->currentMo && moLimitExceeded(inst))
        {
            inst->sopToAdd |= MSA_MOB_SOP_NEXT_PAGE;
        }
        if (inst->sopToAdd)
        {
            return MSA_SIG_MOB_ADD_NEXT_SOP;
        }
        else
        {
            return MSA_SIG_MOB_FINALIZE_WINDOW;
        }
    }

    /* More media objects, keep on adding */    
    return MSA_SIG_MOB_ADD_NEXT_MO;
}


/*!
 * \brief Find out which attachment was selected when pressing Menu or Yes
 * \param inst  the current instance
 * \return The attachment no., or -1 when no focused object can be found
 *****************************************************************************/
static int checkFocus(const MsaMobInstance *inst)
{
    int i = msaUiMobCheckFocus();

    if (i > -1)
    {
        i += (int)inst->moOffset;
    }
    return i;
}

/*!
 * \brief Handle a "MENU" button press
 * \param inst  the current instance
 *****************************************************************************/
static void handleMobMenu(const MsaMobInstance *inst)
{
    int i;

    i = checkFocus(inst);
    /* Pass the index of the object or -1 to the originating FSM */
    if (msaUiMobGetSpecialOp() != MSA_MOB_SOP_NO_OP)
    {
        i = -1;
    }
    (void)MSA_SIGNAL_SENDTO_IU(inst->fsm, inst->returnSignal, i, 
        (unsigned long)MSA_MOB_MENU);
}

/*!
 * \brief Handle an "OK" button press
 * \param inst  the current instance
 *****************************************************************************/
static void handleMobOk(MsaMobInstance *inst)
{
    int i;
    MsaMobSpecialOp sop;

    /* find out if user selected some special op or just an attachment */
    sop = msaUiMobGetSpecialOp();    
    switch (sop)
    {            
    case MSA_MOB_SOP_NEXT_PAGE: /* switch to next page */
    case MSA_MOB_SOP_PREV_PAGE: /* switch to previous page */
        if (MSA_MOB_SOP_NEXT_PAGE == sop)
        {
            gotoNextPage(inst);
        }
        else
        {
            gotoPrevPage(inst);
        }
        msaUiMobDeleteAll();
        /* First, initiate window */
        if (!msaUiInitializeForm())
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "%s(%d): Could not initialize form\n", 
                __FILE__, __LINE__));       
            (void)MSA_SIGNAL_SENDTO(MSA_MOB_FSM, MSA_SIG_MOB_DEACTIVATE);
            break;
        }
        (void)MSA_SIGNAL_SENDTO(MSA_MOB_FSM,
            getNextMediaObject(inst));
        break;

    case MSA_MOB_SOP_NO_OP:     /* send selected item to fsm */
        i = checkFocus(inst);  
        if (i >= 0)
        {
            (void)MSA_SIGNAL_SENDTO_IUP(inst->fsm, inst->returnSignal, i,
                (unsigned long)MSA_MOB_SELECT, (void *)msaUiMobGetLink(i));
        }
        break;

    case MSA_MOB_SOP_ADD_MO:    /* user defined special ops, send to fsm*/
        (void)MSA_SIGNAL_SENDTO_IU(inst->fsm, inst->returnSignal, sop,
            (unsigned long)MSA_MOB_SPECIAL_OP);
        break;

    default:                    
        /* some other, unhandled operation.. */
        break;
    }
}

/*!
 * \brief Signal handler for the multipart mixed message viewer 
 * \param sig The received signal 
 *****************************************************************************/
static void msaMobMain(MsaSignal *sig)
{
    /* reality check - MOB must be active before handling non-activation
       signals */
    if (NULL == mobInstance &&
        (MsaMobSignalId) sig->type != MSA_SIG_MOB_ACTIVATE)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "%s(%d): Received signal %d without being active (ignored)!\n", 
            __FILE__, __LINE__, sig->type));       
        return;
    }

    switch ((MsaMobSignalId) sig->type)
    {

    /* --- Activation/deactivation */

    case MSA_SIG_MOB_ACTIVATE:      /* Startup the mob */                
        msaMobCreateInstance(&mobInstance, (MsaMobActivation*)sig->p_param);       
        msaUiMobInit();
        gotoFirstPage(mobInstance);

        /* First, initiate window */
        if (!msaUiInitializeForm())
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "%s(%d): Could not initialize form\n", 
                __FILE__, __LINE__));       
            (void)MSA_SIGNAL_SENDTO(MSA_MOB_FSM, MSA_SIG_MOB_DEACTIVATE);
            break;
        }       

        /* The, if there are objects, start adding them */
        if (mobInstance->currentMo)
        {
            /* start adding media objects to list */       
            (void)MSA_SIGNAL_SENDTO(MSA_MOB_FSM, MSA_SIG_MOB_ADD_NEXT_MO);        
        }
        /* ... otherwise just add special operations, if any */
        else if (findSpecialOp(mobInstance->sopToAdd))
        {
            (void)MSA_SIGNAL_SENDTO(MSA_MOB_FSM, MSA_SIG_MOB_ADD_NEXT_SOP);            
        }
        /* ...or else, with no objects or specials, just show blank window */
        else
        {
            (void)MSA_SIGNAL_SENDTO(MSA_MOB_FSM, MSA_SIG_MOB_FINALIZE_WINDOW);
        }
        break;

    case MSA_SIG_MOB_DEACTIVATE:    /* Kill the mob */   
        /* Inform actor that MOB is done */
        (void)MSA_SIGNAL_SENDTO_UU(mobInstance->fsm, 
            mobInstance->returnSignal,(unsigned long)MSA_MOB_DEACTIVATED,
            0);        
        /* Kill widgets */
        msaUiMobTerminate();
        /* Clean-up the mob instance */
        msaMobDeleteInstance(&mobInstance);
        break;

    /* --- Internal FSM states */

    case MSA_SIG_MOB_ADD_NEXT_MO:   /* Create widget for next media object */ 
        (void)MSA_SIGNAL_SENDTO(MSA_MOB_FSM, getNextMediaObject(mobInstance));
        break;

    case MSA_SIG_MOB_ADD_NEXT_SOP:  /* Create widget for next special op */
        (void)MSA_SIGNAL_SENDTO(MSA_MOB_FSM, addNextSpecialOp(mobInstance));
        break;

    case MSA_SIG_MOB_FINALIZE_WINDOW: /* Create window with widgets */
        if (!msaUiMobFinalizeForm(mobInstance->winTitle))
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "%s(%d): Could not finalize form\n", 
                __FILE__, __LINE__));       
            (void)MSA_SIGNAL_SENDTO(MSA_MOB_FSM, MSA_SIG_MOB_DEACTIVATE);
        }        
        break;

    /* --- UI related signals */

    case MSA_SIG_MOB_KEY_LEFT:      /* Left key pressed */
        if (!(mobInstance->sopToAdd & MSA_MOB_SOP_PREV_PAGE))
        {
            break;
        } /* if */
        gotoPrevPage(mobInstance);
        msaUiMobDeleteAll();
        if (!msaUiInitializeForm())
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "%s(%d): Could not initialize form\n", 
                __FILE__, __LINE__));       
            (void)MSA_SIGNAL_SENDTO(MSA_MOB_FSM, MSA_SIG_MOB_DEACTIVATE);
            break;
        }
        (void)MSA_SIGNAL_SENDTO(MSA_MOB_FSM, getNextMediaObject(mobInstance));
        break;

    case MSA_SIG_MOB_KEY_RIGHT:    /* Right key pressed */
        if (!(mobInstance->sopToAdd & MSA_MOB_SOP_NEXT_PAGE))
        {
            break;
        } /* if */
        gotoNextPage(mobInstance);
        msaUiMobDeleteAll();
        /* First, initiate window */
        if (!msaUiInitializeForm())
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "%s(%d): Could not initialize form\n", 
                __FILE__, __LINE__));       
            (void)MSA_SIGNAL_SENDTO(MSA_MOB_FSM, MSA_SIG_MOB_DEACTIVATE);
            break;
        }
        (void)MSA_SIGNAL_SENDTO(MSA_MOB_FSM, getNextMediaObject(mobInstance));
        break;

    case MSA_SIG_MOB_MENU_SELECTED: /* User pressed Menu button */
        handleMobMenu(mobInstance);
        break;

    case MSA_SIG_MOB_OK_SELECTED: /* User pressed Ok button */
        handleMobOk(mobInstance);
        break;        
       
    case MSA_SIG_MOB_FOCUS_ACTIVITY: /* audio widget lost focus */
        (void)MSA_SIGNAL_SENDTO_U(mobInstance->fsm, 
            mobInstance->returnSignal, MSA_MOB_FOCUS_ACTIVITY);            
        break;

    /* --- Unknown signals */       
    default: 
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "%s(%d): msaMobMain erroneous signal received(%d)\n", 
            __FILE__, __LINE__, sig->type));       
        break;
    }
    msaSignalDelete(sig);
}

/*!
 * \brief Activate the Mob
 * \param moList   media object list
 * \param sop      bit mask of special operations to add. 
 * \param winTitle title of Mob window. 
 * \param fsm      fsm to send signal to
 * \param signal   signal to send to fsm
 *****************************************************************************/
void msaMobActivate(MsaMoDataItem *moList, MsaMobSpecialOp sop, 
                    WE_UINT32 winTitle, MsaStateMachine fsm, int signal)
{
    MsaMobActivation *mob;

    mob = MSA_ALLOC(sizeof(MsaMobActivation));
    mob->moList = moList;    
    mob->sop = sop;
    mob->winTitle = winTitle;
    mob->fsm = fsm;
    mob->signal = signal;

    (void)MSA_SIGNAL_SENDTO_P(MSA_MOB_FSM, MSA_SIG_MOB_ACTIVATE, mob);     
}

/*!
 * \brief Registers signal handler
 *****************************************************************************/
void msaMobInit(void)
{
    mobInstance = NULL;
    msaSignalRegisterDst(MSA_MOB_FSM, msaMobMain); /* set sig-handler */
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
        "MSA MOB: initialized\n"));
}

/*!
 * \brief Terminates, by de-registering signal handler
 *****************************************************************************/
void msaMobTerminate(void)
{
    /* Kill widgets */
    msaUiMobTerminate();
    /* Clean-up the mob instance */
    msaMobDeleteInstance(&mobInstance);

    /* Deregister signal handler */
    msaSignalDeregister(MSA_MOB_FSM);
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, "MSA MOB: terminated\n"));
}





















