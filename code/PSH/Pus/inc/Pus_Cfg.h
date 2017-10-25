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
 * Pus_Cfg.h
 *
 * Created by Kent Olsson, Thu Sep 11 15:51:11 2002.
 *
 * Revision history:
 *
 */
#ifndef _Pus_Cfg_h
#define _Pus_Cfg_h

/* PUS_CONFIG_WAP_PUSH_MESSAGES Requires the UBS module
 * If not supported PUS will ignore the following message types
 * - SIA
 * - SL
 * - SI
 * - CO
 */
#define PUS_CONFIG_WAP_PUSH_MESSAGES  

#define PUS_CONFIG_WSP_PUSH
#define PUS_CONFIG_HTTP_PUSH
#define PUS_CONFIG_MULTIPART

/**************************************************
 * Parser configuration settings
 **************************************************/

#define PUS_CFG_MAX_CONCURRENT_PARSINGS 10

/* The initial buffer size used for the parser. */
#define PUS_PRSR_BUF_SIZE 500

/* The default character set used when parsing a text document
 * with no encoding information. Must be ASCII based.
 * PUS_CFG_DEFAULT_CHARSET can only take the value WE_CHARSET_UTF_8
 * or WE_CHARSET_LATIN_1. */
#define PUS_CFG_DEFAULT_CHARSET   WE_CHARSET_UTF_8

/**************************************************
 * Connections configuration settings
 **************************************************/

/* Max number of open connections per active Object */
#define PUS_CFG_MAX_CONNECTIONS_PER_USER          5

#define PUS_CFG_MAX_CONNECTIONS                   20

#define PUS_CFG_UDP_DEFAULT_PUSH_PORT             2948
#define PUS_CFG_WTLS_DEFAULT_PUSH_PORT            2949
#define PUS_CFG_HTTP_DEFAULT_PUSH_PORT            4035

/**************************************************
 * Other settings
 **************************************************/
/* 
 * PUS_CFG_SIR_LOCKOUT_TIMER is used for handling a lockout timer for SIR. 
 * The lockut timer is used to protect against denial of service 
 * attacks. The timer is turned om when a SIR arrives and can be handled
 * sucessfully. All other SIR that arrives when the timer is set will 
 * be discarded. The timer is turned of and SIR can be handled again
 * after one of the following cases:
 * - When the connection has been established 
 * - If the user answers the dialog followed after an SIR with cancel 
 *   (then the connection will never be established)
 * - If the timer expires.
 *
 * PUS_CFG_SIR_LOCKOUT_TIMER defines how long the 
 * lockout timer should be on. The value should be given
 * in 1/1000ths of a second
 *
 * If this behavior is not requried set the value to 0,
 * and no lockout tmer will be used
 */
#define PUS_CFG_SIR_LOCKOUT_TIMER                 10000

#endif
