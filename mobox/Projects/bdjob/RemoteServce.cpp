#include "RemoteServce.h"
#include <time.h>
#include "bdexplain.h"


BYTE ComputeCheckSumRdss21(BYTE *Buff, int Len)
{
	BYTE f_bCheckSum = 0;
	for(int i=0; i<Len; i++)
	{
		f_bCheckSum ^= Buff[i];
	}
	return f_bCheckSum;
}

CRemoteInstance g_Remoteinstance;

CRemoteInstance::CRemoteInstance(void)
{
	m_bdarray = CBDDeviceArray::Get();
	m_nRemoteStatus = __remote_nojob;
}


CRemoteInstance::~CRemoteInstance(void)
{
}

