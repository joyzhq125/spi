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
 * We_Log.h
 *
 * Created by Henrik Olsson
 *
 * Revision  history:
 * 
 *
 */

#ifndef _we_log_h
#define _we_log_h

#ifndef _we_int_h
#include "We_Int.h"
#endif
#ifndef _we_cmmn_h
#include "We_Cmmn.h"
#endif

#ifndef _we_cfg_h
#include "We_Cfg.h"
#endif

/**********************************************************************
 * Module Logging
 **********************************************************************/

/* Logging levels */
#define WE_LOG_DETAIL_LOW              TPI_LOG_TYPE_DETAILED_LOW
#define WE_LOG_DETAIL_MEDIUM           TPI_LOG_TYPE_DETAILED_MEDIUM
#define WE_LOG_DETAIL_HIGH             TPI_LOG_TYPE_DETAILED_HIGH


#ifdef WE_LOG_MODULE
#define WE_LOG_MSG(x)                              TPIa_logMsg x
#define WE_LOG_DATA(level, modId, data, dataLen)   TPIa_logData (level, modId, data, dataLen)
#else
#define WE_LOG_MSG(x)
#define WE_LOG_DATA(level, modId, data, dataLen)
#endif


/**********************************************************************
 * Signal Logging
 **********************************************************************/

enum LogSignalWhen{ LS_SEND, LS_RECV};
#define LOG_SIG_CHUNK_SIZE 512


#ifdef WE_LOG_SIGNALS   
typedef struct 
{
   char* pszStr;
   int   strLen;
   int   maxStrLen;
   int   collLevel;
   int   doLog;
   int   src;
   int   dest;
   int   allocator;
}LogSigHelper;

enum LogSigTypes
{
  LS_UINT,
  LS_UINT8,
  LS_UINT16,
  LS_UINT32,
  LS_INT,
  LS_INT8,
  LS_INT16,
  LS_INT32,
  LS_FLOAT32,
  LS_BYTES,
  LS_STR,
  LS_VOID,
  LS_PTR,
  LS_COLL,
  LS_ITEMS
};

typedef struct  
{
   int wid;
   char* str;
}LogSigMap;

void WE_LogSigBegin(LogSigHelper* pObj, const char* signalName, WE_UINT8 when, WE_UINT8 src, WE_UINT8 dest); 
void WE_LogSigParam(LogSigHelper* pObj, const char* paramName, int type, void* paramValue, int paramSize);
void WE_LogSigColl(LogSigHelper* pObj, const char* collName);
void WE_LogSigCollEnd(LogSigHelper* pObj);
void WE_LogSigEnd(LogSigHelper* pObj);
extern LogSigMap WE_LogSigMap[LS_ITEMS + 1];
extern WE_BOOL WE_LogSigDoLog;

#define WE_LOG_SIG_BEGIN(signalName, when, src, dest)\
{\
  LogSigHelper lsHelper;\
  lsHelper.doLog = WE_LogSigDoLog;\
  if(lsHelper.doLog) { WE_LogSigBegin(&lsHelper, signalName, when, src, dest); } 

#define WE_LOG_SIG_UINT(paramName, paramValue)\
  if(lsHelper.doLog) { WE_LogSigParam(&lsHelper, paramName, LS_UINT, &paramValue, sizeof(paramValue)); }

#define WE_LOG_SIG_UINT8(paramName, paramValue)\
  if(lsHelper.doLog) { WE_LogSigParam(&lsHelper, paramName, LS_UINT8, &paramValue, sizeof(paramValue)); }
   
#define WE_LOG_SIG_UINT16(paramName, paramValue)\
  if(lsHelper.doLog) { WE_LogSigParam(&lsHelper, paramName, LS_UINT16, &paramValue, sizeof(paramValue)); }
   
#define WE_LOG_SIG_UINT32(paramName, paramValue)\
  if(lsHelper.doLog) { WE_LogSigParam(&lsHelper, paramName, LS_UINT32, &paramValue, sizeof(paramValue)); }
   
#define WE_LOG_SIG_INT(paramName, paramValue)\
  if(lsHelper.doLog) { WE_LogSigParam(&lsHelper, paramName, LS_INT, &paramValue, sizeof(paramValue)); }
   
#define WE_LOG_SIG_INT8(paramName, paramValue)\
  if(lsHelper.doLog) { WE_LogSigParam(&lsHelper, paramName, LS_INT8, &paramValue, sizeof(paramValue)); }
   
#define WE_LOG_SIG_INT16(paramName, paramValue)\
  if(lsHelper.doLog) { WE_LogSigParam(&lsHelper, paramName, LS_INT16, &paramValue, sizeof(paramValue)); }
   
#define WE_LOG_SIG_INT32(paramName, paramValue)\
  if(lsHelper.doLog) { WE_LogSigParam(&lsHelper, paramName, LS_INT32, &paramValue, sizeof(paramValue)); }
   
#define WE_LOG_SIG_FLOAT(paramName, paramValue)\
  if(lsHelper.doLog) { WE_LogSigParam(&lsHelper, paramName, LS_FLOAT32, &paramValue, sizeof(paramValue)); }
   
#define WE_LOG_SIG_STR(paramName, paramValue)\
  if(lsHelper.doLog) { WE_LogSigParam(&lsHelper, paramName, LS_STR, &paramValue, paramValue ? (we_cmmn_utf8_strlen(paramValue) + 1) : 0); }

#define WE_LOG_SIG_BYTE(paramName, paramValue, paramLength)\
  if(lsHelper.doLog) { WE_LogSigParam(&lsHelper, paramName, LS_BYTES, &paramValue, paramValue ? paramLength : 0); }

#define WE_LOG_SIG_VOID(paramName)\
  if(lsHelper.doLog) { WE_LogSigParam(&lsHelper, paramName, LS_VOID, NULL, 0); }

#define WE_LOG_SIG_PTR(paramName, paramValue)\
  if(lsHelper.doLog) { WE_LogSigParam(&lsHelper, paramName, LS_PTR, &paramValue, 0); }

#define WE_LOG_SIG_COLL(collName)\
  if(lsHelper.doLog) { WE_LogSigColl(&lsHelper, collName); }

#define WE_LOG_SIG_COLL_END()\
  if(lsHelper.doLog) { WE_LogSigCollEnd(&lsHelper); }


#define WE_LOG_SIG_END()\
  if(lsHelper.doLog) { WE_LogSigEnd(&lsHelper); }\
}

#else /*WE_LOG_SIGNALS*/

#define WE_LOG_SIG_BEGIN(signalName, when, src, dest)
#define WE_LOG_SIG_UINT(paramName, paramValue)
#define WE_LOG_SIG_UINT8(paramName, paramValue)
#define WE_LOG_SIG_UINT16(paramName, paramValue)
#define WE_LOG_SIG_UINT32(paramName, paramValue)
#define WE_LOG_SIG_INT(paramName, paramValue)
#define WE_LOG_SIG_INT8(paramName, paramValue)
#define WE_LOG_SIG_INT16(paramName, paramValue)
#define WE_LOG_SIG_INT32(paramName, paramValue)
#define WE_LOG_SIG_FLOAT(paramName, paramValue)
#define WE_LOG_SIG_STR(paramName, paramValue)
#define WE_LOG_SIG_BYTE(paramName, paramValue, paramLength)
#define WE_LOG_SIG_VOID(paramName)
#define WE_LOG_SIG_PTR(paramName, paramValue)
#define WE_LOG_SIG_COLL(collName)
#define WE_LOG_SIG_COLL_END()
#define WE_LOG_SIG_END()

#endif /*WE_LOG_SIGNALS*/






#ifdef WE_LOG_FC
typedef struct 
{
   void* pCriticalSection;
   void* pLogFuncCallObj;
   int nFields;
   int doLog;
}LogFuncCallHelper;

typedef struct 
{
   int nArrays;
   int arrayIndexes[10];
}ArrayDescr;


void      
logFuncCallBegin(LogFuncCallHelper* pLogFuncCallHelper, char* pszName);

void            
logFuncCallPreCall(LogFuncCallHelper* pLogFuncCallHelper);

void      
logFuncCallEnd(LogFuncCallHelper* pLogFuncCallHelper);

void 
logFuncCallUint8(LogFuncCallHelper* pLogFuncCallHelper, const char* szName, const WE_UINT8* value, ArrayDescr* pArrayDescr);

void 
logFuncCallUint16(LogFuncCallHelper* pLogFuncCallHelper, const char* szName, const WE_UINT16* value, ArrayDescr* pArrayDescr);

void
logFuncCallUint32(LogFuncCallHelper* pLogFuncCallHelper, const char* szName, const WE_UINT32* value, ArrayDescr* pArrayDescr);

void
logFuncCallInt8(LogFuncCallHelper* pLogFuncCallHelper, const char* szName, const WE_INT8* value, ArrayDescr* pArrayDescr);

void
logFuncCallInt16(LogFuncCallHelper* pLogFuncCallHelper, const char* szName, const WE_INT16* value, ArrayDescr* pArrayDescr);

void
logFuncCallInt32(LogFuncCallHelper* pLogFuncCallHelper, const char* szName, const WE_INT32* value, int size, ArrayDescr* pArrayDescr);

void 
logFuncCallPtr(LogFuncCallHelper* pLogFuncCallHelper, const char* szName, const void** pValue, ArrayDescr* pArrayDescr);

void
logFuncCallString(LogFuncCallHelper* pLogFuncCallHelper, const char* szName, const char* value, ArrayDescr* pArrayDescr);


void
logFuncCallBytes(LogFuncCallHelper* pLogFuncCallHelper, const char* szName, const unsigned char* value, int length, ArrayDescr* pArrayDescr);
   extern WE_BOOL WE_LogFcDoLog;

   #define WE_LOG_FC_PARAMS_DECLARE LogFuncCallHelper lfcHelper  
   #define WE_LOG_FC_PARAMS_IMPLEMENT lfcHelper 
   #define WE_LOG_FC_DISPATCH_MAP_DECLARE(modName)\
   extern  int\
   modName##_callFunc(const char* szFuncName);
   #define WE_LOG_FC_DISPATCH_MAP_FUNC_PTR(modName) modName##_callFunc
   #define WE_LOG_FC_DISPATCH_MAP_BEGIN(modName)\
   int\
   modName##_callFunc(const char* szFuncName)\
   {\
      int callFuncRetVal = -1; 

   #define WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(fName)\
      if((callFuncRetVal == -1) && !we_cmmn_strcmp_nc(szFuncName, #fName))\
      {

   #define WE_LOG_FC_DISPATCH_MAP_ENTRY_END\
         callFuncRetVal = 0;\
      }


   #define WE_LOG_FC_DISPATCH_MAP_SUB_DISPATCH_MAP(fName)\
    if(callFuncRetVal == -1)\
    {\
       callFuncRetVal = fName##_callFunc(szFuncName);\
    }

   #define WE_LOG_FC_DISPATCH_MAP_END\
      szFuncName = NULL;\
      return callFuncRetVal;\
   }

   #define WE_LOG_FC_BEGIN(p)\
   LogFuncCallHelper lfcHelper;\
   lfcHelper.doLog = WE_LogFcDoLog;\
   if(lfcHelper.doLog) { logFuncCallBegin(&lfcHelper, #p); }

   #define WE_LOG_FC_PRE_IMPL\
   if(lfcHelper.doLog) { logFuncCallPreCall(&lfcHelper); }

   #define WE_LOG_FC_END\
   if(lfcHelper.doLog) { logFuncCallEnd(&lfcHelper); }


   #define WE_LOG_FC_UINT8(p, pArrayDescr) if(lfcHelper.doLog) { logFuncCallUint8(&lfcHelper, #p, &p, pArrayDescr); } 
   #define WE_LOG_FC_UINT16(p, pArrayDescr) if(lfcHelper.doLog) { logFuncCallUint16(&lfcHelper, #p, &p, pArrayDescr); } 
   #define WE_LOG_FC_UINT32(p, pArrayDescr) if(lfcHelper.doLog) { logFuncCallUint32(&lfcHelper, #p, &p, pArrayDescr); } 
   #define WE_LOG_FC_INT8(p, pArrayDescr) if(lfcHelper.doLog) { logFuncCallInt8(&lfcHelper, #p, &p, pArrayDescr); } 
   #define WE_LOG_FC_INT16(p, pArrayDescr) if(lfcHelper.doLog) { logFuncCallInt16(&lfcHelper, #p, &p, pArrayDescr); } 
   #define WE_LOG_FC_INT32(p, pArrayDescr) if(lfcHelper.doLog) { logFuncCallInt32(&lfcHelper, #p, (WE_INT32*)(&p), sizeof(p), pArrayDescr); } 
   #define WE_LOG_FC_INT(p, pArrayDescr)    if(lfcHelper.doLog) { logFuncCallInt32(&lfcHelper, #p, (WE_INT32*)(&p), sizeof(p), pArrayDescr); } 
   #define WE_LOG_FC_STRING(p, pArrayDescr) if(lfcHelper.doLog) { logFuncCallString(&lfcHelper, #p, p, pArrayDescr); }
   #define WE_LOG_FC_BYTES(p,l, pArrayDescr) if(lfcHelper.doLog) { logFuncCallBytes(&lfcHelper, #p, (unsigned char*)p, l, pArrayDescr); }
   #define WE_LOG_FC_PTR(p, pArrayDescr) if(lfcHelper.doLog) { logFuncCallPtr(&lfcHelper, #p, (const void**)&p, pArrayDescr); }
#else
   #define WE_LOG_FC_PARAMS_DECLARE 
   #define WE_LOG_FC_PARAMS_IMPLEMENT 
   #define WE_LOG_FC_DISPATCH_MAP_DECLARE(modName)
   #define WE_LOG_FC_DISPATCH_MAP_FUNC_PTR(modName) NULL
   #define WE_LOG_FC_DISPATCH_MAP_BEGIN(modName) 
   #define WE_LOG_FC_DISPATCH_MAP_ENTRY_BEGIN(fName) 
   #define WE_LOG_FC_DISPATCH_MAP_ENTRY_END 
   #define WE_LOG_FC_DISPATCH_MAP_SUB_DISPATCH_MAP(fName)
   #define WE_LOG_FC_DISPATCH_MAP_END
   #define WE_LOG_FC_BEGIN(p)
   #define WE_LOG_FC_PRE_IMPL
   #define WE_LOG_FC_END
   #define WE_LOG_FC_UINT8(p, pArrayDescr) 
   #define WE_LOG_FC_UINT16(p, pArrayDescr) 
   #define WE_LOG_FC_UINT32(p, pArrayDescr) 
   #define WE_LOG_FC_INT8(p, pArrayDescr) 
   #define WE_LOG_FC_INT16(p, pArrayDescr) 
   #define WE_LOG_FC_INT32(p, pArrayDescr) 
   #define WE_LOG_FC_INT(p, pArrayDescr) 
   #define WE_LOG_FC_STRING(p, pArrayDescr)
   #define WE_LOG_FC_BYTES(p,l, pArrayDescr)
   #define WE_LOG_FC_PTR(p, pArrayDescr) 
#endif

#endif /*_WE_LOG_H*/
