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
/*
 * We_Tel.h
 *
 * Created by Ingmar Persson
 *
 * Revision  history:
 *   
 *
 */
#ifndef _we_tel_h
#define _we_tel_h

#ifndef _we_int_h
#include "We_Int.h"
#endif

/**********************************************************************
 * Telephone
 **********************************************************************/

/*
 * Result types
 */
#define WE_TEL_OK                            TPI_TEL_OK
#define WE_TEL_ERROR_UNSPECIFIED             TPI_TEL_ERROR_UNSPECIFIED
#define WE_TEL_ERROR_CALLED_PARTY_IS_BUSY    TPI_TEL_ERROR_CALLED_PARTY_IS_BUSY
#define WE_TEL_ERROR_NETWORK_NOT_AVAILABLE   TPI_TEL_ERROR_NETWORK_NOT_AVAILABLE
#define WE_TEL_ERROR_CALLED_PARTY_NO_ANSWER  TPI_TEL_ERROR_CALLED_PARTY_NO_ANSWER
#define WE_TEL_ERROR_NO_ACTIVE_CONNECTION    TPI_TEL_ERROR_NO_ACTIVE_CONNECTION
#define WE_TEL_ERROR_INVALID                 TPI_TEL_ERROR_INVALID

/*
 * Functions
 */
#define WE_TEL_MAKE_CALL                     TPIa_telMakeCall
#define WE_TEL_SEND_DTMF                     TPIa_telSendDtmf


/**********************************************************************
 * Phonebook
 **********************************************************************/

/*
 * Result types
 */
#define WE_PB_OK                             TPI_PB_OK
#define WE_PB_ERROR_UNSPECIFIED              TPI_PB_ERROR_UNSPECIFIED
#define WE_PB_ERROR_IN_NAME                  TPI_PB_ERROR_IN_NAME
#define WE_PB_ERROR_NUMBER_TO_LONG           TPI_PB_ERROR_NUMBER_TO_LONG
#define WE_PB_ERROR_PHONE_BOOK_ENTRY         TPI_PB_ERROR_PHONE_BOOK_ENTRY
#define WE_PB_ERROR_PHONE_BOOK_IS_FULL       TPI_PB_ERROR_PHONE_BOOK_IS_FULL
#define WE_PB_ERROR_INVALID                  TPI_PB_ERROR_INVALID

/*
 * Functions
 */
#define WE_PB_ADD_ENTRY                      TPIa_pbAddEntry

#endif
