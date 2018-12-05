#include "Instance.h"
#include "connection/ComConnection.h"

CInstance theInstance;

CInstance::CInstance(void)
{

}

CInstance::~CInstance(void)
{

}

BOOL CInstance::open_com_connection(const int nPort,const int nBaudRate,const int nDataBits,const int nStopBits,BOOL bIfParity,const int nParity,IConnection**ppConnetion)
{
	return CComConnection::OpenAConnection(nPort,nBaudRate,nDataBits,nStopBits,bIfParity,nParity,ppConnetion);
}

PROTOINTERFACE_API  extern BOOL __cdecl open_com_connection(const int nPort,const int nBaudRate,const int nDataBits,const int nStopBits,BOOL bIfParity,const int nParity,IConnection**ppConnetion)
{
	return theInstance.open_com_connection(nPort,nBaudRate,nDataBits,nStopBits,bIfParity,nParity,ppConnetion);
}
