
//����userauth��bdserver֮���ͨ��
#include <stdlib.h>
#include <string.h>
#include "instapp.h"
#include "userauth_expat.h"
#include "instapp.h"
#include "p2p.h"
#include "json/cJSON.h"
#include "tool.h"
#include "subst/subst.h"

static char g_msgcache[64*1024];

#define NULL     0
#define HEADLEN  12
#define _COUNT   4
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

static const int MSGCOUNT = 4;
SInfoTitle_st    MSGTitle[4] =
{
	{cunpack_initsession,"$initack"}, //Initsess
	{cunpack_ping,"$ping"},
	{cunpack_pong,"$pong"},
	{cunpack_message,"$message"}	
};

extern instapp_t g_instapp ;

static int istitle(uchar* str)
{
	int i   = 0 ;
	
	if(str == 0) return 0;

	for(; i < MSGCOUNT ; i++)
		if(memcmp(MSGTitle[i].title,str,strlen(MSGTitle[i].title)) == 0)		
			break;		

	if(i < MSGCOUNT)
		return MSGTitle[i].id;
	else
		return 0;
}

static int search_frame(exp_auth_t explain,unsigned char*p,int len,int* searchpos)
{
	int dwCurrPt = 0 ;
	uchar ch;
	int i = 0;

	int isearlen = 0;
	int iHeadLen = 10;
	int headlen = 0;

	_exp_t exp = &_BASE(explain);

	exp->HeadPtr = 0;

	for(; dwCurrPt<len; dwCurrPt++)
	{
		ch = p[dwCurrPt];

		switch(exp->searchstatus)
		{
		case SEARCHMACHINE_NULL:	//  0: $
			{
				exp->Head[exp->HeadPtr++] = ch;
				if(exp->HeadPtr == iHeadLen)
				{//����ͷ��	

					if(istitle(exp->Head))
					{
						int c = 0;
						exp->searchstatus = SEARCHMACHINE_IDENTIFIER;
						memcpy(exp->frame,exp->Head,iHeadLen);						

						exp->framelen = iHeadLen;
						exp->HeadPtr  = 0;
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
					{//�ҵ�������һ֡����,�һ�������ֻ��һ֡����
						*searchpos = dwCurrPt;
						dwCurrPt=0;
						exp->searchstatus=SEARCHMACHINE_NULL;//added 2002-09-04 23:06						
						return PACK_FINISHED;
					}else{//����һ֡����ȡ���,��������֡������
						*searchpos = dwCurrPt;
						exp->searchstatus=SEARCHMACHINE_NULL;			
						return PACK_STILLDATA;
					}
				}
			}
			break;
		default://Ĭ��״̬
			exp->searchstatus=SEARCHMACHINE_NULL;
		}
	}
	dwCurrPt=0;
	//һ֡����δ������(SearchMachine=SEARCHMACHINE_LENGTH,�´ν���ʱ����)
	if(exp->searchstatus == SEARCHMACHINE_NULL)
	return PACK_UNSEARCHED;
	else
	return PACK_UNFINISHED;
}


////��userauth֮�����Ϣ����
static int   explain(exp_auth_t exp,uchar*frame,int len)
{
	sess_t sess;
	int  msgtype;
	int  iTitleIter = 0;

	NULLReturn(frame,0);
	NULLReturn(exp,0);

	sess   = (sess_t)_BASE(exp).param;	

	NULLReturn(frame,0) ;
	NULLReturn(exp,0);

	//==========================��Ϣ����б�==========================//
	for(iTitleIter=0; iTitleIter < _COUNT; iTitleIter++)
	{
		if(memcmp(frame, MSGTitle[iTitleIter].title, strlen(MSGTitle[iTitleIter].title))==0)
			break;
	}

	TRUEReturn(iTitleIter >=_COUNT,0);

	msgtype = MSGTitle[iTitleIter].id;
	//============================��Ϣ����============================//
	switch(msgtype)
	{

	case cunpack_initsession: //�յ�����userauth��Init�ظ���Ϣ��������н�����������Լ���OK��Ϣ����sess->binitness ����ΪTRUE
		{
			cJSON *jsonroot,*json;									
			char*  jstr;

			jstr = strstr((char*)frame,"{");
			if(jstr == NULL) return 0;

			jsonroot = cJSON_Parse(jstr);
			if(jsonroot == NULL) return 0 ;

			json = cJSON_GetObjectItem(jsonroot,"from");
			if(json == NULL)
			{
				cJSON_Delete(jsonroot);
				return 0;	
			}
			strncpy_s(sess->sname, sizeof(sess->sname), json->valuestring, sizeof(sess->sname));

			json = cJSON_GetObjectItem(jsonroot,"to");
			if(json == NULL || json->valuestring == NULL)
			{
				cJSON_Delete(jsonroot);
				return 0;	
			}

			if(strcmp(json->valuestring,"bdserver" ) != 0 )  // ���Ƿ��������Init��Ϣ
			{
				cJSON_Delete(jsonroot);
				return 0;	
			}

			json = cJSON_GetObjectItem(jsonroot,"rslt");
			if(json == NULL || json->valuestring == NULL)
			{
				cJSON_Delete(jsonroot);
				return 0;	
			}

			if(strcmp(json->valuestring,"ok" ) != 0 )  // Initʧ��
			{
				cJSON_Delete(jsonroot);
				return 0;
			}

			cJSON_Delete(jsonroot);

			sess->binitsess = TRUE;
			printf("=====initack from===== %s\r\n",sess->sname);
			log_write(g_instapp->log,LOG_NOTICE,"initack from %s .\n",sess->sname);	 //��ʾ�յ�Initsession�ظ�
		}
		break;
	case cunpack_pong:		// �յ�����router��Pong��Ϣ������Ǹ��Լ��ģ���д����־
		{
			cJSON * jsonroot,* json;
			char* from;
			char* jstr = strstr((char*)frame,"{");

			if(jstr == NULL) return 0;

			jsonroot = cJSON_Parse(jstr);
			if(jsonroot == NULL) return 0 ;

			json = cJSON_GetObjectItem(jsonroot,"from");
			if(json == NULL || json->valuestring == NULL){
				cJSON_Delete(jsonroot);
				return 0;
			}

			from = json->valuestring;

			json = cJSON_GetObjectItem(jsonroot,"to");
			if(json == NULL|| json->valuestring == NULL){
				cJSON_Delete(jsonroot);
				return 0;
			}

			if(strcmp(json->valuestring,"bdserver" ) != 0 )  //���Ƿ��������pong��Ϣ
			{
				cJSON_Delete(jsonroot);
				return 0;
			}
			
			cJSON_Delete(jsonroot);
			printf("=====pong from %s=====\r\n",from);
			log_write(g_instapp->log,LOG_NOTICE,"pong from %s .\n",from);	 //��ʾ�յ�pong�ظ�
		}
		break;

	case cunpack_message: //�յ���֤����������֤���
		{
			/* �˴������Ȳ�д 
			cJSON * jsonroot,* json_auth,* json_oper,* json_req,* json;
			char *type,*from,*ackseqno,*error;
			int     idatalen = 0;
			int     addrlen = 0;
			char    strip[64] = {0};
			int     iport = 0;
			char * pstr = NULL;


			pstr = (char*)frame;
			pstr = strchr(pstr,'{'); //json��

			jsonroot = cJSON_Parse(pstr);
			if(jsonroot == NULL)
			{
				cJSON_Delete(jsonroot);
				return 0 ;
			}

			json_auth= cJSON_GetObjectItem(jsonroot,"auth");
			if(json_auth == NULL)
			{
				cJSON_Delete(jsonroot);
				return 0 ;
			}

			json = cJSON_GetObjectItem(json_auth,"rslt");
			if(strcmp(json->valuestring,"ok" ) != 0 )  // //��֤ʧ��
			{
				json = cJSON_GetObjectItem(json_auth,"error");
				if(json == NULL)
				{
					cJSON_Delete(jsonroot);
					return 0 ;
				}
                error = json->valuestring;
				json_oper = cJSON_GetObjectItem(jsonroot,"oper");
				if(json_oper == NULL)
				{
					cJSON_Delete(jsonroot);
					return 0 ;
				}
				
				json = cJSON_GetObjectItem(json_oper,"ip");
				if(json == NULL) 
				{
					cJSON_Delete(jsonroot);
					return 0 ;
				}
				
				strcpy_s(strip, sizeof(strip), json->valuestring);
				json = cJSON_GetObjectItem(json_oper,"port");
				if(json == NULL)
				{
					cJSON_Delete(jsonroot);
					return 0 ;
				}
				iport = atoi(json->valuestring);
				
				json_req = cJSON_GetObjectItem(json_oper,"req");
				if(json_req == NULL)
				{
					cJSON_Delete(jsonroot);
					return 0 ;
				}

				json = cJSON_GetObjectItem(json_req,"type");
				if(json == NULL)
				{
					cJSON_Delete(jsonroot);
					return 0 ;
				}
				type = json->valuestring;

				json = cJSON_GetObjectItem(json_req,"from");
				if(json == NULL)
				{
					cJSON_Delete(jsonroot);
					return 0 ;
				}
				from = json->valuestring;

				json = cJSON_GetObjectItem(json_req,"ackseqno");
				if(json == NULL) 
				{
					cJSON_Delete(jsonroot);
					return 0 ;
				}
				ackseqno = json->valuestring;
				
				idatalen +=sprintf_s(g_msgcache+idatalen,sizeof(g_msgcache)-idatalen,"$message,{\"type\":\"%s\",\"from\":\"%s\",\"seqno\":\"%s\",\"rslt\":\"%s\,\"error\":\"%s\"}",type,from,ackseqno,"fail",error);
				idatalen += sprintf_s(g_msgcache+idatalen,sizeof(g_msgcache)-idatalen,"*%02X\r\n",checksum(g_msgcache,idatalen));
				//������͸��ն�
				
				cJSON_Delete(jsonroot);

				sendtoclient(g_instapp->frontend_fd->fd,strip,iport,g_msgcache);
				
				printf("=====bdserver recv userauth failed=====\r\n");
				log_write(g_instapp->log,LOG_NOTICE,"recv userauth failed,%s .\n",g_msgcache);	 //��֤ͨ����ӡ��Ϣ
				
				return 0;
			}else //��֤�ɹ�
			{
				json = cJSON_GetObjectItem(jsonroot,"oper");
				strcpy_s(g_msgcache, sizeof(g_msgcache), cJSON_PrintBuffered(json, 64 * 1024, 0));
				idatalen = strlen(g_msgcache);
				//��֤ͨ��,��auth��Ϣ��ȡ������������͸�router��������������bdsvr���͸�userauth������������һģһ����
				//����֮��userauth���ص���Ϣ��ԭ������֤��Ϣ�����������֤ͨ����Ϣ����ʱ�ǽ��ⲿ���¼ӵ�����ȥ��
				// Ȼ���װ��oper�ӽڵ�����ݣ������json�����ַ�������ʽ���͸��ص��������ڻص������б����ٴν���json��
				if(_BASE(exp).m_cb) _BASE(exp).m_cb(cunpack_message,g_msgcache,idatalen,exp);
				
				cJSON_Delete(jsonroot);//
				printf("=====recv userauth success=====\r\n");
				log_write(g_instapp->log,LOG_NOTICE," bdserver recv userauth success,%s .\n",g_msgcache);	 //��֤ͨ����ӡ��Ϣ
			}
			*/
		}
		break;
	default:
		break;
	}
	return 0;

}

exp_auth_t exp_auth_new(void* param)
{
	exp_auth_t rtn = NULL;

	while((rtn = (exp_auth_t)calloc(1,sizeof(exp_auth_st))) == NULL);

	_BASE(rtn).framelen = 0;
	_BASE(rtn).m_cb = NULL ;
	_BASE(rtn).searchstatus = SEARCHMACHINE_NULL;
	_BASE(rtn).param = param;
	_BASE(rtn).search= search_frame;
	_BASE(rtn).explain = explain;

	return rtn;
}

void    exp_auth_free(exp_auth_t exp)
{
	if(exp)
		free(exp);
}
