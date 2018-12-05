#ifndef _PROTO_BUFFER_H__
#define _PROTO_BUFFER_H__
#include "p2p.h"
#include "p2pclient.h"
#ifdef __cplusplus
extern "C"
{
#endif
typedef struct _rawbuff_st{
	int    len;
	uchar *buffer;
}rawbuff_st,*rawbuff_t; 

// 该枚举需要重新定义，与前端或者后端统一
enum{ eProto_ping = 1, eProto_pong, eProto_initsession, eProto_auth, eProto_message, eProto_jobme, eProto_nojob, eProto_initack };

int protobuffer_svr_send(p2p_t p2p,int msgid,void* pdata);
int protobuffer_clnt_send(p2pclient_t p2p,int msgid,void* pdata);
#ifdef __cplusplus
}
#endif
#endif