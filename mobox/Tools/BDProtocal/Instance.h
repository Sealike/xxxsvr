#pragma once
#include "ProtoInterface.h"
#include "protocal/ChnavtectGlobal.h"
#include "protocal/protocaldef.h"

class CInstance : public CSinker
{
public:
	CInstance(void);
	~CInstance(void);

	virtual void  Update(const UINT uMsgId,const void * pMsgBuffer,int para);
	virtual void  Log(const char * pStrLogInfo);

	int prorotocal_init(pTFunCallback pCB);
	int protocal_settype(char *szProtocalname);
	int protocal_encode_msg(const void* msg,int msgtype,outbuffer_t frame,int bufferlen);
	int protocal_process(inbuffer_t buffer,int datalen);
	int protocal_release();
protected:
	void _init();
	void _clear();
private:
	pTFunCallback  m_pCB;
	char *         m_sz_proto_name;
	CSearchExplainer* m_seachexp;
	char m_msg[2048];
};

