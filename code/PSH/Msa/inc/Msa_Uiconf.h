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

/*!\file mauiconf.h
 * \brief The GUI for the configuration view.
 */

#ifndef _MAUICONF_H_
#define _MAUICONF_H_

#define MSA_CONFIG_MENU_MMS {MSA_STR_ID_CONFIG_VAL_PER, \
    MSA_STR_ID_CONFIG_RR, MSA_STR_ID_CONFIG_DR, MSA_STR_ID_CONFIG_PRIORITY,\
    MSA_STR_ID_REPLY_CHARGING, MSA_STR_ID_CONFIG_SEND_VISIBILITY, 0}

#define MSA_CONFIG_MENU_MR {MSA_STR_ID_CONFIG_IM_SERVER, \
    MSA_STR_ID_CONFIG_IM_SIZE, MSA_STR_ID_CONFIG_TREAT_AS_DEL, \
    MSA_STR_ID_MAX_RETR_SIZE, MSA_STR_ID_CONFIG_ANONYM, \
    MSA_STR_ID_CONFIG_PERS, \
    MSA_STR_ID_CONFIG_ADVERT, MSA_STR_ID_CONFIG_INFO, MSA_STR_ID_CONFIG_AUTO, \
    MSA_STR_ID_CONFIG_USER_STRING, 0}

#define MSA_CONFIG_MENU_MS {MSA_STR_ID_CONFIG_VAL_PER, \
    MSA_STR_ID_CONFIG_RR, MSA_STR_ID_CONFIG_DR, MSA_STR_ID_CONFIG_PRIORITY, \
    MSA_STR_ID_REPLY_CHARGING, MSA_STR_ID_CONFIG_SEND_VISIBILITY, MSA_STR_ID_CONFIG_SOS, \
    MSA_STR_ID_DURATION, MSA_STR_ID_CONFIG_DELIVERY_TIME, MSA_STR_ID_CONFIG_SIGNATURE, 0}

#define MSA_CONFIG_MENU_RETRIEVAL   {MSA_STR_ID_CONFIG_MSG_RETR_HOME_MODE, \
                                     MSA_STR_ID_CONFIG_MSG_RETR_ROAM_MODE, \
                                     MSA_STR_ID_CONFIG_SEND_READ_REPORT, \
                                     MSA_STR_ID_CONFIG_DELIVERY_REPORT_ALLOWED, 0}

#define MSA_CONFIG_MENU_REP_CHRG_ITEMS { MSA_STR_ID_REPLY_CHARGING, \
    MSA_STR_ID_REPLY_CHARGING_DEADLINE, MSA_STR_ID_REPLY_CHARGING_SIZE, 0}

#define MSA_CONFIG_MENU {MSA_STR_ID_CONFIG_SENDING, \
                         MSA_STR_ID_CONFIG_MSG_RETRIEVAL, \
                         MSA_STR_ID_CONFIG_MSG_FILTERS, \
                         MSA_STR_ID_CONFIG_MMSC, \
                         MSA_STR_ID_CONFIG_NETWORK}

#define MSA_CONF_MENU_STRS_NETWORK {MSA_STR_ID_CONN_TYPE, \
                                 MSA_STR_ID_NETWORK_ACCOUNT, \
                                 MSA_STR_ID_WAP_GATEWAY, \
                                 MSA_STR_ID_LOGIN, \
                                 MSA_STR_ID_PASSWD, \
                                 MSA_STR_ID_REALM, \
                                 MSA_STR_ID_SEC_PORT, \
                                 MSA_STR_ID_HTTP_PORT}

#define MSA_CONFIG_MENU_SOS {MSA_STR_ID_OFF, \
                                MSA_STR_ID_ON, 0}
#define MSA_CONFIG_MENU_RR {MSA_STR_ID_OFF, \
                                MSA_STR_ID_ON, 0}
#define MSA_CONFIG_MENU_SIG {MSA_STR_ID_USE_SIGNATURE, \
                                MSA_STR_ID_EDIT_SIGNATURE, 0}
#define MSA_CONFIG_MENU_DR {MSA_STR_ID_OFF, \
                                MSA_STR_ID_ON, 0}
#define MSA_CONFIG_MENU_VALIDITY {MSA_STR_ID_CONFIG_1_HOUR, \
                                    MSA_STR_ID_CONFIG_12_HOURS, \
                                    MSA_STR_ID_CONFIG_1_DAY, \
                                    MSA_STR_ID_CONFIG_1_WEEK, \
                                    MSA_STR_ID_CONFIG_MAX,0}
#define MSA_CONFIG_MENU_PRIORITY {MSA_STR_ID_CONFIG_LOW, \
                                    MSA_STR_ID_CONFIG_NORMAL, \
                                    MSA_STR_ID_CONFIG_HIGH, 0}

#define MSA_CONFIG_MENU_ALLOW_PERSONAL  {MSA_STR_ID_REJECT, MSA_STR_ID_ALLOW, 0}

#define MSA_CONFIG_MENU_ALLOW_ADVERT    {MSA_STR_ID_REJECT, MSA_STR_ID_ALLOW, 0}

#define MSA_CONFIG_MENU_ALLOW_INFO      {MSA_STR_ID_REJECT, MSA_STR_ID_ALLOW, 0}

#define MSA_CONFIG_MENU_ALLOW_AUTO      {MSA_STR_ID_REJECT, MSA_STR_ID_ALLOW, 0}

#define MSA_CONFIG_MENU_ALLOW_ANONYM    {MSA_STR_ID_REJECT, MSA_STR_ID_ALLOW, 0}

#define MSA_CONFIG_MENU_TREAT_AS_DEL    {MSA_STR_ID_OFF, MSA_STR_ID_ON, 0}

#define MSA_CONFIG_MENU_RETRIEVAL_HOME  {MSA_STR_ID_IM_RETRIEVAL, \
                                         MSA_STR_ID_DEL_RETRIEVAL, 0}

#define MSA_CONFIG_MENU_RETRIEVAL_ROAM  {MSA_STR_ID_AUTO_RETRIEVAL_ROAM, \
                                         MSA_STR_ID_ASK_RETRIEVAL_ROAM, \
                                         MSA_STR_ID_RES_RETRIEVAL_ROAM, 0}

#define MSA_CONFIG_MENU_CONN_TYPE       {MSA_STR_ID_CONN_TYPE_HTTP, \
                                         MSA_STR_ID_CONN_TYPE_WSP_CL, \
                                         MSA_STR_ID_CONN_TYPE_WSP_CO, 0}

#define MSA_CONFIG_MENU_REPLY_CHARGING  {MSA_STR_ID_OFF,\
                                         MSA_STR_ID_ON,\
                                         MSA_STR_ID_TEXT_ONLY, 0}

#define MSA_CONFIG_MENU_USE_SIGNATURE  {MSA_STR_ID_OFF,\
                                         MSA_STR_ID_ON, 0}

#define MSA_CONFIG_MENU_REPLY_CHARGING_DEADLINE  {MSA_STR_ID_CONFIG_1_DAY,\
                                         MSA_STR_ID_CONFIG_1_WEEK,\
                                         MSA_STR_ID_CONFIG_MAX, 0}

#define MSA_CONFIG_MENU_REPLY_CHARGING_SIZE {MSA_STR_ID_CONFIG_1_KB,\
                                         MSA_STR_ID_CONFIG_10_KB,\
                                         MSA_STR_ID_CONFIG_MAX, 0}

#define MSA_CONFIG_MENU_SENDER_VISIBILITY {MSA_STR_ID_SHOW_NUMBER,\
                                           MSA_STR_ID_HIDE_NUMBER, 0}

#define MSA_CONFIG_MENU_DELIVERY_REPORT_ALLOWED {MSA_STR_ID_YES, \
                                                MSA_STR_ID_NO,0}

#define MSA_CONFIG_MENU_SEND_READ_REPORT {MSA_STR_ID_CONFIG_ON_REQUEST, \
                                          MSA_STR_ID_CONFIG_ALWAYS, \
                                          MSA_STR_ID_CONFIG_NEVER,0}

#define MSA_CONFIG_MENU_DELIVERY_TIME    {MSA_STR_ID_CONFIG_IMMEDIATE, \
                                          MSA_STR_ID_CONFIG_IN_ONE_HOUR, \
                                          MSA_STR_ID_CONFIG_IN_12_HOURS, \
                                          MSA_STR_ID_CONFIG_IN_24_HOURS,0}

/*! Maximum number of menu items per menu */
#define MSA_CONFIG_MAX_MENU_DEPTH 12

/******************************************************************************
 * Constants
 *****************************************************************************/

 /******************************************************************************
 * Prototypes
 *****************************************************************************/

int  msaCreateConfigView(const MsaConfig *configValues);
void msaConfigUiInit(void);
void msaDeleteConfigView(void);
WE_BOOL msaConfigMenuCallback(char *str, int value, void *p1, int p2);
void msaConfigMenuBackCallback(void *p1, int p2);

WE_BOOL msaConfigActivateMenu(MsaConfigMenu menu, int wid, void *data);
#endif
