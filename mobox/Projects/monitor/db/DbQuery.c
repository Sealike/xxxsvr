#include "stdafx.h"
#include "DbQuery.h"
#include "Function.h"

_INIT_TABLE_BEGIN(CDbQuery)
	_INIT_QUERYTABLE(CQueryAccountImpl)
	_INIT_QUERYTABLE(CQueryOrderFormImpl)
	_INIT_QUERYTABLE(CQueryTicketImpl)
	_INIT_QUERYTABLE(CQuerySessionImpl)
	_INIT_QUERYTABLE(CQueryCardImpl)
	_INIT_QUERYTABLE(CQueryLogUserImpl)
	_INIT_QUERYTABLE(CQueryScoreImpl)
	_INIT_QUERYTABLE(CQueryMailScoreImpl)
	_INIT_QUERYTABLE(CQueryPlayNumberImpl)
_INIT_TABLE_END()

CDbQuery::CDbQuery(void)
{	
	m_query = NULL;	
	m_thThread = NULL;
	m_evStop = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_pingsecs = 60;
	InitQuerytables();
}

CDbQuery::~CDbQuery(void)
{
	db_close();
	CloseHandle(m_evStop);
	mysqldb_free(m_query);
}

BOOL CDbQuery::db_close()
{
	stopping();

	if(m_query)
		mysqldb_close(m_query);

	return TRUE;
}

void CDbQuery::ping()
{
	AUTOLOCK(m_cs);

	mysqldb_ping(m_query);

}

void CDbQuery::stopping()
{
	if(m_thThread != NULL)
	{
		SetEvent(m_evStop);
		DWORD dwRslt = WaitForSingleObject(m_thThread,2000);
		if(dwRslt == WAIT_TIMEOUT){
			TerminateThread(m_thThread,-1);			
		}
		m_thThread = NULL;
	}	
}

BOOL CDbQuery::startping(int seconds)
{
	DWORD thid;

	if(seconds > 0)
		m_pingsecs = seconds;
	
	stopping();

	m_thThread = CreateThread(NULL, 0,CDbQuery::periodjob,this,0,&thid);

	return m_thThread != NULL;
}

DWORD WINAPI CDbQuery::periodjob(void* pvoid)
{
	DWORD dwRtn = -1;

	CDbQuery* pThis = (CDbQuery*)pvoid;

	if(pThis == NULL) return dwRtn;

	DWORD dwRslt = WaitForSingleObject(pThis->m_evStop,pThis->m_pingsecs*1000);
	while(dwRslt != WAIT_OBJECT_0){		
		pThis->ping();
		dwRslt = WaitForSingleObject(pThis->m_evStop,pThis->m_pingsecs*1000);
	}

	if(dwRslt == WAIT_OBJECT_0)
	{
		ResetEvent(pThis->m_evStop);
		return 0;
	}else
		return dwRtn;
}

BOOL CDbQuery::db_connect(const char* strip,int port,const char* dbuser,const char* pwd,const char* dbname,BOOL ifcontinue_ping,int pingseconds)
{
	db_close();

	if(m_query == NULL)
	{
		m_query =   mysqldb_connect_init(const_cast<char*>(strip),port,const_cast<char*>(dbuser),const_cast<char*>(pwd),const_cast<char*>(dbname));
		if(m_query == NULL) {
			printf("connect to database failed!!\n");
		}else{
			mysql_query(m_query->m_con, "SET NAMES 'gb2312'"); 
			if(ifcontinue_ping){
				startping(pingseconds);
			}
		}
		
		return m_query != NULL;
	}	
	
	return FALSE;
}