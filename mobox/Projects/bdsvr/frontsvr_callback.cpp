#include "frontsvr_callback.h"
#include "bdjobber_callback.h"
#include "p2p.h"
#include "protobuffer.h"
#include "frontsvr_expat.h"
#include "instapp.h"
#include "tool.h"
#include "findjobber.h"
#include "myquery/mysqldb.h"
#include "json/cJSON.h"
#include "dbaccess.h"
#include "jobqueue.h"


#ifdef __cplusplus
extern "C"
{
#endif
extern BOOL jobber_free;
extern int g_shutdown;
#ifdef __cplusplus
}
#endif


extern int Seqno2FrontsvrManage(void);

bdjob_json_t bdjob_json_new()
{
	bdjob_json_t rtn = NULL;
	while (rtn = (bdjob_json_t)malloc(sizeof(bdjob_json_st) == NULL)) Sleep(10);

	return rtn;
}

void bdjob_json_free(bdjob_json_t rtn)
{
	if (rtn == NULL) return;
	if (rtn->desc)
		free(rtn->desc);

	if (rtn->maker)
		free(rtn->maker);

	if (rtn->maketime)
		free(rtn->maketime);

	if (rtn->path)
		free(rtn->path);

	if (rtn->taskname)
		free(rtn->taskname);

	free(rtn);
}


#define __hex_char(_hexdata_,_chardata_) \
{   if((_hexdata_)<10&&(_hexdata_)>=0) \
		_chardata_ = (_hexdata_) + '0';\
				else if((_hexdata_)>=10&&(_hexdata_) <16)\
		_chardata_ = (_hexdata_) - 10 + 'A';\
				else _chardata_ ='0';\
}

// 从字符串中截取两个特定字符之间的内容
char * slice_str(char * src, char start, char end,char buffer[],int bufflen)
{
	char * pos1 = strchr(src, start) + 1;
	char * pos2 = strchr(pos1, end);

	int len = pos2 - pos1;

	if (len >= bufflen-1) len = bufflen - 1;
	
	memcpy(buffer, pos1, len);
	buffer[len] = '\0';

	return buffer;
}

// 将frontsvr发来的消息转成BDTXA 2.1协议
static int encode4bdjobber(char* buffer2bdjobber,char * chcontent,int bdid)
{
	TXAInfo txa;
	char buffer[1024] = { 0 };

	memset(&txa, 0, sizeof(txa));

	txa.byCodeType = 2;
	txa.byCommCat = 1;
	txa.nDestId = bdid;

	memcpy(txa.chContent, chcontent, strlen(chcontent));

	if (strlen(txa.chContent) >= 1024)
		txa.nContentLen = 1023;
	else
		txa.nContentLen = strlen(txa.chContent);

	// 把txa结构体中的内容（包括content字符串）转换成2.1协议格式，存入buffer2bdjobber数组中，以备发送给bdjobber
	int bytes = EncodeTXA(&txa, buffer);

	if (bytes == 0)
	{
		log_debug(ZONE, "error: msg length is 0.");
		return -1;
	}

	strcpy(buffer2bdjobber, buffer);

	return 0;
}

static int send2bdjobber(p2p_t p2p, char *buffer2bdjobber, int bytes)
{
	response_pkt_p pkt;

	pkt = response_pkt_new(g_instapp->pktpool, bytes);
	memcpy(pkt->data, buffer2bdjobber, bytes);
	pkt->len = bytes;
	p2p_stream_push(p2p, pkt);

	//log_debug(ZONE, "send msg to bdjobber in frontsvr_cb: %s",buffer2bdjobber);
	log_write(g_instapp->log, LOG_NOTICE, "send msg to bdjobber in frontsvr_cb: %s", buffer2bdjobber);

	return 0;
}

int parse_bdjob_content(bdjob_t bdjob,char chContent[],int* bdid)
{
	cJSON *jsonroot, *json,*json_data;
	char * type = bdjob->type;
	char * content = bdjob->content;
	char cccode[10];

	char *executor,*data,*code;

	jsonroot = cJSON_Parse(bdjob->content);
	if (jsonroot == NULL) return -1;

	if (strcmp(type, "dyncode") == 0)
	{
		json = cJSON_GetObjectItem(jsonroot, "executor");
		if (json == NULL)
		{
			cJSON_Delete(jsonroot);
			return -1;
		}
		executor = json->valuestring;

		// 根据executor查找bdid，然后才能进行下面的赋值操作。因为此时的executor理论上应该是执行人姓名
		*bdid = atoi(executor);

		json = cJSON_GetObjectItem(jsonroot, "data");
		if (json == NULL)
		{
			cJSON_Delete(jsonroot);
			return -1;
		}

		cJSON* item = cJSON_GetObjectItem(json, "code");
		if (item == NULL)
		{
			cJSON_Delete(jsonroot);
			return -1;
		}
		code = item->valuestring;

		//strcpy(chContent, code);
		sprintf_s(cccode, sizeof(cccode), "10%s", code);
		strcpy(chContent, cccode);

		cJSON_Delete(jsonroot);
	}

	return 0;
}

int ParseEncodeSend(void)
{
	sess_t destsess;
	const char * cbdjobber = "bdjob";
	destsess = getrefclnt(cbdjobber);
	if (destsess == NULL)
	{
		return -1;
	}
	p2p_t p2p = (p2p_t)destsess;		// 当需要给bdjobber发消息时，需要这个p2p	

	char buffer2bdjobber[1024] = { 0 };

	if (p2p->sess.fd == NULL)
	{
		log_write(g_instapp->log,LOG_ERR,"Bdjob connection is lost!");
		return -1;
	}

	bdjob_t bdjob = (bdjob_t)jqueue_pull(g_instapp->jobqueue);
	if (bdjob == NULL)
	{
		log_write(g_instapp->log, LOG_NOTICE, "bdjobqueue is empty!");
		return -1;
	}

	char chcontent[1024];
	int bdid = 0;
	if (0 != parse_bdjob_content(bdjob, chcontent, &bdid))
	{
		bdjob_free(bdjob);
		return -1;
	}

	if (0 != encode4bdjobber(buffer2bdjobber, chcontent, bdid))
	{
		bdjob_free(bdjob);
		return -1;
	}

	send2bdjobber(p2p, buffer2bdjobber, strlen(buffer2bdjobber));

	free(bdjob);
	return 0;
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

static unsigned char checksum(unsigned char buffer[], int len)
{
	unsigned char data = 0;
	int i = 0;
	for (; i < len; i++)
		data ^= buffer[i];

	return data;
}



#define __char_hex(_chardata_,_hexdata_) \
{\
	if((_chardata_)>='0' && (_chardata_)<='9')\
	_hexdata_ = _chardata_ - '0';\
																else if((_chardata_) >='A' && (_chardata_) <= 'F')\
	_hexdata_ = (_chardata_) -'A'+ 10;\
																else if((_chardata_) >='a' && (_chardata_) <= 'f')\
	_hexdata_ = (_chardata_) -'a'+ 10 ;\
																else _hexdata_ = 0;\
}



//拼2.1协议通信串
int  EncodeTXA(lpTXAInfo pTXAInfo, char byFrameBuff[1024])
{
	int f_EncodeDataLen = 0;
	int nFrameBuffLen = 1024;
	
	/*f_EncodeDataLen = sprintf_s((char*)byFrameBuff, nFrameBuffLen, "$CCTXA,%07d,%d,%d,", pTXAInfo->nDestId, pTXAInfo->byCommCat, \
		pTXAInfo->byCodeType);*/

	f_EncodeDataLen = sprintf_s((char*)byFrameBuff, nFrameBuffLen, "$CCTXA,%07d,%d,%d,A4BF%s", pTXAInfo->nDestId, pTXAInfo->byCommCat, \
		pTXAInfo->byCodeType,pTXAInfo->chContent);
	f_EncodeDataLen += sprintf_s(byFrameBuff + f_EncodeDataLen, nFrameBuffLen - f_EncodeDataLen, "*%02X\r\n", checksum((BYTE*)byFrameBuff, f_EncodeDataLen));
	
	//if ((pTXAInfo->nContentLen + f_EncodeDataLen) > nFrameBuffLen)
	//	return -1;

	//if (pTXAInfo->byCodeType == 2)
	//{
	//	
	//	byFrameBuff[f_EncodeDataLen++] = 'A';
	//	byFrameBuff[f_EncodeDataLen++] = '4';
	//	byFrameBuff[f_EncodeDataLen++] = 'B';
	//	byFrameBuff[f_EncodeDataLen++] = 'F';

	//	/*for (int i = 0; i < pTXAInfo->nContentLen; i++)
	//	{
	//		byFrameBuff[f_EncodeDataLen++] = pTXAInfo->chContent[i];
	//	}*/
	//

	//	//char buffer[512];
	//	//int contentbits = pTXAInfo->nContentLen * 8;
	//	//if (contentbits > 628 - 8)
	//	//{
	//	//	contentbits = 628 - 8;
	//	//}
	//	//// 将Hex格式的内容转为Asc码格式，所占bit数翻倍
	//	//int countbytes = bytes_hexchars(buffer, sizeof(buffer), (BYTE*)pTXAInfo->chContent, contentbits);
	//	//memcpy(byFrameBuff + f_EncodeDataLen, buffer, countbytes);
	//	//f_EncodeDataLen += countbytes;

	//	//int remain = 1024 - f_EncodeDataLen;
	//	//f_EncodeDataLen += sprintf_s(byFrameBuff + f_EncodeDataLen, remain, "*%02X\r\n", checksum((BYTE*)byFrameBuff, f_EncodeDataLen));

	//}

	//else if (pTXAInfo->byCodeType == 1)
	//{
	//	int contentbits = pTXAInfo->nContentLen * 4;
	//	if (contentbits > 628)
	//		contentbits = 628;

	//	int countbytes = contentbits / 4;

	//	memcpy(byFrameBuff + f_EncodeDataLen, pTXAInfo->chContent, countbytes);
	//	f_EncodeDataLen += countbytes;
	//}
	//else if (pTXAInfo->byCodeType == 0)
	//{
	//	int contentbits = pTXAInfo->nContentLen / 2 * 14;
	//	if (contentbits > 628){
	//		contentbits = 628;
	//	}
	//	int countbytes = contentbits / 14 * 2;
	//	memcpy(byFrameBuff + f_EncodeDataLen, pTXAInfo->chContent, countbytes);
	//	f_EncodeDataLen += countbytes;
	//}
	//else
	//	f_EncodeDataLen = 0;

	return f_EncodeDataLen;
}



void frontsvr_exp_cb(int msgid, void* msg, int len, void* param)
{
	sess_t sess;
	unsigned char* frame = (unsigned char*)msg;

	frontsvr_exp_t exp = (frontsvr_exp_t)param;
	sess = (sess_t)_BASE(exp).param;
	if (exp == NULL || sess == NULL) return;

	p2pclient_t p2pclient = (p2pclient_t)sess;


	if (len - 5 < 0){
		p2pclient_close(p2pclient);
		return;
	}

	char *jstr = (char*)frame;
	jstr[len - 5] = '\0';
	while (*jstr != '{')jstr++;
	
	switch(msgid)
	{
	case cunpack_initack:
	{
		cJSON *jsonroot, *json;
		char  *to, *rest, *jstr;
		char from[64];

		slice_str((char*)frame, ',', ',', from,sizeof(from));	// 截取两个逗号之间的内容		
		strncpy_s(sess->sname, sizeof(sess->sname), from, strlen(from));

		rest = strchr((char *)frame, ',') + 1;				// 第一个逗号及后面所有字符
		to = slice_str(rest, ',', ',', from, sizeof(from));	// 截取两个逗号之间的内容
		//if (to == NULL) return;
		if (to == NULL) 			//不是发给自身的Init信息
		{
			p2pclient_close((p2pclient_t)sess);		// 关闭连接
			return;
		}

		/* 剩下的部分是json串 */
		jstr = strchr((char*)rest, '{');
		if (jstr == NULL) return;

		jsonroot = cJSON_Parse(jstr);
		if (jsonroot == NULL) return;

		json = cJSON_GetObjectItem(jsonroot, "role");
		if (json == NULL || json->valuestring == NULL){
			cJSON_Delete(jsonroot);
			return;
		}

		if (strcmp(json->valuestring, g_instapp->auth_role) != 0)			//不是发给自身的Init信息
		{
			cJSON_Delete(jsonroot);
			p2pclient_close((p2pclient_t)sess);					// 关闭连接
			return;
		}

		json = cJSON_GetObjectItem(jsonroot, "rslt");
		if (json == NULL || json->valuestring == NULL)
		{
			cJSON_Delete(jsonroot);
			return;
		}

		if (strcmp(json->valuestring, "ok") != 0)				// 发给自己，但是init失败 
		{
			cJSON_Delete(jsonroot);
			p2pclient_close((p2pclient_t)sess);					// 关闭连接
			return;
		}

		cJSON_Delete(jsonroot);

		sess->binitsess = TRUE;

		log_write(g_instapp->log, LOG_NOTICE, "initack from %s.", sess->sname);	 //提示收到Initsession回复

		//向服务器发送auth消息
		p2pclient_t p2p = (p2pclient_t)sess;				
		strcpy(p2p->sess.sname, sess->sname);
		protobuffer_clnt_send(p2p, eProto_auth, NULL);		
	}
	break;
	case cunpack_authack:
	{
		cJSON *jsonroot, *json;
		//char  *to, *rest, *jstr;
		//char from[64];

		//slice_str((char*)frame, ',', ',', from, sizeof(from));	// 截取两个逗号之间的内容		
		//strncpy_s(sess->sname, sizeof(sess->sname), from, strlen(from));

		//rest = strchr((char *)frame, ',') + 1;		// 第一个逗号及后面所有字符
		//to = slice_str(rest, ',', ',', from, sizeof(from));				// 截取两个逗号之间的内容
		//if (to == NULL) return;
		//if (strcmp(to, g_instapp->auth_name) != 0)			//不是发给自身的auth ack信息
		//{
		//	p2pclient_close((p2pclient_t)sess);					// 关闭连接
		//	return;
		//}

		///* 剩下的部分是json串 */
		//jstr = strchr((char*)rest, '{');
		//if (jstr == NULL) return;

		jsonroot = cJSON_Parse(jstr);
		if (jsonroot == NULL) return;		

		json = cJSON_GetObjectItem(jsonroot, "rslt");
		if (json == NULL || json->valuestring == NULL)
		{
			cJSON_Delete(jsonroot);
			return;
		}

		if (strcmp(json->valuestring, "ok") != 0)				// 发给自己，但是auth失败 
		{
			cJSON_Delete(jsonroot);
			p2pclient_close((p2pclient_t)sess);					// 关闭连接
			return;
		}
		cJSON_Delete(jsonroot);

		log_write(g_instapp->log, LOG_NOTICE, "auth ack from %s.", sess->sname);	 //提示收到auth回复
	}
	break;

	case cunpack_pong:
	{
		log_write(g_instapp->log, LOG_NOTICE, "pong from frontsvr.");
		//cJSON * jsonroot, *json;
		//char* from;
		//char* jstr = strstr((char*)frame, "{");

		//if (jstr == NULL) return;

		//jsonroot = cJSON_Parse(jstr);
		//if (jsonroot == NULL) return;

		//json = cJSON_GetObjectItem(jsonroot, "from");
		//if (json == NULL){
		//	cJSON_Delete(jsonroot);
		//	return;
		//}

		//from = json->valuestring;

		//json = cJSON_GetObjectItem(jsonroot, "to");
		//if (json == NULL){
		//	cJSON_Delete(jsonroot);
		//	return;
		//}

		//if (strcmp(json->valuestring, g_instapp->auth_name) != 0)	// 不是给自己的pong信息
		//{
		//	cJSON_Delete(jsonroot);
		//	return;
		//}
		//
		//cJSON_Delete(jsonroot);
	}
		break;

	case cunpack_dyncode:
	{
		// 放入job消息队列中
		char * type = "dyncode";
		
		bdjob_t bdjob = bdjob_new();
		if (bdjob == NULL)
		{
			log_write(g_instapp->log, LOG_ERR, "bdjob_new failed.");
			return;
		}
		memcpy(bdjob->type, type, strlen(type));
		memcpy(bdjob->content, jstr, strlen(jstr));
		jqueue_push(g_instapp->jobqueue, bdjob, 0);

		log_write(g_instapp->log, LOG_NOTICE, "put %s frame in bdjobqueue.",type);
	}
	break;

	case cunpack_taskpau:
	{
		// 放入job消息队列中
		char * type = "taskpau";
		
		bdjob_t bdjob = bdjob_new();
		if (bdjob == NULL)
		{
			log_write(g_instapp->log, LOG_ERR, "bdjob_new failed.");
			return;
		}
		memcpy(bdjob->type, type, strlen(type));
		memcpy(bdjob->content, jstr, strlen(jstr));
		jqueue_push(g_instapp->jobqueue, bdjob, 0);

		log_write(g_instapp->log, LOG_NOTICE, "put %s frame in bdjobqueue.",type);
	}
	break;

	case cunpack_taskcon:
	{
		// 放入bdjob消息队列中
		char * type = "taskcon";

		bdjob_t bdjob = bdjob_new();
		if (bdjob == NULL)
		{
			log_write(g_instapp->log, LOG_ERR, "bdjob_new failed.");
			return;
		}
		memcpy(bdjob->type, type, strlen(type));
		memcpy(bdjob->content, jstr, strlen(jstr));
		jqueue_push(g_instapp->jobqueue, bdjob, 0);

		log_write(g_instapp->log, LOG_NOTICE, "put %s frame in bdjobqueue.", type);
	}
	break;

	default:
		break;
	}
}
