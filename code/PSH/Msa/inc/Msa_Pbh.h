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

/*! \file mapbh.h
 *  Phone book handler.
 */

#ifndef _MAPBH_H_
#define _MAPBH_H_

/******************************************************************************
 * Constants
 *****************************************************************************/

/*! \enum MsaCoreSignalId
 *  Signals for the CORE FSM */
typedef enum 
{
    /*! Starts the content routing receive FSM
     * u_param1 = The result of the operation, see #MsaPhLaunchResult
     * u_param2 = The wid provided in the call to #MSAa_pbLaunch
     * p_param  = The items provided in the result, see #MsaAddrItem.
     */
    MSA_SIG_PBH_LAUNCH_RSP,

    /*! Handles the response for the #MSAa_pbLookupEntry call.
     * u_param1 = result
     * u_param2 = instanceId
     * p_param  = MsaAddrItem
     */
    MSA_SIG_PBH_LOOKUP_RSP
}MsaPbhSignalId;

/******************************************************************************
 * Data-types
 *****************************************************************************/

/* !\struct MsaPbhLookupItem
 */
typedef struct MsaPbhLookupItemSt
{
    WE_UINT16          wid;     /*!< The wid of the operation */
    MsaStateMachine     fsm;    /*!< The callback FSM */
    unsigned int        signal; /*!< The callback signal */
    struct MsaPbhLookupItemSt  *next;  /*!< The next item */
}MsaPbhLookupItem;

/******************************************************************************
 * Prototypes 
 *****************************************************************************/

void msaPbhInit(void);
void msaPbhTerminate(void);
void msaPbhLaunchPb(WE_UINT16 wid);
void msaPbhFreeList(MsaPbItem *items);
MsaAddrItem *msaPbhCopyItem(const MsaPbItem *orig);
void msaPbhLookupName(MsaStateMachine fsm, unsigned int signal, char *addr, 
    MsaAddrType addrType);
MsaPbhLookupItem *msaPbhGetLookupEntry(unsigned int wid);

#endif
