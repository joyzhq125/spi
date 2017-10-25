/*==================================================================================================
    HEADER NAME : oem_seclog.c
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
            
    GENERAL DESCRIPTION
        In this file,define the log function prototype.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-07-07   Kevin Yang        None         Draft
    
==================================================================================================*/

/*==================================================================================================
*   Include File 
*=================================================================================================*/

#include "we_def.h"

#include "oem_seclog.h"

#ifdef WE_LOG_MODULE

#include "aeestdlib.h"


/***************************************************************************************************
*   Function Define Section
***************************************************************************************************/

/*==================================================================================================
FUNCTION: 
    Sec_LogMsg
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    print log message.
ARGUMENTS PASSED:
    iType[IN]: no use 
    ucId[IN]: no use
    pcFormat[IN]:the message to be printed.
RETURN VALUE:
    None.
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_LogMsg(WE_INT32 iType, WE_UINT8 ucId, const WE_CHAR * pcFormat,...)
{
    va_list arglist;
    WE_CHAR buffer[512];
    iType = iType;
    ucId = ucId;
    va_start(arglist,pcFormat);
    VSNPRINTF(buffer,sizeof(buffer),pcFormat,arglist);
    DBGPRINTF("%s",buffer);
}
/*==================================================================================================
FUNCTION: 
    Sec_LogData
CREATE DATE:
    2006-07-07
AUTHOR:
    Kevin Yang
DESCRIPTION:
    print  log message
ARGUMENTS PASSED:
    iType[IN]: no use.
    ucId[IN]: no use.
    pucData[IN]: pointer to the data message.
    iDataLen[IN]: data length.
RETURN VALUE:
    Counter number
USED GLOBAL VARIABLES:
    None
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None
==================================================================================================*/
void Sec_LogData (WE_INT32 iType, WE_UINT8 ucId, const WE_UCHAR *pucData, WE_INT32 iDataLen)
{
    WE_INT32 i = 0;
    iType = iType;
    ucId = ucId;
    /* add by sam [070308] */
    if (NULL == pucData)
    {
        DBGPRINTF("POINT TO NULL! Buffer length : %d\n", iDataLen);
        return;
    }
    for(i=0;i<iDataLen;i++)
    {
        DBGPRINTF("%x", (*pucData));
        pucData++;
    }
}

#endif
