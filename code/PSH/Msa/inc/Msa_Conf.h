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

/*! \file maconf.g
 *  Header file for the configuration of the MSA.
 */

#ifndef MSA_CONF_H
#define MSA_CONF_H

#ifndef MMS_DEF_H
#error You must include mms_def.h before maconf.h!
#endif

#ifndef MSA_TYPES_H
#error You must include matypes.h before maconf.h!
#endif

#ifndef _we_core_h
#error You must include we_core.h before maconf.h!
#endif

/****************************************************************************
 * Constants 
 ****************************************************************************/

/* <! Schemes that are supported by the MSA */
#define MSA_CR_SCHEME               "Scheme"
#define MSA_CR_PATH                 "/FRW/RT_DB/MSA"

/*!\enum MsaConfigSignalId
 * \brief Signals in CONF
 */
typedef enum 
{
    /* 
     * Get the configuration and start the application GUI.
     */
    MSA_SIG_CONF_INITIAL_ACTIVATE,

    /* 
     * Start configuration menu GUI.
     */
    MSA_SIG_CONF_ACTIVATE,

    /* 
     * Start settings menu GUI.
     */
    MSA_SIG_CONF_ACTIVATE_SETTINGS,

    /* 
     * Terminate configuration menu GUI.
     */
    MSA_SIG_CONF_DEACTIVATE,

    /* The response from a get registry key operation
     * p_param = the response
     */
    MSA_SIG_CONF_GET_REG_RSP,

    /* External requests to refresh network status variables */
    MSA_SIG_CONF_GET_NETWORK_STATUS,

    /*! No signal !*/
    MSA_SIG_CONF_SIGMAX
} MsaConfigSignalId;

/* The name of the sender */
#define MSA_CONF_FROM_NAME_MAX_SIZE     50

/* The address of the sender */
#define MSA_CONF_FROM_ADDR_MAX_SIZE     50

/* The proxy address for the MMSC */
#define MSA_CONF_PROXY_ADDR_MAX_SIZE    50

/* The proxy URI for the MMSC */
#define MSA_CONF_PROXY_URI_MAX_SIZE     50

/* The proxy port for the MMSC */
#define MSA_CONF_PROXY_PORT_MAX_SIZE    6

/* Proxy scheme gadget properties */
#define MSA_CONF_PROXY_SCHEME_MAX_SIZE  50

/* WAP gateway address */
#define MSA_CONF_WAP_GATEWAY_MAX_SIZE   50

/* Username for the WAP gateway */
#define MSA_CONF_LOGIN_MAX_SIZE         25

/* Password for the WAP gateway */
#define MSA_CONF_PASSWD_MAX_SIZE        25

/* Realm */
#define MSA_CONF_REALM_MAX_SIZE         50

/* Secure port */
#define MSA_CONF_SEC_PORT_MAX_SIZE      6

/* HTTP port */
#define MSA_CONF_HTTP_PORT_MAX_SIZE     6

/* Max message size for immediate retrieval */
#define MSA_CONF_MSG_SIZE_MAX_SIZE      10

/* Maximum input size for the MMSC*/
#define MSA_CONF_MMSC_MAX_SIZE          300

/* Max length (characters) for message class definition */
#define MSA_CONF_ALLOW_STR_MAX_SIZE     100

/******************************************************************************
 * Data types 
 *****************************************************************************/


/******************************************************************************
 * Function declarations 
 *****************************************************************************/

MsaConfig *msaGetConfig(void);
int msaGetAlignment(void);/*CR_17749*/
void msaCopyConfig(MsaConfig *dst);
void msaConfInit(void);
void msaConfTerminate(void);
int  msaIsCreateConfValid(void);
WE_BOOL msaSaveOnSendCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaExpTimeCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaReadReportCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaDelivReportCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaPriorityCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaSlideDurCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaReplyChargingCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaUseSignatureCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaEditSignatureCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaReplyChargingDeadlineCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaReplyChargingSizeCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaRetrievalHomeModeCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaRetrievalRoamModeCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaImRetrSizeCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaTreatAsDelCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaAllowAdvertCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaAllowPersonalCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaAllowInfoCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaAllowAutoCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaAllowAnonymCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaAllowStringCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaImServerCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaDeliveryReportAllowedCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaSendReadReportCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaDeliveryTimeCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaMmscCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaNetworkCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaDownloadSizeCallback(char *str, int value, void *p1, int p2);
WE_BOOL msaSenderVisibilityCallback(char *str, int value, void *p1, int p2);
void msaGetConfigData(MsaConfigMenu menu, int configId, const void *tmpData, 
    int *intValue, char **strValue, int *maxStrLen, WeTextType *textType);
void msaUpdateNetworkStatusInfo(MsaStateMachine fsm, int signal);
void msaSaveProvisioningData(we_registry_response_t *regRsp);
#endif /* _MACONF_H_ */
