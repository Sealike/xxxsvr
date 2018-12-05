#include <stdlib.h>
#include <string.h>
#include "backexpat.h"
#include "client.h"
#include "protobuffer.h"
#include "json/cJSON.h"
#include "p2p.h"

#define NULL     0
#define HEADLEN  4
#define _COUNT   1
#define FALSE    0
#define TRUE     1

static unsigned char g_title[4] ={0xFE,0xFE,0xFE,0xFE};

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
	int rtn = 0;
	int i   = 0 ;

	if(str == 0) return 0;

	for(; i < _COUNT ; i++)
	{
		if(memcmp(g_title,str,4) == 0)		
			break;
	}

	if(i < _COUNT)
		return 1;
	else
		return 0;
}

static int search_frame(back_exp_t explain,uchar*p,int len,int* searchpos)
{
	int dwCurrPt = 0 ;
	uchar ch;
	_exp_t exp = &_BASE(explain);

	for(; dwCurrPt<len; dwCurrPt++)
	{
		ch = p[dwCurrPt];

		switch(exp->searchstatus)
		{
		case SEARCHMACHINE_NULL:	//  0: $
			{
				exp->Head[exp->HeadPtr++]     = ch;
				if(exp->HeadPtr == HEADLEN)
				{//搜索头部
					exp->Head[HEADLEN] = '\0';
					if(istitle(exp->Head))
					{
						
						int c = 0;
						exp->searchstatus = SEARCHMACHINE_IDENTIFIER;
						memcpy(exp->frame,exp->Head,HEADLEN);						

						exp->framelen = HEADLEN;
						exp->HeadPtr = 0;
						explain->checksum = 0;
						for(; c < HEADLEN ; c++)
							explain->checksum ^= exp->Head[c];
						continue;
					}else
					{
						int i = 1;
						for(; i<HEADLEN;i++)
						{
							if(exp->Head[i] == 0xFE)
								break;
						}

						if(i<HEADLEN)
						{
							int j = 0 ;
							for(; j < HEADLEN-i ; j++)
								exp->Head[j] = exp->Head[j+i];

							exp->HeadPtr -= i;
						}else
							exp->HeadPtr  = 0;

						exp->searchstatus = SEARCHMACHINE_NULL;
					}
				}
			}
			break;		

		case SEARCHMACHINE_IDENTIFIER:
			{
				exp->frame[exp->framelen] = ch;
				exp->framelen ++;
				explain->checksum ^= ch;

				if(exp->framelen == 7){
					explain->indlen = (exp->frame[5]<<8)+(unsigned char)exp->frame[6];
					explain->indlen+=8;
					if((explain->indlen) >= (UNPACKBUFFSIZE-1)){
						exp->searchstatus = SEARCHMACHINE_NULL;
						continue;;
					}
				}
				if(exp->framelen >= UNPACKBUFFSIZE-1)
				{
					exp->searchstatus = SEARCHMACHINE_NULL;
					continue;
				}

				//end part..
				if(exp->framelen == explain->indlen)
				{
					
					if(explain->checksum != 0){
						exp->searchstatus = SEARCHMACHINE_NULL;
						continue;
					}

					if(dwCurrPt==(len-1))
					{//找到完整的一帧数据,且缓冲区中只有一帧数据
						*searchpos = dwCurrPt;
						dwCurrPt=0;
						exp->searchstatus=SEARCHMACHINE_NULL;//added 2002-09-04 23:06						
						return PACK_FINISHED;
					}else{//完整一帧数据取完后,还有其它帧的数据
						*searchpos = dwCurrPt;
						exp->searchstatus=SEARCHMACHINE_NULL;			
						return PACK_STILLDATA;
					}
				}
			}
			break;
		default://默认状态
			exp->searchstatus=SEARCHMACHINE_NULL;
		}
	}
	dwCurrPt=0;
	//一帧数据未接收完(SearchMachine=SEARCHMACHINE_LENGTH,下次进入时有用)
	if(exp->searchstatus == SEARCHMACHINE_NULL)
		return PACK_UNSEARCHED;
	else
		return PACK_UNFINISHED;
}

static int   explain(back_exp_t exp,uchar*frame,int len)
{
	int  f_iInfoID = 0;
	sess_t sess;
	int  msgcode=0;
	cJSON *jsonroot, *json;

	NULLReturn(frame,0);
	NULLReturn(exp,0);
	sess = (sess_t)_BASE(exp).param;

	if (sess->state == state_init_SESS && strncasecmp(frame, "$AUTH", 5) == 0 && sess->type == back_server_SESS)
	{
		char* stroot = strchr((char*)frame, '{');
		if (stroot == NULL) return 0;

		jsonroot = cJSON_Parse(frame);
		if (jsonroot == NULL) return 0;

		json = cJSON_GetObjectItem(jsonroot, "name");
		if (json == NULL)
		{
			cJSON_Delete(jsonroot);
			return 0;
		}

		snprintf(sess->skey, sizeof(sess->skey), "%s", json->valuestring);
		//snprintf(sess->sname, sizeof(sess->sname), "%s", json->valuestring);

		xht ht = client()->sessions;
		sess = xhash_get(ht, sess->skey);
		if (sess != NULL){
			p2p_t p2p = (p2p_t)sess; p2p_kill(p2p);
		}

		xhash_put(ht, sess->skey, sess);

		sess->state = state_open_SESS;

		cJSON_Delete(jsonroot);

		return 0;
	}
	else if (strncasecmp(frame, "$PING", 5) == 0 &&  sess->type == back_server_SESS)
	{
		p2p_t p2p = (p2p_t)sess;
		time_t now = time(NULL);
		if(p2p){
			//strftime(buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S",localtime(&now));
			//printf("%s backend ping sence %d\n",buffer,sence);
			protobuffer_send_p2p(p2p,eProto_pong,NULL);
		}
	}
	else if (strncasecmp(frame, "BDCOMM", 5) == 0 && sess->type == back_server_SESS)
	{
		//处理北斗业务数据
		char* stroot = strchr((char*)frame, '{');
		if (stroot == NULL) return 0;

		jsonroot = cJSON_Parse(frame);
		if (jsonroot == NULL) return 0;

		json = cJSON_GetObjectItem(jsonroot, "type");
		if (json == NULL)
		{
			cJSON_Delete(jsonroot);
			return 0;
		}

		if (strcmp("bdpos", json->valuestring) == 0){
		//定位处理,需要转发给存储服务器，需要转发给乡镇分控中心、县级指挥中心。

		}
		else if (strcmp("bdbsi", json->valuestring) == 0){
		//波束信息,需要转发给存储服务器，需要转发给乡镇分控中心、县级指挥中心。
		}
		else if (strcmp("bdevent", json->valuestring) == 0){
		//通信信息,需要转发给存储服务器，需要转发给乡镇分控中心、县级指挥中心。
		
		}
		else if (strcmp("bdgettask", json->valuestring) == 0){
		//任务信息请求,将请求转发给查询服务器，等待响应

		}

		cJSON_Delete(jsonroot);
	}
	
	return 0;
}

back_exp_t back_exp_new(void* param)
{
	back_exp_t rtn = NULL;

	while((rtn = (back_exp_t)calloc(1,sizeof(back_exp_st))) == NULL);

	_BASE(rtn).framelen = 0;
	_BASE(rtn).m_cb = NULL;
	_BASE(rtn).searchstatus = SEARCHMACHINE_NULL;
	_BASE(rtn).param = param;
	_BASE(rtn).search= search_frame;
	_BASE(rtn).explain = explain;

	return rtn;
}

void    back_exp_free(back_exp_t exp)
{
	if(exp)
		free(exp);
}
