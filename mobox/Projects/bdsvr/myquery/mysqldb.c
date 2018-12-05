#include "mysqldb.h"


mysqlquery_t     mysqldb_connect_init(char* svrname,int port,char* user,char*pwd,char* dbname,querycallback_t cb)
{
	mysqlquery_t query = (mysqlquery_t)calloc(1,sizeof(mysqlquery_st));
	query->m_con = mysql_init(query->m_con);

	if (query->m_con !=NULL && (query->m_con = mysql_real_connect(query->m_con,svrname,user,pwd,dbname,port/*TCP IP端口*/,NULL/*Unix Socket 连接类型*/,0/*运行成ODBC数据库标志*/)))
	{
		query->_cb = cb;
		if(query->_cb) query->_cb(query,eSqlConnectOk);
		return query;
	}else{
		if(query != NULL)
		{
			free(query);
			if(cb) cb(NULL,eSqlConnectErr);
			query = NULL;
		}
		return NULL;
	}
}

mysqlquery_t mysqldb_connect_reinit(mysqlquery_t* query_ptr,char* svrname,int port,char* user,char*pwd,char* dbname)
{
	mysqlquery_t query = NULL;

	if(query_ptr == NULL) return NULL;
	
	query = *query_ptr;

	if(query == NULL) return NULL;

	query->m_con = mysql_init(query->m_con);

	if ( query->m_con !=NULL && (query->m_con = mysql_real_connect(query->m_con,svrname,user,pwd,dbname,port/*TCP IP端口*/,NULL/*Unix Socket 连接类型*/,0/*运行成ODBC数据库标志*/)))
	{		
		if(query->_cb) query->_cb(query,eSqlConnectOk);
		return query;
	}else{
	    return query;	
	}
}

void     mysqldb_free(mysqlquery_t query)
{
	if(query)
	{
		mysqldb_close(query);
		free(query);
		query = NULL;
	}
}

void	 mysqldb_close(mysqlquery_t query)
{
	if(query && query->m_con)
	{
		mysql_close(query->m_con);
		query->m_con = NULL;
	}
}

BOOL	mysqldb_isclosed(mysqlquery_t query)
{
	BOOL rtn = TRUE;
	if(query&&query->m_con)
	{
		rtn = FALSE;
	}

	return rtn;
}

int    mysqldb_ping(mysqlquery_t query)
{
	int code = 1;

	if(query && query->m_con)
		code =  mysql_ping(query->m_con);

	if(code == 1 && query && query->_cb){
		query->_cb(query,eSqlQueryerr_errorping);
	}

	return code;
}

void  mysqldb_setcb(mysqlquery_t query, querycallback_t cb)
{
	if(query)
		query->_cb = cb;
}