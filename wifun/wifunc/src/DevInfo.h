#ifndef DEVINFO_H
#define DEVINFO_H

typedef struct tag_stDevInfo
{
	int devid;
	char unit_type;
	char unit_num;
	int  result; //status 
}stDevInfo;


void InitDevInfo();
int GetDevInfo(int devid,char unit_type,char unit_num);
int SetDevInfo(int devid,char unit_type,char unit_num,int result);
int DelDevInfo(int devid,char unit_type,char unit_num);

#endif //DEVINFO_H