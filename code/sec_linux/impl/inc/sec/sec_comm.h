/*==================================================================================================
    HEADER NAME : sec_comm.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
    
    
    GENERAL DESCRIPTION
        In this file,define the initial function prototype ,and will be update later.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
      2006-07-07   Jabber Wu         None         Draft
      2006-08-20   Kevin Yang        None         update
    
==================================================================================================*/

/***************************************************************************************************
*   Multi-Include-Prevent Section
***************************************************************************************************/
#ifndef _SEC_COMM_H
#define _SEC_COMM_H


/***************************************************************************************************
*   include File Section
*******************************************************************************/
/* System head file */
/*
#include "AEESecurity.h"
#include "AEESource.h"
#include "AEERSA.h"
#include "AEEWebOpts.h"
#include "AEEX509Chain.h"
*/
/*support linux*/
#include <sys/un.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#include "we_def.h"
//#include "wap_ast.h"
#include "oem_secerr.h"
#include "oem_secevent.h"
#include "we_mem.h"
#include "oem_seclog.h"
#include "oem_secfile.h"
#include "we_scl.h"   
#include "sec_asn1.h" 
#include "we_libalg.h"


/* Global head file */
#include "oem_seccb.h"
#include "sec.h"
#include "isecb.h"
#include "isecw.h"
#include "isigntext.h"

#include "sec_ast.h"
#include "sec_cfg.h"
#include "sec_msg.h"
#include "sec_iwap.h"
#include "sec_ibrs.h"
#include "sec_lib.h"
#include "sec_main.h"
/*
#include "oem_secpkc.h"*/
#include "we_rsacomm.h"//add by tang 070202

/* Public head file */
#include "sec_uecst.h"
#include "sec_wimsi.h"
#include "sec_cd.h"

/* Certificate related */
#include "sec_cm.h"
#include "we_x509.h" /*add by tang 070205*/
#include "sec_wtlsparser.h"
#include "oem_secx509parser.h"
#include "sec_iwapim.h"

/* WIM related */
#include "sec_wimpkcresp.h"
#include "sec_iwim.h"
#include "sec_wim.h"
#include "sec_wimresp.h"
#include "sec_wimsp.h"

/* User Event */
#include "sec_ue.h"
#include "sec_ueh.h"
#include "sec_iue.h"
#include "sec_ueresp.h"

/* Sec other */
#include "sec_resp.h"
#include "sec_dp.h"
#include "sec_tl.h"
#include "sec_pubdata.h"
#include "sec_evtdcvt.h"


/***************************************************************************************************
*   Type Define Section
***************************************************************************************************/
/*common I interface*/
typedef struct ISec ISec;
struct ISec
{ 
    WE_HANDLE   hPrivateData;
    /*added by Bird 061122*/
    WE_INT32     iIFType;
    St_WimUCertKeyPairInfo      *pstUserCertKeyPair;    
};
/*added by Bird 061122*/
typedef enum tagE_IFType
{
    E_SEC_WTLS,
    E_SEC_BRS,
    E_SEC_SIGNTEXT
}E_IFType;
#endif
