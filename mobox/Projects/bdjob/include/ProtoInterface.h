
/****************************************************************************************
 *��Ȩ���� (C), 2016, ������Զ����ǿ������λ���޹�˾����������Ȩ����
 *δ��������Զ����ǿ������λ���޹�˾����Ȩ��ɲ������Ը��ƣ��ü���ַ���Դ���롣
 ****************************************************************************************/

/****************************************************************************************
 *�ļ���:	ProtoInterface.h
 *����:		����2.1/4.0Э��ӿں���
 *�޸���ʷ:
 *2016.1.11 liuxiaoliang ����
 ****************************************************************************************/

#include "Head.h"

#ifndef _PROTO_INTERFACE_HEAD__
#define _PROTO_INTERFACE_HEAD__

#ifdef PROTOINTERFACE_EXPORTS
#define PROTOINTERFACE_API __declspec(dllexport)
#else
#define PROTOINTERFACE_API __declspec(dllimport)
#endif

/*
 *protocal names
 */
#define  _PROTOCAL_4_0   ("bdprotocal4_0")
#define  _PROTOCAL_2_1   ("bdprotocal2_1")

/*****************************************************************************************
 *c++ type library interface��and this is recommended.
 *
 *****************************************************************************************/
class  CSinker;

class  IMessager;

class  ISearchExplainer;

#ifdef __cplusplus
extern "C"
{
#endif
	/*
	 *making multiple calls is a good idea , you will get multiple explainers and corresponding messagers;   
	 *advising more than one sinkers to a messager is recommended.
	 */
	PROTOINTERFACE_API  extern ISearchExplainer*  __cdecl prorotocal_init_cpp(protocalname_t protocal,IMessager*& messager);

#ifdef __cplusplus
}
#endif



/*****************************************************************************************
 *c library interface, it is depreciated.
 *
 *****************************************************************************************/

typedef int (*pTFunCallback)(int msgid,char* msg, int msglen);

#ifdef __cplusplus
extern "C"
{
#endif

#define PROTO_SUCCESS			(0)
#define PROTO_FAIL				(1)

PROTOINTERFACE_API  extern int  __cdecl prorotocal_init(pTFunCallback pCB);

PROTOINTERFACE_API  extern int  __cdecl protocal_settype(protocalname_t szProtocalname);

PROTOINTERFACE_API  extern int  __cdecl protocal_encode_msg(const void* msg,int msgtype,outbuffer_t frame,int bufferlen);

PROTOINTERFACE_API  extern int  __cdecl protocal_process(const inbuffer_t buffer,int datalen);

PROTOINTERFACE_API  extern int  __cdecl protocal_release();

#ifdef __cplusplus
}
#endif

#endif