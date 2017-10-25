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

/*! \file msa_env.h
 *  Integration functions.
 */

#ifndef _MSA_ENV_H_
#define _MSA_ENV_H_

/*!\enum MsaPhLaunchResult
 */
typedef enum
{
    MSA_PB_LAUNCH_RESULT_OK,
    MSA_PB_LAUNCH_BUSY,
    MSA_PB_LAUNCH_RESULT_ERROR
}MsaPbLaunchResult;

/*!\enum MsaPhLookupResult
 *  \brief Result codes for the MSAc_pbLaunchRsp connector call.
 */
typedef enum
{
    MSA_PB_LOOKUP_RESULT_OK,
    MSA_PB_LOOKUP_RESULT_NO_HIT,
    MSA_PB_LOOKUP_RESULT_ERROR
}MsaPbLookupNameResult;

/*!\brief MsaPhAddrType
 * \brief Possible address types for the phone book
 */
typedef enum
{
    MSA_PB_TYPE_PLMN        = 0x01,   /*!< The address is a phone no */
    MSA_PB_TYPE_EMAIL       = 0x02    /*!< The address is an email address */
}MsaPbAddrType;

/******************************************************************************
 * Data-types
 *****************************************************************************/

/*! \struct MsaPhoneBookItem
 *  \brief Data-structure to provide phone book entries between the MSA and the
 *         external phone book.
 */
typedef struct MsaPbItemSt
{
    MsaPbAddrType   type;           /*!< The type of item, e.g., phone no */
    char            *name;          /*!< The name of the contact, e.g., Homer 
                                     Simpson, as a '\0' terminated UTF-8 string
                                     */
    char            *addr;          /*!< The value is dependent on the size, 
                                     e.g., if the type is; 
                                     MSA_PHONE_BOOK_TYPE_PHONE_NO, then the 
                                     value has the type: "+4670000000" 
                                     Character encoding for EMAIL and PLMN
                                     i US-ASCII with '\0' termination */
    struct MsaPbItemSt *next;       /*!< The next entry in the list or NULL */
}MsaPbItem;

/******************************************************************************
 * Prototypes
 *****************************************************************************/

void MSAc_run(void);
void MSAc_start(void);
int  MSAc_wantsToRun(void);
void MSAc_kill(void);

/*! \brief This function is called as a result to the MSAa_pbLaunch adaptor
 *         call.
 *
 * A call to this function launches the MMI for the phone book. The result is
 * provided as call to the MSA
 *
 * \param result The result of the operation, see #MsaPbLaunchResult
 * \param wid An identity provided in the call to #MSAa_pbLaunch
 * \param items The response data (see #MsaPhoneBookItem) or NULL if no data is
 *              provided
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL MSAc_pbLaunchRsp(WE_UINT16 result, WE_UINT16 wid, MsaPbItem *items);

/*! \brief The response from the MSAa_pbLookupName adaptor
 *
 * Note that only one item is returned from the phone book even if there are 
 * more than one matches for the address. In this case the first match is 
 * returned.
 *
 * \param result The result of the operation, see #MsaPbLookupNameResult
 * \param instanceId An identity provided in the call to #MSAa_pbLookupName
 * \param item The response data (see #MsaPhoneBookItem) or NULL if no data is
 *             provided
 * \return TRUE if successful, otherwise FALSE.
 *****************************************************************************/
WE_BOOL MSAc_pbLookupNameRsp(WE_UINT16 result, WE_UINT16 instanceId, 
    MsaPbItem *item);

#endif
