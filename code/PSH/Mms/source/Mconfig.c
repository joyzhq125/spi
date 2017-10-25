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






#include "We_Lib.h"    
#include "We_Cfg.h"    
#include "We_Def.h"     
#include "We_Log.h"     
#include "We_Mem.h"    
#include "We_Core.h"   
#include "We_Cmmn.h"   
#include "We_Wid.h"    
#include "We_Dcvt.h"   

#include "Stk_If.h"     

#include "Mms_Cfg.h"    
#include "Mms_Def.h"    
#include "Mms_If.h"     

#include "Msig.h"       
#include "Mmem.h"       
#include "Mconfig.h"    
#include "Mcpdu.h"      
#include "Mcwap.h"      
#include "Mmain.h"      
#include "Mms_Rc.h"     
#include "Fldmgr.h"     
#include "Mreceive.h"   
#include "Mmsrec.h"     




typedef struct
{
    int returnValue;    

    int requests;       

    int responses;      

    WE_BOOL isStarted; 

} MmsConfigInstance;



typedef struct
{
    int     keyId;      
    char*   regKeyName; 
    char*   regPath;    
} MmsRegistryMapEntry;




















static int cfgIntValues[MMS_CFG_TOTAL_INT];
static WE_BOOL cfgIntSet[MMS_CFG_TOTAL_INT];




static char *cfgStrValues[MMS_CFG_TOTAL_STR];
static WE_BOOL cfgStrSet[MMS_CFG_TOTAL_STR];

static MmsConfigInstance* cfgFsm;


static const MmsRegistryMapEntry mmsRegistryMapInt[MMS_CFG_TOTAL_INT] =
{
    {MMS_CFG_ANONYMOUS_SENDER, 
        MMS_REG_KEY_ANONYMOUS_SENDER,
        MMS_REG_PATH_IMMEDIATE_RETRIEVAL},
    {MMS_CFG_CLASS_PERSONAL, 
        MMS_REG_KEY_CLASS_PERSONAL,
        MMS_REG_PATH_IMMEDIATE_RETRIEVAL},
    {MMS_CFG_CLASS_ADVERTISEMENT, 
        MMS_REG_KEY_CLASS_ADVERTISEMENT,
        MMS_REG_PATH_IMMEDIATE_RETRIEVAL},
    {MMS_CFG_CLASS_INFORMATIONAL, 
        MMS_REG_KEY_CLASS_INFORMATIONAL,
        MMS_REG_PATH_IMMEDIATE_RETRIEVAL},
    {MMS_CFG_CLASS_AUTO, 
        MMS_REG_KEY_CLASS_AUTO,
        MMS_REG_PATH_IMMEDIATE_RETRIEVAL},
    {MMS_CFG_REPORT_ALLOWED, 
        MMS_REG_KEY_REPORT_ALLOWED,
        MMS_REG_PATH_REPORT},
    {MMS_CFG_DISCONNECT_ON_IDLE, 
        MMS_REG_KEY_DISCONNECT_ON_IDLE,
        MMS_REG_PATH_COM_STK},
    {MMS_CFG_GW_SECURE_PORT,
        MMS_REG_KEY_GW_SECURE_PORT,
        MMS_REG_PATH_COM_GW},
    {MMS_CFG_GW_HTTP_PORT,
        MMS_REG_KEY_GW_HTTP_PORT,
        MMS_REG_PATH_COM_GW},
    {MMS_CFG_FROM_ADDRESS_TYPE, 
        MMS_REG_KEY_FROM_ADDRESS_TYPE,
        MMS_REG_PATH_ADDRESS},
    {MMS_CFG_FROM_ADDRESS_INSERT_TYPE, 
        MMS_REG_KEY_FROM_ADDRESS_INSERT_TYPE,
        MMS_REG_PATH_ADDRESS},
    {MMS_CFG_IMMEDIATE_RETRIEVAL, 
        MMS_REG_KEY_IMMEDIATE_RETRIEVAL,
        MMS_REG_PATH_IMMEDIATE_RETRIEVAL},
    {MMS_CFG_NETWORK_ACCOUNT, 
        MMS_REG_KEY_NETWORK_ACCOUNT,
        MMS_REG_PATH_COM_STK},
    {MMS_CFG_NETWORK_STATUS_ROAMING, 
        MMS_REG_KEY_NETWORK_STATUS_ROAMING,
        MMS_REG_PATH_NETWORK_STATUS},
    {MMS_CFG_NETWORK_STATUS_BEARER, 
        MMS_REG_KEY_NETWORK_STATUS_BEARER,
        MMS_REG_PATH_NETWORK_STATUS},
    {MMS_CFG_NO_SMS_NOTIFY_RESP, 
        MMS_REG_KEY_NO_SMS_NOTIFY_RESP,
        MMS_REG_PATH_COM_MMSC},
    {MMS_CFG_MAX_SIZE_OF_MSG, 
        MMS_REG_KEY_MAX_SIZE_OF_MSG,
        MMS_REG_PATH_IMMEDIATE_RETRIEVAL},
    {MMS_CFG_MAX_RETRIEVAL_SIZE, 
        MMS_REG_KEY_MAX_RETRIEVAL_SIZE,
        MMS_REG_PATH_RETRIEVAL},
    {MMS_CFG_ROAMING_MODE, 
        MMS_REG_KEY_RETRIEVAL_ROAM_MODE,
        MMS_REG_PATH_RETRIEVAL},
    {MMS_CFG_PORT, 
        MMS_REG_KEY_PORT,
        MMS_REG_PATH_COM_MMSC},
    {MMS_CFG_PROXY_RELAY_VERSION, 
        MMS_REG_KEY_PROXY_RELAY_VERSION,
        MMS_REG_PATH_COM_MMSC},
    {MMS_CFG_STK_CONNECTION_TYPE, 
        MMS_REG_KEY_STK_CONNECTION_TYPE,
        MMS_REG_PATH_COM_STK},
    {MMS_CFG_TREAT_AS_DELAYED_RETRIEVAL, 
        MMS_REG_KEY_TREAT_AS_DELAYED_RETRIEVAL,
        MMS_REG_PATH_IMMEDIATE_RETRIEVAL}
};

static const MmsRegistryMapEntry mmsRegistryMapStr[MMS_CFG_TOTAL_STR] =
{
    {MMS_CFG_CLASS_STRING, 
        MMS_REG_KEY_CLASS_STRING,
        MMS_REG_PATH_IMMEDIATE_RETRIEVAL},
    {MMS_CFG_CLIENT_USER_AGENT,
        MMS_REG_KEY_USER_AGENT,
        MMS_REG_PATH_CLIENT},
    {MMS_CFG_GW_ADDRESS,
        MMS_REG_KEY_GW_ADDRESS,
        MMS_REG_PATH_COM_GW},
    {MMS_CFG_GW_PASSWORD,
        MMS_REG_KEY_GW_PASSWORD,
        MMS_REG_PATH_COM_GW},
    {MMS_CFG_GW_REALM,
        MMS_REG_KEY_GW_REALM,
        MMS_REG_PATH_COM_GW},
    {MMS_CFG_GW_USERNAME,
        MMS_REG_KEY_GW_USERNAME,
        MMS_REG_PATH_COM_GW},
    {MMS_CFG_FROM_ADDRESS, 
        MMS_REG_KEY_FROM_ADDRESS,
        MMS_REG_PATH_MSISDN},
    {MMS_CFG_FROM_NAME, 
        MMS_REG_KEY_FROM_NAME,
        MMS_REG_PATH_ADDRESS},
    {MMS_CFG_IMMEDIATE_RETRIEVAL_SERVER, 
        MMS_REG_KEY_IMMEDIATE_RETRIEVAL_SERVER,
        MMS_REG_PATH_IMMEDIATE_RETRIEVAL},
    {MMS_CFG_POST_URI, 
        MMS_REG_KEY_POST_URI,
        MMS_REG_PATH_COM_MMSC},
    {MMS_CFG_PROXY_RELAY, 
        MMS_REG_KEY_PROXY_RELAY,
        MMS_REG_PATH_COM_MMSC},
    {MMS_CFG_PROXY_RELAY_SCHEME, 
        MMS_REG_KEY_PROXY_RELAY_SCHEME,
        MMS_REG_PATH_COM_MMSC},
    {MMS_CFG_TEXT_FROM, 
        MMS_REG_KEY_TEXT_FROM,
        MMS_REG_PATH_REPORT_TEXT},
    {MMS_CFG_TEXT_MSGID, 
        MMS_REG_KEY_TEXT_MSGID,
        MMS_REG_PATH_REPORT_TEXT},
    {MMS_CFG_TEXT_READ, 
        MMS_REG_KEY_TEXT_READ,
        MMS_REG_PATH_REPORT_TEXT},
    {MMS_CFG_TEXT_SENT, 
        MMS_REG_KEY_TEXT_SENT,
        MMS_REG_PATH_REPORT_TEXT},
    {MMS_CFG_TEXT_SUBJECT, 
        MMS_REG_KEY_TEXT_SUBJECT,
        MMS_REG_PATH_REPORT_TEXT},
    {MMS_CFG_TEXT_TO, 
        MMS_REG_KEY_TEXT_TO,
        MMS_REG_PATH_REPORT_TEXT},
    {MMS_CFG_TEXT_WAS_READ, 
        MMS_REG_KEY_TEXT_WAS_READ,
        MMS_REG_PATH_REPORT_TEXT},
    {MMS_CFG_TEXT_YOUR_MSG, 
        MMS_REG_KEY_TEXT_YOUR_MSG,
        MMS_REG_PATH_REPORT_TEXT},
    {MMS_CFG_CLIENT_HTTP_HEADER,
        MMS_REG_KEY_CLIENT_HTTP_HEADER,
        MMS_REG_PATH_COM_STK
    }
};




static void afterChangeCheckInt(MmsConfigInt key);
static void afterChangeCheckStr(MmsConfigStr key);
static int getKeyIdFromNameInt(const char* key);
static int getKeyIdFromNameStr(const char* key);
static void cfgHandleRegistryResponse(MmsRegistryEntry* p);
static void cfgMain(MmsSignal *sig);
static void cfgReadRegistry(int retVal);


#ifdef WE_LOG_MODULE 



const char *fsmCoreConfigSigName(WE_UINT32 sigType)
{
    switch (sigType)
    {
    case MMS_SIG_CORE_READ_REGISTRY:
        return "MMS_SIG_CORE_READ_REGISTRY";
    case MMS_SIG_CORE_REG_RESPONSE:
        return "MMS_SIG_CORE_REG_RESPONSE";
    default:
        return 0;
    }
} 
#endif







void cfgEmergencyAbort(void)
{
    mSignalDeregister(M_FSM_CORE_CONFIG);
} 







static void afterChangeCheckInt(MmsConfigInt key)
{
    
    switch (key)
    {
    case MMS_CFG_DISCONNECT_ON_IDLE :
        mmsWapConnectionCheck();
        break;
    case MMS_CFG_NETWORK_STATUS_ROAMING:
        if (cfgGetInt(MMS_CFG_NETWORK_STATUS_ROAMING) == MMS_ROAMING)
        {            
            if (cfgGetInt(MMS_CFG_ROAMING_MODE) == MMS_ROAM_DELAYED ||
                cfgGetInt(MMS_CFG_ROAMING_MODE) == MMS_ROAM_REJECT)
            {
                


                mmsShowAllImmediateNotif();
            }
        }
        else
        {
            if (cfgGetInt(MMS_CFG_IMMEDIATE_RETRIEVAL) != 0)
            {
                


                mmsImmediateRetrievalGet();
            }
        }
        break;
    case MMS_CFG_NETWORK_STATUS_BEARER:
        if (cfgGetInt(MMS_CFG_NETWORK_STATUS_BEARER) == MMS_BEARER_NOT_AVAILABLE)
        {
            recRetrieveCancel(0);
            recSendCancel(0);
        }
        else
        {    
            if (cfgGetInt(MMS_CFG_IMMEDIATE_RETRIEVAL) != 0)
            {
                


                mmsImmediateRetrievalGet();
            }
        }
        break;
    case MMS_CFG_IMMEDIATE_RETRIEVAL:
        if (cfgGetInt(MMS_CFG_IMMEDIATE_RETRIEVAL) == 0)
        {
            if ((cfgGetInt(MMS_CFG_NETWORK_STATUS_ROAMING) == MMS_ROAMING) &&
                (cfgGetInt(MMS_CFG_ROAMING_MODE) == MMS_ROAM_DELAYED ||
                cfgGetInt(MMS_CFG_ROAMING_MODE) == MMS_ROAM_REJECT))
            {
                



            }
            else
            {
                


                mmsShowAllImmediateNotif();
            }
        }
        else
        {
            


            mmsImmediateRetrievalGet();
        }
        break;
    case MMS_CFG_STK_CONNECTION_TYPE : 
    case MMS_CFG_NETWORK_ACCOUNT : 
    case MMS_CFG_GW_SECURE_PORT :
    case MMS_CFG_GW_HTTP_PORT :
        

        mmsWapChannelDisconnect();
        break;
    default : 
        break;
    } 
} 







static void afterChangeCheckStr(MmsConfigStr key)
{
    
    switch (key)
    {
    case MMS_CFG_GW_ADDRESS:
    case MMS_CFG_GW_PASSWORD:
    case MMS_CFG_GW_REALM:
    case MMS_CFG_GW_USERNAME:
    case MMS_CFG_CLIENT_USER_AGENT:
    case MMS_CFG_CLIENT_HTTP_HEADER:
    



        mmsWapChannelDisconnect();
        break;
    default:
        break;
    } 
} 







int cfgGetInt(MmsConfigInt key)
{
    if (key < 0 || key >= MMS_CFG_TOTAL_INT)
    {
        return -1;
    } 
    
    return cfgIntValues[key];
} 









char *cfgGetStr(MmsConfigStr key)
{
    if (key < 0 || key >= MMS_CFG_TOTAL_STR)
    {
        return NULL;
    } 
    
    return cfgStrValues[key];
} 








static char *cfgGetStringFromHandle(WeStringHandle strHandle)
{
    char *tmpStr = NULL;
    int length;

    length = WE_WIDGET_STRING_GET_LENGTH(strHandle, TRUE, WeUtf8);
    if (length > 0 && NULL == (tmpStr = M_CALLOC((WE_UINT32) length)))
    {
        return NULL;
    }
    if (0 > WE_WIDGET_STRING_GET_DATA(strHandle, tmpStr, WeUtf8))
    {
        return NULL;    
    }
    return tmpStr;
}








static char *cfgGetStringFromID(WE_UINT32 resId)
{
    return cfgGetStringFromHandle(WE_WIDGET_STRING_GET_PREDEFINED(resId));
}






void cfgInit(void)
{
    int i;

    for(i = 0; i<MMS_CFG_TOTAL_INT; i++)
    {
        cfgIntValues[i] = 0;
        cfgIntSet[i] = FALSE;
    }
    for(i = 0; i<MMS_CFG_TOTAL_STR; i++)
    {
        cfgStrValues[i] = NULL;
        cfgStrSet[i] = FALSE;
    }

    cfgFsm = M_ALLOCTYPE(MmsConfigInstance);
    cfgFsm->requests        = 0;
    cfgFsm->responses       = 0;
    cfgFsm->isStarted       = FALSE;
    cfgFsm->returnValue     = 0;

    
    cfgIntValues[MMS_CFG_FROM_ADDRESS_INSERT_TYPE] = MMS_FROM_ADDRESS_PRESENT;
    cfgIntValues[MMS_CFG_PORT] = MMS_DEFAULT_PORT;
    cfgIntValues[MMS_CFG_REPORT_ALLOWED] = MMS_DELIVERY_REPORT_ALLOWED_YES;
    cfgIntValues[MMS_CFG_DISCONNECT_ON_IDLE] = MMS_DEFAULT_DISCONNECT_ON_IDLE;
    cfgStrValues[MMS_CFG_CLIENT_HTTP_HEADER] =
        we_cmmn_strdup( WE_MODID_MMS, MMS_DEFAULT_CLIENT_HTTP_HEADER);

    cfgIntValues[MMS_CFG_PROXY_RELAY_VERSION] = MMS_VERSION_11;
    cfgIntValues[MMS_CFG_STK_CONNECTION_TYPE] = MMS_DEFAULT_STK_CONNECTION_TYPE;
    cfgIntValues[MMS_CFG_NETWORK_ACCOUNT] = MMS_DEFAULT_NETWORK_ACCOUNT;
    cfgIntValues[MMS_CFG_NO_SMS_NOTIFY_RESP] = MMS_DEFAULT_NO_SMS_NOTIFY_RESP;
    cfgIntValues[MMS_CFG_GW_HTTP_PORT] = MMS_DEFAULT_GW_HTTP_PORT;
    cfgIntValues[MMS_CFG_GW_SECURE_PORT] = MMS_DEFAULT_GW_SECURE_PORT;
    
    
    cfgIntValues[MMS_CFG_IMMEDIATE_RETRIEVAL] = FALSE;
    cfgIntValues[MMS_CFG_CLASS_PERSONAL] = TRUE; 
    cfgIntValues[MMS_CFG_CLASS_ADVERTISEMENT] = FALSE; 
    cfgIntValues[MMS_CFG_CLASS_INFORMATIONAL] = TRUE;
    cfgIntValues[MMS_CFG_CLASS_AUTO] = TRUE; 
    cfgIntValues[MMS_CFG_ANONYMOUS_SENDER] = FALSE;
    cfgIntValues[MMS_CFG_MAX_SIZE_OF_MSG] = MMS_DEFAULT_MAX_MSG_SIZE;
    cfgIntValues[MMS_CFG_MAX_RETRIEVAL_SIZE] = MMS_DEFAULT_MAX_RETRIEVAL_SIZE;
    cfgIntValues[MMS_CFG_ROAMING_MODE] = MMS_DEFAULT_ROAMING_MODE;
    
    cfgStrValues[MMS_CFG_FROM_ADDRESS] = 
		we_cmmn_strdup (WE_MODID_MMS, MMS_CFG_MSISDN);
    cfgStrValues[MMS_CFG_CLIENT_USER_AGENT] = 
        we_cmmn_strdup( WE_MODID_MMS, MMS_DEFAULT_USER_AGENT);
    cfgStrValues[MMS_CFG_PROXY_RELAY_SCHEME] = 
        we_cmmn_strdup( WE_MODID_MMS, MMS_DEFAULT_PROXY_RELAY_SCHEME);
    cfgStrValues[MMS_CFG_PROXY_RELAY] = 
        we_cmmn_strdup( WE_MODID_MMS, MMS_DEFAULT_PROXY_RELAY);
    cfgStrValues[MMS_CFG_POST_URI] = 
        we_cmmn_strdup( WE_MODID_MMS, MMS_DEFAULT_POST_URI);
    cfgStrValues[MMS_CFG_GW_ADDRESS] = 
        we_cmmn_strdup( WE_MODID_MMS, MMS_DEFAULT_GW_ADDRESS);
    cfgStrValues[MMS_CFG_GW_PASSWORD] =
        we_cmmn_strdup( WE_MODID_MMS, MMS_DEFAULT_GW_PASSWORD);
    cfgStrValues[MMS_CFG_GW_REALM] =
        we_cmmn_strdup( WE_MODID_MMS, MMS_DEFAULT_GW_REALM);
    cfgStrValues[MMS_CFG_GW_USERNAME] =
        we_cmmn_strdup( WE_MODID_MMS, MMS_DEFAULT_GW_USERNAME);

    
    cfgStrValues[MMS_CFG_TEXT_FROM] = 
        cfgGetStringFromID( MMS_STR_ID_RR_TEXT_FROM);
    cfgStrValues[MMS_CFG_TEXT_TO] = 
        cfgGetStringFromID( MMS_STR_ID_RR_TEXT_TO);
    cfgStrValues[MMS_CFG_TEXT_READ] = 
        cfgGetStringFromID( MMS_STR_ID_RR_TEXT_READ);
    cfgStrValues[MMS_CFG_TEXT_MSGID] = 
        cfgGetStringFromID( MMS_STR_ID_RR_TEXT_MSGID);
    cfgStrValues[MMS_CFG_TEXT_SENT] = 
        cfgGetStringFromID( MMS_STR_ID_RR_TEXT_SENT);
    cfgStrValues[MMS_CFG_TEXT_YOUR_MSG] = 
        cfgGetStringFromID( MMS_STR_ID_RR_TEXT_YOUR_MSG);
    cfgStrValues[MMS_CFG_TEXT_WAS_READ] = 
        cfgGetStringFromID( MMS_STR_ID_RR_TEXT_WAS_READ);
    cfgStrValues[MMS_CFG_TEXT_SUBJECT] = 
        cfgGetStringFromID( MMS_STR_ID_RR_TEXT_SUBJECT);

    
    cfgIntValues[MMS_CFG_NETWORK_STATUS_ROAMING] = MMS_DEFAULT_ROAMING_STATUS;
    cfgIntValues[MMS_CFG_NETWORK_STATUS_BEARER] = MMS_DEFAULT_BEARER_STATUS;
    
    mSignalRegisterDst(M_FSM_CORE_CONFIG, cfgMain);
    
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM CORE CONFIG: initialized\n"));
} 






static void cfgMain(MmsSignal *sig) 
{
    switch (sig->type)
    {
    case MMS_SIG_CORE_READ_REGISTRY:
        cfgReadRegistry(sig->i_param);
        break;
    case MMS_SIG_CORE_REG_RESPONSE:
        cfgHandleRegistryResponse((MmsRegistryEntry*)sig->p_param);
        break;
    default:
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM CORE CONFIG: received unknown signal\n"));
        break;
    } 
    
    
    mSignalDelete(sig);
} 




void cfgTerminate(void)
{
    int i = 0;

    if (cfgFsm)
    {
        M_FREE(cfgFsm);
        cfgFsm = NULL;
    } 

    
    mSignalDeregister(M_FSM_CORE_CONFIG);

    for (i = 0; i < MMS_CFG_TOTAL_STR; i++)
    {
        if (cfgStrValues[i] != NULL)
        {
            M_FREE(cfgStrValues[i]);
            cfgStrValues[i] = NULL;
        } 
    } 
    
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM CORE CONFIG: terminated\n"));
} 





static void cfgReadRegistry(int retVal)
{
    cfgFsm->returnValue = retVal;
    cfgFsm->requests = 0;

    WE_REGISTRY_GET( WE_MODID_MMS, 0, (char *)MMS_REG_PATH_COM_STK, NULL);
    ++cfgFsm->requests;

    WE_REGISTRY_GET( WE_MODID_MMS, 0, (char *)MMS_REG_PATH_COM_MMSC, NULL);
    ++cfgFsm->requests;

    WE_REGISTRY_GET( WE_MODID_MMS, 0, (char *)MMS_REG_PATH_COM_GW, NULL);
    ++cfgFsm->requests;

    WE_REGISTRY_GET( WE_MODID_MMS, 0, (char *)MMS_REG_PATH_REPORT, NULL);
    ++cfgFsm->requests;

    WE_REGISTRY_GET( WE_MODID_MMS, 0, (char *)MMS_REG_PATH_REPORT_TEXT, NULL);
    ++cfgFsm->requests;

    WE_REGISTRY_GET( WE_MODID_MMS, 0, (char *)MMS_REG_PATH_ADDRESS, NULL);
    ++cfgFsm->requests;

    WE_REGISTRY_GET( WE_MODID_MMS, 0, (char *)MMS_REG_PATH_IMMEDIATE_RETRIEVAL, NULL);
    ++cfgFsm->requests;

    WE_REGISTRY_GET( WE_MODID_MMS, 0, (char *)MMS_REG_PATH_CLIENT, NULL);
    ++cfgFsm->requests;

    WE_REGISTRY_GET( WE_MODID_MMS, 0, (char *)MMS_REG_PATH_RETRIEVAL, NULL);
    ++cfgFsm->requests;

    WE_REGISTRY_GET( WE_MODID_MMS, 0, (char *)MMS_REG_PATH_NETWORK_STATUS, NULL);
    ++cfgFsm->requests;
	
	
	WE_REGISTRY_GET( WE_MODID_MMS, 0, MMS_REG_PATH_MSISDN, MMS_REG_KEY_FROM_ADDRESS);
    ++cfgFsm->requests;
} 








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







static WE_BOOL storeSettings()
{
    void    *handle;
    int     i, j;

    
    handle = WE_REGISTRY_SET_INIT(WE_MODID_MMS);
    if (NULL == handle)
    {
        return FALSE;
    }

    for(i = 0; i<MMS_CFG_TOTAL_INT; i++)
    {
        if (cfgIntSet[i] == FALSE)
        {
            for (j = 0; j < MMS_CFG_TOTAL_INT; j++)
            {
                if (mmsRegistryMapInt[j].keyId == i)
                {
                    WE_REGISTRY_SET_PATH (handle, mmsRegistryMapInt[j].regPath);
                    WE_REGISTRY_SET_ADD_KEY_INT(handle, mmsRegistryMapInt[j].regKeyName,
                        (WE_INT32) cfgIntValues[i]);
                } 
            } 
            cfgIntSet[i] = TRUE;
        } 
    } 

    for(i = 0; i<MMS_CFG_TOTAL_STR; i++)
    {
        if (cfgStrSet[i] == FALSE)
        {
            for (j = 0; j < MMS_CFG_TOTAL_STR; j++)
            {
                if (mmsRegistryMapStr[j].keyId == i)
                {
                    WE_REGISTRY_SET_PATH (handle, mmsRegistryMapStr[j].regPath);
                    setRegStr(cfgStrValues[i], handle, mmsRegistryMapStr[j].regKeyName);
                } 
            } 
            cfgStrSet[i] = TRUE;
        } 
    } 
    
    
    WE_REGISTRY_SET_COMMIT(handle);   
    WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MMS,
        "(%s) (%d) Registry updated\n", __FILE__, __LINE__));

    
    WE_REGISTRY_SUBSCRIBE (WE_MODID_MMS, 1, "/MMS", NULL,  TRUE);
        
    return TRUE;
} 






static void cfgHandleRegistryResponse(MmsRegistryEntry* p)
{
    MmsRegistryEntry* regEntry = p;
    MmsRegistryEntry* prev;
    int               key;

    if (!cfgFsm->isStarted)
    {
        ++cfgFsm->responses;
    } 

    
    while (regEntry)
    {
        if (regEntry->type == WE_REGISTRY_TYPE_INT)
        {
            if ((key = getKeyIdFromNameInt(regEntry->key)) != -1)
            {
                cfgIntValues[key] = regEntry->value_i;
                cfgIntSet[key] = TRUE;

                if (cfgFsm->isStarted)
                {
                    afterChangeCheckInt( (MmsConfigInt)key);
                } 
            } 
        }
        else if (regEntry->type == WE_REGISTRY_TYPE_STR)
        {
            if ((key = getKeyIdFromNameStr(regEntry->key)) != -1)
            {
                if (cfgStrValues[key] != NULL)
                {
                    M_FREE(cfgStrValues[key]);
                } 

                cfgStrValues[key] = M_ALLOC(regEntry->value_bv_length);
                memcpy(cfgStrValues[key], regEntry->value_bv, 
                    regEntry->value_bv_length);
                cfgStrSet[key] = TRUE;

                if (cfgFsm->isStarted)
                {
                    afterChangeCheckStr( (MmsConfigStr)key);
                } 
            } 
        } 

        prev = regEntry;
        regEntry = regEntry->next;

        M_FREE(prev->key);
        M_FREE(prev->path);
        M_FREE(prev->value_bv);
        M_FREE(prev);
    } 

    if (cfgFsm->responses == cfgFsm->requests)
    {
        


        if (!storeSettings()) 
        {
            WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MMS,
                "(%s) (%d) Registry updated failed.\n", __FILE__, __LINE__));
        }
        
        cfgFsm->isStarted = TRUE;

        
        M_SIGNAL_SENDTO_I( M_FSM_CORE_MAIN, (int)MMS_SIG_MAIN_START, 
            cfgFsm->returnValue);
    } 
} 







static int getKeyIdFromNameInt(const char* key)
{
    int i;
    int result = -1;

    for (i = 0; i < MMS_CFG_TOTAL_INT; i++)
    {
        if (we_cmmn_strcmp_nc( key, mmsRegistryMapInt[i].regKeyName) == 0)
        {
            result = mmsRegistryMapInt[i].keyId;
            break;
        } 
    } 

    return result;
} 







static int getKeyIdFromNameStr(const char* key)
{
    int i;
    int result = -1;

    for (i = 0; i < MMS_CFG_TOTAL_STR; i++)
    {
        if (we_cmmn_strcmp_nc( key, mmsRegistryMapStr[i].regKeyName) == 0)
        {
            result = mmsRegistryMapStr[i].keyId;
            break;
        } 
    } 

    return result;
} 
