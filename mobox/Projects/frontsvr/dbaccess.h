#ifndef __DBACCESS_H__
#define __DBACCESS_H__

#include "myquery/mysqldb.h"
#include "grouprslt.h"
#include "client.h"
#include "rediscommand.h"


typedef struct _conf_st
{
	int warntime;			// 消失报警的阈值时间，单位是分钟
}conf_st, *conf_t;

typedef struct _plog_st
{
	char *type;
	int taskid;
	char *executor;
	char *time;
	char *lat;
	char *lon;
	char *hgt;
	char *net;
	char *source;
	char *desc;
}plog_st, *plog_t;

typedef struct _boxpos_st
{
	int taskid;
	char *executor;
	char *time;
	char *lat;
	char *lon;
	char *hgt;
	char *net;
	char *source;
}boxpos_st, *boxpos_t;


typedef struct _warning_st
{
	char *type;
	int taskid;
	char *executor;
	char *time;
	char *lat;
	char *lon;
	char *hgt;
	char *net;
	char *source;
	
}warning_st, *warning_t;

typedef struct _status_st
{
	char *battery;
	char *beam;
	int taskid;
	char *executor;
	char *time;
	char *lat;
	char *lon;
	char *hgt;
	char *net;
	char *source;

}status_st, *status_t;


typedef struct _xuserinfo_st
{
	char *userid;
	char *towncode;
	char *bdnum;
}xuserinfo_st, *xuserinfo_t;



typedef struct _position_st
{
	char *userid;
	char *bdid;
	char *towncode;
	char *lat;
	char *lon;
	char *hgt;
	char *time;
	int stype;	// 位置报告来源。   0：北斗上报  1：互联网上报
	char *tabletime;
	void* sqlobj;
}position_st, *position_t;


typedef struct _event_st
{
	char *userid;
	char *bdid;
	char *towncode;
	char *time;
	char *lat;
	char *lon;
	char *hgt;
	char *evttype;
	char *evttypname;
	char *description;
	int stype;	// 位置报告来源。   0：北斗上报  1：互联网上报
	char *tabletime;
	void* sqlobj;
}event_st, *event_t;

typedef struct _dbconfig_st
{
	char *towncode;
	char *ip;
	char *port;
	char *dbname;
	char *username;
	char *password;
}dbconfig_st, *dbconfig_t;

//////////////////////////////////////////////////////////////////////////
int db_loaduser(mysqlquery_t dbinst,char* userid, user_st *user);
int db_loadbduser(mysqlquery_t dbinst, int bdid, char **userid);
int db_query_accountinfo(mysqlquery_t dbinst, char *accountname, account_t info);
int db_query_accountinfo_bycode(mysqlquery_t dbinst, char *code, account_t info);

char * redis_get_towncode(char * userid);
char * redis_get_userid(char * bdid);
int db_get_userid(mysqlquery_t dbinst, char* bdid, char **userid);
int _store_event(event_t evt);
int _store_position(position_t pos);
int db_save_position(mysqlquery_t dbinst, position_t position);
int db_save_event(mysqlquery_t dbinst, event_t evt);

int connect_each_database(mysqlquery_t dbinst);


int db_save_boxpos(mysqlquery_t dbinst,boxpos_t boxpos);
int db_save_warning(mysqlquery_t dbinst, warning_t warning);

int db_save_plog(mysqlquery_t dbinst, plog_t plog);
int db_save_status(mysqlquery_t dbinst, status_t status);
//int db_update_config(mysqlquery_t dbinst, conf_t conf);
//int db_query_config(mysqlquery_t dbinst, conf_t conf);
int db_update_config(mysqlquery_t dbinst, int warntime);
int db_query_config(mysqlquery_t dbinst, int * warntime);

int db_auth_excecutor_simple(mysqlquery_t dbinst, char *name, char ** pwd);
int db_auth_excecutor(mysqlquery_t dbinst, char *name, char *taskid, char ** pwd);

#endif