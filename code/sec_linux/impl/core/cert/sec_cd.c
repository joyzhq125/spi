/*==================================================================================================
    FILE NAME : sec_cd.c
    MODULE NAME : SEC

    GENERAL DESCRIPTION    
        This file offers several functions to get the handle of certificate, 
        copy the certificate data from buffer, and verify the content type 
        of MIME.
        
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                  Tracking
    Date          Author          Number      Description of changes
    ----------   --------------   ---------   --------------------------------------
    2006-07-07      Sam             None           Init  
    2006-09-11      Sam             None           delete macro not used

==================================================================================================*/

/*==================================================================================================
    Include File Section
==================================================================================================*/
#include "sec_comm.h"

/**************************************************************************************************
 * Macro Define Section
 **************************************************************************************************/
#define  SEC_CERT_TRANSFER_INFO     (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pstCertTransferInfo)

 /*******************************************************************************
 *   Function Define Section
*******************************************************************************/

/*==================================================================================================
FUNCTION: 
    Sec_VerifyMimeType
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION: 
    Verify the content type of MIME.
ARGUMENTS PASSED:
    hSecHandle[IO]:Global data handle.
RETURN VALUE: 
    None
USED GLOBAL VARIABLES: 
    None
USED STATIC VARIABLES:
    None
CALL BY: 
    Omit
IMPORTANT NOTES: 
    None
==================================================================================================*/
static void Sec_VerifyMimeType( WE_HANDLE hSecHandle )
{
    WE_INT32 iRes = 0;
    if (NULL == hSecHandle)
    {
        return ;
    }
    /* the content type of MIME is "application/vnd.wap.hashed-certificate" 
    or "application/vnd.wap.signed-certificate". */
    if ((!WeSCL_StrCmpNc((WE_CHAR *)SEC_CERT_TRANSFER_INFO->pcCertMimeType, 
        (WE_CHAR *)"application/vnd.wap.hashed-certificate")) 
        || (!WeSCL_StrCmpNc((WE_CHAR *)SEC_CERT_TRANSFER_INFO->pcCertMimeType, 
        (WE_CHAR *)"application/vnd.wap.signed-certificate")))
    {
        iRes = Sec_MsgTrustedCAInfoSave( hSecHandle, 
            0, 
            (const WE_CHAR *)SEC_CERT_TRANSFER_INFO->pcCertMimeType,
            (const WE_UINT8 *)SEC_CERT_TRANSFER_INFO->pucCertData, 
            SEC_CERT_TRANSFER_INFO->usCertLength );
    }
    /* the content type of MIME is "application/vnd.wap.cert-response". */
    else if (!WeSCL_StrCmpNc((WE_CHAR *)SEC_CERT_TRANSFER_INFO->pcCertMimeType, 
        (WE_CHAR *)"application/vnd.wap.cert-response"))
    {
        iRes = Sec_MsgDeliveryCert( hSecHandle, 
            0, 
            (const WE_UINT8 *)SEC_CERT_TRANSFER_INFO->pucCertData, 
            SEC_CERT_TRANSFER_INFO->usCertLength );
    }
    else
    {/* the content type of MIME did not recognise. */
        iRes = Sec_MsgCertSave( hSecHandle, 
            0, 
            (const WE_CHAR *)SEC_CERT_TRANSFER_INFO->pcCertMimeType,         
            (const WE_UINT8 *) SEC_CERT_TRANSFER_INFO->pucCertData, 
            SEC_CERT_TRANSFER_INFO->usCertLength, 
            (const WE_UINT8 *) "\x03url" );
    }    
    if (iRes != M_SEC_ERR_OK)
    {
        return;
    }
}

/*==================================================================================================
FUNCTION: 
    Sec_GetTransferCertInfo
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION: 
    Get some information of transfering certificate from buffer.
ARGUMENTS PASSED:
    hSecHandle[IO]:Global data handle.
    pvParam[IN]: pointer to the head address of buffer in memory.
RETURN VALUE: 
    None
USED GLOBAL VARIABLES: 
    None
USED STATIC VARIABLES:
    None
CALL BY: 
    Omit
IMPORTANT NOTES: 
    None
==================================================================================================*/
void Sec_GetTransferCertInfo( WE_HANDLE hSecHandle, WE_VOID *pvParam )
{ 
    St_SecCertContent   *pstCertContent = (St_SecCertContent *)pvParam;
    St_SecCertTransferInfo stCertInfo = {0};
    
    if ((NULL == pstCertContent) || (NULL == hSecHandle))
    {
        return ;
    }
    SEC_CERT_TRANSFER_INFO = &stCertInfo;
    /* MIME TYPE */
    SEC_CERT_TRANSFER_INFO->pcCertMimeType = pstCertContent->pcMime;    
    

    SEC_CERT_TRANSFER_INFO->usCertLength 
        = (WE_UINT16) pstCertContent->uiCertLen;
    SEC_CERT_TRANSFER_INFO->pucCertData = pstCertContent->pucCertData;
    Sec_VerifyMimeType( hSecHandle );

    WE_FREE(pstCertContent->pcMime);
    pstCertContent->pcMime = NULL;  
    WE_FREE(pstCertContent->pucCertData);
    pstCertContent->pucCertData = NULL;
    WE_FREE(pvParam); 
    SEC_CERT_TRANSFER_INFO = NULL;
    return;
}



