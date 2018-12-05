#include <stdlib.h>
#include <string.h>
#include "bdjobber_expat.h"
#include "p2p.h"
#include "protobuffer.h"
#include "json/cJSON.h"
#include "include/ConstDefine.h"
#include "tool.h"


static int search_frame(bdjobber_exp_t explain, uchar*p, int len, int* searchpos);
static int explain(bdjobber_exp_t exp, uchar*frame, int len);

static SMsgTitle_st MSGTitle[] =
{
	{ cunpack_ping, "$ping" },				// init是在opensess_exp.c中解析并回复init ok的，所以此处不再需要
	{ cunpack_jobme, "$JOBME" },
	{ cunpack_txr, "$BDTXR" },
	{ cunpack_dwr, "$DWR" },
	{ CUNPACK_BSI, "$BDBSI"}
	//{ CUNPACK_WAA, "$BDWAA" }
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

//////////////////////////////////////////////////////////////////////////
static int search_frame(bdjobber_exp_t explain, uchar*p, int len, int* searchpos)
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

					int isitileid = istitle(exp->Head);
					if (isitileid)
					{
						int c = 0;
						iHeadLen = strlen(MSGTitle[isitileid - 1].title);
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

static int explain(bdjobber_exp_t exp,uchar*frame,int len)
{
	int  msgid = 0;
	int  msgcnt = 0;
	NULLReturn(frame,-1);
	NULLReturn(exp,-1);

	for (msgcnt = 0; msgcnt < _arraysize(MSGTitle); msgcnt++)
	{
		if(memcmp(frame, MSGTitle[msgcnt].title, strlen(MSGTitle[msgcnt].title))==0)
			break;
	}

	TRUEReturn(msgcnt >= _arraysize(MSGTitle), -1);

	msgid = MSGTitle[msgcnt].id;

	if (_BASE(exp).m_cb) 
		_BASE(exp).m_cb(msgid, frame, len, exp);
	
	return 0;
}

bdjobber_exp_t bdjobber_exp_new(void* param)
{
	bdjobber_exp_t rtn = NULL;

	while ((rtn = (bdjobber_exp_t)calloc(1, sizeof(bdjobber_exp_st))) == NULL);

	_BASE(rtn).framelen = 0;
	_BASE(rtn).m_cb = NULL;
	_BASE(rtn).searchstatus = SEARCHMACHINE_NULL;
	_BASE(rtn).param = param;
	_BASE(rtn).search = search_frame;
	_BASE(rtn).explain = explain;

	return rtn;
}

void    bdjobber_exp_free(bdjobber_exp_t exp)
{
	if (exp)
		free(exp);
}