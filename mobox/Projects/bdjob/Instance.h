#pragma once
#include "ConnectionInterface.h"
#include "../BDProtocal/protocal/ChnavtectGlobal.h"

class CInstance
{
public:
	CInstance(void);
	~CInstance(void);
	BOOL open_com_connection(const int nPort,const int nBaudRate,const int nDataBits,const int nStopBits,BOOL bIfParity,const int nParity,IConnection**ppConnetion);
protected:

private:

};

