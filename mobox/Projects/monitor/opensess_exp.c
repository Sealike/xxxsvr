#include "opensess_exp.h"
#include "client.h"
#include "p2p.h"
#include "protobuffer.h"
#include "json/cJSON.h"
#include "globaldata.h"
#include "tool.h"
#include "moniclient_expat.h"
#include "moniclient_callback.h"


static int search_frame(opensess_exp_t explain,uchar*p,int len,int* searchpos);
static int explain(opensess_exp_t exp,uchar*frame,int len);


static SMsgTitle_st MSGTitle[] =
{
	{cunpack_initsession,"$initsess"}

};

static int istitle(uchar* str)
{
	int i   = 0 ;

	if(str == 0) return 0;

	for (; i < _arraysize(MSGTitle); i++)
	{
		if(memcmp(MSGTitle[i].title,str,strlen(MSGTitle[i].title)) == 0)		
			break;		
	}

	return (i < _arraysize(MSGTitle)) ? (i + 1) : 0;
}

opensess_exp_t opensess_exp_new(void* param)
{
	opensess_exp_t rtn = NULL;

	while((rtn = (opensess_exp_t)calloc(1,sizeof(opensess_exp_st))) == NULL);

	_BASE(rtn).framelen = 0;
	_BASE(rtn).m_cb = NULL;
	_BASE(rtn).searchstatus = SEARCHMACHINE_NULL;
	_BASE(rtn).param = param;
	_BASE(rtn).search= search_frame;
	_BASE(rtn).explain = explain;

	return rtn;
}

void    opensess_exp_free(opensess_exp_t exp)
{
	if(exp)
		free(exp);
}

//////////////////////////////////////////////////////////////////////////
static int search_frame(opensess_exp_t explain,uchar*p,int len,int* searchpos)
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

static int   explain(opensess_exp_t exp,uchar*frame,int len)
{
	int rt = 0;
	sess_t sess;
	p2p_t  p2p;
	int    msgid = 0;
	int    msgcnt = 0;

	NULLReturn(frame,0);
	NULLReturn(exp,0);

	sess   = (sess_t)_BASE(exp).param;
	p2p = (p2p_t)sess;

	NULLReturn(sess,0) ;

	for (msgcnt = 0; msgcnt < _arraysize(MSGTitle); msgcnt++)
	{
		if(memcmp(frame, MSGTitle[msgcnt].title, strlen(MSGTitle[msgcnt].title))==0)
			break;
	}

	TRUEReturn(msgcnt >= _arraysize(MSGTitle), 0);

	msgid = MSGTitle[msgcnt].id;
	//============================信息解码============================//
	switch(msgid)
	{
	case cunpack_initsession:
		{
			cJSON * jsonroot,*jrole,*jno;
			char  * jstr = NULL; 	
			int i= 0;			

			if(len-5<0) 
			{
				p2p_kill(p2p); 
				break;
			}
			
			jstr = (char*)strchr(frame,'{');
			if (jstr == NULL) 
			{
				p2p_kill(p2p);
				break;
			}

			jsonroot = cJSON_Parse(jstr);
			if(jsonroot == NULL) 
			{
				p2p_kill(p2p);
				break;
			}

			jrole = cJSON_GetObjectItem(jsonroot,"role");
			if (jrole == NULL) 
			{
				cJSON_Delete(jsonroot);
				jsonroot = NULL;
				p2p_kill(p2p);
				break;
			}						

			if (jrole->valuestring == NULL) 
			{
				cJSON_Delete(jsonroot);
				jsonroot = NULL;
				p2p_kill(p2p);
				break;
			}
			strcpy_s(sess->role, sizeof(sess->role), jrole->valuestring);			

			jno = cJSON_GetObjectItem(jsonroot, "seqno");
			if (jno == NULL) 
			{
				cJSON_Delete(jsonroot);
				jsonroot = NULL;
				p2p_kill(p2p);
				break;
			}
			else if (strncmp(sess->role, "moniclient", sizeof("moniclient")) == 0)
			{
				moniclient_exp_t monitorexp = moniclient_exp_new(sess);
				exp_set_callback((_exp_t)monitorexp, moniclient_exp_cb);
				opensess_exp_free(exp);
				p2p->expat = (_exp_t)monitorexp;
				sess->state = state_init_SESS;
				sess->auth_cnt = 0;//初始化认证次数
				sess->usertype = -1;
				rt = -1;
			}
			else
			{
				cJSON_Delete(jsonroot);
				jsonroot = NULL;
				p2p_kill(p2p);
				break;
			}
			
			p2p->sess.type = back_server_SESS;
			init_ack_st initack = { 0 };
			strcpy_s(initack.from, sizeof(initack.from) - 1, "monitor");
			strcpy_s(initack.to, sizeof(initack.from) - 1, "null");
			strcpy_s(initack.role, sizeof(initack.role) - 1, jrole->valuestring);
			initack.seqno = jno->valuestring ? atoi(jno->valuestring) : 0;
			initack.ifok = 1;
			
			protobuffer_send_p2p(p2p, eProto_initack, &initack);	 //init ack
			
			if (jsonroot)
			{
				cJSON_Delete(jsonroot);
				jsonroot = NULL;
				break;
			}	
		}
		break;
	
	default:
		break;
	}

	return rt;
}