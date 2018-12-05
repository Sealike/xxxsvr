#pragma once
#include "type.h"

class CFunction
{
public:
	CFunction(void);
	~CFunction(void);
};

int  str2hms(char* str,int* hour,int* minute,int* second);
time_t SystimeToTime_t(SYSTEMTIME* pTime);
SYSTEMTIME Time_tToSystime(time_t t);
