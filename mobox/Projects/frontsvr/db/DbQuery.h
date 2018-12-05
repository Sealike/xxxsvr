#pragma once
#include "ChnavtectGlobal.h"
#include <vector>
#include "mysqldb.h"
#include "macros.h"
#include "messagess.h"
#include "type.h"
#include "QueryAccount.h"
#include "DataManage.h"
#include "QueryOrderform.h"
#include "QueryTicketResource.h"
#include "QueryCard.h"
#include "QuerySession.h"
#include "QueryLogUser.h"
#include "QueryScore.h"
#include "QueryScoreMail.h"
#include "QueryPlayNumber.h"
using namespace std;

#define _CLASSNAME_EXTEND(classname) typedef classname##<CQueryTable> classname##Impl

_CLASSNAME_EXTEND(CQueryAccount);
_CLASSNAME_EXTEND(CQueryOrderForm);
_CLASSNAME_EXTEND(CQueryTicket);
_CLASSNAME_EXTEND(CQuerySession);
_CLASSNAME_EXTEND(CQueryCard);
_CLASSNAME_EXTEND(CQueryLogUser);
_CLASSNAME_EXTEND(CQueryScore);
_CLASSNAME_EXTEND(CQueryMailScore);
_CLASSNAME_EXTEND(CQueryPlayNumber);
class CDbQuery :public CSharedRes
{
public:
	CDbQuery(void);
	~CDbQuery(void);
	BOOL db_connect(const char* strip,int port,const char* dbuser,const char* pwd,const char* dbname,BOOL ifcontinue_ping,int pingseconds);
	BOOL db_close();
	BOOL startping(int seconds);
	void stopping();
	ILocker* get_reslock(){return &m_cs;}
	void*    get_queryhandle(){return m_query;}	
	
protected:
	static DWORD WINAPI periodjob(void* pvoid);
	void ping();

	_DECLARE_INIT_QUERYTABLE()
	_ADD_QUERYTABLE(CQueryAccountImpl)
	_ADD_QUERYTABLE(CQueryOrderFormImpl)
	_ADD_QUERYTABLE(CQueryTicketImpl)
	_ADD_QUERYTABLE(CQuerySessionImpl)
	_ADD_QUERYTABLE(CQueryCardImpl)
	_ADD_QUERYTABLE(CQueryLogUserImpl)
	_ADD_QUERYTABLE(CQueryScoreImpl)
	_ADD_QUERYTABLE(CQueryMailScoreImpl)
	_ADD_QUERYTABLE(CQueryPlayNumberImpl)
private:
	mysqlquery_t m_query;
	CLockCs m_cs;
	HANDLE m_thThread;
	HANDLE m_evStop;
	int m_pingsecs;
};