#pragma once
#include "BDDeviceArray.h"


#define __remote_nojob  0
#define __remote_hasjob 1



class CRemoteInstance
{
public:
	CRemoteInstance(void);
	~CRemoteInstance(void);
	CBDDeviceArray* GetBDArray() {return m_bdarray;}
	int  GetRemoteAgentStatus(){return m_nRemoteStatus;}
	void SetRemoteAgentStatus(int status){m_nRemoteStatus = status;}
private:
	CBDDeviceArray*    m_bdarray;
	int     m_nRemoteStatus;
};

extern CRemoteInstance g_Remoteinstance;