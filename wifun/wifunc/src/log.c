
#include "Wifunc.h"


void Wifunc_LogMsg(int iType, unsigned char ucId, const char * pcFormat,...)
{
	va_list arglist;
	va_start(arglist,pcFormat);
	vprintf(pcFormat,arglist);
	va_end(arglist);

}


void Wifunc_LogData (int iType, unsigned charucId, const char *pucData, int iDataLen)
{
	int i = 0;
	if (NULL == pucData)
	{
		printf("POINT TO NULL! Buffer length : %d\n", iDataLen);
		return;
	}
	for(i=0;i<iDataLen;i++)
	{
		printf("%x", (*pucData));
		pucData++;
	}
}
