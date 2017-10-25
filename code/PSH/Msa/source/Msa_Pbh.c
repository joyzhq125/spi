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

/*!\file mapbh.c
 * \brief Phone book handler.
 */

/* WE */
#include "We_Cfg.h"
#include "We_Mem.h"
#include "We_Def.h"
#include "We_Cmmn.h"
#include "We_Lib.h"
#include "We_Core.h"
#include "We_Log.h"

/* MMS */
#include "Mms_Def.h"

/* MSA */
#include "Msa_Rc.h"
#include "Msa_Env.h"
#include "Msa_Cfg.h"
#include "Msa_Int.h"
#include "Msa_Types.h"
#include "Msa_Mem.h"
#include "Msa_Intsig.h"
#include "Msa_Mr.h"
#include "Msa_Me.h"
#include "Msa_Pbh.h"

/*! \file mapbh.c
 *  \brief Phone book handling
 */

/******************************************************************************
 * Constants
 *****************************************************************************/

/******************************************************************************
 * Data-structures
 *****************************************************************************/

/******************************************************************************
 * Variables
 *****************************************************************************/
static WE_BOOL isPbStarted;
static MsaPbhLookupItem *lookupItems;
static WE_UINT16 nextFreeId;

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static void pbhSignalHandler(MsaSignal *sig);
static void freeLookupList(MsaPbhLookupItem **items);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief Initiates the PBH FSM.
 *
 *****************************************************************************/
void msaPbhInit(void)
{
    isPbStarted = FALSE;
    lookupItems = NULL;
    nextFreeId  = 0;
    msaSignalRegisterDst(MSA_PBH_FSM, pbhSignalHandler);
}

/*!
 * \brief Terminates the CRR FSM.
 *
 *****************************************************************************/
void msaPbhTerminate(void)
{
    if ((isPbStarted) || (NULL != lookupItems))
    {
        /* Cancel ongoing phone-book operations */
        MSAa_pbCancel();
    }
    freeLookupList(&lookupItems);
    isPbStarted = FALSE;
    lookupItems = NULL;
    nextFreeId  = 0;
    msaSignalDeregister(MSA_PBH_FSM);
}

/*! \brief Launches the phone book.
 *
 * \param wid An identity that is provided in the response.
 *****************************************************************************/
void msaPbhLaunchPb(WE_UINT16 wid)
{
    isPbStarted = TRUE;
    MSAa_pbLaunch(wid);
}

/*! \brief De-allocates a list of "lookup" entries.
 *
 *****************************************************************************/
static void freeLookupList(MsaPbhLookupItem **items)
{
    MsaPbhLookupItem *current;

    while(NULL != *items)
    {
        current = *items;
        *items = (*items)->next;
        MSA_FREE(current);
    }
    *items = NULL;
}

/*! \brief Gets a phone book lookup entry. The returned entry is removed from 
 *         the list.
 *
 *****************************************************************************/
MsaPbhLookupItem *msaPbhGetLookupEntry(unsigned int wid)
{
    MsaPbhLookupItem *prev;
    MsaPbhLookupItem *current;
    MsaPbhLookupItem *items = lookupItems;
    /* Id it the first item? */
    if (NULL == items)
    {
        return NULL;
    }
    /* Handle the first item */
    if (items->wid == wid)
    {
        current = items;
        lookupItems = items->next;
        return current;
    }
    /* Other items */
    prev = NULL;
    current = items;
    while(NULL != current)
    {
        if (current->wid == wid)
        {
            /*lint -e{613}*/
            prev->next = current->next;
            return current;
        }
        prev = current;
        current = current->next;
    }
    return NULL;
}

/*!
 * \brief The signal handler for the PBH FSM.
 *
 * \param sig The current signal.
 *****************************************************************************/
static void pbhSignalHandler(MsaSignal *sig)
{
    MsaPbhLookupItem *lItem;
    /* Check which signal that was received */
    switch(sig->type)
    {
    case MSA_SIG_PBH_LAUNCH_RSP:
        if (MSA_PB_LAUNCH_RESULT_OK == sig->u_param1)
        {
            (void)MSA_SIGNAL_SENDTO_UP(MSA_ME_FSM, MSA_SIG_ME_ADD_RECEPIENTS, 
                sig->u_param2, sig->p_param);
        }
        else
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) The result of the phone book launch operation was "
                "not successful!\n", __FILE__, __LINE__));
        }
        break;

    case MSA_SIG_PBH_LOOKUP_RSP:
        if (MSA_PB_LOOKUP_RESULT_OK == sig->u_param1)
        {
            if (NULL != (lItem = msaPbhGetLookupEntry(sig->u_param2)))
            {
                (void)MSA_SIGNAL_SENDTO_P(lItem->fsm, (int)lItem->signal, 
                    sig->p_param);
                /*lint -e{774} */
                MSA_FREE(lItem);
            }
        }
        else
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) The result of the phone book launch operation was "
                "not successful!\n", __FILE__, __LINE__));
        }        
        break;

    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) Unhandled signal in crrSignalHandler!\n", __FILE__, 
            __LINE__));
        break;
    }
    msaSignalDelete(sig);
}

/*! \brief De-allocates a list of phone book items
 *
 * \param items the list to de-allocate
 *****************************************************************************/
void msaPbhFreeList(MsaPbItem *items)
{
    MsaPbItem *current;
    while(NULL != items)
    {
        current = items;
        /* De-allocate data */
        MSA_FREE(current->name);
        MSA_FREE(current->addr);
        MSA_FREE(current);
        /* Next item */
        items = items->next;
    }
}

/*! \brief Copies a #MsaPbItem to a new #MsaAddrItem.
 *
 * \param orig The item to copy.
 * \return The new item if successful, otherwise NULL.
 *****************************************************************************/
MsaAddrItem *msaPbhCopyItem(const MsaPbItem *orig)
{
    MsaAddrItem *newItem;
    if ((NULL == orig) || (NULL == orig->addr))
    {
        return NULL;
    }
    /* Create new item */
    if (NULL == (newItem = (MSA_ALLOC_NE(sizeof(MsaPbItem)))))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) Failed to allocate pb item.\n", __FILE__, 
            __LINE__));
        return FALSE;
    }
    memset(newItem, 0, sizeof(MsaPbItem));

    /* Copy the data */
    if (NULL != orig->name)
    {
        /* Name */
        if (NULL == (newItem->name = MSA_ALLOC_NE(strlen(orig->name) + 1)))
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) Failed to allocate pb item.\n", __FILE__, 
                __LINE__));
            return NULL;
        }
        strcpy(newItem->name, orig->name);
    }
    if (NULL != orig->addr)
    {
        /* Value */
        if (NULL == (newItem->address = MSA_ALLOC_NE(strlen(orig->addr) + 1)))
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) Failed to allocate PB item.\n", __FILE__, 
                __LINE__));

            return NULL;
        }
        strcpy(newItem->address, orig->addr);
    }
    /* Type */
    switch(orig->type)
    {
    case MSA_PB_TYPE_EMAIL:
        newItem->addrType = MSA_ADDR_TYPE_EMAIL;
        break;
    case MSA_PB_TYPE_PLMN:
        newItem->addrType = MSA_ADDR_TYPE_PLMN;
        break;
    }
    return newItem;
}

/*!\brief Find the name of a contact.
 *
 * \param fsm The callback FSM.
 * \param signal The callback signal.
 * \param addr The address to lookup the name for
 * \param addrType The address type, see #MsaAddrType.
 *****************************************************************************/
void msaPbhLookupName(MsaStateMachine fsm, unsigned int signal, char *addr, 
    MsaAddrType addrType)
{
    MsaPbhLookupItem *item;
    MsaPbAddrType    pbType;

    if (NULL == (item = MSA_ALLOC_NE(sizeof(MsaPbhLookupItem))))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) Failed to perform a lookup operation, out of memory.\n",
            __FILE__, __LINE__));
        return;
    }
    memset(item, 0, sizeof(MsaPbhLookupItem));
    item->fsm       = fsm;
    item->signal    = signal;
    item->wid        = nextFreeId;
    if (NULL != lookupItems)
    {
        item->next = lookupItems;
    }
    lookupItems = item;
    pbType = (MsaPbAddrType)0;
    switch (addrType)
    {
    case MSA_ADDR_TYPE_EMAIL:
        pbType = MSA_PB_TYPE_EMAIL;
        break;
    case MSA_ADDR_TYPE_PLMN:
        pbType = MSA_PB_TYPE_PLMN;
        break;
    case MSA_ADDR_TYPE_NONE:
        return ;
    }
    MSAa_pbLookupName(nextFreeId, (WE_UINT8)pbType, addr);
    nextFreeId++;
}
