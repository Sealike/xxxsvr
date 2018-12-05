#include "protobuffer.h"
#include "instapp.h"
#include "responsepkt.h"
#include "jsonstring.h"
#include "bdjobber_callback.h"


extern instapp_t g_instapp;
//extern int Seqno2FrontsvrManage(void);



static unsigned char checksum(unsigned char buffer[],int len)
{
	unsigned char data = 0;
	int i = 0 ;
	for(; i < len ;i++)
		data ^= buffer[i];

	return data;
}

// 服务器向客户端发送，即bdsvr向bdjobber发送
int protobuffer_svr_send(p2p_t p2p,int msgid,void* pdata)
{
	if(p2p == NULL || msgid < 0) return 0;

	switch(msgid){

	case eProto_initack:
		{
			unsigned char buffer[512];
			int count = 0;
			int check = 0;
			
			response_pkt_p pkt;
			count += sprintf_s(buffer+count,sizeof(buffer)-count,"$initack,{\"from\":\"%s\",\"to\":\"%s\",\"rslt\":\"%s\"}","bdsvr",(char*)pdata,"ok");
			count += sprintf_s(buffer+count,sizeof(buffer)-count,"*%02X\r\n",checksum(buffer,count));
			
			pkt = response_pkt_new(g_instapp->pktpool,count);
			memcpy(pkt->data,buffer,count);
			pkt->len = count;
			p2p_stream_push(p2p,pkt);
			log_debug(ZONE, "send <init ok> to bdjobber.");

		}
		break;

	case eProto_pong:
		{
			unsigned char buffer[256];
			int count = 0;
			int check = 0;
			response_pkt_p pkt;
			
			count += sprintf_s(buffer+count,sizeof(buffer)-count,"$pong,{\"from\":\"%s\",\"to\":\"%s\"}","bdsvr",(char*)pdata);
			count += sprintf_s(buffer+count,sizeof(buffer)-count,"*%02X\r\n",checksum(buffer,count));
			
			pkt = response_pkt_new(g_instapp->pktpool,count);
			memcpy(pkt->data,buffer,count);
			pkt->len = count;
			p2p_stream_push(p2p,pkt);
			log_debug(ZONE, "send <pong> to bdjobber.");
		}
		break;

	case eProto_nojob:
		{
			unsigned char buffer[256];
			int count = 0;
			int check = 0;
			response_pkt_p pkt;

			count += sprintf_s(buffer + count, sizeof(buffer) - count, "$NOJOB");
			count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum(buffer, count));

			pkt = response_pkt_new(g_instapp->pktpool, count);
			memcpy(pkt->data, buffer, count);
			pkt->len = count;
			p2p_stream_push(p2p, pkt);
		}
		break;

	default:
		break;
	}

	return 0;
}


// 客户端向服务器发送，即bdsvr向frontsvr发送
int protobuffer_clnt_send(p2pclient_t p2p,int msgid,void* pdata)
{
	if(p2p == NULL || msgid < 0) return 0;

	switch(msgid){
	case eProto_initsession:		//向frontsvr发送初始化连接
		{
			unsigned char buffer[256] = { 0 };
			int count = 0;
			int endpos = 0;
			response_pkt_p pkt;
			count += sprintf_s(buffer + count, sizeof(buffer) - count, "$initsess,%s,%s,{\"role\":\"%s\",\"seqno\":\"%d\"}", "bdsvr", "frontsvr", g_instapp->auth_role, g_instapp->seqno2frontsvr++);
			count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum(buffer, count));
			endpos = count;
			buffer[endpos + 1] = '\0';
			pkt = response_pkt_new(p2p->pktpool, endpos + 1);
			memcpy(pkt->data, buffer, endpos + 1);
			pkt->len = endpos + 1;

			p2pclient_write(p2p, pkt);
			log_debug(ZONE, "send <initsession> to frontsvr.");
			Seqno2FrontsvrManage();
		}
		break;

	case eProto_auth:		// 向frontsvr发送登录认证
	{
		unsigned char buffer[256] = { 0 };
		int count = 0;
		int endpos = 0;
		response_pkt_p pkt;
		count += sprintf_s(buffer + count, sizeof(buffer) - count, "$auth,%s,%s,{\"pwd\":\"%s\",\"name\":\"%s\",\"seqno\":\"%d\"}", "bdsvr", "frontsvr", g_instapp->auth_pwd,g_instapp->auth_name,g_instapp->seqno2frontsvr++);
		count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum(buffer, count));
		endpos = count;
		buffer[endpos + 1] = '\0';
		pkt = response_pkt_new(p2p->pktpool, endpos + 1);
		memcpy(pkt->data, buffer, endpos + 1);
		pkt->len = endpos + 1;
		
		p2pclient_write(p2p, pkt);
		log_write(g_instapp->log,LOG_NOTICE, "send auth to frontsvr:%s",buffer);
		//Seqno2FrontsvrManage();
	}
	break;

	case eProto_ping: //向frontsvr发送ping
		{
			unsigned char buffer[256];
			int count = 0;
			int check = 0;
			int endpos = 0;
			response_pkt_p pkt;

			count += sprintf_s(buffer+count,sizeof(buffer)-count,"$ping,{\"from\":\"%s\",\"to\":\"%s\"}",g_instapp->auth_name,"frontsvr");
			count += sprintf_s(buffer+count,sizeof(buffer)-count,"*%02X\r\n",checksum(buffer,count));
			endpos = count;
			buffer[endpos+1] = '\0';
			pkt = response_pkt_new(p2p->pktpool,endpos+1);
			memcpy(pkt->data,buffer,endpos+1);
			pkt->len = endpos+1;

			p2pclient_write(p2p,pkt);
			//log_debug(ZONE, "send <ping> to frontsvr.");
		}
		break;

	default:
		break;
	}
	return 0;
}