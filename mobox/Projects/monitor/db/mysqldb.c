#include "mysqldb.h"

mysqlquery_t     mysqldb_connect_init(char* svrname,int port,char* user,char*pwd,char* dbname)
{
	mysqlquery_t query = calloc(1,sizeof(mysqlquery_st));
	query->m_con = mysql_init(query->m_con);

	if ( query->m_con !=NULL && (query->m_con = mysql_real_connect(query->m_con,svrname,user,pwd,dbname,port/*TCP IP端口*/,NULL/*Unix Socket 连接类型*/,0/*运行成ODBC数据库标志*/)))
	{
		return query;
	}else{
		if(query != NULL)
		{
			free(query);
		}
		return NULL;
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
	BOOL rtn = FALSE;
	rtn = (query&&query->m_con);

	return rtn;
}

void    mysqldb_ping(mysqlquery_t query)
{
	if(query && query->m_con)
		mysql_ping(query->m_con);
}

char* mysqldb_fetch_scarler(mysqlquery_t query)
{
	char*p = NULL;
	query->m_res = mysql_store_result(query->m_con);
	while((query->m_row = mysql_fetch_row(query->m_res)))
	{
		if(query->m_row[0])
		{
			int len = strlen(query->m_row[0]);
			p = (char*)malloc(len+1);
			strncpy(p ,query->m_row[0],len+1);
		}
		break;
	}

	mysql_free_result(query->m_res);

	return p;
}