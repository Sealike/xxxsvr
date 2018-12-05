#ifndef _PROTO_BUFFER_H__
#define _PROTO_BUFFER_H__
#include "p2pclient.h"
#ifdef __cplusplus
extern "C"
{
#endif
enum{eProto_ping = 1,eProto_initsession,eProto_other};

int protobuffer_send(p2pclient_t p2p,int msgid,void* pdata);
#ifdef __cplusplus
}
#endif

#endif