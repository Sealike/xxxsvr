#ifndef __MYSQLDB_H__
#define __MYSQLDB_H__
#include <WinSock2.h>

#include "mysql.h"

enum{eSqlQueryerr_none = 0,eSqlQueryerr_errorquery,eSqlQueryerr_errorping,eSqlConnectOk,eSqlConnectErr};

typedef void (*querycallback_t)(void*,int);

typedef struct   _mysqlquery_st{
	MYSQL       *m_con; 
	MYSQL_RES   *m_res; 
	MYSQL_ROW    m_row; 
	querycallback_t _cb;
}*mysqlquery_t,mysqlquery_st;

mysqlquery_t  mysqldb_connect_init(char* svrname,int port,char* user,char*pwd,char* dbname,querycallback_t cb);
mysqlquery_t  mysqldb_connect_reinit(mysqlquery_t* query_ptr,char* svrname,int port,char* user,char*pwd,char* dbname);
void		  mysqldb_free(mysqlquery_t query);
void		  mysqldb_close(mysqlquery_t);
BOOL		  mysqldb_isclosed(mysqlquery_t);
int 		  mysqldb_ping(mysqlquery_t);
void          mysqldb_setcb(mysqlquery_t, querycallback_t cb);

#endif