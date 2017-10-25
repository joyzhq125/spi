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
 
/*! \file mms_a.c
 * \brief  In this file is the adapter functions in protyped in Mms_Int.h 
 *         implemented as reference code (example code). This functions
 *         needs to integrated in the target device envrionment. 
 */

/*--- Include files ---*/
#include "We_Log.h"    /* WE: Signal logging */ 

#include "Mms_Def.h"    /* MMS: Exported types in adapter/connector functions */
#include "Mms_Int.h"    /* MMS: MMS Exported Adapter functions */ 
#include "We_Log.h"    /* WE: Signal logging */ 

/*--- Definitions/Declarations ---*/

/*--- Types ---*/

/*--- Constants ---*/

/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

/*--- Global variables ---*/

/*--- Static variables ---*/

/*--- Prototypes ---*/

/*****************************************************************************/

/*
 * Informs that a message, notification or read report has been recived
 * 
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void MMSa_newMessage(MmsFileType msgType, MmsMsgId msgId, 
    MmsAddress *from, MmsEncodedText *subject, WE_UINT32 size,
    WE_UINT32 numOfMsg,   WE_UINT32 numOfUnreadMsg,
    WE_UINT32 numOfNotif, WE_UINT32 numOfUnreadNotif,
    WE_UINT32 numOfRR,    WE_UINT32 numOfUnreadRR,
    WE_UINT32 numOfDR,    WE_UINT32 numOfUnreadDR,
    MmsNotificationReason reason
    )
{

    #ifdef WE_LOG_FC
        WE_LOG_FC_BEGIN(MMSa_newMessage)
        WE_LOG_FC_UINT32(numOfMsg, NULL); 
        WE_LOG_FC_UINT32(numOfUnreadMsg, NULL); 
        WE_LOG_FC_UINT32(numOfNotif, NULL); 
        WE_LOG_FC_UINT32(numOfUnreadNotif, NULL); 
        WE_LOG_FC_UINT32(numOfRR, NULL); 
        WE_LOG_FC_UINT32(numOfUnreadRR, NULL); 
        WE_LOG_FC_UINT32(numOfDR, NULL); 
        WE_LOG_FC_UINT32(numOfUnreadDR, NULL); 
        WE_LOG_FC_PRE_IMPL
    #endif
        
    WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_MMS, 
        "MMSa_newMessage: Received, type=%d, msgId=%lu, from=%s, subject=%s, size=%lu\n",
        (int)msgType, (unsigned long)msgId, 
        (from != NULL && from->address != NULL) ? from->address : "NULL", 
        (subject != NULL && subject->text != NULL) ? subject->text : "NULL", 
        (unsigned long)size));
    WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_MMS, 
        "MMSa_newMessage: Received, numOfMsg=%d, numOfUnread=%d numOfNotif=%d numOfUnreadNotif=%d reason = %d\n",
        (int)numOfMsg, (int)numOfUnreadMsg, (int)numOfNotif, (int)numOfUnreadNotif, (int)reason));
    WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_MMS, 
        "MMSa_newMessage: Received(more), numOfRR=%d, numOfUnreadRR=%d numOfDR=%d numOfUnreadDR=%d\n",
        (int)numOfRR, (int)numOfUnreadRR, (int)numOfDR, (int)numOfUnreadDR));
    
    /*
     * Update this switch with calling ring tone etc. depending on type 
     * of received message. 
     */
    switch (msgType) 
    { 
        case MMS_SUFFIX_MSG :           /* Message received, immediate */
        case MMS_SUFFIX_IMMEDIATE :     /* Notification received, immediate */  
        case MMS_SUFFIX_NOTIFICATION :  /* Notification received, delayed */  
        case MMS_SUFFIX_READREPORT :    /* Read Report received */ 
        case MMS_SUFFIX_DELIVERYREPORT :/* Delivery Report received */     
        case MMS_SUFFIX_TEMPLATE:
        case MMS_SUFFIX_INFO:
        case MMS_SUFFIX_SEND_REQ:
        case MMS_SUFFIX_ERROR:
        default: 
            break; 
    } /* switch */ 

    /*
     * Update this function to handle change of unread messages status. 
     */
    #ifdef WE_LOG_FC
        WE_LOG_FC_END
    #endif
        
} /* MMSa_newMessage */ 


/*
 * Informs that a message, notification or read report has been recived
 * 
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void MMSa_messageStatus(
                        WE_UINT32 numOfMsg,   WE_UINT32 numOfUnreadMsg,
                        WE_UINT32 numOfNotif, WE_UINT32 numOfUnreadNotif,
                        WE_UINT32 numOfRR,    WE_UINT32 numOfUnreadRR,
                        WE_UINT32 numOfDR,    WE_UINT32 numOfUnreadDR)
{
    #ifdef WE_LOG_FC
        WE_LOG_FC_BEGIN(MMSa_messageStatus)
        WE_LOG_FC_UINT32(numOfMsg, NULL); 
        WE_LOG_FC_UINT32(numOfUnreadMsg, NULL); 
        WE_LOG_FC_UINT32(numOfNotif, NULL); 
        WE_LOG_FC_UINT32(numOfUnreadNotif, NULL); 
        WE_LOG_FC_UINT32(numOfRR, NULL); 
        WE_LOG_FC_UINT32(numOfUnreadRR, NULL); 
        WE_LOG_FC_UINT32(numOfDR, NULL); 
        WE_LOG_FC_UINT32(numOfUnreadDR, NULL); 
        WE_LOG_FC_PRE_IMPL
    #endif

    WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_MMS, 
        "MMSa_messageStatus: Received, numOfMsg=%d, numOfUnreadMsg=%d", (int)numOfMsg, (int)numOfUnreadMsg));
    WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_MMS, 
        "MMSa_messageStatus: Continued, numOfNotif=%d, numOfUnreadNotif=%d", (int)numOfNotif, (int)numOfUnreadNotif));
    WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_MMS, 
        "MMSa_messageStatus: Continued, numOfRR=%d, numOfUnreadRR=%d", (int)numOfRR, (int)numOfUnreadRR));
    WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_MMS, 
        "MMSa_messageStatus: Continued, numOfDR=%d, numOfUnreadDR=%d", (int)numOfDR, (int)numOfUnreadDR));
    
    /*
     * Update this function to handle change of unread messages status. 
     */
        
    #ifdef WE_LOG_FC
        WE_LOG_FC_END
    #endif
        
} /* MMSa_messageStatus */ 

/*
 * Informs that a message, notification or read report has been recived
 * 
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void MMSa_progressStatus(MmsProgressStatus *progressData)
{
    /*
     * Here the integration should implement any reporting
     * of progress status of MMS download/upload.
     *
     * The structure progressData supplies information about
     * the operation in progress.
     *
     * The status variable informs about lifespan and can
     * be used to allocate/free data for progress tracking.
     */
    WE_LOG_FC_BEGIN(MMSa_progressStatus)

    WE_LOG_FC_UINT32(progressData->msgId, NULL); 
    WE_LOG_FC_UINT8(progressData->operation, NULL); 
    WE_LOG_FC_UINT32(progressData->progress, NULL); 
    WE_LOG_FC_UINT8(progressData->queueLength, NULL); 
    WE_LOG_FC_UINT8(progressData->state, NULL); 
    WE_LOG_FC_UINT32(progressData->totalLength, NULL); 
    WE_LOG_FC_PRE_IMPL
        
    /*
     * Update this function to handle change of unread messages status. 
     */
    WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_MMS, 
     "MMSa_progressStatus: msgId=%d, operation=%d", progressData->msgId, progressData->operation));
    
    WE_LOG_FC_END
}


/*
 * An error has occured when no application was connected to the MMS Service. 
 * 
 * Please note: Doxygen dokumentation, see file description in top of file.
 *****************************************************************************/
void MMSa_error(MmsResult errorCode)
{
    /* 
     * Update this code to the report error
     */ 
    WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_MMS, 
        "MMS: An Error occured when no application was connectd (%d)\n",
        errorCode));
} /* MMSa_error */


#ifdef WE_LOG_FC  
/*
WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(MMSa_messageStatus)
MMSa_messageStatus( 1, 1);
WE_LOG_FC_DISPATCH_MAP_ENTRY_END
*/
#endif /* WE_LOG_FC */





















