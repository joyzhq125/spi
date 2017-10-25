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
 * We_Log.c
 *
 * Created by Henrik Olsson
 *
 * Revision  history:
 *   020903, HON: WE_LogSigStr and WE_LogSigByte now handles null as value-parameter.
 *
 */

#include "We_Lib.h"
#include "We_Log.h"   
#include "We_Mem.h"


/**********************************************************************
 * Signal Logging
 **********************************************************************/

#ifdef WE_LOG_SIGNALS
WE_BOOL WE_LogSigDoLog = TRUE;


LogSigMap WE_LogSigMap[LS_ITEMS + 1] = {
   { LS_UINT,    "UINT"},
   { LS_UINT8,   "UINT8"},
   { LS_UINT16,  "UINT16"},
   { LS_UINT32,  "UINT32"},
   { LS_INT,     "INT"},
   { LS_INT8,    "INT8"},
   { LS_INT16,   "INT16"},
   { LS_INT32,   "INT32"},
   { LS_FLOAT32, "FLOAT32"},
   { LS_BYTES,   "BYTES"},
   { LS_STR,     "STR"},
   { LS_VOID,    "VOID"},
   { LS_PTR,     "PTR"},
   { LS_COLL,    "COLL"},
   { LS_ITEMS,    NULL}
};

const char szCrLfIndent[] = "\r\n          ";


int WE_LogSigCalcCrLfIndentSize(LogSigHelper* pObj)
{
  size_t crLfIndentSize =  strlen("\r\n")  + ((pObj->collLevel + 1) * strlen("  ") );
  if(crLfIndentSize > strlen(szCrLfIndent))
  {
    crLfIndentSize = strlen(szCrLfIndent);
  }
  return crLfIndentSize;
}

void WE_LogSigEnlargeLogStr(LogSigHelper* pObj)
{
  char* pszStr = NULL;

  pObj->maxStrLen += LOG_SIG_CHUNK_SIZE;
  
  pszStr = (char*)WE_MEM_ALLOC((WE_UINT8)pObj->allocator, pObj->maxStrLen);
  memcpy(pszStr, pObj->pszStr, pObj->strLen);      
  WE_MEM_FREE((WE_UINT8)pObj->allocator, pObj->pszStr);
  pObj->pszStr = pszStr;
}

void WE_LogSigBegin(LogSigHelper* pObj, const char* signalName, WE_UINT8 when, WE_UINT8 src, WE_UINT8 dest) 
{
   char szStr[256];
   int size;
   int doLog = pObj->doLog;
   memset(pObj, 0x00, sizeof(*pObj));
   pObj->doLog = doLog;
   pObj->maxStrLen = 0;
   pObj->src = src;
   pObj->dest = dest;
   pObj->allocator = (when == LS_SEND) ? src : dest;
   size = sprintf(szStr, "<signal sname=\"%s\" when=\"%s\" src=\"%u\" dst=\"%u\">",
      signalName, (when == LS_RECV)?"recv":"send", src, dest);
   while((pObj->strLen + size + 1) > pObj->maxStrLen)
   {
     WE_LogSigEnlargeLogStr(pObj);
   }
   memcpy(pObj->pszStr, szStr, size);
   pObj->pszStr[size] = 0;
   pObj->strLen = size;
}


void WE_LogSigParam(LogSigHelper* pObj, const char* paramName, int type, void* paramValue, int paramSize)
{
   char szStr[256];
   char szValue[256];
   int elementLen; 
   int crLfIndentSize;
   char szFormat1[] = "<param name=\"%s\" type=\"%s\" addr=\"0x%8.8X\" value=\"%s\"/>";
   /* BEGIN A fix to avoid paramType paramSize misiatch*/
   switch(type)
   {
     case LS_UINT:
     case LS_UINT8:
     case LS_UINT16:
     case LS_UINT32:
     {
       switch(paramSize)
       {
         case sizeof(WE_UINT8):
         {
           type = LS_UINT8;
           break;
         }
         case sizeof(WE_UINT16):
         {
           type = LS_UINT16;
           break;
         }
         case sizeof(WE_UINT32):
         {
           type = LS_UINT32;
           break;
         }
       }
       break;
     }
     case LS_INT:
     case LS_INT8:
     case LS_INT16:
     case LS_INT32:
     {
       switch(paramSize)
       {
         case sizeof(WE_INT8):
         {
           type = LS_INT8;
           break;
         }
         case sizeof(WE_INT16):
         {
           type = LS_INT16;
           break;
         }
         case sizeof(WE_INT32):
         {
           type = LS_INT32;
           break;
         }
       }
       break;
     }
   }
   /* END */
 

   switch(type)
   {
      case LS_UINT8:
      {
         sprintf(szValue, "%u", *(WE_UINT8*)paramValue);
         break;
      }
      case LS_UINT16:
      {
         sprintf(szValue, "%u", *(WE_UINT16*)paramValue);
         break;
      }
      case LS_UINT32:
      {
         sprintf(szValue, "%lu", *(WE_UINT32*)paramValue);
         break;
      }
      case LS_INT8:
      {
         sprintf(szValue, "%d", *(WE_INT8*)paramValue);
         break;
      }
      case LS_INT16:
      {
         sprintf(szValue, "%d", *(WE_INT16*)paramValue);
         break;
      }
      case LS_INT32:
      {
         sprintf(szValue, "%ld", *(WE_INT32*)paramValue);
         break;
      }
      case LS_FLOAT32:
      {
         sprintf(szValue, "%f", *(WE_FLOAT32*)paramValue);
         break;
      }
      case LS_BYTES:
      {
         int maxLogBytes = 8;
         int i;
         int strLen = 0;
         if(paramValue)
         {
            WE_UINT8* data = *(WE_UINT8 **)paramValue;
            for(i = 0; i < paramSize && i < maxLogBytes; i++)
            {
              sprintf(&szValue[i*3], "%2.2x ", data[i]);
            }
            if(i > 1)
            {
               strLen = (i*3) - 1;  
               szValue[strLen] = 0x00;
            }
            if(paramSize > maxLogBytes)
            {
               strcpy(&szValue[strLen],"...");
               strLen += 3;
            }
         }
         else
         {
            strcpy(szValue, "(null)");
            strLen = strlen("(null)"); 
         }
         sprintf(&szValue[strLen],"\" len=\"%d",paramSize);

         break;
      }
      case LS_STR:
      {
         int maxLogChars = 16;
         sprintf(szValue, "%.15s%s\" len=\"%d", 
               *((char**)paramValue) ? *((char**)paramValue) : "(null)", 
               ((paramSize) > maxLogChars) ? "..." : "", paramSize);
         break;
      }
      case LS_VOID:
      {
         strcpy(szValue, "");
         break;
      }
      case LS_PTR:
      {
         sprintf(szValue, "0x%8.8X", *(void**)paramValue);
         break;
      }
   }

   crLfIndentSize =  WE_LogSigCalcCrLfIndentSize(pObj);
   elementLen = sprintf(szStr, szFormat1, paramName, WE_LogSigMap[type].str, paramValue, szValue);
   while((pObj->strLen + elementLen + crLfIndentSize + 1) > pObj->maxStrLen)
   {
     WE_LogSigEnlargeLogStr(pObj);
   }
   {
      memcpy(&pObj->pszStr[pObj->strLen], szCrLfIndent, crLfIndentSize);
      pObj->strLen += crLfIndentSize;
      memcpy(&pObj->pszStr[pObj->strLen], szStr, elementLen);
      pObj->strLen += elementLen;
      pObj->pszStr[pObj->strLen] = 0;
   }
}
   
   


void WE_LogSigColl(LogSigHelper* pObj, const char* collName)
{
  char szStr[256];
  int size;
  int crLfIndentSize;
  crLfIndentSize =  WE_LogSigCalcCrLfIndentSize(pObj);
  size = sprintf(szStr, "<Param name=\"%s\" type=\"COLL\">", collName);
  while((pObj->strLen + size + crLfIndentSize + 1) > pObj->maxStrLen)
  {
    WE_LogSigEnlargeLogStr(pObj);
  }

  memcpy(&pObj->pszStr[pObj->strLen], szCrLfIndent, crLfIndentSize);
  pObj->strLen += crLfIndentSize;
  memcpy(&pObj->pszStr[pObj->strLen], szStr, size);
  pObj->strLen += size;
  pObj->pszStr[pObj->strLen] = 0;
  pObj->collLevel++;
}


void WE_LogSigCollEnd(LogSigHelper* pObj)
{
  int crLfIndentSize = 0;

  pObj->collLevel--;

  crLfIndentSize =  WE_LogSigCalcCrLfIndentSize(pObj);
  while((pObj->strLen + crLfIndentSize + (int)strlen("</param>") + 1) > pObj->maxStrLen)
  {
    WE_LogSigEnlargeLogStr(pObj);
  }
  memcpy(&pObj->pszStr[pObj->strLen], szCrLfIndent, crLfIndentSize);
  pObj->strLen += crLfIndentSize;
  memcpy(&pObj->pszStr[pObj->strLen], "</param>", strlen("</param>") + 1);
  pObj->strLen += strlen("</param>");
}


void WE_LogSigEnd(LogSigHelper* pObj)
{
  const char szCrLfSignal[] = "\r\n</signal>";

  while((pObj->strLen + (int)sizeof(szCrLfSignal)) > pObj->maxStrLen)
  {
    WE_LogSigEnlargeLogStr(pObj);
  }
  memcpy(&pObj->pszStr[pObj->strLen], szCrLfSignal, sizeof(szCrLfSignal));
  TPIa_logSignal((WE_UINT8)pObj->src, (WE_UINT8)pObj->dest, pObj->pszStr);
  WE_MEM_FREE((WE_UINT8)pObj->allocator, pObj->pszStr); 
}

#endif /*WE_LOG_SIGNALS*/


#ifdef WE_LOG_FC
WE_BOOL WE_LogFcDoLog = TRUE;
#endif /*WE_LOG_FC*/
