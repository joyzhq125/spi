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

/*! \file Sadsp.h
 *  \brief Object dispatch handling, handles timing of media objects during playback.
 */

#ifndef SADSP_H
#define SADSP_H

#ifndef SAINTSIG_H
#error Saintsig.h must be included berfore Sadsp.h!
#endif

/******************************************************************************
 * Constants
 *****************************************************************************/

/******************************************************************************
 * Data-structures
 *****************************************************************************/
typedef void (*SiaObjectActionCallback)(WE_UINT32 objId, SiaMoAction action);

typedef struct SiaDispatchListStruct
{
    WE_UINT32                      objId;
    struct SiaDispatchListStruct    *next;
}SiaDispatchList;

typedef struct SiaTimerActionStruct
{
    WE_UINT32                  time;
    SiaDispatchList             *beginMos;
    SiaDispatchList             *endMos;
    struct SiaTimerActionStruct *next;
}SiaTimerAction;

/******************************************************************************
 * Prototypes
 *****************************************************************************/
void smaDispatchInit(void);
void smaDispatchTerminate(void);

WE_BOOL smaCreateDispatcher(SiaObjectActionCallback callback, 
     WE_UINT32 slideDur, SiaStateMachine retFsm, int retSig);

void smaAddDispatchItem(WE_UINT32 objId, WE_UINT32 beginTime, 
    WE_UINT32 endTime);

void smaStartDispatcher(WE_BOOL doRun);
void smaStopDispatcher(void);
void smaDeleteDispatcher(void);

#endif /* SADSP_H */
