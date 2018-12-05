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
//ʮ������ת��Ϊascii
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


//ƴ2.1Э��ͨ�Ŵ�
static int  EncodeTXA(lpTXAInfo pTXAInfo, char byFrameBuff[1024])
{
	int f_EncodeDataLen = 0;
	int nFrameBuffLen = 1024;
	f_EncodeDataLen = sprintf_s((char*)byFrameBuff, nFrameBuffLen, "$CCTXA,%07d,%d,%d,", pTXAInfo->nUserId, pTXAInfo->byCommCat, \
		pTXAInfo->byCodeType);

	if ((pTXAInfo->nContentLen + f_EncodeDataLen) > nFrameBuffLen)
		return -1;


	if (pTXAInfo->byCodeType == 2)	//�췢
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

// ʹ�÷���
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

	// ��txa�ṹ���е����ݣ�����content�ַ�����ת����2.1Э���ʽ������buffer2bdjobber�����У��Ա����͸�bdjobber
	int bytes = EncodeTXA(&txa, buffer2bdjobber);
	
	// �� buffer2bdjobber �Ͷ�Ӧ�� bytes ���� ���ݽṹ�У���bdjob����jobme��ʱ�����ͳ�ȥ
	printf("=== save msg for bdjobber\r\n%s ===\r\n");
}

send2bdjobber(p2p_t p2p, char *buffer2bdjobber, int bytes)
{
	response_pkt_p pkt;

	//��buffer2bdjobber���͸�bdjobber
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

	// ����3���ǳ���1
	router_exp_t exp = (router_exp_t)param;
	destsess = (sess_t)_BASE(exp).param;
	if (exp == NULL || destsess == NULL) return;
	

	// ����4���ǳ���2
	//const char * cbdjobber = "bdjob";
	//strcpy(destsess->sname, cbdjobber);
	//destsess = (sess_t)xhash_get(g_instapp->sessions, destsess->sname);
	//p2p_t p2p = (p2p_t)destsess;

	// �����ǳ���3
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

		if (strcmp(json->valuestring, "bdserver") != 0)  //���Ƿ��������Init��Ϣ
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
		log_write(g_instapp->log, LOG_NOTICE, "initack from %s .\n", destsess->sname);	 //��ʾ�յ�Initsession�ظ�
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

			//frame[len - 5] = '\0'; //*����\r\n
			pstr = (char*)frame;
			pstr = strchr(pstr, '{'); //json��

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

			// ��һ�࣬����router��Ӧ����Ϣ��������Ϣ����Ҫת����bdjobber����������֤ʧ�ܣ�
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
				if (strcmp(json->valuestring, "ok") != 0)  // ��֤ʧ��
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

					// ��֤ʧ�ܺ󣬸�bdjobber����һ����Ϣ
					printf("auth fail [%s].\r\n", error);
					return;
				}
				else if (strcmp(json->valuestring, "ok") == 0)
				{
					// ��֤�ɹ�


					cJSON_Delete(jsonroot);
					//cJSON_Delete(json_tmp);
					printf("=====  recv auth ok response from  router  =====\r\n");
					//log_write(g_instapp->log, LOG_NOTICE, "=== router->bdserver auth ok.. %s .\n", g_msgcache);
					return;
				}
			}
			else if (strcmp(type, "bdmail") == 0)  // �ڶ��࣬����router�ı����������Ϣ
			{
				// ��ȡfrom��ֵ��������ƽ̨id��Ӧ���ַ���
				printf("=====  recv <bdmail> from router  =====\r\n");
				json = cJSON_GetObjectItem(json_resp, "from");		
				if (json == NULL)
				{
					cJSON_Delete(jsonroot);
					//cJSON_Delete(json_tmp);
					return;
				}
				// ����ƽ̨id��ȡ����id
				
				bdid_from = db_query_bdid((mysqlquery_t)(g_instapp->sqlobj), atoi(json->valuestring));
				/*
				snprintf(sz_from, sizeof(sz_from), "%d", bdid);		// ��bdid��int��ת���ַ�����
				cJSON_AddStringToObject(json_tmp, "from", sz_from);	// ��������һ�䵼���˶���Ŀո��������ȥ������֪�Ƿ���Ҫ

				json = cJSON_GetObjectItem(json_tmp, "from");		// �ڶ�����ȡfrom��ֵ�������Ǳ���id��Ӧ���ַ���
				if (json = NULL)
				{
					cJSON_Delete(jsonroot);
					cJSON_Delete(json_tmp);
					return;
				}
				from = json->valuestring;		// ���յ�from����Ӧ����id���ַ���
				*/
				// ��jsonroot����ȡto�ڵ�����
				json = cJSON_GetObjectItem(json_resp, "to");		// ��ȡto��ֵ��������ƽ̨id��Ӧ���ַ���
				if (json == NULL)
				{
					cJSON_Delete(jsonroot);
					//cJSON_Delete(json_tmp);
					return;
				}
				//to = json->valuestring;

				// ����ƽ̨id���ҳ���Ӧ�ı���id
				bdid_to = db_query_bdid((mysqlquery_t)(g_instapp->sqlobj), atoi(json->valuestring));
				/*
				snprintf(sz_to, sizeof(sz_to), "%d", bdid);		// ��bdid��int��ת���ַ�����
				cJSON_AddStringToObject(json_tmp, "to", sz_to);	// ��������һ�䵼���˶���Ŀո��������ȥ������֪�Ƿ���Ҫ

				// ��json_tmp����ȡto�ڵ�����
				json = cJSON_GetObjectItem(json_tmp, "to");		// �ڶ�����ȡto��ֵ�������Ǳ���id��Ӧ���ַ���
				if (json == NULL)
				{
					cJSON_Delete(jsonroot);
					cJSON_Delete(json_tmp);
					return;
				}
				to = json->valuestring;		// ���յ�to����Ӧ����id���ַ���
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

				// ������ɣ��������ǰ��ձ���ͨ��Э��TXAƴ�������͸�bdjobber
				// TXA��ʽΪ��$--TXA,xxxxxxx,x,x,c--c*hh<CR><LF>
				save4bdjobber(p2p, content, bdid_from, bdid_to);

				cJSON_Delete(jsonroot);
				//cJSON_Delete(json_tmp);
				printf("===== send bdmail to bdjobber =====\r\n");
				//log_write(g_instapp->log, LOG_NOTICE, "=== bdserver->bdjobber .. %s .\n", g_msgcache);
				return;
			}
			else if (strcmp(type, "bdirect") == 0) // �����࣬����router�ı���ֱ������Ϣ
			{
				// ��ȡfrom
				json = cJSON_GetObjectItem(json_resp, "from");		// ��һ����ȡfrom��ֵ��������ƽ̨id��Ӧ���ַ���
				if (json == NULL)
				{
					cJSON_Delete(jsonroot);
					//cJSON_Delete(json_tmp);
					return;
				}

				bdid_from = db_query_bdid((mysqlquery_t)(g_instapp->sqlobj), atoi(json->valuestring));

				//snprintf(sz_from, sizeof(sz_from), "%d", bdid);		// ��bdid��int��ת���ַ�����
				//cJSON_AddStringToObject(json_tmp, "from", sz_from);	// ��������һ�䵼���˶���Ŀո��������ȥ������֪�Ƿ���Ҫ

				//json = cJSON_GetObjectItem(json_tmp, "from");		// �ڶ�����ȡfrom��ֵ�������Ǳ���id��Ӧ���ַ���
				//if (json = NULL)
				//{
				//	cJSON_Delete(jsonroot);
				//	cJSON_Delete(json_tmp);
				//	return;
				//}
				//from = json->valuestring;		// ���յ�from����Ӧ����id���ַ���

				// ��jsonroot����ȡto�ڵ�����
				json = cJSON_GetObjectItem(json_resp, "to");		// ��һ����ȡto��ֵ��������ƽ̨id��Ӧ���ַ���
				if (json = NULL)
				{
					cJSON_Delete(jsonroot);
					//cJSON_Delete(json_tmp);
					return;
				}
				//to = json->valuestring;

				// ��ת�����to����json_tmp��to�ڵ�
				bdid_to = db_query_bdid((mysqlquery_t)(g_instapp->sqlobj), atoi(json->valuestring));
				//snprintf(sz_to, sizeof(sz_to), "%d", bdid);	// ��bdid��int��ת���ַ�����
				//cJSON_AddStringToObject(json_tmp, "to", sz_to);	// ��������һ�䵼���˶���Ŀո��������ȥ������֪�Ƿ���Ҫ

				//// ��json_tmp����ȡto�ڵ�����
				//json = cJSON_GetObjectItem(json_tmp, "to");		// �ڶ�����ȡto��ֵ�������Ǳ���id��Ӧ���ַ���
				//if (json = NULL)
				//{
				//	cJSON_Delete(jsonroot);
				//	cJSON_Delete(json_tmp);
				//	return;
				//}
				//to = json->valuestring;		// ���յ�to����Ӧ����id���ַ���

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

				// ������ɣ��������ǰ��ձ���ͨ��Э��TXAƴ�������͸�bdjobber
				// TXA��ʽΪ��$--TXA,xxxxxxx,x,x,c--c*hh<CR><LF>
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
