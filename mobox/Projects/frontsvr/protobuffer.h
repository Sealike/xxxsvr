#ifndef _PROTO_BUFFER_H__
#define _PROTO_BUFFER_H__
#include "p2pclient.h"
#include "p2p.h"

enum{ eProto_ping = 1, eProto_initsession, eProto_pong, eProto_auth, eProto_initack, eProto_offline, eProto_authack };

int protobuffer_send(p2pclient_t p2p, int msgid, void* pdata);		// protobuffer_clnt_send
int protobuffer_send_p2p(p2p_t p2p, int msgid, void* pdata);		// protobuffer_svr_send

#endif