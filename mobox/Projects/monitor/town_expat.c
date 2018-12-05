#include <stdlib.h>
#include <string.h>
#include "town_expat.h"
#include "client.h"
#include "protobuffer.h"
#include "json/cJSON.h"
#include "p2p.h"
#include "globaldata.h"
#include "dbaccess.h"
#include "tool.h"

#define NULL     0
#define HEADLEN  4
#define FALSE    0
#define TRUE     1

typedef struct _SInfoTitle_st2{
	int id;
	char title[12];
}SInfoTitle_st2, *SInfoTitle_t2;

static SInfoTitle_st2 MSGTitle[2] =
{
	{ cunpack_auth, "$auth" }, //Initsess
	{ cunpack_ping, "$ping" },
};

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

#define ISLCHAR(w)     (w>='a'&&w<='z')
#define ISHCHAR(w)     (w>='A'&&w<='Z')
#define ISDIGIT(w)     (w>='0'&&w<='9')
#define ISCHAR(w)       (ISDIGIT(w)|| ISLCHAR(w)||ISHCHAR(w))

#define NULLReturn(condition , rtnval)      {if((condition) == NULL)  return rtnval;}
#define FALSEReturn(condition, rtnval)      {if((condition) == FALSE) return rtnval;}
#define TRUEReturn(condition , rtnval)      {if((condition) == TRUE)  return rtnval;}

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

static int search_frame(town_exp_t explain, unsigned char*p, int len, int* searchpos)
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

static int   explain(town_exp_t exp, uchar*frame, int len)
{
	sess_t sess;
	cJSON *jsonroot;
	p2p_t p2p;

	NULLReturn(frame, 0);
	NULLReturn(exp, 0);

	sess = (sess_t)_BASE(exp).param;

	if (sess == NULL) return 0;

	p2p = (p2p_t)sess;

	if (sess->state == state_init_SESS && strncasecmp(frame, "$auth", 5) == 0 && sess->type == back_server_SESS)
	{
		p2p->sess.auth_cnt++;	// 登录验证次数累加

		cJSON *jname, *jsubject, *jno, *jpwd;
		int authrslt = 0;
		int authtype = -1;
		char* stroot = strchr((char*)frame, '{');
		if (stroot == NULL) return 0;

		jsonroot = cJSON_Parse(stroot);
		if (jsonroot == NULL) return 0;

		jname = cJSON_GetObjectItem(jsonroot, "name");
		if (jname == NULL)
		{
			p2p_kill(p2p);
			cJSON_Delete(jsonroot);
			return 0;
		}

		jpwd = cJSON_GetObjectItem(jsonroot, "pwd");
		if (jpwd == NULL)
		{
			p2p_kill(p2p);
			cJSON_Delete(jsonroot);
			return 0;
		}

		jsubject = cJSON_GetObjectItem(jsonroot, "subject");
		if (jsubject == NULL) {
			cJSON_Delete(jsonroot);
			p2p_kill(p2p);
			return 0;
		}

		jno = cJSON_GetObjectItem(jsonroot, "seqno");
		if (jno == NULL)
		{
			p2p_kill(p2p);
			cJSON_Delete(jsonroot);
			return 0;
		}
		if (strncmp(jsubject->valuestring, "county", sizeof("county")) == 0)
		{
			authtype = 0;
		}
		else if (strncmp(jsubject->valuestring, "town", sizeof("town")) == 0)
		{
			authtype = 1;
		}
		else if (strncmp(jsubject->valuestring, "country", sizeof("country")) == 0)
		{
			authtype = 2;
		}
		char *psw = NULL;
		account_t accountinfo = account_new();

		int iret = db_query_accountinfo(client()->sqlobj, jname->valuestring, accountinfo);		// 如：查询名为admin的记录
		if (iret == 0)
		{
			if (accountinfo->pwd && strcmp(accountinfo->pwd, jpwd->valuestring) == 0)
			{
				if (accountinfo->itype == authtype)
				{
					authrslt = 1;
				}
				else
				{
					account_free(accountinfo);
					auth_st auth;
					auth.ok = authrslt;
					strncpy(auth.peername, sess->sname, sizeof(auth.peername) - 1);
					strcpy(auth.err, "error: 认证角色不匹配.");
					auth.seqno = atoi(jno->valuestring);

					protobuffer_send_p2p(p2p, eProto_authack, &auth);
					//p2p_kill(p2p);
					printf("itype != authtype\r\n");

					cJSON_Delete(jsonroot);

					return 0;
				}
			}
			else
			{
				if (p2p->sess.auth_cnt < 3)
				{
					account_free(accountinfo);
					auth_st auth;
					auth.ok = authrslt;
					strncpy(auth.peername, sess->sname, sizeof(auth.peername) - 1);
					strcpy(auth.err, "密码错误！");
					auth.seqno = atoi(jno->valuestring);

					protobuffer_send_p2p(p2p, eProto_authack, &auth);
					printf("pwd wrong cnt = %d.\r\n", p2p->sess.auth_cnt);

					cJSON_Delete(jsonroot);
					return 0;
				}
				else
				{
					account_free(accountinfo);
					auth_st auth;
					auth.ok = authrslt;
					strncpy(auth.peername, sess->sname, sizeof(auth.peername) - 1);
					strcpy(auth.err, "登录次数超过限制！");
					auth.seqno = atoi(jno->valuestring);

					protobuffer_send_p2p(p2p, eProto_authack, &auth);
					p2p_kill(p2p);
					printf("pwd wrong cnt = %d, kill sess.\r\n", p2p->sess.auth_cnt);		// 密码错误

					cJSON_Delete(jsonroot);
					return 0;
				}
			}		
		}
		else
		{
			if (p2p->sess.auth_cnt < 3)
			{
				account_free(accountinfo);
				auth_st auth;
				auth.ok = authrslt;
				strncpy(auth.peername, sess->sname, sizeof(auth.peername) - 1);
				strcpy(auth.err, "该用户不存在，请重新输入！");
				auth.seqno = atoi(jno->valuestring);

				protobuffer_send_p2p(p2p, eProto_authack, &auth);
				printf("account does not exist,cnt = %d.\r\n",p2p->sess.auth_cnt);

				cJSON_Delete(jsonroot);

				return 0;
			}
			else
			{
				account_free(accountinfo);
				auth_st auth;
				auth.ok = authrslt;
				strncpy(auth.peername, sess->sname, sizeof(auth.peername) - 1);
				strcpy(auth.err, "登录次数超过限制！");
				auth.seqno = atoi(jno->valuestring);

				protobuffer_send_p2p(p2p, eProto_authack, &auth);
				p2p_kill(p2p);
				printf("account does not exist,cnt = %d, kill sess.\r\n", p2p->sess.auth_cnt);	// 用户不存在

				cJSON_Delete(jsonroot);

				return 0;
			}
			
		}

		p2p->wflush = 1;

		strncpy(sess->towncode, accountinfo->code, sizeof(sess->towncode) - 1);
		sess->usertype = accountinfo->itype;
		account_free(accountinfo);

		snprintf(sess->skey, sizeof(sess->skey), "%s", jname->valuestring);
		snprintf(sess->sname, sizeof(sess->sname), "%s", jname->valuestring);

		xht ht = client()->sessions;
		sess_t oldsess = NULL;
		oldsess = xhash_get(ht, sess->skey);
		if (oldsess != NULL)
		{
			auth_st auth;
			auth.ok = authrslt;
			strncpy(auth.peername, sess->sname, sizeof(auth.peername) - 1);
			strncpy(auth.err, "强制下线", sizeof(auth.err)-1);
			auth.seqno = atoi(jno->valuestring);

			protobuffer_send_p2p((p2p_t)oldsess, eProto_offline, &auth);
			printf("kill client %s\n", oldsess->skey);
			p2p_kill((p2p_t)oldsess);
			//$offline, from, to, { "rslt":"ok", "error" : "被强制下线", "seqno" : "232" }*xx\r\n;
		}

		xhash_put(ht, sess->skey, sess);

		strcpy_s(sess->subject, sizeof(sess->subject), jsubject->valuestring);
		subject_add(client()->subjects, sess->subject, sess); // 把当前连接放入client()->subjects->_ht中以sess->subject为键的连接队列中，如bdsvr、county、town

		auth_st auth;

		memset(&auth, 0, sizeof(auth));
		auth.ok = authrslt;
		strncpy(auth.peername, sess->sname, sizeof(auth.peername) - 1);
		auth.seqno = atoi(jno->valuestring);

		protobuffer_send_p2p(p2p, eProto_authack, &auth);

		sess->state = state_open_SESS;

		cJSON_Delete(jsonroot);

		return 0;
	}
	else if (strncasecmp(frame, "$ping", 5) == 0 && sess->state == state_open_SESS &&  sess->type == back_server_SESS)
	{
		log_write(sess->client->log, LOG_NOTICE, "recv ping from %s.", sess->sname);
		p2p_t p2p = (p2p_t)sess;
		time_t now = time(NULL);
		if (p2p){
			pong_st pong;
			strcpy_s(pong.from, sizeof(pong.from) - 1, "smcache");
			strcpy_s(pong.to, sizeof(pong.to) - 1, sess->sname);
			protobuffer_send_p2p(p2p, eProto_pong, &pong);
			log_write(sess->client->log, LOG_NOTICE, "send pong to %s.", sess->sname);
		}
	}

	return 0;
}

town_exp_t town_exp_new(void* param)
{
	town_exp_t rtn = NULL;

	while ((rtn = (town_exp_t)calloc(1, sizeof(town_exp_st))) == NULL);

	_BASE(rtn).framelen = 0;
	_BASE(rtn).m_cb = NULL;
	_BASE(rtn).searchstatus = SEARCHMACHINE_NULL;
	_BASE(rtn).param = param;
	_BASE(rtn).search = search_frame;
	_BASE(rtn).explain = explain;

	return rtn;
}

void    town_exp_free(town_exp_t exp)
{
	if (exp)
		free(exp);
}



