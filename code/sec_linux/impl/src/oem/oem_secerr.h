#ifndef __WE_ERR_H__
#define __WE_ERR_H__

/*=====================================================================================
    FILE NAME :
        oem_secerr.h
    MODULE NAME :
        WEA
    GENERAL DESCRIPTION
        This file declare all error code for WEA module
    TECHFAITH Wireless Confidential Proprietary(c)
        Copyright 2002 by TECHFAITH Wireless. All Rights Reserved.
=======================================================================================
    Revision History
    Modification Tracking
    Date       Author         Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2006-06-01 Alan           none      Initialization
    
=====================================================================================*/

typedef enum{
    /*just for ads compiling*/
    E_WE_ERROR_BASE = -1,
        
    /* WEA Generic Error */
    E_WE_OK = 0,
    E_WE_ERROR_BAD_HANDLE,      /* Bad Handle */
    E_WE_ERROR_INVALID_PARAM,   /* Invalid Paramter */
    E_WE_ERROR_DELAYED,         /* Operation is delayed, user need call select
                                        to register a event notify function */
    E_WE_ERROR_SOCKET_SHUTDOWN, /* Remote host socket shutdown*/                                        
    E_WE_ERROR_RESOURCE_LIMIT,  /* Resource is not enough */
    E_WE_ERROR_CLOSED,              /* Closed */
    E_WE_ERROR_OTHER,               /* Other */

    /* File Specific Error */
    E_WE_FILE_ERROR_ACCESS,     /* Access error */
    E_WE_FILE_ERROR_BADSEEKPOS,   /* Bad seek position */
    E_WE_FILE_ERROR_PATH,           /* Path is error */
    E_WE_FILE_ERROR_INVALID,        /* File is not avaiable */
    E_WE_FILE_ERROR_SIZE,           /* Size */
    E_WE_FILE_ERROR_FULL,           /* Storage is full */
    E_WE_FILE_ERROR_EOF,            /* EOF */
    E_WE_FILE_ERROR_EXIST,          /* File is exist */

    /* Pipe Specific Error */
    E_WE_PIPE_ERROR_FAIL,       /*general error */
    E_WE_PIPE_ERROR_END,        /* no more data */
    E_WE_PIPE_ERROR_FULL,

    /* Socket Specific Error */
    E_WE_SOCKET_ERROR_MSG_SIZE, /* Message Size is error */
    E_WE_SOCKET_ERROR_OPERATE_FAILED,   /* The operation is failed */
    E_WE_SOCKET_ERROR_HOST_NOT_FOUND,   /* */
    E_WE_SOCKET_ERROR_FAIL,

    /*just for ads compiling,prevent  to be converted to BYTE when compiling*/
    E_WE_ERROR_TOP = 500

}E_WE_ERROR;


#endif  /* */
