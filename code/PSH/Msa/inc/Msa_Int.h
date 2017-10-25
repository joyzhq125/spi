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

/*! \file msa_int.h
 *  Adapter functions for the MSA.
 */

#ifndef _MSA_INT_H_
#define _MSA_INT_H_

/******************************************************************************
 * Prototypes
 *****************************************************************************/

/*!
 * \brief Launches the Phone-book application.
 * 
 * This call launches the phone book application. When the phone book later
 * is closed the answer is provided through a call to MSAc_pbLaunchRsp. The 
 * possible result address type from the response is PLMN or EMAIL.
 *
 * \param wid An identity that is provided in the response function.
 *****************************************************************************/
void MSAa_pbLaunch(WE_UINT16 wid);

/*! \brief Cancels all ongoing operations with the phone book
 *
 * This adaptor function is called if the phone book needs to be closed, for 
 * example, if the MSA receives a stop signal when the phone book has been
 * launched from the MSA.
 *
 *****************************************************************************/
void MSAa_pbCancel(void);

/*!
 * \brief Lookup of names or addresses in the phone book.
 *
 * This function is used in order to find the name for a specific address.
 * The response is provided through a call to MSAc_pbLookupNameRsp().
 *
 * It should be possible to make multiple calls to this functions before
 * a response has been received.
 *
 * \param instanceId An identity that is provided in the response.
 * \param type The type of address to lookup; email or phone number.
 * \param value The address to lookup.
 *****************************************************************************/
void MSAa_pbLookupName(WE_UINT16 instanceId, WE_UINT8 type, void *value);

#endif
