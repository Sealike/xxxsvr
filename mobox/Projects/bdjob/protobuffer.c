#include "protobuffer.h"
#include "instapp.h"
#include "tool.h"

int protobuffer_send(p2pclient_t p2p,int msgid,void* pdata)
{
	if(p2p == NULL || msgid < 0) return 0;
	switch(msgid){
	case eProto_initsession: //向bdsvr发送连接认证
		{
			unsigned char buffer[256] = {0};
			int count = 0;
			int endpos = 0;
			response_pkt_p pkt;
			count += sprintf_s(buffer+count,sizeof(buffer)-count,"$initsess,{\"from\":\"%s\",\"to\":\"%s\"}","bdjob","bdsvr");
			count += sprintf_s(buffer+count,sizeof(buffer)-count,"*%02X\r\n",checksum(buffer,count));
			endpos = count;
			buffer[endpos+1] = '\0';
			pkt = response_pkt_new(p2p->pktpool,endpos+1);
			memcpy(pkt->data,buffer,endpos+1);
			pkt->len = endpos+1;
			p2pclient_write(p2p,pkt);
		}
		break;

	case eProto_ping: //向bdsvr发送ping
		{
			unsigned char buffer[256];
			int count = 0;
			int check = 0;
			int endpos = 0;
			response_pkt_p pkt;

			count += sprintf_s(buffer + count, sizeof(buffer) - count, "$ping,{\"from\":\"%s\",\"to\":\"%s\"}", "bdjob", "bdsvr");
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
		break;
	}

}