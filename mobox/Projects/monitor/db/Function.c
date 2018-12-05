#include "stdafx.h"
#include "Function.h"
#include <string.h>
#include <stdlib.h>

using namespace std;

CFunction::CFunction(void)
{
}


CFunction::~CFunction(void)
{
}
int str2hms(char* str,int* hour,int* minute,int* second)
{
	char* psz = NULL;

	if(str == NULL || hour == NULL || minute == NULL || second == NULL) return 0;

	psz = str;
	while((*psz == ' '|| *psz =='\t') && *psz!='\0') psz++;
	if(*psz=='\0') return 0;

	*hour = atoi(psz);

	psz = strchr(psz,':');
	if(psz){
		psz++;
		*minute = atoi(psz);
	}else
		return 0;

	psz = strchr(psz,':');
	if(psz){
		psz++;
		*second = atoi(psz);
	}else
		return 0;

	return 1;
}

time_t SystimeToTime_t(SYSTEMTIME* pTime)
{
	tm temp;
	memset(&temp,0,sizeof(temp));

	temp.tm_year = pTime->wYear - 1900;
	temp.tm_mon  = pTime->wMonth -1;
	temp.tm_mday = pTime->wDay;
	temp.tm_hour = pTime->wHour;
	temp.tm_min  = pTime->wMinute;
	temp.tm_sec  = pTime->wSecond;	

	return mktime(&temp);
}

SYSTEMTIME Time_tToSystime(time_t t)
{
	tm temptm;
	localtime_s(&temptm,&t);

	SYSTEMTIME rt={
		temptm.tm_year + 1900,
		1+temptm.tm_mon,
		temptm.tm_mday,
		temptm.tm_wday,
		temptm.tm_hour,
		temptm.tm_min,
		temptm.tm_sec,
		0};

		return rt;
}