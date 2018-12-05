//处理router与smudpserver之间的通信

#include <stdlib.h>
#include <string.h>

#include "p2pclient.h"
#include "expat_bdsvr.h"
#include "instapp.h"
#include "p2p.h"
#include "json/cJSON.h"
#include "tool.h"

extern instapp_t g_instapp ;

#define NULL     0
#define HEADLEN  12
#define _COUNT   6
#define FALSE    0
#define TRUE     1

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
typedef struct _SInfoTitle_st{
	int id;
	char title[12];
}SInfoTitle_st,*SInfoTitle_t;

static const int MSGCOUNT = 6;
static SInfoTitle_st  MSGTitle[6] =
{
	{cunpack_initsession,"$initack"}, //Initsess
	{cunpack_ping,"$ping"},
	{cunpack_pong,"$pong"},
	{cunpack_message,"$message"},
	{ cunpack_txa, "$CCTXA" },
	{ cunpack_ide, "$NOJOB" }
	
};

static char g_msgcache[64*1024];

static int istitle(uchar* str)
{
	int i   = 0 ;

	if(str == 0) return 0;

	for(; i < MSGCOUNT ; i++)
	{
		if(memcmp(MSGTitle[i].title,str,strlen(MSGTitle[i].title)) == 0)		
			break;		
	}

	return (i < MSGCOUNT) ? /*MSGTitle[i].id*/ (i+1): 0;
}

static int search_frame(bdsvr_exp_t explain, unsigned char*p, int len, int* searchpos)
{
	int dwCurrPt = 0 ;
	uchar ch;
	int i = 0;
	int isearlen = 0;
	int iHeadLen = 10;

	_exp_t exp = &_BASE(explain);

	exp->HeadPtr = 0;

	for(; dwCurrPt<len; dwCurrPt++)
	{
		ch = p[dwCurrPt];

		switch(exp->searchstatus)
		{
		case SEARCHMACHINE_NULL:	//  0: $
			{
				exp->Head[exp->HeadPtr++]     = ch;
				if(exp->HeadPtr == iHeadLen)
				{//搜索头部
					int ititileid = istitle(exp->Head);
					if(ititileid)
					{
						
						int c = 0;
						iHeadLen = strlen(MSGTitle[ititileid-1].title);
						exp->searchstatus = SEARCHMACHINE_IDENTIFIER;
						memcpy(exp->frame,exp->Head,iHeadLen);						
						
						exp->framelen = iHeadLen;
						exp->HeadPtr = 0;
						dwCurrPt =  iHeadLen-1;
						explain->checksum = 0;
						for(; c < iHeadLen ; c++)
							explain->checksum ^= exp->Head[c];
						continue;
						
					}else
					{
						int i = 1;
						for(; i<iHeadLen;i++)
						{
							if(exp->Head[i] == '$')
								break;
						}

						if(i<iHeadLen)
						{
							int j = 0 ;
							for(; j < iHeadLen-i ; j++)
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
				if(exp->framelen >= UNPACKBUFFSIZE-1)
				{
					exp->searchstatus = SEARCHMACHINE_NULL;
					continue;
				}
				if(ch == '\n' && exp->frame[exp->framelen-2] =='\r')
				{//
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

//与router之间的消息解析
static int   explain(bdsvr_exp_t exp, uchar*frame, int len)
{
	sess_t sess;
	int  f_iInfoID = 0;
	int  iTitleIter = 0;
	NULLReturn(frame,0);
	NULLReturn(exp,0);

	sess   = (sess_t)_BASE(exp).param;
	
	NULLReturn(frame,0) ;
	NULLReturn(exp,0);

	//==========================信息类别判别==========================//
	for(iTitleIter=0; iTitleIter < _COUNT; iTitleIter++)
	{
		if(memcmp(frame, MSGTitle[iTitleIter].title, strlen(MSGTitle[iTitleIter].title))==0)
			break;
	}


	TRUEReturn(iTitleIter >=_COUNT,0);

	f_iInfoID = MSGTitle[iTitleIter].id;
	//============================信息解码============================//
	switch(f_iInfoID)
	{
	case cunpack_initsession: //收到init的回复
		{
			cJSON *jsonroot,*json;									
		    char * jstr;

			jstr = strchr((char*)frame,'{');
			if(jstr == NULL) return 0;

			jsonroot = cJSON_Parse(jstr);
			if(jsonroot == NULL) return 0 ;

			json = cJSON_GetObjectItem(jsonroot,"from");
			if(json == NULL || json->valuestring == NULL)
			{
				cJSON_Delete(jsonroot);
				return 0;	
			}
			strncpy(sess->sname ,json->valuestring,sizeof(sess->sname));

			json = cJSON_GetObjectItem(jsonroot,"to");
			if(json == NULL|| json->valuestring == NULL){
				cJSON_Delete(jsonroot);
				return 0;	
			}

			if(strcmp(json->valuestring,"bdjob" ) != 0 )  //不是发给自身的Init信息
			{
				cJSON_Delete(jsonroot);
				return 0;	
			}

			json = cJSON_GetObjectItem(jsonroot,"rslt");
			if(json == NULL|| json->valuestring == NULL)
			{
				cJSON_Delete(jsonroot);
				return 0;	
			}
			
			if(strcmp(json->valuestring,"ok" ) != 0 )  //
			{
				cJSON_Delete(jsonroot);
				p2pclient_close((p2pclient_t)sess);//close connection..
				return 0;
			}

			cJSON_Delete(jsonroot);
			
			sess->binitsess = TRUE;
			log_write(g_instapp->log,LOG_NOTICE,"initack from %s.",sess->sname);	 //提示收到Initsession回复
			
		}
		break;
	case cunpack_pong:
		{
			cJSON * jsonroot,* json;
			char* from;
			char* jstr = strstr((char*)frame,"{");

			if(jstr == NULL) return 0;

			jsonroot = cJSON_Parse(jstr);
			if(jsonroot == NULL) return 0 ;

			json = cJSON_GetObjectItem(jsonroot,"from");
			if(json == NULL){
				cJSON_Delete(jsonroot);
				return 0;
			}
			
			from = json->valuestring;

			json = cJSON_GetObjectItem(jsonroot,"to");
			if(json == NULL){
				cJSON_Delete(jsonroot);
				return 0;
			}
			
			if(strcmp(json->valuestring,"bdjob" ) != 0 )  //
			{
				cJSON_Delete(jsonroot);
				return 0;
			}
			log_write(g_instapp->log,LOG_NOTICE,"pong from %s.",from);	 //
			cJSON_Delete(jsonroot);
		}
		break;
	case cunpack_txa: //通信请求
	{
		if (_BASE(exp).m_cb) _BASE(exp).m_cb(cunpack_txa, frame, len, exp);
	}
	break;
	case cunpack_ide:
	{
		if (_BASE(exp).m_cb) _BASE(exp).m_cb(cunpack_ide, frame, len, exp);
	}
	break;
	//case cunpack_message: //收到bdsvr操作结果
	//	{
	//		cJSON *jsonroot,*json;			
	//		char strip[64] = {0};
	//		short  iport;
	//		char * pstr;	
	//		char * from;
	//		char * to;
	//		frame[len-5] = '\0'; //*检验\r\n
	//		pstr = strchr((char*)frame,'{');			
	//		if(pstr == NULL) return;
	//				
	//		jsonroot = cJSON_Parse(pstr);

	//		if(jsonroot == NULL) return 0 ;

	//		json = cJSON_GetObjectItem(jsonroot,"response");
	//		if(json == NULL)
	//		{
	//			cJSON_Delete(jsonroot);
	//			return 0;
	//		}
	//		pstr = cJSON_PrintBuffered(json,64*1024,0);
	//		if(strlen(pstr)>sizeof(g_msgcache)-1) 
	//		{
	//			cJSON_Delete(jsonroot);
	//			return 0;
	//		}

	//		strcpy(g_msgcache,pstr);

	//		json = cJSON_GetObjectItem(jsonroot,"ip");
	//		if(json == NULL) 
	//		{
	//			cJSON_Delete(jsonroot);
	//			return 0;
	//		}			
	//		strcpy(strip,json->valuestring);
	//		json = cJSON_GetObjectItem(jsonroot,"port");
	//		if(json == NULL ||json->valuestring == NULL)
	//		{
	//			cJSON_Delete(jsonroot);
	//			return 0;
	//		}
	//		iport = atoi(json->valuestring);
	//		sendtoclient(g_instapp->fe_fd->fd,strip,iport,pstr);		
	//		
	//		cJSON_Delete(jsonroot);
	//		printf("=====recv response from  router  len=%d=====\r\n",len);
	//		log_write(g_instapp->log,LOG_NOTICE,"=== router->smudpsvr .. %s .\n",g_msgcache);	 /
	//	}
	//	break;
	default:
		break;
	}
	return 0;

}

bdsvr_exp_t bdsvr_exp_new(void* param)
{
	bdsvr_exp_t rtn = NULL;

	while ((rtn = (bdsvr_exp_t)calloc(1, sizeof(bdsvr_exp_st))) == NULL);

	_BASE(rtn).framelen = 0;
	_BASE(rtn).m_cb = NULL ;
	_BASE(rtn).searchstatus = SEARCHMACHINE_NULL;
	_BASE(rtn).param = param;
	_BASE(rtn).search= search_frame;
	_BASE(rtn).explain = explain;

	return rtn;
}

void    bdsvr_exp_free(bdsvr_exp_t exp)
{
	if(exp) free(exp);
}
