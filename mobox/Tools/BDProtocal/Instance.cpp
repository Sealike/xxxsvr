#include "Instance.h"
#include "protocal/Rdss2_1SearchExp.h"
#include "protocal/Rdss4_0SearchExp.h"
#include "protocal/Rdss2_1SearchExp_shangyu.h"

CInstance theInstance;

CInstance::CInstance(void)
{
	_init();
}

CInstance::~CInstance(void)
{
	_clear();
}

void CInstance::_init()
{
	m_pCB = NULL;
	m_sz_proto_name = NULL;
	m_seachexp = NULL;
}

void CInstance::_clear()
{
	m_pCB = NULL;

	if(m_sz_proto_name) free(m_sz_proto_name);	

	if(m_seachexp) delete m_seachexp;

	m_sz_proto_name = NULL;
	m_seachexp = NULL;
}

void  CInstance::Update(const UINT uMsgId,const void * pMsgBuffer,int para)
{
	if(m_pCB)
		m_pCB(uMsgId,(char*)pMsgBuffer,para);
}

void  CInstance::Log(const char * pStrLogInfo)
{

}

int CInstance::prorotocal_init(pTFunCallback pCB)
{
	m_pCB = pCB;
	return PROTO_SUCCESS;
}

int CInstance::protocal_settype(char *szProtocalname)
{	
	if(m_sz_proto_name != NULL && m_seachexp != NULL){
		delete m_seachexp;
		free(m_sz_proto_name);
	}

	m_sz_proto_name = _strdup(szProtocalname);

	if(strcmp(m_sz_proto_name,_PROTOCAL_2_1) == 0){
		m_seachexp = new CBDProtocal2_1();		
	}else if(strcmp(m_sz_proto_name,_PROTOCAL_4_0) == 0){
		m_seachexp = new CBDProtocal4_0();
	}

	m_seachexp->Advise(this);

	return PROTO_SUCCESS;
}

int CInstance::protocal_encode_msg(const void* msg,int msgtype,outbuffer_t frame,int bufferlen)
{
	return PROTO_FAIL;
}

int CInstance::protocal_process(const inbuffer_t buffer,int datalen)
{
	if(m_seachexp)
		m_seachexp->ProcessData((BYTE*)buffer,datalen);

	return PROTO_FAIL;
}

int CInstance::protocal_release()
{
	_clear();
	return PROTO_SUCCESS;
}


/*************************************************************
 *c style export functions..
 *
 *
 *
 ************************************************************/

PROTOINTERFACE_API extern int __cdecl prorotocal_init(pTFunCallback pCB)
{
	return theInstance.prorotocal_init(pCB);
}


PROTOINTERFACE_API extern int  __cdecl protocal_settype(char *szProtocalname)
{
	return  theInstance.protocal_settype(szProtocalname);
}

PROTOINTERFACE_API  extern int  __cdecl protocal_encode_msg(const void* msg,int msgtype,outbuffer_t frame,int bufferlen)
{
	return theInstance.protocal_encode_msg(msg,msgtype,frame,bufferlen);
}

PROTOINTERFACE_API  extern int  __cdecl protocal_process(const inbuffer_t buffer,int datalen)
{
	return theInstance.protocal_process(buffer,datalen);
}

PROTOINTERFACE_API  extern int  __cdecl protocal_release()
{
	return theInstance.protocal_release();
}

/*************************************************************
 *c++ style export types..
 *
 *
 *
 ************************************************************/
PROTOINTERFACE_API  extern ISearchExplainer*  __cdecl prorotocal_init_cpp(protocalname_t protocal,IMessager*& messager)
{
	ISearchExplainer* rtn = NULL;
	CSearchExplainer* pSearchExp = NULL;

	if(protocal == NULL) return NULL;

	if(strcmp(protocal,_PROTOCAL_2_1) == 0){
		pSearchExp = new CBDProtocal2_1();		
	}else if(strcmp(protocal,_PROTOCAL_4_0) == 0){
		pSearchExp = new CBDProtocal4_0();				
	}

	rtn = pSearchExp;
	messager = (IMessager*)pSearchExp;

	return rtn;
}