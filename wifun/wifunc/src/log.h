
#ifndef _LOG_H
#define _LOG_H

#define WIFUNCLOG_MODULE

/* Logging levels */
#define WIFUNC_LOG_DETAIL_LOW              0
#define WIFUNC_LOG_DETAIL_MEDIUM        1
#define WIFUNC_LOG_DETAIL_HIGH             2

#ifdef WIFUNCLOG_MODULE

void Wifunc_LogMsg(int iType, unsigned char ucId, const char * pcFormat,...);

void Wifunc_LogData (int iType, unsigned charucId, const char *pucData, int iDataLen);

#define WIFUNC_LOG_MSG(x)                              Wifunc_LogMsg x
#define WIFUNC_LOG_DATA(e, m, d, l)                Wifunc_LogData(e, m, d, l)
#else
#define WIFUNC_LOG_MSG(x)
#define WIFUNC_LOG_DATA(level, m, d, l)
#endif


#endif  //_LOG_H

