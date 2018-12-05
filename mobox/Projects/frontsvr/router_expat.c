//处理router与smudpserver之间的通信
#include "aes.h"
#include <stdlib.h>
#include <string.h>
#include "p2pclient.h"
#include "router_expat.h"
#include "client.h"
#include "p2p.h"
#include "json/cJSON.h"
#include "rediscommand.h"
#include "routermsgcallback.h"
#include "dbaccess.h"
#include "globaldata.h"
#include "tool.h"
#include "userstatus.h"
#include "rptqueue.h"

#define NULL     0
#define HEADLEN  12
#define FALSE    0
#define TRUE     1
static char g_msgcache[64 * 1024];
enum {
	PACK_UNSEARCHED = 0,
	PACK_SEARCHED,
	PACK_STILLDATA,
	PACK_UNFINISHED,
	PACK_FINISHED,
};

enum {
	SEARCHMACHINE_NULL = 0,
	SEARCH_GETHEAD,
	SEARCH_GETDATA,
	SEARCH_GETCHECK,
	SEARCHMACHINE_BEGININD,
	SEARCHMACHINE_IDENTIFIER,
	SEARCHMACHINE_LENGTH,
	SEARCHMACHINE_BINARYCHECK,
};
typedef struct _SInfoTitle_st2{
	int id;
	char title[12];
}SInfoTitle_st2, *SInfoTitle_t2;

static SInfoTitle_st2 MSGTitle[3] =
{
	{ cunpack_initack, "$initack" }, 
	{ cunpack_pong, "$pong" },
	{ cunpack_message, "$message" }

};

#define ISLCHAR(w)     (w>='a'&&w<='z')
#define ISHCHAR(w)     (w>='A'&&w<='Z')
#define ISDIGIT(w)     (w>='0'&&w<='9')
#define ISCHAR(w)       (ISDIGIT(w)|| ISLCHAR(w)||ISHCHAR(w))

#define NULLReturn(condition , rtnval)      {if((condition) == NULL)  return rtnval;}
#define FALSEReturn(condition, rtnval)      {if((condition) == FALSE) return rtnval;}
#define TRUEReturn(condition , rtnval)      {if((condition) == TRUE)  return rtnval;}

typedef enum
{
	inet_evt = 0,
	inet_pos
}inet_type;

static int jcall_posreport(sess_t sess, cJSON * json);
static int jcall_evtreport(sess_t sess, cJSON * json);
static int jcall_hbreport(sess_t sess, cJSON * json);

static int jcall_inetreport(sess_t sess, cJSON * json, inet_type type);
static void evtreport_store(unsigned char *frame);
static void posreport_store(unsigned char *frame);

typedef int(*jcall_ptr)(sess_t sess, cJSON* json);
typedef struct _json_call_st{
	char * _type;
	jcall_ptr _cb;
}json_call_st, *json_call_t;

static json_call_st json_calls[] = {
	//{ "logon", jcall_logon },
	//{ "gettask", jcall_gettask },
	{ "posreport", jcall_posreport },
	{ "evtreport", jcall_evtreport },
	{ "hbreport",jcall_hbreport },
	{ NULL, NULL },
};


static int istitle(uchar* str)
{	
	int i = 0;

	if (str == 0) return 0;

	for (; i < _arraysize(MSGTitle); i++)
	{
		if (memcmp(MSGTitle[i].title, str, strlen(MSGTitle[i].title)) == 0)
			break;
	}

	return (i < _arraysize(MSGTitle)) ? /*MSGTitle[i].id*/ (i + 1) : 0;
}

static int search_frame(router_exp_t explain, unsigned char*p, int len, int* searchpos)
{
	int dwCurrPt = 0;
	uchar ch;
	int i = 0;
	int isearlen = 0;
	int iHeadLen = 10;

	_exp_t exp = &_BASE(explain);

	exp->HeadPtr = 0;

	for (; dwCurrPt < len; dwCurrPt++)
	{
		ch = p[dwCurrPt];

		switch (exp->searchstatus)
		{
		case SEARCHMACHINE_NULL:	//  0: $
		{
			exp->Head[exp->HeadPtr++] = ch;
			if (exp->HeadPtr == iHeadLen)
			{//搜索头部
				int ititileid = istitle(exp->Head);
				if (ititileid)
				{

					int c = 0;
					iHeadLen = strlen(MSGTitle[ititileid - 1].title);
					exp->searchstatus = SEARCHMACHINE_IDENTIFIER;
					memcpy(exp->frame, exp->Head, iHeadLen);

					exp->framelen = iHeadLen;
					exp->HeadPtr = 0;
					dwCurrPt = iHeadLen - 1;
					explain->checksum = 0;
					for (; c < iHeadLen; c++)
						explain->checksum ^= exp->Head[c];
					continue;

				}
				else
				{
					int i = 1;
					for (; i < iHeadLen; i++)
					{
						if (exp->Head[i] == '$')
							break;
					}

					if (i < iHeadLen)
					{
						int j = 0;
						for (; j < iHeadLen - i; j++)
							exp->Head[j] = exp->Head[j + i];

						exp->HeadPtr -= i;
					}
					else
						exp->HeadPtr = 0;

					exp->searchstatus = SEARCHMACHINE_NULL;
				}
			}
		}
		break;

		case SEARCHMACHINE_IDENTIFIER:
		{
			exp->frame[exp->framelen] = ch;
			exp->framelen++;
			explain->checksum ^= ch;
			if (exp->framelen >= UNPACKBUFFSIZE - 1)
			{
				exp->searchstatus = SEARCHMACHINE_NULL;
				continue;
			}
			if (ch == '\n' && exp->frame[exp->framelen - 2] == '\r')
			{//
				if (dwCurrPt == (len - 1))
				{//找到完整的一帧数据,且缓冲区中只有一帧数据
					*searchpos = dwCurrPt;
					dwCurrPt = 0;
					exp->searchstatus = SEARCHMACHINE_NULL;//added 2002-09-04 23:06						
					return PACK_FINISHED;
				}
				else{//完整一帧数据取完后,还有其它帧的数据
					*searchpos = dwCurrPt;
					exp->searchstatus = SEARCHMACHINE_NULL;
					return PACK_STILLDATA;
				}
			}
		}
		break;
		default://默认状态
			exp->searchstatus = SEARCHMACHINE_NULL;
		}
	}
	dwCurrPt = 0;
	//一帧数据未接收完(SearchMachine=SEARCHMACHINE_LENGTH,下次进入时有用)
	if (exp->searchstatus == SEARCHMACHINE_NULL)
		return PACK_UNSEARCHED;
	else
		return PACK_UNFINISHED;
}

//收到消息后给终端答复router
static char*  HandelResponse(unsigned char *frame, int len)
{
	cJSON *jsonroot, *json;
	char *type = NULL;
	char *seqno = NULL;
	char *toid = NULL;
	char  * rslt = NULL;
	char szvalue[48] = { 0 };

	jsonroot = cJSON_Parse(frame);
	if (jsonroot == NULL) return 0;

	json = cJSON_GetObjectItem(jsonroot, "type");
	if (json == NULL)
	{
		cJSON_Delete(jsonroot);
		return 0;
	}
	type = json->valuestring;

	json = cJSON_GetObjectItem(jsonroot, "to");
	if (json == NULL)
	{
		cJSON_Delete(jsonroot);
		return 0;
	}
	toid = json->valuestring;

	json = cJSON_GetObjectItem(jsonroot, "seqno");
	if (json == NULL)
	{
		cJSON_Delete(jsonroot);
		return 0;
	}
	seqno = json->valuestring;

	cJSON * root = cJSON_CreateObject();

	cJSON * responsejson = cJSON_CreateObject();
	cJSON_AddStringToObject(responsejson, "to", toid);
	sprintf(szvalue, "%srslt", type);
	cJSON_AddStringToObject(responsejson, "type", szvalue);
	cJSON_AddStringToObject(responsejson, "rslt", "ok");
	cJSON_AddStringToObject(responsejson, "seqno", seqno);
	cJSON_AddItemToObject(root, "response", responsejson);
	rslt = cJSON_Print(root);

	cJSON_Delete(jsonroot);
	free(root);			// 避免内存泄漏
	free(responsejson);			// 避免内存泄漏

	return rslt;
}

static int send2namend(char* name ,char* buffer,int bufferlen)
{
	int rt = 0;
	if (name == NULL || buffer == NULL) return 0;

	sess_t sess = (sess_t)xhash_get(client()->sessions, name);
	if (sess)
	{
		response_pkt_p pkt;
		p2p_t p2p = (p2p_t)sess;

		buffer[bufferlen + 1] = '\0';
		pkt = response_pkt_new(client()->pktpool, bufferlen + 1);
		memcpy(pkt->data, buffer, bufferlen + 1);
		pkt->len = bufferlen + 1;

		p2p_stream_push(p2p, pkt);
		log_write(sess->client->log, LOG_NOTICE, "send2named inet-buffer = %s", pkt->data);

		rt = 1;
	}

	return rt;
}

static char * online_jsonstring(char userid[])
{
	char  * rslt = NULL;
	char    szvalue[64];
	char    sztime[64];
	cJSON * root = cJSON_CreateObject();

	SYSTEMTIME T;
	GetLocalTime(&T);
	sprintf(sztime, "%04d-%02d-%02d %02d:%02d:%02d", T.wYear, T.wMonth, T.wDay, T.wHour, T.wMinute, T.wSecond);

	cJSON_AddStringToObject(root, "type", "online");

	cJSON_AddStringToObject(root, "userid", userid);

	cJSON_AddStringToObject(root, "time", sztime);

	/*snprintf(szvalue, sizeof(szvalue), "%d", g_instapp->seqno2smcache);
	cJSON_AddStringToObject(root, "seqno", szvalue);*/

	rslt = cJSON_PrintUnformatted(root);
	free(root);
	return rslt;
}


// 来自巡检终端APP的心跳信息，用来维持用户在线状态
static int jcall_hbreport(sess_t sess, cJSON * json)
{
	char * town = NULL;
	cJSON* jreq = NULL;
	cJSON* jid = NULL;
	user_st user;
	char   buffer[2048];
	char*  jstr = NULL;

	if (sess == NULL || json == NULL) return 0;
	jstr = cJSON_Print(json);

	if (jstr == NULL) return 0;

	jreq = cJSON_GetObjectItem(json, "req");
	if (jreq == NULL /*|| jreq->valuestring == NULL*/) { free(jstr); return 0; }

	jid = cJSON_GetObjectItem(jreq, "userid");
	if (jid == NULL || jid->valuestring == NULL) { free(jstr); return 0; }

	if (client()->user_act) users_act_save(client()->user_act, jid->valuestring, _max_life, _user_inet);

	char * towncode = redis_get_towncode(jid->valuestring);		// 由userid获取towncode
	if (towncode == NULL)
	{
		free(jstr);
		return 1;
	}

	char * onlinestr = online_jsonstring(jid->valuestring);		// 拼好上线消息串备用

	//转发给县与乡镇		
	int size = jqueue_size(client()->session_que);
	for (int i = 0; i < size; i++){
		sess_t sess = jqueue_pull(client()->session_que);
		jqueue_push(client()->session_que, sess, 0);

		if (((sess->usertype == _town_type) && strcmp(sess->towncode, towncode) == 0) || ((sess->usertype == _county_type)))
		{
			int nprint = 0;
			//message_aes_encrypt(sess, onlinestr, buffer, INET_MSG, &nprint);	// AES加密

			nprint += sprintf_s(buffer + nprint, sizeof(buffer) - nprint, "$message,smcache,%s,%s", sess->skey, onlinestr);
			nprint += sprintf_s(buffer + nprint, sizeof(buffer) - nprint, "*%02X\r\n", checksum(buffer, nprint));
			send2namend(sess->skey, buffer, nprint);
		}
	}

	free(towncode);
	free(jstr);
	free(onlinestr);		// 避免内存泄漏

	return 0;
}
//lq
static int jcall_inetreport(sess_t sess, cJSON * json, inet_type type)
{
	char * town = NULL;
	cJSON* jreq = NULL;
	cJSON* jid  = NULL;
	char * towncode = NULL;
	char   buffer[4096];
	char   content[2048];
	char*  jstr = NULL;

	if (sess == NULL || json == NULL) return 0;	
	jstr = cJSON_Print(json);

	if (jstr == NULL) return 0;
	
	log_write(logger(), LOG_NOTICE, "recv --message-- from %s.", sess->sname);

	// 存储互联网事件与位置
	if (type == inet_pos)
		posreport_store(jstr);
	else if (type == inet_evt)
		evtreport_store(jstr);

	jid = cJSON_GetObjectItem(json, "userid");
	if (jid == NULL || jid->valuestring == NULL) { free(jstr); return 0; }
	
	if (client()->user_act) users_act_save(client()->user_act, jid->valuestring, _max_life, _user_inet);
	
	towncode = redis_get_towncode(jid->valuestring);		// 由userid获取towncode
	if (towncode == NULL)
	{
		free(jstr);
		return 1;
	}
	
	char * onlinestr = online_jsonstring(jid->valuestring);		// 拼好上线消息串备用

	memset(buffer, 0, sizeof(buffer));
	//转发给县与乡镇		
	int size = jqueue_size(client()->session_que);
	for (int i = 0; i < size; i++){
		sess_t sess = jqueue_pull(client()->session_que);
		jqueue_push(client()->session_que, sess, 0);

		if (((sess->usertype == _town_type) && strcmp(sess->towncode, towncode) == 0) || ((sess->usertype == _county_type)))
		{
		int nprint = 0;
		//message_aes_encrypt(sess, jstr, buffer, INET_MSG, &nprint);		// AES加密

		nprint += sprintf_s(buffer + nprint, sizeof(buffer) - nprint, "$message,smcache,%s,%s", sess->skey, jstr);
		nprint += sprintf_s(buffer + nprint, sizeof(buffer) - nprint, "*%02X\r\n", checksum(buffer, nprint));
		send2namend(sess->skey, buffer, nprint);
		}
	}

	free(towncode);
	free(jstr);

	return 1;
}

static int jcall_evtreport(sess_t sess, cJSON * json)
{
	if (sess == NULL || json == NULL) return 0;
	return jcall_inetreport(sess, json, inet_evt);
}

static int jcall_posreport(sess_t sess, cJSON * json)
{
	if (sess == NULL || json == NULL) return 0;
	return jcall_inetreport(sess, json, inet_pos);
}



//与router之间的消息解析
static int   explain(router_exp_t exp, uchar*frame, int len)
{
	sess_t sess;
	p2pclient_t  p2p;
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;
	NULLReturn(frame, 0);
	NULLReturn(exp, 0);

	sess = (sess_t)_BASE(exp).param;
	p2p = (p2pclient_t)sess;

	NULLReturn(frame, 0);
	NULLReturn(exp, 0);

	//==========================信息类别判别==========================//
	for (iTitleIter = 0; iTitleIter < _arraysize(MSGTitle); iTitleIter++)
	{
		if (memcmp(frame, MSGTitle[iTitleIter].title, strlen(MSGTitle[iTitleIter].title)) == 0)
			break;
	}

	TRUEReturn(iTitleIter >= _arraysize(MSGTitle), 0);

	f_iInfoID = MSGTitle[iTitleIter].id;
	//============================信息解码============================//
	switch (f_iInfoID)
	{
	case cunpack_initack: //收到来自router的initack
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
		strncpy(sess->sname, json->valuestring, sizeof(sess->sname));

		json = cJSON_GetObjectItem(jsonroot, "to");
		if (json == NULL || json->valuestring == NULL){
			cJSON_Delete(jsonroot);
			return 0;
		}

		if (strcmp(json->valuestring, "smcache") != 0)  //不是发给自身的Init信息
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

		if (strcmp(json->valuestring, "ok") != 0)  //不是发给自身的Init信息
		{
			cJSON_Delete(jsonroot);
			p2pclient_close((p2pclient_t)sess);//close connection..
			return 0;
		}

		cJSON_Delete(jsonroot);

		sess->binitsess = TRUE;
		log_write(client()->log, LOG_NOTICE, "initack from %s.", sess->sname);	 //提示收到Initsession回复
	}
	break;
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

		if (strcmp(json->valuestring, "smcache") != 0)  //不是发给自身的Init信息
		{
			cJSON_Delete(jsonroot);
			return 0;
		}
		log_write(client()->log, LOG_NOTICE, "pong from %s", from);	 //提示收到Initsession回复	
		cJSON_Delete(jsonroot);
	}
	break;
	case cunpack_message: //收到router发送来的信息
	{
		cJSON * jsonroot, *jtype, *jreq;
		char* jstr = strstr((char*)frame, "{");

		if (jstr == NULL) return 0;

		jsonroot = cJSON_Parse(jstr);
		if (jsonroot == NULL) return 0;

		jreq = cJSON_GetObjectItem(jsonroot, "req");
		if (jreq == NULL){
			cJSON_Delete(jsonroot);
			return 0;
		}

		jtype = cJSON_GetObjectItem(jreq, "type");
		if (jtype == NULL){
			cJSON_Delete(jsonroot);
			return 0;
		}
		
		for (int i = 0; i < sizeof(json_calls) / sizeof(json_call_st)-1; i++)
		{
			if (json_calls[i]._cb != NULL && json_calls[i]._type!=NULL && strcmp(json_calls[i]._type, jtype->valuestring) == 0){
				json_calls[i]._cb(sess, jreq);
				break;
			}
		}

		cJSON_Delete(jsonroot);		
		
	}
	break;
	default:
		break;
	}
	return 0;

}

router_exp_t router_exp_new(void* param)
{
	router_exp_t rtn = NULL;

	while ((rtn = (router_exp_t)calloc(1, sizeof(router_exp_st))) == NULL);

	_BASE(rtn).framelen = 0;
	_BASE(rtn).m_cb = NULL;
	_BASE(rtn).searchstatus = SEARCHMACHINE_NULL;
	_BASE(rtn).param = param;
	_BASE(rtn).search = search_frame;
	_BASE(rtn).explain = explain;

	return rtn;
}

void    dev_exp_free(router_exp_t exp)
{
	if (exp)
		free(exp);
}

static void posreport_store(unsigned char *frame)
{

	/************************************************************************/
	/*                  第1步，解析json串，取出各个节点的值					*/
	/************************************************************************/
	cJSON * jsonroot, *json;
	char *userid, *towncode, *lon, *lat, *hgt, *time;
	char* jstr = strstr((char*)frame, "{");
	int rt = 0;

	if (jstr == NULL) return;

	jsonroot = cJSON_Parse(jstr);
	if (jsonroot == NULL) return;


	json = cJSON_GetObjectItem(jsonroot, "userid");
	if (json == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}
	userid = json->valuestring;

	towncode = redis_get_towncode(userid);	// 从redis中查找userid对应的towncode
	if (towncode == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}

	json = cJSON_GetObjectItem(jsonroot, "lon");
	if (json == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}
	lon = json->valuestring;

	json = cJSON_GetObjectItem(jsonroot, "lat");
	if (json == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}
	lat = json->valuestring;

	json = cJSON_GetObjectItem(jsonroot, "hgt");
	if (json == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}
	hgt = json->valuestring;

	/************************************************************************/
	/*                  第2步，存入数据库									*/
	/************************************************************************/
	position_t pos = posrpt_new();

	SYSTEMTIME t;
	GetLocalTime(&t);
	char strtime[64] = { 0 };
	sprintf_s(strtime, sizeof(strtime), "%04d-%02d-%02d %02d:%02d:%02d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
	//char tabletime[10] = { 0 };
	//sprintf_s(tabletime, sizeof(tabletime), "%04d%02d", t.wYear, t.wMonth);	// for selecting table from databases

	pos->userid = _strdup(userid);
	pos->bdid = NULL;
	pos->towncode = towncode;	// towncode是redis_get_str出来的结果，是堆上的内存空间，不需要再_strdup
	pos->time = _strdup(strtime);
	pos->lon = _strdup(lon);
	pos->lat = _strdup(lat);
	pos->hgt = _strdup(hgt);
	pos->stype = 1;	// 来自互联网
	//pos->tabletime = _strdup(tabletime);

	_store_position(pos);
	posrpt_free(pos);

	cJSON_Delete(jsonroot);
}

static void evtreport_store(unsigned char *frame)
{
	/************************************************************************/
	/*                  第1步，解析json串，取出各个节点的值					*/
	/************************************************************************/
	cJSON * jsonroot, *json;
	char *userid, *towncode, *lon, *lat, *hgt, *time, *evttype, *evttypename, *description;
	char* jstr = strstr((char*)frame, "{");
	int rt = 0;

	if (jstr == NULL) return;

	jsonroot = cJSON_Parse(jstr);
	if (jsonroot == NULL) return;

	json = cJSON_GetObjectItem(jsonroot, "userid");
	if (json == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}
	userid = json->valuestring;

	towncode = redis_get_towncode(userid);	// 从redis中查找userid对应的towncode
	if (towncode == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}

	json = cJSON_GetObjectItem(jsonroot, "lat");
	if (json == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}
	lat = json->valuestring;

	json = cJSON_GetObjectItem(jsonroot, "lon");
	if (json == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}
	lon = json->valuestring;

	json = cJSON_GetObjectItem(jsonroot, "hgt");
	if (json == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}
	hgt = json->valuestring;
	if (strncmp(hgt, "-1", sizeof("-1")) == 0)
	{
		cJSON_Delete(jsonroot);
		return;
	}

	json = cJSON_GetObjectItem(jsonroot, "evttype");
	if (json == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}
	evttype = json->valuestring;

	json = cJSON_GetObjectItem(jsonroot, "evttypename");
	if (json == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}
	evttypename = json->valuestring;

	json = cJSON_GetObjectItem(jsonroot, "desc");
	if (json == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}
	description = json->valuestring;

	/************************************************************************/
	/*                  第2步，存入数据库									*/
	/************************************************************************/
	event_t evt = evtrpt_new();

	SYSTEMTIME t;
	GetLocalTime(&t);
	char strtime[64] = { 0 };
	sprintf_s(strtime, sizeof(strtime), "%04d-%02d-%02d %02d:%02d:%02d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
	//char tabletime[10] = { 0 };
	//sprintf_s(tabletime, sizeof(tabletime), "%04d%02d", t.wYear, t.wMonth);	// for selecting table from databases

	evt->userid = _strdup(userid);
	evt->bdid = NULL;
	evt->towncode = towncode;   // towncode是redis_get_str出来的结果，是堆上的内存空间，不需要再_strdup
	evt->time = _strdup(strtime);
	evt->lon = _strdup(lon);
	evt->lat = _strdup(lat);
	evt->hgt = _strdup(hgt);
	evt->evttype = _strdup(evttype);
	evt->evttypname = _strdup(evttypename);
	evt->description = _strdup(description);
	evt->stype = 1;			//来自互联网
	//evt->tabletime = _strdup(tabletime);

	_store_event(evt);
	evtrpt_free(evt);

	cJSON_Delete(jsonroot);
}

