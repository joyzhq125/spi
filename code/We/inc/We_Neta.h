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
 * We_Neta.h
 *
 * Created by Ingmar Persson
 *
 * Revision  history:
 *   
 *
 */
#ifndef _we_neta_h
#define _we_neta_h

#ifndef _we_int_h
#include "We_Int.h"
#endif

/****************************************
 * Network Account
 ****************************************/

/*
 * Error codes
 */
#define WE_NETWORK_ACCOUNT_ERROR_BAD_ID            TPI_NETWORK_ACCOUNT_ERROR_BAD_ID
#define WE_NETWORK_ACCOUNT_ERROR_NO_MORE_ID        TPI_NETWORK_ACCOUNT_ERROR_NO_MORE_ID
#define WE_NETWORK_ACCOUNT_ERROR_BEARER_NOT_FOUND  TPI_NETWORK_ACCOUNT_ERROR_BEARER_NOT_FOUND

/*
 * Functions
 */
#define WE_NETWORK_ACCOUNT_GET_BEARER            TPIa_networkAccountGetBearer
#define WE_NETWORK_ACCOUNT_GET_NAME              TPIa_networkAccountGetName
#define WE_NETWORK_ACCOUNT_GET_FIRST             TPIa_networkAccountGetFirst
#define WE_NETWORK_ACCOUNT_GET_NEXT              TPIa_networkAccountGetNext
#define WE_NETWORK_ACCOUNT_GET_ID                TPIa_networkAccountGetId


#endif
