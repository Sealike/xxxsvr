#ifndef __MYSQLDB_H__
#define __MYSQLDB_H__
#include <WinSock2.h>

#include "mysql.h"
#ifdef __cplusplus
	extern "C"{
#endif

typedef struct   _mysqlquery_st{
	MYSQL       *m_con;
	MYSQL_RES   *m_res;
	MYSQL_ROW    m_row;
}*mysqlquery_t,mysqlquery_st;

mysqlquery_t  mysqldb_connect_init(char* svrname,int port,char* user,char*pwd,char* dbname);
void		  mysqldb_free(mysqlquery_t query);
void		  mysqldb_close(mysqlquery_t);
BOOL		  mysqldb_isclosed(mysqlquery_t);
void		  mysqldb_ping(mysqlquery_t);
char*         mysqldb_fetch_scarler(mysqlquery_t query);

#ifdef __cplusplus
	}
#endif

#endif