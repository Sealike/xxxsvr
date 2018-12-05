#include "protobuffer.h"
#include "client.h"
#include "globaldata.h"

extern int g_seqno2frontsvr;

static unsigned char checksum(unsigned char buffer[],int len)
{
	unsigned char data = 0;
	int i = 0 ;
	for(; i < len ;i++)
		data ^= buffer[i];

	return data;
}

// 向服务端发送
int protobuffer_send(p2pclient_t p2p,int msgid,void* pdata)
{
	if(p2p == NULL || msgid < 0) return 0;
	switch(msgid){
	case eProto_initsession: //向frontsvr发送连接认证
		{
			init_sess_t initsess = (init_sess_t)pdata;

			unsigned char buffer[256] = {0};
			int count  = 0;
			int endpos = 0;
			response_pkt_p pkt;
			if (initsess == NULL){
				count += sprintf_s(buffer + count, sizeof(buffer) - count, "$initsess,%s,%s{\"role\":\"%s\",\"seqno\":\"%d\"}", "monitor", "frontsvr","monitor",g_seqno2frontsvr);
			}else{
				count += sprintf_s(buffer + count, sizeof(buffer) - count, "$initsess,%s,%s{\"role\":\"%s\",\"seqno\":\"%d\"}", initsess->from, initsess->to,initsess->from,g_seqno2frontsvr);
			}
			count += sprintf_s(buffer+count,sizeof(buffer)-count,"*%02X\r\n",checksum(buffer,count));
			endpos = count;
			buffer[endpos+1] = '\0';

			g_seqno2frontsvr++;

			pkt = response_pkt_new(p2p->pktpool,endpos+1);
			memcpy(pkt->data,buffer,endpos+1);
			pkt->len = endpos+1;

			p2pclient_write(p2p,pkt);
		}
		break;

	case eProto_auth:
	{

		unsigned char buffer[256] = { 0 };
		int count = 0;
		int endpos = 0;
		response_pkt_p pkt;

		count += sprintf_s(buffer + count, sizeof(buffer) - count, "$auth,%s,%s,{\"pwd\":\"%s\",\"name\":\"%s\",\"seqno\":\"%d\"}", "monitor", "frontsvr", "123456", "monitor", g_seqno2frontsvr);

		count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum(buffer, count));
		endpos = count;
		buffer[endpos + 1] = '\0';

		pkt = response_pkt_new(p2p->pktpool, endpos + 1);
		memcpy(pkt->data, buffer, endpos + 1);
		pkt->len = endpos + 1;

		p2pclient_write(p2p, pkt);
		g_seqno2frontsvr++;

		log_write(client()->log, LOG_NOTICE, "%s.", buffer);
	}
	break;

	case eProto_ping: //向服务端发送ping，如router或者storagesvr
		{
			char * dest = p2p->sess.sname;
			unsigned char buffer[256];
			int count = 0;
			int check = 0;
			int endpos = 0;
			response_pkt_p pkt;

			/*count += sprintf_s(buffer+count,sizeof(buffer)-count,"$ping,{\"from\":\"%s\",\"to\":\"%s\"}","smcache","router");*/
			count += sprintf_s(buffer + count, sizeof(buffer) - count, "$ping,{\"from\":\"%s\",\"to\":\"%s\"}", "monitor", dest);
			count += sprintf_s(buffer+count,sizeof(buffer)-count,"*%02X\r\n",checksum(buffer,count));
			endpos = count;
			buffer[endpos+1] = '\0';
			pkt = response_pkt_new(p2p->pktpool,endpos+1);
			memcpy(pkt->data,buffer,endpos+1);
			pkt->len = endpos+1;

			p2pclient_write(p2p,pkt);
		}
		break;

	default:
		return 0;
	}

	return 1;
}

int protobuffer_send_p2p(p2p_t p2p, int msgid, void* pdata)
{
	if (p2p == NULL || msgid < 0) return 0;
	switch (msgid){
	case eProto_initack:
	{
		init_ack_t initack = (init_ack_t)pdata;

		unsigned char buffer[256] = { 0 };
		int count = 0;
		int endpos = 0;
		response_pkt_p pkt;
		if (initack == NULL){
			count += sprintf_s(buffer + count, sizeof(buffer) - count, "$initack,%s,%s{\"rolse\":\"%s\",\"rslt\":\"%s\",\":seqno\":\"%d\"}", "monitor", "moniclient");//obsolete..
		}
		else{
			count += sprintf_s(buffer + count, sizeof(buffer) - count, "$initack,%s,%s,{\"role\":\"%s\",\"rslt\":\"%s\",\"seqno\":\"%d\"}", initack->from, initack->to, initack->role, initack->ifok ? "ok" : "fail", initack->seqno);
		}
		count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum(buffer, count));
		endpos = count;
		buffer[endpos + 1] = '\0';

		pkt = response_pkt_new(client()->pktpool, endpos + 1);
		memcpy(pkt->data, buffer, endpos + 1);
		pkt->len = endpos + 1;

		p2p_stream_push(p2p, pkt);
		log_write(client()->log, LOG_NOTICE, "%s", buffer);
	}
	break;

	case eProto_pong:
	{
		unsigned char buffer[256];
		int count = 0;
		int check = 0;
		int endpos = 0;
		response_pkt_p pkt;
		pong_t pong = (pong_t)pdata;
		count += sprintf_s(buffer + count, sizeof(buffer) - count, "$pong,{\"from\":\"%s\",\"to\":\"%s\"}", pong->from, pong->to);
		count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum(buffer, count));
		endpos = count;
		buffer[endpos + 1] = '\0';
		pkt = response_pkt_new(client()->pktpool, endpos + 1);
		memcpy(pkt->data, buffer, endpos + 1);
		pkt->len = endpos + 1;

		p2p_stream_push(p2p, pkt);
		log_write(logger(), LOG_NOTICE, "send pong to %s.", pong->to);
	}
	break;
	case eProto_authack:
	{
		unsigned char buffer[256];
		int count = 0;
		int check = 0;
		int endpos = 0;
		response_pkt_p pkt;
		auth_t auth = (auth_t)pdata;

		count += sprintf_s(buffer + count, sizeof(buffer) - count, "$authack,monitor,%s,{\"rslt\":\"%s\",\"error\":\"%s\",\"seqno\" : \"%d\"}", auth->peername, auth->ok ? "ok" : "fail", auth->ok ? "":auth->err,auth->seqno);
		count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum(buffer, count));
		endpos = count;
		buffer[endpos + 1] = '\0';
		pkt = response_pkt_new(client()->pktpool, endpos + 1);
		memcpy(pkt->data, buffer, endpos + 1);
		pkt->len = endpos + 1;

		p2p_stream_push(p2p, pkt);
	}
	break;

	case eProto_offline:
	{
		unsigned char buffer[512];
		int count = 0;
		int check = 0;
		int endpos = 0;
		response_pkt_p pkt;
		auth_t auth = (auth_t)pdata;

		count += sprintf_s(buffer + count, sizeof(buffer) - count, "$offline,monitor,%s,{\"rslt\":\"%s\",\"error\":\"%s\",\"seqno\" : \"%d\"}", auth->peername, "fail", auth->err, auth->seqno);
		count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum(buffer, count));
		endpos = count;
		buffer[endpos + 1] = '\0';
		pkt = response_pkt_new(client()->pktpool, endpos + 1);
		memcpy(pkt->data, buffer, endpos + 1);
		pkt->len = endpos + 1;

		p2p_stream_push(p2p, pkt);
	}
	break;

	default:
		return 0;		
	}

	return 1;
}