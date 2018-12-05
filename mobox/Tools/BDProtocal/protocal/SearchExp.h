#pragma once
#include "../Head.h"

class CFrameCallback{
public:
	virtual void notifyframe(const char* frame,int framelen) = 0;
};

class CSearchExp
{
public:	
	virtual void SetFrameCallback(CFrameCallback* cb) = 0;
	virtual int  ProcessData(const char byBuffData[] ,int nDataLen) =0;
	virtual int  EncodeFrame(const void * pMsg,const unsigned uMsgId,const int nMsgBlockLen, char byFrameBuff[],const int nFrameBuffLen) =0;	
	virtual BYTE DataExplain(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len) = 0;
	virtual int  GetAccumulateCnt(UINT uMsgId) = 0;
};

