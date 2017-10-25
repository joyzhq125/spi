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

/* WE */
#include "We_Core.h"
#include "We_Log.h"
#include "We_Cfg.h"
#include "We_Int.h"
#include "We_Def.h"
#include "We_Mem.h"
#include "We_Lib.h"
#include "We_Wid.h"
#include "We_Url.h"
#include "We_Cmmn.h"
#include "We_Neta.h"

/* STK */
#include "Stk_If.h"     /* STK: WSP/HTTP Stack signals and types */

/* MMS */
#include "Mms_Def.h"

/* MSA */
#include "Msa_Def.h"
#include "Msa_Cfg.h"
#include "Msa_Types.h"
#include "Msa_Rc.h"
#include "Msa_Intsig.h"
#include "Msa_Core.h"
#include "Msa_Intsig.h"
#include "Msa_Conf.h"
#include "Msa_Uiconf.h"
#include "Msa_Uicmn.h"
#include "Msa_Mem.h"
#include "Msa_Uidia.h"

/*! \file maconf.c
 *! \brief The configuration logic.
 */

/******************************************************************************
 * Structures
 *****************************************************************************/

typedef enum
{
    MSA_CONF_S_UNINITIALIZED,   /*<! Msa config is uninitialized */
    MSA_CONF_S_INITIALIZED      /*<! Msa config is initialized */
}MsaConfState;

typedef struct {
    MsaStateMachine fsm;    /*<! Callback fsm for external requests */
    int             signal; /*<! Callback signal for external requests */
    MsaConfState    state;  /*<! The state of MsaConf */
}MsaConfInstance;

typedef enum
{
    MSA_PRS_KEY_MMSC_URL,
    MSA_PRS_KEY_NET_CONNECTION_TYPE,
    MSA_PRS_KEY_NETWORK_ACCOUNT_ID,
    MSA_PRS_KEY_WAP_GW,
    MSA_PRS_KEY_LOGIN,
    MSA_PRS_KEY_PASSWORD,
    MSA_PRS_KEY_HTTP_PORT
}MsaPrsKeyId;

typedef enum
{
    MSA_PRS_KEY_CLEANSE_YES,
    MSA_PRS_KEY_CLEANSE_NO
}MsaPrsKeyCleanse;

typedef struct 
{
    char                *prsStr;    /*!< Received string from PRS */
    MsaPrsKeyId         keyId;      /*!< string/path/key combo identifier */
    MsaConfigNetwork    groupId;    /*!< Network group wid */
    MsaPrsKeyCleanse    cleanse;    /*!< Whether to clear when receiving 
                                         provisioned settings */
}MsaPrsKeyTable;

/*
 *	Received string from PRS, enumerating identifier, network group
 */
#define MSA_PRS_KEYS {\
{"mmsc_url",       MSA_PRS_KEY_MMSC_URL,            -1,                     MSA_PRS_KEY_CLEANSE_NO},\
{"px_type",        MSA_PRS_KEY_NET_CONNECTION_TYPE, MSA_CONFIG_CONN_TYPE,   MSA_PRS_KEY_CLEANSE_NO},\
{"net_account_id", MSA_PRS_KEY_NETWORK_ACCOUNT_ID,  MSA_CONFIG_NET_ACCOUNT, MSA_PRS_KEY_CLEANSE_NO},\
{"px_address",     MSA_PRS_KEY_WAP_GW,              MSA_CONFIG_WAP_GW,      MSA_PRS_KEY_CLEANSE_YES},\
{"px_auth_name",   MSA_PRS_KEY_LOGIN,               MSA_CONFIG_LOGIN,       MSA_PRS_KEY_CLEANSE_YES},\
{"px_auth_secret", MSA_PRS_KEY_PASSWORD,            MSA_CONFIG_PASSWD,      MSA_PRS_KEY_CLEANSE_YES},\
{"px_port",        MSA_PRS_KEY_HTTP_PORT,           MSA_CONFIG_HTTP_PORT,   MSA_PRS_KEY_CLEANSE_YES},\
{NULL, 0,0}}

/* Configuration IDs */
/*lint -e{751} */
typedef enum
{
    GET_CONFIG_UNDEFINED,
	GET_CONFIG_ALIGNMENT,
    GET_CONFIG_ADDRESS,
    GET_CONFIG_MMSC,
    GET_CONFIG_IM_RETR,
    GET_CONFIG_RETR,
    GET_CONFIG_STK,
    GET_CONFIG_COM_GW,
    GET_CONFIG_ADDITIONAL,
    GET_CONFIG_EXTERNAL,
    GET_CONFIG_NETWORK_STATUS,
    GET_CONFIG_REPORT,
    GET_CONFIG_COUNT
}ConfigGroups;

/******************************************************************************
 * Macros
 *****************************************************************************/

/* Validate port number */
#define VALIDATE_PORT(p) ( (p >= MSA_CFG_MIN_PORT) && ( p <= MSA_CFG_MAX_PORT))

/******************************************************************************
 * Constants 
 *****************************************************************************/

static char configRegistryGroup[GET_CONFIG_COUNT][30] =
{
    "",
	MSA_REG_PATH_ALIGNMENT,
    MMS_REG_PATH_ADDRESS,
    MMS_REG_PATH_COM_MMSC,
    MMS_REG_PATH_IMMEDIATE_RETRIEVAL,
    MMS_REG_PATH_RETRIEVAL,
    MMS_REG_PATH_COM_STK,
    MMS_REG_PATH_COM_GW,
    MSA_REG_PATH_MMS,
    MSA_REG_PATH_EXT,
    MMS_REG_PATH_NETWORK_STATUS,
    MMS_REG_PATH_REPORT
};

static const MsaPrsKeyTable prsKeys[] = MSA_PRS_KEYS;

/******************************************************************************
 * Global variables 
 *****************************************************************************/


static MsaConfig config; /*!<Global configuration data  */
static MsaConfInstance *confInstance; /*!< Global instance */

static we_scheme_t suppSchemes[] = {we_scheme_http,
    we_scheme_https, (we_scheme_t)0}; /*MSA_SUPPORTED_COMM_SCHEMES*/

/******************************************************************************
 * Prototypes
 *****************************************************************************/

static int  getNetworkAccountId(int accountIdx);
static int  getNetworkAccountIndex(int accountId);
static void handleGetRegRsp(MsaConfInstance *inst, MsaConfig *conf, 
                            we_registry_response_t *reg);
static void msaClearConfig(MsaConfig *conf);
static void msaConfig(MsaSignal *sig);
static void readRegistry(void);
static void getNetworkStatus(MsaConfInstance *inst, MsaStateMachine fsm, 
    int signal);
static int  saveSettings(const MsaConfig *tmpConfig, MsaConfigGroups group);
static WE_BOOL saveNetworkSettings(const MsaConfig *tmpConfig, 
                                    MsaConfigNetwork group);
static int saveImRetrSettings(const MsaConfig *tmpConfig, MsaConfigFilter group);
static int saveMsgSendSettings(const MsaConfig *tmpConfig,MsaConfigSend group);
static int saveMsgReplyChargingSettings(const MsaConfig *tmpConfig,
                                        MsaConfigReplyCharging group);
static WE_BOOL setParamInt(const we_registry_param_t *param, const char *key, 
    int *dest);
static WE_BOOL setParamStr(const we_registry_param_t *param, const char *key, 
    char **dest);
static void setRegStr(char *str, void *handle, const char *key);
static void setDstStr(char **dst, char *src);
static WE_BOOL showErrorDlg(const char *str);
static void deleteConfInstance(MsaConfInstance **instance);
static WE_BOOL saveRetrievalSettings(const MsaConfig *tmpConfig, 
    MsaConfigRetrieve item);

/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!
 * \brief Initiates the MSA Config FSM by registering a signalhandler
 *****************************************************************************/
void msaConfInit(void)
{
    msaClearConfig(msaGetConfig());
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, "msaConfigInit()\n"));
    msaSignalRegisterDst(MSA_CONFIG_FSM, msaConfig); /* set sig-handler */
}

/*!
 * \brief Terminates the MSA Config FSM
 *****************************************************************************/
void msaConfTerminate(void)
{
    msaSignalDeregister(MSA_CONFIG_FSM); /* remove sig-handler */
    deleteConfInstance(&confInstance);
    msaClearConfig(msaGetConfig());
    WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, "msaConfTerminate()\n"));
}

/*!
 * \brief Initiates an instance
 * \param The instance to initiate
 *****************************************************************************/
static void initConfInstance(MsaConfInstance **instance)
{
    *instance = MSA_ALLOC(sizeof(MsaConfInstance));
    /* Clear the structure */
    memset(*instance, 0, sizeof(MsaConfInstance));
    (*instance)->state = MSA_CONF_S_UNINITIALIZED;
}

/*!
 * \brief Deletes an instance
 * \param The instance to delete
 *****************************************************************************/
static void deleteConfInstance(MsaConfInstance **instance)
{
    MsaConfInstance *inst;
    
    if (*instance != NULL)
    {
        inst = *instance;

        MSA_FREE(inst);
        *instance = NULL;
    }
}

/*!
 * \brief Handles the response when getting configuration from the registry.
 *
 * \param conf The configuration struct to add the received settings to. 
 * \param req  The registry response from WE registry.
 *
 *****************************************************************************/

static void handleGetRegRsp(MsaConfInstance *inst, MsaConfig *conf, 
                            we_registry_response_t *reg)
{
    we_registry_param_t  param;
    int stkConnectionType;
    void *setHandle;
    int tmpInt;
    WE_BOOL valueSet;
    
    switch(reg->wid)
    {
	case GET_CONFIG_ALIGNMENT:
		while (WE_REGISTRY_RESPONSE_GET_NEXT(reg, &param)) 
        {
			if (!we_cmmn_strcmp_nc(param.path, MSA_REG_PATH_ALIGNMENT) && 
                !we_cmmn_strcmp_nc(param.key, MSA_REG_KEY_TEXT_ALIGNMENT) )
            {
				/* Write the new alignment value to registry */
                    setHandle = WE_REGISTRY_SET_INIT(WE_MODID_MSA);
                    WE_REGISTRY_SET_PATH(setHandle, (char *)MSA_REG_PATH_ALIGNMENT);
                    WE_REGISTRY_SET_ADD_KEY_INT(setHandle, 
                        (char *)MSA_REG_KEY_TEXT_ALIGNMENT, param.value_i);
                    WE_REGISTRY_SET_COMMIT(setHandle);    
				/* Save the new value in the config variables */	
                conf->alignment = (MsaTextAlignment)param.value_i;
                continue;
            }
        }
        break;

    case GET_CONFIG_UNDEFINED:
		/* CR 17749 Someone outside the MSA application has updated the registry */
        while (WE_REGISTRY_RESPONSE_GET_NEXT(reg, &param)) 
        {
			if (!we_cmmn_strcmp_nc(param.path, MSA_REG_PATH_ALIGNMENT) && 
                !we_cmmn_strcmp_nc(param.key, MSA_REG_KEY_TEXT_ALIGNMENT) )
            {
				/* Write the new alignment value to registry */
                    setHandle = WE_REGISTRY_SET_INIT(WE_MODID_MSA);
                    WE_REGISTRY_SET_PATH(setHandle, (char *)MSA_REG_PATH_ALIGNMENT);
                    WE_REGISTRY_SET_ADD_KEY_INT(setHandle, 
                        (char *)MSA_REG_KEY_TEXT_ALIGNMENT, param.value_i);
                    WE_REGISTRY_SET_COMMIT(setHandle);    
				/* Save the new value in the config variables */	
                conf->alignment = (MsaTextAlignment)param.value_i;
                continue;
            }

            /* CR12706. Manually updating MMSC version in MSA registry. */
            if (!we_cmmn_strcmp_nc(param.path, MSA_REG_PATH_MMS) && 
                !we_cmmn_strcmp_nc(param.key, MSA_REG_KEY_MMSC_VERSION)) 
            {
                if (MMS_VERSION_10 == param.value_i ||
                    MMS_VERSION_11 == param.value_i) 
                {
                    /* Write the new version value to registry */
                    setHandle = WE_REGISTRY_SET_INIT(WE_MODID_MSA);
                    WE_REGISTRY_SET_PATH(setHandle, (char *)MMS_REG_PATH_COM_MMSC);
                    WE_REGISTRY_SET_ADD_KEY_INT(setHandle, 
                        (char *)MMS_REG_KEY_PROXY_RELAY_VERSION, param.value_i);
                    WE_REGISTRY_SET_COMMIT(setHandle);        
                    /* Save the new value in the config variables */
                    conf->proxyVersion = (MmsVersion)param.value_i;
                }
                continue;
            }
        }
        break;
    case GET_CONFIG_ADDRESS:
        /* Address */
        while (WE_REGISTRY_RESPONSE_GET_NEXT(reg, &param)) 
        {
            if (setParamInt(&param, MMS_REG_KEY_FROM_ADDRESS_TYPE, &tmpInt))
            {
                conf->addressType = (MmsAddressType)tmpInt;
                continue;
            }
            if (setParamStr(&param, MMS_REG_KEY_FROM_NAME, 
                    &(conf->fromName)))
            {
                continue;
            }
        }
        break;
    case GET_CONFIG_IM_RETR:
        valueSet = FALSE;
        while (WE_REGISTRY_RESPONSE_GET_NEXT(reg, &param)) 
        {
            if (setParamInt(&param, MMS_REG_KEY_IMMEDIATE_RETRIEVAL, &tmpInt))
            {
                conf->retrievalMode  = (TRUE == tmpInt)
                    ? MSA_RETRIEVAL_MODE_AUTOMATIC
                    : MSA_RETRIEVAL_MODE_ALWAYS_ASK;
                valueSet = TRUE;
                continue;
            }
            if (setParamInt(&param, MMS_REG_KEY_MAX_SIZE_OF_MSG, &tmpInt))
            {
                conf->maxMsgSize = (unsigned int)tmpInt;
                continue;
            }
            if (setParamInt(&param, MMS_REG_KEY_CLASS_PERSONAL, &tmpInt))
            {
                conf->allowPersonal = tmpInt;
                continue;
            }
            if (setParamInt(&param, MMS_REG_KEY_CLASS_ADVERTISEMENT, &tmpInt))
            {
                conf->allowAdvert = tmpInt;
                continue;
            }
            if (setParamInt(&param, MMS_REG_KEY_CLASS_INFORMATIONAL, &tmpInt))
            {
                conf->allowInfo = tmpInt;
                continue;
            }
            if (setParamStr(&param, MMS_REG_KEY_CLASS_STRING, 
                &(conf->allowString)))
            {
                continue;
            }
            if (setParamStr(&param, MMS_REG_KEY_IMMEDIATE_RETRIEVAL_SERVER, 
                &(conf->imServer)))
            {
                continue;
            }
            if (setParamInt(&param, MMS_REG_KEY_ANONYMOUS_SENDER, &tmpInt))
            {
                conf->allowAnonym = tmpInt;
                continue;
            }
            if (setParamInt(&param, MMS_REG_KEY_TREAT_AS_DELAYED_RETRIEVAL, 
                &tmpInt))
            {
                conf->treatAsDel = tmpInt;
                continue;
            }            
        }
        if (!valueSet)
        {
            /* must initialize MSA and registry with default setting */
            msaGetConfig()->retrievalMode = MSA_CFG_RETRIEVAL_MODE;
            (void)saveRetrievalSettings(msaGetConfig(), MSA_CONFIG_HOME_NETWORK);
        }
        break;
    case GET_CONFIG_RETR:
        valueSet = FALSE;
        while (WE_REGISTRY_RESPONSE_GET_NEXT(reg, &param)) 
        {
            if (setParamInt(&param, MMS_REG_KEY_MAX_RETRIEVAL_SIZE, &tmpInt))
            {
                conf->maxRetrSize = (unsigned int)tmpInt;
                continue;
            }
            if (setParamInt(&param, MMS_REG_KEY_RETRIEVAL_ROAM_MODE, &tmpInt))
            {
                conf->retrievalModeRoam = (MMS_ROAM_DEFAULT == tmpInt)
                    ? MSA_RETRIEVAL_MODE_ROAMING_AUTOMATIC
                    : (MMS_ROAM_DELAYED == tmpInt)
                    ? MSA_RETRIEVAL_MODE_ROAMING_ALWAYS_ASK
                    : MSA_RETRIEVAL_MODE_ROAMING_RESTRICTED;
                valueSet = TRUE;
                continue;
            }
        }
        if (!valueSet)
        {
            /* must initialize MSA and registry with default setting */
            msaGetConfig()->retrievalModeRoam = 
                MSA_CFG_RETRIEVAL_MODE_ROAMING;
            (void)saveRetrievalSettings(msaGetConfig(), MSA_CONFIG_ROAM_NETWORK);
        }

        break;
    case GET_CONFIG_MMSC:
        while (WE_REGISTRY_RESPONSE_GET_NEXT(reg, &param)) 
        {
            if (setParamStr(&param, MMS_REG_KEY_PROXY_RELAY, 
                    &(conf->proxyHost)))
            {
                continue;
            }
            if (setParamStr(&param, MMS_REG_KEY_POST_URI, 
                    &(conf->proxyUri)))
            {
                continue;
            }
            if (setParamInt(&param, MMS_REG_KEY_PORT, &tmpInt))
            {
                conf->proxyPort = (unsigned int)tmpInt;
                continue;
            }
            if (setParamInt(&param, MMS_REG_KEY_PROXY_RELAY_VERSION, &tmpInt))
            {
                conf->proxyVersion = (MmsVersion)tmpInt;
                continue;
            }
            if (setParamStr(&param, MMS_REG_KEY_PROXY_RELAY_SCHEME, 
                    &(conf->proxyScheme)))
            {
                continue;
            }
        }
        break;
    case GET_CONFIG_STK:
        while (WE_REGISTRY_RESPONSE_GET_NEXT(reg, &param)) 
        {

            if (setParamInt(&param, MMS_REG_KEY_NETWORK_ACCOUNT, &tmpInt))
            {
                conf->networkAccount= tmpInt;
                continue;
            }
            if (setParamInt(&param, MMS_REG_KEY_STK_CONNECTION_TYPE, &tmpInt))
            {
                stkConnectionType = tmpInt;
                /* Convert connection type */
                switch (stkConnectionType)
                {
                case STK_CONNECTION_TYPE_HTTP:
                    conf->connectionType = MSA_CONNECTION_TYPE_HTTP;
                    break;
                case STK_CONNECTION_TYPE_WSP_CO:
                    conf->connectionType = MSA_CONNECTION_TYPE_WSP_CO;
                    break;
                case STK_CONNECTION_TYPE_WSP_CL:
                    conf->connectionType = MSA_CONNECTION_TYPE_WSP_CL;
                    break;
                }
                continue;
            }
        }
        break;
    case GET_CONFIG_COM_GW:
        while (WE_REGISTRY_RESPONSE_GET_NEXT(reg, &param)) 
        {
            if (setParamStr(&param, MMS_REG_KEY_GW_ADDRESS, 
                    &(conf->wapGateway)))
            {
                continue;
            }
            if (setParamStr(&param, MMS_REG_KEY_GW_USERNAME, 
                    &(conf->login)))
            {
                continue;
            }
            if (setParamStr(&param, MMS_REG_KEY_GW_PASSWORD, 
                    &(conf->passwd)))
            {
                continue;
            }
            if (setParamStr(&param, MMS_REG_KEY_GW_REALM, 
                    &(conf->realm)))
            {
                continue;
            }
            if (setParamInt(&param, MMS_REG_KEY_GW_SECURE_PORT, &tmpInt))
            {
                conf->securePort= (unsigned int)tmpInt;
                continue;
            }
            if (setParamInt(&param, MMS_REG_KEY_GW_HTTP_PORT, &tmpInt))
            {
                conf->httpPort = (unsigned int)tmpInt;
                continue;
            }
        }
        break;
    case GET_CONFIG_ADDITIONAL:
        while (WE_REGISTRY_RESPONSE_GET_NEXT(reg, &param)) 
        {
            if (setParamInt(&param, MSA_REG_KEY_SAVE_ON_SEND, &tmpInt))
            {
                conf->saveOnSend = tmpInt;
                continue;
            }
            if (setParamInt(&param, MSA_REG_KEY_PRIORITY, &tmpInt))
            {
                conf->priority = (MsaPriorityType)tmpInt;
                continue;
            }
            if (setParamInt(&param, MSA_REG_KEY_EXP_TIME, &tmpInt))
            {
                conf->expiryTime = (MsaExpiryTimeType)tmpInt;
                continue;
            }
            if (setParamInt(&param, MSA_REG_KEY_READ_REPORT, &tmpInt))
            {
                conf->readReport = tmpInt;
                continue;
            }
            if (setParamInt(&param, MSA_REG_KEY_DELIVERY_REPORT, &tmpInt))
            {
                conf->deliveryReport = tmpInt;
                continue;
            }
            if (setParamInt(&param, MSA_REG_KEY_SLIDE_DUR, &tmpInt))
            {
                conf->slideDuration= tmpInt;
                continue;
            }
            if (setParamInt(&param, MSA_REG_KEY_USE_SIGNATURE, &tmpInt))
            {
                conf->useSignature = tmpInt;
                continue;
            }
            if (setParamStr(&param, MSA_REG_KEY_SIGNATURE, &(conf->signature)))
            {
                continue;
            }
            if (setParamInt(&param, MSA_REG_KEY_REPLY_CHARGING, &tmpInt))
            {
                conf->replyCharging = (MsaReplyChargingType)tmpInt;
                continue;
            }
            if (setParamInt(&param, MSA_REG_KEY_REPLY_CHARGING_DEADLINE,
                &tmpInt))
            {
                conf->replyChargingDeadline = (MsaReplyChargingDeadlineType)tmpInt;
            }
            if (setParamInt(&param, MSA_REG_KEY_REPLY_CHARGING_SIZE,
                &tmpInt))
            {
                conf->replyChargingSize = (MsaReplyChargingSizeType)tmpInt;
            }

            if (setParamInt(&param, MSA_REG_KEY_SENDER_VISIBILITY, &tmpInt))
            {
                conf->senderVisibility = (MsaVisibilityType)tmpInt;
            }

            if (setParamInt(&param, MSA_REG_KEY_SEND_READ_REPORT, &tmpInt))
            {
                conf->sendReadReport = tmpInt;
            }

            if (setParamInt(&param, MSA_REG_KEY_DELIVERY_TIME, &tmpInt))
            {
                conf->deliveryTime = tmpInt;
            }

        }
        break;
    case GET_CONFIG_EXTERNAL:
        while (WE_REGISTRY_RESPONSE_GET_NEXT(reg, &param)) 
        {
            if (!we_cmmn_strcmp_nc(param.key, MSA_REG_KEY_MSISDN))
            {
                if (param.value_bv_length > 0)
                {
                    /* There was a value in the registry => Use it */
                    MSA_FREE(conf->fromAddress);
                    conf->fromAddress = MSA_ALLOC(param.value_bv_length + 1);
                    (void)strcpy(conf->fromAddress, 
                        (const char *)param.value_bv);
                    /*setHandle = WE_REGISTRY_SET_INIT(WE_MODID_MSA);
                    WE_REGISTRY_SET_PATH(setHandle, 
                        (char *)MMS_REG_PATH_ADDRESS);
                    setRegStr(conf->fromAddress, setHandle, 
                        (char *)MMS_REG_KEY_FROM_ADDRESS);
                    WE_REGISTRY_SET_COMMIT(setHandle);  */                                                                      
                }
                continue;
            }
        }
        /* Check if we got any values from the registry */
        if (!conf->fromAddress)
        {
            /* There was no value in the registry => Use default */
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
                "(%s) (%d) WARNING!!! MSISDN was not set in registry!\n", 
                __FILE__, __LINE__));
            conf->fromAddress = MSA_ALLOC(strlen(MSA_CFG_MSISDN) + 1);
            (void)strcpy(conf->fromAddress, MSA_CFG_MSISDN);
            /*setHandle = WE_REGISTRY_SET_INIT(WE_MODID_MSA);
            WE_REGISTRY_SET_PATH(setHandle, (char *)MMS_REG_PATH_ADDRESS);
            setRegStr(conf->fromAddress, setHandle, 
                (char *)MMS_REG_KEY_FROM_ADDRESS);
            WE_REGISTRY_SET_COMMIT(setHandle);*/                                
        }
        break;

    case GET_CONFIG_NETWORK_STATUS:
        while (WE_REGISTRY_RESPONSE_GET_NEXT(reg, &param))
        {
            if (setParamInt(&param, MMS_REG_KEY_NETWORK_STATUS_BEARER, &tmpInt))
            {
                conf->bearerStatus = tmpInt;
                continue;
            }
            if (setParamInt(&param, MMS_REG_KEY_NETWORK_STATUS_ROAMING, &tmpInt))
            {
                conf->roamingStatus = tmpInt;
                continue;
            }
        }
        /* alert any waiting fsm that network status data have been updated */
        if (inst->fsm != MSA_NOTUSED_FSM)
        {
            (void)MSA_SIGNAL_SENDTO(inst->fsm, inst->signal);
            inst->fsm = MSA_NOTUSED_FSM;
            inst->signal = 0;
        }
        break;

    case GET_CONFIG_REPORT:
        while (WE_REGISTRY_RESPONSE_GET_NEXT(reg, &param))
        {
            if (setParamInt(&param, MMS_REG_KEY_REPORT_ALLOWED, &tmpInt))
            {
                conf->deliveryReportAllowed = 
                    (MMS_DELIVERY_REPORT_ALLOWED_YES == tmpInt) ? 0: 1; /* Note that 0 is YES and 1 is NO */
            }
        }
        break;
        
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) erroneous config wid!\n", __FILE__, __LINE__));
        break;
    }
    /* Free the response data */
    WE_REGISTRY_RESPONSE_FREE(WE_MODID_MSA, reg);
}

/*!
 * \brief       Clears a configuration struct
 * \param conf  Pointer to the struct to be cleared
 *****************************************************************************/
static void msaClearConfig(MsaConfig *conf)
{
    if (NULL == conf)
    {
        return;
    }
    MSA_FREE(conf->fromAddress);
    MSA_FREE(conf->fromName);
    MSA_FREE(conf->proxyHost);
    MSA_FREE(conf->proxyUri); 
    MSA_FREE(conf->proxyScheme);
    MSA_FREE(conf->wapGateway);
    MSA_FREE(conf->login);
    MSA_FREE(conf->passwd);
    MSA_FREE(conf->realm);
    MSA_FREE(conf->allowString);
    MSA_FREE(conf->imServer);
    MSA_FREE(conf->signature);
    
    memset(conf, 0, sizeof(MsaConfig)); /* sets all pointers to NULL */
}

/*!
 * \brief Use the MSA for the schemes "mms:", "mmsto:" and "mailto:".
 *
 *****************************************************************************/
static void setContentRoutingInformation(void)
{
#ifdef MSA_CFG_CT_BROWSER_SCHEMES
    void* setHandle;
    setHandle = WE_REGISTRY_SET_INIT(WE_MODID_MSA);
    WE_REGISTRY_SET_PATH(setHandle, MSA_CR_PATH);
    WE_REGISTRY_SET_ADD_KEY_STR(setHandle, MSA_CR_SCHEME, 
        (unsigned char *)MSA_CFG_CT_BROWSER_SCHEMES,
        sizeof(MSA_CFG_CT_BROWSER_SCHEMES));
    WE_REGISTRY_SET_COMMIT(setHandle);
#endif
}

/*!
 * \brief Set initial MMS settings.
 *
 *****************************************************************************/
static void setMmsSettings(void)
{
    /*void *setHandle;
    int ver;*/

    /* Subscribe to changes */
    WE_REGISTRY_SUBSCRIBE (WE_MODID_MSA, 1, "/MSA", NULL,  TRUE);
    WE_REGISTRY_SUBSCRIBE (WE_MODID_MSA, 1, MSA_REG_PATH_EXT, 
        MSA_REG_KEY_MSISDN,  TRUE);

    /*
     *	Init non-configurable MMS registry settings.
     */
	/*TR 17258 let MMS controls the mmsc version*/

    config.addressType = MMS_PLMN;
    /*ver = MSA_CFG_MMSC_VERSION;
    setHandle = WE_REGISTRY_SET_INIT(WE_MODID_MSA);
    WE_REGISTRY_SET_PATH(setHandle, (char *)MMS_REG_PATH_COM_MMSC);*/
    /*lint -e{774} */
    /*if (ver != MMS_VERSION_10 && ver != MMS_VERSION_11)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "(%s) (%d) Erroneous MMSC version. Defaulting to 1.1!\n", 
            __FILE__, __LINE__));
        ver = MMS_VERSION_11;
    }
    WE_REGISTRY_SET_ADD_KEY_INT(setHandle, (char*)MMS_REG_KEY_PROXY_RELAY_VERSION,
        ver);
    WE_REGISTRY_SET_PATH(setHandle, (char *)MMS_REG_PATH_ADDRESS);
    WE_REGISTRY_SET_ADD_KEY_INT(setHandle, (char *)MMS_REG_KEY_FROM_ADDRESS_TYPE,
        config.addressType);
    WE_REGISTRY_SET_PATH(setHandle, (char *)MSA_REG_PATH_MMS);
    WE_REGISTRY_SET_ADD_KEY_INT(setHandle, (char *)MSA_REG_KEY_MMSC_VERSION,
        ver);*/

    /* Store in registry */
   /* WE_REGISTRY_SET_COMMIT(setHandle);*/
    
    /*
     *	Init configurable settings.
     */
	config.alignment        = 1; /*MSA_CFG_TEXT_ALIGNMENT;*/
    config.deliveryReport   = (WE_BOOL)MSA_CFG_DELIVERY_REPORT;
    config.expiryTime       = MSA_CFG_EXPIRY_TIME;
    config.priority         = MSA_CFG_PRIORITY;
    config.readReport       = (WE_BOOL)MSA_CFG_READ_REPORT;
    config.saveOnSend       = MSA_CFG_SAVE_ON_SEND;
    config.slideDuration    = MSA_CFG_DEFAULT_DURATION/1000;
    config.allowPersonal    = MSA_CFG_ALLOW_PERSONAL;
    config.allowAdvert      = MSA_CFG_ALLOW_ADVERT;
    config.allowInfo        = MSA_CFG_ALLOW_INFO;
    config.allowAuto        = MSA_CFG_ALLOW_AUTO; 
    config.allowAnonym      = MSA_CFG_ALLOW_ANONYM;
    config.treatAsDel       = MSA_CFG_TREAT_AS_DELAYED;
    config.useSignature     = MSA_CONFIG_SIGNATURE;
    config.signature        = NULL;
    config.replyCharging    = MSA_CFG_REPLY_CHARGING;
    config.replyChargingDeadline = MSA_CFG_REPLY_CHARGING_DEADLINE;
    config.replyChargingSize = MSA_CFG_REPLY_CHARGING_SIZE;
    config.senderVisibility = MSA_CFG_SHOW_NUMBER;
    config.deliveryReportAllowed = MSA_CFG_DELIVERY_REPORT_ALLOWED;
    config.sendReadReport   = MSA_CFG_SEND_READ_REPORT;
    config.deliveryTime     = MSA_CFG_DELIVERY_TIME;

    config.allowString = (char*)MSA_ALLOC(strlen(MSA_CFG_ALLOW_STRING) + 1);
    (void)strcpy(config.allowString, MSA_CFG_ALLOW_STRING);

    config.imServer = (char*)MSA_ALLOC(strlen(MSA_CFG_IM_SERVER) + 1);
    (void)strcpy(config.imServer, MSA_CFG_IM_SERVER);

    config.maxMsgSize = MSA_CFG_MAX_MSG_SIZE;
    config.maxRetrSize = MSA_CFG_MIN_RETR_SIZE;

    config.roamingStatus = MSA_CFG_DEFAULT_ROAMING_STATUS;
    config.bearerStatus = MSA_CFG_DEFAULT_BEARER_STATUS;
}

/*!
 * \brief MSA Config FSM signal handler
 * \param sig The incoming signal
 ******************************************************************************/
static void msaConfig(MsaSignal *sig)
{
    static int getConfigCount; /* counter for get registry operations */
    
    if (NULL == sig)
    {
        msaPanic(TRUE);
        return;
    }
    
    switch (sig->type)
    {
    case MSA_SIG_CONF_ACTIVATE:
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "msaConfig(): GUI activation\n"));
        
        (void)msaConfigActivateMenu(MSA_CONFIG_MENU_DEFAULT, 
            (int)MSA_CONFIG_MENU_DEFAULT, NULL);
        break;
        
    case MSA_SIG_CONF_ACTIVATE_SETTINGS:
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "msaConfig(): GUI settings activation\n"));
        (void)msaConfigActivateMenu(MSA_CONFIG_MENU_MMS_SETTINGS,
            MSA_CONFIG_SENDING, (MsaPropItem*)sig->p_param);
        break;

    case MSA_SIG_CONF_INITIAL_ACTIVATE:
        /* init instance */
        initConfInstance(&confInstance);

        getConfigCount = 0;
        /* Init MMS settings */
        setMmsSettings();
        /* Tell the FRW that the MSA handles mmsto and mailto schemes */
        setContentRoutingInformation();

        /* Read configuration from registry */
        readRegistry();
        break;

    case MSA_SIG_CONF_DEACTIVATE:
        if (MSA_STARTUP_MODE_CONFIG == msaGetStartupMode()) 
        {
            /*
             *	If we started MSA in config mode we request to close down MSA 
             *  new and let all running FSMs run their work first.
             */
            (void)MSA_SIGNAL_SENDTO(MSA_CORE_FSM, MSA_SIG_CORE_TERMINATE_APP);            
        }
        break;
        
    case MSA_SIG_CONF_GET_REG_RSP:
        handleGetRegRsp(confInstance, msaGetConfig(), 
            (we_registry_response_t*)sig->p_param);
        ++getConfigCount;
        /*  Start the GUI for the application when the configuration has been 
         *  received. The -1 is because the first enum field is not used
         *  for getting configuration values
         */
        if (MSA_CONF_S_UNINITIALIZED == confInstance->state &&
            getConfigCount >= (GET_CONFIG_COUNT - 1))
        {
            msaActivateStartupFsm();
            confInstance->state = MSA_CONF_S_INITIALIZED;
        }
        break;

    case MSA_SIG_CONF_GET_NETWORK_STATUS:
        /* Refresh network status variables */
        getNetworkStatus(confInstance, (MsaStateMachine)sig->u_param1, 
            (int)sig->u_param2);
        break;
        
    default :
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "msaConfig(): Mysterious signal recieved"));
        break;
    } 
    /* Delete the signal */
    msaSignalDelete(sig);
}

/*!
 * \brief  Callback for changing setting of SaveOnSend-flag. This function can 
 *         handle both global and temporary setting.
 * \param  str N/A.
 * \param  value The new save on send setting.
 * \param  p1 A pointer to the configuration data to change.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaSaveOnSendCallback(char *str, int value, void *p1, int p2)
{
    if (str || p1 || p2) 
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) str is not NULL\n", __FILE__, 
            __LINE__));
    }
    if (value != 0 && value != 1) 
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "msaSaveOnSendCallback(): Impossible value (%d) received", value));
        /* This should not happen since we restrict the value in the form */
        return FALSE;
    }
    config.saveOnSend = value;
    if (!saveMsgSendSettings(msaGetConfig(), MSA_CONFIG_SAVE_ON_SEND))
    {
        return FALSE;
    }
    return TRUE;
}

/*!
 * \brief  Callback for changing setting of validity period a.k.a. expiry time
 *         This function can handle both global and temporary setting.
 * \param  str N/A.
 * \param  value The new expiry time setting.
 * \param  p1 A pointer to the configuration data to change. NULL if global
 *         configuration data should be used.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818}*/
WE_BOOL msaExpTimeCallback(char *str, int value, void *p1, int p2)
{
    MsaConfig *tmpConfig;
    MsaPropItem *msgProp;

    if (str || p2) 
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) str is not NULL\n", __FILE__, 
            __LINE__));
    }
    if (value < MSA_EXPIRY_1_HOUR || value > MSA_EXPIRY_MAX) 
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "msaExpTimeCallback(): Impossible value (%d) received", value));
        /* This should not happen since we restrict the value in the form */
        return FALSE;
    }
    msgProp = (MsaPropItem*)p1;
    if (!msgProp)
    {
        tmpConfig = msaGetConfig();
        tmpConfig->expiryTime = (MsaExpiryTimeType)value;
        if (!saveMsgSendSettings(tmpConfig, MSA_CONFIG_EXP_TIME))
        {
            return FALSE;
        }
    }
    else
    {
        msgProp->expiryTime = (MsaExpiryTimeType)value;
    }
    return TRUE;
}

/*!
 * \brief  Callback for changing setting of read report. This function can 
 *         handle both global and temporary setting.
 * \param  str N/A.
 * \param  value The new read report setting.
 * \param  p1 A pointer to the configuration data to change. NULL if global
 *         configuration data should be used.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaReadReportCallback(char *str, int value, void *p1, int p2)
{
    MsaConfig *tmpConfig;
    MsaPropItem *msgProp;
    
    if (str || p2) 
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) str is not NULL\n", __FILE__, 
            __LINE__));
    }
    if (value != 0 && value != 1) 
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "msaReadReportCallback(): Impossible value (%d) received", value));
        /* This should not happen since we restrict the value in the form */
        return FALSE;
    }
    msgProp = (MsaPropItem*)p1;
    if (!msgProp)
    {
        tmpConfig = msaGetConfig();
        tmpConfig->readReport = value;
        if (!saveMsgSendSettings(msaGetConfig(), MSA_CONFIG_READ_REPORT))
        {
            return FALSE;
        }
    }
    else
    {
        msgProp->readReport = value;
    }
    return TRUE;
}

/*!
 * \brief  Callback for changing setting of delivery report. This function can 
 *         handle both global and temporary setting.
 * \param  str N/A.
 * \param  value The new delivery report setting.
 * \param  p1 A pointer to the configuration data to change. NULL if global
 *         configuration should be used.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaDelivReportCallback(char *str, int value, void *p1, int p2)
{
    MsaConfig *tmpConfig;
    MsaPropItem *msgProp;
    
    if (str || p2) 
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) str is not NULL\n", __FILE__, 
            __LINE__));
    }
    if (value != 0 && value != 1) 
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
           "msaDelivReportCallback(): Impossible value (%d) received", value));
        /* This should not happen since we restrict the value in the form */
        return FALSE;
    }
    msgProp = (MsaPropItem*)p1;
    if (!msgProp)
    {
        tmpConfig = msaGetConfig();
        tmpConfig->deliveryReport = value;
        if (!saveMsgSendSettings(msaGetConfig(), MSA_CONFIG_DELIV_REPORT))
        {
            return FALSE;
        }
    }
    else
    {
        msgProp->deliveryReport = value;        
    }
    return TRUE;
}

/*!
 * \brief  Callback for changing setting of priority. This function can handle
 *         both global and temporary setting.
 * \param  str N/A.
 * \param  value The new priority setting.
 * \param  p1 A pointer to the configuration data to change. NULL if global
 *         configuration should be used.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaPriorityCallback(char *str, int value, void *p1, int p2)
{
    MsaConfig *tmpConfig;
    MsaPropItem *msgProp;
    
    if (str || p2) 
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) str is not NULL\n", __FILE__, 
            __LINE__));
    }
    if (value < MSA_PRIORITY_LOW || value > MSA_PRIORITY_HIGH) 
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
           "msaPriorityCallback(): Impossible value (%d) received", value));
        /* This should not happen since we restrict the value in the form */
        return FALSE;
    }
    msgProp = (MsaPropItem*)p1;
    if (!msgProp)
    {
        tmpConfig = msaGetConfig();
        tmpConfig->priority = (MsaPriorityType)value;
        if (!saveMsgSendSettings(msaGetConfig(), MSA_CONFIG_PRIORITY))
        {
            return FALSE;
        }
    }
    else
    {
        msgProp->priority = (MsaPriorityType)value;
        
    }

    return TRUE;
}

/*!
 * \brief  Callback for changing setting of slide duration
 * \param  str The new slide duration.
 * \param  value N/A.
 * \param  p1 A pointer to the configuration data to change.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaSlideDurCallback(char *str, int value, void *p1, int p2)
{
    int tmpVal;
    MsaConfig *tmpConfig;

    if (0 != value || p2) 
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) value is not 0\n", __FILE__, 
            __LINE__));
    }
    tmpVal = atoi(str);
    /* Check boundaries */
    if (tmpVal <= 0 || tmpVal > MSA_CFG_MAX_DURATION) 
    {
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_INVALID_VALUE), 
            MSA_DIALOG_ERROR);
        /*lint -e{774} */
        MSA_FREE(str);
        return FALSE;
    }
    tmpConfig = (MsaConfig*)p1;
    if (!tmpConfig)
    {
        tmpConfig = msaGetConfig();
    }
    tmpConfig->slideDuration = tmpVal;
    if (!p1)
    {
        if (!saveMsgSendSettings(tmpConfig, MSA_CONFIG_SLIDE_DUR))
        {
            return FALSE;
        }
    }
    /*lint -e{774} */
    MSA_FREE(str);
    return TRUE;
}

/*!
 * \brief  Callback for changing setting of use signature.
 * \param  str N/A.
 * \param  value The new use signature setting.
 * \param  p1 N/A.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
WE_BOOL msaUseSignatureCallback(char *str, int value, void *p1, int p2)
{
    MsaConfig *tmpConfig;

    /*Just to remove warnings*/
    p2 = p2;
    p1 = p1;
    str = str;

    tmpConfig = msaGetConfig();
    if (tmpConfig)
    {
        if (1 == value || 0 == value)
        {
            tmpConfig->useSignature = value;
            if (!saveMsgSendSettings(msaGetConfig(), MSA_CONFIG_USE_SIGNATURE_MENU))
            {
                return FALSE;
            }
            return TRUE;
        }
    }
    return FALSE;
}


/*!
 * \brief  Callback for changing setting of signature.
 * \param  str The new signature.
 * \param  value N/A.
 * \param  p1 N/A.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
WE_BOOL msaEditSignatureCallback(char *str, int value, void *p1, int p2)
{
    MsaConfig *tmpConfig;

    /*Just to remove warnings*/
    p2 = p2;
    p1 = p1;
    value = value;

    tmpConfig = msaGetConfig();
    if (tmpConfig)
    {
        if (str)
        {
            tmpConfig->signature = str;
            if (!saveMsgSendSettings(msaGetConfig(), MSA_CONFIG_SIGNATURE_MENU))
            {
                return FALSE;
            }
            return TRUE;
        }
    }
    return FALSE;
}

/*!
 * \brief  Callback for changing setting of reply charging. This function can 
 *         handle both global and temporary setting.
 * \param  str N/A.
 * \param  value The new reply charging setting.
 * \param  p1 A pointer to the configuration data to change. NULL if global
 *         configuration should be used.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaReplyChargingCallback(char *str, int value, void *p1, int p2)
{
    MsaConfig *tmpConfig;
    MsaPropItem *msgProp;
    
    if (str || p2) 
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) str is not NULL\n", __FILE__, 
            __LINE__));
    }
    if (value < MSA_REPLY_CHARGING_OFF || value > MSA_REPLY_CHARGING_TEXT_ONLY) 
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
           "msaReplyChargingCallback(): Impossible value (%d) received", value));
        /* This should not happen since we restrict the value in the form */
        return FALSE;
    }
    msgProp = (MsaPropItem*)p1;
    if (!msgProp) 
    {
        tmpConfig = msaGetConfig();
        tmpConfig->replyCharging = (MsaReplyChargingType)value;
        if (!saveMsgReplyChargingSettings(msaGetConfig(),
            MSA_CONFIG_REPLY_CHARGING))
        {
            return FALSE;
        }
    }
    else
    {
        msgProp->replyCharging = (MsaReplyChargingType)value;        
    }

    return TRUE;
}

/*!
 * \brief  Callback for changing setting of reply charging. This function can 
 *         handle both global and temporary setting.
 * \param  str N/A.
 * \param  value The new reply charging deadline setting.
 * \param  p1 A pointer to the configuration data to change. NULL if global
 *         configuration should be used.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaReplyChargingDeadlineCallback(char *str, int value, void *p1, int p2)
{
    MsaConfig *tmpConfig;
    MsaPropItem *msgProp;
    
    if (str || p2) 
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) str is not NULL\n", __FILE__, 
            __LINE__));
    }
    if (value < MSA_REPLY_CHARGING_DEADLINE_1_DAY || 
        value > MSA_REPLY_CHARGING_DEADLINE_MAX)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
           "msaReplyChargingDeadlineCallback(): Impossible value (%d) received",
           value));
        return FALSE;
    }
    msgProp = (MsaPropItem*)p1;
    if (!msgProp) 
    {
        tmpConfig = msaGetConfig();
        tmpConfig->replyChargingDeadline = (MsaReplyChargingDeadlineType)value;
        if (!saveMsgReplyChargingSettings(msaGetConfig(),
            MSA_CONFIG_REPLY_CHARGING_DEADLINE))
        {
            return FALSE;
        }
    }
    else
    {
        msgProp->replyChargingDeadline = (MsaReplyChargingDeadlineType)value;        
    }

    return TRUE;
}

/*!
 * \brief  Callback for changing setting of reply charging. This function can 
 *         handle both global and temporary setting.
 * \param  str N/A.
 * \param  value The new reply charging size setting.
 * \param  p1 A pointer to the configuration data to change. NULL if global
 *         configuration should be used.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaReplyChargingSizeCallback(char *str, int value, void *p1, int p2)
{
    MsaConfig *tmpConfig;
    MsaPropItem *msgProp;
    
    if (str || p2) 
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) str is not NULL\n", __FILE__, 
            __LINE__));
    }
    if (value < MSA_REPLY_CHARGING_SIZE_1_KB || 
        value > MSA_REPLY_CHARGING_SIZE_MAX)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
           "msaReplyChargingSizeCallback(): Impossible value (%d) received",
           value));
        return FALSE;
    }
    msgProp = (MsaPropItem*)p1;
    if (!msgProp) 
    {
        tmpConfig = msaGetConfig();
        tmpConfig->replyChargingSize = (MsaReplyChargingSizeType)value;
        if (!saveMsgReplyChargingSettings(msaGetConfig(),
            MSA_CONFIG_REPLY_CHARGING_SIZE))
        {
            return FALSE;
        }
    }
    else
    {
        msgProp->replyChargingSize = (MsaReplyChargingSizeType)value;        
    }

    return TRUE;
}

/*!
 * \brief  Callback for changing setting of retrieval home network mode. 
 * \param  str N/A.
 * \param  value The new value.
 * \param  p1 A pointer to the configuration data to change.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaRetrievalHomeModeCallback(char *str, int value, void *p1, int p2)
{
    if (str || p1 || p2)
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) str is not NULL\n", __FILE__, 
            __LINE__));
    }
    if (value != 0 && value != 1)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
         "msaRetrievalHomeModeCallback(): Impossible value (%d) received", value));
        /* This should not happen since we restrict the value in the form */
        return FALSE;
    }
    config.retrievalMode = value;
    return saveRetrievalSettings(msaGetConfig(), MSA_CONFIG_HOME_NETWORK);
}

/*!
 * \brief  Callback for changing setting of retrieval roaming network mode. 
 * \param  str N/A.
 * \param  value The new value.
 * \param  p1 A pointer to the configuration data to change.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaRetrievalRoamModeCallback(char *str, int value, void *p1, int p2)
{
    if (str || p1 || p2)
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) str is not NULL\n", __FILE__, 
            __LINE__));
    }
    if (value != 0 && value != 1 && value != 2)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
         "msaRetrievalRoamModeCallback(): Impossible value (%d) received", value));
        /* This should not happen since we restrict the value in the form */
        return FALSE;
    }
    config.retrievalModeRoam = value;
    if (!saveRetrievalSettings(msaGetConfig(), MSA_CONFIG_ROAM_NETWORK))
    {
        return FALSE;
    }
    return TRUE;
}

/*!
 * \brief  Callback for changing setting of maximum size for immediate 
 *         retrieval. 
 * \param  str The new value.
 * \param  value N/A.
 * \param  p1 N/A.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaImRetrSizeCallback(char *str, int value, void *p1, int p2)
{
    int tmpVal;
    if (value || p1 || p2)
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) str is not NULL\n", __FILE__, 
            __LINE__));
    }
    tmpVal = atoi(str);
    if (tmpVal < MSA_CFG_MIN_MSG_SIZE || tmpVal > MSA_CFG_MAX_MSG_SIZE)
    {
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_INVALID_VALUE), 
            MSA_DIALOG_ERROR);
        /*lint -e{774} */
        MSA_FREE(str);
        return FALSE;
    }
    config.maxMsgSize = (unsigned int)tmpVal;
    if (!saveImRetrSettings(msaGetConfig(), MSA_CONFIG_IM_SIZE))
    {
        return FALSE;
    }
    /*lint -e{774} */
    MSA_FREE(str);
    return TRUE;
}

/*!
 * \brief  Callback for changing setting of the treat-as-delayed flag. 
 * \param  str N/A..
 * \param  value 1=YES, 0=NO.
 * \param  p1 N/A.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaTreatAsDelCallback(char *str, int value, void *p1, int p2)
{
    if (str || p1 || p2)
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) Some values are not 0\n", __FILE__, 
            __LINE__));
    }
    if (value != 0 && value != 1)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "msaTreatAsDelCallback(): Impossible value (%d) received", value));
        /* This should not happen since we restrict the value in the form */
        return FALSE;
    }
    config.treatAsDel = value;
    if (!saveImRetrSettings(msaGetConfig(), MSA_CONFIG_TREAT_AS_DEL))
    {
        return FALSE;
    }
    return TRUE;
}

/*!
 * \brief  Callback for changing setting for allowing class ADVERTISE messages.
 * \param  str N/A.
 * \param  value 1=ALLOW, 0=REJECT.
 * \param  p1 N/A.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaAllowAdvertCallback(char *str, int value, void *p1, int p2)
{
    if (str || p1 || p2)
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) str is not NULL\n", __FILE__, 
            __LINE__));
    }
    if (value != 0 && value != 1)
    {
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_INVALID_VALUE), 
            MSA_DIALOG_ERROR);
        return FALSE;
    }
    config.allowAdvert = value;
    if (!saveImRetrSettings(msaGetConfig(), MSA_CONFIG_ALLOW_ADVERT))
    {
        return FALSE;
    }
    return TRUE;
}

/*!
 * \brief  Callback for changing setting for allowing class PERSONAL messages.
 * \param  str N/A.
 * \param  value 1=ALLOW, 0=REJECT.
 * \param  p1 N/A.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaAllowPersonalCallback(char *str, int value, void *p1, int p2)
{
    if (str || p1 || p2)
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) str is not NULL\n", __FILE__, 
            __LINE__));
    }
    if (value != 0 && value != 1)
    {
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_INVALID_VALUE), 
            MSA_DIALOG_ERROR);
        return FALSE;
    }
    config.allowPersonal = value;
    if (!saveImRetrSettings(msaGetConfig(), MSA_CONFIG_ALLOW_PERSONAL))
    {
        return FALSE;
    }
    return TRUE;
}

/*!
 * \brief  Callback for changing setting for allowing class INFO messages.
 * \param  str N/A.
 * \param  value 1=ALLOW, 0=REJECT.
 * \param  p1 N/A.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaAllowInfoCallback(char *str, int value, void *p1, int p2)
{
    if (str || p1 || p2)
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) str is not NULL\n", __FILE__, 
            __LINE__));
    }
    if (value != 0 && value != 1)
    {
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_INVALID_VALUE), 
            MSA_DIALOG_ERROR);
        return FALSE;
    }
    config.allowInfo = value;
    (void)saveImRetrSettings(msaGetConfig(), MSA_CONFIG_ALLOW_INFO);
    return TRUE;
}

/*!
 * \brief  Callback for changing setting for allowing class AUTO messages.
 * \param  str N/A.
 * \param  value 1=ALLOW, 0=REJECT.
 * \param  p1 N/A.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaAllowAutoCallback(char *str, int value, void *p1, int p2)
{
    if (str || p1 || p2)
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) str is not NULL\n", __FILE__, 
            __LINE__));
    }
    if (value != 0 && value != 1)
    {
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_INVALID_VALUE), 
            MSA_DIALOG_ERROR);
        return FALSE;
    }
    config.allowAuto = value;
    (void)saveImRetrSettings(msaGetConfig(), MSA_CONFIG_ALLOW_AUTO);
    return TRUE;
}

/*!
 * \brief  Callback for changing setting for allowing anonymous senders.
 * \param  str N/A.
 * \param  value 1=ALLOW, 0=REJECT.
 * \param  p1 N/A.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaAllowAnonymCallback(char *str, int value, void *p1, int p2)
{
    if (str || p1 || p2)
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) str is not NULL\n", __FILE__, 
            __LINE__));
    }
    if (value != 0 && value != 1)
    {
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_INVALID_VALUE), 
            MSA_DIALOG_ERROR);
        return FALSE;
    }
    config.allowAnonym = value;
    (void)saveImRetrSettings(msaGetConfig(), MSA_CONFIG_ALLOW_ANONYM);
    return TRUE;
}

/*!
 * \brief  Callback for changing setting of user defined string class 
 *         for immediate retrieval filtering.
 * \param  str The new value.
 * \param  value N/A.
 * \param  p1 N/A.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaAllowStringCallback(char *str, int value, void *p1, int p2)
{
    if (value || p1 || p2)
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) The values are not 0\n", __FILE__, 
            __LINE__));
    }
    MSA_FREE(config.allowString);
    config.allowString = str;
    (void)saveImRetrSettings(msaGetConfig(), MSA_CONFIG_ALLOW_STRING);
    return TRUE;
}

/*!
 * \brief  Callback for changing setting of immediate retrieval server. 
 * \param  str The new value.
 * \param  value N/A.
 * \param  p1 N/A.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaImServerCallback(char *str, int value, void *p1, int p2)
{
    if (value || p1 || p2)
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) str is not NULL\n", __FILE__, 
            __LINE__));
    }
    if (!we_url_is_valid(str))
    {
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_INVALID_PROXY_HOST), 
            MSA_DIALOG_ERROR);
        MSA_FREE(str);
        return FALSE;
    }
    MSA_FREE(config.imServer);
    config.imServer = str;
    (void)saveImRetrSettings(msaGetConfig(), MSA_CONFIG_IM_SERVER);
    return TRUE;
}

/*!
 * \brief  Callback for changing setting of MMSC. 
 * \param  str The new value.
 * \param  value N/A.
 * \param  p1 N/A.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaMmscCallback(char *str, int value, void *p1, int p2)
{
    char *tmpScheme, *newScheme;
    char *tmpHost;
    char *tmpPath, *newPath;
    char *tmpPort;
    int i;
    we_scheme_t tmpSchemeType;
    WE_BOOL unsupp;

    if (value || p1 || p2)
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) str is not NULL\n", __FILE__, 
            __LINE__));
    }

    if (!we_url_is_valid(str))
    {
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_INVALID_PROXY_HOST), 
            MSA_DIALOG_ERROR);
        MSA_FREE(str);
        return FALSE;
    }
    tmpSchemeType = we_url_get_scheme_type(str);
    unsupp = TRUE;
    for (i = 0; suppSchemes[i] != 0; i++)
    {
        if (suppSchemes[i] == tmpSchemeType)
        {
            unsupp = FALSE;
            break;
        }
    }
    if (unsupp)
    {
        (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_INVALID_PROXY_SCHEME), 
            MSA_DIALOG_ERROR);
        MSA_FREE(str);
        return FALSE;        
    }

    tmpPort = we_url_get_port(WE_MODID_MSA, str);
    if (tmpPort)
    {
        i = atoi(tmpPort);
        if (!VALIDATE_PORT(i))
        {
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_INVALID_PROXY_PORT), 
                MSA_DIALOG_ERROR);            
            MSA_FREE(str);
            /*lint -e{774} */
            MSA_FREE(tmpPort);
            return FALSE;
        }
    }
    
    tmpScheme = we_url_get_scheme(WE_MODID_MSA, str);
    tmpHost = we_url_get_host(WE_MODID_MSA, str);
    tmpPath = (char *)we_url_find_path((const char *)str);
    if (NULL != tmpScheme)
    {
        newScheme = MSA_ALLOC(strlen(tmpScheme) + 1 + 3);
        sprintf(newScheme, "%s://", tmpScheme);
        /*lint -e{774} */
        MSA_FREE(tmpScheme);
        setDstStr(&(config.proxyScheme), newScheme);
    }
    setDstStr(&(config.proxyHost), tmpHost);
    newPath = NULL;
    if (tmpPath)
    {
        newPath = MSA_ALLOC(strlen(tmpPath) + 1);
        strcpy(newPath, tmpPath);
        tmpPath = NULL;
    }
    setDstStr(&(config.proxyUri), newPath);
    if (NULL != tmpPort)
    {
        config.proxyPort = (unsigned int)atoi(tmpPort);
        /*lint -e{774} */
        MSA_FREE(tmpPort);
    }
    else
    {
        config.proxyPort = 0;
    }

    (void)saveSettings(msaGetConfig(), MSA_CONFIG_MMSC);
    MSA_FREE(str);
    return TRUE;
}

/*!
 * \brief  Callback for changing network settings. 
 * \param  str The new string value if applicable.
 * \param  value The new integer value if applicable.
 * \param  p1 N/A.
 * \param  p2 The selected setting. See #MsaConfigNetwork.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaNetworkCallback(char *str, int value, void *p1, int p2)
{
    MsaConfigNetwork choice;
    MsaConfig *conf;
    int tmp;

    if (p1)
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) p1 is not NULL\n", __FILE__, 
            __LINE__));
    }
    choice = (MsaConfigNetwork)p2;
    conf = msaGetConfig();
    switch (choice) 
    {
    case MSA_CONFIG_CONN_TYPE :
        if (value < MSA_CONNECTION_TYPE_HTTP || 
            value > MSA_CONNECTION_TYPE_WSP_CO) 
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
                "(%s) (%d) Impossible value (%d) received.\n", 
                __FILE__, __LINE__, value));
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_ERROR), 
                MSA_DIALOG_ERROR);            
            return FALSE;
        }
        conf->connectionType = (MsaConnectionType)value;
        break;
    case MSA_CONFIG_NET_ACCOUNT :
        if (value < 0)
        {
            WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
                "(%s) (%d) Impossible value (%d) received.\n", 
                __FILE__, __LINE__, value));
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_ERROR), 
                MSA_DIALOG_ERROR);            
            return FALSE;
        }
        conf->networkAccount = getNetworkAccountId(value);
        break;
    case MSA_CONFIG_WAP_GW :
        if (str && !we_url_is_valid(str))
        {
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_INVALID_WAP_GATEWAY),
                MSA_DIALOG_ERROR);
            return FALSE;
        }
        MSA_FREE(conf->wapGateway);
        conf->wapGateway = str;
        break;
    case MSA_CONFIG_LOGIN :
        MSA_FREE(conf->login);
        conf->login = str;
        break;
    case MSA_CONFIG_PASSWD :
        MSA_FREE(conf->passwd);
        conf->passwd = str;        
        break;
    case MSA_CONFIG_REALM :
        MSA_FREE(conf->realm);
        conf->realm = str;        
        break;
    case MSA_CONFIG_SEC_PORT :
        if (NULL != str)
        {
            tmp = atoi(str);
            if (!VALIDATE_PORT(tmp))
            {
                (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_INVALID_VALUE), 
                    MSA_DIALOG_ERROR);            
                /*lint -e{774} */
                MSA_FREE(str);
                return FALSE;
            }
        }
        conf->securePort = (unsigned int)(str ? atoi(str) : 0);
        MSA_FREE(str);
        break;
    case MSA_CONFIG_HTTP_PORT :
		if(NULL != str )
		{
			tmp = atoi(str);
			if (!VALIDATE_PORT(tmp))
			{	
				(void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_INVALID_VALUE), 
					MSA_DIALOG_ERROR);
				/*lint -e{774} */ 
				MSA_FREE(str);
				return FALSE;
			}
        }        
        conf->httpPort = (unsigned int)(str ? atoi(str) : 0);
		MSA_FREE(str);
        break;
    case MSA_CONFIG_NETWORK_COUNT:
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA, 
            "(%s) (%d) Impossible value (%d) received.\n", 
            __FILE__, __LINE__, p2));
    }
    return saveNetworkSettings(conf, choice);
}

/*!
 * \brief  Callback for changing maximum download size. 
 * \param  str The new string value if applicable.
 * \param  value The new integer value if applicable.
 * \param  p1 N/A.
 * \param  p2 The selected setting. See #MsaConfigFilter.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
 /*lint -e{818} */
WE_BOOL msaDownloadSizeCallback(char *str, int value, void *p1, int p2)
{
    int tmpVal;
    char *dlgStr;

    if (value || p1 || p2)
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) something is not NULL\n", __FILE__, 
            __LINE__));
    }
    tmpVal = atoi(str);
    /*  We don't need to check for negative values since they are restricted
     *  from the form */
    if (tmpVal > 0 && (tmpVal < MSA_CFG_MIN_RETR_SIZE 
        || tmpVal > MSA_CFG_MAX_RETR_SIZE))
    {
        dlgStr = msaGetIntervalString(MSA_STR_ID_INTERVAL_PREFIX,
            MSA_CFG_MIN_RETR_SIZE, MSA_STR_ID_INTERVAL_INFIX, 
            MSA_CFG_MAX_RETR_SIZE);
        if (!showErrorDlg(dlgStr))
        {
            /* If we were unable to create the dynamic dialog we 
             * try to show a static message */
            (void)msaShowDialog(MSA_GET_STR_ID(MSA_STR_ID_INVALID_VALUE), 
                MSA_DIALOG_ERROR);
        }
        MSA_FREE(dlgStr);
        /*lint -e{774} */
        MSA_FREE(str);
        return FALSE;
    }
    config.maxRetrSize = (unsigned int)tmpVal;
    (void)saveImRetrSettings(msaGetConfig(), MSA_CONFIG_RETR_SIZE);
    /*lint -e{774} */
    MSA_FREE(str);
    return TRUE;
}

/*!
 * \brief  Callback for changing setting of Sender visibility. 
 * \param  str N/A.
 * \param  value The new value.
 * \param  p1 A pointer to the configuration data to change.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaSenderVisibilityCallback(char *str, int value, void *p1, int p2)
{
    MsaConfig *tmpConfig;
    MsaPropItem *msgProp;

    if (str || p1 || p2)
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) str is not NULL\n", __FILE__, 
            __LINE__));
    }
    if (value != 0 && value != 1)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
         "msaSenderVisibilityCallback(): Impossible value (%d) received", value));
        /* This should not happen since we restrict the value in the form */
        return FALSE;
    }    

    msgProp = (MsaPropItem*)p1;
    if (!msgProp)
    {
        tmpConfig = msaGetConfig();
        tmpConfig->senderVisibility = (MsaVisibilityType)value;
        (void)saveMsgSendSettings(tmpConfig, MSA_CONFIG_SENDER_VISIBILITY);
    }
    else
    {
        msgProp->senderVisibility = (MsaVisibilityType)value;        
    }

    return TRUE;
}

/*!
 * \brief  Callback for changing setting for Delivery Report Allowed. 
 * \param  str N/A.
 * \param  value The new value.
 * \param  p1 A pointer to the configuration data to change.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaDeliveryReportAllowedCallback(char *str, int value, void *p1, int p2)
{
    MsaConfig *tmpConfig;

    if (str || p1 || p2)
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) str is not NULL\n", __FILE__, 
            __LINE__));
    }
    if (value != 0 && value != 1)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
         "msaDeliveryReportAllowedCallback(): Impossible value (%d) received", value));
        /* This should not happen since we restrict the value in the form */
        return FALSE;
    }    

    tmpConfig = msaGetConfig();
    tmpConfig->deliveryReportAllowed = value;
    (void)saveRetrievalSettings(tmpConfig, MSA_CONFIG_DELIVERY_REPORT_ALLOWED);

    return TRUE;
}

/*!
 * \brief  Callback for changing setting for Send Read Report. 
 * \param  str N/A.
 * \param  value The new value.
 * \param  p1 A pointer to the configuration data to change.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaSendReadReportCallback(char *str, int value, void *p1, int p2)
{
    MsaConfig *tmpConfig;

    if (str || p1 || p2)
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) str is not NULL\n", __FILE__, 
            __LINE__));
    }
    if (value != 0 && value != 1 && value != 2)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
         "msaSendReadReportCallback(): Impossible value (%d) received", value));
        /* This should not happen since we restrict the value in the form */
        return FALSE;
    }    

    tmpConfig = msaGetConfig();
    tmpConfig->sendReadReport = value;
    (void)saveRetrievalSettings(tmpConfig, MSA_CONFIG_SEND_READ_REPORT);

    return TRUE;
}

/*!
 * \brief  Callback for changing setting for Delivery Time. 
 * \param  str N/A.
 * \param  value The new value.
 * \param  p1 A pointer to the configuration data to change.
 * \param  p2 N/A.
 * \return TRUE on successful save of value, else FALSE.
 *****************************************************************************/
/*lint -e{818} */
WE_BOOL msaDeliveryTimeCallback(char *str, int value, void *p1, int p2)
{
    MsaConfig *tmpConfig;

    if (str || p1 || p2)
    {
        /* Some msaningless logging not to get warnings */
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA, 
            "(%s) (%d) str is not NULL\n", __FILE__, 
            __LINE__));
    }
    if (value != 0 && value != 1 && value != 2 && value != 3)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
         "msaDeliveryTimeCallback(): Impossible value (%d) received", value));
        /* This should not happen since we restrict the value in the form */
        return FALSE;
    }    

    tmpConfig = msaGetConfig();
    tmpConfig->deliveryTime = value;
    (void)saveMsgSendSettings(tmpConfig, MSA_CONFIG_DELIVERY_TIME);

    return TRUE;
}


/*!
 * \brief  Returns the address of the global configuration data storage struct
 * \return The address of the MsaConfig struct
 *****************************************************************************/
MsaConfig *msaGetConfig(void)
{
    return &config;
}

/*!
 * \brief  Returns the text alignment of the global configuration data storage struct
 * \return The alignment of text
 *****************************************************************************/
int msaGetAlignment(void)
{
	return config.alignment;
}

/*!
 * \brief Checks if the configuration is set so that a message can be created.
 * 
 * \return TRUE if the configuration is ok, otherwise FALSE.
 *****************************************************************************/
int msaIsCreateConfValid(void)
{
    /* Maybe we should verify from address here but how do we know
     * if we want to use insert address token instead?
     */

    /* Verify MMSC version */
    if ( (config.proxyVersion != MMS_VERSION_10) &&
         (config.proxyVersion != MMS_VERSION_11))
    {
        return FALSE;
    }
    return TRUE;
}

/*!
 * \brief Gets the network account wid for a specified menu index. Used when
 *      configurating network account from menu and writes the corresponding
 *      wid to registry.
 * \param accountIdx Which account menu index to get wid for.
 * \return Network account wid or default first wid if not found.
 *****************************************************************************/
static int getNetworkAccountId(int accountIdx)
{
    int wid;
    int index;

    /* Search for wid */
    wid = WE_NETWORK_ACCOUNT_GET_FIRST();
    for (index = 0; index < accountIdx && wid >= 0; index++)
    {
        wid = WE_NETWORK_ACCOUNT_GET_NEXT(wid);
    }
    if (wid < 0)
    {
        wid = WE_NETWORK_ACCOUNT_GET_FIRST();
    }
    return wid;
}

/*!
 * \brief Gets the network account menu index from the specified account wid.
 *      Used when creating the network account menu.
 * \param accountId Which Network account wid to get the menu index for.
 * \return The network account index or default 0 if not found.
 *****************************************************************************/
static int  getNetworkAccountIndex(int accountId)
{
    int wid;
    int index = 0;

    /* Search for wid */
    wid = WE_NETWORK_ACCOUNT_GET_FIRST();
    while (wid != accountId && wid >= 0)
    {
        wid = WE_NETWORK_ACCOUNT_GET_NEXT(wid);
        index++;
    }
    if (wid < 0)
    {
        index = 0;
    }
    return index;
}

/*!
 * \brief Reads one specific configuration group from the registry. The 
 *        configurations are received through a signal.
 * \param The group to read
 *****************************************************************************/
static void readRegistryGroup(ConfigGroups group)
{
    /* only read valid groups */
    if (group <= GET_CONFIG_UNDEFINED || group >= GET_CONFIG_COUNT)
    {
        return;
    }

    /* get the group */
    if (GET_CONFIG_EXTERNAL != group) 
    {
        WE_REGISTRY_GET(WE_MODID_MSA, group, configRegistryGroup[(int)group], 
            NULL);
    }
    else
    {
        WE_REGISTRY_GET(WE_MODID_MSA, group, configRegistryGroup[(int)group], 
            MSA_REG_KEY_MSISDN);
    }
}

/*!
 * \brief Reads the configuration from the registry. The configurations are
 *        received through a signal.
 *
 *****************************************************************************/
static void readRegistry()
{
    int i;

    /* Get all registry entries needed to configure the MMS Service */
    for (i = (int)GET_CONFIG_UNDEFINED+1; i <= (int)GET_CONFIG_COUNT; i++)
    {
        readRegistryGroup((ConfigGroups)i);
    }
}


/*!
 * \brief Update internal config struct with network status registry values
 * \param inst   The current instance
 * \param fsm    The fsm to call when response is received
 * \param signal The signal to use when response is received
 *****************************************************************************/
static void getNetworkStatus(MsaConfInstance *inst, MsaStateMachine fsm, 
    int signal)
{
    inst->fsm = fsm;
    inst->signal = signal;
    readRegistryGroup(GET_CONFIG_NETWORK_STATUS);
}

/*!
 * \brief Saves the settings to the registry
 *
 * \param tmpConfig The configuration to save.
 * \param group The configuration group to save, See #MsaConfigGroups.
 *****************************************************************************/
static WE_BOOL saveSettings(const MsaConfig *tmpConfig, MsaConfigGroups group)
{
    void* handle;
    /* Create new registry instance */
    handle = WE_REGISTRY_SET_INIT(WE_MODID_MSA);
    if (NULL == handle)
    {
        return FALSE;
    }
    /* Set params for one group */
    switch (group)
    {
    case MSA_CONFIG_MMSC:
        WE_REGISTRY_SET_PATH (handle, (char *)MMS_REG_PATH_COM_MMSC);
        /* Proxy address */
        setRegStr(tmpConfig->proxyHost, handle, (char *)MMS_REG_KEY_PROXY_RELAY);
        /* Proxy POST URI */
        setRegStr(tmpConfig->proxyUri, handle, (char *)MMS_REG_KEY_POST_URI);
        /* Proxy scheme */
        setRegStr(tmpConfig->proxyScheme, handle, 
            (char *)MMS_REG_KEY_PROXY_RELAY_SCHEME);
        /* Proxy port */
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MMS_REG_KEY_PORT,
            (long)tmpConfig->proxyPort);
        break;
        /* This is only a group, i.e., not handled here */
    case MSA_CONFIG_SENDING:
    case MSA_CONFIG_RETRIEVAL:
    case MSA_CONFIG_FILTER:
    case MSA_CONFIG_NETWORK:
    case MSA_CONFIG_GROUPS_COUNT:
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA, 
            "(%s) (%d) erroneous configuration group\n", __FILE__, 
            __LINE__));
        break;
    }
    /* Set configuration */
    WE_REGISTRY_SET_COMMIT(handle);   
    WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA,
        "(%s) (%d) Config group %d saved\n", __FILE__, __LINE__, (int)group));
    return TRUE;
}


/*!
 * \brief Saves the retrieval settings to the registry
 *
 * \param tmpConfig The configuration to save.
 * \param item The item to save, See #MsaConfigRetrieve.
 *****************************************************************************/
static WE_BOOL saveRetrievalSettings(const MsaConfig *tmpConfig, 
    MsaConfigRetrieve item)
{
    void* handle;
    /* Create new registry instance */
    handle = WE_REGISTRY_SET_INIT(WE_MODID_MSA);
    if (NULL == handle)
    {
        return FALSE;
    }
    /* Set params for one group */
    switch (item)
    {
    case MSA_CONFIG_HOME_NETWORK:
        /* Immediate or delayed retrieval  */
        WE_REGISTRY_SET_PATH (handle,
            (char *)MMS_REG_PATH_IMMEDIATE_RETRIEVAL);
        
        WE_REGISTRY_SET_ADD_KEY_INT(handle, 
            (char *)MMS_REG_KEY_IMMEDIATE_RETRIEVAL,
            MSA_RETRIEVAL_MODE_AUTOMATIC == tmpConfig->retrievalMode
            ? TRUE : FALSE);
        break;
    case MSA_CONFIG_ROAM_NETWORK:
        /* Immediate or delayed retrieval when roaming  */
        WE_REGISTRY_SET_PATH (handle, (char *)MMS_REG_PATH_RETRIEVAL); 
        
        WE_REGISTRY_SET_ADD_KEY_INT(handle, 
            (char *)MMS_REG_KEY_RETRIEVAL_ROAM_MODE,
            MSA_RETRIEVAL_MODE_ROAMING_AUTOMATIC == 
            tmpConfig->retrievalModeRoam ? MMS_ROAM_DEFAULT 
            : MSA_RETRIEVAL_MODE_ROAMING_ALWAYS_ASK == 
            tmpConfig->retrievalModeRoam
            ? MMS_ROAM_DELAYED : MMS_ROAM_RESTRICTED);   

        break;
    case MSA_CONFIG_SEND_READ_REPORT:
        WE_REGISTRY_SET_PATH (handle, (char *)MSA_REG_PATH_MMS); 
        /* Send read reports or not  */
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MSA_REG_KEY_SEND_READ_REPORT,
            tmpConfig->sendReadReport);
        break;
    case MSA_CONFIG_DELIVERY_REPORT_ALLOWED:
        WE_REGISTRY_SET_PATH (handle, (char *)MMS_REG_PATH_REPORT); 
        /* Send read reports or not  */
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MMS_REG_KEY_REPORT_ALLOWED,
            (tmpConfig->deliveryReportAllowed == 0) ? 
            MMS_DELIVERY_REPORT_ALLOWED_YES: MMS_DELIVERY_REPORT_ALLOWED_NO);
        break;
    case MSA_CONFIG_RETRIEVE_COUNT:
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA, 
            "(%s) (%d) erroneous configuration item\n", __FILE__, 
            __LINE__));
        break;
    }
    /* Set configuration */
    WE_REGISTRY_SET_COMMIT(handle);   
    WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA,
        "(%s) (%d) Retrieval config item %d saved\n", __FILE__, __LINE__, 
        (int)item));
    return TRUE;
}

/*!
 * \brief Saves network settings to the registry.
 *
 * \param tmpConfig The configuration to save.
 * \param group The configuration group to save. See #MsaConfigNetwork.
 *****************************************************************************/
static WE_BOOL saveNetworkSettings(const MsaConfig *tmpConfig, 
                               MsaConfigNetwork group)
{
    void* handle;
    /* Create new registry instance */
    handle = WE_REGISTRY_SET_INIT(WE_MODID_MSA);
    if (NULL == handle)
    {
        return FALSE;
    }
    switch (group)
    {
    case MSA_CONFIG_CONN_TYPE:
        /* Connection type */
        WE_REGISTRY_SET_PATH (handle, (char *)MMS_REG_PATH_COM_STK);
        switch (tmpConfig->connectionType)
        {
        case MSA_CONNECTION_TYPE_HTTP:
            WE_REGISTRY_SET_ADD_KEY_INT(handle,
                (char *)MMS_REG_KEY_STK_CONNECTION_TYPE, STK_CONNECTION_TYPE_HTTP);
            break;
        case MSA_CONNECTION_TYPE_WSP_CL:
            WE_REGISTRY_SET_ADD_KEY_INT(handle,
                (char *)MMS_REG_KEY_STK_CONNECTION_TYPE, 
                STK_CONNECTION_TYPE_WSP_CL);
            break;
        case MSA_CONNECTION_TYPE_WSP_CO:
            WE_REGISTRY_SET_ADD_KEY_INT(handle, 
                (char *)MMS_REG_KEY_STK_CONNECTION_TYPE, 
                STK_CONNECTION_TYPE_WSP_CO);
            break;
        default:
            WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA, 
                "(%s) (%d) erroneous connection type.\n", __FILE__, __LINE__));
            break;
        }
        break;
        
    case MSA_CONFIG_NET_ACCOUNT:
        /* Network account */
        WE_REGISTRY_SET_PATH (handle, (char *)MMS_REG_PATH_COM_STK);
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MMS_REG_KEY_NETWORK_ACCOUNT, 
            tmpConfig->networkAccount);
        break;
        
    case MSA_CONFIG_WAP_GW:
        /* WAP gateway */
		WE_REGISTRY_SET_PATH (handle, (char *)MMS_REG_PATH_COM_GW);
        setRegStr(tmpConfig->wapGateway, handle, MMS_REG_KEY_GW_ADDRESS);
        break;
        
    case MSA_CONFIG_LOGIN:
		WE_REGISTRY_SET_PATH (handle, (char *)MMS_REG_PATH_COM_GW);
        setRegStr(tmpConfig->login, handle, MMS_REG_KEY_GW_USERNAME);
        break;
        
    case MSA_CONFIG_PASSWD:
		WE_REGISTRY_SET_PATH (handle, (char *)MMS_REG_PATH_COM_GW);
        setRegStr(tmpConfig->passwd, handle, MMS_REG_KEY_GW_PASSWORD);
        break;
        
    case MSA_CONFIG_REALM:
		WE_REGISTRY_SET_PATH (handle, (char *)MMS_REG_PATH_COM_GW);
        setRegStr(tmpConfig->realm, handle, MMS_REG_KEY_GW_REALM);
        break;

    case MSA_CONFIG_SEC_PORT:
        /* Secure port */
		WE_REGISTRY_SET_PATH (handle, (char *)MMS_REG_PATH_COM_GW);
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MMS_REG_KEY_GW_SECURE_PORT, 
        (long)tmpConfig->securePort);
        break;
        
    case MSA_CONFIG_HTTP_PORT:
        /* HTTP port */
		WE_REGISTRY_SET_PATH (handle, (char *)MMS_REG_PATH_COM_GW);
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MMS_REG_KEY_GW_HTTP_PORT, 
            (long)tmpConfig->httpPort);
        break;
    case MSA_CONFIG_NETWORK_COUNT:
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA, 
            "(%s) (%d) erroneous configuration group\n", __FILE__, 
            __LINE__));
        break;
    }
    /* Set configuration */
    WE_REGISTRY_SET_COMMIT(handle);   
    WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA,
        "(%s) (%d) Config group %d saved\n", __FILE__, __LINE__, (int)group));
    return TRUE;
}

/*!
 * \brief Saves the immediate retrieval settings to the registry
 *
 * \param tmpConfig The configuration to save.
 * \param group The configuration group to save, e.g., MMSC.
 *****************************************************************************/
static WE_BOOL saveImRetrSettings(const MsaConfig *tmpConfig, MsaConfigFilter group)
{
    void* handle;
    /* Create new registry instance */
    handle = WE_REGISTRY_SET_INIT(WE_MODID_MSA);
    if (NULL == handle)
    {
        return FALSE;
    }
    /* Set params for one group */
    switch (group)
    {
    case MSA_CONFIG_IM_SIZE:
        WE_REGISTRY_SET_PATH (handle, (char *)MMS_REG_PATH_IMMEDIATE_RETRIEVAL);
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MMS_REG_KEY_MAX_SIZE_OF_MSG,
            (WE_INT32)tmpConfig->maxMsgSize);
        break;

    case MSA_CONFIG_RETR_SIZE:
        WE_REGISTRY_SET_PATH (handle, (char *)MMS_REG_PATH_RETRIEVAL);
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MMS_REG_KEY_MAX_RETRIEVAL_SIZE,
            (WE_INT32)tmpConfig->maxRetrSize);
        break;

    case MSA_CONFIG_TREAT_AS_DEL:
        WE_REGISTRY_SET_PATH (handle, (char *)MMS_REG_PATH_IMMEDIATE_RETRIEVAL);
        WE_REGISTRY_SET_ADD_KEY_INT(handle, 
            (char *)MMS_REG_KEY_TREAT_AS_DELAYED_RETRIEVAL, 
            (WE_INT32)tmpConfig->treatAsDel);
        break;
        
    case MSA_CONFIG_ALLOW_PERSONAL:
        WE_REGISTRY_SET_PATH (handle, (char *)MMS_REG_PATH_IMMEDIATE_RETRIEVAL);
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MMS_REG_KEY_CLASS_PERSONAL,
            tmpConfig->allowPersonal);
        break;
        
    case MSA_CONFIG_ALLOW_ADVERT:
        WE_REGISTRY_SET_PATH (handle, (char *)MMS_REG_PATH_IMMEDIATE_RETRIEVAL);
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MMS_REG_KEY_CLASS_ADVERTISEMENT,
            tmpConfig->allowAdvert);
        break;
        
    case MSA_CONFIG_ALLOW_INFO:
        WE_REGISTRY_SET_PATH (handle, (char *)MMS_REG_PATH_IMMEDIATE_RETRIEVAL);
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MMS_REG_KEY_CLASS_INFORMATIONAL,
            tmpConfig->allowInfo);
        break;
        
    case MSA_CONFIG_ALLOW_AUTO:
        WE_REGISTRY_SET_PATH (handle, (char *)MMS_REG_PATH_IMMEDIATE_RETRIEVAL);
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MMS_REG_KEY_CLASS_AUTO,
            tmpConfig->allowAuto);
        break;

    case MSA_CONFIG_ALLOW_ANONYM:
        WE_REGISTRY_SET_PATH (handle, (char *)MMS_REG_PATH_IMMEDIATE_RETRIEVAL);
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MMS_REG_KEY_ANONYMOUS_SENDER,
            tmpConfig->allowAnonym);
        break;
        
    case MSA_CONFIG_ALLOW_STRING:
        WE_REGISTRY_SET_PATH (handle, (char *)MMS_REG_PATH_IMMEDIATE_RETRIEVAL);
        setRegStr(tmpConfig->allowString, handle, MMS_REG_KEY_CLASS_STRING);
        break;

    case MSA_CONFIG_IM_SERVER:
        WE_REGISTRY_SET_PATH (handle, (char *)MMS_REG_PATH_IMMEDIATE_RETRIEVAL);
        setRegStr(tmpConfig->imServer, handle, 
            MMS_REG_KEY_IMMEDIATE_RETRIEVAL_SERVER);
        break;
    case Msa_CONFIG_RETR_COUNT:        
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA, 
            "(%s) (%d) erroneous configuration group\n", __FILE__, 
            __LINE__));
        break;
    }
    /* Set configuration */
    WE_REGISTRY_SET_COMMIT(handle);   
    WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA,
        "(%s) (%d) Config group %d saved\n", __FILE__, __LINE__, (int)group));
    return TRUE;
}

/*!
 * \brief Saves the immediate retrieval settings to the registry
 *
 * \param tmpConfig The configuration to save.
 * \param group The configuration group to save, e.g., MMSC.
 *****************************************************************************/
static WE_BOOL saveMsgSendSettings(const MsaConfig *tmpConfig, MsaConfigSend group)
{
    void* handle;
    /* Create new registry instance */
    handle = WE_REGISTRY_SET_INIT(WE_MODID_MSA);
    if (NULL == handle)
    {
        return FALSE;
    }
    /* Set params for one group */
    switch (group)
    {
    case MSA_CONFIG_SAVE_ON_SEND:
        WE_REGISTRY_SET_PATH (handle, (char *)MSA_REG_PATH_MMS);
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MSA_REG_KEY_SAVE_ON_SEND,
            tmpConfig->saveOnSend);
        break;
        
    case MSA_CONFIG_PRIORITY:
        WE_REGISTRY_SET_PATH (handle, (char *)MSA_REG_PATH_MMS);
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MSA_REG_KEY_PRIORITY,
            tmpConfig->priority);
        break;
        
    case MSA_CONFIG_EXP_TIME:
        WE_REGISTRY_SET_PATH (handle, (char *)MSA_REG_PATH_MMS);
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MSA_REG_KEY_EXP_TIME,
            tmpConfig->expiryTime);
        break;
        
    case MSA_CONFIG_READ_REPORT:
        WE_REGISTRY_SET_PATH (handle, (char *)MSA_REG_PATH_MMS);
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MSA_REG_KEY_READ_REPORT,
            tmpConfig->readReport);
        break;
        
    case MSA_CONFIG_USE_SIGNATURE_MENU:
        WE_REGISTRY_SET_PATH (handle, (char *)MSA_REG_PATH_MMS);
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MSA_REG_KEY_USE_SIGNATURE,
            tmpConfig->useSignature);
        break;

    case MSA_CONFIG_SIGNATURE_MENU:
        WE_REGISTRY_SET_PATH (handle, (char *)MSA_REG_PATH_MMS);
        if (NULL == tmpConfig->signature)
        {
            WE_REGISTRY_SET_ADD_KEY_STR(handle, (char *)MSA_REG_KEY_SIGNATURE, (unsigned char *)"",
                (WE_UINT16)(sizeof("")));
        }
        else
        {
            WE_REGISTRY_SET_ADD_KEY_STR(handle, (char *)MSA_REG_KEY_SIGNATURE, (unsigned char *)tmpConfig->signature,
                (WE_UINT16)(strlen(tmpConfig->signature) + 1));
        }
        break;

    case MSA_CONFIG_DELIV_REPORT:
        WE_REGISTRY_SET_PATH (handle, (char *)MSA_REG_PATH_MMS);
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MSA_REG_KEY_DELIVERY_REPORT,
            tmpConfig->deliveryReport);
        break;
        
    case MSA_CONFIG_SLIDE_DUR:
        WE_REGISTRY_SET_PATH (handle, (char *)MSA_REG_PATH_MMS);
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MSA_REG_KEY_SLIDE_DUR,
            tmpConfig->slideDuration);
        break;

    case MSA_CONFIG_SENDER_VISIBILITY:
        WE_REGISTRY_SET_PATH (handle, (char *)MSA_REG_PATH_MMS);
        /* Immediate or delayed retrieval when roaming  */
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MSA_REG_KEY_SENDER_VISIBILITY,
            tmpConfig->senderVisibility);
        break;
        
    case MSA_CONFIG_DELIVERY_TIME:
        WE_REGISTRY_SET_PATH (handle, (char *)MSA_REG_PATH_MMS); 
        /* Deliver message immediately or delayed  */
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MSA_REG_KEY_DELIVERY_TIME,
            tmpConfig->deliveryTime);
        break;

    case MSA_CONFIG_REPLY_CHRG_MENU:
    case MSA_CONFIG_SEND_COUNT:
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA, 
            "(%s) (%d) erroneous configuration group\n", __FILE__, 
            __LINE__));
        return FALSE;
    }
    /* Set configuration */
    WE_REGISTRY_SET_COMMIT(handle);   
    WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA,
        "(%s) (%d) Config group %d saved\n", __FILE__, __LINE__, (int)group));
    return TRUE;
}

/*!
 * \brief Saves the reply charging settings to the registry
 *
 * \param tmpConfig The configuration to save.
 * \param group The configuration group to save, e.g., deadline.
 *****************************************************************************/
static WE_BOOL saveMsgReplyChargingSettings(const MsaConfig *tmpConfig,
    MsaConfigReplyCharging group)
{
    void* handle;
    /* Create new registry instance */
    handle = WE_REGISTRY_SET_INIT(WE_MODID_MSA);
    if (NULL == handle)
    {
        return FALSE;
    }
    /* Set params for one group */
    switch (group)
    {
    case MSA_CONFIG_REPLY_CHARGING:
        WE_REGISTRY_SET_PATH (handle, (char *)MSA_REG_PATH_MMS);
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MSA_REG_KEY_REPLY_CHARGING,
            tmpConfig->replyCharging);
        break;
    case MSA_CONFIG_REPLY_CHARGING_DEADLINE:
        WE_REGISTRY_SET_PATH (handle, (char *)MSA_REG_PATH_MMS);
        WE_REGISTRY_SET_ADD_KEY_INT(handle,
            (char *)MSA_REG_KEY_REPLY_CHARGING_DEADLINE,
            tmpConfig->replyChargingDeadline);
        break;
    case MSA_CONFIG_REPLY_CHARGING_SIZE:
        WE_REGISTRY_SET_PATH (handle, (char *)MSA_REG_PATH_MMS);
        WE_REGISTRY_SET_ADD_KEY_INT(handle, (char *)MSA_REG_KEY_REPLY_CHARGING_SIZE,
            tmpConfig->replyChargingSize);
        break;        
    default:
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA, 
            "(%s) (%d) erroneous configuration group\n", __FILE__, 
            __LINE__));
        return FALSE;
    }
    /* Set configuration */
    WE_REGISTRY_SET_COMMIT(handle);   
    WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MSA,
        "(%s) (%d) Config group %d saved\n", __FILE__, __LINE__, (int)group));
    return TRUE;
}

/*!
 * \brief Matches the param and the key, if they are the same the param's value
 *        are set.
 *
 * \param param The param to check.
 * \param key   The key which to match against the param.
 * \param dest  The destination to write the result to.
 * \return      TRUE if the param matches the key, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL setParamInt(const we_registry_param_t *param, const char *key, 
    int *dest)
{
    if (0 != we_cmmn_strcmp_nc(param->key, key))
    {
        return FALSE;
    }
    if (dest == NULL)
    {
        return FALSE;
    }
    *dest = param->value_i;
    return TRUE;
}

/*!
 * \brief Matches the param and the key, if they are the same the param's value
 *        are set.
 *
 * \param param The param to check.
 * \param key The key which to match against the param.
 * \param dest The destination to write the result to.
 * \return TRUE if the param matches the key, otherwise FALSE.
 *****************************************************************************/
static WE_BOOL setParamStr(const we_registry_param_t *param, const char *key, 
    char **dest)
{
    if (0 != we_cmmn_strcmp_nc(param->key, key))
        return FALSE;
    MSA_FREE(*dest);
    *dest = MSA_ALLOC(param->value_bv_length + 1);
    strcpy(*dest, (const char *)param->value_bv);   
    return TRUE;
}

/*!
 * \brief Sets a param string value.
 *
 * \param str The string to set.
 * \param handle The handle of the param.
 * \param key The key to assign a value to.
 *****************************************************************************/
static void setRegStr(char *str, void *handle, const char *key)
{
    if (NULL == str)
    {
        WE_REGISTRY_SET_ADD_KEY_STR(handle, (char *)key, (unsigned char *)"",
            (WE_UINT16)(sizeof("")));
    }
    else
    {
        WE_REGISTRY_SET_ADD_KEY_STR(handle, (char *)key, (unsigned char *)str,
            (WE_UINT16)(strlen(str) + 1));
    }
}

/*!
 * \brief Sets a string value but deallocates the memory of destination if
 *        it exists.
 *
 * \param dst The memory to set.
 * \param src The new string value.
 *****************************************************************************/
static void setDstStr(char **dst, char *src)
{
    MSA_FREE(*dst);
    *dst = src;
}

/*!
 * \brief Gets configuration data for config GUI.
 *        This function returns 
 *
 * \param menu The menu to get data for. See #MsaConfigMenu.
 * \param configId The menu item to get data for. Valid IDs are from 
 *                 #MsaConfigGroups, #MsaConfigFilter and #MsaConfigSend.
 * \param tmpData NULL if global data should be used. Otherwise temporary data.
 * \param intValue The configuration data if it is an integer. Default 0.
 * \param strValue The configuration data if it is a string. Default NULL.
 *                 #MSA_CONFIG_MMSC, #MSA_CONFIG_SLIDE_DUR and 
 *                 #MSA_CONFIG_SLIDE_DUR as configId will cause a memory 
 *                 allocation that has to be freed.
 * \param maxStrLen Returns maximum number of characters in the string.
 * \param textType Returns the text type of the string if applicable.
 *****************************************************************************/
void msaGetConfigData(MsaConfigMenu menu, int configId, const void *tmpData, 
    int *intValue, char **strValue, int *maxStrLen, WeTextType *textType)
{
    MsaConfig *conf;
    char *configMMSC;
    unsigned int len;

    *strValue = NULL;
    *intValue = 0;
    *maxStrLen = 0;
    *textType = WeText;

    conf = msaGetConfig();
    switch (menu) 
    {
    case MSA_CONFIG_MENU_ROOT:
        switch (configId)
        {
        case MSA_CONFIG_MMSC:
            len = 0;
            if (conf->proxyPort > 0)
            {
                len = strlen((conf->proxyScheme ? conf->proxyScheme 
                    : "http://")) + strlen((conf->proxyHost ? 
                    conf->proxyHost : "")) + 6 /*port*/ + 
                    strlen((conf->proxyUri ? conf->proxyUri : "")) +
                    strlen(":") + 1;
                if (len <= MSA_CONF_MMSC_MAX_SIZE)
                {
                    len = MSA_CONF_MMSC_MAX_SIZE + 1;
                }
                configMMSC = MSA_ALLOC((unsigned int)len);                    
                sprintf(configMMSC, "%s%s:%u%s", 
                    (conf->proxyScheme ? conf->proxyScheme : "http://"),
                    (conf->proxyHost ? conf->proxyHost : ""), 
                    conf->proxyPort, (conf->proxyUri ? conf->proxyUri : ""));
            }
            else
            {
                len = strlen((conf->proxyScheme ? conf->proxyScheme 
                    : "http://")) + 
                    strlen((conf->proxyHost ? conf->proxyHost : "")) +
                    strlen((conf->proxyUri ? conf->proxyUri : "")) + 1;
                if (len <= MSA_CONF_MMSC_MAX_SIZE)
                {
                    len = MSA_CONF_MMSC_MAX_SIZE + 1;
                }
                configMMSC = MSA_ALLOC((unsigned int)len);
                sprintf(configMMSC, "%s%s%s",
                    (conf->proxyScheme ? conf->proxyScheme : "http://"),
                    (conf->proxyHost ? conf->proxyHost : ""), 
                    (conf->proxyUri ? conf->proxyUri : ""));
            }            

            *maxStrLen = (int)len;
            *textType = WeUrl;
            *strValue = configMMSC;
            break;
        default:
            break;
        }
        break;
    case MSA_CONFIG_MENU_SEND:
        switch (configId)
        {
        case MSA_CONFIG_EXP_TIME:
            *intValue = (tmpData ? (int)((MsaPropItem*)tmpData)->expiryTime : 
            conf->expiryTime);
            break;
        case MSA_CONFIG_READ_REPORT:
            *intValue = (tmpData ? ((MsaPropItem*)tmpData)->readReport : 
            conf->readReport);
            break;
        case MSA_CONFIG_DELIV_REPORT:
            *intValue = (tmpData ? ((MsaPropItem*)tmpData)->deliveryReport : 
            conf->deliveryReport);
            break;
        case MSA_CONFIG_PRIORITY:
            *intValue = (tmpData ? (int)((MsaPropItem*)tmpData)->priority : 
            conf->priority);
            break;
        case MSA_CONFIG_SAVE_ON_SEND:
            *intValue = conf->saveOnSend;
            break;
        case MSA_CONFIG_SLIDE_DUR:
            *strValue = MSA_ALLOC(MSA_CFG_DURATION_SIZE + 1);
            sprintf(*strValue, "%d", conf->slideDuration);            
            *maxStrLen = MSA_CFG_DURATION_SIZE;
            *textType = WeInteger;
            break;
        case MSA_CONFIG_SENDER_VISIBILITY:
            *intValue = (tmpData ? (int)((MsaPropItem*)tmpData)->senderVisibility : conf->senderVisibility);
            break;
        case MSA_CONFIG_DELIVERY_TIME:
            *intValue = conf->deliveryTime;
            break;
        default:
            break;
        }
        break;            
    case MSA_CONFIG_MENU_REP_CHRG_SETTINGS:
        switch (configId)
        {
        case MSA_CONFIG_REPLY_CHARGING:
            *intValue = (tmpData ? (int)((MsaPropItem*)tmpData)->replyCharging 
                : conf->replyCharging);
            break;
        case MSA_CONFIG_REPLY_CHARGING_DEADLINE:
            *intValue = (tmpData 
                ? (int)((MsaPropItem*)tmpData)->replyChargingDeadline
                : conf->replyChargingDeadline);
            break;
        case MSA_CONFIG_REPLY_CHARGING_SIZE:
            *intValue = (tmpData 
                ? (int)((MsaPropItem*)tmpData)->replyChargingSize
                : conf->replyChargingSize);
            break;
        default:
            break;
        }
        break;
    case MSA_CONFIG_MENU_SIGNATURE:
        switch(configId)
        {
        case MSA_CONFIG_USE_SIGNATURE:
            *intValue = conf->useSignature;
            break;
        case MSA_CONFIG_EDIT_SIGNATURE:
            *strValue = conf->signature;
            break;
        default:
            break;
        }
        break;
    case MSA_CONFIG_MENU_FILTER:
        switch (configId)
        {
        case MSA_CONFIG_IM_SERVER:
            *strValue = conf->imServer;
            *maxStrLen = MSA_CONF_MMSC_MAX_SIZE;
            *textType = WeUrl;
            break;
        case MSA_CONFIG_IM_SIZE:
            *strValue = MSA_ALLOC(MSA_CFG_MAX_IM_RETR_SIZE_DIGITS + 1);
            sprintf(*strValue, "%d", conf->maxMsgSize);            
            *maxStrLen = MSA_CFG_MAX_IM_RETR_SIZE_DIGITS;
            *textType = WeInteger;
            break;
        case MSA_CONFIG_RETR_SIZE:
            *strValue = MSA_ALLOC(MSA_CFG_MAX_RETRIEVAL_SIZE_DIGITS + 1);
            sprintf(*strValue, "%d", conf->maxRetrSize);            
            *maxStrLen = MSA_CFG_MAX_RETRIEVAL_SIZE_DIGITS;
            *textType = WeInteger;
            break;
        case MSA_CONFIG_TREAT_AS_DEL:
            *intValue = conf->treatAsDel;
            break;
        case MSA_CONFIG_ALLOW_ANONYM:
            *intValue = conf->allowAnonym;
            break;
        case MSA_CONFIG_ALLOW_PERSONAL:
            *intValue = conf->allowPersonal;            
            break;
        case MSA_CONFIG_ALLOW_ADVERT:
            *intValue = conf->allowAdvert;            
            break;
        case MSA_CONFIG_ALLOW_INFO:
            *intValue = conf->allowInfo;            
            break;
        case MSA_CONFIG_ALLOW_AUTO:
            *intValue = conf->allowAuto;            
            break;
        case MSA_CONFIG_ALLOW_STRING:
            *strValue = conf->allowString;
            *maxStrLen = MSA_CONF_ALLOW_STR_MAX_SIZE;
            *textType = WeText;
            break;
        default:
            break;
        }
    	break;
    case MSA_CONFIG_MENU_NETWORK:
        switch (configId)
        {
        case MSA_CONFIG_CONN_TYPE :
            *intValue = (int)conf->connectionType;
            break;
        case MSA_CONFIG_NET_ACCOUNT :
            *intValue = getNetworkAccountIndex(conf->networkAccount);
            break;
        case MSA_CONFIG_WAP_GW :
            *strValue = conf->wapGateway;
            *maxStrLen = MSA_CONF_WAP_GATEWAY_MAX_SIZE;
            *textType = WeText;
            break;
        case MSA_CONFIG_LOGIN :
            *strValue = conf->login;
            *maxStrLen = MSA_CONF_LOGIN_MAX_SIZE;
            *textType = WeText;
            break;
        case MSA_CONFIG_PASSWD :
            *strValue = conf->passwd;
            *maxStrLen = MSA_CONF_PASSWD_MAX_SIZE;
            *textType = WePasswordText;
            break;
        case MSA_CONFIG_REALM :
            *strValue = conf->realm;
            *maxStrLen = MSA_CONF_REALM_MAX_SIZE;
            *textType = WeText;
            break;
        case MSA_CONFIG_SEC_PORT :
            *strValue = MSA_ALLOC(MSA_CONF_SEC_PORT_MAX_SIZE + 1);
            sprintf(*strValue, "%d", (int)conf->securePort);            
            *maxStrLen = MSA_CONF_SEC_PORT_MAX_SIZE;
            *textType = WeInteger;
            break;
        case MSA_CONFIG_HTTP_PORT :
            *strValue = MSA_ALLOC(MSA_CONF_HTTP_PORT_MAX_SIZE + 1);
            sprintf(*strValue, "%d", (int)conf->httpPort);
            *maxStrLen = MSA_CONF_HTTP_PORT_MAX_SIZE;
            *textType = WeInteger;
            break;
        case MSA_CONFIG_NETWORK_COUNT:
            conf->roamingStatus = 0;
            break;         

        default:
            break;
        }
        break; 
        
    case MSA_CONFIG_MENU_RETRIEVE:
      switch (configId)
      {
        case MSA_CONFIG_HOME_NETWORK:
            *intValue = conf->retrievalMode;
            break;
        case MSA_CONFIG_ROAM_NETWORK:
            *intValue = conf->retrievalModeRoam;
            break;
        case MSA_CONFIG_SEND_READ_REPORT:
            *intValue = conf->sendReadReport;
            break;
        case MSA_CONFIG_DELIVERY_REPORT_ALLOWED:
            *intValue = conf->deliveryReportAllowed;
            break;
      }
      break;
      case MSA_CONFIG_MENU_DEFAULT:
      case MSA_CONFIG_MENU_MMS_SETTINGS:
      case MSA_CONFIG_MENU_COUNT:
    default:
        break;
    }
}


/*!
 * \brief Displays an error dialog from a string.
 *
 * \param str The string to show in the dialog.
 * \return TRUE on success, else FALSE.
 *****************************************************************************/
void msaUpdateNetworkStatusInfo(MsaStateMachine fsm, int signal)
{
    (void)MSA_SIGNAL_SENDTO_UU(MSA_CONFIG_FSM, MSA_SIG_CONF_GET_NETWORK_STATUS,
        fsm, (unsigned int)signal);
}

/*!
 * \brief Displays an error dialog from a string.
 *
 * \param str The string to show in the dialog.
 * \return TRUE on success, else FALSE.
 *****************************************************************************/
static WE_BOOL showErrorDlg(const char *str)
{
    WeStringHandle strHandle;

    if (NULL == str)
    {
        return FALSE;
    }
    if (0 == (strHandle = WE_WIDGET_STRING_CREATE(WE_MODID_MSA, 
        str, WeUtf8, (int)strlen(str) + 1, 0)))
    {
        return FALSE;
    }
    (void)msaShowDialog(strHandle, MSA_DIALOG_ERROR);
    (void)WE_WIDGET_RELEASE(strHandle);
    return TRUE;
}

/*!
 * \brief Saves a provisioning element to the registry
 * 
 * \param param the provisioning element
 *****************************************************************************/
static void SaveProvisioningElement(we_registry_param_t *param)
{
    char *str;
    int i;
    MsaConnectionType connType;

    if ((NULL != param) && (NULL != param->key))
    {
        for (i = 0; prsKeys[i].prsStr != NULL; i++)
        {
            if (we_cmmn_strcmp_nc(param->key, prsKeys[i].prsStr) == 0) 
            {
                switch (prsKeys[i].keyId)
                {                            
                case MSA_PRS_KEY_MMSC_URL:
                case MSA_PRS_KEY_WAP_GW:
                case MSA_PRS_KEY_LOGIN:
                case MSA_PRS_KEY_PASSWORD:
                    /*
                     *	String values
                     */
                    str = NULL;
                    if (param->value_bv_length > 0)
                    {
                        MSA_CALLOC(str, param->value_bv_length+1);
                        memcpy(str, param->value_bv, param->value_bv_length);
                    } /* if */
                    if (MSA_PRS_KEY_MMSC_URL == prsKeys[i].keyId)
                    {
                        /* Special case, mmsc needs to be treated differently */
                        msaMmscCallback(str, 0, NULL, 0);
                    }
                    else
                    {
                        msaNetworkCallback(str, 0, 0, prsKeys[i].groupId);
                    } /* if */
                    break;                            
                    
                case MSA_PRS_KEY_HTTP_PORT:
                case MSA_PRS_KEY_NETWORK_ACCOUNT_ID:
                    /*
                     *	Integer values
                     */
                    msaNetworkCallback(NULL, param->value_i, 0, 
                        prsKeys[i].groupId);
                    break;
                    
                case MSA_PRS_KEY_NET_CONNECTION_TYPE:
                    /*
                     *	Mapping needed 
                     */
                    switch (param->value_i)
                    {
                    case STK_CONNECTION_TYPE_HTTP:
                        connType = MSA_CONNECTION_TYPE_HTTP;
                        break;
                    case STK_CONNECTION_TYPE_WSP_CL:
                        connType = MSA_CONNECTION_TYPE_WSP_CL;
                        break;
                    case STK_CONNECTION_TYPE_WSP_CO:
                        connType = MSA_CONNECTION_TYPE_WSP_CO;
                        break;
                    default :
                        connType = -1;
                        break;
                    } /* switch */
                    if (-1 != connType)
                    {
                        msaNetworkCallback(NULL, connType, 0, 
                            prsKeys[i].groupId);
                    } /* if */
                    break;
                    
                    default :
                        break;
                } /* switch */
            } /* if */
        } /* for */
    } /* if */
} /* SaveProvisioningElement */

/*!
 * \brief Handles input from PRS and adds it to the registry
 *
 * \param regRsp    Structure with all provisioned settings
 *****************************************************************************/
void msaSaveProvisioningData(we_registry_response_t *regRsp)
{    
    we_registry_param_t param;
    int i;

    /*
     *	First, clear everything.
     */
    for (i = 0; prsKeys[i].prsStr != NULL; i++)
    {
        if (MSA_PRS_KEY_CLEANSE_YES == prsKeys[i].cleanse)
        {
            /*
             *	Simulate receival of zero'ed param
             */
            memset(&param, 0, sizeof(we_registry_param_t));
            param.key = prsKeys[i].prsStr;
            SaveProvisioningElement(&param);
        } /* if */
    } /* for */

    /*
     *	Then, fill in received provisioning parameters one by one
     */
    while (WE_REGISTRY_RESPONSE_GET_NEXT(regRsp, &param))
    {
        SaveProvisioningElement(&param);
    } /* while */
}
