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

#ifndef SIA_IF_H
#define SIA_IF_H

#include "Sia_def.h"

/**********************************************************************
 * External Signals
 **********************************************************************/

/*! Return Signal sent from the SMA Module */
#define SIA_SIG_PLAY_SMIL_RSP   ((WE_MODID_SIA << 8) + 0x02)


/**********************************************************************
 * Signal parameter definitions:
 **********************************************************************/


/*! \struct SiaPlaySmilReq Used for the signal SIA_SIG_PLAY_SMIL_REQ */
typedef struct
{
    WE_UINT32 msgId;
    WE_UINT32 userData;
} SiaPlaySmilReq;

/*! \struct SiaPlaySmilRsp Used for the signal SIA_SIG_PLAY_SMIL_RSP */
typedef struct
{
    SiaResult result;
    WE_UINT32 userData;
} SiaPlaySmilRsp;



/*! Starts SMA if not previously started. */
void SIAif_startInterface (void);

/**********************************************************************
 * Signal-sending API
 * These functions should be used by any other module that wants to
 * send a signal to the SMA module.
 **********************************************************************/

/*!
 * \brief Signal sent from client to SMA-Module, plays a SMIL in a MMS message
 * 
 * \param sender Sender module
 * \param msgId ID of a MMS message containing a SMIL
 * \param fileType The type of the message to handle: 
 *                 MMS_SUFFIX_SEND_REQ or MMS_SUFFIX_MSG.
 * \return Result of the operation.
 *****************************************************************************/
SiaResult SIAif_playSmil(WE_UINT8 sender, WE_UINT32 msgId,
    WE_UINT32 fileType);

/**********************************************************************
 * Conversion routines for all signal owned by this module.
 * None of these functions should be called directly by another module.
 **********************************************************************/

/*! Convert from memory buffer to signal structs */
void *Sia_convert (WE_UINT8 module, WE_UINT16 signal, void* buffer);

/*! Released a signal structure to a signal */
void Sia_destruct (WE_UINT8 module, WE_UINT16 signal, void* p);

/*!
 * \brief  Converts a parameters into a signal buffer for SiaPlaySmilReq 
 * 
 * \param obj  The WE obejct. 
 * \param data The data structure to convert to
 *****************************************************************************/
int Sia_cvt_SiaPlaySmilReq( we_dcvt_t *obj, SiaPlaySmilReq *data);

/*!
 * \brief  Converts a parameters into a signal buffer for SiaPlaySmilReq 
 * 
 * \param obj  The WE obejct. 
 * \param data The data structure to convert to
 *****************************************************************************/
int Sia_cvt_SiaPlaySmilRsp( we_dcvt_t *obj, SiaPlaySmilRsp *data);


#endif
