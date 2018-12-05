#include "router_callback.h"
#include "p2p.h"
#include "protobuffer.h"
#include "router_expat.h"
#include "instapp.h"
#include "tool.h"
#include "findjobber.h"

#include "myquery/mysqldb.h"
#include "json/cJSON.h"


#include <list>
using namespace  std;

list<char*>  m_messages;

#define __hex_char(_hexdata_,_chardata_) \
{   if((_hexdata_)<10&&(_hexdata_)>=0) \
		_chardata_ = (_hexdata_) + '0';\
				else if((_hexdata_)>=10&&(_hexdata_) <16)\
		_chardata_ = (_hexdata_) - 10 + 'A';\
				else _chardata_ ='0';\
}
//十六进制转化为ascii
static int  HexToAsc(char *destbuff, unsigned int destbufflen, unsigned char *srchexbuff, int srcbitscnt)
{
	char chdata;
	int i = 0;

	for (; i < srcbitscnt;)
	{
		if (i % 8 == 0){
			__hex_char(srchexbuff[i / 8] >> 4, chdata);
		}
		else{
			__hex_char(srchexbuff[i / 8] & 0x0F, chdata);
		}

		destbuff[i / 4] = chdata;
		i += 4;
	}

	return i / 4;
}

static int bytes_hexchars(char DestBuffer[], int nDestBufferLen, BYTE byBitArray[], int nBitsNum)
{
	return HexToAsc(DestBuffer, nDestBufferLen, byBitArray, nBitsNum);
}


//拼2.1协议通信串
static int  EncodeTXA(lpTXAInfo pTXAInfo, char byFrameBuff[1024])
{
	int f_EncodeDataLen = 0;
	int nFrameBuffLen = 1024;
	f_EncodeDataLen = sprintf_s((char*)byFrameBuff, nFrameBuffLen, "$CCTXA,%07d,%d,%d,", pTXAInfo->nUserId, pTXAInfo->byCommCat, \
		pTXAInfo->byCodeType);

	if ((pTXAInfo->nContentLen + f_EncodeDataLen) > nFrameBuffLen)
		return -1;


	if (pTXAInfo->byCodeType == 2)	//混发
	{
		char buffer[512];
		byFrameBuff[f_EncodeDataLen++] = 'A';
		byFrameBuff[f_EncodeDataLen++] = '4';
		int contentbits = pTXAInfo->nContentLen * 8;
		byFrameBuff[f_EncodeDataLen++] = '0';

		int countbytes = bytes_hexchars(buffer, sizeof(buffer), (BYTE*)pTXAInfo->chContent, contentbits);

		memcpy(byFrameBuff + f_EncodeDataLen, buffer, countbytes);
		f_EncodeDataLen += countbytes;

		int remain = 1024 - f_EncodeDataLen;
		f_EncodeDataLen += sprintf_s(byFrameBuff + f_EncodeDataLen, remain, "*%02X\r\n", checksum((BYTE*)byFrameBuff, f_EncodeDataLen));

	}
	else
		f_EncodeDataLen = 0;

	return f_EncodeDataLen;
}

// 使用方法
int save4bdjobber(p2p_t p2p, char * content, int from_id, int to_id)
{
	TXAInfo txa;
	char buffer2bdjobber[1024];
	int endpos = 0;
	response_pkt_p pkt;

	memset(&txa, 0, sizeof(txa));
	
	txa.byCodeType = 2;
	txa.byCommCat = 1;

	strncpy(txa.chContent, content, sizeof(txa.chContent) - 1);

	if (strlen(txa.chContent) >= 1024)
		txa.nContentLen = 1023;
	else
		txa.nContentLen = strlen(content);

	txa.nLocalId = from_id;
	txa.nUserId = to_id;

	// 把txa结构体中的内容（包括content字符串）转换成2.1协议格式，存入buffer2bdjobber数组中，以备发送给bdjobber
	int bytes = EncodeTXA(&txa, buffer2bdjobber);
	
	// 将 buffer2bdjobber 和对应的 bytes 存入 数据结构中，当bdjob发来jobme的时候，推送出去
	printf("=== save msg for bdjobber\r\n%s ===\r\n");
}

send2bdjobber(p2p_t p2p, char *buffer2bdjobber, int bytes)
{
	response_pkt_p pkt;

	//把buffer2bdjobber发送给bdjobber
	pkt = response_pkt_new(g_instapp->pktpool, bytes);
	memcpy(pkt->data, buffer2bdjobber, bytes);
	pkt->len = bytes;
	p2p_stream_push(p2p, pkt);

	printf("=== send msg to bdjobber\r\n%s ===\r\n", pkt->data);
}

void router_exp_cb(int msgid ,void* msg,int len,void* param)
{
	sess_t destsess;
	unsigned char* frame = (unsigned char*)msg;

	// 下面3行是尝试1
	router_exp_t exp = (router_exp_t)param;
	destsess = (sess_t)_BASE(exp).param;
	if (exp == NULL || destsess == NULL) return;
	

	// 下面4行是尝试2
	//const char * cbdjobber = "bdjob";
	//strcpy(destsess->sname, cbdjobber);
	//destsess = (sess_t)xhash_get(g_instapp->sessions, destsess->sname);
	//p2p_t p2p = (p2p_t)destsess;

	// 下面是尝试3
	//const char * cbdjobber = "bdjob";
	//destsess = getrefclnt(cbdjobber);
	//if (destsess == NULL) return;
	//p2p_t p2p = (p2p_t)destsess;

	switch(msgid)
	{
	case cunpack_initsession:
	{
		cJSON *jsonroot, *json;
		char * jstr;

		jstr = strchr((char*)frame, '{');
		if (jstr == NULL) return 0;

		jsonroot = cJSON_Parse(jstr);
		if (jsonroot == NULL) return 0;

		json = cJSON_GetObjectItem(jsonroot, "from");
		if (json == NULL || json->valuestring == NULL)
		{
			cJSON_Delete(jsonroot);
			return 0;
		}
		strncpy_s(destsess->sname, sizeof(destsess->sname), json->valuestring, sizeof(destsess->sname));

		json = cJSON_GetObjectItem(jsonroot, "to");
		if (json == NULL || json->valuestring == NULL){
			cJSON_Delete(jsonroot);
			return 0;
		}

		if (strcmp(json->valuestring, "bdserver") != 0)  //不是发给自身的Init信息
		{
			cJSON_Delete(jsonroot);
			return 0;
		}

		json = cJSON_GetObjectItem(jsonroot, "rslt");
		if (json == NULL || json->valuestring == NULL)
		{
			cJSON_Delete(jsonroot);
			return 0;
		}

		if (strcmp(json->valuestring, "ok") != 0)  //
		{
			cJSON_Delete(jsonroot);
			p2pclient_close((p2pclient_t)destsess);//close connection..
			return 0;
		}

		cJSON_Delete(jsonroot);

		destsess->binitsess = TRUE;
		printf("=====initack from===== %s\r\n", destsess->sname);
		log_write(g_instapp->log, LOG_NOTICE, "initack from %s .\n", destsess->sname);	 //提示收到Initsession回复
	}

	case cunpack_pong:
	{
		cJSON * jsonroot, *json;
		char* from;
		char* jstr = strstr((char*)frame, "{");

		if (jstr == NULL) return 0;

		jsonroot = cJSON_Parse(jstr);
		if (jsonroot == NULL) return 0;

		json = cJSON_GetObjectItem(jsonroot, "from");
		if (json == NULL){
			cJSON_Delete(jsonroot);
			return 0;
		}

		from = json->valuestring;

		json = cJSON_GetObjectItem(jsonroot, "to");
		if (json == NULL){
			cJSON_Delete(jsonroot);
			return 0;
		}

		if (strcmp(json->valuestring, "bdserver") != 0)
		{
			cJSON_Delete(jsonroot);
			return 0;
		}
		printf("==========  recv <pong> from %s... =========\r\n", from);
		log_write(g_instapp->log, LOG_NOTICE, "pong from %s .\n", from);	 //
		cJSON_Delete(jsonroot);
	}
		break;
	case cunpack_message:
		{
			printf("=====  recv <message> from router  =====\r\n");
			const char * cbdjobber = "bdjob";
			destsess = getrefclnt(cbdjobber);
			if (destsess == NULL) return;
			p2p_t p2p = (p2p_t)destsess;

			cJSON * jsonroot, *json, *json_resp;
			char *type, *ackseqno, *seqno, *rslt, *error, *time, *content;
			char sz_from[64];
			char sz_to[64];
			int bdid_from, bdid_to;
			char * pstr = NULL;
			//cJSON * json_tmp = cJSON_CreateObject();

			//frame[len - 5] = '\0'; //*检验\r\n
			pstr = (char*)frame;
			pstr = strchr(pstr, '{'); //json串

			jsonroot = cJSON_Parse(pstr);
			if (jsonroot == NULL)
			{
				cJSON_Delete(jsonroot);
				//cJSON_Delete(json_tmp);
				return;
			}

			json_resp = cJSON_GetObjectItem(jsonroot, "response");
			if (json_resp == NULL)
			{
				cJSON_Delete(jsonroot);
				//cJSON_Delete(json_tmp);
				return;
			}

			json = cJSON_GetObjectItem(json_resp, "type");
			if (json == NULL)
			{
				cJSON_Delete(jsonroot);
				//cJSON_Delete(json_tmp);
				return;
			}
			type = json->valuestring;

			// 第一类，来自router的应答消息，该类消息不需要转发给bdjobber（除非是认证失败）
			if ((strcmp(type, "bdsos") == 0) || (strcmp(type, "bdposreport") == 0) \
				|| (strcmp(type, "bd2inetmsg") == 0) || (strcmp(type, "bd2smsmsg") == 0))	// type = bdsos / bdposreport / bd2inetmsg / bd2smsmsg
			{
				printf("=====  recv <response> from router  =====\r\n");
				//log_write(g_instapp->log, LOG_NOTICE, "=== router->bdserver .. %s .\n", g_msgcache);

				json = cJSON_GetObjectItem(jsonroot, "rslt");
				if (json = NULL)
				{
					cJSON_Delete(jsonroot);
					//cJSON_Delete(json_tmp);
					return;
				}
				if (strcmp(json->valuestring, "ok") != 0)  // 认证失败
				{
					json = cJSON_GetObjectItem(jsonroot, "seqno");
					if (json = NULL)
					{
						cJSON_Delete(jsonroot);
						//cJSON_Delete(json_tmp);
						return;
					}
					ackseqno = json->valuestring;

					json = cJSON_GetObjectItem(jsonroot, "error");
					if (json = NULL)
					{
						cJSON_Delete(jsonroot);
						//cJSON_Delete(json_tmp);
						return;
					}
					error = json->valuestring;

					// 认证失败后，给bdjobber返回一条消息
					printf("auth fail [%s].\r\n", error);
					return;
				}
				else if (strcmp(json->valuestring, "ok") == 0)
				{
					// 认证成功


					cJSON_Delete(jsonroot);
					//cJSON_Delete(json_tmp);
					printf("=====  recv auth ok response from  router  =====\r\n");
					//log_write(g_instapp->log, LOG_NOTICE, "=== router->bdserver auth ok.. %s .\n", g_msgcache);
					return;
				}
			}
			else if (strcmp(type, "bdmail") == 0)  // 第二类，来自router的北斗邮箱短消息
			{
				// 提取from的值，其中是平台id对应的字符串
				printf("=====  recv <bdmail> from router  =====\r\n");
				json = cJSON_GetObjectItem(json_resp, "from");		
				if (json == NULL)
				{
					cJSON_Delete(jsonroot);
					//cJSON_Delete(json_tmp);
					return;
				}
				// 根据平台id获取北斗id
				
				bdid_from = db_query_bdid((mysqlquery_t)(g_instapp->sqlobj), atoi(json->valuestring));
				/*
				snprintf(sz_from, sizeof(sz_from), "%d", bdid);		// 把bdid从int型转成字符串型
				cJSON_AddStringToObject(json_tmp, "from", sz_from);	// 如果上面的一句导致了多余的空格，这里可以去掉，不知是否需要

				json = cJSON_GetObjectItem(json_tmp, "from");		// 第二次提取from的值，其中是北斗id对应的字符串
				if (json = NULL)
				{
					cJSON_Delete(jsonroot);
					cJSON_Delete(json_tmp);
					return;
				}
				from = json->valuestring;		// 最终的from，对应北斗id的字符串
				*/
				// 从jsonroot中提取to节点内容
				json = cJSON_GetObjectItem(json_resp, "to");		// 提取to的值，其中是平台id对应的字符串
				if (json == NULL)
				{
					cJSON_Delete(jsonroot);
					//cJSON_Delete(json_tmp);
					return;
				}
				//to = json->valuestring;

				// 根据平台id，找出对应的北斗id
				bdid_to = db_query_bdid((mysqlquery_t)(g_instapp->sqlobj), atoi(json->valuestring));
				/*
				snprintf(sz_to, sizeof(sz_to), "%d", bdid);		// 把bdid从int型转成字符串型
				cJSON_AddStringToObject(json_tmp, "to", sz_to);	// 如果上面的一句导致了多余的空格，这里可以去掉，不知是否需要

				// 从json_tmp中提取to节点内容
				json = cJSON_GetObjectItem(json_tmp, "to");		// 第二次提取to的值，其中是北斗id对应的字符串
				if (json == NULL)
				{
					cJSON_Delete(jsonroot);
					cJSON_Delete(json_tmp);
					return;
				}
				to = json->valuestring;		// 最终的to，对应北斗id的字符串
				*/
				json = cJSON_GetObjectItem(json_resp, "seqno");
				if (json == NULL)
				{
					cJSON_Delete(jsonroot);
					//cJSON_Delete(json_tmp);
					return;
				}
				seqno = json->valuestring;

				json = cJSON_GetObjectItem(json_resp, "time");
				if (json == NULL)
				{
					cJSON_Delete(jsonroot);
					//cJSON_Delete(json_tmp);
					return;
				}
				time = json->valuestring;

				json = cJSON_GetObjectItem(json_resp, "content");
				if (json == NULL)
				{
					cJSON_Delete(jsonroot);
					//cJSON_Delete(json_tmp);
					return;
				}
				content = json->valuestring;

				// 解析完成，接下来是按照北斗通信协议TXA拼串，发送给bdjobber
				// TXA格式为：$--TXA,xxxxxxx,x,x,c--c*hh<CR><LF>
				save4bdjobber(p2p, content, bdid_from, bdid_to);

				cJSON_Delete(jsonroot);
				//cJSON_Delete(json_tmp);
				printf("===== send bdmail to bdjobber =====\r\n");
				//log_write(g_instapp->log, LOG_NOTICE, "=== bdserver->bdjobber .. %s .\n", g_msgcache);
				return;
			}
			else if (strcmp(type, "bdirect") == 0) // 第三类，来自router的北斗直播短消息
			{
				// 提取from
				json = cJSON_GetObjectItem(json_resp, "from");		// 第一次提取from的值，其中是平台id对应的字符串
				if (json == NULL)
				{
					cJSON_Delete(jsonroot);
					//cJSON_Delete(json_tmp);
					return;
				}

				bdid_from = db_query_bdid((mysqlquery_t)(g_instapp->sqlobj), atoi(json->valuestring));

				//snprintf(sz_from, sizeof(sz_from), "%d", bdid);		// 把bdid从int型转成字符串型
				//cJSON_AddStringToObject(json_tmp, "from", sz_from);	// 如果上面的一句导致了多余的空格，这里可以去掉，不知是否需要

				//json = cJSON_GetObjectItem(json_tmp, "from");		// 第二次提取from的值，其中是北斗id对应的字符串
				//if (json = NULL)
				//{
				//	cJSON_Delete(jsonroot);
				//	cJSON_Delete(json_tmp);
				//	return;
				//}
				//from = json->valuestring;		// 最终的from，对应北斗id的字符串

				// 从jsonroot中提取to节点内容
				json = cJSON_GetObjectItem(json_resp, "to");		// 第一次提取to的值，其中是平台id对应的字符串
				if (json = NULL)
				{
					cJSON_Delete(jsonroot);
					//cJSON_Delete(json_tmp);
					return;
				}
				//to = json->valuestring;

				// 将转换后的to存入json_tmp的to节点
				bdid_to = db_query_bdid((mysqlquery_t)(g_instapp->sqlobj), atoi(json->valuestring));
				//snprintf(sz_to, sizeof(sz_to), "%d", bdid);	// 把bdid从int型转成字符串型
				//cJSON_AddStringToObject(json_tmp, "to", sz_to);	// 如果上面的一句导致了多余的空格，这里可以去掉，不知是否需要

				//// 从json_tmp中提取to节点内容
				//json = cJSON_GetObjectItem(json_tmp, "to");		// 第二次提取to的值，其中是北斗id对应的字符串
				//if (json = NULL)
				//{
				//	cJSON_Delete(jsonroot);
				//	cJSON_Delete(json_tmp);
				//	return;
				//}
				//to = json->valuestring;		// 最终的to，对应北斗id的字符串

				json = cJSON_GetObjectItem(json_resp, "seqno");
				if (json = NULL)
				{
					cJSON_Delete(jsonroot);
					//cJSON_Delete(json_tmp);
					return;
				}
				seqno = json->valuestring;

				json = cJSON_GetObjectItem(json_resp, "content");
				if (json = NULL)
				{
					cJSON_Delete(jsonroot);
					//cJSON_Delete(json_tmp);
					return;
				}
				content = json->valuestring;

				// 解析完成，接下来是按照北斗通信协议TXA拼串，发送给bdjobber
				// TXA格式为：$--TXA,xxxxxxx,x,x,c--c*hh<CR><LF>
				save4bdjobber(p2p, content, bdid_from, bdid_to);

				cJSON_Delete(jsonroot);
				//cJSON_Delete(json_tmp);
				printf("===== send bdirect to bdjobber =====\r\n", len);
				//log_write(g_instapp->log, LOG_NOTICE, "=== bdserver->bdjobber .. %s .\n", g_msgcache);
				return;
			}
		}
		break;
	default:
		break;
	}
}
