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

/*!\file mauiconf.c
 * \brief User Interface for the configuration view.
 */

/* WE */
#include "We_Wid.h"
#include "We_Int.h"
#include "We_Mem.h"
#include "We_Lib.h"
#include "We_Def.h"
#include "We_Log.h"
#include "We_Cfg.h"
#include "We_Neta.h"
#include "We_Core.h"

/* MMS */
#include "Mms_Def.h"

/* MSA */
#include "Msa_Intsig.h"
#include "Msa_Rc.h"
#include "Msa_Cfg.h"
#include "Msa_Types.h"
#include "Msa_Uicmn.h"
#include "Msa_Conf.h"
#include "Msa_Uiconf.h"
#include "Msa_Uiform.h"
#include "Msa_Mem.h"

/******************************************************************************
 * Data-types
 *****************************************************************************/

/*! \struct MsaMenuTable
 *  \brief Table data for all configuration menus.
 */
typedef struct 
{
    int             configId;           /*!< Menu WID. See #MsaConfigMenu */
    int             childId;            /*!< Menu item WID. See #MsaConfigGroups
                                             #MsaConfigFilter, #MsaConfigSend */
    MsaInputType    menuType;           /*!< Type of input form. */
    MsaInputActionCallBack callback;    /*!< Callback function for input form*/
    MsaInputActionBackCallBack backCallback; /*!< BACK Callback function */
    WeStringHandle menuItems[MSA_CONFIG_MAX_MENU_DEPTH]; /*!< String 
                                                      handles for menu items */
    WE_UINT32      titleStrId;         /*!< String WID for the form title */
}MsaMenuTable;

/******************************************************************************
 * Constants
 *****************************************************************************/

#define NET_ACCOUNT_BUFF_LEN 20  /* !< Maximum length for the name of a network
                                       account*/

/******************************************************************************
 * Static data
 *****************************************************************************/

/*! This variable contains all menu information needed to present the 
 *  configuration menus 
 */
static const 
MsaMenuTable menuTable[MSA_CONFIG_MENU_COUNT][MSA_CONFIG_MAX_MENU_DEPTH] =
{
    /*lint -e{785} */
    { /* MSA_CONFIG_MENU_DEFAULT */
        {MSA_CONFIG_MENU_DEFAULT, MSA_CONFIG_MENU_ROOT, MSA_SELECT_INPUT, 
            (MsaInputActionCallBack)msaConfigMenuCallback, 
            (MsaInputActionBackCallBack)msaConfigMenuBackCallback,
            MSA_CONFIG_MENU, MSA_STR_ID_CONFIG}
    },
    { /* MSA_CONFIG_MENU_ROOT */
        {MSA_CONFIG_SENDING, MSA_CONFIG_MENU_SEND, MSA_SELECT_INPUT, 
            (MsaInputActionCallBack)msaConfigMenuCallback, NULL,
            MSA_CONFIG_MENU_MS, MSA_STR_ID_CONFIG_SENDING},

        {MSA_CONFIG_RETRIEVAL, MSA_CONFIG_MENU_RETRIEVE, MSA_SELECT_INPUT,
            (MsaInputActionCallBack)msaConfigMenuCallback, NULL, 
            MSA_CONFIG_MENU_RETRIEVAL, MSA_STR_ID_CONFIG_MSG_RETRIEVAL},

        {MSA_CONFIG_FILTER, MSA_CONFIG_MENU_FILTER, MSA_SELECT_INPUT, 
            (MsaInputActionCallBack)msaConfigMenuCallback, NULL, 
            MSA_CONFIG_MENU_MR, MSA_STR_ID_CONFIG_MSG_FILTERS},

        {MSA_CONFIG_MMSC, -1, MSA_SINGLE_LINE_INPUT, 
            (MsaInputActionCallBack)msaMmscCallback, NULL, 
            {0}, MSA_STR_ID_CONFIG_MMSC},

        {MSA_CONFIG_NETWORK, MSA_CONFIG_MENU_NETWORK, MSA_SELECT_INPUT, 
            (MsaInputActionCallBack)msaConfigMenuCallback, NULL, 
            MSA_CONF_MENU_STRS_NETWORK, MSA_STR_ID_CONFIG_NETWORK}

    },
    { /* MSA_CONFIG_MENU_SEND */
        {MSA_CONFIG_EXP_TIME, -1, MSA_RADIO_BUTTON_INPUT, 
            (MsaInputActionCallBack)msaExpTimeCallback, NULL, 
            MSA_CONFIG_MENU_VALIDITY, MSA_STR_ID_CONFIG_VAL_PER},

        {MSA_CONFIG_READ_REPORT, -1, MSA_RADIO_BUTTON_INPUT, 
            (MsaInputActionCallBack)msaReadReportCallback, NULL, 
            MSA_CONFIG_MENU_RR, MSA_STR_ID_CONFIG_RR},

        {MSA_CONFIG_DELIV_REPORT, -1, MSA_RADIO_BUTTON_INPUT, 
            (MsaInputActionCallBack)msaDelivReportCallback, NULL, 
            MSA_CONFIG_MENU_DR, MSA_STR_ID_CONFIG_DR},

        {MSA_CONFIG_PRIORITY, -1, MSA_RADIO_BUTTON_INPUT, 
            (MsaInputActionCallBack)msaPriorityCallback, NULL, 
            MSA_CONFIG_MENU_PRIORITY, MSA_STR_ID_CONFIG_PRIORITY},

        {MSA_CONFIG_REPLY_CHRG_MENU, MSA_CONFIG_MENU_REP_CHRG_SETTINGS, 
            MSA_SELECT_INPUT, 
            (MsaInputActionCallBack)msaConfigMenuCallback, NULL, 
            MSA_CONFIG_MENU_REP_CHRG_ITEMS, MSA_STR_ID_REPLY_CHARGING},

        {MSA_CONFIG_SENDER_VISIBILITY, -1, MSA_RADIO_BUTTON_INPUT, 
            (MsaInputActionCallBack)msaSenderVisibilityCallback, NULL, 
            MSA_CONFIG_MENU_SENDER_VISIBILITY, MSA_STR_ID_CONFIG_SEND_VISIBILITY},

        {MSA_CONFIG_SAVE_ON_SEND, -1, MSA_RADIO_BUTTON_INPUT, 
            (MsaInputActionCallBack)msaSaveOnSendCallback, NULL, 
            MSA_CONFIG_MENU_SOS, MSA_STR_ID_CONFIG_SOS},
            
        {MSA_CONFIG_SLIDE_DUR, -1, MSA_SINGLE_LINE_INPUT, 
            (MsaInputActionCallBack)msaSlideDurCallback, NULL, 
            {0}, MSA_STR_ID_DURATION},

        {MSA_CONFIG_DELIVERY_TIME, -1, MSA_RADIO_BUTTON_INPUT, 
            (MsaInputActionCallBack)msaDeliveryTimeCallback, NULL, 
            MSA_CONFIG_MENU_DELIVERY_TIME, MSA_STR_ID_CONFIG_DELIVERY_TIME},

        {MSA_CONFIG_SIGNATURE_MENU, MSA_CONFIG_MENU_SIGNATURE, MSA_SELECT_INPUT, 
            (MsaInputActionCallBack)msaConfigMenuCallback, NULL, 
            MSA_CONFIG_MENU_SIG, MSA_STR_ID_CONFIG_SIGNATURE}
    },
    { /* MSA_CONFIG_MENU_FILTER */
        {MSA_CONFIG_IM_SERVER, -1, MSA_SINGLE_LINE_INPUT, 
            (MsaInputActionCallBack)msaImServerCallback, NULL, 
            {0}, MSA_STR_ID_CONFIG_IM_SERVER},

        {MSA_CONFIG_IM_SIZE, -1, MSA_SINGLE_LINE_INPUT, 
            (MsaInputActionCallBack)msaImRetrSizeCallback, NULL, 
            {0}, MSA_STR_ID_CONFIG_IM_SIZE},
        
        {MSA_CONFIG_TREAT_AS_DEL, -1, MSA_RADIO_BUTTON_INPUT, 
            (MsaInputActionCallBack)msaTreatAsDelCallback, NULL, 
            MSA_CONFIG_MENU_TREAT_AS_DEL, MSA_STR_ID_CONFIG_TREAT_AS_DEL},
        
        {MSA_CONFIG_RETR_SIZE, -1, MSA_SINGLE_LINE_INPUT, 
            (MsaInputActionCallBack)msaDownloadSizeCallback, NULL, 
            {0}, MSA_STR_ID_MAX_RETR_SIZE},
        
        {MSA_CONFIG_ALLOW_ANONYM, -1, MSA_RADIO_BUTTON_INPUT, 
            (MsaInputActionCallBack)msaAllowAnonymCallback, NULL, 
            MSA_CONFIG_MENU_ALLOW_ANONYM, MSA_STR_ID_CONFIG_ANONYM},
        
        {MSA_CONFIG_ALLOW_PERSONAL, -1, MSA_RADIO_BUTTON_INPUT, 
            (MsaInputActionCallBack)msaAllowPersonalCallback, NULL, 
            MSA_CONFIG_MENU_ALLOW_PERSONAL, MSA_STR_ID_CONFIG_PERS},
        
        {MSA_CONFIG_ALLOW_ADVERT, -1, MSA_RADIO_BUTTON_INPUT, 
            (MsaInputActionCallBack)msaAllowAdvertCallback, NULL, 
            MSA_CONFIG_MENU_ALLOW_ADVERT, MSA_STR_ID_CONFIG_ADVERT},
        
        {MSA_CONFIG_ALLOW_INFO, -1, MSA_RADIO_BUTTON_INPUT, 
            (MsaInputActionCallBack)msaAllowInfoCallback, NULL, 
            MSA_CONFIG_MENU_ALLOW_INFO, MSA_STR_ID_CONFIG_INFO},
        
        {MSA_CONFIG_ALLOW_AUTO, -1, MSA_RADIO_BUTTON_INPUT, 
            (MsaInputActionCallBack)msaAllowAutoCallback, NULL, 
            MSA_CONFIG_MENU_ALLOW_AUTO, MSA_STR_ID_CONFIG_AUTO},
        
        {MSA_CONFIG_ALLOW_STRING, -1, MSA_SINGLE_LINE_INPUT, 
            (MsaInputActionCallBack)msaAllowStringCallback, NULL, 
            {0}, MSA_STR_ID_CONFIG_USER_STRING}
    },
    { /* MSA_CONFIG_MENU_NETWORK */
        {MSA_CONFIG_CONN_TYPE, MSA_CONFIG_CONN_TYPE, MSA_RADIO_BUTTON_INPUT, 
            (MsaInputActionCallBack)msaNetworkCallback, NULL, 
            MSA_CONFIG_MENU_CONN_TYPE, MSA_STR_ID_CONN_TYPE},

        {MSA_CONFIG_NET_ACCOUNT, MSA_CONFIG_NET_ACCOUNT, MSA_RADIO_BUTTON_INPUT, 
            (MsaInputActionCallBack)msaNetworkCallback, NULL, 
            {0}/*Special case: dynamic content*/, MSA_STR_ID_NETWORK_ACCOUNT},

        {MSA_CONFIG_WAP_GW, MSA_CONFIG_WAP_GW, MSA_SINGLE_LINE_INPUT, 
            (MsaInputActionCallBack)msaNetworkCallback, NULL, 
            {0}, MSA_STR_ID_WAP_GATEWAY},
        
        {MSA_CONFIG_LOGIN, MSA_CONFIG_LOGIN, MSA_SINGLE_LINE_INPUT, 
            (MsaInputActionCallBack)msaNetworkCallback, NULL, 
            {0}, MSA_STR_ID_LOGIN},
        
        {MSA_CONFIG_PASSWD, MSA_CONFIG_PASSWD, MSA_SINGLE_LINE_INPUT, 
            (MsaInputActionCallBack)msaNetworkCallback, NULL, 
            {0}, MSA_STR_ID_PASSWD},
        
        {MSA_CONFIG_REALM, MSA_CONFIG_REALM, MSA_SINGLE_LINE_INPUT, 
            (MsaInputActionCallBack)msaNetworkCallback, NULL, 
            {0}, MSA_STR_ID_REALM},
        
        {MSA_CONFIG_SEC_PORT, MSA_CONFIG_SEC_PORT, MSA_SINGLE_LINE_INPUT, 
            (MsaInputActionCallBack)msaNetworkCallback, NULL, 
            {0}, MSA_STR_ID_SEC_PORT},
        
        {MSA_CONFIG_HTTP_PORT, MSA_CONFIG_HTTP_PORT, MSA_SINGLE_LINE_INPUT, 
            (MsaInputActionCallBack)msaNetworkCallback, NULL, 
            {0}, MSA_STR_ID_HTTP_PORT}
    },
    { /* MSA_CONFIG_MENU_MMS_SETTINGS */
        /*  This menu reuses the first 4 entries in the MSA_CONFIG_MENU_SEND
         *  menu so if that menu changes, this menu has to be re-written 
         */
        {MSA_CONFIG_SENDING, MSA_CONFIG_MENU_SEND, MSA_SELECT_INPUT, 
            (MsaInputActionCallBack)msaConfigMenuCallback, NULL, 
            MSA_CONFIG_MENU_MMS, MSA_STR_ID_CONFIG}
    },
    { /* MSA_CONFIG_MENU_REP_CHRG_SETTINGS */
        {MSA_CONFIG_REPLY_CHARGING, -1, MSA_RADIO_BUTTON_INPUT, 
            (MsaInputActionCallBack)msaReplyChargingCallback, NULL, 
            MSA_CONFIG_MENU_REPLY_CHARGING, MSA_STR_ID_REPLY_CHARGING},

        {MSA_CONFIG_REPLY_CHARGING_DEADLINE, -1, MSA_RADIO_BUTTON_INPUT, 
            (MsaInputActionCallBack)msaReplyChargingDeadlineCallback, NULL,
            MSA_CONFIG_MENU_REPLY_CHARGING_DEADLINE, 
            MSA_STR_ID_REPLY_CHARGING_DEADLINE},
        
        {MSA_CONFIG_REPLY_CHARGING_SIZE, -1, MSA_RADIO_BUTTON_INPUT, 
            (MsaInputActionCallBack)msaReplyChargingSizeCallback, NULL,
            MSA_CONFIG_MENU_REPLY_CHARGING_SIZE, 
            MSA_STR_ID_REPLY_CHARGING_SIZE}
    },      
    { /* MSA_CONFIG_MENU_RETRIEVE */
        {MSA_CONFIG_HOME_NETWORK, -1, MSA_RADIO_BUTTON_INPUT, 
        (MsaInputActionCallBack)msaRetrievalHomeModeCallback, NULL, 
        MSA_CONFIG_MENU_RETRIEVAL_HOME, MSA_STR_ID_CONFIG_MSG_RETR_HOME_MODE},

        {MSA_CONFIG_ROAM_NETWORK, -1, MSA_RADIO_BUTTON_INPUT, 
        (MsaInputActionCallBack)msaRetrievalRoamModeCallback, NULL, 
        MSA_CONFIG_MENU_RETRIEVAL_ROAM, MSA_STR_ID_CONFIG_MSG_RETR_ROAM_MODE},

        {MSA_CONFIG_SEND_READ_REPORT, -1, MSA_RADIO_BUTTON_INPUT, 
            (MsaInputActionCallBack)msaSendReadReportCallback, NULL, 
            MSA_CONFIG_MENU_SEND_READ_REPORT, MSA_STR_ID_CONFIG_SEND_READ_REPORT},

        {MSA_CONFIG_DELIVERY_REPORT_ALLOWED, -1, MSA_RADIO_BUTTON_INPUT, 
            (MsaInputActionCallBack)msaDeliveryReportAllowedCallback, NULL, 
            MSA_CONFIG_MENU_DELIVERY_REPORT_ALLOWED, MSA_STR_ID_CONFIG_DELIVERY_REPORT_ALLOWED}
    },
    { /* MSA_CONFIG_MENU_SIGNATURE */
        {MSA_CONFIG_USE_SIGNATURE, -1, MSA_RADIO_BUTTON_INPUT, 
            (MsaInputActionCallBack)msaUseSignatureCallback, NULL, 
            MSA_CONFIG_MENU_USE_SIGNATURE, MSA_STR_ID_USE_SIGNATURE},

        {MSA_CONFIG_EDIT_SIGNATURE, -1, MSA_TEXT_INPUT, 
            (MsaInputActionCallBack)msaEditSignatureCallback, NULL,
        {0}, MSA_STR_ID_EDIT_SIGNATURE}
    },      
};

/******************************************************************************
 * Prototypes
 *****************************************************************************/
static WeStringHandle *createNetworkAccountList(void);
static void releaseNetworkAccountList(WeStringHandle **list);
static WeStringHandle *getStrItems(const WeStringHandle items[], int size);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief Creates a list of network accounts.
 *        This function creates a widget string for each account name
 *        and puts it in a list allocated by the function. 
 * \return The list of string handles on success, otherwise NULL.
 *****************************************************************************/
static WeStringHandle *createNetworkAccountList(void)
{
    int wid;
    int index;
    int accountCount;
    WeStringHandle *strings;
    char buffer[NET_ACCOUNT_BUFF_LEN];

    /* Count total number of network accounts */
    accountCount = 0;
    wid = WE_NETWORK_ACCOUNT_GET_FIRST();
    while(wid >= 0)
    {
        ++accountCount;
        wid = WE_NETWORK_ACCOUNT_GET_NEXT(wid);
    }

    if (accountCount > MSA_CONFIG_MAX_MENU_DEPTH) 
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) WARNING! Not all network accounts could be enumerated\n", 
            __FILE__, __LINE__));        
    }
    
    strings = MSA_ALLOC((unsigned int)(accountCount + 1) * 
        sizeof(WeStringHandle));

    /* Add items to the list */
    wid = WE_NETWORK_ACCOUNT_GET_FIRST();
    index = 0;
    while (wid >= 0 && index < (MSA_CONFIG_MAX_MENU_DEPTH - 1))
    {
        if (0 > WE_NETWORK_ACCOUNT_GET_NAME(wid, buffer, NET_ACCOUNT_BUFF_LEN))
        {
            MSA_FREE(strings);
            return NULL;
        }
        strings[index] = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, buffer, 
            WeUtf8, (int)strlen(buffer) + 1, 0);
        if (0 == strings[index])
        {
            MSA_FREE(strings);
            return NULL;
        }
        index++;
        wid = WE_NETWORK_ACCOUNT_GET_NEXT(wid);
    }
    /* Set an end marker last in the list */
    strings[index] = 0;
    return strings;
}

/*!
 * \brief Allocates an array of string handles.
 *
 * \param items An array of resource IDs
 * \param size The size in bytes of the array.
 * \return The newly allocated array if everything was ok or else the old
 *          array.
 *****************************************************************************/
static WeStringHandle *getStrItems(const WeStringHandle items[], int size)
{
    int i, n;
    WeStringHandle *tmpItems;
    tmpItems = MSA_ALLOC((unsigned)size);
    for (i = 0, n = 1; items[i] != 0; i++)
    {
        tmpItems[i] = MSA_GET_STR_ID(items[i]);
        n++;
    }
    tmpItems[n - 1] = 0;
    return tmpItems;
} /*getStrItems*/

/*!
 *  \brief Activates an arbitrary configuration menu.
 *  \param menu The menu to activate.
 *  \param wid The WID of the form to activate. See #MsaConfigGroups
 *            #MsaConfigFilter, #MsaConfigSend.
 *  \param data If not NULL then this data will be set instead of global
 *              configuration data.
 *  \return Always FALSE since the form should stay visible until we close it.
 *****************************************************************************/
WE_BOOL msaConfigActivateMenu(MsaConfigMenu menu, int wid, void *data)
{
    MsaInputItem item = {0};
    int value, maxTextSize;
    char *str;
    WeTextType textType;
    MsaMenuTable *mt;
    
    mt = (MsaMenuTable *)&menuTable[menu][wid];

    item.dataPtr = data;
    item.uintValue = (unsigned int)mt->childId;
    item.callback = mt->callback;
    item.backCallback = mt->backCallback;
    item.type = mt->menuType;
    item.items = NULL;
    str = NULL;
    switch (item.type)
    {
    case MSA_TEXT_INPUT:
    case MSA_SINGLE_LINE_INPUT:
        msaGetConfigData(menu, mt->configId, data,
            &value, &str, &maxTextSize, &textType);
        item.selectedItem = value;
        item.maxTextSize = (unsigned)maxTextSize;
        item.textType = textType;
        item.text = str;
        break;
    case MSA_RADIO_BUTTON_INPUT:
    case MSA_SELECT_INPUT:
        msaGetConfigData(menu, mt->configId, data,
            &value, &str, &maxTextSize, &textType);
        item.selectedItem = value;
        if (MSA_CONFIG_MENU_NETWORK == menu &&
            MSA_CONFIG_NET_ACCOUNT == (MsaConfigNetwork)wid) 
        {
            /*  Special case since the network account strings are dynamic. 
             *  The string handles needs to be released after the form is 
             *  closed and for this purpose the string handles are saved in
             *  the menuTable. */
            item.items = createNetworkAccountList();
        }
        else
        {
            item.items = getStrItems(mt->menuItems, 
                sizeof(mt->menuItems));
        }
        break;
    default:
        break;
    }
    (void)msaCreateInputForm(&item, MSA_GET_STR_ID(mt->titleStrId));
    
    /*  Take care of special cases where we have to free allocated data 
     *  from the msaGetConfigData function.
     */
    if ((MSA_CONFIG_MENU_ROOT == menu && 
         MSA_CONFIG_MMSC == (MsaConfigGroups)mt->configId) ||
        (MSA_CONFIG_MENU_SEND == menu &&
         MSA_CONFIG_SLIDE_DUR == (MsaConfigSend)mt->configId) ||
        (MSA_CONFIG_MENU_FILTER == menu &&
         MSA_CONFIG_IM_SIZE == (MsaConfigFilter)mt->configId) ||
        (MSA_CONFIG_MENU_FILTER == menu &&
         MSA_CONFIG_RETR_SIZE == (MsaConfigFilter)mt->configId) ||
        (MSA_CONFIG_MENU_NETWORK == menu && 
         MSA_CONFIG_SEC_PORT == (MsaConfigNetwork)mt->configId) ||
        (MSA_CONFIG_MENU_NETWORK == menu && 
         MSA_CONFIG_HTTP_PORT == (MsaConfigNetwork)mt->configId))
    {
        MSA_FREE(str);
    }

    /*  Special case since the network account strings are dynamic. 
     *  The string handles needs to be released after the form is 
     *  created.
     */
    if (MSA_CONFIG_MENU_NETWORK == menu &&
        MSA_CONFIG_NET_ACCOUNT == (MsaConfigNetwork)mt->configId) 
    {
        releaseNetworkAccountList(&(item.items));
    }

    MSA_FREE(item.items);
    item.items = NULL;
    return FALSE;
} /*msaConfigActivateMenu*/

/*!
 *  \brief Handles the selection of configuration type.
 * \param  str N/A.
 * \param  value The selected menu item.
 * \param  p1 N/A.
 * \param  p2 The new menu to show.
 * \return Always FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL 
msaConfigMenuCallback(char *str, int value, void *p1, int p2)
{
    if (str)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA,
            "(%s) (%d) Msaningless logging to avoid warnings\n", __FILE__, 
            __LINE__));
    }
    WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA,
        "(%s) (%d) Callback for root menu\n", __FILE__, 
        __LINE__));
    return msaConfigActivateMenu((MsaConfigMenu)p2, value, p1);
} /*msaConfigMenuCallback*/

/*!
 * \brief  Handles the Back-action from the settings menu.
 * \param  p1 N/A.
 * \param  p2 The new menu to show.
 * \return Always FALSE.
 *****************************************************************************/
/*lint -e{818} */
void msaConfigMenuBackCallback(void *p1, int p2)
{
    if (p1 || p2)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA,
            "(%s) (%d) Msaningless logging to avoid warnings\n", __FILE__, 
            __LINE__));
    }
    /* If it is the last menu we send a signal to maconf so it can
     * deactivate the module if necessary */
    (void)MSA_SIGNAL_SENDTO(MSA_CONFIG_FSM, MSA_SIG_CONF_DEACTIVATE);            
} /*msaConfigMenuBackCallback*/

/*!
 * \brief Initiates internal data-structures for the config view.
 *
 *****************************************************************************/
void msaConfigUiInit()
{
    WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA,
        "(%s) (%d) Initiating config UI\n", __FILE__, __LINE__));
} /*msaConfigUiInit*/

/*!
 * \brief Frees a list of network account strings.
 *****************************************************************************/
/*lint -e{818} */
static void releaseNetworkAccountList(WeStringHandle **list)
{
    int i = 0;
    while ((*list)[i] && i < MSA_CONFIG_MAX_MENU_DEPTH) 
    {
        (void)WE_WIDGET_RELEASE((*list)[i]);
        (*list)[i++] = 0;
    }
} /*releaseNetworkAccountList*/

